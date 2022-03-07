PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	platform_check_image_ipq "$1"
}

platform_do_upgrade() {
	platform_do_upgrade_ipq "$1"
}

# added with io_expander validation
platform_check_hw_support() {
	local metadata="/tmp/sysupgrade.meta"
	local io_expander_file="/proc/device-tree/io_expander"
	local found=0

	[ -e "$metadata" ] || ( fwtool -q -i $metadata $1 ) && {
		json_load_file "$metadata"
		# previous devices were always supported
		[ ! -e "$io_expander_file" ] && return 0
		json_select hw_support

# io_expander type validation
		local io_expander="$(cat $io_expander_file)"
		# if support type is absent in metadata, we assume it's supported
		if ( json_select io_expander 2> /dev/null ); then
			json_select io_expander
			json_get_values io_exp_values

			for val in $io_exp_values; do
				regex_value=$(echo "$io_expander" | grep -e "$val")
				[ "$io_expander" = "$regex_value" ] && found=1
			done

			[ $found -eq 0 ] && return 1
			json_select ..
		else
		# fail if not default/initial type
			[ "$io_expander" != "stm32" ] && return 1
		fi
# ...
	}
	return 0;
}
