# TODO/FIXME/HACK Audit - Quick Reference

## Critical Issues Requiring Immediate Attention (3)

### 1. Frontend XSS Vulnerability
- **File**: `/home/user/openmptcprouter/common/package/luci-theme-omr-optimized/htdocs/luci-static/omr-optimized/js/theme.js:85`
- **Issue**: Unsafe `innerHTML` usage with user input
- **Fix**: Replace with `textContent`
- **Time**: 15 minutes

### 2. Frontend Code Injection Risk  
- **File**: `js/theme.js:304`
- **Issue**: `new Function()` with user input
- **Fix**: Refactor to proper event handlers
- **Time**: 30 minutes

### 3. WCAG Accessibility Violation
- **File**: `/home/user/openmptcprouter/common/package/luci-theme-omr-optimized/luasrc/view/themes/omr-optimized/header.htm:20`
- **Issue**: User zoom disabled in viewport meta tag
- **Fix**: Remove `maximum-scale=1.0` and `user-scalable=no`
- **Time**: 5 minutes

---

## Active Testing Checklists (40 items)

### QOL Features Testing (12 items)
- **File**: `QOL_FEATURES.md` (lines 434-445)
- **Status**: All unchecked
- **Effort**: End-user feature validation

### Test Plan Coverage (28 items)
- **File**: `TEST_PLAN.md`
- **Categories**: 
  - 6 Build workflow tests
  - 3 Modem detection tests
  - 2 Initialization tests
  - 3 CA optimization tests
  - 3 Performance tests
  - 2 USB mode switching tests
  - 2 Integration tests
  - 3 Platform-specific tests
  - 2 Documentation tests
  - 2 Regression tests
- **Note**: Most require RM551E hardware

---

## High Priority Issues (3 Issues)

| Issue | File | Line | Severity | Time |
|-------|------|------|----------|------|
| Performance: uname per page | footer.htm | 17 | HIGH | 20min |
| Display Bug: Format string | footer.htm | 19 | HIGH | 15min |
| Memory Leaks: 15 event listeners | theme.js | 78,105,113... | HIGH | 45min |

---

## DHCP Configuration TODOs (20 items across 5 files)

**Affected Files**:
- `5.4/package/network/services/dnsmasq/files/dnsmasq.init`
- `6.1/package/network/services/dnsmasq/files/dnsmasq.init`
- `6.6/package/network/services/dnsmasq/files/dnsmasq.init`
- `6.10/package/network/services/dnsmasq/files/dnsmasq.init`
- `6.12/package/network/services/dnsmasq/files/dnsmasq.init`

**Recurring Issues** (each file):
- Line 271: DHCPV6 circuitid handling
- Line 304: DHCPv6 vendor class definitions
- Line 492: BOOTURL DHCPv4/v6 differences
- Line 617: Lease time to route lifetime conversion

**Recommendation**: Create unified DHCP improvement task instead of 20 duplicate TODOs.

---

## Network Driver TODOs

- **6.10**: mediatek-2p5ge.c:280 - Firmware rate adaptation (FIXME)
- **5.4**: b53_common.c - Multiple auto-detect TODOs (lines 1423, 1436, 1449, 1462, 1475)
- **5.4**: igc driver (4 FIXME comments on timestamp/ETF/performance)

---

## Legacy Code (Low Priority)

**U-Boot Bootloader** (~80 TODO/FIXME comments)
- Location: `common/package/boot/uboot-ipq40xx/src/`
- Priority: LOW (legacy, not actively developed)
- Recommendation: Review for potential upgrade path

---

## Documentation Status

**Marked Complete**:
- ✅ IMPLEMENTATION_SUMMARY.md - "No TODO comments"
- ✅ 6 major requirements completed
- ✅ All drivers and patches updated
- ✅ VPS scripts and setup guides done

**Issue Reports Generated**:
- FRONTEND_ISSUES_SUMMARY.txt (12 issues documented)
- SECURITY_AUDIT_REPORT.md
- AUDIT_REPORT.md
- And 10 more analysis documents

---

## Recommended Action Plan

### Week 1 (Critical Path)
1. Fix 3 frontend security vulnerabilities (2 hours)
2. Fix WCAG accessibility violation (15 minutes)
3. Fix memory leaks in theme.js (45 minutes)
4. Total: ~3 hours

### Week 2-3 (Release Prep)
1. Run QOL Features test checklist (12 items)
2. Execute build workflow tests
3. Fix performance issues (uname caching)
4. Consolidate DHCP TODOs

### Month 2 (Stabilization)
1. Hardware testing with RM551E modem
2. Accessibility improvements
3. Add CSP headers
4. CSS dark mode fixes

### Q2 2025 (Architecture)
1. Review U-Boot upgrade path
2. Consolidate duplicate TODOs
3. Implement test automation

---

## File References

### Critical Frontend Files
```
/home/user/openmptcprouter/common/package/luci-theme-omr-optimized/
├── htdocs/luci-static/omr-optimized/
│   ├── cascade.css (CSS issues)
│   ├── js/theme.js (XSS, injection, memory leaks)
│   └── css/utilities.css (excessive !important)
└── luasrc/view/themes/omr-optimized/
    ├── header.htm (accessibility violation, CSP missing)
    └── footer.htm (performance issue, display bug)
```

### Test Plans
```
/home/user/openmptcprouter/
├── TEST_PLAN.md (28 test items)
├── QOL_FEATURES.md (12 test checkboxes)
└── TODO_AUDIT_REPORT.txt (this audit)
```

### Issue Reports
```
/home/user/openmptcprouter/
├── FRONTEND_ISSUES_SUMMARY.txt (12 documented issues)
├── AUDIT_REPORT.md
├── SECURITY_AUDIT_REPORT.md
└── And 10+ other analysis documents
```

---

## Statistics

- **Total TODO/FIXME matches**: 1,081 files
- **Critical issues**: 3
- **High priority issues**: 3
- **Medium priority issues**: 3
- **Test items awaiting execution**: 28
- **QOL feature tests**: 12
- **Files analyzed**: 200+
- **Report lines**: 455

---

Generated: 2025-11-18
Last Updated: See TODO_AUDIT_REPORT.txt for full details
