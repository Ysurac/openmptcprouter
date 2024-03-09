Package/netxen-firmware = $(call Package/firmware-default,QLogic Intelligent Ethernet 3000 and 3100 Series firmware)
define Package/netxen-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/netxen
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/phanfw.bin $(1)/lib/firmware
endef
$(eval $(call BuildPackage,netxen-firmware))
