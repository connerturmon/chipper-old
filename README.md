#### Emulator Projects
* Chipper
* [Gibi](https://github.com/connerturmon/gibi) *WIP*

# Chipper - A Chip-8 Emulator in C

Chipper is a Chip-8 Emulator written in C with the SDL2 framework for
audio, video, and input. This emulator is the first of hopefully many emulator
projects, mostly aimed at providing an educational learning experience in
low level hardware emulation.

This emulator is not expected to be fully fleshed out, as I fairly new to software
development and computer engineering. I do hope, however, that with time I am
able to improve on this emulator and many more in the future.

## Compilation

To compile Chipper, make sure that you have [SDL2](https://www.libsdl.org/download-2.0.php)
installed on your system.
```
git clone https://github.com/connerturmon/chipper.git
cd chipper/
make
```

If you are not on a Unix based system, you may need to change:
```
"#include <SDL2/SDL.h>"
```
to
```
"#include <SDL.h>"
```
and then build. This depends on your system/IDE.

## Using Chipper

Chipper uses these keys as the keyboard layout:

```
1 2 3 4

Q W E R

A S D F

Z X C V
```

Chipper is started by passing a ROM file as a command line argument to
Chipper.

**Linux:**

```
./chipper ~/ROMS/PONG
```

**Windows:**

*(Open command line in directory with executable)*

```
chipper C:\ROMS\PONG
```