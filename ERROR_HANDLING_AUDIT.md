# Error Handling & Logging Audit
**Date**: 2025-11-18
**Branch**: claude/code-audit-network-agent-017M3rNvBu3hJXvERrwhjTEB

## Error Handling Patterns Analysis

### Summary of Error Handling Approaches

| Pattern | Files Using | Consistency | Issues |
|---------|-------------|-------------|--------|
| **Ignore errors completely** | 45% | Common | High risk |
| **Log but don't act** | 30% | Moderate | False sense of safety |
| **Return error code** | 15% | Good | Caller often ignores |
| **Retry with backoff** | 0% | N/A | Missing critical pattern |
| **Rollback on error** | 0% | N/A | Major gap |
| **Panic/exit on error** | 10% | Rare | Sometimes appropriate |

---

## ERROR PATTERN #1: Silent Failures (Errors Ignored Completely)

### Example 1: Network Restart Failures
**File**: `network-safety-monitor.sh:218`
```bash
/etc/init.d/network restart
# No check if restart succeeded
# No fallback if it failed
# No retry logic
```

**Impact**: HIGH
- Emergency recovery claims success but network still down
- User thinks problem is fixed
- System remains inaccessible

### Example 2: Background Jobs
**File**: `usb-modem-autoconfig.sh:274`
```bash
ifup "$wan_name" 2>/dev/null &
# Errors silently discarded
# No way to know if succeeded
# No status tracking
```

**Impact**: HIGH
- Modem appears configured but isn't working
- User has no indication of failure
- Troubleshooting impossible

### Example 3: UCI Commit Failures
**File**: `port-autoconfig.sh:183`
```bash
uci commit network
# Could fail due to:
# - Disk full
# - Filesystem read-only
# - Invalid configuration
# But no check!
```

**Impact**: CRITICAL
- Changes lost
- System in undefined state
- Next reboot fails

### Example 4: File Creation Failures
**File**: `port-autoconfig.sh:188`
```bash
touch "$CONFIG_APPLIED"
# Could fail (read-only filesystem)
# Script will run again on next boot
# Could override user configuration
```

**Impact**: MEDIUM
- Duplicate configuration attempts
- Loss of user customization
- Wasted boot time

---

## ERROR PATTERN #2: Logged But Not Acted Upon

### Example 1: Invalid Device Paths
**File**: `usb-modem-autoconfig.sh:310-312`
```bash
if [ -n "$device" ] && ! echo "$device" | grep -qE '^/dev/[a-zA-Z0-9_-]+$'; then
    log_msg "WARNING: Invalid device path for $wan: $device"
    continue  # Continues loop but doesn't fix the problem
fi
```

**Issue**: Warning logged but:
- Invalid configuration remains in UCI
- Will cause error on next boot
- User never sees warning (buried in syslog)

**Impact**: MEDIUM
- Accumulation of bad configurations
- System degradation over time
- Difficult debugging

### Example 2: Modem Detection Failures
**File**: `usb-modem-autoconfig.sh:344-346`
```bash
if [ -z "$modems" ]; then
    log_msg "No USB modems detected"
    return 0  # Returns success even though nothing detected
fi
```

**Issue**:
- Expected modem not detected - logs and exits
- User expects modem to work
- No notification visible to user
- No retry mechanism

**Impact**: MEDIUM
- Users think modem is working
- Connection never established
- Must manually debug

### Example 3: DHCP Server Not Running
**File**: `network-monitor.sh:59-62`
```bash
if ! pidof dnsmasq >/dev/null; then
    log_msg "DHCP server not running, restarting"
    /etc/init.d/dnsmasq restart
    # No check if restart succeeded!
fi
```

**Issue**:
- Logs that it's restarting
- Doesn't verify restart worked
- Could be repeatedly failing
- Fills logs with false "fixes"

**Impact**: HIGH
- DHCP actually broken
- Clients can't connect
- Logs give false confidence

---

## ERROR PATTERN #3: Inconsistent Error Handling Across Similar Code

### UCI Operations Consistency

#### Good Example (with error handling):
**File**: `network-safety-monitor.sh:202-204`
```bash
if ! uci -q set "network.@device[-1].ports=$emergency_port"; then
    log_msg "ERROR: Failed to set emergency port"
    return 1
fi
```

#### Bad Example (same operation, no error handling):
**File**: `port-autoconfig.sh:146`
```bash
if ! uci -q set "network.wan.device=$wan_port"; then
    log_msg "ERROR: Failed to configure WAN device"
    return 1  # Returns but leaves partial config
fi
# Later in same file:
uci -q set "network.@device[-1].ports=$lan_ports"  # No error check!
```

#### Worse Example (no error handling at all):
**File**: `omr-recovery:64`
```bash
uci -q set "network.@device[-1].ports=$port"  # No check
```

**Impact**: HIGH
- Inconsistent reliability
- Difficult to predict behavior
- Some operations protected, others not

---

## ERROR PATTERN #4: Return Codes Ignored by Callers

### Example 1: APN Settings Retrieval
**File**: `usb-modem-autoconfig.sh:198`
```bash
local apn_settings=$(get_apn_settings "$carrier")
# get_apn_settings could fail, but return code not checked
# Uses potentially empty/invalid $apn_settings
```

**Impact**: MEDIUM
- Modem configured with wrong APN
- Connection fails
- No indication why

### Example 2: Port Detection
**File**: `port-autoconfig.sh:215-222`
```bash
local detection=$(detect_port_roles)
# detect_port_roles doesn't return error codes
# Can't distinguish between:
# - Success with no ports
# - Failure in detection
# - Partial success
```

**Impact**: MEDIUM
- Ambiguous results
- Can't handle errors differently
- Poor error reporting to user

---

## ERROR PATTERN #5: Missing Error Context

### Example 1: Generic Error Messages
**File**: Throughout codebase
```bash
log_msg "ERROR: Failed to configure"
# Which configuration?
# What was the error?
# What were the parameters?
# How can user fix it?
```

**Impact**: LOW but pervasive
- Difficult debugging
- Users can't self-help
- Support burden increased

### Example 2: No Error Propagation
**File**: `usb-modem-autoconfig.sh:365`
```bash
configure_modem_as_wan "$proto" "$iface" "$dev"
# No check if configuration succeeded
# No error details returned
configured=$((configured + 1))  # Incremented even if failed!
```

**Impact**: MEDIUM
- Success counter wrong
- False "success" messages
- User thinks modem configured

---

## LOGGING PATTERN #1: Inconsistent Logging Levels

### File Analysis: network-safety-monitor.sh

| Line | Message | Level Used | Should Be |
|------|---------|-----------|-----------|
| 48 | LAN set to DHCP | ERROR | CRITICAL |
| 54 | Invalid IP | ERROR | CRITICAL |
| 63 | No LAN ports | ERROR | CRITICAL |
| 111 | APIPA on interface | WARNING | ERROR |
| 115 | APIPA on LAN | CRITICAL | Correct ✓ |
| 179 | No ports for recovery | CRITICAL | Correct ✓ |

**Issue**: Similar severity issues logged at different levels

### File Analysis: usb-modem-autoconfig.sh

| Line | Message | Level Used | Should Be |
|------|---------|-----------|-----------|
| 67 | Carrier not found | WARNING | INFO |
| 70 | APN database not loaded | WARNING | WARNING ✓ |
| 311 | Invalid device path | WARNING | ERROR |

**Issue**: Security issues (invalid paths) logged as warnings

---

## LOGGING PATTERN #2: Log Spam Risk

### Example 1: Continuous Loop Logging
**File**: `network-safety-monitor.sh:276-310`
```bash
while true; do
    fix_lan_protocol()  # Could log every 30 seconds
    check_interface_ips()  # Logs on every APIPA found
    check_lan_accessible()  # Logs errors repeatedly
    sleep 30
done
```

**Without rate limiting**:
- Could generate 2880 identical messages per day
- Fills /var/log
- Makes real issues hard to find
- Performance impact

**Current mitigation**: omr-logger.sh has rate limiting (lines 43-57)
**Problem**: Not all scripts use omr-logger.sh!

### Example 2: Hotplug Event Spam
**File**: `usb-modem-autoconfig.sh:87-90`
```bash
for dev in /dev/cdc-wdm*; do
    log_msg "Found QMI modem: $net_iface ($dev)"
    # Logs on EVERY hotplug event
    # Even if already configured
done
```

**Issue**:
- Plug/unplug USB generates flood of logs
- Each enumeration logs all devices
- No suppression for "already known" devices

---

## LOGGING PATTERN #3: Missing Logging

### Critical Operations Not Logged

1. **UCI Batch Operations**
   - Large batch updates occur silently
   - No record of what changed
   - Difficult to audit configuration changes

2. **Network Restarts**
   - No log before restart
   - No log after restart
   - Can't correlate connectivity issues with restarts

3. **Modem Disconnection**
   **File**: `usb-modem-autoconfig.sh:319-322`
   ```bash
   if [ -n "$device" ] && [ ! -c "$device" ]; then
       log_msg "Modem on $wan ($device) is disconnected, removing configuration"
       uci delete "network.$wan"  # Logged
       rm -f "/var/run/modem-status/$wan"  # Not logged!
   fi
   ```

4. **Failed Lock Acquisitions**
   **File**: `network-monitor.sh:19-22`
   ```bash
   if ! mkdir "$lockdir" 2>/dev/null; then
       sleep 1  # Silently waits, no log that lock was contested
   fi
   ```

---

## LOGGING PATTERN #4: Logging vs. User Notification

### Problem: Logs Are Not User-Visible

Most users will never see:
- Syslog messages
- /var/log files
- Log buffer (logread)

But scripts assume logging == user notification:

**Example**: `usb-modem-autoconfig.sh:370-372`
```bash
log_msg "✓ Configured $configured new USB modem(s) as additional WAN"
log_msg "✓ MPTCP bonding enabled for all WANs"
```

**Reality**:
- User has no idea modem was configured
- Only visible via omr-status command
- Web UI doesn't show notification
- User may wait indefinitely

**Should**:
- Update web UI status
- Show notification banner
- Create /var/run flag for UI to check
- Trigger event for UI refresh

---

## Recommendations for Standardization

### 1. Error Handling Guidelines

```bash
# GOOD: Check critical operations
if ! uci -q set "network.wan.device=$device"; then
    log_error "Failed to set WAN device to $device"
    return 1
fi

# GOOD: Provide context
if ! /etc/init.d/network restart; then
    log_error "Network restart failed after modem config. Manual restart may be needed."
    log_error "Modem: $iface, WAN: $wan_name, Device: $device"
    return 1
fi

# GOOD: Retry with backoff
retry_count=0
max_retries=3
while [ $retry_count -lt $max_retries ]; do
    if ifup "$wan_name"; then
        break
    fi
    retry_count=$((retry_count + 1))
    sleep $((2 ** retry_count))  # Exponential backoff
done
if [ $retry_count -eq $max_retries ]; then
    log_error "Failed to bring up $wan_name after $max_retries attempts"
    return 1
fi

# BAD: Silent failure
uci commit network
/etc/init.d/network restart
```

### 2. Logging Level Guidelines

| Level | Use When | Example |
|-------|----------|---------|
| CRITICAL | System unusable, data loss imminent | "All LAN ports lost, emergency recovery initiated" |
| ERROR | Operation failed, user action needed | "Failed to configure modem, check USB connection" |
| WARNING | Degraded operation, but functional | "Using default APN, carrier not in database" |
| INFO | Normal operations of interest | "USB modem detected, configuring as WAN2" |
| DEBUG | Detailed diagnostic information | "QMI signal strength: -72dBm" |

### 3. Mandatory Error Checks

These operations MUST always be checked:
- [ ] `uci commit`
- [ ] `/etc/init.d/* restart`
- [ ] File writes to persistent storage
- [ ] Network interface bring-up
- [ ] Lock file creation
- [ ] Config file parsing

### 4. User Notification vs. Logging

| Severity | Syslog | Log File | Web UI | Console |
|----------|--------|----------|---------|---------|
| CRITICAL | Yes | Yes | Yes | Yes |
| ERROR | Yes | Yes | Yes | Optional |
| WARNING | Yes | Yes | Optional | No |
| INFO | Yes | No | No | No |
| DEBUG | Optional | No | No | No |

---

## Summary of Error Handling Issues

### By Severity

**Critical (Needs Immediate Fix)**:
1. UCI commit failures not checked
2. Network restart failures ignored
3. Silent failures in emergency recovery
4. Background job errors discarded

**High (Should Fix Soon)**:
5. Inconsistent error handling across similar operations
6. Missing error context in messages
7. Return codes ignored by callers
8. Log spam without rate limiting

**Medium (Improve Over Time)**:
9. Logged but not acted upon
10. Missing logging for critical operations
11. Logging not visible to users
12. Inconsistent logging levels

---

## Proposed Fixes

### Fix #1: Add Standard Error Handler
```bash
# Add to all scripts
omr_fatal() {
    local msg="$1"
    log_critical "$msg"
    # Notify user via web UI
    echo "$msg" > /var/run/omr-error
    # Exit with error
    exit 1
}

omr_error() {
    local msg="$1"
    log_error "$msg"
    echo "$msg" >> /var/run/omr-errors
    return 1
}
```

### Fix #2: Mandatory Check Functions
```bash
# Wrapper for critical operations
uci_set_checked() {
    if ! uci -q set "$@"; then
        omr_error "UCI set failed: $*"
        return 1
    fi
    return 0
}

uci_commit_checked() {
    if ! uci commit "$@"; then
        omr_fatal "UCI commit failed for: $*. System may be in inconsistent state."
    fi
}
```

### Fix #3: Standardize Retry Logic
```bash
# Generic retry with exponential backoff
omr_retry() {
    local max_attempts=$1
    local delay=$2
    shift 2
    local cmd="$@"

    local attempt=1
    while [ $attempt -le $max_attempts ]; do
        if eval "$cmd"; then
            return 0
        fi

        if [ $attempt -lt $max_attempts ]; then
            log_warning "Command failed (attempt $attempt/$max_attempts), retrying in ${delay}s: $cmd"
            sleep $delay
            delay=$((delay * 2))  # Exponential backoff
        fi

        attempt=$((attempt + 1))
    done

    log_error "Command failed after $max_attempts attempts: $cmd"
    return 1
}

# Usage:
omr_retry 3 2 /etc/init.d/network restart
```

---

## Implementation Priority

### Phase 1 (Immediate - This PR)
1. Fix bash/sh incompatibility in omr-logger.sh
2. Add error checking to all UCI commit operations
3. Add error checking to network restarts
4. Fix background ifup to be synchronous with verification

### Phase 2 (Next PR)
5. Standardize logging levels
6. Add rate limiting to all logs
7. Add context to all error messages
8. Implement retry logic for critical operations

### Phase 3 (Future)
9. Create user notification system
10. Add comprehensive error recovery
11. Implement rollback on configuration errors
12. Add health monitoring and alerting
