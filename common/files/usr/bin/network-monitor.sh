#!/bin/sh
#
# OpenMPTCProuter Optimized - Self-Aware Network Monitor
# Continuously monitors network state and auto-adjusts configuration
# Runs as a daemon and maintains optimal network configuration
# Immediately responds to port changes and reconfigures on the fly
#

LOG_TAG="network-monitor"
PID_FILE="/var/run/network-monitor.pid"
STATE_FILE="/var/run/network-monitor.state"
CHECK_INTERVAL=5  # Check every 5 seconds for faster response
QUICK_RECONFIG_COOLDOWN=30  # Allow quick reconfig every 30 seconds
FULL_RECONFIG_COOLDOWN=180  # Full reconfiguration every 3 minutes max

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

# Get current network state (more detailed)
get_network_state() {
    local state=""
    
    # Get list of interfaces with link and speed
    for iface_path in /sys/class/net/*/carrier; do
        local if_name=$(echo "$iface_path" | cut -d/ -f5)
        
        # Skip virtual interfaces
        case "$if_name" in
            lo|sit*|ip6*|gre*|tun*|tap*|br-*|wlan*|ifb*) continue ;;
        esac
        
        local carrier=$(cat "$iface_path" 2>/dev/null)
        local speed=$(cat "/sys/class/net/$if_name/speed" 2>/dev/null)
        local has_ip=0
        
        # Check if interface has an IP address
        if ip addr show dev "$if_name" 2>/dev/null | grep -q "inet "; then
            has_ip=1
        fi
        
        state="${state}${if_name}:${carrier:-0}:${speed:-0}:${has_ip} "
    done
    
    # Get WiFi radio states
    for radio in $(uci show wireless 2>/dev/null | grep "wireless\.radio.*=wifi-device" | cut -d. -f2 | cut -d= -f1); do
        local disabled=$(uci -q get wireless.$radio.disabled)
        state="${state}${radio}:${disabled:-0} "
    done
    
    echo "$state" | xargs
}

# Check if network state has changed significantly
network_state_changed() {
    local current_state=$(get_network_state)
    local last_state=""
    
    if [ -f "$STATE_FILE" ]; then
        last_state=$(cat "$STATE_FILE")
    fi
    
    if [ "$current_state" != "$last_state" ]; then
        # Check what changed
        local changes=""
        
        # Parse old and new states
        for item in $current_state; do
            if ! echo "$last_state" | grep -q "$item"; then
                local iface=$(echo "$item" | cut -d: -f1)
                changes="${changes}${iface} "
            fi
        done
        
        if [ -n "$changes" ]; then
            log_msg "Network state changed - interfaces affected: $changes"
        else
            log_msg "Network state changed (details)"
        fi
        
        echo "$current_state" > "$STATE_FILE"
        echo "$changes"
        return 0
    fi
    
    return 1
}

# Check if we can do a quick reconfiguration
can_quick_reconfig() {
    local last_reconfig_file="/var/run/last-quick-reconfig"
    local current_time=$(date +%s)
    
    if [ -f "$last_reconfig_file" ]; then
        local last_time=$(cat "$last_reconfig_file")
        local elapsed=$((current_time - last_time))
        
        if [ $elapsed -lt $QUICK_RECONFIG_COOLDOWN ]; then
            return 1
        fi
    fi
    
    echo "$current_time" > "$last_reconfig_file"
    return 0
}

# Check if we can do a full reconfiguration
can_full_reconfig() {
    local last_reconfig_file="/var/run/last-full-reconfig"
    local current_time=$(date +%s)
    
    if [ -f "$last_reconfig_file" ]; then
        local last_time=$(cat "$last_reconfig_file")
        local elapsed=$((current_time - last_time))
        
        if [ $elapsed -lt $FULL_RECONFIG_COOLDOWN ]; then
            log_msg "Full reconfig cooldown: $((FULL_RECONFIG_COOLDOWN - elapsed))s remaining"
            return 1
        fi
    fi
    
    echo "$current_time" > "$last_reconfig_file"
    return 0
}

# Quickly reassign a single interface without full reconfiguration
quick_reassign_interface() {
    local iface="$1"
    
    log_msg "Quick reassigning interface $iface"
    
    # Check if interface has upstream internet
    ip link set "$iface" up
    sleep 2
    
    local has_upstream=0
    if timeout 10 udhcpc -i "$iface" -n -q -s /dev/null 2>&1 | grep -q "obtained"; then
        has_upstream=1
        ip addr flush dev "$iface"
    fi
    
    if [ $has_upstream -eq 1 ]; then
        log_msg "$iface appears to have upstream internet - configuring as WAN"
        
        # Find next available WAN name
        local wan_num=1
        while uci -q get network.wan${wan_num} >/dev/null 2>&1; do
            wan_num=$((wan_num + 1))
        done
        
        local wan_name="wan"
        [ $wan_num -gt 1 ] && wan_name="wan${wan_num}"
        
        # Remove from LAN bridge if present
        local bridge_ports=$(uci -q get network.@device[0].ports)
        local new_bridge_ports=$(echo "$bridge_ports" | sed "s/$iface//g" | xargs)
        
        if [ "$bridge_ports" != "$new_bridge_ports" ]; then
            uci set network.@device[0].ports="$new_bridge_ports"
            log_msg "Removed $iface from LAN bridge"
        fi
        
        # Configure as WAN
        uci -q batch <<-EOF
			delete network.$wan_name
			set network.$wan_name=interface
			set network.$wan_name.device='$iface'
			set network.$wan_name.proto='dhcp'
			set network.$wan_name.metric='$((wan_num * 10))'
			set network.$wan_name.multipath='on'
		EOF
        
        uci commit network
        ifup "$wan_name" 2>/dev/null &
        
    else
        log_msg "$iface does not have upstream internet - ensuring it's in LAN"
        
        # Check if it's configured as WAN
        local is_wan=0
        for wan in $(uci show network | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1); do
            local wan_device=$(uci -q get network.$wan.device)
            if [ "$wan_device" = "$iface" ]; then
                log_msg "Removing $iface from WAN ($wan)"
                uci delete network.$wan
                is_wan=1
            fi
        done
        
        # Add to LAN bridge if not already there
        local bridge_ports=$(uci -q get network.@device[0].ports)
        if ! echo "$bridge_ports" | grep -qw "$iface"; then
            local new_bridge_ports="$bridge_ports $iface"
            uci set network.@device[0].ports="$(echo $new_bridge_ports | xargs)"
            log_msg "Added $iface to LAN bridge"
        fi
        
        uci commit network
        
        if [ $is_wan -eq 1 ]; then
            /etc/init.d/network reload &
        fi
    fi
}

# Full port auto-configuration
full_port_reconfig() {
    log_msg "Triggering full port auto-configuration"
    
    # Remove the configuration marker to allow reconfiguration
    rm -f /etc/port-autoconfig-applied
    
    # Run port auto-config
    if [ -x /usr/bin/port-autoconfig.sh ]; then
        /usr/bin/port-autoconfig.sh
    else
        log_msg "WARNING: port-autoconfig.sh not found"
    fi
}

# Auto-configure WiFi
auto_configure_wifi() {
    log_msg "Checking WiFi configuration"
    
    # Check if any WiFi is enabled
    local wifi_enabled=0
    for radio in $(uci show wireless 2>/dev/null | grep "wireless\.radio.*=wifi-device" | cut -d. -f2 | cut -d= -f1); do
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
    for wan in $(uci show network 2>/dev/null | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1); do
        local device=$(uci -q get network.$wan.device)
        
        if [ -n "$device" ]; then
            # Check if interface has IP
            if ip addr show dev "$device" 2>/dev/null | grep -q "inet "; then
                # Try to ping gateway
                local gateway=$(ip route show dev "$device" 2>/dev/null | grep "default" | awk '{print $3}')
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

# Handle network state change
handle_network_change() {
    local changed_ifaces="$1"
    
    log_msg "Handling network change for: $changed_ifaces"
    
    # If we can do a quick reconfig, handle individual interfaces
    if can_quick_reconfig; then
        for iface in $changed_ifaces; do
            # Skip if it's not a physical interface
            case "$iface" in
                eth*|lan*|wan*)
                    quick_reassign_interface "$iface"
                    ;;
            esac
        done
    else
        log_msg "Quick reconfig cooldown active, will retry soon"
    fi
}

# Main monitoring loop
main() {
    log_msg "Starting self-aware network monitor (PID: $$)"
    log_msg "Response time: ${CHECK_INTERVAL}s, Quick reconfig: ${QUICK_RECONFIG_COOLDOWN}s"
    check_running
    
    # Wait for system to stabilize
    log_msg "Waiting for system initialization..."
    sleep 15
    
    # Initial configuration
    log_msg "Running initial auto-configuration"
    
    # Configure ports
    if [ ! -f /etc/port-autoconfig-applied ]; then
        full_port_reconfig
        sleep 10
    fi
    
    # Configure WiFi
    auto_configure_wifi
    sleep 5
    
    # Initialize state
    get_network_state > "$STATE_FILE"
    
    log_msg "Entering self-aware monitoring loop"
    log_msg "System will auto-adjust to port changes immediately"
    
    # Main loop
    local loop_count=0
    while true; do
        sleep $CHECK_INTERVAL
        loop_count=$((loop_count + 1))
        
        # Check if network state changed
        local changed_ifaces=$(network_state_changed)
        if [ -n "$changed_ifaces" ]; then
            log_msg "Network change detected (loop #$loop_count)"
            
            # Immediate response to changes
            handle_network_change "$changed_ifaces"
        fi
        
        # Periodic full check every 60 loops (~5 minutes at 5s intervals)
        if [ $((loop_count % 60)) -eq 0 ]; then
            log_msg "Periodic health check (loop #$loop_count)"
            
            # Check WAN connectivity
            if ! check_wan_connectivity; then
                log_msg "WARNING: No WAN connectivity detected"
                
                # If we can do a full reconfig, do it
                if can_full_reconfig; then
                    log_msg "Triggering full port reconfiguration"
                    full_port_reconfig
                fi
            fi
            
            # Ensure WiFi is configured
            auto_configure_wifi
            
            # Ensure DHCP is running
            check_dhcp_server
        fi
        
        # Quick DHCP check every 20 loops (~100 seconds)
        if [ $((loop_count % 20)) -eq 0 ]; then
            check_dhcp_server
        fi
    done
}

# Start main monitoring loop
main
