

# call with MAKE_TARGET_ARCH=X86 or =BEAGLEBONE


#
# CC Command
#

# own settings
OS_CCDEFS =
ifeq ($(MAKE_CONFIGURATION),RELEASE)
CC_DBG_FLAGS = -O3
else
CC_DBG_FLAGS = -ggdb3 -O0
endif
LDFLAGS = -pthread

	# all other cases: X86
	OBJDIR=bin-x86

ifeq ($(MAKE_TARGET_ARCH),BBW)
	#### !!! when cross compiling !!!!
	# fuer beaglebone und angstrom stand alone toolchain
	CC=$(BBW_CC) -std=c99 -U__STRICT_ANSI__ $(CC_DBG_FLAGS) $(OS_CCDEFS) -I.
	OBJDIR=bin-bbw
	SSHTARGET=root@beaglebone
	LDFLAGS += -lrt
else ifeq ($(MAKE_TARGET_ARCH),BBB)
	#### !!! when cross compiling !!!!
	# fuer beaglebone und angstrom stand alone toolchain
	CC=$(BBB_CC) -std=c99 -U__STRICT_ANSI__ $(CC_DBG_FLAGS) $(OS_CCDEFS) -I.
	OBJDIR=bin-bbb
	SSHTARGET=root@beaglebone
	LDFLAGS += -lrt
else ifeq ($(MAKE_TARGET_ARCH),RPI)
	OBJDIR=bin-rpi
	SSHTARGET=pi@raspberrypi
	LDFLAGS=-lrt -pthread
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


BLINKENLIGHT_COMMON_DIR=../common
# sub dir for the API. is same level (= sibling) as server and client
BLINKENLIGHT_API_DIR=../blinkenlight_api
# dir of standard server
BLINKENLIGHT_SERVER_DIR=../blinkenlight_server

# *_svc, *_xdr and api.h sources must be created with rpcgen
BLINKENLIGHT_API_SOURCES.c = \
	$(BLINKENLIGHT_API_DIR)/blinkenlight_panels.c	\
	$(BLINKENLIGHT_API_DIR)/blinkenlight_api_server_procs.c \
	$(BLINKENLIGHT_API_DIR)/rpcgen_linux/rpc_blinkenlight_api_svc.c \
	$(BLINKENLIGHT_API_DIR)/rpcgen_linux/rpc_blinkenlight_api_xdr.c	\
	$(BLINKENLIGHT_API_DIR)/historybuffer.c	\
	$(BLINKENLIGHT_COMMON_DIR)/bitcalc.c	\
	$(BLINKENLIGHT_COMMON_DIR)/radix.c	\
	$(BLINKENLIGHT_COMMON_DIR)/getopt2.c	\
	$(BLINKENLIGHT_COMMON_DIR)/errno2txt.c

BLINKENLIGHT_API_SOURCES.h = \
	$(BLINKENLIGHT_API_DIR)/rpcgen_linux/rpc_blinkenlight_api.h	\
	$(BLINKENLIGHT_API_DIR)/blinkenlight_api_server_procs.h \
	$(BLINKENLIGHT_API_DIR)/historybuffer.h	\
	$(BLINKENLIGHT_COMMON_DIR)/bitcalc.h	\
	$(BLINKENLIGHT_COMMON_DIR)/radix.h	\
	$(BLINKENLIGHT_COMMON_DIR)/getopt2.h	\
	$(BLINKENLIGHT_COMMON_DIR)/errno2txt.h

$(BLINKENLIGHT_API_SOURCES.c) $(BLINKENLIGHT_API_SOURCES.h):
	# rpcgen
	make --directory=$(BLINKENLIGHT_API_DIR)/rpcgen_linux sources


#########################################################

SOURCES.h = \
	main.h	\
	iopattern.h	\
	gpio.h	\
	$(BLINKENLIGHT_SERVER_DIR)/print.h	\
	$(BLINKENLIGHT_SERVER_DIR)/blinkenbus.h	\
	$(BLINKENLIGHT_API_DIR)/blinkenlight_panels.h

SOURCES.c = \
	main.c	\
	iopattern.c	\
	gpio.c	\
	$(BLINKENLIGHT_SERVER_DIR)/print.c	\
	$(BLINKENLIGHT_SERVER_DIR)/blinkenbus.c	\
	$(BLINKENLIGHT_API_SOURCES.c)


OBJECTS = $(SOURCES.c:%.c=%.o)
#
# Build everything
#
all:    pdp15_blinkenlightd

clean:
	rm -f core pdp15_blinkenlightd $(OBJECTS) $(OBJDIR)/pdp15_blinkenlightd
	make --directory=$(BLINKENLIGHT_API_DIR)/rpcgen_linux clean

install:
	echo "copy server to beaglebone ... call only for ARM server!"
ifneq ($(MAKE_TARGET_ARCH),RPI)
	echo --- set remote time
	ssh $(SSHTARGET) "date -u `date -u +%m%d%H%M%Y.%S`; echo `cat /etc/timezone` > /etc/timezone"
endif
	echo copy module ...
	scp $(OBJDIR)/pdp15_blinkenlightd $(SSHTARGET):


pdp15_blinkenlightd:	$(SOURCES.c) $(SOURCES.h)
	# printenv | sort >env.make
	${CC} $^ -o $@ $(CCDEFS) ${LDFLAGS}
	mkdir -p $(OBJDIR)
	mv $@ $(OBJDIR)
	# Verify: was it x86 or ARM?
	file $(OBJDIR)/$@

