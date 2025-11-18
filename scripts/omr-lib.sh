#!/bin/bash
#
# OpenMPTCProuter Optimized - Common Library Functions
# Shared functions for all setup and configuration scripts
#
# Source this file in other scripts:
#   . /path/to/omr-lib.sh
#

# Color codes (if not already defined)
if [ -z "$RED" ]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[1;33m'
    BLUE='\033[0;34m'
    PURPLE='\033[0;35m'
    CYAN='\033[0;36m'
    NC='\033[0m'
fi

#
# Dependency Checking
#

# Check if required dependencies are installed
# Usage: check_dependencies curl wget jq || exit 1
check_dependencies() {
    local missing=""
    local tool

    for tool in "$@"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing="$missing $tool"
        fi
    done

    if [ -n "$missing" ]; then
        echo -e "${RED}✗ Error: Missing required dependencies:${NC}$missing" >&2
        echo "Please install them before continuing." >&2
        echo ""
        echo "On Debian/Ubuntu:" >&2
        echo "  apt-get update && apt-get install -y$missing" >&2
        echo ""
        echo "On OpenWrt:" >&2
        echo "  opkg update && opkg install$missing" >&2
        return 1
    fi

    return 0
}

# Check single dependency and install if possible (OpenWrt only)
# Usage: ensure_package jq
ensure_package() {
    local package="$1"

    if command -v "$package" >/dev/null 2>&1; then
        return 0
    fi

    # Try to install on OpenWrt
    if command -v opkg >/dev/null 2>&1; then
        echo -e "${CYAN}Installing $package...${NC}"
        if opkg update >/dev/null 2>&1 && opkg install "$package" >/dev/null 2>&1; then
            echo -e "${GREEN}✓ $package installed${NC}"
            return 0
        else
            echo -e "${RED}✗ Failed to install $package${NC}" >&2
            return 1
        fi
    fi

    echo -e "${RED}✗ $package not found and cannot be installed automatically${NC}" >&2
    return 1
}

#
# IP Detection with Multiple Fallbacks
#

# Detect public IP address using multiple services
# Returns IP address or empty string on failure
detect_public_ip() {
    local services=(
        "https://ifconfig.me"
        "https://icanhazip.com"
        "https://ipinfo.io/ip"
        "https://api.ipify.org"
        "https://checkip.amazonaws.com"
    )
    local ip
    local service

    for service in "${services[@]}"; do
        if ip=$(curl -4 -s --max-time 5 "$service" 2>/dev/null) && [ -n "$ip" ]; then
            # Validate IP format
            if echo "$ip" | grep -Eq '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
                echo "$ip"
                return 0
            fi
        fi
    done

    return 1
}

# Get public IP with fallback to manual entry
# Usage: VPS_IP=$(get_public_ip_interactive)
get_public_ip_interactive() {
    local ip

    echo -e "${CYAN}Detecting your public IP address...${NC}"

    if ip=$(detect_public_ip); then
        echo -e "${GREEN}✓ Detected IP: $ip${NC}"
        echo "$ip"
        return 0
    else
        echo -e "${YELLOW}⚠ Could not auto-detect IP address${NC}"
        echo -e "${YELLOW}Please enter it manually:${NC}"
        read -r -p "Public IP: " ip < /dev/tty

        # Validate format
        if ! echo "$ip" | grep -Eq '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
            echo -e "${RED}✗ Invalid IP address format${NC}" >&2
            return 1
        fi

        echo "$ip"
        return 0
    fi
}

#
# Input Validation
#

# Validate IP address format
# Usage: validate_ip "192.168.1.1" || exit 1
validate_ip() {
    local ip="$1"

    if echo "$ip" | grep -Eq '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
        # Additional check: each octet should be 0-255
        local valid=1
        local IFS='.'
        local -a octets
        read -ra octets <<< "$ip"

        for octet in "${octets[@]}"; do
            if [ "$octet" -lt 0 ] 2>/dev/null || [ "$octet" -gt 255 ] 2>/dev/null; then
                valid=0
                break
            fi
        done

        if [ "$valid" -eq 1 ]; then
            return 0
        fi
    fi

    echo -e "${RED}✗ Invalid IP address: $ip${NC}" >&2
    return 1
}

# Validate port number
# Usage: validate_port 65500 || exit 1
validate_port() {
    local port="$1"

    if [ "$port" -ge 1 ] 2>/dev/null && [ "$port" -le 65535 ] 2>/dev/null; then
        return 0
    fi

    echo -e "${RED}✗ Invalid port: $port (must be 1-65535)${NC}" >&2
    return 1
}

# Validate hostname/domain
# Usage: validate_hostname "example.com" || exit 1
validate_hostname() {
    local hostname="$1"

    if echo "$hostname" | grep -Eq '^[a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(\.[a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$'; then
        return 0
    fi

    echo -e "${RED}✗ Invalid hostname: $hostname${NC}" >&2
    return 1
}

#
# UCI Configuration Management (OpenWrt only)
#

# Safely commit UCI changes with automatic backup
# Usage: uci_safe_commit network || rollback_failed
uci_safe_commit() {
    local config="$1"
    local backup_file="/tmp/uci-backup-${config}-$(date +%s).conf"

    # Check if UCI is available
    if ! command -v uci >/dev/null 2>&1; then
        echo -e "${RED}✗ UCI not available (not OpenWrt?)${NC}" >&2
        return 1
    fi

    # Create backup
    if uci export "$config" > "$backup_file" 2>/dev/null; then
        echo -e "${BLUE}ℹ Backup created: $backup_file${NC}"
    else
        echo -e "${YELLOW}⚠ Could not create backup (config may not exist yet)${NC}"
    fi

    # Commit changes
    if uci commit "$config" 2>/dev/null; then
        echo -e "${GREEN}✓ Configuration committed: $config${NC}"
        return 0
    else
        echo -e "${RED}✗ Configuration commit failed: $config${NC}" >&2

        # Attempt restore if backup exists
        if [ -f "$backup_file" ]; then
            echo -e "${CYAN}Attempting to restore from backup...${NC}"
            if uci import "$config" < "$backup_file" 2>/dev/null; then
                echo -e "${GREEN}✓ Restored from backup${NC}"
            else
                echo -e "${RED}✗ Restore failed - manual intervention required${NC}" >&2
                echo -e "${YELLOW}Backup file: $backup_file${NC}"
            fi
        fi

        return 1
    fi
}

# Rollback to a specific UCI backup
# Usage: uci_rollback network /tmp/uci-backup-network-123456.conf
uci_rollback() {
    local config="$1"
    local backup_file="$2"

    if [ ! -f "$backup_file" ]; then
        echo -e "${RED}✗ Backup file not found: $backup_file${NC}" >&2
        return 1
    fi

    if ! command -v uci >/dev/null 2>&1; then
        echo -e "${RED}✗ UCI not available${NC}" >&2
        return 1
    fi

    echo -e "${CYAN}Rolling back $config to $backup_file...${NC}"

    if uci import "$config" < "$backup_file" && uci commit "$config"; then
        echo -e "${GREEN}✓ Rollback successful${NC}"
        return 0
    else
        echo -e "${RED}✗ Rollback failed${NC}" >&2
        return 1
    fi
}

#
# Service Management
#

# Wait for service to start (with timeout)
# Usage: wait_for_service shadowsocks-libev 30 || echo "Failed to start"
wait_for_service() {
    local service="$1"
    local timeout="${2:-30}"
    local elapsed=0

    echo -n "Waiting for $service to start..."

    while [ $elapsed -lt "$timeout" ]; do
        # Check if init script exists
        if [ -f "/etc/init.d/$service" ]; then
            # Try OpenWrt/OpenRC status check
            if /etc/init.d/"$service" status >/dev/null 2>&1; then
                echo -e " ${GREEN}✓${NC}"
                return 0
            fi
        fi

        # Fallback: Check if any process with service name is running
        if pgrep -f "$service" >/dev/null 2>&1; then
            echo -e " ${GREEN}✓${NC}"
            return 0
        fi

        sleep 1
        elapsed=$((elapsed + 1))
        echo -n "."
    done

    echo -e " ${RED}✗ timeout${NC}"
    return 1
}

# Restart service and wait for confirmation
# Usage: restart_service_safe shadowsocks-libev || handle_error
restart_service_safe() {
    local service="$1"
    local timeout="${2:-30}"

    if [ ! -f "/etc/init.d/$service" ]; then
        echo -e "${YELLOW}⚠ Service not found: $service${NC}"
        return 1
    fi

    echo -e "${CYAN}Restarting $service...${NC}"

    if /etc/init.d/"$service" restart >/dev/null 2>&1; then
        # Wait for service to be running
        if wait_for_service "$service" "$timeout"; then
            echo -e "${GREEN}✓ $service restarted successfully${NC}"
            return 0
        else
            echo -e "${RED}✗ $service did not start within ${timeout}s${NC}" >&2
            echo -e "${YELLOW}Check logs: logread | grep $service${NC}"
            return 1
        fi
    else
        echo -e "${RED}✗ Failed to restart $service${NC}" >&2
        return 1
    fi
}

#
# Systemd Service Management (for VPS)
#

# Check systemd service status
# Usage: systemd_service_active shadowsocks-server || echo "Not running"
systemd_service_active() {
    local service="$1"

    if ! command -v systemctl >/dev/null 2>&1; then
        return 1
    fi

    systemctl is-active --quiet "$service"
}

# Restart systemd service and verify
# Usage: systemd_restart_safe shadowsocks-server.service
systemd_restart_safe() {
    local service="$1"
    local timeout="${2:-30}"
    local elapsed=0

    if ! command -v systemctl >/dev/null 2>&1; then
        echo -e "${RED}✗ systemd not available${NC}" >&2
        return 1
    fi

    echo -e "${CYAN}Restarting $service...${NC}"

    if systemctl restart "$service" 2>/dev/null; then
        # Wait for service to be active
        while [ $elapsed -lt "$timeout" ]; do
            if systemctl is-active --quiet "$service"; then
                echo -e "${GREEN}✓ $service restarted successfully${NC}"
                return 0
            fi
            sleep 1
            elapsed=$((elapsed + 1))
        done

        echo -e "${RED}✗ $service did not start within ${timeout}s${NC}" >&2
        systemctl status "$service" --no-pager
        return 1
    else
        echo -e "${RED}✗ Failed to restart $service${NC}" >&2
        return 1
    fi
}

#
# Configuration Defaults
#

# Load configuration defaults from file or environment
# Usage: load_omr_defaults
load_omr_defaults() {
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
        echo "OMR Configuration:"
        echo "  Shadowsocks Port: $OMR_SHADOWSOCKS_PORT"
        echo "  Glorytun TCP: $OMR_GLORYTUN_TCP_PORT"
        echo "  Glorytun UDP: $OMR_GLORYTUN_UDP_PORT"
        echo "  Web UI Port: $OMR_WEB_UI_PORT"
        echo "  Pairing Port: $OMR_PAIRING_PORT"
        echo "  LAN IP: $OMR_LAN_IP"
    fi
}

#
# Progress Indicators
#

# Show spinner while command runs
# Usage: run_with_spinner "Installing packages" "apt-get install -y curl"
run_with_spinner() {
    local message="$1"
    local command="$2"
    local pid
    local spin='-\|/'
    local i=0

    echo -n "$message... "

    # Run command in background
    eval "$command" > /tmp/omr-spinner.$$ 2>&1 &
    pid=$!

    # Show spinner while running
    while kill -0 $pid 2>/dev/null; do
        i=$(( (i+1) % 4 ))
        printf "\r%s... %s" "$message" "${spin:$i:1}"
        sleep 0.1
    done

    # Get exit code
    wait $pid
    local exit_code=$?

    if [ $exit_code -eq 0 ]; then
        echo -e "\r${message}... ${GREEN}✓${NC}"
        rm -f /tmp/omr-spinner.$$
        return 0
    else
        echo -e "\r${message}... ${RED}✗${NC}"
        echo -e "${YELLOW}Command output:${NC}"
        cat /tmp/omr-spinner.$$
        rm -f /tmp/omr-spinner.$$
        return $exit_code
    fi
}

#
# Logging
#

# Log to system logger and console
# Usage: omr_log info "Configuration completed"
omr_log() {
    local level="$1"
    shift
    local message="$*"

    # Log to syslog if available
    if command -v logger >/dev/null 2>&1; then
        logger -t openmptcprouter "[$level] $message"
    fi

    # Also print to console with color
    case "$level" in
        error)
            echo -e "${RED}✗ Error: $message${NC}" >&2
            ;;
        warning)
            echo -e "${YELLOW}⚠ Warning: $message${NC}"
            ;;
        info)
            echo -e "${BLUE}ℹ $message${NC}"
            ;;
        success)
            echo -e "${GREEN}✓ $message${NC}"
            ;;
        *)
            echo "$message"
            ;;
    esac
}

# Initialize library
# This is called automatically when sourced
omr_lib_init() {
    # Load defaults
    load_omr_defaults

    # Set locale for consistent behavior
    export LC_ALL=C

    return 0
}

# Auto-initialize when sourced
omr_lib_init
