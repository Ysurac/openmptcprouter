# OpenMPTCProuter Universal Drivers Status

This document tracks the driver and firmware components in the OpenMPTCProuter repository for Wi-Fi 7 and 5G modem support.

## Repository Overview

- **Type**: OpenWrt overlay repository
- **Base**: Clones upstream OpenWrt and applies overlays from `common/` and kernel-version-specific directories
- **External Feeds**: openmptcprouter-feeds, OpenWrt packages/luci/routing

## Supported Kernel Versions

| Kernel | OpenWrt Base | Commit | Notes |
|--------|--------------|--------|-------|
| 5.4 | OpenWrt 21.02 | `170d9e447df0f52882a8b7a61bf940b062b2cacc` | Legacy support |
| 6.1 | OpenWrt 23.x | `b6d7048c8bcf23d9852cf17f1fa1d532d1552474` | Transitional |
| 6.6 | OpenWrt 24.10 | `8843108ff0fffb2abdc87465c323c97c7821df05` | Stable Wi-Fi 7 baseline |
| 6.10 | OpenWrt 24.10 | `8843108ff0fffb2abdc87465c323c97c7821df05` | Development |
| 6.12 | OpenWrt main | `46129bbbf5a423fd44a21b2f80f27a5a579fbb56` | Latest features |
| 6.18 | OpenWrt main | `46129bbbf5a423fd44a21b2f80f27a5a579fbb56` | Bleeding edge |

---

## BEFORE State (Current)

### Wi-Fi Drivers

| Component | Version/Commit | Kernel Support | Wi-Fi 7 Ready | Notes |
|-----------|----------------|----------------|---------------|-------|
| mac80211 | OpenWrt upstream | 6.6+ | Partial | EHT support via backports |
| mt76 | OpenWrt upstream | All | Yes (6.6+) | MT7996 support for Wi-Fi 7 |
| ath11k | OpenWrt upstream | 5.4 (ipq807x) | No | Wi-Fi 6E support only |
| ath12k | Not present | N/A | N/A | Qualcomm Wi-Fi 7 not in repo |
| mwlwifi | Custom patches | 6.6 | No | Marvell Wi-Fi support |
| hostapd/wpad | OpenWrt upstream | All | Yes (24.10+) | EHT/802.11be support |

### Wi-Fi Firmware

| Component | Version | Target Devices | Notes |
|-----------|---------|----------------|-------|
| kmod-mt7996-firmware | OpenWrt upstream | MT7996 (BPi-R4) | Wi-Fi 7 firmware |
| kmod-mt7996-233-firmware | OpenWrt upstream | MT7996 variant | 233MHz variant |
| mt7988-wo-firmware | OpenWrt upstream | MT7988 SoC | Wireless offload |
| mt7988-2p5g-phy-firmware | OpenWrt upstream | MT7988 SoC | 2.5G PHY firmware |

### Wi-Fi 7 Device Support

| Device | Chipset | Driver | Firmware | Status |
|--------|---------|--------|----------|--------|
| Banana Pi BPi-R4 | MT7988 + MT7996 | mt76 | mt7996-firmware | Supported |
| Banana Pi BPi-R4 PoE | MT7988 + MT7996 | mt76 | mt7996-233-firmware | Supported |

---

### 5G Modem Drivers

#### USB Path (QMI/MBIM)

| Component | Type | Kernel Support | Notes |
|-----------|------|----------------|-------|
| kmod-usb-net-qmi-wwan | Kernel module | All | QMI protocol support |
| kmod-usb-net-cdc-mbim | Kernel module | All | MBIM protocol support |
| kmod-usb-net-cdc-ncm | Kernel module | All | NCM protocol support |
| kmod-usb-net-cdc-ether | Kernel module | All | CDC Ethernet support |
| kmod-usb-net-rndis | Kernel module | All | RNDIS protocol support |
| kmod-usb-serial-option | Kernel module | All | USB serial option driver |
| kmod-usb-serial-qualcomm | Kernel module | All | Qualcomm USB serial |
| kmod-usb-serial-wwan | Kernel module | All | WWAN USB serial |
| kmod-usb-wdm | Kernel module | All | USB WDM support |
| kmod-usb-acm | Kernel module | All | USB ACM modem support |

#### PCIe Path (MHI/WWAN)

| Component | Type | Kernel Support | Notes |
|-----------|------|----------------|-------|
| kmod-mhi-bus | Kernel module | 6.6+ | MHI bus driver |
| kmod-mhi-pci-generic | Kernel module | 6.6+ | Generic MHI PCI controller |
| kmod-wwan | Kernel module | 6.6+ | WWAN driver core |
| kmod-mhi-wwan-ctrl | Kernel module | 6.18 | MHI WWAN control (AT/QMI/DIAG) |
| kmod-mhi-wwan-mbim | Kernel module | 6.18 | MHI MBIM data path |
| kmod-mhi-net | Kernel module | 6.18 | MHI networking |
| kmod-mtk-t7xx | Kernel module | 6.6+ | MediaTek T7xx 5G modem |

#### Userspace Tools

| Component | Type | Notes |
|-----------|------|-------|
| uqmi | Userspace | QMI control utility |
| umbim | Userspace | MBIM control utility |
| modemmanager | Userspace | Universal modem manager |
| wwan (scripts) | Userspace | WWAN protocol handlers |
| modems | Package | Modem identification database |

### Supported 5G Modems

| Vendor | Models | Interface | Protocol | Status |
|--------|--------|-----------|----------|--------|
| Quectel | RM500Q-GL | USB | QMI/MBIM | Supported |
| Quectel | RM520N-GL | USB | QMI/MBIM | Supported |
| Quectel | RM520N-GL | USB | QMI | Supported (2c7c:0900) |
| Quectel | RM520N-GL | USB | MBIM | Supported (2c7c:0901) |
| Quectel | EG12/EG18 | USB | QMI/MBIM | Supported |
| Quectel | EG06/EP06/EM06 | USB | QMI | Supported |
| MediaTek | T7xx series | PCIe | WWAN | Supported (mtk_t7xx) |
| Sierra Wireless | Various | USB | QMI/MBIM | Supported |
| Huawei | Various | USB | QMI/MBIM | Supported |

---

## Hardware Family to Driver Mapping

| Device Family | Kernel Driver | Firmware Package | Userspace Tools |
|---------------|---------------|------------------|-----------------|
| **Wi-Fi** | | | |
| MediaTek MT7996 (Wi-Fi 7) | kmod-mt7996e | kmod-mt7996-firmware | hostapd, wpa_supplicant |
| MediaTek MT7915/MT7916 | kmod-mt7915e | kmod-mt7915-firmware | hostapd, wpa_supplicant |
| Qualcomm IPQ807x (ath11k) | kmod-ath11k-pci | ath11k-firmware | hostapd, wpa_supplicant |
| **5G Modems (USB)** | | | |
| Quectel 5G (QMI mode) | kmod-usb-net-qmi-wwan | N/A | uqmi |
| Quectel 5G (MBIM mode) | kmod-usb-net-cdc-mbim | N/A | umbim, modemmanager |
| Generic USB modems | kmod-usb-serial-option | N/A | socat, microcom |
| **5G Modems (PCIe)** | | | |
| Qualcomm (MHI) | kmod-mhi-bus, kmod-mhi-pci-generic | Vendor-specific | modemmanager |
| MediaTek T7xx | kmod-mtk-t7xx | Included in driver | modemmanager |

---

## Minimum Requirements

### Wi-Fi 7 (802.11be/EHT)

| Requirement | Minimum Version |
|-------------|-----------------|
| Kernel | 6.6+ |
| OpenWrt Base | 24.10 |
| hostapd | 2.11+ (with CONFIG_IEEE80211BE) |
| wpa_supplicant | 2.11+ (with CONFIG_IEEE80211BE) |
| mac80211 | Backports with EHT support |
| Regulatory Database | 2024+ with 6 GHz rules |

### 5G Modem (PCIe/MHI)

| Requirement | Minimum Version |
|-------------|-----------------|
| Kernel | 6.6+ (MHI bus), 6.18+ (full WWAN stack) |
| MHI Bus | CONFIG_MHI_BUS enabled |
| WWAN Core | CONFIG_WWAN enabled |

---

## Known Limitations

1. **ath12k (Qualcomm Wi-Fi 7)**: Not currently supported in this repository
2. **6 GHz band**: Requires proper regulatory database and may show NO-IR on some regions
3. **PCIe 5G modems**: Full support requires kernel 6.18+ for mhi_wwan_ctrl/mhi_wwan_mbim
4. **Wi-Fi 7 MLO**: Multi-Link Operation requires recent hostapd/wpa_supplicant and kernel support

---

## AFTER State (Post-Update)

### Summary of Changes

| Component | Before | After | Change |
|-----------|--------|-------|--------|
| mt76 | Upstream | Upstream + per-radio patch | Added radio_idx support for Wi-Fi 7 |
| mac80211 | Upstream | Upstream + EHT patches | Added 802.11be EHT support |
| MHI/WWAN | 6.18 only | Documented for 6.6+ | Documented backport path |
| Modem database | 5G partial | 5G complete | Added RM530N, RG650V, EM9191 |
| Documentation | None | Complete | Added 3 enablement guides |
| Scripts | None | 2 scripts | Added build verification tools |
| Config fragments | None | config-universal-drivers | Added universal driver selection |

### New 5G Modem IDs Added

| USB ID | Modem | Protocol | Notes |
|--------|-------|----------|-------|
| 2c7c:0122 | Quectel RG650V | QMI | New 5G modem |
| 2c7c:0530 | Quectel RM530N-GL | QMI | New 5G modem |
| 2c7c:0531 | Quectel RM530N-GL | MBIM | New 5G modem |
| 1199:90d2 | Sierra EM9191 | MBIM | 5G modem |

### New Documentation

| Document | Purpose |
|----------|---------|
| docs/DRIVERS_STATUS.md | Driver/firmware status tracking |
| docs/WIFI7_ENABLEMENT.md | Wi-Fi 7 configuration guide |
| docs/5G_MODEM_ENABLEMENT.md | 5G modem (USB + PCIe) guide |
| docs/DIAGRAMS.md | Architecture diagrams |

### New Scripts

| Script | Purpose |
|--------|---------|
| scripts/driver_stack_report.sh | Generate driver version report |
| scripts/driver_build_matrix.sh | Build verification for drivers |

### New Config Fragments

| File | Purpose |
|------|---------|
| config-universal-drivers | Universal Wi-Fi 7 + 5G modem driver selection |

---

## Troubleshooting

### Wi-Fi 7 Issues

1. **Radio not detected**: Check `dmesg | grep mt7996` for firmware loading errors
2. **6 GHz not available**: Check regulatory domain with `iw reg get`
3. **EHT mode not working**: Verify hostapd is built with 802.11be support

### 5G Modem Issues

1. **USB modem not detected**: Check `dmesg | grep usb` and verify kmod-usb-serial-option loaded
2. **QMI interface missing**: Verify kmod-usb-net-qmi-wwan and check `/dev/cdc-wdm*`
3. **PCIe modem not working**: Check MHI bus status with `dmesg | grep mhi`

---

## Build Verification

To verify driver packages compile correctly:

```bash
# Generate driver stack report
./scripts/driver_stack_report.sh

# Build verification (dry run)
./scripts/driver_build_matrix.sh --dry-run

# Build verification (actual)
./scripts/driver_build_matrix.sh --kernel 6.6
```

---

*Last updated: 2026-01-22*
*Status: AFTER (updated with Wi-Fi 7 and 5G modem enhancements)*
