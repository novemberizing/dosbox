__NOVEMBERIZING DOSBOX PORTED TO EMSCRIPTEN__
=============================================

"dosbox" is distributed under the GNU General Public License.
See the COPYING file for more information.


DOSBOX, EMSCRIPTEN

## __BUILD__

```sh
$ npm run build
```

## __INSTALL__

```sh
$ npm install --save @novemberizing/dosbox
```

## __SAMPLE__

```
import dosbox from '@novemberizing/dosbox';

dosbox.run(null)
       .then(o => console.log(o))
       .catch(e => console.log(e));
```



```sh
$ cd dosbox-0.74-3
$ LIBS="-lidbfs.js" CXXFLAGS="-g -O3" LDFLAGS="-s TOTAL_MEMORY=67108864 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASYNCIFY -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s EXPORT_ES6=1 -s MODULARIZE=1 -s EXPORTED_RUNTIME_METHODS=['FS']" emconfigure ./configure --bindir=${PWD}/../src --datarootdir=${PWD}/../build --host=`./config.guess` --with-zip=${PWD}/../../zip
$ emmake make clean
$ emmake make 
$ emmake make install
```


"prebuild": "cd dosbox-0.74-3 && 

emconfigure ./configure --bindir=${PWD}/../src --datarootdir=${PWD}/../build --host=`./config.guess`

&&

emmake make clean &&
emmake make &&
emmake make install",




DOSBox ported to Emscripten
About
DOSBox is an open source DOS emulator designed for running old games. Emscripten compiles C/C++ code to JavaScript. This is a version of DOSBox which can be compiled with Emscripten to run in a web browser. It allows running old DOS games and other DOS programs in a web browser.

DOSBox is distributed under the GNU General Public License. See the COPYING file for more information.

Status
Em-DOSBox runs most games successfully in web browsers. Although DOSBox has not been fully re-structured for running as an Emscripten main loop, most functionality is available thanks to emterpreter sync. A few programs can still run into problems due to paging exceptions.

Other issues
Game save files are written into the Emscripten file system, which is by default an in-memory file system. Saved games will be lost when you close the web page.
Compiling in Windows is not supported. The build process requires a Unix-like environment due to use of GNU Autotools. See Emscripten issue 2208.
Emscripten issue 1909 used to make large switch statements highly inefficient. It seems fixed now, but V8 JavaScript Engine issue 2275 prevents large switch statements from being optimized. Because of this, the simple, normal and prefetch cores are automatically transformed. Case statements for x86 instructions become functions, and an array of function pointers is used instead of the switch statements. The --enable-funarray configure option controls this and defaults to yes.
The same origin policy prevents access to data files when running via a file:// URL in some browsers. Use a web server such as python -m SimpleHTTPServer instead.
In Firefox, ensure that dom.max_script_run_time is set to a reasonable value that will allow you to regain control in case of a hang.
Firefox may use huge amounts of memory when starting asm.js builds which have not been minified.
The FPU code uses doubles and does not provide full 80 bit precision. DOSBox can only give full precision when running on an x86 CPU.
Compiling
Use the latest stable version of Emscripten (from the master branch). For more information see the the Emscripten installation instructions. Em-DOSBox depends on bug fixes and new features found in recent versions of Emscripten. Some Linux distributions have packages with old versions, which should not be used.

First, create ./configure by running ./autogen.sh. Then configure with emconfigure ./configure and build with make. This will create src/dosbox.js which contains DOSBox and src/dosbox.html, a web page for use as a template by the packager. These cannot be used as-is. You need to provide DOSBox with files to run and command line arguments for running them.

This branch supports SDL 2 and uses it by default. Emscripten will automatically fetch SDL 2 from Emscripten Ports and build it. Use of make -j to speed up compilation by running multiple Emscripten processes in parallel may break this. Once SDL 2 has been built by Emscripten, you can use make -j. To use a different pre-built copy of Emscripten SDL 2, specify a path as in emconfigure ./configure --with-sdl2=/path/to/SDL-emscripten. To use SDL 1, give a --with-sdl2=no or --without-sdl2 argument to ./configure.

Emscripten emterpreter sync is used by default. This enables more DOSBox features to work, but requires an Emscripten version after 1.29.4 and may cause a small performance penalty. To disable use of emterpreter sync, add the --disable-sync argument to ./configure. When sync is used, the Emscripten memory initialization file is enabled, which means dosbox.html.mem needs to be in the same folder as dosbox.js. The memory initialization file is large. Serve it in compressed format to save bandwidth.

WebAssembly
WebAssembly is a binary format which is meant to be a more efficient replacement for asm.js. To build to WebAssembly instead of asm.js, the -s WASM=1 option needs to be added to the final emcc link command. There is no need to rebuild the object files. You can do this using the --enable-wasm option when running ./configure. Since WebAssembly is still under very active development, use the latest incoming Emscripten and browser development builds like Firefox Nightly and Chrome Canary.

Building with WebAssembly changes the dosbox.html file. Files packaged using a dosbox.html without WebAssembly will not work with a WebAssembly build.

Running DOS Programs
To run DOS programs, you need to provide a suitable web page, load files into the Emscripten file system, and give command line arguments to DOSBox. The simplest method is by using the included packager tools.

The normal packager tool is src/packager.py, which runs the Emscripten packager. It requires dosbox.html, which is created when building Em-DOSBox. If you do not have Emscripten installed, you need to use src/repackager.py instead. Any packager or repackager HTML output file can be used as a template for the repackager. Name it template.html and put it in the same directory as repackager.py.

The following instructions assume use of the normal packager. If using repackager, replace packager.py with repackager.py. You need Python 2 to run either packager.

If you have a single DOS executable such as Gwbasic.exe, place it in the same src directory as packager.py and package it using:

./packager.py gwbasic Gwbasic.exe

This creates gwbasic.html and gwbasic.data. Placing those in the same directory as dosbox.js and viewing gwbasic.html will run the program in a web browser:

Some browsers have a same origin policy that prevents access to the required data files while using file:// URLs. To get around this you can use Python's built in Really Simple HTTP Server and point the browser to http://localhost:8000.

python -m SimpleHTTPServer 8000

If you need to package a collection of DOS files. Place all the files in a single directory and package that directory with the executable specified. For example, if Major Stryker's files are in the subdirectory src/major_stryker and it's launched using STRYKER.EXE you would package it using:

./packager.py stryker major_stryker STRYKER.EXE

Again, place the created stryker.html and stryker.data files in the same directory as dosbox.js and view stryker.html to run the game in browser.

You can also include a DOSBox configuration file that will be acknowledged by the emulator to modify any speed, audio or graphic settings. Simply include a dosbox.conf text file in the package directory before you run ./packager.py.

To attempt to run Major Stryker in CGA graphics mode, you would create the configuration file /src/major_stryker/dosbox.conf and include this body of text:

[dosbox]
machine=cga
Then package it using:

./packager.py stryker-cga major_stryker STRYKER.EXE

Credits
Most of the credit belongs to the DOSBox crew. They created DOSBox and made it compatible with a wide variety of DOS games. Ismail Khatib got DOSBox to compile with Emscripten, but didn't get it to work. Boris Gjenero started with that and got it to work. Then, Boris re-implemented Ismail's changes a cleaner way, fixed issues and improved performance to make many games usable in web browsers. Meanwhile, Alon Zakai quickly fixed Emscripten bugs which were encountered and added helpful Emscripten features.






```sh
npm run prebuild
```



## BUILD

```
CXXFLAGS="-g -O3" LDFLAGS="-s TOTAL_MEMORY=67108864 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASYNCIFY -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -lidbfs.js -s EXTRA_EXPORTED_RUNTIME_METHODS=['FS']" emconfigure ./configure
emmake make
em++  -g -O3 -mno-ms-bitfields   -s TOTAL_MEMORY=67108864 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASYNCIFY -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -o dosbox.html dosbox.o  cpu/libcpu.a debug/libdebug.a dos/libdos.a fpu/libfpu.a  hardware/libhardware.a gui/libgui.a ints/libints.a misc/libmisc.a shell/libshell.a hardware/mame/libmame.a hardware/serialport/libserial.a libs/gui_tk/libgui_tk.a -lX11 -lGL
docker run -it --rm --name dosbox -v ${PWD}:/usr/local/apache2/htdocs -p 80:80 httpd
```

## BUILD

```
CXXFLAGS="-g -O3" LDFLAGS="-s TOTAL_MEMORY=67108864 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASYNCIFY -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -lidbfs.js -s EXPORTED_RUNTIME_METHODS=['FS'] -s ENVIRONMENT=web -s FILESYSTEM=0" emconfigure ./configure --bindir=${PWD}/../docs --datarootdir=${PWD}/../build --host=`./config.guess`
emmake make
emmake make install
```



## BUILD ENVIRONMENT

```
sudo apt install git
sudo apt install build-essential
sudo apt install automake autoconf
sudo apt install python
sudo apt install python3
```

https://emscripten.org/docs/getting_started/downloads.html

## BUILD ENVIRONMENT FOR WINDOWS (USING WINDOWS SUBSYSTEM - UBUNTU 20.04)

```
sudo apt install git
sudo apt install build-essential
sudo apt install automake autoconf
sudo apt install python
sudo apt install python3
```

https://code.visualstudio.com/docs/remote/wsl

https://github.com/emscripten-core/emscripten/issues/4515

warning: Audio callback had starved sending audio by 0.761900226757291 seconds.

https://stackoverflow.com/questions/54617194/how-to-save-files-from-c-to-browser-storage-with-emscripten

https://zellwk.com/blog/publish-to-npm/

https://stackoverflow.com/questions/35840047/configure-automake-to-install-extra-programs