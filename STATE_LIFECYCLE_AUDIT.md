# State Handling & Lifecycle Audit
**Date**: 2025-11-18
**Branch**: claude/code-audit-network-agent-017M3rNvBu3hJXvERrwhjTEB

## State Management Overview

OpenMPTCProuter uses multiple state persistence mechanisms:

### 1. UCI Configuration Database
- **Location**: `/etc/config/network`, `/etc/config/dhcp`, `/etc/config/system`
- **Type**: Persistent, survives reboot
- **Atomicity**: Partial (uci commit is atomic per file)
- **Synchronization**: Single-threaded (uci lock file)

### 2. Runtime State Files
- **Location**: `/var/run/*`
- **Type**: Volatile (tmpfs), lost on reboot
- **Atomicity**: None (regular file writes)
- **Synchronization**: None

### 3. Flag Files
- **Location**: `/etc/*`
- **Type**: Persistent
- **Purpose**: Track one-time initialization
- **Synchronization**: None

### 4. PID Files
- **Location**: `/var/run/*.pid`
- **Type**: Volatile
- **Purpose**: Prevent duplicate processes
- **Synchronization**: Best-effort (lock directories)

---

## STATE ISSUE #1: No Synchronization for Concurrent UCI Access

### Affected Files
- `network-safety-monitor.sh` (continuous loop)
- `usb-modem-autoconfig.sh` (hotplug triggered)
- `port-autoconfig.sh` (boot time)
- `omr-recovery` (user triggered)

### Problem
Multiple scripts can modify UCI configuration simultaneously:
```bash
# Script A (network-safety-monitor running every 30s)
uci set network.lan.ipaddr='192.168.2.1'
uci commit network

# Script B (hotplug event for USB modem)
uci set network.wan2.proto='qmi'
uci commit network  # Could overwrite Script A's changes

# Script C (user running omr-recovery)
uci delete network.wan
uci commit network  # Could create inconsistent state
```

### Consequences
- Lost configuration updates
- Partial configurations applied
- Race conditions between scripts
- Inconsistent network state after concurrent operations

### Current Mitigation
UCI has internal locking, but:
- Lock is only held during individual uci commands
- Batch operations are not transactional
- Multiple commits can interleave

### Recommended Fix
1. Implement script-level locking before UCI operations
2. Use uci batch operations with error checking
3. Add retry logic for lock contention

---

## STATE ISSUE #2: Incomplete Cleanup on Script Termination

### Affected Files
- `network-monitor.sh`
- `network-safety-monitor.sh`

### Problem - network-monitor.sh
```bash
cleanup() {
    rm -f "$PID_FILE"
    exit 0
}
trap cleanup INT TERM EXIT

# But what about KILL signal?
# What about system crash?
# PID file lingers...
```

### Problem - network-safety-monitor.sh
```bash
# NO cleanup handler at all!
# No trap for signals
# PID file never created
# No way to know if running
```

### Consequences
- Stale PID files after crash
- Multiple instances run on next boot
- Resource waste (CPU, file descriptors)
- Duplicate log messages

### Recommended Fix
1. Add cleanup traps to all long-running scripts
2. Implement PID file validation on start
3. Add process name checking beyond PID
4. Create systemd-style watchdog

---

## STATE ISSUE #3: Race Condition in Flag File Creation

### Affected Files
- `port-autoconfig.sh` (line 188)

### Problem
```bash
# Check if already configured
if [ -f "$CONFIG_APPLIED" ]; then
    exit 0
fi

# ... 100+ lines of code ...

# Mark as configured
touch "$CONFIG_APPLIED"  # Window of multiple executions
```

Time gap between check and flag creation means:
- Two instances could both pass the check
- Both attempt configuration
- UCI changes conflict
- Network in undefined state

### Consequences
- Duplicate configuration attempts
- UCI database corruption
- Port assignments conflict
- Users lose network access

### Recommended Fix
1. Use atomic operations (mkdir for lock)
2. Check for lock file at start
3. Create flag file before operations
4. Implement exclusive locking

---

## STATE ISSUE #4: Resource Leak - Unclosed File Descriptors

### Affected Files
- `usb-modem-autoconfig.sh` (lines 80-134)

### Problem
```bash
for dev in /dev/cdc-wdm*; do
    if [ -c "$dev" ]; then
        # Opens device for testing
        if umbim -d "$dev" -n caps 2>/dev/null | grep -q "device_type"; then
            # Device handle potentially left open
        fi
    fi
done
```

While commands typically close FDs, successive failures could accumulate:
- umbim might not clean up on errors
- Pipe to grep creates subprocesses
- File descriptor exhaustion possible

### Consequences
- File descriptor leaks on repeated failures
- System runs out of FDs
- Cannot open new connections
- Kernel limits hit

### Recommended Fix
1. Add explicit FD tracking
2. Use `exec` with FD management
3. Limit iterations with max count
4. Add FD monitoring to health checks

---

## STATE ISSUE #5: No Atomicity for State File Writes

### Affected Files
- `usb-modem-autoconfig.sh` (lines 260-269)
- `network-safety-monitor.sh` (lines 186-189)

### Problem - Modem Status Files
```bash
cat > "$status_dir/$wan_name" <<-EOFF
INTERFACE=$wan_name
PHYSICAL_DEVICE=$iface
PROTOCOL=$proto
CONTROL_DEVICE=$dev
INFO=$modem_info
CONFIGURED_AT=$(date)
EOFF
```

If process crashes mid-write:
- Partial file left on disk
- Reader gets incomplete data
- Parsing fails

### Problem - Emergency Port File
```bash
(
    umask 077
    echo "$emergency_port" > "$EMERGENCY_PORT_FILE"
)
```

Better (uses umask), but still not atomic:
- Write could fail midway
- Reader gets empty or partial data
- Emergency recovery uses wrong port

### Consequences
- Status readers crash on malformed files
- Emergency recovery fails
- Monitoring dashboards show incorrect data
- Scripts make decisions on stale data

### Recommended Fix
1. Write to temp file, then atomic rename
2. Use file locking during writes
3. Add version/checksum to state files
4. Validate state files before use

---

## STATE ISSUE #6: Shared State Without Synchronization

### Affected Files
- `omr-logger.sh` (lines 30-32)

### Problem
```bash
declare -A _omr_log_last_time
declare -A _omr_log_count
```

These associative arrays are:
- Shared across all log calls
- Modified without locking
- Accessed from subprocesses
- Not preserved across sourcing

### Consequences
- Race conditions in rate limiting
- Counts may be incorrect
- Messages suppressed incorrectly
- Important logs lost

### Current State
Bash arrays are not shared across processes, so this "works" by accident:
- Each script instance has its own copy
- But within a script, concurrent logging still races

### Recommended Fix
1. Use file-based rate limiting state
2. Add locking around state updates
3. Use atomic increment operations
4. Or accept racy behavior and document it

---

## STATE ISSUE #7: No Cleanup for Disconnected Modem State

### Affected Files
- `usb-modem-autoconfig.sh` (lines 300-329)

### Problem
```bash
cleanup_disconnected_modems() {
    # Only removes UCI config and status file
    # Doesn't clean up:
    # - Network interfaces
    # - Routing table entries
    # - iptables rules
    # - Connection tracking entries
    # - DNS cache entries
}
```

### Consequences
- Stale network interfaces
- Routing table pollution
- Memory leaks in kernel
- DNS resolution issues
- Connection attempts to dead interface

### Recommended Fix
1. Add `ifdown` for removed interfaces
2. Flush routing table entries
3. Clear connection tracking
4. Restart dependent services

---

## STATE ISSUE #8: UCI Batch Operations Lack Rollback

### Affected Files
- All files using `uci -q batch`

### Problem
```bash
uci -q batch <<-EOF
    delete network.wan
    set network.wan=interface
    set network.wan.proto='dhcp'
    set network.wan.metric='10'
EOF
```

If any command fails:
- Previous commands already executed
- No rollback mechanism
- Partial configuration applied
- Network in inconsistent state

### Consequences
- Half-configured interfaces
- Non-functional WAN connections
- User unable to recover without manual UCI edit
- Factory reset may be required

### Recommended Fix
1. Validate all changes before commit
2. Keep backup of config before changes
3. Implement rollback on error
4. Use revert feature: `uci revert network`

---

## STATE ISSUE #9: PID File Staleness Detection Insufficient

### Affected Files
- `network-monitor.sh` (lines 25-37)

### Problem
```bash
if [ -f "$PID_FILE" ]; then
    local old_pid=$(cat "$PID_FILE" 2>/dev/null)
    if echo "$old_pid" | grep -qE '^[0-9]+$'; then
        if kill -0 "$old_pid" 2>/dev/null; then
            # Assumes this PID is our process
            # But PID could be reused!
            exit 0
        fi
    fi
fi
```

### PID Reuse Problem
- Process exits, PID released
- New unrelated process gets same PID
- Script thinks it's already running
- Never starts, network monitoring disabled

### Consequences
- Monitoring silently disabled
- DHCP failures undetected
- Network degradation
- No recovery

### Recommended Fix
1. Store process name with PID
2. Verify /proc/$PID/cmdline matches
3. Use process start time
4. Implement unique session ID

---

## STATE ISSUE #10: No Ordering Guarantees for Concurrent Modifications

### Affected Files
- System-wide issue

### Problem
Three scripts could run simultaneously:
1. `network-safety-monitor.sh` (continuous, every 30s)
2. `usb-modem-autoconfig.sh` (hotplug event)
3. User via Web UI or omr-recovery

Each modifies UCI, network restarts, all without coordination.

### Example Scenario
```
T+0s:   Safety monitor detects APIPA, starts emergency_recovery
T+2s:   User plugs in USB modem, hotplug triggered
T+3s:   Safety monitor: uci delete network.@device[0]
T+4s:   Hotplug: uci set network.wan2.device='/dev/cdc-wdm0'
T+5s:   Safety monitor: uci commit network
T+6s:   Safety monitor: /etc/init.d/network restart
T+7s:   Hotplug: uci commit network  # Lost emergency recovery changes!
T+8s:   Hotplug: /etc/init.d/network reload
T+9s:   Network in undefined state (neither recovery nor modem config applied correctly)
```

### Consequences
- Configuration changes lost
- Network restarts interfere
- State oscillates
- Users experience connection drops
- Difficult to debug

### Recommended Fix
1. Implement global configuration lock
2. Queue configuration changes
3. Batch network restarts
4. Add change notification/waiting
5. Use network configuration daemon

---

## Summary of State Issues

### Critical Issues
1. ❌ No synchronization for concurrent UCI access
2. ❌ No atomicity for state file writes
3. ❌ UCI batch operations lack rollback
4. ❌ No ordering guarantees for concurrent modifications

### High Priority
5. ⚠️ Incomplete cleanup on script termination
6. ⚠️ Race condition in flag file creation
7. ⚠️ PID file staleness detection insufficient

### Medium Priority
8. ⚠️ Resource leak potential (file descriptors)
9. ⚠️ No cleanup for disconnected modem state
10. ⚠️ Shared state without synchronization (logger)

---

## Resource Lifecycle Analysis

### Properly Managed
✅ **Temporary files in /var/run/** - Cleared on reboot
✅ **UCI lock files** - Automatically managed by uci
✅ **Umask usage** - Good security practices in several places

### Needs Improvement
❌ **PID files** - Not cleaned up reliably
❌ **Lock directories** - Accumulate over time
❌ **Status files** - No expiration or validation
❌ **Network interfaces** - Not cleaned up when modems removed
❌ **File descriptors** - Potential leaks in error paths

### Missing
❌ **Connection tracking cleanup** - Never flushed
❌ **Routing table cleanup** - Entries accumulate
❌ **DNS cache invalidation** - Stale entries persist
❌ **Kernel module tracking** - No verification of loaded modules
❌ **Resource limits** - No checks for FD, memory, connection limits

---

## Recommendations

### Immediate Actions
1. Implement UCI operation locking with flock
2. Add atomic state file writes (write temp + rename)
3. Add rollback logic for batch operations
4. Fix PID file validation to prevent reuse issues

### Short Term
5. Add cleanup handlers to all long-running scripts
6. Implement process start time verification
7. Add file descriptor leak detection
8. Create state file validation

### Long Term
9. Develop centralized configuration manager
10. Implement change queuing and batching
11. Add comprehensive resource monitoring
12. Create automated state consistency checks
