#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=imx6ull
BOARDNAME:=Freescale i.MX 6 ULL
FEATURES:=fpu gpio pcie rtc usb usbgadget squashfs targz
SUBTARGETS:=cortexa7
MAINTAINER:=

KERNEL_PATCHVER:=4.14

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=zImage

DEFAULT_PACKAGES += uboot-envtools
DEFAULT_PACKAGES += uboot-imx6ull
DEFAULT_PACKAGES += partx-utils mkf2fs e2fsprogs

$(eval $(call BuildTarget))
