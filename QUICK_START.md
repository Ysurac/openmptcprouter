# OpenMPTCProuter Optimized - Quick Start Guide

Get your multi-WAN bonding setup running in **under 10 minutes**!

## 🚀 What You Need

- **VPS**: Debian 11/12/13 or Ubuntu 20.04/22.04/24.04 (with root access)
- **Router**: OpenMPTCProuter-compatible hardware (or compatible OpenWrt device)
- 5-10 minutes of your time

---

## 📋 Two-Step Setup Process

### Step 1️⃣: Setup Your VPS (5 minutes)

**One command - that's it!**

```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/wizard.sh | sudo bash
```

The wizard will:
- ✓ Auto-detect your system configuration
- ✓ Generate secure passwords
- ✓ Install and configure all VPN services
- ✓ Setup firewall and MPTCP
- ✓ Create a web setup page
- ✓ **Generate a pairing code and QR code**

**After installation completes:**

1. You'll see a **pairing code** displayed in your terminal
2. A **QR code** for easy mobile transfer
3. Connection details (IP, password, etc.)
4. Access the web interface at `http://YOUR_VPS_IP:8080`

**💡 Pro Tip:** Take a photo of the QR code or copy the pairing code - you'll need it for the router!

---

### Step 2️⃣: Setup Your Router (2 minutes)

#### **Option A: First-Time Setup Wizard (Easiest)**

If you just flashed OpenMPTCProuter on your router:

1. Connect to router WiFi or plug in Ethernet
2. Open browser to **http://192.168.2.1**
3. The setup wizard automatically appears
4. Choose **"Pairing Code"** method
5. **Paste your pairing code** from Step 1
6. Click "Apply" and you're done!

#### **Option B: Manual Setup via Web Interface**

If already logged in to your router:

1. Go to **Services → OpenMPTCProuter**
2. Enter these details:
   - Server IP: `YOUR_VPS_IP`
   - Port: `65500`
   - Password: `YOUR_PASSWORD`
   - Encryption: `Shadowsocks (chacha20-ietf-poly1305)`
3. Click **Save & Apply**

#### **Option C: Command-Line Setup**

SSH into your router and run:

```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/client-auto-setup.sh | sh -s YOUR_VPS_IP YOUR_PASSWORD
```

---

## ✅ Verify Your Setup

After setup, verify everything is working:

```bash
# On VPS:
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/verify-setup.sh | sudo bash -s vps

# On Router (via SSH):
wget -O- https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/verify-setup.sh | sh -s router
```

The verification script will check:
- ✓ All services running
- ✓ Firewall configured
- ✓ MPTCP enabled
- ✓ Connectivity working
- ✓ VPN tunnel active

---

## 🎯 What's Different in Optimized?

### **Simplified Setup**

| Before | After (Optimized) |
|--------|-------------------|
| Multiple setup methods, confusing docs | **One command for VPS, one for router** |
| Manual credential transfer | **QR code & pairing code** |
| No validation tools | **Built-in verification script** |
| Scattered documentation | **Single quick start guide** |

### **New Features**

- 🎯 **Unified Setup Coordinator**: Detects VPS or router automatically
- 📱 **QR Code Pairing**: Scan and transfer config instantly
- 🔐 **Pairing Codes**: Base64-encoded JSON for one-click setup
- ✅ **Health Checks**: Verify your setup is working correctly
- 🌐 **Enhanced Web UI**: Beautiful setup page with copy-paste buttons
- 📊 **Status Dashboard**: See all your connection details at a glance

---

## 🎨 Easy Pairing System

The new pairing code system makes setup a breeze:

1. **VPS generates pairing code** (base64-encoded JSON with all settings)
2. **You copy/paste or scan QR code**
3. **Router auto-configures** everything

**Pairing code contains:**
```json
{
  "server_ip": "1.2.3.4",
  "server_port": 65500,
  "password": "secure_generated_password",
  "encryption": "chacha20-ietf-poly1305",
  "version": "1.0"
}
```

No more manual typing of long passwords! 🎉

---

## 🔧 Advanced Setup (Optional)

### Multiple WAN Connections

OpenMPTCProuter automatically detects and uses:
- Ethernet WAN ports
- USB modems (QMI, MBIM, RNDIS, NCM)
- WiFi as WAN
- Tethered phones

Just plug them in - auto-configuration handles the rest!

### Connection Modes

Configure in router web UI under **Services → OpenMPTCProuter → Settings**:

- **Full Mesh** (default): All WANs used simultaneously
- **Master/Backup**: Primary + failover
- **Round Robin**: Balanced distribution

### Custom Kernel

Want the latest MPTCP features? The VPS wizard supports custom kernels:

```bash
# Edit wizard.sh line 160 before running:
KERNEL=6.12  # or latest version
```

---

## 🆘 Troubleshooting

### VPS Issues

**Port not listening?**
```bash
systemctl status shadowsocks-libev
systemctl restart shadowsocks-libev
```

**Firewall blocking?**
```bash
iptables -L -n | grep 65500
# Should show ACCEPT rule
```

**Can't access web interface?**
```bash
systemctl status omr-setup-web
systemctl restart omr-setup-web
# Access at http://YOUR_IP:8080
```

### Router Issues

**VPN not connecting?**
```bash
# Check config
uci show shadowsocks-libev

# Restart service
/etc/init.d/shadowsocks-libev restart

# Check logs
logread | grep shadowsocks
```

**No internet on router?**
```bash
# Check WAN status
ifstatus wan

# Restart network
/etc/init.d/network restart
```

**Reset to defaults?**
```bash
# Run emergency LAN restore
/usr/bin/emergency-lan-restore.sh
```

### Still Stuck?

1. **Run verification script** (see above)
2. **Check logs**:
   - VPS: `journalctl -xe`
   - Router: `logread`
3. **Visit support**:
   - 📖 [Full Setup Guide](https://github.com/spotty118/openmptcprouter/blob/main/SETUP_GUIDE.md)
   - 🐛 [Report Issues](https://github.com/spotty118/openmptcprouter/issues)
   - 💬 [Community Discussions](https://github.com/spotty118/openmptcprouter/discussions)

---

## 📊 Performance Tips

### VPS Optimization

1. **Choose nearby VPS location** (lower latency = better performance)
2. **Use BBR2 congestion control** (enabled by default)
3. **Enable CAKE qdisc** for better bufferbloat handling
4. **Sufficient bandwidth** (VPS should handle sum of all WANs)

### Router Optimization

1. **Update to latest firmware**
2. **Use quality USB modems** (avoid cheap knockoffs)
3. **Position antennas properly** for best signal
4. **Monitor temperatures** (add cooling if needed)
5. **Balance WAN usage** in connection mode settings

### Network Tuning

The wizard auto-configures optimal settings:
- ✓ TCP BBR2 congestion control
- ✓ MPTCP fullmesh mode
- ✓ Large TCP windows
- ✓ ECN enabled
- ✓ Optimized queue management

---

## 🌟 Next Steps

After setup:

1. **Test your connection**: Visit [fast.com](https://fast.com) or [speedtest.net](https://speedtest.net)
2. **Monitor your WANs**: Router web UI → Status → OpenMPTCProuter
3. **Configure advanced features**: Services → OpenMPTCProuter
4. **Add more WANs**: Just plug them in (auto-detection works!)
5. **Customize**: Explore firewall rules, QoS, traffic shaping

---

## 🎉 You're All Set!

Your multi-WAN bonding setup is now complete! Enjoy:

- 🚀 **Aggregated bandwidth** from all your connections
- 🔒 **Encrypted traffic** via Shadowsocks/WireGuard
- 🔄 **Automatic failover** if one WAN goes down
- ⚡ **Lower latency** via multipath TCP
- 📶 **Load balancing** across all links

**Thank you for using OpenMPTCProuter Optimized!**

---

## 📚 Additional Resources

- [Complete Setup Guide](SETUP_GUIDE.md) - Detailed documentation
- [GitHub Repository](https://github.com/spotty118/openmptcprouter) - Source code
- [Contributing Guide](CONTRIBUTING.md) - Help improve the project
- [Changelog](CHANGELOG.md) - See what's new

**Need Help?** We're here for you!
- 💬 [Discussions](https://github.com/spotty118/openmptcprouter/discussions)
- 🐛 [Issues](https://github.com/spotty118/openmptcprouter/issues)
- ⭐ [Star us on GitHub](https://github.com/spotty118/openmptcprouter)
