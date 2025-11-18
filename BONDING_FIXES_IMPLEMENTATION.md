# OpenMPTCP Bonding Behavior Fixes - Implementation Guide

## Overview

This document describes the **implemented fixes** to address critical bonding behavior issues in OpenMPTCProuter. All changes are incremental, reversible, and designed to improve failover performance without requiring architectural changes.

---

## What Was Fixed

### Critical Issues Addressed

1. **6.25-minute failover timeout** → **Reduced to <60 seconds** (86% improvement)
2. **RTT-only scheduler ignoring bandwidth** → **BLEST scheduler enabled** (bandwidth-aware)
3. **Instant failback causing flapping** → **30-second hysteresis** (stability required)
4. **No path blacklisting** → **Automatic blacklisting** with self-healing
5. **Modem resets out of sync with MPTCP** → **Coordinated failover**
6. **User-space metrics unused** → **Metrics collection** for decision-making

---

## Implementation Summary

### Tier 1: Critical Fixes (Sysctl Configuration)

**File Modified**: `/common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf`

#### Fix 1.1: Aggressive Failover Detection
```bash
# OLD (6.25 minutes total failover time):
net.ipv4.tcp_keepalive_time = 300
net.ipv4.tcp_keepalive_probes = 5
net.ipv4.tcp_keepalive_intvl = 15

# NEW (50 seconds total failover time):
net.ipv4.tcp_keepalive_time = 20
net.ipv4.tcp_keepalive_probes = 3
net.ipv4.tcp_keepalive_intvl = 10
```

**Impact**: Dead path detected in ~50 seconds vs 6+ minutes

#### Fix 1.2: Faster TCP Retry Timeouts
```bash
# NEW: Faster failover for multi-path systems
net.ipv4.tcp_retries1 = 3    # Network unreachable after ~3-8s
net.ipv4.tcp_retries2 = 8    # Connection timeout after ~2-3 min (vs 13-30 min default)
net.ipv4.tcp_orphan_retries = 0
```

**Impact**: Quicker path failover, less time waiting on dead connections

#### Fix 1.3: Enable BLEST Scheduler
```bash
# NEW: Intelligent bandwidth-delay optimization
net.mptcp.mptcp_scheduler = blest
net.mptcp.mptcp_path_manager = fullmesh
net.mptcp.mptcp_checksum = 0
net.mptcp.mptcp_syn_retries = 3
```

**Impact**:
- Prevents slow paths (10Mbps LTE) from blocking fast paths (1Gbps fiber)
- Calculates opportunity cost before using slower links
- Better bandwidth utilization across heterogeneous links

---

### Tier 2: High-Priority Fixes (New Daemons)

#### Fix 2.1 & 2.2: Path Manager with Hysteresis and Blacklisting

**New Files**:
- `/common/files/usr/bin/mptcp-path-manager` (executable)
- `/common/files/etc/init.d/mptcp-manager` (service)

**Features**:

1. **Path Recovery Hysteresis**
   - Requires 30 seconds of stability before marking path healthy
   - Prevents flapping on unstable links
   - Logs all state transitions to syslog

2. **Path Blacklisting**
   - Tracks failure patterns (5 failures in 5 minutes → blacklist)
   - Automatic blacklist expiration after 5 minutes
   - Applies metric penalty to reduce path priority

3. **Proactive Degradation Detection**
   - Monitors packet loss rate from metrics
   - Deprioritizes paths with >5% loss
   - Automatic recovery when quality improves

**Usage**:
```bash
# Enable service
/etc/init.d/mptcp-manager enable
/etc/init.d/mptcp-manager start

# Check status
logread | grep mptcp-path

# View path states
ls -la /var/run/mptcp-paths/
```

#### Fix 2.3: Modem Monitor Coordination

**File Modified**: `/common/package/modems/files/rm551e-monitor.sh`

**Changes**:
- Added `notify_mptcp_path_manager()` function
- Notifies MPTCP before modem reset starts
- Notifies MPTCP when reset completes
- Coordinates 90-second modem timeout with 50-second MPTCP timeout

**Impact**:
- MPTCP knows path is resetting, doesn't waste packets
- Faster failover during modem issues
- Reduced packet loss during recovery

---

### Tier 3: Medium-Priority Fixes (Metrics Infrastructure)

#### Fix 3.1: Metrics Exporter

**New File**: `/common/files/usr/bin/mptcp-metrics-exporter` (executable)

**Metrics Collected**:
- Packet loss rate (calculated from error counters)
- Throughput (RX/TX bytes per second)
- Signal strength (for cellular modems via QMI/MBIM)
- Signal quality (RSRP for LTE/5G)
- Interface speed
- MTU

**Export Format**:
```
/var/run/mptcp-metrics/{interface}.current:
iface|timestamp|rx_packets|tx_packets|rx_bytes|tx_bytes|rx_errors|tx_errors|loss_rate|signal_strength|signal_quality|throughput_rx|throughput_tx|speed|mtu|rx_dropped|tx_dropped

/var/run/mptcp-metrics/summary:
Human-readable summary of all WAN paths
```

**Usage**:
```bash
# View current metrics
cat /var/run/mptcp-metrics/summary

# View specific interface
cat /var/run/mptcp-metrics/wwan0.current

# Monitor loss rate for interface
watch -n 1 "cat /var/run/mptcp-metrics/wwan0.current | awk -F'|' '{print \"Loss Rate: \" \$9/100 \"%\"}'"
```

**Integration**:
- Used by path manager for degradation detection
- Available for future BPF scheduler integration
- Provides visibility into link quality

---

## File Changes Summary

### Modified Files
1. `/common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf`
   - Added MPTCP scheduler configuration
   - Reduced TCP keepalive timeouts
   - Added TCP retry tuning

2. `/common/package/modems/files/rm551e-monitor.sh`
   - Added MPTCP coordination functions
   - Notifies path manager on modem events

### New Files
1. `/common/files/usr/bin/mptcp-path-manager` (755)
   - Hysteresis and blacklisting logic
   - Path state tracking
   - Metric-based degradation detection

2. `/common/files/usr/bin/mptcp-metrics-exporter` (755)
   - Link quality metrics collection
   - Signal strength monitoring
   - Loss rate calculation

3. `/common/files/etc/init.d/mptcp-manager` (755)
   - Procd service for both daemons
   - Auto-restart on failure
   - Reload trigger on network changes

---

## How to Deploy

### Step 1: Make Scripts Executable
```bash
chmod +x /home/user/openmptcprouter/common/files/usr/bin/mptcp-path-manager
chmod +x /home/user/openmptcprouter/common/files/usr/bin/mptcp-metrics-exporter
chmod +x /home/user/openmptcprouter/common/files/etc/init.d/mptcp-manager
```

### Step 2: Build Updated Image
```bash
cd /home/user/openmptcprouter
make -j$(nproc)
```

### Step 3: Flash Router (or Test in VM)
```bash
# Flash via sysupgrade
sysupgrade -n /path/to/new/firmware.bin

# Or test in QEMU first
```

### Step 4: Verify Deployment
```bash
# Check sysctl applied
sysctl net.mptcp.mptcp_scheduler
# Should output: net.mptcp.mptcp_scheduler = blest

sysctl net.ipv4.tcp_keepalive_time
# Should output: net.ipv4.tcp_keepalive_time = 20

# Check services running
/etc/init.d/mptcp-manager status

# Check logs
logread | grep -E "mptcp-path|mptcp-metrics"
```

---

## Validation Tests

### Test 1: Failover Speed
**Objective**: Verify <60s failover on path failure

```bash
# Terminal 1: Start continuous ping
ping -i 0.2 8.8.8.8 | ts '[%Y-%m-%d %H:%M:%S]'

# Terminal 2: Monitor MPTCP paths
watch -n 1 'cat /var/run/mptcp-paths/*; echo; cat /var/run/mptcp-metrics/summary'

# Action: Unplug WAN cable or disable interface
ifconfig wwan0 down

# Expected Result:
# - Ping interruption < 60 seconds
# - Path marked DOWN in /var/run/mptcp-paths/wwan0
# - Traffic continues on remaining paths
# - Log entry: "Path wwan0 DOWN"
```

### Test 2: Hysteresis (No Instant Failback)
**Objective**: Verify 30-second stability requirement

```bash
# Monitor logs
logread -f | grep mptcp-path

# Action: Rapidly toggle interface up/down
ifconfig wwan0 down; sleep 5; ifconfig wwan0 up; sleep 10; ifconfig wwan0 down; sleep 5; ifconfig wwan0 up

# Expected Result:
# - First down: "Path wwan0 DOWN (failure #1)"
# - First up: "Path wwan0 RECOVERING (failures: 1, hysteresis: 30s)"
# - Second down (before 30s): "Path wwan0 DOWN (failure #2)"
# - Second up: "Path wwan0 RECOVERING (failures: 2, hysteresis: 30s)"
# - After 30s stable: "Path wwan0 STABLE (recovered after Xs, cleared 2 failures)"
```

### Test 3: Blacklisting
**Objective**: Verify automatic blacklisting after repeated failures

```bash
# Monitor path state
watch -n 1 'cat /var/run/mptcp-paths/*; uci show network | grep metric'

# Action: Cause 5 failures within 5 minutes
for i in 1 2 3 4 5; do
    ifconfig wwan0 down
    sleep 30
    ifconfig wwan0 up
    sleep 30
done

# Expected Result:
# - After 5th failure: "BLACKLISTING wwan0 (5 failures in 300s window)"
# - Metric penalty applied: network.wwan0.metric increased by 1000
# - Path excluded from rotation
# - After 5 minutes: "UNBLACKLISTING wwan0 (served 300s penalty)"
# - Metric restored
```

### Test 4: BLEST Scheduler (Heterogeneous Links)
**Objective**: Verify fast path preferred over slow path

```bash
# Setup: 2 WAN links with different speeds
# wwan0: 10Mbps LTE with 15ms RTT
# eth1: 1Gbps fiber with 20ms RTT

# Monitor MPTCP subflows
watch -n 1 'cat /proc/net/mptcp'

# Action: Transfer large file
iperf3 -c <vps-ip> -t 60

# Expected Result:
# - Majority of traffic on eth1 (fiber)
# - wwan0 used minimally (opportunistic)
# - Throughput close to 1Gbps (not limited by slow path)
```

### Test 5: Modem Reset Coordination
**Objective**: Verify smooth failover during modem reset

```bash
# Monitor logs in real-time
logread -f | grep -E "modem|mptcp-path"

# Monitor ping continuity
ping -i 0.5 8.8.8.8 | ts '[%Y-%m-%d %H:%M:%S]'

# Action: Trigger modem reset
# (Can simulate by forcing monitor to detect failures or manual reset command)

# Expected Result:
# - Log: "Modem wwan0 resetting - path marked as down"
# - MPTCP excludes wwan0 immediately
# - Ping continues on alternate paths
# - After reset: "Modem wwan0 reset complete - entering recovery period"
# - 30s hysteresis before wwan0 rejoins
# - Minimal packet loss (only during actual reset, not detection delay)
```

### Test 6: Degradation Detection
**Objective**: Verify proactive path deprioritization on high loss

```bash
# Monitor metrics
watch -n 2 'cat /var/run/mptcp-metrics/summary'

# Action: Induce packet loss on interface (using tc netem)
tc qdisc add dev wwan0 root netem loss 10%

# Expected Result:
# - Loss rate in metrics increases to ~1000 (10%)
# - Log: "Path wwan0 DEGRADED (loss rate: 10.00%)"
# - Metric penalty applied
# - Traffic shifts to healthier paths
# - After removing loss: "Path wwan0 RECOVERED from degradation"
```

---

## Expected Behavior Changes

### Before Fixes

| Scenario | Old Behavior | Failover Time |
|----------|-------------|---------------|
| Dead WAN cable unplugged | Keepalive timeout | **6.25 minutes** |
| Flapping LTE connection | Instant failback, continuous reinjection | N/A (unstable) |
| 10Mbps LTE @ 15ms vs 1Gbps fiber @ 20ms | LTE selected (lower RTT) | N/A (suboptimal) |
| Chronically lossy WiFi (30% loss) | Keeps using it | Never excluded |
| Modem reset (90s) | MPTCP unaware, parallel timeouts | Inconsistent |

### After Fixes

| Scenario | New Behavior | Failover Time |
|----------|-------------|---------------|
| Dead WAN cable unplugged | Aggressive keepalive | **<50 seconds** |
| Flapping LTE connection | 30s hysteresis, 5-min blacklist after pattern | Stable |
| 10Mbps LTE @ 15ms vs 1Gbps fiber @ 20ms | Fiber selected (BLEST opportunity cost) | Optimal |
| Chronically lossy WiFi (30% loss) | Deprioritized, eventually blacklisted | Auto-excluded |
| Modem reset (90s) | Coordinated exclusion & recovery | **~90 seconds** |

---

## Performance Metrics

### Failover Performance
- **Detection time**: 375s → 50s (**86% improvement**)
- **Recovery time**: Instant (flap-prone) → 30s hysteresis (**stability gain**)
- **Blacklist reaction**: None → 5 failures/5min (**chronic issue handling**)

### Scheduling Intelligence
- **Metric awareness**: RTT-only → RTT + bandwidth + opportunity cost
- **Link utilization**: Equal distribution → Proportional to capacity
- **Reordering**: No change (meta-level OFO queue still handles it)

### Operational Visibility
- **Path state**: Hidden → Visible in `/var/run/mptcp-paths/`
- **Metrics**: Collected but unused → Collected and used for decisions
- **Logging**: Minimal → Comprehensive state transitions

---

## Troubleshooting

### Services Not Starting
```bash
# Check if scripts are executable
ls -la /usr/bin/mptcp-*
# Should show: -rwxr-xr-x

# Check init script
ls -la /etc/init.d/mptcp-manager
# Should show: -rwxr-xr-x

# Check logs for errors
logread | grep mptcp
```

### Sysctl Settings Not Applied
```bash
# Manually apply
sysctl -p /etc/sysctl.d/99-omr-mptcp-5g-optimization.conf

# Check for errors
sysctl -a | grep mptcp

# Verify MPTCP scheduler exists
cat /proc/sys/net/mptcp/mptcp_scheduler
```

### Path Manager Not Detecting Interfaces
```bash
# Check if interfaces have multipath enabled
uci show network | grep multipath

# Check sysfs
ls -la /sys/class/net/

# Check state directory
ls -la /var/run/mptcp-paths/
```

### Metrics Not Being Exported
```bash
# Check if metrics exporter is running
ps | grep mptcp-metrics-exporter

# Check metrics directory
ls -la /var/run/mptcp-metrics/

# Check for errors
logread | grep mptcp-metrics

# Test QMI/MBIM manually
uqmi -d /dev/cdc-wdm0 --get-signal-info
```

---

## Rollback Procedures

### Rollback All Changes
```bash
# Stop services
/etc/init.d/mptcp-manager stop
/etc/init.d/mptcp-manager disable

# Restore original sysctl
cat > /etc/sysctl.d/99-omr-mptcp-5g-optimization.conf << 'EOF'
net.ipv4.tcp_keepalive_time = 300
net.ipv4.tcp_keepalive_probes = 5
net.ipv4.tcp_keepalive_intvl = 15
EOF
sysctl -p /etc/sysctl.d/99-omr-mptcp-5g-optimization.conf

# Remove new files
rm -f /usr/bin/mptcp-path-manager
rm -f /usr/bin/mptcp-metrics-exporter
rm -f /etc/init.d/mptcp-manager
rm -rf /var/run/mptcp-paths
rm -rf /var/run/mptcp-metrics

# Restore modem monitor from git
cd /path/to/openmptcprouter
git checkout common/package/modems/files/rm551e-monitor.sh

# Restart network
/etc/init.d/network restart
```

### Rollback Individual Components

**Rollback BLEST Scheduler Only**:
```bash
# Use default scheduler
sysctl -w net.mptcp.mptcp_scheduler=default
# Make permanent
uci set network.globals.mptcp_scheduler=default
uci commit
```

**Rollback Aggressive Keepalive Only**:
```bash
sysctl -w net.ipv4.tcp_keepalive_time=300
sysctl -w net.ipv4.tcp_keepalive_probes=5
sysctl -w net.ipv4.tcp_keepalive_intvl=15
```

**Disable Path Manager Only**:
```bash
/etc/init.d/mptcp-manager stop
/etc/init.d/mptcp-manager disable
```

---

## Future Enhancements

### Short-Term (Next Release)
1. **Web UI Integration**
   - Display path states in dashboard
   - Show metrics graphs
   - Manual blacklist/unblacklist controls

2. **Additional Modem Support**
   - Extend coordination to other modem types (Quectel RG520N, Sierra EM series)
   - Generic QMI/MBIM coordination

3. **Tunable Parameters**
   - UCI configuration for hysteresis time
   - Configurable blacklist thresholds
   - Per-interface scheduler selection

### Medium-Term
1. **BPF Scheduler (Kernel 6.12+)**
   - Custom BPF program using exported metrics
   - Weighted fair queueing based on bandwidth
   - Flow-aware scheduling option

2. **Application-Level Probes**
   - HTTP/ICMP health checks
   - Supplement TCP keepalive
   - Detect ISP-specific issues

3. **Historical Analytics**
   - Store metrics to database
   - Trend analysis
   - Predictive failover

### Long-Term
1. **Machine Learning Path Predictor**
   - Learn daily patterns (LTE congestion at 6pm)
   - Proactive path switching
   - Anomaly detection

2. **VPS-Side Coordination**
   - Bidirectional path quality feedback
   - VPS-initiated failover
   - Coordinated scheduling decisions

---

## References

### Code Locations
- **Audit Reports**:
  - `/home/user/openmptcprouter/BONDING_METRICS_AUDIT.md`
  - `/home/user/openmptcprouter/BONDING_FIX_PROPOSAL.md`

- **Implementation Files**:
  - `/common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf`
  - `/common/files/usr/bin/mptcp-path-manager`
  - `/common/files/usr/bin/mptcp-metrics-exporter`
  - `/common/files/etc/init.d/mptcp-manager`
  - `/common/package/modems/files/rm551e-monitor.sh`

### MPTCP Documentation
- Linux MPTCP: https://www.multipath-tcp.org/
- MPTCP RFC 8684: https://tools.ietf.org/html/rfc8684
- BLEST Scheduler Paper: "BLEST: Blocking Estimation-based MPTCP Scheduler for Heterogeneous Networks"

### OpenMPTCProuter
- Project: https://www.openmptcprouter.com/
- GitHub: https://github.com/Ysurac/openmptcprouter

---

## Support

For issues or questions about these fixes:
1. Check logs: `logread | grep -E "mptcp|modem"`
2. Verify state: `cat /var/run/mptcp-paths/*; cat /var/run/mptcp-metrics/summary`
3. Review this documentation for troubleshooting steps
4. Open issue with full logs and configuration

---

**Version**: 1.0
**Last Updated**: 2025-11-18
**Author**: OpenMPTCProuter Bonding Audit Agent
**Status**: Implemented, Ready for Testing
