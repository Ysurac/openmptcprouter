#!/bin/bash
#
# driver_stack_report.sh - Generate a report of driver/firmware versions
#
# This script reports versions/commits for key wireless and modem driver components
# in the OpenMPTCProuter build system.
#
# Usage: ./scripts/driver_stack_report.sh [kernel_version] [target_dir]
#
# Copyright (C) 2024-2026 OpenMPTCProuter Project
# Licensed under GPL v3

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"

# Default values
KERNEL_VERSION="${1:-6.6}"
TARGET_DIR="${2:-}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "=========================================="
echo "OpenMPTCProuter Driver Stack Report"
echo "=========================================="
echo "Date: $(date)"
echo "Kernel Version: $KERNEL_VERSION"
echo "Repository: $REPO_ROOT"
echo ""

# Function to get package version from Makefile
get_pkg_version() {
    local makefile="$1"
    if [ -f "$makefile" ]; then
        local version=$(grep -E "^PKG_VERSION:=" "$makefile" 2>/dev/null | cut -d'=' -f2 | head -1)
        local release=$(grep -E "^PKG_RELEASE:=" "$makefile" 2>/dev/null | cut -d'=' -f2 | head -1)
        local source_date=$(grep -E "^PKG_SOURCE_DATE:=" "$makefile" 2>/dev/null | cut -d'=' -f2 | head -1)
        local source_version=$(grep -E "^PKG_SOURCE_VERSION:=" "$makefile" 2>/dev/null | cut -d'=' -f2 | head -1)

        if [ -n "$version" ]; then
            echo "$version${release:+-$release}${source_date:+ ($source_date)}${source_version:+ [${source_version:0:8}]}"
        elif [ -n "$source_date" ]; then
            echo "$source_date${source_version:+ [${source_version:0:8}]}"
        else
            echo "Unknown"
        fi
    else
        echo "Not found"
    fi
}

# Function to check if package exists in overlay
check_overlay() {
    local pkg_path="$1"
    local kernel="$2"

    if [ -d "$REPO_ROOT/$kernel/package/$pkg_path" ]; then
        echo -e "${GREEN}[Overlay: $kernel]${NC}"
    elif [ -d "$REPO_ROOT/common/package/$pkg_path" ]; then
        echo -e "${YELLOW}[Overlay: common]${NC}"
    else
        echo -e "${BLUE}[Upstream]${NC}"
    fi
}

# Function to count patches
count_patches() {
    local patch_dir="$1"
    if [ -d "$patch_dir" ]; then
        local count=$(find "$patch_dir" -name "*.patch" 2>/dev/null | wc -l)
        echo "$count patch(es)"
    else
        echo "No patches"
    fi
}

echo "=========================================="
echo "Wi-Fi Driver Stack"
echo "=========================================="
echo ""

# mac80211
echo -e "${GREEN}mac80211 (Wireless Subsystem):${NC}"
MAC80211_PATH="$REPO_ROOT/$KERNEL_VERSION/package/kernel/mac80211"
if [ -d "$MAC80211_PATH" ]; then
    echo "  Status: $(check_overlay kernel/mac80211 $KERNEL_VERSION)"
    echo "  Patches: $(count_patches "$MAC80211_PATH/patches")"
else
    echo "  Status: Upstream (no local overlay)"
fi
echo ""

# mt76
echo -e "${GREEN}mt76 (MediaTek Wi-Fi Driver):${NC}"
MT76_PATH="$REPO_ROOT/$KERNEL_VERSION/package/kernel/mt76"
if [ -d "$MT76_PATH" ]; then
    echo "  Status: $(check_overlay kernel/mt76 $KERNEL_VERSION)"
    echo "  Patches: $(count_patches "$MT76_PATH/patches")"
else
    echo "  Status: Upstream (no local overlay)"
fi
echo "  Supports: MT7996 (Wi-Fi 7), MT7915/MT7916 (Wi-Fi 6)"
echo ""

# ath11k/ath12k
echo -e "${GREEN}ath11k (Qualcomm Wi-Fi 6/6E):${NC}"
if [ -d "$REPO_ROOT/5.4/target/linux/ipq807x" ]; then
    echo "  Status: Supported (ipq807x target, kernel 5.4)"
else
    echo "  Status: Check upstream"
fi
echo ""

echo -e "${GREEN}ath12k (Qualcomm Wi-Fi 7):${NC}"
echo "  Status: Not in overlay (check upstream OpenWrt)"
echo ""

# mwlwifi
echo -e "${GREEN}mwlwifi (Marvell Wi-Fi):${NC}"
MWLWIFI_PATH="$REPO_ROOT/$KERNEL_VERSION/package/kernel/mwlwifi"
if [ -d "$MWLWIFI_PATH" ]; then
    echo "  Status: $(check_overlay kernel/mwlwifi $KERNEL_VERSION)"
    echo "  Patches: $(count_patches "$MWLWIFI_PATH/patches")"
else
    echo "  Status: Not present in $KERNEL_VERSION"
fi
echo ""

# hostapd/wpad
echo -e "${GREEN}hostapd/wpad (Userspace Wi-Fi):${NC}"
echo "  Status: Upstream OpenWrt (with 802.11be support in 24.10+)"
echo "  Features: EHT/802.11be, SAE, WPA3"
echo ""

echo "=========================================="
echo "Wi-Fi Firmware"
echo "=========================================="
echo ""

echo "MT7996 Firmware (Wi-Fi 7):"
echo "  Package: kmod-mt7996-firmware"
echo "  Variants: standard, 233MHz"
echo "  Location: /lib/firmware/mediatek/mt7996/"
echo ""

echo "MT7988 WO Firmware (Wireless Offload):"
echo "  Package: mt7988-wo-firmware"
echo "  Location: /lib/firmware/mediatek/"
echo ""

echo "=========================================="
echo "5G Modem Driver Stack"
echo "=========================================="
echo ""

# USB Modem Drivers
echo -e "${GREEN}USB Modem Drivers:${NC}"
echo "  kmod-usb-net-qmi-wwan: QMI protocol"
echo "  kmod-usb-net-cdc-mbim: MBIM protocol"
echo "  kmod-usb-net-cdc-ncm:  NCM protocol"
echo "  kmod-usb-serial-option: USB serial (AT commands)"
echo "  kmod-usb-serial-qualcomm: Qualcomm modems"
echo ""

# PCIe/MHI Modem Drivers
echo -e "${GREEN}PCIe/MHI Modem Drivers:${NC}"
MHI_PATH="$REPO_ROOT/$KERNEL_VERSION/package/kernel/linux/modules"
echo "  kmod-mhi-bus: MHI bus driver (kernel 6.6+)"
echo "  kmod-mhi-pci-generic: MHI PCI controller"
echo "  kmod-wwan: WWAN core"
echo "  kmod-mhi-wwan-ctrl: MHI WWAN control (AT/QMI/DIAG)"
echo "  kmod-mhi-wwan-mbim: MHI MBIM data path"
echo "  kmod-mtk-t7xx: MediaTek T7xx PCIe 5G modem"
echo ""

# Userspace tools
echo -e "${GREEN}Userspace Modem Tools:${NC}"
echo "  uqmi: QMI control utility"
echo "  umbim: MBIM control utility"
echo "  modemmanager: Universal modem manager"
echo "  wwan: WWAN protocol handlers"
echo ""

# Modem database
echo -e "${GREEN}Modem Database:${NC}"
MODEM_COUNT=$(find "$REPO_ROOT/common/package/modems/src/data" -type f 2>/dev/null | wc -l)
echo "  Supported modems: $MODEM_COUNT entries"
echo "  Location: common/package/modems/src/data/"
echo ""

# 5G modems specifically
echo "  5G Modems (recent additions):"
for modem in 2c7c-0800 2c7c-0801 2c7c-0900 2c7c-0901 2c7c-0122 2c7c-0530 2c7c-0531 1199-90d2; do
    if [ -f "$REPO_ROOT/common/package/modems/src/data/$modem" ]; then
        desc=$(grep '"desc"' "$REPO_ROOT/common/package/modems/src/data/$modem" | cut -d'"' -f4)
        echo "    $modem: $desc"
    fi
done
echo ""

echo "=========================================="
echo "Target Support (Wi-Fi 7 Devices)"
echo "=========================================="
echo ""

for kernel in 6.6 6.10 6.12 6.18; do
    if [ -f "$REPO_ROOT/$kernel/target/linux/mediatek/image/filogic.mk" ]; then
        echo "Kernel $kernel - MediaTek Filogic (MT7988/MT7986):"
        grep -E "^define Device/bananapi_bpi-r4" "$REPO_ROOT/$kernel/target/linux/mediatek/image/filogic.mk" -A 3 2>/dev/null | head -5 || echo "  BPi-R4 definitions in $kernel"
        echo ""
    fi
done

echo "=========================================="
echo "Config Fragments"
echo "=========================================="
echo ""

echo "Available device configs:"
for config in "$REPO_ROOT"/config-*; do
    if [ -f "$config" ]; then
        name=$(basename "$config")
        wifi7=$(grep -l "mt7996\|MT7996" "$config" 2>/dev/null && echo " [Wi-Fi 7]" || echo "")
        modem5g=$(grep -l "mhi-bus\|mtk-t7xx\|qmi-wwan" "$config" 2>/dev/null && echo " [5G Modem]" || echo "")
        echo "  $name$wifi7$modem5g"
    fi
done
echo ""

echo "=========================================="
echo "Report Complete"
echo "=========================================="
