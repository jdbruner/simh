# simh with BlinkenBone and PiPD11 extensions

This repository is a fork of simh/simh that integrates the following:
- [simh](https://github.com/simh/simh): The Computer History Simulation Project
- [BlinkenBone](https://github.com/j-hoppe/BlinkenBone): extend the SimH simulator with real or simulated console panels
- [PiDP11](https://obsolescence.wixsite.com/obsolescence/pidp-11): recreating the PDP-11/70

Please consult the individual README and LICENSE files in the tree.

The BlinkenBone and PiDP11 sources have been reorganized and merged into the simh source tree (including changes to ``scp.c`` and the addition of ``REALCONS``). To build with REALCONS support, build simh with ``USE_REALCONS=1``; to build for PIDP11 (on a Raspberry Pi only), build with ``USE_PIDP11=1``. The BlinkenBone and PiDP11 binaries can be built on Linux by running ``make.sh`` in the ``BlinkenBone`` subdirectory. Unlike Joerge Hoppe's original distribution, this builds only for the current machine (not cross-compilation).

The PiDP panelserver has been rewritten to use libgpiod (rather than directly manipulating device registers by mmap'ing ``/dev/mem``). It works on both 32-bit and 64-bit systems. It has not been ported to libgpiod2.

The PiDP8 panel server has not been updated.

###### Build/update PiDP11 on Raspberry Pi

This has diverged from [Oscar Vermeulen's PiDP11 installation and runtime scripts](http://pidp.net/pidp11/pidp11.tar.gz). It does not include the [PDP-11 disk images](http://pidp.net/pidp11/systems.tar.gz). 

To build on a Raspberry Pi, change directory to the root of the GIT repository and do the following:
```bash
sudo apt install ant rpcbind default-jdk libgpiod-dev libtirpc-dev 
sudo apt install libsdl2-dev libpcap-dev libreadline-dev libpcre3-dev libedit-dev libpng-dev libvdeplug-dev
cd BlinkenBone
./make.sh
```
An automated procedure to install this (```install.sh``` in the ```BlinkenBone``` directory) is still in development.

###### Build/install on Windows

Windows support is very brittle at this time. Nonetheless, if you want to try...

simh itself has good support for building and running on Windows.
There is no supported version of Windows on the Raspberry Pi, so the PiDP panel simulators are not applicable.
However, with some work it is possible to use simh with the Java panel servers.

Java is "write once, run anywhere", so you can build ``BlinkenBone/javapanelsim/panelsim_all.jar``
on Linux and run it on Windows with an equivalent JDK.
To build it on Windows, install JDK 19 or newer and also install Apache Ant;
then cd to the ``BlinkenBone`` directory and run ``make.bat``.

The ``Visual Studio Projects`` folder contains a project file to build a PDP11 with REALCONS,
but this has only been used with Visual Studio 2022. It creates the executable ``BIN/pdp11_realcons.exe``
If you need to rebuild it for an older IDE (particularly if you want to build for Windows XP
using Visual Studio 2008), then you will probably also need to rebuild ``BlinkenBone/3rdparty/oncrpc_win32``.
Warning: there be dragons here...

There is no support (at least not yet) for building a REALCONS-enabled PDP11 simulator using mingw64.
