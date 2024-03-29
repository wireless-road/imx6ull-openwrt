#
# Copyright (C) 2009-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gst1-plugins-good-alexa
PKG_VERSION:=1.16.2
PKG_RELEASE:=1

PKG_SOURCE:=gst-plugins-good-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://gstreamer.freedesktop.org/src/gst-plugins-good/
PKG_HASH:=40bb3bafda25c0b739c8fc36e48380fccf61c4d3f83747e97ac3f9b0171b1319
PKG_BUILD_DIR:=$(BUILD_DIR)/gst-plugins-good-$(PKG_VERSION)

PKG_MAINTAINER:=W. Michael Petullo <mike@flyn.org> \
		Ted Hess <thess@kitschensync.net>
PKG_LICENSE:=LGPL-2.1-or-later
PKG_LICENSE_FILES:=COPYING
PKG_CPE_ID:=cpe:/a:gstreamer:gst-plugins-good-alexa

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk
#include ../../devel/meson/meson.mk
include ../../feeds/packages/meson/meson.mk

PKG_CONFIG_DEPENDS:= \
	CONFIG_PACKAGE_gst1-alexa-mod-lame \
	CONFIG_PACKAGE_gst1-alexa-mod-mpg123

GST_GOOD_MODULES:=

define Package/gstreamer1-good-alexa/Default
  CATEGORY:=Multimedia
  SECTION:=multimedia
  TITLE:=GStreamer (Alexa)
  URL:=https://gstreamer.freedesktop.org/
  DEPENDS:= +libgstreamer1 +libgst1pbutils-alexa $(ICONV_DEPENDS)
endef

define Package/gstreamer1-good-alexa/description/Default
 GStreamer open source multimedia framework
endef


define Package/gst1-plugins-good-alexa
$(call Package/gstreamer1-good-alexa/Default)
  TITLE+= plugins collection (good) (Alexa)
  DEPENDS+= $(GST_DEPENDS)
  HIDDEN:=1
endef

define Package/gst1-plugins-good-alexa/description
$(call Package/gstreamer1-good-alexa/description/Default)
 .
 This meta package contains only dependencies to the other plugins from
 the good plugins collection.
endef


define Package/gstreamer1-plugins-good-alexa
  $(call Package/gstreamer1-good-alexa/Default)
  TITLE+= plugins collection (good) (Alexa)
  DEPENDS:=+ALL:gst1-plugins-good-alexa +gstreamer1-plugins-base-alexa
endef

define Package/gstreamer1-plugins-good-alexa/config
    menu "Select GStreamer good modules (Alexa)"
      depends on PACKAGE_gstreamer1-plugins-good-alexa


    config gst1-plugins-good-alexa-all
      bool "Include all GStreamer good plugins (Alexa)"
      select PACKAGE_gst1-plugins-good-alexa

    comment "Modules"

   $(foreach mod,$(GST_GOOD_MODULES), \
    config PACKAGE_gst1-alexa-mod-$(mod)
      prompt "GStreamer $(mod) module (Alexa)"

   )

  endmenu

endef

GST_COND_SELECT = -D$(1)=$(if $(CONFIG_PACKAGE_gst1-alexa-mod-$(1)),en,dis)abled

GST_VERSION:=1.0

MESON_ARGS += \
	$(call GST_COND_SELECT,alaw) \
	$(call GST_COND_SELECT,alpha) \
	$(call GST_COND_SELECT,apetag) \
	$(call GST_COND_SELECT,audiofx) \
	$(call GST_COND_SELECT,audioparsers) \
	$(call GST_COND_SELECT,auparse) \
	$(call GST_COND_SELECT,autodetect) \
	$(call GST_COND_SELECT,avi) \
	$(call GST_COND_SELECT,cutter) \
	-Ddebugutils=$(if $(CONFIG_PACKAGE_gst1-mod-debug),en,dis)abled \
	$(call GST_COND_SELECT,deinterlace) \
	$(call GST_COND_SELECT,dtmf) \
	$(call GST_COND_SELECT,effectv) \
	$(call GST_COND_SELECT,equalizer) \
	$(call GST_COND_SELECT,flv) \
	-Dflx=$(if $(CONFIG_PACKAGE_gst1-mod-flxdec),en,dis)abled \
	$(call GST_COND_SELECT,goom) \
	$(call GST_COND_SELECT,goom2k1) \
	$(call GST_COND_SELECT,icydemux) \
	$(call GST_COND_SELECT,id3demux) \
	$(call GST_COND_SELECT,imagefreeze) \
	$(call GST_COND_SELECT,interleave) \
	$(call GST_COND_SELECT,isomp4) \
	-Dlaw=$(if $(CONFIG_PACKAGE_gst1-mod-mulaw),en,dis)abled \
	$(call GST_COND_SELECT,level) \
	$(call GST_COND_SELECT,matroska) \
	$(call GST_COND_SELECT,monoscope) \
	$(call GST_COND_SELECT,multifile) \
	$(call GST_COND_SELECT,multipart) \
	$(call GST_COND_SELECT,replaygain) \
	$(call GST_COND_SELECT,rtp) \
	$(call GST_COND_SELECT,rtpmanager) \
	$(call GST_COND_SELECT,rtsp) \
	$(call GST_COND_SELECT,shapewipe) \
	$(call GST_COND_SELECT,smpte) \
	$(call GST_COND_SELECT,spectrum) \
	$(call GST_COND_SELECT,udp) \
	$(call GST_COND_SELECT,videobox) \
	$(call GST_COND_SELECT,videofilter) \
	$(call GST_COND_SELECT,videomixer) \
	$(call GST_COND_SELECT,wavenc) \
	$(call GST_COND_SELECT,wavparse) \
	$(call GST_COND_SELECT,y4m) \
	\
	-Daalib=disabled \
	-Dbz2=disabled \
	-Dcairo=disabled \
	-Ddirectsound=disabled \
        -Ddv=disabled \
        -Ddv1394=disabled \
        $(call GST_COND_SELECT,flac) \
	-Dgdk-pixbuf=disabled \
	-Dgtk3=disabled \
	-Djack=disabled \
        $(call GST_COND_SELECT,jpeg) \
        $(call GST_COND_SELECT,lame) \
        -Dlibcaca=disabled \
        $(call GST_COND_SELECT,mpg123) \
	-Doss=$(if $(CONFIG_PACKAGE_gst1-mod-ossaudio),en,dis)abled \
        $(call GST_COND_SELECT,oss4) \
	-Dosxaudio=disabled \
	-Dosxvideo=disabled \
        $(call GST_COND_SELECT,png) \
        -Dpulse=disabled \
	-Dqt5=disabled \
        $(call GST_COND_SELECT,shout2) \
        $(call GST_COND_SELECT,soup) \
        $(call GST_COND_SELECT,speex) \
        -Dtaglib=disabled \
	-Dtwolame=disabled \
        $(call GST_COND_SELECT,vpx) \
	-Dwaveform=disabled \
        -Dwavpack=disabled \
	\
	-Dximagesrc=disabled \
	-Dv4l2=$(if $(CONFIG_PACKAGE_gst1-mod-video4linux2),en,dis)abled \
	-Dexamples=disabled \
	-Dtests=disabled \
	-Dnls=enabled \
	-Dorc=disabled \
	-Dgobject-cast-checks=disabled \
	-Dglib-asserts=disabled \
	-Dglib-checks=disabled

define Package/gst1-plugins-good-alexa/install
	/bin/true
endef


# 1: short name
# 2: description
# 3: dependencies on other gstreamer libraries (short name)
# 4: dependencies on other gstreamer plugins (short name)
# 5: dependencies on other packages
define GstBuildPlugin

  GST_DEPENDS += +gst1-alexa-mod-$(1)

  GST_GOOD_MODULES+= $(1)

  define Package/gst1-alexa-mod-$(1)
    $(call Package/gstreamer1-good-alexa/Default)
    TITLE+= $(2) plugin (good)
    DEPENDS+= $$(foreach p,$(3),+libgst1$$(p)-alexa) $$(foreach p,$(4),+gst1-alexa-mod-$$(p)) $(5)
    HIDDEN:=1
  endef

  define Package/gst1-alexa-mod-$(1)/description
   $(call Package/gstreamer1-good-alexa/description/Default)
   .
   This package contains the GStreamer $(2) plugin. (Alexa)
  endef

  define Package/gst1-alexa-mod-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/gstreamer-$(GST_VERSION)
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/gstreamer-$(GST_VERSION)/libgst$(1).so* \
		$$(1)/usr/lib/gstreamer-$(GST_VERSION)/ \
	)
  endef

  $$(eval $$(call BuildPackage,gst1-alexa-mod-$(1)))
endef

#$(eval $(call GstBuildPlugin,1394,IEEE 1394 support,,,))
$(eval $(call GstBuildPlugin,alaw,alaw codec,audio,,))
$(eval $(call GstBuildPlugin,alpha,alpha support,video,,))
$(eval $(call GstBuildPlugin,alphacolor,alphacolor support,video,,))
$(eval $(call GstBuildPlugin,apetag,apetag support,audio pbutils tag video,,))
$(eval $(call GstBuildPlugin,audiofx,audio effects,audio fft,,+libgst1controller))
$(eval $(call GstBuildPlugin,audioparsers,audioparsers,audio tag pbutils,,))
$(eval $(call GstBuildPlugin,auparse,auparse,audio tag pbutils,,))
$(eval $(call GstBuildPlugin,autodetect,format auto-detection,,,))
$(eval $(call GstBuildPlugin,avi,avi support,audio riff tag video,,))
$(eval $(call GstBuildPlugin,cutter,audio cutter,audio,,))
$(eval $(call GstBuildPlugin,debug,debugging,,,))
$(eval $(call GstBuildPlugin,deinterlace,deinterlace support,video,,))
$(eval $(call GstBuildPlugin,dtmf,dtmf support,rtp,,))
#$(eval $(call GstBuildPlugin,dv,dv support,audio tag video,,))
$(eval $(call GstBuildPlugin,effectv,effectvsupport,video,,))
$(eval $(call GstBuildPlugin,equalizer,audio equalizer,audio,,+libgst1controller))
$(eval $(call GstBuildPlugin,flac,FLAC codec,audio pbutils tag,,+libflac))
$(eval $(call GstBuildPlugin,flv,flv codec,audio pbutils tag video,,))
$(eval $(call GstBuildPlugin,flxdec,flx codec,,,))
#$(eval $(call GstBuildPlugin,gdkpixbuf,gdkpixbuf support,video,,))
$(eval $(call GstBuildPlugin,goom2k1,goom support,audio video pbutils,,))
$(eval $(call GstBuildPlugin,goom,goom support,audio video pbutils,,))
$(eval $(call GstBuildPlugin,icydemux,icy demuxer,audio tag,,))
$(eval $(call GstBuildPlugin,id3demux,ID3v1/v2 demuxer,pbutils tag,,))
$(eval $(call GstBuildPlugin,imagefreeze,imagefreeze support,,,))
$(eval $(call GstBuildPlugin,interleave,audio interleave,audio,,))
$(eval $(call GstBuildPlugin,isomp4,isomp4 support,audio pbutils riff rtp tag video,,))
#$(eval $(call GstBuildPlugin,jack,jack support,audio,,))
$(eval $(call GstBuildPlugin,jpeg,jpeg support,video,,+libjpeg))
$(eval $(call GstBuildPlugin,lame,MP3 encoder (using LAME),audio,,+lame-lib))
$(eval $(call GstBuildPlugin,level,audio level,audio,,))
$(eval $(call GstBuildPlugin,matroska,matroska support,audio pbutils riff tag video,,))
$(eval $(call GstBuildPlugin,mpg123,MP3 decoder (using mpg123),audio tag,,+libid3tag +mpg123))
$(eval $(call GstBuildPlugin,mulaw,mulaw support,audio,,))
$(eval $(call GstBuildPlugin,multifile,multiple files access,video,,))
$(eval $(call GstBuildPlugin,multipart,multipart stream handling,,,))
$(eval $(call GstBuildPlugin,navigationtest,navigationtest support,video,,))
$(eval $(call GstBuildPlugin,oss4,OSS 4 audio support,audio tag,,))
$(eval $(call GstBuildPlugin,ossaudio,OSS audio support,audio,,))
$(eval $(call GstBuildPlugin,png,png support,video,,+libpng))
#$(eval $(call GstBuildPlugin,pulse,pulse support,audio pbutils tag video,,))
$(eval $(call GstBuildPlugin,replaygain,volume normalization,pbutils,,))
$(eval $(call GstBuildPlugin,rtpmanager,RTP manager,audio rtp tag pbutils video,,+libgst1net))
$(eval $(call GstBuildPlugin,rtp,RTP,audio rtp tag pbutils video,,))
$(eval $(call GstBuildPlugin,rtsp,RTSP,rtp rtsp sdp,,+libgst1net))
$(eval $(call GstBuildPlugin,shapewipe,shapewipe support,video,,))
$(eval $(call GstBuildPlugin,shout2,shout2 support,audio,,+libshout))
$(eval $(call GstBuildPlugin,smpte,smpte support,video,,))
$(eval $(call GstBuildPlugin,soup,soup input,audio tag,,+libsoup))
$(eval $(call GstBuildPlugin,spectrum,spectrum data output,audio fft,,))
$(eval $(call GstBuildPlugin,speex,speex support,audio tag,,+libspeex))
#$(eval $(call GstBuildPlugin,taglib,taglib support,tag,,))
$(eval $(call GstBuildPlugin,udp,UDP,,,+libgst1net))
$(eval $(call GstBuildPlugin,video4linux2,video4linux2 support,video allocators,,+libv4l))
$(eval $(call GstBuildPlugin,videobox,videobox support,video,,))
$(eval $(call GstBuildPlugin,videocrop,videocrop support,video,,))
$(eval $(call GstBuildPlugin,videofilter,videofilter support,video,,))
$(eval $(call GstBuildPlugin,videomixer,videomixer support,video,,))
$(eval $(call GstBuildPlugin,vpx,vpx support,tag video,,+libvpx))
$(eval $(call GstBuildPlugin,wavenc,Wav encoder,riff,,))
#$(eval $(call GstBuildPlugin,wavpack,Wav packer,audio riff tag,,))
$(eval $(call GstBuildPlugin,wavparse,Wav parser,audio riff tag,,))
#$(eval $(call GstBuildPlugin,ximagesrc,ximagesrc support,video,,))
#$(eval $(call GstBuildPlugin,y4m,sty4menc support,video,,))

$(eval $(call BuildPackage,gstreamer1-plugins-good-alexa))
$(eval $(call BuildPackage,gst1-plugins-good-alexa))
