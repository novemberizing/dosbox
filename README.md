__NOVEMBERIZING DOSBOX FOR HTML5__
==================================

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