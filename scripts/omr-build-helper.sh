#!/bin/sh
#
# OpenMPTCProuter Build Helper
# Wrapper around build.sh with progress tracking and build time measurement
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
BUILD_LOG_DIR="${REPO_ROOT}/.build-logs"
BUILD_STATS_FILE="${BUILD_LOG_DIR}/build-stats.log"

show_help() {
    cat <<EOF
OpenMPTCProuter Build Helper

${OMR_BOLD}USAGE:${OMR_NC}
    $0 [options]

${OMR_BOLD}OPTIONS:${OMR_NC}
    ${OMR_CYAN}--target <platform>${OMR_NC}   Set target platform (e.g., x86_64, bpi-r4)
    ${OMR_CYAN}--kernel <version>${OMR_NC}    Set kernel version (5.4, 6.1, 6.6, 6.12)
    ${OMR_CYAN}--interactive${OMR_NC}         Launch interactive platform selection
    ${OMR_CYAN}--log${OMR_NC}                 Save build output to log file
    ${OMR_CYAN}--stats${OMR_NC}               Show build statistics
    ${OMR_CYAN}--help${OMR_NC}                Show this help message

${OMR_BOLD}EXAMPLES:${OMR_NC}
    # Interactive platform selection
    $0 --interactive

    # Build specific target
    $0 --target x86_64 --kernel 6.12

    # Build with logging
    $0 --target bpi-r4 --kernel 6.12 --log

    # View build statistics
    $0 --stats

${OMR_BOLD}FEATURES:${OMR_NC}
    • Build time tracking
    • Progress indicators
    • Build statistics and history
    • Automatic log management
    • Pre-build validation

EOF
}

ensure_log_dir() {
    if [ ! -d "$BUILD_LOG_DIR" ]; then
        mkdir -p "$BUILD_LOG_DIR"
    fi
}

record_build_stat() {
    local target="$1"
    local kernel="$2"
    local duration="$3"
    local status="$4"

    ensure_log_dir

    local timestamp
    timestamp=$(omr_get_timestamp)

    echo "$timestamp | $target | $kernel | $duration | $status" >> "$BUILD_STATS_FILE"
}

show_build_stats() {
    ensure_log_dir

    if [ ! -f "$BUILD_STATS_FILE" ]; then
        omr_log_warning "No build statistics available yet"
        return
    fi

    omr_log_header "Build Statistics"

    printf "%-20s %-10s %-10s %-15s %-10s\n" "TIMESTAMP" "TARGET" "KERNEL" "DURATION" "STATUS"
    printf "%-20s %-10s %-10s %-15s %-10s\n" "---------" "------" "------" "--------" "------"

    tail -n 20 "$BUILD_STATS_FILE" | while IFS='|' read -r timestamp target kernel duration status; do
        timestamp=$(echo "$timestamp" | xargs)
        target=$(echo "$target" | xargs)
        kernel=$(echo "$kernel" | xargs)
        duration=$(echo "$duration" | xargs)
        status=$(echo "$status" | xargs)

        if [ "$status" = "SUCCESS" ]; then
            status_color="${OMR_GREEN}${status}${OMR_NC}"
        else
            status_color="${OMR_RED}${status}${OMR_NC}"
        fi

        printf "%-20s %-10s %-10s %-15s " "$timestamp" "$target" "$kernel" "$duration"
        printf "%b\n" "$status_color"
    done

    echo ""

    # Show summary stats
    local total_builds
    total_builds=$(wc -l < "$BUILD_STATS_FILE")
    local successful_builds
    successful_builds=$(grep -c "SUCCESS" "$BUILD_STATS_FILE" || echo "0")
    local failed_builds
    failed_builds=$(grep -c "FAILED" "$BUILD_STATS_FILE" || echo "0")

    omr_print_summary "Summary" \
        "Total Builds" "$total_builds" \
        "Successful" "$successful_builds" \
        "Failed" "$failed_builds"
}

pre_build_checks() {
    omr_log_header "Pre-Build Validation"

    local all_ok=1

    # Check disk space
    omr_progress_start "Checking disk space (30GB required)"
    local available_kb
    available_kb=$(df . | tail -1 | awk '{print $4}')
    local required_kb=$((30 * 1024 * 1024))
    if [ "$available_kb" -lt "$required_kb" ]; then
        omr_progress_fail
        omr_log_error "Insufficient disk space: $((available_kb / 1024 / 1024))GB available"
        all_ok=0
    else
        omr_progress_done
    fi

    # Check required tools
    for tool in git curl patch sed make gcc g++ python3; do
        omr_progress_start "Checking for $tool"
        if omr_validate_command "$tool"; then
            omr_progress_done
        else
            omr_progress_fail
            omr_log_error "Required tool not found: $tool"
            all_ok=0
        fi
    done

    # Check for config file
    if [ -n "${OMR_TARGET:-}" ]; then
        omr_progress_start "Validating config file: config-${OMR_TARGET}"
        if [ -f "$REPO_ROOT/config-${OMR_TARGET}" ]; then
            omr_progress_done
        else
            omr_progress_fail
            omr_log_error "Config file not found: config-${OMR_TARGET}"
            all_ok=0
        fi
    fi

    echo ""

    if [ $all_ok -eq 0 ]; then
        omr_log_error "Pre-build validation failed"
        exit 1
    fi

    omr_log_success "All pre-build checks passed"
    echo ""
}

run_build() {
    local log_build="${1:-no}"

    # Display build configuration
    omr_print_summary "Build Configuration" \
        "Target Platform" "${OMR_TARGET:-not set}" \
        "Kernel Version" "${OMR_KERNEL:-not set}" \
        "Build Directory" "$REPO_ROOT" \
        "Logging" "$log_build"

    # Run pre-build checks
    pre_build_checks

    # Prepare build
    local start_time
    start_time=$(date +%s)
    local log_file=""

    if [ "$log_build" = "yes" ]; then
        ensure_log_dir
        log_file="${BUILD_LOG_DIR}/build-$(omr_get_timestamp_compact)-${OMR_TARGET:-unknown}-${OMR_KERNEL:-unknown}.log"
        omr_log_info "Build output will be saved to: $log_file"
        echo ""
    fi

    omr_log_header "Starting Build"
    omr_log_info "Start time: $(omr_get_timestamp)"
    echo ""

    # Run the build
    local build_status="SUCCESS"
    if [ "$log_build" = "yes" ]; then
        if ! "$REPO_ROOT/build.sh" 2>&1 | tee "$log_file"; then
            build_status="FAILED"
        fi
    else
        if ! "$REPO_ROOT/build.sh"; then
            build_status="FAILED"
        fi
    fi

    # Calculate duration
    local end_time
    end_time=$(date +%s)
    local duration=$((end_time - start_time))
    local duration_formatted
    duration_formatted=$(omr_format_duration "$duration")

    echo ""
    omr_log_header "Build Complete"

    # Show build summary
    if [ "$build_status" = "SUCCESS" ]; then
        omr_log_success "Build completed successfully!"
    else
        omr_log_error "Build failed!"
    fi

    omr_print_summary "Build Summary" \
        "Status" "$build_status" \
        "Duration" "$duration_formatted" \
        "Start Time" "$(date -d "@$start_time" '+%Y-%m-%d %H:%M:%S' 2>/dev/null || date -r "$start_time" '+%Y-%m-%d %H:%M:%S')" \
        "End Time" "$(date -d "@$end_time" '+%Y-%m-%d %H:%M:%S' 2>/dev/null || date -r "$end_time" '+%Y-%m-%d %H:%M:%S')"

    if [ "$log_build" = "yes" ]; then
        omr_log_info "Build log: $log_file"
        echo ""
    fi

    # Record statistics
    record_build_stat "${OMR_TARGET:-unknown}" "${OMR_KERNEL:-unknown}" "$duration_formatted" "$build_status"

    if [ "$build_status" != "SUCCESS" ]; then
        exit 1
    fi
}

# Parse command line arguments
SHOW_STATS=0
LOG_BUILD=no
INTERACTIVE=0

while [ $# -gt 0 ]; do
    case "$1" in
        --target)
            export OMR_TARGET="$2"
            shift 2
            ;;
        --kernel)
            export OMR_KERNEL="$2"
            shift 2
            ;;
        --interactive)
            INTERACTIVE=1
            shift
            ;;
        --log)
            LOG_BUILD=yes
            shift
            ;;
        --stats)
            SHOW_STATS=1
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

# Handle stats display
if [ $SHOW_STATS -eq 1 ]; then
    show_build_stats
    exit 0
fi

# Handle interactive mode
if [ $INTERACTIVE -eq 1 ]; then
    exec "$SCRIPT_DIR/omr-select-platform.sh"
fi

# Run the build
run_build "$LOG_BUILD"
