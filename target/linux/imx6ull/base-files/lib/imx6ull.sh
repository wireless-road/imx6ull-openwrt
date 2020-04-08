#!/bin/sh
#
# Copyright (C) 2010-2013 OpenWrt.org
#

IMX6_BOARD_NAME=
IMX6_MODEL=

imx6ull_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)

	case "$machine" in
	"LoraWan Gateway Ethernet")
		name="lorawan_gateway_ethernet";
		;;
	"LoraWan Gateway WiFi")
		name="lorawan_gateway_wifi";
		;;
	"Video Stream Ethernet")
		name="video_stream_ethernet";
		;;
	"Video Stream WiFi")
		name="video_stream_wifi";
		;;
	*)
		name="generic"
		;;
	esac
	
	if [ "$name" != "generic" ];then
		model="$name";
	fi;
	
	[ -z "$IMX6_BOARD_NAME" ] && IMX6_BOARD_NAME="$name"
	[ -z "$IMX6_MODEL" ] && IMX6_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$IMX6_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$IMX6_MODEL" > /tmp/sysinfo/model
}
