#!/bin/sh
#
# OpenMPTCProuter Optimized - Self-Aware Network Monitor
# Continuously monitors network state and auto-adjusts configuration
# Runs as a daemon and maintains optimal network configuration
#

LOG_TAG="network-monitor"
PID_FILE="/var/run/network-monitor.pid"
STATE_FILE="/var/run/network-monitor.state"
CHECK_INTERVAL=30
RECONFIG_COOLDOWN=300  # Don't reconfigure more than once every 5 minutes

log_msg() {
    logger -t "$LOG_TAG" "$1"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" >&2
}

# Check if monitor is already running
check_running() {
    if [ -f "$PID_FILE" ]; then
        old_pid=$(cat "$PID_FILE")
        if kill -0 "$old_pid" 2>/dev/null; then
            log_msg "Monitor already running with PID $old_pid"
            exit 0
        fi
    fi
    echo $$ > "$PID_FILE"
}

# Cleanup on exit
cleanup() {
    log_msg "Network monitor stopping"
    rm -f "$PID_FILE"
    exit 0
}

trap cleanup INT TERM EXIT

# Get current network state
get_network_state() {
    local state=""
    
    # Get list of interfaces with link
    for iface in /sys/class/net/*/carrier; do
        local if_name=$(echo "$iface" | cut -d/ -f5)
        local carrier=$(cat "$iface" 2>/dev/null)
        
        # Skip virtual interfaces
        case "$if_name" in
            lo|sit*|ip6*|gre*|tun*|tap*|br-*|wlan*) continue ;;
        esac
        
        state="${state}${if_name}:${carrier:-0} "
    done
    
    # Get WiFi radio states
    for radio in $(uci show wireless | grep "wireless\.radio.*=wifi-device" | cut -d. -f2 | cut -d= -f1); do
        local disabled=$(uci -q get wireless.$radio.disabled)
        state="${state}${radio}:${disabled:-0} "
    done
    
    echo "$state" | xargs
}

# Check if network state has changed
network_state_changed() {
    local current_state=$(get_network_state)
    local last_state=""
    
    if [ -f "$STATE_FILE" ]; then
        last_state=$(cat "$STATE_FILE")
    fi
    
    if [ "$current_state" != "$last_state" ]; then
        log_msg "Network state changed"
        log_msg "Old: $last_state"
        log_msg "New: $current_state"
        echo "$current_state" > "$STATE_FILE"
        return 0
    fi
    
    return 1
}

# Check if it's time to reconfigure
can_reconfigure() {
    local last_reconfig_file="/var/run/last-network-reconfig"
    local current_time=$(date +%s)
    
    if [ -f "$last_reconfig_file" ]; then
        local last_time=$(cat "$last_reconfig_file")
        local elapsed=$((current_time - last_time))
        
        if [ $elapsed -lt $RECONFIG_COOLDOWN ]; then
            log_msg "Cooldown active: $((RECONFIG_COOLDOWN - elapsed))s remaining"
            return 1
        fi
    fi
    
    echo "$current_time" > "$last_reconfig_file"
    return 0
}

# Auto-configure network ports
auto_configure_ports() {
    log_msg "Triggering port auto-configuration"
    
    # Remove the configuration marker to allow reconfiguration
    rm -f /etc/port-autoconfig-applied
    
    # Run port auto-config
    if [ -x /usr/bin/port-autoconfig.sh ]; then
        /usr/bin/port-autoconfig.sh &
    else
        log_msg "WARNING: port-autoconfig.sh not found"
    fi
}

# Auto-configure WiFi
auto_configure_wifi() {
    log_msg "Checking WiFi configuration"
    
    # Check if any WiFi is enabled
    local wifi_enabled=0
    for radio in $(uci show wireless | grep "wireless\.radio.*=wifi-device" | cut -d. -f2 | cut -d= -f1); do
        local disabled=$(uci -q get wireless.$radio.disabled)
        if [ "$disabled" != "1" ]; then
            wifi_enabled=1
            break
        fi
    done
    
    # If no WiFi is enabled, auto-configure
    if [ $wifi_enabled -eq 0 ]; then
        log_msg "No WiFi enabled, triggering auto-configuration"
        if [ -x /usr/bin/wifi-autoconfig.sh ]; then
            /usr/bin/wifi-autoconfig.sh &
        fi
    fi
}

# Check WAN connectivity
check_wan_connectivity() {
    local wan_ok=0
    
    # Check each WAN interface
    for wan in $(uci show network | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1); do
        local device=$(uci -q get network.$wan.device)
        
        if [ -n "$device" ]; then
            # Check if interface has IP
            if ip addr show dev "$device" 2>/dev/null | grep -q "inet "; then
                # Try to ping gateway
                local gateway=$(ip route show dev "$device" | grep "default" | awk '{print $3}')
                if [ -n "$gateway" ]; then
                    if ping -c 1 -W 2 -I "$device" "$gateway" >/dev/null 2>&1; then
                        wan_ok=1
                        break
                    fi
                fi
            fi
        fi
    done
    
    return $((1 - wan_ok))
}

# Monitor DHCP leases and ensure service is running
check_dhcp_server() {
    # Check if dnsmasq is running
    if ! pidof dnsmasq >/dev/null; then
        log_msg "WARNING: DHCP server not running, restarting"
        /etc/init.d/dnsmasq restart
    fi
    
    # Check if DHCP is configured on LAN
    local dhcp_enabled=$(uci -q get dhcp.lan.dhcpv4)
    if [ "$dhcp_enabled" != "server" ]; then
        log_msg "WARNING: DHCP not configured on LAN, fixing"
        uci set dhcp.lan.dhcpv4='server'
        uci commit dhcp
        /etc/init.d/dnsmasq restart
    fi
}

# Monitor and recover from network issues
monitor_and_recover() {
    # Check WAN connectivity
    if ! check_wan_connectivity; then
        log_msg "WARNING: No WAN connectivity detected"
        
        # If we can reconfigure, try to detect ports again
        if can_reconfigure; then
            log_msg "Attempting to reconfigure network ports"
            auto_configure_ports
        fi
    fi
    
    # Ensure WiFi is configured
    auto_configure_wifi
    
    # Ensure DHCP is running
    check_dhcp_server
}

# Main monitoring loop
main() {
    log_msg "Starting self-aware network monitor (PID: $$)"
    check_running
    
    # Wait for system to stabilize
    log_msg "Waiting for system initialization..."
    sleep 15
    
    # Initial configuration
    log_msg "Running initial auto-configuration"
    
    # Configure ports
    if [ ! -f /etc/port-autoconfig-applied ]; then
        auto_configure_ports
        sleep 10
    fi
    
    # Configure WiFi
    auto_configure_wifi
    sleep 5
    
    # Initialize state
    get_network_state > "$STATE_FILE"
    
    log_msg "Entering monitoring loop (check interval: ${CHECK_INTERVAL}s)"
    
    # Main loop
    local loop_count=0
    while true; do
        sleep $CHECK_INTERVAL
        loop_count=$((loop_count + 1))
        
        # Check if network state changed
        if network_state_changed; then
            log_msg "Network change detected (loop #$loop_count)"
            
            # Wait a bit for things to stabilize
            sleep 5
            
            # Try to recover/optimize
            monitor_and_recover
        fi
        
        # Periodic health check every 10 loops
        if [ $((loop_count % 10)) -eq 0 ]; then
            log_msg "Periodic health check (loop #$loop_count)"
            monitor_and_recover
        fi
    done
}

# Start main monitoring loop
main
