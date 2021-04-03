

include $(TOPDIR)/rules.mk

PKG_NAME:=qmi-usbreset
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/qmi-usbreset
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=WWAN
  DEPENDS:=+libubox +libblobmsg-json +kmod-usb-net +kmod-usb-net-qmi-wwan +wwan +uqmi +usbreset
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
  TITLE:=Reset all usb on boot for qmi to work properly.
endef

define Package/qmi-usbreset/description
  Reset usb on boot process to be sure that qmi device gets up properly.
endef

define Build/Compile
	echo "Nothing to compile."
endef

define Package/qmi-usbreset/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,qmi-usbreset))
