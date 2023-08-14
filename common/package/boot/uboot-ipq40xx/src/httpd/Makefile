#
# Makefile for http stuff
#

include $(TOPDIR)/config.mk

# Suppress warnings when building with size optimization
CFLAGS += -fno-strict-aliasing

LIB   = libhttpd.o
OBJS += uip.o \
	uip_arch.o \
	uip_arp.o \
	httpd.o \
	fs.o

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) crv $@ $(OBJS)

#########################################################################

.depend: Makefile fsdata.c $(OBJS:.o=.c)
	$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
