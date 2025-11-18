# Code Quality Analysis Report - OpenMPTCProuter

## Executive Summary
Comprehensive analysis of the codebase reveals multiple code quality issues across shell scripts in:
- `/build.sh` (1,105 lines)
- `/vps-scripts/` (wizard.sh, omr-vps-install.sh)
- `/common/files/usr/bin/` (6 monitoring/configuration scripts)

---

## 1. CRITICAL SYNTAX ERRORS

### Issue 1.1: Invalid Redirection Syntax in build.sh
**File**: `/home/user/openmptcprouter/build.sh`
**Lines**: 291, 299
**Severity**: CRITICAL - Script will fail

```bash
# Line 291
cat "$OMR_TARGET_CONFIG" config -> "$OMR_TARGET/${OMR_KERNEL}/source/.config" <<-EOF

# Line 299
cat config -> "$OMR_TARGET/${OMR_KERNEL}/source/.config" <<-EOF
```

**Problem**: The syntax `cat file -> output` is invalid. Shell redirection uses `>` not `->`. This should be:
```bash
cat "$OMR_TARGET_CONFIG" config > "$OMR_TARGET/${OMR_KERNEL}/source/.config" <<-EOF
```

**Impact**: Build process will fail immediately when trying to generate .config file.

---

## 2. CODE DUPLICATION

### Issue 2.1: Identical log_msg() Function Definition
**Files**:
- `/home/user/openmptcprouter/common/files/usr/bin/network-monitor.sh` (lines 12-14)
- `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh` (lines 14-16)
- `/home/user/openmptcprouter/common/files/usr/bin/emergency-lan-restore.sh` (lines 10-13)
- `/home/user/openmptcprouter/common/files/usr/bin/usb-modem-autoconfig.sh` (lines 10-13)
- `/home/user/openmptcprouter/common/files/usr/bin/wifi-autoconfig.sh` (lines 10-12)
- `/home/user/openmptcprouter/common/files/usr/bin/port-autoconfig.sh` (lines 12-14)

**Severity**: MEDIUM

**Problem**: Six different scripts define identical or nearly identical log_msg() functions:

```bash
# Standard pattern repeated 6 times
log_msg() {
    logger -t "$LOG_TAG" "$1"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}
```

**Impact**:
- Code maintenance nightmare - fix in one place doesn't propagate
- Lines 1-50 are near-duplicates across all six scripts
- Total wasted lines: ~100+ lines of duplicated code

**Solution**: Create shared `/etc/profile.d/omr-functions.sh` sourced by all scripts.

---

### Issue 2.2: Repeated Color Code Definitions
**Files**:
- `/home/user/openmptcprouter/vps-scripts/omr-vps-install.sh` (lines 14-18)
- `/home/user/openmptcprouter/vps-scripts/wizard.sh` (lines 24-30)
- `/home/user/openmptcprouter/vps-scripts/test-integration.sh` (lines 16-20)

**Severity**: LOW

**Problem**: Color codes defined identically across multiple scripts:

```bash
# Repeated 3+ times across vps-scripts
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'
```

---

### Issue 2.3: Repeated APIPA/IP Validation Logic
**Files**:
- `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh` (lines 19-38, 87-117)
- `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh` (lines 103-104, 107)

**Severity**: MEDIUM

**Problem**: APIPA check (169.254.x.x) appears 3 times with nearly identical grep patterns:

```bash
# Line 23
echo "$ip" | grep -q "^169\.254\."

# Line 103
echo "$current_ip" | grep -q "^169\.254\."

# Line 107
echo "$if_name" | grep -q "^br-lan"
```

---

### Issue 2.4: Repeated uci Query Patterns
**Files**:
- `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh` (lines 136, 154)
- `/home/user/openmptcprouter/common/files/usr/bin/emergency-lan-restore.sh` (lines 29, 47, 62)
- `/home/user/openmptcprouter/common/files/usr/bin/usb-modem-autoconfig.sh` (lines 190, 205)

**Severity**: MEDIUM

**Problem**: Complex uci parsing command repeated verbatim:

```bash
# Repeated 6 times across files
for wan in $(uci show network 2>/dev/null | grep "=interface" | grep -E "\.wan" | cut -d. -f2 | cut -d= -f1); do
```

**Better approach**: Extract into helper function.

---

## 3. MISSING ERROR HANDLING

### Issue 3.1: No Check After Critical uci commit
**Files**:
- `/home/user/openmptcprouter/common/files/usr/bin/port-autoconfig.sh` (line 145)
- `/home/user/openmptcprouter/common/files/usr/bin/usb-modem-autoconfig.sh` (line 165)
- `/home/user/openmptcprouter/common/files/usr/bin/wifi-autoconfig.sh` (line 188)

**Severity**: HIGH

**Problem**: Critical uci operations with no error checking:

```bash
# Line 145 - port-autoconfig.sh
uci commit network
# No check if commit succeeded

# Next line assumes success
/etc/init.d/network reload
```

**Better approach**:
```bash
if ! uci commit network; then
    log_msg "ERROR: Failed to commit network configuration"
    return 1
fi
```

---

### Issue 3.2: Unprotected System File Operations
**File**: `/home/user/openmptcprouter/build.sh`
**Lines**: 150, 155, 162, 165, 168, 171, 184, 187-188

**Severity**: HIGH

**Problem**: Destructive operations without verification:

```bash
# Line 150 - No check if variable is set
rm -rf "$OMR_TARGET/${OMR_KERNEL}/source/bin"

# Line 155 - Multiple directories deleted without checking if they exist
rm -rf "$OMR_TARGET/${OMR_KERNEL}/source/files" "$OMR_TARGET/${OMR_KERNEL}/source/tmp"

# Line 187-188 - Recursive copy without error handling
cp -rf common/* "$OMR_TARGET/${OMR_KERNEL}/source"
cp -rf ${OMR_KERNEL}/* "$OMR_TARGET/${OMR_KERNEL}/source"
# No check if copy succeeded
```

**Risk**: If variables are unset or paths are wrong, script silently deletes wrong directories.

**Better approach**:
```bash
[ -n "$OMR_TARGET" ] || { echo "ERROR: OMR_TARGET not set"; exit 1; }
[ -d "$OMR_TARGET/${OMR_KERNEL}/source" ] || { echo "ERROR: Source dir not found"; exit 1; }
rm -rf "$OMR_TARGET/${OMR_KERNEL}/source/bin" || { echo "ERROR: Failed to remove bin"; exit 1; }
```

---

### Issue 3.3: Silent Failures in Modem Configuration
**File**: `/home/user/openmptcprouter/common/files/usr/bin/usb-modem-autoconfig.sh`
**Lines**: 182, 215

**Severity**: HIGH

**Problem**: Interface bring-up in background without verification:

```bash
# Line 182
ifup "$wan_name" 2>/dev/null &
# Silently backgrounded, no error checking
```

**Better approach**:
```bash
if ! ifup "$wan_name"; then
    log_msg "ERROR: Failed to bring up $wan_name"
    return 1
fi
```

---

### Issue 3.4: Unverified Command Execution
**File**: `/home/user/openmptcprouter/common/files/usr/bin/wifi-autoconfig.sh`
**Lines**: 146, 150, 169-170

**Severity**: MEDIUM

**Problem**: Commands execute without checking if they succeeded:

```bash
# Line 146 - iw command may fail but continues
local bands=$(iw phy "$phy" info 2>/dev/null | grep "Band" | awk '{print $2}')

# Line 150 - Piped command chains with no error checking
local freq_info=$(iw phy "$phy" info 2>/dev/null | grep "MHz")
```

---

## 4. COMPLEX FUNCTIONS NEEDING REFACTORING

### Issue 4.1: Oversized Main Functions
**File**: `/home/user/openmptcprouter/build.sh`
**Scope**: Entire script (1,105 lines)

**Severity**: MEDIUM

**Problem**: Single monolithic script with no function decomposition:

```bash
# build.sh structure (WRONG):
- Lines 1-50: Variable initialization (50 lines)
- Lines 50-90: Complex target mapping logic
- Lines 90-400: Repository setup
- Lines 200-300: Feed configuration (300+ lines of nested if/else)
- Lines 330-850: Build configuration (520+ lines)
# No functions, no modularity
```

**Specific complexity**: Lines 228-287 - Feed configuration section:
- 60 lines of deeply nested if/else statements
- 3-level indentation making logic hard to follow
- Repeated package feed configuration (opkg vs apk) with ~40% code duplication

**Better approach**:
```bash
configure_feeds_opkg() { ... }
configure_feeds_apk() { ... }
configure_package_feeds() { 
    if [ "$OMR_KERNEL" != "6.12" ]; then
        configure_feeds_opkg
    else
        configure_feeds_apk
    fi
}
```

---

### Issue 4.2: Complex Port Detection Logic
**File**: `/home/user/openmptcprouter/common/files/usr/bin/port-autoconfig.sh`
**Lines**: 41-96 (56 lines)

**Severity**: MEDIUM

**Problem**: 56-line function handling multiple concerns:

```bash
detect_port_roles() {
    # Get ports (lines 42)
    local all_ports=$(get_all_ports)
    
    # WAN detection logic (lines 53-77)
    for port in wan wan0 eth0; do
        if echo "$all_ports" | grep -qw "$port"; then
            wan_port="$port"
            all_ports=$(echo "$all_ports" | sed "s/$port//g" | xargs)
            break
        fi
    done
    
    # Fallback WAN detection (lines 65-77)
    if [ -z "$wan_port" ]; then
        local port_count=$(echo "$all_ports" | wc -w)
        if [ $port_count -gt 1 ]; then
            # Logic continues...
        fi
    fi
    
    # LAN configuration (lines 79-90)
    # Safety checks (lines 83-90)
}
```

**Better approach**: Split into helper functions:
- `find_wan_port()`
- `assign_lan_ports()`
- `validate_port_config()`

---

### Issue 4.3: Complex Network Safety Logic
**File**: `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh`
**Lines**: 120-201 (81 lines in one function)

**Severity**: MEDIUM

**Problem**: emergency_recovery() mixes multiple concerns:

```bash
emergency_recovery() {
    # Find port (lines 127-150)
    # Validate port not WAN (lines 135-142)
    # Fallback logic (lines 153-162)
    # Validation (lines 164-167)
    # Configuration (lines 172-185)
    # Restart (lines 189-190)
    # Logging (lines 192-198)
    # Return status (lines 200)
}
```

---

## 5. POOR NAMING CONVENTIONS

### Issue 5.1: Non-Standard Variable Naming
**File**: `/home/user/openmptcprouter/build.sh`
**Lines**: Throughout

**Severity**: LOW-MEDIUM

**Problem**: Inconsistent naming conventions:

```bash
# Lines 30-55: Variables use UPPER_SNAKE_CASE (good)
OMR_DIST=${OMR_DIST:-openmptcprouter}
OMR_HOST=${OMR_HOST:-$(curl -sS ifconfig.co)}

# But also:
UPSTREAM=${UPSTREAM:-no}          # Different prefix convention
SYSLOG=${SYSLOG:-logd}            # Inconsistent with OMR_ prefix

# Target mapping uses OMR_TARGET (good) but references:
OMR_REAL_TARGET                   # Inconsistent naming (why REAL?)
OMR_TARGET_CONFIG="config-$OMR_TARGET"  # Inconsistent reference
```

**Better approach**:
- All build configuration variables should use `BUILD_` prefix: `BUILD_DIST`, `BUILD_KERNEL`
- OR all use `OMR_` prefix consistently: `OMR_KERNEL_TYPE`, `OMR_REAL_TARGET_ARCH`

---

### Issue 5.2: Misleading Function Names
**File**: `/home/user/openmptcprouter/common/files/usr/bin/wifi-autoconfig.sh`
**Line**: 120

**Severity**: LOW

**Problem**: Function name doesn't match behavior:

```bash
configure_all_radios() {
    # Actually does:
    # 1. Generate password
    # 2. Get device name
    # 3. Save to file
    # 4. Configure each radio
    # 5. Configure each interface
    # This should be: setup_wifi_complete() or initialize_wifi()
}
```

---

### Issue 5.3: Ambiguous Variable Names
**File**: `/home/user/openmptcprouter/common/files/usr/bin/usb-modem-autoconfig.sh`
**Lines**: Throughout

**Severity**: MEDIUM

**Problem**: Single-letter or abbreviated variables in complex logic:

```bash
# Line 23
local iface=$(basename "$dev")

# Line 25
local net_iface=$(ls -1 /sys/class/usbmisc/$iface/device/net/ 2>/dev/null | head -n1)

# Line 26
modems="$modems qmi:$net_iface:$dev"

# What do iface, net_iface, dev represent?
# Should be: control_device, network_interface, usb_device_path
```

---

### Issue 5.4: Non-Descriptive Return Codes
**File**: `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh`
**Lines**: Throughout

**Severity**: LOW

**Problem**: Boolean logic unclear:

```bash
# Line 19
is_bad_ip() {
    local ip="$1"
    if echo "$ip" | grep -q "^169\.254\."; then
        return 0  # This means "bad" but 0 typically means "success"
    fi
    return 1  # This means "good" but 1 typically means "error"
}

# Calling code (line 53):
if is_bad_ip "$lan_ip"; then
    log_msg "ERROR: LAN has invalid IP: $lan_ip"
    return 1
fi
# This is confusing - function returns 0 for bad, 1 for good
```

**Better approach**: Use explicit function names or write clearly:

```bash
# Option 1: Rename function
is_valid_ip() {
    # Returns 0 if valid, 1 if invalid
}

# Option 2: Add comment
is_bad_ip() {
    # Returns 0 if bad (APIPA or invalid), 1 if good
    ...
}
```

---

## 6. TRAILING WHITESPACE & FORMATTING ISSUES

**File**: `/home/user/openmptcprouter/common/files/usr/bin/`
**Total**: 1,230 trailing whitespace instances across 6 scripts

**Severity**: LOW (style issue, but affects git diffs)

**Problem**: Excessive trailing spaces create noise in version control:

```bash
# Example from network-monitor.sh line 50
auto_configure_wifi() {          # <-- trailing spaces
```

**Fix**: `sed -i 's/[[:space:]]*$//' *.sh`

---

## 7. UNPROTECTED VARIABLE EXPANSIONS

### Issue 7.1: Unquoted Variable Expansion in Dangerous Contexts
**File**: `/home/user/openmptcprouter/build.sh`
**Lines**: 71, 150, 155

**Severity**: HIGH

**Problem**: Unquoted variable expansion in path operations:

```bash
# Line 71
elif [ "$OMR_TARGET" = "wrt3200acm" ] || [ "$OMR_TARGET" = "wrt32x" ]; then

# Lines 150, 155 - Multiple unquoted expansions
rm -rf "$OMR_TARGET/${OMR_KERNEL}/source/bin"    # Good - quoted
rm -rf ${CUSTOM_FEED}/source/${OMR_TARGET}/${OMR_KERNEL}/* # BAD - unquoted

# Line 59
all_ports=$(echo "$all_ports" | sed "s/$port//g" | xargs)
# Variable $port not escaped in sed pattern - will break if $port contains regex chars
```

**Risk**: If $port = "eth[0]", sed will interpret as character class.

**Better approach**:
```bash
all_ports=$(echo "$all_ports" | sed "s/$(echo "$port" | sed -e 's/[\/&]/\\&/g')//g" | xargs)
# Or better:
all_ports="${all_ports% $port *}${all_ports##* $port }"
```

---

## 8. HARDCODED VALUES

### Issue 8.1: Hardcoded IP Addresses
**Files**:
- `/home/user/openmptcprouter/common/files/usr/bin/port-autoconfig.sh` (line 138)
- `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh` (lines 182, 212)
- `/home/user/openmptcprouter/common/files/usr/bin/emergency-lan-restore.sh` (line 83)
- `/home/user/openmptcprouter/common/files/usr/bin/network-monitor.sh` (line 167)

**Severity**: MEDIUM

**Problem**: IP address 192.168.2.1 hardcoded 5+ times:

```bash
set network.lan.ipaddr='192.168.2.1'  # Multiple files
```

**Better approach**: Define as constant at script top:
```bash
LAN_IP_DEFAULT="192.168.2.1"
LAN_NETMASK_DEFAULT="255.255.255.0"
```

---

### Issue 8.2: Hardcoded Sleep Values
**Files**:
- `/home/user/openmptcprouter/common/files/usr/bin/port-autoconfig.sh` (line 174)
- `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh` (lines 253, 270)
- `/home/user/openmptcprouter/common/files/usr/bin/usb-modem-autoconfig.sh` (line 230)

**Severity**: LOW

**Problem**: Sleep durations scattered throughout code:

```bash
sleep 5         # Line 174 - port-autoconfig
sleep 30        # Line 253 - network-safety-monitor
sleep 10        # Line 270 - network-safety-monitor
sleep 15        # network-monitor
sleep 3         # usb-modem-autoconfig
```

**Better approach**:
```bash
STARTUP_DELAY=5
HEALTH_CHECK_INTERVAL=30
RECOVERY_WAIT=10
```

---

## 9. MISSING VALIDATIONS

### Issue 9.1: No Validation of uci get Results
**File**: `/home/user/openmptcprouter/common/files/usr/bin/network-safety-monitor.sh`
**Lines**: 43-66

**Severity**: HIGH

**Problem**: Assumes uci values exist without checking:

```bash
check_lan_accessible() {
    local lan_ip=$(uci -q get network.lan.ipaddr)     # Could be empty
    local lan_proto=$(uci -q get network.lan.proto)   # Could be empty
    
    # Line 47: Direct comparison without null check
    if [ "$lan_proto" = "dhcp" ]; then
        log_msg "ERROR: LAN set to DHCP - this will cause APIPA addresses!"
        return 1
    fi
    
    # Line 53: Uses lan_ip without checking if empty
    if is_bad_ip "$lan_ip"; then
```

**Better approach**:
```bash
check_lan_accessible() {
    local lan_ip=$(uci -q get network.lan.ipaddr)
    [ -n "$lan_ip" ] || { log_msg "ERROR: LAN IP not configured"; return 1; }
    
    local lan_proto=$(uci -q get network.lan.proto)
    [ -n "$lan_proto" ] || { log_msg "ERROR: LAN protocol not configured"; return 1; }
    
    ...
}
```

---

### Issue 9.2: No Validation of File Operations
**File**: `/home/user/openmptcprouter/common/files/usr/bin/usb-modem-autoconfig.sh`
**Lines**: 168-177

**Severity**: MEDIUM

**Problem**: Directory and file operations without error checks:

```bash
# Line 168-169: mkdir without check, file write without check
local status_dir="/var/run/modem-status"
mkdir -p "$status_dir"  # Could fail (permission denied)

# Line 170-177: File write without validation
cat > "$status_dir/$wan_name" <<-EOFF
    ...
EOFF
# No check if write succeeded
```

---

## 10. MISCELLANEOUS ISSUES

### Issue 10.1: Inconsistent Error Messages
**Files**: Multiple

**Severity**: LOW

**Problem**: Error message formats vary:

```bash
# Different formats:
log_msg "ERROR: LAN set to DHCP..."        # Format 1
log_msg "ERROR: LAN has invalid IP..."     # Format 2
log_msg "CRITICAL: No ports available..."  # Format 3 (different prefix)
log_msg "✗ Error: ..." (vps-scripts)       # Format 4 (with symbol)
echo -e "${RED}Error: ...${NC}" (vps-scripts)  # Format 5
```

---

### Issue 10.2: Missing Script Headers/Documentation
**File**: `/home/user/openmptcprouter/common/files/usr/bin/network-monitor.sh`

**Severity**: LOW

**Problem**: No description of what script does beyond comment on line 4:

```bash
# Line 1-7: Minimal header
#!/bin/sh
#
# OpenMPTCProuter Optimized - Network Health Monitor
# Simple monitoring service that ensures DHCP and WiFi keep running
# Does NOT reconfigure ports - user has full control via web UI
#

# Missing:
# - Usage instructions
# - Configuration options
# - Exit codes documented
# - Return values from functions
```

---

## Summary Table

| Category | Count | Severity | Impact |
|----------|-------|----------|--------|
| Critical Syntax Errors | 2 | CRITICAL | Build failure |
| Code Duplication | 4 major patterns | MEDIUM | Maintenance burden |
| Missing Error Handling | 8+ instances | HIGH | Silent failures |
| Complex Functions | 3+ functions | MEDIUM | Hard to maintain |
| Poor Naming | 5+ violations | LOW-MEDIUM | Confusion |
| Hardcoded Values | 10+ instances | MEDIUM | Inflexible config |
| Missing Validations | 4 instances | HIGH | Crash risk |
| Other Issues | 5+ | LOW | Code quality |

## Recommendations (Priority Order)

1. **CRITICAL**: Fix syntax error in build.sh lines 291, 299 (change `->` to `>`)
2. **HIGH**: Add error checking after all uci commit/set operations
3. **HIGH**: Add validation of command results before using
4. **MEDIUM**: Extract `log_msg()` to shared library
5. **MEDIUM**: Refactor build.sh feed configuration into separate functions
6. **MEDIUM**: Extract repeated `uci show network` queries into helper function
7. **MEDIUM**: Define constants for hardcoded values (IPs, sleep durations)
8. **LOW**: Remove trailing whitespace
9. **LOW**: Standardize error message format across all scripts
10. **LOW**: Add comprehensive documentation headers
