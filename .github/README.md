# simh with BlinkenBone and PiDP extensions

[This is ``.github/README.md``. You can find the simh README [here](../README.md).]

This repository integrates [simh](https://github.com/simh/simh)_: The Computer History Simulation Project_
with REALCONS and BlinkenBone support and a PiDP11 panel server.
The non-simh sources are based upon:
- [BlinkenBone](https://github.com/j-hoppe/BlinkenBone)_: extend the SimH simulator with real or simulated console panels_
- [PiDP11](https://obsolescence.wixsite.com/obsolescence/pidp-11)_: recreating the PDP-11/70_

Please consult the individual README and LICENSE files in the tree.

The BlinkenBone and PiDP11 sources have been reorganized into a submodule,
with changes to simh itself in the simh source tree
(including changes to ``scp.c`` and the addition of ``REALCONS``).
There are additional build targets for REALCONS
and Richard Cornwell's PIPANEL interface to the PiDP10 in the pdp10-kx simulators:
- ``pdp11_realcons`` (for the PiDP11)
- ``pdp10-ka_realcons`` and ``pdp10-ka_pipanel`` (PDP-10 model KA10)
- ``pdp10-ki_realcons`` and ``pdp10-ki_pipanel`` (PDP-10 model KI10)
- ``pdp10-kl_realcons`` and ``pdp10-kl_pipanel`` (PDP-10 model KL10)
- ``pdp10-ks_realcons`` abd ``pdp10-ks_pipanel`` (PDP-10 model KS10)
- ``pdp8_realcons`` (for the PiDP8, untested)
- ``pdp15_realcons`` (PDP-15, incomplete)

There are PiDP panel servers, modified from Oscar Vermeulen's original versions, for:
- ``11/70`` (PDP-11/70)
- ``PDP10-KA10`` (PDP-10)
- ``PDP8I`` (PDP-8, untested)

There are Java panel servers, largely unmodified from Joerge Hoppe's versions, for:
- ``11/20`` (PDP-11/20)
- ``11/40`` (PDP-11/40)
- ``11/70`` (PDP-11/70)
- ``PDP10-KI10`` (PDP-10)
- ``PDP8I`` (PDP-8i)
- ``PDP15`` (PDP-15)

Note that the PiDP10 implements a KA10 panel but the Java server
emulates a KI10 panel.
The ``pdp10-kx_realcons`` simulators will work with either one.

The Java panel server and PiDP binaries can be built on Linux by
running ``make.sh`` in the ``BlinkenBone`` subdirectory.
Unlike Joerge Hoppe's original BlinkenBone distribution,
this builds only for the current machine (not cross-compilation).
The PiDP-specific binaries are only built on a Raspberry Pi,
but the clients,
Blinkenlight test program,
and Java panel servers will be built on any Linux machine.

The PiDP panelserver has been rewritten to use the Raspberry Pi
``pinctrl/gpiolib``
(which provides functions that wrap the underlying mmap access to ``/dev/gpiomem``).
It works on both 32-bit and 64-bit systems.

There are three REALCONS panel servers:
- ``server11`` (PiDP11, implements the "11/70" panel)
- ``server10`` (PiDP10, implements the "PDP10-KA10" panel)
- ``server8`` (PiDP8, untested and therefore unlikely to work correctly)

Panel servers for other machines (e.g., PDP-1) are yet to be written.

##### Clone repository

To include the ``BlinkenBone`` submodule, clone this repository with
```bash
git clone https://github.com/jdbruner/simh --recursive
```

You may wish to enable submodule recursion when doing pulls with
```bash
git config set submodule.recurse true
```

##### Build PiDP on Raspberry Pi

This has diverged significantly from
Oscar Vermeulen's [PiDP11 installation and runtime](https://github.com/obsolescence/pidp11)
and
[PiDP10 distribution](https://github.com/obsolescence/pidp10).
It does not include the
[PDP-11 disk images](http://pidp.net/pidp11/systems.tar.gz)
or PDP-10 disk images.

To build on a Raspberry Pi, change directory to the root of the
GIT repository and do the following:
```bash
cd BlinkenBone
./make.sh [-x10]
```
Specify ``-x10`` to build PIDP10;
the default is ``-x11``, which builds PiDP11.
On some systems you may need to configure the kernel variable
``dev.tty.legacy_tiocsti`` to 1
to allow the console to inject characters into the simh console input.

##### Build PiDP on Linux

Building on Linux is similar to Raspberry Pi,
except that the build will skip binaries that are Pi-specific
(e.g., the PiDP panel servers and PDP-10 PIPANEL simulators).

##### Build on Windows

simh itself has good support for building and running on Windows.
There is no supported version of Windows on the Raspberry Pi,
so the PiDP panel simulators are not applicable.
However, with some work it is possible to use simh with the Java panel servers.

Java is "write once, run anywhere",
so you can build ``BlinkenBone/javapanelsim/panelsim_all.jar``
on Linux and run it on Windows with an equivalent JDK.
To build it on Windows, install JDK 19 or newer and also install Apache Ant;
then cd to the ``BlinkenBone`` directory and run ``make.bat``.

The ``Visual Studio Projects`` folder contains project files to build with REALCONS:
- ``PDP11_realcons.vcproj``
- ``PDP10-KA_realcons.vcproj``
- ``PDP10-KI_realcons.vcproj``
- ``PDP10-KL_realcons.vcproj``
- ``PDP10-KS_realcons.vcproj``
but they have only been used with Visual Studio 2026
(which will convert them to ``vcxproj`` files when they are first used).
The built executables can be found in ``BIN\NT``.
If you need to rebuild these using an older IDE
(particularly if you want to build for Windows XP using Visual Studio 2008),
then you will probably also need to rebuild ``BlinkenBone/3rdparty/oncrpc_win32``.
Warning: there be dragons here...

There is no support for building REALCONS-enabled simulators with cmake or with mingw64.

##### Installation

Installation only runs on the build machine at this time.
From the root of the simh repository, do the following:
```bash
cd BlinkenBone
./install.sh [-x10]
```
Specify ``-x10`` to install PiDP10.
The default is ``-x11``, which installs PiDP11:
1. ``sysctl dev.tty.legacy_tiocsti=1``
2. Create the user ``pidp11``
3. Populate the ``pidp11`` bin directory with the script ``pdp.sh``
   (which connects to the ``screen`` session) and add it to the
   ``.profile``
4. Create ``/opt/pidp11/bin`` and populate it with ``pdp11_realcons``, ``server11``,
   ``getcsw``, and ``pidp.sh``
5. Create an initial version of ``/opt/pidp11/systems`` containing ``idled``.
6. Install the systemd service files and start the services

(The installation on PiDP10 is similar.)

If you want the simulated system to share the Ethernet on your device,
specify the ``-e`` option to ``install.sh``.
This will set the appropriate capabilities on the installed executables.

If the address select and data select LEDs rotate in the opposite
direction of the corresponding knobs,
then ``server11`` should be invoked with the ``-r`` flag.
Change the ``ExecStart`` line in ``/usr/local/lib/systemd/system/pidp11panel.server`` to:
```bash
ExecStart=/opt/pidp11/server11 -r
```
Alternatively, re-install and specify the ``-k`` flag to ``install.sh``.

##### Major Changes Relative to the Upstream

###### pdp11_realcons, pdp10-k*x*_realcons, pdp10-k*x*_pipanel (simh)

Relative to [simh](https://github.com/simh/simh),
the ``makefile`` and sources have been
updated with additional build targets for
REALCONS-enabled variants of the PDP8, PDP10, PDP11, and PDP15;
and PIPANEL-enabled variants of the PDP10.

The ``makefile`` will also build all of the simulators from the upstream (unmodified).
On a Raspberry Pi, ``make.sh`` command in the ``BlinkenBone`` directory
will build ``pdp11_realcons``,
and ``make.sh -x10`` will build the PDP-10 REALCONS and PIPANEL targets.

##### server8, server10, server11

The REALCONS panel servers for PiDP8 (untested), PiDP10, and PiDP11
have been refactored to share as much source code as possible.
They use the ``pinctrl/gpiolib`` library to access
the GPIO device registers,
and thus they should be run as a user that belongs to the ``gpio`` group.
It is not necessary for them to run as root;
however, it is preferable that they be able to
run the GPIO service thread at a real-time priority. 

##### getcsw, scansw10, scansw00

The new utility ``getcsw`` ("get console switches") replaces ``scansw``
for the PiDP11 and for the PiDP10 with the REALCONS panel server.
It reads the switch register from the panel server via the Blinkenlight
API (rather than directly from the PiDP GPIOs.
If the PiDP10 is using the PIPANEL interface instead of REALCONS,
then ``scansw10`` is used instead -
it directly uses the GPIOs to read the switches.
``scansw00`` doesn't access real hardware at all;
instead, it simply returns a value specifed by a command-line argument.
All three share command-line options that control how the value is formatted.

##### systemd services

On the PiDP11 there are two systemd services:
- *pidp11panel.service* - runs the panel server (server11) under
  the non-privileged account ``pidp11`` with group ``gpio`` and
  with the ability to set a real-time thread priority.
  Unlike Oscar's version, the panel server runs continuously
  (even when the client is restarted).
- *pidp11.service* - runs ``/opt/pidp11/bin/pidp.sh`` with a new ``screen``
  under the non-privileged account ``pidp11``.

On the PiDP10 there are two alternatives:
1. Use the PIPANEL interface (simh uses ``gpiolib`` to access the PiDP hardware directly).
   There is a single systemd service:
    - *pidp10-pipanel.service* - runs ``/opt/pidp10/bin/pidp.sh`` that uses ``scansw10``
      and runs ``pdp10-kX_pipanel`` in a new ``screen`` under the non-privileged account ``pidp10``.
2. Use the REALCONS interface.
   There are two systemd services:
    - *pidp10panel.service* - runs the REALCONS panel server (server10) under
      the non-privileged account ``pidp10`` with group ``gpio`` and
      with the ability to set a real-time thread priority.
      The panel server runs continuously.
    - *pidp10-realcons.service* - runs ``/opt/pidp10/bin/pidp.sh`` that uses ``getcsw``
      and runs ``pdp10-kX_realcons`` in a new ``screen`` under the non-privileged account ``pidp10``.

##### /opt/pidp{10,11}/bin/pidp.sh

This script is the main client loop.
It reads the switch register (with ``getcsw`` or ``scansw10``)
and selects the system to simulate.
Oscar's version creates a temporary command script file
to launch the simulation and also to control what happens
when the address select knob is pressed.
This version does not use an intermediate file;
instead, simh directly uses the boot script in the selected system directory.

On the PiDP11, when the address select knob is pressed, simh exits.
The simh exit status and behavior of ``pidp.sh`` is:
- 0 if the RUN/HALT switch is in the RUN position: the process repeats
  (reading the switch register, selecting the system, starting simh)
- 1 if the RUN/HALT switch is in the HALT position: the script exits
  (which causes the PiDP systemd service to terminate)

On the PiDP10, pressing the POWER key on the Java panel causes simh to exit.
At present there is no way to trigger this using the PiDP10 panel.

On both the PiDP10 and PiDP11,
the user can cause SIMH to exit with a zero or non-zero status
with the ``exit`` and ``exit 1`` commands, respectively.
``exit`` will cause the ``pidp.sh`` to repeat the process,
and ``exit 1`` will cause it to exit.

When there is a REALCONS panel (either PiDP or Java),
``pidp.sh`` sets the environment variable
``REALCONS_PANEL`` to the name of the panel before starting simh.
The boot script may choose to modify its behavior based upon this information.

##### /opt/pidp{10,11}/systems

This directory contains all of the emulated operating systems,
each in its own subdirectory.
Each subdirectory has one or more disk image files
and the following additional files:
- ``pidp_info``: This file specifies PiDP-specific metadata for the O/S:
  - console switch value associated with this O/S (e.g., ``0102`` for 2.11BSD)
  - descriptive name of the O/S (e.g., ``2.11 BSD``)  
    [if absent, the name of the directory is used]
  - CPU used to run the O/S (on PDP-10, e.g., ``pdp10-kl``)  
    [the default on the PiDP10 is ``pdp10-ka``]
- ``boot.ini``: Boot script that configures and boots the O/S with REALCONS
- ``boot.pidp`` (PiDP-10 only): Boot script that configures and boots the O/S with PIPANEL

### See Also
- [simh README](../README.md)
- [BlinkenBone submodule README](../BlinkenBone/README.md)
- [The Computer History Simulation Project (GitHub)](https://github.com/simh/simh)
- [Oscar Vermeulen's PiDP11 distribution (GitHub)](https://github.com/obsolescence/pidp11)
- [Terri Kennedy's PiDP11 distribution (GitHub)](https://github.com/Terri-Kennedy/pidp11)
- [Oscar Vermeulen's PiDP10 distribution (GitHub)](https://github.com/obsolescence/pidp10)
- [Richard Cornwell's PDP10 simulators (GItHub)](https://github.com/rcornwell/sims)
- [Obsolescence website](https://obsolescence.dev)