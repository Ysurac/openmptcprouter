
include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(BOARD).o
CPPFLAGS += -I$(TOPDIR)/drivers/net/ipq40xx -I$(TOPDIR)/board/qcom/common
CFLAGS += -I$(TOPDIR)/drivers/net/ipq40xx -I$(TOPDIR)/board/qcom/common
COBJS	:= ipq40xx_cdp.o

SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

clean:
	rm -f $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak $(obj).depend

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
