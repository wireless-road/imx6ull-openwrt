[Back to Main Readme](../README.md)

# WirelessRoad GW-IMX6ULL

[Build image description](README.lorawan-details.md)

### Image build process.
**1.** Install all required tools. For Ubuntu:
> sudo apt-get update

> sudo apt-get install git-core build-essential libssl-dev libncurses5-dev unzip gawk zlib1g-dev subversion mercurial

**2.** Clone source code:
> git clone https://github.com/wireless-road/lorawan-imx6ull

**3.** Enter cloned folder:
> cd lorawan-imx6ull

**4.** Install all available packages:
> ./scripts/feeds update -a

> ./scripts/feeds install -a

**5.** Clone one of available configuration file:
> cp openwrt-configs/amazon_voice_service.config ./.config

**6.** _This step might be missed if nothing unusual required for you_

  To change configuration:
> make menuconfig

**7.** Dowload all required packages:
> make download

**8.** Start image compilation by:
> make

or by setting amount of parallel processes to speed up build process:
> make -j 4

**9.** Check `bin/targets/imx6ull/cortexa7/` folder after build process finished. Here you can find resulted binaries you may use to update your hardware:
`openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.sdcard.bin` - image to write on SD/MMC-card.

`openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-sysupgrade.bin` - image to update SPI flash IC. Used for remote firmware upgrade.

`openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin` - SPI flash IC image to burn flash IC using programmators.

`openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.u-boot.bin` - U-boot image.


### Firmware upgrade.

###### Copy firmware image using SCP.

For example, if your target hardware IP address is 192.168.1.100.
Type following from your host machine you used to compile firmware image:
> scp ./bin/targets/imx6ull/cortexa7/openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin root@192.168.1.100:/tmp/

###### Burn firmware to flash IC:
Then start firmware burning to flash IC from target hardware console by typing:
> mtd write openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin factory

##### Upgrade firmware using sysupgrade utility.

Deliver firmware image using SCP as described above.

To upgrade U-boot you have to download `openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.u-boot.bin` file to `/tmp` folder of target hardware and run:
> mtd write /tmp/openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.u-boot.bin u-boot

To upgrade Linux kernel image download `openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-sysupgrade.bin` to target hardware and run: 
> sysupgrade -с /tmp/openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-sysupgrade.bin

`sysupgrade` updates firmware while keeps (key '-c') current configuration (`/etc` directory, files from `/etc/sysupgrade.conf` list, `/etc/config/` files mostly).
