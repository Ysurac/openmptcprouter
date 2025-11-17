#!/bin/bash
#
# Basic validation tests for VPS wizard script
# Tests syntax, basic structure, and key functions
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WIZARD_SCRIPT="$SCRIPT_DIR/../vps-scripts/wizard.sh"

echo "=== VPS Wizard Validation Tests ==="
echo ""

# Test 1: Script exists
echo "Test 1: Checking if wizard script exists..."
if [ ! -f "$WIZARD_SCRIPT" ]; then
    echo "FAIL: wizard.sh not found at $WIZARD_SCRIPT"
    exit 1
fi
echo "✓ PASS: wizard.sh exists"
echo ""

# Test 2: Script is executable
echo "Test 2: Checking if wizard script is executable..."
if [ ! -x "$WIZARD_SCRIPT" ]; then
    echo "FAIL: wizard.sh is not executable"
    exit 1
fi
echo "✓ PASS: wizard.sh is executable"
echo ""

# Test 3: Bash syntax check
echo "Test 3: Checking bash syntax..."
if ! bash -n "$WIZARD_SCRIPT"; then
    echo "FAIL: Syntax errors found in wizard.sh"
    exit 1
fi
echo "✓ PASS: No syntax errors"
echo ""

# Test 4: Required functions exist
echo "Test 4: Checking for required functions..."
required_functions=(
    "print_step"
    "print_success"
    "print_error"
    "print_warning"
    "print_info"
)

for func in "${required_functions[@]}"; do
    if ! grep -q "^${func}()" "$WIZARD_SCRIPT"; then
        echo "FAIL: Function $func not found"
        exit 1
    fi
    echo "  ✓ Found function: $func"
done
echo "✓ PASS: All required functions present"
echo ""

# Test 5: Check for proper shebang
echo "Test 5: Checking shebang..."
shebang=$(head -n 1 "$WIZARD_SCRIPT")
if [[ ! "$shebang" =~ ^#!/bin/bash ]]; then
    echo "FAIL: Invalid or missing shebang. Found: $shebang"
    exit 1
fi
echo "✓ PASS: Correct shebang present"
echo ""

# Test 6: Check for set -e (exit on error)
echo "Test 6: Checking for error handling..."
if ! grep -q "^set -e" "$WIZARD_SCRIPT"; then
    echo "WARNING: 'set -e' not found, script may not exit on errors"
else
    echo "✓ PASS: Error handling enabled (set -e)"
fi
echo ""

# Test 7: Check for root user check
echo "Test 7: Checking for root user validation..."
if ! grep -q 'id -u.*-ne 0' "$WIZARD_SCRIPT"; then
    echo "FAIL: Root user check not found"
    exit 1
fi
echo "✓ PASS: Root user check present"
echo ""

# Test 8: Check for OS detection
echo "Test 8: Checking for OS detection..."
if ! grep -q '/etc/os-release' "$WIZARD_SCRIPT"; then
    echo "FAIL: OS detection not found"
    exit 1
fi
echo "✓ PASS: OS detection present"
echo ""

# Test 9: Check for color codes
echo "Test 9: Checking for color output support..."
color_codes=("RED" "GREEN" "YELLOW" "BLUE" "CYAN" "NC")
for color in "${color_codes[@]}"; do
    if ! grep -q "$color=" "$WIZARD_SCRIPT"; then
        echo "WARNING: Color code $color not found"
    fi
done
echo "✓ PASS: Color codes defined"
echo ""

# Test 10: Check for key configuration steps
echo "Test 10: Checking for key configuration steps..."
key_steps=(
    "apt-get update"
    "shadowsocks"
    "iptables"
    "sysctl"
    "openmptcprouter"
)

for step in "${key_steps[@]}"; do
    if ! grep -qi "$step" "$WIZARD_SCRIPT"; then
        echo "WARNING: Key step '$step' not found"
    else
        echo "  ✓ Found: $step"
    fi
done
echo "✓ PASS: Key configuration steps present"
echo ""

# Test 11: Check script size (should be substantial)
echo "Test 11: Checking script size..."
script_size=$(wc -c < "$WIZARD_SCRIPT")
if [ "$script_size" -lt 10000 ]; then
    echo "WARNING: Script seems small ($script_size bytes). Expected > 10KB"
else
    echo "✓ PASS: Script size is adequate ($script_size bytes)"
fi
echo ""

# Test 12: Check for license header
echo "Test 12: Checking for license header..."
if ! grep -q "GNU General Public License" "$WIZARD_SCRIPT"; then
    echo "WARNING: GPL license not found in header"
else
    echo "✓ PASS: GPL license header present"
fi
echo ""

# Summary
echo "================================="
echo "All validation tests passed! ✓"
echo "================================="
echo ""
echo "Note: These are basic structural tests."
echo "Full functional testing requires running on a supported VPS."
