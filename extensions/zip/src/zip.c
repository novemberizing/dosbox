// http://www.iana.org/assignments/media-types/application/zip

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>

#include "novemberizing/zip.h"



static char * __filename(char * o, uint32_t len, const char * path) {
    if(o && len > 0 && path) {
        const char * s = strrchr(path, '/');
        if(s) {
            strncpy(o, s + 1, len);
        } else {
            strncpy(o, path, len);
        }
        s = strrchr(path, '.');
        uint32_t extension_len = strlen(s);
        o[strlen(o) - extension_len] = 0;
    }
    return o;
}

static int __mkdir(char * p, int isfile) {
    char * path = malloc(strlen(p) + 1);
    strcpy(path, p);
    int len = strlen(path);
    int progress = 0;
    char * work = (char *) malloc(len + 2);
    work[progress] = 0;
    char * name = strtok(path, "/");

    while(name) {
        if(progress > 0) {
            strcat(&work[progress], "/");
            progress++;
        }
        strcpy(&work[progress], name);
        progress += strlen(name);
        work[progress] = 0;
        name = strtok(NULL, "/");
        if(name) {
            mkdir(work, 0755);
        } else {
            if(!isfile) {
                mkdir(work, 0755);
            }
            free(work);
            free(path);
            return 0;
        }
    }
    free(work);
    free(path);
    return -1;
}

static void * __memory_del(void * o) {
    if(o) {
        free(o);
    }
    return NULL;
}

static void * __generate_value_from_fread(unsigned char * p, uint32_t n, FILE * fp)
{
    if(p)
    {
        free(p);
        p = NULL;
    }
    if(n > 0)
    {
        p = malloc(n + 1);
        if(p) {
            if(fread(p, sizeof(unsigned char), n, fp) == n) {
                p[n] = 0;
                return p;
            }
            fprintf(stdout, "fail to __generate_value_from_fread caused by fread\n");
            return NULL;
        }
        fprintf(stdout, "fail to __generate_value_from_fread caused by malloc (%d)\n", errno);
    }
    return NULL;
}

#define SIGNATURE_FILE          0x04034b50
#define SIGNATURE_CENTRAL       0x02014b50
#define SIGNATURE_CENTRAL_END   0x06054b50

typedef struct __File
{
    uint32_t signature;
    uint16_t version;
    uint16_t flags;
    uint16_t method;
    uint16_t time;
    uint16_t date;
    struct {
        uint32_t crc32;
        uint32_t compressed;
        uint32_t uncompressed;
    } __attribute__((aligned(1), packed)) data;
    uint16_t filename;
    uint16_t extra;
} __attribute__((aligned(1), packed)) File;

typedef struct __Central
{
    uint32_t signature;
    uint16_t made;
    uint16_t version;
    uint16_t flags;
    uint16_t method;
    uint16_t time;
    uint16_t date;
    struct {
        uint32_t crc32;
        uint32_t compressed;
        uint32_t uncompressed;
    } __attribute__((aligned(1), packed)) data;
    uint16_t filename;
    uint16_t extra;
    uint16_t comment;
    uint16_t disk;
    uint16_t internal;
    uint32_t external;
    uint32_t relative;
} __attribute__((aligned(1), packed)) Central;

typedef struct __End
{
    uint32_t signature;
    uint16_t disk;
    uint16_t start;
    uint16_t tracks;
    uint16_t directories;
    uint32_t size;
    uint32_t offset;
    uint16_t comment;
} __attribute__((aligned(1), packed)) End;

typedef struct Item
{
    struct {
        File * data;
        uint32_t offset;
        char * name;
        char * extra;
    } file;
    struct {
        Central * data;
        char * name;
        char * extra;
        char * comment;
    } info;
    struct Item * prev;
    struct Item * next;
} Item;

typedef struct __Root {
    struct {
        End * data;
        char * comment;
    } info;
    Item * head;
    Item * tail;
    uint32_t len;
} Root;


static void __clear_root(Root * root);
static Item * __generate_item(Root * root);
static Item * __remove_item(Root * root, Item * item);
static Item * __find_item(Root * root, const char * filename);
static Item * __generate_file_tag(Root * root, FILE * fp);
static Item * __generate_central_tag(Root * root, FILE * fp);
static End * __generate_end_tag(Root * root, FILE * fp);

static int __inflate(Item * item, FILE * source, const char * filepath);
static int __deflate(Item * item, FILE * source, const char * filepath, int level);


static End * __generate_end_tag(Root * root, FILE * fp)
{
    if(root && fp)
    {
        if(root->info.data) {
            free(root->info.data);
            root->info.data = NULL;
        }
        root->info.data = (End *) calloc(sizeof(End), 1);
        if(root->info.data == NULL) {
            fprintf(stdout, "fail to __generate_end_tag caused by calloc (%d)\n", errno);
            return NULL;
        }
        if(fread(root->info.data, sizeof(unsigned char), sizeof(End), fp) != sizeof(End)) {
            fprintf(stdout, "fail to __generate_end_tag caused by fread\n");
            return NULL;
        }
        if((root->info.comment = __generate_value_from_fread(root->info.comment, root->info.data->comment, fp)) == NULL && root->info.data->comment > 0){
            fprintf(stdout, "fail to __generate_end_tag caused by __generate_value_from_fread\n");
            return NULL;
        }
        return root->info.data;
    }
    fprintf(stdout, "fail to __generate_end_tag caused by root is nullptr or fp is nullptr\n");
    return NULL;
}

static Item * __generate_file_tag(Root * root, FILE * fp)
{
    if(fp) {
        Item * item = __generate_item(root);
        if(item) {
            item->file.data = (File *) calloc(sizeof(File), 1);
            if(item->file.data == NULL) {
                fprintf(stdout, "fail to __generate_file_tag caused by calloc (%d)\n", errno);
                return NULL;
            }
            item->file.offset = ftell(fp);
            if(fread(item->file.data, sizeof(unsigned char), sizeof(File), fp) != sizeof(File)) {
                fprintf(stdout, "fail to __generate_file_tag caused by fread\n");
                return __remove_item(root, item);
            }
            if((item->file.name = __generate_value_from_fread(item->file.name, item->file.data->filename, fp)) == NULL && item->file.data->filename > 0) {
                fprintf(stdout, "fail to __generate_file_tag caused by __generate_value_from_fread\n");
                return __remove_item(root, item);
            }
            if((item->file.extra = __generate_value_from_fread(item->file.extra, item->file.data->extra, fp)) == NULL && item->file.data->extra > 0) {
                fprintf(stdout, "fail to __generate_file_tag caused by __generate_value_from_fread\n");
                return __remove_item(root, item);
            }
            if(fseek(fp, item->file.data->data.compressed, SEEK_CUR) < 0) {
                fprintf(stdout, "fail to __generate_file_tag caused by fseek (%d)\n", errno);
                return __remove_item(root, item);
            }
            if(item->file.data->flags & 0x08) {
                fprintf(stdout, "implement this\n");
                return __remove_item(root, item);
            }
            // fprintf(stdout, "debug: file: %s\n", item->file.name);
            return item;
        }
        fprintf(stdout, "fail to __generate_file_tag caused by __generate_item\n");
        return NULL;
    }
    fprintf(stdout, "fail to __generate_file_tag caused by root is nullptr or fp is nullptr\n");
    return NULL;
}

static Item * __generate_central_tag(Root * root, FILE * fp)
{
    if(fp && root) {
        Central * central = calloc(sizeof(Central), 1);
        if(fread(central, sizeof(unsigned char), sizeof(Central), fp) != sizeof(Central)) {
            free(central);
            return NULL;
        }
        char * filename = __generate_value_from_fread(NULL, central->filename, fp);
        if(filename == NULL && central->filename > 0) {
            free(central);
            fprintf(stdout, "fail to __generate_central_tag caused by __generate_value_from_fread\n");
            return NULL;
        }
        char * extra = __generate_value_from_fread(NULL, central->extra, fp);
        if(extra == NULL && central->extra > 0) {
            free(central);
            if(filename) {
                free(filename);
            }
            fprintf(stdout, "fail to __generate_central_tag caused by __generate_value_from_fread\n");
            return NULL;
        }
        char * comment = __generate_value_from_fread(NULL, central->comment, fp);
        if(comment == NULL && central->comment > 0) {
            free(central);
            if(filename) {
                free(filename);
            }
            if(extra) {
                free(extra);
            }
            fprintf(stdout, "fail to __generate_central_tag caused by __generate_value_from_fread\n");
            return NULL;
        }
        Item * item = __find_item(root, filename);
        if(item) {
            free(item->info.data);
            free(item->info.name);
            free(item->info.extra);
            free(item->info.comment);

            item->info.data = central;
            item->info.name = filename;
            item->info.extra = extra;
            item->info.comment = comment;
            // fprintf(stdout, "debug: central: %s\n", filename);
            return item;
        }
        free(central);
        if(filename) {
            free(filename);
        }
        if(extra) {
            free(extra);
        }
        if(comment) {
            free(comment);
        }
        fprintf(stdout, "fail to __generate_central_tag caused by not found item\n");
        return NULL;
    }
    fprintf(stdout, "fail to __generate_central_tag caused by root is nullptr or fp is nullptr\n");
    return NULL;
}

static void __clear_root(Root * root)
{
    if(root)
    {
        while(root->head) {
            __remove_item(root, root->head);
        }
        root->info.data = __memory_del(root->info.data);
        root->info.comment = __memory_del(root->info.comment);
    }
}

static Item * __remove_item(Root * root, Item * item)
{
    if(root && item)
    {
        if(item->prev) {
            item->prev->next = item->next;
        } else {
            root->head = item->next;
        }
        if(item->next) {
            item->next->prev = item->prev;
        } else {
            root->tail = item->prev;
        }
        root->len--;

        item->file.data = __memory_del(item->file.data);
        item->file.extra = __memory_del(item->file.extra);
        item->file.name = __memory_del(item->file.name);

        item->info.data = __memory_del(item->info.data);
        item->info.extra = __memory_del(item->info.extra);
        item->info.name = __memory_del(item->info.name);
        item->info.comment = __memory_del(item->info.comment);

        free(item);
    }
    return NULL;
}

static Item * __find_item(Root * root, const char * filename)
{
    if(root && filename && strlen(filename)) {
        Item * item = root->head;
        while(item) {
            if(strcmp(item->file.name, filename) == 0) {
                return item;
            }
            item = item->next;
        }
    }
    return NULL;
}

static Item * __generate_item(Root * root)
{
    if(root) {
        Item * item = (Item *) calloc(sizeof(Item), 1);
        if(item) {
            item->prev = root->tail;
            if(root->tail) {
                root->tail->next = item;
            }
            if(root->head == NULL) {
                root->head = item;
            }
            root->tail = item;
            root->len++;
            return item;
        }
        fprintf(stdout, "fail to generate item caused by malloc (%d)\n", errno);    
        return NULL;
    }
    fprintf(stdout, "fail to generate item caused by root is nullptr\n");
    return NULL;
}

/**
 * https://zlib.net/zlib_how.html
 */
#define CHUNK 16384

static int __inflate(Item * item, FILE * source, const char * filepath)
{
    // DECLARE
    int ret;
    unsigned have;
    z_stream strm = { 0, };
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    // ALLOCATE INFLATE STATE
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    printf("offset (%d)\n", item->file.offset);
    if(fseek(source, item->file.offset + item->file.data->filename + item->file.data->extra + sizeof(File), SEEK_SET) < 0) {
        fprintf(stdout, "fail to __inflate caused by fseek (%d)\n", errno);
        return -1;
    }
    printf("%d\n", item->file.offset + item->file.data->filename + item->file.data->extra + sizeof(File));
    printf("%d\n", ftell(source));

    FILE * destination = fopen(filepath, "w+");
    if(destination == NULL) {
        fprintf(stdout, "fail to __inflate caused by fopen (%d)\n", errno);
        return -1;
    }
    printf("method: %d\n", item->file.data->method);

    ret = inflateInit2(&strm, -MAX_WBITS);
    if (ret != Z_OK) {
        fprintf(stdout, "fail to __inflate caused by inflateInit\n");
        return -1;
    }

    uint32_t remain = item->file.data->data.compressed;

    do {
        uint32_t progress = remain > CHUNK ? CHUNK : remain;
        printf("%d\n", progress);
        strm.avail_in = fread(in, 1, progress, source);
        printf("%d\n", ftell(source));
        printf("%d\n", strm.avail_in);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            fclose(destination);
            return -1;
        }
        if (strm.avail_in == 0) {
            printf("why avail in == 0 && %d\n", progress);
            break;
        }
        strm.next_in = in;
        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_SYNC_FLUSH);
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
                printf("need dic error\n");
                (void)inflateEnd(&strm);
                fclose(destination);
                return -1;
            case Z_DATA_ERROR:
                printf("data error : %s\n", strm.msg);
                (void)inflateEnd(&strm);
                fclose(destination);
                return -1;
            case Z_MEM_ERROR:
                printf("memory error\n");
                (void)inflateEnd(&strm);
                fclose(destination);
                return -1;
            case Z_STREAM_ERROR:
                printf("stream error\n");
                (void)inflateEnd(&strm);
                fclose(destination);
                return -1;
            }

            have = CHUNK - strm.avail_out;
            printf("have: %d\n", have);
            if (fwrite(out, 1, have, destination) != have || ferror(destination)) {
                (void)inflateEnd(&strm);
                fclose(destination);
                return -1;
            }
        } while (strm.avail_out == 0);
        remain -= progress;
        printf("%d\n", remain);
    } while (ret != Z_STREAM_END);
    fprintf(stdout, "succeed to inflate\n");
    fclose(destination);
    return 0;
}

static int __deflate(Item * item, FILE * source, const char * filepath, int level)
{
    printf("deflate\n");
}

static uint32_t __get_signature(FILE * fp);

#ifdef __cplusplus
extern "C" {
#endif
    EMSCRIPTEN_KEEPALIVE int unzip(const char * path)
    {
        FILE * fp = fopen(path, "rb");
        if(fp) {
            Root root = { 0, };
            uint32_t signature = 0;
            while((signature = __get_signature(fp)) != 0) {
                if(signature == SIGNATURE_FILE) {
                    if(__generate_file_tag(&root, fp) == NULL) {
                        fprintf(stdout, "fail to unzip caused by __generate_file_tag\n");
                        __clear_root(&root);
                        fclose(fp);
                        return -1;
                    }
                } else if(signature == SIGNATURE_CENTRAL) {
                    if(__generate_central_tag(&root, fp) == NULL) {
                        fprintf(stdout, "fail to unzip caused by __generate_central_tag\n");
                        __clear_root(&root);
                        fclose(fp);
                        return -1;
                    }
                } else if(signature == SIGNATURE_CENTRAL_END) {
                    if(__generate_end_tag(&root, fp) == NULL) {
                        fprintf(stdout, "fail to unzip caused by __generate_end_tag\n");
                        __clear_root(&root);
                        fclose(fp);
                        return -1;
                    }
                    break;
                } else {
                    __clear_root(&root);
                    fclose(fp);
                    fprintf(stdout, "fail to unzip caused by unknown error :%x\n", signature);
                    break;
                }
            }

            Item * item = root.head;
            char workdir[256];
            workdir[0] = 0;
            __filename(workdir, 256, path);
            char filepath[PATH_MAX];

            while(item) {
                snprintf(filepath, PATH_MAX, "%s/%s", workdir, item->file.name);
                printf("filepath: %s\n", filepath);
                // FILE CHECK
                __mkdir(filepath, 1);
                printf("%s\n", filepath);
                __inflate(item, fp, filepath);


                item = item->next;
            }
            __clear_root(&root);
            fclose(fp);
        }
        return -1;
    }
    EMSCRIPTEN_KEEPALIVE int zip(const char * path)
    {
        fprintf(stdout, "implement this\n");
        return -1;
    }
#ifdef __cplusplus
}
#endif // 

static uint32_t __get_signature(FILE * fp)
{
    uint32_t v = 0;
    if(fread(&v, sizeof(uint8_t), sizeof(uint32_t), fp) == sizeof(uint32_t)) {
        if(fseek(fp, -4, SEEK_CUR) < 0) {
            fprintf(stdout, "fail to fseek caused by %d\n", errno);
            return 0;
        }
        return v;
    }
    // TODO: CHECK FAIL REASON
    fprintf(stdout, "fail to get signature\n");
    return 0;
}