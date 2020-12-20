#ifndef   __NOVEMBERIZING_ZIP__H__
#define   __NOVEMBERIZING_ZIP__H__

#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif 
#endif 

EMSCRIPTEN_KEEPALIVE int unzip(const char * path);
EMSCRIPTEN_KEEPALIVE int zip(const char * path);

#endif // __NOVEMBERIZING_ZIP__H__