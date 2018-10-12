#
# Copyright (C) 2010-2015 OpenWrt.org
#

platform_check_image() {
	local board=$(board_name)
	local magic="$(get_magic_long "$1")"

	if [ "$magic" == "27051956" ]; then
		PART_NAME="firmware";
		return 0;
	fi

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_do_upgrade() {
	local board=$(board_name)
	local magic="$(get_magic_long "$1")"

	if [ "$magic" == "27051956" ]; then
		PART_NAME="firmware";
		default_do_upgrade "$ARGV";
		return 0;
	fi

}
