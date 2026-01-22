# 5G Modem Enablement Guide

This document describes how to enable and configure 5G modem support in OpenMPTCProuter, covering both USB (QMI/MBIM) and PCIe (MHI/WWAN) modem paths.

## Overview

5G modems connect to OpenWrt/OpenMPTCProuter systems via two primary interfaces:

1. **USB Path (QMI/MBIM)**: Traditional USB-based modems using QMI or MBIM protocols
2. **PCIe Path (MHI/WWAN)**: Modern PCIe-based modems using the MHI bus

## USB Path: QMI/MBIM Modems

### Supported Modems (USB)

| Vendor | Model | USB ID | Protocol | 5G Bands | Status |
|--------|-------|--------|----------|----------|--------|
| Quectel | RM500Q-GL | 2c7c:0800 | QMI/MBIM | Sub-6 + mmWave | Supported |
| Quectel | RM520N-GL | 2c7c:0801 | QMI/MBIM | Sub-6 | Supported |
| Quectel | RM520N-GL (QMI) | 2c7c:0900 | QMI | Sub-6 | Supported |
| Quectel | RM520N-GL (MBIM) | 2c7c:0901 | MBIM | Sub-6 | Supported |
| Quectel | EG12/EG18 | 2c7c:0512 | QMI/MBIM | LTE | Supported |
| Quectel | EG06/EP06/EM06 | 2c7c:0306 | QMI | LTE | Supported |
| Quectel | EC25 | 2c7c:0125 | QMI | LTE | Supported |
| Sierra Wireless | EM9191 | 1199:90d2 | QMI/MBIM | Sub-6 | Supported |
| Telit | FN980m | 1bc7:1050 | MBIM | Sub-6 | Supported |

### Required Packages (USB)

#### Kernel Modules

```bash
# USB Core
CONFIG_PACKAGE_kmod-usb-core=y
CONFIG_PACKAGE_kmod-usb2=y
CONFIG_PACKAGE_kmod-usb3=y

# USB Networking
CONFIG_PACKAGE_kmod-usb-net=y
CONFIG_PACKAGE_kmod-usb-wdm=y

# QMI Support
CONFIG_PACKAGE_kmod-usb-net-qmi-wwan=y

# MBIM Support
CONFIG_PACKAGE_kmod-usb-net-cdc-mbim=y
CONFIG_PACKAGE_kmod-usb-net-cdc-ncm=y
CONFIG_PACKAGE_kmod-usb-net-cdc-ether=y

# USB Serial (AT commands, diagnostics)
CONFIG_PACKAGE_kmod-usb-serial=y
CONFIG_PACKAGE_kmod-usb-serial-option=y
CONFIG_PACKAGE_kmod-usb-serial-qualcomm=y
CONFIG_PACKAGE_kmod-usb-serial-wwan=y

# Additional protocols
CONFIG_PACKAGE_kmod-usb-net-rndis=y
CONFIG_PACKAGE_kmod-usb-acm=y
```

#### Userspace Tools

```bash
# QMI control utility
CONFIG_PACKAGE_uqmi=y

# MBIM control utility
CONFIG_PACKAGE_umbim=y

# Universal modem manager
CONFIG_PACKAGE_modemmanager=y

# WWAN scripts (netifd protocol handlers)
CONFIG_PACKAGE_wwan=y

# LuCI interface (optional)
CONFIG_PACKAGE_luci-proto-qmi=y
CONFIG_PACKAGE_luci-proto-mbim=y
CONFIG_PACKAGE_luci-proto-modemmanager=y
```

### USB QMI Configuration

#### Basic QMI Setup (UCI)

```bash
# /etc/config/network

config interface 'wwan'
    option proto 'qmi'
    option device '/dev/cdc-wdm0'
    option apn 'internet'
    option auth 'none'
    option pdptype 'ipv4v6'
    option delay '10'

# Optional: Set specific APN credentials
config interface 'wwan'
    option proto 'qmi'
    option device '/dev/cdc-wdm0'
    option apn 'fast.t-mobile.com'
    option auth 'none'
    option pdptype 'ipv4v6'
```

#### QMI Command-Line Usage

```bash
# Check SIM status
uqmi -d /dev/cdc-wdm0 --get-pin-status

# Unlock SIM (if PIN protected)
uqmi -d /dev/cdc-wdm0 --verify-pin1 1234

# Get signal strength
uqmi -d /dev/cdc-wdm0 --get-signal-info

# Get serving system
uqmi -d /dev/cdc-wdm0 --get-serving-system

# Start data connection
uqmi -d /dev/cdc-wdm0 --start-network --apn internet --autoconnect

# Check data connection status
uqmi -d /dev/cdc-wdm0 --get-data-status
```

### USB MBIM Configuration

#### Basic MBIM Setup (UCI)

```bash
# /etc/config/network

config interface 'wwan'
    option proto 'mbim'
    option device '/dev/cdc-wdm0'
    option apn 'internet'
    option auth 'none'
    option pdptype 'ipv4v6'
```

#### MBIM Command-Line Usage

```bash
# Query device capabilities
umbim -d /dev/cdc-wdm0 caps

# Query subscriber ready state
umbim -d /dev/cdc-wdm0 subscriber

# Query registration state
umbim -d /dev/cdc-wdm0 registration

# Connect to network
umbim -d /dev/cdc-wdm0 connect apn=internet

# Query IP configuration
umbim -d /dev/cdc-wdm0 config
```

### Expected Device Nodes (USB)

When a USB modem is connected, the following device nodes should appear:

| Device Node | Purpose | Used By |
|-------------|---------|---------|
| `/dev/cdc-wdm0` | QMI/MBIM control channel | uqmi, umbim |
| `/dev/ttyUSB0` | AT command port | socat, minicom |
| `/dev/ttyUSB1` | NMEA GPS (if available) | gpsd |
| `/dev/ttyUSB2` | Diagnostics/Debug | vendor tools |
| `wwan0` | Data interface | netifd |

---

## PCIe Path: MHI/WWAN Modems

### Why MHI Matters for PCIe 5G

Modern PCIe-based 5G modems use the **MHI (Modem Host Interface)** bus, which provides:

1. **Higher throughput**: PCIe offers multi-Gbps bandwidth vs USB 3.0's ~5 Gbps shared
2. **Lower latency**: Direct memory access reduces processing overhead
3. **Better power management**: Native PCIe power states
4. **Multiple data channels**: Simultaneous QMI control + multiple data paths

PCIe 5G modems expose **mhi_hwip* style netdevs** (e.g., `mhi_hwip0`, `mhi_hwip1`) on some systems, which are high-performance data interfaces managed by the WWAN subsystem.

### Supported Modems (PCIe)

| Vendor | Model | PCI ID | Protocol | 5G Bands | Status |
|--------|-------|--------|----------|----------|--------|
| Qualcomm | SDX55/X55 | 17cb:0306 | MHI/MBIM | Sub-6 | Supported |
| Qualcomm | SDX65/X65 | 17cb:0308 | MHI/MBIM | Sub-6 + mmWave | Supported |
| MediaTek | T700 (T7xx) | 14c3:4d75 | WWAN | Sub-6 | Supported |
| Telit | FN980m | 1bc7:1070 | MHI/MBIM | Sub-6 | Supported |
| Sierra Wireless | EM9191 (PCIe) | 1199:c081 | MHI | Sub-6 | Supported |

### Required Packages (PCIe)

#### Kernel Modules

```bash
# MHI Bus (required)
CONFIG_PACKAGE_kmod-mhi-bus=y
CONFIG_PACKAGE_kmod-mhi-pci-generic=y

# WWAN Core (required)
CONFIG_PACKAGE_kmod-wwan=y

# MHI WWAN Control (AT/QMI/DIAG ports)
CONFIG_PACKAGE_kmod-mhi-wwan-ctrl=y

# MHI MBIM (data path)
CONFIG_PACKAGE_kmod-mhi-wwan-mbim=y

# MHI Networking
CONFIG_PACKAGE_kmod-mhi-net=y

# MediaTek T7xx specific driver
CONFIG_PACKAGE_kmod-mtk-t7xx=y
```

#### Userspace Tools

```bash
# ModemManager (recommended for PCIe modems)
CONFIG_PACKAGE_modemmanager=y

# LuCI integration
CONFIG_PACKAGE_luci-proto-modemmanager=y
```

### Minimum Kernel Requirements

| Feature | Minimum Kernel | Notes |
|---------|----------------|-------|
| MHI Bus | 5.12+ | Basic MHI support |
| MHI WWAN | 5.13+ | WWAN subsystem integration |
| mhi_wwan_ctrl | 5.14+ | Control ports (AT/QMI/DIAG) |
| mhi_wwan_mbim | 5.18+ | MBIM over MHI |
| Full WWAN Stack | 6.6+ | Recommended for 5G |
| mtk_t7xx | 6.4+ | MediaTek T7xx driver |

### Expected Device Nodes (PCIe)

When a PCIe modem is connected, the following device nodes should appear:

| Device Node | Purpose | Used By |
|-------------|---------|---------|
| `/dev/wwan0at0` | AT command channel | socat, minicom |
| `/dev/wwan0qmi0` | QMI control channel | uqmi, ModemManager |
| `/dev/wwan0mbim0` | MBIM control channel | umbim, ModemManager |
| `mhi_hwip0` | High-performance data interface | netifd |
| `wwan0` | WWAN data interface | netifd |

### PCIe Modem Configuration

#### ModemManager Setup (Recommended)

```bash
# /etc/config/network

config interface 'wwan'
    option proto 'modemmanager'
    option device '/sys/devices/pci0000:00/0000:00:00.0/0000:01:00.0'
    option apn 'internet'
    option auth 'none'
    option iptype 'ipv4v6'
    option signalrate '30'
```

#### ModemManager Command-Line Usage

```bash
# List detected modems
mmcli -L

# Show modem details
mmcli -m 0

# Show SIM information
mmcli -m 0 --sim=0

# Enable modem
mmcli -m 0 -e

# Create bearer/connection
mmcli -m 0 --simple-connect="apn=internet"

# Check connection status
mmcli -m 0 --bearer=0
```

### Verification Checklist (PCIe)

#### 1. Hardware Detection

```bash
# Check PCI device
lspci | grep -i "modem\|qualcomm\|mediatek\|5g"
# Expected: Shows 5G modem PCI device

# Check kernel modules loaded
lsmod | grep -E "mhi|wwan|t7xx"
# Expected: mhi, mhi_pci_generic, wwan, (mtk_t7xx for MediaTek)
```

#### 2. MHI Bus Status

```bash
# Check MHI device state
cat /sys/bus/mhi/devices/*/mhi_state
# Expected: M0 (normal operation)

# Check for MHI errors
dmesg | grep -i mhi
```

#### 3. WWAN Device Nodes

```bash
# List WWAN control devices
ls -la /dev/wwan*
# Expected: wwan0at0, wwan0qmi0, wwan0mbim0

# List network interfaces
ip link show | grep -E "wwan|mhi"
# Expected: wwan0 or mhi_hwip0
```

#### 4. ModemManager Detection

```bash
# Check if ModemManager detects modem
mmcli -L
# Expected: Shows modem with path

# Get detailed modem info
mmcli -m 0 --verbose
```

---

## Common Configuration for Both Paths

### Firewall Configuration

```bash
# /etc/config/firewall

config zone
    option name 'wwan'
    list network 'wwan'
    option input 'REJECT'
    option output 'ACCEPT'
    option forward 'REJECT'
    option masq '1'
    option mtu_fix '1'

config forwarding
    option src 'lan'
    option dest 'wwan'
```

### MWAN3 Multi-WAN Configuration

For multipath aggregation with OpenMPTCProuter:

```bash
# /etc/config/mwan3

config interface 'wwan'
    option enabled '1'
    list track_ip '8.8.8.8'
    list track_ip '1.1.1.1'
    option reliability '2'
    option count '1'
    option timeout '2'
    option interval '5'
    option down '3'
    option up '3'
```

### DNS Configuration

```bash
# Ensure DNS servers are configured
uci set network.wwan.peerdns='0'
uci add_list network.wwan.dns='8.8.8.8'
uci add_list network.wwan.dns='8.8.4.4'
uci commit network
```

---

## Troubleshooting

### USB Modem Not Detected

1. Check USB device enumeration:
   ```bash
   lsusb
   # Should show modem vendor/product ID
   ```

2. Check kernel messages:
   ```bash
   dmesg | tail -50
   # Look for USB device attachment messages
   ```

3. Verify kernel modules:
   ```bash
   lsmod | grep -E "qmi|mbim|option|usb"
   ```

4. Check for missing USB ID:
   ```bash
   cat /sys/bus/usb/devices/*/idVendor
   cat /sys/bus/usb/devices/*/idProduct
   ```

### PCIe Modem Not Detected

1. Check PCI enumeration:
   ```bash
   lspci -nn | grep -i "modem\|network\|5g"
   ```

2. Check MHI bus status:
   ```bash
   ls /sys/bus/mhi/devices/
   ```

3. Check for firmware loading issues:
   ```bash
   dmesg | grep -i "firmware\|mhi"
   ```

### No Network Connection

1. Verify SIM is inserted and detected:
   ```bash
   # USB QMI
   uqmi -d /dev/cdc-wdm0 --get-pin-status

   # ModemManager
   mmcli -m 0 --sim=0
   ```

2. Check registration status:
   ```bash
   # USB QMI
   uqmi -d /dev/cdc-wdm0 --get-serving-system

   # ModemManager
   mmcli -m 0 | grep "state"
   ```

3. Verify APN is correct:
   - Check with your carrier for the correct APN
   - Some carriers require specific auth settings

### Poor Performance

1. Check signal strength:
   ```bash
   # USB QMI
   uqmi -d /dev/cdc-wdm0 --get-signal-info

   # ModemManager
   mmcli -m 0 | grep "signal"
   ```

2. Check band selection:
   ```bash
   mmcli -m 0 | grep "bands"
   ```

3. Monitor throughput:
   ```bash
   iftop -i wwan0
   ```

---

## Adding New Modem Support

If your modem is not detected:

### 1. Add USB ID (USB Modems)

For USB modems, add the vendor:product ID to `/lib/network/wwan/`:

```bash
# Create file with format: vendor_id-product_id
# Example: /lib/network/wwan/2c7c-0905
{
    "desc": "Quectel RM530N-GL",
    "control": "ttyUSB2",
    "data": "qmi_wwan0",
    "qmi": 0,
    "mbim": 0
}
```

### 2. Add USB Serial Option ID

For AT command support, the ID may need to be added to kmod-usb-serial-option.

### 3. PCIe Modems

PCIe modems typically require upstream kernel support. Check if your modem's PCI ID is in `mhi_pci_generic` or has a dedicated driver.

---

## References

- [QMI Protocol Documentation](https://osmocom.org/projects/quectel-modems/wiki/QMI)
- [MBIM Specification](https://docs.microsoft.com/en-us/windows-hardware/drivers/network/mb-interface-model)
- [Linux MHI Bus Documentation](https://www.kernel.org/doc/html/latest/bus/mhi.html)
- [ModemManager Documentation](https://modemmanager.org/)
- [OpenWrt WWAN Documentation](https://openwrt.org/docs/guide-user/network/wan/wwan/start)

---

*Last updated: 2026-01-22*
