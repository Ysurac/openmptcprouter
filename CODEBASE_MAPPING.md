# OpenMPTCProuter - Comprehensive Codebase Mapping

## Executive Summary

OpenMPTCProuter Optimized is a multipath TCP (MPTCP) based internet connection bonding solution built on OpenWrt. It aggregates multiple WAN connections (Fiber, DSL, 4G/5G modems) and routes them through a VPS for security, failover, and optimization.

**Repository Location**: `/home/user/openmptcprouter`
**Current Branch**: `claude/code-audit-network-agent-017M3rNvBu3hJXvERrwhjTEB`
**Primary Language**: Shell scripting, Lua (OpenWrt standard), C (kernel patches)

---

# 1. DIRECTORY STRUCTURE & ORGANIZATION

## Root Level Structure

```
/home/user/openmptcprouter/
├── common/                  # Shared files across all kernel versions
├── 5.4/, 6.1/, 6.6/, 6.10/, 6.12/  # Kernel version-specific configs
├── patches/                 # Kernel and package patches
├── scripts/                 # Setup and deployment scripts
├── vps-scripts/            # VPS-side installation and configuration
├── build.sh                # Main build orchestration script
├── quick-setup.sh          # Quick setup utility
├── sign.sh                 # Release signing script
└── [Documentation files]   # Multiple audit and reference documents
```

### Kernel Version Directories
- **5.4/, 6.1/, 6.6/, 6.10/, 6.12/**: OpenWrt OpenMPTCProuter builds for different kernel versions
  - Each contains: `package/`, `toolchain/`, `include/` subdirectories
  - Host architecture-specific configurations (ipq40xx, x86, rockchip, bcm27xx, etc.)

---

# 2. CORE LOGIC FILES - CRITICAL COMPONENTS

## 2.1 Network Configuration & Initialization

### Primary Configuration Files
| File Path | Lines | Purpose | Criticality |
|-----------|-------|---------|------------|
| `/home/user/openmptcprouter/common/files/etc/uci-defaults/10-omr-network-defaults` | 72 | LAN static IP setup, DHCP server config | **CRITICAL** |
| `/home/user/openmptcprouter/common/files/etc/uci-defaults/15-omr-autoconfig-init` | 92 | Initialize network monitors on first boot | **HIGH** |
| `/home/user/openmptcprouter/common/files/etc/uci-defaults/90-omr-first-boot-wizard` | 647 | Web-based setup wizard for VPS pairing | **HIGH** |
| `/home/user/openmptcprouter/common/package/base-files/files/bin/config_generate` | 200+ | Network and system config generation | **CRITICAL** |

**Key Responsibilities**:
- LAN configuration: Always static IP at 192.168.2.1
- DHCP server setup for LAN clients
- System hostname and timezone configuration
- Prevention of APIPA (169.254.x.x) addresses
- UCI defaults setup and network interface binding

### System Optimization Files
| File Path | Lines | Purpose | Criticality |
|-----------|-------|---------|------------|
| `/home/user/openmptcprouter/common/files/etc/sysctl.d/99-omr-mptcp-5g-optimization.conf` | 164 | Kernel parameter optimization | **CRITICAL** |

**Key Settings**:
- MPTCP enablement: `net.mptcp.enabled = 1`
- TCP congestion control: BBR (Bottleneck Bandwidth and Round-trip time)
- Memory buffers for 5G: 128MB max buffer sizes
- Connection tracking: `nf_conntrack_max = 524288`
- Multipath hash policy: Layer 4 for better load balancing
- IPv6 forwarding enabled
- RP filter (loose mode 2) for multi-WAN support

---

## 2.2 Network Monitoring & Safety Systems

### Packet Handling & Network Health

| File Path | Lines | Purpose | Criticality |
|-----------|-------|---------|------------|
| `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh` | 314 | Prevents user lockout via misconfiguration | **CRITICAL** |
| `/home/user/openmptcprouter/common/files/usr/bin/network-monitor.sh` | 114 | Continuous network health monitoring | **HIGH** |
| `/home/user/openmptcprouter/common/files/usr/bin/omr-status` | 300+ | WAN connection status dashboard | **HIGH** |
| `/home/user/openmptcprouter/common/files/usr/bin/omr-diagnostics` | 150+ | Smart diagnostics and health checks | **HIGH** |

**Critical Functions**:

**network-safety-monitor.sh**:
- Detects and prevents APIPA addresses on interfaces
- Monitors LAN accessibility continuously every 30 seconds
- Ensures static LAN configuration (never DHCP)
- Validates at least one physical LAN port is assigned
- Triggers emergency recovery if needed
- Logs critical network issues

**omr-status**:
- Shows all active WAN connections with types (Ethernet, USB Modem, Cellular QMI/MBIM)
- Displays signal strength for cellular modems
- Shows traffic statistics and link speeds
- Color-coded terminal output for quick status assessment

**omr-diagnostics**:
- Checks network connectivity and default gateway
- Verifies DNS resolution
- Monitors MPTCP module and subflow status
- Tracks VPN connections (Shadowsocks, WireGuard, Glorytun)
- Reports system resource usage
- Generates health status JSON

---

## 2.3 Port & Interface Auto-Configuration

| File Path | Lines | Purpose | Criticality |
|-----------|-------|---------|------------|
| `/home/user/openmptcprouter/common/files/usr/bin/port-autoconfig.sh` | 239 | First-boot physical port detection | **HIGH** |
| `/home/user/openmptcprouter/common/files/usr/bin/usb-modem-autoconfig.sh` | 382 | USB modem detection and WAN setup | **HIGH** |
| `/home/user/openmptcprouter/common/files/usr/bin/wifi-autoconfig.sh` | 239 | WiFi interface auto-configuration | **MEDIUM** |
| `/home/user/openmptcprouter/common/files/etc/hotplug.d/usb/20-usb-modem` | 61 | USB hotplug event handler | **HIGH** |

**port-autoconfig.sh Functions**:
- Detects all physical ethernet interfaces (eth*, lan*, wan*)
- Implements smart port role detection (WAN vs LAN)
- Ensures at least one LAN port for user access
- Creates LAN bridge with remaining ports
- Applies UCI network configuration
- One-time execution on first boot

**usb-modem-autoconfig.sh Functions**:
- Supports QMI and MBIM protocol modems
- Gets carrier APN settings from USA carrier database
- Configures multipath TCP bonding
- Supports concurrent multiple USB modems
- Creates separate WAN interfaces for each modem
- Maps device protocols to UCI configuration

**Supported Modem Vendors**: Quectel, Huawei, Sierra Wireless, ZTE, SIMCom, Fibocom, Telit, MediaTek

---

## 2.4 Recovery & Emergency Systems

| File Path | Lines | Purpose | Criticality |
|-----------|-------|---------|------------|
| `/home/user/openmptcprouter/common/files/usr/bin/omr-recovery` | 200 | Interactive emergency recovery tool | **CRITICAL** |
| `/home/user/openmptcprouter/common/files/usr/bin/emergency-lan-restore.sh` | 146 | Fast LAN restoration on boot | **HIGH** |

**omr-recovery Features**:
1. All ports assigned to WAN - restores first available port to LAN
2. Wrong IP configuration - resets LAN to 192.168.2.1
3. DHCP server issues - re-enables and reconfigures DHCP
4. Factory reset - complete network configuration reset
5. Validates user input to prevent injection attacks

---

# 3. NETWORK/BONDING LOGIC FILES

## 3.1 MPTCP & Bonding Core

| File Path | Lines | Purpose | Criticality |
|-----------|-------|---------|------------|
| `/home/user/openmptcprouter/common/include/bpf_mptcp.mk` | 95 | BPF toolchain for MPTCP kernel compilation | **CRITICAL** |

**Key Responsibilities**:
- Configures LLVM/Clang for BPF (eBPF) code compilation
- Supports MPTCP kernel module building
- Defines compiler flags for various architectures (MIPS, ARM)
- Minimum LLVM version requirement: 12
- Target BPF architecture configuration

### MPTCP Configuration Details
```
MPTCP Enabled: net.mptcp.enabled = 1
Congestion Control: BBR (Better throughput, lower latency)
Maximum subflows per connection: Kernel default (usually 8)
Hash policy: Layer 4 (transport layer) for better distribution
Multipath use neighbor: Enabled for per-neighbor multipath selection
```

## 3.2 Multi-WAN & Routing Logic

### Network Configuration Primitives
Located in UCI config format (via `/etc/config/network`):

- **WAN Interface Creation**: Each WAN gets separate interface with protocol (DHCP/QMI/MBIM)
- **Multipath Routing**: `multipath='on'` UCI setting enables load distribution
- **Metric-based Priority**: Lower metric = higher priority (for failover)
- **Device Binding**: Physical interfaces (eth*, wwan*, USB devices) bound to UCI interfaces

### Multi-WAN Failover Strategy
```
1. All active WANs aggregated via MPTCP
2. Layer 4 hash distribution (per 5-tuple)
3. Loose RPF (reverse path filter) mode for asymmetric routing
4. Connection tracking with 524k max connections
5. Per-neighbor nexthop selection when available
```

---

# 4. CONFIGURATION & ENVIRONMENT HANDLING

## 4.1 Configuration Parsing & UCI System

| File Path | Purpose | Type |
|-----------|---------|------|
| `/home/user/openmptcprouter/common/package/base-files/files/bin/config_generate` | Generates `/etc/config/network` from board.json | Shell |
| `/etc/uci-defaults/*` | UCI first-boot defaults | Shell |
| `/etc/config/network` (generated) | Runtime network configuration | UCI format |
| `/etc/config/dhcp` (generated) | DHCP server configuration | UCI format |
| `/etc/config/system` (generated) | System hostname/timezone | UCI format |

## 4.2 Environment Variables & Setup

| File Path | Lines | Purpose |
|-----------|-------|---------|
| `/home/user/openmptcprouter/common/files/usr/lib/omr/omr-logger.sh` | 155 | Centralized logging with rate limiting |
| `/home/user/openmptcprouter/common/files/etc/usa-carrier-apns.conf` | 173 | USA cellular carrier APN database |
| `/home/user/openmptcprouter/common/files/etc/profile.d/99-omr-banner.sh` | 51 | Console login banner with quick tips |

### Logger Functions
```
omr_log_debug()      - Debug level messages
omr_log_info()       - Informational messages
omr_log_warning()    - Warning conditions
omr_log_error()      - Error conditions
omr_log_critical()   - Critical system issues

Features:
- Rate limiting (max 5 identical messages/minute)
- File rotation at 1MB
- Syslog integration
- Component tagging
```

### Supported USA Carriers (APN Database)
- Verizon, AT&T, T-Mobile
- Visible, Mint Mobile, Straight Talk
- MVNO providers with detailed APN, username, password, auth type

---

# 5. ADMIN/CLI/WEB UI LAYERS

## 5.1 Web User Interface

| Component | Path | Type | Purpose |
|-----------|------|------|---------|
| LuCI Theme | `/home/user/openmptcprouter/common/package/luci-theme-omr-optimized/` | Lua + HTML/CSS/JS | Customized web dashboard |
| Theme CSS | `/htdocs/luci-static/omr-optimized/css/` | CSS | Styling (utilities, components) |
| Theme JS | `/htdocs/luci-static/omr-optimized/js/theme.js` | JavaScript | Frontend interactions |
| Theme Views | `/luasrc/view/themes/omr-optimized/` | Lua + HTML | Header, footer templates |

### First-Boot Wizard
| Component | Lines | Purpose |
|-----------|-------|---------|
| Setup HTML | `/home/user/openmptcprouter/common/files/etc/uci-defaults/90-omr-first-boot-wizard` | 647 | Interactive multi-step wizard |

**Setup Wizard Features**:
1. **Pairing Code Method**: Paste pre-generated code from VPS
2. **Auto-Discovery**: Enter VPS IP, fetch config automatically
3. **Manual Setup**: Input VPS IP, port, password, encryption method
4. **Progress Tracking**: Step indicators, progress bar
5. **Success Confirmation**: Post-setup instructions

**Encryption Methods Offered**:
- Shadowsocks ChaCha20-IETF-Poly1305
- Shadowsocks AES-256-GCM

## 5.2 Command-Line Tools

| Tool | Path | Lines | Purpose |
|------|------|-------|---------|
| omr-status | `/home/user/openmptcprouter/common/files/usr/bin/omr-status` | 300+ | Status dashboard |
| omr-diagnostics | `/home/user/openmptcprouter/common/files/usr/bin/omr-diagnostics` | 150+ | System health check |
| omr-recovery | `/home/user/openmptcprouter/common/files/usr/bin/omr-recovery` | 200 | Emergency recovery |
| omr-logger.sh | `/home/user/openmptcprouter/common/files/usr/lib/omr/omr-logger.sh` | 155 | Logging infrastructure |

---

# 6. SETUP & DEPLOYMENT SCRIPTS

## 6.1 Router Client Setup

| Script | Lines | Purpose | Execution Context |
|--------|-------|---------|-------------------|
| `scripts/client-auto-setup.sh` | 328 | Automatic router configuration | Router (SSH) |
| `scripts/auto-pair.sh` | 388 | Bidirectional server-client pairing | VPS or Router |
| `scripts/easy-install.sh` | 473 | VPS installation with web UI | VPS |
| `scripts/verify-setup.sh` | 367 | Connection verification and testing | Router |

## 6.2 VPS Server Setup

| Script | Lines | Purpose | Execution Context |
|--------|-------|---------|-------------------|
| `vps-scripts/wizard.sh` | 37,645 | Interactive VPS setup wizard | VPS (Debian/Ubuntu) |
| `vps-scripts/omr-vps-install.sh` | 15,698 | Automated VPS installation | VPS |
| `vps-scripts/install.sh` | 1,702 | Minimal VPS installer | VPS |

### Wizard Features
- Generates secure credentials (Shadowsocks, Glorytun, MLVPN passwords)
- Installs Shadowsocks-libev, iptables, required packages
- Configures firewall rules for multi-tunnel support
- Creates web interface at port 8080
- Generates pairing codes for clients
- Supports multiple client connections

## 6.3 Build System

| File | Lines | Purpose |
|------|-------|---------|
| `build.sh` | 59,215 | Main build orchestration |
| `quick-setup.sh` | 6,649 | Quick environment setup |
| `sign.sh` | 236 | Release signing utility |

---

# 7. PATCHES & KERNEL OPTIMIZATION

## 7.1 Kernel Patches Directory

**Location**: `/home/user/openmptcprouter/patches/`

| Patch | Size | Target | Purpose |
|-------|------|--------|---------|
| `bbr2-5.15.patch` | 129KB | Kernel 5.15+ | BBR v2 congestion control |
| `bbr2.patch` | 929B | Generic | BBR v2 metadata |
| `mt76-wifi7-optimizations.patch` | 367B | WiFi drivers | MediaTek WiFi 7 optimization |
| `smsc75xx.patch` | 884B | USB net driver | SMSC75xx USB Ethernet fix |
| `ipt-nat6.patch` | 617B | iptables | IPv6 NAT support |
| `luci-nftables.patch` | 2,345B | LuCI | NFTables integration |
| `luci-syslog*.patch` | Various | LuCI | System log viewer updates |
| `uefi.patch` | 45KB | UEFI | UEFI firmware support |

---

# 8. CRITICAL FILE DEPENDENCIES

## 8.1 State Persistence & Configuration Management

| Path | Purpose | Format |
|------|---------|--------|
| `/etc/config/network` | Network configuration (generated from defaults) | UCI |
| `/etc/config/dhcp` | DHCP/DNS configuration | UCI |
| `/etc/config/system` | System settings | UCI |
| `/var/run/omr-health-status.json` | Runtime health status | JSON |
| `/var/run/emergency-port` | Emergency LAN port tracking | Plain text |
| `/etc/omr-network-configured` | First-boot network config flag | Flag file |
| `/etc/port-autoconfig-applied` | Port autoconfig completion flag | Flag file |
| `/etc/omr-setup-complete` | Setup wizard completion flag | Flag file |
| `/var/log/omr.log` | Persistent operation log | Text |

## 8.2 Packet Handling & Forwarding

**IP Forwarding**: Enabled via `net.ipv4.ip_forward = 1` and `net.ipv6.conf.all.forwarding = 1`

**Connection Tracking**:
- Module: nf_conntrack (built-in kernel)
- Max connections: 524,288
- TCP timeout (established): 432,000s (5 days)
- UDP timeout: 60s

**NAT/Firewall** (via iptables/nftables):
- Default firewall rules applied via `/etc/init.d/firewall`
- MPTCP-aware NAT rules
- Multi-WAN load balancing rules

---

# 9. MPTCP/WIREGUARD/VPN INTERACTION FILES

## 9.1 MPTCP Integration Points

| Component | File | Integration Type |
|-----------|------|------------------|
| Kernel Module | `bpf_mptcp.mk` | Compilation flags |
| System Config | `sysctl.d/99-omr-mptcp-5g-optimization.conf` | Runtime parameters |
| Network Config | `uci-defaults/10-omr-network-defaults` | Interface setup |
| Wizard | `90-omr-first-boot-wizard` | UI for setup |

**MPTCP Subflow Management**:
- Subflows detected via `ss -tin` command (omr-diagnostics)
- Per-interface statistics viewable via status tools
- Automatic subflow creation on multi-path detect
- BBR congestion control for optimal throughput

## 9.2 VPN/Shadowsocks Configuration

**Configuration Locations**:
- UCI Config: `/etc/config/shadowsocks-libev` (generated by wizard)
- Service: Started via `/etc/init.d/shadowsocks-libev`
- Data Flow: Client → Router → Shadowsocks (VPS) → Internet

**Supported Encryption Methods**:
- ChaCha20-IETF-Poly1305 (recommended)
- AES-256-GCM

## 9.3 WireGuard Integration

**Detection Points**:
- Module check: `/sys/module/wireguard/refcnt`
- Peer detection: `wg show all peers` command
- Status: Monitored by omr-diagnostics

**Configuration**:
- Not explicitly configured in audited files
- Integration points present for potential future enhancement

---

# 10. STATE PERSISTENCE & RUNTIME MANAGEMENT

## 10.1 Initialization Sequence

```
1. Boot → Kernel loads
2. /etc/init.d/* services start
3. /etc/uci-defaults/* scripts run (first boot only)
   ├─ 10-omr-network-defaults (static LAN setup)
   ├─ 15-omr-autoconfig-init (monitor setup)
   └─ 90-omr-first-boot-wizard (if first boot)
4. Network services start
5. Safety monitor starts (every 30s check)
6. Port autoconfig runs (first boot)
7. USB hotplug handler ready for modems
8. System fully operational
```

## 10.2 Runtime Monitoring & Management

| Service | Init Script | Start Order | Purpose |
|---------|------------|-------------|---------|
| network-safety | `/etc/init.d/network-safety` | 98 | Prevent lockouts |
| network-monitor | `/etc/init.d/network-monitor` | 99 | Health monitoring |
| firewall | `/etc/init.d/firewall` | (standard) | Packet filtering |
| dnsmasq | `/etc/init.d/dnsmasq` | (standard) | DNS/DHCP |

---

# 11. CRITICAL FILES REFERENCE MATRIX

## Packet Handling & Forwarding
- **Primary**: `sysctl.d/99-omr-mptcp-5g-optimization.conf` (kernel parameters)
- **Secondary**: `base-files/bin/config_generate` (interface setup)
- **Monitoring**: `usr/bin/network-safety-monitor.sh`, `omr-diagnostics`

## MPTCP/Bonding
- **Kernel Build**: `common/include/bpf_mptcp.mk`
- **System Config**: `sysctl.d/99-omr-mptcp-5g-optimization.conf`
- **UI Setup**: `uci-defaults/90-omr-first-boot-wizard`
- **Status Display**: `usr/bin/omr-status`, `omr-diagnostics`

## State Persistence & Configuration
- **Boot Defaults**: `uci-defaults/10-omr-network-defaults`, `15-omr-autoconfig-init`
- **Runtime Config**: Generated in `/etc/config/` (network, dhcp, system, firewall)
- **Health Tracking**: `/var/run/omr-health-status.json`

## Admin/CLI/Web UI
- **CLI Tools**: `usr/bin/omr-status`, `omr-diagnostics`, `omr-recovery`
- **Web Wizard**: `uci-defaults/90-omr-first-boot-wizard` (HTML + JavaScript)
- **Theme**: `luci-theme-omr-optimized/` (Lua, HTML, CSS, JavaScript)
- **Logger**: `usr/lib/omr/omr-logger.sh`

---

# 12. SECURITY-CRITICAL FILES

| File | Risk Level | Protection Mechanism |
|------|-----------|----------------------|
| `omr-recovery`, `network-safety-monitor.sh` | **CRITICAL** | Input validation, no eval |
| `usb-modem-autoconfig.sh` | **HIGH** | Device path validation |
| `config_generate` | **CRITICAL** | UCI writes permissions |
| `wizard.sh` (VPS) | **CRITICAL** | Credential generation security |
| Setup wizard HTML | **HIGH** | CORS, CSP considerations |

---

# 13. MISSING OR FUTURE COMPONENTS

Based on audit, the following are NOT currently implemented but may be needed:

1. **LuCI Admin Modules** - Web UI for network/service configuration
2. **Explicit Firewall Rules** - Only default OpenWrt firewall referenced
3. **VPN Protocol Flexibility** - WireGuard/IPSEC not explicitly configured
4. **QoS/Traffic Shaping** - Not found in audited files
5. **Statistics Database** - Real-time usage tracking infrastructure
6. **Update Management** - FW update mechanism not detailed
7. **Advanced Routing Policies** - Policy-based routing not in core files
8. **Load Balancing Algorithms** - Hash-based only, no active health checks

---

# 14. BUILD SYSTEM OVERVIEW

## Kernel Versions Supported
- 5.4 (legacy)
- 6.1 (stable)
- 6.6 (LTS)
- 6.10 (current)
- 6.12 (latest)

## Target Architectures
- ARM (ipq40xx, ipq807x, bcm27xx, rockchip, omap, mvebu)
- x86/x86_64
- MIPS (various variants)

## Build Process
```
build.sh → Selects kernel version
        → Configures OpenWrt feeds
        → Applies patches from /patches/
        → Includes common files from /common/
        → Builds firmware image
        → sign.sh → Generates release signatures
```

---

# 15. PERFORMANCE CHARACTERISTICS

## Memory Optimizations
- **Connection Tracking**: 524K max (tunable via sysctl)
- **Buffer Sizes**: 128MB max for high-speed links
- **Device Backlog**: 300K queue depth for high packet rates
- **VM Dirty Ratio**: 60% (optimized for throughput)

## Throughput Optimizations
- **TCP Congestion**: BBR (Better than CUBIC for variable latency)
- **Window Scaling**: Enabled for high-latency 5G links
- **Fast Open**: Enabled (3-way handshake reduction)
- **SACK/DSACK**: Enabled for loss recovery
- **MTU Probing**: Enabled for path discovery

## Latency Optimizations
- **TCP No Metrics Save**: Per-connection optimization
- **Connection Reuse**: TIME_WAIT reduction
- **SYN Backlog**: 16K queue for high connection rates
- **ECN**: Disabled for ISP compatibility

---

# 16. DEPLOYMENT WORKFLOW

## Router Deployment
```
1. Flash firmware image to device
2. Power on → Boot sequence
3. Auto-detection of network ports (port-autoconfig.sh)
4. Network safety monitor starts
5. First-boot wizard appears at http://192.168.2.1
6. User selects setup method (pairing/auto-discovery/manual)
7. Configuration applied to /etc/config/
8. Services restart with new configuration
9. System ready for traffic
```

## VPS Deployment
```
1. Run vps-scripts/wizard.sh on Debian/Ubuntu
2. Interactive configuration:
   - Port selection
   - Credential generation
   - Firewall setup
3. Services start (Shadowsocks, iptables, etc.)
4. Web UI available at http://VPS:8080
5. Pairing codes generated for clients
```

---

# 17. RECOMMENDED AUDIT FOCUS AREAS

For future audits, prioritize:
1. **Security**: Input validation in all shell scripts
2. **Network Logic**: Multi-WAN failover edge cases
3. **State Management**: Config consistency after reboots
4. **Emergency Paths**: Recovery sequence validation
5. **Cellular Support**: USB modem protocol handling
6. **LuCI Integration**: Web UI auth and privilege escalation
7. **Performance**: Connection tracking under load
8. **Logging**: Rate limiting effectiveness

