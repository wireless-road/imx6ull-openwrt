dt="/home/build/bin/"$(date '+%Y_%m_%d_%H_%M_%S/');
mkdir "$dt"

# mkdir -p ./bin/targets/imx6ull/cortexa7/
# touch ./bin/targets/imx6ull/cortexa7/hello.txt
# touch ./bin/targets/imx6ull/cortexa7/hello2.txt
./compile.sh lorawan_gateway_ethernet
ls /home/build/openwrt/bin/targets/imx6ull/cortexa7/

cp -R /home/build/openwrt/bin/targets/imx6ull/cortexa7/ "$dt"
