#!/bin/sh
# Quectel RM551E-GL 5G Modem Initialization Script
# Ensures proper detection and configuration of RM551E modems

MODEM_VENDOR_ID="2c7c"
MODEM_PRODUCT_IDS="0800 0801 0900 0901"
LOG_TAG="rm551e_init"

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
    modprobe option 2>/dev/null
    modprobe usb_wwan 2>/dev/null
    modprobe qcserial 2>/dev/null
    
    # Network drivers
    modprobe qmi_wwan 2>/dev/null
    modprobe cdc_mbim 2>/dev/null
    modprobe cdc_ncm 2>/dev/null
    modprobe cdc_ether 2>/dev/null
    modprobe rndis_host 2>/dev/null
    
    # Add USB ID to drivers if not auto-detected
    echo "$MODEM_VENDOR_ID 0801" > /sys/bus/usb-serial/drivers/option1/new_id 2>/dev/null
    echo "$MODEM_VENDOR_ID 0801" > /sys/bus/usb/drivers/qmi_wwan/new_id 2>/dev/null
    
    log_msg "Modem drivers loaded"
}

# Configure modem for optimal mode
configure_modem_mode() {
    local device="$1"
    
    if [ -z "$device" ]; then
        log_msg "No control device specified"
        return 1
    fi
    
    log_msg "Configuring modem on $device"
    
    # Wait for device to be ready
    sleep 2
    
    # Check modem status
    echo "AT" > "$device"
    sleep 1
    
    # Get current USB configuration
    echo "AT+QCFG=\"usbnet\"" > "$device"
    sleep 1
    
    # For RM551E, QMI mode (0) is recommended for best performance
    # MBIM (1) and RNDIS (5) are alternatives
    echo "AT+QCFG=\"usbnet\",0" > "$device"
    sleep 1
    
    # Get modem info
    echo "ATI" > "$device"
    sleep 1
    
    # Check firmware version
    echo "AT+QGMR" > "$device"
    sleep 1
    
    log_msg "Modem configuration complete"
}

# Main initialization
log_msg "Starting RM551E initialization"

# Detect modem
if ! detect_rm551e; then
    log_msg "No RM551E modem detected"
    exit 0
fi

# Load drivers
load_modem_drivers

# Wait for device enumeration
sleep 3

# Find control interface
CONTROL_DEV=""
for dev in /dev/ttyUSB*; do
    if [ -c "$dev" ]; then
        # Try to communicate with the device
        if timeout 2 sh -c "echo AT > $dev && cat $dev 2>/dev/null" | grep -q "OK"; then
            CONTROL_DEV="$dev"
            log_msg "Found control interface: $CONTROL_DEV"
            break
        fi
    fi
done

# Configure modem if control interface found
if [ -n "$CONTROL_DEV" ]; then
    configure_modem_mode "$CONTROL_DEV"
else
    log_msg "Warning: Could not find modem control interface"
fi

# Trigger carrier aggregation optimization
if [ -x /usr/bin/modem-ca-optimize.sh ]; then
    log_msg "Running carrier aggregation optimization"
    /usr/bin/modem-ca-optimize.sh &
fi

log_msg "RM551E initialization complete"
exit 0
