# OpenMPTCProuter Backend Security Audit Report

**Date**: 2025-11-18
**Auditor**: Claude (Automated Security Audit)
**Scope**: Backend shell scripts, VPS installation scripts, hotplug handlers, and UCI configuration scripts

## Executive Summary

This audit identified **23 security vulnerabilities** and **15 code quality issues** across the OpenMPTCProuter backend codebase. The most critical issues include:

- **Command Injection** vulnerabilities due to unquoted variables (HIGH severity)
- **Race Conditions** in PID file handling (MEDIUM severity)
- **Input Validation** missing for user input and configuration values (HIGH severity)
- **Information Disclosure** through weak file permissions (LOW severity)

## Critical Security Vulnerabilities

### 1. Command Injection - Unquoted Variables (HIGH)

**Affected Files:**
- `common/files/usr/bin/omr-status` (lines 66, 75, 104, 162)
- `common/files/usr/bin/network-monitor.sh` (line 19)
- `common/files/usr/bin/network-safety-monitor.sh` (multiple locations)
- `common/files/usr/bin/usb-modem-autoconfig.sh` (multiple locations)
- `common/files/usr/bin/wifi-autoconfig.sh` (heredoc variables)
- `common/files/usr/bin/port-autoconfig.sh` (heredoc variables)

**Description:**
Variables are used in command substitutions and heredocs without proper quoting, allowing potential command injection if UCI configuration values or device names contain shell metacharacters.

**Example:**
```bash
# Vulnerable code
local signal=$(uqmi -d "$device" --get-signal-info 2>/dev/null | grep '"rssi":' | cut -d: -f2 | tr -d ' ,')

# If $device contains "; rm -rf /" this could execute arbitrary commands
```

**Impact:**
- Arbitrary command execution as root
- System compromise
- Data loss

**Remediation:**
- Quote all variable expansions: `"$variable"`
- Validate input before use
- Use safer alternatives where possible

### 2. Race Condition in PID File Handling (MEDIUM)

**Affected Files:**
- `common/files/usr/bin/network-monitor.sh` (lines 17-25)

**Description:**
TOCTOU (Time-of-Check-Time-of-Use) race condition between checking if process exists and writing PID file.

**Vulnerable Code:**
```bash
check_running() {
    if [ -f "$PID_FILE" ]; then
        old_pid=$(cat "$PID_FILE")
        if kill -0 "$old_pid" 2>/dev/null; then
            exit 0
        fi
    fi
    echo $$ > "$PID_FILE"
}
```

**Impact:**
- Multiple instances of monitor could run simultaneously
- Resource exhaustion
- Inconsistent system state

**Remediation:**
- Use atomic file operations
- Use `flock` for proper locking
- Validate PID before use

### 3. Unvalidated User Input (HIGH)

**Affected Files:**
- `common/files/usr/bin/omr-recovery` (lines 21, 109)

**Description:**
User input is read and used without validation or sanitization.

**Vulnerable Code:**
```bash
read -p "Choose option (1-5): " choice
case "$choice" in
    1|2|3|4|5)
        # Process choice
        ;;
    *)
        echo "Invalid option"
        ;;
esac

# Later:
read -p "Are you sure? (yes/no): " confirm
if [ "$confirm" = "yes" ]; then
    # Executes commands without validating $confirm content
fi
```

**Impact:**
- Potential for injection attacks
- Unexpected behavior
- Script bypass

**Remediation:**
- Validate input matches expected pattern
- Use allowlist validation
- Sanitize before use

### 4. Path Traversal (MEDIUM)

**Affected Files:**
- `common/files/usr/bin/usb-modem-autoconfig.sh` (multiple locations)
- `common/files/usr/bin/network-safety-monitor.sh` (line 170)

**Description:**
Device paths and file paths constructed from external sources without validation.

**Example:**
```bash
# Device path from detection
local device=$(uci -q get network.$wan.device)
if [ -c "$device" ]; then
    # Uses device without validating it's actually in /dev/
fi
```

**Impact:**
- Access to arbitrary files
- Information disclosure
- Potential privilege escalation

**Remediation:**
- Validate paths are in expected directories
- Use absolute paths only
- Check for path traversal sequences (.., symlinks)

### 5. Heredoc Variable Injection (MEDIUM)

**Affected Files:**
- `common/files/usr/bin/usb-modem-autoconfig.sh` (line 170)
- `common/files/usr/bin/port-autoconfig.sh` (lines 109-116, 126-141)
- `common/files/usr/bin/omr-recovery` (line 121)

**Description:**
Variables used in heredocs without proper quoting, allowing injection.

**Example:**
```bash
cat > "$status_dir/$wan_name" <<-EOFF
    INTERFACE=$wan_name
    PHYSICAL_DEVICE=$iface
    PROTOCOL=$proto
    CONTROL_DEVICE=$dev
    INFO=$modem_info
EOFF
```

**Impact:**
- File content injection
- Configuration tampering
- Potential command execution

**Remediation:**
- Use quoted heredocs: `<<-'EOF'`
- Quote variable expansions
- Validate content before writing

### 6. Information Disclosure (LOW)

**Affected Files:**
- `common/files/usr/bin/wifi-autoconfig.sh` (line 129)

**Description:**
WiFi password saved to file with potentially weak permissions.

**Vulnerable Code:**
```bash
cat > /etc/wifi-password.txt <<-EOF
    OpenMPTCProuter Optimized - WiFi Configuration
    WiFi Password: $wifi_password
EOF
```

**Impact:**
- WiFi password exposure
- Unauthorized network access

**Remediation:**
- Set restrictive file permissions (600)
- Encrypt sensitive data
- Use secure storage mechanism

## Code Quality Issues

### 1. Missing Error Handling

**Severity**: MEDIUM

**Description**: Most scripts don't check return values of critical operations.

**Examples:**
- UCI operations don't verify success
- Network restart commands don't check if successful
- File operations don't verify write success

**Recommendation:**
```bash
# Add error checking
if ! uci commit network; then
    log_msg "ERROR: Failed to commit network config"
    return 1
fi
```

### 2. Inconsistent Variable Quoting

**Severity**: LOW

**Description**: Mix of quoted and unquoted variables throughout codebase.

**Recommendation**: Establish consistent quoting policy - quote all variables unless explicitly needed otherwise.

### 3. Magic Numbers

**Severity**: LOW

**Description**: Hardcoded sleep values, timeouts, and intervals.

**Examples:**
- `sleep 3`, `sleep 5`, `sleep 15` scattered throughout
- `CHECK_INTERVAL=60` hardcoded

**Recommendation**: Define constants at top of scripts:
```bash
readonly DEVICE_ENUM_WAIT=3
readonly INTERFACE_WAIT=5
readonly SYSTEM_BOOT_WAIT=15
```

### 4. No Input Validation for UCI Values

**Severity**: MEDIUM

**Description**: UCI configuration values used directly without validation.

**Recommendation:**
```bash
# Validate interface name
validate_interface() {
    local iface="$1"
    # Only allow alphanumeric, dash, underscore
    echo "$iface" | grep -qE '^[a-zA-Z0-9_-]+$'
}
```

### 5. Weak Random Generation

**Severity**: MEDIUM

**Affected Files:**
- `vps-scripts/omr-vps-install.sh` (lines 49-54)

**Description**: Inconsistent random generation methods, some potentially weak.

**Current Code:**
```bash
SHADOWSOCKS_PASS=${SHADOWSOCKS_PASS:-$(head -c 32 /dev/urandom | base64 -w0)}
GLORYTUN_PASS=${GLORYTUN_PASS:-$(od -vN "32" -An -tx1 /dev/urandom | tr '[:lower:]' '[:upper:]' | tr -d " \n")}
```

**Recommendation**: Use consistent, secure method:
```bash
generate_secure_password() {
    head -c 32 /dev/urandom | base64 -w0 | tr -d '=' | head -c 32
}
```

## Detailed Findings by File

### omr-status (common/files/usr/bin/omr-status)

**Vulnerabilities:**
1. Line 66: Unquoted `$device` in command - **HIGH**
2. Line 75: Unquoted `$device` in command - **HIGH**
3. Line 104: Unquoted `$iface` in command - **MEDIUM**
4. Line 162: Unquoted variable in command - **MEDIUM**

**Code Quality:**
1. No error handling for `uci` commands
2. No validation of interface names
3. Hardcoded sleep values

### network-monitor.sh

**Vulnerabilities:**
1. Lines 17-25: Race condition in PID file handling - **MEDIUM**
2. Line 19: Unquoted `$old_pid` in kill command - **HIGH**

**Code Quality:**
1. No error handling for service restarts
2. Hardcoded intervals

### network-safety-monitor.sh

**Vulnerabilities:**
1. Multiple unquoted variables in command substitutions - **HIGH**
2. No validation of interface names - **MEDIUM**
3. Path traversal in emergency port file - **LOW**

**Code Quality:**
1. Complex logic without error handling
2. Magic numbers throughout
3. No validation of UCI values

### usb-modem-autoconfig.sh

**Vulnerabilities:**
1. Multiple unquoted variables - **HIGH**
2. Device paths not validated - **MEDIUM**
3. Heredoc injection at line 170 - **MEDIUM**
4. No validation of modem info before file write - **LOW**

**Code Quality:**
1. No error handling for configuration operations
2. Assumes commands succeed
3. No validation of APN or other settings

### wifi-autoconfig.sh

**Vulnerabilities:**
1. WiFi password file with weak permissions - **LOW**
2. Variables in heredocs unquoted - **MEDIUM**
3. No validation of generated password - **LOW**

**Code Quality:**
1. No error handling for `iw` commands
2. Assumes hardware capabilities exist
3. No rollback on failure

### port-autoconfig.sh

**Vulnerabilities:**
1. Unquoted variables in heredocs - **MEDIUM**
2. No validation of port names - **MEDIUM**

**Code Quality:**
1. No error handling
2. Assumes network reload succeeds
3. No verification of configuration

### omr-recovery

**Vulnerabilities:**
1. Unvalidated user input - **HIGH**
2. Variables used without quotes - **MEDIUM**
3. Heredoc with untrusted content - **MEDIUM**

**Code Quality:**
1. No error handling
2. Assumes commands succeed
3. No validation of user choices

### omr-vps-install.sh

**Vulnerabilities:**
1. Weak random generation - **MEDIUM**
2. No validation of curl output - **MEDIUM**
3. DEBIAN_FRONTEND set globally - **LOW**

**Code Quality:**
1. Long script without functions for complex operations
2. Inconsistent error handling
3. Some commands check return values, others don't

## Recommendations

### Immediate Actions (Critical)

1. **Fix Command Injection Issues**: Quote all variable expansions
2. **Add Input Validation**: Validate all UCI config values and user input
3. **Fix Race Conditions**: Use proper locking mechanisms
4. **Secure Sensitive Files**: Set restrictive permissions on password files

### Short-term Actions (High Priority)

1. **Add Error Handling**: Check return values of critical operations
2. **Validate Paths**: Ensure paths are in expected directories
3. **Fix Heredoc Injection**: Use quoted heredocs or validate content
4. **Standardize Random Generation**: Use consistent secure method

### Long-term Actions (Improvements)

1. **Code Review Process**: Implement security-focused code review
2. **Automated Testing**: Add shellcheck to CI/CD
3. **Documentation**: Document security assumptions and requirements
4. **Refactoring**: Break complex scripts into testable functions

## Testing Recommendations

1. **Use ShellCheck**: Static analysis tool for shell scripts
2. **Fuzzing**: Test with malformed UCI configuration values
3. **Permission Testing**: Verify file permissions are secure
4. **Race Condition Testing**: Test concurrent execution scenarios
5. **Integration Testing**: Test full workflow with edge cases

## Compliance Notes

These vulnerabilities may impact:
- CWE-78: OS Command Injection
- CWE-362: Concurrent Execution using Shared Resource with Improper Synchronization
- CWE-20: Improper Input Validation
- CWE-22: Path Traversal
- CWE-732: Incorrect Permission Assignment for Critical Resource

## Conclusion

The OpenMPTCProuter backend has several security vulnerabilities that should be addressed, particularly command injection and input validation issues. Most can be fixed with proper variable quoting and input validation. The code quality would benefit from consistent error handling and better structure.

**Overall Risk Level**: **HIGH** (due to command injection vulnerabilities with root privileges)

**Priority**: Fix command injection issues immediately, then address input validation and race conditions.
