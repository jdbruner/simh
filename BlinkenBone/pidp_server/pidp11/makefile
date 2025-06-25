# call with MAKE_TARGET_ARCH=X86 or =RPI

#
# CC Command
#

# own settings
OS_CCDEFS =
CC_DBG_FLAGS = -ggdb3 -O0
LDFLAGS = -pthread

ifeq ($(MAKE_TARGET_ARCH),X86)
	OBJDIR=bin-x86
else ifeq ($(MAKE_TARGET_ARCH),BEAGLEBONE)
	#### !!! when cross compiling !!!!
	# fuer beaglebone und angstrom stand alone toolchain
	CC=arm-angstrom-linux-gnueabi-gcc -mthumb -march=armv7-a -std=c99 -U__STRICT_ANSI__ $(CC_DBG_FLAGS) $(OS_CCDEFS) -I.
	OBJDIR=bin-beaglebone
	SSHTARGET=root@beaglebone
	LDFLAGS += -lrt
else
	OS_CCDEFS=-I/usr/include/tirpc
	OBJDIR=../../../BIN
	SSHTARGET=pi@raspberrypi
	LDFLAGS=-lrt -pthread -lgpiod -ltirpc
endif

# Reference also the standard blinkenbone server in BLINKENLIGHT_SERVER_DIR

CCDEFS= \
	-DBLINKENLIGHT_SERVER	\
	-I.	\
	-I$(BLINKENLIGHT_COMMON_DIR)	\
	-I$(BLINKENLIGHT_API_DIR)	\
	-I$(BLINKENLIGHT_API_DIR)/rpcgen_linux	\
	-I$(BLINKENLIGHT_SERVER_DIR)	\
	$(CC_DBG_FLAGS) $(OS_CCDEFS)


BLINKENLIGHT_COMMON_DIR=../../common
# sub dir for the API. is same level (= sibling) as server and client
BLINKENLIGHT_API_DIR=../../blinkenlight_api
# dir of standard server
BLINKENLIGHT_SERVER_DIR=../../blinkenlight_server
# *_svc, *_xdr and api.h sources must be created with rpcgen
BLINKENLIGHT_API_SOURCES.c = \
	$(BLINKENLIGHT_API_DIR)/blinkenlight_panels.c	\
	$(BLINKENLIGHT_API_DIR)/blinkenlight_api_server_procs.c \
	$(BLINKENLIGHT_API_DIR)/rpcgen_linux/rpc_blinkenlight_api_svc.c \
	$(BLINKENLIGHT_API_DIR)/rpcgen_linux/rpc_blinkenlight_api_xdr.c	\
	$(BLINKENLIGHT_API_DIR)/historybuffer.c	\
	$(BLINKENLIGHT_COMMON_DIR)/bitcalc.c	\
	$(BLINKENLIGHT_COMMON_DIR)/radix.c	\
	$(BLINKENLIGHT_COMMON_DIR)/mcout.c	\
	$(BLINKENLIGHT_COMMON_DIR)/errno2txt.c

BLINKENLIGHT_API_SOURCES.h = \
	$(BLINKENLIGHT_API_DIR)/rpcgen_linux/rpc_blinkenlight_api.h	\
	$(BLINKENLIGHT_API_DIR)/blinkenlight_api_server_procs.h \
	$(BLINKENLIGHT_API_DIR)/historybuffer.h	\
	$(BLINKENLIGHT_COMMON_DIR)/bitcalc.h	\
	$(BLINKENLIGHT_COMMON_DIR)/radix.h	\
	$(BLINKENLIGHT_COMMON_DIR)/mcout.h	\
	$(BLINKENLIGHT_COMMON_DIR)/errno2txt.h

all:    $(OBJDIR)/server11 $(OBJDIR)/scansw

$(BLINKENLIGHT_API_SOURCES.c) $(BLINKENLIGHT_API_SOURCES.h):
	# rpcgen
	make --directory=$(BLINKENLIGHT_API_DIR)/rpcgen_linux sources


#########################################################

SOURCES.h = \
	main.h	\
	gpiopattern.h	\
	$(BLINKENLIGHT_SERVER_DIR)/print.h	\
	$(BLINKENLIGHT_API_DIR)/blinkenlight_panels.h

SOURCES.c = \
	main.c	\
	gpio.c	\
	gpiopattern.c	\
	$(BLINKENLIGHT_SERVER_DIR)/print.c	\
	$(BLINKENLIGHT_API_SOURCES.c)


OBJECTS = $(SOURCES.c:%.c=%.o)
#
# Build everything (target 'all' is above)
#
clean:
	echo "CLEAN"
	rm -f core $(OBJECTS) $(OBJDIR)/server11 $(OBJDIR)/scansw
	make --directory=$(BLINKENLIGHT_API_DIR)/rpcgen_linux clean

install:
ifeq ($(MAKE_TARGET_ARCH),RPI)
	# no-op for now
	# eventually this should copy the binaries and set the capabilities for the panel server
else
	echo "copy server to beaglebone/rpi ... call only for ARM server!"
	echo --- set remote time
	ssh $(SSHTARGET) "date -u `date -u +%m%d%H%M%Y.%S`; echo `cat /etc/timezone` > /etc/timezone"
endif


$(OBJDIR)/server11:	$(SOURCES.c) $(SOURCES.h)
	${CC} -o $@ $(CCDEFS) $^ ${LDFLAGS}
	file $@

$(OBJDIR)/scansw: scansw.c
	$(CC) -o $@ $^ -lgpiod
	file $@
