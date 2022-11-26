#!/bin/ash

. /lib/functions.sh
. /usr/share/libubox/jshn.sh

ucidef_add_nand_info() {
	local model="$1"

	model=${model:0:7}

	json_select_object nand

	case "$model" in
	TRB1412)
		json_add_int blocksize 128
		json_add_int pagesize 2048
		json_add_int subpagesize 2048
	;;
	TRB1422 |\
	TRB1423 |\
	TRB1452)
		json_add_int blocksize 256
		json_add_int pagesize 4096
		json_add_int subpagesize 4096
	;;
	TRB14*0)
		json_add_int blocksize 128
		json_add_int pagesize 2048
		json_add_int subpagesize 2048
	;;
	TRB14*1)
		json_add_int blocksize 256
		json_add_int pagesize 4096
		json_add_int subpagesize 4096
	;;
	esac

	json_select ..
}

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

ucidef_add_trb14x_lte_modem() {
	print_array() {
				json_add_array $1
				for element in $2
				do
						json_add_string "" "$(echo $element)"
				done
				json_close_array
		}
	#Parameters: model primary
	local model vendor product boudrate gps type desc control region modem_counter
	modem_counter=1
	json_select_array "modems"

	model="$1"

	model=${model:0:7}

	case "$model" in
		TRB1422)
				vendor=05c6
				product=9215
		;;
		TRB1412 |\
		TRB1423 |\
		TRB1452 |\
		TRB140*)
				vendor=2c7c
				product=0125
		;;
		TRB14*)
				vendor=2c7c
				product=0121
		;;
		esac

	case "$model" in
		TRB1412 |\
		TRB14*0)
				region="EU"
				[ "$product" = "0121" ] && {
					lte_bands="1 3 7 8 20 28"
					trysg_bands="wcdma_2100 wcdma_900"
					dug_bands="gsm_1800 gsm_900"
				}
				[ "$product" = "0125" ] && {
					lte_bands="1 3 7 8 20 28 38 40 41"
					trysg_bands="wcdma_2100 wcdma_900"
					dug_bands="gsm_1800 gsm_900"
				}
		;;
		TRB1422)
				region="CE"
				lte_bands="1 3 5 8 34 38 39 40 41"
				trysg_bands="bc-0-a-system bc-0-b-system wcdma_2100 wcdma_900"
				dug_bands="gsm_1800 gsm_900"
		;;
		TRB1423 |\
		TRB1452 |\
		TRB14*1)
				region="AU"
				[ "$product" = "0121" ] && {
					lte_bands="1 2 3 4 5 7 8 28 40"
					trysg_bands="wcdma_2100 wcdma_1900 wcdma_900 wcdma_850"
					dug_bands="gsm_1800 gsm_900 gsm_850 gsm_1900"
				}
				[ "$product" = "0125" ] && {
					lte_bands="1 2 3 4 5 7 8 28 40"
					trysg_bands="wcdma_2100 wcdma_1900 wcdma_900 wcdma_850"
					dug_bands="gsm_1800 gsm_900 gsm_850 gsm_1900"
				}
		;;
		esac

	[ -f "/lib/network/wwan/$vendor:$product" ] && {
		devicename="3-1"
		json_set_namespace defaults old_cb
		json_load "$(cat /lib/network/wwan/$vendor:$product)"
		json_get_vars gps boudrate type desc control stop_bits
		json_set_namespace $old_cb

		[ "${devicename%%:*}" = "$devicename" ] && {
			json_add_object
				json_add_string id "$devicename"
				json_add_string num "$modem_counter"
				json_add_string vendor "$vendor"
				json_add_string product "$product"
				json_add_string stop_bits "$stop_bits"
				json_add_string gps "$gps"
				json_add_string boudrate "$boudrate"
				json_add_string type "$type"
				json_add_string desc "$desc"
				json_add_string region "$region"
				json_add_string control "$control"
				json_add_int simcount 1
				json_add_boolean builtin 1
				[ -n "$2" ] && json_add_boolean primary 1
				json_add_object service_modes
								[ -z "$lte_bands" ] || print_array "4G" "$lte_bands"
								[ -z "$trysg_bands" ] || print_array "3G" "$trysg_bands"
								[ -z "$dug_bands" ] || print_array "2G" "$dug_bands"
				json_close_object
			json_close_object
		}
	}
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
		json_close_object
	json_select ..
}

ucidef_set_hwinfo() {
	local function
	local dual_sim=0
	local wifi=0
	local dual_band_ssid=0
	local wps=0
	local mobile=0
	local gps=0
	local usb=0
	local bluetooth=0
	local ethernet=0
	local sfp_port=0
	local ios=0

	for function in "$@"; do
		case "$function" in
			dual_sim)
				dual_sim=1
			;;
			wifi)
				wifi=1
			;;
			dual_band_ssid)
				dual_band_ssid=1
			;;
			wps)
				wps=1
			;;
			mobile)
				mobile=1
			;;
			gps)
				gps=1
			;;
			usb)
				usb=1
			;;
			bluetooth)
				bluetooth=1
			;;
			ethernet)
				ethernet=1
			;;
			sfp_port)
				sfp_port=1
			;;
			ios)
				ios=1
			;;
			at_sim)
				at_sim=1
			;;
		esac
	done

	json_select_object hwinfo
		json_add_boolean dual_sim "$dual_sim"
		json_add_boolean usb "$usb"
		json_add_boolean bluetooth "$bluetooth"
		json_add_boolean wifi "$wifi"
		json_add_boolean dual_band_ssid "$dual_band_ssid"
		json_add_boolean wps "$wps"
		json_add_boolean mobile "$mobile"
		json_add_boolean gps "$gps"
		json_add_boolean ethernet "$ethernet"
		json_add_boolean sfp_port "$sfp_port"
		json_add_boolean ios "$ios"
		json_add_boolean at_sim "$at_sim"
	json_select ..
}

ucidef_set_release_version() {
	json_add_string release_version "$1"
}
