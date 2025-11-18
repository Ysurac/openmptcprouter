#!/bin/ash

. /lib/functions.sh
. /usr/share/libubox/jshn.sh

ucidef_add_static_modem_info() {
	#Parameters: model usb_id sim_count other_params
	local model usb_id count
	local modem_counter=0
	local sim_count=1

	model="$1"
	usb_id="$2"

	[ -n "$3" ] && sim_count="$3"

	json_get_keys count modems
	[ -n "$count" ] && modem_counter="$(echo "$count" | wc -w)"

	json_select_array "modems"
		json_add_object
			json_add_string id "$usb_id"
			json_add_string num "$((modem_counter + 1))"
			json_add_boolean builtin 1
			json_add_int simcount "$sim_count"

			for i in "$@"; do
				case "$i" in
					primary)
						json_add_boolean primary 1
						;;
					gps_out)
						json_add_boolean gps_out 1
					;;
				esac
			done

		json_close_object
	json_select ..
}

ucidef_add_serial_capabilities() {
	json_select_array serial
		json_add_object
			[ -n "$1" ] && {
				json_select_array devices
				for d in $1; do
					json_add_string "" $d
				done
				json_select ..
			}

			json_select_array bauds
			for b in $2; do
				json_add_string "" $b
			done
			json_select ..

			json_select_array data_bits
			for n in $3; do
				json_add_string "" $n
			done
			json_select ..

			json_select_array flow_control
			for n in $4; do
				json_add_string "" $n
			done
			json_select ..

			json_select_array stop_bits
			for n in $5; do
				json_add_string "" $n
			done
			json_select ..

			json_select_array parity_types
			for n in $6; do
				json_add_string "" $n
			done
			json_select ..

			json_add_string "path" $7

		json_close_object
	json_select ..
}

ucidef_add_wlan_bssid_limit() {
	json_select_object wlan
		json_add_object "$1"
			json_add_int bssid_limit "$2"
		json_close_object
	json_select ..
}

ucidef_set_hwinfo() {
	local args=" $* "
	local options='
	dual_sim
	at_sim
	wifi
	dual_band_ssid
	wps
	mobile
	gps
	usb
	poe
	bluetooth
	ethernet
	sfp_port
	ios
	sfp_switch
	rs232
	rs485
	console
	dual_modem
	sd_card
	sw_rst_on_init
	dsa
	hw_nat
	'

	json_select_object hwinfo

	for opt in $options; do
		if [[ "$args" =~ " $(echo "$opt" | tr -d '\011\012\015\040') " ]]; then
			json_add_boolean "$opt" 1
		else
			json_add_boolean "$opt" 0
		fi
	done

	json_select ..
}

ucidef_set_release_version() {
	json_add_string release_version "$1"
}

ucidef_set_usb_jack() {
	json_add_string "usb_jack" "$1"
}
