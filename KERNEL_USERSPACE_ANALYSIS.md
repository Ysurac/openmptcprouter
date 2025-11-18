# OpenMPTCProuter - Kernel and Userspace Integration Analysis

## Project Overview
OpenMPTCProuter is an OpenWrt-based multipath TCP (MPTCP) router that enables bonding of multiple WAN connections (ethernet, modems, WiFi) using kernel MPTCP support. The project targets multiple kernel versions (5.4, 6.1, 6.6, 6.10, 6.12) with version-specific optimizations.

---

## 1. KERNEL MODULES & MPTCP COMPONENTS

### 1.1 MPTCP Kernel Configuration

**Kernel Versions and MPTCP Support:**

- **5.4**: Custom MPTCP v0.96 implementation with advanced scheduler
  - Path: `/5.4/target/linux/generic/hack-5.4/690-mptcp_v0.96.patch`
  - CONFIG_MPTCP_FULLMESH, CONFIG_MPTCP_ROUNDROBIN, CONFIG_MPTCP_NDIFFPORTS enabled
  - Advanced PM (Path Management) and Scheduling features

- **6.1**: Mainline MPTCP with IPv6 support
  - CONFIG_MPTCP=y, CONFIG_MPTCP_IPV6=y, CONFIG_INET_MPTCP_DIAG=y
  - Integrated netlink-based path manager

- **6.6**: Enhanced MPTCP with BPF support
  - Path: `/6.6/target/linux/generic/hack-6.6/999-mptcp-bpf.patch`
  - Subflow ACK improvements for stale subflow avoidance
  - BPF hooks for MPTCP policy enforcement

- **6.10**: Latest upstream with IPv6 and diagnostics
  - CONFIG_MPTCP=y, CONFIG_MPTCP_IPV6=y, CONFIG_INET_MPTCP_DIAG=y

- **6.12**: Cutting-edge with MPTCP enhancements
  - Path: `/6.12/target/linux/generic/hack-6.12/999-mptcp-bpf.patch`
  - Path: `/6.12/target/linux/generic/hack-6.12/999-mptcp-increase-subflows-limits.patch`
  - Increased subflow limits (up to 64) for high-concurrency scenarios
  - BPF-based subflow management

### 1.2 Kernel Patches Details

**MPTCP BPF Patch (6.6, 6.12):**
- Enables BPF programs to attach to MPTCP socket hooks
- Allows runtime policy modification without kernel recompilation
- Subflow lifecycle management via eBPF

**Subflow Limits Patch (6.12):**
```c
// Allows configuration of maximum subflows and address limits
// Default increased from 32 to 64 subflows max
// Configurable via netlink MPTCP_PM_ATTR_SUBFLOWS
```

### 1.3 WiFi & Network Drivers

**Custom Kernel Modules:**
- **mwlwifi** (6.6): Marvell WiFi driver with OMR patches
- **rtl8812au-ct** (6.1): Realtek USB WiFi driver with kernel 6.1 compatibility
- **mt76** (6.12): MediaTek WiFi7 with performance optimizations
- **mac80211** (6.10): Linux wireless subsystem
- **nat46** (6.12): NAT46/TAT464 for IPv4/IPv6 translation

**BPF Compilation Infrastructure:**
- Path: `/common/include/bpf_mptcp.mk`
- LLVM/Clang-based eBPF compiler for kernel 6.12+
- BPF headers for MPTCP socket tracing and management

### 1.4 Modem Support Kernel Modules

**Modem Drivers:**
- `option`: USB serial driver for modems
- `usb_wwan`: Generic USB WWAN driver
- `qmi_wwan`: QMI network device driver (Quectel RM551E, etc.)
- `cdc_mbim`: MBIM protocol driver
- `cdc_ncm`: NCM protocol driver
- `cdc_ether`: CDC Ethernet support
- `cdc_wdm`: Wireless Device Management

---

## 2. USERSPACE COMPONENTS

### 2.1 Modem Management Package

**Location:** `/common/package/modems/`

**Core Scripts:**

1. **rm551e-init.sh** (7.4 KB)
   - Quectel RM551E 5G modem initialization
   - Detects modem via USB IDs (2c7c:0800, 0801, 0900, 0901)
   - Loads kernel drivers: qmi_wwan, cdc_mbim, cdc_ncm
   - Configures modem USB mode (QMI mode 0 preferred over MBIM)
   - Enables carrier aggregation (LTE CA and 5G NR CA)
   - Enables EN-DC (E-UTRA-NR Dual Connectivity)
   - Sets optimal URB/QMI aggregation for throughput
   - 3 retry attempts for robustness

2. **rm551e-monitor.sh** (9 KB)
   - Continuous modem health monitoring
   - Detects modem responsiveness via AT commands
   - Monitors signal strength via RSSI
   - Automatic reset after 3 consecutive failures
   - PID-based singleton enforcement
   - 30-second check intervals

3. **modem-ca-optimize.sh** (3.5 KB)
   - Carrier aggregation optimization for LTE/5G
   - Band configuration and preference settings

4. **Modem Data Files:** `/common/package/modems/src/data/`
   - USB device ID mappings (vendor:product format)
   - Pre-configured profiles for 100+ modem models
   - Support for: Quectel, Huawei, Sierra Wireless, ZTE, etc.

### 2.2 Hotplug Integration

**Path:** `/common/files/etc/hotplug.d/usb/`

**USB Modem Hotplug Handler** (`20-usb-modem`):
```bash
# Triggers on USB device insertion
Detects: Quectel (2c7c), Huawei (12d1), Sierra (1199), ZTE (19d2)
Action: Auto-initializes modems via /usr/bin/usb-modem-autoconfig.sh
```

### 2.3 Network Management Tools

**Location:** `/common/files/usr/bin/`

#### Network Monitoring

1. **omr-status** (10 KB)
   - Real-time WAN connection status dashboard
   - Reads from: UCI configuration, /sys/class/net/, netlink
   - Supports:
     - DHCP/QMIB/MBIM protocols
     - Signal strength (uqmi, umbim)
     - Link speed from sysfs
     - Traffic statistics (RX/TX bytes)
     - MPTCP bonding status
   - UCI Integration: Reads network.*.proto, network.*.device, network.*.multipath
   - sysfs Access: /sys/class/net/{iface}/operstate, speed, statistics/

2. **network-monitor.sh**
   - Background health monitor
   - Ensures dnsmasq remains running
   - Auto-configures WiFi on first boot
   - Integrates with procd (OpenWrt service manager)

3. **network-safety-monitor.sh**
   - Prevents network lockouts
   - Monitors LAN IP integrity
   - Validates DHCP server functionality
   - Emergency fallback mechanisms

4. **port-autoconfig.sh**
   - Automatic Ethernet port detection
   - Converts ports to WAN interfaces
   - MPTCP metric assignment

5. **wifi-autoconfig.sh**
   - WiFi radio auto-configuration
   - Random SSID/password generation
   - Integration with UCI wireless config

6. **usb-modem-autoconfig.sh**
   - USB modem auto-integration
   - Creates WAN interfaces from modems
   - QMI/MBIM protocol selection

7. **emergency-lan-restore.sh**
   - Network recovery for lockouts
   - LAN port restoration
   - Configuration rollback

### 2.4 System Recovery Tools

**omr-recovery** - Emergency system recovery with:
- Network interface diagnostics
- Automatic troubleshooting
- Safe fallback modes

---

## 3. COMMUNICATION MECHANISMS - KERNEL/USERSPACE

### 3.1 Configuration Propagation

**UCI (Unified Configuration Interface)** - OpenWrt's config system

**Key Configuration Points:**

1. **Network Configuration** (`/etc/config/network`)
   - Managed by: network-monitor.sh, omr-status, etc.
   - Read/Write via: `uci get/set/batch` commands
   - Critical settings:
     ```
     network.lan.proto=static (MUST be static, prevents APIPA)
     network.lan.ipaddr=192.168.2.1
     network.*.proto=qmi|mbim|dhcp (WAN protocol)
     network.*.device=<interface> (WAN interface)
     network.*.multipath=on|off (MPTCP bonding)
     network.*.metric=<priority> (MPTCP path priority)
     ```

2. **DHCP Configuration** (`/etc/config/dhcp`)
   - Configured by: 10-omr-network-defaults
   - dnsmasq settings for DNS/DHCP server

3. **Wireless Configuration** (`/etc/config/wireless`)
   - Auto-populated by: wifi-autoconfig.sh
   - Radio enable/disable via UCI

### 3.2 Kernel-Userspace Interfaces

#### A. Netlink Communication

**MPTCP Path Manager (netlink):**
```
kernel/net/mptcp/pm_netlink.c
↓
Userspace path management via netlink
↑
User apps → netlink socket → kernel PM
```

**iproute2 Integration:**
- `ip mptcp monitor` - Monitor MPTCP events
- `ip mptcp endpoint` - Manage MPTCP endpoints
- Path selection via netlink attributes

#### B. sysfs Interface

**Network Interface Statistics:**
```
/sys/class/net/{iface}/
  - operstate: UP/DOWN/DORMANT
  - speed: Link speed in Mbps
  - statistics/: rx_bytes, tx_bytes, rx_packets, tx_packets
```

**Usage in omr-status:**
```bash
cat /sys/class/net/eth0/operstate  # Check if UP
cat /sys/class/net/eth0/speed      # Get speed
cat /sys/class/net/eth0/statistics/rx_bytes  # Traffic stats
```

#### C. ioctl-based Cellular Control

**modem initialization via AT commands:**
```
/dev/ttyUSB2 (AT command port)
→ AT+QCFG="usbnet",0  # Set QMI mode
→ AT+QNWCFG="lte_ca",1  # Enable CA
→ AT+QNWCFG="endc",1   # Enable EN-DC
```

**Signal monitoring:**
```bash
uqmi -d /dev/cdc-wdm0 --get-signal-info  # Via QMI protocol
umbim -d /dev/cdc-wdm0 -n signal        # Via MBIM protocol
```

#### D. init.d / OpenRC Scripts

**Path:** `/common/files/etc/init.d/`

Init scripts (created by 15-omr-autoconfig-init):
1. **network-monitor** - procd-managed health monitor
   - START=99 (runs after network)
   - Respawn on failure
   - stdout/stderr logging

2. **network-safety** - Network lockout prevention
   - START=98 (runs before network-monitor)
   - Monitors LAN integrity
   - Emergency restore trigger

3. **rm551e-monitor** - Created by modems package
   - Monitors RM551E modem health
   - Auto-restart on failure

### 3.3 ubus Integration

**OpenWrt's RPC mechanism** (potential future integration)
- Currently uses UCI shell-based config
- Can extend with ubus for dynamic control
- Enables web UI (LuCI) integration

---

## 4. INTEGRATION POINTS

### 4.1 First Boot Initialization

**UCI Defaults Scripts** (`/etc/uci-defaults/`)

1. **10-omr-network-defaults**
   - Runs once on first boot
   - Sets LAN to 192.168.2.1 (static, prevents APIPA)
   - Enables DHCP/DHCPv6/RA server
   - Configures DNS (dnsmasq)
   - Sets hostname to OMR-Optimized
   - Creates `/etc/omr-network-configured` marker

2. **15-omr-autoconfig-init**
   - Enables executable permissions on scripts
   - Creates init.d scripts for network monitoring
   - Starts network-safety and network-monitor services
   - Prepares for hotplug events

3. **90-omr-first-boot-wizard** (Luci wizard)
   - Web UI-based initial configuration
   - Network type selection (WAN, Cellular, etc.)

### 4.2 Event-Driven Architecture

**Hotplug System:**
```
USB Modem Inserted
    ↓ (kernel generates hotplug event)
/etc/hotplug.d/usb/ triggered
    ↓
20-usb-modem handler
    ↓
/usr/bin/usb-modem-autoconfig.sh
    ↓
Creates /etc/config/network entries
    ↓
Calls: uci set, uci commit
    ↓
Triggers: /etc/init.d/network reload
    ↓
Kernel: Loads qmi_wwan/cdc_mbim drivers
    ↓
Network: Brings up WWAN interface (wwan0)
```

### 4.3 Configuration Flow

```
UCI Config (network, wireless, dhcp)
    ↓ (uci read: /etc/config/*)
Userspace Apps (omr-status, network-monitor)
    ↓ (netlink, sysfs, ioctl)
Kernel Networking Stack
    ↓ (MPTCP scheduler, QMI/MBIM handlers)
Hardware (NICs, modems, WiFi)
```

---

## 5. DATA STRUCTURES & PROTOCOLS

### 5.1 MPTCP Kernel Data Structures

**Key kernel files involved:**
- `net/mptcp/pm_netlink.c` - Path management
- `net/mptcp/protocol.c` - MPTCP protocol logic
- `net/mptcp/subflow.c` - Subflow management
- `net/mptcp/mib.c` - Statistics/diagnostics

**MPTCP Socket Options:**
```c
MPTCP_ENABLED    // Enable MPTCP on socket
MPTCP_PM_CMD     // Path manager commands
MPTCP_SCHED      // Scheduler selection
```

### 5.2 Modem Communication Protocols

**QMI (Qualcomm Modem Interface):**
- Binary protocol for modem control
- Tools: uqmi command
- Supports: Data calls, signal info, device management

**MBIM (Mobile Broadband Interface Model):**
- Standardized WWAN control protocol
- Tools: umbim command
- HTTPS-based control messages

**AT Commands:**
- Legacy serial interface
- Used for: Mode switching, band configuration, signal monitoring

### 5.3 Driver Communication

**USB Serial Drivers:**
```
/dev/ttyUSB0-3  ← Modem ports (DM, NMEA, AT)
     ↓ (option driver)
kernel/drivers/usb/serial/
```

**Network Drivers:**
```
qmi_wwan, cdc_mbim, cdc_ncm, cdc_ether, rndis_host
     ↓
/sys/class/net/wwan0
     ↓
ip, netlink, sysfs interfaces
```

---

## 6. FILE PATHS SUMMARY

### Kernel Versions Structure
```
/home/user/openmptcprouter/
├── 5.4/
│   ├── target/linux/generic/hack-5.4/
│   │   └── 690-mptcp_v0.96.patch
│   └── package/kernel/linux/modules/
│       ├── netsupport.mk (ATM, bonding, tunneling)
│       ├── netfilter.mk
│       ├── crypto.mk
│       └── fs.mk
├── 6.1/
│   ├── target/linux/generic/config-6.1 (CONFIG_MPTCP=y)
│   └── package/kernel/
│       ├── rtl8812au-ct/ (Realtek WiFi)
│       ├── linux/modules/other.mk
│       └── patches/
├── 6.6/
│   ├── target/linux/generic/
│   │   ├── config-6.6
│   │   └── hack-6.6/999-mptcp-bpf.patch
│   ├── package/kernel/
│   │   ├── mwlwifi/ (Marvell WiFi)
│   │   └── patches/
│   ├── package/network/
│   │   └── utils/nftables/patches/ (Full-cone NAT)
│   └── package/libs/libnftnl/patches/
├── 6.10/
│   ├── target/linux/generic/config-6.10 (CONFIG_MPTCP=y, IPv6)
│   ├── package/kernel/
│   │   ├── mac80211/patches/
│   │   └── linux/modules/fs.mk
│   └── package/network/utils/
├── 6.12/
│   ├── target/linux/generic/
│   │   ├── config-6.12
│   │   └── hack-6.12/
│   │       ├── 999-mptcp-bpf.patch
│   │       └── 999-mptcp-increase-subflows-limits.patch
│   └── package/kernel/
│       ├── mt76/patches/ (WiFi7)
│       └── nat46/ (NAT46/TAT464)
└── common/
    ├── package/
    │   ├── modems/ (Modem support package)
    │   │   ├── Makefile
    │   │   ├── files/
    │   │   │   ├── rm551e-init.sh
    │   │   │   ├── rm551e-monitor.sh
    │   │   │   └── modem-ca-optimize.sh
    │   │   └── src/data/ (100+ modem profiles)
    │   ├── network/
    │   │   ├── ipv6/6in4/ (IPv6 tunnel support)
    │   │   ├── config/
    │   │   │   └── firewall/patches/ (fullconenat.patch)
    │   │   ├── services/dnsmasq/
    │   │   └── utils/wwan/
    │   ├── utils/wmt/ (MediaTek firmware loader)
    │   ├── base-files/
    │   └── luci-theme-omr-optimized/ (Web UI)
    ├── files/
    │   ├── etc/
    │   │   ├── uci-defaults/
    │   │   │   ├── 10-omr-network-defaults
    │   │   │   ├── 15-omr-autoconfig-init
    │   │   │   └── 90-omr-first-boot-wizard
    │   │   ├── hotplug.d/usb/
    │   │   │   └── 20-usb-modem
    │   │   ├── profile.d/
    │   │   │   └── 99-omr-banner.sh
    │   │   └── rc.button/
    │   │       └── reset
    │   └── usr/bin/
    │       ├── omr-status (Status dashboard)
    │       ├── omr-recovery (Recovery tool)
    │       ├── network-monitor.sh (Health monitor)
    │       ├── network-safety-monitor.sh (Lockout prevention)
    │       ├── port-autoconfig.sh (Port detection)
    │       ├── wifi-autoconfig.sh (WiFi setup)
    │       ├── usb-modem-autoconfig.sh (Modem setup)
    │       └── emergency-lan-restore.sh (Fallback)
    └── include/
        └── bpf_mptcp.mk (BPF compilation infrastructure)
├── patches/ (Global patches)
│   ├── bbr2.patch (Congestion control)
│   ├── mt76-wifi7-optimizations.patch
│   ├── ipt-nat6.patch
│   └── luci-*.patch (Web UI enhancements)
└── scripts/
    ├── auto-pair.sh
    ├── easy-install.sh
    └── client-auto-setup.sh
```

---

## 7. KEY INTEGRATION EXAMPLES

### Example 1: USB Modem Insertion → Network Integration

```
EVENT: User plugs in Quectel RM551E modem
  ↓
KERNEL: USB subsystem detects device (2c7c:0800)
  ↓
HOTPLUG: Calls /etc/hotplug.d/usb/20-usb-modem
  ↓
HANDLER: Detects Quectel (2c7c/*) → calls usb-modem-autoconfig.sh
  ↓
AUTOCONFIG:
  1. Waits 5 seconds for full enumeration
  2. Loads drivers: modprobe qmi_wwan, cdc_mbim, etc.
  3. Configures modem via AT commands on /dev/ttyUSB2
     - AT+QCFG="usbnet",0     (set QMI mode)
     - AT+QNWCFG="lte_ca",1   (enable CA)
  4. Creates UCI network entry: network.wwan1=interface
  5. Sets: protocol=qmi, device=/dev/cdc-wdm0, metric=20
  6. Calls: uci commit network → triggers network reload
  ↓
NETWORK: /etc/init.d/network reload
  ↓
KERNEL: wwan0 interface comes up, gets IP via QMI
  ↓
MONITOR: network-monitor.sh detects new interface
  ↓
MPTCP: Kernel enables MPTCP on this subflow
  ↓
omr-status: Shows wwan0 as "UP (192.168.1.X)" with signal strength
```

### Example 2: MPTCP Bonding Configuration

```
UCI CONFIG (user via web UI):
  network.wan1.multipath=on
  network.wan2.multipath=on
  network.wan1.metric=10
  network.wan2.metric=20
  ↓
KERNEL (netlink):
  - MPTCP path manager receives endpoint info
  - Configures PM mode to FULLMESH or NDIFFPORTS
  - Creates MPTCP subflows for each WAN
  ↓
SCHEDULER:
  - MPTCP scheduler (ROUNDROBIN, BLEST, ECMP, etc.)
  - Balances packets across subflows based on metric
  ↓
omr-status shows:
  WAN1: UP (inet 203.0.113.1) MPTCP: enabled (metric 10)
  WAN2: UP (inet 198.51.100.1) MPTCP: enabled (metric 20)
  Aggregation: MPTCP bonding active across 2 connections
```

### Example 3: Network Lockout Recovery

```
SCENARIO: User misconfigures network, loses SSH access
  ↓
SAFETY MONITOR: /usr/bin/network-safety-monitor.sh detects
  - LAN IP not responding
  - DHCP server not running
  - System inaccessible for 5 minutes
  ↓
ACTION:
  1. Triggers emergency-lan-restore.sh
  2. Restores last working LAN configuration
  3. Re-enables DHCP on LAN port
  4. Resets WiFi to factory defaults
  5. Creates /etc/omr-network-configured marker
  ↓
RESULT:
  User can SSH back to 192.168.2.1
  Web UI available for reconfiguration
```

---

## 8. COMMUNICATION LAYERS SUMMARY

```
APPLICATION LAYER
├─ omr-status (Status display)
├─ network-monitor.sh (Health monitoring)
├─ usb-modem-autoconfig.sh (Modem setup)
├─ rm551e-init.sh (Modem init)
└─ WiFi/port autoconfig scripts
     ↓ (UCI shell commands)
     
UCI CONFIGURATION LAYER
├─ /etc/config/network (uci get/set)
├─ /etc/config/wireless
├─ /etc/config/dhcp
└─ /etc/config/firewall
     ↓ (netlink, ioctl, sysfs)
     
KERNEL INTERFACE LAYER
├─ netlink sockets (MPTCP PM, net events)
├─ ioctl (Modem control, network)
├─ sysfs (Interface stats, state)
└─ procfs (Diagnostics)
     ↓ (kernel modules)
     
KERNEL LAYER
├─ MPTCP subsystem (/net/mptcp/)
├─ QMI/MBIM drivers (qmi_wwan, cdc_mbim)
├─ Network stack (TCP/IP)
├─ USB drivers (qcserial, option)
└─ Wireless drivers (mac80211, mt76, mwlwifi)
     ↓ (hardware I/O)
     
HARDWARE LAYER
├─ Ethernet NICs
├─ USB Modems
├─ WiFi Radios
└─ System buses
```

---

## 9. TECHNICAL DEEP DIVES

### MPTCP Subflow Management (6.12)

**Kernel Patch Impact:**
```diff
// Allows setting higher subflow limits
parse_limit(info, id, limit, max) {
  if (limit > max) return -EINVAL;
}

// Now supports up to 64 subflows (was 32)
subflows_max = 64;
```

**Use Case:**
- Single router bonding 64 modems
- Load balancing across multiple carriers
- Fallback paths for redundancy

### BPF MPTCP Hooks (6.6, 6.12)

**Enables:**
```c
// User-space BPF programs can hook MPTCP events
// Route decisions without kernel patch
// Scheduler policies via eBPF
// Real-time telemetry collection
```

**Compiled via:** `/common/include/bpf_mptcp.mk`
- LLVM/Clang-based compilation
- eBPF object files loaded via bpf() syscall
- Runtime modification of behavior

### Full-Cone NAT Support

**Patches:**
- `/common/package/network/config/firewall/patches/fullconenat.patch`
- `6.6/package/network/utils/nftables/patches/`
- `6.10/package/libs/libnftnl/patches/`

**Purpose:**
- Game-friendly NAT (no symmetric filtering)
- Improves P2P performance
- Crucial for MPTCP clients behind router

---

## 10. CONFIGURATION EXAMPLES

### Network Configuration (UCI)

```bash
# /etc/config/network

config interface 'lan'
    option proto 'static'
    option ipaddr '192.168.2.1'
    option netmask '255.255.255.0'

config interface 'wan1'
    option proto 'dhcp'
    option device 'eth0'
    option multipath 'on'
    option metric '10'

config interface 'wan2'
    option proto 'qmi'
    option device '/dev/cdc-wdm0'
    option multipath 'on'
    option metric '20'

config interface 'wan3'
    option proto 'mbim'
    option device '/dev/cdc-wdm1'
    option multipath 'on'
    option metric '25'
```

### Modem Initialization Flow

```bash
# Detected via hotplug
/etc/hotplug.d/usb/20-usb-modem ACTION=add PRODUCT=2c7c/0800/0
    → /usr/bin/usb-modem-autoconfig.sh
        → Load drivers: modprobe qmi_wwan
        → /usr/bin/rm551e-init.sh
            → Detect device: lsusb | grep 2c7c:08
            → Load drivers: option, qmi_wwan, cdc_mbim
            → Wait for /dev/ttyUSB*
            → Find AT port (usually /dev/ttyUSB2)
            → Configure modem:
                echo 'AT+QCFG="usbnet",0' > /dev/ttyUSB2
                echo 'AT+QNWCFG="lte_ca",1' > /dev/ttyUSB2
            → Start rm551e-monitor.sh background service
```

---

## 11. DEBUGGING & MONITORING

### Available Tools

**Kernel MPTCP Diagnostics:**
```bash
# Monitor MPTCP events (requires kernel support)
ip mptcp monitor

# Check MPTCP endpoints
ip mptcp endpoint show

# Check TCP metrics
cat /proc/net/tcp_metrics_nl

# MPTCP statistics
cat /proc/net/mptcp_pm_addr
cat /proc/net/mptcp_net
```

**Modem Status:**
```bash
# QMI signal strength
uqmi -d /dev/cdc-wdm0 --get-signal-info

# MBIM status
umbim -d /dev/cdc-wdm1 -n signal

# AT commands (direct)
echo -e 'AT\r' > /dev/ttyUSB2; cat /dev/ttyUSB2
```

**Network Interfaces:**
```bash
# System status
omr-status
omr-status --plain

# Recovery tool
omr-recovery diagnose

# sysfs interface states
cat /sys/class/net/eth0/operstate
cat /sys/class/net/wwan0/speed
```

**Logs:**
```bash
# System logging
logread
logread -f (follow)
logread -l 100 (last 100 lines)

# Tag filtering
logread | grep network-monitor
logread | grep "rm551e"
logread | grep usb-modem
```

---

## 12. SUMMARY TABLE

| Component | Type | Location | Purpose |
|-----------|------|----------|---------|
| MPTCP v0.96 Patch | Kernel | 5.4/target/linux/ | Custom MPTCP scheduler (v0.96) |
| MPTCP BPF Patch | Kernel | 6.6/6.12/target/linux/hack/ | eBPF hooks for policy |
| MPTCP Subflow Limits | Kernel | 6.12/target/linux/hack/ | Support up to 64 subflows |
| qmi_wwan | Driver | kernel/drivers/net | QMI modem driver |
| cdc_mbim | Driver | kernel/drivers/net | MBIM modem driver |
| mac80211 | Framework | kernel/net/mac80211 | Wireless stack |
| mt76 | Driver | kernel/drivers/net/wireless | MediaTek WiFi7 |
| Modems Package | Userspace | common/package/modems/ | Modem init & monitor |
| rm551e-init.sh | Script | common/package/modems/files/ | Quectel RM551E setup |
| rm551e-monitor.sh | Script | common/package/modems/files/ | Health monitoring |
| omr-status | Tool | common/files/usr/bin/ | Status dashboard |
| network-monitor.sh | Service | common/files/usr/bin/ | Health monitoring daemon |
| network-safety.sh | Service | common/files/etc/init.d/ | Lockout prevention |
| 20-usb-modem | Hotplug | common/files/etc/hotplug.d/usb/ | USB event handler |
| UCI Config | Config | /etc/config/network | Network configuration |
| BPF Makefile | Build | common/include/bpf_mptcp.mk | eBPF compilation |
| Full-Cone NAT Patch | Kernel | patches/fullconenat.patch | Game-friendly NAT |

---

## CONCLUSION

OpenMPTCProuter provides a comprehensive kernel-userspace integration:

1. **Kernel Support**: MPTCP is built into all supported kernels with version-specific optimizations (BPF, increased subflows, scheduler improvements)

2. **Modem Integration**: Automatic detection and initialization via hotplug → kernel drivers → AT command configuration

3. **Network Management**: UCI-based configuration system with monitoring daemons that use sysfs, netlink, and ioctl for real-time status

4. **Recovery & Safety**: Multiple layers of protection against configuration errors and network lockouts

5. **Extensibility**: BPF infrastructure allows runtime policy modifications without kernel recompilation

6. **Multi-Connection Support**: Seamless bonding of Ethernet, cellular, and WiFi interfaces via MPTCP scheduler
