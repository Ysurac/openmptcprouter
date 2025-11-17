#!/bin/sh
#
# Emergency LAN Restore Script
# Called by reset button or can be run from serial console
# Restores at least one LAN port for network access
#

LOG_TAG="emergency-restore"

log_msg() {
    logger -t "$LOG_TAG" "$1"
    echo "$1"
}

log_msg "═══════════════════════════════════════════════════"
log_msg "EMERGENCY LAN RESTORE"
log_msg "═══════════════════════════════════════════════════"

# Find any available physical port
emergency_port=""

# Try to find a port not assigned to WAN
for iface in /sys/class/net/eth* /sys/class/net/lan*; do
    if [ -e "$iface" ]; then
        port=$(basename "$iface")
        
        # Check if this port is assigned to a WAN
        is_wan=0
        for wan in $(uci show network 2>/dev/null | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1); do
            wan_device=$(uci -q get network.$wan.device)
            if [ "$wan_device" = "$port" ]; then
                is_wan=1
                break
            fi
        done
        
        # If not a WAN, use it for emergency LAN
        if [ $is_wan -eq 0 ]; then
            emergency_port="$port"
            break
        fi
    fi
done

# If all ports are WANs, take the last one back
if [ -z "$emergency_port" ]; then
    for wan in $(uci show network 2>/dev/null | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1 | tail -n 1); do
        emergency_port=$(uci -q get network.$wan.device)
        if [ -n "$emergency_port" ]; then
            log_msg "Taking WAN port $emergency_port for emergency LAN"
            uci delete network.$wan
            break
        fi
    done
fi

# If still no port, assign ALL ports to LAN
if [ -z "$emergency_port" ]; then
    log_msg "No ports available - assigning ALL ports to LAN"
    
    # Delete all WANs
    for wan in $(uci show network 2>/dev/null | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1); do
        uci delete network.$wan
    done
    
    # Collect all physical ports
    all_ports=""
    for iface in /sys/class/net/eth* /sys/class/net/lan*; do
        if [ -e "$iface" ]; then
            port=$(basename "$iface")
            all_ports="$all_ports $port"
        fi
    done
    
    all_ports=$(echo "$all_ports" | xargs)
    
    if [ -n "$all_ports" ]; then
        uci -q batch <<-EOF
			delete network.@device[0]
			add network device
			set network.@device[-1].name='br-lan'
			set network.@device[-1].type='bridge'
			set network.@device[-1].ports='$all_ports'
			set network.lan.device='br-lan'
			set network.lan.proto='static'
			set network.lan.ipaddr='192.168.2.1'
			set network.lan.netmask='255.255.255.0'
			set network.lan.ip6assign='60'
		EOF
        
        uci commit network
        /etc/init.d/network restart
        
        log_msg "✓ ALL ports assigned to LAN"
        log_msg "✓ LAN IP: 192.168.2.1"
        log_msg "✓ Connect to any port and access http://192.168.2.1"
        exit 0
    fi
fi

# Create LAN with emergency port
if [ -n "$emergency_port" ]; then
    log_msg "Restoring LAN on port: $emergency_port"
    
    uci -q batch <<-EOF
		delete network.@device[0]
		add network device
		set network.@device[-1].name='br-lan'
		set network.@device[-1].type='bridge'
		set network.@device[-1].ports='$emergency_port'
		set network.lan.device='br-lan'
		set network.lan.proto='static'
		set network.lan.ipaddr='192.168.2.1'
		set network.lan.netmask='255.255.255.0'
		set network.lan.ip6assign='60'
	EOF
    
    uci commit network
    
    # Ensure DHCP is enabled
    uci -q batch <<-EOF
		set dhcp.lan=dhcp
		set dhcp.lan.interface='lan'
		set dhcp.lan.start='100'
		set dhcp.lan.limit='150'
		set dhcp.lan.leasetime='12h'
		set dhcp.lan.dhcpv4='server'
		commit dhcp
	EOF
    
    # Restart network services
    /etc/init.d/network restart
    /etc/init.d/dnsmasq restart
    
    log_msg "═══════════════════════════════════════════════════"
    log_msg "✓ EMERGENCY RESTORE COMPLETE"
    log_msg "  LAN Port: $emergency_port"
    log_msg "  LAN IP: 192.168.2.1"
    log_msg "  Access: http://192.168.2.1"
    log_msg "═══════════════════════════════════════════════════"
else
    log_msg "ERROR: Could not find any ports for LAN!"
    exit 1
fi

exit 0
