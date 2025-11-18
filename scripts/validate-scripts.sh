#!/bin/sh
#
# Script validation tool - checks all shell scripts for common issues
# Uses shellcheck for static analysis
#

set -e

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo "${CYAN}=== OpenMPTCProuter Script Validation ===${NC}"
echo ""

# Check if shellcheck is installed
if ! command -v shellcheck >/dev/null 2>&1; then
    echo "${RED}ERROR: shellcheck is not installed${NC}"
    echo "Install it with: sudo apt-get install shellcheck"
    exit 1
fi

echo "${GREEN}✓ shellcheck found${NC}"
echo ""

# Find all shell scripts
SCRIPTS=$(find "$REPO_ROOT" -type f -name "*.sh" ! -path "*/.*" ! -path "*/dl/*" ! -path "*/build_dir/*" ! -path "*/staging_dir/*")

TOTAL=0
PASSED=0
FAILED=0
WARNINGS=0

echo "${CYAN}Validating shell scripts...${NC}"
echo ""

for script in $SCRIPTS; do
    TOTAL=$((TOTAL + 1))
    RELATIVE_PATH="${script#$REPO_ROOT/}"

    printf "Checking %s ... " "$RELATIVE_PATH"

    # Run shellcheck
    if OUTPUT=$(shellcheck -f gcc "$script" 2>&1); then
        echo "${GREEN}PASS${NC}"
        PASSED=$((PASSED + 1))
    else
        # Check if only warnings or actual errors
        if echo "$OUTPUT" | grep -q "error:"; then
            echo "${RED}FAIL${NC}"
            FAILED=$((FAILED + 1))
            echo "${RED}$OUTPUT${NC}"
        else
            echo "${YELLOW}WARN${NC}"
            WARNINGS=$((WARNINGS + 1))
            echo "${YELLOW}$OUTPUT${NC}"
        fi
    fi
    echo ""
done

echo "${CYAN}=== Validation Summary ===${NC}"
echo "Total scripts:  $TOTAL"
echo "${GREEN}Passed:         $PASSED${NC}"
echo "${YELLOW}Warnings:       $WARNINGS${NC}"
echo "${RED}Failed:         $FAILED${NC}"
echo ""

if [ $FAILED -gt 0 ]; then
    echo "${RED}❌ Validation FAILED - fix errors above${NC}"
    exit 1
elif [ $WARNINGS -gt 0 ]; then
    echo "${YELLOW}⚠️  Validation passed with warnings${NC}"
    exit 0
else
    echo "${GREEN}✅ All scripts validated successfully!${NC}"
    exit 0
fi
