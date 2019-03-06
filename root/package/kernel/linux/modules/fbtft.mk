#
# Copyright (C) 2016 OpenWrt.org
# Copyright (C) 2019 Ycarus (Yannick Chabanois) OpenMPTCProuter.com
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

VIDEO_MENU:=Video Support

#
# FB TFT Display
#

define KernelPackage/fb-tft-all
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Framebuffer support for all TFT displays
  DEPENDS:=+kmod-fb-tft
  KCONFIG:= \
	CONFIG_STAGING=y \
	CONFIG_FB_TFT_AGM1264K_FL=m \
	CONFIG_FB_TFT_BD663474=m \
	CONFIG_FB_TFT_HX8340BN=m \
	CONFIG_FB_TFT_HX8347D=m \
	CONFIG_FB_TFT_HX8353D=m \
	CONFIG_FB_TFT_HX8357D=m \
	CONFIG_FB_TFT_ILI9163=m \
	CONFIG_FB_TFT_ILI9320=m \
	CONFIG_FB_TFT_ILI9325=m \
	CONFIG_FB_TFT_ILI9340=m \
	CONFIG_FB_TFT_ILI9341=m \
	CONFIG_FB_TFT_ILI9481=m \
	CONFIG_FB_TFT_ILI9486=m \
	CONFIG_FB_TFT_PCD8544=m \
	CONFIG_FB_TFT_RA8875=m \
	CONFIG_FB_TFT_S6D02A1=m \
	CONFIG_FB_TFT_S6D1121=m \
	CONFIG_FB_TFT_SH1106=m \
	CONFIG_FB_TFT_SSD1289=m \
	CONFIG_FB_TFT_SSD1305=m \
	CONFIG_FB_TFT_SSD1306=m \
	CONFIG_FB_TFT_SSD1325=m \
	CONFIG_FB_TFT_SSD1331=m \
	CONFIG_FB_TFT_SSD1351=m \
	CONFIG_FB_TFT_ST7735R=m \
	CONFIG_FB_TFT_ST7789V=m \
	CONFIG_FB_TFT_TINYLCD=m \
	CONFIG_FB_TFT_TLS8204=m \
	CONFIG_FB_TFT_UC1611=m \
	CONFIG_FB_TFT_UC1701=m \
	CONFIG_FB_TFT_UPD161704=m \
	CONFIG_FB_TFT_WATTEROTT=m \
	CONFIG_FB_FLEX=m \
	CONFIG_FB_TFT_FBTFT_DEVICE=m
  FILES:=\
	$(LINUX_DIR)/drivers/staging/fbtft/fbtft_device.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/flexfb.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_agm1264k-fl.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_bd663474.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_hx8340bn.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_hx8347d.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_hx8353d.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_hx8357d.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ili9163.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ili9320.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ili9325.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ili9340.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ili9341.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ili9481.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ili9486.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_pcd8544.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ra8875.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_s6d02a1.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_s6d1121.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_sh1106.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ssd1289.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ssd1305.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ssd1306.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ssd1325.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ssd1331.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_ssd1351.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_st7735r.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_st7789v.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_tinylcd.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_tls8204.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_uc1611.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_uc1701.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_upd161704.ko \
	$(LINUX_DIR)/drivers/staging/fbtft/fb_watterott.ko

  AUTOLOAD:=$(call AutoLoad,06,fbtft_device)
endef

define KernelPackage/fb-tft-all/description
 Kernel support for small TFT LCD display modules
endef

$(eval $(call KernelPackage,fb-tft-all))
