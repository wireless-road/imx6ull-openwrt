Linux driver for Silicon Laboratories WFx00 series
==================================================

Compiling and installing
------------------------

Compiling and installing the driver as a module is straightforward if your
kernel sources are located in `/lib/modules/$(uname -r)/build`. Just change
directory to driver source directory and run:

    make
    sudo make install

If kernel sources are located somewhere else, change `KDIR` variable:

    make KDIR=your_kernel_directory
    sudo make KDIR=your_kernel_directory install

Note that driver is called `wfx.ko` and is installed in
`/lib/modules/$(uname -r)/extra/`.

At runtime, WFx module needs:
- `/lib/firmware/wfm_wf200.sec`, the firmware
- `/lib/firmware/wf200.pds`, configuration data of your hardware configuration.
  Check [README for `wfx-firmware` repository][1] for more information.

These files can be retrieved from [Github `wfx-firmware` repository][2].

[1]: https://github.com/SiliconLabs/wfx-firmware/blob/master/PDS/README.md
[2]: https://github.com/SiliconLabs/wfx-firmware

Clock type
----------

By default, the driver is configured to use a crystal for the clock.
If you want to use an external oscillator, in `fwio.c` edit function
`init_gpr()` to add the line `{ 0x0A, 0x10240F },` in `gpr_init[]`.

Loading and probing
-------------------

The WFx chip series can be connected via SPI or via SDIO.


### SPI

You have to declare the WFx chip in your device tree.

Required properties:
- `compatible`: Should be `"silabs,wf200"`
- `reg`: Chip select address of device
- `spi-max-frequency`: Maximum SPI clocking speed of device in Hz
- `interrupts-extended`: Should contain interrupt line (`interrupt-parent` +
  `interrupt` can also been used). Trigger should be `IRQ_TYPE_EDGE_RISING`.

Optional properties:
- `reset-gpios`: phandle of gpio that will be used to reset chip during probe.
   Without this property, you may encounter issues with warm boot.
   (Legacy: when compatible == "silabs,wfx-spi", the gpio is inverted.)

Please consult [`Documentation/devicetree/bindings/spi/spi-bus.txt`][3] for optional
SPI connection related properties,

Example:

    &spi1 {
    	wfx {
    		compatible = "silabs,wf200";
    		pinctrl-names = "default";
    		pinctrl-0 = <&wfx_irq &wfx_gpios>;
    		interrupts-extended = <&gpio 16 IRQ_TYPE_EDGE_RISING>;
    		wakeup-gpios = <&gpio 12 GPIO_ACTIVE_HIGH>;
    		reset-gpios = <&gpio 13 GPIO_ACTIVE_LOW>;
    		reg = <0>;
    		spi-max-frequency = <42000000>;
    	};
    };


[3]: https://www.kernel.org/doc/Documentation/devicetree/bindings/spi/spi-bus.txt

### SDIO

The driver is able to detect a WFx chip on SDIO bus by matching its Vendor ID
and Product ID. However, driver will only provide limited features in this
case. Thus declaring Wfx chip in device tree is strongly recommended (and may
become mandatory in the future).

Required properties:
- `compatible`: Should be `"silabs,wf200"`
- `reg`: Should be `1`

In addition, it is recommended to declare a `mmc-pwrseq` on SDIO host above
WFx.  Without it, you may encounter issues with warm boot. `mmc-pwrseq` should
be compatible with `mmc-pwrseq-simple`. Please consult
[`Documentation/devicetree/bindings/mmc/mmc-pwrseq-simple.txt`][4] for more
information.

Example:

    / {
    	wfx_pwrseq: wfx_pwrseq {
    		compatible = "mmc-pwrseq-simple";
    		pinctrl-names = "default";
    		pinctrl-0 = <&wfx_reset>;
    		reset-gpios = <&gpio 13 GPIO_ACTIVE_LOW>;
    	};
    };
    
    &mmc1 {
    	mmc-pwrseq = <&wfx_pwrseq>;
    	#address-size = <1>;
    	#size = <0>;
    
    	mmc@1 {
    		compatible = "silabs,wf200";
    		reg = <1>;
    		pinctrl-names = "default";
    		pinctrl-0 = <&wfx_wakeup>;
    		wakeup-gpios = <&gpio 12 GPIO_ACTIVE_HIGH>;
    	};
    };

Note that `#address-size` and `#size` shoud already be defined in node `mmc1`,
but it is rarely the case.

[4]: https://www.kernel.org/doc/Documentation/devicetree/bindings/mmc/mmc-pwrseq-simple.txt

### Common properties

Some properties are recognized either by SPI or SDIO versions:
- `wakeup-gpios`: phandle of gpio that will be used to wake-up chip. Without
  this property, driver will disable most of power saving features
- `config-file`: Use an alternative file as PDS. Default is `wf200.pds`. Only
  necessary for development/debug purpose.
- `slk_key`: String representing hexadecimal value of secure link key to
  use (only if driver is compiled with `CONFIG_WFX_SECURE_LINK`). Must contains
  64 hexadecimal digits.

WFx driver also supports `mac-address` and `local-mac-address` as described in
[`Documentation/devicetree/bindings/net/ethernet.txt`][5]

[5]: https://www.kernel.org/doc/Documentation/devicetree/bindings/net/ethernet.txt

### How to change MAC address?

WFx follows standard rules related to MAC addresses under Linux. You can set
them in device tree or once driver is loaded with:

    $ ip link set wlan0 address 01:02:03:04:05:06

### How to use Secure Link?

Secure link feature allows to encrypt communication between host and chip.

First make sure that driver is compiled with `CONFIG_WFX_SECURE_LINK=y`. This
option will pull mbedtls library in ths driver.

Chip may have three mode:
- enforced: a key is burned in chip OTP and secure link is mandatory
- eval: no key is burned but secure link is possible.
- unavailable: secure link is not possible

Key can be set either using `slk_key` module parameter or using
`slk_key` DT attribute. In both case, it should contains 64 hexadecimal
digits.

If chip is in enforced mode, local key is compared with OTP key from chip. Chip
binding can continue only if both keys are equal.

If chip is in eval mode, local key is sent to chip then process continue as in
enforced mode. It should always succed since keys are garanteed to be the same.

If chip is in eval mode, user has also possibility to burn a key in OTP.
Operation is irreversible and chip will automaticaly use enforced mode on next
reset. User has to write key to
`/sys/kernel/debug/ieee80211/phy*/wfx/burn_slk_key`. In order to avoid
unintended key burning, user must follow key with its CRC32. Whole data must be
formatted as a string of hexadecimal digits. So overall process is:

    $ dd if=/dev/urandom bs=8 count=1 > secret
    $ ( xxd -p secret; crc32 secret ) | tr -d '\n' > secret+crc32
    $ dd if=secret+crc32 of=/sys/kernel/debug/ieee80211/phy0/wfx/burn_slk_key

### How to use nl80211 interface?

The driver offer a nl80211 interface from some tasks. The simplest way to
access to this API is to use the command `iw vendor`:

     iw dev <devname> vendor recvbin <oui> <subcmd> <filename|-|hex data>
     iw dev <devname> vendor recv <oui> <subcmd> <filename|-|hex data>
     iw dev <devname> vendor send <oui> <subcmd> <filename|-|hex data>

You can find necessary constants in `nl80211_wfx.h`:
  - The `oui` is always `0x90fd9f`
  - `subcmd` can be `0x11` (`PS_TIMEOUT`), `0x21` (`BURN_PREVENT_ROLLBACK`) and
    `0x31` (`PTA_PARMS`)
  - The argument of the `subcmd` contains a list of attribute in Netlink
    attribute (`nla`) format: 16bits for size, 16bits for ID of the
    attribute, then data and finally padding to align on 32bits.
  - Each attribute is identified by a  number: `1 = PS_TIMEOUT`,
   `2 = ROLLBACK_MAGIC`, `3 = PTA_ENABLE`, `4 = PTA_PRIORITY`,
   `5 = PTA_SETTINGS`
  - The size and the format of each attribute is defined in variable
    `wfx_nl_policy`

Thus, the command below run the command `PS_TIMEOUT` (`0x11`) with argument
`PS_TIMEOUT` (ID `0x01`, then signed 32bit number) with value 0x64:

    $ echo -ne '\x08\x00\x01\x00\x64\x00\x00\x00' | iw dev wlan0 vendor send 0x90fd9f 0x11 -

You also run command `PS_TIMEOUT` (`0x11`) with `recv` to retrieve value:

    $ iw dev wlan0 vendor recv 0x001234 0x11 - < /dev/null
    vendor response: 08 00 01 00 64 00 00 00

Finally nothing prevents you to write and read value in same time:

    $ echo -ne '\x08\x00\x01\x00\x40\x00\x00\x00' | iw dev wlan0 vendor recv 0x90fd9f 0x11 -
    vendor response: 08 00 01 00 40 00 00 00

Note that attribute ID not recognized by command is just ignored:

    $ echo -ne '\x08\x00\x02\x00\xFF\xFF\xFF\xFF' | iw dev wlan0 vendor recv 0x90fd9f 0x11 -
    vendor response: 08 00 01 00 40 00 00 00

In case you want to get ride of `iw`, you can use `libnl` directly (in C,
python, etc...). `libnl` allows to forge complete netlink packets.

### How to prevent firmware rollback?

You use the command `BURN_PREVENT_ROLLBACK` from the [nl80211
API](#how-to-use-nl80211-interface). This command will work only if it
receive attribute `ROLLBACK_MAGIC` with value defined in HIF API
(`0x5C8912F3`):

    $ echo -ne '\x08\x00\x02\x00\xF3\x12\x89\x5C' | iw dev wlan0 vendor send 0x90fd9f 0x21 -

### How to set PTA parameters?

You use the command `PTA_PARMS` from the [nl80211
API](#how-to-use-nl80211-interface) with the attributes ` PTA_SETTINGS`,
`PTA_PRIORITY` and `PTA_ENABLE`. See the HIF API for more information
about content of these attribute.


Advanced driver usage
---------------------

### How to get error messages?

Run `dmesg` to display all message generated by WFx (or any drivers). Note that
`dmesg -w` allow to display driver messages as they arrive.


### Reset device without rebooting target

In order to reset device, device tree should correctly declare `reset-gpio`
(directly in device node for SPI and using `mmc-pwrseq` for SDIO). Once done,
it is possible to dynamically bind and unbind device.

For spi, it is possible to just unbind the device:

    $ ls /sys/bus/spi/drivers/wfx-spi/
    ...
    /sys/bus/spi/drivers/wfx-spi/spi0.0
    ...
    $ echo spi0.0 > /sys/bus/spi/drivers/wfx-spi/unbind
    $ echo spi0.0 > /sys/bus/spi/drivers/wfx-spi/bind

For sdio, it is necessary to unbind the whole sdio bus:

    $ ls /sys/bus/platform/drivers/mmc-bcm2835
    ...
    /sys/bus/platform/drivers/mmc-bcm2835/3f300000.mmc
    ...
    $ echo 3f300000.mmc > /sys/bus/platform/drivers/mmc-bcm2835/unbind
    $ echo 3f300000.mmc > /sys/bus/platform/drivers/mmc-bcm2835/bind

Note: reloading WFx driver allows to reset chip on SPI bus, but it does not work
on SDIO bus.


### Using `spidev` besides `wfx-spi`

It is possible to declare your device compatible with both `silabs,wf200` and
`spidev` (ie. using `/dev/spi0.0`). In this case, you will be able to use
alternatively both drivers without rebooting or changing device tree.

In order to avoid automatic handling of device by spidev, we suggest to
blacklist spidev:

    echo blacklist spidev > /etc/modprobe.d/silabs.conf

You can load spidev later with:

    modprobe spidev

Next, it is possible to bind/unbind device from/to wfx/spidev driver.

To make `/dev/spi0.0` appear:

    echo spi0.0 > /sys/bus/spi/drivers/wfx-spi/unbind
    echo spi0.0 > /sys/bus/spi/drivers/spidev/bind

To make `wlan0` appear:

    echo spi0.0 > /sys/bus/spi/drivers/spidev/unbind
    echo spi0.0 > /sys/bus/spi/drivers/wfx-spi/bind


### Controlling gpio manually

When device is not binded, you can control the reset gpio manually:

    echo spi0.0 > /sys/bus/spi/drivers/wfx-spi/unbind
    echo 13 > /sys/class/gpio/export
    echo out > /sys/class/gpio/gpio13/direction
    echo 0 > /sys/class/gpio/gpio13/value
    echo 1 > /sys/class/gpio/gpio13/value

The WFx driver normally takes over the reset gpio on device binding. However,
it is possible to change this behavior by passing `gpio-reset=-1` as parameter:

    modprobe wfx gpio-reset=-1

It is also possible to change this parameter without unloading the driver with:

    echo -1 > /sys/module/wfx/parameters/gpio_reset

However, you have to unbind/bind device in order to take into account any new
value.

Of course, you can also remove the `reset-gpio` attribute from device tree.


### Loading PDS

You can send PDS fragment to chip using `/sys/kernel/debug/ieee80211/phy*/wfx/send_pds`

You can directly execute `pds_compress` on this file:

    pds_compress YOUR.pds.in /sys/kernel/debug/ieee80211/phy*/wfx/send_pds


## Send arbitray HIF request to chip

For debug purpose (and only for this purpose please), driver provides a way to
send arbitrary HIF request to chip. Thus, data written to file
`/sys/kernel/debug/ieee80211/phy*/wfx/send_hif_msg` will be sent directly to
Ineo. For exemple:

    echo -en "\x18\x00\x2b\x00\x04\x01\x01\x01\x00\x00\x01\x00\x0a\x32\x28\x48\x8c\x96\x6c\x02\x4c\x1d\x4c\x1d" | tee /sys/kernel/debug/ieee80211/phy*/wfx/send_hif_msg

Payload should include HIF header containing message length and command id.

Data must be sent in one call. In example above, `tee` is mandatory else `echo`
tend to send string in multiple parts (it split data on `\n`).

Write will fail only if request is malformatted. So, it succeed even if
firmware returns an error. It is possible to get confirmation returned by
request by reading `send_hif_msg` after writing it (without closing it).  This
process is difficult to support in shell.

Obviously, User is responsible of consequence of this data to driver behavior.
For exemple, sending a Tx request using this interface won't work.

If user want run exemple above with `sudo`, he must take care that pattern
`phy*` won't work.

### Enable/Disable UAPSD

For some tests, UAPSD is required. To get it, you need a kernel compiled with
`CONFIG_MAC80211_DEBUGFS`. Next, you can enable UAPSD with:

    echo 0xF > /sys/kernel/debug/ieee80211/phy*/netdev:wlan0/uapsd_queues

Obviously, you can disable UAPSD with:

    echo 0 > /sys/kernel/debug/ieee80211/phy*/netdev:wlan0/uapsd_queues

### Improving scheduling on slow targets

The wfx driver works with the high priority workqueue. It allows to
limit the latency when waiting for the bus to complete the transactions.
The gains appears when the bus is slow compared to the Wifi connection
(either because the bandwidth of the bus is lower or because the HIF
buffers are full of small frames). In this case, it can provide up to
30% of throughput improvement.

Unfortunately, when the CPU is slow, the driver tries hard to send
frames to the device and the frames supplier has no time to execute. It
ends with a frames shortage that decrease the overall performances.

Therefore:
  - apply the patch below if your CPU is slow (and whatever the speed of
    the bus between the device and your host).
  - don't apply this patch if you use the WF200 over a slow bus (eg.
    SPI)
  - it won't have any effect if your bus and your CPU are fast enough.

```diff
diff --git a/bh.c b/bh.c
index 9f64fac6..2a53368c 100644
--- a/bh.c
+++ b/bh.c
@@ -336,7 +336,7 @@ void wfx_bh_request_rx(struct wfx_dev *wdev)
        control_reg_read(wdev, &cur);
        prev = atomic_xchg(&wdev->hif.ctrl_reg, cur);
        complete(&wdev->hif.ctrl_ready);
-       queue_work(system_highpri_wq, &wdev->hif.bh);
+       schedule_work(&wdev->hif.bh);

        if (!(cur & CTRL_NEXT_LEN_MASK))
                dev_err(wdev->dev, "unexpected control register value: length field is 0: %04x\n",
@@ -351,7 +351,7 @@ void wfx_bh_request_rx(struct wfx_dev *wdev)
  */
 void wfx_bh_request_tx(struct wfx_dev *wdev)
 {
-       queue_work(system_highpri_wq, &wdev->hif.bh);
+       schedule_work(&wdev->hif.bh);
 }

 /*
```

Debugging
---------

### How to enable `dev_dbg()`?

By default, traces defined with `dev_dbg()` are not displayed. The easiest way
to enable them is to add `#define DEBUG` on top of files you want to trace. It
is also possible to enable all traces by setting the `Makefile` variable
`ccflags-y` to `-DDEBUG` but it is insane since it enables too much traces.

It is possible to dynamically enable traces. This way allows to enable/disable
each trace. For example:

    echo 'file sta.c line 1603 +p' > /sys/kernel/debug/dynamic_debug/control

You can also pass `dyndbg=p` to `modprobe` to enable messages during module
loading.

All features of dynamic debug are described in
[`Documentation/dynamic-debug-howto.txt`][6].

Of course, you need to run `dmesg` (or even better `dmesg -w`) to display
messages.

[6]: https://www.kernel.org/doc/Documentation/dynamic-debug-howto.txt

### How to trace events?

Tracers allow to trace plenty of useful events from kernel. It is described in
[`Documentation/trace/events.txt`][7].

You can get a list of tracepoints implemented by WFx with:

    $ ls /sys/kernel/debug/tracing/events/wfx/

Thus, you can trace all bus transfers with:

    $ echo 1 > /sys/kernel/debug/tracing/events/wfx/io_read/enable
    $ echo 1 > /sys/kernel/debug/tracing/events/wfx/io_read32/enable
    $ echo 1 > /sys/kernel/debug/tracing/events/wfx/io_write/enable
    $ echo 1 > /sys/kernel/debug/tracing/events/wfx/io_write32/enable
    $ cat /sys/kernel/debug/tracing/trace_pipe
    kworker/2:0H-23    [002] .... 429125.079545: io_write: QUEUE: 10 04 06 28 02 10 04 04 00 00 c4...
    kworker/2:0H-23    [002] .... 429125.079597: io_read32: CONTROL: 00003004
    kworker/2:0H-23    [002] .... 429125.079631: io_read: QUEUE: 08 00 06 20 00 00 00 00 00 30 (10 bytes)
    kworker/2:0H-23    [002] .... 429125.079685: io_read32: CONTROL: 00003000
    kworker/2:0H-23    [002] .... 429125.079719: io_read32: CONFIG: 01070000
    kworker/2:0H-23    [002] .... 429125.079749: io_write32: CONFIG: 01070000
    <Ctrl+C>

Note, the `tee` command can replace a series of `echo X > file`:

    $ echo 1 | tee /sys/kernel/debug/tracing/events/wfx/io_*/enable

Disable traces with:

    $ echo 0 > /sys/kernel/debug/tracing/events/enable

It can be more convenient to follow higher level HIF messages:

    $ echo 1 | tee /sys/kernel/debug/tracing/events/wfx/hif_*/enable
    $ cat /sys/kernel/debug/tracing/trace_pipe
    kworker/2:0H-23    [002] .... 429125.079556: hif_send: 40:WRITE_MIB_REQ/TEMPLATE_FRAME: 00 00 c4 00...
    kworker/2:0H-23    [002] .... 429125.079636: hif_recv: 32:WRITE_MIB_CNF: 00 00 00 00 (8 bytes)
    kworker/2:0H-23    [002] .... 429125.079834: hif_send: 48:WRITE_MIB_REQ/RX_FILTER: 08 00 00 00 40 00 00 00 (16 bytes)
    kworker/2:0H-23    [002] .... 429125.079915: hif_recv: 40:WRITE_MIB_CNF: 00 00 00 00 (8 bytes)
    kworker/2:0H-23    [002] .... 429125.080412: hif_send: 56:START_SCAN_REQ: 00 00 02 00 00 00 00 00 02...
    <Ctrl+C>

It is possible to filter events (see section 4 and 5 of
[`Documentation/trace/events.txt`][7]). For example, to remove Tx request, Tx
confirmation and Rx indication from results, you can do:

    $ echo 'msg_id != 4 && msg_id != 0x84' | tee /sys/kernel/debug/tracing/events/wfx/hif_*/filter

It can also be convenient to trace IRQs associated to the WFx chip. We will
trace all IRQs and add a filter to only show IRQs related to WFx:

    $ echo 'name == "wfx"' >  /sys/kernel/debug/tracing/events/irq/irq_handler_entry/filter
    $ echo 1 > /sys/kernel/debug/tracing/events/irq/irq_handler_entry/enable
    $ cat /sys/kernel/debug/tracing/trace_pipe
          <idle>-0     [000] d.h. 429585.854353: irq_handler_entry: irq=167 name=wfx
    kworker/2:0H-23    [002] .... 429585.854353: hif_send: 16:START_SCAN_REQ: 00 00 02 00 00 00 00 00 02...
    kworker/2:0H-23    [002] .... 429585.854413: hif_recv: 8:START_SCAN_CNF: 00 00 00 00 (8 bytes)
          <idle>-0     [000] d.h. 429585.859621: irq_handler_entry: irq=167 name=wfx
    kworker/2:0H-23    [002] .... 429585.859942: hif_recv: 20:RX_IND: 00 00 00 00 01 00 00 6c 80 00 04 00...

Another example would be to also trace wake-up gpio:

    $ echo 1 > /sys/kernel/debug/tracing/events/gpio/gpio_value/enable
    $ echo 'gpio == 12' > /sys/kernel/debug/tracing/events/gpio/gpio_value/filter
    $ cat  /sys/kernel/debug/tracing/trace_pipe
    kworker/3:0H-21    [003] ...1   342.741704: gpio_value: 12 set 1
           <...>-1172  [000] d.h2   342.743710: irq_handler_entry: irq=182 name=wfx
            spi0-182   [000] d.h3   342.745488: irq_handler_entry: irq=182 name=wfx
    kworker/3:0H-21    [003] ...1   342.749736: gpio_value: 12 set 0
    kworker/2:0H-23    [002] ....   342.854353: hif_send: 16:START_SCAN_REQ: 00 00 02 00 00 00 00 00 02...
    kworker/2:0H-23    [002] ....   342.854413: hif_recv: 8:START_SCAN_CNF: 00 00 00 00 (8 bytes)

It is also possible to trace requests from mac80211 stack to WFx driver:

    $ echo 1 | tee /sys/kernel/debug/tracing/events/mac80211/drv_*/enable
    $ cat /sys/kernel/debug/tracing/trace_pipe
    wpa_supplicant-156 [002] .... 429945.348883: drv_hw_scan: phy0 vif:wlan0(2)
     ksoftirqd/0-9     [000] d.H. 429945.349256: irq_handler_entry: irq=167 name=wfx
    kworker/2:0H-23    [002] .... 429945.349360: hif_send: 48:WRITE_MIB_REQ/TEMPLATE_FRAME: 00 09 c4 00 40...
    kworker/2:0H-23    [002] .... 429945.349425: hif_recv: 16:WRITE_MIB_CNF: 00 00 00 00 (8 bytes)
            sshd-5400  [000] d.h. 429945.349595: irq_handler_entry: irq=167 name=wfx
    kworker/2:0H-23    [002] .... 429945.349596: hif_send: 56:WRITE_MIB_REQ/RX_FILTER: 08 00 00 00 40 00 00...
    kworker/2:0H-23    [002] .... 429945.349637: hif_recv: 24:WRITE_MIB_CNF: 00 00 00 00 (8 bytes)
    wpa_supplicant-156 [002] .... 429945.349752: drv_return_int: phy0 - 0
    kworker/2:0H-23    [002] .... 429945.349867: hif_send: 0:START_SCAN_REQ: 00 00 02 00 00 00 00 00 02 64...
          <idle>-0     [000] d.h. 429945.701657: irq_handler_entry: irq=167 name=wfx
    kworker/2:0H-23    [002] .... 429945.701732: hif_recv: 8:SCAN_CMPL_IND: 00 00 00 00 00 0d 00 00 (12 bytes)
    wpa_supplicant-156 [002] .... 429945.702647: drv_get_survey: phy0 idx:0
    wpa_supplicant-156 [002] .... 429945.702664: drv_return_int: phy0 - -95

Note that there is no real way to trace events during module loading. The best
solution is to defer probing, setup traces and finally request probe manually:

    $ echo 0 > /sys/bus/spi/drivers_autoprobe
    $ modprobe wfx
    $ echo 1 | tee /sys/kernel/debug/tracing/events/wfx/hif_*/enable
    $ cat /sys/kernel/debug/tracing/trace_pipe &
    $ echo spi0.0 > /sys/bus/spi/drivers/wfx-spi/bind

(see also [this patch][8])

An alternative (less intrusive?) that does not imply driver reload is:

    $ echo spi0.0 > /sys/bus/spi/drivers/wfx-spi/unbind
    $ echo 1 | tee /sys/kernel/debug/tracing/events/wfx/hif_*/enable
    $ cat /sys/kernel/debug/tracing/trace_pipe &
    $ echo spi0.0 > /sys/bus/spi/drivers/wfx-spi/bind


Also note that `perf` command provides an alternative way to access to
tracepoints:

    $ perf trace --no-syscalls --event 'wfx:*' sleep 10

However, for our needs, features are more limited than
`/sys/kernel/debug/tracing` interface.

[7]: https://www.kernel.org/doc/Documentation/trace/events.txt
[8]: https://lore.kernel.org/patchwork/patch/240185/

### How to use gdb

Before to start, you need to install `gdb-multiarch`:

    apt-get install gdb-multiarch

Also add `set auto-load safe-path /` to your `~/.gdbinit`:

    echo 'set auto-load safe-path /' >> ~/.gdbinit

Make sure you have write access to serial port (usually /dev/ttyUSB0). If not,
run:

    adduser YOUR_USER dialout

then reboot your work station.

Next, follow these steps:

 1. Connect uart between your workstation and your target
 2. Make sure uart is not used by WFx chip (check your PDS)
 3. Make sure uart is not used as console by host. Check `/proc/cmdline` does
    not contains `console=ttyAMA0,115200`. Remove 'console=serial0,115200'
    from `/boot/cmdline.txt` if necessary.
 4. Make sure you have compiled/deployed your own kernel (it is possible to not
    compile kernel yourself, it simplifies things a lot)
 5. Connect to your target using ssh
 6. Configure kgdb to use serial line:

         $ echo ttyAMA0,115200 > /sys/module/kgdboc/parameters/kgdboc

 7. Load WFx module

         $ modprobe wfx

 8. Break kernel execution:

         $ echo g > /proc/sysrq-trigger

 9. Run `gdb` on `vmlinux` file from your build directory

         $ gdb-multiarch vmlinux

 10. Under gdb, connect to target

         (gdb) set remotebaud 115200
         (gdb) target remote /dev/ttyUSB0
         Remote debugging using /dev/ttyUSB0
         kgdb_breakpoint () at kernel/debug/debug_core.c:1071
         1071            arch_kgdb_breakpoint();

 11. Load symbols from all loaded modules (note that `lx-symbols` does not
     recognize `~`)

          (gdb) lx-symbols /home/jpouiller/wfx-linux-driver
          loading vmlinux
          scanning for modules in /home/jpouiller/wfx-linux-driver
          scanning for modules in /home/jpouiller/linux
          loading @0x7f111000: /home/jpouiller/wfx-linux-driver/wfx.ko
          loading @0x7f10a000: /home/jpouiller/linux/drivers/spi/spidev.ko
          [...]


  12. Place a breakpoint:

          (gdb) b wfx_scan_start
          Breakpoint 1 at 0x7f136664: file /home/jpouiller/wfx-linux-driver/scan.c, line 32.

  13. Continue execution:

          (gdb) c

  14. End with:

          (gdb) detach

Note that it is possible to use it inside eclipse, however, process is far from
being easy and many things must be done manually.

Architecture
------------

The diagram below show the driver architecture:

    ,------------------------------------.
    |                mac80211            |
    `------------------------------------'
    ,------------+-----------+-----------.
    |    sta     |           |           |
    |    scan    |           |           |
    |    main    |           |           |
    +------------+  data_tx  |           |
    |    key     |           |  data_rx  |
    | hif_tx_mib |   queue   |           |
    |   hif_tx   |           |           |
    |   hif_rx   |           |           |
    |  hif_api_* |           |           |
    +------------+-----------+-----------+--------.
    |                  bh                |        |
    |              secure_link           |  fwio  |
    +------------------------------------+--------+
    |                     hwio                    |
    +---------------------------------------------+
    |                   bus_sdio                  |
    |                   bus_spi                   |
    |                    hwbus                    |
    `---------------------------------------------'


 * `bus_sdio.c`, `bus_spi.c` and `hwbus.h` provide abstraction from bus access.
   They are in charge of driver registration, device probing and low-level
   access to hardware.

 * No functions directly access to the hardware, they use abstraction provided
   by `hwio.c` and `hwio.h`. Most of these functions are only used during
   device initialisation. Once device is initialized, only `wfx_data_read()`,
   `wfx_data_write()` and `control_reg_read()` are used.

 * `fwio.c` is in charge of firmware loading.

 * Once the device is initialized, functions provided by hwio.c are only used
   by a `work_struct` launched by `bh.c` This thread schedules data from/to
   device.

 * `hif_rx.c`, `hif_tx.c` and `hif_tx_mib.c` provide a high layer interface to
   bh thread. It manages asynchronous communication with bh thread.

 * `sta.c` and `scan.c` provide interfaces with kernel API.

 * Beside that, `data_tx.c` and `queue.c` are in charge of Tx data while
   `data_rx.c` is in charge of Rx data.

Upstream status
---------------

The developments of the driver is now located in the Linux kernel. This driver
continues to live but it only get backports from the mainstream driver.

The following table show versions equivalences between mainstream and this
driver:

    Kernel  |  This driver
  ----------+------------------------------------
    5.5     |  2.3.5
    5.6     |  somewhere between 2.3.5 and 2.4
    5.7     |  same than kernel 5.6
    5.8     |  2.4.3
    5.9     |  2.5.1
    5.10    |  2.7

Keep in mind that it is not an exact science. This driver add support for secure
link and is able to compile with kernel up to 3.18. So the code is necessarily a
bit different than the mainstream kernel. In addition, the patch from mainstream
may apply directly to this repository and a few changes could be necessary.

And also, keep in mind that some patches are automatically backported the Long
Term Support of the kernel.

