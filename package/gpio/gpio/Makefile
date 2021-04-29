include $(TOPDIR)/rules.mk

PKG_NAME:=gpio
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/gpio-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/gpio
  SECTION:=examples
  CATEGORY:=Examples
  TITLE:=gpio driving example
endef

define Package/gpio/description
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

define Package/gpio/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gpio $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/gpio_conf.json $(1)/etc/gpio_conf.json
endef

$(eval $(call BuildPackage,gpio))
