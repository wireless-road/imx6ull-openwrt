#!/bin/bash

#Put Openwrt config for new board to './openwrt-configs' directory with '.config' suffix
# so this script can work with it.


name="$0";
boardsdir="./openwrt-configs";

show_boards() {
	echo -n "Available board names:";
	local itr=0;
	for l in $(ls -1 $boardsdir/*.config|sed -e 's/\.config//g'|sed -e 's/.*\///g');do
		if [ $itr == "0" ];then
		    echo ; echo "     ";
		    itr=5;
		fi;
		echo -n "$l ";
		itr=$(expr $itr - 1);
	done
	echo;echo;
};

show_usage() {
	echo;
	echo "USAGE:";
	echo "     $name [command or boardname]";
	echo ;
	echo "Where [command] can be one of:";
	echo "     list       to list available boards configs to compile.";
	echo "     help       to show this help.";
	echo "     usage      to show this help.";
	echo ;
	echo "Example:";
	echo "     $name list";
	echo "  or";
	echo "     $name zbt-we1326";
	echo;
};

compile() {
	local boardname="$1";
	local configfile="$boardsdir/$boardname.config";
	if [ ! -f "$configfile" ];then
		echo;
		echo "   No such config file found. Use 'list' argument to see available configs.";
		echo;
		return;
	fi
	
	if [ ! -d "./feeds" ];then
		./scripts/feeds update -a
		./scripts/feeds install -a
	fi
	
	cp "$configfile" ./.config;
	yes "" | make oldconfig;
	make download;
	make -j $(nproc --ignore=3);
	echo "   Done.";
};


if [ X"$1" = X"" ]; then
	show_usage;
	exit 0;
fi

case "$1" in
list)
	show_boards;
	;;
help|usage)
	show_usage;
	;;
*)
	compile "$1";
	;;
esac




