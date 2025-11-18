#!/bin/sh
#
# OpenMPTCProuter Optimized - Network Health Monitor
# Simple monitoring service that ensures DHCP and WiFi keep running
# Does NOT reconfigure ports - user has full control via web UI
#

set -u  # Catch undefined variables

LOG_TAG="network-monitor"
PID_FILE="/var/run/network-monitor.pid"
CHECK_INTERVAL=60  # Check every minute

log_msg() {
    logger -t "$LOG_TAG" "$1"
}

# Check if monitor is already running
check_running() {
    # Use atomic mkdir for lock to prevent race condition
    local lockdir="${PID_FILE}.lock"
    if ! mkdir "$lockdir" 2>/dev/null; then
        # Another instance is starting, wait and check
        sleep 1
    fi

    if [ -f "$PID_FILE" ]; then
        local old_pid
        old_pid=$(cat "$PID_FILE" 2>/dev/null)
        # Validate PID is a number to prevent command injection
        if echo "$old_pid" | grep -qE '^[0-9]+$'; then
            # Check if process exists
            if kill -0 "$old_pid" 2>/dev/null; then
                # Verify it's actually our script by checking command line
                local cmdline
                cmdline=$(cat "/proc/$old_pid/cmdline" 2>/dev/null | tr '\0' ' ')
                if echo "$cmdline" | grep -q "network-monitor"; then
                    log_msg "Monitor already running with PID $old_pid"
                    rmdir "$lockdir" 2>/dev/null
                    exit 0
                else
                    log_msg "PID $old_pid exists but is not network-monitor (PID reused)"
                    rm -f "$PID_FILE"
                fi
            else
                # Process doesn't exist, remove stale PID file
                log_msg "Removing stale PID file (process $old_pid not running)"
                rm -f "$PID_FILE"
            fi
        else
            log_msg "Invalid PID in PID file, removing"
            rm -f "$PID_FILE"
        fi
    fi

    # Use atomic write with umask for security
    (
        umask 077
        echo $$ > "$PID_FILE"
    )
    rmdir "$lockdir" 2>/dev/null
}

# Cleanup on exit
cleanup() {
    rm -f "$PID_FILE"
    exit 0
}

trap cleanup INT TERM EXIT

# Monitor DHCP server
check_dhcp_server() {
    # Check if dnsmasq is running
    if ! pidof dnsmasq >/dev/null; then
        log_msg "DHCP server not running, restarting"
        /etc/init.d/dnsmasq restart
    fi
}

# Auto-configure WiFi on first boot only
auto_configure_wifi() {
    # Check if WiFi password file exists (means WiFi was already configured)
    if [ -f /etc/wifi-password.txt ]; then
        return 0
    fi

    # Check if any WiFi is enabled
    local wifi_enabled=0
    for radio in $(uci show wireless 2>/dev/null | grep "wireless\.radio.*=wifi-device" | cut -d. -f2 | cut -d= -f1); do
        local disabled=$(uci -q get wireless.$radio.disabled)
        if [ "$disabled" != "1" ]; then
            wifi_enabled=1
            break
        fi
    done

    # If no WiFi is enabled, auto-configure (first boot only)
    if [ $wifi_enabled -eq 0 ]; then
        log_msg "First boot: auto-configuring WiFi"
        if [ -x /usr/bin/wifi-autoconfig.sh ]; then
            /usr/bin/wifi-autoconfig.sh &
        fi
    fi
}

# Check WAN connectivity
check_wan_connectivity() {
    # Check if any WAN interface has link up
    local wan_up=0
    local wan_ifaces=""

    # Find all WAN interfaces (dhcp, qmi, mbim, static with gateway)
    wan_ifaces=$(uci -q show network | grep -E "proto='dhcp'|proto='qmi'|proto='mbim'|proto='3g'|proto='ncm'" | cut -d. -f2 | cut -d= -f1 | grep -v "^lan$" 2>/dev/null)

    # Check each WAN interface for link status
    for iface in $wan_ifaces; do
        local ifname=$(uci -q get network.$iface.device)
        [ -z "$ifname" ] && ifname=$(uci -q get network.$iface.ifname)

        if [ -n "$ifname" ] && ip link show "$ifname" 2>/dev/null | grep -q "state UP"; then
            wan_up=1
            break
        fi
    done

    if [ $wan_up -eq 0 ]; then
        log_msg "WARNING: No WAN interfaces UP"
    fi
}

# Main monitoring loop
main() {
    log_msg "Network health monitor starting"
    check_running
    
    # Wait for system to stabilize
    sleep 15
    
    # One-time first boot WiFi setup
    auto_configure_wifi
    
    log_msg "Monitoring DHCP and system health"
    
    # Main loop - keep services running and monitor WAN
    while true; do
        sleep $CHECK_INTERVAL

        # Ensure DHCP is running
        check_dhcp_server

        # Check WAN connectivity
        check_wan_connectivity
    done
}

# Start
main
