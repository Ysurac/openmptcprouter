![openmptcprouter](https://github.com/Ysurac/openmptcprouter-feeds/workflows/openmptcprouter/badge.svg)
![Latest tag](https://img.shields.io/github/tag/ysurac/openmptcprouter-feeds.svg)
[![Paypal](https://www.openmptcprouter.com/img/donate-PayPal-green.svg)](https://www.paypal.me/ycarus)
[![Flattr](https://www.openmptcprouter.com/img/donate-flattr-yellow.svg)](https://flattr.com/@ycarus)
[![Liberapay](https://img.shields.io/liberapay/patrons/Moul.svg?logo=liberapay)](https://liberapay.com/Ycarus/)
[![LinkedIn](https://www.openmptcprouter.com/img/linkedin.png)](https://www.linkedin.com/in/yannick-chabanois-550330146/)
[![Twitter](https://www.openmptcprouter.com/img/twitter.jpg)](https://twitter.com/OpenMPTCProuter)
[![Atom](https://www.openmptcprouter.com/img/feed.png)](https://www.openmptcprouter.com/atom)

# OpenMPTCProuter

OpenMPTCProuter is an open source solution to aggregate and encrypt multiple internet connections and terminates it over any VPS which make clients benefit security, reliability, net neutrality, as well as dedicated public IP.

The aggregation is based on Multipath TCP (MPTCP), which is ISP, WAN type, and latency independent "whether it was Fiber, VDSL, SHDSL, ADSL, 4G or even 5G", different scenarios can be configured to have either aggregation or failover based on MPTCP.

Aggregation via [Multi-link VPN (MLVPN)](https://github.com/markfoodyburton/MLVPN/commits/new-reorder) and [Glorytun UDP](https://github.com/angt/glorytun) with multipath support are also supported.

The solution takes advantage of the OpenWRT/LEDE system, which is user friendly and also adds the possibility of installing other packages like VPN, QoS, routing protocols, monitoring, etc. through web-interface or terminal.


Main website: [https://www.openmptcprouter.com/](https://www.openmptcprouter.com/)

Packages made for OpenMPTCProuter are available here: [https://github.com/Ysurac/openmptcprouter-feeds](https://github.com/Ysurac/openmptcprouter-feeds)

OpenMPTCProuter VPS script part: [https://github.com/Ysurac/openmptcprouter-vps](https://github.com/Ysurac/openmptcprouter-vps)


## Install from pre-compiled images

You can download precompiled images from [https://www.openmptcprouter.com/](https://www.openmptcprouter.com/)

Then copy it to a sdcard:

```sh
gunzip omr-*.img.gz
dd bs=4M if=omr-*.img of=/dev/sdX conv=fsync
```

## Install from source

[Create image](https://github.com/Ysurac/openmptcprouter/wiki/Create-image-for-unsupported-platform)


## Features and Optimizations

### Hardware Support

OpenMPTCProuter supports a wide range of hardware platforms with optimized configurations:

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