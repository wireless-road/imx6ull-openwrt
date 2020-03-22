#!/bin/sh

sudo valgrind -s --leak-check=full --show-leak-kinds=all ./ttyapd -f -i wlan0 -t /dev/tty63
