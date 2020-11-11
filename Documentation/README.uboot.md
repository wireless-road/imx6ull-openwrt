[Back to Main Readme](../README.md)

# imx6ull UBoot upgrade

To make upgrade through uboot you'll need serial connection to board, tftp-server installed on laptop/PC and direct ethernet connection between them.

In this manual we use tftp-server IPv4 address **192.168.0.100**, board IPv4 address **192.168.0.99** and firmware upgrade file (sysupgrade, not factory) **fw.bin**

1. Hit any key while uboot running.

2. Setup addresses for uboot:

> setenv serverip 192.168.0.100

> setenv ipaddr 192.168.0.99

> setenv ethaddr 00:11:22:33:44:55


3. Execute load to memory to loadaddr (default 0x82000000)
>tftpboot fw.bin

4. Probe SPI flash
>sf probe

5. Erase block before writing (it can take a while)
>sf erase 0x100000 0xf00000

6. Write firmware to flash
>sf write 0x82000000 0x100000 0xf00000

7. At this point firmware is loaded and you can restart board by executing:
>reset

command or simply powering it off and on.

### Caveat
After executing command *tftpboot fw.bin* on success you will get line like this:
> Bytes transferred = 7864606 (78011e hex)

You can use HEX number in sf erase and sf write command as last argument (instead of 0xf00000) to make things faster. For erase we need to round up this number in higher order. So in this example we can use this commands:
>sf erase 0x100000 0x790000

>sf write 0x82000000 0x100000 0x78011e

