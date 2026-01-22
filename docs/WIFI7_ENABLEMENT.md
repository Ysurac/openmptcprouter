# Wi-Fi 7 (802.11be/EHT) Enablement Guide

This document describes how to enable and configure Wi-Fi 7 support in OpenMPTCProuter.

## Supported Chipsets

### MediaTek MT7996 Family

| Chipset | Interface | Bands | Max Speed | Status |
|---------|-----------|-------|-----------|--------|
| MT7996 | PCIe | 2.4/5/6 GHz | 4x4 BE7200 | Fully Supported |
| MT7996 (233MHz) | PCIe | 2.4/5/6 GHz | 4x4 BE7200 | Fully Supported |
| MT7992 | PCIe | 2.4/5/6 GHz | 2x2 BE5000 | Supported |

### Supported Hardware

| Device | SoC | Wi-Fi Chipset | Wi-Fi 7 Ready |
|--------|-----|---------------|---------------|
| Banana Pi BPi-R4 | MT7988 | MT7996 | Yes |
| Banana Pi BPi-R4 PoE | MT7988 | MT7996 (233MHz) | Yes |

## Required Packages

### Kernel Modules

```bash
# Core Wi-Fi stack
CONFIG_PACKAGE_kmod-mac80211=y
CONFIG_PACKAGE_kmod-cfg80211=y

# MediaTek MT7996 driver
CONFIG_PACKAGE_kmod-mt76=y
CONFIG_PACKAGE_kmod-mt7996e=y

# Firmware packages
CONFIG_PACKAGE_kmod-mt7996-firmware=y
# For 233MHz variant (BPi-R4 PoE):
CONFIG_PACKAGE_kmod-mt7996-233-firmware=y

# Wireless offload firmware (MT7988 SoC)
CONFIG_PACKAGE_mt7988-wo-firmware=y
```

### Userspace Packages

```bash
# Hostapd with 802.11be support (AP mode)
CONFIG_PACKAGE_hostapd-openssl=y
# OR wpad (combined AP+client)
CONFIG_PACKAGE_wpad-openssl=y

# WPA supplicant with 802.11be support (client mode)
CONFIG_PACKAGE_wpa-supplicant-openssl=y

# Wireless tools
CONFIG_PACKAGE_iw-full=y
CONFIG_PACKAGE_iwinfo=y
```

## Minimum Requirements

| Component | Minimum Version | Notes |
|-----------|-----------------|-------|
| Kernel | 6.6+ | Required for MT7996 driver |
| OpenWrt | 24.10 | Required for EHT/802.11be stack |
| hostapd | 2.11+ | Must be built with CONFIG_IEEE80211BE |
| wpa_supplicant | 2.11+ | Must be built with CONFIG_IEEE80211BE |
| Regulatory DB | 2024+ | For 6 GHz band rules |

## Configuration Examples

### Basic AP Configuration (UCI)

```bash
# /etc/config/wireless

config wifi-device 'radio0'
    option type 'mac80211'
    option path 'platform/soc/18000000.pcie/pci0000:00/0000:00:00.0/0000:01:00.0'
    option band '6g'
    option channel 'auto'
    option htmode 'EHT320'
    option cell_density '0'
    option country 'US'

config wifi-iface 'default_radio0'
    option device 'radio0'
    option network 'lan'
    option mode 'ap'
    option ssid 'OpenMPTCProuter-WiFi7'
    option encryption 'sae'
    option key 'your_secure_password'
    option ieee80211w '2'
```

### Multi-Band AP Configuration

```bash
# 2.4 GHz Band
config wifi-device 'radio0'
    option type 'mac80211'
    option band '2g'
    option channel 'auto'
    option htmode 'HE40'
    option country 'US'

config wifi-iface 'wlan_2g'
    option device 'radio0'
    option network 'lan'
    option mode 'ap'
    option ssid 'MyNetwork'
    option encryption 'sae-mixed'
    option key 'password123'

# 5 GHz Band
config wifi-device 'radio1'
    option type 'mac80211'
    option band '5g'
    option channel 'auto'
    option htmode 'EHT160'
    option country 'US'

config wifi-iface 'wlan_5g'
    option device 'radio1'
    option network 'lan'
    option mode 'ap'
    option ssid 'MyNetwork'
    option encryption 'sae-mixed'
    option key 'password123'

# 6 GHz Band (Wi-Fi 7)
config wifi-device 'radio2'
    option type 'mac80211'
    option band '6g'
    option channel 'auto'
    option htmode 'EHT320'
    option country 'US'

config wifi-iface 'wlan_6g'
    option device 'radio2'
    option network 'lan'
    option mode 'ap'
    option ssid 'MyNetwork-6G'
    option encryption 'sae'
    option key 'password123'
    option ieee80211w '2'
```

### hostapd Configuration (Direct)

```conf
# /etc/hostapd/hostapd.conf

interface=wlan0
driver=nl80211
ssid=WiFi7-Network

# 6 GHz Band
hw_mode=a
channel=37
op_class=134

# Wi-Fi 7 (802.11be) mode
ieee80211ax=1
ieee80211be=1

# EHT capabilities
eht_oper_chwidth=2
eht_oper_centr_freq_seg0_idx=47

# Security (WPA3 required for 6 GHz)
wpa=2
wpa_key_mgmt=SAE
wpa_passphrase=YourSecurePassword
ieee80211w=2
sae_require_mfp=1

# Country code (critical for 6 GHz)
country_code=US
ieee80211d=1
ieee80211h=1
```

## 6 GHz Regulatory Considerations

### NO-IR (No Initiating Radiation) Issues

The 6 GHz band has strict regulatory requirements. You may see `NO-IR` flags in `iw list` output, which means:

1. The device cannot initiate radiation until it detects another AP on the channel
2. Low Power Indoor (LPI) and Standard Power (SP) modes have different requirements
3. AFC (Automated Frequency Coordination) may be required for outdoor use

### Checking Regulatory Status

```bash
# View current regulatory domain
iw reg get

# Check available frequencies and flags
iw phy phy0 info | grep -A 100 "Frequencies"

# View 6 GHz capabilities
iw dev wlan0 info
```

### Setting Regulatory Domain

```bash
# Set country code
iw reg set US

# Verify in UCI
uci set wireless.radio0.country='US'
uci commit wireless
wifi reload
```

### Common 6 GHz Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| 6 GHz band not visible | Wrong regulatory domain | Set correct country code |
| NO-IR on all channels | Regulatory database outdated | Update wireless-regdb |
| Low TX power | LPI mode active | Check for SP mode support |
| Channel not available | DFS required | Wait for radar detection |

## Validation Checklist

### 1. Hardware Detection

```bash
# Check if driver loaded
lsmod | grep mt7996
# Expected: mt7996e, mt76_connac_lib, mt76, mac80211, cfg80211

# Check dmesg for firmware loading
dmesg | grep -i mt7996
# Should show firmware loaded successfully
```

### 2. Radio Interface Check

```bash
# List wireless interfaces
iw dev
# Should show wlan0 (or similar)

# Check PHY capabilities
iw phy phy0 info | grep -i "EHT\|320\|6 GHz"
# Should show EHT capabilities and 6 GHz band
```

### 3. AP Functionality

```bash
# Start wireless
wifi up

# Check AP is broadcasting
iw dev wlan0 info
# Should show "type AP" and ssid

# Check for connected clients
iw dev wlan0 station dump
```

### 4. Performance Validation

```bash
# Install iperf3
opkg update && opkg install iperf3

# Server side
iperf3 -s

# Client side (from Wi-Fi 7 device)
iperf3 -c <router-ip> -t 30 -P 4

# Expected: High throughput matching Wi-Fi 7 capabilities
# BE7200: Up to ~2.4 Gbps practical throughput
```

### 5. EHT Mode Verification

```bash
# Check if EHT mode is active
iw dev wlan0 link | grep -i eht

# Check channel width
iw dev wlan0 info | grep width
# Should show "320 MHz" for full EHT320
```

## Troubleshooting

### Radio Not Detected

1. Check PCI device visibility:
   ```bash
   lspci | grep -i mediatek
   ```

2. Verify kernel modules:
   ```bash
   lsmod | grep mt76
   ```

3. Check dmesg for errors:
   ```bash
   dmesg | grep -i "mt7996\|error\|firmware"
   ```

### Firmware Loading Failure

1. Verify firmware files exist:
   ```bash
   ls -la /lib/firmware/mediatek/mt7996/
   ```

2. Check firmware version matches driver:
   ```bash
   dmesg | grep -i "firmware version"
   ```

### 6 GHz Not Available

1. Check regulatory database:
   ```bash
   iw reg get
   ```

2. Verify country code supports 6 GHz:
   ```bash
   iw phy phy0 channels | grep "6 GHz"
   ```

3. Update regulatory database:
   ```bash
   opkg update && opkg install wireless-regdb
   ```

### Low Performance

1. Check channel width:
   ```bash
   iw dev wlan0 info | grep width
   ```

2. Verify no interference:
   ```bash
   iw dev wlan0 scan | grep -E "SSID|signal"
   ```

3. Check for throttling:
   ```bash
   cat /sys/class/thermal/thermal_zone*/temp
   ```

## Known Limitations

1. **Multi-Link Operation (MLO)**: Full MLO requires additional kernel and hostapd support
2. **320 MHz channels**: Only available in certain regions and frequency ranges
3. **AFC support**: Automated Frequency Coordination not yet fully implemented
4. **Mesh mode**: EHT mesh support is limited in current releases

## References

- [MediaTek MT7996 Driver](https://wireless.wiki.kernel.org/en/users/drivers/mt76)
- [OpenWrt Wi-Fi Configuration](https://openwrt.org/docs/guide-user/network/wifi/basic)
- [hostapd 802.11be Configuration](https://w1.fi/cgit/hostap/plain/hostapd/hostapd.conf)
- [Wi-Fi Alliance Wi-Fi 7](https://www.wi-fi.org/discover-wi-fi/wi-fi-certified-7)

---

*Last updated: 2026-01-22*
