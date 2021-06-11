
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

ifeq ($(SUBTARGET),cortexa7)

FAT32_BLOCK_SIZE=1024

#Max flash is 32MiB
MAXFWSIZE := $(shell echo $$(( ( 32 * 1024 * 1024 ) / $(FAT32_BLOCK_SIZE) )) )

#On FAT32 there's restriction to pad FS to this value.
# SECTOR size is 512 and we need to pad to 32 sectors.
FAT32_SECTORSTRACK_SIZE := $(shell echo $$(( 32 * 512)) )

#here we calculate BOOT partition size and squashfs partition to fill within Max flash size.
define Build/calc-sizes
	$(eval FILE_KERNEL_SIZE:= $(shell echo $$(stat --printf='%s' "$(IMAGE_KERNEL)") ))
	$(eval FILE_BOOTSCR_SIZE:= $(shell echo $$(stat --printf='%s' "$(KDIR)/6x_bootscript-$(DEVICE_NAME).scr") ))
	$(eval PART_BOOT_SIZE := $(shell echo "$$(( ( ( $(FILE_KERNEL_SIZE) + $(FILE_BOOTSCR_SIZE) \
		) / $(FAT32_SECTORSTRACK_SIZE) + 5 ) * $(FAT32_SECTORSTRACK_SIZE) / $(FAT32_BLOCK_SIZE)  ))"))

	$(eval PART_ROOTFS_SIZE := $(shell echo "$$(( $(MAXFWSIZE) - $(PART_BOOT_SIZE) ))" ))
endef

define Build/imx6ull-bootscript
	mkimage -A arm -O linux -T script -C none -a 0 -e 0 \
		-n '$(DEVICE_NAME) OpenWrt bootscript' \
		-d ./6x_bootscript-wirelessroad-imx6ull.txt \
		$(KDIR)/6x_bootscript-$(DEVICE_NAME).scr
endef

define Build/imx6ull-sdcard
	rm -f $@.boot

	$(call Build/calc-sizes)

	echo "FILE_KERNEL_SIZE: $(FILE_KERNEL_SIZE)"
	echo "FILE_BOOTSCR_SIZE: $(FILE_BOOTSCR_SIZE)"

	echo "PART_BOOT_SIZE: $(PART_BOOT_SIZE)";
	echo "PART_ROOTFS_SIZE: $(PART_ROOTFS_SIZE)";
	mkfs.fat $@.boot -C $(PART_BOOT_SIZE)

	mcopy -i $@.boot $(KDIR)/6x_bootscript-$(DEVICE_NAME).scr ::6x_bootscript
	mcopy -i $@.boot $(IMAGE_KERNEL) ::zImage
	chmod a+x ./gen_imx6ull_sdcard_img.sh
	./gen_imx6ull_sdcard_img.sh $@ \
		$@.boot \
		$(IMAGE_ROOTFS) \
		$(PART_BOOT_SIZE) \
		$(PART_ROOTFS_SIZE) \
		$(STAGING_DIR_IMAGE)/wirelessroad_ecspi3-u-boot.imx
	rm -f $@.boot
endef

define Build/imx6ull-ubootimg
	rm -f $@
	rm -f $@.preamble
	dd if=/dev/zero of=$@.preamble bs=1024 count=1
	cat $@.preamble $(STAGING_DIR_IMAGE)/wirelessroad_ecspi3-u-boot.imx >$@
	
	$(eval IMX6ULL_UBOOTIMG := $@)
endef

define Build/imx6ull-mtd-factory
	
	mv $@ $@.fw
	
	$(call Build/imx6ull-ubootimg)
	dd if=/dev/zero of=$@.new bs=1M count=1
	dd if=$@ of=$@.new conv=notrunc
	
	echo "ARGUMENT NAME: $@";
	
	cat $@.fw >>$@.new
	
	mv $@.new $@
	rm -f $@.new
endef


define Device/lorawan_gateway_ethernet
	DEVICE_TITLE := Lorawan Gateway Ethernet
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := lorawan_gateway_ethernet
	DEVICE_DTS := lorawan_gateway_ethernet
	BOARDNAME := WIRELESSROAD_GW_IMX6ULL
	SUPPORTED_DEVICES:= wirelessroad_gw-imx6ull lorawan_gateway_ethernet
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += lorawan_gateway_ethernet

define Device/lorawan_gateway_3g
	DEVICE_TITLE := Lorawan Gateway 3G
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := lorawan_gateway_3g
	DEVICE_DTS := lorawan_gateway_3g
	BOARDNAME := WIRELESSROAD_GW_IMX6ULL
	SUPPORTED_DEVICES:= wirelessroad_gw-imx6ull lorawan_gateway_3g
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += lorawan_gateway_3g

define Device/lorawan_gateway_wifi
	DEVICE_TITLE := Lorawan Gateway WiFi
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev kmod-wfx iwinfo wpad-mini
	DEVICE_NAME := lorawan_gateway_wifi
	DEVICE_DTS := lorawan_gateway_wifi
	BOARDNAME := WIRELESSROAD_GW_WIFI_IMX6ULL
	SUPPORTED_DEVICES:= lorawan_gateway_wifi
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += lorawan_gateway_wifi


define Device/video_stream_ethernet
	DEVICE_TITLE := Video Stream Ethernet
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := video_stream_ethernet
	DEVICE_DTS := video_stream_ethernet
	BOARDNAME := WIRELESSROAD_STREAM_IMX6ULL
	SUPPORTED_DEVICES := wirelessroad_stream-imx6ull video_stream_ethernet
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += video_stream_ethernet

define Device/video_stream_3g
	DEVICE_TITLE := Video Stream 3G
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := video_stream_3g
	DEVICE_DTS := video_stream_3g
	BOARDNAME := WIRELESSROAD_STREAM_3G_IMX6ULL
	SUPPORTED_DEVICES := wirelessroad_stream-imx6ull video_stream_3g
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += video_stream_3g

define Device/video_stream_wifi
	DEVICE_TITLE := Video Stream Wifi
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev kmod-wfx iwinfo wpad-mini
	DEVICE_NAME := video_stream_wifi
	DEVICE_DTS := video_stream_wifi
	BOARDNAME := WIRELESSROAD_STREAM_WIFI_IMX6ULL
	SUPPORTED_DEVICES := wirelessroad_stream_wifi-imx6ull video_stream_wifi
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += video_stream_wifi


define Device/flexcan_ethernet
        DEVICE_TITLE := FlexCAN Ethernet
        DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev kmod-can-flexcan
        DEVICE_NAME := flexcan_ethernet
        DEVICE_DTS := flexcan_ethernet
        BOARDNAME := WIRELESSROAD_FLEXCAN_ETHERNET_IMX6ULL
        SUPPORTED_DEVICES := wirelessroad_stream-imx6ull flexcan_ethernet
        IMAGE_SIZE := 31m
        IMAGE_SIZE_FACTORY := 32m
        CONSOLE := ttymxc0,115200
        KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
        IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
        IMAGE/u-boot.bin := imx6ull-ubootimg
        IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
        IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
        IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += flexcan_ethernet


define Device/flexcan_wifi
        DEVICE_TITLE := FlexCAN WiFi
        DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev kmod-can-flexcan kmod-wfx iwinfo wpad-mini
        DEVICE_NAME := flexcan_wifi
        DEVICE_DTS := flexcan_wifi
        BOARDNAME := WIRELESSROAD_FLEXCAN_WIFI_IMX6ULL
        SUPPORTED_DEVICES := wirelessroad_stream-imx6ull flexcan_wifi
        IMAGE_SIZE := 31m
        IMAGE_SIZE_FACTORY := 32m
        CONSOLE := ttymxc0,115200
        KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
        IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
        IMAGE/u-boot.bin := imx6ull-ubootimg
        IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
        IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
        IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += flexcan_wifi

define Device/audio_stream_ethernet
	DEVICE_TITLE := Audio Stream Ethernet
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := audio_stream_ethernet
	DEVICE_DTS := audio_stream_ethernet
	BOARDNAME := WIRELESSROAD_AUDIOSTREAM_IMX6ULL
	SUPPORTED_DEVICES:= audio_stream_ethernet
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += audio_stream_ethernet

define Device/audio_stream_3g
	DEVICE_TITLE := Audio Stream 3G
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := audio_stream_3g
	DEVICE_DTS := audio_stream_3g
	BOARDNAME := WIRELESSROAD_AUDIOSTREAM_3G_IMX6ULL
	SUPPORTED_DEVICES:= audio_stream_3g
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += audio_stream_3g

define Device/audio_stream_wifi
	DEVICE_TITLE := Audio Stream WiFi
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev kmod-wfx iwinfo wpad-mini
	DEVICE_NAME := audio_stream_wifi
	DEVICE_DTS := audio_stream_wifi
	BOARDNAME := WIRELESSROAD_AUDIOSTREAM_WIFI_IMX6ULL
	SUPPORTED_DEVICES:= audio_stream_wifi
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += audio_stream_wifi

define Device/amazon_voice_service_ethernet
	DEVICE_TITLE := Amazon Voice Service Ethernet
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := amazon_voice_service_ethernet
	DEVICE_DTS := amazon_voice_service_ethernet
	BOARDNAME := WIRELESSROAD_AMAZON_VOICE_SERVICE_IMX6ULL
	SUPPORTED_DEVICES:= amazon_voice_service_ethernet
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += amazon_voice_service_ethernet

define Device/amazon_voice_service_3g
	DEVICE_TITLE := Amazon Voice Service 3G
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := amazon_voice_service_3g
	DEVICE_DTS := amazon_voice_service_3g
	BOARDNAME := WIRELESSROAD_AMAZON_VOICE_SERVICE_3G_IMX6ULL
	SUPPORTED_DEVICES:= amazon_voice_service_3g
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += amazon_voice_service_3g

define Device/amazon_voice_service_wifi
	DEVICE_TITLE := Amazon Voice Service WiFi
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev kmod-wfx iwinfo wpad-mini
	DEVICE_NAME := amazon_voice_service_wifi
	DEVICE_DTS := amazon_voice_service_wifi
	BOARDNAME := WIRELESSROAD_AMAZON_VOICE_SERVICE_WIFI_IMX6ULL
	SUPPORTED_DEVICES:= amazon_voice_service_wifi
	IMAGE_SIZE := 31m
	IMAGE_SIZE_FACTORY := 32m
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory | append-metadata | check-size $$$$(IMAGE_SIZE_FACTORY)
endef
TARGET_DEVICES += amazon_voice_service_wifi


endif
