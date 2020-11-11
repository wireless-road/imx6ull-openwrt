[Back to Main Readme](../README.md)

# WiFi DualStack setup (AP+STA) UCI (console)

In this document we'll describe how to setup second virtual 
wireless interface with UCI (console) to connect to 
upstream wireless network and work as simple repeater in router mode.

Main disadvantage of this setup is speed drop twice to end client 
connected to our AP wireless virtual interface.

Also AP interface will not be available until sta interface is connected
to uplink (for now, will be resolved with fallback daemon). This is due to
impossibility of virtual interfaces
to work on different channels simultaneously, so physical interface must first
acquire channel number after connecting station, so AP interface will also
work on same channel.

## Description

To make AP+STA repeater in router mode we need to add openwrt network interface,
openwrt firewall rule and additional wireless wifi interface in Client (sta) mode.

All this can be done with editing configuration files or by using `uci` utility.
All steps are presented in corresponding sections.


## Setup with UCI

**1.** Connect to device with SSH
By default device has IPv4 address 192.168.1.1 and no password for `root` user.
Just press enter with blank password when connecting to device.

```
ssh root@192.168.1.1
```

**2.** Add OpenwWrt network interface

To do this, execute next commands:

```
uci rename network.$(uci add network interface)=wsta
uci set network.wsta.proto='dhcp'
uci commit
```

Here we created new `interface` section with name `wsta` and assigned `dhcp` 
protocol to assign addresses on it.

**3.** Add OpenWrt firewall zone and rules

Here we create new zone called `wsta_zone` and assign early created `wsta`
network interface to it. Also we enabled Masquerading with `masq` option and 
allow `input`, `forward` and `output` directives to make it possible to control 
device from uplink wireless network.

```
uci rename firewall.$(uci add firewall zone)=wsta_zone
uci set firewall.wsta_zone.network='wsta'
uci set firewall.wsta_zone.name='wsta_zone'
uci set firewall.wsta_zone.mtu_fix='1'
uci set firewall.wsta_zone.masq='1'
uci set firewall.wsta_zone.input='ACCEPT'
uci set firewall.wsta_zone.forward='ACCEPT'
uci set firewall.wsta_zone.output='ACCEPT'
uci commit
```

Next we enable forwarding between `lan` firewall zone and our `wsta_zone`
to make it possible to access networks from one to another, Internet can also be
accessible with this directives.

```
uci rename firewall.$(uci add firewall forwarding)=wsta2lan
uci set firewall.wsta2lan.dest='lan'
uci set firewall.wsta2lan.src='wsta_zone'


uci rename firewall.$(uci add firewall forwarding)=lan2wsta
uci set firewall.lan2wsta.dest='wsta_zone'
uci set firewall.lan2wsta.src='lan'

uci commit
```

**4.** Add New wireless interface

Ta add additional virtual wireless interface in Client mode you can execute next
commands from console.

`network` option points to our earlier created `wsta` network interface.

`mode` option sets to be `sta` for station in Infrastructure networks. 

`ssid` option is the name of uplink wireless interface.

`encryption` option sets wireless encryption paramaters, it can be `none`, 

`psk-mixed` or `psk2`. Usually `psk-mixed` is universal.

`key` option is the encryption key for uplink wireless network.


```
uci rename wireless.$(uci add wireless wifi-iface)=wsta_wifi
uci set wireless.wsta_wifi.device='radio0'
uci set wireless.wsta_wifi.network='wsta'
uci set wireless.wsta_wifi.mode='sta'
uci set wireless.wsta_wifi.ssid='HomeLan'
uci set wireless.wsta_wifi.encryption='psk-mixed'
uci set wireless.wsta_wifi.key='123456789'

uci commit
```

**5.** Apply all changes.

To apply changes you can reboot device or execute next commands:

```
/etc/init.d/firewall restart
/etc/init.d/network restart
```


## Setup with editing configuration files.

You can also use embedded editor `vi` in openwrt to apply changes.

**1.** Connect to device with SSH
By default device has IPv4 address 192.168.1.1 and no password for `root` user.
Just press enter with blank password when connecting to device.

```
ssh root@192.168.1.1
```

**2.** Add OpenwWrt network interface section to `/etc/config/network` file

```
config interface 'wsta'
        option proto 'dhcp'
```

**3.** Add OpenWrt firewall zone and rules to `/etc/config/firewall` file

```
config zone 'wsta_zone'
        option network 'wsta'
        option name 'wsta_zone'
        option mtu_fix '1'
        option masq '1'
        option input 'ACCEPT'
        option forward 'ACCEPT'
        option output 'ACCEPT'

config forwarding 'wsta2lan'
        option dest 'lan'
        option src 'wsta_zone'

config forwarding 'lan2wsta'
        option dest 'wsta_zone'
        option src 'lan'
```

**4.** Add New wireless interface to `/etc/config/wireless` file

```
config wifi-iface 'wsta_wifi'
        option device 'radio0'
        option network 'wsta'
        option mode 'sta'
        option ssid 'HomeLan'
        option encryption 'psk-mixed'
        option key '123456789'
```

**5.** Apply all changes.

To apply changes you can reboot device or execute next commands:

```
/etc/init.d/firewall restart
/etc/init.d/network restart
```
