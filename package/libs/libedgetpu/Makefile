#
# Copyright (C) 2010-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libedgetpu
PKG_RELEASE:=

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/google-coral/libedgetpu.git
PKG_MIRROR_HASH:=43ef4b8782272d61e04c162d1ee7d64580957342de020ee85ad636e58d5b3352
PKG_SOURCE_DATE:=2021-07-27
PKG_SOURCE_VERSION:=3164995622300286ef2bb14d7fdc2792dae045b7

include $(INCLUDE_DIR)/package.mk

define Package/libedgetpu
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=The Edge TPU Runtime (libedgetpu)
  DEPENDS:=+librt +libpthread +tensorflow +libusb-1.0
endef

MAKE_VARS+= TFROOT=$(STAGING_DIR)/usr/src
MAKE_PATH:=makefile_build

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/tflite/public/edgetpu*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/out/direct/k8/libedgetpu.so* $(1)/usr/lib/
endef

define Package/libedgetpu/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/out/direct/k8/libedgetpu.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libedgetpu))
