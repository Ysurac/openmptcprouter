#!/bin/sh
#
# OpenMPTCProuter Shared Library
# Common functions for scripts to ensure consistent UX
# Copyright (C) 2025 spotty118 - OpenMPTCProuter Optimized
#
# Usage: Source this file in your script:
#   . "$(dirname "$0")/omr-lib.sh" || . /usr/share/omr/omr-lib.sh
#
# POSIX sh compatible - works on both bash and sh

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
    if echo "$ip" | grep -qE '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
        # Additional check: each octet should be 0-255
        local valid=1
        local octet
        for octet in $(echo "$ip" | tr '.' ' '); do
            if [ "$octet" -lt 0 ] 2>/dev/null || [ "$octet" -gt 255 ] 2>/dev/null; then
                valid=0
                break
            fi
        done
        [ "$valid" -eq 1 ] && return 0
    fi
    return 1
}

omr_validate_port() {
    local port="$1"
    if [ "$port" -ge 1 ] 2>/dev/null && [ "$port" -le 65535 ] 2>/dev/null; then
        return 0
    fi
    return 1
}

#
# Dependency checking
#

omr_check_dependencies() {
    local missing=""
    local tool

    for tool in "$@"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing="$missing $tool"
        fi
    done

    if [ -n "$missing" ]; then
        omr_log_error "Missing required dependencies:$missing"
        echo "Please install them before continuing." >&2
        return 1
    fi

    return 0
}

omr_ensure_package() {
    local package="$1"

    if command -v "$package" >/dev/null 2>&1; then
        return 0
    fi

    # Try to install on OpenWrt
    if command -v opkg >/dev/null 2>&1; then
        omr_progress_start "Installing $package"
        if opkg update >/dev/null 2>&1 && opkg install "$package" >/dev/null 2>&1; then
            omr_progress_done
            return 0
        else
            omr_progress_fail
            return 1
        fi
    fi

    omr_log_error "$package not found and cannot be installed automatically"
    return 1
}

#
# IP Detection
#

omr_detect_public_ip() {
    local services="https://ifconfig.me https://icanhazip.com https://ipinfo.io/ip https://api.ipify.org https://checkip.amazonaws.com"
    local ip
    local service

    for service in $services; do
        if ip=$(curl -4 -s --max-time 5 "$service" 2>/dev/null) && [ -n "$ip" ]; then
            # Validate IP format
            if omr_validate_ip "$ip"; then
                echo "$ip"
                return 0
            fi
        fi
    done

    return 1
}

omr_get_public_ip_interactive() {
    local ip

    omr_progress_start "Detecting public IP address"

    if ip=$(omr_detect_public_ip); then
        omr_progress_done
        omr_log_info "Detected IP: $ip"
        echo "$ip"
        return 0
    else
        omr_progress_fail
        omr_log_warning "Could not auto-detect IP address"
        ip=$(omr_ask_input "Enter public IP manually")

        if omr_validate_ip "$ip"; then
            echo "$ip"
            return 0
        else
            omr_log_error "Invalid IP address format: $ip"
            return 1
        fi
    fi
}

#
# UCI Configuration Management (OpenWrt only)
#

omr_uci_safe_commit() {
    local config="$1"
    local backup_file="/tmp/uci-backup-${config}-$(date +%s).conf"

    # Check if UCI is available
    if ! command -v uci >/dev/null 2>&1; then
        omr_log_error "UCI not available (not OpenWrt?)"
        return 1
    fi

    # Create backup
    if uci export "$config" > "$backup_file" 2>/dev/null; then
        omr_log_info "Backup created: $backup_file"
    else
        omr_log_warning "Could not create backup (config may not exist yet)"
    fi

    # Commit changes
    if uci commit "$config" 2>/dev/null; then
        omr_log_success "Configuration committed: $config"
        return 0
    else
        omr_log_error "Configuration commit failed: $config"

        # Attempt restore if backup exists
        if [ -f "$backup_file" ]; then
            omr_log_info "Attempting to restore from backup..."
            if uci import "$config" < "$backup_file" 2>/dev/null; then
                omr_log_success "Restored from backup"
            else
                omr_log_error "Restore failed - manual intervention required"
                omr_log_info "Backup file: $backup_file"
            fi
        fi

        return 1
    fi
}

omr_uci_rollback() {
    local config="$1"
    local backup_file="$2"

    if [ ! -f "$backup_file" ]; then
        omr_log_error "Backup file not found: $backup_file"
        return 1
    fi

    if ! command -v uci >/dev/null 2>&1; then
        omr_log_error "UCI not available"
        return 1
    fi

    omr_progress_start "Rolling back $config"

    if uci import "$config" < "$backup_file" && uci commit "$config"; then
        omr_progress_done
        return 0
    else
        omr_progress_fail
        return 1
    fi
}

#
# Service Management
#

omr_wait_for_service() {
    local service="$1"
    local timeout="${2:-30}"
    local elapsed=0

    printf -- "Waiting for %s to start..." "$service"

    while [ $elapsed -lt "$timeout" ]; do
        # Check if init script exists
        if [ -f "/etc/init.d/$service" ]; then
            # Try OpenWrt/OpenRC status check
            if /etc/init.d/"$service" status >/dev/null 2>&1; then
                printf -- " ${OMR_GREEN}${OMR_CHECK}${OMR_NC}\n"
                return 0
            fi
        fi

        # Fallback: Check if any process with service name is running
        if pgrep -f "$service" >/dev/null 2>&1; then
            printf -- " ${OMR_GREEN}${OMR_CHECK}${OMR_NC}\n"
            return 0
        fi

        sleep 1
        elapsed=$((elapsed + 1))
        printf -- "."
    done

    printf -- " ${OMR_RED}${OMR_CROSS}${OMR_NC}\n"
    return 1
}

omr_restart_service_safe() {
    local service="$1"
    local timeout="${2:-30}"

    if [ ! -f "/etc/init.d/$service" ]; then
        omr_log_warning "Service not found: $service"
        return 1
    fi

    omr_progress_start "Restarting $service"

    if /etc/init.d/"$service" restart >/dev/null 2>&1; then
        omr_progress_done
        # Wait for service to be running
        if omr_wait_for_service "$service" "$timeout"; then
            return 0
        else
            omr_log_error "$service did not start within ${timeout}s"
            omr_log_info "Check logs: logread | grep $service"
            return 1
        fi
    else
        omr_progress_fail
        omr_log_error "Failed to restart $service"
        return 1
    fi
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
        printf -- "\b%s" "$(echo "$spin" | cut -c$((i+1))-$((i+1)))"
        sleep 0.1
    done

    printf -- "\b"
}

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
# Configuration defaults loading
#

omr_load_defaults() {
    # Default values (can be overridden)
    : "${OMR_SHADOWSOCKS_PORT:=65500}"
    : "${OMR_GLORYTUN_TCP_PORT:=65510}"
    : "${OMR_GLORYTUN_UDP_PORT:=65520}"
    : "${OMR_WEB_UI_PORT:=8080}"
    : "${OMR_PAIRING_PORT:=9999}"
    : "${OMR_LAN_IP:=192.168.2.1}"
    : "${OMR_LAN_NETMASK:=255.255.255.0}"
    : "${OMR_DHCP_START:=100}"
    : "${OMR_DHCP_LIMIT:=150}"

    # Export for use in scripts
    export OMR_SHADOWSOCKS_PORT
    export OMR_GLORYTUN_TCP_PORT
    export OMR_GLORYTUN_UDP_PORT
    export OMR_WEB_UI_PORT
    export OMR_PAIRING_PORT
    export OMR_LAN_IP
    export OMR_LAN_NETMASK
    export OMR_DHCP_START
    export OMR_DHCP_LIMIT

    # Load from config file if exists
    if [ -f /etc/openmptcprouter/defaults.conf ]; then
        # shellcheck disable=SC1091
        . /etc/openmptcprouter/defaults.conf
    fi

    # Debug output (if OMR_DEBUG=1)
    if [ "${OMR_DEBUG:-0}" -eq 1 ]; then
        omr_log_info "OMR Configuration loaded:"
        omr_log_info "  Shadowsocks Port: $OMR_SHADOWSOCKS_PORT"
        omr_log_info "  LAN IP: $OMR_LAN_IP"
    fi
}

#
# Functions are available when this file is sourced with: . omr-lib.sh
# No need to export functions in POSIX sh
#

# Auto-load defaults when sourced
omr_load_defaults
