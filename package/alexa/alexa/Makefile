#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=alexa
PKG_VERSION:=1
PKG_RELEASE:=
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

PKG_BUILD_PARALLEL:=1

CMAKE_BINARY_SUBDIR=alexa-sdk-build
CMAKE_SOURCE_SUBDIR=alexa-sdk-source

CMAKE_OPTIONS=-DCMAKE_BUILD_TYPE=DEBUG -DPORTAUDIO=ON -DGSTREAMER_MEDIA_PLAYER=ON -DPORTAUDIO_LIB_PATH=$(STAGING_DIR)/usr/lib/libportaudio.so -DPORTAUDIO_INCLUDE_DIR=$(STAGING_DIR)/usr/include/
EXTRA_CFLAGS+=-Wno-error=format=

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/alexa
  SECTION:=ALEXA
  CATEGORY:=ALEXA software
  TITLE:=Alexa
  URL:=
  DEPENDS:=+libpthread +libcurl +glib2 +libnghttp2 +liboil +libgst1app-alexa +gst1-alexa-mod-playback +gstreamer1 +libstdcpp +libsqlite3 +libatomic +libopenssl +portaudio
#  DEPENDS:=+libpthread +LIBCURL_NGHTTP2:libcurl +glib2 +libnghttp2 +liboil +libgst1app-alexa +gst1-alexa-mod-playback +gstreamer1 +libstdcpp +SQLITE3_SESSION:libsqlite3 +libatomic +libopenssl +portaudio
#  DEPENDS:=+libpthread @LIBCURL_TLS_SRP @LIBCURL_NGHTTP2 +glib2 +libnghttp2 +liboil +libgst1app-alexa +gst1-alexa-mod-playback +gstreamer1 +libstdcpp @SQLITE3_SESSION +libatomic +libopenssl +portaudio
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)
	mkdir -p $(PKG_BUILD_DIR)/$(CMAKE_SOURCE_SUBDIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/$(CMAKE_SOURCE_SUBDIR)/
#	cd $(PKG_BUILD_DIR)/alexa-sdk-build/ && cmake $(PKG_BUILD_DIR)/alexa-sdk-src/
endef

define Package/alexa/description
 Alexa voice assistance for AVS.
endef

define Build/Compile
	$(MAKE_VARS) $(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR) $(MAKE_FLAGS) -j 4
	$(MAKE_VARS) $(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR) $(MAKE_FLAGS) SampleApp -j 4
endef

define Package/alexa/install
	$(INSTALL_DIR) $(1)/etc/alexa $(1)/etc/alexa/database $(1)/usr/lib $(1)/etc/ssl $(1)/etc/ssl/certs $(1)/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(CP) ./files/* $(1)/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/SampleApp/src/SampleApp $(1)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/SampleApp/src/libLibSampleApp.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/Integration/test/SpeechSynthesizerIntegrationTest $(1)/bin/
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/Integration/AlexaClientSDKConfig.json $(1)/etc/alexa
endef

#$(eval $(call BuildPackage,$(PKG_NAME)))
$(eval $(call BuildPackage,$(PKG_NAME),libpthread,libcurl,glib2,libnghttp2,liboil,libgst1app-alexa,gst1-alexa-mod-playback,gstreamer1,libsqlite3,libatomic,libopenssl,portaudio))
#$(eval $(call BuildPackage,alexa,+libpthread,+libcurl,+glib2,+libnghttp2,+liboil,+uclibcxx,+libgstapp,+libgstbase,+libgstreamer))
