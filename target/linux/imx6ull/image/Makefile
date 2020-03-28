#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

#################################################
# Devices
#################################################

DEVICE_VARS += DEVICE_DTS_DIR DEVICE_DTS IMAGE_SIZE IMAGE_SIZE_FACTORY

KERNEL_LOADADDR=0x80008000

define Build/buildDtb
	$(call Image/BuildDTB,$(DEVICE_DTS_DIR)/$(DEVICE_DTS).dts,$(KDIR)/image-$(DEVICE_DTS).dtb)
endef

define Device/Default
  PROFILES := Generic
  FILESYSTEMS := squashfs
  DEVICE_DTS_DIR := ../dts
  KERNEL_INSTALL := 1
  KERNEL_SUFFIX := -uImage
  KERNEL_NAME := zImage
  KERNEL_PREFIX := $$(IMAGE_PREFIX)
  KERNEL_DEPENDS = $$(wildcard $$(DEVICE_DTS_DIR)/$$(DTS).dts)
  KERNEL := kernel-bin | buildDtb | append-dtb | uImage none
  IMAGE_NAME = $$(IMAGE_PREFIX)-$$(1).$$(2)
  IMAGES :=
endef

include ./wirelessroad.mk

$(eval $(call BuildImage))
