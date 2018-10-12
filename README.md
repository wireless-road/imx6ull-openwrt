
# WirelessRoad GW-IMX6ULL

### Порядок сборки прошивки.
`1.` Клонируйте репозиторий исходных текстов командой:

> git clone https://github.com/wireless-road/lorawan-imx6ull

`2.` Перейдите в директорию c исходными текстами:

> cd lorawan-imx6ull

`3.` Скопируйте конфигурационный файл openwrt для устройства GW-IMX6ULL:

> cp openwrt-configs/gw-imx6ull ./.config

`4.` _Этот шаг можно пропустить, если вам не нужны дополнительные опции_

  Для тонкой настройки выполните команду:
> make menuconfig

`5.` Выберите один из вариантов, приведённых ниже, и запустите процесс сборки командой:

> make

или командой с указанием количества параллельных процессов для меньшего времени сборки, в примере указано 4 процесса:

> make -j 4

`6.` После завершения сборки в директории `bin/targets/imx6ull/cortexa7/` будут находится образы прошивки и загрузчика для записи/обновления:

`openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.sdcard.bin` - образ для записи на SD/MMC-карту.
`openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-sysupgrade.bin` - образ прошивки для использования на spi-flash, этот файл используется для удалённого обновления.
`openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin` - образ spi-flash для записи при производстве или полного обновления прошивки и загрузчика один файлом.
`openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.u-boot.bin` - образ загрузчика.

### Обновление и запись образа.

##### Первичная запись.

Для использования прошивки необходимо записать образ на внешний накопитель (SD-карта) командой
> dd if=openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.sdcard.bin of=/dev/sdX

Вместо /dev/sdX укажите путь к устройству с SD-картой.

После этого необходимо загрузить устройство с openwrt на SD-карте. Далее скачиваем файл прошивки для SPI-NOR (MTD) на устройство. Для этого можно воспользоваться FTP- или HTTP-сервером на локальном компьютере, или воспользоваться SCP-протоколом.

###### Загрузка с FTP- или HTTP-сервера.

Положите файл прошивки в директорию, к которой даёт доступ сервер. Для FTP используйте пользователя anonymous для обмена файлами по этому протоколу.
Допустим, IP-адрес сервера 192.168.1.100. Скачиваем файл в директорию /tmp на устройстве командой (для FTP):
> wget ftp://192.168.1.100/openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin -O /tmp/openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin

Или командой (для HTTP):
> wget http://192.168.1.100/openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin -O /tmp/openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin

Далее нам необходимо записать прошивку на SPI-NOR (MTD) флеш командой
> mtd write /tmp/openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin factory

Важно, команда mtd из состава openwrt перед записью проверяет название разделов в файле /proc/mtd, последний аргумент в команде указывает название раздела для записи, т.к. блочное устройство может отличаться при каждой загрузке.
После выполнения этой команды устройство готово для работы без SD-карты.

###### Загрузка с использованием SCP.

Допустим, IP-адрес устройства после загрузки 192.168.1.1.
Для выгрузки файла на устройство необходимо на компьютере выполнить команду
> scp openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-factory.bin root@192.168.1.1:/tmp/

После этого необходимо выполнить команду записи прошивку на флеш с самого устройства.
> mtd write /tmp/openwrt-sunxi-cortexa7-sun8i-h2-plus-orangepi-zero-squashfs-mtd-factory.bin factory


##### Обновление.

Как загружать файлы на устройство смотрите в предыдущем разделе.

Для обновления UBoot раздела скачайте файл `openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.u-boot.bin` на устройство в директорию /tmp и выполните команду
> mtd write /tmp/openwrt-sunxi-cortexa7-sun8i-h2-plus-orangepi-zero-squashfs-uboot.bin u-boot

Для обновления прошивки загрузите файл прошивки `openwrt-imx6ull-cortexa7-wirelessroad_gw-imx6ull-squashfs.mtd-sysupgrade.bin` на устройство с использованием wget (FTP или HTTP протокол) или scp, как в предыдущем разделе (первичная запись) и выполните команду 
> sysupgrade -с /tmp/openwrt-sunxi-cortexa7-sun8i-h2-plus-orangepi-zero-squashfs-mtd-sysupgrade.bin 

Команда sysupgrade обновит прошивку с сохранением текущей конфигурации устройства. Ключ '-c' указывает команде найти все изменения в директории /etc/ на устройстве и сохранить их для следующей загрузки (по-умолчанию сохраняются только файлы из списка /etc/sysupgrade.conf, в основном, это конфигурационные файлы в /etc/config/).