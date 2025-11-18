#!/bin/sh
#
# OpenMPTCProuter Configuration Manager
# Backup, restore, and manage build configurations
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
BACKUP_DIR="${REPO_ROOT}/.config-backups"

show_help() {
    cat <<EOF
OpenMPTCProuter Configuration Manager

${OMR_BOLD}USAGE:${OMR_NC}
    $0 <command> [options]

${OMR_BOLD}COMMANDS:${OMR_NC}
    ${OMR_CYAN}backup${OMR_NC}              Create a backup of current configuration
    ${OMR_CYAN}restore${OMR_NC} <name>      Restore a configuration from backup
    ${OMR_CYAN}list${OMR_NC}                List all available backups
    ${OMR_CYAN}delete${OMR_NC} <name>       Delete a backup
    ${OMR_CYAN}diff${OMR_NC} <name>         Compare current config with a backup
    ${OMR_CYAN}export${OMR_NC} <name>       Export backup to tar.gz file
    ${OMR_CYAN}import${OMR_NC} <file>       Import backup from tar.gz file
    ${OMR_CYAN}help${OMR_NC}                Show this help message

${OMR_BOLD}EXAMPLES:${OMR_NC}
    # Create a backup before making changes
    $0 backup

    # List all backups
    $0 list

    # Restore a specific backup
    $0 restore 20250118_143022

    # Compare current config with a backup
    $0 diff 20250118_143022

    # Export backup to share or archive
    $0 export 20250118_143022

${OMR_BOLD}WHAT GETS BACKED UP:${OMR_NC}
    • All config-* files (platform configurations)
    • .config file (if exists in build directory)
    • feeds.conf (if customized)
    • Custom patches in patches/ directory

EOF
}

ensure_backup_dir() {
    if [ ! -d "$BACKUP_DIR" ]; then
        mkdir -p "$BACKUP_DIR"
        omr_log_info "Created backup directory: $BACKUP_DIR"
    fi
}

backup_config() {
    ensure_backup_dir

    local timestamp
    timestamp="$(omr_get_timestamp_compact)"
    local backup_name="$timestamp"
    local backup_path="${BACKUP_DIR}/${backup_name}"

    omr_log_header "Creating Configuration Backup"

    mkdir -p "$backup_path"

    # Backup all config files
    omr_progress_start "Backing up config-* files"
    find "$REPO_ROOT" -maxdepth 1 -name "config-*" -type f -exec cp {} "$backup_path/" \;
    cp "$REPO_ROOT/config" "$backup_path/" 2>/dev/null || true
    omr_progress_done

    # Backup .config if exists in any kernel directory
    omr_progress_start "Backing up .config files from build directories"
    for kernel_dir in "$REPO_ROOT"/*/; do
        if [ -f "${kernel_dir}.config" ]; then
            kernel_name=$(basename "$kernel_dir")
            cp "${kernel_dir}.config" "$backup_path/.config-${kernel_name}" 2>/dev/null || true
        fi
    done
    omr_progress_done

    # Backup feeds.conf if customized
    if [ -f "$REPO_ROOT/feeds.conf" ] || [ -f "$REPO_ROOT/feeds.conf.default" ]; then
        omr_progress_start "Backing up feeds configuration"
        cp "$REPO_ROOT/feeds.conf" "$backup_path/" 2>/dev/null || true
        cp "$REPO_ROOT/feeds.conf.default" "$backup_path/" 2>/dev/null || true
        omr_progress_done
    fi

    # Backup custom patches
    if [ -d "$REPO_ROOT/patches" ]; then
        omr_progress_start "Backing up custom patches"
        mkdir -p "$backup_path/patches"
        cp -r "$REPO_ROOT/patches/"* "$backup_path/patches/" 2>/dev/null || true
        omr_progress_done
    fi

    # Create metadata
    cat > "$backup_path/metadata.txt" <<METADATA
Backup created: $(omr_get_timestamp)
Hostname: $(hostname)
User: $(whoami)
Git branch: $(cd "$REPO_ROOT" && git branch --show-current 2>/dev/null || echo "unknown")
Git commit: $(cd "$REPO_ROOT" && git rev-parse --short HEAD 2>/dev/null || echo "unknown")
METADATA

    omr_log_success "Backup created: ${OMR_BOLD}${backup_name}${OMR_NC}"
    omr_log_info "Location: ${backup_path}"
    echo ""
}

list_backups() {
    ensure_backup_dir

    omr_log_header "Available Configuration Backups"

    if [ ! "$(ls -A "$BACKUP_DIR" 2>/dev/null)" ]; then
        omr_log_warning "No backups found"
        echo ""
        return
    fi

    printf "%-20s %-20s %-30s\n" "NAME" "CREATED" "FILES"
    printf "%-20s %-20s %-30s\n" "----" "-------" "-----"

    for backup in "$BACKUP_DIR"/*; do
        if [ -d "$backup" ]; then
            local name
            name=$(basename "$backup")
            local file_count
            file_count=$(find "$backup" -type f | wc -l)
            local created="N/A"

            if [ -f "$backup/metadata.txt" ]; then
                created=$(grep "Backup created:" "$backup/metadata.txt" | cut -d: -f2- | xargs)
            fi

            printf "%-20s %-20s %-30s\n" "$name" "$created" "${file_count} files"
        fi
    done

    echo ""
}

restore_backup() {
    local backup_name="$1"

    if [ -z "$backup_name" ]; then
        omr_die "Please specify a backup name to restore"
    fi

    local backup_path="${BACKUP_DIR}/${backup_name}"

    if [ ! -d "$backup_path" ]; then
        omr_die "Backup not found: $backup_name"
    fi

    omr_log_header "Restoring Configuration Backup"

    # Show what will be restored
    if [ -f "$backup_path/metadata.txt" ]; then
        omr_log_info "Backup information:"
        cat "$backup_path/metadata.txt" | sed 's/^/  /'
        echo ""
    fi

    # Confirm before restoring
    if ! omr_ask_yes_no "Restore this backup? This will overwrite current configuration" "n"; then
        omr_log_warning "Restore cancelled"
        return 1
    fi

    # Create a safety backup of current state
    omr_log_info "Creating safety backup of current configuration..."
    local safety_backup="${BACKUP_DIR}/pre-restore-$(omr_get_timestamp_compact)"
    mkdir -p "$safety_backup"
    find "$REPO_ROOT" -maxdepth 1 -name "config-*" -type f -exec cp {} "$safety_backup/" \; 2>/dev/null || true
    cp "$REPO_ROOT/config" "$safety_backup/" 2>/dev/null || true

    # Restore config files
    omr_progress_start "Restoring config-* files"
    find "$backup_path" -maxdepth 1 -name "config*" -type f -exec cp {} "$REPO_ROOT/" \;
    omr_progress_done

    # Restore .config files to kernel directories
    omr_progress_start "Restoring .config files to build directories"
    for config_file in "$backup_path"/.config-*; do
        if [ -f "$config_file" ]; then
            kernel_name=$(basename "$config_file" | sed 's/\.config-//')
            if [ -d "$REPO_ROOT/$kernel_name" ]; then
                cp "$config_file" "$REPO_ROOT/$kernel_name/.config" 2>/dev/null || true
            fi
        fi
    done
    omr_progress_done

    # Restore feeds.conf if exists
    if [ -f "$backup_path/feeds.conf" ]; then
        omr_progress_start "Restoring feeds configuration"
        cp "$backup_path/feeds.conf" "$REPO_ROOT/" 2>/dev/null || true
        omr_progress_done
    fi

    # Restore patches if exists
    if [ -d "$backup_path/patches" ]; then
        omr_progress_start "Restoring custom patches"
        mkdir -p "$REPO_ROOT/patches"
        cp -r "$backup_path/patches/"* "$REPO_ROOT/patches/" 2>/dev/null || true
        omr_progress_done
    fi

    omr_log_success "Configuration restored from: ${backup_name}"
    omr_log_info "Safety backup saved to: ${safety_backup}"
    echo ""
}

delete_backup() {
    local backup_name="$1"

    if [ -z "$backup_name" ]; then
        omr_die "Please specify a backup name to delete"
    fi

    local backup_path="${BACKUP_DIR}/${backup_name}"

    if [ ! -d "$backup_path" ]; then
        omr_die "Backup not found: $backup_name"
    fi

    # Confirm deletion
    if ! omr_ask_yes_no "Delete backup '${backup_name}'?" "n"; then
        omr_log_warning "Deletion cancelled"
        return 1
    fi

    rm -rf "$backup_path"
    omr_log_success "Backup deleted: ${backup_name}"
}

diff_backup() {
    local backup_name="$1"

    if [ -z "$backup_name" ]; then
        omr_die "Please specify a backup name to compare"
    fi

    local backup_path="${BACKUP_DIR}/${backup_name}"

    if [ ! -d "$backup_path" ]; then
        omr_die "Backup not found: $backup_name"
    fi

    omr_log_header "Comparing Configuration with Backup: ${backup_name}"

    local has_diff=0

    # Compare config files
    for backup_file in "$backup_path"/config*; do
        if [ -f "$backup_file" ]; then
            local filename
            filename=$(basename "$backup_file")
            local current_file="$REPO_ROOT/$filename"

            if [ -f "$current_file" ]; then
                if ! diff -q "$backup_file" "$current_file" >/dev/null 2>&1; then
                    omr_log_warning "Differences in: ${filename}"
                    diff -u "$backup_file" "$current_file" | head -n 50 || true
                    has_diff=1
                    echo ""
                fi
            else
                omr_log_warning "File exists in backup but not in current: ${filename}"
                has_diff=1
            fi
        fi
    done

    if [ $has_diff -eq 0 ]; then
        omr_log_success "No differences found"
    fi

    echo ""
}

export_backup() {
    local backup_name="$1"

    if [ -z "$backup_name" ]; then
        omr_die "Please specify a backup name to export"
    fi

    local backup_path="${BACKUP_DIR}/${backup_name}"

    if [ ! -d "$backup_path" ]; then
        omr_die "Backup not found: $backup_name"
    fi

    local export_file="${REPO_ROOT}/omr-config-${backup_name}.tar.gz"

    omr_progress_start "Exporting backup to ${export_file}"
    tar -czf "$export_file" -C "$BACKUP_DIR" "$backup_name"
    omr_progress_done

    omr_log_success "Backup exported to: ${export_file}"
    local size
    size=$(stat -f%z "$export_file" 2>/dev/null || stat -c%s "$export_file" 2>/dev/null)
    omr_log_info "Size: $(omr_format_bytes "$size")"
    echo ""
}

import_backup() {
    local import_file="$1"

    if [ -z "$import_file" ]; then
        omr_die "Please specify a tar.gz file to import"
    fi

    if [ ! -f "$import_file" ]; then
        omr_die "File not found: $import_file"
    fi

    ensure_backup_dir

    omr_progress_start "Importing backup from ${import_file}"
    tar -xzf "$import_file" -C "$BACKUP_DIR"
    omr_progress_done

    omr_log_success "Backup imported successfully"
    list_backups
}

# Main command dispatcher
case "${1:-}" in
    backup)
        backup_config
        ;;
    restore)
        restore_backup "$2"
        ;;
    list|ls)
        list_backups
        ;;
    delete|rm)
        delete_backup "$2"
        ;;
    diff)
        diff_backup "$2"
        ;;
    export)
        export_backup "$2"
        ;;
    import)
        import_backup "$2"
        ;;
    help|--help|-h|"")
        show_help
        ;;
    *)
        omr_log_error "Unknown command: $1"
        echo ""
        show_help
        exit 1
        ;;
esac
