#!/bin/sh
#
# OpenMPTCProuter Optimized - WiFi Auto-Configuration
# Automatically detects and configures WiFi radios
# Enables WiFi with secure defaults
#

LOG_TAG="wifi-autoconfig"

log_msg() {
    logger -t "$LOG_TAG" "$1"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Generate a random but memorable WiFi password
generate_wifi_password() {
    # Generate a 12-character password with readable characters
    tr -dc 'A-Za-z0-9' < /dev/urandom | head -c 12
}

# Get device hostname for SSID
get_device_name() {
    local hostname=$(uci -q get system.@system[0].hostname)
    echo "${hostname:-OMR-Optimized}"
}

# Configure a single WiFi radio
configure_radio() {
    local radio="$1"
    local band="$2"  # 2g, 5g, or 6g
    local channel="$3"
    
    log_msg "Configuring radio $radio ($band)"
    
    # Determine htmode based on band
    local htmode="HE40"
    local country="US"
    
    case "$band" in
        2g)
            htmode="HE20"
            [ -z "$channel" ] && channel="6"
            ;;
        5g)
            htmode="HE80"
            [ -z "$channel" ] && channel="36"
            ;;
        6g)
            htmode="EHT160"  # WiFi 7
            [ -z "$channel" ] && channel="33"
            ;;
    esac
    
    # Configure radio
    uci -q batch <<-EOF
		set wireless.$radio.disabled='0'
		set wireless.$radio.country='$country'
		set wireless.$radio.channel='$channel'
		set wireless.$radio.htmode='$htmode'
		set wireless.$radio.cell_density='0'
	EOF
    
    # Enable advanced features if supported
    case "$band" in
        6g)
            # WiFi 7 specific features
            uci -q batch <<-EOF
				set wireless.$radio.he_bss_color='1'
				set wireless.$radio.multiple_bssid='1'
			EOF
            ;;
        5g)
            # WiFi 6/6E features
            uci -q batch <<-EOF
				set wireless.$radio.he_su_beamformer='1'
				set wireless.$radio.he_su_beamformee='1'
				set wireless.$radio.he_mu_beamformer='1'
			EOF
            ;;
    esac
}

# Configure WiFi interface (SSID)
configure_wifi_interface() {
    local radio="$1"
    local band="$2"
    local ssid="$3"
    local password="$4"
    local iface_name="default_${radio}"
    
    log_msg "Creating WiFi network on $radio: $ssid"
    
    # Delete existing interface if present
    uci -q delete wireless.$iface_name
    
    # Create new interface
    uci -q batch <<-EOF
		set wireless.$iface_name=wifi-iface
		set wireless.$iface_name.device='$radio'
		set wireless.$iface_name.network='lan'
		set wireless.$iface_name.mode='ap'
		set wireless.$iface_name.ssid='$ssid'
		set wireless.$iface_name.encryption='sae-mixed'
		set wireless.$iface_name.key='$password'
		set wireless.$iface_name.ieee80211w='1'
		set wireless.$iface_name.wpa_disable_eapol_key_retries='1'
		set wireless.$iface_name.isolate='0'
		set wireless.$iface_name.disabled='0'
	EOF
    
    # Enable Fast BSS Transition (802.11r) for better roaming
    uci -q batch <<-EOF
		set wireless.$iface_name.ieee80211r='1'
		set wireless.$iface_name.ft_over_ds='1'
		set wireless.$iface_name.ft_psk_generate_local='1'
	EOF
}

# Detect and configure all WiFi radios
configure_all_radios() {
    local device_name=$(get_device_name)
    local wifi_password=$(generate_wifi_password)
    local radio_count=0
    
    log_msg "Starting WiFi auto-configuration"
    log_msg "Device: $device_name"
    
    # Save password to file for user reference
    cat > /etc/wifi-password.txt <<-EOF
		OpenMPTCProuter Optimized - WiFi Configuration
		Generated: $(date)
		
		WiFi Password: $wifi_password
		
		Networks:
	EOF
    
    # Detect and configure each radio
    for radio_path in /sys/class/ieee80211/phy*; do
        [ ! -e "$radio_path" ] && continue
        
        local phy=$(basename "$radio_path")
        local radio="radio${radio_count}"
        
        # Get radio capabilities
        local bands=$(iw phy "$phy" info 2>/dev/null | grep "Band" | awk '{print $2}')
        
        # Determine band
        local band=""
        local freq_info=$(iw phy "$phy" info 2>/dev/null | grep "MHz")
        
        if echo "$freq_info" | grep -q "6[0-9][0-9][0-9] MHz"; then
            band="6g"
        elif echo "$freq_info" | grep -q "5[0-9][0-9][0-9] MHz"; then
            band="5g"
        else
            band="2g"
        fi
        
        log_msg "Detected $phy as $band radio"
        
        # Configure radio
        configure_radio "$radio" "$band"
        
        # Create SSID based on band
        local ssid="${device_name}"
        case "$band" in
            2g) ssid="${device_name}" ;;
            5g) ssid="${device_name}-5G" ;;
            6g) ssid="${device_name}-6G" ;;
        esac
        
        # Configure WiFi interface
        configure_wifi_interface "$radio" "$band" "$ssid" "$wifi_password"
        
        # Add to password file
        echo "  - $ssid (${band^^})" >> /etc/wifi-password.txt
        
        radio_count=$((radio_count + 1))
    done
    
    echo "" >> /etc/wifi-password.txt
    echo "Access web interface at: http://192.168.2.1" >> /etc/wifi-password.txt
    
    if [ $radio_count -eq 0 ]; then
        log_msg "WARNING: No WiFi radios detected"
        return 1
    fi
    
    # Commit wireless configuration
    uci commit wireless
    
    log_msg "Configured $radio_count WiFi radio(s)"
    log_msg "WiFi password saved to /etc/wifi-password.txt"
    log_msg "Password: $wifi_password"
    
    return 0
}

# Main
main() {
    log_msg "Starting WiFi auto-configuration"
    
    # Wait for WiFi drivers to load
    sleep 3
    
    # Configure all radios
    if configure_all_radios; then
        log_msg "WiFi configuration complete"
        
        # Reload WiFi
        wifi reload
        
        # Display password on console
        echo ""
        echo "=========================================="
        echo "  WiFi Configuration Complete!"
        echo "=========================================="
        cat /etc/wifi-password.txt
        echo "=========================================="
        echo ""
    else
        log_msg "WiFi configuration failed or no radios found"
    fi
}

main
exit 0
