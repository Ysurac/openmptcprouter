#!/bin/sh
# Carrier Aggregation Optimization Script for 5G Modems
# Optimizes Quectel RM551E-GL and other 5G modems for maximum CA performance

# Function to send AT command and wait for response
send_at_command() {
    local device="$1"
    local command="$2"
    local timeout="${3:-2}"

    # Validate device path
    if [ ! -c "$device" ]; then
        logger -t modem_ca "ERROR: Invalid device: $device"
        return 1
    fi

    # Sanitize device path - only allow known modem devices
    case "$device" in
        /dev/ttyUSB*|/dev/cdc-wdm*) ;;
        *)
            logger -t modem_ca "ERROR: Unsafe device path: $device"
            return 1
            ;;
    esac

    # Send command
    echo "$command" > "$device"
    sleep "$timeout"

    logger -t modem_ca "Sent command: $command to $device"
}

# Function to detect modem model
detect_modem_model() {
    local device="$1"
    local response
    
    # Query modem information
    echo "ATI" > "$device"
    sleep 1
    response=$(timeout 2 cat "$device" 2>/dev/null | head -n 5)
    
    echo "$response"
}

# Cleanup on exit - restore modem to functional state
cleanup_modem() {
    if [ -n "$CURRENT_MODEM_DEVICE" ] && [ -c "$CURRENT_MODEM_DEVICE" ]; then
        logger -t modem_ca "Cleanup: Restoring modem on $CURRENT_MODEM_DEVICE"
        # Restore modem to functional state (exit airplane mode)
        echo "AT+CFUN=1" > "$CURRENT_MODEM_DEVICE" 2>/dev/null || true
    fi
}

trap cleanup_modem EXIT INT TERM

set_modem_ca_optimization() {
    local device="$1"
    local model_info="$2"

    # Store for cleanup handler
    CURRENT_MODEM_DEVICE="$device"

    logger -t modem_ca "Optimizing modem on $device: $model_info"
    
    # RM551E specific optimizations
    if echo "$model_info" | grep -qi "RM551E\|RM551"; then
        logger -t modem_ca "Detected RM551E modem, applying enhanced settings"
        
        # Reset to known state
        send_at_command "$device" "AT+CFUN=0" 2
        
        # Configure USB mode for optimal performance
        # Mode 3 = QMI, Mode 1 = MBIM, Mode 0 = RNDIS
        send_at_command "$device" "AT+QCFG=\"usbnet\",0" 2
        
        # Enable 5G NR carrier aggregation
        send_at_command "$device" "AT+QNWCFG=\"nr5g_carrier_aggregation\",1" 1
        
        # Enable EN-DC (E-UTRA-NR Dual Connectivity)
        send_at_command "$device" "AT+QNWCFG=\"endc\",1" 1
        
        # Set maximum bandwidth aggregation
        send_at_command "$device" "AT+QNWCFG=\"nr5g_bandwidth\",\"auto\"" 1
        
        # Enable all LTE carrier aggregation combinations
        send_at_command "$device" "AT+QNWCFG=\"lte_ca\",1" 1
        
        # Enable data aggregation with optimized URB size
        send_at_command "$device" "AT+QCFG=\"data_aggregation\",1,32768,64" 1
        
        # Set network search mode (auto LTE/5G)
        send_at_command "$device" "AT+QNWPREFCFG=\"mode_pref\",AUTO" 1
        
        # Enable flow control for better stability
        send_at_command "$device" "AT+IFC=2,2" 1
        
        # Optimize for low latency
        send_at_command "$device" "AT+QCFG=\"nat\",1" 1
        
        # Enable function
        send_at_command "$device" "AT+CFUN=1" 3
        
        logger -t modem_ca "RM551E optimizations completed on $device"
    else
        # Generic Quectel 5G optimizations
        logger -t modem_ca "Applying generic 5G optimizations"
        
        send_at_command "$device" "AT+QNWCFG=\"nr5g_carrier_aggregation\",1" 1
        send_at_command "$device" "AT+QNWCFG=\"endc\",1" 1
        send_at_command "$device" "AT+QNWCFG=\"nr5g_bandwidth\",\"auto\"" 1
        send_at_command "$device" "AT+QNWCFG=\"lte_ca\",1" 1
        send_at_command "$device" "AT+IFC=2,2" 1
        
        logger -t modem_ca "Generic optimizations completed on $device"
    fi
}

# Main execution
logger -t modem_ca "Starting modem optimization scan"

# Detect Quectel modems
for device in /dev/ttyUSB* /dev/cdc-wdm*; do
    if [ -c "$device" ]; then
        # Check if it's a Quectel modem
        model_info=$(detect_modem_model "$device")
        if echo "$model_info" | grep -qi "Quectel"; then
            logger -t modem_ca "Found Quectel modem at $device"
            set_modem_ca_optimization "$device" "$model_info"
            break  # Only configure the first modem found
        fi
    fi
done

logger -t modem_ca "Modem optimization scan completed"
exit 0
