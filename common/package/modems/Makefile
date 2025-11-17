#
# Copyright (C) 2019 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modems
PKG_VERSION:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/modems
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=3G/4G/5G modem list and optimization tools
	DEPENDS:=+uqmi +umbim +comgt
endef

define Package/modems/description
	Modem configuration database and optimization scripts for 3G/4G/5G modems.
	Includes specific optimizations for Quectel RM551E-GL and other 5G modems.
endef

define Build/Compile
endef

define Package/modems/install
	$(INSTALL_DIR) $(1)/lib/network/wwan/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/data/* $(1)/lib/network/wwan/
	# Rename files with colons
	shopt -s nullglob ; \
	for filevar in $(1)/lib/network/wwan/*-* ; \
	do \
		FILENAME=$$$$(basename $$$$filevar) ; \
		NEWNAME=$$$${FILENAME//-/:} ; \
		mv "$(1)/lib/network/wwan/$$$$FILENAME" "$(1)/lib/network/wwan/$$$$NEWNAME" ; \
	done
	
	# Install modem optimization and stability scripts
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/modem-ca-optimize.sh $(1)/usr/bin/modem-ca-optimize.sh
	$(INSTALL_BIN) ./files/rm551e-init.sh $(1)/usr/bin/rm551e-init.sh
	$(INSTALL_BIN) ./files/rm551e-monitor.sh $(1)/usr/bin/rm551e-monitor.sh
	
	# Install hotplug script for automatic modem initialization
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usb
	echo '#!/bin/sh' > $(1)/etc/hotplug.d/usb/99-modem-init
	echo '# Auto-initialize Quectel RM551E modem on USB insertion' >> $(1)/etc/hotplug.d/usb/99-modem-init
	echo '[ "$$ACTION" = "add" ] && [ "$$PRODUCT" = "2c7c/800/*" -o "$$PRODUCT" = "2c7c/801/*" -o "$$PRODUCT" = "2c7c/900/*" -o "$$PRODUCT" = "2c7c/901/*" ] && /usr/bin/rm551e-init.sh &' >> $(1)/etc/hotplug.d/usb/99-modem-init
	chmod +x $(1)/etc/hotplug.d/usb/99-modem-init
	
	# Install init script to start monitor at boot
	$(INSTALL_DIR) $(1)/etc/init.d
	echo '#!/bin/sh /etc/rc.common' > $(1)/etc/init.d/rm551e-monitor
	echo '# RM551E Modem Stability Monitor' >> $(1)/etc/init.d/rm551e-monitor
	echo 'START=99' >> $(1)/etc/init.d/rm551e-monitor
	echo 'STOP=10' >> $(1)/etc/init.d/rm551e-monitor
	echo '' >> $(1)/etc/init.d/rm551e-monitor
	echo 'start() {' >> $(1)/etc/init.d/rm551e-monitor
	echo '	# Start monitor if RM551E modem is detected' >> $(1)/etc/init.d/rm551e-monitor
	echo '	if lsusb | grep -q "2c7c:08"; then' >> $(1)/etc/init.d/rm551e-monitor
	echo '		/usr/bin/rm551e-monitor.sh &' >> $(1)/etc/init.d/rm551e-monitor
	echo '	fi' >> $(1)/etc/init.d/rm551e-monitor
	echo '}' >> $(1)/etc/init.d/rm551e-monitor
	echo '' >> $(1)/etc/init.d/rm551e-monitor
	echo 'stop() {' >> $(1)/etc/init.d/rm551e-monitor
	echo '	if [ -f /var/run/rm551e-monitor.pid ]; then' >> $(1)/etc/init.d/rm551e-monitor
	echo '		kill $$(cat /var/run/rm551e-monitor.pid) 2>/dev/null' >> $(1)/etc/init.d/rm551e-monitor
	echo '		rm -f /var/run/rm551e-monitor.pid' >> $(1)/etc/init.d/rm551e-monitor
	echo '	fi' >> $(1)/etc/init.d/rm551e-monitor
	echo '}' >> $(1)/etc/init.d/rm551e-monitor
	chmod +x $(1)/etc/init.d/rm551e-monitor
endef

$(eval $(call BuildPackage,modems))
