#!/bin/sh
#
# Smoke test suite - basic functionality tests
# Run this before committing changes to catch obvious issues
#

set -e

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# Test result tracking
TEST_RESULTS=""

run_test() {
    local test_name="$1"
    local test_command="$2"

    TESTS_RUN=$((TESTS_RUN + 1))

    printf "[%d/%d] Testing: %s ... " "$TESTS_RUN" "$TOTAL_TESTS" "$test_name"

    if eval "$test_command" >/dev/null 2>&1; then
        echo "${GREEN}PASS${NC}"
        TESTS_PASSED=$((TESTS_PASSED + 1))
        TEST_RESULTS="${TEST_RESULTS}✓ $test_name\n"
        return 0
    else
        echo "${RED}FAIL${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
        TEST_RESULTS="${TEST_RESULTS}✗ $test_name\n"
        return 1
    fi
}

echo "${CYAN}=== OpenMPTCProuter Smoke Tests ===${NC}"
echo ""

# Count total tests
TOTAL_TESTS=20

# Test 1: Repository structure
run_test "Repository root directory exists" "test -d '$REPO_ROOT'"
run_test "build.sh exists and executable" "test -x '$REPO_ROOT/build.sh'"
run_test "README.md exists" "test -f '$REPO_ROOT/README.md'"
run_test "LICENSE file exists" "test -f '$REPO_ROOT/LICENSE'"

# Test 2: Configuration files
run_test "Master config file exists" "test -f '$REPO_ROOT/config'"
run_test "At least 10 config-* files exist" "test $(find '$REPO_ROOT' -maxdepth 1 -name 'config-*' | wc -l) -ge 10"

# Test 3: Kernel directories
run_test "Kernel 5.4 directory exists" "test -d '$REPO_ROOT/5.4'"
run_test "Kernel 6.1 directory exists" "test -d '$REPO_ROOT/6.1'"
run_test "Kernel 6.6 directory exists" "test -d '$REPO_ROOT/6.6'"
run_test "Kernel 6.12 directory exists" "test -d '$REPO_ROOT/6.12'"

# Test 4: Scripts directory
run_test "Scripts directory exists" "test -d '$REPO_ROOT/scripts'"
run_test "easy-install.sh exists" "test -f '$REPO_ROOT/scripts/easy-install.sh'"
run_test "auto-pair.sh exists" "test -f '$REPO_ROOT/scripts/auto-pair.sh'"
run_test "client-auto-setup.sh exists" "test -f '$REPO_ROOT/scripts/client-auto-setup.sh'"

# Test 5: GitHub workflows
run_test "GitHub workflows directory exists" "test -d '$REPO_ROOT/.github/workflows'"
run_test "build.yml workflow exists" "test -f '$REPO_ROOT/.github/workflows/build.yml'"
run_test "build-vps.yml workflow exists" "test -f '$REPO_ROOT/.github/workflows/build-vps.yml'"

# Test 6: Documentation
run_test "FAQ.md exists" "test -f '$REPO_ROOT/FAQ.md'"
run_test "CONTRIBUTING.md exists" "test -f '$REPO_ROOT/CONTRIBUTING.md'"
run_test "SETUP_GUIDE.md exists" "test -f '$REPO_ROOT/SETUP_GUIDE.md'"

echo ""
echo "${CYAN}=== Test Summary ===${NC}"
echo "Tests run:     $TESTS_RUN"
echo "${GREEN}Passed:        $TESTS_PASSED${NC}"
echo "${RED}Failed:        $TESTS_FAILED${NC}"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo "${GREEN}✅ All smoke tests passed!${NC}"
    exit 0
else
    echo "${RED}❌ Some tests failed. Review the output above.${NC}"
    exit 1
fi
