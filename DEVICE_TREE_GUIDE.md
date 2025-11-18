# Device Tree Organization Guide

## Overview

This document describes the organization of device trees across different kernel versions in OpenMPTCProuter, how they are structured, and best practices for maintaining them.

## Directory Structure

Device trees are organized by kernel version and platform:

```
openmptcprouter/
├── 5.4/
│   └── target/linux/
│       ├── bcm27xx/          # Raspberry Pi platforms
│       ├── generic/          # Generic kernel configuration
│       ├── ipq40xx/          # Qualcomm IPQ40xx platform
│       ├── ipq60xx/          # Qualcomm IPQ60xx platform
│       ├── ipq807x/          # Qualcomm IPQ807x platform
│       ├── rockchip/         # Rockchip platforms (R2S, R4S, etc.)
│       └── x86/              # x86/x86_64 platforms
├── 6.1/
│   └── target/linux/
│       ├── bcm27xx/          # Raspberry Pi 2/3/4/5
│       ├── generic/          # Generic kernel configuration
│       ├── ipq40xx/          # Qualcomm IPQ40xx
│       ├── mediatek/         # MediaTek MT7981/MT7986 (BPI-R4, ZBT, etc.)
│       ├── qualcommax/       # Qualcomm Max platforms
│       ├── ramips/           # MediaTek MIPS platforms
│       └── x86/              # x86/x86_64
├── 6.6/
│   └── target/linux/
│       ├── bcm27xx/
│       ├── generic/
│       ├── mediatek/
│       ├── qualcommax/
│       └── x86/
├── 6.10/
│   └── target/linux/
│       └── ... (similar structure)
└── 6.12/
    └── target/linux/
        ├── generic/
        ├── ipq40xx/
        ├── ipq806x/
        └── mediatek/
```

## Platform Support Matrix

### Kernel 5.4
- **bcm27xx**: Raspberry Pi 2, 3, 4
- **ipq40xx**: Qualcomm IPQ4019 based devices (EdgeRouter X, R619AC, etc.)
- **ipq60xx**: Qualcomm IPQ60xx devices
- **ipq807x**: Qualcomm IPQ807x devices
- **rockchip**: Rockchip RK3328/RK3399 (R2S, R4S, R5S, R5C, R6S)
- **x86**: x86 and x86_64 platforms

### Kernel 6.1
- **bcm27xx**: Raspberry Pi 2, 3, 4, 5
- **ipq40xx**: Qualcomm IPQ4019 devices
- **mediatek**: MT7981, MT7986 (BPI-R4, ZBT Z8102AX/Z8109AX, Cudy TR3000)
- **qualcommax**: Qualcomm IPQ807x/IPQ60xx
- **ramips**: MediaTek MIPS platforms
- **x86**: x86/x86_64 platforms

### Kernel 6.6
- **bcm27xx**: Raspberry Pi platforms
- **mediatek**: MediaTek SoC platforms
- **qualcommax**: Qualcomm platforms
- **x86**: x86/x86_64 platforms

### Kernel 6.12 (Latest)
- **generic**: Generic kernel configuration
- **ipq40xx**: Qualcomm IPQ4019 devices
- **ipq806x**: Qualcomm IPQ806x devices
- **mediatek**: MediaTek MT7981/MT7986 platforms

## MediaTek Device Trees (Detailed)

### Supported Devices (All kernels 6.1+)

#### ZBT Z8102AX Family
- **mt7981b-zbt-z8102ax-64m.dts**: 64MB RAM variant
- **mt7981b-zbt-z8102ax-128m.dts**: 128MB RAM variant
- **mt7981b-zbt-z8102ax-eMMC.dts**: eMMC storage variant
- **mt7981b-zbt-z8102ax.dtsi**: Shared base for all Z8102AX variants

**Features:**
- MediaTek MT7981B SoC
- 2.5GbE WAN + GbE LAN
- Dual modem support with SIM switching
- Multiple storage options (NAND, eMMC)
- GPIO-controlled modem power and SIM selection

#### ZBT Z8109AX Family
- **mt7981b-zbt-z8109ax-128m.dts**: 128MB RAM variant
- **mt7981b-zbt-z8109ax-512m.dts**: 512MB RAM variant
- **mt7981b-zbt-z8109ax.dtsi**: Shared base for all Z8109AX variants

**Features:**
- MediaTek MT7981B SoC
- 2.5GbE WAN + 5x GbE LAN ports
- Quad modem support (4 USB modems)
- Quad SIM support with GPIO switching
- Enhanced for multi-modem bonding scenarios

#### Cudy TR3000
- **mt7981b-cudy-tr3000-256mb-v1.dts**: 256MB RAM variant

**Features:**
- MediaTek MT7981B SoC
- 2.5GbE PHY with Realtek chipset
- Built-in WiFi support
- USB 3.0 for modems

### Device Tree Structure

Each platform follows this general structure:

```
platform/
├── Makefile                  # Build configuration
├── config-X.XX               # Kernel configuration
├── target.mk                 # Target definitions
├── image/                    # Image generation scripts
│   ├── Makefile
│   └── *.mk                  # Device-specific image recipes
├── dts/                      # Device tree sources
│   ├── *.dts                 # Complete device definitions
│   └── *.dtsi                # Shared/included device tree fragments
├── files/                    # Root filesystem overlay
├── patches-X.XX/             # Kernel patches
└── modules/                  # Kernel module definitions
```

## Device Tree Best Practices

### 1. Use .dtsi for Shared Components

When multiple devices share common hardware (e.g., Z8102AX and Z8109AX both use MT7981B), create a base `.dtsi` file:

```dts
// mt7981b-zbt-common.dtsi
/ {
    compatible = "mediatek,mt7981";

    // Common hardware definitions
    ethernet {
        // Shared ethernet config
    };

    spi {
        // Shared SPI config
    };
};
```

Then include it in device-specific `.dts` files:

```dts
#include "mt7981b-zbt-common.dtsi"

/ {
    model = "ZBT Z8102AX 128M";
    // Device-specific overrides
};
```

### 2. GPIO Naming Convention

Use descriptive names for GPIO exports:

```dts
gpio-export {
    compatible = "gpio-export";

    modem1_power {
        gpio-export,name = "modem1";
        gpio-export,output = <1>;
        gpios = <&pio 4 GPIO_ACTIVE_HIGH>;
    };

    sim1_select {
        gpio-export,name = "sim1";
        gpio-export,output = <1>;
        gpios = <&pio 6 GPIO_ACTIVE_HIGH>;
    };
};
```

### 3. LED Definitions

Use standard LED function definitions:

```dts
leds {
    compatible = "gpio-leds";

    led_status: status_led {
        function = LED_FUNCTION_STATUS;
        color = <LED_COLOR_ID_GREEN>;
        gpios = <&pio 10 GPIO_ACTIVE_LOW>;
    };

    modem1_led {
        function = LED_FUNCTION_USB;
        function-enumerator = <0>;
        color = <LED_COLOR_ID_BLUE>;
        gpios = <&pio 8 GPIO_ACTIVE_LOW>;
    };
};
```

### 4. Network Interface Naming

Use consistent labels for network interfaces:

```dts
&eth {
    gmac0: mac@0 {
        compatible = "mediatek,eth-mac";
        reg = <0>;
        phy-mode = "2500base-x";
        // WAN interface configuration
    };

    gmac1: mac@1 {
        compatible = "mediatek,eth-mac";
        reg = <1>;
        phy-mode = "gmii";
        // LAN interface configuration
    };
};
```

### 5. Memory and Storage

Define memory regions clearly:

```dts
memory {
    reg = <0 0x40000000 0 0x08000000>;  // 128MB
};

// For NAND partitions
partitions {
    compatible = "fixed-partitions";

    partition@0 {
        label = "u-boot";
        reg = <0x0 0x100000>;
        read-only;
    };

    partition@100000 {
        label = "u-boot-env";
        reg = <0x100000 0x80000>;
    };

    // ... more partitions
};
```

## Adding New Device Support

### Step 1: Identify the Base Platform

Determine which SoC your device uses:
- MediaTek MT7981/MT7986 → `mediatek/` directory
- Qualcomm IPQ40xx → `ipq40xx/` directory
- Broadcom BCM27xx → `bcm27xx/` directory
- Rockchip → Look in older kernels (5.4) or use generic ARM64

### Step 2: Create Device Tree

1. Check if there's a similar device with a `.dtsi` you can reuse
2. Create your `.dts` file in the appropriate `dts/` directory
3. Include necessary headers and base `.dtsi` files
4. Define device-specific hardware

Example:
```dts
// SPDX-License-Identifier: GPL-2.0-or-later OR MIT

/dts-v1/;
#include <dt-bindings/gpio/gpio.h>
#include <dt-bindings/input/input.h>

#include "mt7981b.dtsi"

/ {
    model = "Your Device Name";
    compatible = "vendor,model", "mediatek,mt7981";

    // Device configuration
};
```

### Step 3: Add Image Generation Recipe

Add your device to `image/Makefile`:

```makefile
define Device/vendor_model
  DEVICE_VENDOR := Vendor
  DEVICE_MODEL := Model Name
  DEVICE_DTS := mt7981b-vendor-model
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e
endef
TARGET_DEVICES += vendor_model
```

### Step 4: Update Target Configuration

If needed, update `target.mk` and `config-X.XX` files.

### Step 5: Test Build

```bash
OMR_TARGET=your_device OMR_KERNEL=6.12 ./build.sh
```

## Kernel Version Migration

When migrating device trees between kernel versions:

1. **Check for API changes**: Device tree bindings may change between kernels
2. **Update compatible strings**: Ensure they match the kernel version's expectations
3. **Verify GPIO numbers**: GPIO mappings may change
4. **Test bootloader compatibility**: Ensure U-Boot can load the new device tree
5. **Update partition layouts** if needed for new features

### Example Migration (6.1 → 6.12)

```diff
// Old (6.1)
&wifi {
    mediatek,mtd-eeprom = <&factory 0x0>;
};

// New (6.12)
&wifi {
    nvmem-cells = <&eeprom_factory_0>;
    nvmem-cell-names = "eeprom";
};
```

## Common Issues and Solutions

### Issue: Device tree not found during boot

**Solution**: Check that:
1. DTS file name matches `DEVICE_DTS` in image Makefile
2. File is in the correct `dts/` directory
3. Device tree is included in the image generation

### Issue: GPIO not working

**Solution**:
1. Verify GPIO number is correct for your SoC
2. Check active-high vs active-low polarity
3. Ensure GPIO controller is enabled in device tree

### Issue: Network interfaces missing

**Solution**:
1. Check ethernet node is enabled (`status = "okay"`)
2. Verify PHY configuration matches hardware
3. Ensure switch configuration is correct

### Issue: Storage not detected

**Solution**:
1. Check SPI/MMC controller is enabled
2. Verify partition table is correct
3. Ensure compatible string matches driver expectations

## Platform-Specific Notes

### MediaTek MT7981/MT7986

- Use `mediatek,mt7531` for switch
- 2.5GbE requires `phy-mode = "2500base-x"`
- NAND requires `mediatek,nmbm` for bad block management

### Qualcomm IPQ40xx

- Device trees in `files-X.XX/arch/arm/boot/dts/qcom/`
- Use `qcom,ipq4019` compatible string
- Dual-band WiFi requires separate entries

### Broadcom BCM27xx (Raspberry Pi)

- Overlay-based device tree system
- Many device trees upstream from RPi foundation
- Check `bcm2712/` for RPi 5 specific trees

## References

- [Linux Device Tree Documentation](https://www.kernel.org/doc/Documentation/devicetree/)
- [OpenWrt Device Tree Guide](https://openwrt.org/docs/techref/hardware/soc/soc.mediatek.mt7621)
- [MediaTek MT7981 Reference](https://www.mediatek.com/)

## Changelog

### 2025-11-18
- Added ZBT Z8109AX device trees to kernel 6.1
- Added Cudy TR3000 support to kernel 6.1
- Added eMMC variant for Z8102AX
- Standardized device tree organization across kernel versions

---

**Last Updated**: 2025-11-18
**Maintainer**: OpenMPTCProuter Development Team
