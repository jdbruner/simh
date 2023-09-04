# simh with BlinkenBone and PiPD11 extensions

This repository is a fork of simh/simh that integrates the following:
- [simh](https://github.com/simh/simh): The Computer History Simulation Project
- [BlinkenBone](https://github.com/j-hoppe/BlinkenBone): extend the SimH simulator with real or simulated console panels
- [PiDP11](https://obsolescence.wixsite.com/obsolescence/pidp-11): recreating the PDP-11/70

The BlinkenBone and PiDP11 sources have been reorganized a little and merged into the simh source tree (including changes to ``scp.c`` and the addition of ``REALCONS``). To build with REALCONS support, build simh with ``USE_REALCONS=1``; to build with PIDP11 support (on a Raspberry Pi only), build with ``USE_PIDP11=1``. The BlinkenBone and PiDP11 binaries can be built on Linux by running ``make.sh`` in the ``BlinkenBone`` subdirectory. Unlike Joerge Hoppe's original distribution, this builds only for the current machine (not cross-compilation). It has not yet been updated to build on Windows.

The PiDP panelserver has been rewritten to use libgpiod (rather than directly manipulating device registers by mmap'ing ``/dev/mem``). This incurs more CPU overhead (because all GPIO actions are done with ioctls). On slower CPUs it may be desirable to use Oscar's panelserver (``/opt/pidp11/bin/server11``) instead.

The PiDP8 panel server has not been updated, and the PiDP11 panelserver has not (yet) been tested for 64-bit ARM.

This does not replicate Oscar Vermeulen's PiDP11 installation and runtime scripts (see http://pidp.net/pidp11/pidp11.tar.gz), not does it include the [PDP-11 disk images](http://pidp.net/pidp11/systems.tar.gz). Those should be installed as per Oscar's instructions. Then, to build and install manually on a Raspberry Pi as replacements for the binaries that were installed by Oscar's distribution, change directory to the root of the GIT repository and do the following:
```bash
sudo apt-get install libgpiod-dev
USE_PIDP11=1 make pdp11
(cd BlinkenBone; ./make.sh)
cd BIN
cp pdp11 /opt/pidp11/bin/client11
cp pidp1170_blinkenlightd /opt/pidp11/bin/server11
cp scansw /opt/pidp11/bin
```

Please consult the individual README and LICENSE files in the tree.
