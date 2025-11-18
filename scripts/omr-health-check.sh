#!/bin/sh
#
# OpenMPTCProuter Health Check
# Quick diagnostic tool to check build environment and system status
# Copyright (C) 2025 spotty118 - OpenMPTCProuter Optimized
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
OpenMPTCProuter Health Check

${OMR_BOLD}USAGE:${OMR_NC}
    $0 [options]

${OMR_BOLD}OPTIONS:${OMR_NC}
    ${OMR_CYAN}--quick${OMR_NC}       Quick check (essential items only)
    ${OMR_CYAN}--full${OMR_NC}        Full system check (default)
    ${OMR_CYAN}--fix${OMR_NC}         Attempt to fix common issues
    ${OMR_CYAN}--help${OMR_NC}        Show this help message

${OMR_BOLD}CHECKS PERFORMED:${OMR_NC}
    • System dependencies
    • Disk space availability
    • Git repository status
    • Build environment
    • Configuration files
    • Kernel directories
    • Script permissions

EOF
}

CHECK_FAILED=0
CHECK_WARNINGS=0
CHECK_PASSED=0

check_item() {
    local name="$1"
    local command="$2"
    local is_critical="${3:-yes}"

    omr_progress_start "$name"

    if eval "$command" >/dev/null 2>&1; then
        omr_progress_done
        CHECK_PASSED=$((CHECK_PASSED + 1))
        return 0
    else
        if [ "$is_critical" = "yes" ]; then
            omr_progress_fail
            CHECK_FAILED=$((CHECK_FAILED + 1))
        else
            printf "${OMR_YELLOW}WARN${OMR_NC}\n"
            CHECK_WARNINGS=$((CHECK_WARNINGS + 1))
        fi
        return 1
    fi
}

check_dependencies() {
    omr_log_subheader "System Dependencies"

    check_item "git installed" "command -v git"
    check_item "curl installed" "command -v curl"
    check_item "patch installed" "command -v patch"
    check_item "sed installed" "command -v sed"
    check_item "make installed" "command -v make"
    check_item "gcc installed" "command -v gcc"
    check_item "g++ installed" "command -v g++"
    check_item "python3 installed" "command -v python3"

    # Optional but recommended tools
    check_item "wget installed" "command -v wget" "no"
    check_item "rsync installed" "command -v rsync" "no"
    check_item "tar installed" "command -v tar" "no"
    check_item "gzip installed" "command -v gzip" "no"

    echo ""
}

check_disk_space() {
    omr_log_subheader "Disk Space"

    local available_kb
    available_kb=$(df "$REPO_ROOT" | tail -1 | awk '{print $4}')
    local available_gb=$((available_kb / 1024 / 1024))
    local used_kb
    used_kb=$(df "$REPO_ROOT" | tail -1 | awk '{print $3}')
    local used_gb=$((used_kb / 1024 / 1024))

    omr_progress_start "Checking available space (30GB minimum required)"
    if [ "$available_kb" -ge $((30 * 1024 * 1024)) ]; then
        omr_progress_done
        CHECK_PASSED=$((CHECK_PASSED + 1))
    else
        omr_progress_fail
        CHECK_FAILED=$((CHECK_FAILED + 1))
    fi

    omr_log_info "Available: ${available_gb}GB, Used: ${used_gb}GB"
    echo ""
}

check_git_status() {
    omr_log_subheader "Git Repository"

    check_item "Repository is a git repo" "test -d '$REPO_ROOT/.git'"
    check_item "Git config is valid" "cd '$REPO_ROOT' && git config --get user.email" "no"

    if [ -d "$REPO_ROOT/.git" ]; then
        local branch
        branch=$(cd "$REPO_ROOT" && git branch --show-current 2>/dev/null || echo "unknown")
        local commit
        commit=$(cd "$REPO_ROOT" && git rev-parse --short HEAD 2>/dev/null || echo "unknown")
        local dirty
        if cd "$REPO_ROOT" && git diff-index --quiet HEAD -- 2>/dev/null; then
            dirty="clean"
        else
            dirty="modified"
        fi

        omr_log_info "Branch: $branch"
        omr_log_info "Commit: $commit"
        omr_log_info "Status: $dirty"
    fi

    echo ""
}

check_build_environment() {
    omr_log_subheader "Build Environment"

    check_item "build.sh exists" "test -f '$REPO_ROOT/build.sh'"
    check_item "build.sh is executable" "test -x '$REPO_ROOT/build.sh'"
    check_item "config file exists" "test -f '$REPO_ROOT/config'"

    # Check for at least some config-* files
    local config_count
    config_count=$(find "$REPO_ROOT" -maxdepth 1 -name "config-*" -type f | wc -l)
    check_item "Platform configs exist ($config_count found)" "test $config_count -ge 10"

    echo ""
}

check_kernel_directories() {
    omr_log_subheader "Kernel Directories"

    check_item "Kernel 5.4 directory" "test -d '$REPO_ROOT/5.4'"
    check_item "Kernel 6.1 directory" "test -d '$REPO_ROOT/6.1'"
    check_item "Kernel 6.6 directory" "test -d '$REPO_ROOT/6.6'"
    check_item "Kernel 6.12 directory" "test -d '$REPO_ROOT/6.12'"
    check_item "Common directory" "test -d '$REPO_ROOT/common'"

    echo ""
}

check_scripts() {
    omr_log_subheader "Scripts and Tools"

    check_item "Scripts directory exists" "test -d '$REPO_ROOT/scripts'"
    check_item "VPS scripts exist" "test -d '$REPO_ROOT/vps-scripts'" "no"

    # Check our new QoL scripts
    if [ -f "$REPO_ROOT/scripts/omr-lib.sh" ]; then
        check_item "omr-lib.sh exists" "test -f '$REPO_ROOT/scripts/omr-lib.sh'" "no"
        check_item "omr-config-manager.sh exists" "test -f '$REPO_ROOT/scripts/omr-config-manager.sh'" "no"
        check_item "omr-select-platform.sh exists" "test -f '$REPO_ROOT/scripts/omr-select-platform.sh'" "no"
        check_item "omr-build-helper.sh exists" "test -f '$REPO_ROOT/scripts/omr-build-helper.sh'" "no"
    fi

    echo ""
}

check_documentation() {
    omr_log_subheader "Documentation"

    check_item "README.md exists" "test -f '$REPO_ROOT/README.md'" "no"
    check_item "LICENSE exists" "test -f '$REPO_ROOT/LICENSE'" "no"
    check_item "FAQ.md exists" "test -f '$REPO_ROOT/FAQ.md'" "no"
    check_item "CONTRIBUTING.md exists" "test -f '$REPO_ROOT/CONTRIBUTING.md'" "no"

    echo ""
}

check_permissions() {
    omr_log_subheader "File Permissions"

    # Check if scripts are executable
    local script_issues=0

    for script in "$REPO_ROOT"/scripts/*.sh; do
        if [ -f "$script" ] && [ ! -x "$script" ]; then
            script_issues=$((script_issues + 1))
        fi
    done

    check_item "Scripts have execute permissions" "test $script_issues -eq 0" "no"

    if [ $script_issues -gt 0 ]; then
        omr_log_warning "$script_issues script(s) missing execute permission"
    fi

    echo ""
}

fix_common_issues() {
    omr_log_header "Attempting to Fix Common Issues"

    # Fix script permissions
    omr_progress_start "Setting execute permissions on scripts"
    if find "$REPO_ROOT/scripts" -type f -name "*.sh" -exec chmod +x {} \; 2>/dev/null; then
        omr_progress_done
    else
        omr_progress_fail
    fi

    # Fix build.sh permission
    omr_progress_start "Setting execute permission on build.sh"
    if chmod +x "$REPO_ROOT/build.sh" 2>/dev/null; then
        omr_progress_done
    else
        omr_progress_fail
    fi

    echo ""
    omr_log_info "Re-run health check to verify fixes"
}

print_summary() {
    omr_log_header "Health Check Summary"

    local total=$((CHECK_PASSED + CHECK_WARNINGS + CHECK_FAILED))

    printf "  ${OMR_GREEN}Passed:${OMR_NC}   %d\n" "$CHECK_PASSED"
    printf "  ${OMR_YELLOW}Warnings:${OMR_NC} %d\n" "$CHECK_WARNINGS"
    printf "  ${OMR_RED}Failed:${OMR_NC}   %d\n" "$CHECK_FAILED"
    printf "  ${OMR_DIM}Total:${OMR_NC}    %d\n" "$total"
    echo ""

    if [ $CHECK_FAILED -eq 0 ]; then
        if [ $CHECK_WARNINGS -eq 0 ]; then
            omr_log_success "All checks passed! Your build environment is ready."
        else
            printf "${OMR_YELLOW}%s${OMR_NC} " "⚠️"
            echo "System is functional but has warnings."
        fi
    else
        omr_log_error "Critical issues found. Please fix them before building."
        omr_log_info "Run with --fix to attempt automatic fixes"
        exit 1
    fi

    echo ""
}

# Parse command line arguments
CHECK_MODE="full"
FIX_MODE=0

while [ $# -gt 0 ]; do
    case "$1" in
        --quick)
            CHECK_MODE="quick"
            shift
            ;;
        --full)
            CHECK_MODE="full"
            shift
            ;;
        --fix)
            FIX_MODE=1
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

# Handle fix mode
if [ $FIX_MODE -eq 1 ]; then
    fix_common_issues
    exit 0
fi

# Run health checks
omr_log_header "OpenMPTCProuter Health Check"
omr_log_info "Checking build environment..."
echo ""

check_dependencies
check_disk_space

if [ "$CHECK_MODE" = "full" ]; then
    check_git_status
    check_build_environment
    check_kernel_directories
    check_scripts
    check_documentation
    check_permissions
else
    check_build_environment
    check_kernel_directories
fi

print_summary
