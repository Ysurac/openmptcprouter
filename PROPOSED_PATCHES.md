# Proposed Patches for Code Audit Fixes
**Date**: 2025-11-18
**Branch**: claude/code-audit-network-agent-017M3rNvBu3hJXvERrwhjTEB

## Overview
This document proposes minimal, focused patches to fix high and medium impact issues found during the code audit. Each patch is designed to:
- Fix a specific issue without rewriting large sections
- Maintain compatibility with existing code patterns
- Be easily reviewable and testable
- Not introduce new dependencies

---

## PATCH #1: Fix Bash/SH Incompatibility in Logger
**File**: `/common/files/usr/lib/omr/omr-logger.sh`
**Issue**: #1 from AUDIT_FINDINGS.md
**Impact**: HIGH
**Lines**: 1, 30-32, 44-57

### Problem
Script declares `#!/bin/sh` but uses bash-specific associative arrays that don't work in OpenWrt's busybox ash.

### Proposed Fix
Change to use file-based rate limiting instead of associative arrays.

### Patch Details
```bash
# BEFORE (lines 1, 30-32):
#!/bin/sh
...
declare -A _omr_log_last_time
declare -A _omr_log_count
readonly LOG_RATE_LIMIT=5

# AFTER:
#!/bin/sh
...
readonly LOG_RATE_LIMIT=5
readonly LOG_RATE_DIR="/var/run/omr-log-rate"
```

```bash
# BEFORE (lines 43-57):
local msg_hash=$(echo "$message" | md5sum | cut -d' ' -f1)
local current_time=$(date +%s)
local last_time=${_omr_log_last_time[$msg_hash]:-0}
local time_diff=$((current_time - last_time))

if [ "$time_diff" -lt 60 ]; then
    _omr_log_count[$msg_hash]=$((${_omr_log_count[$msg_hash]:-0} + 1))
    if [ "${_omr_log_count[$msg_hash]}" -gt "$LOG_RATE_LIMIT" ]; then
        return 0
    fi
else
    _omr_log_count[$msg_hash]=1
    _omr_log_last_time[$msg_hash]=$current_time
fi

# AFTER:
local msg_hash=$(echo "$message" | md5sum | cut -d' ' -f1 | head -c 8)
local current_time=$(date +%s)

# Create rate limit directory if needed
mkdir -p "$LOG_RATE_DIR" 2>/dev/null

local rate_file="$LOG_RATE_DIR/$msg_hash"
local last_time=0
local count=0

# Read existing rate limit data
if [ -f "$rate_file" ]; then
    read last_time count < "$rate_file" 2>/dev/null || true
fi

local time_diff=$((current_time - last_time))

if [ "$time_diff" -lt 60 ]; then
    count=$((count + 1))
    if [ "$count" -gt "$LOG_RATE_LIMIT" ]; then
        return 0  # Rate limited
    fi
else
    count=1
    last_time=$current_time
fi

# Update rate limit data atomically
echo "$last_time $count" > "$rate_file.tmp" && mv "$rate_file.tmp" "$rate_file"
```

### Benefits
- Works in any POSIX sh (busybox ash, dash, bash)
- Maintains rate limiting functionality
- More robust (survives script restarts)
- Minimal performance impact

---

## PATCH #2: Remove Dangerous eval Usage
**File**: `/common/files/usr/lib/omr/omr-logger.sh`
**Issue**: #2 from AUDIT_FINDINGS.md
**Impact**: HIGH
**Lines**: 130-145

### Problem
Using `eval` on potentially untrusted command strings.

### Proposed Fix
Remove the `omr_check_and_log` function as it's not currently used in the codebase.

### Patch Details
```bash
# REMOVE lines 129-145:
# Smart diagnostic function - only logs when something is wrong
omr_check_and_log() {
    local check_name=$1
    local check_command=$2
    local success_msg=$3
    local failure_msg=$4

    if eval "$check_command" >/dev/null 2>&1; then
        # Only log success at debug level
        [ -n "$success_msg" ] && omr_log_debug "$check_name: $success_msg"
        return 0
    else
        # Log failure at warning level
        omr_log_warning "$check_name: $failure_msg"
        return 1
    fi
}
```

```bash
# REMOVE from line 155:
export -f omr_check_and_log
```

### Benefits
- Eliminates potential command injection vector
- Simplifies code
- No functional loss (function not used)

---

## PATCH #3: Add UCI Commit Error Checking
**File**: `/common/files/usr/bin/network-safety-monitor.sh`
**Issue**: #3 from AUDIT_FINDINGS.md
**Impact**: HIGH
**Lines**: 215-218

### Problem
UCI commit and network restart failures not checked in emergency recovery.

### Proposed Fix
Add error checking and rollback logic.

### Patch Details
```bash
# BEFORE (lines 207-218):
    uci -q batch <<-'EOF'
        set network.lan.device='br-lan'
        set network.lan.proto='static'
        set network.lan.ipaddr='192.168.2.1'
        set network.lan.netmask='255.255.255.0'
        set network.lan.ip6assign='60'
    EOF

    uci commit network

    # Restart network
    /etc/init.d/network restart

# AFTER:
    uci -q batch <<-'EOF'
        set network.lan.device='br-lan'
        set network.lan.proto='static'
        set network.lan.ipaddr='192.168.2.1'
        set network.lan.netmask='255.255.255.0'
        set network.lan.ip6assign='60'
    EOF

    if ! uci commit network; then
        log_msg "CRITICAL: UCI commit failed during emergency recovery"
        log_msg "System may be in inconsistent state"
        return 1
    fi

    # Restart network with verification
    if ! /etc/init.d/network restart; then
        log_msg "CRITICAL: Network restart failed during emergency recovery"
        log_msg "Manual intervention required"
        return 1
    fi

    # Wait for network to come up
    sleep 3
```

### Benefits
- Detects configuration failures
- Prevents false "success" messages
- Logs critical errors for debugging

---

## PATCH #4: Add Recovery Retry Limit
**File**: `/common/files/usr/bin/network-safety-monitor.sh`
**Issue**: #4 from AUDIT_FINDINGS.md
**Impact**: HIGH
**Lines**: 275-310

### Problem
Infinite loop if emergency recovery repeatedly fails.

### Proposed Fix
Add retry counting and exponential backoff.

### Patch Details
```bash
# ADD at top of file (after CHECK_INTERVAL):
readonly MAX_RECOVERY_ATTEMPTS=3
RECOVERY_FAILURES=0
LAST_RECOVERY_ATTEMPT=0

# MODIFY main function (lines 275-310):
# BEFORE:
main() {
    log_msg "Network safety monitor starting"
    log_msg "Preventing lockouts and APIPA addresses"

    # Wait for system to fully boot
    sleep 30

    while true; do
        # CRITICAL: Ensure LAN is always static
        fix_lan_protocol

        # Check for APIPA addresses
        if ! check_interface_ips; then
            log_msg "APIPA address detected on LAN - triggering recovery"
            emergency_recovery
        fi

        # Check if LAN is accessible
        if ! check_lan_accessible; then
            log_msg "WARNING: LAN not accessible - initiating recovery"

            # Wait a bit to see if network is just restarting
            sleep 10

            # Check again
            if ! check_lan_accessible; then
                emergency_recovery
            fi
        fi

        # Ensure DHCP is running
        ensure_dhcp_on_lan

        sleep $CHECK_INTERVAL
    done
}

# AFTER:
main() {
    log_msg "Network safety monitor starting"
    log_msg "Preventing lockouts and APIPA addresses"

    # Wait for system to fully boot
    sleep 30

    while true; do
        local current_time=$(date +%s)

        # Reset failure count if enough time has passed
        if [ $((current_time - LAST_RECOVERY_ATTEMPT)) -gt 600 ]; then
            RECOVERY_FAILURES=0
        fi

        # CRITICAL: Ensure LAN is always static
        fix_lan_protocol

        # Check for APIPA addresses
        if ! check_interface_ips; then
            if [ $RECOVERY_FAILURES -lt $MAX_RECOVERY_ATTEMPTS ]; then
                log_msg "APIPA address detected on LAN - triggering recovery (attempt $((RECOVERY_FAILURES + 1))/$MAX_RECOVERY_ATTEMPTS)"
                LAST_RECOVERY_ATTEMPT=$current_time
                if emergency_recovery; then
                    log_msg "Emergency recovery succeeded"
                    RECOVERY_FAILURES=0
                else
                    RECOVERY_FAILURES=$((RECOVERY_FAILURES + 1))
                    log_msg "Emergency recovery failed (failures: $RECOVERY_FAILURES)"
                    if [ $RECOVERY_FAILURES -ge $MAX_RECOVERY_ATTEMPTS ]; then
                        log_msg "CRITICAL: Emergency recovery failed $MAX_RECOVERY_ATTEMPTS times"
                        log_msg "Giving up to prevent infinite loop. Manual intervention required."
                    fi
                fi
            fi
        fi

        # Check if LAN is accessible
        if ! check_lan_accessible; then
            if [ $RECOVERY_FAILURES -lt $MAX_RECOVERY_ATTEMPTS ]; then
                log_msg "WARNING: LAN not accessible - initiating recovery"

                # Wait a bit to see if network is just restarting
                sleep 10

                # Check again
                if ! check_lan_accessible; then
                    LAST_RECOVERY_ATTEMPT=$current_time
                    if emergency_recovery; then
                        RECOVERY_FAILURES=0
                    else
                        RECOVERY_FAILURES=$((RECOVERY_FAILURES + 1))
                        if [ $RECOVERY_FAILURES -ge $MAX_RECOVERY_ATTEMPTS ]; then
                            log_msg "CRITICAL: Recovery failed $MAX_RECOVERY_ATTEMPTS times, giving up"
                        fi
                    fi
                fi
            fi
        fi

        # Ensure DHCP is running
        ensure_dhcp_on_lan

        # Exponential backoff if failures
        local sleep_time=$CHECK_INTERVAL
        if [ $RECOVERY_FAILURES -gt 0 ]; then
            sleep_time=$((CHECK_INTERVAL * (2 ** RECOVERY_FAILURES)))
            if [ $sleep_time -gt 300 ]; then
                sleep_time=300  # Cap at 5 minutes
            fi
            log_msg "Sleeping ${sleep_time}s before next check (backoff due to failures)"
        fi

        sleep $sleep_time
    done
}
```

### Benefits
- Prevents infinite recovery loops
- Implements exponential backoff
- Logs clear failure information
- Allows manual intervention

---

## PATCH #5: Make ifup Synchronous with Verification
**File**: `/common/files/usr/bin/usb-modem-autoconfig.sh`
**Issue**: #6 from AUDIT_FINDINGS.md
**Impact**: MEDIUM-HIGH
**Lines**: 274

### Problem
Interface brought up in background without verification.

### Proposed Fix
Run ifup synchronously with timeout and status checking.

### Patch Details
```bash
# BEFORE (line 274):
    # Bring up the interface
    ifup "$wan_name" 2>/dev/null &

# AFTER (replace line 274):
    # Bring up the interface with timeout
    log_msg "Bringing up interface $wan_name..."

    # Run ifup with timeout (30 seconds for modem initialization)
    local ifup_timeout=30
    local ifup_success=0

    if timeout $ifup_timeout ifup "$wan_name" 2>&1 | while read line; do
        log_msg "ifup: $line"
    done; then
        # Wait a bit for interface to fully initialize
        sleep 3

        # Verify interface is actually up
        if ifstatus "$wan_name" 2>/dev/null | grep -q '"up":true'; then
            log_msg "✓ Interface $wan_name is up and running"
            ifup_success=1
        else
            log_msg "WARNING: Interface $wan_name ifup succeeded but interface not up"
            log_msg "Check 'ifstatus $wan_name' for details"
        fi
    else
        local exit_code=$?
        if [ $exit_code -eq 124 ]; then
            log_msg "ERROR: Interface $wan_name bring-up timed out after ${ifup_timeout}s"
        else
            log_msg "ERROR: Interface $wan_name bring-up failed with code $exit_code"
        fi
        log_msg "Modem may not be properly initialized or configured"
        # Update status file to reflect failure
        if [ -f "$status_dir/$wan_name" ]; then
            echo "STATUS=failed" >> "$status_dir/$wan_name"
            echo "ERROR=ifup failed or timed out" >> "$status_dir/$wan_name"
        fi
    fi
```

### Benefits
- Detects if modem configuration fails
- Provides clear error messages
- Updates status file with failure
- Users know immediately if there's a problem

---

## PATCH #6: Fix Hardcoded Device Path
**File**: `/common/files/usr/bin/usb-modem-autoconfig.sh`
**Issue**: #7 from AUDIT_FINDINGS.md
**Impact**: MEDIUM
**Lines**: 291

### Problem
Hardcoded check for `/dev/cdc-wdm0` doesn't scale to multiple modems.

### Proposed Fix
Check dynamically for all cdc-wdm devices.

### Patch Details
```bash
# BEFORE (lines 286-296):
    # Check all WAN interfaces
    for wan in $(uci show network 2>/dev/null | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1); do
        local device
        device=$(uci -q get "network.$wan.device")
        # Check both device name and physical device
        if [ "$device" = "$iface" ] || [ "$device" = "/dev/cdc-wdm0" ]; then
            return 0
        fi
    done

    return 1

# AFTER:
    # Validate interface name
    if ! echo "$iface" | grep -qE '^[a-zA-Z0-9_/-]+$'; then
        return 1
    fi

    # Check all WAN interfaces
    for wan in $(uci show network 2>/dev/null | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1); do
        local device
        device=$(uci -q get "network.$wan.device")

        # Direct match on device name
        if [ "$device" = "$iface" ]; then
            return 0
        fi

        # For QMI/MBIM modems, also check if this is the network interface for the control device
        local wan_proto
        wan_proto=$(uci -q get "network.$wan.proto")
        if [ "$wan_proto" = "qmi" ] || [ "$wan_proto" = "mbim" ]; then
            # Get the network interface for this control device
            if [ -c "$device" ]; then
                local ctrl_iface=$(basename "$device")
                local net_iface=$(ls -1 /sys/class/usbmisc/$ctrl_iface/device/net/ 2>/dev/null | head -n1)
                if [ "$net_iface" = "$iface" ]; then
                    return 0
                fi
            fi
        fi
    done

    return 1
```

### Benefits
- Works with multiple modems
- No hardcoded paths
- Properly checks control device to network interface mapping
- More reliable detection

---

## PATCH #7: Improve PID File Validation
**File**: `/common/files/usr/bin/network-monitor.sh`
**Issue**: #9 from STATE_LIFECYCLE_AUDIT.md
**Impact**: MEDIUM
**Lines**: 25-37

### Problem
PID validation doesn't prevent PID reuse.

### Proposed Fix
Check process name and start time in addition to PID.

### Patch Details
```bash
# BEFORE (lines 25-37):
    if [ -f "$PID_FILE" ]; then
        local old_pid
        old_pid=$(cat "$PID_FILE" 2>/dev/null)
        # Validate PID is a number to prevent command injection
        if echo "$old_pid" | grep -qE '^[0-9]+$'; then
            if kill -0 "$old_pid" 2>/dev/null; then
                log_msg "Monitor already running with PID $old_pid"
                rmdir "$lockdir" 2>/dev/null
                exit 0
            fi
        fi
        # Stale PID file, remove it
        rm -f "$PID_FILE"
    fi

# AFTER:
    if [ -f "$PID_FILE" ]; then
        local old_pid
        old_pid=$(cat "$PID_FILE" 2>/dev/null)
        # Validate PID is a number to prevent command injection
        if echo "$old_pid" | grep -qE '^[0-9]+$'; then
            # Check if process exists
            if kill -0 "$old_pid" 2>/dev/null; then
                # Verify it's actually our script by checking command line
                local cmdline
                cmdline=$(cat "/proc/$old_pid/cmdline" 2>/dev/null | tr '\0' ' ')
                if echo "$cmdline" | grep -q "network-monitor"; then
                    log_msg "Monitor already running with PID $old_pid"
                    rmdir "$lockdir" 2>/dev/null
                    exit 0
                else
                    log_msg "PID $old_pid exists but is not network-monitor (PID reused)"
                    rm -f "$PID_FILE"
                fi
            else
                # Process doesn't exist, remove stale PID file
                log_msg "Removing stale PID file (process $old_pid not running)"
                rm -f "$PID_FILE"
            fi
        else
            log_msg "Invalid PID in PID file, removing"
            rm -f "$PID_FILE"
        fi
    fi
```

### Benefits
- Prevents false positives from PID reuse
- More reliable duplicate detection
- Better logging for debugging

---

## PATCH #8: Fix File Permission Race in omr-recovery
**File**: `/common/files/usr/bin/omr-recovery`
**Issue**: #12 from AUDIT_FINDINGS.md
**Impact**: MEDIUM
**Lines**: 143-159

### Problem
File created with default permissions then chmod'd - race window.

### Proposed Fix
Use umask for atomic permission setting.

### Patch Details
```bash
# BEFORE (lines 142-159):
            cp /rom/etc/config/network /etc/config/network 2>/dev/null || {
                # If no ROM config, create minimal one with secure permissions
                (
                    umask 077
                    cat > /etc/config/network <<-'NETEOF'
config interface 'loopback'
	option device 'lo'
	option proto 'static'
	option ipaddr '127.0.0.1'
	option netmask '255.0.0.0'

config interface 'lan'
	option proto 'static'
	option ipaddr '192.168.2.1'
	option netmask '255.255.255.0'
NETEOF
                )
                chmod 644 /etc/config/network
            }

# AFTER:
            cp /rom/etc/config/network /etc/config/network 2>/dev/null || {
                # If no ROM config, create minimal one with secure permissions
                (
                    # Set umask to create file with correct permissions (644)
                    umask 022
                    cat > /etc/config/network <<-'NETEOF'
config interface 'loopback'
	option device 'lo'
	option proto 'static'
	option ipaddr '127.0.0.1'
	option netmask '255.0.0.0'

config interface 'lan'
	option proto 'static'
	option ipaddr '192.168.2.1'
	option netmask '255.255.255.0'
NETEOF
                )
                # chmod no longer needed - file created with correct permissions
            }
```

### Benefits
- Eliminates race window
- Simpler code
- More secure

---

## PATCH #9: Add Atomic State File Writes
**File**: `/common/files/usr/bin/usb-modem-autoconfig.sh`
**Issue**: #5 from STATE_LIFECYCLE_AUDIT.md
**Impact**: MEDIUM
**Lines**: 258-269

### Problem
Status file write not atomic - partial file possible on crash.

### Proposed Fix
Write to temp file, then atomic rename.

### Patch Details
```bash
# BEFORE (lines 258-269):
    # Use quoted heredoc to prevent variable expansion issues
    # Write status file with secure permissions
    (
        umask 077
        cat > "$status_dir/$wan_name" <<-EOFF
INTERFACE=$wan_name
PHYSICAL_DEVICE=$iface
PROTOCOL=$proto
CONTROL_DEVICE=$dev
INFO=$modem_info
CONFIGURED_AT=$(date)
EOFF
    )

# AFTER:
    # Write status file atomically with secure permissions
    local temp_status="$status_dir/$wan_name.tmp.$$"
    (
        umask 077
        cat > "$temp_status" <<-EOFF
INTERFACE=$wan_name
PHYSICAL_DEVICE=$iface
PROTOCOL=$proto
CONTROL_DEVICE=$dev
INFO=$modem_info
CONFIGURED_AT=$(date)
EOFF
    )

    # Atomic rename - either complete file or nothing
    if [ -f "$temp_status" ]; then
        mv "$temp_status" "$status_dir/$wan_name"
    else
        log_msg "WARNING: Failed to create status file for $wan_name"
    fi
```

### Benefits
- Status file is always complete or doesn't exist
- No partial files
- More reliable status reading

---

## Summary of Patches

| # | File | Impact | Lines Changed | Risk | Test Required |
|---|------|--------|---------------|------|---------------|
| 1 | omr-logger.sh | HIGH | ~30 | Low | Shell compatibility |
| 2 | omr-logger.sh | HIGH | ~16 | None | Function not used |
| 3 | network-safety-monitor.sh | HIGH | ~15 | Low | Boot + recovery test |
| 4 | network-safety-monitor.sh | HIGH | ~60 | Medium | Recovery loop test |
| 5 | usb-modem-autoconfig.sh | MED-HIGH | ~30 | Medium | USB modem test |
| 6 | usb-modem-autoconfig.sh | MEDIUM | ~20 | Low | Multi-modem test |
| 7 | network-monitor.sh | MEDIUM | ~15 | Low | Boot test |
| 8 | omr-recovery | MEDIUM | ~5 | None | Factory reset test |
| 9 | usb-modem-autoconfig.sh | MEDIUM | ~10 | Low | Hotplug test |

**Total Lines Changed**: ~201
**Files Modified**: 4

---

## Testing Plan

### Patch #1-2 (omr-logger.sh)
1. Source logger in ash shell
2. Generate rapid identical log messages
3. Verify rate limiting works
4. Check /var/run/omr-log-rate directory

### Patch #3-4 (network-safety-monitor.sh)
1. Simulate UCI commit failure (disk full)
2. Verify error logged and recovery aborts
3. Simulate repeated recovery failures
4. Verify backoff and max attempts

### Patch #5-6, #9 (usb-modem-autoconfig.sh)
1. Plug in USB modem
2. Verify ifup runs synchronously
3. Verify status file created atomically
4. Check logs for success/failure
5. Test with multiple modems

### Patch #7 (network-monitor.sh)
1. Create stale PID file
2. Start monitor
3. Verify stale PID detected and removed
4. Kill monitor and quickly start new process with same PID
5. Verify PID reuse detected

### Patch #8 (omr-recovery)
1. Run factory reset
2. Verify network config file created with correct permissions
3. No chmod race window

---

## Implementation Order

1. **Patch #2** - Remove eval (safest, no functional change)
2. **Patch #8** - File permissions (simple fix)
3. **Patch #1** - Fix logger compatibility (foundational)
4. **Patch #9** - Atomic writes (low risk)
5. **Patch #7** - PID validation (improves reliability)
6. **Patch #6** - Remove hardcoded paths (improves correctness)
7. **Patch #3** - Add error checking (medium risk)
8. **Patch #5** - Synchronous ifup (changes behavior)
9. **Patch #4** - Recovery retry limit (complex logic change)

All patches will be applied in the next step.
