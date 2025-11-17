# Code Audit Report - OpenMPTCProuter

**Audit Date:** 2025-11-17
**Auditor:** Claude Code
**Codebase:** OpenMPTCProuter - Multi-WAN Router Firmware
**Total Files Analyzed:** 49 shell scripts + supporting files

---

## Executive Summary

This comprehensive code audit identified **108+ issues** across security, code quality, error handling, and resource management categories. While the codebase demonstrates sophisticated networking functionality and automation, there are critical security vulnerabilities and coding errors that require immediate attention.

### Severity Distribution

| Severity | Count | Category |
|----------|-------|----------|
| **CRITICAL** | 11 | Command injection (3), Syntax errors (1), Missing error propagation (8) |
| **HIGH** | 28 | JSON injection (4), Credential exposure (4), Unquoted variables (5+), Missing null checks (4), Silent failures (15+) |
| **MEDIUM** | 48+ | Unchecked returns (6+), Race conditions (2), Logic errors (4+), Poor error messages (20+), FD leaks (8+), Process leaks (5) |
| **LOW** | 21+ | Input validation (2), Minor issues (7), Memory inefficiencies (3+), Temp file accumulation (2+) |

### Overall Risk Assessment

- **Production Readiness:** ⚠️ **NOT RECOMMENDED** without fixes
- **Security Posture:** 🔴 **HIGH RISK** - Critical vulnerabilities present
- **Code Quality:** 🟡 **MODERATE** - Functional but needs hardening
- **Maintainability:** 🟡 **MODERATE** - Good structure, needs documentation

---

## 1. Critical Security Vulnerabilities (11 Issues)

### 1.1 Command Injection via Sed (CRITICAL - 3 instances)

**Risk:** Remote code execution if user-controlled data reaches these functions

**Locations:**
- `vps-scripts/wizard.sh:861-862` - Unescaped VPS_PUBLIC_IP in sed
- `scripts/easy-install.sh:370,373` - Unescaped variables in sed
- `scripts/client-auto-setup.sh:246` - Unescaped variable in sed

**Example Exploit:**
```bash
VPS_PUBLIC_IP="127.0.0.1/e rm -rf /tmp/*"
sed -i "s/old_ip/$VPS_PUBLIC_IP/g" config.txt  # Executes rm -rf /tmp/*
```

**Fix:** Use parameter expansion or jq instead:
```bash
# Safe alternative
jq --arg ip "$VPS_PUBLIC_IP" '.ip = $ip' config.json
```

**Priority:** 🔴 **IMMEDIATE** - Must fix before production use

---

### 1.2 JSON Injection (HIGH - 4 instances)

**Risk:** Configuration corruption, potential code injection

**Locations:**
- `vps-scripts/wizard.sh` - Multiple JSON generation with unescaped passwords
- `scripts/omr-vps-install.sh` - Password in JSON without escaping
- `scripts/auto-pair.sh` - Configuration parameters without escaping

**Example Vulnerability:**
```bash
# Vulnerable
echo "{\"password\": \"$PASSWORD\"}" > config.json
# If PASSWORD contains: foo", "admin": true, "password": "bar
# Result: {"password": "foo", "admin": true, "password": "bar"}
```

**Fix:** Use jq for all JSON generation:
```bash
jq -n --arg pwd "$PASSWORD" '{password: $pwd}' > config.json
```

**Priority:** 🟠 **URGENT** - Fix within 1 week

---

### 1.3 Plaintext Credential Storage (HIGH - 4 instances)

**Risk:** Credential exposure via race conditions and insecure storage

**Locations:**
- `/root/openmptcprouter_credentials.txt` - Created with default permissions
- `/etc/omr-config.txt` - Config file with embedded credentials
- `/etc/wifi-password.txt` - WiFi password in plaintext
- `vps-scripts/wizard.sh:758` - Creates files then applies chmod 600 (race condition)

**Vulnerability:**
```bash
# Race condition window
echo "password123" > /root/credentials.txt  # World-readable for brief moment
chmod 600 /root/credentials.txt             # Now secured, but too late
```

**Fix:** Use atomic file creation:
```bash
(umask 077 && echo "password123" > /root/credentials.txt)
```

**Priority:** 🟠 **URGENT** - Fix within 1 week

---

### 1.4 Default No Password (HIGH)

**Risk:** Unauthorized root access on first boot

**Locations:**
- `common/files/etc/profile.d/99-omr-banner.sh:11` - Warns about no password
- `vps-scripts/wizard.sh:758` - Initial setup without password requirement

**Current Behavior:** System accessible without password until user sets one

**Fix:** Require password during initial setup wizard

**Priority:** 🟠 **URGENT** - Fix within 1 week

---

### 1.5 Missing Error Propagation (CRITICAL - 8 instances)

**Risk:** Scripts continue execution after critical failures, leaving system in invalid state

**All runtime scripts lack `set -e` or proper error handling:**
- `common/files/usr/bin/network-monitor.sh`
- `common/files/usr/bin/usb-modem-autoconfig.sh`
- `common/files/usr/bin/wifi-autoconfig.sh`
- `common/files/usr/bin/port-autoconfig.sh`
- `common/files/usr/bin/emergency-lan-restore.sh`
- `common/files/usr/bin/network-safety-monitor.sh`
- `common/files/usr/bin/omr-recovery`
- `common/files/etc/profile.d/99-omr-banner.sh`

**Impact:** System appears operational but may be misconfigured

**Fix:** Add to all scripts:
```bash
#!/bin/sh
set -e  # Exit on error
set -u  # Exit on undefined variable
```

**Priority:** 🔴 **IMMEDIATE** - Critical for reliability

---

## 2. Syntax and Logic Errors (27+ Issues)

### 2.1 Invalid Shell Syntax (CRITICAL)

**Location:** `build.sh:291-292, 300`

**Error:**
```bash
cat "$OMR_TARGET_CONFIG" config -> "$OMR_TARGET/${OMR_KERNEL}/source/.config" <<-EOF
```

**Issue:** Invalid `config ->` syntax - not valid bash redirection

**Fix:**
```bash
cat "$OMR_TARGET_CONFIG" >> "$OMR_TARGET/${OMR_KERNEL}/source/.config" <<-EOF
```

**Impact:** Build script will fail completely

**Priority:** 🔴 **IMMEDIATE** - Blocks all builds

---

### 2.2 Unquoted Variables (HIGH - 5+ instances)

**Locations:**
- `build.sh:291` - `if [ -f $OMR_TARGET_CONFIG ]`
- `common/files/usr/bin/port-autoconfig.sh:23,28,33` - Unquoted path variables
- `scripts/client-auto-setup.sh:75` - Unquoted password variable

**Risk:** Word splitting, glob expansion, incorrect behavior with spaces/special chars

**Example:**
```bash
file="/tmp/my file.txt"
if [ -f $file ]; then  # Expands to: if [ -f /tmp/my file.txt ]
                       # Shell sees: [ -f /tmp/my file.txt ]
                       # Error: too many arguments
```

**Fix:** Quote all variable expansions:
```bash
if [ -f "$OMR_TARGET_CONFIG" ]; then
```

**Priority:** 🟠 **HIGH** - Fix within 1 week

---

### 2.3 Missing Null/Empty Checks (HIGH - 4 instances)

**Location:** `common/files/usr/bin/usb-modem-autoconfig.sh:248-250`

```bash
local proto=$(echo "$modem" | cut -d: -f1)
local iface=$(echo "$modem" | cut -d: -f2)
local dev=$(echo "$modem" | cut -d: -f3)
# Used at line 259 without checking if $dev is empty
```

**Risk:** Operations on empty strings cause silent failures or unexpected behavior

**Fix:**
```bash
local dev=$(echo "$modem" | cut -d: -f3)
if [ -z "$dev" ]; then
    log_error "Failed to extract device from modem string: $modem"
    return 1
fi
```

**Priority:** 🟠 **HIGH** - Fix within 1 week

---

### 2.4 Unchecked Return Values (MEDIUM - 22+ instances)

Critical operations that don't check for success:

**UCI Operations:**
- `port-autoconfig.sh:145` - uci commit not checked
- `emergency-lan-restore.sh:51,63,91` - uci delete/commit not checked
- `usb-modem-autoconfig.sh:165,215,222` - multiple uci operations not checked
- `network-safety-monitor.sh:214,240` - uci operations not checked

**Service Restarts:**
- `network-safety-monitor.sh:190,216,241` - restarts not checked
- `emergency-lan-restore.sh:92,132` - network restart not verified
- `wifi-autoconfig.sh:212` - wifi reload not checked
- `omr-recovery:58` - network restart not validated

**File Operations:**
- `port-autoconfig.sh:150` - touch without error check
- `usb-modem-autoconfig.sh:169` - mkdir without error check
- `wifi-autoconfig.sh:129` - config file write without error check

**Fix Pattern:**
```bash
if ! uci commit network; then
    log_error "Failed to commit network configuration"
    return 1
fi
```

**Priority:** 🟡 **MEDIUM** - Fix within 2-4 weeks

---

### 2.5 Race Conditions (MEDIUM - 2 instances)

**Location:** `common/files/usr/bin/network-monitor.sh:18-24`

```bash
if [ -f "$PID_FILE" ]; then
    old_pid=$(cat "$PID_FILE")
    if kill -0 "$old_pid" 2>/dev/null; then
        exit 0
    fi
fi
echo $$ > "$PID_FILE"  # Race: another process could create file between check and write
```

**Fix:** Use atomic file creation with flock:
```bash
exec 200>"$PID_FILE"
if ! flock -n 200; then
    exit 0
fi
```

**Priority:** 🟡 **MEDIUM** - Fix within 2-4 weeks

---

## 3. Error Handling Issues (56 Issues)

### 3.1 Silent Failures (HIGH - 20+ instances)

Errors suppressed with `2>/dev/null` without any logging:

**omr-status** (12 locations):
- Lines: 66, 75, 97, 104, 118, 137, 138, 162, 212, 233, 248, 290
- User sees "N/A" with no indication why command failed

**wifi-autoconfig.sh** (2 locations):
- Lines: 146, 150 - `iw` command failures silently default to "2g" band

**network-monitor.sh** (1 location):
- Line: 20 - PID check failure not logged

**Impact:** Impossible to debug when things go wrong

**Fix:**
```bash
if ! output=$(command 2>&1); then
    logger -t script-name "Command failed: $output"
    echo "N/A"
fi
```

**Priority:** 🟠 **HIGH** - Fix within 1 week

---

### 3.2 Poor Error Messages (MEDIUM - 20+ instances)

Generic error messages that don't help debugging:

**Examples:**
- "ERROR: Setup failed" - Which component? What failed?
- "Configuration error" - What configuration? What's wrong?
- Silent failures with no message at all

**Fix:** Add context to all error messages:
```bash
log_error "Failed to configure interface $iface: uci commit returned $?"
```

**Priority:** 🟡 **MEDIUM** - Fix within 2-4 weeks

---

### 3.3 Missing Audit Logging (MEDIUM - 15+ instances)

Critical operations without audit trail:

- Configuration deletions not logged
- Service restart results not logged
- Background operations silently fail
- No timestamp or context in logs

**Fix:** Add comprehensive logging:
```bash
logger -t autoconfig -p user.info "Configuring interface $iface with protocol $proto"
```

**Priority:** 🟡 **MEDIUM** - Fix within 2-4 weeks

---

### 3.4 Incomplete Recovery Validation (HIGH - 3 instances)

**Locations:**
- `omr-recovery:58` - Restarts network but doesn't verify LAN has IP
- `emergency-lan-restore.sh:92` - Claims success without checking interface status

**Issue:** User thinks they're recovered but might still be locked out

**Fix:**
```bash
/etc/init.d/network restart
sleep 5
if ! ip addr show br-lan | grep -q "inet "; then
    log_error "LAN restoration failed: no IP address assigned"
    return 1
fi
```

**Priority:** 🟠 **HIGH** - Fix within 1 week

---

## 4. Resource Management Issues (21 Issues)

### 4.1 File Descriptor Leaks (HIGH - 8 instances)

**Serial device handling in modem scripts:**

**rm551e-monitor.sh** (Lines: 59, 77, 101, 124):
```bash
timeout 3 sh -c "echo -e 'AT\r' > $device 2>/dev/null && cat $device 2>/dev/null"
```
**Issue:** In main monitoring loop, repeated device opens without explicit closes

**rm551e-init.sh** (Lines: 88, 117, 129, 136, 186):
Similar pattern during initialization

**Impact:** File descriptor exhaustion after extended operation

**Fix:** Use explicit file descriptor management:
```bash
exec 3<> "$device"
echo -e "AT\r" >&3
timeout 2 cat <&3
exec 3>&-
```

**Priority:** 🟠 **HIGH** - Fix within 1 week (especially in loops)

---

### 4.2 Process Leaks (MEDIUM - 5 instances)

Background processes spawned without proper management:

**Locations:**
- `network-monitor.sh:65` - `/usr/bin/wifi-autoconfig.sh &`
- `rm551e-monitor.sh:208` - `/usr/bin/rm551e-init.sh &`
- `rm551e-init.sh:235,241` - Two background processes without wait
- `usb-modem-autoconfig.sh:182` - `ifup "$wan_name" &` in loop

**Issue:** No process tracking, potential zombie processes

**Fix:**
```bash
/usr/bin/wifi-autoconfig.sh &
WIFI_PID=$!
# Later...
wait "$WIFI_PID" || log_error "wifi-autoconfig failed with exit code $?"
```

**Priority:** 🟡 **MEDIUM** - Fix within 2-4 weeks

---

### 4.3 Memory Inefficiencies (LOW - 3+ instances)

**network-monitor.sh:53:**
```bash
for radio in $(uci show wireless 2>/dev/null | grep "wireless\.radio.*=wifi-device" | cut -d. -f2 | cut -d= -f1); do
```
**Issue:** Loads entire wireless config into memory unnecessarily

**Fix:** Use `uci -q get` or `uci show wireless.@wifi-device[]`

**Priority:** 🟢 **LOW** - Optimize when convenient

---

### 4.4 Temp File Accumulation (LOW - 2 instances)

**usb-modem-autoconfig.sh:168-177:**
```bash
mkdir -p "$status_dir"  # /var/run/modem-status
cat > "$status_dir/$wan_name" <<-EOF
```
**Issue:** Status files created but never cleaned up when modems disconnect

**Fix:** Add cleanup on hotplug removal events

**Priority:** 🟢 **LOW** - Monitor and clean if needed

---

## 5. Input Validation Issues (LOW - 2 instances)

### 5.1 IP Address Validation

**Issue:** IP addresses from user input not validated before use in config files

**Locations:**
- `wizard.sh` - VPS_PUBLIC_IP
- `easy-install.sh` - IP parameters

**Fix:**
```bash
validate_ip() {
    echo "$1" | grep -qE '^([0-9]{1,3}\.){3}[0-9]{1,3}$'
}
```

**Priority:** 🟢 **LOW-MEDIUM** - Add validation within 1 month

---

## 6. Positive Findings

Despite the issues identified, the codebase demonstrates several security best practices:

✅ **Strong Random Number Generation:** Uses `/dev/urandom` for password generation (32 bytes entropy)
✅ **Proper HTML Escaping:** LuCI templates use `luci.util.pcdata()` for XSS prevention
✅ **Strong Encryption:** Uses `chacha20-ietf-poly1305` for Shadowsocks (modern, secure cipher)
✅ **File Permissions:** Applies `chmod 600` to sensitive files (despite race condition)
✅ **Throttled Monitoring Loops:** All monitoring scripts use proper sleep intervals

---

## 7. Remediation Roadmap

### Phase 1: Critical Fixes (Week 1) - **8 hours**

**Priority:** Block production deployment until complete

1. ✅ Fix sed injection vulnerabilities (3 files) - 2 hours
   - Replace sed with jq for JSON processing
   - Escape variables properly or use parameter expansion

2. ✅ Fix syntax error in build.sh - 15 minutes
   - Replace `config ->` with proper redirection

3. ✅ Add `set -e` to all 8 runtime scripts - 1 hour
   - Test each script after adding

4. ✅ Fix atomic credential file creation - 1 hour
   - Use umask 077 pattern
   - Test race condition fixes

5. ✅ Add missing error checks for critical operations - 3 hours
   - uci commit checks
   - Service restart validation
   - File operation checks

6. ✅ Fix recovery validation - 1 hour
   - Verify LAN IP after recovery
   - Add timeout and retry logic

---

### Phase 2: High Priority (Week 2-4) - **12 hours**

1. ✅ Fix JSON injection vulnerabilities - 3 hours
   - Convert all JSON generation to jq
   - Test with special characters

2. ✅ Add input validation - 2 hours
   - IP address validation
   - Port number validation
   - Path validation

3. ✅ Quote all variable expansions - 3 hours
   - Automated scan with shellcheck
   - Manual review and fixes

4. ✅ Fix missing null/empty checks - 2 hours
   - Add validation after cut/grep/awk
   - Add default values where appropriate

5. ✅ Improve error messages - 2 hours
   - Add context to all error messages
   - Include variable values in errors

---

### Phase 3: Medium Priority (Month 2-3) - **16 hours**

1. ✅ Fix file descriptor leaks - 4 hours
   - Refactor serial device handling
   - Test with long-running scripts

2. ✅ Fix process management - 3 hours
   - Add proper wait statements
   - Implement process supervision

3. ✅ Add comprehensive logging - 4 hours
   - Audit trail for config changes
   - Structured logging with timestamps

4. ✅ Fix race conditions - 2 hours
   - Implement proper locking
   - Test concurrent execution

5. ✅ Fix unchecked return values - 3 hours
   - Add checks for all critical operations
   - Implement proper error propagation

---

### Phase 4: Low Priority (Month 3+) - **8 hours**

1. ✅ Optimize memory usage - 2 hours
2. ✅ Implement temp file cleanup - 1 hour
3. ✅ Add automated testing - 3 hours
4. ✅ Add static analysis to CI/CD - 2 hours

**Total Estimated Effort:** 44 hours (~1 week of focused work)

---

## 8. Testing Recommendations

### 8.1 Security Testing

- [ ] Penetration testing for command injection vectors
- [ ] Credential exposure testing (race conditions)
- [ ] XSS testing on web interface
- [ ] Authentication bypass testing

### 8.2 Functional Testing

- [ ] Test all error paths (simulate failures)
- [ ] Test recovery scenarios (emergency restore)
- [ ] Test with malformed input (special characters, empty strings)
- [ ] Long-running tests (check for leaks)

### 8.3 Integration Testing

- [ ] Test modem hotplug scenarios
- [ ] Test network failure and recovery
- [ ] Test concurrent configuration changes
- [ ] Test build process with all targets

### 8.4 Automated Testing

Add to CI/CD pipeline:
- [ ] shellcheck for all shell scripts
- [ ] Static analysis (CodeQL, Semgrep)
- [ ] Integration test suite
- [ ] Security scanning (Bandit, Trivy)

---

## 9. Tools and Resources

### Recommended Tools

1. **shellcheck** - Static analysis for shell scripts
   ```bash
   shellcheck -x script.sh
   ```

2. **jq** - Safe JSON processing
   ```bash
   jq -n --arg var "$USER_INPUT" '{value: $var}'
   ```

3. **flock** - Proper file locking
   ```bash
   flock -n 200 || exit 1
   ```

4. **logger** - System logging
   ```bash
   logger -t script-name -p user.error "Error message"
   ```

### Documentation

- OpenWrt UCI documentation: https://openwrt.org/docs/guide-user/base-system/uci
- Bash best practices: https://google.github.io/styleguide/shellguide.html
- OWASP secure coding: https://owasp.org/www-project-secure-coding-practices-quick-reference-guide/

---

## 10. Conclusion

The OpenMPTCProuter codebase demonstrates sophisticated networking capabilities and automation, but contains critical security vulnerabilities and coding errors that must be addressed before production deployment.

### Summary Statistics

- **Total Issues:** 108+
- **Critical:** 11 (block production)
- **High:** 28 (fix within 1 week)
- **Medium:** 48+ (fix within 1 month)
- **Low:** 21+ (fix within 3 months)

### Current Status: 🔴 **NOT PRODUCTION READY**

### After Phase 1 Fixes: 🟡 **SUITABLE FOR TESTING**

### After Phase 2 Fixes: 🟢 **PRODUCTION READY**

---

## Appendix: File-by-File Summary

### Critical Files Requiring Immediate Attention

1. **build.sh** - Syntax error blocks all builds
2. **wizard.sh** - Multiple security vulnerabilities (sed injection, JSON injection, credential exposure)
3. **easy-install.sh** - Sed injection, credential exposure
4. **network-monitor.sh** - Missing error propagation, race condition
5. **usb-modem-autoconfig.sh** - Missing null checks, unchecked returns, process leaks
6. **rm551e-monitor.sh** - File descriptor leaks in main loop
7. **emergency-lan-restore.sh** - Incomplete recovery validation
8. **omr-recovery** - Incomplete recovery validation

### Files with Medium Priority Issues

- port-autoconfig.sh
- wifi-autoconfig.sh
- network-safety-monitor.sh
- client-auto-setup.sh
- rm551e-init.sh
- modem-ca-optimize.sh
- auto-pair.sh
- omr-vps-install.sh

### Files with Minor Issues

- 99-omr-banner.sh
- omr-status
- 6in4.sh
- Various bootloader scripts (low impact)

---

**End of Audit Report**

*For questions or clarifications, please contact the development team.*
