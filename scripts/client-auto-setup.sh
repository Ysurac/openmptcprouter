#!/bin/sh
#
# OpenMPTCProuter Optimized - Automated Client Setup
# Run this script on your OpenMPTCProuter router for automatic configuration
#
# Usage: 
#   curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/main/scripts/client-auto-setup.sh | sh -s YOUR_VPS_IP YOUR_PASSWORD
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

clear

echo -e "${CYAN}"
cat << 'EOF'
╔══════════════════════════════════════════════════════╗
║  OpenMPTCProuter Optimized - Client Auto Setup      ║
╚══════════════════════════════════════════════════════╝
EOF
echo -e "${NC}"

# Check if running on OpenWrt
if [ ! -f /etc/openwrt_release ]; then
    echo -e "${RED}Error: This script must be run on an OpenWrt/OpenMPTCProuter router${NC}"
    exit 1
fi

# Parse arguments
VPS_IP="$1"
VPS_PASSWORD="$2"
VPS_PORT="${3:-65500}"

# Interactive mode if no arguments
if [ -z "$VPS_IP" ]; then
    echo -e "${YELLOW}═══ Automated Router Configuration ═══${NC}"
    echo ""
    echo -e "${BLUE}This script will automatically configure your router to connect to your VPS.${NC}"
    echo ""
    
    echo -e "${CYAN}Enter your VPS details:${NC}"
    printf "VPS IP Address: "
    read VPS_IP
    
    printf "VPS Password: "
    read VPS_PASSWORD
    
    printf "VPS Port (default 65500): "
    read VPS_PORT_INPUT
    VPS_PORT="${VPS_PORT_INPUT:-65500}"
fi

# Validate inputs
if [ -z "$VPS_IP" ] || [ -z "$VPS_PASSWORD" ]; then
    echo -e "${RED}Error: VPS IP and Password are required${NC}"
    echo ""
    echo "Usage: $0 VPS_IP PASSWORD [PORT]"
    echo "   or: curl -sSL https://url/client-auto-setup.sh | sh -s VPS_IP PASSWORD"
    exit 1
fi

echo ""
echo -e "${GREEN}╔════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║  Configuration Summary                 ║${NC}"
echo -e "${GREEN}╠════════════════════════════════════════╣${NC}"
echo -e "${GREEN}║${NC} VPS IP:      ${YELLOW}$VPS_IP${NC}"
echo -e "${GREEN}║${NC} VPS Port:    ${YELLOW}$VPS_PORT${NC}"
echo -e "${GREEN}║${NC} Password:    ${YELLOW}$(echo $VPS_PASSWORD | sed 's/./*/g')${NC}"
echo -e "${GREEN}║${NC} Encryption:  ${YELLOW}Shadowsocks${NC}"
echo -e "${GREEN}╚════════════════════════════════════════╝${NC}"
echo ""

# Confirm
printf "${YELLOW}Proceed with configuration? (Y/n): ${NC}"
read CONFIRM
if [ "$CONFIRM" = "n" ] || [ "$CONFIRM" = "N" ]; then
    echo -e "${YELLOW}Configuration cancelled.${NC}"
    exit 0
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
    echo "      Installing shadowsocks-libev..."
    opkg update > /dev/null 2>&1
    opkg install shadowsocks-libev-ss-redir shadowsocks-libev-ss-local > /dev/null 2>&1
fi

# Configure Shadowsocks
uci set shadowsocks-libev.omr=ss_redir
uci set shadowsocks-libev.omr.server="$VPS_IP"
uci set shadowsocks-libev.omr.server_port="$VPS_PORT"
uci set shadowsocks-libev.omr.password="$VPS_PASSWORD"
uci set shadowsocks-libev.omr.method='chacha20-ietf-poly1305'
uci set shadowsocks-libev.omr.local_address='0.0.0.0'
uci set shadowsocks-libev.omr.local_port='1100'
uci set shadowsocks-libev.omr.timeout='600'
uci set shadowsocks-libev.omr.fast_open='1'
uci set shadowsocks-libev.omr.mode='tcp_and_udp'
uci commit shadowsocks-libev

echo -e "${GREEN}      Shadowsocks configured${NC}"

echo ""
echo -e "${CYAN}[3/6]${NC} Configuring MPTCP..."

# Enable MPTCP
if [ -f /proc/sys/net/mptcp/mptcp_enabled ]; then
    echo 1 > /proc/sys/net/mptcp/mptcp_enabled
    
    # Make persistent
    uci set network.globals.mptcp_enabled='1'
    uci set network.globals.mptcp_path_manager='fullmesh'
    uci set network.globals.mptcp_scheduler='default'
    uci commit network
    
    echo -e "${GREEN}      MPTCP enabled (fullmesh mode)${NC}"
else
    echo -e "${YELLOW}      MPTCP not available in kernel, skipping...${NC}"
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

uci commit firewall

echo -e "${GREEN}      Firewall configured${NC}"

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

uci commit network

echo -e "${GREEN}      Routing configured${NC}"

echo ""
echo -e "${CYAN}[6/6]${NC} Applying configuration and restarting services..."

# Reload services
/etc/init.d/network reload > /dev/null 2>&1 &
sleep 2
/etc/init.d/firewall reload > /dev/null 2>&1 &
sleep 1
/etc/init.d/shadowsocks-libev restart > /dev/null 2>&1 &

echo -e "${GREEN}      Services restarted${NC}"

# Create connection test script
cat > /usr/bin/omr-test << 'TESTEOF'
#!/bin/sh
echo "Testing VPN connection..."
echo ""

# Test VPN connectivity
if ping -c 3 -W 2 $1 > /dev/null 2>&1; then
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
sed -i "s/\$1/$VPS_IP/g" /usr/bin/omr-test

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
Encryption: Shadowsocks (chacha20-ietf-poly1305)

Network:
--------
MPTCP:      Enabled (fullmesh)
Interfaces: $WAN_COUNT WAN interface(s)

Status:
-------
Configuration: Complete
Services:      Restarted
Routing:       Configured

Test Command:
-------------
Run: omr-test

To view this config: cat /etc/omr-config.txt
ENDCONFIG

chmod 600 /etc/omr-config.txt

echo -e "${BLUE}Configuration Summary:${NC}"
echo -e "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo -e "${CYAN}VPS:${NC}          $VPS_IP:$VPS_PORT"
echo -e "${CYAN}MPTCP:${NC}        Enabled (fullmesh)"
echo -e "${CYAN}WAN Ports:${NC}    $WAN_COUNT interface(s)"
echo -e "${CYAN}Config Saved:${NC} /etc/omr-config.txt"
echo -e "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

echo -e "${YELLOW}Next Steps:${NC}"
echo -e "  ${CYAN}1.${NC} Wait 30 seconds for services to fully start"
echo -e "  ${CYAN}2.${NC} Run ${GREEN}omr-test${NC} to verify connection"
echo -e "  ${CYAN}3.${NC} Check web interface at ${GREEN}http://192.168.100.1${NC}"
echo -e "  ${CYAN}4.${NC} Go to ${GREEN}Status → OpenMPTCProuter${NC} to see connection status"
echo ""

echo -e "${GREEN}Your router is now configured and connecting to your VPS!${NC}"
echo ""
echo -e "${BLUE}Testing connection in 30 seconds...${NC}"
sleep 30

echo ""
echo -e "${CYAN}Running connection test...${NC}"
/usr/bin/omr-test

echo ""
echo -e "${GREEN}Setup complete! Enjoy your bonded internet connection! 🎉${NC}"
echo ""
