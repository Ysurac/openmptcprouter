#!/bin/bash
# OpenMPTCProuter Centralized Logging Library
# Provides intelligent, non-annoying logging and diagnostics
# Copyright 2025 OpenMPTCProuter Optimized
# Licensed under GPL-3.0

# Log levels (syslog standard)
readonly LOG_EMERG=0    # System is unusable
readonly LOG_ALERT=1    # Action must be taken immediately
readonly LOG_CRIT=2     # Critical conditions
readonly LOG_ERR=3      # Error conditions
readonly LOG_WARNING=4  # Warning conditions
readonly LOG_NOTICE=5   # Normal but significant
readonly LOG_INFO=6     # Informational
readonly LOG_DEBUG=7    # Debug-level messages

# Default log level (can be overridden by /etc/config/omr)
OMR_LOG_LEVEL=${OMR_LOG_LEVEL:-$LOG_INFO}

# Component name (should be set by calling script)
OMR_COMPONENT="${OMR_COMPONENT:-omr}"

# Log file for persistent storage (optional)
OMR_LOG_FILE="/var/log/omr.log"

# Maximum log file size (in KB)
readonly LOG_MAX_SIZE=1024

# Rate limiting to prevent log spam
declare -A _omr_log_last_time
declare -A _omr_log_count
# Configurable rate limit (default 5 msgs/min, override via OMR_LOG_RATE_LIMIT env var)
# Set to 0 to disable rate limiting
readonly LOG_RATE_LIMIT=${OMR_LOG_RATE_LIMIT:-5}

# Core logging function
omr_log() {
	local level=$1
	local message=$2
	local tag="${3:-$OMR_COMPONENT}"

	# Skip if below log level
	[ "$level" -gt "$OMR_LOG_LEVEL" ] && return 0

	# Rate limiting check (bypass for CRITICAL/ERROR/ALERT/EMERG or if disabled)
	# Critical messages should never be rate limited
	if [ "$LOG_RATE_LIMIT" -gt 0 ] && [ "$level" -gt "$LOG_ERR" ]; then
		local msg_hash=$(echo "$message" | md5sum | cut -d' ' -f1)
		local current_time=$(date +%s)
		local last_time=${_omr_log_last_time[$msg_hash]:-0}
		local time_diff=$((current_time - last_time))

		if [ "$time_diff" -lt 60 ]; then
			_omr_log_count[$msg_hash]=$((${_omr_log_count[$msg_hash]:-0} + 1))
			if [ "${_omr_log_count[$msg_hash]}" -gt "$LOG_RATE_LIMIT" ]; then
				return 0  # Rate limited, skip
			fi
		else
			_omr_log_count[$msg_hash]=1
			_omr_log_last_time[$msg_hash]=$current_time
		fi
	fi

	# Convert level to priority name
	local priority
	case $level in
		$LOG_EMERG)   priority="emerg" ;;
		$LOG_ALERT)   priority="alert" ;;
		$LOG_CRIT)    priority="crit" ;;
		$LOG_ERR)     priority="err" ;;
		$LOG_WARNING) priority="warning" ;;
		$LOG_NOTICE)  priority="notice" ;;
		$LOG_INFO)    priority="info" ;;
		$LOG_DEBUG)   priority="debug" ;;
		*)            priority="info" ;;
	esac

	# Log to syslog
	logger -t "$tag" -p "user.$priority" "$message"

	# Optionally log to file for persistence (only for important messages)
	if [ "$level" -le "$LOG_WARNING" ]; then
		_omr_log_to_file "$tag" "$priority" "$message"
	fi
}

# Write to persistent log file with rotation
_omr_log_to_file() {
	local tag=$1
	local priority=$2
	local message=$3

	# Create log directory if it doesn't exist
	mkdir -p "$(dirname "$OMR_LOG_FILE")"

	# Rotate if file is too large
	if [ -f "$OMR_LOG_FILE" ]; then
		local size=$(du -k "$OMR_LOG_FILE" | cut -f1)
		if [ "$size" -gt "$LOG_MAX_SIZE" ]; then
			mv "$OMR_LOG_FILE" "${OMR_LOG_FILE}.old"
			touch "$OMR_LOG_FILE"
		fi
	fi

	# Write log entry with timestamp
	echo "$(date '+%Y-%m-%d %H:%M:%S') [$priority] [$tag] $message" >> "$OMR_LOG_FILE"
}

# Convenience wrappers
omr_log_debug() {
	omr_log "$LOG_DEBUG" "$1" "${2:-$OMR_COMPONENT}"
}

omr_log_info() {
	omr_log "$LOG_INFO" "$1" "${2:-$OMR_COMPONENT}"
}

omr_log_notice() {
	omr_log "$LOG_NOTICE" "$1" "${2:-$OMR_COMPONENT}"
}

omr_log_warning() {
	omr_log "$LOG_WARNING" "$1" "${2:-$OMR_COMPONENT}"
}

omr_log_error() {
	omr_log "$LOG_ERR" "$1" "${2:-$OMR_COMPONENT}"
}

omr_log_critical() {
	omr_log "$LOG_CRIT" "$1" "${2:-$OMR_COMPONENT}"
}

# Smart diagnostic function - only logs when something is wrong
omr_check_and_log() {
	local check_name=$1
	local check_command=$2
	local success_msg=$3
	local failure_msg=$4

	if eval "$check_command" >/dev/null 2>&1; then
		# Only log success at debug level
		[ -n "$success_msg" ] && omr_log_debug "$check_name: $success_msg"
		return 0
	else
		# Log failure at warning level
		omr_log_warning "$check_name: $failure_msg"
		return 1
	fi
}

# Export functions for use by other scripts
export -f omr_log
export -f omr_log_debug
export -f omr_log_info
export -f omr_log_notice
export -f omr_log_warning
export -f omr_log_error
export -f omr_log_critical
export -f omr_check_and_log
