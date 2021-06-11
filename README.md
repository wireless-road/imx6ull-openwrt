
# NetSoM based on IMX6ULL

[Uboot update](Documentation/README.uboot.md)

[OpenWrt image build steps explanation. LoRaWAN Gateway configuration as example](Documentation/README.lorawan-details.md)

[Resulted built image description. LoRaWAN Gateway configuration as example.](Documentation/README.lorawan-details.md)

[ffmpeg video streaming configuration files](Documentation/README.ffmpeg.md)

[Audio codec WM8960 configuration.](Documentation/README.audio.md)

[Audio streaming using SIP protocol](Documentation/README.sip.md)

[WiFi station Dual Setup (AP+STA) UCI (console)](Documentation/README.wifi-sta-dualsetup-uci.md)

[WiFi station Dual Setup (AP+STA) LuCi (Web-interface)](Documentation/README.wifi-sta-dualsetup-luci.md)

[Amazon Voice Service SDK Setup](Documentation/README.amazon-voice-service.md)

Documentation in `Documentation` folder might be outdated. Better to check latest manuals from our website:

# User manuals and developer guides
[OpenWrt NetSoM developers guide. How to compile image.](https://m2m-tele.com/blog/2021/03/11/openwrt-som-developers-guide-part-1/)

[OpenWrt NetSom users manual. How to configure network.](https://m2m-tele.com/blog/2021/03/12/openwrt-configuration-users-guide-part-1-how-to-configure-network/)

[OpenWrt NetSoM users manual. How to configure WI-FI in STA+AP mode.](https://m2m-tele.com/blog/2021/03/12/openwrt-configuration-users-guide-part-1-how-to-configure-network-2/)

[IMX6ULL CAN bus developers guide. Part 1.](https://m2m-tele.com/blog/2021/03/12/imx6-can-bus/)

[IMX6 CAN bus developers guide. Part 2.](https://m2m-tele.com/blog/2021/03/13/imx6-can-bus-2/)

[IMX6 audio recording and playing solution](https://m2m-tele.com/blog/2021/03/18/imx6-audio-recording-playing/)

[IMX6 camera. Video streaming solution. RTSP/MPEGTS.](https://m2m-tele.com/blog/2021/03/19/imx6-camera-module-video-streaming/)

[IMX6 audio streaming solution using SIP protocol](https://m2m-tele.com/blog/2021/03/21/imx6-audio-streaming/)

[OpenWrt NetSom developers guide. MicroSD card support.](https://m2m-tele.com/blog/2021/03/22/openwrt-netsom-developers-guide-adding-support-of-microsd-card/)

[OpenWrt NetSoM users guide. Running Alexa Voice service.](https://m2m-tele.com/blog/2021/03/26/openwrt-netsom-users-guide-using-as-amazon-alexa-development-kit/)

[OpenWrt NetSoM users guide. Lorawan configuration using Web Interface.](https://m2m-tele.com/blog/2021/04/02/lora-indoor-gateway-configuration-using-ui-web-interface/)

[OpenWrt NetSoM user guide. 3G network configuration.](https://m2m-tele.com/blog/2021/04/04/users-guide-openwrt-3g-modem-network-configuration/)

[OpenWrt GPIO configuration and control using web interface.](https://m2m-tele.com/blog/2021/04/29/imx6-gpio-control-web-interface/)

### Getting started.
To simplify build process, you may use on of multiple pre-defined configuration and automatization build process script:
> ./compile.sh

To get list of available configurations type:
> ./compile.sh list

To compile one of configuration run same script with configuration name provided as argument. For example:
> ./compile.sh amazon_voice_service_wifi
