#
# Copyright (C) 2010-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libyuv
PKG_RELEASE:=0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://chromium.googlesource.com/libyuv/libyuv.git
PKG_MIRROR_HASH:=45235ee60e8ee962023a5a085a5883f5deaf189643d1caf065b543ebd69bb708
PKG_SOURCE_DATE:=2021-07-19
PKG_SOURCE_VERSION:=639dd4ea76403fb2e69df0636ce1df6a9587b004
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libyuv
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=YUV scaling and conversion library
  DEPENDS:=+libstdcpp +libjpeg
  ABI_VERSION:=0
endef

define Package/libyuv/description
  libyuv is an open source project that includes YUV scaling and conversion functionality.
  Scale YUV to prepare content for compression, with point, bilinear or box filter.
  Convert to YUV from webcam formats for compression.
  Convert to RGB formats for rendering/effects.
  Rotate by 90/180/270 degrees to adjust for mobile devices in portrait mode.
  Optimized for SSSE3/AVX2 on x86/x64.
  Optimized for Neon on Arm.
  Optimized for MSA on Mips.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libyuv $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libyuv.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libyuv.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/yuvconvert $(1)/usr/bin/
endef

define Package/libyuv/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libyuv.so $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/yuvconvert $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libyuv))
