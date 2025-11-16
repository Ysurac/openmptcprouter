![OpenMPTCProuter Optimized](https://img.shields.io/badge/OpenMPTCProuter-Optimized-blue.svg)
![Build Status](https://github.com/spotty118/openmptcprouter/workflows/Build%20OpenMPTCProuter%20Optimized%20Images/badge.svg)
![Latest tag](https://img.shields.io/github/tag/spotty118/openmptcprouter.svg)
[![Paypal](https://www.openmptcprouter.com/img/donate-PayPal-green.svg)](https://www.paypal.me/ycarus)
[![Flattr](https://www.openmptcprouter.com/img/donate-flattr-yellow.svg)](https://flattr.com/@ycarus)
[![Liberapay](https://img.shields.io/liberapay/patrons/Moul.svg?logo=liberapay)](https://liberapay.com/Ycarus/)
[![LinkedIn](https://www.openmptcprouter.com/img/linkedin.png)](https://www.linkedin.com/in/yannick-chabanois-550330146/)
[![Twitter](https://www.openmptcprouter.com/img/twitter.jpg)](https://twitter.com/OpenMPTCProuter)
[![Atom](https://www.openmptcprouter.com/img/feed.png)](https://www.openmptcprouter.com/atom)

# OpenMPTCProuter Optimized

> **Note:** This is an optimized fork of [OpenMPTCProuter](https://github.com/Ysurac/openmptcprouter) with enhanced features, updated patches, and driver optimizations.

OpenMPTCProuter Optimized is an optimized fork of the open source solution to aggregate and encrypt multiple internet connections and terminates it over any VPS which make clients benefit security, reliability, net neutrality, as well as dedicated public IP.

The aggregation is based on Multipath TCP (MPTCP), which is ISP, WAN type, and latency independent "whether it was Fiber, VDSL, SHDSL, ADSL, 4G or even 5G", different scenarios can be configured to have either aggregation or failover based on MPTCP.

Aggregation via [Multi-link VPN (MLVPN)](https://github.com/markfoodyburton/MLVPN/commits/new-reorder) and [Glorytun UDP](https://github.com/angt/glorytun) with multipath support are also supported.

The solution takes advantage of the OpenWRT/LEDE system, which is user friendly and also adds the possibility of installing other packages like VPN, QoS, routing protocols, monitoring, etc. through web-interface or terminal.


Main website: [https://www.openmptcprouter.com/](https://www.openmptcprouter.com/)

Packages made for OpenMPTCProuter Optimized are based on the upstream: [https://github.com/Ysurac/openmptcprouter-feeds](https://github.com/Ysurac/openmptcprouter-feeds)

OpenMPTCProuter Optimized VPS configuration is based on: [https://github.com/Ysurac/openmptcprouter-vps](https://github.com/Ysurac/openmptcprouter-vps)


## 🚀 Quick Start - Super Easy Setup!

### Step 1: Set Up Your VPS (2 minutes)

On your VPS server, run this single command:

```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/main/scripts/easy-install.sh | sudo bash
```

**That's it for the server!** The script will:
- ✅ Automatically install and configure everything
- ✅ Generate secure passwords
- ✅ Set up firewall and networking
- ✅ Create a beautiful web page with your settings

### Step 2: Configure Your Router (1 minute)

1. Flash your router with an image from [Releases](https://github.com/spotty118/openmptcprouter/releases)
2. Connect to your router at `http://192.168.100.1`
3. Go to **Services → OpenMPTCProuter**
4. Open `http://YOUR_VPS_IP:8080` and copy the settings shown
5. Click **Save & Apply**

**Done!** Your multiple internet connections are now bonded together!

📖 **Detailed Guide:** [Complete Setup Guide](SETUP_GUIDE.md)
🎥 **Need Help?** [Setup Scripts README](scripts/README.md)

## Install from pre-compiled images

Pre-compiled images will be available from [Releases](https://github.com/spotty118/openmptcprouter/releases)

Flash to SD card:

```sh
gunzip omr-*.img.gz
dd bs=4M if=omr-*.img of=/dev/sdX conv=fsync
```

## Build from source

See the [build documentation](https://github.com/Ysurac/openmptcprouter/wiki/Create-image-for-unsupported-platform) or use our automated build workflow


## Features and Optimizations

### Hardware Support

OpenMPTCProuter Optimized supports a wide range of hardware platforms with optimized configurations:

#### Banana Pi R4 - Enhanced Support
The Banana Pi R4 receives special attention with comprehensive optimizations:
- **WiFi 7 (802.11be) Support**: Full MT7996 driver integration with WiFi 7 capabilities
- **Multi-standard WiFi**: Support for 802.11ac (WiFi 5), 802.11ax (WiFi 6), and 802.11be (WiFi 7)
- **Advanced MAC80211 Framework**: Includes debugfs and mesh networking support
- **5G/LTE Modem Support**: Optimized for Quectel RM551E-GL and other popular modems
- **Thermal Management**: Hardware monitoring and thermal control for optimal performance
- **PoE Variant**: Dedicated configuration for BPI-R4-PoE with all optimizations

### WiFi Optimizations

- **WiFi 7 Ready**: MT7996 driver with cutting-edge 802.11be support
- **Full Driver Stack**: 
  - MT76 framework for MediaTek chipsets
  - MT7996e driver for WiFi 7 capable hardware
  - MT7996 firmware integration
  - Wireless regulatory database (wireless-regdb)
- **Advanced Tools**: iw-full, iwinfo, and hostapd-common for comprehensive WiFi management
- **WPA3 Support**: wpad-mbedtls for modern security protocols
- **Mesh Networking**: MAC80211 mesh support for advanced topologies

### Modem and Cellular Optimizations

Comprehensive modem support for reliable 5G/4G connectivity:

#### USB Network Drivers
- CDC Ethernet, MBIM, NCM protocols
- QMI WWAN for Qualcomm modems
- RNDIS protocol support
- Serial communication drivers (option, wwan, qualcomm)

#### Management Tools
- **uqmi**: QMI protocol management
- **umbim**: MBIM protocol management
- **comgt**: AT command scripting
- **picocom**: Serial terminal access
- **ModemManager**: Comprehensive modem management
- **libqmi & libmbim**: Protocol libraries for advanced control

### Kernel Optimizations

- **BBR2 Congestion Control**: CONFIG_KERNEL_TCP_CONG_BBR2 for improved TCP performance
- **ARM64 Module Support**: CONFIG_KERNEL_ARM64_MODULE_PLTS for modular kernel design
- **Thermal Management**: Kernel-level thermal monitoring and control
- **Hardware Monitoring**: hwmon core for system health monitoring

### Network Performance

- **MPTCP (Multipath TCP)**: Aggregate multiple connections for increased bandwidth and reliability
- **Multiple VPN Options**: 
  - MLVPN (Multi-link VPN) with multipath support
  - Glorytun UDP with multipath capabilities
  - Shadowsocks for encrypted tunneling
- **BBR2 TCP Congestion Control**: Latest Google BBR algorithm for optimal throughput
- **Connection Aggregation**: ISP and latency independent aggregation
- **Failover Support**: Automatic failover for uninterrupted connectivity

### Supported Hardware Platforms

- **Banana Pi**: R1, R2, R3, R3-Mini, R4, R4-PoE, R64
- **Raspberry Pi**: RPi2, RPi3, RPi4, RPi5
- **Rockchip**: R2S, R4S, R5C, R5S, R6S
- **x86/x64**: Full x86 and x86_64 support
- **GL.iNet**: MT2500, MT3000, MT6000
- **Other Platforms**: Ubiquiti EdgeRouter X, Linksys WRT3200ACM/WRT32X, and more

## Credits

Our solution is mainly based on:

* [OpenWRT](https://openwrt.org)
* [MultiPath TCP (MPTCP)](https://multipath-tcp.org)
* [Shadowsocks](https://shadowsocks.org)
* [Glorytun](https://github.com/angt/glorytun)

**Special Thanks:**
* Original OpenMPTCProuter by [Ysurac](https://github.com/Ysurac/openmptcprouter)

## What's Different in the Optimized Version?

### 🎯 Ease of Use
- ✅ **One-command installation** - No complex setup required
- ✅ **Web-based configuration** - Visual setup guide at http://VPS_IP:8080
- ✅ **Automated everything** - Firewall, networking, VPN all configured automatically
- ✅ **Copy-paste ready** - All credentials ready to use
- ❌ Original: Multiple manual steps, easy to make mistakes

### 🎨 Modern Interface
- ✅ **Clean, modern LuCI theme** - Professional gradient design
- ✅ **Simplified menus** - Hide advanced options (unhide if needed)
- ✅ **Responsive design** - Works on mobile and desktop
- ✅ **Dark mode support** - Easy on the eyes
- ❌ Original: Dated interface, cluttered menus

### ⚡ Latest Everything
- ✅ **Updated kernel commits** - Latest 6.12, 6.6, 6.1, 5.4 kernels
- ✅ **Current patches** - BBR2, MT76 WiFi7, all optimizations
- ✅ **Modern protocols** - Shadowsocks, WireGuard, MPTCP ready
- ✅ **Continuous updates** - GitHub Actions automated builds
- ❌ Original: Older commits, manual updates

### 📚 Documentation
- ✅ **Complete setup guide** - Step-by-step with screenshots
- ✅ **Troubleshooting section** - Common issues resolved
- ✅ **Video-ready instructions** - Clear, beginner-friendly
- ❌ Original: Scattered docs, technical jargon

## Repository Structure

```
openmptcprouter/
├── scripts/                    # Easy installation scripts
│   ├── easy-install.sh        # One-command VPS setup
│   └── README.md              # Installation guide
├── vps-scripts/               # VPS server scripts
│   ├── omr-vps-install.sh     # Full VPS installer
│   └── install.sh             # Symlink to easy installer
├── common/                    # Common packages for all builds
│   └── package/
│       └── luci-theme-omr-optimized/  # Modern LuCI theme
├── patches/                   # System patches
├── 5.4/, 6.1/, 6.6/, 6.12/   # Kernel-specific configurations
├── .github/workflows/         # Automated builds
│   └── build.yml             # Build all images
├── SETUP_GUIDE.md            # Detailed setup instructions
└── README.md                 # This file
```

## Development & Contributing

### Building Images

Images are automatically built via GitHub Actions when you push code.

Manual build:
```bash
OMR_TARGET=x86_64 OMR_KERNEL=6.12 ./build.sh
```

### Testing Changes

1. Make your changes
2. Test locally or wait for GitHub Actions build
3. Flash image to hardware
4. Verify functionality
5. Submit pull request

## Support & Community

- 📖 [Setup Guide](SETUP_GUIDE.md) - Complete documentation
- 💬 [Discussions](https://github.com/spotty118/openmptcprouter/discussions) - Ask questions
- 🐛 [Issues](https://github.com/spotty118/openmptcprouter/issues) - Report bugs
- 🌟 [Star this repo](https://github.com/spotty118/openmptcprouter) - Show your support

## License

This project is licensed under GPL-3.0 - see the [LICENSE](LICENSE) file for details.