![CHIP-8](https://github.com/connerturmon/chipper/blob/master/resource/chip8.png)

[![Build Status](https://travis-ci.org/connerturmon/chipper.svg?branch=master)](https://travis-ci.org/connerturmon/chipper)
[![GitHub release](https://img.shields.io/github/release/connerturmon/chipper.svg)](https://github.com/connerturmon/chipper/releases/tag/v1.0)
[![Gitter chat](https://badges.gitter.im/connerturmon-chipper/Lobby.png)](https://gitter.im/connerturmon-chipper/Lobby)

# Chipper - A Chip-8 Emulator in C

Chipper is a Chip-8 Emulator written in C with the SDL2 framework for
audio, video, and input.

#### Overview:
* [About](https://github.com/connerturmon/chipper#about)
* [Building](https://github.com/connerturmon/chipper#building)
    * [Linux](https://github.com/connerturmon/chipper#linux)
    * [Windows & OSX](https://github.com/connerturmon/chipper#windows--osx)
* [Compiling & Linking](https://github.com/connerturmon/chipper#compiling--linking)
    * [Linux](https://github.com/connerturmon/chipper#linux-1)
    * Windows & OSX <sub>*coming soon*</sub>
* [Using Chipper](https://github.com/connerturmon/chipper#using-chipper)

#### Want to Program Your Own CHIP-8 Emulator?
Check out: [Our Wiki For Learning How To Make a CHIP-8 Emulator](https://github.com/connerturmon/chipper/wiki)

## About

This emulator is the first of hopefully many emulator
projects, mostly aimed at providing an educational learning experience in
low level hardware emulation.

This emulator is not expected to be fully fleshed out, as I am fairly new to software
development and computer engineering. I do hope, however, that with time I am
able to improve on this emulator and many more in the future.

## Building

Here are the steps for building the most current version of Chipper on your system:

### Dependencies

You must have [SDL2](https://www.libsdl.org/download-2.0.php) installed on 
your system. This varies from system to system.

### Linux:
#### Ubuntu (16.04 LTS +)
```
sudo apt-get install libsdl2-2.0-0 libsdl2-dev
```
#### Arch / Manjaro
```
sudo pacman -S sdl2
```
#### Fedora
```
sudo yum install SDL2
```

### Windows & OSX:
Download and install the Development Libraries located at the bottom of the [SDL2 Download Page](https://www.libsdl.org/download-2.0.php).

Actually getting them to compile SDL2 code is beyond the scope of this documentation, but a simple google search will return instructions on how to compile SDL2 code for your given OS + IDE.

### Compiling & Linking
<sup>Chipper does not currently have direct compilation support for Windows / OSX.</sup>
#### Linux:
```Bash
git clone https://github.com/connerturmon/chipper.git
cd chipper/
make
```
A binary file `chipper` will be located inside the nest/ directory.

## Using Chipper

Chipper uses these keys as the keyboard layout:

`1 2 3 4`

`Q W E R`

`A S D F`

`Z X C V`

Chipper is started by passing a ROM file as a command line argument to Chipper.

**Linux:**

```
./chipper ~/ROMS/PONG
```

**Windows:**

*(Open command line in directory with executable)*

```
chipper C:\ROMS\PONG
```
