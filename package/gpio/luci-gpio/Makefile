#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-gpio
PKG_VERSION:=1

include $(INCLUDE_DIR)/package.mk

define Package/luci-gpio
  SECTION:=examples
  CATEGORY:=Examples
  TITLE:=LuCi web-interface for gpio configuration.
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/luci-gpio/description
 Default configuration applied on first boot and reset.
endef

define Build/Compile
	echo "Nothing to compile";
endef

define Package/luci-gpio/install
#	[ -e "./files" ] && $(CP) ./files/* $(1)/
	[ -e "./luci" ] && (mkdir -p $(1)/usr/lib/lua/luci/ ;$(CP) ./luci/* $(1)/usr/lib/lua/luci/ )
	[ -e "./styles" ] && (mkdir -p $(1)/www/luci-static/resources/ ;$(CP) ./styles/* $(1)/www/luci-static/resources/ )
endef

$(eval $(call BuildPackage,luci-gpio))

