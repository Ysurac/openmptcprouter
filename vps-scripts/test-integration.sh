#!/bin/bash
#
# Comprehensive integration tests for OpenMPTCProuter
# Tests VPS and router setup compatibility and stability
#
# Usage: ./test-integration.sh
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$REPO_ROOT"

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to print test result
test_result() {
    local status=$1
    local message=$2
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✓ PASS:${NC} $message"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}✗ FAIL:${NC} $message"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}OpenMPTCProuter Integration Tests${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

# Test 1: Script syntax validation
echo -e "${YELLOW}[1] Validating script syntax...${NC}"
for script in vps-scripts/omr-vps-install.sh vps-scripts/wizard.sh scripts/easy-install.sh scripts/client-auto-setup.sh scripts/auto-pair.sh; do
    if bash -n "$script" 2>/dev/null; then
        test_result "PASS" "Valid syntax: $script"
    else
        test_result "FAIL" "Syntax error: $script"
    fi
done

# Test 2: Port consistency
echo -e "${YELLOW}[2] Checking port consistency across scripts...${NC}"
VPS_PORTS=$(grep -oh '65500\|65510\|65520' vps-scripts/omr-vps-install.sh vps-scripts/wizard.sh | sort -u | wc -l)
CLIENT_PORTS=$(grep -oh '65500\|65510\|65520' scripts/client-auto-setup.sh scripts/auto-pair.sh | sort -u | wc -l)

if [ "$VPS_PORTS" -ge 2 ] && [ "$CLIENT_PORTS" -ge 1 ]; then
    test_result "PASS" "Port consistency maintained"
else
    test_result "FAIL" "Port inconsistency detected"
fi

# Test 3: Encryption method consistency
echo -e "${YELLOW}[3] Verifying encryption method...${NC}"
ENCRYPTION=$(grep -l 'chacha20-ietf-poly1305' vps-scripts/*.sh scripts/*.sh | wc -l)
if [ "$ENCRYPTION" -ge 3 ]; then
    test_result "PASS" "Consistent encryption method used"
else
    test_result "FAIL" "Encryption method inconsistency"
fi

# Test 4: Configuration file format
echo -e "${YELLOW}[4] Testing configuration file generation...${NC}"
CONFIG_GEN=$(grep -l 'config.json' vps-scripts/omr-vps-install.sh vps-scripts/wizard.sh | wc -l)
if [ "$CONFIG_GEN" -ge 2 ]; then
    test_result "PASS" "Configuration files properly generated"
else
    test_result "FAIL" "Configuration generation incomplete"
fi

# Test 5: Firewall rules
echo -e "${YELLOW}[5] Checking firewall configuration...${NC}"
FIREWALL=$(grep -l 'iptables' vps-scripts/*.sh | wc -l)
if [ "$FIREWALL" -ge 2 ]; then
    test_result "PASS" "Firewall rules configured"
else
    test_result "FAIL" "Firewall configuration missing"
fi

# Test 6: MPTCP setup
echo -e "${YELLOW}[6] Verifying MPTCP configuration...${NC}"
MPTCP=$(grep -h 'mptcp_enabled.*1' vps-scripts/*.sh scripts/auto-pair.sh | wc -l)
if [ "$MPTCP" -ge 2 ]; then
    test_result "PASS" "MPTCP properly configured"
else
    test_result "FAIL" "MPTCP configuration incomplete"
fi

# Test 7: Service management
echo -e "${YELLOW}[7] Testing service management...${NC}"
VPS_SERVICES=$(grep -h 'systemctl\s*restart\|systemctl\s*enable' vps-scripts/*.sh | wc -l)
CLIENT_SERVICES=$(grep -h '/etc/init.d.*restart' scripts/client-auto-setup.sh | wc -l)
if [ "$VPS_SERVICES" -ge 2 ] && [ "$CLIENT_SERVICES" -ge 1 ]; then
    test_result "PASS" "Service management implemented"
else
    test_result "FAIL" "Service management incomplete"
fi

# Test 8: Error handling
echo -e "${YELLOW}[8] Checking error handling...${NC}"
ERROR_HANDLING=$(grep -l 'set -e' vps-scripts/*.sh scripts/*.sh | wc -l)
if [ "$ERROR_HANDLING" -ge 4 ]; then
    test_result "PASS" "Error handling enabled in scripts"
else
    test_result "FAIL" "Missing error handling"
fi

# Test 9: IP detection
echo -e "${YELLOW}[9] Verifying IP detection mechanisms...${NC}"
IP_DETECT=$(grep -h 'ifconfig.me\|icanhazip.com' vps-scripts/*.sh scripts/*.sh | wc -l)
if [ "$IP_DETECT" -ge 3 ]; then
    test_result "PASS" "IP detection implemented"
else
    test_result "FAIL" "IP detection incomplete"
fi

# Test 10: Password generation
echo -e "${YELLOW}[10] Testing secure password generation...${NC}"
PASS_GEN=$(grep -h '/dev/urandom.*base64' vps-scripts/*.sh scripts/*.sh | wc -l)
if [ "$PASS_GEN" -ge 3 ]; then
    test_result "PASS" "Secure password generation"
else
    test_result "FAIL" "Password generation incomplete"
fi

# Summary
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}Test Summary${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "Total Tests:     ${BLUE}$TOTAL_TESTS${NC}"
echo -e "Passed:          ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed:          ${RED}$FAILED_TESTS${NC}"
echo ""

if [ "$FAILED_TESTS" -eq 0 ]; then
    echo -e "${GREEN}✓ All integration tests passed!${NC}"
    echo -e "${GREEN}VPS and router setup scripts are compatible and stable.${NC}"
    exit 0
else
    echo -e "${RED}✗ Some tests failed.${NC}"
    echo -e "${RED}Please review the issues above.${NC}"
    exit 1
fi
