# Quectel RM551E Quick Reference

## Quick Start

### Check if Modem is Detected

```bash
# Check USB detection
lsusb | grep 2c7c

# Expected output (one of):
# 2c7c:0800  Quectel RM551E-GL (MBIM)
# 2c7c:0801  Quectel RM551E-GL (QMI) ⭐ Recommended
# 2c7c:0900  Quectel RM551E-GL (RNDIS)
# 2c7c:0901  Quectel RM551E-GL (NCM)
```

### Check Modem Status

```bash
# Find control interface
ls -l /dev/ttyUSB*

# Check modem info (use first available ttyUSB)
echo "ATI" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Check network info
echo "AT+QNWINFO" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Check signal strength
echo "AT+QRSRP" > /dev/ttyUSB2 && cat /dev/ttyUSB2
```

### Run Optimization

```bash
# Manual initialization
/usr/bin/rm551e-init.sh

# Apply carrier aggregation
/usr/bin/modem-ca-optimize.sh

# Check logs
logread | grep -E "modem|rm551e"
```

## AT Commands Reference

### Basic Commands

```bash
# Modem info
echo "ATI" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Firmware version
echo "AT+QGMR" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# SIM status
echo "AT+CPIN?" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Network registration
echo "AT+CREG?" > /dev/ttyUSB2 && cat /dev/ttyUSB2
```

### Network Info

```bash
# Current network
echo "AT+QNWINFO" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Signal quality
echo "AT+CSQ" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Cell info
echo "AT+QENG=\"servingcell\"" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Neighbor cells
echo "AT+QENG=\"neighbourcell\"" > /dev/ttyUSB2 && cat /dev/ttyUSB2
```

### Carrier Aggregation

```bash
# Check CA status
echo "AT+QCAINFO" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Check EN-DC status
echo "AT+QNWCFG=\"endc\"" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Check 5G CA
echo "AT+QNWCFG=\"nr5g_carrier_aggregation\"" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Check LTE CA
echo "AT+QNWCFG=\"lte_ca\"" > /dev/ttyUSB2 && cat /dev/ttyUSB2
```

### USB Configuration

```bash
# Check current USB mode
echo "AT+QCFG=\"usbnet\"" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Set QMI mode (recommended)
echo "AT+QCFG=\"usbnet\",0" > /dev/ttyUSB2

# Set MBIM mode
echo "AT+QCFG=\"usbnet\",1" > /dev/ttyUSB2

# Set RNDIS mode
echo "AT+QCFG=\"usbnet\",5" > /dev/ttyUSB2

# Reboot to apply
echo "AT+CFUN=1,1" > /dev/ttyUSB2
```

## Performance Optimization

### Maximum Speed Settings

```bash
# Enable all CA features (done automatically by rm551e-init.sh)
echo "AT+QNWCFG=\"nr5g_carrier_aggregation\",1" > /dev/ttyUSB2
echo "AT+QNWCFG=\"endc\",1" > /dev/ttyUSB2
echo "AT+QNWCFG=\"lte_ca\",1" > /dev/ttyUSB2
echo "AT+QNWCFG=\"nr5g_bandwidth\",\"auto\"" > /dev/ttyUSB2

# Optimize data aggregation
echo "AT+QCFG=\"data_aggregation\",1,32768,64" > /dev/ttyUSB2

# Enable flow control
echo "AT+IFC=2,2" > /dev/ttyUSB2

# Low latency mode
echo "AT+QCFG=\"nat\",1" > /dev/ttyUSB2
```

### Network Preferences

```bash
# Auto mode (LTE + 5G)
echo "AT+QNWPREFCFG=\"mode_pref\",AUTO" > /dev/ttyUSB2

# 5G only
echo "AT+QNWPREFCFG=\"mode_pref\",NR5G" > /dev/ttyUSB2

# LTE only
echo "AT+QNWPREFCFG=\"mode_pref\",LTE" > /dev/ttyUSB2

# Check current preference
echo "AT+QNWPREFCFG=\"mode_pref\"" > /dev/ttyUSB2 && cat /dev/ttyUSB2
```

## Troubleshooting

### Modem Not Detected

```bash
# Check USB connection
lsusb | grep -i quectel

# Check kernel modules
lsmod | grep -E "option|qmi_wwan|cdc"

# Load modules manually
modprobe option
modprobe qmi_wwan
modprobe cdc_mbim

# Check kernel messages
dmesg | tail -50 | grep -i usb
```

### No Network Connection

```bash
# Check registration
echo "AT+CREG?" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Manual network search
echo "AT+COPS=?" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Force network
echo "AT+COPS=1,2,\"46000\"" > /dev/ttyUSB2  # Replace with your operator

# Restart network
echo "AT+CFUN=0" > /dev/ttyUSB2
sleep 2
echo "AT+CFUN=1" > /dev/ttyUSB2
```

### Poor Performance

```bash
# Check connection type
echo "AT+QNWINFO" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Check if CA is active
echo "AT+QCAINFO" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Check signal levels
echo "AT+QRSRP" > /dev/ttyUSB2 && cat /dev/ttyUSB2
echo "AT+QSINR" > /dev/ttyUSB2 && cat /dev/ttyUSB2

# Re-run optimization
/usr/bin/modem-ca-optimize.sh
```

### Factory Reset

```bash
# Reset to factory settings
echo "AT&F" > /dev/ttyUSB2

# Reboot modem
echo "AT+CFUN=1,1" > /dev/ttyUSB2

# Re-run initialization
/usr/bin/rm551e-init.sh
```

## Interface Management

### QMI Interface

```bash
# List QMI devices
ls -l /dev/cdc-wdm*

# Query device mode (requires uqmi)
uqmi -d /dev/cdc-wdm0 --get-device-operating-mode

# Get signal info
uqmi -d /dev/cdc-wdm0 --get-signal-info
```

### MBIM Interface

```bash
# Check capabilities (requires umbim)
umbim -d /dev/cdc-wdm0 caps

# Query subscriber info
umbim -d /dev/cdc-wdm0 subscriber

# Get signal state
umbim -d /dev/cdc-wdm0 radio
```

## Recommended Settings

### For Maximum Speed
- **USB Mode**: QMI (0)
- **CA**: All enabled
- **Bandwidth**: Auto
- **Data Aggregation**: 32KB / 64 datagrams

### For Stability
- **USB Mode**: MBIM (1)
- **Flow Control**: Enabled
- **Network Mode**: Auto

### For Low Latency
- **NAT**: Enabled
- **QMI Mode**: Preferred
- **5G SA**: If supported by carrier

## Platform-Specific Notes

### Banana Pi R4
- USB 3.0 port recommended
- Optimal performance with QMI mode
- Thermal management included

### Raspberry Pi 4/5
- Use blue USB 3.0 ports
- May need powered USB hub for stability

### x86/x64
- Check USB 3.0 controller compatibility
- Intel controllers work best

## Status Indicators

### Signal Quality (RSRP)

- **-80 dBm or better**: Excellent
- **-80 to -90 dBm**: Good
- **-90 to -100 dBm**: Fair
- **-100 dBm or worse**: Poor

### Registration Status

- `0,1` - Registered (home network)
- `0,5` - Registered (roaming)
- `0,2` - Searching
- `0,0` - Not registered

## Getting Help

1. Check logs: `logread | grep modem`
2. Read full documentation: [README_RM551E.md](common/package/modems/src/README_RM551E.md)
3. Check signal quality and network status
4. Verify USB mode matches your configuration
5. Open issue with logs if problems persist
