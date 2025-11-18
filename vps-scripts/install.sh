#!/bin/bash
#
# OpenMPTCProuter VPS Installation Script
# Simple wrapper that calls the main installation wizard
#
# Usage:
#   sudo ./install.sh
#
# Or download and run:
#   curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/install.sh | sudo bash
#

set -e

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

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
    # If wizard.sh doesn't exist locally, download it
    echo -e "${YELLOW}Downloading installation wizard...${NC}"

    TEMP_WIZARD=$(mktemp)
    trap "rm -f $TEMP_WIZARD" EXIT

    if curl -sSL -o "$TEMP_WIZARD" https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/wizard.sh; then
        chmod +x "$TEMP_WIZARD"
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
