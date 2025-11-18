#!/bin/sh
#
# OpenMPTCProuter Optimized - Network Health Monitor
# Simple monitoring service that ensures DHCP and WiFi keep running
# Does NOT reconfigure ports - user has full control via web UI
#

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
            if kill -0 "$old_pid" 2>/dev/null; then
                log_msg "Monitor already running with PID $old_pid"
                rmdir "$lockdir" 2>/dev/null
                exit 0
            fi
        fi
        # Stale PID file, remove it
        rm -f "$PID_FILE"
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

# Main monitoring loop
main() {
    log_msg "Network health monitor starting"
    check_running
    
    # Wait for system to stabilize
    sleep 15
    
    # One-time first boot WiFi setup
    auto_configure_wifi
    
    log_msg "Monitoring DHCP and system health"
    
    # Main loop - just keep services running
    while true; do
        sleep $CHECK_INTERVAL
        
        # Ensure DHCP is running
        check_dhcp_server
    done
}

# Start
main
