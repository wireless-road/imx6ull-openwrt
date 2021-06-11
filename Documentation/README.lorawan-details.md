[Back to Main Readme](../README.md)

### OpenWrt default parameters.

##### IP-address, remote access, user and password.

On first boot device uses default IP address - `192.168.1.1`.
It can be remotely accessed via `ssh`.

default user - `root`
default password - no password.

##### LoraWan configuration.

`packet-forwarder` utility configuration file might be found here:
> /etc/global_conf.json

`lora_pkt_fwd` service that implements LoRaWAN gateway functionality might be driven in following way. To start daemon manually type following:
> /etc/init.d/lora_pkt_fwd start

To enable autostart of daemon on powering up the device:
> /etc/init.d/lora_pkt_fwd enable


On first load, after powering up `/etc/init.d/lora_pkt_fwd` script resets `SX1301's` RESET GPIO. GPIO configuration might be found here: 
> /etc/config/lora

By default, following gpio pins used:
> config sx1301 'SX1301_conf'
>        option chipreset_pin '1'
>        option pllreset_pin '11'

`pllreset_pin` not used currently, `chipreset_pin` transforms to `IMX6's` GPIO pad number (130).


##### Детальное описание изменений.

###### UBoot

Для поддержки загрузки прошивки со SPI-NOR флеш-накопителя был сформирован пакет загрузчика специально для платформы i.mx6ull. Был сформирован патч от производителя для релиза u-boot 2017.07. Адрес репозитория:
> https://github.com/boundarydevices/u-boot-imx6.git

Из этого репозитория и оригинального релиза 2017.07 был сформирован патч `package/boot/uboot-imx6ull/patches/200-uboot_2017.07_i.mx6.patch`. Таким образом мы привели исходные тексты загрузчика к официально поставляемым производителем.
Далее, патч `package/boot/uboot-imx6ull/patches/205-uboot-add_wirelessroad_gw_imx6ull.patch` добавляет конфигурационные и dts файлы для поддержки GW-IMX6ULL.
В патче `package/boot/uboot-imx6ull/patches/300-uboot-add-spinor-boot.patch` отключается загрузка с NAND накопителя, а вместо неё происходит загрузка со SPI-NOR флеш-накопителя.

###### LoraWan
Пакеты для поддержки устройств Lora были портированы из репозитория:
> https://github.com/xueliu/lora-feed

Пакеты были переработаны для локальной сборки, обновление исходных текстов производится вручную, без привязки к внешним ресурсам. Патчи из репозитория применены к исходным текстам пакетов вручную.
Для выбора пакетов LoraWan в menuconfig системы сборки openwrt создан раздел Lora Packages, а не стандартные пути Libraries, Network. Это сделано для удобства нахождения пакетов в конфигурационном меню.

##### Разделы SPI-NOR флеш-накопителя.
Openwrt позволяет динамически изменять логическую структуру флеш-накопителей при загрузке системы. Это сделано для удобства работы основных подсистем, требовательных к размеру файлов, техническими ограничениями работы с NOR-флеш-накопителями (отсутствие wear-leveling и невозможностью работы как с обычным блочным устройством).
Основной файловой системой в среде OpenWrt для ядра linux считается squashfs, вторичная файловая система, используемая OpenWRT - JFFS2. Первая не поддерживает режим записи (статическая) и формируется в момент сборки прошивки, имеет лучшие характеристики по параметрам сжатия дерева директорий и файлов. Вторая файловая система поддерживает запись и wear-leveling для уменьшения износа флеш-накопителей, но из-за этого обладает значительно худшими характеристиками сжатия.

Базовым набором разделов на флеш-накопителе считается раздел uboot для загрузчика и раздел firmware для основной системы.

Базовые разделы упомянуты в соответствующих dts-файлах для UBoot и ядра linux.

При каждой загрузке Openwrt происходит два логических разделения (split) разделов (partition). Первое разделение происходит по признаку имени раздела с прошивкой (в разделе firmware подразумевается наличие ядра linux упакованного в особый формат образов uImage, создаётся утилитой mkimage из состава UBoot, и наличие squashfs), в котором считывается размер ядра linux, в конце которого по границе erasesize (минимальный объём стирания) флеш-накопителя ищется признак (MAGIC комбинация байт) начала squashfs. Если файловая система найдена, в системе динамически создаются разделы `kernel` и `rootfs`. Раздел kernel точно соответствует границам ядра linux, а `rootfs` занимает всю оставшуюся область.
Второе разделение происходит при появлении раздела `rootfs`, в конце которого ядро ищет (по границе erasesize) указатель на начало JFFS2. Указатель помещается при сборке прошивки и выравнивается по размеру блока накопителя. После чего ядро создаёт ещё один раздел с названием `rootfs_data`.
При дальнейшей загрузке системы происходит монтирование доступных разделов в необходимом порядке. За это отвечает исполняемый файл `mount_root`. При самой первой загрузке устройства происходит создание JFFS2 файловой системы на разделе `rootfs_data`.

Для привычной работы в среде openwrt с возможностью записи и удаления файлов используется подсистема ядра OverlayFS. Эта файловая система позволяет эмулировать поведение стандартных систем оперируя только одной файловой системой доступной для записи, все изменения в структуре сохраняются в ней. 

Для удобства обновления и первичной записи на флеш также представлен раздел `factory`, охватывающий весь объём SPI-NOR флеш-накопителя (8МиБ).

Наглядное представление разделов на SPI-NOR представлено ниже:

<table width="100%" style="color: #eeeeee; vertical-align: middle; text-align: center;">
  <tr>
    <td rowspan="2" style="background-color: #777777;">
      Доступно на всех этапах загрузки. Вписаны статически в DTS файлах UBoot и ядра linux
    </td>
    <td colspan="4" style="background-color: #444444; font-family: Courier New, monospace">
     <b>factory</b><br>
      (8192 KiB)<br>
      (0x000000 - 0x800000)
    </td>
  </tr>
  <tr>
    <td rowspan="3" style="background-color: #444444; font-family: Courier New, monospace"> 
      <b>u-boot</b><br>
      (1024 KiB)<br>
      (0x000000 - 0x100000)</td>
    <td colspan="3" style="background-color: #444444; font-family: Courier New, monospace">
      <b>firmware</b><br>
    (7168 KiB)<br>
    (0x100000 - 0x800000)
    </td>
  </tr>
  <tr>
    <td style="background-color: #777777;">Первый этап разделения при загрузке ядра</td>
    <td rowspan="2" style="background-color: #444444; font-family: Courier New, monospace">
    <b>kernel</b><br>
    (динамический)
    </td>
    <td colspan="2" style="background-color: #444444; font-family: Courier New, monospace">
    <b>rootfs</b><br>
    (динамический)
    </td>
  </tr>
  <tr>
    <td style="background-color: #777777;">Второй и окончательный этап разделения при загрузке ядра.</td>
    <td style="background-color: #444444; font-family: Courier New, monospace">
    <b>rootfs</b><br>
    (динамический)
    </td>
    <td style="background-color: #444444; font-family: Courier New, monospace">
    <b>rootfs_data</b><br>
    (динамический)
    </td>
  </tr>
</table>
