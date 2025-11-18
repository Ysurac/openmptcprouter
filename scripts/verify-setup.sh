#!/bin/bash
#
# OpenMPTCProuter Setup Verification Script
# Validates VPS and Router configuration
#
# Usage: ./verify-setup.sh [vps|router]
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# Unicode
CHECK="✓"
CROSS="✗"
WARN="⚠"
INFO="ℹ"

# Counters
PASSED=0
FAILED=0
WARNINGS=0

# Print functions
print_header() {
    echo -e "\n${PURPLE}${BOLD}╔════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}${BOLD}║   OpenMPTCProuter Setup Verification                   ║${NC}"
    echo -e "${PURPLE}${BOLD}╚════════════════════════════════════════════════════════╝${NC}\n"
}

print_section() {
    echo -e "\n${CYAN}━━━ $1 ━━━${NC}"
}

test_pass() {
    echo -e "${GREEN}${CHECK}${NC} $1"
    ((PASSED++))
}

test_fail() {
    echo -e "${RED}${CROSS}${NC} $1"
    ((FAILED++))
}

test_warn() {
    echo -e "${YELLOW}${WARN}${NC} $1"
    ((WARNINGS++))
}

test_info() {
    echo -e "${BLUE}${INFO}${NC} $1"
}

# Detect environment
detect_environment() {
    if [ -f /etc/openwrt_release ]; then
        echo "router"
    elif [ -f /etc/os-release ]; then
        . /etc/os-release
        case "$ID" in
            debian|ubuntu)
                echo "vps"
                ;;
            *)
                echo "unknown"
                ;;
        esac
    else
        echo "unknown"
    fi
}

# VPS Validation
verify_vps() {
    print_section "System Information"

    # Check OS
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        test_pass "Operating System: $PRETTY_NAME"
    else
        test_fail "Unable to detect OS"
    fi

    # Check kernel
    KERNEL_VERSION=$(uname -r)
    test_info "Kernel: $KERNEL_VERSION"

    print_section "Network Configuration"

    # Check public IP
    PUBLIC_IP=$(curl -s -4 ifconfig.me 2>/dev/null || echo "Unable to detect")
    if [ "$PUBLIC_IP" != "Unable to detect" ]; then
        test_pass "Public IP: $PUBLIC_IP"
    else
        test_fail "Unable to detect public IP"
    fi

    # Check network interface
    if ip link show | grep -q "state UP"; then
        INTERFACE=$(ip route | grep default | awk '{print $5}' | head -n1)
        test_pass "Network interface: $INTERFACE (UP)"
    else
        test_fail "No active network interface found"
    fi

    print_section "Required Packages"

    # Check essential packages
    PACKAGES=(
        "shadowsocks-libev"
        "iptables"
        "curl"
        "jq"
        "qrencode"
        "wireguard"
    )

    for pkg in "${PACKAGES[@]}"; do
        if dpkg -l | grep -q "^ii.*$pkg"; then
            test_pass "$pkg installed"
        else
            test_fail "$pkg NOT installed"
        fi
    done

    print_section "Services Status"

    # Check Shadowsocks
    if systemctl is-active --quiet shadowsocks-libev; then
        test_pass "Shadowsocks service running"
    else
        if systemctl is-enabled --quiet shadowsocks-libev 2>/dev/null; then
            test_warn "Shadowsocks enabled but not running"
        else
            test_fail "Shadowsocks not running or not enabled"
        fi
    fi

    # Check setup web interface
    if systemctl is-active --quiet omr-setup-web; then
        test_pass "Setup web interface running"
        test_info "Access at: http://$PUBLIC_IP:8080"
    else
        test_warn "Setup web interface not running"
    fi

    print_section "Firewall Configuration"

    # Check if iptables rules exist
    if iptables -L -n | grep -q "65500"; then
        test_pass "Firewall rules configured (port 65500 open)"
    else
        test_warn "Firewall rules may not be configured"
    fi

    # Check IP forwarding
    if [ "$(cat /proc/sys/net/ipv4/ip_forward)" = "1" ]; then
        test_pass "IP forwarding enabled"
    else
        test_fail "IP forwarding disabled"
    fi

    print_section "MPTCP Configuration"

    # Check MPTCP
    if grep -q "net.mptcp.mptcp_enabled.*=.*1" /etc/sysctl.conf /etc/sysctl.d/* 2>/dev/null; then
        test_pass "MPTCP enabled in sysctl"
    else
        test_warn "MPTCP may not be enabled"
    fi

    print_section "Configuration Files"

    # Check config files
    if [ -f /etc/shadowsocks-libev/config.json ]; then
        test_pass "Shadowsocks configuration exists"
    else
        test_fail "Shadowsocks configuration missing"
    fi

    if [ -f /etc/openmptcprouter/config.json ]; then
        test_pass "OMR configuration exists"
    else
        test_warn "OMR configuration file not found"
    fi

    if [ -f /root/openmptcprouter_credentials.txt ]; then
        test_pass "Credentials file saved"
    else
        test_warn "Credentials file not found"
    fi

    print_section "Connectivity Tests"

    # Check port 65500 listening
    if ss -tlnp | grep -q ":65500"; then
        test_pass "Shadowsocks listening on port 65500"
    else
        test_fail "Port 65500 not listening"
    fi

    # Check internet connectivity
    if ping -c 1 8.8.8.8 &>/dev/null; then
        test_pass "Internet connectivity working"
    else
        test_fail "No internet connectivity"
    fi
}

# Router Validation
verify_router() {
    print_section "System Information"

    # Check OpenWrt
    if [ -f /etc/openwrt_release ]; then
        . /etc/openwrt_release
        test_pass "OpenWrt: $DISTRIB_DESCRIPTION"
    else
        test_fail "Not running OpenWrt"
    fi

    print_section "Network Configuration"

    # Check LAN interface
    if uci get network.lan.ipaddr &>/dev/null; then
        LAN_IP=$(uci get network.lan.ipaddr)
        test_pass "LAN IP: $LAN_IP"
    else
        test_fail "LAN interface not configured"
    fi

    # Check WAN interfaces
    WAN_COUNT=$(uci show network | grep -c "proto='dhcp'\|proto='static'" || echo 0)
    if [ "$WAN_COUNT" -gt 0 ]; then
        test_pass "WAN interfaces: $WAN_COUNT configured"
    else
        test_warn "No WAN interfaces found"
    fi

    print_section "VPN Configuration"

    # Check if Shadowsocks is configured
    if uci get shadowsocks-libev.@ss_redir[0].server &>/dev/null; then
        SS_SERVER=$(uci get shadowsocks-libev.@ss_redir[0].server)
        test_pass "VPN server configured: $SS_SERVER"
    else
        test_fail "VPN not configured"
    fi

    # Check Shadowsocks password
    if uci get shadowsocks-libev.@ss_redir[0].password &>/dev/null; then
        test_pass "VPN password set"
    else
        test_fail "VPN password not set"
    fi

    print_section "Services Status"

    # Check Shadowsocks service
    if /etc/init.d/shadowsocks-libev enabled &>/dev/null; then
        test_pass "Shadowsocks service enabled"
    else
        test_fail "Shadowsocks service not enabled"
    fi

    # Check network monitor
    if [ -f /etc/init.d/network-monitor ]; then
        if /etc/init.d/network-monitor enabled &>/dev/null; then
            test_pass "Network monitor enabled"
        else
            test_warn "Network monitor not enabled"
        fi
    fi

    print_section "MPTCP Configuration"

    # Check if MPTCP is enabled
    if grep -q "mptcp_enabled=1" /etc/sysctl.conf /etc/sysctl.d/* 2>/dev/null; then
        test_pass "MPTCP enabled"
    else
        test_warn "MPTCP may not be enabled"
    fi

    print_section "Connectivity Tests"

    # Check internet connectivity
    if ping -c 1 8.8.8.8 &>/dev/null; then
        test_pass "Internet connectivity working"
    else
        test_fail "No internet connectivity"
    fi

    # Check VPN connectivity (if configured)
    if uci get shadowsocks-libev.@ss_redir[0].server &>/dev/null; then
        VPN_SERVER=$(uci get shadowsocks-libev.@ss_redir[0].server)
        if ping -c 1 "$VPN_SERVER" &>/dev/null; then
            test_pass "VPN server reachable"
        else
            test_warn "VPN server not reachable (check firewall/routing)"
        fi
    fi
}

# Print summary
print_summary() {
    echo ""
    echo -e "${PURPLE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BOLD}Summary:${NC}"
    echo -e "  ${GREEN}${CHECK} Passed:${NC}   $PASSED"
    echo -e "  ${RED}${CROSS} Failed:${NC}   $FAILED"
    echo -e "  ${YELLOW}${WARN} Warnings:${NC} $WARNINGS"
    echo -e "${PURPLE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""

    if [ $FAILED -eq 0 ]; then
        if [ $WARNINGS -eq 0 ]; then
            echo -e "${GREEN}${CHECK} All checks passed! Your setup looks great!${NC}"
            return 0
        else
            echo -e "${YELLOW}${WARN} Setup complete with warnings. Review above.${NC}"
            return 0
        fi
    else
        echo -e "${RED}${CROSS} Some checks failed. Please review and fix issues above.${NC}"
        return 1
    fi
}

# Main
main() {
    print_header

    # Detect or use argument
    if [ -n "$1" ]; then
        ENV_TYPE="$1"
    else
        ENV_TYPE=$(detect_environment)
    fi

    case "$ENV_TYPE" in
        vps)
            echo -e "${CYAN}Running VPS verification...${NC}\n"
            verify_vps
            ;;
        router)
            echo -e "${CYAN}Running Router verification...${NC}\n"
            verify_router
            ;;
        *)
            echo -e "${RED}Unable to detect environment type.${NC}"
            echo "Usage: $0 [vps|router]"
            exit 1
            ;;
    esac

    print_summary
}

main "$@"
