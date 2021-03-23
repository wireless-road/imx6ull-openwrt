
# WirelessRoad IMX6ULL

[Uboot update](Documentation/README.uboot.md)

[Пример сборки прошивки с тонкой настройкой на примере LoraWan(GW)](Documentation/README.lorawan-details.md)

[Детальное описание собранной прошивки на примере LoraWan(GW)](Documentation/README.lorawan-details.md)

[Детальное описание конфигурации ffmpeg для прошивок STREAM и STREAM-WIFI](Documentation/README.ffmpeg.md)

[Описание настройки аудиокодека WM8960 (английский язык)](Documentation/README.audio.md)

[Описание настройки SIP для работы в прошивке](Documentation/README.sip.md)

[WiFi station Dual Setup (AP+STA) UCI (console)](Documentation/README.wifi-sta-dualsetup-uci.md)

[WiFi station Dual Setup (AP+STA) LuCi (Web-interface)](Documentation/README.wifi-sta-dualsetup-luci.md)

[Amazon Voice Service SDK Setup](Documentation/README.amazon-voice-service.md)


### Автоматическая сборка прошивки.
Для простоты сборки можно использовать скрипт, который использует готовые конфигурационные файлы openwrt для автоматической загрузки исходных текстов пакетов и компиляции прошивки.
> ./compile.sh

Для вывода списка доступных целевых прошивок можно использовать команду
> ./compile.sh list

Для автоматической сборки нужно использовать имя целевого конфигурационного файла как аргумент.
Например,
> ./compile.sh stream-imx6ull
