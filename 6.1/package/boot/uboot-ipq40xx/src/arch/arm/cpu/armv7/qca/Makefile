
include $(TOPDIR)/config.mk

SOBJS-y = timer.o gpio.o clock.o scm.o smem.o

CPPFLAGS += -I$(TOPDIR)/board/qcom/ipq40xx_cdp -I$(TOPDIR)/board/qcom/common
CFLAGS += -I$(TOPDIR)/board/qcom/ipq40xx_cdp -I$(TOPDIR)/board/qcom/common
LIB	=  $(obj)lib$(SOC).o

COBJS-y += cmd_bootqca.o
COBJS-y += cmd_firstboot.o
COBJS-y += mnf_flash.o
COBJS-y += mnf_fields.o
ifeq ($(CONFIG_QCA_APPSBL_DLOAD),y)
COBJS-y	+= cmd_dumpqca_data.o
endif
COBJS-y += cmd_blowsecfuse.o
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
