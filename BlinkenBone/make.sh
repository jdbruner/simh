#! /bin/bash
#
# This is a revision of the original make.sh script from the BlinkenBone project
# which only builds on the local machine (no cross-compiling) and currently
# only builds the Java panel simulation and the PiDP11 panel server and scansw
#

if [ -e /usr/bin/raspi-config ]; then
    MAKE_TARGET_NAME=RaspberryPi
    export MAKE_TARGET_ARCH=RPI
 elif [ -e /usr/bin/bbb-config ]; then
    MAKE_TARGET_NAME=BeagleBoneBlack
    export MAKE_TARGET_ARCH=BBB
 else
    case $(uname -m) in
        i386|i486|i586|i686)
            MAKE_TARGET_NAME=x86
            export MAKE_TARGET_ARCH=X86
            ;;
        x86_64)
            MAKE_TARGET_NAME=x64
            export MAKE_TARGET_ARCH=X64
            ;;
        *)
            echo Unsupported machine type: $(uname -m)
            exit 1
            ;;
    esac
 fi

# stop on error
set -e

# Debugging:
# set -x

# needed packages:

# compile all binaries for all platforms
pwd
MAKEOPTIONS=--silent
MAKETARGETS="clean all"

# optimize all compiles, see makefiles
#export MAKE_CONFIGURATION=DEBUG
export MAKE_CONFIGURATION=RELEASE

(
    # All classes and resources for all Java panels into one jar
    # sudo apt-get install ant default-jdk
    cd javapanelsim
    ant -f build.xml compile jar
)

if [ $MAKE_TARGET_ARCH = RPI ]; then
    (
        # the Blinkenligt API server for Oscar Vermeulen's PiDP11
        cd pidp_server/pidp11
        echo ; echo "*** blinkenlight_server for PiDP11"
        make $MAKEOPTIONS $MAKETARGETS
    )
    (
        # The Blinkenligt API test client for all platforms
        # (but currently only being built for Raspberry)
        cd blinkenlight_test
        echo ; echo "*** blinkenlight_test for $MAKE_TARGET_NAME"
        make $MAKEOPTIONS $MAKETARGETS
    )
    (
        # An alternative to scansw for the PiDP11 that uses
        # the pidp11 server rather than directly accessing the GPIO
        cd blinkenlight_getcsw/pidp11
        echo ; echo "*** blinkenlight_getcsw/pidp11 for $MAKE_TARGET_NAME"
        make $MAKEOPTIONS $MAKETARGETS
    )
fi

echo
echo "All OK!"
exit 0

# the following is not (yet) updated, so don't build any of it
(
    # the Blinkenlight API server for BlinkenBus is only useful on BEAGLEBONE
    # Simulation and syntax test modes work also on desktop Linuxes
    cd blinkenlight_server
    echo ; echo "*** blinkenlight_server for $MAKE_TARGET_NAME"
    make $MAKEOPTIONS $MAKETARGETS
)

if [ $MAKE_TARGET_ARCH = BBW -o $MAKE_TARGET_ARCH = BBB ]; then
    (
        # the Blinkenlight API server for BlinkenBoard PDP-15
        cd blinkenlight_server_pdp15
        echo ; echo "*** blinkenlight_server for PDP-15 on $MAKE_TARGET_NAME"
        make $MAKEOPTIONS $MAKETARGETS
    )
fi

if [ $MAKE_TARGET_ARCH = RPI ]; then
    (
        # the Blinkenligt API server for Oscar Vermeulen's PiDP8
        cd pidp_server/pidp8
        echo ; echo "*** blinkenlight_server for PiDP8"
        make $MAKEOPTIONS $MAKETARGETS
    )
fi
