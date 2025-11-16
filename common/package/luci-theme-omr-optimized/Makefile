#
# Copyright (C) 2025 OpenMPTCProuter Optimized
#
# This is free software, licensed under the GNU General Public License v3.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-theme-omr-optimized
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_LICENSE:=GPL-3.0

include $(INCLUDE_DIR)/package.mk

define Package/luci-theme-omr-optimized
	SECTION:=luci
	CATEGORY:=LuCI
	SUBMENU:=4. Themes
	TITLE:=OpenMPTCProuter Optimized - Modern Clean Theme
	DEPENDS:=+luci-base
	PKGARCH:=all
endef

define Package/luci-theme-omr-optimized/description
	Modern, clean theme for OpenMPTCProuter Optimized with simplified interface
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/luci-theme-omr-optimized/install
	$(INSTALL_DIR) $(1)/www/luci-static/omr-optimized
	$(INSTALL_DIR) $(1)/www/luci-static/omr-optimized/css
	$(INSTALL_DIR) $(1)/www/luci-static/omr-optimized/images
	
	$(INSTALL_DATA) ./htdocs/luci-static/omr-optimized/cascade.css $(1)/www/luci-static/omr-optimized/cascade.css
	$(INSTALL_DATA) ./htdocs/luci-static/omr-optimized/css/* $(1)/www/luci-static/omr-optimized/css/
	
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/themes/omr-optimized
	$(INSTALL_DATA) ./luasrc/view/themes/omr-optimized/* $(1)/usr/lib/lua/luci/view/themes/omr-optimized/
	
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./uci-defaults/30_luci-theme-omr-optimized $(1)/etc/uci-defaults/30_luci-theme-omr-optimized
endef

define Package/luci-theme-omr-optimized/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	uci set luci.main.mediaurlbase='/luci-static/omr-optimized'
	uci commit luci
}
exit 0
endef

$(eval $(call BuildPackage,luci-theme-omr-optimized))
