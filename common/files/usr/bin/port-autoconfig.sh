#!/bin/sh
#
# OpenMPTCProuter Optimized - First Boot Port Setup
# Simple first-boot helper that detects a basic WAN/LAN configuration
# Works like any standard router - smart defaults, then user customizes
# Only runs once on first boot, then user has full control
#

LOG_TAG="port-autoconfig"
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
        if [ -e "$iface" ]; then
            local port
            port=$(basename "$iface")
            # Validate port name
            if echo "$port" | grep -qE '^[a-zA-Z0-9_-]+$'; then
                ports="$ports $port"
            fi
        fi
    done

    # Check for lan* interfaces (common on some devices)
    for iface in /sys/class/net/lan*; do
        if [ -e "$iface" ]; then
            local port
            port=$(basename "$iface")
            # Validate port name
            if echo "$port" | grep -qE '^[a-zA-Z0-9_-]+$'; then
                ports="$ports $port"
            fi
        fi
    done

    # Check for wan* interfaces
    for iface in /sys/class/net/wan*; do
        if [ -e "$iface" ]; then
            local port
            port=$(basename "$iface")
            # Validate port name
            if echo "$port" | grep -qE '^[a-zA-Z0-9_-]+$'; then
                ports="$ports $port"
            fi
        fi
    done

    echo "$ports" | xargs
}

# Simple port assignment like standard routers
# Look for a WAN port by name, everything else is LAN
detect_port_roles() {
    local all_ports=$(get_all_ports)
    local wan_port=""
    local lan_ports=""
    
    log_msg "First boot setup - detecting ports: $all_ports"
    
    # Strategy: Look for common WAN port names first (like standard routers)
    # If found, use it. Otherwise pick the first port as WAN.
    # Everything else becomes LAN.
    
    # Check for dedicated WAN port by name (most devices have this)
    for port in wan wan0 eth0; do
        if echo "$all_ports" | grep -qw "$port"; then
            log_msg "Found WAN port: $port"
            wan_port="$port"
            # Remove from all_ports - use word boundary matching to avoid regex issues
            all_ports=$(echo "$all_ports" | tr ' ' '\n' | grep -v "^${port}$" | tr '\n' ' ' | xargs)
            break
        fi
    done
    
    # If no WAN port found by name and we have multiple ports,
    # use the first one as WAN (like most routers do)
    if [ -z "$wan_port" ]; then
        local port_count=$(echo "$all_ports" | wc -w)
        
        if [ $port_count -gt 1 ]; then
            wan_port=$(echo "$all_ports" | awk '{print $1}')
            log_msg "Using first port as WAN: $wan_port"
            # Remove from all_ports - use word boundary matching to avoid regex issues
            all_ports=$(echo "$all_ports" | tr ' ' '\n' | grep -v "^${wan_port}$" | tr '\n' ' ' | xargs)
        elif [ $port_count -eq 1 ]; then
            # Only one port - make it LAN so user can login
            log_msg "Single port detected - using as LAN for initial login"
            wan_port=""
        fi
    fi
    
    # Everything else is LAN
    lan_ports="$all_ports"
    
    # Ensure we have at least one LAN port for login
    if [ -z "$lan_ports" ]; then
        log_msg "WARNING: No LAN ports - need at least one for login"
        if [ -n "$wan_port" ]; then
            # Move WAN to LAN
            lan_ports="$wan_port"
            wan_port=""
        fi
    fi
    
    log_msg "Default setup - WAN: ${wan_port:-none}, LAN: $lan_ports"
    
    echo "WAN=$wan_port"
    echo "LAN=$lan_ports"
}

# Apply simple default configuration
apply_port_configuration() {
    local wan_port="$1"
    local lan_ports="$2"
    
    log_msg "Applying first boot configuration..."
    
    # Configure WAN if we have one
    if [ -n "$wan_port" ]; then
        # Validate WAN port name
        if ! echo "$wan_port" | grep -qE '^[a-zA-Z0-9_-]+$'; then
            log_msg "ERROR: Invalid WAN port name: $wan_port"
            return 1
        fi

        log_msg "Configuring WAN on: $wan_port"

        # Use quoted heredoc
        uci -q batch <<-'EOF'
			delete network.wan
			set network.wan=interface
			set network.wan.proto='dhcp'
			set network.wan.metric='10'
			set network.wan.multipath='on'
		EOF

        # Set device separately with validation
        if ! uci -q set "network.wan.device=$wan_port"; then
            log_msg "ERROR: Failed to configure WAN device"
            return 1
        fi
    else
        log_msg "No WAN port - user can configure later"
    fi

    # Configure LAN bridge with remaining ports
    if [ -n "$lan_ports" ]; then
        log_msg "Configuring LAN on: $lan_ports"

        # Create bridge device - use quoted heredoc
        uci -q batch <<-'EOF'
			delete network.@device[0]
			add network device
			set network.@device[-1].name='br-lan'
			set network.@device[-1].type='bridge'
		EOF

        # Set ports separately with validation
        if ! uci -q set "network.@device[-1].ports=$lan_ports"; then
            log_msg "ERROR: Failed to set LAN ports"
            return 1
        fi

        # Configure LAN interface
        uci -q batch <<-'EOF'
			set network.lan.device='br-lan'
			set network.lan.proto='static'
			set network.lan.ipaddr='192.168.2.1'
			set network.lan.netmask='255.255.255.0'
			set network.lan.ip6assign='60'
		EOF
    fi
    
    # Commit changes
    if ! uci commit network 2>/dev/null; then
        log_msg "ERROR: Failed to commit network configuration"
        return 1
    fi

    log_msg "Configuration applied successfully"
    
    # Mark as configured
    touch "$CONFIG_APPLIED"
    
    # Reload network
    /etc/init.d/network reload
    
    return 0
}

# Main function
main() {
    log_msg "═══════════════════════════════════════════════════"
    log_msg "OpenMPTCProuter First Boot Setup"
    log_msg "═══════════════════════════════════════════════════"
    
    # Check if already configured
    if [ -f "$CONFIG_APPLIED" ]; then
        log_msg "Already configured - skipping"
        log_msg "You can customize via web UI at http://192.168.2.1"
        exit 0
    fi
    
    log_msg "Running one-time first boot setup..."
    
    # Wait for interfaces
    sleep 5
    
    # Detect ports
    local detection=$(detect_port_roles)
    
    # Parse results
    local wan_port=$(echo "$detection" | grep "^WAN=" | cut -d= -f2)
    local lan_ports=$(echo "$detection" | grep "^LAN=" | cut -d= -f2)
    
    # Apply configuration
    if apply_port_configuration "$wan_port" "$lan_ports"; then
        log_msg "═══════════════════════════════════════════════════"
        log_msg "✓ First boot setup complete!"
        log_msg "  WAN: ${wan_port:-none (add USB modem or configure manually)}"
        log_msg "  LAN: $lan_ports"
        log_msg "  Login: http://192.168.2.1"
        log_msg "  Customize everything via web UI"
        log_msg "═══════════════════════════════════════════════════"
    else
        log_msg "ERROR: Setup failed"
        return 1
    fi
}

# Run
main

exit 0
