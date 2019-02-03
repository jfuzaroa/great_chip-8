# The Great Chip-8 Emulator

## Introduction

Chip-8 is an interpreted programming language developed for some microcomputer
systems in the 1970s.
The Great Chip-8 emulator aims to emulate the hardware of these computers in
order to accurately reproduce their program execution.

If you would like to learn more about the specifications and implementations of
Great Chip-8, consider browsing the repository wiki (if it exists) or the
acknowledgments listed.

I have yet to implement any sound and there are still a few things to perfect
like input handling and emulation speed.

This was my first emulator but probably won't be my last.
In the future if I find the time, I would like to add extensive documentation
with doxygen and optimize graphics and input.

I had good fun writing it and would recommend anyone interested in computer
architecture, graphics, low-level programming, and/or game development try it
out too.
It's a great learning experience and is easily written in other languages if
you're not a fan of C.

## Dependencies

Great Chip-8 is written in C utilizing modern OpenGL for rendering and GLFW3
for input handling and window creation.
OpenGL extensions are loaded using GLEW.

In short order:

 - C compiler (you should have this)
 - [OpenGL](https://www.opengl.org) (you probably have this)
 - [GLEW](http://glew.sourceforge.net)
 - [GLFW3](https://github.com/glfw/glfw)

## Building

### _Make_
`$ make release`

### _CMake_
`$ CMake CMakeLists.txt`

## Running
`
$ cd great_chip-8

$ ./bin/great_chip-8 ./roms/_[ROM_FILE]_
`

## Acknowledgements
[Google](https://www.google.com)

[EmuDev Subreddit](https://www.reddit.com/r/EmuDev)

[CHIP-8 Wikipedia article](https://www.wikiwand.com/en/CHIP-8)

[Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

[How to write an emulator (Chip-8 interpreter)](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter)

[Learn OpenGL](https://learnopengl.com)
