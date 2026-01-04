# simh with BlinkenBone and PiPD11 extensions

This repository integrates [simh](https://github.com/simh/simh)_: The Computer History Simulation Project_
with REALCONS and BlinkenBone support and a PiDP11 panel server.
The non-simh sources are based upon:
- [BlinkenBone](https://github.com/j-hoppe/BlinkenBone)_: extend the SimH simulator with real or simulated console panels_
- [PiDP11](https://obsolescence.wixsite.com/obsolescence/pidp-11)_: recreating the PDP-11/70_

Please consult the individual README and LICENSE files in the tree.

The BlinkenBone and PiDP11 sources have been reorganized into a submodule,
with changes to simh itself in the simh source tree
(including changes to ``scp.c`` and the addition of ``REALCONS``).
To build with REALCONS support, build simh with ``USE_REALCONS=1``;
to build for PIDP11 (on a Raspberry Pi only),
build with ``USE_PIDP11=1``.
The BlinkenBone and PiDP11 binaries can be built on Linux by
running ``make.sh`` in the ``BlinkenBone`` subdirectory.
Unlike Joerge Hoppe's original distribution,
this builds only for the current machine (not cross-compilation).

The PiDP panelserver has been rewritten to use libgpiod
(rather than directly manipulating device registers by mmap'ing ```/dev/mem```).
It works on both 32-bit and 64-bit systems.
There are different source files for v1 (which, oddly, is the
package ```libgpiod2```) and v2 (package ```libgpiod3```).

Panel servers for other machines (PDP-1, PDP-8, PDP-10) are yet to be written.

##### Clone repository

To include the ```BlinkenBone``` submodule, clone this repository with
```bash
git clone https://github.com/jdbruner/simh --recursive
```

##### Build/update PiDP11 on Raspberry Pi

This has diverged from [Oscar Vermeulen's PiDP11 installation and runtime
[scripts](http://pidp.net/pidp11/pidp11.tar.gz).
It does not include the
[PDP-11 disk images](http://pidp.net/pidp11/systems.tar.gz). 

To build on a Raspberry Pi, change directory to the root of the
GIT repository and do the following:
```bash
cd BlinkenBone
./make.sh
```
On some systems you may need to configure the kernel variable
```dev.tty.legacy_tiocsti``` to 1
to allow the console to inject characters into the simh console input.

##### Build/install on Windows

Windows support is very brittle at this time. Nonetheless, if you want to try...

simh itself has good support for building and running on Windows.
There is no supported version of Windows on the Raspberry Pi,
so the PiDP panel simulators are not applicable.
However, with some work it is possible to use simh with the Java panel servers.

Java is "write once, run anywhere",
so you can build ``BlinkenBone/javapanelsim/panelsim_all.jar``
on Linux and run it on Windows with an equivalent JDK.
To build it on Windows, install JDK 19 or newer and also install Apache Ant;
then cd to the ``BlinkenBone`` directory and run ``make.bat``.

The ``Visual Studio Projects`` folder contains a project file to build
a PDP11 with REALCONS,
but this has only been used with Visual Studio 2022.
It creates the executable ``BIN/pdp11_realcons.exe``
If you need to rebuild it for an older IDE
(particularly if you want to build for Windows XP using Visual Studio 2008),
then you will probably also need to rebuild ``BlinkenBone/3rdparty/oncrpc_win32``.
Warning: there be dragons here...

There is no support for building a REALCONS-enabled PDP11 simulator
using mingw64.

##### Installation

Installation runs on the build machine only at this time.
From the root of the simh repository, do the following:
```bash
cd BlinkenBone
./install.sh
```
which does the following:
1. ```sysctl dev.tty.legacy_tiocsti=1```
2. Create the user ```pidp11```
3. Populate the ```pidp11``` home directory with the script ```pdp.sh```
   (which connects to the ```screen``` session) and add it to the
   ```.profile```
4. Create ```/opt/pidp11``` and populate it with ```client11```, ```server11```,
   ```getcsw```, and ```pidp11.sh```
5. Create an initial version of ```/opt/pidp11/systems``` containing ```idled```.
6. Install the systemd service files and start the services


If you want the simulated system to share the Ethernet on your device,
un-comment the following line in ```install.sh```:
```bash
# setcap cap_net_raw,cap_net_admin=+ep ${PIDP11_OPT}/client11q
```

If the address select and data select LEDs rotate in the opposite
direction of the corresponding knobs,
then ``server11`` should be invoked with the ``-r`` flag.
Change the ``ExecStart`` line in ``/usr/local/lib/systemd/system/pidp11panel.server`` to:
```bash
ExecStart=/opt/pidp11/server11 -r
```

##### Major Changes Relative to the Upstream

###### client11 (simh)

Relative to [simh](https://github.com/simh/simh),
the ```makefile``` and sources have been
updated with two additional environment variable options:
- ```USE_REALCONS=1```: Include the ```REALCONS``` client support from Joerg Hoppe
- ```USE_PIDP11=1```: Include ```REALCONS``` and an additional change for PiDP-11/70 support (see below).

If these are not set, the ```makefile``` will build all of the simulators
the same as the upstream.
On a Raspberry Pi, ```make.sh``` command in the ```BlinkenBone``` directory
will build the PiDP11 variant of the pdp11 and rename it to ```client11```.

The only difference for the PiDP-11/70 (```USE_PIDP11=1```)
is the handling of a power switch event.
The address select knob on the PiDP-11 generates this event,
which indicates that the simulator should be restarted.
For other ```REALCONS``` builds, this event causes simh to break
to the command line with the ```quit``` command pre-populated.
##### server11

The PiDP-11 panel server has been rewritten to use the ```lipgpiod```
library instead of mapping ```/dev/mem``` and accessing the GPIO
device registers directly.
It can be run as a non-root user that belongs to the ```gpio``` group.

##### getcsw

This new utility replaces ```scansw```.
It reads the switch register from the panel server via the Blinkenlight
API (rather than directly from the PiDP-11 GPIOs.
There are some command-line options that control how the value is formatted.

##### systemd services

There are two systemd services:
- *pidp11panel.service* - runs the panel server (server11) under
  the non-privileged account ```pidp11``` with group ```gpio``` and
  with the ability to set a real-time thread priority.
  Unlike Oscar's version, the panel server runs continuously
  (even when the client is restarted).
- *pidp11.service* - runs ```/opt/pidp11/pidp11.sh``` with a new ```screen```
  under the non-privileged account ```pidp11```.

##### /opt/pidp11/pidp11.sh

This script is the main client loop.
It reads the switch register (with ```getcsw```)
and selects the system to simulate.
Oscar's version creates a command script in
```/var/run``` to launch the simulation and also to control what happens
when the address select knob is pressed.
This version does not use an intermediate file;
instead, simh uses the ```boot.ini``` script in the selected system directory.
When the address select knob is pressed, simh exits.
The simh exit status and behavior of ```pidp11.sh``` is:
- 0 if the RUN/HALT switch is in the RUN position: the process repeats
  (reading the switch register, selecting the system, starting simh)
- 1 if the RUN/HALT switch is in the HALT position: the script exits
  (which causes the ```pidp11.service``` to terminate)

