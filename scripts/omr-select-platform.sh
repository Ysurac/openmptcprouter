#!/bin/bash
#
# OpenMPTCProuter Platform Selection Helper
# Interactive menu for selecting build platform
# Copyright (C) 2025 spotty118 - OpenMPTCProuter Optimized
#
# Note: Requires bash for array support
#

set -e

# Load shared library
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
if [ -f "$SCRIPT_DIR/omr-lib.sh" ]; then
    . "$SCRIPT_DIR/omr-lib.sh"
else
    echo "Error: omr-lib.sh not found"
    exit 1
fi

REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

show_help() {
    cat <<EOF
OpenMPTCProuter Platform Selection Helper

${OMR_BOLD}USAGE:${OMR_NC}
    $0 [options]

${OMR_BOLD}OPTIONS:${OMR_NC}
    ${OMR_CYAN}--kernel <version>${OMR_NC}    Pre-select kernel version (5.4, 6.1, 6.6, 6.12)
    ${OMR_CYAN}--category <name>${OMR_NC}     Show only platforms in category
    ${OMR_CYAN}--list${OMR_NC}                List all platforms and exit
    ${OMR_CYAN}--help${OMR_NC}                Show this help message

${OMR_BOLD}EXAMPLES:${OMR_NC}
    # Interactive selection
    $0

    # Pre-select kernel version
    $0 --kernel 6.12

    # Show only ARM platforms
    $0 --category arm

    # List all available platforms
    $0 --list

EOF
}

# Platform categories for better organization
categorize_platform() {
    local platform="$1"

    case "$platform" in
        x86_64|x86)
            echo "x86/PC"
            ;;
        rpi*|bpi*)
            echo "ARM SBC"
            ;;
        r2s|r4s|r5s|r6s|r5c)
            echo "Rockchip"
            ;;
        mt2500|mt3000|mt6000)
            echo "MediaTek"
            ;;
        edgerouter-x*)
            echo "MIPS"
            ;;
        *)
            echo "Other"
            ;;
    esac
}

get_platform_description() {
    local platform="$1"

    case "$platform" in
        x86_64) echo "Generic x86_64 PC (64-bit)" ;;
        x86) echo "Generic x86 PC (32-bit)" ;;
        rpi2) echo "Raspberry Pi 2" ;;
        rpi3) echo "Raspberry Pi 3" ;;
        rpi4) echo "Raspberry Pi 4" ;;
        rpi5) echo "Raspberry Pi 5" ;;
        bpi-r1) echo "Banana Pi R1" ;;
        bpi-r2) echo "Banana Pi R2" ;;
        bpi-r3) echo "Banana Pi R3" ;;
        bpi-r4) echo "Banana Pi R4 (WiFi 7)" ;;
        bpi-r64) echo "Banana Pi R64" ;;
        r2s) echo "NanoPi R2S" ;;
        r4s) echo "NanoPi R4S" ;;
        r5s) echo "NanoPi R5S" ;;
        r6s) echo "NanoPi R6S" ;;
        r5c) echo "NanoPi R5C" ;;
        mt2500) echo "GL.iNet GL-MT2500" ;;
        mt3000) echo "GL.iNet GL-MT3000 (Beryl AX)" ;;
        mt6000) echo "GL.iNet GL-MT6000 (Flint 2)" ;;
        edgerouter-x) echo "Ubiquiti EdgeRouter X" ;;
        edgerouter-x-sfp) echo "Ubiquiti EdgeRouter X SFP" ;;
        *) echo "$platform" ;;
    esac
}

list_all_platforms() {
    omr_log_header "Available Platforms"

    local current_category=""
    local count=0

    # Get all config files and sort them
    for config_file in "$REPO_ROOT"/config-*; do
        if [ -f "$config_file" ]; then
            local platform
            platform=$(basename "$config_file" | sed 's/^config-//')

            local category
            category=$(categorize_platform "$platform")

            local description
            description=$(get_platform_description "$platform")

            # Print category header if changed
            if [ "$category" != "$current_category" ]; then
                if [ $count -gt 0 ]; then
                    echo ""
                fi
                printf "${OMR_CYAN}${OMR_BOLD}%s:${OMR_NC}\n" "$category"
                current_category="$category"
            fi

            printf "  ${OMR_WHITE}%-20s${OMR_NC} %s\n" "$platform" "$description"
            count=$((count + 1))
        fi
    done

    echo ""
    omr_log_info "Total platforms: $count"
    echo ""
}

select_kernel() {
    local preselected="$1"

    if [ -n "$preselected" ]; then
        case "$preselected" in
            5.4|6.1|6.6|6.12)
                echo "$preselected"
                return 0
                ;;
            *)
                omr_log_error "Invalid kernel version: $preselected"
                omr_log_info "Valid versions: 5.4, 6.1, 6.6, 6.12"
                exit 1
                ;;
        esac
    fi

    omr_log_header "Select Kernel Version"

    printf "  ${OMR_CYAN}1)${OMR_NC} 5.4  ${OMR_DIM}(OpenWrt 21.02 - Stable, older hardware)${OMR_NC}\n"
    printf "  ${OMR_CYAN}2)${OMR_NC} 6.1  ${OMR_DIM}(OpenWrt 23.05 - LTS)${OMR_NC}\n"
    printf "  ${OMR_CYAN}3)${OMR_NC} 6.6  ${OMR_DIM}(OpenWrt 24.10 - Latest stable)${OMR_NC}\n"
    printf "  ${OMR_CYAN}4)${OMR_NC} 6.12 ${OMR_DIM}(OpenWrt main - Bleeding edge)${OMR_NC}\n"

    echo ""
    printf "${OMR_YELLOW}?${OMR_NC} Select kernel version [1-4]: "
    read -r choice

    case "$choice" in
        1) echo "5.4" ;;
        2) echo "6.1" ;;
        3) echo "6.6" ;;
        4) echo "6.12" ;;
        *)
            omr_log_error "Invalid selection"
            exit 1
            ;;
    esac
}

select_platform() {
    local category_filter="$1"

    omr_log_header "Select Platform"

    # Collect platforms by category
    local -a platforms
    local -a descriptions
    local -a categories
    local idx=0

    for config_file in "$REPO_ROOT"/config-*; do
        if [ -f "$config_file" ]; then
            local platform
            platform=$(basename "$config_file" | sed 's/^config-//')

            local category
            category=$(categorize_platform "$platform")

            # Skip if category filter is set and doesn't match
            if [ -n "$category_filter" ] && [ "$category" != "$category_filter" ]; then
                continue
            fi

            platforms[$idx]="$platform"
            descriptions[$idx]="$(get_platform_description "$platform")"
            categories[$idx]="$category"
            idx=$((idx + 1))
        fi
    done

    if [ $idx -eq 0 ]; then
        omr_log_error "No platforms found"
        exit 1
    fi

    # Display platforms grouped by category
    local current_category=""
    local display_idx=1

    for i in $(seq 0 $((idx - 1))); do
        if [ "${categories[$i]}" != "$current_category" ]; then
            if [ $display_idx -gt 1 ]; then
                echo ""
            fi
            printf "${OMR_CYAN}${OMR_BOLD}%s:${OMR_NC}\n" "${categories[$i]}"
            current_category="${categories[$i]}"
        fi

        printf "  ${OMR_CYAN}%2d)${OMR_NC} %-20s ${OMR_DIM}%s${OMR_NC}\n" \
            "$display_idx" "${platforms[$((i))]}" "${descriptions[$i]}"
        display_idx=$((display_idx + 1))
    done

    echo ""
    printf "${OMR_YELLOW}?${OMR_NC} Select platform [1-%d]: " "$idx"
    read -r choice

    if [ "$choice" -ge 1 ] && [ "$choice" -le "$idx" ]; then
        echo "${platforms[$((choice - 1))]}"
    else
        omr_log_error "Invalid selection"
        exit 1
    fi
}

# Parse command line arguments
KERNEL_VERSION=""
CATEGORY_FILTER=""
LIST_ONLY=0

while [ $# -gt 0 ]; do
    case "$1" in
        --kernel)
            KERNEL_VERSION="$2"
            shift 2
            ;;
        --category)
            CATEGORY_FILTER="$2"
            shift 2
            ;;
        --list)
            LIST_ONLY=1
            shift
            ;;
        --help|-h)
            show_help
            exit 0
            ;;
        *)
            omr_log_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Handle list-only mode
if [ $LIST_ONLY -eq 1 ]; then
    list_all_platforms
    exit 0
fi

# Interactive selection
SELECTED_KERNEL=$(select_kernel "$KERNEL_VERSION")
SELECTED_PLATFORM=$(select_platform "$CATEGORY_FILTER")

# Display summary
omr_print_summary "Build Configuration" \
    "Kernel Version" "$SELECTED_KERNEL" \
    "Platform" "$SELECTED_PLATFORM" \
    "Config File" "config-${SELECTED_PLATFORM}"

# Ask if user wants to start build
echo ""
if omr_ask_yes_no "Start build with this configuration?" "y"; then
    omr_log_info "Starting build..."
    echo ""

    # Export environment variables for build.sh
    export OMR_KERNEL="$SELECTED_KERNEL"
    export OMR_TARGET="$SELECTED_PLATFORM"

    # Run build.sh
    exec "$REPO_ROOT/build.sh"
else
    omr_log_info "Build cancelled"
    echo ""
    omr_log_info "To build manually, run:"
    printf "  ${OMR_DIM}OMR_KERNEL=%s OMR_TARGET=%s ./build.sh${OMR_NC}\n" \
        "$SELECTED_KERNEL" "$SELECTED_PLATFORM"
    echo ""
fi
