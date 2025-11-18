# OpenMPTCProuter - Configuration & Deployment Audit Report

**Agent 4: Config, Scripts & Deployment Auditor**
**Date:** 2025-11-18
**Status:** Complete Analysis - Recommendations Ready

---

## EXECUTIVE SUMMARY

This audit examined all configuration files, shell scripts, and deployment mechanisms in the OpenMPTCProuter Optimized project. The system uses **pure shell scripts** (no Python admin tools) with OpenWrt's UCI configuration system. Overall code quality is **GOOD with notable security improvements**, but several fragility areas require attention.

### Risk Level: **MODERATE**
- ✅ Good: Error handling with `set -e`, input validation improving, user experience excellent
- ⚠️  Moderate: Hardcoded values everywhere, limited rollback mechanisms
- 🔴 Critical: Insecure HTTP APIs, curl -k disabling SSL, plaintext credential storage

---

## 1. SCRIPT & CONFIG INVENTORY

### A. VPS/Server Setup Scripts

| File | Lines | Role | Fragility Level |
|------|-------|------|-----------------|
| `vps-scripts/wizard.sh` | 1033 | Full-featured interactive VPS setup | Medium |
| `vps-scripts/omr-vps-install.sh` | 510 | Non-interactive VPS automation | Medium |
| `vps-scripts/install.sh` | 56 | Download-and-execute wrapper | High |

**Key Issues:**
- Hardcoded ports: 65500 (Shadowsocks), 65510 (Glorytun TCP), 65520 (Glorytun UDP)
- Hardcoded web UI port: 8080
- IP detection via external services (ifconfig.me, icanhazip.com) - single point of failure
- Download-and-execute pattern with no signature verification

### B. Client/Router Setup Scripts

| File | Lines | Role | Fragility Level |
|------|-------|------|-----------------|
| `scripts/client-auto-setup.sh` | 329 | OpenWrt router automated configuration | Medium |
| `scripts/auto-pair.sh` | 389 | Bidirectional auto-pairing system | **HIGH** |
| `scripts/easy-install.sh` | 474 | One-command VPS installer | Medium |
| `quick-setup.sh` | 242 | Universal setup script | High |

**Critical Issues in auto-pair.sh:**
- Line 345: `curl -k` disables SSL verification (SECURITY RISK)
- Line 477: Unauthenticated HTTP API on port 9999 for auto-discovery
- Base64-encoded credentials (encoding, not encryption)
- No authentication required for pairing API

### C. Router Autoconfig Scripts

| File | Lines | Role | Quality |
|------|-------|------|---------|
| `common/files/usr/bin/port-autoconfig.sh` | 240 | First-boot port detection | **GOOD** |
| `common/files/usr/bin/wifi-autoconfig.sh` | 240 | Automatic WiFi radio configuration | Good |
| `common/files/usr/bin/network-monitor.sh` | 115 | Network health monitoring daemon | **EXCELLENT** |
| `common/files/usr/bin/emergency-lan-restore.sh` | 147 | Emergency LAN port restoration | Good |

**Good Patterns Found:**
- port-autoconfig.sh:27,129 - Input validation with regex
- port-autoconfig.sh - Uses quoted heredocs to prevent injection
- network-monitor.sh:19 - Atomic locking with `mkdir`
- network-monitor.sh:29 - PID validation against injection
- wifi-autoconfig.sh - Generates random passwords securely (umask 077)

**Fragile Areas:**
- wifi-autoconfig.sh:37 - Hardcoded country code "US"
- Assumes standard port naming (eth*, lan*, wan*)

### D. Build Scripts

| File | Lines | Role | Quality |
|------|-------|------|---------|
| `build.sh` | 1900+ | Main OpenWrt build system | **EXCELLENT** |
| `sign.sh` | 236 | Build signing script | Good |

**Excellent Patterns in build.sh:**
- Lines 59-71: Comprehensive dependency validation
- Lines 73-86: Disk space checking (30GB minimum required)
- Lines 55-56: Environment variable override support

**Issues:**
- Lines 124-142: Hardcoded git commit SHAs (brittle for updates)
- No checksum validation on external downloads

### E. Configuration Files

| File | Role | Issues |
|------|------|--------|
| `common/files/etc/uci-defaults/10-omr-network-defaults` | Sets static LAN IP | Hardcoded 192.168.2.1 |
| `common/files/etc/uci-defaults/90-omr-first-boot-wizard` | Interactive first-boot wizard | Line 477: HTTP-only auto-discovery |
| `common/files/etc/init.d/omr-diagnostics` | Init script for diagnostics | GOOD - uses procd with respawn limits |

---

## 2. FRAGILITY AUDIT - CRITICAL FINDINGS

### 🔴 CRITICAL: Security Vulnerabilities

#### 2.1 Insecure SSL Verification Bypass
**Location:** `scripts/auto-pair.sh:345`
```bash
curl -s --max-time 10 -k "https://$VPS_IP:9999/pair.json"
```
**Risk:** Man-in-the-middle attacks, credential interception
**Impact:** HIGH - Credentials transmitted over compromised connection

#### 2.2 Unauthenticated HTTP API
**Location:** `scripts/auto-pair.sh:345-351`, `common/files/etc/uci-defaults/90-omr-first-boot-wizard:477`
```javascript
fetch(`http://${vpsIp}:9999/pair.json`, { method: 'GET' })
```
**Risk:** Anyone can query pairing API and retrieve credentials
**Impact:** HIGH - No authentication required to access sensitive config

#### 2.3 Plaintext Credential Storage
**Locations:**
- `/root/openmptcprouter_credentials.txt` (chmod 600 but still plaintext)
- `/etc/openmptcprouter/config.json` (JSON with plaintext passwords)
- `/etc/shadowsocks-libev/config.json` (plaintext password field)

**Risk:** File system access = full credential compromise
**Impact:** MEDIUM - Mitigated by file permissions, but no encryption at rest

#### 2.4 Download-and-Execute Pattern
**Locations:** Multiple scripts
```bash
curl -sSL https://raw.githubusercontent.com/spotty118/...sh | bash
```
**Risk:** No signature verification, GitHub dependency, MITM attacks
**Impact:** MEDIUM - Relies entirely on GitHub infrastructure security

### ⚠️  MODERATE: Hardcoded Values

#### 2.5 Port Numbers (Throughout codebase)
```
Shadowsocks:   65500
Glorytun TCP:  65510
Glorytun UDP:  65520
Web UI:        8080
Pairing API:   9999
Router LAN:    192.168.2.1
```
**Impact:** Cannot deploy multiple instances without conflicts, difficult customization

**Affected Files:**
- vps-scripts/wizard.sh:175,195,432-437,487,523-525
- vps-scripts/omr-vps-install.sh:329-334,407-409,423,461,486-488
- common/files/etc/uci-defaults/10-omr-network-defaults:15

#### 2.6 IP Detection External Dependencies
```bash
VPS_IP=$(curl -4 -s --max-time 5 ifconfig.me || curl -4 -s icanhazip.com)
```
**Impact:** Installation fails if external services are down or blocked
**Locations:** wizard.sh, omr-vps-install.sh, auto-pair.sh, easy-install.sh

#### 2.7 Hardcoded Paths
- `/etc/openmptcprouter/config.json`
- `/root/openmptcprouter_credentials.txt`
- `/var/www/omr-setup`
- `/etc/omr-config.txt`

**Impact:** No flexibility for alternative installations

### ⚠️  MODERATE: Missing Dependency Checks

#### 2.8 Dependency Validation Gap
**GOOD:** build.sh has excellent dependency checking (lines 59-71)
**BAD:** Most deployment scripts assume tools exist

**Missing checks for:**
- `jq` (used in wizard.sh, omr-vps-install.sh, auto-pair.sh)
- `qrencode` (optional but no graceful degradation)
- `base64` (used for pairing codes)
- `ss-local`, `ss-redir` (Shadowsocks binaries)
- Kernel module availability (MPTCP, WireGuard)

**Example Issue:** client-auto-setup.sh:113-117
```bash
if ! opkg list-installed | grep -q shadowsocks-libev; then
    opkg install shadowsocks-libev-ss-redir shadowsocks-libev-ss-local
fi
```
✅ Good: Checks if installed
❌ Bad: Doesn't check if opkg install succeeded

### ⚠️  MODERATE: Environment Assumptions

#### 2.9 OS Detection
**GOOD:** Scripts validate OS (wizard.sh, omr-vps-install.sh)
**FRAGILE:** Only supports Debian 11/12/13, Ubuntu 20.04/22.04/24.04

```bash
if [[ ! "$VERSION_ID" =~ ^(11|12|13|20.04|22.04|24.04)$ ]]; then
    print_error "Unsupported OS version"
fi
```

#### 2.10 Hardware Assumptions
- WiFi scripts assume `iw` command works (no fallback)
- Modem scripts assume specific USB paths
- Port detection assumes eth*/lan*/wan* naming

### 🔶 LOW: Missing Rollback Mechanisms

#### 2.11 No Configuration Backups
**Issue:** UCI changes committed immediately without backup

**Example:** client-auto-setup.sh:120-130
```bash
uci set shadowsocks-libev.omr.server="$VPS_IP"
uci set shadowsocks-libev.omr.password="$VPS_PASSWORD"
uci commit shadowsocks-libev
```
**Risk:** No way to undo if configuration breaks network

#### 2.12 Service Restarts Without Confirmation
**Example:** client-auto-setup.sh:205-209
```bash
/etc/init.d/network reload > /dev/null 2>&1 &
sleep 2
/etc/init.d/firewall reload > /dev/null 2>&1 &
sleep 1
/etc/init.d/shadowsocks-libev restart > /dev/null 2>&1 &
```
**Issues:**
- Services restarted in background (can't detect failures)
- No health checks after restart
- Fixed sleep timers (race conditions)

---

## 3. ERROR HANDLING REVIEW

### ✅ GOOD: Consistent use of `set -e`

**Finding:** Most scripts use `set -e` (exit on error)
- wizard.sh:22
- omr-vps-install.sh:36
- auto-pair.sh:9
- client-auto-setup.sh:10
- easy-install.sh:7
- build.sh:11

**Impact:** Scripts will abort on first error (prevents cascading failures)

### ✅ GOOD: Error Functions

**Example:** wizard.sh:78-82
```bash
print_error() {
    echo -e "${RED}✗ Error:${NC} $1" >&2
    exit 1
}
```
**Quality:** Good separation, uses stderr, exits with non-zero code

### ⚠️  MODERATE: Inconsistent Exit Code Checking

**GOOD Example:** build.sh validates dependencies
```bash
if ! command -v "$tool" >/dev/null 2>&1; then
    MISSING_DEPS="$MISSING_DEPS $tool"
fi
```

**BAD Example:** client-auto-setup.sh doesn't check opkg install result
```bash
opkg install shadowsocks-libev-ss-redir shadowsocks-libev-ss-local > /dev/null 2>&1
# No check if install succeeded!
```

### 🔶 LOW: Silent Failures in Background Jobs

**Issue:** Services restarted in background with output redirected to /dev/null

**Impact:** Cannot detect if service failed to start

---

## 4. CONFIG GENERATION & VALIDATION

### ⚠️  MODERATE: Input Validation Quality

#### 4.1 GOOD Validation Examples

**IP Address Validation:** client-auto-setup.sh:53
```bash
if ! echo "$VPS_IP" | grep -Eq '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
    echo -e "${RED}Error: Invalid IP address format${NC}"
    exit 1
fi
```
✅ Uses regex for format validation

**Port Validation:** client-auto-setup.sh:66
```bash
if ! [ "$VPS_PORT" -ge 1 ] 2>/dev/null || ! [ "$VPS_PORT" -le 65535 ] 2>/dev/null; then
    echo -e "${RED}Error: Port must be between 1-65535${NC}"
    exit 1
fi
```
✅ Validates numeric range

**Port Name Validation:** port-autoconfig.sh:27,129
```bash
if ! echo "$port" | grep -Eq '^[a-zA-Z0-9_-]+$'; then
    logger -t port-autoconfig "Invalid port name: $port"
    continue
fi
```
✅ Uses regex to prevent injection

#### 4.2 WEAK Validation Examples

**Password Escaping:** easy-install.sh:376 (ACTUALLY GOOD!)
```bash
PASSWORD_ESCAPED=$(printf '%s\n' "$PASSWORD" | sed 's/[&/\]/\\&/g')
sed -i "s|REPLACE_PASSWORD|$PASSWORD_ESCAPED|g" /var/www/omr-setup/index.html
```
✅ **This is GOOD** - escapes special characters before sed replacement

**Pairing Code Validation:** 90-omr-first-boot-wizard:449-460
```javascript
try {
    const decoded = JSON.parse(atob(code));
    configData = { ip: decoded.ip, port: decoded.port || 65500, ... };
} catch (e) {
    alert('Invalid pairing code.');
}
```
⚠️  Validates format but doesn't validate IP/port ranges

#### 4.3 Config Generation Issues

**Potential Injection Risk:** 90-omr-first-boot-wizard
- JavaScript embedded in shell heredoc
- JSON data from web form parsed without schema validation
- Relies on try/catch for validation (reactive, not proactive)

**No Schema Validation:** Config files generated without formal schemas
- /etc/openmptcprouter/config.json written directly
- No validation of required fields before writing

---

## 5. SAFE IMPROVEMENTS - PROPOSED CHANGES

### Priority 1: CRITICAL SECURITY FIXES

#### 5.1 Remove SSL Verification Bypass
**File:** `scripts/auto-pair.sh:345`

**Current:**
```bash
curl -s --max-time 10 -k "https://$VPS_IP:9999/pair.json"
```

**Proposed:**
```bash
# Try HTTPS with proper verification first, fallback to HTTP with warning
if CONFIG_JSON=$(curl -s --max-time 10 "https://$VPS_IP:9999/pair.json" 2>/dev/null); then
    echo -e "${GREEN}✓ Secure connection (HTTPS)${NC}"
elif CONFIG_JSON=$(curl -s --max-time 10 "http://$VPS_IP:9999/pair.json" 2>/dev/null); then
    echo -e "${YELLOW}⚠ WARNING: Using insecure HTTP connection${NC}"
    echo -e "${YELLOW}⚠ Credentials transmitted in plaintext!${NC}"
    read -p "Continue anyway? (y/N): " -n 1 -r < /dev/tty
    [[ ! $REPLY =~ ^[Yy]$ ]] && exit 1
else
    echo -e "${RED}Error: Could not connect to VPS${NC}"
    exit 1
fi
```

**Impact:** Eliminates MITM attack surface, warns users about insecure fallback

#### 5.2 Add Authentication to Pairing API
**Recommendation:** Add a one-time token for pairing API

**Implementation:**
```bash
# On VPS: Generate one-time pairing token
PAIRING_TOKEN=$(head -c 16 /dev/urandom | base64 -w0)
echo "$PAIRING_TOKEN" > /etc/openmptcprouter/pairing-token
chmod 600 /etc/openmptcprouter/pairing-token

# Show token to user for manual entry
echo "Pairing Token: $PAIRING_TOKEN"

# On router: Require token in request
curl -H "Authorization: Bearer $PAIRING_TOKEN" "http://$VPS_IP:9999/pair.json"
```

**Impact:** Prevents unauthorized access to pairing API

#### 5.3 Encrypt Credentials at Rest
**Recommendation:** Use age or openssl to encrypt stored credentials

**Example:**
```bash
# Encrypt credentials with derived key
echo "$SHADOWSOCKS_PASS" | openssl enc -aes-256-cbc -salt -pbkdf2 \
    -out /etc/openmptcprouter/credentials.enc
chmod 600 /etc/openmptcprouter/credentials.enc
```

**Impact:** Reduces risk if file system is compromised

### Priority 2: HIGH - Configuration Flexibility

#### 5.4 Environment Variable Support for Ports
**Files:** All VPS/client scripts

**Proposed:**
```bash
# Support environment variable overrides
OMR_SHADOWSOCKS_PORT=${OMR_SHADOWSOCKS_PORT:-65500}
OMR_GLORYTUN_TCP_PORT=${OMR_GLORYTUN_TCP_PORT:-65510}
OMR_GLORYTUN_UDP_PORT=${OMR_GLORYTUN_UDP_PORT:-65520}
OMR_WEB_UI_PORT=${OMR_WEB_UI_PORT:-8080}
OMR_PAIRING_PORT=${OMR_PAIRING_PORT:-9999}
OMR_LAN_IP=${OMR_LAN_IP:-192.168.2.1}
```

**Benefits:**
- Allows multiple VPS instances on same server
- Enables custom network configurations
- Backwards compatible (defaults unchanged)

#### 5.5 Configuration File for Defaults
**Create:** `/etc/openmptcprouter/defaults.conf`

```bash
# OpenMPTCProuter Default Configuration
# Override by setting environment variables

# VPS Ports
SHADOWSOCKS_PORT=65500
GLORYTUN_TCP_PORT=65510
GLORYTUN_UDP_PORT=65520
WEB_UI_PORT=8080
PAIRING_API_PORT=9999

# Router Configuration
LAN_IP=192.168.2.1
LAN_NETMASK=255.255.255.0
DHCP_START=100
DHCP_LIMIT=150

# External IP Detection Services
IP_DETECT_SERVICES="ifconfig.me icanhazip.com ipinfo.io/ip"
```

**Load in scripts:**
```bash
[ -f /etc/openmptcprouter/defaults.conf ] && . /etc/openmptcprouter/defaults.conf
```

### Priority 3: MEDIUM - Robustness Improvements

#### 5.6 Dependency Checking Function
**Create:** Common dependency checking function for all scripts

```bash
check_dependencies() {
    local missing=""
    for tool in "$@"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing="$missing $tool"
        fi
    done

    if [ -n "$missing" ]; then
        echo -e "${RED}Error: Missing required dependencies:${NC}$missing"
        echo "Please install them before continuing."
        return 1
    fi
    return 0
}

# Usage:
check_dependencies curl wget jq base64 || exit 1
```

#### 5.7 UCI Rollback Support
**Implement:** Automatic UCI backup before changes

```bash
uci_safe_commit() {
    local config="$1"

    # Create backup
    local backup_file="/tmp/uci-backup-${config}-$(date +%s)"
    uci export "$config" > "$backup_file"

    # Commit changes
    if uci commit "$config"; then
        echo -e "${GREEN}✓ Configuration committed${NC}"
        echo "Backup: $backup_file"
        return 0
    else
        echo -e "${RED}✗ Configuration commit failed${NC}"
        echo "Restoring from backup..."
        uci import "$config" < "$backup_file"
        return 1
    fi
}
```

#### 5.8 Service Health Checks
**Implement:** Wait for service confirmation instead of sleep

```bash
wait_for_service() {
    local service="$1"
    local timeout="${2:-30}"
    local elapsed=0

    echo -n "Waiting for $service to start..."
    while [ $elapsed -lt $timeout ]; do
        if /etc/init.d/"$service" status >/dev/null 2>&1; then
            echo -e " ${GREEN}✓${NC}"
            return 0
        fi
        sleep 1
        elapsed=$((elapsed + 1))
        echo -n "."
    done

    echo -e " ${RED}✗ timeout${NC}"
    return 1
}

# Usage:
/etc/init.d/shadowsocks-libev restart
wait_for_service shadowsocks-libev || {
    echo "Warning: Service may not have started correctly"
}
```

#### 5.9 Multiple IP Detection Fallbacks
**Improve:** IP detection with multiple services

```bash
detect_public_ip() {
    local services=(
        "https://ifconfig.me"
        "https://icanhazip.com"
        "https://ipinfo.io/ip"
        "https://api.ipify.org"
    )

    for service in "${services[@]}"; do
        if IP=$(curl -4 -s --max-time 5 "$service" 2>/dev/null) && [ -n "$IP" ]; then
            # Validate IP format
            if echo "$IP" | grep -Eq '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
                echo "$IP"
                return 0
            fi
        fi
    done

    return 1
}

VPS_IP=$(detect_public_ip) || {
    echo -e "${YELLOW}Could not auto-detect IP. Please enter manually:${NC}"
    read -r -p "VPS Public IP: " VPS_IP < /dev/tty
}
```

#### 5.10 Exit Code Validation for Critical Commands
**Pattern:** Check critical command results

```bash
# Before:
opkg install shadowsocks-libev-ss-redir > /dev/null 2>&1

# After:
if ! opkg install shadowsocks-libev-ss-redir > /dev/null 2>&1; then
    echo -e "${RED}Error: Failed to install shadowsocks-libev${NC}"
    echo "Please check network connection and opkg configuration"
    exit 1
fi
echo -e "${GREEN}✓ Shadowsocks installed${NC}"
```

### Priority 4: LOW - User Experience

#### 5.11 Add --help and --version Flags
**All user-facing scripts should support:**

```bash
show_help() {
    cat << EOF
Usage: $(basename "$0") [OPTIONS]

OpenMPTCProuter VPS Installation Wizard

OPTIONS:
    -h, --help          Show this help message
    -v, --version       Show version information
    --non-interactive   Run without prompts (requires env vars)
    --port PORT         Set Shadowsocks port (default: 65500)
    --ip IP             Set VPS IP (default: auto-detect)

ENVIRONMENT VARIABLES:
    OMR_VPS_IP              VPS public IP address
    OMR_SHADOWSOCKS_PORT    Shadowsocks port (default: 65500)
    OMR_WEB_UI_PORT         Web UI port (default: 8080)

EXAMPLES:
    # Interactive mode
    sudo ./wizard.sh

    # Non-interactive with custom port
    sudo OMR_SHADOWSOCKS_PORT=8388 ./wizard.sh --non-interactive

For more information: https://github.com/spotty118/openmptcprouter
EOF
}

case "${1:-}" in
    -h|--help) show_help; exit 0 ;;
    -v|--version) echo "OpenMPTCProuter Optimized v$VERSION"; exit 0 ;;
esac
```

#### 5.12 Add Dry-Run Mode
```bash
DRY_RUN=${DRY_RUN:-0}

uci_set() {
    if [ "$DRY_RUN" -eq 1 ]; then
        echo "[DRY-RUN] uci set $*"
    else
        uci set "$@"
    fi
}

# Usage: DRY_RUN=1 ./script.sh
```

---

## 6. IMPLEMENTATION PLAN

### Phase 1: Security Fixes (IMMEDIATE)
- [ ] Remove `-k` flag from curl in auto-pair.sh
- [ ] Add warning for HTTP fallback in pairing
- [ ] Implement pairing API authentication token
- [ ] Add input validation for all user-provided values

**Risk:** LOW - These are additions/warnings, not breaking changes
**Effort:** 2-4 hours
**Files:** auto-pair.sh, 90-omr-first-boot-wizard

### Phase 2: Configuration Flexibility (HIGH PRIORITY)
- [ ] Create `/etc/openmptcprouter/defaults.conf`
- [ ] Add environment variable support for all hardcoded values
- [ ] Update all scripts to source defaults.conf
- [ ] Add --help and --version to user-facing scripts

**Risk:** LOW - Backwards compatible (defaults unchanged)
**Effort:** 6-8 hours
**Files:** All VPS/client scripts, UCI defaults

### Phase 3: Robustness (MEDIUM PRIORITY)
- [ ] Implement `check_dependencies()` function
- [ ] Add dependency checks to all scripts
- [ ] Implement `uci_safe_commit()` with backups
- [ ] Replace sleep timers with service health checks
- [ ] Add exit code checking for critical commands

**Risk:** LOW - Improves reliability without changing behavior
**Effort:** 8-12 hours
**Files:** All scripts

### Phase 4: Advanced Features (OPTIONAL)
- [ ] Implement credential encryption at rest
- [ ] Add dry-run mode to all scripts
- [ ] Create automated test suite for deployment scenarios
- [ ] Add rollback capability for failed installations

**Risk:** MEDIUM - Requires careful testing
**Effort:** 16-24 hours
**Files:** All scripts, new testing framework

---

## 7. DRY-RUN DEPLOYMENT ANALYSIS

### 7.1 Fresh Install Scenario

**VPS Fresh Install:**
1. ✅ User runs: `curl -sSL https://.../wizard.sh | sudo bash`
2. ✅ Script detects OS, validates version
3. ✅ Script auto-detects public IP (or prompts)
4. ✅ Generates secure passwords
5. ⚠️  Installs packages (no checksum validation)
6. ⚠️  Configures firewall (no backup, no rollback)
7. ✅ Creates systemd services
8. ✅ Generates web UI with QR code
9. ⚠️  Services started in background (no health check)
10. ✅ Shows pairing code to user

**Risks:**
- If step 5 fails partway through, system in inconsistent state
- If step 6 fails, firewall rules may be incomplete (security risk)
- No way to undo if user made a mistake

**Router Fresh Install:**
1. ✅ Flash OpenMPTCProuter image
2. ✅ On first boot, UCI defaults run automatically
3. ✅ LAN IP set to 192.168.2.1 (static)
4. ✅ DHCP server enabled
5. ✅ Port autoconfig detects WAN/LAN
6. ✅ WiFi autoconfig generates random passwords
7. ✅ Web wizard shows setup options
8. ⚠️  User enters pairing code or VPS IP
9. ⚠️  Auto-discovery fetches config via HTTP (insecure)
10. ⚠️  UCI changes committed immediately (no backup)
11. ⚠️  Services restarted in background (no confirmation)

**Risks:**
- Step 9: Credentials transmitted over HTTP (if HTTPS fails)
- Step 10: No rollback if config breaks network
- Step 11: User might reboot before services start

### 7.2 Upgrade Scenario

**Current State:** No upgrade mechanism found!

**Issues:**
- Scripts are designed for fresh install only
- No version detection
- No migration path for config changes
- Running wizard.sh again would overwrite credentials

**Recommendation:** Create `upgrade.sh` script that:
1. Detects current version from `/etc/openmptcprouter/version`
2. Backs up current configuration
3. Updates binaries and scripts
4. Migrates configuration if schema changed
5. Validates new config before committing
6. Offers rollback if validation fails

### 7.3 Restart/Reboot Scenarios

**VPS Restart:**
- ✅ systemd services have Restart=always
- ✅ Services start automatically on boot
- ❓ No health check after boot
- ⚠️  If firewall rules lost, VPN won't work (iptables not persistent by default)

**Recommendation:** Ensure iptables-persistent is installed

**Router Reboot:**
- ✅ UCI config persists across reboots
- ✅ OpenRC/procd restarts services automatically
- ✅ Network monitor daemon restarts interfaces if needed
- ✅ Emergency LAN restore activates if locked out

**Verdict:** Reboot handling is GOOD for router, MODERATE for VPS

### 7.4 What's Now Safer

**After implementing proposed changes:**
- 🔐 Credentials less vulnerable to MITM attacks
- 🔐 Pairing API requires authentication
- 🎯 Multiple VPS instances possible (configurable ports)
- 🔄 UCI changes can be rolled back
- ✅ Services confirmed to be running (not just assumed)
- 📦 Missing dependencies detected before failure
- 🌐 IP detection more resilient (multiple services)
- 📝 Clear error messages with recovery suggestions

### 7.5 What Still Feels Risky

**Even after improvements:**
- ⚠️  Credentials stored in plaintext (even if encrypted, key is on same system)
- ⚠️  Download-and-execute pattern (curl | bash) - no signature verification
- ⚠️  No formal upgrade/migration mechanism
- ⚠️  Firewall rules not guaranteed persistent on VPS reboot
- ⚠️  No health monitoring after initial setup
- ⚠️  If GitHub is compromised, installation scripts could be malicious

**Recommendations for Future:**
1. Implement GPG signature verification for scripts
2. Create proper upgrade system with rollback
3. Add health monitoring daemon on VPS
4. Consider using systemd-resolved or unbound instead of external IP services
5. Implement secrets management (HashiCorp Vault, age, etc.)

---

## 8. CONCLUSION

### Summary of Findings

**Strengths:**
- ✅ Excellent user experience (QR codes, web wizards, auto-pairing)
- ✅ Good error handling foundation (`set -e`, error functions)
- ✅ Input validation improving (regex, range checks)
- ✅ Build system has excellent dependency/disk checks
- ✅ Some scripts show security awareness (atomic locks, PID validation)

**Critical Issues:**
- 🔴 SSL verification bypass (`curl -k`)
- 🔴 Unauthenticated pairing API on port 9999
- 🔴 Credentials in plaintext storage

**Moderate Issues:**
- ⚠️  Hardcoded ports/IPs throughout
- ⚠️  Limited dependency checking in deployment scripts
- ⚠️  No rollback mechanisms for UCI/config changes
- ⚠️  Download-and-execute without signature verification

**Overall Assessment:**
The codebase is **well-structured and user-friendly**, but has **security and flexibility gaps**. The proposed improvements are **backwards-compatible** and can be implemented **incrementally** with **low risk**.

### Recommended Next Steps

1. **IMMEDIATE:** Implement Priority 1 security fixes (2-4 hours)
2. **THIS WEEK:** Add configuration flexibility (6-8 hours)
3. **THIS MONTH:** Improve robustness with dependency checks and rollback (8-12 hours)
4. **FUTURE:** Consider advanced features (encryption, upgrade system)

### Success Metrics

After implementation, we should achieve:
- ✅ Zero SSL verification bypasses
- ✅ All pairing APIs require authentication
- ✅ All hardcoded values overrideable via env vars or config file
- ✅ All critical dependencies checked before use
- ✅ All UCI changes backed up before commit
- ✅ All service restarts confirmed (not assumed)
- ✅ All scripts have --help and --version

---

**End of Audit Report**
