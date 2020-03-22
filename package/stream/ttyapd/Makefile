#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=stream-ttyapd
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/stream-ttyapd
  SECTION:=Stream IMX
  CATEGORY:=Stream IMX
  DEPENDS:=+libjson-c +iwinfo +uci +libpthread
  TITLE:=TTY interaction with AP interface daemon.
  MAINTAINER:=Nick Leiten <nickleiten@gmail.com>
endef

TARGET_CPPFLAGS :=     \
	-D_GNU_SOURCE      \
	-I$(PKG_BUILD_DIR) \

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/*.c $(PKG_BUILD_DIR)/
	$(CP) ./src/*.h $(PKG_BUILD_DIR)/
	$(CP) ./src/Makefile $(PKG_BUILD_DIR)/
endef

define Build/Compile
	CFLAGS="$(TARGET_CFLAGS)" $(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
endef

define Package/stream-ttyapd/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ttyapd $(1)/sbin/ttyapd
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,stream-ttyapd))
