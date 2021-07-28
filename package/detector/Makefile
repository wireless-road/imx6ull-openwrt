#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=detector
PKG_VERSION:=1
PKG_RELEASE:=
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

MAKE_FLAGS+= WITHOUT_RTSP=yes WITHOUT_ENCODER=yes WITH_JPEG=yes WITH_YUV=yes OPENWRT=yes

define Package/detector
  SECTION:=tensorflow
  CATEGORY:=Examples
  TITLE:=detector
  DEPENDS:=+libstdcpp +tensorflow +libyuv +libjpeg +librt +libpthread +libusb-1.0 +libedgetpu
endef

define Package/detector/install
	$(INSTALL_DIR) $(1)/etc/models $(1)/usr/bin $(1)/etc/models/edgetpu $(1)/etc/models/lite
	$(CP) $(PKG_BUILD_DIR)/models/* $(1)/etc/models
	$(CP) $(PKG_BUILD_DIR)/models/edgetpu/* $(1)/etc/models/edgetpu
	$(CP) $(PKG_BUILD_DIR)/models/lite/* $(1)/etc/models/lite
	$(STRIP) $(PKG_BUILD_DIR)/detector
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/detector $(1)/usr/bin/
endef

$(eval $(call BuildPackage,detector,+libstdcpp))
