#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tensorflow
PKG_VERSION:=1
PKG_RELEASE:=
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)


CMAKE_BINARY_SUBDIR=tensorflow-sdk-build
SOURCE_SUBDIR=tensorflow-sdk-source
CMAKE_SOURCE_SUBDIR=tensorflow-sdk-source/tensorflow/lite/examples/minimal

#CMAKE_SYSTEM_PROCESSOR=armv7
CMAKE_OPTIONS=-DCMAKE_BUILD_TYPE=MINSIZEREL -DTFLITE_ENABLE_XNNPACK=OFF

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/tensorflow
  SECTION:=tensorflow
  CATEGORY:=Examples
  TITLE:=tensorflow
  DEPENDS:=+libstdcpp
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)
	mkdir -p $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/
	$(MAKE) --version
	cmake --version
	whereis cmake
endef

define Package/tensorflow/description
 Tensorflow-lite for embedded linux minimal working example.
endef

define Build/Compile
	$(MAKE_VARS) $(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR) $(MAKE_FLAGS)
endef

define Package/tensorflow/install
	$(INSTALL_DIR) $(1)/etc/tensorflow $(1)/usr/lib $(1)/bin
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(CP) ./files/* $(1)/
	$(STRIP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/minimal
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/minimal $(1)/bin/
endef

$(eval $(call BuildPackage,tensorflow,+libstdcpp))
