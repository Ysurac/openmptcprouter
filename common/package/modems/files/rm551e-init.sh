#!/bin/sh
# Quectel RM551E-GL 5G Modem Initialization Script
# Ensures proper detection and configuration of RM551E modems
# Enhanced for stability and automatic recovery

MODEM_VENDOR_ID="2c7c"
MODEM_PRODUCT_IDS="0800 0801 0900 0901"
LOG_TAG="rm551e_init"
MAX_INIT_ATTEMPTS=3
INIT_WAIT_TIME=5

log_msg() {
    logger -t "$LOG_TAG" "$1"
    echo "[$(date)] $1"
}

# Check if modem is present
detect_rm551e() {
    local found=0
    
    for pid in $MODEM_PRODUCT_IDS; do
        if lsusb | grep -qi "${MODEM_VENDOR_ID}:${pid}"; then
            log_msg "Detected RM551E modem (${MODEM_VENDOR_ID}:${pid})"
            found=1
            break
        fi
    done
    
    return $found
}

# Load required kernel modules
load_modem_drivers() {
    log_msg "Loading modem drivers..."
    
    # USB Serial drivers
    modprobe option 2>/dev/null || true
    modprobe usb_wwan 2>/dev/null || true
    modprobe qcserial 2>/dev/null || true
    
    # Network drivers
    modprobe qmi_wwan 2>/dev/null || true
    modprobe cdc_mbim 2>/dev/null || true
    modprobe cdc_ncm 2>/dev/null || true
    modprobe cdc_ether 2>/dev/null || true
    modprobe rndis_host 2>/dev/null || true
    modprobe cdc_wdm 2>/dev/null || true
    
    # Add USB ID to drivers if not auto-detected
    echo "$MODEM_VENDOR_ID 0801" > /sys/bus/usb-serial/drivers/option1/new_id 2>/dev/null || true
    echo "$MODEM_VENDOR_ID 0801" > /sys/bus/usb/drivers/qmi_wwan/new_id 2>/dev/null || true
    echo "$MODEM_VENDOR_ID 0800" > /sys/bus/usb/drivers/cdc_mbim/new_id 2>/dev/null || true
    
    log_msg "Modem drivers loaded"
}

# Wait for device to be ready
wait_for_device_ready() {
    local max_wait=30
    local count=0
    
    log_msg "Waiting for modem devices to be ready..."
    
    while [ $count -lt $max_wait ]; do
        if [ -c /dev/ttyUSB0 ] || [ -c /dev/ttyUSB1 ] || [ -c /dev/ttyUSB2 ]; then
            log_msg "Modem devices detected"
            return 0
        fi
        sleep 1
        count=$((count + 1))
    done
    
    log_msg "WARNING: Timeout waiting for modem devices"
    return 1
}

# Find the AT command port (usually ttyUSB2 for RM551E)
find_at_port() {
    # RM551E typically uses:
    # ttyUSB0 - DM (Diagnostic)
    # ttyUSB1 - NMEA (GPS)
    # ttyUSB2 - AT command interface
    # ttyUSB3 - AT command interface (backup)
    
    for port in /dev/ttyUSB2 /dev/ttyUSB3 /dev/ttyUSB1 /dev/ttyUSB0; do
        if [ -c "$port" ]; then
            # Test if port responds to AT commands
            if timeout 2 sh -c "echo -e 'AT\r' > $port 2>/dev/null && cat $port 2>/dev/null" | grep -q "OK"; then
                echo "$port"
                return 0
            fi
        fi
    done
    
    return 1
}

# Configure modem for optimal mode
configure_modem_mode() {
    local device="$1"
    local attempt=1
    
    if [ -z "$device" ]; then
        log_msg "No control device specified"
        return 1
    fi
    
    log_msg "Configuring modem on $device"
    
    while [ $attempt -le $MAX_INIT_ATTEMPTS ]; do
        log_msg "Configuration attempt $attempt/$MAX_INIT_ATTEMPTS"
        
        # Wait for device to be ready
        sleep $INIT_WAIT_TIME
        
        # Test basic connectivity
        if ! timeout 3 sh -c "echo -e 'AT\r' > $device && cat $device" | grep -q "OK"; then
            log_msg "Device $device not responding on attempt $attempt"
            attempt=$((attempt + 1))
            continue
        fi
        
        # Disable echo for cleaner communication
        echo -e 'ATE0\r' > "$device" 2>/dev/null
        sleep 1
        
        # Get modem info
        log_msg "Getting modem information..."
        timeout 3 sh -c "echo -e 'ATI\r' > $device && cat $device" 2>/dev/null | head -n 10
        
        # Check firmware version
        echo -e 'AT+QGMR\r' > "$device" 2>/dev/null
        sleep 1
        
        # Get current USB configuration
        local usb_mode=$(timeout 3 sh -c "echo -e 'AT+QCFG=\"usbnet\"\r' > $device && cat $device" 2>/dev/null | grep "+QCFG" | cut -d, -f1 | cut -d'"' -f2)
        log_msg "Current USB mode: ${usb_mode:-unknown}"
        
        # Set to QMI mode (0) for best performance with OpenWrt
        # Mode 0 = QMI (recommended)
        # Mode 1 = MBIM
        # Mode 5 = RNDIS
        log_msg "Setting USB mode to QMI (0)"
        echo -e 'AT+QCFG="usbnet",0\r' > "$device" 2>/dev/null
        sleep 1
        
        # Enable all LTE bands for better compatibility
        log_msg "Configuring LTE bands"
        echo -e 'AT+QCFG="band",0,0,1\r' > "$device" 2>/dev/null
        sleep 1
        
        # Enable 5G NR bands
        log_msg "Configuring 5G NR bands"
        echo -e 'AT+QNWPREFCFG="nr5g_band",1:2:3:5:7:8:12:20:25:28:38:40:41:48:66:71:77:78:79\r' > "$device" 2>/dev/null
        sleep 1
        
        # Set preferred network mode to AUTO (LTE and 5G)
        log_msg "Setting network mode to AUTO"
        echo -e 'AT+QNWPREFCFG="mode_pref",AUTO\r' > "$device" 2>/dev/null
        sleep 1
        
        # Enable carrier aggregation
        log_msg "Enabling carrier aggregation"
        echo -e 'AT+QNWCFG="lte_ca",1\r' > "$device" 2>/dev/null
        sleep 1
        
        # Enable 5G NR carrier aggregation
        echo -e 'AT+QNWCFG="nr5g_carrier_aggregation",1\r' > "$device" 2>/dev/null
        sleep 1
        
        # Enable EN-DC (E-UTRA-NR Dual Connectivity)
        echo -e 'AT+QNWCFG="endc",1\r' > "$device" 2>/dev/null
        sleep 1
        
        # Optimize URB size for better throughput
        log_msg "Optimizing data transfer settings"
        echo -e 'AT+QCFG="data_interface",0,0\r' > "$device" 2>/dev/null
        sleep 1
        
        # Set QMI aggregation for better performance
        echo -e 'AT+QMAP="mpdn_rule",1,1,0,1,1,"INTERNET"\r' > "$device" 2>/dev/null
        sleep 1
        
        # Verify configuration
        log_msg "Verifying configuration..."
        timeout 3 sh -c "echo -e 'AT+QCFG=\"usbnet\"\r' > $device && cat $device" 2>/dev/null | head -n 5
        
        log_msg "Modem configuration complete"
        return 0
    done
    
    log_msg "ERROR: Failed to configure modem after $MAX_INIT_ATTEMPTS attempts"
    return 1
}

# Main initialization
log_msg "Starting RM551E initialization (enhanced)"

# Detect modem
if ! detect_rm551e; then
    log_msg "No RM551E modem detected"
    exit 0
fi

# Load drivers
load_modem_drivers

# Wait for device enumeration
if ! wait_for_device_ready; then
    log_msg "ERROR: Modem devices did not appear"
    exit 1
fi

# Additional wait for stability
sleep 3

# Find control interface
CONTROL_DEV=$(find_at_port)

# Configure modem if control interface found
if [ -n "$CONTROL_DEV" ]; then
    log_msg "Found AT command port: $CONTROL_DEV"
    if configure_modem_mode "$CONTROL_DEV"; then
        log_msg "Modem configured successfully"
    else
        log_msg "WARNING: Modem configuration had issues"
    fi
else
    log_msg "WARNING: Could not find modem AT command port"
fi

# Trigger carrier aggregation optimization
if [ -x /usr/bin/modem-ca-optimize.sh ]; then
    log_msg "Running carrier aggregation optimization"
    /usr/bin/modem-ca-optimize.sh &
fi

# Start stability monitor if available
if [ -x /usr/bin/rm551e-monitor.sh ]; then
    log_msg "Starting stability monitor"
    /usr/bin/rm551e-monitor.sh &
fi

log_msg "RM551E initialization complete"
exit 0
