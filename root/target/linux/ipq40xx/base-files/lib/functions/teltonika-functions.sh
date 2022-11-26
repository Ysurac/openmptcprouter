#!/bin/sh

. /usr/share/libubox/jshn.sh

is_ios_enabled() {
	local ios
	json_load_file "/etc/board.json" && \
	json_select hwinfo && \
	json_get_var ios ios && [ "$ios" = "1" ]
}
