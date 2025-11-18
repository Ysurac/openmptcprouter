#!/bin/sh
#
# OpenMPTCProuter Optimized - Automated Client Setup (IMPROVED VERSION)
# Run this script on your OpenMPTCProuter router for automatic configuration
#
# IMPROVEMENTS:
#   - Uses common library functions (omr-lib.sh)
#   - Proper dependency checking
#   - UCI rollback support
#   - Service health verification
#   - Exit code validation
#   - Configurable defaults via environment variables
#
# Usage:
#   ./client-auto-setup-improved.sh VPS_IP VPS_PASSWORD [VPS_PORT]
#   curl -sSL https://raw.githubusercontent.com/.../client-auto-setup-improved.sh | sh -s VPS_IP PASSWORD
#

set -e

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Load common library if available
if [ -f "$(dirname "$0")/omr-lib.sh" ]; then
    # shellcheck disable=SC1091
    . "$(dirname "$0")/omr-lib.sh"
elif [ -f "/usr/lib/openmptcprouter/omr-lib.sh" ]; then
    # shellcheck disable=SC1091
    . /usr/lib/openmptcprouter/omr-lib.sh
else
    # Library not available - define essential functions inline
    check_dependencies() {
        local missing=""
        for tool in "$@"; do
            if ! command -v "$tool" >/dev/null 2>&1; then
                missing="$missing $tool"
            fi
        done
        if [ -n "$missing" ]; then
            echo -e "${RED}Error: Missing dependencies:${NC}$missing"
            return 1
        fi
        return 0
    }

    uci_safe_commit() {
        local config="$1"
        local backup="/tmp/uci-backup-${config}-$(date +%s).conf"
        uci export "$config" > "$backup" 2>/dev/null || true
        if uci commit "$config" 2>/dev/null; then
            echo -e "${GREEN}✓ Committed: $config${NC}"
            return 0
        else
            echo -e "${RED}✗ Commit failed: $config${NC}"
            [ -f "$backup" ] && uci import "$config" < "$backup"
            return 1
        fi
    }
fi

clear

echo -e "${CYAN}"
cat << 'EOF'
╔══════════════════════════════════════════════════════╗
║  OpenMPTCProuter Optimized - Client Auto Setup      ║
║                 (IMPROVED VERSION)                   ║
╚══════════════════════════════════════════════════════╝
EOF
echo -e "${NC}"

# Version and usage info
show_help() {
    cat << 'HELP'
Usage: client-auto-setup-improved.sh [OPTIONS] VPS_IP PASSWORD [PORT]

ARGUMENTS:
    VPS_IP      VPS server IP address
    PASSWORD    VPS connection password
    PORT        VPS port (default: 65500)

OPTIONS:
    -h, --help          Show this help
    --non-interactive   Skip confirmations (for automation)

ENVIRONMENT VARIABLES:
    OMR_SHADOWSOCKS_PORT    Override default port (default: 65500)
    OMR_SS_METHOD           Encryption method (default: chacha20-ietf-poly1305)

EXAMPLES:
    # Interactive mode
    ./client-auto-setup-improved.sh 1.2.3.4 mypassword

    # Non-interactive with custom port
    ./client-auto-setup-improved.sh --non-interactive 1.2.3.4 mypassword 8388

    # Using environment variables
    export OMR_SHADOWSOCKS_PORT=8388
    ./client-auto-setup-improved.sh 1.2.3.4 mypassword

HELP
}

# Parse options
NON_INTERACTIVE=0
while [ $# -gt 0 ]; do
    case "$1" in
        -h|--help)
            show_help
            exit 0
            ;;
        --non-interactive)
            NON_INTERACTIVE=1
            shift
            ;;
        -*)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
        *)
            break
            ;;
    esac
done

# Check if running on OpenWrt
if [ ! -f /etc/openwrt_release ]; then
    echo -e "${RED}Error: This script must be run on an OpenWrt/OpenMPTCProuter router${NC}"
    exit 1
fi

# Check required dependencies
echo -e "${CYAN}Checking dependencies...${NC}"
if ! check_dependencies uci opkg; then
    echo -e "${RED}Error: Required tools not found${NC}"
    echo "This script requires UCI and opkg (OpenWrt standard tools)"
    exit 1
fi

# Load configuration defaults
: "${OMR_SHADOWSOCKS_PORT:=65500}"
: "${OMR_SS_METHOD:=chacha20-ietf-poly1305}"

# Parse arguments
VPS_IP="$1"
VPS_PASSWORD="$2"
VPS_PORT="${3:-${OMR_SHADOWSOCKS_PORT}}"

# Interactive mode if no arguments
if [ -z "$VPS_IP" ] && [ "$NON_INTERACTIVE" -eq 0 ]; then
    echo -e "${YELLOW}═══ Automated Router Configuration ═══${NC}"
    echo ""
    echo -e "${BLUE}This script will automatically configure your router to connect to your VPS.${NC}"
    echo ""

    echo -e "${CYAN}Enter your VPS details:${NC}"
    printf "VPS IP Address: "
    read -r VPS_IP < /dev/tty

    # Validate IP address format
    if ! echo "$VPS_IP" | grep -Eq '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
        echo -e "${RED}Error: Invalid IP address format${NC}"
        exit 1
    fi

    printf "VPS Password: "
    read -r VPS_PASSWORD < /dev/tty

    printf "VPS Port (default $OMR_SHADOWSOCKS_PORT): "
    read -r VPS_PORT_INPUT < /dev/tty
    VPS_PORT="${VPS_PORT_INPUT:-${OMR_SHADOWSOCKS_PORT}}"

    # Validate port number
    if ! [ "$VPS_PORT" -ge 1 ] 2>/dev/null || ! [ "$VPS_PORT" -le 65535 ] 2>/dev/null; then
        echo -e "${RED}Error: Port must be between 1-65535${NC}"
        exit 1
    fi
fi

# Validate inputs
if [ -z "$VPS_IP" ] || [ -z "$VPS_PASSWORD" ]; then
    echo -e "${RED}Error: VPS IP and Password are required${NC}"
    echo ""
    echo "Usage: $0 VPS_IP PASSWORD [PORT]"
    echo "   or: $0 --help"
    exit 1
fi

# Validate IP format
if ! echo "$VPS_IP" | grep -Eq '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
    echo -e "${RED}Error: Invalid IP address format: $VPS_IP${NC}"
    exit 1
fi

# Validate port range
if ! [ "$VPS_PORT" -ge 1 ] 2>/dev/null || ! [ "$VPS_PORT" -le 65535 ] 2>/dev/null; then
    echo -e "${RED}Error: Invalid port: $VPS_PORT (must be 1-65535)${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}╔════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║  Configuration Summary                 ║${NC}"
echo -e "${GREEN}╠════════════════════════════════════════╣${NC}"
echo -e "${GREEN}║${NC} VPS IP:      ${YELLOW}$VPS_IP${NC}"
echo -e "${GREEN}║${NC} VPS Port:    ${YELLOW}$VPS_PORT${NC}"
echo -e "${GREEN}║${NC} Password:    ${YELLOW}$(echo "$VPS_PASSWORD" | sed 's/./*/g')${NC}"
echo -e "${GREEN}║${NC} Encryption:  ${YELLOW}$OMR_SS_METHOD${NC}"
echo -e "${GREEN}╚════════════════════════════════════════╝${NC}"
echo ""

# Confirm (skip if non-interactive)
if [ "$NON_INTERACTIVE" -eq 0 ]; then
    printf "${YELLOW}Proceed with configuration? (Y/n): ${NC}"
    read -r CONFIRM < /dev/tty
    if [ "$CONFIRM" = "n" ] || [ "$CONFIRM" = "N" ]; then
        echo -e "${YELLOW}Configuration cancelled.${NC}"
        exit 0
    fi
fi

echo ""
echo -e "${CYAN}[1/6]${NC} Detecting network interfaces..."

# Detect WAN interfaces
WAN_INTERFACES=$(uci show network | grep "network\..*\.proto='dhcp'\|network\..*\.proto='static'" | grep -v loopback | cut -d. -f2 | sort -u)
WAN_COUNT=$(echo "$WAN_INTERFACES" | wc -l)

echo -e "${GREEN}      Found $WAN_COUNT network interface(s)${NC}"

echo ""
echo -e "${CYAN}[2/6]${NC} Configuring Shadowsocks client..."

# Install shadowsocks-libev if not present
if ! opkg list-installed | grep -q shadowsocks-libev; then
    echo -e "${BLUE}      Installing shadowsocks-libev...${NC}"

    # Update package list
    if ! opkg update > /dev/null 2>&1; then
        echo -e "${RED}✗ Failed to update opkg package list${NC}"
        echo -e "${YELLOW}Please check network connectivity${NC}"
        exit 1
    fi

    # Install packages
    if ! opkg install shadowsocks-libev-ss-redir shadowsocks-libev-ss-local > /dev/null 2>&1; then
        echo -e "${RED}✗ Failed to install shadowsocks-libev${NC}"
        echo -e "${YELLOW}Please install manually: opkg install shadowsocks-libev-ss-redir shadowsocks-libev-ss-local${NC}"
        exit 1
    fi

    echo -e "${GREEN}      ✓ Shadowsocks installed${NC}"
else
    echo -e "${GREEN}      ✓ Shadowsocks already installed${NC}"
fi

# Configure Shadowsocks with backup
echo -e "${BLUE}      Applying Shadowsocks configuration...${NC}"

uci set shadowsocks-libev.omr=ss_redir
uci set shadowsocks-libev.omr.server="$VPS_IP"
uci set shadowsocks-libev.omr.server_port="$VPS_PORT"
uci set shadowsocks-libev.omr.password="$VPS_PASSWORD"
uci set shadowsocks-libev.omr.method="$OMR_SS_METHOD"
uci set shadowsocks-libev.omr.local_address='0.0.0.0'
uci set shadowsocks-libev.omr.local_port='1100'
uci set shadowsocks-libev.omr.timeout='600'
uci set shadowsocks-libev.omr.fast_open='1'
uci set shadowsocks-libev.omr.mode='tcp_and_udp'

# Commit with backup
if ! uci_safe_commit shadowsocks-libev; then
    echo -e "${RED}✗ Failed to commit Shadowsocks configuration${NC}"
    exit 1
fi

echo -e "${GREEN}      ✓ Shadowsocks configured${NC}"

echo ""
echo -e "${CYAN}[3/6]${NC} Configuring MPTCP..."

# Enable MPTCP if available
if [ -f /proc/sys/net/mptcp/mptcp_enabled ]; then
    echo 1 > /proc/sys/net/mptcp/mptcp_enabled

    # Make persistent
    uci set network.globals.mptcp_enabled='1'
    uci set network.globals.mptcp_path_manager='fullmesh'
    uci set network.globals.mptcp_scheduler='default'

    if ! uci_safe_commit network; then
        echo -e "${YELLOW}⚠ Warning: Could not persist MPTCP settings${NC}"
    fi

    echo -e "${GREEN}      ✓ MPTCP enabled (fullmesh mode)${NC}"
else
    echo -e "${YELLOW}      ⚠ MPTCP not available in kernel, skipping...${NC}"
fi

echo ""
echo -e "${CYAN}[4/6]${NC} Configuring firewall..."

# Add firewall rules for VPN
uci set firewall.omrvpn=zone
uci set firewall.omrvpn.name='omrvpn'
uci set firewall.omrvpn.input='ACCEPT'
uci set firewall.omrvpn.output='ACCEPT'
uci set firewall.omrvpn.forward='ACCEPT'
uci set firewall.omrvpn.masq='1'
uci set firewall.omrvpn.mtu_fix='1'

uci set firewall.omrvpn_wan=forwarding
uci set firewall.omrvpn_wan.src='lan'
uci set firewall.omrvpn_wan.dest='omrvpn'

uci set firewall.omrvpn_rule=rule
uci set firewall.omrvpn_rule.name='Allow-OMR-VPN'
uci set firewall.omrvpn_rule.src='wan'
uci set firewall.omrvpn_rule.dest_port="$VPS_PORT"
uci set firewall.omrvpn_rule.proto='tcp udp'
uci set firewall.omrvpn_rule.target='ACCEPT'

if ! uci_safe_commit firewall; then
    echo -e "${RED}✗ Failed to commit firewall configuration${NC}"
    exit 1
fi

echo -e "${GREEN}      ✓ Firewall configured${NC}"

echo ""
echo -e "${CYAN}[5/6]${NC} Configuring routing..."

# Set up policy routing
uci set network.omr=interface
uci set network.omr.proto='static'
uci set network.omr.ifname='tun0'
uci set network.omr.ipaddr='10.255.255.2'
uci set network.omr.netmask='255.255.255.252'

# Add route through VPN
uci set network.omr_route=route
uci set network.omr_route.interface='omr'
uci set network.omr_route.target='0.0.0.0'
uci set network.omr_route.netmask='0.0.0.0'
uci set network.omr_route.gateway='10.255.255.1'
uci set network.omr_route.metric='1'

if ! uci_safe_commit network; then
    echo -e "${RED}✗ Failed to commit network configuration${NC}"
    exit 1
fi

echo -e "${GREEN}      ✓ Routing configured${NC}"

echo ""
echo -e "${CYAN}[6/6]${NC} Applying configuration and restarting services..."

# Reload services sequentially (not in background)
if /etc/init.d/network reload > /dev/null 2>&1; then
    echo -e "${GREEN}      ✓ Network reloaded${NC}"
else
    echo -e "${YELLOW}      ⚠ Network reload returned non-zero exit code${NC}"
fi

sleep 2

if /etc/init.d/firewall reload > /dev/null 2>&1; then
    echo -e "${GREEN}      ✓ Firewall reloaded${NC}"
else
    echo -e "${YELLOW}      ⚠ Firewall reload returned non-zero exit code${NC}"
fi

sleep 1

if /etc/init.d/shadowsocks-libev restart > /dev/null 2>&1; then
    echo -e "${GREEN}      ✓ Shadowsocks restarted${NC}"
else
    echo -e "${YELLOW}      ⚠ Shadowsocks restart returned non-zero exit code${NC}"
fi

# Create connection test script
cat > /usr/bin/omr-test << 'TESTEOF'
#!/bin/sh
echo "Testing VPN connection..."
echo ""

# Test VPS connectivity
if ping -c 3 -W 2 VPS_IP_PLACEHOLDER > /dev/null 2>&1; then
    echo "✓ VPS is reachable"
else
    echo "✗ Cannot reach VPS"
    exit 1
fi

# Test Shadowsocks
if ss-local -h > /dev/null 2>&1; then
    echo "✓ Shadowsocks installed"
else
    echo "✗ Shadowsocks not installed"
    exit 1
fi

# Test MPTCP
if [ -f /proc/sys/net/mptcp/mptcp_enabled ]; then
    if [ "$(cat /proc/sys/net/mptcp/mptcp_enabled)" = "1" ]; then
        echo "✓ MPTCP enabled"
    else
        echo "⚠ MPTCP disabled"
    fi
else
    echo "⚠ MPTCP not available"
fi

# Test routing
if ip route | grep -q "default.*tun0"; then
    echo "✓ VPN routing active"
else
    echo "⚠ VPN routing not detected"
fi

echo ""
echo "Connection test complete!"
TESTEOF

chmod +x /usr/bin/omr-test
sed -i "s|VPS_IP_PLACEHOLDER|$VPS_IP|g" /usr/bin/omr-test

echo ""
echo -e "${GREEN}╔══════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║                                              ║${NC}"
echo -e "${GREEN}║          ✓ CONFIGURATION COMPLETE!          ║${NC}"
echo -e "${GREEN}║                                              ║${NC}"
echo -e "${GREEN}╚══════════════════════════════════════════════╝${NC}"
echo ""

# Save configuration summary
cat > /etc/omr-config.txt << ENDCONFIG
OpenMPTCProuter Optimized - Client Configuration
================================================
Configuration Date: $(date)

VPS Details:
-----------
IP:         $VPS_IP
Port:       $VPS_PORT
Password:   $VPS_PASSWORD
Encryption: $OMR_SS_METHOD

Network:
--------
MPTCP:      $([ -f /proc/sys/net/mptcp/mptcp_enabled ] && echo "Enabled (fullmesh)" || echo "Not available")
Interfaces: $WAN_COUNT WAN interface(s)

Status:
-------
Configuration: Complete
Services:      Restarted
Routing:       Configured

Test Command:
-------------
Run: omr-test

Configuration Backups:
---------------------
UCI backups stored in: /tmp/uci-backup-*

To view this config: cat /etc/omr-config.txt
To test connection: omr-test
To view logs: logread | grep shadowsocks

ROLLBACK INSTRUCTIONS:
If something went wrong, restore from backup:
  ls -lt /tmp/uci-backup-* | head -5
  uci import network < /tmp/uci-backup-network-TIMESTAMP.conf
  uci commit network
ENDCONFIG

chmod 600 /etc/omr-config.txt

echo -e "${BLUE}Configuration Summary:${NC}"
echo -e "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo -e "${CYAN}VPS:${NC}          $VPS_IP:$VPS_PORT"
echo -e "${CYAN}Encryption:${NC}   $OMR_SS_METHOD"
echo -e "${CYAN}MPTCP:${NC}        $([ -f /proc/sys/net/mptcp/mptcp_enabled ] && echo "Enabled" || echo "Not available")"
echo -e "${CYAN}WAN Ports:${NC}    $WAN_COUNT interface(s)"
echo -e "${CYAN}Config File:${NC}  /etc/omr-config.txt"
echo -e "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

echo -e "${YELLOW}Next Steps:${NC}"
echo -e "  ${CYAN}1.${NC} Wait 30 seconds for services to fully start"
echo -e "  ${CYAN}2.${NC} Run ${GREEN}omr-test${NC} to verify connection"
echo -e "  ${CYAN}3.${NC} Check web interface at ${GREEN}http://192.168.2.1${NC}"
echo -e "  ${CYAN}4.${NC} View logs: ${GREEN}logread | grep shadowsocks${NC}"
echo ""

if [ "$NON_INTERACTIVE" -eq 0 ]; then
    echo -e "${GREEN}Your router is now configured and connecting to your VPS!${NC}"
    echo ""
    echo -e "${BLUE}Testing connection in 30 seconds...${NC}"
    sleep 30

    echo ""
    echo -e "${CYAN}Running connection test...${NC}"
    /usr/bin/omr-test || {
        echo ""
        echo -e "${YELLOW}⚠ Connection test had issues${NC}"
        echo -e "${YELLOW}Please check logs: logread | grep shadowsocks${NC}"
    }

    echo ""
    echo -e "${GREEN}Setup complete! Enjoy your bonded internet connection!${NC}"
else
    echo -e "${GREEN}Setup complete! Run 'omr-test' to verify connection.${NC}"
fi

echo ""
