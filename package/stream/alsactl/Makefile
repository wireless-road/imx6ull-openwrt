#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=alsactl
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/alsactl
  SECTION:=Stream IMX
  CATEGORY:=Stream IMX
  DEPENDS:=+alsa-utils
  TITLE:=ALSA store and restore script with persistent configuration.
  MAINTAINER:=Nick Leiten <nickleiten@gmail.com>
endef

define Build/Prepare
endef

define Build/Compile
endef

define Package/alsactl/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,alsactl))
