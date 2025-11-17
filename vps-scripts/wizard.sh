#!/bin/bash
#
# OpenMPTCProuter Optimized - VPS Installation Wizard
# Copyright (C) 2018-2025 Ycarus (Yannick Chabanois) <ycarus@zugaina.org> for OpenMPTCProuter
# Copyright (C) 2025 spotty118 - OpenMPTCProuter Optimized fork
#
# Single-file, self-contained VPS installation wizard
# Just download and run: No external dependencies during installation!
#
# Usage:
#   wget https://raw.githubusercontent.com/spotty118/openmptcprouter/main/vps-scripts/wizard.sh
#   chmod +x wizard.sh
#   sudo ./wizard.sh
#
# Or one-liner:
#   curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/main/vps-scripts/wizard.sh | sudo bash
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.
#

set -e

# Color codes for beautiful output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Version info
VERSION="1.0.0"
WIZARD_NAME="OpenMPTCProuter Optimized VPS Wizard"

# Clear screen and show banner
clear

echo -e "${CYAN}"
cat << 'EOF'
╔════════════════════════════════════════════════════════════════════════╗
║                                                                        ║
║   ██████╗ ███╗   ███╗██████╗     ██╗   ██╗██████╗ ███████╗           ║
║  ██╔═══██╗████╗ ████║██╔══██╗    ██║   ██║██╔══██╗██╔════╝           ║
║  ██║   ██║██╔████╔██║██████╔╝    ██║   ██║██████╔╝███████╗           ║
║  ██║   ██║██║╚██╔╝██║██╔══██╗    ╚██╗ ██╔╝██╔═══╝ ╚════██║           ║
║  ╚██████╔╝██║ ╚═╝ ██║██║  ██║     ╚████╔╝ ██║     ███████║           ║
║   ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝      ╚═══╝  ╚═╝     ╚══════╝           ║
║                                                                        ║
║                    INSTALLATION WIZARD - OPTIMIZED                     ║
║                                                                        ║
╚════════════════════════════════════════════════════════════════════════╝
EOF
echo -e "${NC}"

echo -e "${GREEN}${WIZARD_NAME} v${VERSION}${NC}"
echo -e "${YELLOW}The easiest way to set up your OpenMPTCProuter VPS!${NC}"
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

# Function to print step headers
print_step() {
    local step_num=$1
    local step_text=$2
    echo ""
    echo -e "${CYAN}┌─────────────────────────────────────────────────────────────┐${NC}"
    echo -e "${CYAN}│${NC} ${GREEN}Step ${step_num}:${NC} ${step_text}"
    echo -e "${CYAN}└─────────────────────────────────────────────────────────────┘${NC}"
}

# Function to print success message
print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

# Function to print error and exit
print_error() {
    echo -e "${RED}✗ Error:${NC} $1" >&2
    exit 1
}

# Function to print warning
print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

# Function to print info
print_info() {
    echo -e "${BLUE}ℹ${NC} $1"
}

# Check if running as root
if [ "$(id -u)" -ne 0 ]; then 
    print_error "This script must be run as root. Please use: sudo $0"
fi

# Detect OS
print_step "1/8" "Detecting System Information"

if [ -f /etc/os-release ]; then
    . /etc/os-release
elif [ -f /usr/lib/os-release ]; then
    . /usr/lib/os-release
else
    print_error "Cannot detect OS version. Unsupported system."
fi

print_info "Operating System: ${GREEN}$PRETTY_NAME${NC}"
print_info "Kernel: ${GREEN}$(uname -r)${NC}"

# Validate OS
print_info "Validating OS compatibility..."

if [ "$ID" = "debian" ]; then
    if [ "$VERSION_ID" != "11" ] && [ "$VERSION_ID" != "12" ] && [ "$VERSION_ID" != "13" ]; then
        print_error "This script requires Debian 11 (Bullseye), 12 (Bookworm), or 13 (Trixie). Current: $PRETTY_NAME"
    fi
elif [ "$ID" = "ubuntu" ]; then
    if [ "$VERSION_ID" != "20.04" ] && [ "$VERSION_ID" != "22.04" ] && [ "$VERSION_ID" != "24.04" ]; then
        print_error "This script requires Ubuntu 20.04, 22.04, or 24.04. Current: $PRETTY_NAME"
    fi
else
    print_error "Unsupported OS: $PRETTY_NAME. Please use Debian 11/12/13 or Ubuntu 20.04/22.04/24.04"
fi

print_success "Operating system is supported!"

# Detect public IP and network interface
print_info "Detecting network configuration..."

VPS_PUBLIC_IP=$(curl -4 -s --max-time 5 ifconfig.me 2>/dev/null || curl -4 -s --max-time 5 icanhazip.com 2>/dev/null || echo "")

if [ -z "$VPS_PUBLIC_IP" ]; then
    print_warning "Could not auto-detect public IP"
    read -p "Please enter your VPS public IP address: " VPS_PUBLIC_IP
    if [ -z "$VPS_PUBLIC_IP" ]; then
        print_error "VPS public IP is required"
    fi
fi

INTERFACE=$(ip -o -4 route show to default | awk '{print $5}' | head -n1)

if [ -z "$INTERFACE" ]; then
    print_warning "Could not auto-detect network interface"
    INTERFACE="eth0"
    print_info "Using default interface: $INTERFACE"
fi

print_success "Network configuration detected"
print_info "Public IP: ${GREEN}$VPS_PUBLIC_IP${NC}"
print_info "Interface: ${GREEN}$INTERFACE${NC}"

# Generate secure passwords and keys
print_step "2/8" "Generating Secure Credentials"

print_info "Creating cryptographically secure passwords..."

KERNEL=${KERNEL:-6.12}
SHADOWSOCKS_PASS=$(head -c 32 /dev/urandom | base64 -w0)
GLORYTUN_PASS=$(od -vN "32" -An -tx1 /dev/urandom | tr '[:lower:]' '[:upper:]' | tr -d " \n")
MLVPN_PASS=$(head -c 32 /dev/urandom | base64 -w0)
DSVPN_PASS=$(od -vN "32" -An -tx1 /dev/urandom | tr '[:lower:]' '[:upper:]' | tr -d " \n")
OMR_ADMIN_PASS=$(od -vN "32" -An -tx1 /dev/urandom | tr '[:lower:]' '[:upper:]' | tr -d " \n")
V2RAY_UUID=$(cat /proc/sys/kernel/random/uuid | tr -d "\n")
XRAY_UUID=$V2RAY_UUID

print_success "Secure credentials generated"

# Ask for user confirmation
echo ""
echo -e "${PURPLE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}Ready to Install OpenMPTCProuter VPS${NC}"
echo -e "${PURPLE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo "This wizard will:"
echo "  ✓ Update system packages"
echo "  ✓ Install VPN services (Shadowsocks, WireGuard, etc.)"
echo "  ✓ Configure kernel for MPTCP multi-WAN bonding"
echo "  ✓ Set up firewall rules"
echo "  ✓ Create configuration files"
echo "  ✓ Generate easy setup webpage"
echo ""
echo -e "${YELLOW}Estimated time: 5-10 minutes${NC}"
echo ""

read -p "Continue with installation? [Y/n]: " -r
# Default to yes if empty (user just presses Enter)
REPLY=${REPLY:-Y}
if [[ ! $REPLY =~ ^[Yy][Ee][Ss]$|^[Yy]$ ]]; then
    echo -e "${YELLOW}Installation cancelled by user.${NC}"
    exit 0
fi

# Update system
print_step "3/8" "Updating System Packages"

export DEBIAN_FRONTEND=noninteractive

print_info "Updating package lists..."
apt-get update -qq || print_error "Failed to update package lists"

print_info "Upgrading installed packages (this may take a few minutes)..."
apt-get upgrade -y -qq || print_warning "Some packages could not be upgraded"

print_success "System packages updated"

# Install required packages
print_step "4/8" "Installing Required Packages"

print_info "Installing network and VPN tools..."

apt-get install -y -qq \
    curl wget git build-essential \
    iptables iptables-persistent \
    net-tools iproute2 ipset \
    dnsutils bind9-dnsutils \
    htop vim nano \
    ca-certificates gnupg lsb-release \
    jq \
    conntrack conntrackd \
    nftables \
    ethtool \
    ifenslave \
    vlan \
    bridge-utils \
    traceroute mtr-tiny \
    tcpdump \
    iperf3 \
    shadowsocks-libev \
    wireguard wireguard-tools \
    python3 || print_error "Failed to install required packages"

print_success "Required packages installed"

# Load kernel modules
print_step "5/8" "Configuring Kernel Modules"

print_info "Loading kernel modules for multi-WAN bonding..."

modprobe bonding 2>/dev/null || print_warning "bonding module already loaded or not available"
modprobe 8021q 2>/dev/null || print_warning "8021q module already loaded or not available"
modprobe nf_conntrack 2>/dev/null || print_warning "nf_conntrack module already loaded or not available"
modprobe xt_TCPMSS 2>/dev/null || print_warning "xt_TCPMSS module already loaded or not available"
modprobe xt_mark 2>/dev/null || print_warning "xt_mark module already loaded or not available"
modprobe xt_multiport 2>/dev/null || print_warning "xt_multiport module already loaded or not available"
modprobe sch_fq_codel 2>/dev/null || print_warning "sch_fq_codel module already loaded or not available"
modprobe sch_cake 2>/dev/null || print_warning "sch_cake module already loaded or not available"
modprobe sch_htb 2>/dev/null || print_warning "sch_htb module already loaded or not available"

# Make modules load on boot
cat > /etc/modules-load.d/openmptcprouter.conf << 'MODULES'
# OpenMPTCProuter Multi-WAN Bonding Modules
bonding
8021q
nf_conntrack
xt_TCPMSS
xt_mark
xt_multiport
sch_fq_codel
sch_cake
sch_htb
MODULES

print_success "Kernel modules configured"

# Configure kernel parameters
print_step "6/8" "Optimizing Kernel Parameters for MPTCP"

print_info "Applying kernel optimizations for multi-WAN performance..."

# Backup existing sysctl.conf
cp /etc/sysctl.conf /etc/sysctl.conf.backup.$(date +%Y%m%d-%H%M%S) 2>/dev/null || true

# Configure kernel parameters
cat > /etc/sysctl.d/99-openmptcprouter.conf << 'SYSCTL'
# OpenMPTCProuter Optimized - Kernel Configuration

# Enable IP forwarding
net.ipv4.ip_forward = 1
net.ipv6.conf.all.forwarding = 1

# MPTCP Configuration - Enhanced for Multi-WAN Bonding
net.mptcp.mptcp_enabled = 1
net.mptcp.mptcp_checksum = 0
net.mptcp.mptcp_debug = 0
net.mptcp.mptcp_syn_retries = 3
net.mptcp.mptcp_path_manager = fullmesh
net.mptcp.mptcp_scheduler = default

# BBR2 Congestion Control
net.ipv4.tcp_congestion_control = bbr2
net.core.default_qdisc = fq_codel

# Network Performance Tuning - Enhanced for Multi-WAN
net.core.rmem_max = 134217728
net.core.wmem_max = 134217728
net.core.rmem_default = 67108864
net.core.wmem_default = 67108864
net.core.netdev_max_backlog = 250000
net.core.somaxconn = 4096
net.core.optmem_max = 65536

# TCP Performance - Optimized for Multiple Connections
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
net.ipv4.tcp_moderate_rcvbuf = 1

# Enable TCP Fast Open
net.ipv4.tcp_fastopen = 3

# Connection Tracking - Enhanced for Multi-WAN
net.netfilter.nf_conntrack_max = 262144
net.netfilter.nf_conntrack_tcp_timeout_established = 432000
net.netfilter.nf_conntrack_tcp_timeout_time_wait = 30
net.netfilter.nf_conntrack_tcp_timeout_close_wait = 15
net.netfilter.nf_conntrack_tcp_timeout_fin_wait = 30

# Multi-path routing enhancements
net.ipv4.fib_multipath_hash_policy = 1
net.ipv4.fib_multipath_use_neigh = 1

# TCP optimizations for bonding
net.ipv4.tcp_no_metrics_save = 1
net.ipv4.tcp_ecn = 0
net.ipv4.tcp_frto = 2
net.ipv4.tcp_mtu_probing = 1
net.ipv4.tcp_rfc1337 = 1
net.ipv4.tcp_sack = 1
net.ipv4.tcp_fack = 1
net.ipv4.tcp_timestamps = 1

# Increase connection tracking table size for multi-WAN
net.nf_conntrack_max = 262144

# Security
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1

# IPv6 Security
net.ipv6.conf.all.accept_redirects = 0
net.ipv6.conf.all.accept_source_route = 0

# Kernel panic behavior for stability
kernel.panic = 10
kernel.panic_on_oops = 1

# Memory and file system optimizations
vm.swappiness = 10
vm.dirty_ratio = 60
vm.dirty_background_ratio = 2

# File descriptor limits
fs.file-max = 2097152
SYSCTL

# Apply sysctl settings
sysctl -p /etc/sysctl.d/99-openmptcprouter.conf > /dev/null 2>&1 || print_warning "Some kernel parameters could not be applied"

print_success "Kernel parameters optimized"

# Configure firewall
print_step "7/8" "Configuring Firewall"

print_info "Setting up iptables firewall rules..."

# Backup existing iptables rules
iptables-save > /root/iptables-backup-$(date +%Y%m%d-%H%M%S).rules 2>/dev/null || true

# Create firewall rules
cat > /etc/iptables/rules.v4 << IPTABLES
*filter
:INPUT DROP [0:0]
:FORWARD DROP [0:0]
:OUTPUT ACCEPT [0:0]

# Allow loopback
-A INPUT -i lo -j ACCEPT

# Allow established connections
-A INPUT -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT
-A FORWARD -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT

# Allow SSH
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
-A INPUT -p tcp --dport 8080 -j ACCEPT

# Forward traffic from VPN to internet
-A FORWARD -i tun+ -o $INTERFACE -j ACCEPT
-A FORWARD -i mlvpn+ -o $INTERFACE -j ACCEPT

COMMIT

*nat
:PREROUTING ACCEPT [0:0]
:INPUT ACCEPT [0:0]
:OUTPUT ACCEPT [0:0]
:POSTROUTING ACCEPT [0:0]

# NAT for VPN traffic
-A POSTROUTING -o $INTERFACE -j MASQUERADE

COMMIT
IPTABLES

# Apply iptables rules
iptables-restore < /etc/iptables/rules.v4 || print_warning "Some iptables rules could not be applied"

print_success "Firewall configured"

# Configure VPN services
print_step "8/8" "Configuring VPN Services"

print_info "Setting up Shadowsocks..."

# Create configuration directory
mkdir -p /etc/openmptcprouter
mkdir -p /etc/shadowsocks-libev

# Create Shadowsocks configuration
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
systemctl enable shadowsocks-libev-server@config > /dev/null 2>&1 || print_warning "Could not enable Shadowsocks service"
systemctl restart shadowsocks-libev-server@config > /dev/null 2>&1 || print_warning "Could not start Shadowsocks service"

print_success "Shadowsocks configured"

# Save configuration
print_info "Saving configuration files..."

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

print_success "Configuration saved"

# Create setup web page
print_info "Creating setup web page..."

mkdir -p /var/www/omr-setup

cat > /var/www/omr-setup/index.html << 'ENDHTML'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>OpenMPTCProuter - VPS Setup Complete</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 900px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 40px;
            text-align: center;
        }
        .header h1 { font-size: 2.5em; margin-bottom: 10px; }
        .header p { opacity: 0.9; font-size: 1.1em; }
        .content { padding: 40px; }
        .success-banner {
            background: #d4edda;
            border: 2px solid #28a745;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 30px;
            text-align: center;
        }
        .success-banner h2 {
            color: #155724;
            font-size: 1.8em;
            margin-bottom: 10px;
        }
        .section {
            margin-bottom: 30px;
            padding: 25px;
            background: #f8f9fa;
            border-radius: 10px;
            border-left: 5px solid #667eea;
        }
        .section h2 {
            color: #667eea;
            margin-bottom: 20px;
            font-size: 1.5em;
        }
        .config-grid {
            display: grid;
            gap: 15px;
        }
        .config-item {
            padding: 15px;
            background: white;
            border-radius: 8px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        .config-label {
            font-weight: 600;
            color: #333;
            font-size: 1.1em;
        }
        .config-value {
            font-family: 'Courier New', monospace;
            color: #667eea;
            font-weight: bold;
            padding: 8px 12px;
            background: #f0f0f0;
            border-radius: 5px;
            cursor: pointer;
            transition: background 0.3s;
        }
        .config-value:hover {
            background: #e0e0e0;
        }
        .copy-btn {
            background: #667eea;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 1em;
            margin-left: 15px;
            transition: background 0.3s;
        }
        .copy-btn:hover {
            background: #5568d3;
        }
        .step {
            background: white;
            padding: 20px;
            margin: 15px 0;
            border-radius: 8px;
            border-left: 4px solid #28a745;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        .step-number {
            display: inline-block;
            background: #28a745;
            color: white;
            width: 35px;
            height: 35px;
            line-height: 35px;
            text-align: center;
            border-radius: 50%;
            margin-right: 15px;
            font-weight: bold;
            font-size: 1.1em;
        }
        .warning-box {
            background: #fff3cd;
            border-left-color: #ffc107;
            color: #856404;
            padding: 20px;
            border-radius: 8px;
            margin: 20px 0;
            border-left: 4px solid #ffc107;
        }
        .btn-primary {
            background: #667eea;
            color: white;
            border: none;
            padding: 12px 30px;
            border-radius: 8px;
            cursor: pointer;
            font-size: 1.1em;
            margin: 10px 5px;
            transition: background 0.3s;
        }
        .btn-primary:hover {
            background: #5568d3;
        }
        @media print {
            body { background: white; }
            .no-print { display: none; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🚀 VPS Setup Complete!</h1>
            <p>Your OpenMPTCProuter Optimized server is ready to use</p>
        </div>
        
        <div class="content">
            <div class="success-banner">
                <h2>✅ Installation Successful!</h2>
                <p>Your VPS has been configured for multi-WAN bonding</p>
            </div>

            <div class="section">
                <h2>📋 Server Connection Details</h2>
                <div class="config-grid">
                    <div class="config-item">
                        <span class="config-label">🌐 Server IP:</span>
                        <div>
                            <span class="config-value" id="server-ip">REPLACE_VPS_IP</span>
                            <button class="copy-btn" onclick="copyValue('server-ip')">📋 Copy</button>
                        </div>
                    </div>
                    <div class="config-item">
                        <span class="config-label">🔌 Port:</span>
                        <div>
                            <span class="config-value">65500</span>
                            <button class="copy-btn" onclick="copyText('65500')">📋 Copy</button>
                        </div>
                    </div>
                    <div class="config-item">
                        <span class="config-label">🔑 Password:</span>
                        <div>
                            <span class="config-value" id="password">REPLACE_PASSWORD</span>
                            <button class="copy-btn" onclick="copyValue('password')">📋 Copy</button>
                        </div>
                    </div>
                    <div class="config-item">
                        <span class="config-label">🔐 Encryption:</span>
                        <div>
                            <span class="config-value">Shadowsocks (chacha20-ietf-poly1305)</span>
                        </div>
                    </div>
                </div>
            </div>

            <div class="section">
                <h2>📱 Router Configuration (3 Easy Steps)</h2>
                
                <div class="step">
                    <span class="step-number">1</span>
                    <strong>Access Your Router</strong><br><br>
                    • Connect to router via WiFi or Ethernet<br>
                    • Open browser to: <code style="background:#f0f0f0;padding:8px;border-radius:5px">http://192.168.2.1</code><br>
                    • Login (default username: <strong>root</strong>, no password)
                </div>

                <div class="step">
                    <span class="step-number">2</span>
                    <strong>Navigate to VPN Settings</strong><br><br>
                    • Go to: <strong>Services → OpenMPTCProuter</strong><br>
                    • Or: <strong>VPN → OpenMPTCProuter Configuration</strong>
                </div>

                <div class="step">
                    <span class="step-number">3</span>
                    <strong>Enter Connection Details</strong><br><br>
                    • Server IP: <strong><span style="color:#667eea" id="ip-display">REPLACE_VPS_IP</span></strong><br>
                    • Port: <strong>65500</strong><br>
                    • Password: <strong><span style="color:#667eea" id="pass-display">REPLACE_PASSWORD</span></strong><br>
                    • Encryption: <strong>Shadowsocks</strong><br>
                    • Click <strong>"Save & Apply"</strong>
                </div>
            </div>

            <div class="section">
                <h2>✅ Verify Connection</h2>
                <div class="step">
                    <span class="step-number">✓</span>
                    <strong>After 30 seconds, check status:</strong><br><br>
                    • Go to: <strong>Status → OpenMPTCProuter</strong><br>
                    • Connection status should show: <span style="background:#28a745;color:white;padding:5px 10px;border-radius:5px">Connected</span><br>
                    • You should see bandwidth aggregation from multiple WANs
                </div>
            </div>

            <div class="warning-box">
                <h3 style="margin-bottom:10px">🔐 Security Important!</h3>
                <p><strong>Save these credentials securely!</strong></p>
                <p style="margin-top:10px">Anyone with this password can access your VPS. Never share publicly.</p>
                <div style="margin-top:15px">
                    <button class="btn-primary no-print" onclick="window.print()">🖨️ Print This Page</button>
                    <button class="btn-primary no-print" onclick="downloadCredentials()">💾 Download Credentials</button>
                </div>
            </div>

            <div class="section">
                <h2>📚 Additional Resources</h2>
                <ul style="margin-left:20px;line-height:2">
                    <li>📖 <a href="https://github.com/spotty118/openmptcprouter/blob/main/SETUP_GUIDE.md" target="_blank">Complete Setup Guide</a></li>
                    <li>🐛 <a href="https://github.com/spotty118/openmptcprouter/issues" target="_blank">Report Issues</a></li>
                    <li>💬 <a href="https://github.com/spotty118/openmptcprouter/discussions" target="_blank">Community Support</a></li>
                    <li>⭐ <a href="https://github.com/spotty118/openmptcprouter" target="_blank">Star the Project</a></li>
                </ul>
            </div>
        </div>
    </div>

    <script>
        function copyValue(elementId) {
            const text = document.getElementById(elementId).textContent;
            copyText(text);
        }
        
        function copyText(text) {
            navigator.clipboard.writeText(text).then(() => {
                alert('✅ Copied to clipboard!');
            }).catch(() => {
                alert('❌ Failed to copy. Please copy manually.');
            });
        }

        function downloadCredentials() {
            const ip = document.getElementById('server-ip').textContent;
            const password = document.getElementById('password').textContent;
            const content = `OpenMPTCProuter Optimized - Connection Details
============================================

Server IP: ${ip}
Port: 65500
Password: ${password}
Encryption: Shadowsocks (chacha20-ietf-poly1305)

Quick Setup:
1. Access router at http://192.168.2.1
2. Go to Services → OpenMPTCProuter
3. Enter above details
4. Save & Apply

IMPORTANT: Keep this file secure!
`;
            const blob = new Blob([content], { type: 'text/plain' });
            const url = window.URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = 'openmptcprouter-credentials.txt';
            document.body.appendChild(a);
            a.click();
            window.URL.revokeObjectURL(url);
            document.body.removeChild(a);
        }
    </script>
</body>
</html>
ENDHTML

# Replace placeholders
sed -i "s/REPLACE_VPS_IP/$VPS_PUBLIC_IP/g" /var/www/omr-setup/index.html
sed -i "s/REPLACE_PASSWORD/$SHADOWSOCKS_PASS/g" /var/www/omr-setup/index.html

# Create systemd service for web interface
cat > /etc/systemd/system/omr-setup-web.service << 'ENDSERVICE'
[Unit]
Description=OpenMPTCProuter Setup Web Interface
After=network.target

[Service]
Type=simple
WorkingDirectory=/var/www/omr-setup
ExecStart=/usr/bin/python3 -m http.server 8080
Restart=always

[Install]
WantedBy=multi-user.target
ENDSERVICE

systemctl daemon-reload
systemctl enable omr-setup-web > /dev/null 2>&1
systemctl restart omr-setup-web

print_success "Setup web page created"

# Installation complete
clear

echo -e "${GREEN}"
cat << 'EOF'
╔════════════════════════════════════════════════════════════════════════╗
║                                                                        ║
║                  ✓✓✓ INSTALLATION COMPLETE! ✓✓✓                       ║
║                                                                        ║
╚════════════════════════════════════════════════════════════════════════╝
EOF
echo -e "${NC}"

echo ""
echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}   Your VPS is Ready for OpenMPTCProuter!${NC}"
echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

echo -e "${PURPLE}📋 Connection Details:${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${CYAN}Server IP:${NC}     ${GREEN}$VPS_PUBLIC_IP${NC}"
echo -e "${CYAN}Port:${NC}          ${GREEN}65500${NC}"
echo -e "${CYAN}Password:${NC}      ${GREEN}$SHADOWSOCKS_PASS${NC}"
echo -e "${CYAN}Encryption:${NC}    ${GREEN}Shadowsocks (chacha20-ietf-poly1305)${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

echo ""
echo -e "${YELLOW}🌐 Easy Setup Web Page:${NC}"
echo -e "   ${GREEN}http://$VPS_PUBLIC_IP:8080${NC}"
echo ""
echo -e "   ${PURPLE}Open this in your browser for:${NC}"
echo -e "   • Step-by-step router setup guide"
echo -e "   • Copy-paste ready configuration"
echo -e "   • Printable credentials"
echo ""

echo -e "${YELLOW}📁 Credentials Also Saved To:${NC}"
echo -e "   ${CYAN}/root/openmptcprouter_credentials.txt${NC}"
echo -e "   ${CYAN}/etc/openmptcprouter/config.json${NC}"
echo ""

echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}Next Steps:${NC}"
echo -e "  ${PURPLE}1.${NC} Open ${GREEN}http://$VPS_PUBLIC_IP:8080${NC} in your browser"
echo -e "  ${PURPLE}2.${NC} Follow the 3-step router setup instructions"
echo -e "  ${PURPLE}3.${NC} Enjoy multi-WAN bonding and aggregation!"
echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

echo -e "${GREEN}🎉 Thank you for using OpenMPTCProuter Optimized!${NC}"
echo ""
echo -e "${BLUE}Documentation:${NC} https://github.com/spotty118/openmptcprouter"
echo -e "${BLUE}Support:${NC}       https://github.com/spotty118/openmptcprouter/discussions"
echo ""
