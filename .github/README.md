# simh with BlinkenBone and PiPD11 extensions

This repository is a form of simh/simh that integrates the following:
- [simh](https://github.com/simh/simh): The Computer History Simulation Project
- [BlinkenBone](https://github.com/j-hoppe/BlinkenBone): extend the SimH simulator with real or simulated console panels
- [PiDP11](https://obsolescence.wixsite.com/obsolescence/pidp-11): recreating the PDP-11/70

The BlinkenBone and PiDP11 sources have been reorganized a little and merged into the simh source tree (including changes to ``scp.c`` and the addition of ``REALCONS``). To build with REALCONS support, build simh with ``USE_REALCONS=1``; to build with PIDP11 support (on a Raspberry Pi only), build with ``USE_PIDP11=1``. The BlinkenBone and PiDP11 binaries can be built on Linux by running ``make.sh`` in the ``BlinkenBone`` subdirectory. Unlike Joerge Hoppe's original distribution, this builds only for the current machine (not cross-compilation). It has not yet been updated to build on Windows, and the PiDP11 panelserver has not been built for 64-bit ARM. The Raspberry Pi 4 changes for the PiDP8 are not yet merged.

The PiDP panelserver has been rewritten to use libgpiod (rather than directly manipulating device register by mmap'ing /dev/mem). This incurs more CPU overhead (because GPIO actions are done by ioctls). On slower CPUs it may be desirable to use Oscar's binaries instead.

Binaries are built in the tree. This does not (yet) replicate Oscar Vermeulen's PiDP11 installation and runtime scripts (see http://pidp.net/pidp11/pidp11.tar.gz), not does it include the [PDP-11 disk images](http://pidp.net/pidp11/systems.tar.gz). 

Please consult the individual README and LICENSE files in the tree.