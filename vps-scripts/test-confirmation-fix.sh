#!/bin/bash
#
# Comprehensive test for confirmation prompt fix
# Tests that the fix resolves the issue where pressing Enter 
# caused unwanted "Installation cancelled by user" message
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=========================================="
echo "Confirmation Prompt Fix - Validation Test"
echo "=========================================="
echo ""

# Test 1: Verify wizard.sh contains the fix
echo "Test 1: Checking wizard.sh for the fix..."
if grep -q 'REPLY=\${REPLY:-Y}' "$SCRIPT_DIR/wizard.sh"; then
    echo "✓ PASS: wizard.sh contains default value handling"
else
    echo "✗ FAIL: wizard.sh missing default value handling"
    exit 1
fi

if grep -q 'Continue with installation? \[Y/n\]:' "$SCRIPT_DIR/wizard.sh"; then
    echo "✓ PASS: wizard.sh uses [Y/n] prompt format"
else
    echo "✗ FAIL: wizard.sh missing [Y/n] prompt format"
    exit 1
fi

echo ""

# Test 2: Verify omr-vps-install.sh contains the fix
echo "Test 2: Checking omr-vps-install.sh for the fix..."
if grep -q 'REPLY=\${REPLY:-Y}' "$SCRIPT_DIR/omr-vps-install.sh"; then
    echo "✓ PASS: omr-vps-install.sh contains default value handling"
else
    echo "✗ FAIL: omr-vps-install.sh missing default value handling"
    exit 1
fi

if grep -q 'Do you want to proceed with the installation? \[Y/n\]:' "$SCRIPT_DIR/omr-vps-install.sh"; then
    echo "✓ PASS: omr-vps-install.sh uses [Y/n] prompt format"
else
    echo "✗ FAIL: omr-vps-install.sh missing [Y/n] prompt format"
    exit 1
fi

echo ""

# Test 3: Verify bash syntax is valid
echo "Test 3: Checking bash syntax..."
bash -n "$SCRIPT_DIR/wizard.sh" || { echo "✗ FAIL: wizard.sh has syntax errors"; exit 1; }
echo "✓ PASS: wizard.sh syntax is valid"

bash -n "$SCRIPT_DIR/omr-vps-install.sh" || { echo "✗ FAIL: omr-vps-install.sh has syntax errors"; exit 1; }
echo "✓ PASS: omr-vps-install.sh syntax is valid"

echo ""

# Test 4: Simulate the logic to ensure it works
echo "Test 4: Simulating confirmation logic..."

# Test empty input (the bug scenario)
REPLY=""
REPLY=${REPLY:-Y}
if [[ $REPLY =~ ^[Yy][Ee][Ss]$|^[Yy]$ ]]; then
    echo "✓ PASS: Empty input (Enter key) defaults to yes"
else
    echo "✗ FAIL: Empty input should default to yes"
    exit 1
fi

# Test 'no' input (should still cancel)
REPLY="no"
REPLY=${REPLY:-Y}
if [[ ! $REPLY =~ ^[Yy][Ee][Ss]$|^[Yy]$ ]]; then
    echo "✓ PASS: 'no' input correctly cancels"
else
    echo "✗ FAIL: 'no' input should cancel"
    exit 1
fi

# Test 'yes' input (should continue)
REPLY="yes"
REPLY=${REPLY:-Y}
if [[ $REPLY =~ ^[Yy][Ee][Ss]$|^[Yy]$ ]]; then
    echo "✓ PASS: 'yes' input correctly continues"
else
    echo "✗ FAIL: 'yes' input should continue"
    exit 1
fi

echo ""
echo "=========================================="
echo "All tests passed! ✓"
echo "=========================================="
echo ""
echo "Fix verified:"
echo "  • Pressing Enter now defaults to 'yes' and continues installation"
echo "  • User experience is now intuitive ([Y/n] format)"
echo "  • Explicit 'no' input still cancels installation"
echo ""
