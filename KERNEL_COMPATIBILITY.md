# Kernel Version Compatibility Matrix

This document provides a comprehensive overview of kernel version compatibility across different hardware platforms and features.

## Quick Reference

| Kernel | Status | OpenWrt Base | Recommended For |
|--------|--------|--------------|-----------------|
| 5.4 | Legacy | 21.02 | Maximum compatibility, older hardware |
| 6.1 | Stable | 22.03 | Production use, good balance |
| 6.6 | Current | 24.10 | Modern hardware, latest features |
| 6.10 | Testing | 24.10 | Bleeding edge, experimental |
| 6.12 | Latest | main | Newest hardware, active development |

---

## Platform Support Matrix

### Legend
- ✅ Fully supported
- ⚠️ Partially supported / Testing
- ❌ Not supported / Not available
- 🔧 Requires manual configuration

| Platform | 5.4 | 6.1 | 6.6 | 6.10 | 6.12 | Notes |
|----------|-----|-----|-----|------|------|-------|
| **Banana Pi R1** | ✅ | ❌ | ❌ | ❌ | ❌ | 5.4 only, forced by build.sh |
| **Banana Pi R2** | ✅ | ✅ | ✅ | ✅ | ⚠️ | Full support 5.4-6.10 |
| **Banana Pi R3** | ✅ | ✅ | ✅ | ✅ | ✅ | MediaTek platform |
| **Banana Pi R4** | ❌ | ✅ | ✅ | ✅ | ✅ | WiFi 7 support in 6.6+ |
| **Banana Pi R4-PoE** | ❌ | ✅ | ✅ | ✅ | ✅ | Same as R4 |
| **Banana Pi R64** | ✅ | ✅ | ✅ | ✅ | ⚠️ | MediaTek platform |
| **Raspberry Pi 2** | ✅ | ✅ | ✅ | ✅ | ❌ | bcm27xx missing in 6.12 |
| **Raspberry Pi 3** | ✅ | ✅ | ✅ | ✅ | ❌ | bcm27xx missing in 6.12 |
| **Raspberry Pi 4** | ✅ | ✅ | ✅ | ✅ | ❌ | bcm27xx missing in 6.12 |
| **Raspberry Pi 5** | ❌ | ✅ | ✅ | ✅ | ❌ | bcm27xx missing in 6.12 |
| **NanoPi R2S** | ✅ | ✅ | ✅ | ✅ | ❌ | Rockchip missing in 6.12 |
| **NanoPi R4S** | ✅ | ✅ | ✅ | ✅ | ❌ | Rockchip missing in 6.12 |
| **NanoPi R5S** | ✅ | ✅ | ✅ | ✅ | ❌ | Rockchip missing in 6.12 |
| **NanoPi R5C** | ✅ | ✅ | ✅ | ✅ | ❌ | Rockchip missing in 6.12 |
| **NanoPi R6S** | ✅ | ✅ | ✅ | ✅ | ❌ | Rockchip missing in 6.12 |
| **Netgear R7800** | ✅ | ✅ | ✅ | ✅ | ❌ | Rockchip missing in 6.12 |
| **x86** | ✅ | ✅ | ✅ | ✅ | ❌ | Generic x86 missing in 6.12 |
| **x86_64** | ✅ | ✅ | ✅ | ✅ | ⚠️ | Full support, recommended |
| **GL.iNet MT2500** | ❌ | ✅ | ✅ | ✅ | ✅ | MediaTek platform |
| **GL.iNet MT3000** | ❌ | ✅ | ✅ | ✅ | ✅ | MediaTek platform |
| **GL.iNet MT6000** | ❌ | ✅ | ✅ | ✅ | ✅ | MediaTek platform |
| **Linksys WRT3200ACM** | ✅ | ✅ | ✅ | ✅ | ❌ | mvebu missing in 6.12 |
| **Linksys WRT32X** | ✅ | ✅ | ✅ | ✅ | ❌ | mvebu missing in 6.12 |
| **Espressobin** | ✅ | ✅ | ✅ | ✅ | ⚠️ | mvebu platform |
| **Ubiquiti EdgeRouter X** | ✅ | ✅ | ✅ | ✅ | ⚠️ | MIPS platform |
| **Teltonika RUTX** | ✅ | ✅ | ✅ | ✅ | ⚠️ | Maps to rutx12 |
| **Teltonika RUTX12** | ✅ | ✅ | ✅ | ✅ | ⚠️ | ARM platform |
| **Teltonika RUTX50** | ✅ | ✅ | ✅ | ✅ | ⚠️ | ARM platform |
| **QNAP 301W** | ✅ | ✅ | ✅ | ✅ | ⚠️ | ARM platform |

---

## Feature Compatibility

### MPTCP (Multipath TCP)

| Kernel | MPTCP Version | Schedulers | Subflows | Status |
|--------|---------------|------------|----------|--------|
| 5.4 | 0.96 | 11 (FULLMESH, NDIFFPORTS, BINDER, BLEST, REDUNDANT, ROUNDROBIN, ECF, etc.) | 8 default | ✅ Most complete |
| 6.1 | Upstream | 2 (default, IPv6) | 8 default | ⚠️ Reduced features |
| 6.6 | Upstream | 2-3 (default, IPv6, minrtt) | 8 default | ⚠️ Reduced features |
| 6.10 | Upstream | 2-3 (default, IPv6, minrtt) | 8 default | ⚠️ Reduced features |
| 6.12 | Upstream | 3 (default, IPv6, minrtt) | Increased via patch | ⚠️ Reduced features |

**Note:** Kernel 6.1+ has significantly fewer MPTCP schedulers compared to 5.4. Advanced multi-path features are limited.

### TCP Congestion Control

| Kernel | BBR | BBR2 | BBR3 | Recommended |
|--------|-----|------|------|-------------|
| 5.4 | ✅ | ✅ (via patch) | ❌ | BBR2 |
| 6.1 | ✅ | ✅ | ✅ (via patch) | BBR2 or BBR3 |
| 6.6 | ✅ | ✅ | ✅ (via patch) | BBR2 or BBR3 |
| 6.10 | ✅ | ✅ | ✅ (via patch) | BBR2 or BBR3 |
| 6.12 | ✅ | ❌ (removed) | ✅ (via patch) | BBR3 |

**Important:** Device configs (config-r4s, config-bpi-r4) still request BBR2, but 6.12 only has BBR3 available via patches.

### WiFi Support

| Kernel | WiFi 5 (11ac) | WiFi 6 (11ax) | WiFi 7 (11be) | MT7996 Driver |
|--------|---------------|---------------|---------------|---------------|
| 5.4 | ✅ | ⚠️ Limited | ❌ | ❌ |
| 6.1 | ✅ | ✅ | ❌ | ⚠️ Early |
| 6.6 | ✅ | ✅ | ✅ | ✅ Full support |
| 6.10 | ✅ | ✅ | ✅ | ✅ Full support |
| 6.12 | ✅ | ✅ | ✅ | ✅ Full support |

**Best for WiFi 7:** Use kernel 6.6+ with MT7996-capable hardware (BPI-R4, GL.iNet MT6000)

### 5G/LTE Modem Support

All kernels support standard QMI/MBIM modems. Specific improvements:

| Kernel | Basic Modems | Quectel RM551E | Multi-band CA | Notes |
|--------|--------------|----------------|---------------|-------|
| 5.4 | ✅ | ✅ | ⚠️ Limited | Standard support |
| 6.1 | ✅ | ✅ | ✅ | Improved CA |
| 6.6 | ✅ | ✅ | ✅ | Full support |
| 6.10 | ✅ | ✅ | ✅ | Full support |
| 6.12 | ✅ | ✅ | ✅ | Latest drivers |

---

## Build System Compatibility

### Package Manager

| Kernel | OpenWrt Version | Package Manager | Feed System |
|--------|-----------------|-----------------|-------------|
| 5.4 | 21.02 | opkg | Traditional feeds |
| 6.1 | 22.03 | opkg | Traditional feeds |
| 6.6 | 24.10 | opkg | Traditional feeds |
| 6.10 | 24.10 | opkg | Traditional feeds |
| 6.12 | main | **APK** (Alpine) | New package system |

**Breaking Change:** Kernel 6.12 uses APK instead of OPKG, requiring feed format changes.

### Build Dependencies

| Kernel | GCC Version | LLVM/Clang | Python | Notes |
|--------|-------------|------------|--------|-------|
| 5.4 | GCC 8+ | Optional | 3.x | Stable |
| 6.1 | GCC 10+ | Recommended | 3.x | Stable |
| 6.6 | GCC 11+ | Required | 3.x | LLVM mandatory |
| 6.10 | GCC 12+ | Required | 3.x | LLVM mandatory |
| 6.12 | GCC 13+ | Required | 3.x | Latest toolchain |

---

## Migration Paths

### From 5.4 to 6.6 (Recommended)

**Pros:**
- WiFi 7 support
- Latest security fixes
- Modern driver support
- Good hardware compatibility

**Cons:**
- Reduced MPTCP schedulers
- Requires LLVM/Clang

**Steps:**
1. Backup router configuration
2. Build 6.6 image: `OMR_TARGET=your_target OMR_KERNEL=6.6 ./build.sh`
3. Flash image
4. Restore configuration
5. Test all WAN connections
6. Verify MPTCP aggregation

### From 6.6/6.10 to 6.12 (Caution)

**Pros:**
- Latest kernel features
- BBR3 support
- Newest hardware support

**Cons:**
- **Platform support reduced** (no Rockchip, bcm27xx, x86)
- BBR2 removed (BBR3 only)
- APK package system (breaking change)
- **Not recommended for production**

**Currently Unsupported Platforms:**
- All Raspberry Pi devices
- NanoPi R2S/R4S/R5S/R6S
- Netgear R7800
- x86 (generic)
- Linksys WRT series
- Most non-MediaTek devices

**Recommendation:** Stay on 6.6 or 6.10 until 6.12 platform support is restored.

---

## Platform-Specific Notes

### Banana Pi R1
- **Locked to 5.4** by build.sh (line 124-128)
- Kernel 6.1+ will error and exit
- No migration path currently available

### Banana Pi R4
- **Best on 6.6+** for WiFi 7 support
- Requires MT7996 driver (not in 5.4)
- BBR2 recommended for 6.6/6.10
- BBR3 for 6.12 (if using)

### Raspberry Pi Series
- **Avoid 6.12** (bcm27xx platform missing)
- Use 6.6 or 6.10 for latest features
- 5.4 still fully functional for older Pi 2/3

### NanoPi R-series (Rockchip)
- **Very popular** (R4S, R5S, R6S)
- **Cannot use 6.12** (Rockchip platform removed)
- Stay on 6.6 or 6.10
- Excellent performance on all supported kernels

### x86/x86_64
- x86_64 works on all kernels
- Generic x86 missing in 6.12
- Best for VMs and mini-PCs
- Use 6.6 for production

### MediaTek Devices (GL.iNet, BPI-R3/R4)
- **Full support on all kernels** including 6.12
- WiFi 7 requires 6.6+
- Safe to use latest kernels

---

## Known Issues by Kernel

### 5.4
- ❌ No WiFi 7 support
- ❌ Older security patches
- ✅ Most stable MPTCP implementation
- ✅ Maximum hardware compatibility

### 6.1
- ⚠️ MPTCP scheduler reduction (97.5%)
- ✅ Good balance of features and stability
- ✅ Broad hardware support

### 6.6
- ⚠️ MPTCP scheduler reduction continues
- ✅ WiFi 7 support
- ✅ BBR2 and BBR3 available
- ✅ Recommended for most users

### 6.10
- Similar to 6.6
- ✅ Slightly newer features
- ⚠️ Less testing than 6.6

### 6.12
- ❌ Major platform support loss (43% reduction)
- ❌ BBR2 removed
- ❌ APK package system (breaking)
- ⚠️ **Not recommended for production use**
- 🔧 Suitable for MediaTek-only deployments

---

## Recommendation Summary

**For Production:**
- **Best Overall:** Kernel 6.6
- **Maximum Compatibility:** Kernel 5.4
- **WiFi 7 Devices:** Kernel 6.6 or 6.10

**For Testing/Development:**
- **Latest Features:** Kernel 6.10
- **Bleeding Edge (MediaTek only):** Kernel 6.12

**Platform-Specific:**
- **Banana Pi R1:** Must use 5.4
- **Raspberry Pi:** Use 6.6 or 6.10 (avoid 6.12)
- **NanoPi R-series:** Use 6.6 or 6.10 (avoid 6.12)
- **GL.iNet/MediaTek:** Any kernel works, 6.6+ for WiFi 7
- **x86_64:** 6.6 recommended

---

## Future Roadmap

**Planned Improvements:**
1. Restore Rockchip platform support in 6.12
2. Restore bcm27xx platform support in 6.12
3. Resolve BBR2/BBR3 version conflicts
4. Document MPTCP scheduler alternatives
5. Create platform-specific build profiles

**Timeline:**
- Q1 2025: Platform support restoration
- Q2 2025: MPTCP scheduler documentation
- Q3 2025: Unified build system for all kernels

---

## Getting Help

If you have questions about kernel compatibility:
1. Check [FAQ.md](FAQ.md)
2. Review [GitHub Issues](https://github.com/spotty118/openmptcprouter/issues)
3. Ask in [Discussions](https://github.com/spotty118/openmptcprouter/discussions)

**Last Updated:** 2025-11-18
**Document Version:** 1.0
