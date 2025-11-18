#!/bin/bash
#
# OpenMPTCProuter VPS Installation Script
# Simple wrapper that calls the main installation wizard
#
# Usage:
#   sudo ./install.sh
#
# SECURITY: This script now verifies downloaded files with SHA256 checksums
#

set -e

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Expected SHA256 hash of wizard.sh (update this when wizard.sh changes)
# Generate with: sha256sum wizard.sh
EXPECTED_HASH="${WIZARD_SHA256:-skip}"  # Set WIZARD_SHA256 env var to enforce verification

# Check if running as root
if [ "$(id -u)" -ne 0 ]; then
    echo -e "${RED}Error:${NC} This script must be run as root. Please use: sudo $0" >&2
    exit 1
fi

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check if wizard.sh exists in the same directory
if [ -f "$SCRIPT_DIR/wizard.sh" ]; then
    echo -e "${GREEN}Starting OpenMPTCProuter VPS installation wizard...${NC}"
    echo ""
    exec "$SCRIPT_DIR/wizard.sh"
else
    # If wizard.sh doesn't exist locally, download it with integrity verification
    echo -e "${YELLOW}Downloading installation wizard...${NC}"

    # Create temp file with restricted permissions (security fix)
    umask 077
    TEMP_WIZARD=$(mktemp)
    trap "rm -f '$TEMP_WIZARD'" EXIT INT TERM

    if curl -sSL -o "$TEMP_WIZARD" https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/wizard.sh; then
        # Verify download integrity if hash is provided
        if [ "$EXPECTED_HASH" != "skip" ] && [ -n "$EXPECTED_HASH" ]; then
            echo -e "${CYAN}Verifying download integrity...${NC}"
            ACTUAL_HASH=$(sha256sum "$TEMP_WIZARD" | awk '{print $1}')
            if [ "$ACTUAL_HASH" != "$EXPECTED_HASH" ]; then
                echo -e "${RED}Error:${NC} Downloaded file checksum mismatch!" >&2
                echo -e "${RED}Expected:${NC} $EXPECTED_HASH" >&2
                echo -e "${RED}Got:${NC}      $ACTUAL_HASH" >&2
                echo -e "${YELLOW}This could indicate a man-in-the-middle attack or corrupted download.${NC}" >&2
                exit 1
            fi
            echo -e "${GREEN}✓ Integrity verified${NC}"
        else
            echo -e "${YELLOW}Warning: Skipping integrity verification (WIZARD_SHA256 not set)${NC}"
            echo -e "${YELLOW}For maximum security, set WIZARD_SHA256 environment variable${NC}"
        fi

        chmod 700 "$TEMP_WIZARD"
        echo -e "${GREEN}Starting OpenMPTCProuter VPS installation wizard...${NC}"
        echo ""
        exec "$TEMP_WIZARD"
    else
        echo -e "${RED}Error:${NC} Failed to download installation wizard" >&2
        echo -e "${YELLOW}Please try downloading manually:${NC}"
        echo "  wget https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/wizard.sh"
        echo "  chmod +x wizard.sh"
        echo "  sudo ./wizard.sh"
        exit 1
    fi
fi
