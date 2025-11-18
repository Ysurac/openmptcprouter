# Frequently Asked Questions (FAQ)

## General Questions

### What is OpenMPTCProuter Optimized?

OpenMPTCProuter Optimized is an enhanced fork of OpenMPTCProuter that provides:
- Simplified one-command installation
- Modern web interface with automatic configuration
- Latest kernel support (6.6, 6.12)
- Enhanced WiFi 7 support for compatible hardware
- Comprehensive modem support (5G/4G)
- Optimized performance with BBR2/BBR3 congestion control

### How is this different from the original OpenMPTCProuter?

See [README.md](README.md#whats-different-in-the-optimized-version) for a detailed comparison. Key improvements include:
- ✅ One-command VPS installation
- ✅ Web-based configuration wizard
- ✅ Automated everything (firewall, networking, VPN)
- ✅ Modern LuCI theme
- ✅ Latest kernel versions and patches

---

## Installation & Setup

### Q: Can I use my existing router?

**A:** Yes, if your router is supported. Check the [supported hardware list](README.md#supported-hardware-platforms). Common compatible devices include:
- Banana Pi R4, R3, R2, R1
- Raspberry Pi 2, 3, 4, 5
- x86/x64 systems
- Many OpenWrt-compatible routers

### Q: Do I need a VPS?

**A:** Yes, OpenMPTCProuter requires a VPS (Virtual Private Server) to aggregate your connections. The VPS acts as the endpoint for all your connections and provides:
- Connection aggregation
- Secure tunneling
- Public IP address
- Low latency routing

Recommended VPS providers:
- Vultr (global locations, good peering)
- DigitalOcean (reliable, easy to use)
- Hetzner (EU-based, affordable)
- Linode/Akamai (excellent network)

### Q: What are the VPS requirements?

**A:** Minimum VPS specifications:
- **RAM:** 512MB minimum, 1GB recommended
- **CPU:** 1 core minimum, 2+ cores recommended
- **Storage:** 10GB
- **Network:** Unlimited bandwidth preferred
- **OS:** Debian 11/12 or Ubuntu 20.04/22.04/24.04

### Q: How do I install on the VPS?

**A:** Simple one-command installation:
```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/easy-install.sh | sudo bash
```

See [SETUP_GUIDE.md](SETUP_GUIDE.md) for detailed instructions.

### Q: Can I change the default LAN IP from 192.168.2.1?

**A:** Yes! Access your router's web interface and navigate to:
1. Go to **Network → Interfaces**
2. Click **Edit** on the LAN interface
3. Change the IPv4 address to your preferred IP
4. Click **Save & Apply**

---

## Configuration & Usage

### Q: How do I configure multiple WANs?

**A:** OpenMPTCProuter automatically detects multiple WAN connections. To configure:
1. Go to **Network → Interfaces**
2. Click **Add new interface**
3. Select your WAN connection type (Ethernet, USB modem, WiFi, etc.)
4. Configure the interface settings
5. Enable the interface

The system will automatically aggregate all active WAN connections.

### Q: Which VPN protocol should I use?

**A:** We recommend this priority order:
1. **MPTCP (Multipath TCP)** - Best performance, native multi-path support
2. **Glorytun UDP** - Good for high-latency/jitter connections
3. **Shadowsocks** - Good for restrictive networks
4. **MLVPN** - Reliable fallback option

For most users, MPTCP provides the best performance.

### Q: How do I know if aggregation is working?

**A:** Check the status in multiple ways:
1. Web Interface: **Status → OpenMPTCProuter**
2. Command line: `omr-test`
3. Check bandwidth: Run speed test and compare to sum of individual connections

You should see traffic distributed across all WAN interfaces.

### Q: My speed is slower than expected. Why?

**A:** Common causes:
- VPS bandwidth limits
- VPS location too far (high latency)
- CPU bottleneck on router or VPS
- Network congestion
- Suboptimal VPN protocol selection

**Solutions:**
- Choose VPS closer to your location
- Upgrade VPS resources
- Try different VPN protocols
- Check for CPU usage (should be <80%)

---

## Troubleshooting

### Q: I can't access the router at 192.168.2.1

**A:** Try these steps:
1. Ensure you're connected to the router's network
2. Try 192.168.1.1 or 192.168.100.1 (some configs may differ)
3. Check if DHCP gave you an IP in the correct subnet: `ip addr show`
4. Try connecting via Ethernet instead of WiFi
5. Factory reset if needed (see [EMERGENCY_RECOVERY.md](EMERGENCY_RECOVERY.md))

### Q: VPN connection fails

**A:** Check these items:
1. Verify VPS IP address is correct
2. Check port 65500 is open on VPS firewall
3. Confirm password matches exactly (no extra spaces)
4. Check VPS service status:
   ```bash
   systemctl status shadowsocks-libev-server@config
   ```
5. Review logs:
   ```bash
   logread | grep -i shadowsocks
   ```

### Q: One of my WANs isn't being used

**A:** Verify:
1. Interface is enabled: **Network → Interfaces**
2. Interface has valid IP address
3. Gateway is reachable: `ping -I <interface> 8.8.8.8`
4. MPTCP is configured: **Services → OpenMPTCProuter**
5. Check logs for errors: `logread | grep -i mptcp`

### Q: High latency/ping on aggregated connection

**A:** This is expected behavior. OpenMPTCProuter adds ~10-50ms latency due to:
- VPN encryption overhead
- VPS routing
- Multi-path coordination

For gaming or real-time applications, consider:
- Choosing VPS closer to game servers
- Using split tunneling (bypass VPN for specific traffic)
- Optimizing MPTCP scheduler settings

### Q: How do I update OpenMPTCProuter?

**A:**
**Router:**
1. Download latest image from [Releases](https://github.com/spotty118/openmptcprouter/releases)
2. Go to **System → Backup / Flash Firmware**
3. Upload the new image
4. Choose "Keep settings" if desired
5. Flash and reboot

**VPS:**
Run the installation script again:
```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/easy-install.sh | sudo bash
```

### Q: Build fails with "No space left on device"

**A:** You need at least 30GB free disk space for building. To free up space:
```bash
# Clean old build artifacts
make clean

# Remove download cache (will re-download on next build)
rm -rf dl/

# Remove toolchain (will rebuild on next build)
rm -rf staging_dir/
```

### Q: How do I enable debug logging?

**A:**
1. SSH into router: `ssh root@192.168.2.1`
2. Enable verbose logging:
   ```bash
   uci set system.@system[0].log_level='7'
   uci commit system
   /etc/init.d/log restart
   ```
3. View logs: `logread -f`

---

## Advanced Topics

### Q: Can I use OpenMPTCProuter with Starlink?

**A:** Yes! Starlink works well as one of multiple WAN connections. Configure it as a standard DHCP WAN interface.

### Q: Does this support IPv6?

**A:** Yes, MPTCP supports IPv6. Configure IPv6 on your WAN interfaces and VPS for full IPv6 support.

### Q: Can I run this in a VM?

**A:** Yes, the x86/x86_64 images work in:
- VirtualBox
- VMware
- KVM/QEMU
- Proxmox
- ESXi

Ensure the VM has access to your WAN interfaces (bridged networking recommended).

### Q: How do I add custom firewall rules?

**A:**
1. Go to **Network → Firewall → Custom Rules**
2. Add your iptables rules
3. Click **Restart Firewall**

Example:
```bash
# Allow port 8080
iptables -A INPUT -p tcp --dport 8080 -j ACCEPT
```

### Q: Can I use this with a 5G modem?

**A:** Yes! Supported modems include:
- Quectel RM551E-GL (recommended, see [RM551E_QUICK_REF.md](RM551E_QUICK_REF.md))
- Quectel RM520N
- Sierra Wireless modems
- Huawei modems
- Most QMI/MBIM compatible modems

### Q: What's the maximum number of WANs I can aggregate?

**A:** Theoretically unlimited, but practical limits:
- **Router CPU:** Most routers handle 2-4 WANs well
- **VPS bandwidth:** VPS must handle combined bandwidth
- **MPTCP kernel limits:** Default is 8 subflows (can be increased)

For most use cases, 2-4 WANs is optimal.

### Q: Does this work with CGNAT?

**A:** Yes! OpenMPTCProuter works perfectly with Carrier-Grade NAT. The VPN tunnel bypasses CGNAT limitations.

---

## Performance & Optimization

### Q: How can I improve performance?

**A:** Optimization tips:
1. **Choose closer VPS:** <50ms latency ideal
2. **Upgrade VPS:** More CPU/RAM = better performance
3. **Use wired connections:** Ethernet > WiFi for WANs
4. **Enable BBR2/BBR3:** Congestion control optimization
5. **Tune MPTCP:** Adjust scheduler and subflow count
6. **Disable unused features:** Save CPU cycles

### Q: Which MPTCP scheduler should I use?

**A:** Depends on your use case:
- **default:** Good all-around performance
- **redundant:** Best for reliability, sends packets on all paths (uses more bandwidth)
- **roundrobin:** Balanced load distribution
- **minrtt:** Prefers lowest latency path (good for gaming)

### Q: What bandwidth can I expect?

**A:** Expected bandwidth ≈ sum of all WANs minus ~10-20% overhead:
- 2x 100Mbps WANs → ~180Mbps aggregated
- 1x 100Mbps + 1x 50Mbps → ~135Mbps aggregated

Actual performance depends on:
- VPS bandwidth limits
- Network conditions
- VPN overhead
- Router CPU capacity

---

## Security & Privacy

### Q: Is my traffic encrypted?

**A:** Yes! All traffic through the VPN tunnel is encrypted using:
- Shadowsocks: ChaCha20-Poly1305 or AES-256-GCM
- Glorytun: ChaCha20-Poly1305
- MPTCP: IPsec (if configured)

### Q: Can my ISP see what I'm doing?

**A:** Your ISP can see:
- You're connecting to a VPS
- Amount of data transferred
- Connection timing

Your ISP **cannot** see:
- Websites you visit
- Data content
- Specific applications

### Q: Is this a VPN?

**A:** It's more than a traditional VPN. OpenMPTCProuter provides:
- Multi-path connection aggregation (traditional VPNs can't do this)
- Automatic failover between connections
- Load balancing
- Encrypted tunnel (like VPN)

### Q: Do I need additional firewall rules?

**A:** The installation script configures secure firewall rules automatically. Additional rules are optional based on your needs.

---

## Development & Contribution

### Q: How can I contribute?

**A:** See [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Code contribution guidelines
- Bug reporting procedures
- Feature request process
- Development setup

### Q: Can I build custom images?

**A:** Yes! See build instructions:
```bash
git clone https://github.com/spotty118/openmptcprouter.git
cd openmptcprouter
OMR_TARGET=x86_64 OMR_KERNEL=6.12 ./build.sh
```

### Q: Where can I get help?

**A:** Multiple support channels:
- [GitHub Discussions](https://github.com/spotty118/openmptcprouter/discussions) - Community support
- [GitHub Issues](https://github.com/spotty118/openmptcprouter/issues) - Bug reports
- Documentation files in this repository

### Q: How do I report bugs?

**A:** Please include:
1. Router model and OpenMPTCProuter version
2. Kernel version (6.6, 6.12, etc.)
3. VPS operating system
4. Detailed problem description
5. Relevant logs
6. Steps to reproduce

Submit at: https://github.com/spotty118/openmptcprouter/issues

---

## License & Legal

### Q: What license is this under?

**A:** GPL-3.0 License. See [LICENSE](LICENSE) for full details.

### Q: Can I use this commercially?

**A:** Yes, under the GPL-3.0 license terms. You must:
- Provide source code to users
- Maintain GPL-3.0 license
- Give credit to original authors

### Q: Who maintains this project?

**A:** OpenMPTCProuter Optimized is maintained by:
- **spotty118** - Fork maintainer and optimizer
- **Original project:** [Ycarus](https://github.com/Ysurac/openmptcprouter)

---

## Still Have Questions?

If your question isn't answered here:
1. Check [SETUP_GUIDE.md](SETUP_GUIDE.md) for setup help
2. Review [EMERGENCY_RECOVERY.md](EMERGENCY_RECOVERY.md) for recovery procedures
3. Search [GitHub Discussions](https://github.com/spotty118/openmptcprouter/discussions)
4. Open a new discussion or issue

**Happy aggregating!** 🚀
