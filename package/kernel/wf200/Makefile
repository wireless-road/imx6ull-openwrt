#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wf200-firmware
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/wf200-firmware
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Silabs WF200 firmware
endef

define Package/wf200-firmware/description
 This package contains the microcode needed to use the Silabs WF200 network co-processor
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./bin/* $(PKG_BUILD_DIR)/
endef

define Build/Compile

endef

define Package/wf200-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(CP) $(PKG_BUILD_DIR)/wf* $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,wf200-firmware))
