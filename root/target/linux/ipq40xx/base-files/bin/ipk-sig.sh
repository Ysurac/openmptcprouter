#!/bin/sh

sig="/tmp/.$(cat /proc/sys/kernel/random/uuid).sig"

check() {
	local ipk="$1"
	[ -z "$ipk" ] && return 1

	tar -xzOf "$ipk" ./control+data.sig > $sig || return 2

	tar -xzOf "$ipk" ./control.tar.gz ./data.tar.gz | usign -V -m - -P /etc/opkg/keys -x $sig || return 3
}

check $1
status=$?
rm -f $sig
return $status
