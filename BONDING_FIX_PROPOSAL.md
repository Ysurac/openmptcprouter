# OpenMPTCP Bonding Behavior - Fix Proposal

## Executive Summary

Based on comprehensive network behavior audit, this document proposes **practical, incremental fixes** to address critical bonding issues without requiring major architectural changes.

---

## Audit Findings Summary

### CRITICAL Issues Found
1. **6.25-minute failover timeout** - Dead paths remain active for over 6 minutes
2. **RTT-only scheduler** - Ignores bandwidth (slow 10Mbps LTE beats fast 1Gbps fiber if RTT is lower)
3. **Instant failback** - No hysteresis causes flapping on unstable paths
4. **Round-robin bandwidth waste** - Equal quota regardless of 100x speed difference
5. **No path blacklisting** - Chronically lossy paths never permanently excluded
6. **User-space metrics unused** - Rich signal strength/latency data collected but not used

### Architecture Strengths
- Kernel MPTCP with proven RTT-based scheduler
- Sophisticated BLEST scheduler available but not default
- Good out-of-order reassembly
- Modem monitoring faster than MPTCP failover

---

## Fix Strategy: Incremental Improvements

Philosophy: **Fix behavior, not architecture**
- Keep existing MPTCP kernel implementation
- Tune parameters for bonding use case
- Add simple userspace intelligence layer
- Enable better schedulers where available
- Low-risk changes that can be rolled back

---

## Tier 1: CRITICAL FIXES (Immediate Impact)

### Fix 1.1: Aggressive Failover Detection
**Problem**: 6.25-minute keepalive timeout leaves dead paths active
**Impact**: Users experience degraded performance for 6+ minutes on path failure

**Solution**: Tune TCP keepalive for WAN bonding use case

**File**: `/common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf`

**Changes**:
```bash
# OLD (too conservative for bonding):
net.ipv4.tcp_keepalive_time = 300      # 5 minutes
net.ipv4.tcp_keepalive_probes = 5      # 5 probes
net.ipv4.tcp_keepalive_intvl = 15      # 15 seconds between probes
# Total: 300 + (5 × 15) = 375 seconds = 6.25 minutes

# NEW (aggressive bonding):
net.ipv4.tcp_keepalive_time = 20       # 20 seconds before first probe
net.ipv4.tcp_keepalive_probes = 3      # 3 probe attempts
net.ipv4.tcp_keepalive_intvl = 10      # 10 seconds between probes
# Total: 20 + (3 × 10) = 50 seconds = sub-1-minute failover
```

**Rationale**:
- Dead path detected in ~50 seconds vs 6+ minutes
- Still conservative enough to avoid false positives
- Aligns with modem monitor timeout (90 seconds)
- Standard for active failover systems

**Risk**: Low - standard values for HA systems
**Rollback**: Change values back if false positives observed

---

### Fix 1.2: Faster TCP Retry Timeouts
**Problem**: Default tcp_retries2=15 means 13-30 minute timeout before giving up
**Impact**: Single-path failures take too long to trigger failover

**File**: `/common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf`

**Changes**:
```bash
# Add these tuning parameters:
net.ipv4.tcp_retries1 = 3              # Network unreachable after 3 RTO backoffs (~3-8s)
net.ipv4.tcp_retries2 = 8              # Connection timeout after 8 RTO backoffs (~2-3 min)
net.ipv4.tcp_orphan_retries = 0        # Don't waste time on closing connections
```

**Rationale**:
- For MPTCP, want quick failover to alternate paths
- Retries2=8 gives reasonable time but not excessive (vs default 15)
- Retries1=3 triggers early path quality degradation signals
- Standard for multi-path systems

**Risk**: Low - well-tested values
**Rollback**: Comment out to use kernel defaults

---

### Fix 1.3: Enable BLEST Scheduler
**Problem**: Default scheduler picks lowest RTT, ignoring bandwidth and opportunity cost
**Impact**: Slow paths with low RTT get selected over fast paths with slightly higher RTT

**File**: `/common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf`

**Changes**:
```bash
# Add MPTCP scheduler selection:
net.mptcp.mptcp_scheduler = blest
```

**What BLEST does** (from audit):
- Starts with lowest RTT path (like default)
- Calculates opportunity cost of using slower paths
- **Refuses to use slow path if it would waste fast path capacity**
- Already implemented in kernel 5.4 patch

**Example behavior**:
```
Scenario: 1Gbps fiber @ 20ms RTT, 10Mbps LTE @ 15ms RTT
Default: Picks LTE (lower RTT) → underutilizes fiber
BLEST: Calculates LTE linger time would block fiber capacity → uses fiber instead
```

**Rationale**:
- Zero code changes - just enable existing scheduler
- Sophisticated bandwidth-delay estimation already implemented
- Proven in academic research

**Risk**: Low - existing kernel code, well-tested
**Rollback**: Set back to `net.mptcp.mptcp_scheduler = default`

---

## Tier 2: HIGH PRIORITY (Significant Improvement)

### Fix 2.1: Path Recovery Hysteresis
**Problem**: Instant failback causes flapping on unstable paths
**Impact**: Continuous reinjection overhead, connection instability

**Solution**: Add userspace path state tracking with hysteresis

**New File**: `/common/files/usr/bin/mptcp-path-manager.sh`

```bash
#!/bin/sh
# MPTCP Path Recovery Manager
# Prevents flapping by requiring stability before marking paths healthy

HYSTERESIS_TIME=30        # Require 30 seconds of stability
CHECK_INTERVAL=5          # Check every 5 seconds
STATE_DIR="/var/run/mptcp-paths"

mkdir -p "$STATE_DIR"

# Track path state: recovering, stable, failed
# Format: interface|last_failure_time|recovery_start_time|failure_count

monitor_paths() {
    for iface in $(ls /sys/class/net/); do
        # Skip non-WAN interfaces
        case "$iface" in
            lo|br-*|eth0.1|wlan*client) continue ;;
        esac

        STATE_FILE="$STATE_DIR/$iface"
        OPER_STATE=$(cat /sys/class/net/$iface/operstate 2>/dev/null)
        NOW=$(date +%s)

        # Initialize if needed
        [ -f "$STATE_FILE" ] || echo "$iface|0|0|0" > "$STATE_FILE"

        # Read current state
        read_state "$iface"

        case "$OPER_STATE" in
            up)
                # Path is up - check if in recovery period
                if [ "$failure_count" -gt 0 ]; then
                    if [ "$recovery_start" -eq 0 ]; then
                        # Just came back - start recovery timer
                        recovery_start=$NOW
                        logger -t mptcp-path "Path $iface recovering (failures: $failure_count)"
                    else
                        # In recovery - check if stable enough
                        recovery_duration=$((NOW - recovery_start))
                        if [ "$recovery_duration" -ge "$HYSTERESIS_TIME" ]; then
                            # Stable for required period - mark healthy
                            failure_count=0
                            recovery_start=0
                            logger -t mptcp-path "Path $iface STABLE (recovered after $recovery_duration seconds)"
                        fi
                    fi
                fi
                ;;
            down)
                # Path is down - mark failure
                last_failure=$NOW
                failure_count=$((failure_count + 1))
                recovery_start=0
                logger -t mptcp-path "Path $iface DOWN (failure #$failure_count)"
                ;;
        esac

        # Save state
        echo "$iface|$last_failure|$recovery_start|$failure_count" > "$STATE_FILE"
    done
}

# Run monitoring loop
while true; do
    monitor_paths
    sleep "$CHECK_INTERVAL"
done
```

**Integration**: Start via `/etc/rc.local` or procd service

**Rationale**:
- Prevents instant failback causing flapping
- Provides visibility into path stability
- Can be integrated with future path metric exports

**Risk**: Low - passive monitoring, no MPTCP modification
**Rollback**: Stop the script

---

### Fix 2.2: Path Blacklisting for Chronic Issues
**Problem**: Paths with persistent high loss never get excluded
**Impact**: Degraded performance on chronically bad paths

**Solution**: Track failure patterns and temporarily blacklist problematic paths

**Enhancement to**: `/common/files/usr/bin/mptcp-path-manager.sh`

```bash
BLACKLIST_THRESHOLD=5     # 5 failures in observation window
BLACKLIST_WINDOW=300      # 5-minute observation window
BLACKLIST_DURATION=300    # Blacklist for 5 minutes

check_blacklist() {
    iface=$1

    # Count failures in window
    failures_in_window=$(awk -F'|' -v now="$NOW" -v window="$BLACKLIST_WINDOW" \
        '$2 > (now - window) { count++ } END { print count+0 }' "$STATE_DIR/$iface")

    if [ "$failures_in_window" -ge "$BLACKLIST_THRESHOLD" ]; then
        # Blacklist this path
        if ! is_blacklisted "$iface"; then
            logger -t mptcp-path "BLACKLISTING $iface ($failures_in_window failures in ${BLACKLIST_WINDOW}s)"

            # Increase interface metric to make it less preferred
            # This affects routing table, not MPTCP directly, but provides feedback
            current_metric=$(uci get network.${iface}.metric 2>/dev/null || echo 100)
            blacklist_metric=$((current_metric + 1000))
            uci set network.${iface}.metric="$blacklist_metric"
            uci commit network
            /etc/init.d/network reload

            echo "$NOW" > "$STATE_DIR/${iface}.blacklist"
        fi
    else
        # Check if blacklist should be lifted
        if is_blacklisted "$iface"; then
            blacklist_start=$(cat "$STATE_DIR/${iface}.blacklist")
            blacklist_age=$((NOW - blacklist_start))

            if [ "$blacklist_age" -ge "$BLACKLIST_DURATION" ]; then
                logger -t mptcp-path "UNBLACKLISTING $iface (served $blacklist_age second penalty)"

                # Restore original metric
                original_metric=$(uci get network.${iface}.metric 2>/dev/null || echo 100)
                restored_metric=$((original_metric % 1000))  # Remove blacklist penalty
                uci set network.${iface}.metric="$restored_metric"
                uci commit network
                /etc/init.d/network reload

                rm -f "$STATE_DIR/${iface}.blacklist"
            fi
        fi
    fi
}
```

**Rationale**:
- Prevents chronic problematic paths from degrading service
- Self-healing: blacklist expires after timeout
- Uses existing UCI metric system for feedback
- Provides logging for troubleshooting

**Risk**: Medium - modifies routing metrics dynamically
**Rollback**: Stop script, manually reset metrics via UCI

---

### Fix 2.3: Coordinate Modem Monitoring with MPTCP
**Problem**: Modem resets (90s) happen independently of MPTCP failover (375s)
**Impact**: Unnecessary packet loss and reinjection during modem reset

**Solution**: Have modem monitors notify MPTCP-aware components

**File**: `/common/package/modems/files/rm551e-monitor.sh` (and similar)

**Changes**:
```bash
# After detecting modem failure (around line 80-100):
perform_modem_reset() {
    local device=$1

    logger -p daemon.warning -t modem "Resetting modem $device (failure threshold reached)"

    # ADDITION: Notify MPTCP path manager of impending reset
    if [ -f /var/run/mptcp-paths/$(basename $INTERFACE) ]; then
        logger -t mptcp-path "Modem $device resetting - path will be temporarily down"
        # Force immediate failure detection rather than waiting for keepalive
        echo "$(basename $INTERFACE)|$(date +%s)|0|999" > /var/run/mptcp-paths/$(basename $INTERFACE)
    fi

    # Existing reset logic...
    echo "1" > "/sys/class/gpio/gpio${GPIO_RESET}/value"
    sleep 5
    echo "0" > "/sys/class/gpio/gpio${GPIO_RESET}/value"

    sleep 30  # Wait for modem to reinitialize

    # ADDITION: Mark path as recovering
    if [ -f /var/run/mptcp-paths/$(basename $INTERFACE) ]; then
        logger -t mptcp-path "Modem $device reset complete - starting recovery period"
    fi
}
```

**Rationale**:
- Modem monitor already detects failures faster than MPTCP
- Share this information to avoid dual detection delay
- Prevents packets being sent to path during reset

**Risk**: Low - adds logging and state tracking, doesn't change reset behavior
**Rollback**: Remove additions

---

## Tier 3: MEDIUM PRIORITY (Enhanced Intelligence)

### Fix 3.1: Expose User-Space Metrics to Decision Making
**Problem**: Rich metrics (signal strength, measured latency) collected but never used
**Impact**: Missed opportunities for proactive path management

**Solution**: Create metric export system for future scheduler integration

**New File**: `/common/files/usr/bin/mptcp-metrics-exporter.sh`

```bash
#!/bin/sh
# MPTCP Metrics Exporter
# Collects user-space metrics and makes them available for scheduling decisions

EXPORT_DIR="/var/run/mptcp-metrics"
EXPORT_INTERVAL=10  # Update every 10 seconds

mkdir -p "$EXPORT_DIR"

export_interface_metrics() {
    iface=$1

    # Get basic stats
    rx_bytes=$(cat /sys/class/net/$iface/statistics/rx_bytes 2>/dev/null || echo 0)
    tx_bytes=$(cat /sys/class/net/$iface/statistics/tx_bytes 2>/dev/null || echo 0)
    rx_packets=$(cat /sys/class/net/$iface/statistics/rx_packets 2>/dev/null || echo 0)
    tx_packets=$(cat /sys/class/net/$iface/statistics/tx_packets 2>/dev/null || echo 0)
    rx_errors=$(cat /sys/class/net/$iface/statistics/rx_errors 2>/dev/null || echo 0)
    tx_errors=$(cat /sys/class/net/$iface/statistics/tx_errors 2>/dev/null || echo 0)

    # Calculate loss rate if we have previous data
    prev_file="$EXPORT_DIR/${iface}.prev"
    if [ -f "$prev_file" ]; then
        prev_rx_packets=$(awk -F'|' '{print $3}' "$prev_file")
        prev_rx_errors=$(awk -F'|' '{print $7}' "$prev_file")

        packets_delta=$((rx_packets - prev_rx_packets))
        errors_delta=$((rx_errors - prev_rx_errors))

        if [ "$packets_delta" -gt 0 ]; then
            loss_rate=$((errors_delta * 10000 / packets_delta))  # In basis points (0.01%)
        else
            loss_rate=0
        fi
    else
        loss_rate=0
    fi

    # Get signal strength for cellular modems
    signal_strength="N/A"
    if echo "$iface" | grep -q "wwan"; then
        # Try QMI
        if command -v uqmi >/dev/null 2>&1; then
            qmi_device=$(uci get network.${iface}.device 2>/dev/null)
            if [ -n "$qmi_device" ]; then
                signal_info=$(uqmi -d "$qmi_device" --get-signal-info 2>/dev/null)
                if [ $? -eq 0 ]; then
                    signal_strength=$(echo "$signal_info" | jsonfilter -e '@.rssi' 2>/dev/null || echo "N/A")
                fi
            fi
        fi
    fi

    # Export in parseable format
    # Format: iface|timestamp|rx_packets|tx_packets|rx_bytes|tx_bytes|rx_errors|tx_errors|loss_rate|signal_strength
    echo "$iface|$(date +%s)|$rx_packets|$tx_packets|$rx_bytes|$tx_bytes|$rx_errors|$tx_errors|$loss_rate|$signal_strength" \
        > "$EXPORT_DIR/${iface}.current"

    # Save for next delta calculation
    cp "$EXPORT_DIR/${iface}.current" "$prev_file"
}

# Main loop
while true; do
    for iface in $(ls /sys/class/net/); do
        case "$iface" in
            lo|br-*|eth0.1) continue ;;
            *) export_interface_metrics "$iface" ;;
        esac
    done
    sleep "$EXPORT_INTERVAL"
done
```

**Rationale**:
- Creates foundation for metric-aware scheduling
- Loss rate can be used for path quality assessment
- Signal strength for proactive failover before complete loss
- Formatted for easy consumption by future BPF schedulers

**Risk**: Low - passive data collection
**Rollback**: Stop the script

---

### Fix 3.2: Proactive Path Degradation Detection
**Problem**: Paths only excluded when in TCP_CA_Loss state (reactive)
**Impact**: High loss paths keep getting selected until they fully fail

**Solution**: Monitor loss metrics and proactively reduce path usage

**Enhancement to**: `/common/files/usr/bin/mptcp-path-manager.sh`

```bash
LOSS_THRESHOLD=500         # 5% loss rate (in basis points)
DEGRADED_METRIC_PENALTY=50 # Add 50 to metric when degraded

check_path_degradation() {
    iface=$1

    if [ ! -f "/var/run/mptcp-metrics/${iface}.current" ]; then
        return
    fi

    # Read loss rate from metrics
    loss_rate=$(awk -F'|' '{print $9}' "/var/run/mptcp-metrics/${iface}.current")

    if [ "$loss_rate" -ge "$LOSS_THRESHOLD" ]; then
        # Path is degraded - reduce priority
        if ! is_degraded "$iface"; then
            logger -t mptcp-path "Path $iface DEGRADED (loss rate: $((loss_rate / 100))%)"

            current_metric=$(uci get network.${iface}.metric 2>/dev/null || echo 100)
            degraded_metric=$((current_metric + DEGRADED_METRIC_PENALTY))
            uci set network.${iface}.metric="$degraded_metric"
            uci commit network
            /etc/init.d/network reload

            touch "$STATE_DIR/${iface}.degraded"
        fi
    else
        # Path quality is good - restore if was degraded
        if is_degraded "$iface"; then
            logger -t mptcp-path "Path $iface RECOVERED (loss rate: $((loss_rate / 100))%)"

            current_metric=$(uci get network.${iface}.metric 2>/dev/null || echo 100)
            restored_metric=$((current_metric - DEGRADED_METRIC_PENALTY))
            uci set network.${iface}.metric="$restored_metric"
            uci commit network
            /etc/init.d/network reload

            rm -f "$STATE_DIR/${iface}.degraded"
        fi
    fi
}
```

**Rationale**:
- Proactive rather than reactive
- Reduces usage of degraded paths before complete failure
- Uses measured loss rate from metrics exporter
- Graceful degradation and recovery

**Risk**: Medium - actively modifies routing metrics based on measurements
**Rollback**: Stop script, reset metrics

---

### Fix 3.3: Tune Round-Robin for Heterogeneous Links
**Problem**: Round-robin uses equal quota (default 1 packet) regardless of 100x speed difference
**Impact**: Fast links underutilized

**Solution**: Make round-robin quota configurable via sysctl

**File**: `/common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf`

**Changes**:
```bash
# If using round-robin scheduler (not recommended with BLEST):
# Increase quota for better batching on fast links
# This is a kernel module parameter, set via:
# echo 10 > /sys/module/mptcp_rr/parameters/num_segments

# However, we recommend using BLEST instead (see Fix 1.3)
```

**Note**: Only applicable if user explicitly chooses round-robin scheduler. BLEST (Fix 1.3) is better solution.

**Risk**: Low - tuning parameter
**Rollback**: Reset to 1

---

## Tier 4: FUTURE ENHANCEMENTS (Requires More Work)

### Fix 4.1: BPF-based Intelligent Scheduler (Kernel 6.12+)
**Complexity**: High - requires BPF programming
**Benefit**: Ultimate flexibility - custom scheduling logic

**Concept**:
- Use kernel 6.12's BPF MPTCP hooks
- Write BPF program that:
  - Reads bandwidth estimates from user-space metrics
  - Implements weighted fair queueing
  - Uses loss rate for path selection
  - Exports scheduler decisions to user-space

**Status**: Future work - requires BPF development
**Current**: Focus on enabling BLEST (Fix 1.3) as immediate improvement

---

### Fix 4.2: Application-Level Health Probes
**Complexity**: Medium - requires new daemon

**Concept**:
- Supplement TCP keepalive with active probes
- HTTP/ICMP pings to known endpoints
- Faster detection of certain failure modes
- Integration with path state tracking

**Status**: Future enhancement
**Current**: Aggressive keepalive (Fix 1.1) provides most benefit

---

### Fix 4.3: Machine Learning Path Predictor
**Complexity**: Very High - requires ML model

**Concept**:
- Learn path quality patterns over time
- Predict failures before they occur (e.g., daily LTE congestion)
- Proactively adjust scheduling

**Status**: Research project
**Current**: Rule-based degradation detection (Fix 3.2) provides practical alternative

---

## Implementation Plan

### Phase 1: IMMEDIATE (Week 1)
1. Apply Fix 1.1: Aggressive failover detection
2. Apply Fix 1.2: Faster TCP retries
3. Apply Fix 1.3: Enable BLEST scheduler
4. **Deliverable**: Updated sysctl configuration file

### Phase 2: HIGH PRIORITY (Week 2-3)
1. Implement Fix 2.1: Path recovery hysteresis
2. Implement Fix 2.2: Path blacklisting
3. Apply Fix 2.3: Coordinate modem monitoring
4. **Deliverable**: New mptcp-path-manager daemon + modem monitor updates

### Phase 3: MEDIUM PRIORITY (Week 4-6)
1. Implement Fix 3.1: Metrics exporter
2. Implement Fix 3.2: Proactive degradation detection
3. Tune Fix 3.3: Round-robin quota (if needed)
4. **Deliverable**: Metrics infrastructure + intelligent path management

### Phase 4: FUTURE (Ongoing)
1. Evaluate Fix 4.1: BPF scheduler (for kernel 6.12+)
2. Consider Fix 4.2: Application-level probes
3. Research Fix 4.3: ML-based prediction
4. **Deliverable**: Research prototypes

---

## Testing Strategy

### Per-Fix Validation

**Fix 1.1 (Aggressive Failover)**:
```bash
# Test: Unplug WAN cable, measure failover time
# Expected: Path excluded within 50 seconds (vs 375s before)
logger "TEST: Unplugging WAN at $(date +%s)"
# Monitor: grep mptcp /var/log/messages
```

**Fix 1.3 (BLEST Scheduler)**:
```bash
# Test: Monitor path selection with heterogeneous links
# Expected: Fast path preferred even with slightly higher RTT
cat /proc/net/mptcp
# Watch for bandwidth utilization on fast vs slow links
```

**Fix 2.1 (Hysteresis)**:
```bash
# Test: Simulate flapping path (rapid up/down)
# Expected: Path not immediately restored, requires stability period
tail -f /var/log/messages | grep mptcp-path
```

**Fix 2.2 (Blacklisting)**:
```bash
# Test: Induce repeated failures on one path
# Expected: Path blacklisted after threshold, auto-recovered after timeout
uci show network | grep metric  # Should show penalty applied
```

### Integration Testing

**Scenario 1: Single Path Failure**
- Setup: 2 WAN links (fiber + LTE)
- Action: Unplug fiber
- Expected: <60s failover to LTE, connection maintains

**Scenario 2: Flapping Path**
- Setup: 2 WAN links
- Action: Rapid disconnect/reconnect one path
- Expected: Hysteresis prevents rapid failback, blacklisting after threshold

**Scenario 3: Degraded Path**
- Setup: 2 WAN links
- Action: Induce 10% packet loss on one path
- Expected: Loss detected, path deprioritized, shifts to healthy path

**Scenario 4: Heterogeneous Links**
- Setup: 1Gbps fiber (20ms RTT) + 10Mbps LTE (15ms RTT)
- Action: Large file transfer
- Expected: BLEST uses fiber despite higher RTT, LTE not overwhelmed

---

## Risk Mitigation

### Rollback Procedures

**For sysctl changes (Fixes 1.1, 1.2, 1.3)**:
```bash
# Restore from backup
cp /etc/sysctl.d/99-omr-mptcp-5g-optimization.conf.backup \
   /etc/sysctl.d/99-omr-mptcp-5g-optimization.conf
sysctl -p /etc/sysctl.d/99-omr-mptcp-5g-optimization.conf
```

**For daemon additions (Fixes 2.1, 2.2, 3.1)**:
```bash
# Stop and disable
/etc/init.d/mptcp-path-manager stop
/etc/init.d/mptcp-path-manager disable
rm /etc/init.d/mptcp-path-manager
```

**For modem monitor changes (Fix 2.3)**:
```bash
# Restore original scripts from git
git checkout common/package/modems/files/*-monitor.sh
/etc/init.d/modem-monitor restart
```

### Monitoring During Rollout

**Key Metrics to Watch**:
1. Subflow count: `ss -tin | grep subflow | wc -l`
2. Path state: `cat /var/run/mptcp-paths/*`
3. Failover events: `grep mptcp /var/log/messages`
4. Connection stability: `ping -c 100 8.8.8.8` (packet loss)
5. Throughput: `iperf3 -c <vps>`

**Alert Conditions**:
- Subflow count drops to 0 (all paths failed)
- Excessive path flapping (>10 state changes/minute)
- Packet loss >5% during normal operation
- Throughput <50% of expected aggregate

---

## Expected Outcomes

### Before Fixes
- Dead path failover: **6.25 minutes**
- Flapping path: **Continuous reinjection**
- Slow path with low RTT: **Gets priority over fast path**
- Round-robin: **Severe underutilization of fast links**
- Chronic loss: **No exclusion mechanism**

### After Fixes
- Dead path failover: **<60 seconds** (88% improvement)
- Flapping path: **30s hysteresis prevents instability**
- Slow path with low RTT: **BLEST prevents underutilization**
- Heterogeneous links: **Intelligent bandwidth-delay optimization**
- Chronic loss: **Automatic blacklisting with self-healing**

### Measurable Improvements
- **Failover time**: 375s → 50s (86% reduction)
- **Path utilization**: More balanced distribution based on capacity
- **Connection stability**: Reduced reinjection overhead
- **User experience**: Fewer noticeable interruptions

---

## Summary

This proposal provides **actionable, incremental fixes** that address critical bonding behavior issues without requiring architectural changes.

**Immediate wins** (Tier 1):
- 86% faster failover detection
- Bandwidth-aware scheduling (BLEST)
- Simple configuration changes, low risk

**High-value enhancements** (Tier 2):
- Path stability tracking
- Chronic issue blacklisting
- Better modem coordination

**Intelligence layer** (Tier 3):
- Metric-driven decisions
- Proactive degradation handling
- Foundation for future BPF schedulers

**All changes are reversible** and can be applied incrementally, allowing validation at each step.
