include $(TOPDIR)/rules.mk

PKG_NAME:=udp2can
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/udp2can-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/udp2can
  SECTION:=examples
  CATEGORY:=Examples
  TITLE:=udp2can driving example
endef

define Package/udp2can/description
  GPIO input/output driving example
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp ./src/* $(PKG_BUILD_DIR)/
	$(Build/Patch)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/udp2can/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/udp2can $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/udp2can.conf $(1)/etc/udp2can.json
endef

$(eval $(call BuildPackage,udp2can))
