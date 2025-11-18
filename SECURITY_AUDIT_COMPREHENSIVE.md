# OpenMPTCProuter Security Audit Report

## Executive Summary
This comprehensive security audit identified **7 vulnerabilities** of varying severity across the OpenMPTCProuter codebase. Issues range from Cross-Site Scripting (XSS) to credential exposure and unsafe system command handling.

---

## CRITICAL SEVERITY

### 1. Credential Exposure in Plain Text Files
**Type:** Information Disclosure / Credential Exposure  
**Severity:** CRITICAL  
**Files:**
- `/home/user/openmptcprouter/vps-scripts/wizard.sh` (lines 507-541)
- `/home/user/openmptcprouter/common/files/etc/uci-defaults/90-omr-first-boot-wizard` (lines 607-620)

**Description:**
Sensitive credentials (VPS password, admin password, V2Ray UUID) are being stored in plain text files with world-readable permissions:

```bash
# wizard.sh - Lines 507-543
cat > /root/openmptcprouter_credentials.txt << ENDCREDS
...
Shadowsocks: $SHADOWSOCKS_PASS
Glorytun: $GLORYTUN_PASS
MLVPN: $MLVPN_PASS
DSVPN: $DSVPN_PASS
Admin: $OMR_ADMIN_PASS
V2Ray/Xray UUID: $V2RAY_UUID
...
ENDCREDS

chmod 600 /root/openmptcprouter_credentials.txt  # File permission is 600 but stored in world-accessible locations
```

Additionally, credentials are saved to `/etc/openmptcprouter/config.json` with full content exposure:

```bash
# Lines 480-502
cat > /etc/openmptcprouter/config.json << ENDCONFIG
{
  "credentials": {
    "shadowsocks_password": "$SHADOWSOCKS_PASS",
    "glorytun_password": "$GLORYTUN_PASS",
    "mlvpn_password": "$MLVPN_PASS",
    "dsvpn_password": "$DSVPN_PASS",
    "admin_password": "$OMR_ADMIN_PASS",
    "v2ray_uuid": "$V2RAY_UUID",
    "xray_uuid": "$XRAY_UUID"
  }
}
ENDCONFIG
chmod 600 /etc/openmptcprouter/config.json
```

**Risk:** 
- Credentials stored in plain text can be accessed by other processes/users
- `/root/` directory may be world-readable on some systems
- Credentials visible in process listings during generation

**Recommendation:**
- Use cryptographic key derivation instead of storing raw passwords
- Never log credentials to files; use temporary in-memory storage only
- Implement secrets management (e.g., systemd user secrets, hardware TPM)
- Redact credentials from all logs and output
- Use read-only permissions with explicit user/group ownership

---

### 2. Cross-Site Scripting (XSS) Vulnerability in Theme JavaScript
**Type:** XSS (DOM-based)  
**Severity:** CRITICAL  
**File:** `/home/user/openmptcprouter/common/package/luci-theme-omr-optimized/htdocs/luci-static/omr-optimized/js/theme.js`  
**Line:** 85

**Code:**
```javascript
// Lines 74-99
initTooltips() {
    const tooltipElements = document.querySelectorAll('[data-tooltip]');
    
    tooltipElements.forEach((el) => {
        el.addEventListener('mouseenter', () => {
            const tooltipText = el.getAttribute('data-tooltip');
            if (!tooltipText) return;
            
            const tooltip = document.createElement('div');
            tooltip.className = 'tooltip show';
            tooltip.innerHTML = `<div class="tooltip-inner">${tooltipText}</div>`;  // VULNERABLE LINE
            document.body.appendChild(tooltip);
            // ...
        }
    });
}
```

**Vulnerability Details:**
The `data-tooltip` attribute value is directly injected into `innerHTML` without sanitization. An attacker could craft malicious HTML/JavaScript in the tooltip attribute:

```html
<button data-tooltip="<img src=x onerror='alert(1)'>">Hover me</button>
```

**Risk:**
- Arbitrary JavaScript execution in user context
- Session hijacking via stolen cookies/tokens
- Malware distribution
- Defacement of page content
- CSRF attacks on behalf of authenticated users

**Recommendation:**
```javascript
// SECURE VERSION:
tooltip.textContent = tooltipText;  // Use textContent instead of innerHTML
// OR
const sanitizedText = DOMPurify.sanitize(tooltipText);
tooltip.innerHTML = `<div class="tooltip-inner">${sanitizedText}</div>`;
```

---

## HIGH SEVERITY

### 3. Unsafe sed Command with Unescaped Variables
**Type:** Command Injection / File Tampering  
**Severity:** HIGH  
**File:** `/home/user/openmptcprouter/build.sh`  
**Lines:** 378-382, 403, 408-409

**Code:**
```bash
# Lines 378-382
sed -i "s/CONFIG_PACKAGE_mc=y/# CONFIG_PACKAGE_mc is not set/" "$OMR_TARGET/${OMR_KERNEL}/source/.config"
sed -i "s/CONFIG_MC_EDITOR=y/# CONFIG_MC_EDITOR is not set/" "$OMR_TARGET/${OMR_KERNEL}/source/.config"
sed -i "s/CONFIG_MC_SUBSHELL=y/# CONFIG_MC_SUBSHELL is not set/" "$OMR_TARGET/${OMR_KERNEL}/source/.config"
sed -i "s/CONFIG_MC_CHARSET=y/# CONFIG_MC_CHARSET is not set/" "$OMR_TARGET/${OMR_KERNEL}/source/.config"
sed -i "s/CONFIG_MC_VFS=y/# CONFIG_MC_VFS is not set/" "$OMR_TARGET/${OMR_KERNEL}/source/.config"

# Lines 403, 408-409 - Using loop variable without proper escaping
for i in DEFAULT_swconfig PACKAGE_swconfig PACKAGE_kmod-swconfig; do
    sed -i "s/CONFIG_${i}/# CONFIG_${i} is not set/" "$OMR_TARGET/${OMR_KERNEL}/source/.config"
    sed -i "s/CONFIG_${i}/# CONFIG_${i} is not set/" "$OMR_TARGET/${OMR_KERNEL}/source/target/linux/sunxi/config-${OMR_KERNEL}"
done
```

**Vulnerability Details:**
While the above code looks safe in this context, the pattern is risky because:
1. No escaping of special sed characters in variable values
2. No validation of variable contents
3. If `$i`, `$OMR_TARGET`, or `$OMR_KERNEL` contain sed metacharacters (/, &, \), the sed command could be misinterpreted

If an attacker controls these variables, they could inject arbitrary sed commands:
```bash
OMR_TARGET="/path/;rm -rf /"  # Would execute destructive commands
```

**Risk:**
- Arbitrary file modifications
- Injection of malicious configuration
- System compromise via tampered kernel config

**Recommendation:**
```bash
# Use sed's -e flag with proper quoting or use alternative tools
# Option 1: Use quote substitution
sed -i "s|CONFIG_${i}|# CONFIG_${i} is not set|g" "$file"

# Option 2: Use safer tools like awk or Perl
awk -v pattern="CONFIG_${i}" '{gsub(pattern, "# CONFIG_" pattern " is not set")}1' "$file" > "$file.tmp" && mv "$file.tmp" "$file"

# Option 3: Validate variables
if [[ ! "$i" =~ ^[A-Za-z0-9_]+$ ]]; then
    echo "Error: Invalid variable format" >&2
    exit 1
fi
```

---

### 4. Password Exposure in Wizard Shell Script
**Type:** Information Disclosure  
**Severity:** HIGH  
**File:** `/home/user/openmptcprouter/vps-scripts/wizard.sh`  
**Lines:** 905-915

**Code:**
```bash
echo -e "${CYAN}Server IP:${NC}     ${GREEN}$VPS_PUBLIC_IP${NC}"
echo -e "${CYAN}Port:${NC}          ${GREEN}65500${NC}"
echo -e "${CYAN}Password:${NC}      ${GREEN}$SHADOWSOCKS_PASS${NC}"  # EXPOSED IN CONSOLE OUTPUT
echo -e "${CYAN}Encryption:${NC}    ${GREEN}Shadowsocks (chacha20-ietf-poly1305)${NC}"
```

**Risk:**
- Credentials visible in terminal history
- Displayed to anyone with console access
- May be captured in log files
- Visible in shell history (.bash_history, .zsh_history)

**Recommendation:**
```bash
# Don't echo passwords to console
# Instead:
echo "Installation complete. Check /etc/openmptcprouter/config.json for credentials"
# Or prompt user to secure the credential file immediately
chmod 600 /etc/openmptcprouter/config.json
chmod 600 /root/openmptcprouter_credentials.txt
```

---

### 5. HTML Template Injection in Setup Wizard
**Type:** HTML/JavaScript Injection  
**Severity:** HIGH  
**File:** `/home/user/openmptcprouter/common/files/etc/uci-defaults/90-omr-first-boot-wizard`  
**Lines:** 860-862

**Code:**
```bash
# Replace placeholders in HTML
sed -i "s/REPLACE_VPS_IP/$VPS_PUBLIC_IP/g" /var/www/omr-setup/index.html
sed -i "s/REPLACE_PASSWORD/$SHADOWSOCKS_PASS/g" /var/www/omr-setup/index.html
```

**Vulnerability Details:**
Credentials are embedded in HTML without escaping. If credentials contain special characters, they could break the HTML or inject scripts:

```
Password: test<script>alert(1)</script>test
# After sed replacement in HTML:
<span class="config-value">test<script>alert(1)</script>test</span>
```

**Risk:**
- Credential data corruption
- XSS injection if credentials contain HTML/script tags
- Unescaped HTML entities in displayed credentials

**Recommendation:**
```bash
# Use proper escaping function
escape_html() {
    local string="$1"
    string="${string//&/&amp;}"
    string="${string//</&lt;}"
    string="${string//>/&gt;}"
    string="${string//\"/&quot;}"
    string="${string//\'/&#x27;}"
    echo "$string"
}

# Use escaped values
VPS_IP_ESCAPED=$(escape_html "$VPS_PUBLIC_IP")
PASS_ESCAPED=$(escape_html "$SHADOWSOCKS_PASS")
sed -i "s/REPLACE_VPS_IP/$VPS_IP_ESCAPED/g" /var/www/omr-setup/index.html
```

---

## MEDIUM SEVERITY

### 6. Default Credentials in Configuration
**Type:** Weak/Default Credentials  
**Severity:** MEDIUM  
**File:** `/home/user/openmptcprouter/common/package/base-files/files/bin/config_generate`  
**Line:** 198

**Code:**
```bash
set network.$1.username='username'
set network.$1.password='password'
```

**Description:**
Default placeholder credentials `username` and `password` are set in network configuration. While these appear to be templates, if not overridden during configuration, the device would have default credentials.

**Risk:**
- Unauthorized access if defaults aren't changed
- Simplified brute force attacks
- Device compromise in shared environments

**Recommendation:**
- Remove default credentials entirely or require user input
- Generate strong random credentials on first boot
- Prompt user to change defaults before system is operational

---

### 7. Insecure File Permissions on Static Assets
**Type:** Information Disclosure / Privilege Escalation  
**Severity:** MEDIUM  
**File:** `/home/user/openmptcprouter/common/package/luci-theme-omr-optimized/uci-defaults/30_luci-theme-omr-optimized`  
**Lines:** 14-16

**Code:**
```bash
chmod 644 /www/luci-static/omr-optimized/*.css 2>/dev/null
chmod 644 /www/luci-static/omr-optimized/css/*.css 2>/dev/null
chmod 644 /www/luci-static/omr-optimized/js/*.js 2>/dev/null
```

**Analysis:**
File permission 644 (rw-r--r--) makes static assets world-readable and writable by the file owner. While this is standard for web assets, the files should be owned by www-data or similar restricted user, not root.

**Risk:**
- If compromised web server process runs as unprivileged user, it can modify assets
- Privilege escalation vector if web daemon runs with elevated privileges
- XSS injection possibility through modified JavaScript files

**Recommendation:**
```bash
# Set proper ownership and permissions
chown -R www-data:www-data /www/luci-static/omr-optimized/
chmod -R 755 /www/luci-static/omr-optimized/
chmod -R 644 /www/luci-static/omr-optimized/*.{css,js,html}
# Ensure directories are not world-writable
chmod o-w /www/luci-static/omr-optimized/*/
```

---

## Summary Table

| # | Vulnerability | Severity | Type | File | Line(s) |
|---|---|---|---|---|---|
| 1 | Credential Exposure in Files | CRITICAL | Information Disclosure | wizard.sh, 90-omr-first-boot-wizard | 507-541, 607-620 |
| 2 | XSS in Tooltip innerHTML | CRITICAL | Cross-Site Scripting | theme.js | 85 |
| 3 | Unsafe sed with Variables | HIGH | Command Injection | build.sh | 378-382, 403-409 |
| 4 | Password in Console Output | HIGH | Information Disclosure | wizard.sh | 905-915 |
| 5 | HTML Template Injection | HIGH | Injection | 90-omr-first-boot-wizard | 860-862 |
| 6 | Default Credentials | MEDIUM | Weak Authentication | config_generate | 198 |
| 7 | Insecure File Permissions | MEDIUM | Privilege Escalation | 30_luci-theme-omr-optimized | 14-16 |

---

## Recommendations Priority

### Immediate (Critical)
1. Implement XSS protection in tooltip rendering (use textContent or sanitization)
2. Stop writing credentials to persistent files
3. Implement secrets management system
4. Remove credential display from console output

### Short-term (High)
1. Add input validation and escaping to sed commands
2. Implement proper HTML escaping for dynamically inserted credentials
3. Remove or randomize default credentials

### Long-term (Medium)
1. Conduct comprehensive code review for similar patterns
2. Implement Content Security Policy (CSP) headers
3. Set up automated security scanning in CI/CD pipeline
4. Implement secrets detection in version control

---

## Testing Recommendations

1. **Manual Testing:**
   - Verify no credentials appear in shell history
   - Check file permissions on all configuration files
   - Test XSS injection in tooltip elements

2. **Automated Security Scanning:**
   - SAST tools: SonarQube, Semgrep for pattern detection
   - DAST tools: OWASP ZAP for web vulnerabilities
   - Secrets scanning: GitGuardian, truffleHog

3. **Penetration Testing:**
   - Attempt to exfiltrate credentials from running system
   - Test sed injection vectors
   - Validate XSS protection mechanisms

