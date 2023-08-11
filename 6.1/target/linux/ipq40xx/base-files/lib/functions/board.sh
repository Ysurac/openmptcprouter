
. /usr/share/libubox/jshn.sh

# when device contains 2 internal modems, this function will return '2' if
#  selected modem(inc_id) is builtin and primary.
# And if it's only builtin, then '1'
is_builtin_modem() {
	local inc_id="$1"
	local modem modems id builtin primary

	json_init
	json_load_file "/etc/board.json"

	json_get_keys modems modems
	json_select modems

	for modem in $modems; do
		json_select "$modem"
		json_get_vars id builtin primary

		[ "$id" = "$inc_id" ] && {
			[ -n "$builtin" ] && {
				[ -n "$primary" ] && {
					echo 2
					return
				}

				echo 1
				return
			}

			echo 0
			return
		}

		json_select ..
	done

	echo 0
}
