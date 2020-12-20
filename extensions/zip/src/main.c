#include <novemberizing/zip.h>

#include <stdio.h>

int main(int argc, char ** argv)
{
    if(argc <= 1) {
        printf("./example-unzip [a zip file path - relative]\n");
        return -1;
    }
    const char * path = argv[1];
    return unzip(path);
}