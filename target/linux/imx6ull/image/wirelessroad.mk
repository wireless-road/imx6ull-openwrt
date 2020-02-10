
ifeq ($(SUBTARGET),cortexa7)

FAT32_BLOCK_SIZE=1024

#Max flash is 8MiB
MAXFWSIZE := $(shell echo $$(( ( 8 * 1024 * 1024 ) / $(FAT32_BLOCK_SIZE) )) )

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
		-d ./6x_bootscript-$(DEVICE_NAME).txt \
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
		$(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-u-boot.imx
	rm -f $@.boot
endef

define Build/imx6ull-ubootimg
	rm -f $@
	rm -f $@.preamble
	dd if=/dev/zero of=$@.preamble bs=1024 count=1
	cat $@.preamble $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-u-boot.imx >$@
	
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


define Device/wirelessroad_gw-imx6ull
	DEVICE_TITLE := WirelessRoad GW-IMX6ULL
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := wirelessroad_gw-imx6ull
	DEVICE_DTS := wirelessroad_gw-imx6ull
	BOARDNAME := WIRELESSROAD_GW_IMX6ULL
	IMAGE_SIZE := 7000k
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory
endef
TARGET_DEVICES += wirelessroad_gw-imx6ull

define Device/wirelessroad_stream-imx6ull
	DEVICE_TITLE := WirelessRoad STREAM-IMX6ULL
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-spi-dev
	DEVICE_NAME := wirelessroad_stream-imx6ull
	DEVICE_DTS := wirelessroad_stream-imx6ull
	BOARDNAME := WIRELESSROAD_STREAM_IMX6ULL
	IMAGE_SIZE := 7000k
	CONSOLE := ttymxc0,115200
	KERNEL := kernel-bin | buildDtb | append-dtb | uImage none | imx6ull-bootscript
	IMAGES := u-boot.bin sdcard.bin mtd-sysupgrade.bin mtd-factory.bin
	IMAGE/u-boot.bin := imx6ull-ubootimg
	IMAGE/sdcard.bin := imx6ull-sdcard | append-metadata
	IMAGE/mtd-sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs
	IMAGE/mtd-factory.bin := append-kernel | append-rootfs | pad-rootfs | imx6ull-mtd-factory
endef
TARGET_DEVICES += wirelessroad_stream-imx6ull

endif