#!/bin/sh
# Quectel RM551E-GL 5G Modem Stability Monitor
# Monitors modem health and automatically recovers from failures
# Copyright (C) 2025 OpenMPTCProuter Optimized

MODEM_VENDOR_ID="2c7c"
MODEM_PRODUCT_IDS="0800 0801 0900 0901"
LOG_TAG="rm551e_monitor"
CHECK_INTERVAL=30  # Check every 30 seconds
FAILURE_THRESHOLD=3  # Number of consecutive failures before action
MAX_RESET_ATTEMPTS=3  # Maximum modem reset attempts
MONITOR_PID_FILE="/var/run/rm551e-monitor.pid"

# Counters
failure_count=0
reset_count=0
last_signal_check=0

log_msg() {
    logger -t "$LOG_TAG" "$1"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Check if monitor is already running (with atomic lock)
check_running() {
    local lockdir="${MONITOR_PID_FILE}.lock"

    # Use atomic mkdir for lock to prevent race condition
    if ! mkdir "$lockdir" 2>/dev/null; then
        # Another instance is starting, wait and check
        sleep 1
    fi

    if [ -f "$MONITOR_PID_FILE" ]; then
        local old_pid
        old_pid=$(cat "$MONITOR_PID_FILE" 2>/dev/null)
        # Validate PID is a number to prevent command injection
        if echo "$old_pid" | grep -qE '^[0-9]+$'; then
            if kill -0 "$old_pid" 2>/dev/null; then
                # Verify it's actually our script
                local cmdline
                cmdline=$(cat "/proc/$old_pid/cmdline" 2>/dev/null | tr '\0' ' ')
                if echo "$cmdline" | grep -q "rm551e-monitor"; then
                    log_msg "Monitor already running with PID $old_pid"
                    rmdir "$lockdir" 2>/dev/null
                    exit 0
                else
                    log_msg "PID $old_pid exists but is not rm551e-monitor"
                    rm -f "$MONITOR_PID_FILE"
                fi
            else
                log_msg "Removing stale PID file (process $old_pid not running)"
                rm -f "$MONITOR_PID_FILE"
            fi
        else
            log_msg "Invalid PID in PID file, removing"
            rm -f "$MONITOR_PID_FILE"
        fi
    fi

    # Write PID with secure permissions
    (
        umask 077
        echo $$ > "$MONITOR_PID_FILE"
    )
    rmdir "$lockdir" 2>/dev/null
}

# Cleanup on exit
cleanup() {
    log_msg "Monitor stopping"
    rm -f "$MONITOR_PID_FILE"
    exit 0
}

trap cleanup INT TERM EXIT

# Detect if RM551E modem is present
detect_modem() {
    for pid in $MODEM_PRODUCT_IDS; do
        if lsusb | grep -qi "${MODEM_VENDOR_ID}:${pid}"; then
            return 0
        fi
    done
    return 1
}

# Find modem control device
find_control_device() {
    for dev in /dev/ttyUSB*; do
        if [ -c "$dev" ]; then
            if timeout 2 sh -c "echo -e 'AT\r' > $dev 2>/dev/null && cat $dev 2>/dev/null" | grep -q "OK"; then
                echo "$dev"
                return 0
            fi
        fi
    done
    return 1
}

# Check modem responsiveness
check_modem_responsive() {
    local device="$1"
    
    if [ -z "$device" ] || [ ! -c "$device" ]; then
        return 1
    fi
    
    # Try AT command
    if ! timeout 3 sh -c "echo -e 'AT\r' > $device 2>/dev/null && cat $device 2>/dev/null" | grep -q "OK"; then
        return 1
    fi
    
    return 0
}

# Check signal quality
check_signal_quality() {
    local device="$1"
    local current_time=$(date +%s)
    
    # Only check signal every 5 minutes to reduce load
    if [ $((current_time - last_signal_check)) -lt 300 ]; then
        return 0
    fi
    
    last_signal_check=$current_time
    
    if [ -z "$device" ] || [ ! -c "$device" ]; then
        return 1
    fi
    
    # Get signal quality
    local signal=$(timeout 3 sh -c "echo -e 'AT+CSQ\r' > $device 2>/dev/null && timeout 2 cat $device 2>/dev/null" | grep "+CSQ:" | cut -d: -f2 | cut -d, -f1 | tr -d ' ')

    # Validate signal is numeric before comparison
    if [ -n "$signal" ] && echo "$signal" | grep -qE '^[0-9]+$' && [ "$signal" -ge 0 ] && [ "$signal" -le 31 ]; then
        if [ "$signal" -lt 10 ]; then
            log_msg "Warning: Low signal quality: $signal/31"
        else
            log_msg "Signal quality: $signal/31"
        fi
        return 0
    fi
    
    return 1
}

# Check network registration
check_network_registration() {
    local device="$1"
    
    if [ -z "$device" ] || [ ! -c "$device" ]; then
        return 1
    fi
    
    # Check registration status
    local reg_status=$(timeout 3 sh -c "echo -e 'AT+CEREG?\r' > $device 2>/dev/null && cat $device 2>/dev/null" | grep "+CEREG:" | cut -d, -f2 | tr -d ' ')
    
    # Status 1 = registered home network, 5 = registered roaming
    if [ "$reg_status" = "1" ] || [ "$reg_status" = "5" ]; then
        return 0
    fi
    
    log_msg "Warning: Not registered on network (status: $reg_status)"
    return 1
}

# Check data connection
check_data_connection() {
    local qmi_device=$(ls /dev/cdc-wdm* 2>/dev/null | head -n1)
    
    if [ -z "$qmi_device" ]; then
        return 1
    fi
    
    # Check connection status via uqmi
    if which uqmi >/dev/null 2>&1; then
        local conn_status=$(uqmi -d "$qmi_device" --get-data-status 2>/dev/null)
        if [ "$conn_status" = "\"connected\"" ]; then
            return 0
        fi
    fi
    
    return 1
}

# Notify MPTCP path manager of modem event
notify_mptcp_path_manager() {
    local event="$1"
    local interface="$2"

    # FIX 2.3: Coordinate modem resets with MPTCP path manager
    if [ -d "/var/run/mptcp-paths" ]; then
        case "$event" in
            reset_start)
                # Mark path as failing with high failure count
                # This forces immediate exclusion rather than waiting for keepalive
                local now=$(date +%s)
                echo "${interface}|${now}|0|999" > "/var/run/mptcp-paths/${interface}" 2>/dev/null
                logger -t mptcp-path "Modem $interface resetting - path marked as down"
                ;;
            reset_complete)
                # Mark path as recovering (will need hysteresis period)
                logger -t mptcp-path "Modem $interface reset complete - entering recovery period"
                ;;
        esac
    fi
}

# Find wwan interface for this modem
find_wwan_interface() {
    # Look for wwan interfaces in network config
    for iface in wwan0 wwan1 wwan2; do
        if [ -d "/sys/class/net/$iface" ]; then
            # Check if this interface uses our modem
            local proto=$(uci get network.${iface}.proto 2>/dev/null)
            if [ "$proto" = "qmi" ] || [ "$proto" = "mbim" ]; then
                echo "$iface"
                return 0
            fi
        fi
    done
    return 1
}

# Soft reset modem (AT command)
soft_reset_modem() {
    local device="$1"

    log_msg "Attempting soft reset of modem"

    if [ -z "$device" ] || [ ! -c "$device" ]; then
        return 1
    fi

    # FIX 2.3: Notify MPTCP before reset
    local wwan_if=$(find_wwan_interface)
    if [ -n "$wwan_if" ]; then
        notify_mptcp_path_manager "reset_start" "$wwan_if"
    fi

    # Reset modem via AT command
    echo -e 'AT+CFUN=1,1\r' > "$device" 2>/dev/null

    sleep 10
    
    # Wait for modem to come back online
    local wait_count=0
    while [ $wait_count -lt 30 ]; do
        if detect_modem; then
            log_msg "Modem detected after soft reset"
            sleep 5

            # FIX 2.3: Notify MPTCP of successful recovery
            if [ -n "$wwan_if" ]; then
                notify_mptcp_path_manager "reset_complete" "$wwan_if"
            fi

            return 0
        fi
        sleep 2
        wait_count=$((wait_count + 1))
    done

    log_msg "Modem did not respond after soft reset"
    return 1
}

# Validate USB device name format to prevent sysfs injection
validate_usb_device_name() {
    local dev_name=$1
    # USB device names follow pattern: bus-port or bus-port.port
    if ! echo "$dev_name" | grep -qE '^[0-9]+-[0-9]+(\.[0-9]+)*$'; then
        log_msg "ERROR: Invalid USB device name format: $dev_name"
        return 1
    fi
    return 0
}

# Hard reset modem (USB reset)
hard_reset_modem() {
    log_msg "Attempting hard reset of modem (USB reset)"

    # FIX 2.3: Notify MPTCP before reset
    local wwan_if=$(find_wwan_interface)
    if [ -n "$wwan_if" ]; then
        notify_mptcp_path_manager "reset_start" "$wwan_if"
    fi

    # Find USB device
    for usb_dev in /sys/bus/usb/devices/*; do
        if [ -f "$usb_dev/idVendor" ]; then
            vendor=$(cat "$usb_dev/idVendor")
            if [ "$vendor" = "$MODEM_VENDOR_ID" ]; then
                log_msg "Found modem USB device: $usb_dev"

                # Unbind and rebind USB device with validation
                dev_name=$(basename "$usb_dev")
                if ! validate_usb_device_name "$dev_name"; then
                    log_msg "ERROR: Skipping unsafe device name"
                    continue
                fi
                echo "$dev_name" > /sys/bus/usb/drivers/usb/unbind 2>/dev/null
                sleep 2
                echo "$dev_name" > /sys/bus/usb/drivers/usb/bind 2>/dev/null

                sleep 10

                if detect_modem; then
                    log_msg "Modem detected after hard reset"

                    # FIX 2.3: Notify MPTCP of successful recovery
                    if [ -n "$wwan_if" ]; then
                        notify_mptcp_path_manager "reset_complete" "$wwan_if"
                    fi

                    # Reinitialize modem
                    if [ -x /usr/bin/rm551e-init.sh ]; then
                        /usr/bin/rm551e-init.sh &
                    fi
                    return 0
                fi
            fi
        fi
    done

    log_msg "Hard reset failed"
    return 1
}

# Recover modem
recover_modem() {
    local device="$1"
    
    reset_count=$((reset_count + 1))
    
    if [ $reset_count -gt $MAX_RESET_ATTEMPTS ]; then
        log_msg "ERROR: Maximum reset attempts reached. Manual intervention required."
        # Send alert notification if available
        if [ -x /usr/bin/send-notification.sh ]; then
            /usr/bin/send-notification.sh "Modem RM551E requires manual intervention"
        fi
        return 1
    fi
    
    log_msg "Attempting modem recovery (attempt $reset_count/$MAX_RESET_ATTEMPTS)"
    
    # Try soft reset first
    if soft_reset_modem "$device"; then
        failure_count=0
        log_msg "Modem recovered via soft reset"
        return 0
    fi
    
    # Try hard reset if soft reset failed
    sleep 5
    if hard_reset_modem; then
        failure_count=0
        log_msg "Modem recovered via hard reset"
        return 0
    fi
    
    return 1
}

# Main monitoring loop
main() {
    log_msg "Starting RM551E stability monitor (PID: $$)"
    check_running
    
    # Initial check
    if ! detect_modem; then
        log_msg "No RM551E modem detected. Exiting monitor."
        exit 0
    fi
    
    log_msg "RM551E modem detected. Starting monitoring..."
    
    while true; do
        sleep $CHECK_INTERVAL
        
        # Check if modem is still present
        if ! detect_modem; then
            log_msg "ERROR: Modem disappeared from USB bus"
            failure_count=$((failure_count + 1))
            # Prevent counter overflow
            [ "$failure_count" -gt 1000 ] && failure_count=1000

            if [ $failure_count -ge $FAILURE_THRESHOLD ]; then
                hard_reset_modem
                failure_count=0
            fi
            continue
        fi
        
        # Find control device
        control_dev=$(find_control_device)

        if [ -z "$control_dev" ]; then
            log_msg "WARNING: Cannot find modem control interface"
            failure_count=$((failure_count + 1))
            # Prevent counter overflow
            [ "$failure_count" -gt 1000 ] && failure_count=1000

            if [ $failure_count -ge $FAILURE_THRESHOLD ]; then
                recover_modem ""
            fi
            continue
        fi
        
        # Check modem responsiveness
        if ! check_modem_responsive "$control_dev"; then
            log_msg "WARNING: Modem not responding to AT commands"
            failure_count=$((failure_count + 1))
            # Prevent counter overflow
            [ "$failure_count" -gt 1000 ] && failure_count=1000

            if [ $failure_count -ge $FAILURE_THRESHOLD ]; then
                recover_modem "$control_dev"
            fi
            continue
        fi
        
        # Check network registration
        if ! check_network_registration "$control_dev"; then
            failure_count=$((failure_count + 1))
            # Prevent counter overflow
            [ "$failure_count" -gt 1000 ] && failure_count=1000

            if [ $failure_count -ge $FAILURE_THRESHOLD ]; then
                log_msg "Network registration failed multiple times"
                recover_modem "$control_dev"
            fi
            continue
        fi
        
        # Check data connection
        if ! check_data_connection; then
            log_msg "INFO: Data connection not active (may be normal if not configured)"
        fi
        
        # Check signal quality periodically
        check_signal_quality "$control_dev"
        
        # Reset failure count if all checks passed
        failure_count=0
        reset_count=0  # Reset the reset counter on successful checks
    done
}

# Start main monitoring loop
main
