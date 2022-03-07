#Mobile configuration management lib

. /usr/share/libubox/jshn.sh
. /lib/functions.sh

gsm_soft_reset() {
	gsmctl -n -A at+cfun=4
	sleep 2
	gsmctl -n -A at+cfun=1
}

qmi_error_handle() {
	local error="$1"
	local modem_id="$2"

	$(echo "$error" | grep -q "error") && {
		echo "$error"
	}

	$(echo "$error" | grep -q "Client IDs exhausted") && {
			echo "ClientIdsExhausted! reseting counter..."
			proto_notify_error "$interface" NO_CID
			uqmi -s -d "$device" --sync
			return 1
	}

#	Reik papildyt ERROR handlinima
#	$(echo "$error" | grep -q "multiple-connection-to-same-pdn-not-allowed") && {
#			echo "Reseting due dublicated connection..."
#			qmicli -p -d "$device" --uim-sim-power-off=1
#			qmicli -p -d "$device" --uim-sim-power-on=1
#			return 1
#	}

#	$(echo "$error" | grep -q "Transaction timed out") && {
#			echo "Device not responding, restarting module"
#			gsmctl -O $modem_id -A at+cfun=1,1
#	}
#
#	$(echo "$error" | grep -q 'verbose call end reason (2,236)') && {
   #                     echo "Failed to start network, clearing all cids"
  #                      qmicli -p -d "$device" --wds-noop --device-open-sync
 #                       return 1
#        }

	$(echo "$error" | grep -q "Call Failed") && {
		echo "Device not responding, restarting module"
		sleep 10
		gsm_soft_reset
		return 1
	}

	$(echo "$error" | grep -q "Policy Mismatch") && {
		echo "Reseting network..."
		gsm_soft_reset
		return 1
	}

	$(echo "$error" | grep -q "Failed to connect to service") && {
		echo "Device not responding, restarting module"
		gsmctl -A at+cfun=1,1
		return 1
	}

	$(echo "$error" | grep -q "error") && {
		echo "$error"
	}

	return 0
}

passthrough_mode=
get_passthrough() {
	config_get primary "$1" primary
	[ "$primary" = "1" ] && {
		config_get sim "$1" position;
		passthrough_mode=$(uci -q get network.mob1s${sim}a1.passthrough_mode 2>/dev/null);
	}
}

setup_bridge_v4() {
	local dev="$1"
	local dhcp_param_file="/tmp/dnsmasq.d/bridge"
	echo "$parameters4"

	json_load "$parameters4"
	json_select "ipv4"
	json_get_var bridge_ipaddr ip
	json_get_var bridge_mask subnet
	json_get_var bridge_gateway gateway
	json_get_var bridge_dns1 dns1
	json_get_var bridge_dns2 dns2

	json_init
	json_add_string name "${interface}_4"
	json_add_string ifname "$dev"
	json_add_string proto "none"
	json_add_object "data"
	ubus call network add_dynamic "$(json_dump)"
	IFACE4="${interface}_4"

	json_init
	json_add_string interface "${interface}_4"
	json_add_string zone "lan"
	ubus call network.interface set_data "$(json_dump)"

	json_init
	json_add_string interface "${interface}"
	json_add_string bridge_ipaddr "$bridge_ipaddr"
	ubus call network.interface set_data "$(json_dump)"

	json_init
	json_add_string modem "$modem"
	json_add_string sim "$sim"
	ubus call network.interface."${interface}_4" set_data "$(json_dump)"
	json_close_object

	ip route add default dev "$dev" table 42
	ip route add default dev br-lan table 43
	ip route add "$bridge_ipaddr" dev br-lan

	ip rule add pref 5042 from "$bridge_ipaddr" lookup 42
	ip rule add pref 5043 iif "$dev" lookup 43
	#sysctl -w net.ipv4.conf.br-lan.proxy_arp=1 #2>/dev/null
	ip neighbor add proxy "$bridge_gateway" dev br-lan

        iptables -A postrouting_rule -m comment --comment "Bridge mode" -o "$dev" -j ACCEPT -tnat

	config_load simcard
	config_foreach get_passthrough sim

	> $dhcp_param_file
	[ -z "$mac" ] && mac="*:*:*:*:*:*"
	[ "$passthrough_mode" != "no_dhcp" ] && {
		echo "dhcp-range=tag:mobbridge,$bridge_ipaddr,static,$bridge_mask,${leasetime:-1h}" > "$dhcp_param_file"
		echo "shared-network=br-lan,$bridge_ipaddr" >> "$dhcp_param_file"
		echo "dhcp-host=$mac,set:mobbridge,$bridge_ipaddr" >> "$dhcp_param_file"
		echo "dhcp-option=tag:mobbridge,br-lan,3,$bridge_gateway" >> "$dhcp_param_file"
		echo "dhcp-option=tag:mobbridge,br-lan,6,$bridge_dns1,$bridge_dns2" >> "$dhcp_param_file"
		echo "server=$bridge_dns1" >> "$dhcp_param_file"
		echo "server=$bridge_dns2" >> "$dhcp_param_file"
	}
	/etc/init.d/dnsmasq reload
	swconfig dev 'switch0' set soft_reset 5 &
}

setup_static_v4() {
	local dev="$1"
	echo "Setting up $dev V4 static"
	echo "$parameters4"

	json_load "$parameters4"
	json_select "ipv4"
	json_get_var ip_4 ip
	json_get_var dns1_4 dns1
	json_get_var dns2_4 dns2

	json_init
	json_add_string name "${interface}_4"
	json_add_string ifname "$dev"
	json_add_string proto static
	json_add_string gateway "0.0.0.0"

	json_add_array ipaddr
		json_add_string "" "$ip_4"
	json_close_array

	json_add_array dns
		[ -n "$dns1_4" ] && json_add_string "" "$dns1_4"
		[ -n "$dns2_4" ] && json_add_string "" "$dns2_4"
	json_close_array

	[ -n "$ip4table" ] && json_add_string ip4table "$ip4table"
	proto_add_dynamic_defaults

	ubus call network add_dynamic "$(json_dump)"
}

setup_dhcp_v4() {
	local dev="$1"
	echo "Setting up $dev V4 DCHP"
	json_init
	json_add_string name "${interface}_4"
	json_add_string ifname "$dev"
	json_add_string proto "dhcp"
	json_add_string script "/lib/netifd/dhcp_mobile.script"
	[ -n "$ip4table" ] && json_add_string ip4table "$ip4table"
	proto_add_dynamic_defaults
	[ -n "$zone" ] && json_add_string zone "$zone"
	ubus call network add_dynamic "$(json_dump)"
}

setup_dhcp_v6() {
	local dev="$1"
	echo "Setting up $dev V6 DHCP"
	json_init
	json_add_string name "${interface}_6"
	json_add_string ifname "$dev"
	json_add_string proto "dhcpv6"
	[ -n "$ip6table" ] && json_add_string ip6table "$ip6table"
	json_add_boolean ignore_valid 1
	proto_add_dynamic_defaults
	# RFC 7278: Extend an IPv6 /64 Prefix to LAN
	json_add_string extendprefix 1
	[ -n "$zone" ] && json_add_string zone "$zone"
	ubus call network add_dynamic "$(json_dump)"
}

setup_static_v6() {
	local dev="$1"
	echo "Setting up $dev V6 static"
	echo "$parameters6"

	json_load "$parameters6"
	json_select "ipv6"
	json_get_var ip6_with_prefix ip
	ip_6="${ip6_with_prefix%/*}"
	ip_prefix_length="${ip6_with_prefix#*/}"
	json_get_var ip6_gateway_with_prefix gateway
	gateway_6="${ip6_gateway_with_prefix%/*}"
	json_get_var dns1_6 dns1
	json_get_var dns2_6 dns2

	json_init
	json_add_string name "${interface}_6"
	json_add_string ifname "$dev"
	json_add_string proto static
	json_add_string ip6gw "$gateway_6"

	json_add_array ip6prefix
	json_add_string "" "$ip6_with_prefix"
	json_close_array

	json_add_array ip6addr
		json_add_string "" "$ip6_with_prefix"
	json_close_array

	json_add_array dns
		[ -n "$dns1_6" ] && json_add_string "" "$dns1_6"
		[ -n "$dns2_6" ] && json_add_string "" "$dns2_6"
	json_close_array

	[ -n "$ip6table" ] && json_add_string ip6table "$ip6table"
	proto_add_dynamic_defaults

	ubus call network add_dynamic "$(json_dump)"
}
