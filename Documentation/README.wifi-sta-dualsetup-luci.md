[Back to Main Readme](../README.md)

# WiFi DualStack setup (AP+STA) LuCi (Web-interface)

In this document we'll describe how to setup second virtual 
wireless interface with LuCi (web-interface) to connect to 
upstream wireless network and work as simple repeater in router mode.

Main disadvantage of this setup is speed drop twice to end client 
connected to our AP wireless virtual interface.
Also AP interface will not be available until sta interface is connected
to uplink (for now, will be resolved with fallback daemon). This is due to
impossibility of virtual interfaces
to work on different channels simultaneously, so physical interface must first
acquire channel number after connecting station, so AP interface will also
work on same channel.

## Terminology
***PHY*** - physical interface not showed in network interfaces but can be 
shown by `iw list` command with all information about interface available.

***VIF*** - virtual interface, that is presented in system as wlan* network
interfaces. There can be several *vifs* for each *phy* interface.

***radio*** - just *phy* interface name in terms of openwrt naming scheme.


## Setup

**1.** Login to web-page of device using it's IP address (default 192.168.1.1).
Username is `root` password by default is blank, so leave it empty.
![Login Page](images/wifi-sta-dualsetup/01-login_page.png)

**2.** Navigate to network interfaces page with menu on top of page.
![Network Interfaces menu](images/wifi-sta-dualsetup/02-interfaces-menu.png)

3. Push `Add new interface...` button.
![Add new interface](images/wifi-sta-dualsetup/03-interfaces-page.png)

**4.** Here we add our outgoing network interface with name `wsta` and DHCP-client.
After finished just push `Create interface` button.
**WARNING!** Don't enable bridge on this interface as station *vif* cannot be 
added to bridge.
![New interface tab](images/wifi-sta-dualsetup/04-interfaces-addif.png)

**5.** Navigate to firewall configuration with menu on top of page.
![Network Interfaces menu](images/wifi-sta-dualsetup/05-firewall-menu.png)

**6.** Push `Add` button to add new firewall zone.
![Network Interfaces menu](images/wifi-sta-dualsetup/06-firewall-page.png)

**7.** On this page we creating new firewall zone for our station named `wsta`. 
Be sure, that `Input` set to `accept` and `Masquerading` is enabled for outgoing 
traffic. We doing `accept` on `Input` to make configuration available (LuCi/telnet)
from our uplink network.
![Network Interfaces menu](images/wifi-sta-dualsetup/07-firewall-addzone1.png)
Here we set `Covered networks` to our previously created interface `wsta`
and enable forwarding with `lan` zone to enable access to uplink for our 
own stations connected to our AP *vif*.
![Network Interfaces menu](images/wifi-sta-dualsetup/07-firewall-addzone2.png)
After finished push `Save` button.

**8.** Navigate to Wireless configuration with menu on top of page.
![Network Interfaces menu](images/wifi-sta-dualsetup/08-wireless-menu.png)

**9.** In order to add new *vif* push `Add` button on first row last cell
describing *phy/radio* interface.
![Network Interfaces menu](images/wifi-sta-dualsetup/09-wireless-page.png)

**10.** This window is separated with `Device Configuration` which describes 
*phy/radio* and `Interface Configuration` which describes *vif*. 
We don't need to change anything on `Device Configuration` section.
Scroll down to Last one
![Network Interfaces menu](images/wifi-sta-dualsetup/10-wireless-addif1.png)
Here we set `mode` to `Client`, setup `ESSID` with your wireless network name to 
connect to. Don't forget to set `Network` to our newly created `wsta` interface.
![Network Interfaces menu](images/wifi-sta-dualsetup/10-wireless-addif2.png)

**11.** Switch to `Wireless Security` tab and setup parameters of uplink network.
Usually `Encryption` set to `WPA-PSK/WPA2-PSK Mixed Mode` is universal. 
Set `Encryption` and `Key` according to your uplink network.
![Network Interfaces menu](images/wifi-sta-dualsetup/11-wireless-addif-enc.png)
After done, just push `Save` button.

**12.** Recheck downstream AP *vif* parameters by pressing `Edit` button on
corresponding row.
![Network Interfaces menu](images/wifi-sta-dualsetup/12-wireless-checkap-edit.png)

**13.** Be sure that our AP interface `Network` is set to `lan`.
![Network Interfaces menu](images/wifi-sta-dualsetup/13-wireless-checkap-netif.png)
After finished push `Save` button.

**14.** At the end press `Save & Apply` button at the bottom of page.
![Network Interfaces menu](images/wifi-sta-dualsetup/14-wireless-save-n-apply.png)

After parameters is applied everything should work fine. 
AP interface will go up after station interface successfully connects to uplink.




