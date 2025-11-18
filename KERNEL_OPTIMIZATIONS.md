# Kernel and Network Stack Optimizations for MPTCP and 5G Modems

This document describes the comprehensive kernel and network stack optimizations applied to OpenMPTCProuter for optimal MPTCP performance and 5G modem support.

## Overview

The optimizations target three kernel versions:
- **Kernel 6.12** (latest)
- **Kernel 6.10** (stable)
- **Kernel 6.6** (LTS)

## Kernel Configuration Changes

### 1. TCP Congestion Control Algorithms

#### Kernel 6.12 Enhancements
**File**: `6.12/target/linux/generic/config-6.12`

Enabled advanced TCP congestion control algorithms optimized for high-latency, high-bandwidth 5G links:

- `CONFIG_TCP_CONG_BBR=y` - **Bottleneck Bandwidth and RTT** (Google BBR) - Ideal for 5G
- `CONFIG_TCP_CONG_HSTCP=y` - HighSpeed TCP for high-bandwidth paths
- `CONFIG_TCP_CONG_HTCP=y` - Hamilton TCP
- `CONFIG_TCP_CONG_HYBLA=y` - Optimized for satellite/high-latency links
- `CONFIG_TCP_CONG_ILLINOIS=y` - Loss and delay-based algorithm
- `CONFIG_TCP_CONG_SCALABLE=y` - Scalable TCP for high-speed networks
- `CONFIG_TCP_CONG_VEGAS=y` - Vegas congestion avoidance
- `CONFIG_TCP_CONG_VENO=y` - Veno for wireless networks
- `CONFIG_TCP_CONG_WESTWOOD=y` - Westwood+ for wireless
- `CONFIG_TCP_CONG_YEAH=y` - YeAH-TCP hybrid algorithm

**Impact**: Provides multiple congestion control options for different network conditions. BBR is particularly effective for 5G modems with variable latency.

#### Kernels 6.10 and 6.6
These already had BBR, BBR1, and BBR2 enabled with full congestion control support.

---

### 2. Network Packet Schedulers (QoS)

Enabled comprehensive traffic shaping and QoS capabilities across all kernel versions:

#### Key Schedulers Enabled:

- `CONFIG_NET_SCH_CAKE=y` - **Common Applications Kept Enhanced**
  - Best-in-class scheduler for WAN bonding
  - Automatic per-flow fairness
  - Built-in traffic shaping

- `CONFIG_NET_SCH_FQ=y` - **Fair Queue**
  - Works optimally with BBR congestion control
  - Per-flow pacing support
  - Essential for high-speed links

- `CONFIG_NET_SCH_HTB=y` - **Hierarchical Token Bucket**
  - Advanced traffic shaping
  - Per-class bandwidth guarantees

- `CONFIG_NET_SCH_HFSC=y` - **Hierarchical Fair Service Curve**
  - Precise latency and bandwidth control
  - Ideal for real-time traffic over bonded WANs

- `CONFIG_NET_SCH_CODEL=y` - **Controlled Delay**
  - Active Queue Management (AQM)
  - Reduces bufferbloat

- `CONFIG_NET_SCH_FQ_CODEL=y` - Combined Fair Queue + CoDel
- `CONFIG_NET_SCH_FQ_PIE=y` - FQ with PIE AQM
- `CONFIG_NET_SCH_DRR=y` - Deficit Round Robin
- `CONFIG_NET_SCH_HHF=y` - Heavy-Hitter Filter
- `CONFIG_NET_SCH_PRIO=y` - Priority queuing
- `CONFIG_NET_SCH_QFQ=y` - Quick Fair Queueing
- `CONFIG_NET_SCH_RED=y` - Random Early Detection
- `CONFIG_NET_SCH_SFB=y` - Stochastic Fair Blue
- `CONFIG_NET_SCH_SFQ=y` - Stochastic Fair Queueing
- `CONFIG_NET_SCH_TBF=y` - Token Bucket Filter
- `CONFIG_NET_SCH_NETEM=y` - Network emulation (testing)
- `CONFIG_NET_SCH_PIE=y` - Proportional Integral controller Enhanced
- `CONFIG_NET_SCH_INGRESS=y` - Ingress traffic control

**Impact**: Enables sophisticated traffic shaping, QoS, and bufferbloat mitigation critical for WAN bonding with multiple 5G modems.

---

### 3. Network Classifiers and Actions

Enabled traffic classification and manipulation capabilities:

#### Classifiers:
- `CONFIG_NET_CLS_ACT=y` - Actions on classified packets
- `CONFIG_NET_CLS_BASIC=y` - Basic packet classifier
- `CONFIG_NET_CLS_FLOW=y` - Flow-based classifier
- `CONFIG_NET_CLS_FLOWER=y` - Flow-based classifier with actions
- `CONFIG_NET_CLS_FW=y` - Firewall mark-based classifier
- `CONFIG_NET_CLS_MATCHALL=y` - Match all packets
- `CONFIG_NET_CLS_ROUTE4=y` - Route-based classifier
- `CONFIG_NET_CLS_U32=y` - U32 packet classifier

#### Actions:
- `CONFIG_NET_ACT_CSUM=y` - Checksum updating
- `CONFIG_NET_ACT_CT=y` - Connection tracking
- `CONFIG_NET_ACT_GACT=y` - Generic actions
- `CONFIG_NET_ACT_MIRRED=y` - Traffic mirroring/redirect
- `CONFIG_NET_ACT_NAT=y` - Network Address Translation
- `CONFIG_NET_ACT_PEDIT=y` - Packet editing
- `CONFIG_NET_ACT_POLICE=y` - Traffic policing
- `CONFIG_NET_ACT_SIMP=y` - Simple actions
- `CONFIG_NET_ACT_SKBEDIT=y` - SKB editing
- `CONFIG_NET_ACT_SKBMOD=y` - SKB modification
- `CONFIG_NET_ACT_VLAN=y` - VLAN manipulation

**Impact**: Enables advanced packet classification, manipulation, and QoS marking for intelligent traffic routing across bonded WANs.

---

## Runtime Network Stack Optimizations (sysctl)

### Router-Side Configuration
**File**: `common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf`

### VPS-Side Configuration
**File**: `vps-scripts/omr-vps-install.sh` (enhanced sysctl section)

### Key Optimizations:

#### 1. Network Buffer Sizes (Critical for 5G)

```
net.core.rmem_max = 134217728        # 128MB receive buffer
net.core.wmem_max = 134217728        # 128MB send buffer
net.core.rmem_default = 67108864     # 64MB default receive
net.core.wmem_default = 67108864     # 64MB default send
```

**Impact**: Essential for high-bandwidth 5G links. Default Linux buffers (4MB) severely limit throughput on high-speed, high-latency paths.

#### 2. TCP Buffer Tuning

```
net.ipv4.tcp_rmem = 4096 131072 134217728
net.ipv4.tcp_wmem = 4096 131072 134217728
```

- Min: 4KB (connection minimum)
- Default: 128KB (optimal for most connections)
- Max: 128MB (for high-bandwidth flows)

**Impact**: Automatically scales TCP windows for optimal throughput on 5G links.

#### 3. Network Device Backlog

```
net.core.netdev_max_backlog = 300000
net.core.netdev_budget = 600
net.core.netdev_budget_usecs = 8000
```

**Impact**: Handles high packet rates from multiple 5G modems without dropping packets.

#### 4. TCP Performance Tuning

```
net.ipv4.tcp_window_scaling = 1           # Enable large windows (essential for 5G)
net.ipv4.tcp_slow_start_after_idle = 0    # Maintain speed after idle
net.ipv4.tcp_fastopen = 3                 # TFO for reduced latency
net.ipv4.tcp_mtu_probing = 1              # Discover optimal MTU
net.ipv4.tcp_no_metrics_save = 1          # Don't cache metrics (better for dynamic paths)
```

**Impact**: Optimizes TCP for variable latency and changing network conditions common with mobile 5G.

#### 5. Connection Tracking

```
net.netfilter.nf_conntrack_max = 524288   # 512K connections
net.nf_conntrack_max = 524288
```

**Impact**: Supports high connection counts from bonded multi-WAN setup.

#### 6. Multipath Routing

```
net.ipv4.fib_multipath_hash_policy = 1    # Layer 4 hash (better load balancing)
net.ipv4.fib_multipath_use_neigh = 1      # Use neighbor cache for multipath
```

**Impact**: Better traffic distribution across multiple WAN interfaces.

#### 7. Queue Discipline

```
net.core.default_qdisc = fq               # Fair Queue (optimal for BBR)
```

**Impact**: FQ scheduler works optimally with BBR congestion control.

#### 8. ARP and Neighbor Cache

```
net.ipv4.neigh.default.gc_thresh1 = 2048
net.ipv4.neigh.default.gc_thresh2 = 4096
net.ipv4.neigh.default.gc_thresh3 = 8192
```

**Impact**: Larger neighbor cache for multiple interfaces and higher connection counts.

---

## Performance Impact

### Expected Improvements:

1. **5G Throughput**:
   - Up to 2-3x improvement on high-latency 5G links
   - Better utilization of available bandwidth
   - Reduced packet loss from buffer overflows

2. **WAN Bonding**:
   - More efficient traffic distribution
   - Better per-flow fairness with CAKE scheduler
   - Reduced bufferbloat with FQ-CoDel/CAKE

3. **MPTCP Performance**:
   - Better subflow management with BBR
   - Improved failover handling
   - More efficient use of multiple paths

4. **Latency**:
   - Reduced bufferbloat (10-100ms improvement)
   - TCP Fast Open reduces connection setup time
   - Better AQM with CoDel/PIE

5. **Connection Capacity**:
   - Support for 512K concurrent connections
   - Better handling of NAT on multi-WAN setups

---

## Configuration Files Modified

### Kernel Configs:
1. `6.12/target/linux/generic/config-6.12`
2. `6.10/target/linux/generic/config-6.10`
3. `6.6/target/linux/generic/config-6.6`

### Runtime Configs:
1. `common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf` (new)
2. `vps-scripts/omr-vps-install.sh` (enhanced)

---

## Existing Optimizations (Unchanged)

The following optimizations were already present and remain active:

1. **MPTCP Support**:
   - Kernels 6.6, 6.10, 6.12 have MPTCP enabled
   - Kernel 5.4 has MPTCP v0.96 patch applied

2. **BBR Congestion Control**:
   - Kernels 6.6 and 6.10 have BBR1, BBR2 enabled
   - Kernel 6.12 now has BBR enabled
   - BBR3 patches available in 6.12

3. **5G Modem Optimizations**:
   - Quectel 5G modem patches (RM551E-GL, RM520N)
   - Increased MTU to 32768 for aggregated throughput
   - Hardware offload (TSO, checksum, scatter-gather)

---

## Testing and Validation

### Recommended Tests:

1. **Throughput Testing**:
   ```bash
   iperf3 -c <server> -P 10 -t 60 -C bbr
   ```

2. **Bufferbloat Testing**:
   ```bash
   # Run during heavy load
   ping -c 100 8.8.8.8
   ```
   Should see minimal latency increase under load with CAKE/FQ-CoDel.

3. **MPTCP Verification**:
   ```bash
   ss -tni | grep mptcp
   ```

4. **Congestion Control Verification**:
   ```bash
   sysctl net.ipv4.tcp_congestion_control
   ```

---

## References

- **BBR**: https://research.google/pubs/pub45646/
- **CAKE**: https://www.bufferbloat.net/projects/codel/wiki/CakeTechnical/
- **MPTCP**: https://www.multipath-tcp.org/
- **TCP Tuning**: https://www.kernel.org/doc/Documentation/networking/ip-sysctl.txt

---

## Notes

- BBR works best with FQ (Fair Queue) scheduler
- CAKE provides comprehensive bufferbloat mitigation
- Large buffers (128MB) are essential for high-bandwidth delay products (5G)
- Connection tracking limits increased for high-connection scenarios
- These optimizations are particularly effective for:
  - Multiple bonded 5G modems
  - High-latency satellite/cellular links
  - Variable bandwidth conditions
  - High-throughput applications

---

**Last Updated**: 2025-11-18
**Applied to**: OpenMPTCProuter kernels 6.6, 6.10, 6.12
