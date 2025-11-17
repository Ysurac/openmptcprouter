# Test Plan for On-Demand Builds and RM551E Support

## Overview

This document outlines the testing strategy for the new features:
1. On-demand build capability via GitHub Actions
2. Enhanced Quectel RM551E-GL 5G modem support

## 1. On-Demand Build Testing

### 1.1 Workflow Dispatch Functionality

**Test Case 1.1.1: Trigger Single Platform Build**
- **Steps:**
  1. Navigate to Actions → "Build OpenMPTCProuter Optimized Images"
  2. Click "Run workflow"
  3. Select target: `bpi-r4`, kernel: `6.12`
  4. Click "Run workflow"
- **Expected Result:**
  - Workflow starts
  - Only builds bpi-r4 with kernel 6.12
  - Build completes in ~45 minutes
  - Artifact `bpi-r4-6.12` is available
- **Status:** ⏳ Pending

**Test Case 1.1.2: Build All Platforms**
- **Steps:**
  1. Run workflow with target: `all`, kernel: `all`
- **Expected Result:**
  - 31 platforms × 2 kernels = 62 jobs run
  - All artifacts available after ~90 minutes
- **Status:** ⏳ Pending

**Test Case 1.1.3: Build Specific Kernel Version**
- **Steps:**
  1. Run workflow with target: `all`, kernel: `6.6`
- **Expected Result:**
  - Builds all platforms with kernel 6.6 only
  - 31 artifacts generated
- **Status:** ⏳ Pending

### 1.2 VPS Build Workflow

**Test Case 1.2.1: Build VPS Package**
- **Steps:**
  1. Navigate to Actions → "Build VPS Images"
  2. Click "Run workflow"
  3. Select VPS type: `ubuntu`
- **Expected Result:**
  - Workflow completes in <5 minutes
  - `vps-install-package` artifact available
  - `vps-documentation` artifact available
- **Status:** ⏳ Pending

### 1.3 Backward Compatibility

**Test Case 1.3.1: Automatic Push Builds**
- **Steps:**
  1. Push a commit to the repository
- **Expected Result:**
  - Workflow triggers automatically
  - Builds all platforms and kernels as before
  - No regression in build process
- **Status:** ⏳ Pending

## 2. Quectel RM551E Modem Testing

### 2.1 Modem Detection

**Test Case 2.1.1: USB Detection - QMI Mode**
- **Prerequisites:** RM551E modem in QMI mode (PID 0x0801)
- **Steps:**
  1. Connect modem to USB port
  2. Run: `lsusb | grep 2c7c`
- **Expected Result:**
  - Shows `2c7c:0801 Quectel`
  - `/dev/cdc-wdm0` created
  - `/dev/ttyUSB*` devices created
- **Status:** ⏳ Pending (requires hardware)

**Test Case 2.1.2: USB Detection - MBIM Mode**
- **Prerequisites:** RM551E modem in MBIM mode (PID 0x0800)
- **Steps:**
  1. Connect modem to USB port
  2. Run: `lsusb | grep 2c7c`
- **Expected Result:**
  - Shows `2c7c:0800 Quectel`
  - MBIM interface detected
- **Status:** ⏳ Pending (requires hardware)

**Test Case 2.1.3: USB Detection - Other Modes**
- **Prerequisites:** Test with PIDs 0x0900, 0x0901
- **Expected Result:**
  - All modes detected correctly
  - Appropriate drivers loaded
- **Status:** ⏳ Pending (requires hardware)

### 2.2 Automatic Initialization

**Test Case 2.2.1: Hotplug Initialization**
- **Steps:**
  1. Connect RM551E modem
  2. Wait 5 seconds
  3. Check: `logread | grep rm551e`
- **Expected Result:**
  - Log shows "Starting RM551E initialization"
  - Log shows "RM551E initialization complete"
  - Modem is configured
- **Status:** ⏳ Pending (requires hardware)

**Test Case 2.2.2: Manual Initialization**
- **Steps:**
  1. Run: `/usr/bin/rm551e-init.sh`
  2. Check logs
- **Expected Result:**
  - Script completes without errors
  - Modem detected and configured
- **Status:** ⏳ Pending (requires hardware)

### 2.3 Carrier Aggregation Optimization

**Test Case 2.3.1: Apply CA Optimizations**
- **Steps:**
  1. Run: `/usr/bin/modem-ca-optimize.sh`
  2. Check modem: `echo "AT+QCAINFO" > /dev/ttyUSB2 && cat /dev/ttyUSB2`
- **Expected Result:**
  - CA settings applied
  - Modem reports active CA if network supports it
- **Status:** ⏳ Pending (requires hardware)

**Test Case 2.3.2: Verify 5G NR CA**
- **Steps:**
  1. Check: `echo "AT+QNWCFG=\"nr5g_carrier_aggregation\"" > /dev/ttyUSB2 && cat /dev/ttyUSB2`
- **Expected Result:**
  - Shows `"nr5g_carrier_aggregation",1` (enabled)
- **Status:** ⏳ Pending (requires hardware)

**Test Case 2.3.3: Verify EN-DC**
- **Steps:**
  1. Check: `echo "AT+QNWCFG=\"endc\"" > /dev/ttyUSB2 && cat /dev/ttyUSB2`
- **Expected Result:**
  - Shows `"endc",1` (enabled)
- **Status:** ⏳ Pending (requires hardware)

### 2.4 Performance Testing

**Test Case 2.4.1: Speed Test - QMI Mode**
- **Prerequisites:** RM551E in QMI mode, good signal
- **Steps:**
  1. Configure network interface
  2. Run speed test
- **Expected Result:**
  - Achieves expected 5G speeds for your area
  - Low latency (<50ms)
- **Status:** ⏳ Pending (requires hardware + network)

**Test Case 2.4.2: Speed Test - MBIM Mode**
- **Prerequisites:** RM551E in MBIM mode
- **Expected Result:**
  - Similar performance to QMI
- **Status:** ⏳ Pending (requires hardware + network)

**Test Case 2.4.3: Multi-Band CA Performance**
- **Prerequisites:** Network with CA support
- **Steps:**
  1. Check active bands: `echo "AT+QCAINFO" > /dev/ttyUSB2 && cat /dev/ttyUSB2`
  2. Run speed test
- **Expected Result:**
  - Multiple bands active
  - Aggregated throughput higher than single band
- **Status:** ⏳ Pending (requires hardware + CA network)

### 2.5 USB Mode Switching

**Test Case 2.5.1: Switch to QMI Mode**
- **Steps:**
  1. Run: `echo "AT+QCFG=\"usbnet\",0" > /dev/ttyUSB2`
  2. Reboot: `echo "AT+CFUN=1,1" > /dev/ttyUSB2`
  3. Wait 30 seconds
  4. Check: `lsusb | grep 2c7c`
- **Expected Result:**
  - PID changes to 0x0801
  - QMI interface available
- **Status:** ⏳ Pending (requires hardware)

**Test Case 2.5.2: Switch to MBIM Mode**
- **Steps:**
  1. Run: `echo "AT+QCFG=\"usbnet\",1" > /dev/ttyUSB2`
  2. Reboot modem
- **Expected Result:**
  - PID changes to 0x0800
  - MBIM interface available
- **Status:** ⏳ Pending (requires hardware)

## 3. Integration Testing

### 3.1 Build System Integration

**Test Case 3.1.1: Modem Package Build**
- **Steps:**
  1. Trigger build for bpi-r4
  2. Check if modems package is included
- **Expected Result:**
  - Package builds successfully
  - Scripts installed in `/usr/bin/`
  - Hotplug rules created
- **Status:** ⏳ Pending

**Test Case 3.1.2: Configuration Validation**
- **Steps:**
  1. Extract built image
  2. Check for modem configs in `/lib/network/wwan/`
- **Expected Result:**
  - Files `2c7c:0800`, `2c7c:0801`, `2c7c:0900`, `2c7c:0901` present
- **Status:** ⏳ Pending

### 3.2 Platform-Specific Testing

**Test Case 3.2.1: Banana Pi R4**
- **Prerequisites:** BPI-R4 hardware
- **Steps:**
  1. Flash image built with kernel 6.12
  2. Connect RM551E to USB 3.0 port
  3. Check modem detection
- **Expected Result:**
  - Modem detected and initialized
  - Full 5G performance
- **Status:** ⏳ Pending (requires hardware)

**Test Case 3.2.2: Raspberry Pi 4**
- **Prerequisites:** RPi4 hardware
- **Expected Result:**
  - Similar functionality to BPI-R4
- **Status:** ⏳ Pending (requires hardware)

**Test Case 3.2.3: x86_64**
- **Prerequisites:** x86_64 system
- **Expected Result:**
  - Full compatibility
- **Status:** ⏳ Pending (requires hardware)

## 4. Documentation Testing

**Test Case 4.1: User Following ON_DEMAND_BUILDS.md**
- **Steps:**
  1. New user follows guide to build specific platform
- **Expected Result:**
  - User successfully triggers build
  - Downloads and flashes image
- **Status:** ⏳ Pending

**Test Case 4.2: User Following RM551E_QUICK_REF.md**
- **Steps:**
  1. User follows AT commands from quick reference
- **Expected Result:**
  - Commands work as documented
  - User can troubleshoot modem
- **Status:** ⏳ Pending (requires hardware)

## 5. Regression Testing

**Test Case 5.1: Existing Modem Support**
- **Steps:**
  1. Test with other Quectel modems (EC21, EC25, etc.)
- **Expected Result:**
  - No regression in existing modem support
- **Status:** ⏳ Pending (requires hardware)

**Test Case 5.2: Build System Stability**
- **Steps:**
  1. Run multiple builds consecutively
- **Expected Result:**
  - All builds succeed
  - No artifacts corruption
- **Status:** ⏳ Pending

## 6. Validation Checklist

### Code Quality
- [x] YAML syntax validation passed
- [x] JSON configuration validation passed
- [x] Shell script syntax check passed
- [x] No shellcheck warnings in scripts
- [x] Documentation is clear and comprehensive

### Functionality
- [x] Workflow dispatch inputs configured correctly
- [x] Matrix configuration uses proper conditionals
- [x] Modem configs follow existing format
- [x] Scripts have proper error handling
- [x] Hotplug rules are correctly formatted

### Testing Status Summary

| Category | Total Tests | Passed | Pending | Failed |
|----------|-------------|--------|---------|--------|
| Workflow | 6 | 0 | 6 | 0 |
| Modem Detection | 3 | 0 | 3 | 0 |
| Initialization | 2 | 0 | 2 | 0 |
| CA Optimization | 3 | 0 | 3 | 0 |
| Performance | 3 | 0 | 3 | 0 |
| USB Switching | 2 | 0 | 2 | 0 |
| Integration | 2 | 0 | 2 | 0 |
| Platform Tests | 3 | 0 | 3 | 0 |
| Documentation | 2 | 0 | 2 | 0 |
| Regression | 2 | 0 | 2 | 0 |
| **TOTAL** | **28** | **0** | **28** | **0** |

## Notes

- Most tests require actual hardware (RM551E modem)
- Workflow tests can be done immediately in GitHub Actions
- Performance tests require good network coverage
- CA tests require carrier network support

## Next Steps

1. ✅ Code implementation complete
2. ⏳ Trigger test workflow in GitHub Actions
3. ⏳ Hardware testing with RM551E modem
4. ⏳ Performance benchmarking
5. ⏳ User acceptance testing
6. ⏳ Documentation review
