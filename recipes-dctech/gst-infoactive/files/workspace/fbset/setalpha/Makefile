PREFIX ?= $(BUILDDIR)/tmp/sysroots/x86_64-linux/usr/bin/arm-poky-linux-gnueabi
CROSS_COMPILE ?= arm-poky-linux-gnueabi-

MACHINE ?= `grep MACHINE $(BUILDDIR)/conf/local.conf | sed "s/.*\(imx6[a-z]*\).*/\1/"`
SYSROOT ?= $(BUILDDIR)/tmp/sysroots/$(MACHINE)

CC = $(PREFIX)/$(CROSS_COMPILE)gcc -march=armv7-a -mtune=cortex-a9 -mno-thumb-interwork -mfpu=neon -mfloat-abi=hard --sysroot $(SYSROOT)
OBJECTS = setalpha.o

APP = setalpha

INC= -I$(SYSROOT)/usr/src/kernel/include/uapi -I$(SYSROOT)/usr/src/kernel/include
LDADD = 
LDINC =  

CFLAGS = -O0 -pipe -g -feliminate-unused-debug-types
#CFLAGS += -Wall `pkg-config --cflags --libs gstreamer-1.0`

all: $(OBJECTS_LDS) $(OBJECTS) $(OBJECTS_ASM) $(SUBDIRS) $(LIBS) $(SOLIBS) $(APP)

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) $(INC) $< -o $@
	$(CC) -MM $(CFLAGS) $(INC) $*.c | \
	sed 's,\($(notdir $*)\.o\) *:,$(dir $*.d)\1 $*.d: ,' > $*.d.tmp
	mv $*.d.tmp $*.d

$(APP): $(OBJECTS) $(OBJECTS_ASM) $(LIBRARIES)
	@echo "Linking application $@ for Linux"
	$(CC) \
		$(OBJECTS) \
		$(OBJECTS_ASM) \
		$(LIBRARIES) \
		$(APP_LIBS) \
		$(LDADD) $(LDINC) -o $@

.PHONY: clean
clean::
	rm -f *.o *.d *.a *.map *.bin *.elf \
	$(OBJECTS) $(OBJECTS_LDS) $(OBJECTS_ASM) $(APP)\
		$(patsubst %.o,%.d,$(OBJECTS)) \
		$(patsubst %.o,%.d,$(OBJECTS_ASM))
