#!/bin/sh

. /lib/functions.sh
. /usr/share/libubox/jshn.sh

CFG_PATH="/etc/config/"

get_section_name() {
	local ___var="$1"
	[ "$#" -ge 1 ] && shift
	local ___type="$1"
	[ "$#" -ge 1 ] && shift
	local section cfgtype

	[ -z "$CONFIG_SECTIONS" ] && return 0
	for section in ${CONFIG_SECTIONS}; do
		config_get cfgtype "$section" TYPE
		[ -n "$___type" -a "x$cfgtype" != "x$___type" ] && continue
		eval export "${___var}=\${section}"
		return 0
	done
}

_set_option() {
	local option="$1"
	local value="$2"

	uci_set "$_NEW_CONFIG" "$_NEW_SEC_NAME" "$option" "$value"
}

_set_list_option() {
	local option="$1"
	local value="$2"

	for element in $value; do
		uci_add_list "$_NEW_CONFIG" "$_NEW_SEC_NAME" "$option" "$element"
	done
}

_del_uci_element() {
	local section="$1"
	local option="$2"

	uci_remove "$_OLD_CONFIG" "$section" "$option"
}
_option_cond_cb() {
	local value=$3

	json_select $2
	json_get_var old 1
	json_get_var new 2

	[ "$old" = "$value" ] && _COND_VALUE="$new"

	json_select ..
}

_parse_condition(){
	local value="$1"

	_COND_VALUE=
	json_for_each_item _option_cond_cb "if" "$value"
}

_option_rule_cb(){
	local rule="$1"
	local option="$2"
	local value

	[ -n "$rule" ] || return 0

	json_select "$option"
	json_get_vars new_name "if" default cb type

	if [ -n "$cb" ]; then
		eval "$cb \"\$option\" \"\$_OLD_SEC_NAME\" \"\$_NEW_SEC_NAME\""
		[ "$?" -eq 0 ] && {
			json_select ..
			return 0
		}

		value="$_OPTION_VALUE"
	else
		config_get value $_OLD_SEC_NAME "$option" "$default"
	fi

	[ -n "$if" ] && {
		_parse_condition "$value"
		value="${_COND_VALUE:-${value:-$default}}"
	}

	if [ -n "$type" -a "$type" = "list" ]; then
		_set_list_option "${new_name:-$option}" "$value"
	else
		_set_option "${new_name:-$option}" "$value"
	fi

	json_select ..
}

_init_section() {
	local sec_t

	json_get_vars old_name new_name new_type old_type
	[ -n "$old_name" -o -n "$old_type" ] || return 1

	if [ -z "$old_name" ]; then
		get_section_name _OLD_SEC_NAME "$old_type"
	else
		_OLD_SEC_NAME=$old_name
	fi

	_NEW_SEC_NAME=$new_name
	_OLD_SEC_TYPE=$old_type
	_NEW_SEC_TYPE=${new_type:-$old_type}
	[ -n "$_NEW_SEC_TYPE" ] || \
		eval "_NEW_SEC_TYPE=\$CONFIG_${_OLD_SEC_NAME}_TYPE"

	if [ -n "$_NEW_SEC_NAME" ]; then
		uci set "$_NEW_CONFIG"."$_NEW_SEC_NAME"="$_NEW_SEC_TYPE"
	else
		_NEW_SEC_NAME="$(uci -q add "$_NEW_CONFIG" "$_NEW_SEC_TYPE")"
	fi

	[ -n "$_NEW_SEC_NAME" ] || return 1
}

_section_rule_cb(){
	local rule="$1"
	local section="$2"
	local value

	[ -n "$rule" ] || return 0

	json_select "$section"
	json_get_vars cb old_name new_name new_type old_type remove
	[ -n "$cb" ] && {
		eval "$cb \"\$old_name\" \"\$new_name\" \"\$old_type\" \"\$new_type\""
		[ "$?" -eq 0 ] && {
			json_select ..
			return 0
		}
	}

	_init_section
	[ $? -ne 0 ] && {
		logger -t "Migration" "Unable to init section"
		json_select ..

		return 1
	}

	json_for_each_item _option_rule_cb options
	json_select ..
	[ -n "$remove" ] && {
		_del_uci_element "$old_name"
		uci_commit "$_OLD_CONFIG"
	}
}

_init_config() {
	json_select config
	json_get_vars old_name new_name

	[ -n "$old_name" ] || return 1
	[ -f "$CFG_PATH$old_name" ] || return 1
	[ -f "$CFG_PATH$new_name" ] || touch $CFG_PATH$new_name

	config_load "$old_name"
	_NEW_CONFIG="${new_name:-$old_name}"
	_OLD_CONFIG="$old_name"

	json_select ..
}

migrate() {
	local remove
	local json_file="$1"

	[ -f "$json_file" ] || return 0

	json_init
	json_load_file "$json_file"
	json_select
	_init_config
	[ $? -ne 0 ] && {
		logger -t "Migration" "Unable to load config"
		return 1
	}

	json_for_each_item _section_rule_cb sections
	uci_commit "$_NEW_CONFIG"

	json_get_vars remove
	[ -n "$remove" ] && rm "$CFG_PATH$_OLD_CONFIG"
}
