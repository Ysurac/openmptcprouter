# Copyright (c) 2012 The Linux Foundation. All rights reserved.

include $(TOPDIR)/config.mk

LIB	=  $(obj)lib$(SOC).o

SOBJS-y = timer.o
COBJS-y += clock.o
COBJS-y += gpio.o
COBJS-y += smem.o
COBJS-y += scm.o
COBJS-y += cmd_bootipq.o
ifeq ($(CONFIG_IPQ_APPSBL_DLOAD),y)
COBJS-y += cmd_dumpipq_data.o
endif

SRCS	:= $(SOBJS-y:.o=.S) $(COBJS-y:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS-y) $(SOBJS-y))

all:	 $(obj).depend $(LIB)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

