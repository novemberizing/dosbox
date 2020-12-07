__NOVEMBERIZING DOSBOX FOR HTML5__
==================================

```
CXXFLAGS="-g -O3" LDFLAGS="-s TOTAL_MEMORY=67108864 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASYNCIFY -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0" emconfigure ./configure
emmake make
em++  -g -O3 -mno-ms-bitfields   -s TOTAL_MEMORY=67108864 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASYNCIFY -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -o dosbox.html dosbox.o  cpu/libcpu.a debug/libdebug.a dos/libdos.a fpu/libfpu.a  hardware/libhardware.a gui/libgui.a ints/libints.a misc/libmisc.a shell/libshell.a hardware/mame/libmame.a hardware/serialport/libserial.a libs/gui_tk/libgui_tk.a -lX11 -lGL
```

## BUILD ENVIRONMENT

```
sudo apt install git
sudo apt install build-essential
sudo apt install python
sudo apt install python3
```

https://emscripten.org/docs/getting_started/downloads.html

## BUILD ENVIRONMENT FOR WINDOWS (USING WINDOWS SUBSYSTEM - UBUNTU 20.04)

```
sudo apt install git
sudo apt install build-essential
sudo apt install python
sudo apt install python3
```

https://code.visualstudio.com/docs/remote/wsl

