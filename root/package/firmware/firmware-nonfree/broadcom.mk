Package/brcmfmac-firmware-43455-sdio = $(call Package/firmware-default,Broadcom BCM43455 FullMac SDIO firmware)
define Package/brcmfmac-firmware-43455-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.bin \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.clm_blob \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.txt \
		$(1)/lib/firmware/brcm/
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43455-sdio))
