#!/bin/bash
#
# OpenMPTCProuter VPS Installation Script
# Convenience wrapper that launches the wizard
#
# Usage: ./install.sh
# Or: curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/install.sh | sudo bash
#

set -e

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}╔═══════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║  OpenMPTCProuter VPS Installation            ║${NC}"
echo -e "${GREEN}╚═══════════════════════════════════════════════╝${NC}"
echo ""

# Determine script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check if wizard.sh exists in the same directory
if [ -f "$SCRIPT_DIR/wizard.sh" ]; then
    echo -e "${GREEN}✓${NC} Found installation wizard"
    echo -e "${YELLOW}→${NC} Launching wizard..."
    echo ""
    exec bash "$SCRIPT_DIR/wizard.sh" "$@"
elif [ -f "$SCRIPT_DIR/omr-vps-install.sh" ]; then
    echo -e "${GREEN}✓${NC} Found VPS installer"
    echo -e "${YELLOW}→${NC} Launching installer..."
    echo ""
    exec bash "$SCRIPT_DIR/omr-vps-install.sh" "$@"
else
    # Try to download the wizard if we're running from curl
    echo -e "${YELLOW}→${NC} Downloading installation wizard..."
    TEMP_WIZARD=$(mktemp)
    if curl -sSL -o "$TEMP_WIZARD" https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/wizard.sh; then
        echo -e "${GREEN}✓${NC} Download successful"
        echo -e "${YELLOW}→${NC} Launching wizard..."
        echo ""
        exec bash "$TEMP_WIZARD" "$@"
    else
        echo -e "${RED}✗${NC} Failed to download wizard"
        echo ""
        echo "Please try one of these methods instead:"
        echo ""
        echo "  1. One-line install:"
        echo "     curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/wizard.sh | sudo bash"
        echo ""
        echo "  2. Download and run:"
        echo "     wget https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/wizard.sh"
        echo "     chmod +x wizard.sh"
        echo "     sudo ./wizard.sh"
        echo ""
        exit 1
    fi
fi
