#!/bin/sh
# Carrier Aggregation Optimization Script for 5G Modems
# Optimizes Quectel RM551E-GL and other 5G modems for maximum CA performance

set_modem_ca_optimization() {
    local device="$1"
    
    # Enable 5G NR carrier aggregation
    echo "AT+QNWCFG=\"nr5g_carrier_aggregation\",1" > "$device"
    sleep 1
    
    # Enable EN-DC (E-UTRA-NR Dual Connectivity)
    echo "AT+QNWCFG=\"endc\",1" > "$device"
    sleep 1
    
    # Set maximum bandwidth aggregation
    echo "AT+QNWCFG=\"nr5g_bandwidth\",\"auto\"" > "$device"
    sleep 1
    
    # Enable all carrier aggregation combinations
    echo "AT+QNWCFG=\"lte_ca\",1" > "$device"
    sleep 1
    
    # Optimize data throughput settings
    echo "AT+QMAP=\"MPENABLE\",1" > "$device"
    sleep 1
    
    # Enable data aggregation
    echo "AT+QCFG=\"data_aggregation\",1" > "$device"
    sleep 1
    
    # Set URB size for optimal throughput
    echo "AT+QCFG=\"usbnet\",3" > "$device"  # MBIM mode with data aggregation
    sleep 1
    
    # Enable flow control for better stability
    echo "AT+IFC=2,2" > "$device"
    sleep 1
    
    logger -t modem_ca "Carrier aggregation optimizations applied to $device"
}

# Detect Quectel modems
for device in /dev/ttyUSB*; do
    if [ -e "$device" ]; then
        # Check if it's a Quectel modem
        response=$(echo "ATI" > "$device" 2>&1)
        if echo "$response" | grep -qi "Quectel"; then
            set_modem_ca_optimization "$device"
        fi
    fi
done

exit 0
