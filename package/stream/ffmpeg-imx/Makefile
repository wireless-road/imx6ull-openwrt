#
# Copyright (C) 2017-2019 Ian Leonard <antonlacon@gmail.com>
# Copyright (C) 2018 Ted Hess <thess@kitschensync.net>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ffmpeg-imx
PKG_VERSION:=3.4.7
PKG_RELEASE:=3

PKG_SOURCE:=ffmpeg-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://ffmpeg.org/releases/
#PKG_HASH:=3572279cb139d9e39dcfbc23edf438ff5311ec3fc5d0dcb3558e49591e5cb83e
PKG_HASH:=efd01782fd29b62505c973f5b61a65632d7b435dc9f4a6c97e92e6afd36cf5c8
PKG_MAINTAINER:=Ted Hess <thess@kitschensync.net>, \
		Ian Leonard <antonlacon@gmail.com>

#PKG_LICENSE:=LGPL-2.1-or-later GPL-2.0-or-later LGPL-3
#PKG_LICENSE_FILES:=COPYING.GPLv2 COPYING.GPLv3 COPYING.LGPLv2.1 COPYING.LGPLv3
#PKG_CPE_ID:=cpe:/a:ffmpeg:ffmpeg

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/ffmpeg-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/ffmpeg-imx
 TITLE:=FFmpeg IMX
 URL:=https://ffmpeg.org/
 SECTION:=Stream IMX
 CATEGORY:=Stream IMX
 DEPENDS+= +libpthread +libx264 +libv4l +zlib +libbz2 +fdk-aac +alsa-lib
# PROVIDES:= libffmpeg
endef

define Package/ffmpeg-imx/description
 FFmpeg is a a software package that can record, convert and stream digital
 audio and video in numerous formats.

 FFmpeg licensing / patent issues are complex. It is the reponsibility of the
 user to understand any requirements in this regard with its usage. See:
 https://ffmpeg.org/legal.html for further information.
endef


# Strip off FPU notation
REAL_CPU_TYPE:=$(firstword $(subst +, ,$(CONFIG_CPU_TYPE)))
# Fixup cpu types recogized by ffmpeg configure
REAL_CPU_TYPE:=$(subst octeonplus,octeon+,$(REAL_CPU_TYPE))

FFMPEG_LATEST_CONFIGURE:= \
	CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) $(FPIC)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	./configure \
	--enable-cross-compile \
	--cross-prefix="$(TARGET_CROSS)" \
	--arch="$(ARCH)" \
	$(if $(REAL_CPU_TYPE),--cpu=$(call qstrip,$(REAL_CPU_TYPE)),) \
	--target-os=linux \
	--prefix="/usr" \
	--pkg-config="pkg-config" \
	--enable-shared \
	--enable-static \
	--enable-pthreads \
	--enable-zlib \
	--disable-doc --disable-debug \
	--disable-lzma --disable-vaapi --disable-vdpau --disable-outdevs \
	--disable-filters --disable-protocols --disable-bsfs --disable-parsers \
	--disable-demuxers --disable-muxers --disable-hwaccels \
	--disable-decoders --disable-encoders \
	--enable-gpl --enable-nonfree --enable-version3 \
	--enable-libv4l2 --enable-indev=v4l2 \
	--enable-libx264 --enable-encoder=libx264 \
	--enable-libfdk-aac --enable-encoder=libfdk_aac \
	--enable-encoder=rawvideo --enable-decoder=rawvideo \
	--enable-encoder=mpeg2video --enable-decoder=mpeg2video \
	--enable-encoder=mpeg4 --enable-decoder=mpeg4 \
	--enable-encoder=libx264 \
	--enable-encoder=aac --enable-decoder=aac --enable-parser=aac \
	--enable-muxer=h264 --enable-demuxer=h264 --enable-parser=h264 \
	--enable-muxer=mpegts --enable-demuxer=mpegts --enable-parser=mpegvideo \
	--enable-muxer=ffm --enable-demuxer=ffm \
	--enable-muxer=flv --enable-demuxer=flv \
	--enable-muxer=rtsp --enable-demuxer=rtsp \
	--enable-muxer=rtp --enable-demuxer=rtp --enable-protocol=rtp \
	--enable-muxer=rtp_mpegts \
	--enable-protocol=tcp --enable-protocol=udp \
	--enable-protocol=http --enable-protocol=file --enable-protocol=ftp --enable-protocol=pipe \
	--enable-filter=scale --enable-filter=tblend --enable-filter=minterpolate
	

ifeq ($(CONFIG_SOFT_FLOAT),y)
FFMPEG_LATEST_CONFIGURE+= \
	--disable-altivec \
	--disable-vsx \
	--disable-power8 \
	--disable-armv5te \
	--disable-armv6 \
	--disable-armv6t2 \
	--disable-inline-asm \
	--disable-mipsdsp \
	--disable-mipsdspr2 \
	--disable-mipsfpu \
	--disable-msa \
	--disable-mmi \
	--disable-fast-unaligned \
	--disable-runtime-cpudetect

else ifneq ($(findstring arm,$(CONFIG_ARCH)),)
FFMPEG_LATEST_CONFIGURE+= \
	--disable-runtime-cpudetect
# XXX: GitHub issue 3320 ppc cpu with fpu but no altivec (WNDR4700)
else ifneq ($(findstring powerpc,$(CONFIG_ARCH)),)
FFMPEG_LATEST_CONFIGURE+= \
	--disable-altivec
endif

# libavcodec/mips/aacdec_mips.c build problem
# XXX: disable use of assembly on MIPS. Patches welcome.
ifneq ($(findstring mips,$(CONFIG_ARCH)),)
FFMPEG_LATEST_CONFIGURE+= \
        --disable-asm --disable-inline-asm
endif

# selectively disable optimizations according to arch/cpu type
ifneq ($(findstring arm,$(CONFIG_ARCH)),)
	FFMPEG_LATEST_CONFIGURE+= --enable-lto

	ifneq ($(findstring vfp,$(CONFIG_TARGET_OPTIMIZATION)),)
		FFMPEG_LATEST_CONFIGURE+= --enable-vfp
	else
		FFMPEG_LATEST_CONFIGURE+= --disable-vfp
	endif
	ifneq ($(findstring neon,$(CONFIG_TARGET_OPTIMIZATION)),)
		FFMPEG_LATEST_CONFIGURE+= \
			--enable-neon \
			--enable-vfp
	else
		FFMPEG_LATEST_CONFIGURE+= --disable-neon
	endif
endif

ifneq ($(CONFIG_TARGET_x86),)
  TARGET_CFLAGS+= -fomit-frame-pointer
ifeq ($(CONFIG_NASM),y)
  FFMPEG_LATEST_CONFIGURE += --enable-x86asm
else
  FFMPEG_LATEST_CONFIGURE += --disable-x86asm
endif
endif
ifeq ($(ARCH),x86_64)
	FFMPEG_LATEST_CONFIGURE+= --enable-hardcoded-tables
	FFPMEG_LATEST_CONFIGURE+= --enable-lto
else
	FFMPEG_LATEST_CONFIGURE+= --enable-small
endif


define Build/Configure
	( cd $(PKG_BUILD_DIR); $(FFMPEG_LATEST_CONFIGURE) )
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

# Only ffmpeg with libx264 is GPL (yes libpostproc); all other builds are lgpl (no libpostproc)
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
#	$(CP) $(PKG_INSTALL_DIR)/usr/include/lib{avcodec,avdevice,avfilter,avformat,avresample,avutil,swresample,swscale} $(1)/usr/include/
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{avcodec,avdevice,avfilter,avformat,avresample,avutil,swresample,swscale}.{a,so*} $(1)/usr/lib/
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/lib{avcodec,avdevice,avfilter,avformat,avresample,avutil,swresample,swscale}.pc $(1)/usr/lib/pkgconfig/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/lib{avcodec,avdevice,avfilter,avformat,avutil,swresample,swscale} $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{avcodec,avdevice,avfilter,avformat,avutil,swresample,swscale}.{a,so*} $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/lib{avcodec,avdevice,avfilter,avformat,avutil,swresample,swscale}.pc $(1)/usr/lib/pkgconfig/
ifneq ($(CONFIG_PACKAGE_libx264),)
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libpostproc $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpostproc.{a,so*} $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libpostproc.pc $(1)/usr/lib/pkgconfig/
endif
endef


# Only ffmpeg with libx264 is GPL (yes libpostproc); all other builds are lgpl (no libpostproc)
define Package/ffmpeg-imx/install
	[ -e "./files" ] && $(CP) ./files/* $(1)/
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ffserver $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ffprobe $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ffmpeg $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{avcodec,avdevice,avfilter,avformat,avutil,swresample,swscale}.so.* $(1)/usr/lib/
ifneq ($(CONFIG_PACKAGE_libx264),)
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpostproc.so.* $(1)/usr/lib/
endif
endef

$(eval $(call BuildPackage,ffmpeg-imx))

