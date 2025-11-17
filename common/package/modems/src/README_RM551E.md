# Quectel RM551E-GL 5G Modem Support

## Overview

This document describes the support for Quectel RM551E-GL 5G modems in OpenMPTCProuter Optimized.

## Supported Product IDs

The RM551E modem can enumerate with different USB Product IDs depending on its configuration mode:

- `2c7c:0800` - RM551E-GL (MBIM mode)
- `2c7c:0801` - RM551E-GL (QMI mode) - **Recommended**
- `2c7c:0900` - RM551E-GL (RNDIS mode)
- `2c7c:0901` - RM551E-GL (NCM mode)

All variants are now supported with optimized configurations.

## Features

### Carrier Aggregation Support

The RM551E supports advanced carrier aggregation features:

- **5G NR Carrier Aggregation**: Up to 4 bands simultaneously
- **EN-DC (E-UTRA-NR Dual Connectivity)**: LTE + 5G NR aggregation
- **LTE Carrier Aggregation**: Multiple LTE bands
- **Dynamic Bandwidth**: Automatic bandwidth selection

### Optimized Configuration

The modem is automatically configured with optimal settings:

1. **USB Mode**: QMI mode (recommended for best performance)
2. **Data Aggregation**: Enabled with 32KB URB size and 64 datagrams
3. **Flow Control**: Enabled for stability
4. **Network Mode**: Auto LTE/5G selection
5. **Low Latency**: NAT optimizations enabled

## Installation

The modem support is automatically included in OpenMPTCProuter builds for supported platforms.

### Required Kernel Modules

The following kernel modules are required (automatically loaded):

```
CONFIG_PACKAGE_kmod-usb-net=y
CONFIG_PACKAGE_kmod-usb-net-qmi-wwan=y
CONFIG_PACKAGE_kmod-usb-net-cdc-mbim=y
CONFIG_PACKAGE_kmod-usb-net-cdc-ncm=y
CONFIG_PACKAGE_kmod-usb-net-rndis=y
CONFIG_PACKAGE_kmod-usb-serial=y
CONFIG_PACKAGE_kmod-usb-serial-option=y
CONFIG_PACKAGE_kmod-usb-serial-wwan=y
CONFIG_PACKAGE_kmod-usb-serial-qualcomm=y
```

### Required Packages

```
CONFIG_PACKAGE_uqmi=y
CONFIG_PACKAGE_umbim=y
CONFIG_PACKAGE_comgt=y
CONFIG_PACKAGE_libqmi=y
CONFIG_PACKAGE_libmbim=y
```

## Automatic Detection and Configuration

When an RM551E modem is connected:

1. **Hotplug Event**: USB hotplug detects the modem
2. **Driver Loading**: Appropriate kernel modules are loaded
3. **Initialization**: `/usr/bin/rm551e-init.sh` runs automatically
4. **Optimization**: Carrier aggregation settings are applied
5. **Network Setup**: QMI/MBIM interface is configured

## Manual Configuration

### Check Modem Status

```bash
# List USB devices
lsusb | grep 2c7c

# Check if modem is detected
ls -l /dev/ttyUSB* /dev/cdc-wdm*

# Check current USB mode
echo "AT+QCFG=\"usbnet\"" > /dev/ttyUSB2
cat /dev/ttyUSB2
```

### Run Initialization Manually

```bash
/usr/bin/rm551e-init.sh
```

### Apply Carrier Aggregation Optimizations

```bash
/usr/bin/modem-ca-optimize.sh
```

### Check Network Interface

```bash
# For QMI mode
uqmi -d /dev/cdc-wdm0 --get-device-operating-mode

# For MBIM mode
umbim -d /dev/cdc-wdm0 caps
```

## Troubleshooting

### Modem Not Detected

1. Check USB connection:
   ```bash
   lsusb | grep 2c7c
   ```

2. Check kernel logs:
   ```bash
   dmesg | grep -i quectel
   logread | grep rm551e
   ```

3. Manually load drivers:
   ```bash
   modprobe qmi_wwan
   modprobe option
   ```

### Poor Performance

1. Verify carrier aggregation is enabled:
   ```bash
   /usr/bin/modem-ca-optimize.sh
   ```

2. Check signal quality:
   ```bash
   echo "AT+QRSRP" > /dev/ttyUSB2
   cat /dev/ttyUSB2
   ```

3. Verify 5G connection:
   ```bash
   echo "AT+QNWINFO" > /dev/ttyUSB2
   cat /dev/ttyUSB2
   ```

### Switching USB Modes

To change the modem's USB mode:

```bash
# QMI mode (recommended)
echo "AT+QCFG=\"usbnet\",0" > /dev/ttyUSB2

# MBIM mode
echo "AT+QCFG=\"usbnet\",1" > /dev/ttyUSB2

# RNDIS mode
echo "AT+QCFG=\"usbnet\",5" > /dev/ttyUSB2

# Reboot modem to apply
echo "AT+CFUN=1,1" > /dev/ttyUSB2
```

## Platform-Specific Notes

### Banana Pi R4

The BPI-R4 has excellent support for RM551E with:
- USB 3.0 for maximum throughput
- Hardware thermal management
- Optimized kernel configuration

Configuration is in `config-bpi-r4` and `config-bpi-r4-poe`.

### Raspberry Pi Models

RPi 4 and RPi 5 support RM551E via USB 3.0. Earlier models use USB 2.0 which may limit 5G throughput.

### x86/x64 Systems

Full support on all x86 platforms with standard USB 3.0 controllers.

## Performance Tuning

### Maximum Throughput Settings

For maximum throughput, ensure:

1. **USB 3.0 Connection**: Use USB 3.0 port
2. **QMI Mode**: Best performance
3. **Data Aggregation**: Enabled (automatic)
4. **Carrier Aggregation**: All bands enabled (automatic)

### Latency Optimization

For low latency applications:

1. NAT optimization is enabled by default
2. Flow control is configured
3. Use QoS settings in OpenMPTCProuter

## Firmware Updates

To update RM551E firmware:

1. Download firmware from Quectel
2. Use QFlash tool (Windows) or qfirehose (Linux)
3. Follow Quectel's update procedures

**Note**: Firmware updates should be done carefully and may require Windows tools.

## Additional Resources

- [Quectel RM551E Product Page](https://www.quectel.com/product/5g-rm551e-gl)
- [OpenMPTCProuter Documentation](https://www.openmptcprouter.com/)
- [OpenWrt Modem Support](https://openwrt.org/docs/guide-user/network/wan/wwan/start)

## Support

For issues specific to RM551E support in OpenMPTCProuter:

1. Check logs: `logread | grep -i modem`
2. Review this documentation
3. Open an issue on GitHub with logs and configuration details
