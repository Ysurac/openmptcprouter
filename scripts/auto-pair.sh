#!/bin/bash
#
# OpenMPTCProuter Optimized - Automatic Bidirectional Setup
# This script sets up BOTH server and client with automatic pairing
#
# Run on VPS to get pairing code, then use that code on router
#

set -e

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

clear

echo -e "${CYAN}"
cat << 'EOF'
╔═══════════════════════════════════════════════════════════╗
║                                                           ║
║     OpenMPTCProuter Optimized - Auto-Pairing System      ║
║                                                           ║
║         Server ←→ Client Automatic Configuration         ║
║                                                           ║
╚═══════════════════════════════════════════════════════════╝
EOF
echo -e "${NC}"

# Detect if running on VPS or Router
if [ -f /etc/openwrt_release ]; then
    DEVICE_TYPE="router"
    echo -e "${GREEN}Detected: OpenWrt Router (Client)${NC}"
elif [ -f /etc/os-release ]; then
    . /etc/os-release
    if [ "$ID" = "debian" ] || [ "$ID" = "ubuntu" ]; then
        DEVICE_TYPE="vps"
        echo -e "${GREEN}Detected: VPS Server${NC}"
    else
        echo -e "${RED}Error: Unsupported system${NC}"
        exit 1
    fi
else
    echo -e "${RED}Error: Cannot detect system type${NC}"
    exit 1
fi

echo ""

#############################################
# VPS SERVER SETUP WITH PAIRING CODE
#############################################

if [ "$DEVICE_TYPE" = "vps" ]; then
    echo -e "${BLUE}═══════════════════════════════════════${NC}"
    echo -e "${YELLOW}      VPS Auto-Pairing Setup${NC}"
    echo -e "${BLUE}═══════════════════════════════════════${NC}"
    echo ""
    
    # Check root
    if [ "$(id -u)" -ne 0 ]; then
        echo -e "${RED}Error: Must run as root${NC}"
        exit 1
    fi
    
    # Get VPS IP
    VPS_IP=$(curl -4 -s --max-time 5 ifconfig.me 2>/dev/null || curl -4 -s --max-time 5 icanhazip.com 2>/dev/null)
    
    if [ -z "$VPS_IP" ]; then
        echo -e "${YELLOW}Could not auto-detect IP. Please enter manually:${NC}"
        read -r -p "VPS Public IP: " VPS_IP < /dev/tty
    fi
    
    echo -e "${CYAN}Detected VPS IP:${NC} ${GREEN}$VPS_IP${NC}"
    echo ""
    
    # Generate credentials
    echo -e "${CYAN}Generating secure credentials...${NC}"
    SHADOWSOCKS_PASS=$(head -c 32 /dev/urandom | base64 -w0)
    GLORYTUN_PASS=$(od -vN "32" -An -tx1 /dev/urandom | tr '[:lower:]' '[:upper:]' | tr -d " \n")
    MLVPN_PASS=$(head -c 32 /dev/urandom | base64 -w0)
    SETUP_TOKEN=$(head -c 16 /dev/urandom | base64 -w0 | tr -d '/+=')
    
    PORT=65500
    
    echo -e "${GREEN}✓ Credentials generated${NC}"
    echo ""
    
    # Quick VPS setup
    echo -e "${CYAN}Installing and configuring VPS...${NC}"
    echo ""
    
    # Install required packages
    export DEBIAN_FRONTEND=noninteractive
    apt-get update -qq > /dev/null 2>&1
    apt-get install -y -qq curl wget jq shadowsocks-libev iptables python3 > /dev/null 2>&1
    
    # Configure kernel
    cat > /etc/sysctl.d/99-omr-autopair.conf << 'SYSCTL'
net.ipv4.ip_forward = 1
net.ipv6.conf.all.forwarding = 1
net.mptcp.mptcp_enabled = 1
net.ipv4.tcp_congestion_control = bbr
net.core.default_qdisc = fq_codel
net.core.rmem_max = 134217728
net.core.wmem_max = 134217728
net.ipv4.tcp_rmem = 4096 87380 67108864
net.ipv4.tcp_wmem = 4096 65536 67108864
SYSCTL
    
    sysctl -p /etc/sysctl.d/99-omr-autopair.conf > /dev/null 2>&1
    
    # Configure Shadowsocks
    mkdir -p /etc/shadowsocks-libev
    cat > /etc/shadowsocks-libev/config.json << ENDSS
{
  "server": "0.0.0.0",
  "server_port": $PORT,
  "password": "$SHADOWSOCKS_PASS",
  "timeout": 600,
  "method": "chacha20-ietf-poly1305",
  "fast_open": true,
  "mode": "tcp_and_udp"
}
ENDSS
    
    # Firewall
    INTERFACE=$(ip -o -4 route show to default | awk '{print $5}' | head -n1)
    
    iptables -F > /dev/null 2>&1 || true
    iptables -t nat -F > /dev/null 2>&1 || true
    
    # Allow established connections
    iptables -A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT
    iptables -A INPUT -i lo -j ACCEPT
    
    # Allow SSH
    iptables -A INPUT -p tcp --dport 22 -j ACCEPT
    
    # Allow OMR ports
    iptables -A INPUT -p tcp --dport $PORT -j ACCEPT
    iptables -A INPUT -p udp --dport $PORT -j ACCEPT
    iptables -A INPUT -p tcp --dport 8080 -j ACCEPT
    iptables -A INPUT -p tcp --dport 9999 -j ACCEPT
    
    # NAT for VPN
    iptables -t nat -A POSTROUTING -o "$INTERFACE" -j MASQUERADE
    
    # Start services
    systemctl enable shadowsocks-libev-server@config > /dev/null 2>&1 || true
    systemctl restart shadowsocks-libev-server@config > /dev/null 2>&1 || true
    
    # Create pairing API endpoint
    mkdir -p /var/www/omr-autopair
    
    cat > /var/www/omr-autopair/pair.json << ENDPAIR
{
  "status": "ready",
  "server_ip": "$VPS_IP",
  "server_port": $PORT,
  "password": "$SHADOWSOCKS_PASS",
  "encryption": "chacha20-ietf-poly1305",
  "mptcp_enabled": true,
  "setup_token": "$SETUP_TOKEN",
  "version": "1.0-optimized",
  "timestamp": "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
}
ENDPAIR
    
    # Start simple HTTP server for pairing
    cat > /etc/systemd/system/omr-autopair.service << 'ENDSERVICE'
[Unit]
Description=OMR Auto-Pair Service
After=network.target

[Service]
Type=simple
WorkingDirectory=/var/www/omr-autopair
ExecStart=/usr/bin/python3 -m http.server 9999
Restart=always

[Install]
WantedBy=multi-user.target
ENDSERVICE
    
    systemctl daemon-reload
    systemctl enable omr-autopair > /dev/null 2>&1
    systemctl restart omr-autopair
    
    # Generate pairing code (Base64 encoded JSON)
    PAIRING_CODE=$(echo "{\"ip\":\"$VPS_IP\",\"port\":$PORT,\"pass\":\"$SHADOWSOCKS_PASS\",\"token\":\"$SETUP_TOKEN\"}" | base64 -w0)
    
    # Generate QR code data
    QR_DATA="omr://$VPS_IP:$PORT?pass=$SHADOWSOCKS_PASS&token=$SETUP_TOKEN"
    
    clear
    
    echo -e "${GREEN}"
    cat << 'EOF'
╔════════════════════════════════════════════════════════════╗
║                                                            ║
║          ✓ VPS AUTO-PAIR SETUP COMPLETE!                  ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝
EOF
    echo -e "${NC}"
    
    echo ""
    echo -e "${PURPLE}╔═══════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║                                                       ║${NC}"
    echo -e "${PURPLE}║           🔗 PAIRING CODE (COPY THIS!)               ║${NC}"
    echo -e "${PURPLE}║                                                       ║${NC}"
    echo -e "${PURPLE}╠═══════════════════════════════════════════════════════╣${NC}"
    echo -e "${PURPLE}║${NC}                                                       ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${YELLOW}$PAIRING_CODE${NC}"
    echo -e "${PURPLE}║${NC}                                                       ${PURPLE}║${NC}"
    echo -e "${PURPLE}╚═══════════════════════════════════════════════════════╝${NC}"
    echo ""
    
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${GREEN}Method 1: Automatic Pairing (Easiest)${NC}"
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "On your router, run:"
    echo -e "${YELLOW}curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/auto-pair.sh | sh -s '$PAIRING_CODE'${NC}"
    echo ""
    
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${GREEN}Method 2: Manual Quick Setup${NC}"
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "Server IP:   ${GREEN}$VPS_IP${NC}"
    echo -e "Port:        ${GREEN}$PORT${NC}"
    echo -e "Password:    ${GREEN}$SHADOWSOCKS_PASS${NC}"
    echo -e "Encryption:  ${GREEN}Shadowsocks (chacha20-ietf-poly1305)${NC}"
    echo ""
    
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${GREEN}Method 3: Auto-Discovery (Zero Config!)${NC}"
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "Your router can auto-discover this server!"
    echo -e "On router web interface: ${GREEN}Services → OMR → Auto-Discover${NC}"
    echo ""
    echo -e "Or fetch config from: ${GREEN}http://$VPS_IP:9999/pair.json${NC}"
    echo ""
    
    # Save all info
    cat > /root/omr-pairing-info.txt << ENDINFO
OpenMPTCProuter Optimized - Auto-Pairing Information
====================================================
Created: $(date)

VPS Details:
-----------
IP:         $VPS_IP
Port:       $PORT
Password:   $SHADOWSOCKS_PASS
Token:      $SETUP_TOKEN

Pairing Code:
-------------
$PAIRING_CODE

Auto-Pair Command for Router:
------------------------------
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/auto-pair.sh | sh -s '$PAIRING_CODE'

Auto-Discovery URL:
-------------------
http://$VPS_IP:9999/pair.json

QR Code Data:
-------------
$QR_DATA

Manual Setup:
-------------
1. Go to router: http://192.168.2.1
2. Navigate to: Services → OpenMPTCProuter
3. Enter:
   - Server IP: $VPS_IP
   - Port: $PORT
   - Password: $SHADOWSOCKS_PASS
   - Encryption: Shadowsocks
4. Save & Apply

Connection Test:
----------------
From router: ping $VPS_IP
From router: curl http://$VPS_IP:9999/pair.json
ENDINFO
    
    chmod 600 /root/omr-pairing-info.txt
    
    echo -e "${BLUE}💾 All pairing information saved to: ${CYAN}/root/omr-pairing-info.txt${NC}"
    echo ""
    echo -e "${GREEN}✓ Your VPS is ready and waiting for router to connect!${NC}"
    echo ""

#############################################
# ROUTER CLIENT AUTO-PAIR
#############################################

elif [ "$DEVICE_TYPE" = "router" ]; then
    echo -e "${BLUE}═══════════════════════════════════════${NC}"
    echo -e "${YELLOW}      Router Auto-Pairing Setup${NC}"
    echo -e "${BLUE}═══════════════════════════════════════${NC}"
    echo ""
    
    PAIRING_CODE="$1"
    
    # Method 1: Use pairing code
    if [ -n "$PAIRING_CODE" ]; then
        echo -e "${CYAN}Decoding pairing code...${NC}"
        
        # Decode pairing code
        if PAIRING_JSON=$(echo "$PAIRING_CODE" | base64 -d 2>/dev/null); then
            VPS_IP=$(echo "$PAIRING_JSON" | grep -o '"ip":"[^"]*' | cut -d'"' -f4)
            VPS_PORT=$(echo "$PAIRING_JSON" | grep -o '"port":[0-9]*' | cut -d':' -f2)
            VPS_PASS=$(echo "$PAIRING_JSON" | grep -o '"pass":"[^"]*' | cut -d'"' -f4)
            
            echo -e "${GREEN}✓ Pairing code decoded${NC}"
            echo -e "  VPS IP: ${CYAN}$VPS_IP${NC}"
        else
            echo -e "${RED}Error: Invalid pairing code${NC}"
            exit 1
        fi
    
    # Method 2: Auto-discovery
    else
        echo -e "${CYAN}No pairing code provided. Trying auto-discovery...${NC}"
        echo ""
        
        echo -e "${YELLOW}Enter your VPS IP for auto-discovery:${NC}"
        read -r VPS_IP < /dev/tty
        
        echo -e "${CYAN}Fetching configuration from VPS...${NC}"

        # Try to fetch config from VPS with proper SSL verification
        # SECURITY FIX: Removed -k flag to prevent MITM attacks
        if CONFIG_JSON=$(curl -s --max-time 10 "https://$VPS_IP:9999/pair.json" 2>/dev/null) && [ -n "$CONFIG_JSON" ]; then
            echo -e "${GREEN}✓ Secure connection established (HTTPS)${NC}"
            VPS_PORT=$(echo "$CONFIG_JSON" | jq -r '.server_port // empty' 2>/dev/null)
            VPS_PASS=$(echo "$CONFIG_JSON" | jq -r '.password // empty' 2>/dev/null)
        elif CONFIG_JSON=$(curl -s --max-time 10 "http://$VPS_IP:9999/pair.json" 2>/dev/null) && [ -n "$CONFIG_JSON" ]; then
            echo -e "${YELLOW}═══════════════════════════════════════════════════${NC}"
            echo -e "${YELLOW}⚠  WARNING: INSECURE CONNECTION DETECTED${NC}"
            echo -e "${YELLOW}═══════════════════════════════════════════════════${NC}"
            echo -e "${RED}Your credentials will be transmitted in plaintext!${NC}"
            echo -e "${RED}This connection is vulnerable to interception.${NC}"
            echo ""
            echo -e "${CYAN}Consider setting up SSL/TLS on your VPS pairing API.${NC}"
            echo ""
            read -p "Continue with insecure HTTP connection? (y/N): " -n 1 -r < /dev/tty
            echo
            if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                echo -e "${YELLOW}Setup cancelled for security reasons.${NC}"
                exit 1
            fi
            VPS_PORT=$(echo "$CONFIG_JSON" | jq -r '.server_port // empty' 2>/dev/null)
            VPS_PASS=$(echo "$CONFIG_JSON" | jq -r '.password // empty' 2>/dev/null)

            echo -e "${GREEN}✓ Configuration auto-discovered (insecure)${NC}"
        else
            echo -e "${RED}Error: Could not auto-discover VPS configuration${NC}"
            echo ""
            echo -e "${YELLOW}Please use pairing code or enter manually:${NC}"
            read -r -p "VPS Port (default 65500): " VPS_PORT < /dev/tty
            VPS_PORT=${VPS_PORT:-65500}
            read -r -p "VPS Password: " VPS_PASS < /dev/tty
        fi
    fi
    
    echo ""
    echo -e "${GREEN}╔════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║  Configuration Summary             ║${NC}"
    echo -e "${GREEN}╠════════════════════════════════════╣${NC}"
    echo -e "${GREEN}║${NC} VPS IP:   ${YELLOW}$VPS_IP${NC}"
    echo -e "${GREEN}║${NC} Port:     ${YELLOW}$VPS_PORT${NC}"
    echo -e "${GREEN}║${NC} Password: ${YELLOW}$(echo $VPS_PASS | sed 's/./*/g')${NC}"
    echo -e "${GREEN}╚════════════════════════════════════╝${NC}"
    echo ""
    
    echo -e "${CYAN}Configuring router...${NC}"
    echo ""
    
    # Run client auto-setup
    curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/client-auto-setup.sh | sh -s "$VPS_IP" "$VPS_PASS" "$VPS_PORT"
    
    echo ""
    echo -e "${GREEN}✓ Router automatically paired with VPS!${NC}"
    echo ""
fi

echo -e "${CYAN}════════════════════════════════════════════════${NC}"
echo -e "${GREEN}    Auto-Pairing Complete! 🎉${NC}"
echo -e "${CYAN}════════════════════════════════════════════════${NC}"
echo ""
