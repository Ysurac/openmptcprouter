#!/bin/bash
#
# OpenMPTCProuter Optimized - VPS Installation Script
# Copyright (C) 2018-2025 Ycarus (Yannick Chabanois) <ycarus@zugaina.org> for OpenMPTCProuter
# Copyright (C) 2025 spotty118 - OpenMPTCProuter Optimized fork
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.
#

set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Banner
echo -e "${BLUE}"
cat << 'EOF'
   ___                   __  __ ___ _____ ___ ___            _            
  / _ \ _ __   ___ _ __ |  \/  |  _ \_   _/ __| _ \_ _ ___ | |_ ___ _ _ 
 | | | | '_ \ / _ \ '_ \| |\/| | |_) || || (__| |_) | '_/ _ \  _/ -_) '_|
 | |_| | |_) |  __/ | | | |  | |  __/ | | \___| .__/|_| \___/\__\___|_|  
  \___/| .__/ \___|_| |_|_|  |_|_|    |_|  \___/_|                       
       |_|    ___       _   _           _              _                    
              / _ \ _ __ | |_(_)_ __ ___ (_)_____   _  __| |                   
             | | | | '_ \| __| | '_ ` _ \| |_  / | | |/ _` |                   
             | |_| | |_) | |_| | | | | | | |/ /| |_| | (_| |                   
              \___/| .__/ \__|_|_| |_| |_|_/___|\__,_|\__,_|                   
                   |_|                                                          

EOF
echo -e "${NC}"
echo -e "${GREEN}OpenMPTCProuter Optimized - Easy VPS Setup Script${NC}"
echo -e "${BLUE}Version: 1.0 - Optimized Edition${NC}"
echo ""

# Check if running as root
if [ "$(id -u)" -ne 0 ]; then 
    echo -e "${RED}Error: This script must be run as root${NC}" >&2
    exit 1
fi

# Default configuration
KERNEL=${KERNEL:-6.12}
SHADOWSOCKS_PASS=${SHADOWSOCKS_PASS:-$(head -c 32 /dev/urandom | base64 -w0)}
GLORYTUN_PASS=${GLORYTUN_PASS:-$(od -vN "32" -An -tx1 /dev/urandom | tr '[:lower:]' '[:upper:]' | tr -d " \n")}
MLVPN_PASS=${MLVPN_PASS:-$(head -c 32 /dev/urandom | base64 -w0)}
DSVPN_PASS=${DSVPN_PASS:-$(od -vN "32" -An -tx1 /dev/urandom | tr '[:lower:]' '[:upper:]' | tr -d " \n")}
OMR_ADMIN_PASS=${OMR_ADMIN_PASS:-$(od -vN "32" -An -tx1 /dev/urandom | tr '[:lower:]' '[:upper:]' | tr -d " \n")}
V2RAY_UUID=${V2RAY_UUID:-$(cat /proc/sys/kernel/random/uuid | tr -d "\n")}
XRAY_UUID=${XRAY_UUID:-$V2RAY_UUID}

VPS_PUBLIC_IP=${VPS_PUBLIC_IP:-$(curl -4 -s ifconfig.me || curl -4 -s icanhazip.com)}
INTERFACE=${INTERFACE:-$(ip -o -4 route show to default | awk '{print $5}' | head -n1)}

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
elif [ -f /usr/lib/os-release ]; then
    . /usr/lib/os-release
else
    echo -e "${RED}Error: Cannot detect OS version${NC}"
    exit 1
fi

echo -e "${YELLOW}Detected OS: $ID $VERSION_ID${NC}"
echo -e "${YELLOW}Public IP: $VPS_PUBLIC_IP${NC}"
echo -e "${YELLOW}Interface: $INTERFACE${NC}"
echo ""

# Check supported OS
if [ "$ID" = "debian" ]; then
    if [ "$VERSION_ID" != "11" ] && [ "$VERSION_ID" != "12" ] && [ "$VERSION_ID" != "13" ]; then
        echo -e "${RED}Error: This script requires Debian 11 (Bullseye), 12 (Bookworm), or 13 (Trixie)${NC}"
        exit 1
    fi
elif [ "$ID" = "ubuntu" ]; then
    if [ "$VERSION_ID" != "20.04" ] && [ "$VERSION_ID" != "22.04" ] && [ "$VERSION_ID" != "24.04" ]; then
        echo -e "${RED}Error: This script requires Ubuntu 20.04, 22.04, or 24.04${NC}"
        exit 1
    fi
else
    echo -e "${RED}Error: Unsupported OS. Use Debian 11/12/13 or Ubuntu 20.04/22.04/24.04${NC}"
    exit 1
fi

# Interactive setup
echo -e "${GREEN}=== OpenMPTCProuter Optimized VPS Setup ===${NC}"
echo ""
echo "This script will install and configure your VPS for use with OpenMPTCProuter Optimized."
echo ""

# Ask for confirmation
read -p "Do you want to proceed with the installation? (yes/no): " -r
if [[ ! $REPLY =~ ^[Yy][Ee][Ss]$|^[Yy]$ ]]; then
    echo -e "${YELLOW}Installation cancelled.${NC}"
    exit 0
fi

echo ""
echo -e "${GREEN}Step 1/6: Updating system packages...${NC}"
export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
apt-get upgrade -y -qq

echo -e "${GREEN}Step 2/6: Installing required packages...${NC}"
apt-get install -y -qq \
    curl wget git build-essential \
    iptables iptables-persistent \
    net-tools iproute2 ipset \
    dnsutils bind9-dnsutils \
    htop vim nano \
    ca-certificates gnupg lsb-release \
    jq

echo -e "${GREEN}Step 3/6: Configuring kernel parameters for MPTCP and BBR2...${NC}"

# Backup existing sysctl.conf
cp /etc/sysctl.conf /etc/sysctl.conf.backup

# Configure kernel parameters
cat > /etc/sysctl.d/99-openmptcprouter.conf << 'SYSCTL'
# OpenMPTCProuter Optimized - Kernel Configuration

# Enable IP forwarding
net.ipv4.ip_forward = 1
net.ipv6.conf.all.forwarding = 1

# MPTCP Configuration
net.mptcp.mptcp_enabled = 1
net.mptcp.mptcp_checksum = 0
net.mptcp.mptcp_debug = 0
net.mptcp.mptcp_syn_retries = 3

# BBR2 Congestion Control
net.ipv4.tcp_congestion_control = bbr2
net.core.default_qdisc = fq_codel

# Network Performance Tuning
net.core.rmem_max = 134217728
net.core.wmem_max = 134217728
net.core.rmem_default = 67108864
net.core.wmem_default = 67108864
net.core.netdev_max_backlog = 250000
net.core.somaxconn = 4096

# TCP Performance
net.ipv4.tcp_rmem = 4096 87380 67108864
net.ipv4.tcp_wmem = 4096 65536 67108864
net.ipv4.tcp_max_syn_backlog = 8192
net.ipv4.tcp_slow_start_after_idle = 0
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_fin_timeout = 15
net.ipv4.tcp_keepalive_time = 300
net.ipv4.tcp_keepalive_probes = 5
net.ipv4.tcp_keepalive_intvl = 15

# Optimize TCP window size
net.ipv4.tcp_window_scaling = 1
net.ipv4.tcp_adv_win_scale = 1

# Enable TCP Fast Open
net.ipv4.tcp_fastopen = 3

# Security
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1

# IPv6 Security
net.ipv6.conf.all.accept_redirects = 0
net.ipv6.conf.all.accept_source_route = 0
SYSCTL

# Apply sysctl settings
sysctl -p /etc/sysctl.d/99-openmptcprouter.conf > /dev/null

echo -e "${GREEN}Step 4/6: Configuring firewall rules...${NC}"

# Backup existing iptables rules
iptables-save > /root/iptables-backup-$(date +%Y%m%d-%H%M%S).rules 2>/dev/null || true

# Basic firewall configuration
cat > /etc/iptables/rules.v4 << 'IPTABLES'
*filter
:INPUT DROP [0:0]
:FORWARD DROP [0:0]
:OUTPUT ACCEPT [0:0]

# Allow loopback
-A INPUT -i lo -j ACCEPT

# Allow established connections
-A INPUT -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT
-A FORWARD -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT

# Allow SSH (be careful with this!)
-A INPUT -p tcp --dport 22 -j ACCEPT

# Allow OpenMPTCProuter ports
-A INPUT -p tcp --dport 65500 -j ACCEPT
-A INPUT -p udp --dport 65500 -j ACCEPT
-A INPUT -p tcp --dport 65510 -j ACCEPT
-A INPUT -p udp --dport 65510 -j ACCEPT
-A INPUT -p tcp --dport 65520 -j ACCEPT
-A INPUT -p udp --dport 65520 -j ACCEPT

# Allow ICMP (ping)
-A INPUT -p icmp -j ACCEPT

# Allow DNS
-A INPUT -p udp --dport 53 -j ACCEPT
-A INPUT -p tcp --dport 53 -j ACCEPT

# Allow HTTP/HTTPS for web admin
-A INPUT -p tcp --dport 80 -j ACCEPT
-A INPUT -p tcp --dport 443 -j ACCEPT

# Forward traffic from VPN to internet
-A FORWARD -i tun+ -o INTERFACE_PLACEHOLDER -j ACCEPT
-A FORWARD -i mlvpn+ -o INTERFACE_PLACEHOLDER -j ACCEPT

COMMIT

*nat
:PREROUTING ACCEPT [0:0]
:INPUT ACCEPT [0:0]
:OUTPUT ACCEPT [0:0]
:POSTROUTING ACCEPT [0:0]

# NAT for VPN traffic
-A POSTROUTING -o INTERFACE_PLACEHOLDER -j MASQUERADE

COMMIT
IPTABLES

# Replace placeholder with actual interface
sed -i "s/INTERFACE_PLACEHOLDER/$INTERFACE/g" /etc/iptables/rules.v4

# Apply iptables rules
iptables-restore < /etc/iptables/rules.v4

echo -e "${GREEN}Step 5/6: Installing VPN software...${NC}"

# Install Shadowsocks
if ! command -v ss-server &> /dev/null; then
    echo "Installing Shadowsocks-libev..."
    apt-get install -y -qq shadowsocks-libev
fi

# Install WireGuard
if ! command -v wg &> /dev/null; then
    echo "Installing WireGuard..."
    apt-get install -y -qq wireguard wireguard-tools
fi

# Create configuration directory
mkdir -p /etc/openmptcprouter

echo -e "${GREEN}Step 6/6: Generating configuration files...${NC}"

# Create configuration file
cat > /etc/openmptcprouter/config.json << ENDCONFIG
{
  "version": "1.0-optimized",
  "public_ip": "$VPS_PUBLIC_IP",
  "interface": "$INTERFACE",
  "kernel_version": "$KERNEL",
  "credentials": {
    "shadowsocks_password": "$SHADOWSOCKS_PASS",
    "glorytun_password": "$GLORYTUN_PASS",
    "mlvpn_password": "$MLVPN_PASS",
    "dsvpn_password": "$DSVPN_PASS",
    "admin_password": "$OMR_ADMIN_PASS",
    "v2ray_uuid": "$V2RAY_UUID",
    "xray_uuid": "$XRAY_UUID"
  },
  "ports": {
    "shadowsocks": 65500,
    "glorytun_tcp": 65510,
    "glorytun_udp": 65520,
    "mlvpn": 65530,
    "admin_web": 8080
  }
}
ENDCONFIG

chmod 600 /etc/openmptcprouter/config.json

# Create Shadowsocks configuration
mkdir -p /etc/shadowsocks-libev
cat > /etc/shadowsocks-libev/config.json << ENDSS
{
  "server": "0.0.0.0",
  "server_port": 65500,
  "password": "$SHADOWSOCKS_PASS",
  "timeout": 600,
  "method": "chacha20-ietf-poly1305",
  "fast_open": true,
  "mode": "tcp_and_udp",
  "plugin": "",
  "plugin_opts": ""
}
ENDSS

# Enable and start Shadowsocks
systemctl enable shadowsocks-libev-server@config || true
systemctl restart shadowsocks-libev-server@config || true

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Installation Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo -e "${BLUE}VPS Configuration Summary:${NC}"
echo -e "Public IP: ${YELLOW}$VPS_PUBLIC_IP${NC}"
echo -e "Interface: ${YELLOW}$INTERFACE${NC}"
echo -e "Kernel: ${YELLOW}$KERNEL${NC}"
echo ""
echo -e "${BLUE}Connection Details (save these securely):${NC}"
echo -e "Shadowsocks Password: ${YELLOW}$SHADOWSOCKS_PASS${NC}"
echo -e "Glorytun Password: ${YELLOW}$GLORYTUN_PASS${NC}"
echo -e "MLVPN Password: ${YELLOW}$MLVPN_PASS${NC}"
echo -e "Admin Password: ${YELLOW}$OMR_ADMIN_PASS${NC}"
echo -e "V2Ray/Xray UUID: ${YELLOW}$V2RAY_UUID${NC}"
echo ""
echo -e "${BLUE}Configuration file saved to:${NC} ${YELLOW}/etc/openmptcprouter/config.json${NC}"
echo ""
echo -e "${GREEN}Next Steps:${NC}"
echo "1. Configure your OpenMPTCProuter Optimized router with these details"
echo "2. Set server IP to: $VPS_PUBLIC_IP"
echo "3. Use Shadowsocks password: $SHADOWSOCKS_PASS"
echo "4. Default port: 65500"
echo ""
echo -e "${YELLOW}Note: Configuration has been saved to /etc/openmptcprouter/config.json${NC}"
echo -e "${YELLOW}Keep this file secure as it contains all your passwords!${NC}"
echo ""

# Save credentials to file for later reference
cat > /root/openmptcprouter_credentials.txt << ENDCREDS
OpenMPTCProuter Optimized - VPS Credentials
============================================
Installation Date: $(date)
VPS IP: $VPS_PUBLIC_IP
Interface: $INTERFACE

PASSWORDS (keep secure!):
=========================
Shadowsocks: $SHADOWSOCKS_PASS
Glorytun: $GLORYTUN_PASS
MLVPN: $MLVPN_PASS
DSVPN: $DSVPN_PASS
Admin: $OMR_ADMIN_PASS
V2Ray/Xray UUID: $V2RAY_UUID

PORTS:
======
Shadowsocks: 65500
Glorytun TCP: 65510
Glorytun UDP: 65520
MLVPN: 65530
Admin Web: 8080

QUICK START:
============
On your router:
1. Go to Services → OpenMPTCProuter
2. Server IP: $VPS_PUBLIC_IP
3. Server Port: 65500
4. Password: $SHADOWSOCKS_PASS
5. Encryption: Shadowsocks (chacha20-ietf-poly1305)
6. Save & Apply
ENDCREDS

chmod 600 /root/openmptcprouter_credentials.txt

echo -e "${GREEN}Credentials also saved to: ${YELLOW}/root/openmptcprouter_credentials.txt${NC}"
echo ""
echo -e "${GREEN}For support and documentation:${NC}"
echo -e "https://github.com/spotty118/openmptcprouter"
echo ""
