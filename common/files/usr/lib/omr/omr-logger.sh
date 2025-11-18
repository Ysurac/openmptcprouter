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
readonly LOG_RATE_LIMIT=5  # Max 5 identical messages per minute
readonly LOG_RATE_DIR="/var/run/omr-log-rate"

# Core logging function
omr_log() {
	local level=$1
	local message=$2
	local tag="${3:-$OMR_COMPONENT}"

	# Skip if below log level
	[ "$level" -gt "$OMR_LOG_LEVEL" ] && return 0

	# Rate limiting check (file-based for POSIX sh compatibility)
	local msg_hash=$(echo "$message" | md5sum | cut -d' ' -f1 | head -c 8)
	local current_time=$(date +%s)

	# Create rate limit directory if needed
	mkdir -p "$LOG_RATE_DIR" 2>/dev/null

	local rate_file="$LOG_RATE_DIR/$msg_hash"
	local last_time=0
	local count=0

	# Read existing rate limit data
	if [ -f "$rate_file" ]; then
		read last_time count < "$rate_file" 2>/dev/null || true
	fi

	local time_diff=$((current_time - last_time))

	if [ "$time_diff" -lt 60 ]; then
		count=$((count + 1))
		if [ "$count" -gt "$LOG_RATE_LIMIT" ]; then
			return 0  # Rate limited, skip
		fi
	else
		count=1
		last_time=$current_time
	fi

	# Update rate limit data atomically
	echo "$last_time $count" > "$rate_file.tmp" && mv "$rate_file.tmp" "$rate_file"

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

# Export functions for use by other scripts
export -f omr_log
export -f omr_log_debug
export -f omr_log_info
export -f omr_log_notice
export -f omr_log_warning
export -f omr_log_error
export -f omr_log_critical
