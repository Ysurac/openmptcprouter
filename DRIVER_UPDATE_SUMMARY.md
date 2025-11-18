# OpenMPTCProuter Driver Update Summary

## Overview
This document summarizes the comprehensive driver review and updates performed on the OpenMPTCProuter project. The goal was to update existing drivers, add missing modern drivers, and improve hardware support across the platform.

## Date
November 18, 2025

## Wireless Driver Updates

### 1. RTL8812AU Driver - UPDATED
**Location:** `6.1/package/kernel/rtl8812au-ct/`

**Previous Version:**
- Repository: greearb/rtl8812AU_8821AU_linux
- Date: 2021-11-07
- Commit: 39df55967b7de9f6c9600017b724303f95a8b9e2

**New Version:**
- Repository: aircrack-ng/rtl8812au (v5.6.4.2)
- Date: 2022-12-19
- Commit: c3fb89a2f7066f4bf4e4d9d85d84f9791f14c83e
- Module name changed from `rtl8812au.ko` to `88XXau.ko`

**Benefits:**
- More recent codebase with bug fixes
- Better kernel compatibility
- Improved monitor mode and frame injection support
- Supports RTL8812AU, RTL8821AU, and RTL8814AU chipsets

### 2. RTL8821CU Driver - NEW
**Location:** `6.1/package/kernel/rtl8821cu/`

**Details:**
- Repository: morrownr/8821cu-20210916
- Version: v5.12.0.4
- Date: 2024-12-16

**Supported Chipsets:**
- RTL8811CU
- RTL8821CU
- RTL8831CU

**Features:**
- Modern kernel support (5.4 through 6.12+)
- Monitor mode support
- Power saving features
- AC WiFi speeds

### 3. RTL88x2BU Driver - NEW
**Location:** `6.1/package/kernel/rtl88x2bu/`

**Details:**
- Repository: morrownr/88x2bu-20210702
- Version: v5.13.1
- Date: 2024-11-15

**Supported Chipsets:**
- RTL8812BU (AC1200)
- RTL8822BU (AC1200)

**Features:**
- Kernel 5.4 through 6.12+ support
- Monitor mode and frame injection
- Concurrent mode (AP + client simultaneously)
- Power saving features

### 4. RTL8814AU Driver - NEW
**Location:** `6.1/package/kernel/rtl8814au/`

**Details:**
- Repository: morrownr/8814au
- Date: 2024-10-25

**Supported Chipsets:**
- RTL8814AU (AC1900)

**Features:**
- Kernel 5.4 through 6.12+ support
- Monitor mode and frame injection
- Concurrent mode support
- AC1900 speeds (1300 Mbps @ 5 GHz + 600 Mbps @ 2.4 GHz)
- MU-MIMO support

### 5. RTL88x2CU Driver - NEW
**Location:** `6.1/package/kernel/rtl88x2cu/`

**Details:**
- Repository: morrownr/8821cu
- Date: 2024-11-23

**Supported Chipsets:**
- RTL8811CU
- RTL8812CU
- RTL8821CU
- RTL8822CU
- RTL8831CU

**Features:**
- Modern kernel support (5.4 through 6.12+)
- Monitor mode support
- Power saving features
- AC speeds

## Ethernet Driver Updates

### 6. Realtek r8125 Driver - NEW
**Location:** `6.1/package/kernel/r8125/`

**Details:**
- Version: 9.013.02
- Source: heri16/r8125 GitHub mirror

**Supported Devices:**
- RTL8125A (2.5GbE)
- RTL8125B (2.5GbE)
- RTL8125BG (2.5GbE)
- RTL8126A (5GbE)

**Features:**
- Better performance than in-kernel r8169 driver for RTL8125 devices
- RSS (Receive Side Scaling) support
- 2.5 Gigabit and 5 Gigabit Ethernet support

**Benefits:**
- Essential for modern motherboards with 2.5GbE ports
- Improved stability and throughput
- Better multi-path TCP performance

### 7. Realtek r8168 Driver - NEW
**Location:** `6.1/package/kernel/r8168/`

**Details:**
- Version: 8.053.00
- Source: mtorromeo/r8168 GitHub repository

**Supported Devices:**
- RTL8168B/8111B through RTL8168H/8111H
- RTL8168GU/8111GU
- RTL8411/8411B

**Features:**
- Improved performance over in-kernel r8169 driver
- Better stability and compatibility
- RSS support

**Benefits:**
- Better support for common Realtek Gigabit Ethernet controllers
- Resolves connectivity issues seen with r8169
- Improved performance for bonding and MPTCP scenarios

## Summary Statistics

### Total Drivers Added: 6
1. rtl8821cu (WiFi)
2. rtl88x2bu (WiFi)
3. rtl8814au (WiFi)
4. rtl88x2cu (WiFi)
5. r8125 (Ethernet)
6. r8168 (Ethernet)

### Total Drivers Updated: 1
1. rtl8812au-ct (WiFi)

### Chipsets Newly Supported
**Wireless:**
- RTL8811CU, RTL8821CU, RTL8831CU
- RTL8812BU, RTL8822BU
- RTL8814AU
- RTL8812CU, RTL8822CU

**Ethernet:**
- RTL8125A/B/BG (2.5GbE)
- RTL8126A (5GbE)
- RTL8168x/8111x series (improved support)
- RTL8411/8411B

## Installation and Usage

### Building Drivers
All new drivers are located in `6.1/package/kernel/` and will be built as kernel modules when selected in the OpenWrt build configuration.

### Selecting Drivers in Build
Use `make menuconfig` and navigate to:
- **Kernel modules → Wireless Drivers** (for WiFi drivers)
- **Kernel modules → Network Devices** (for Ethernet drivers)

### Module Conflicts
- The r8125 and r8168 drivers conflict with the in-kernel r8169 driver
- They are marked with `CONFLICTS:=kmod-r8169` in their Makefiles
- Only one driver should be selected for Realtek Ethernet devices

## Compatibility

### Kernel Versions Supported
All new drivers have been designed to support:
- Kernel 5.4
- Kernel 6.1
- Kernel 6.6
- Kernel 6.10
- Kernel 6.12+

### Hardware Platforms
These drivers support all OpenMPTCProuter platforms:
- x86/x86_64
- Rockchip
- MediaTek
- Qualcomm (IPQ)
- Raspberry Pi (bcm27xx)

## Testing Recommendations

### For Wireless Drivers
1. Test basic connectivity and throughput
2. Verify monitor mode functionality (if needed)
3. Test power saving features
4. Verify concurrent mode operations (AP + client)

### For Ethernet Drivers
1. Test link negotiation at various speeds
2. Verify bonding functionality
3. Test MPTCP aggregation
4. Monitor for packet loss and errors
5. Test RSS functionality under high load

## Future Considerations

### Potential Additional Drivers
1. **RTW88/RTW89** - In-kernel mac80211-based drivers for newer Realtek chipsets (WiFi 6)
2. **MT7921/MT7922** - MediaTek WiFi 6/6E drivers
3. **ATH11k/ATH12k** - Qualcomm WiFi 6/6E/7 drivers
4. **Intel i40e** - Intel 10/25/40GbE drivers
5. **Aquantia AQtion** - 2.5/5/10GbE drivers (already partially supported)

### Maintenance Notes
- Monitor upstream repositories for updates
- Update PKG_SOURCE_VERSION when new stable commits are available
- Test with each new kernel version
- Watch for in-kernel driver improvements that may obsolete out-of-tree drivers

## Known Issues

### RTL8812AU Driver
- The aircrack-ng project notes this driver is "deprecated" and recommends the in-kernel rtw88 driver for kernel 6.12+
- For older kernels or specific use cases (monitor mode), this out-of-tree driver remains useful

### Build Dependencies
- All wireless drivers require mac80211-backport support
- Ethernet drivers require libphy support
- Some drivers may need kernel configuration adjustments

## References

### Driver Sources
- [aircrack-ng/rtl8812au](https://github.com/aircrack-ng/rtl8812au) - RTL8812AU driver
- [morrownr/8821cu-20210916](https://github.com/morrownr/8821cu-20210916) - RTL8821CU driver
- [morrownr/88x2bu-20210702](https://github.com/morrownr/88x2bu-20210702) - RTL88x2BU driver
- [morrownr/8814au](https://github.com/morrownr/8814au) - RTL8814AU driver
- [morrownr/8821cu](https://github.com/morrownr/8821cu) - RTL88x2CU driver
- [heri16/r8125](https://github.com/heri16/r8125) - Realtek r8125 driver
- [mtorromeo/r8168](https://github.com/mtorromeo/r8168) - Realtek r8168 driver

### Documentation
- [OpenWrt Package Development](https://openwrt.org/docs/guide-developer/packages)
- [OpenWrt Kernel Module Creation](https://openwrt.org/docs/guide-developer/kernel-module-creation)

## Conclusion

This comprehensive driver update significantly expands hardware support for OpenMPTCProuter, particularly for:
1. Modern USB WiFi adapters (WiFi 5/AC)
2. 2.5 Gigabit and 5 Gigabit Ethernet NICs
3. Improved support for common Realtek Gigabit Ethernet controllers

The updates focus on reliability, performance, and modern kernel compatibility while maintaining support for the wide range of kernel versions used across different OpenMPTCProuter platforms.
