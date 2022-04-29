include $(TOPDIR)/rules.mk

PKG_NAME:=qca-ssdk
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-12-07
PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/lklm/qca-ssdk
PKG_SOURCE_VERSION:=e7dcdd63724170465072f03ab04eab9d40b98954
PKG_MIRROR_HASH:=8f0f33fd09821a09ef27d7e145596d80d20fb8a1d0766b9f8d7b517762daed27

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-ssdk
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for QCA SSDK
  FILES:=$(PKG_BUILD_DIR)/build/bin/qca-ssdk.ko
  AUTOLOAD:=$(call AutoLoad,30,qca-ssdk)
  PROVIDES:=qca-ssdk
endef

define KernelPackage/qca-ssdk/Description
This package contains a qca-ssdk driver for QCA chipset
endef

GCC_VERSION=$(shell echo "$(CONFIG_GCC_VERSION)" | sed 's/[^0-9.]*\([0-9.]*\).*/\1/')

TOOLCHAIN_BIN_PATH=$(TOOLCHAIN_DIR)/bin

QCASSDK_CONFIG_OPTS+= \
	$(KERNEL_MAKE_FLAGS) \
	KVER=$(LINUX_VERSION) \
	SYS_PATH=$(LINUX_DIR) \
	GCC_VERSION=$(GCC_VERSION) \
	TOOLPREFIX=$(TARGET_CROSS) \
	TOOL_PATH=$(TOOLCHAIN_BIN_PATH) \
	TARGET_SUFFIX=$(CONFIG_TARGET_SUFFIX) \
	EXTRA_CFLAGS=-I$(STAGING_DIR)/usr/include

ifeq ($(BOARD),ipq50xx)
    QCASSDK_CONFIG_OPTS+= ISISC_ENABLE=enable
    QCASSDK_CONFIG_OPTS+= CHIP_TYPE=MP
else ifeq ($(BOARD),ipq60xx)
    QCASSDK_CONFIG_OPTS+= CHIP_TYPE=CPPE
    QCASSDK_CONFIG_OPTS+= PTP_FEATURE=enable
else ifeq ($(BOARD),ipq807x)
    QCASSDK_CONFIG_OPTS+= CHIP_TYPE=HPPE
    QCASSDK_CONFIG_OPTS+= PTP_FEATURE=enable
else ifeq ($(BOARD),ipq95xx)
    QCASSDK_CONFIG_OPTS+= CHIP_TYPE=APPE
else
    QCASSDK_CONFIG_OPTS+= HK_CHIP=enable
endif

ifneq (, $(findstring $(CONFIG_KERNEL_IPQ_MEM_PROFILE), 256 512))
    QCASSDK_CONFIG_OPTS+= MINI_SSDK=enable
    QCASSDK_CONFIG_OPTS+= PTP_FEATURE=disable
endif

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCASSDK_CONFIG_OPTS))
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/qca-ssdk
	$(INSTALL_DIR) $(1)/usr/include/qca-ssdk/api
	$(INSTALL_DIR) $(1)/usr/include/qca-ssdk/ref
	$(INSTALL_DIR) $(1)/usr/include/qca-ssdk/fal
	$(INSTALL_DIR) $(1)/usr/include/qca-ssdk/sal
	$(INSTALL_DIR) $(1)/usr/include/qca-ssdk/init
	$(CP) -rf $(PKG_BUILD_DIR)/include/api/sw_ioctl.h $(1)/usr/include/qca-ssdk/api
	if [ -f $(PKG_BUILD_DIR)/include/ref/ref_vsi.h ]; then \
	$(CP) -rf $(PKG_BUILD_DIR)/include/ref/ref_vsi.h $(1)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/ref/ref_fdb.h ]; then \
	$(CP) -rf $(PKG_BUILD_DIR)/include/ref/ref_fdb.h $(1)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/ref/ref_port_ctrl.h ]; then \
	$(CP) -rf $(PKG_BUILD_DIR)/include/ref/ref_port_ctrl.h $(1)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/init/ssdk_init.h ]; then \
	$(CP) -rf $(PKG_BUILD_DIR)/include/init/ssdk_init.h $(1)/usr/include/qca-ssdk/init/; \
	fi
	$(CP) -rf $(PKG_BUILD_DIR)/include/fal $(1)/usr/include/qca-ssdk
	$(CP) -rf $(PKG_BUILD_DIR)/include/common/*.h $(1)/usr/include/qca-ssdk
	$(CP) -rf $(PKG_BUILD_DIR)/include/sal/os/linux/*.h $(1)/usr/include/qca-ssdk
	$(CP) -rf $(PKG_BUILD_DIR)/include/sal/os/*.h $(1)/usr/include/qca-ssdk
endef

define KernelPackage/qca-ssdk/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qca-ssdk $(1)/etc/init.d/qca-ssdk
endef

$(eval $(call KernelPackage,qca-ssdk))
