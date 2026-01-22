#!/bin/bash
#
# driver_build_matrix.sh - Build key driver packages for verification
#
# This script builds Wi-Fi and modem driver packages for representative targets
# to verify they compile correctly.
#
# Usage: ./scripts/driver_build_matrix.sh [--dry-run] [--kernel VERSION] [--target TARGET]
#
# Examples:
#   ./scripts/driver_build_matrix.sh                    # Build for default targets
#   ./scripts/driver_build_matrix.sh --kernel 6.6      # Build for kernel 6.6
#   ./scripts/driver_build_matrix.sh --dry-run         # Show what would be built
#
# Copyright (C) 2024-2026 OpenMPTCProuter Project
# Licensed under GPL v3

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"

# Default settings
DRY_RUN=false
KERNEL_VERSION="6.6"
TARGETS=("mediatek/filogic" "x86/64")
VERBOSE=false

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        --kernel)
            KERNEL_VERSION="$2"
            shift 2
            ;;
        --target)
            TARGETS=("$2")
            shift 2
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --dry-run      Show what would be built without building"
            echo "  --kernel VER   Specify kernel version (default: 6.6)"
            echo "  --target T     Specify single target (default: mediatek/filogic, x86/64)"
            echo "  --verbose,-v   Verbose output"
            echo "  --help,-h      Show this help"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo "=========================================="
echo "OpenMPTCProuter Driver Build Matrix"
echo "=========================================="
echo "Date: $(date)"
echo "Kernel: $KERNEL_VERSION"
echo "Targets: ${TARGETS[*]}"
echo "Dry Run: $DRY_RUN"
echo ""

# Package lists for each category
WIFI_PACKAGES=(
    "kmod-mac80211"
    "kmod-cfg80211"
    "kmod-mt76-core"
    "kmod-mt76-connac"
    "kmod-mt7996e"
    "kmod-mt7996-firmware"
    "hostapd-openssl"
    "wpad-openssl"
)

WIFI7_PACKAGES=(
    "kmod-mt7996e"
    "kmod-mt7996-firmware"
    "kmod-mt7996-233-firmware"
    "mt7988-wo-firmware"
    "mt7988-2p5g-phy-firmware"
)

MODEM_USB_PACKAGES=(
    "kmod-usb-net-qmi-wwan"
    "kmod-usb-net-cdc-mbim"
    "kmod-usb-net-cdc-ncm"
    "kmod-usb-serial-option"
    "kmod-usb-serial-qualcomm"
    "kmod-usb-wdm"
    "uqmi"
    "umbim"
)

MODEM_PCIE_PACKAGES=(
    "kmod-mhi-bus"
    "kmod-mhi-pci-generic"
    "kmod-wwan"
    "kmod-mhi-wwan-ctrl"
    "kmod-mhi-wwan-mbim"
    "kmod-mhi-net"
    "kmod-mtk-t7xx"
    "modemmanager"
)

# Function to check if build environment exists
check_build_env() {
    local target="$1"
    local target_dir="${target//\//_}"
    local source_dir="$REPO_ROOT/$target_dir/$KERNEL_VERSION/source"

    if [ -d "$source_dir" ]; then
        echo -e "${GREEN}Found:${NC} $source_dir"
        return 0
    else
        echo -e "${YELLOW}Not found:${NC} $source_dir"
        echo "  Run build.sh to set up the environment first"
        return 1
    fi
}

# Function to verify package exists in feeds/packages
check_package_available() {
    local pkg="$1"
    local source_dir="$2"

    # Check in main package directory
    if find "$source_dir/package" -name "Makefile" -exec grep -l "^PKG_NAME:=$pkg$\|Package/$pkg$" {} \; 2>/dev/null | head -1 | grep -q .; then
        return 0
    fi

    # Check in feeds
    if [ -d "$source_dir/feeds" ]; then
        if find "$source_dir/feeds" -name "Makefile" -exec grep -l "^PKG_NAME:=$pkg$\|Package/$pkg$" {} \; 2>/dev/null | head -1 | grep -q .; then
            return 0
        fi
    fi

    return 1
}

# Function to build packages for a target
build_target() {
    local target="$1"
    local packages=("${@:2}")
    local target_dir="${target//\//_}"
    local source_dir="$REPO_ROOT/$target_dir/$KERNEL_VERSION/source"

    echo ""
    echo -e "${BLUE}Building for target: $target${NC}"
    echo "Source directory: $source_dir"
    echo ""

    if [ ! -d "$source_dir" ]; then
        echo -e "${RED}ERROR: Source directory not found${NC}"
        echo "Run: OMR_KERNEL=$KERNEL_VERSION OMR_TARGET=${target_dir} ./build.sh"
        return 1
    fi

    local built=0
    local skipped=0
    local failed=0

    for pkg in "${packages[@]}"; do
        echo -n "  $pkg: "

        if $DRY_RUN; then
            if check_package_available "$pkg" "$source_dir"; then
                echo -e "${GREEN}[Would build]${NC}"
                ((built++))
            else
                echo -e "${YELLOW}[Not available]${NC}"
                ((skipped++))
            fi
        else
            if check_package_available "$pkg" "$source_dir"; then
                if $VERBOSE; then
                    echo ""
                    (cd "$source_dir" && make package/$pkg/compile V=s 2>&1) || {
                        echo -e "${RED}[FAILED]${NC}"
                        ((failed++))
                        continue
                    }
                else
                    if (cd "$source_dir" && make package/$pkg/compile V=s >/dev/null 2>&1); then
                        echo -e "${GREEN}[OK]${NC}"
                        ((built++))
                    else
                        echo -e "${RED}[FAILED]${NC}"
                        ((failed++))
                    fi
                fi
            else
                echo -e "${YELLOW}[Not available]${NC}"
                ((skipped++))
            fi
        fi
    done

    echo ""
    echo "Results: $built built, $skipped skipped, $failed failed"
    return $failed
}

# Main build process
echo "=========================================="
echo "Checking Build Environment"
echo "=========================================="

for target in "${TARGETS[@]}"; do
    check_build_env "$target"
done
echo ""

# Build Wi-Fi packages
echo "=========================================="
echo "Wi-Fi Driver Packages"
echo "=========================================="
for target in "${TARGETS[@]}"; do
    build_target "$target" "${WIFI_PACKAGES[@]}" || true
done

# Build Wi-Fi 7 specific packages (MediaTek Filogic only)
echo "=========================================="
echo "Wi-Fi 7 Packages (MediaTek Filogic)"
echo "=========================================="
for target in "${TARGETS[@]}"; do
    if [[ "$target" == *"mediatek"* ]]; then
        build_target "$target" "${WIFI7_PACKAGES[@]}" || true
    fi
done

# Build USB modem packages
echo "=========================================="
echo "USB Modem Packages"
echo "=========================================="
for target in "${TARGETS[@]}"; do
    build_target "$target" "${MODEM_USB_PACKAGES[@]}" || true
done

# Build PCIe modem packages
echo "=========================================="
echo "PCIe/MHI Modem Packages"
echo "=========================================="
for target in "${TARGETS[@]}"; do
    build_target "$target" "${MODEM_PCIE_PACKAGES[@]}" || true
done

echo ""
echo "=========================================="
echo "Build Matrix Complete"
echo "=========================================="

if $DRY_RUN; then
    echo ""
    echo "This was a dry run. To actually build, run without --dry-run"
fi
