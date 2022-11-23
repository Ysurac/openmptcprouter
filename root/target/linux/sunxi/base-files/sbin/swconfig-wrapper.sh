#!/bin/sh
# swconfig wrapper for BPI-R1 switch in DSA enabled environment
# Copyright (c) 2021 Oliver Welter <oliver@welter.rocks>

SWCONFIG="/sbin/swconfig"
SWCONFIG_REAL="$SWCONFIG.real"
SWCONFIG_DSA="$SWCONFIG.dsa"
SWCONFIG_WRAPPER="$SWCONFIG-wrapper.sh"
SWCONFIG_LINK=`readlink $SWCONFIG`

DSA_MAP="/etc/dsa.map"
DSA_MODE=0

UCI="/sbin/uci"
GREP="/bin/grep"
CUT="/usr/bin/cut"
AWK="/usr/bin/awk"
IP="/sbin/ip"

SWITCHNULL="switch0"

[ -f "$DSA_MAP" ] && DSA_MODE=1

if [ "$1" = "setup-wrapper" ]; then
	if [ "$SWCONFIG_LINK" = "$SWCONFIG_WRAPPER" ]; then
		echo "Already linked to wrapper" >/dev/stderr
		exit 1
	elif [ -x "$SWCONFIG" ]; then		
		mv $SWCONFIG $SWCONFIG_REAL && \
		ln -sf $SWCONFIG_WRAPPER $SWCONFIG && \
		exit 0 || exit $?
	else
		echo "Unable to find swconfig binary" >/dev/stderr
		exit 2
	fi
elif [ "$DSA_MODE" = 0 ]; then
	if [ "$1" = "" ]; then
		$SWCONFIG_REAL && exit 0 || exit $?
	elif [ "$2" = "" ]; then
		$SWCONFIG_REAL $1 && exit 0 || exit $?
	elif [ "$3" = "" ]; then
		$SWCONFIG_REAL $1 $2 && exit 0 || exit $?
	elif [ "$4" = "" ]; then
		$SWCONFIG_REAL $1 $2 $3 && exit 0 || exit $?
	elif [ "$5" = "" ]; then
		$SWCONFIG_REAL $1 $2 $3 $4 && exit 0 || exit $?
	elif [ "$6" = "" ]; then
		$SWCONFIG_REAL $1 $2 $3 $4 $5 && exit 0 || exit $?
	elif [ "$7" = "" ]; then
		$SWCONFIG_REAL $1 $2 $3 $4 $5 $6 && exit 0 || exit $?
	else
		exit 255
	fi
fi

. $DSA_MAP

 get_interface_by_portlist() {
	local ports="$1"
	
	for port in $ports; do
		port_id=`echo $port | $CUT -d "t" -f1`
		port_tagged=`echo $port | $GREP "t" >/dev/null 2>&1 && echo 1 || echo 0`
		interface=`eval echo "\${port_$port_id}"`
		name=`eval echo "\${port_name_$port_id}"`
		
		echo "$port_id:$port_tagged:$interface:$name"
	done	
}

swconfig_usage() {
	echo "WARNING: swconfig runs in DSA wrapper mode"
	$SWCONFIG_REAL && exit 0 || exit $?
}

swconfig_port_get() {
	local port="$1"
	local key="$2"
	
	return 0
}

swconfig_vlan_get() {
	local vlan="$1"
	local key="$2"
	
	return 0
}

swconfig_get() {
	local key="$1"
	
	case $key in
		reset|reset_mib|apply)
			# This is ignored, but leads to exit code 0 to not confuse the networking scripts
			return 0
		;;
		*)
			echo "Unknown key $key for device" >/dev/stderr
			return 1
		;;
	esac
	
	return 0
}

swconfig_port_set() {
	local port="$1"
	local key="$2"
	local val="$3"

	case $key in
		*)
			echo "Unknown key $key for port" >/dev/stderr
			return 1
		;;
	esac
	
	return 0
}

swconfig_vlan_set() {
	local vlan="$1"
	local key="$2"
	local val="$3"

	case $key in
		*)
			echo "Unknown key $key for vlan" >/dev/stderr
			return 1
		;;
	esac
	
	return 0
}

swconfig_set() {
	local key="$1"
	local val="$2"

	case $key in
		reset|reset_mib|apply)
			# This is ignored, but leads to exit code 0 to not confuse the networking scripts
			return 0
		;;
		*)
			echo "Unknown key $key for device" >/dev/stderr
			return 1
		;;
	esac
	
	return 0
}

swconfig_port_load() {
	local port="$1"
	local config="$2"
	
	return 0
}

swconfig_vlan_load() {
	local vlan="$1"
	local config="$2"
	
	return 0
}

swconfig_load() {
	local config="$1"
	
	# This is the part, where the magic happens.
	# Due to its structure, swconfig gets the configuration to use by itself.
	# At this point, we use uci to fetch the configuration for the vlans to setup.
	
	[ "$config" != "network" ] && return 1
	
	# Set the CPU port
	local CPUPORT=`eval echo "\${port_$port_cpu}"`
	
	# Bring up the CPU port
	$IP link set $CPUPORT up
	
	for section in `$UCI show $config | $GREP "=switch_vlan" | $CUT -d "=" -f1`; do
		section_id=`$UCI show $section | $GREP "=switch_vlan" | $CUT -d "=" -f1 | $CUT -d "." -f2`
		
		vlan=`$UCI show $config.$section_id.vlan | $CUT -d "=" -f2 | $CUT -d "'" -f2`
		ports=`$UCI show $config.$section_id.ports | $CUT -d "=" -f2 | $CUT -d "'" -f2`
		device=`$UCI show $config.$section_id.device | $CUT -d "=" -f2 | $CUT -d "'" -f2`
		
		[ "$device" != "$SWITCHNULL" ] && continue
		
		for iface in `get_interface_by_portlist $ports`; do
			port_id=`echo $iface | $CUT -d ":" -f1`
			
			# We just want the CPU ports here
			[ "$port_id" != "$port_cpu" ] && continue
			
			port_tagged=`echo $iface | $CUT -d ":" -f2`
			interface=`echo $iface | $CUT -d ":" -f3`
			name=`echo $iface | $CUT -d ":" -f4`
			
			# At this point, we have all we need.
			if [ "$port_tagged" = 1 ]; then
				# Tag the traffic on CPU port as master interface
				$IP link add link $interface name $interface.$vlan type vlan id $vlan
			
				# Bring up the master interface before the slaves
				$IP link set $interface.$vlan up
			fi
		done

		for iface in `get_interface_by_portlist $ports`; do
			port_id=`echo $iface | $CUT -d ":" -f1`
			
			# We just want the slave ports here
			[ "$port_id" = "$port_cpu" ] && continue
			
			port_tagged=`echo $iface | $CUT -d ":" -f2`
			interface=`echo $iface | $CUT -d ":" -f3`
			name=`echo $iface | $CUT -d ":" -f4`
			
			if [ "$port_tagged" = 1 ]; then
				interface="$interface.$vlan"
			fi
			
			# Bring up the slave interface
			$IP link set $interface up

			# Create the bridge
			$IP link add name $name type bridge
			
			# Set VLAN filtering and PVID
			$IP link set dev $name type bridge vlan_filtering 1 vlan_default_pvid $vlan
		done

		for iface in `get_interface_by_portlist $ports`; do
			port_id=`echo $iface | $CUT -d ":" -f1`			
			port_tagged=`echo $iface | $CUT -d ":" -f2`
			interface=`echo $iface | $CUT -d ":" -f3`
			name=`echo $iface | $CUT -d ":" -f4`
			
			if [ "$port_tagged" = 1 ]; then
				interface="$interface.$vlan"
			fi
			
			# Add port to its corresponding bridge
			$IP link set dev $interface master $name
		done
	done

	return 0
}

swconfig_port_show() {
	local port="$1"
	
	return 0
}

swconfig_vlan_show() {
	local vlan="$1"
	
	return 0
}

swconfig_show() {
	return 0
}

case $1 in
	dev)
		device="$2"
		mode="$3"
		op="$5"
		
		key="$6"
		val="$7"
		
		port=""
		vlan=""
		
		case $3 in
			port)
				port="$4"
			;;
			vlan)
				vlan="$4"
			;;
			*)
				mode="switch"
				op="$3"
				key="$4"
				val="$5"
			;;
		esac
		
		case $op in
			help)
				$SWCONFIG_REAL $1 $2 $3 $4 && exit 0 || exit $?
			;;
			set)
				if [ "$mode" = "port" ]; then
					swconfig_port_set $port $key $val && exit 0 || exit $?
				elif [ "$mode" = "vlan" ]; then
					swconfig_vlan_set $vlan $key $val && exit 0 || exit $?
				else
					swconfig_set $key $val && exit 0 || exit $?
				fi
			;;
			get)
				if [ "$mode" = "port" ]; then
					swconfig_port_get $port $key && exit 0 || exit $?
				elif [ "$mode" = "vlan" ]; then
					swconfig_vlan_get $vlan $key && exit 0 || exit $?
				else
					swconfig_get $key && exit 0 || exit $?
				fi
			;;
			load)
				if [ "$mode" = "port" ]; then
					swconfig_port_load $port $key && exit 0 || exit $?
				elif [ "$mode" = "vlan" ]; then
					swconfig_vlan_load $vlan $key && exit 0 || exit $?
				else
					swconfig_load $key && exit 0 || exit $?
				fi
			;;
			show)
				if [ "$mode" = "port" ]; then
					swconfig_port_show $port && exit 0 || exit $?
				elif [ "$mode" = "vlan" ]; then
					swconfig_vlan_show $vlan && exit 0 || exit $?
				else
					swconfig_show && exit 0 || exit $?
				fi
			;;
			*)
				swconfig_usage
			;;
		esac
	;;
	list)
		echo $SWITCHNULL
		exit 0
	;;
	*)
		swconfig_usage
	;;
esac
