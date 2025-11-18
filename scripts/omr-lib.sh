#!/bin/sh
#
# OpenMPTCProuter Shared Library
# Common functions for scripts to ensure consistent UX
# Copyright (C) 2025 spotty118 - OpenMPTCProuter Optimized
#
# Usage: Source this file in your script:
#   . "$(dirname "$0")/omr-lib.sh" || . /usr/share/omr/omr-lib.sh
#

# Color definitions
if [ -t 1 ]; then
    # Only use colors if stdout is a terminal
    export OMR_RED='\033[0;31m'
    export OMR_GREEN='\033[0;32m'
    export OMR_YELLOW='\033[1;33m'
    export OMR_BLUE='\033[0;34m'
    export OMR_MAGENTA='\033[0;35m'
    export OMR_CYAN='\033[0;36m'
    export OMR_WHITE='\033[1;37m'
    export OMR_BOLD='\033[1m'
    export OMR_DIM='\033[2m'
    export OMR_NC='\033[0m' # No Color
else
    # No colors for non-terminal output (pipes, files, etc.)
    export OMR_RED=''
    export OMR_GREEN=''
    export OMR_YELLOW=''
    export OMR_BLUE=''
    export OMR_MAGENTA=''
    export OMR_CYAN=''
    export OMR_WHITE=''
    export OMR_BOLD=''
    export OMR_DIM=''
    export OMR_NC=''
fi

# Emoji/symbols (with fallbacks for non-UTF8 terminals)
if locale | grep -qi "utf-8\|utf8"; then
    export OMR_CHECK="✓"
    export OMR_CROSS="✗"
    export OMR_ARROW="→"
    export OMR_BULLET="•"
    export OMR_STAR="★"
    export OMR_INFO="ℹ"
    export OMR_WARN="⚠"
else
    export OMR_CHECK="[OK]"
    export OMR_CROSS="[X]"
    export OMR_ARROW="->"
    export OMR_BULLET="*"
    export OMR_STAR="*"
    export OMR_INFO="[i]"
    export OMR_WARN="[!]"
fi

#
# Logging functions
#

omr_log_success() {
    printf -- "${OMR_GREEN}${OMR_CHECK}${OMR_NC} %s\n" "$*"
}

omr_log_error() {
    printf -- "${OMR_RED}${OMR_CROSS}${OMR_NC} %s\n" "$*" >&2
}

omr_log_warning() {
    printf -- "${OMR_YELLOW}${OMR_WARN}${OMR_NC} %s\n" "$*"
}

omr_log_info() {
    printf -- "${OMR_CYAN}${OMR_INFO}${OMR_NC} %s\n" "$*"
}

omr_log_step() {
    printf -- "${OMR_BLUE}${OMR_ARROW}${OMR_NC} %s\n" "$*"
}

omr_log_header() {
    printf -- "\n${OMR_CYAN}${OMR_BOLD}=== %s ===${OMR_NC}\n\n" "$*"
}

omr_log_subheader() {
    printf -- "\n${OMR_WHITE}--- %s ---${OMR_NC}\n" "$*"
}

#
# Progress indicator functions
#

omr_progress_start() {
    local message="$1"
    printf -- "${OMR_BLUE}${OMR_ARROW}${OMR_NC} %s ... " "$message"
}

omr_progress_done() {
    printf -- "${OMR_GREEN}${OMR_CHECK}${OMR_NC}\n"
}

omr_progress_fail() {
    printf -- "${OMR_RED}${OMR_CROSS}${OMR_NC}\n"
}

omr_progress_skip() {
    printf -- "${OMR_YELLOW}SKIP${OMR_NC}\n"
}

#
# Question/prompt functions
#

omr_ask_yes_no() {
    local question="$1"
    local default="${2:-n}"
    local answer

    if [ "$default" = "y" ]; then
        printf -- "${OMR_YELLOW}?${OMR_NC} %s [Y/n]: " "$question"
    else
        printf -- "${OMR_YELLOW}?${OMR_NC} %s [y/N]: " "$question"
    fi

    read -r answer
    answer="${answer:-$default}"

    case "$answer" in
        [Yy]|[Yy][Ee][Ss])
            return 0
            ;;
        *)
            return 1
            ;;
    esac
}

omr_ask_input() {
    local prompt="$1"
    local default="$2"
    local value

    if [ -n "$default" ]; then
        printf -- "${OMR_YELLOW}?${OMR_NC} %s [%s]: " "$prompt" "$default"
    else
        printf -- "${OMR_YELLOW}?${OMR_NC} %s: " "$prompt"
    fi

    read -r value
    echo "${value:-$default}"
}

#
# Validation functions
#

omr_validate_command() {
    local cmd="$1"
    if command -v "$cmd" >/dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

omr_validate_file() {
    local file="$1"
    if [ -f "$file" ]; then
        return 0
    else
        return 1
    fi
}

omr_validate_directory() {
    local dir="$1"
    if [ -d "$dir" ]; then
        return 0
    else
        return 1
    fi
}

omr_validate_ip() {
    local ip="$1"
    # Simple IPv4 validation
    echo "$ip" | grep -qE '^([0-9]{1,3}\.){3}[0-9]{1,3}$'
}

#
# Utility functions
#

omr_format_bytes() {
    local bytes="$1"
    if [ "$bytes" -lt 1024 ]; then
        echo "${bytes}B"
    elif [ "$bytes" -lt 1048576 ]; then
        echo "$((bytes / 1024))KB"
    elif [ "$bytes" -lt 1073741824 ]; then
        echo "$((bytes / 1048576))MB"
    else
        echo "$((bytes / 1073741824))GB"
    fi
}

omr_format_duration() {
    local seconds="$1"
    local hours=$((seconds / 3600))
    local minutes=$(( (seconds % 3600) / 60 ))
    local secs=$((seconds % 60))

    if [ "$hours" -gt 0 ]; then
        printf -- "%dh %dm %ds" "$hours" "$minutes" "$secs"
    elif [ "$minutes" -gt 0 ]; then
        printf -- "%dm %ds" "$minutes" "$secs"
    else
        printf -- "%ds" "$secs"
    fi
}

omr_get_timestamp() {
    date '+%Y-%m-%d %H:%M:%S'
}

omr_get_timestamp_compact() {
    date '+%Y%m%d_%H%M%S'
}

#
# Error handling
#

omr_die() {
    omr_log_error "$*"
    exit 1
}

omr_require_root() {
    if [ "$(id -u)" -ne 0 ]; then
        omr_die "This script must be run as root"
    fi
}

omr_require_command() {
    local cmd="$1"
    local install_hint="${2:-Install it with your package manager}"

    if ! omr_validate_command "$cmd"; then
        omr_log_error "Required command not found: $cmd"
        omr_log_info "$install_hint"
        exit 1
    fi
}

#
# Spinner for long-running tasks
#

omr_spinner_start() {
    local message="$1"
    local pid="$2"

    local spin='⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏'
    local i=0

    printf -- "${OMR_BLUE}${OMR_ARROW}${OMR_NC} %s " "$message"

    while kill -0 "$pid" 2>/dev/null; do
        i=$(( (i+1) % 10 ))
        printf -- "\b${spin:$i:1}"
        sleep 0.1
    done

    printf -- "\b"
}

#
# Menu/selection functions
# Note: Complex menu functions require bash arrays
# For POSIX sh compatibility, implement menus in individual scripts
#

#
# Summary/table functions
#

omr_print_summary() {
    local title="$1"
    shift

    omr_log_header "$title"

    while [ $# -gt 0 ]; do
        local key="$1"
        local value="$2"
        shift 2

        printf -- "  %-30s %s\n" "${OMR_DIM}${key}:${OMR_NC}" "${OMR_WHITE}${value}${OMR_NC}"
    done

    echo ""
}

#
# Functions are available when this file is sourced with: . omr-lib.sh
# No need to export functions in POSIX sh
#
