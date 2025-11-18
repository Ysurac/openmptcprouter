# OpenMPTCProuter - Kernel and Userspace Integration Report

**Investigation Date:** 2025-11-18
**Branch:** claude/investigate-kernel-usage-01JKf7UoncjUPkpGWZhNrx95
**Purpose:** Comprehensive analysis of kernel-userspace communication mechanisms

---

## Executive Summary

OpenMPTCProuter implements a sophisticated multi-layered architecture that integrates kernel-level MPTCP (Multipath TCP) support with userspace management tools to provide seamless multi-WAN connection bonding. The system uses multiple communication channels including netlink sockets, sysfs, ioctl, UCI configuration, and hotplug events to create a cohesive networking solution.

### Key Findings

1. **Multi-Kernel Support**: The project supports kernels 5.4, 6.1, 6.6, 6.10, and 6.12 with version-specific MPTCP optimizations
2. **Advanced MPTCP**: Latest kernels (6.12) support up to 64 concurrent subflows with BPF-based policy management
3. **Automated Modem Integration**: Hotplug-driven initialization with comprehensive driver support for QMI, MBIM, and NCM protocols
4. **Robust Event Architecture**: Event-driven system with automatic device detection and configuration
5. **Safety Mechanisms**: Multiple layers of network lockout prevention and recovery tools

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                    APPLICATION LAYER                             │
├─────────────────────────────────────────────────────────────────┤
│  omr-status  │  network-monitor  │  modem-init  │  autoconfig   │
├─────────────────────────────────────────────────────────────────┤
│                    UCI CONFIGURATION                             │
├─────────────────────────────────────────────────────────────────┤
│  /etc/config/network  │  /etc/config/wireless  │  dhcp/firewall │
├─────────────────────────────────────────────────────────────────┤
│                  KERNEL INTERFACES                               │
├─────────────────────────────────────────────────────────────────┤
│   netlink   │   sysfs   │   ioctl   │   procfs   │   hotplug   │
├─────────────────────────────────────────────────────────────────┤
│                    KERNEL LAYER                                  │
├─────────────────────────────────────────────────────────────────┤
│   MPTCP    │  qmi_wwan  │  cdc_mbim  │  mac80211  │   TCP/IP   │
├─────────────────────────────────────────────────────────────────┤
│                    HARDWARE LAYER                                │
├─────────────────────────────────────────────────────────────────┤
│   Ethernet NICs  │  USB Modems  │  WiFi Radios  │  System Bus  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 1. Kernel Components

### 1.1 MPTCP Evolution Across Kernel Versions

#### Kernel 5.4 - Custom MPTCP v0.96
- **Patch Location**: `5.4/target/linux/generic/hack-5.4/690-mptcp_v0.96.patch` (128KB)
- **Features**:
  - Custom v0.96 implementation with advanced scheduling
  - `CONFIG_MPTCP_FULLMESH` - Full mesh topology support
  - `CONFIG_MPTCP_ROUNDROBIN` - Round-robin packet distribution
  - `CONFIG_MPTCP_NDIFFPORTS` - Multiple ports from single IP
- **Use Case**: Legacy hardware requiring proven stable MPTCP

#### Kernel 6.1 - Mainline MPTCP
- **Configuration**: `6.1/target/linux/generic/config-6.1`
- **Features**:
  - `CONFIG_MPTCP=y` - Mainline kernel MPTCP
  - `CONFIG_MPTCP_IPV6=y` - IPv6 support
  - `CONFIG_INET_MPTCP_DIAG=y` - Diagnostic tools
  - Netlink-based path manager
- **Use Case**: Standard deployments with modern MPTCP

#### Kernel 6.6 - BPF-Enhanced MPTCP
- **Patch Location**: `6.6/target/linux/generic/hack-6.6/999-mptcp-bpf.patch`
- **Features**:
  - BPF hooks for MPTCP socket events
  - Runtime policy modification via eBPF programs
  - Subflow lifecycle management
  - Enhanced stale subflow avoidance
- **Use Case**: Advanced deployments with custom MPTCP policies

#### Kernel 6.12 - Latest MPTCP (Recommended)
- **Patch Locations**:
  - `6.12/target/linux/generic/hack-6.12/999-mptcp-bpf.patch` (19KB)
  - `6.12/target/linux/generic/hack-6.12/999-mptcp-increase-subflows-limits.patch` (1.1KB)
  - `6.12/target/linux/generic/hack-6.12/0002-bbr3.patch` (125KB)
- **Features**:
  - **64 concurrent subflows** (increased from 32)
  - BBR3 congestion control
  - Enhanced BPF integration
  - Improved performance for high-concurrency scenarios
- **Use Case**: Maximum performance with many WAN connections

### 1.2 Network Drivers

#### Modem Drivers
| Driver | Protocol | Usage |
|--------|----------|-------|
| `qmi_wwan` | QMI (Qualcomm) | Primary for Quectel modems |
| `cdc_mbim` | MBIM (Standard) | Fallback/alternative protocol |
| `cdc_ncm` | NCM | Network Control Model |
| `cdc_ether` | CDC Ethernet | Generic USB networking |
| `option` | Serial | AT command interface |
| `usb_wwan` | Generic WWAN | USB modem abstraction |
| `qcserial` | Qualcomm Serial | Qualcomm-specific serial |

#### WiFi Drivers
| Kernel | Driver | Hardware | Features |
|--------|--------|----------|----------|
| 6.12 | mt76 | MediaTek | WiFi 7 (802.11be) support |
| 6.10 | mac80211 | Generic | Wireless stack framework |
| 6.6 | mwlwifi | Marvell | WiFi 5/6 optimization |
| 6.1 | rtl8812au-ct | Realtek | USB WiFi adapters |

### 1.3 BBR Congestion Control
- **BBR3 Patch**: `6.12/target/linux/generic/hack-6.12/0002-bbr3.patch` (125KB)
- **Purpose**: Google's Bottleneck Bandwidth and RTT algorithm
- **Benefits**:
  - Optimized for variable latency (cellular connections)
  - Reduced bufferbloat
  - Better throughput on lossy networks
  - Critical for MPTCP performance

---

## 2. Userspace Components

### 2.1 Modem Management Package
**Location**: `common/package/modems/`

#### Core Scripts

**1. rm551e-init.sh** (246 lines, 7.4KB)
```bash
Purpose: Initialize Quectel RM551E 5G modems
Trigger: Hotplug event on USB insertion
Actions:
  ├─ Detect modem via USB IDs (2c7c:0800/0801/0900/0901)
  ├─ Load 8 kernel modules (qmi_wwan, cdc_mbim, option, etc.)
  ├─ Wait for /dev/ttyUSB* enumeration (30s timeout)
  ├─ Find AT command port (usually /dev/ttyUSB2)
  ├─ Configure via AT commands:
  │   ├─ AT+QCFG="usbnet",0      # Set QMI mode
  │   ├─ AT+QNWCFG="lte_ca",1    # Enable LTE CA
  │   ├─ AT+QNWCFG="endc",1      # Enable 5G EN-DC
  │   └─ AT+QMAP="mpdn_rule"...  # QMI aggregation
  └─ Start rm551e-monitor.sh daemon
```

**2. rm551e-monitor.sh** (9KB)
```bash
Purpose: Continuous modem health monitoring
Features:
  ├─ 30-second health check intervals
  ├─ AT command responsiveness verification
  ├─ RSSI signal strength monitoring
  ├─ Automatic reset after 3 consecutive failures
  └─ PID-based singleton enforcement
```

**3. modem-ca-optimize.sh** (3.5KB)
```bash
Purpose: Carrier aggregation optimization
Actions:
  ├─ Band configuration for local carriers
  ├─ Preference settings for optimal aggregation
  └─ Performance tuning for URB sizes
```

### 2.2 Network Monitoring Tools
**Location**: `common/files/usr/bin/`

#### omr-status (320 lines)
**Primary User Interface for WAN Status**

**Kernel Interfaces Used**:
```bash
# sysfs - Interface state and statistics
/sys/class/net/{iface}/operstate         # Read: UP/DOWN/DORMANT
/sys/class/net/{iface}/speed             # Read: Link speed (Mbps)
/sys/class/net/{iface}/statistics/rx_bytes
/sys/class/net/{iface}/statistics/tx_bytes

# UCI - Configuration reading
uci get network.{wan}.proto              # Read: qmi, mbim, dhcp
uci get network.{wan}.device             # Read: Interface name
uci get network.{wan}.multipath          # Read: MPTCP enabled
uci get network.{wan}.metric             # Read: Path priority

# ioctl - IP address retrieval
ip -4 addr show dev {iface}              # Read: IPv4 address

# QMI/MBIM - Signal strength
uqmi -d /dev/cdc-wdm0 --get-signal-info  # Read: RSSI
umbim -d /dev/cdc-wdm0 -n signal         # Read: Signal quality
```

**Example Output**:
```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  OpenMPTCProuter Optimized - Connection Status
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

WAN Interfaces (3 total):

  wan1
    Type:       Ethernet
    Device:     eth0
    Status:     UP (203.0.113.1)
    Speed:      1000Mbps
    Traffic:    ↓1024MB ↑512MB
    MPTCP:      enabled (metric: 10)

  wan2
    Type:       Cellular (qmi)
    Device:     wwan0
    Status:     UP (198.51.100.5)
    Signal:     -75dBm
    Traffic:    ↓2048MB ↑1024MB
    MPTCP:      enabled (metric: 20)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aggregation Status:
  Active WANs: 2/3
  ✓ MPTCP bonding active across multiple connections
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

#### network-monitor.sh
**Purpose**: Background health monitoring daemon
**Integration**: Managed by procd (OpenWrt's init system)

```bash
Features:
  ├─ START=99 (runs after network service)
  ├─ Respawn on failure (automatic restart)
  ├─ dnsmasq health monitoring
  ├─ First-boot WiFi auto-configuration
  └─ Logging to syslog (logread)
```

#### network-safety-monitor.sh
**Purpose**: Prevent network lockouts

```bash
Monitors:
  ├─ LAN IP integrity (prevents APIPA addresses)
  ├─ DHCP server functionality
  ├─ SSH accessibility
  └─ Web UI availability

Actions on Failure:
  ├─ Trigger emergency-lan-restore.sh
  ├─ Restore last known working config
  ├─ Reset to 192.168.2.1 if needed
  └─ Re-enable DHCP server
```

### 2.3 Auto-Configuration Scripts

#### usb-modem-autoconfig.sh
**Purpose**: Automatic USB modem integration
**Triggered by**: `/etc/hotplug.d/usb/20-usb-modem`

```bash
Actions:
  ├─ Detect modem type (Quectel, Huawei, Sierra, ZTE)
  ├─ Call vendor-specific init script
  ├─ Create UCI network entry:
  │   ├─ network.wwan{N}.proto='qmi' or 'mbim'
  │   ├─ network.wwan{N}.device='/dev/cdc-wdm0'
  │   ├─ network.wwan{N}.multipath='on'
  │   └─ network.wwan{N}.metric=20
  ├─ Commit UCI changes
  └─ Reload network service
```

#### port-autoconfig.sh
**Purpose**: Automatic Ethernet port detection and WAN conversion

```bash
Actions:
  ├─ Scan /sys/class/net/ for ethernet interfaces
  ├─ Identify unconfigured ports (not in UCI)
  ├─ Create WAN interfaces with MPTCP enabled
  └─ Assign sequential metrics (10, 15, 20...)
```

---

## 3. Communication Mechanisms

### 3.1 Netlink - MPTCP Path Manager

**Kernel Interface**: `net/mptcp/pm_netlink.c`
**Purpose**: Manage MPTCP endpoints and subflows

```bash
# Userspace commands
ip mptcp endpoint add 192.168.1.1 dev eth0 signal
ip mptcp endpoint show
ip mptcp limits set subflows 64 add_addr_accepted 64

# Kernel receives via netlink and configures MPTCP
# Attributes:
#   - MPTCP_PM_ATTR_ADDR
#   - MPTCP_PM_ATTR_SUBFLOWS
#   - MPTCP_PM_ATTR_IF_IDX
```

**Integration with OpenMPTCProuter**:
- Automatic endpoint management based on UCI `multipath=on`
- Metric values translate to MPTCP path priorities
- netifd (OpenWrt network daemon) handles netlink communication

### 3.2 sysfs - Interface Statistics and State

**Read Locations**:
```bash
# Interface operational state
/sys/class/net/{iface}/operstate
  Values: up, down, dormant, unknown
  Used by: omr-status, network-monitor.sh

# Link speed (Ethernet)
/sys/class/net/{iface}/speed
  Values: 10, 100, 1000, 2500, 10000 (Mbps)
  Used by: omr-status

# Traffic statistics
/sys/class/net/{iface}/statistics/rx_bytes
/sys/class/net/{iface}/statistics/tx_bytes
/sys/class/net/{iface}/statistics/rx_packets
/sys/class/net/{iface}/statistics/tx_packets
  Used by: omr-status (traffic display)

# Driver information
/sys/class/net/{iface}/device/uevent
  Contains: Vendor ID, Product ID, driver name
  Used by: Hotplug scripts for device identification
```

### 3.3 ioctl - Modem Control

**AT Command Communication**:
```c
// File descriptor: /dev/ttyUSB2 (AT command port)
// Protocol: Hayes AT commands

// Examples from rm551e-init.sh:
write(fd, "AT+QCFG=\"usbnet\",0\r", len)   // Set QMI mode
write(fd, "AT+QNWCFG=\"lte_ca\",1\r", len) // Enable CA
write(fd, "AT+QNWCFG=\"endc\",1\r", len)   // Enable EN-DC
```

**QMI Protocol** (via libqmi):
```bash
# Uses /dev/cdc-wdm0 (WWAN control device)
uqmi -d /dev/cdc-wdm0 --start-network apn=internet
uqmi -d /dev/cdc-wdm0 --get-signal-info
uqmi -d /dev/cdc-wdm0 --get-current-settings

# Kernel driver: qmi_wwan
# Protocol: QMI (Qualcomm MSM Interface)
```

### 3.4 UCI - Unified Configuration Interface

**Configuration Flow**:
```
User Action (Web UI / CLI)
    ↓
uci set network.wan1.multipath='on'
    ↓
uci commit network
    ↓
/etc/init.d/network reload
    ↓
netifd reads /etc/config/network
    ↓
netifd calls protocol handlers
    ↓
Protocol handlers (qmi.sh, mbim.sh, dhcp) configure interfaces
    ↓
Kernel brings up interfaces with MPTCP enabled
```

**Critical UCI Settings**:
```bash
# Network configuration (/etc/config/network)
network.lan.proto='static'        # MUST be static (never DHCP)
network.lan.ipaddr='192.168.2.1'  # LAN IP
network.wan1.proto='dhcp'         # WAN protocol
network.wan1.device='eth0'        # Physical interface
network.wan1.multipath='on'       # MPTCP bonding
network.wan1.metric='10'          # MPTCP priority (lower = higher priority)
```

### 3.5 Hotplug - Event-Driven Integration

**Hotplug Architecture**:
```
Kernel Event (USB device insertion)
    ↓
Hotplugd daemon receives event
    ↓
Environment variables set:
  - ACTION=add
  - DEVTYPE=usb_device
  - PRODUCT=2c7c/0800/0
  - DEVPATH=/devices/...
    ↓
Execute: /etc/hotplug.d/usb/20-usb-modem
    ↓
Script detects Quectel modem (2c7c/*)
    ↓
Calls: /usr/bin/usb-modem-autoconfig.sh &
    ↓
Background: Initialize and configure modem
```

**Hotplug Script** (`/etc/hotplug.d/usb/20-usb-modem`):
```bash
#!/bin/sh
[ "$ACTION" = "add" ] || exit 0

case "$PRODUCT" in
    2c7c/*) logger -t usb-modem "Quectel detected: $PRODUCT" ;;
    12d1/*) logger -t usb-modem "Huawei detected: $PRODUCT" ;;
    1199/*) logger -t usb-modem "Sierra detected: $PRODUCT" ;;
    19d2/*) logger -t usb-modem "ZTE detected: $PRODUCT" ;;
    *) exit 0 ;;
esac

sleep 5  # Wait for enumeration
/usr/bin/usb-modem-autoconfig.sh &
```

---

## 4. Integration Flows

### 4.1 USB Modem Insertion Flow

```
┌──────────────────────────────────────────────────────────────┐
│ 1. HARDWARE LAYER                                             │
│    User plugs Quectel RM551E modem into USB port            │
└──────────────────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────────────────┐
│ 2. KERNEL LAYER                                               │
│    ├─ USB subsystem detects device (2c7c:0800)              │
│    ├─ Generates hotplug event                                │
│    └─ Sets environment: ACTION=add, PRODUCT=2c7c/0800/0     │
└──────────────────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────────────────┐
│ 3. HOTPLUG LAYER                                              │
│    ├─ hotplugd executes /etc/hotplug.d/usb/20-usb-modem     │
│    ├─ Script detects Quectel (2c7c/*)                       │
│    └─ Calls: /usr/bin/usb-modem-autoconfig.sh &             │
└──────────────────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────────────────┐
│ 4. INITIALIZATION LAYER                                       │
│    ├─ usb-modem-autoconfig.sh waits 5s for enumeration      │
│    ├─ Calls: /usr/bin/rm551e-init.sh                        │
│    └─ rm551e-init.sh executes:                               │
│         ├─ modprobe qmi_wwan, cdc_mbim, option              │
│         ├─ Wait for /dev/ttyUSB* (30s timeout)              │
│         ├─ Find AT port (/dev/ttyUSB2)                      │
│         ├─ Configure via AT commands:                        │
│         │   ├─ AT+QCFG="usbnet",0 (QMI mode)                │
│         │   ├─ AT+QNWCFG="lte_ca",1 (Enable CA)             │
│         │   └─ AT+QNWCFG="endc",1 (Enable 5G EN-DC)         │
│         └─ Start rm551e-monitor.sh daemon                    │
└──────────────────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────────────────┐
│ 5. CONFIGURATION LAYER                                        │
│    ├─ usb-modem-autoconfig.sh creates UCI entry:            │
│    │   uci set network.wwan1='interface'                    │
│    │   uci set network.wwan1.proto='qmi'                    │
│    │   uci set network.wwan1.device='/dev/cdc-wdm0'         │
│    │   uci set network.wwan1.multipath='on'                 │
│    │   uci set network.wwan1.metric='20'                    │
│    │   uci commit network                                    │
│    └─ Triggers: /etc/init.d/network reload                   │
└──────────────────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────────────────┐
│ 6. NETWORK DAEMON LAYER                                       │
│    ├─ netifd reads /etc/config/network                      │
│    ├─ Executes protocol handler: /lib/netifd/proto/qmi.sh   │
│    ├─ qmi.sh calls: uqmi --start-network                    │
│    └─ Configures interface with MPTCP enabled                │
└──────────────────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────────────────┐
│ 7. KERNEL LAYER                                               │
│    ├─ qmi_wwan driver creates wwan0 interface               │
│    ├─ Interface gets IP via QMI protocol                    │
│    ├─ MPTCP path manager adds endpoint                      │
│    │   ip mptcp endpoint add <IP> dev wwan0                 │
│    └─ MPTCP subflow created with metric 20                   │
└──────────────────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────────────────┐
│ 8. MONITORING LAYER                                           │
│    ├─ network-monitor.sh detects new interface              │
│    ├─ omr-status shows:                                      │
│    │   wan2 (Cellular-QMI): UP (IP) Signal:-75dBm           │
│    │   MPTCP: enabled (metric:20)                           │
│    └─ rm551e-monitor.sh starts health checks                 │
└──────────────────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────────────────┐
│ 9. RESULT                                                     │
│    ✓ Modem fully operational                                 │
│    ✓ MPTCP bonding active                                    │
│    ✓ Traffic flows through multiple paths                   │
└──────────────────────────────────────────────────────────────┘
```

### 4.2 MPTCP Bonding Configuration Flow

```
USER: Enables multipath on WAN1 and WAN2 via Web UI
    ↓
Web UI calls UCI commands:
    uci set network.wan1.multipath='on'
    uci set network.wan1.metric='10'
    uci set network.wan2.multipath='on'
    uci set network.wan2.metric='20'
    uci commit network
    ↓
Web UI triggers: /etc/init.d/network reload
    ↓
netifd reads new configuration:
    Detects: wan1 and wan2 have multipath=on
    ↓
netifd brings up interfaces with MPTCP flags
    ↓
KERNEL MPTCP Path Manager (via netlink):
    ├─ Receives endpoint notifications
    ├─ Adds MPTCP endpoints:
    │   ├─ 203.0.113.1 (wan1) - metric 10
    │   └─ 198.51.100.5 (wan2) - metric 20
    ├─ Creates subflows for both paths
    └─ MPTCP scheduler distributes packets:
        ├─ FULLMESH mode: All paths to all destinations
        ├─ Priority based on metric (10 > 20)
        └─ Automatic failover if path fails
    ↓
APPLICATION CONNECTIONS:
    ├─ New TCP sockets automatically use MPTCP
    ├─ Traffic distributed across both WANs
    ├─ Combined bandwidth = wan1_bandwidth + wan2_bandwidth
    └─ Transparent failover on link failure
    ↓
omr-status displays:
    WAN1: UP (203.0.113.1) MPTCP:enabled (metric:10)
    WAN2: UP (198.51.100.5) MPTCP:enabled (metric:20)
    ✓ MPTCP bonding active across 2 connections
```

### 4.3 First Boot Initialization

```
System First Boot
    ↓
UCI Defaults Execution (priority order):
    ↓
┌─────────────────────────────────────────────────────┐
│ /etc/uci-defaults/10-omr-network-defaults           │
│   ├─ Set LAN IP: 192.168.2.1/24 (STATIC)           │
│   ├─ Enable DHCP server (192.168.2.100-250)        │
│   ├─ Configure DNS (dnsmasq)                        │
│   ├─ Set hostname: OMR-Optimized                    │
│   ├─ Mark: touch /etc/omr-network-configured        │
│   └─ Reload: network, dnsmasq                       │
└─────────────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────────────┐
│ /etc/uci-defaults/15-omr-autoconfig-init            │
│   ├─ chmod +x /usr/bin/*.sh                         │
│   ├─ Create /etc/init.d/network-monitor             │
│   ├─ Create /etc/init.d/network-safety              │
│   ├─ Enable services: START=98, START=99            │
│   └─ Start monitoring daemons                       │
└─────────────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────────────┐
│ /etc/uci-defaults/90-omr-first-boot-wizard          │
│   └─ Enable LuCI first-boot wizard                  │
│       ├─ VPS pairing code entry                     │
│       ├─ Auto-discovery setup                       │
│       └─ Manual configuration                        │
└─────────────────────────────────────────────────────┘
    ↓
System Ready:
    ├─ LAN: 192.168.2.1 (accessible)
    ├─ DHCP: Serving 192.168.2.100-250
    ├─ Monitoring: Active (network-monitor, network-safety)
    └─ Web UI: http://192.168.2.1 (wizard appears)
```

---

## 5. File System Integration Points

### 5.1 Critical Kernel Patches

```
6.12/target/linux/generic/hack-6.12/
├── 0002-bbr3.patch (125KB)
│   └─ BBR3 congestion control for MPTCP
├── 999-mptcp-bpf.patch (19KB)
│   └─ eBPF hooks for MPTCP policy enforcement
└── 999-mptcp-increase-subflows-limits.patch (1.1KB)
    └─ Increase max subflows from 32 to 64

6.6/target/linux/generic/hack-6.6/
└── 999-mptcp-bpf.patch
    └─ BPF integration for kernel 6.6

5.4/target/linux/generic/hack-5.4/
└── 690-mptcp_v0.96.patch (128KB)
    └─ Full custom MPTCP v0.96 implementation
```

### 5.2 Critical Userspace Scripts

```
common/files/
├── etc/
│   ├── uci-defaults/
│   │   ├── 10-omr-network-defaults       (72 lines)
│   │   ├── 15-omr-autoconfig-init         (Creates init.d scripts)
│   │   └── 90-omr-first-boot-wizard       (LuCI wizard)
│   └── hotplug.d/usb/
│       └── 20-usb-modem                   (45 lines)
└── usr/bin/
    ├── omr-status                         (320 lines)
    ├── network-monitor.sh                 (Health monitoring)
    ├── network-safety-monitor.sh          (Lockout prevention)
    ├── usb-modem-autoconfig.sh           (Modem UCI integration)
    ├── port-autoconfig.sh                 (Ethernet detection)
    ├── wifi-autoconfig.sh                 (WiFi setup)
    └── emergency-lan-restore.sh           (Recovery)

common/package/modems/files/
├── rm551e-init.sh                         (246 lines)
├── rm551e-monitor.sh                      (9KB)
└── modem-ca-optimize.sh                   (3.5KB)
```

### 5.3 Configuration Files

```
/etc/config/
├── network          (UCI network configuration)
│   ├── LAN interfaces (static IP)
│   ├── WAN interfaces (dhcp/qmi/mbim)
│   ├── Multipath settings (MPTCP enable)
│   └── Metrics (path priority)
├── wireless         (WiFi configuration)
├── dhcp             (DHCP/DNS server)
└── firewall         (Firewall rules)
```

---

## 6. Debugging and Monitoring

### 6.1 Kernel-Level Debugging

```bash
# MPTCP endpoint monitoring
ip mptcp monitor
ip mptcp endpoint show
ip mptcp limits show

# MPTCP statistics
cat /proc/net/mptcp_pm_addr
cat /proc/net/mptcp_net

# TCP metrics
cat /proc/net/tcp_metrics_nl

# Interface state
cat /sys/class/net/wwan0/operstate
cat /sys/class/net/wwan0/speed
cat /sys/class/net/wwan0/statistics/rx_bytes
```

### 6.2 Userspace Monitoring

```bash
# Connection status
omr-status
omr-status --plain

# System logs
logread -f
logread | grep network-monitor
logread | grep rm551e
logread | grep usb-modem

# UCI configuration
uci show network
uci show wireless
uci show dhcp

# Network interfaces
ip addr show
ip route show
ip mptcp endpoint show
```

### 6.3 Modem Diagnostics

```bash
# QMI modems
uqmi -d /dev/cdc-wdm0 --get-signal-info
uqmi -d /dev/cdc-wdm0 --get-current-settings
uqmi -d /dev/cdc-wdm0 --get-data-status

# MBIM modems
umbim -d /dev/cdc-wdm0 -n signal
umbim -d /dev/cdc-wdm0 -n registration
umbim -d /dev/cdc-wdm0 -n connect

# AT commands (direct)
echo -e 'AT+QCSQ\r' > /dev/ttyUSB2    # Signal quality
cat /dev/ttyUSB2

echo -e 'AT+QNWINFO\r' > /dev/ttyUSB2  # Network info
cat /dev/ttyUSB2
```

---

## 7. Security and Safety Mechanisms

### 7.1 Network Lockout Prevention

**Problem**: Users can misconfigure network and lose access

**Solution**: Multi-layer safety net

```bash
Layer 1: Static LAN IP enforcement
  ├─ LAN MUST be static (never DHCP)
  ├─ Prevents APIPA addresses (169.254.x.x)
  └─ Enforced by: 10-omr-network-defaults

Layer 2: Network safety monitor
  ├─ Monitors LAN IP accessibility
  ├─ Checks DHCP server functionality
  ├─ Triggers emergency restore if needed
  └─ Script: network-safety-monitor.sh

Layer 3: Emergency restore
  ├─ Accessible via hardware reset button
  ├─ Restores LAN to 192.168.2.1
  ├─ Re-enables DHCP server
  └─ Script: emergency-lan-restore.sh
```

### 7.2 Modem Stability

**Problem**: Modems can hang or become unresponsive

**Solution**: Continuous health monitoring

```bash
rm551e-monitor.sh:
  ├─ 30-second health check intervals
  ├─ AT command ping test
  ├─ RSSI signal monitoring
  ├─ Failure counter (3 strikes)
  └─ Automatic modem reset on failure
```

---

## 8. Performance Optimizations

### 8.1 MPTCP Scheduler Selection

**Available Schedulers** (kernel-dependent):

```bash
Kernel 5.4 (MPTCP v0.96):
  ├─ FULLMESH       (all paths to all destinations)
  ├─ ROUNDROBIN     (distribute packets evenly)
  ├─ REDUNDANT      (send on all paths - reliability)
  └─ NDIFFPORTS     (multiple connections from one IP)

Kernel 6.1+:
  ├─ default        (kernel default scheduler)
  ├─ roundrobin     (round-robin packet distribution)
  └─ redundant      (reliability over efficiency)
```

### 8.2 BBR Congestion Control

**BBR3 Benefits**:
- Optimized for high-latency cellular links
- Reduced bufferbloat (important for MPTCP)
- Better throughput on lossy networks
- Adaptive to varying RTT

**Configuration**:
```bash
# Applied automatically via kernel patch
# No userspace configuration needed
sysctl net.ipv4.tcp_congestion_control  # Shows: bbr
```

### 8.3 Modem Optimization

**Quectel RM551E Tuning** (from rm551e-init.sh):

```bash
# USB mode: QMI (fastest)
AT+QCFG="usbnet",0

# Carrier aggregation: Enabled
AT+QNWCFG="lte_ca",1
AT+QNWCFG="nr5g_carrier_aggregation",1

# EN-DC (5G+LTE): Enabled
AT+QNWCFG="endc",1

# QMI aggregation: Optimized for throughput
AT+QMAP="mpdn_rule",1,1,0,1,1,"INTERNET"
```

---

## 9. Recommendations

### 9.1 For Production Deployments

1. **Use Kernel 6.12** for latest MPTCP features and 64 subflow support
2. **Enable BBR3** for optimal cellular performance
3. **Set appropriate metrics**: Lower metric = higher priority (10, 15, 20, 25...)
4. **Monitor with omr-status**: Regular checks for connection health
5. **Enable health monitors**: Ensure network-monitor and network-safety are running

### 9.2 For Development

1. **Use Kernel 6.6 with BPF** for custom MPTCP policy development
2. **Enable MPTCP diagnostics**: `CONFIG_INET_MPTCP_DIAG=y`
3. **Monitor kernel logs**: `dmesg | grep mptcp`
4. **Use netlink tools**: `ip mptcp monitor` for real-time events
5. **Test failover scenarios**: Unplug cables, disable modems

### 9.3 For Troubleshooting

1. **Check logs first**: `logread | grep -E "network|modem|mptcp"`
2. **Verify UCI config**: `uci show network`
3. **Check interface state**: `cat /sys/class/net/*/operstate`
4. **Test modem AT port**: `echo -e 'AT\r' > /dev/ttyUSB2 && cat /dev/ttyUSB2`
5. **Use omr-recovery**: Built-in diagnostic and recovery tool

---

## 10. Conclusion

OpenMPTCProuter demonstrates a sophisticated integration between kernel and userspace:

### Strengths

1. **Comprehensive Automation**: From modem insertion to MPTCP bonding, most processes are automated
2. **Multi-Kernel Support**: Flexible deployment across 5 kernel versions with appropriate optimizations
3. **Robust Event Handling**: Hotplug-driven architecture ensures devices are automatically configured
4. **Safety Mechanisms**: Multiple layers prevent network lockouts and modem failures
5. **Modern Technologies**: BPF integration, BBR3, WiFi 7 support

### Architecture Highlights

1. **Clear Separation**: Kernel handles MPTCP/drivers, userspace handles configuration/monitoring
2. **Standard Interfaces**: Relies on netlink, sysfs, ioctl - no proprietary protocols
3. **Event-Driven**: Hotplug events trigger automatic device integration
4. **Configuration-Driven**: UCI provides centralized, version-controlled configuration
5. **Monitoring-First**: Built-in health checks and status dashboards

### Future Enhancements

1. **BPF MPTCP Policies**: Custom schedulers via eBPF (6.6+)
2. **Increased Subflows**: Support 64+ concurrent connections (6.12)
3. **WiFi 7 Integration**: Full 802.11be MPTCP bonding
4. **Enhanced Telemetry**: MPTCP statistics export to web UI
5. **Automated Optimization**: ML-based path selection and scheduling

---

## Appendix: File Path Reference

### Complete File Paths

**Kernel Patches**:
- `/home/user/openmptcprouter/5.4/target/linux/generic/hack-5.4/690-mptcp_v0.96.patch`
- `/home/user/openmptcprouter/6.6/target/linux/generic/hack-6.6/999-mptcp-bpf.patch`
- `/home/user/openmptcprouter/6.12/target/linux/generic/hack-6.12/999-mptcp-bpf.patch`
- `/home/user/openmptcprouter/6.12/target/linux/generic/hack-6.12/999-mptcp-increase-subflows-limits.patch`
- `/home/user/openmptcprouter/6.12/target/linux/generic/hack-6.12/0002-bbr3.patch`

**Userspace Scripts**:
- `/home/user/openmptcprouter/common/package/modems/files/rm551e-init.sh`
- `/home/user/openmptcprouter/common/package/modems/files/rm551e-monitor.sh`
- `/home/user/openmptcprouter/common/files/usr/bin/omr-status`
- `/home/user/openmptcprouter/common/files/usr/bin/network-monitor.sh`
- `/home/user/openmptcprouter/common/files/etc/hotplug.d/usb/20-usb-modem`
- `/home/user/openmptcprouter/common/files/etc/uci-defaults/10-omr-network-defaults`

**Detailed Analysis Document**:
- `/home/user/openmptcprouter/KERNEL_USERSPACE_ANALYSIS.md`

---

**Report Generated**: 2025-11-18
**Investigation Branch**: claude/investigate-kernel-usage-01JKf7UoncjUPkpGWZhNrx95
**Status**: Complete
