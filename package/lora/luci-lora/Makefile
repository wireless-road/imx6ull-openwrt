#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-lora
PKG_VERSION:=1

include $(INCLUDE_DIR)/package.mk

define Package/luci-lora
  SECTION:=LORA
  CATEGORY:=LoraWAN Packages
  SUBMENU:=LoRaWAN
  TITLE:=LuCi web-interface for lorawan configuration.
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
  DEPENDS:= +packet-forwarder
endef

define Package/luci-lora/description
 Default configuration applied on first boot and reset.
 This package contains deaults for main router OpenVPN.
endef

define Build/Compile
	echo "Nothing to compile";
endef

define Package/luci-lora/install
	[ -e "./files" ] && $(CP) ./files/* $(1)/
	[ -e "./luci" ] && (mkdir -p $(1)/usr/lib/lua/luci/ ;$(CP) ./luci/* $(1)/usr/lib/lua/luci/ )
endef

$(eval $(call BuildPackage,luci-lora))
