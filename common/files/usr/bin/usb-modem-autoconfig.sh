#!/bin/sh
#
# OpenMPTCProuter Optimized - USB Modem Auto-Configuration
# Automatically detects and configures USB modems (4G/5G) as WAN interfaces
# Supports multiple concurrent modems with MPTCP bonding
#

LOG_TAG="usb-modem-autoconfig"

# Load USA carrier APN database if available
if [ -f "/etc/usa-carrier-apns.conf" ]; then
    . /etc/usa-carrier-apns.conf
fi

log_msg() {
    logger -t "$LOG_TAG" "$1"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Get carrier configuration from UCI or environment
get_carrier_config() {
    local wan_name="$1"
    local carrier=""

    # Priority 1: Per-interface carrier setting
    carrier=$(uci -q get "network.$wan_name.carrier")

    # Priority 2: Global carrier setting
    [ -z "$carrier" ] && carrier=$(uci -q get "network.globals.carrier")

    # Priority 3: Environment variable
    [ -z "$carrier" ] && carrier="$USA_CARRIER"

    echo "$carrier"
}

# Get APN settings for the configured carrier
get_apn_settings() {
    local carrier="$1"
    local default_apn="internet"
    local apn="$default_apn"
    local username=""
    local password=""
    local auth_type="PAP"

    # If no carrier specified, use default
    if [ -z "$carrier" ]; then
        log_msg "No carrier specified, using default APN: $default_apn"
        echo "$apn:$username:$password:$auth_type"
        return
    fi

    # Get carrier APN from database
    if type get_carrier_apn >/dev/null 2>&1; then
        local carrier_data=$(get_carrier_apn "$carrier")
        if [ $? -eq 0 ] && [ -n "$carrier_data" ]; then
            log_msg "Found carrier APN for '$carrier'"
            if type parse_apn_data >/dev/null 2>&1; then
                parse_apn_data "$carrier_data"
                apn="$APN_NAME"
                username="$APN_USERNAME"
                password="$APN_PASSWORD"
                auth_type="$APN_AUTH_TYPE"
                log_msg "Using APN: $apn (auth: $auth_type)"
            fi
        else
            log_msg "WARNING: Carrier '$carrier' not found in database, using default APN"
        fi
    else
        log_msg "WARNING: USA carrier APN database not loaded, using default APN"
    fi

    echo "$apn:$username:$password:$auth_type"
}

# Detect USB modems (QMI, MBIM, RNDIS, NCM)
detect_usb_modems() {
    local modems=""
    
    # QMI modems (cdc-wdm devices)
    for dev in /dev/cdc-wdm*; do
        if [ -c "$dev" ]; then
            local iface=$(basename "$dev")
            # Get the corresponding network interface
            local net_iface=$(ls -1 /sys/class/usbmisc/$iface/device/net/ 2>/dev/null | head -n1)
            if [ -n "$net_iface" ]; then
                modems="$modems qmi:$net_iface:$dev"
                log_msg "Found QMI modem: $net_iface ($dev)"
            fi
        fi
    done
    
    # MBIM modems
    for dev in /dev/cdc-wdm*; do
        if [ -c "$dev" ]; then
            # Check if it's MBIM by trying umbim
            if command -v umbim >/dev/null 2>&1; then
                if umbim -d "$dev" -n caps 2>/dev/null | grep -q "device_type"; then
                    local iface=$(basename "$dev")
                    local net_iface=$(ls -1 /sys/class/usbmisc/$iface/device/net/ 2>/dev/null | head -n1)
                    if [ -n "$net_iface" ]; then
                        # Check if not already counted as QMI
                        if ! echo "$modems" | grep -q "qmi:$net_iface"; then
                            modems="$modems mbim:$net_iface:$dev"
                            log_msg "Found MBIM modem: $net_iface ($dev)"
                        fi
                    fi
                fi
            fi
        fi
    done
    
    # USB Ethernet adapters that might be modems (usb*, wwan*)
    for iface in /sys/class/net/usb* /sys/class/net/wwan*; do
        if [ -e "$iface" ]; then
            local if_name=$(basename "$iface")
            # Check if it's a USB device
            if [ -e "$iface/device/uevent" ]; then
                local driver=$(cat "$iface/device/uevent" 2>/dev/null | grep DRIVER | cut -d= -f2)
                case "$driver" in
                    qmi_wwan|cdc_mbim|cdc_ncm|rndis_host|cdc_ether)
                        # Check if not already counted
                        if ! echo "$modems" | grep -q ":$if_name:"; then
                            modems="$modems eth:$if_name"
                            log_msg "Found USB modem interface: $if_name (driver: $driver)"
                        fi
                        ;;
                esac
            fi
        fi
    done
    
    echo "$modems" | xargs
}

# Get modem information (signal, carrier, etc.)
get_modem_info() {
    local proto="$1"
    local iface="$2"
    local dev="$3"

    # Validate device path
    if [ -n "$dev" ] && ! echo "$dev" | grep -qE '^/dev/[a-zA-Z0-9_-]+$'; then
        echo "Type: $proto (invalid device path)"
        return
    fi

    local info="Type: $proto"

    case "$proto" in
        qmi)
            if command -v uqmi >/dev/null 2>&1 && [ -n "$dev" ] && [ -c "$dev" ]; then
                # Get signal strength
                local signal
                signal=$(uqmi -d "$dev" --get-signal-info 2>/dev/null | grep rssi | cut -d: -f2 | tr -d ' ,')
                [ -n "$signal" ] && info="$info, Signal: ${signal}dBm"

                # Get network registration
                local network
                network=$(uqmi -d "$dev" --get-serving-system 2>/dev/null | grep description | cut -d\" -f4)
                [ -n "$network" ] && info="$info, Network: $network"
            fi
            ;;
        mbim)
            if command -v umbim >/dev/null 2>&1 && [ -n "$dev" ] && [ -c "$dev" ]; then
                local signal
                signal=$(umbim -d "$dev" -n signal 2>/dev/null | grep rssi | cut -d: -f2)
                [ -n "$signal" ] && info="$info, Signal: ${signal}dBm"
            fi
            ;;
    esac

    echo "$info"
}

# Configure a USB modem as a WAN interface
configure_modem_as_wan() {
    local proto="$1"
    local iface="$2"
    local dev="$3"
    
    # Find the next available WAN number
    local wan_num=1
    while uci -q get network.wan${wan_num} >/dev/null 2>&1; do
        wan_num=$((wan_num + 1))
    done
    
    local wan_name="wan${wan_num}"
    
    log_msg "Configuring $iface as $wan_name (protocol: $proto)"
    
    # Get modem info for logging
    local modem_info=$(get_modem_info "$proto" "$iface" "$dev")
    log_msg "Modem info: $modem_info"

    # Get carrier and APN settings
    local carrier=$(get_carrier_config "$wan_name")
    local apn_settings=$(get_apn_settings "$carrier")
    local apn=$(echo "$apn_settings" | cut -d: -f1)
    local username=$(echo "$apn_settings" | cut -d: -f2)
    local password=$(echo "$apn_settings" | cut -d: -f3)
    local auth_type=$(echo "$apn_settings" | cut -d: -f4)
    
    # Configure based on protocol
    case "$proto" in
        qmi)
            uci -q batch <<-EOF
				delete network.$wan_name
				set network.$wan_name=interface
				set network.$wan_name.proto='qmi'
				set network.$wan_name.device='$dev'
				set network.$wan_name.apn='$apn'
				set network.$wan_name.metric='$((wan_num * 10))'
				set network.$wan_name.multipath='on'
				set network.$wan_name.auto='1'
			EOF
            # Add username and password if provided
            [ -n "$username" ] && uci -q set "network.$wan_name.username=$username"
            [ -n "$password" ] && uci -q set "network.$wan_name.password=$password"
            [ -n "$auth_type" ] && uci -q set "network.$wan_name.auth=$auth_type"
            ;;
        mbim)
            uci -q batch <<-EOF
				delete network.$wan_name
				set network.$wan_name=interface
				set network.$wan_name.proto='mbim'
				set network.$wan_name.device='$dev'
				set network.$wan_name.apn='$apn'
				set network.$wan_name.metric='$((wan_num * 10))'
				set network.$wan_name.multipath='on'
				set network.$wan_name.auto='1'
			EOF
            # Add username and password if provided
            [ -n "$username" ] && uci -q set "network.$wan_name.username=$username"
            [ -n "$password" ] && uci -q set "network.$wan_name.password=$password"
            [ -n "$auth_type" ] && uci -q set "network.$wan_name.auth=$auth_type"
            ;;
        eth)
            # Generic USB ethernet (could be RNDIS, NCM, etc.)
            uci -q batch <<-EOF
				delete network.$wan_name
				set network.$wan_name=interface
				set network.$wan_name.device='$iface'
				set network.$wan_name.proto='dhcp'
				set network.$wan_name.metric='$((wan_num * 10))'
				set network.$wan_name.multipath='on'
			EOF
            ;;
    esac
    
    uci commit network
    
    # Save modem info to a status file
    local status_dir="/var/run/modem-status"
    mkdir -p "$status_dir"

    # Use quoted heredoc to prevent variable expansion issues
    # Write status file with secure permissions
    (
        umask 077
        cat > "$status_dir/$wan_name" <<-EOFF
		INTERFACE=$wan_name
		PHYSICAL_DEVICE=$iface
		PROTOCOL=$proto
		CONTROL_DEVICE=$dev
		INFO=$modem_info
		CONFIGURED_AT=$(date)
		EOFF
    )
    
    log_msg "$wan_name configured successfully"
    
    # Bring up the interface
    ifup "$wan_name" 2>/dev/null &
}

# Check if a modem is already configured
is_modem_configured() {
    local iface="$1"

    # Validate interface name
    if ! echo "$iface" | grep -qE '^[a-zA-Z0-9_/-]+$'; then
        return 1
    fi

    # Check all WAN interfaces
    for wan in $(uci show network 2>/dev/null | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1); do
        local device
        device=$(uci -q get "network.$wan.device")
        # Check both device name and physical device
        if [ "$device" = "$iface" ] || [ "$device" = "/dev/cdc-wdm0" ]; then
            return 0
        fi
    done

    return 1
}

# Remove modems that are no longer present
cleanup_disconnected_modems() {
    log_msg "Checking for disconnected modems..."

    for wan in $(uci show network 2>/dev/null | grep "=interface" | grep -E "\.wan[0-9]" | cut -d. -f2 | cut -d= -f1); do
        local proto
        local device
        proto=$(uci -q get "network.$wan.proto")
        device=$(uci -q get "network.$wan.device")

        # Validate device path
        if [ -n "$device" ] && ! echo "$device" | grep -qE '^/dev/[a-zA-Z0-9_-]+$'; then
            log_msg "WARNING: Invalid device path for $wan: $device"
            continue
        fi

        # Check if this is a modem interface
        case "$proto" in
            qmi|mbim)
                # Check if device still exists
                if [ -n "$device" ] && [ ! -c "$device" ]; then
                    log_msg "Modem on $wan ($device) is disconnected, removing configuration"
                    uci delete "network.$wan"
                    rm -f "/var/run/modem-status/$wan"
                fi
                ;;
        esac
    done

    uci commit network
}

# Main function
main() {
    log_msg "USB modem detection and auto-configuration"
    
    # Wait for USB devices to enumerate
    sleep 3
    
    # Cleanup disconnected modems first
    cleanup_disconnected_modems
    
    # Detect all USB modems
    local modems=$(detect_usb_modems)
    
    if [ -z "$modems" ]; then
        log_msg "No USB modems detected"
        return 0
    fi
    
    log_msg "Detected modems: $modems"
    
    # Configure each modem
    local configured=0
    for modem in $modems; do
        local proto=$(echo "$modem" | cut -d: -f1)
        local iface=$(echo "$modem" | cut -d: -f2)
        local dev=$(echo "$modem" | cut -d: -f3)
        
        # Check if already configured
        if is_modem_configured "$iface"; then
            log_msg "Modem $iface is already configured, skipping"
            continue
        fi
        
        # Configure the modem
        configure_modem_as_wan "$proto" "$iface" "$dev"
        configured=$((configured + 1))
    done
    
    if [ $configured -gt 0 ]; then
        log_msg "✓ Configured $configured new USB modem(s) as additional WAN"
        log_msg "✓ MPTCP bonding enabled for all WANs"
        log_msg "Reloading network to apply changes..."
        /etc/init.d/network reload
    else
        log_msg "No new modems to configure"
    fi
}

# Run main function
main

exit 0
