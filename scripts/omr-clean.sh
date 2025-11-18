#!/bin/sh
#
# OpenMPTCProuter Build Cleanup Utility
# Clean build artifacts and free up disk space
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
OpenMPTCProuter Build Cleanup Utility

${OMR_BOLD}USAGE:${OMR_NC}
    $0 <mode> [options]

${OMR_BOLD}CLEANUP MODES:${OMR_NC}
    ${OMR_CYAN}light${OMR_NC}         Remove temporary files and caches (safe, ~1-5GB)
    ${OMR_CYAN}moderate${OMR_NC}      Remove build artifacts but keep downloads (~5-15GB)
    ${OMR_CYAN}deep${OMR_NC}          Remove everything including downloads (~15-30GB)
    ${OMR_CYAN}nuclear${OMR_NC}       Complete clean - like fresh checkout (30GB+)
    ${OMR_CYAN}analyze${OMR_NC}       Show disk usage without cleaning

${OMR_BOLD}OPTIONS:${OMR_NC}
    ${OMR_CYAN}--kernel <ver>${OMR_NC}  Only clean specific kernel directory (5.4, 6.1, 6.6, 6.12)
    ${OMR_CYAN}--force${OMR_NC}        Skip confirmation prompts
    ${OMR_CYAN}--dry-run${OMR_NC}      Show what would be deleted without deleting
    ${OMR_CYAN}--help${OMR_NC}         Show this help message

${OMR_BOLD}EXAMPLES:${OMR_NC}
    # Analyze disk usage
    $0 analyze

    # Light cleanup (safe)
    $0 light

    # Deep clean with confirmation
    $0 deep

    # Clean only kernel 6.12 build directory
    $0 moderate --kernel 6.12

    # Dry run to see what would be deleted
    $0 deep --dry-run

${OMR_BOLD}WHAT GETS CLEANED:${OMR_NC}

    ${OMR_BOLD}light:${OMR_NC}
        • tmp/ directories
        • .ccache/ caches
        • logs and temporary files

    ${OMR_BOLD}moderate:${OMR_NC}
        • Everything in light mode
        • build_dir/ (compiled objects)
        • staging_dir/ (staged files)
        • bin/ (built images - keep downloads)

    ${OMR_BOLD}deep:${OMR_NC}
        • Everything in moderate mode
        • dl/ (downloaded packages)
        • feeds/ (feed repositories)

    ${OMR_BOLD}nuclear:${OMR_NC}
        • Everything - like fresh git clone
        • All kernel build directories
        • All downloaded sources
        • Build logs and caches

${OMR_BOLD}IMPORTANT:${OMR_NC}
    • Always backup important data first
    • Build configurations (config-*) are never deleted
    • Use 'analyze' mode to check before cleaning

EOF
}

get_dir_size() {
    local dir="$1"
    if [ -d "$dir" ]; then
        du -sb "$dir" 2>/dev/null | awk '{print $1}'
    else
        echo "0"
    fi
}

format_size() {
    local bytes="$1"
    local gb=$((bytes / 1024 / 1024 / 1024))
    local mb=$((bytes / 1024 / 1024))

    if [ $gb -gt 0 ]; then
        echo "${gb}GB"
    elif [ $mb -gt 0 ]; then
        echo "${mb}MB"
    else
        echo "${bytes}B"
    fi
}

analyze_disk_usage() {
    omr_log_header "Disk Usage Analysis"

    local total_size=0

    # Analyze each kernel directory
    for kernel_dir in "$REPO_ROOT"/*/; do
        if [ -d "$kernel_dir" ] && [ -f "${kernel_dir}Makefile" ]; then
            local kernel_name
            kernel_name=$(basename "$kernel_dir")

            omr_log_subheader "Kernel $kernel_name"

            local dl_size
            dl_size=$(get_dir_size "${kernel_dir}dl")
            local build_size
            build_size=$(get_dir_size "${kernel_dir}build_dir")
            local staging_size
            staging_size=$(get_dir_size "${kernel_dir}staging_dir")
            local bin_size
            bin_size=$(get_dir_size "${kernel_dir}bin")
            local tmp_size
            tmp_size=$(get_dir_size "${kernel_dir}tmp")
            local feeds_size
            feeds_size=$(get_dir_size "${kernel_dir}feeds")

            if [ -d "${kernel_dir}dl" ]; then
                printf "  dl/ (downloads):      %10s\n" "$(format_size "$dl_size")"
            fi
            if [ -d "${kernel_dir}build_dir" ]; then
                printf "  build_dir/:           %10s\n" "$(format_size "$build_size")"
            fi
            if [ -d "${kernel_dir}staging_dir" ]; then
                printf "  staging_dir/:         %10s\n" "$(format_size "$staging_size")"
            fi
            if [ -d "${kernel_dir}bin" ]; then
                printf "  bin/ (images):        %10s\n" "$(format_size "$bin_size")"
            fi
            if [ -d "${kernel_dir}tmp" ]; then
                printf "  tmp/:                 %10s\n" "$(format_size "$tmp_size")"
            fi
            if [ -d "${kernel_dir}feeds" ]; then
                printf "  feeds/:               %10s\n" "$(format_size "$feeds_size")"
            fi

            local kernel_total=$((dl_size + build_size + staging_size + bin_size + tmp_size + feeds_size))
            printf "  ${OMR_BOLD}Total for %s:${OMR_NC}      %10s\n\n" "$kernel_name" "$(format_size "$kernel_total")"

            total_size=$((total_size + kernel_total))
        fi
    done

    # Check for other directories
    local backup_size
    backup_size=$(get_dir_size "$REPO_ROOT/.config-backups")
    local log_size
    log_size=$(get_dir_size "$REPO_ROOT/.build-logs")

    if [ "$backup_size" != "0" ] || [ "$log_size" != "0" ]; then
        omr_log_subheader "Other Directories"
        if [ "$backup_size" != "0" ]; then
            printf "  .config-backups/:     %10s\n" "$(format_size "$backup_size")"
            total_size=$((total_size + backup_size))
        fi
        if [ "$log_size" != "0" ]; then
            printf "  .build-logs/:         %10s\n" "$(format_size "$log_size")"
            total_size=$((total_size + log_size))
        fi
        echo ""
    fi

    omr_print_summary "Total Cleanable Space" \
        "Total Size" "$(format_size "$total_size")"
}

clean_light() {
    local kernel_filter="$1"

    omr_log_subheader "Light Cleanup"
    omr_log_info "Removing temporary files and caches..."
    echo ""

    for kernel_dir in "$REPO_ROOT"/*/; do
        if [ -d "$kernel_dir" ] && [ -f "${kernel_dir}Makefile" ]; then
            local kernel_name
            kernel_name=$(basename "$kernel_dir")

            # Skip if kernel filter is set and doesn't match
            if [ -n "$kernel_filter" ] && [ "$kernel_name" != "$kernel_filter" ]; then
                continue
            fi

            omr_progress_start "Cleaning $kernel_name/tmp"
            rm -rf "${kernel_dir}tmp" 2>/dev/null || true
            omr_progress_done

            omr_progress_start "Cleaning $kernel_name/.ccache"
            rm -rf "${kernel_dir}.ccache" 2>/dev/null || true
            omr_progress_done
        fi
    done

    # Clean build logs (keep last 10)
    if [ -d "$REPO_ROOT/.build-logs" ]; then
        omr_progress_start "Cleaning old build logs (keeping last 10)"
        cd "$REPO_ROOT/.build-logs"
        ls -t build-*.log 2>/dev/null | tail -n +11 | xargs rm -f 2>/dev/null || true
        omr_progress_done
    fi

    echo ""
}

clean_moderate() {
    local kernel_filter="$1"

    clean_light "$kernel_filter"

    omr_log_subheader "Moderate Cleanup"
    omr_log_info "Removing build artifacts..."
    echo ""

    for kernel_dir in "$REPO_ROOT"/*/; do
        if [ -d "$kernel_dir" ] && [ -f "${kernel_dir}Makefile" ]; then
            local kernel_name
            kernel_name=$(basename "$kernel_dir")

            if [ -n "$kernel_filter" ] && [ "$kernel_name" != "$kernel_filter" ]; then
                continue
            fi

            omr_progress_start "Cleaning $kernel_name/build_dir"
            rm -rf "${kernel_dir}build_dir" 2>/dev/null || true
            omr_progress_done

            omr_progress_start "Cleaning $kernel_name/staging_dir"
            rm -rf "${kernel_dir}staging_dir" 2>/dev/null || true
            omr_progress_done

            omr_progress_start "Cleaning $kernel_name/bin"
            rm -rf "${kernel_dir}bin" 2>/dev/null || true
            omr_progress_done
        fi
    done

    echo ""
}

clean_deep() {
    local kernel_filter="$1"

    clean_moderate "$kernel_filter"

    omr_log_subheader "Deep Cleanup"
    omr_log_info "Removing downloads and feeds..."
    echo ""

    for kernel_dir in "$REPO_ROOT"/*/; do
        if [ -d "$kernel_dir" ] && [ -f "${kernel_dir}Makefile" ]; then
            local kernel_name
            kernel_name=$(basename "$kernel_dir")

            if [ -n "$kernel_filter" ] && [ "$kernel_name" != "$kernel_filter" ]; then
                continue
            fi

            omr_progress_start "Cleaning $kernel_name/dl"
            rm -rf "${kernel_dir}dl" 2>/dev/null || true
            omr_progress_done

            omr_progress_start "Cleaning $kernel_name/feeds"
            rm -rf "${kernel_dir}feeds" 2>/dev/null || true
            omr_progress_done
        fi
    done

    echo ""
}

clean_nuclear() {
    omr_log_subheader "Nuclear Cleanup"
    omr_log_warning "This will remove EVERYTHING except source code and configs!"
    echo ""

    if ! omr_ask_yes_no "Are you ABSOLUTELY sure? This cannot be undone!" "n"; then
        omr_log_info "Cleanup cancelled"
        exit 0
    fi

    echo ""

    # Clean all kernel directories completely
    for kernel_dir in "$REPO_ROOT"/*/; do
        if [ -d "$kernel_dir" ] && [ -f "${kernel_dir}Makefile" ]; then
            local kernel_name
            kernel_name=$(basename "$kernel_dir")

            omr_progress_start "Nuking $kernel_name (keeping source files)"
            # Remove everything except the source directories from the repo
            cd "$kernel_dir"
            for item in *; do
                if [ "$item" != "package" ] && [ "$item" != "target" ] && [ "$item" != "Makefile" ]; then
                    rm -rf "$item" 2>/dev/null || true
                fi
            done
            omr_progress_done
        fi
    done

    # Clean all cached data
    omr_progress_start "Cleaning build logs"
    rm -rf "$REPO_ROOT/.build-logs" 2>/dev/null || true
    omr_progress_done

    omr_progress_start "Cleaning .ccache"
    find "$REPO_ROOT" -name ".ccache" -type d -exec rm -rf {} + 2>/dev/null || true
    omr_progress_done

    echo ""
}

# Parse command line arguments
MODE="${1:-}"
KERNEL_FILTER=""
FORCE=0
DRY_RUN=0

shift || true

while [ $# -gt 0 ]; do
    case "$1" in
        --kernel)
            KERNEL_FILTER="$2"
            shift 2
            ;;
        --force)
            FORCE=1
            shift
            ;;
        --dry-run)
            DRY_RUN=1
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

# Validate kernel filter if set
if [ -n "$KERNEL_FILTER" ]; then
    case "$KERNEL_FILTER" in
        5.4|6.1|6.6|6.12)
            ;;
        *)
            omr_log_error "Invalid kernel version: $KERNEL_FILTER"
            omr_log_info "Valid versions: 5.4, 6.1, 6.6, 6.12"
            exit 1
            ;;
    esac
fi

# Handle analyze mode
if [ "$MODE" = "analyze" ]; then
    analyze_disk_usage
    exit 0
fi

# Handle help mode
case "$MODE" in
    help|--help|-h)
        show_help
        exit 0
        ;;
esac

# Validate mode
case "$MODE" in
    light|moderate|deep|nuclear)
        ;;
    "")
        omr_log_error "No cleanup mode specified"
        show_help
        exit 1
        ;;
    *)
        omr_log_error "Unknown cleanup mode: $MODE"
        show_help
        exit 1
        ;;
esac

# Show what will be cleaned
omr_log_header "OpenMPTCProuter Cleanup - $MODE mode"

if [ $DRY_RUN -eq 1 ]; then
    omr_log_warning "DRY RUN - No files will actually be deleted"
    echo ""
fi

if [ -n "$KERNEL_FILTER" ]; then
    omr_log_info "Target: Kernel $KERNEL_FILTER only"
else
    omr_log_info "Target: All kernel directories"
fi
echo ""

# Confirm unless forced
if [ $FORCE -eq 0 ] && [ $DRY_RUN -eq 0 ]; then
    if ! omr_ask_yes_no "Proceed with cleanup?" "n"; then
        omr_log_info "Cleanup cancelled"
        exit 0
    fi
    echo ""
fi

# Run cleanup based on mode
if [ $DRY_RUN -eq 0 ]; then
    case "$MODE" in
        light)
            clean_light "$KERNEL_FILTER"
            ;;
        moderate)
            clean_moderate "$KERNEL_FILTER"
            ;;
        deep)
            clean_deep "$KERNEL_FILTER"
            ;;
        nuclear)
            clean_nuclear
            ;;
    esac

    omr_log_success "Cleanup complete!"
    echo ""
    omr_log_info "Run '$0 analyze' to see current disk usage"
else
    omr_log_info "Dry run complete - no files were deleted"
fi

echo ""
