AM_CPPFLAGS = -I$(top_srcdir)/include

SUBDIRS = cpu debug dos fpu gui hardware libs ints misc shell platform 

if HAVE_WINDRES
ico_stuff = winres.rc
endif

.rc.o:
	$(WINDRES) -o $@ $<

if EMSCRIPTEN
bin_PROGRAMS = dosbox.js dosbox.wasm

dosbox_wasm_SOURCES = 
dosbox_wasm_LDADD =

dosbox_js_CXXFLAGS = -g -O3
dosbox_js_LDFLAGS = -s TOTAL_MEMORY=67108864 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASYNCIFY -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s EXPORT_ES6=1 -s MODULARIZE=1 -s EXPORTED_RUNTIME_METHODS=['FS']
dosbox_js_SOURCES = dosbox.cpp $(ico_stuff)
dosbox_js_LDADD = cpu/libcpu.a debug/libdebug.a dos/libdos.a fpu/libfpu.a  hardware/libhardware.a gui/libgui.a \
               ints/libints.a misc/libmisc.a shell/libshell.a hardware/mame/libmame.a \
               hardware/serialport/libserial.a libs/gui_tk/libgui_tk.a -lidbfs.js
else
bin_PROGRAMS = dosbox
dosbox_SOURCES = dosbox.cpp $(ico_stuff)
dosbox_LDADD = cpu/libcpu.a debug/libdebug.a dos/libdos.a fpu/libfpu.a  hardware/libhardware.a gui/libgui.a \
               ints/libints.a misc/libmisc.a shell/libshell.a hardware/mame/libmame.a \
               hardware/serialport/libserial.a libs/gui_tk/libgui_tk.a
EXTRA_DIST = winres.rc dosbox.ico
endif
