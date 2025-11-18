# Kernel Tree Organization Guide

## Overview

OpenMPTCProuter supports multiple kernel versions to provide compatibility across different hardware platforms. This document describes how the kernel tree is organized, how to maintain it, and best practices for working with multiple kernel versions.

## Supported Kernel Versions

| Kernel Version | Status | Primary Use Case | Platforms |
|---------------|--------|------------------|-----------|
| **5.4** | Legacy | Older hardware support | bcm27xx, ipq40xx, rockchip, x86 |
| **6.1** | LTS | Long-term support, production | bcm27xx, ipq40xx, mediatek, x86, ramips |
| **6.6** | LTS | Long-term support, modern hardware | bcm27xx, mediatek, qualcommax, x86 |
| **6.10** | Stable | Recent features | Similar to 6.6 |
| **6.12** | Latest | Bleeding edge, newest hardware | mediatek, ipq40xx, ipq806x, generic |

## Directory Structure

```
openmptcprouter/
├── 5.4/                      # Kernel 5.4 tree
│   ├── package/              # Package definitions
│   │   ├── kernel/           # Kernel modules
│   │   ├── network/          # Network packages
│   │   └── firmware/         # Firmware packages
│   ├── target/               # Target platform definitions
│   │   └── linux/            # Linux target platforms
│   │       ├── generic/      # Generic kernel config
│   │       │   ├── config-5.4
│   │       │   ├── patches-5.4/
│   │       │   └── files/
│   │       ├── bcm27xx/      # Raspberry Pi
│   │       ├── ipq40xx/      # Qualcomm IPQ40xx
│   │       ├── rockchip/     # Rockchip SoCs
│   │       └── x86/          # x86/x86_64
│   └── toolchain/            # Compiler toolchain
├── 6.1/                      # Kernel 6.1 tree (similar structure)
├── 6.6/                      # Kernel 6.6 tree
├── 6.10/                     # Kernel 6.10 tree
├── 6.12/                     # Kernel 6.12 tree
└── common/                   # Shared across all kernels
    ├── package/              # Common packages
    │   ├── network/
    │   ├── modems/           # Modem support packages
    │   └── luci-theme-omr-optimized/
    ├── files/                # Common root filesystem files
    └── tools/                # Build tools
```

## Generic Kernel Configuration

### Location

Each kernel version has a generic configuration:
- `5.4/target/linux/generic/config-5.4`
- `6.1/target/linux/generic/config-6.1`
- `6.6/target/linux/generic/config-6.6`
- `6.10/target/linux/generic/config-6.10`
- `6.12/target/linux/generic/config-6.12`

### Key Configuration Areas

#### 1. **Network Stack Optimizations**

All kernels include comprehensive networking features:

```conf
# TCP Congestion Control
CONFIG_TCP_CONG_BBR=y
CONFIG_TCP_CONG_CUBIC=y
CONFIG_TCP_CONG_HTCP=y
CONFIG_TCP_CONG_WESTWOOD=y

# Packet Schedulers (QoS)
CONFIG_NET_SCH_CAKE=y
CONFIG_NET_SCH_FQ=y
CONFIG_NET_SCH_HTB=y
CONFIG_NET_SCH_HFSC=y
CONFIG_NET_SCH_FQ_CODEL=y

# Network Classifiers
CONFIG_NET_CLS_ACT=y
CONFIG_NET_CLS_U32=y
CONFIG_NET_CLS_FLOWER=y
```

**Why**: Essential for MPTCP aggregation and QoS on bonded connections

#### 2. **MPTCP Support**

```conf
# Kernel 6.1+
CONFIG_MPTCP=y
CONFIG_INET_MPTCP_DIAG=y
CONFIG_MPTCP_IPV6=y

# Kernel 5.4 (via patches)
# Uses MPTCP v0.96 patches
```

**Why**: Core functionality for multipath TCP aggregation

#### 3. **USB and Modem Support**

```conf
# USB network drivers
CONFIG_USB_NET_CDC_MBIM=y
CONFIG_USB_NET_CDC_NCM=y
CONFIG_USB_NET_QMI_WWAN=y
CONFIG_USB_NET_RNDIS_HOST=y
CONFIG_USB_SERIAL_OPTION=y
CONFIG_USB_SERIAL_QUALCOMM=y
CONFIG_USB_SERIAL_WWAN=y

# USB 3.0 support
CONFIG_USB_XHCI_HCD=y
CONFIG_USB_XHCI_PLATFORM=y
```

**Why**: Support for 5G/4G modems (Quectel, Sierra Wireless, etc.)

#### 4. **File Systems**

```conf
CONFIG_EXT4_FS=y
CONFIG_F2FS_FS=y
CONFIG_SQUASHFS=y
CONFIG_OVERLAY_FS=y
CONFIG_UBIFS_FS=y
CONFIG_JFFS2_FS=y
```

**Why**: Various storage types (eMMC, NAND, NOR flash)

#### 5. **Networking Features**

```conf
# Network filtering
CONFIG_NETFILTER=y
CONFIG_NF_CONNTRACK=y
CONFIG_NF_NAT=y
CONFIG_IP_NF_IPTABLES=y
CONFIG_IP6_NF_IPTABLES=y

# Network bonding
CONFIG_BONDING=y
CONFIG_NET_TEAM=y

# VLANs
CONFIG_VLAN_8021Q=y
```

**Why**: Firewall, NAT, and advanced networking features

## Platform-Specific Trees

### MediaTek (6.1, 6.6, 6.12)

**Location**: `target/linux/mediatek/`

**Structure**:
```
mediatek/
├── Makefile
├── config-6.XX
├── filogic/              # MT7981/MT7986 subtarget
│   ├── config-default
│   └── target.mk
├── dts/                  # Device trees
│   ├── mt7981b-*.dts
│   └── mt7986a-*.dts
├── image/
│   ├── Makefile
│   └── filogic.mk       # Image generation for Filogic
├── patches-6.XX/
└── files/
```

**Supported SoCs**:
- MT7981B (Filogic 820)
- MT7986A/B (Filogic 830)

**Key Features**:
- 2.5GbE support
- WiFi 6/6E (MT7915/MT7916)
- Hardware NAT offload
- PCIe 3.0

### Qualcomm IPQ40xx (5.4, 6.1, 6.12)

**Location**: `target/linux/ipq40xx/`

**Structure**:
```
ipq40xx/
├── Makefile
├── config-X.XX
├── generic/
│   └── target.mk
├── image/
│   ├── Makefile
│   └── generic.mk
├── files-X.XX/
│   └── arch/arm/boot/dts/qcom/
│       ├── qcom-ipq4019-*.dts
│       └── qcom-ipq4018-*.dts
└── patches-X.XX/
```

**Supported SoCs**:
- IPQ4018 (dual-core)
- IPQ4019 (quad-core)

**Key Features**:
- Integrated WiFi (2.4GHz + 5GHz)
- Hardware crypto
- GbE support

### Broadcom BCM27xx (5.4, 6.1, 6.6)

**Location**: `target/linux/bcm27xx/`

**Structure**:
```
bcm27xx/
├── Makefile
├── bcm2708/             # RPi 1
├── bcm2709/             # RPi 2
├── bcm2710/             # RPi 3
├── bcm2711/             # RPi 4
├── bcm2712/             # RPi 5
│   ├── config-6.XX
│   └── target.mk
├── image/
├── patches-6.XX/
└── modules/
```

**Supported Platforms**:
- Raspberry Pi 2, 3, 4, 5
- Compute Modules

**Key Features**:
- VideoCore GPU
- CSI camera support
- Multiple USB 3.0 ports (RPi 4/5)
- PCIe (RPi 5)

### x86/x86_64 (All kernels)

**Location**: `target/linux/x86/`

**Structure**:
```
x86/
├── Makefile
├── config-X.XX
├── 64/                  # x86_64 subtarget
│   ├── config-default
│   └── target.mk
├── generic/             # Generic x86
├── image/
│   ├── Makefile
│   └── grub-*.cfg
└── patches-X.XX/
```

**Supported CPUs**:
- x86_64 (Intel/AMD 64-bit)
- x86 (32-bit, legacy)

**Key Features**:
- UEFI and BIOS boot
- Multiple disk controllers (AHCI, NVMe)
- All network cards supported

## Kernel Patches

### Organization

Patches are organized by kernel version and purpose:

```
patches-X.XX/
├── 0XX-categoryprefix-description.patch
├── 1XX-categoryprefix-description.patch
└── 9XX-local-custom.patch
```

**Numbering**:
- `0XX-1XX`: Backports from newer kernels
- `2XX-3XX`: OpenWrt generic patches
- `4XX-5XX`: Platform-specific patches
- `6XX-7XX`: Driver patches
- `8XX-9XX`: Local custom patches

### Key Patch Categories

#### 1. **MPTCP Patches** (Kernel 5.4)

```
patches-5.4/
├── 990-mptcp-fullmesh-raise-addresses-limit.patch
├── 991-mptcp-add-scheduler-options.patch
└── 992-mptcp-improve-retransmission.patch
```

**Purpose**: Backport MPTCP v0.96 to kernel 5.4

#### 2. **Network Performance Patches**

```
patches-X.XX/
├── 650-netfilter-conntrack-optimization.patch
├── 651-tcp-bbr2-implementation.patch
└── 652-cake-scheduler-enhancements.patch
```

**Purpose**: Network stack optimizations

#### 3. **Hardware Support Patches**

```
patches-X.XX/
├── 700-mt76-wifi6-support.patch
├── 701-qmi-wwan-quectel-support.patch
└── 702-usb-serial-5g-modems.patch
```

**Purpose**: Add/improve hardware driver support

### Creating Patches

1. **Make changes in kernel source**:
```bash
cd openmptcprouter/6.12/build_dir/target-*/linux-*/linux-*
# Make your changes
```

2. **Generate patch**:
```bash
make target/linux/refresh V=s
```

3. **Patch appears in**:
```
target/linux/PLATFORM/patches-X.XX/
```

## Kernel Module Packages

### Location

Kernel modules are defined in:
- `package/kernel/` - Kernel-version specific
- `common/package/` - Shared across kernel versions

### Structure

```
package/kernel/MODULE_NAME/
├── Makefile              # Package definition
└── src/                  # Module source (if not in kernel)
    ├── Makefile
    └── *.c
```

### Example: r8125 Driver

```makefile
# 6.1/package/kernel/r8125/Makefile
include $(TOPDIR)/rules.mk

PKG_NAME:=r8125
PKG_VERSION:=9.013.02
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/heri16/r8125.git
PKG_SOURCE_VERSION:=main

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/r8125
  SUBMENU:=Network Devices
  TITLE:=Realtek RTL8125 2.5GbE driver
  FILES:=$(PKG_BUILD_DIR)/src/r8125.ko
  AUTOLOAD:=$(call AutoProbe,r8125)
  CONFLICTS:=kmod-r8169
endef

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)/src" \
		modules
endef

$(eval $(call KernelPackage,r8125))
```

### Adding New Kernel Modules

1. **Create package directory**:
```bash
mkdir -p 6.12/package/kernel/new-module
```

2. **Create Makefile** (see example above)

3. **Add source** (if external):
```bash
mkdir src/
# Add source files
```

4. **Test build**:
```bash
make package/kernel/new-module/compile V=s
```

## Build System Integration

### Target Selection

**File**: `config-DEVICE_NAME`

Example:
```conf
CONFIG_TARGET_mediatek=y
CONFIG_TARGET_mediatek_filogic=y
CONFIG_TARGET_mediatek_filogic_DEVICE_bananapi_bpi-r4=y

# Kernel modules
CONFIG_PACKAGE_kmod-usb3=y
CONFIG_PACKAGE_kmod-mt7915e=y
CONFIG_PACKAGE_kmod-r8125=y

# Packages
CONFIG_PACKAGE_uqmi=y
CONFIG_PACKAGE_picocom=y
```

### Build Process

```bash
# Set kernel and target
export OMR_KERNEL=6.12
export OMR_TARGET=bpi-r4

# Build script handles:
# 1. Selecting correct kernel tree
# 2. Copying common packages
# 3. Applying patches
# 4. Configuring kernel
# 5. Building image
./build.sh
```

## Kernel Configuration Management

### Updating Kernel Config

1. **Enter menuconfig**:
```bash
cd 6.12
make kernel_menuconfig
```

2. **Make changes** in the TUI

3. **Save config**:
```bash
make kernel_oldconfig
cd target/linux/generic
scripts/kconfig.pl '+' config-6.12.old config-6.12 > config-6.12.new
mv config-6.12.new config-6.12
```

4. **Verify changes**:
```bash
git diff target/linux/generic/config-6.12
```

### Platform-Specific Config

Override generic config in platform config:

```
target/linux/mediatek/config-6.12
```

Example overrides:
```conf
# MediaTek-specific
CONFIG_MTK_HSDMA=y
CONFIG_MTK_THERMAL=y
CONFIG_MTK_EFUSE=y
```

## Version Migration Strategy

### Adding New Kernel Version

1. **Create kernel directory**:
```bash
mkdir 6.13
```

2. **Copy base structure** from latest kernel:
```bash
cp -r 6.12/target 6.13/
cp -r 6.12/package 6.13/
```

3. **Update configs**:
```bash
cd 6.13/target/linux/generic
mv config-6.12 config-6.13
# Update for new kernel APIs
```

4. **Review and update patches**:
```bash
cd patches-6.12
# Check which patches are still needed
# Update for new kernel
mv ../patches-6.12 ../patches-6.13
```

5. **Test build**:
```bash
OMR_KERNEL=6.13 OMR_TARGET=x86_64 ./build.sh
```

### Deprecating Old Kernel

1. **Mark as deprecated** in documentation
2. **Update build scripts** to warn
3. **Remove after 2 LTS cycles**

## Testing and Validation

### Pre-Commit Checks

1. **Build test** on all platforms:
```bash
for target in x86_64 bpi-r4 rpi5; do
    OMR_KERNEL=6.12 OMR_TARGET=$target ./build.sh
done
```

2. **Boot test** on hardware
3. **Network test** (MPTCP, bonding)
4. **Modem test** (if applicable)

### Regression Testing

1. **Previous kernel** still works
2. **Existing devices** not broken
3. **Patches** still apply cleanly

## Common Issues

### Issue: Kernel doesn't boot

**Diagnosis**:
1. Check bootloader compatibility
2. Verify device tree is correct
3. Check console output for errors

**Solution**:
1. Update bootloader if needed
2. Fix device tree issues
3. Add missing drivers to config

### Issue: Module fails to load

**Diagnosis**:
```bash
dmesg | grep module
lsmod
```

**Solution**:
1. Check kernel version compatibility
2. Verify module dependencies
3. Enable missing kernel symbols

### Issue: Patches don't apply

**Diagnosis**:
```bash
make target/linux/PLATFORM/refresh V=s
```

**Solution**:
1. Rebase patches on new kernel
2. Remove obsolete patches
3. Update patch content for API changes

## Best Practices

1. **Keep generic config minimal** - Only essential features
2. **Use platform configs for specifics** - Hardware-specific features
3. **Document patches** - Add detailed descriptions
4. **Test across kernel versions** - Ensure compatibility
5. **Keep common/ truly common** - Don't add version-specific code
6. **Follow naming conventions** - Consistent file naming
7. **Version control** - Commit logically, good messages

## References

- [OpenWrt Build System](https://openwrt.org/docs/guide-developer/build-system/start)
- [Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/)
- [Device Tree Specification](https://www.devicetree.org/specifications/)

## Changelog

### 2025-11-18
- Documented kernel 6.12 addition
- Added MediaTek device tree standardization
- Updated wireless driver integration
- Added Realtek NIC driver support

---

**Last Updated**: 2025-11-18
**Maintainer**: OpenMPTCProuter Development Team
