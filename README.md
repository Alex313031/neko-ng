# Neko-ng

Neko98 Reborn!

<p align="center">
<img src="./assets/neko.png?raw=true" width="48px">
</p>

## About
Modernized Win32 version of the [classic Neko desktop toy](https://en.wikipedia.org/wiki/Neko_(software)).

 - Written in C++, this fork aims to modernize the codebase, fix bugs, and ensure it runs on Windows 2000 - Windows 11.

A full history of the Neko desktop toy, and all the platforms it has been ported to, can be found in this nice writeup > https://eliotakira.com/neko/.  
See also this history page > https://nekothecat.neocities.org/

## Building

It can be built with Visual Studio 2005 - 2022, however, the primary way I use to build it is with GN/Ninja,
using the BUILD.gn files inside. TODO: Add build instructions

### Support Info
Support for Windows 9x and NT 4.0 is not planned, the original releases for Win 9x are [Here](./original/Releases/Neko98_1999_release.zip)
and original release for NT 4.0 is [Here](./original/Releases/Neko98_NT4.0_2012_i386.zip) (both are inside this repo in ./original/Releases/).

 - The [original](./original) directory contains original sources and releases for the 1998 and 2010 builds.

## Credits

Masayuki Koba - First open source port, for X Window System

Dara Khani & Michael Bankstahl - Independently made the first Win16 ports. "Neko Runs Free" and "WNEKO"

David Harvey - First Win32 Port > https://github.com/eliot-akira/neko/tree/main/1998-windows-neko98

[neozeed](https://virtuallyfun.com/) - First Win64 Port > https://github.com/neozeed/neko98 https://sourceforge.net/projects/neko98/

[leiqunni](https://www.facebook.com/hiroyuki.takeda) - For some fixes from his Win32 fork > https://github.com/leiqunni/Neko98

[Eliot Akira](https://eliotakira.com/) - Documents all the platforms it was ported to > https://github.com/eliot-akira/neko
