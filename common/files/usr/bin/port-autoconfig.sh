#!/bin/sh
#
# OpenMPTCProuter Optimized - Auto WAN/LAN Port Detection
# Automatically configures ports based on what's connected
# Detects upstream internet on any port and configures it as WAN
# Other ports become LAN
#

LOG_TAG="port-autoconfig"
CHECK_INTERVAL=10
DETECTION_FILE="/var/run/port-autoconfig.state"
CONFIG_APPLIED="/etc/port-autoconfig-applied"

log_msg() {
    logger -t "$LOG_TAG" "$1"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Get list of all physical ethernet ports
get_all_ports() {
    local ports=""
    
    # Check for eth* interfaces
    for iface in /sys/class/net/eth*; do
        [ -e "$iface" ] && ports="$ports $(basename $iface)"
    done
    
    # Check for lan* interfaces (common on some devices)
    for iface in /sys/class/net/lan*; do
        [ -e "$iface" ] && ports="$ports $(basename $iface)"
    done
    
    # Check for wan* interfaces
    for iface in /sys/class/net/wan*; do
        [ -e "$iface" ] && ports="$ports $(basename $iface)"
    done
    
    echo "$ports" | xargs
}

# Check if a port has link (cable plugged in)
port_has_link() {
    local port="$1"
    
    [ ! -e "/sys/class/net/$port" ] && return 1
    
    # Check carrier status
    local carrier=$(cat /sys/class/net/$port/carrier 2>/dev/null)
    [ "$carrier" = "1" ] && return 0
    
    return 1
}

# Check if a port has DHCP offer (upstream internet)
port_has_upstream() {
    local port="$1"
    local timeout=15
    
    log_msg "Testing $port for upstream internet..."
    
    # Bring interface up
    ip link set "$port" up
    sleep 2
    
    # Try to get DHCP offer
    local dhcp_result=$(timeout $timeout udhcpc -i "$port" -n -q -s /dev/null 2>&1)
    local dhcp_status=$?
    
    # Check if we got an IP
    if [ $dhcp_status -eq 0 ]; then
        log_msg "$port has upstream DHCP (likely WAN)"
        # Release the IP since we're just testing
        ip addr flush dev "$port"
        return 0
    fi
    
    # Try to detect gateway via ARP
    arping -I "$port" -c 3 -b 255.255.255.255 2>/dev/null | grep -q "reply from" && {
        log_msg "$port has active network devices (potential WAN)"
        return 0
    }
    
    log_msg "$port does not appear to have upstream internet"
    return 1
}

# Detect which ports should be WAN and which should be LAN
detect_port_roles() {
    local all_ports=$(get_all_ports)
    local wan_ports=""
    local lan_ports=""
    
    log_msg "Scanning ports: $all_ports"
    
    for port in $all_ports; do
        if port_has_link "$port"; then
            log_msg "Port $port has link (cable connected)"
            
            # Test for upstream internet
            if port_has_upstream "$port"; then
                wan_ports="$wan_ports $port"
            else
                lan_ports="$lan_ports $port"
            fi
        else
            log_msg "Port $port has no link (no cable)"
            # Assume unused ports are LAN by default
            lan_ports="$lan_ports $port"
        fi
    done
    
    # Clean up whitespace
    wan_ports=$(echo "$wan_ports" | xargs)
    lan_ports=$(echo "$lan_ports" | xargs)
    
    # If no WAN detected, use traditional naming
    if [ -z "$wan_ports" ]; then
        log_msg "No WAN ports detected via auto-detection, using defaults"
        # Check for common WAN port names
        for port in eth0 eth1 wan; do
            if echo "$all_ports" | grep -qw "$port"; then
                wan_ports="$port"
                # Remove from lan_ports if present
                lan_ports=$(echo "$lan_ports" | sed "s/$port//g" | xargs)
                break
            fi
        done
    fi
    
    # Ensure we have at least one LAN port
    if [ -z "$lan_ports" ]; then
        log_msg "WARNING: No LAN ports detected, this seems wrong"
        # Keep at least one port for LAN
        first_port=$(echo "$all_ports" | awk '{print $1}')
        if [ "$first_port" != "$wan_ports" ]; then
            lan_ports="$first_port"
        fi
    fi
    
    log_msg "Detected WAN ports: ${wan_ports:-none}"
    log_msg "Detected LAN ports: ${lan_ports:-none}"
    
    echo "WAN=$wan_ports"
    echo "LAN=$lan_ports"
}

# Apply detected configuration to UCI
apply_port_configuration() {
    local wan_ports="$1"
    local lan_ports="$2"
    
    log_msg "Applying port configuration..."
    log_msg "Configuring WAN: $wan_ports"
    log_msg "Configuring LAN: $lan_ports"
    
    # Backup current config
    cp /etc/config/network /etc/config/network.backup-autodetect 2>/dev/null
    
    # Configure WAN interfaces
    local wan_num=1
    for port in $wan_ports; do
        local wan_name="wan"
        [ $wan_num -gt 1 ] && wan_name="wan${wan_num}"
        
        log_msg "Configuring $port as $wan_name"
        
        uci -q batch <<-EOF
			delete network.$wan_name
			set network.$wan_name=interface
			set network.$wan_name.device='$port'
			set network.$wan_name.proto='dhcp'
			set network.$wan_name.metric='$((wan_num * 10))'
			set network.$wan_name.multipath='on'
		EOF
        
        wan_num=$((wan_num + 1))
    done
    
    # Configure LAN bridge with remaining ports
    if [ -n "$lan_ports" ]; then
        log_msg "Configuring LAN bridge with: $lan_ports"
        
        # Create bridge device
        uci -q batch <<-EOF
			delete network.@device[0]
			add network device
			set network.@device[-1].name='br-lan'
			set network.@device[-1].type='bridge'
		EOF
        
        # Add ports to bridge
        local port_list=""
        for port in $lan_ports; do
            port_list="$port_list $port"
        done
        port_list=$(echo "$port_list" | xargs)
        
        uci set network.@device[-1].ports="$port_list"
        
        # Configure LAN interface
        uci -q batch <<-EOF
			set network.lan.device='br-lan'
			set network.lan.proto='static'
			set network.lan.ipaddr='192.168.2.1'
			set network.lan.netmask='255.255.255.0'
			set network.lan.ip6assign='60'
		EOF
    fi
    
    # Commit changes
    uci commit network
    
    log_msg "Port configuration applied successfully"
    log_msg "Reloading network..."
    
    # Mark configuration as applied
    touch "$CONFIG_APPLIED"
    
    # Reload network
    /etc/init.d/network reload
    
    return 0
}

# Main function
main() {
    log_msg "Starting automatic WAN/LAN port detection"
    
    # Check if already configured
    if [ -f "$CONFIG_APPLIED" ]; then
        log_msg "Port auto-configuration already applied"
        log_msg "Delete $CONFIG_APPLIED to re-run detection"
        exit 0
    fi
    
    # Wait for network interfaces to initialize
    log_msg "Waiting for network interfaces to initialize..."
    sleep 5
    
    # Detect port roles
    local detection=$(detect_port_roles)
    
    # Parse results
    local wan_ports=$(echo "$detection" | grep "^WAN=" | cut -d= -f2)
    local lan_ports=$(echo "$detection" | grep "^LAN=" | cut -d= -f2)
    
    # Save detection state
    cat > "$DETECTION_FILE" <<-EOF
		# Auto-detected port configuration
		# Generated: $(date)
		WAN_PORTS="$wan_ports"
		LAN_PORTS="$lan_ports"
	EOF
    
    # Apply configuration
    if apply_port_configuration "$wan_ports" "$lan_ports"; then
        log_msg "Auto-configuration completed successfully"
        log_msg "WAN ports: $wan_ports"
        log_msg "LAN ports: $lan_ports"
    else
        log_msg "ERROR: Failed to apply configuration"
        return 1
    fi
}

# Run main function
main

exit 0
