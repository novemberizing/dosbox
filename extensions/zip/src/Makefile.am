AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES=libnovemberizingzip.la
libnovemberizingzip_la_SOURCES=zip.c

if EMSCRIPTEN
bin_PROGRAMS = zip

zip_CXXFLAGS = -g -O3
zip_LDFLAGS = -s TOTAL_MEMORY=67108864 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASYNCIFY -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s EXPORT_ES6=1 -s MODULARIZE=1 -s "EXPORTED_FUNCTIONS=['_zip', '_unzip']"
zip_SOURCES = 
zip_LDADD = libnovemberizingzip.la

zipdir = $(datadir)
zip_DATA = zip.wasm
endif