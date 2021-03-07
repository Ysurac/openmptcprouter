#!/bin/sh
# Copyright (C) 2009 OpenWrt.org
# Copyright (c) 2021 Oliver Welter

setup_switch_dsa() {
	local tries=15
	
	/bin/sleep 10

	local bridge_names=`/sbin/uci show network | /bin/grep type | /bin/grep bridge | /usr/bin/cut -d . -f2`

	for name in $bridge_names; do
		local bridge_interface=`/sbin/uci get network.$name.ifname`		
		local trycount=1
		
		while true; do		
			local my_interface=""
			
			for intf in $bridge_interface; do
				local eth=`/bin/echo $intf | /bin/grep -v eth`
				
				if [ "$eth" != "" ]; then
					my_interface="$eth"
				fi
			done
			
			if [ "$my_interface" = "" ]; then
				break
			fi
			
			local has_interface=`/sbin/ifconfig $my_interface >/dev/null 2>&1 && echo 1`
					
			if [ "$has_interface" = "" ]; then
				/bin/sleep 1
		
				trycount=$(($trycount+1))
				
				if [ $trycount -gt $tries ]; then
					break
				fi
			else
				break
			fi
		done
	
		if [ "$has_interface" = "1" -a "$bridge_interface" != "" ]; then
			for iface in $bridge_interface; do
				local eth=`/bin/echo $iface | /bin/grep eth`
				
				if [ "$eth" != "" ]; then
					local vlan=`/bin/echo $iface | /usr/bin/cut -d . -f2`
									
					if [ "$vlan" != "" -a "$vlan" != "$eth" ]; then
						/usr/bin/logger -t switch.sh "Found interface $my_interface for alias $iface in bridge $name - adding to VLAN $vlan"
						/usr/sbin/bridge vlan add dev $my_interface vid $vlan pvid untagged
					fi
				fi
			done
		fi
	done
}
		
setup_switch_dev() {
        local name
        config_get name "$1" name
        name="${name:-$1}"
        [ -d "/sys/class/net/$name" ] && ip link set dev "$name" up
        swconfig dev "$name" load network
}

setup_switch() {
        config_load network
        
        if [ -f "/etc/.lamobo-r1.dsa" ]; then
        	setup_switch_dsa & return
        else
	        config_foreach setup_switch_dev switch
	fi
}
