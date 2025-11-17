# VPS Scripts - OpenMPTCProuter Optimized

This directory contains scripts for setting up your VPS server for OpenMPTCProuter.

## Quick Start - VPS Installation Wizard 🚀

The easiest way to set up your VPS is using our **self-contained installation wizard**:

### Method 1: One-Line Install (Recommended)

```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/main/vps-scripts/wizard.sh | sudo bash
```

### Method 2: Download and Run

```bash
wget https://raw.githubusercontent.com/spotty118/openmptcprouter/main/vps-scripts/wizard.sh
chmod +x wizard.sh
sudo ./wizard.sh
```

## What the Wizard Does

The VPS installation wizard will:

1. ✅ **Detect your system** - Validates OS compatibility (Debian 11/12/13, Ubuntu 20.04/22.04/24.04)
2. ✅ **Auto-configure network** - Detects public IP and network interface
3. ✅ **Generate secure credentials** - Creates cryptographically secure passwords
4. ✅ **Update system** - Installs all required packages
5. ✅ **Configure kernel** - Optimizes for MPTCP multi-WAN bonding
6. ✅ **Set up firewall** - Applies secure iptables rules
7. ✅ **Install VPN services** - Configures Shadowsocks, WireGuard, etc.
8. ✅ **Create setup page** - Generates easy web interface at `http://YOUR_VPS_IP:8080`

**Total time:** 5-10 minutes ⏱️

## Features

### 🎨 Beautiful Interface
- Color-coded output with progress indicators
- Clear step-by-step feedback
- Professional ASCII art banner

### 🔒 Security First
- Cryptographically secure random passwords
- Automatic firewall configuration
- Secure credential storage (600 permissions)

### 📱 User-Friendly
- Interactive prompts with validation
- Auto-detection of system settings
- Web-based setup page for router configuration

### 📋 Complete Documentation
- Credentials saved to `/root/openmptcprouter_credentials.txt`
- Configuration saved to `/etc/openmptcprouter/config.json`
- Web interface with copy-paste ready settings

## After Installation

Once the wizard completes, you'll get:

1. **Web Setup Page** - `http://YOUR_VPS_IP:8080`
   - Visual step-by-step router configuration
   - Copy-paste ready credentials
   - Printable setup guide

2. **Credentials File** - `/root/openmptcprouter_credentials.txt`
   - All passwords and settings
   - Quick start instructions
   - Secure storage (root only)

3. **Configuration File** - `/etc/openmptcprouter/config.json`
   - Machine-readable config
   - Version information
   - All connection details

## Router Setup (After VPS Installation)

1. **Access your router**: `http://192.168.100.1`
2. **Navigate to**: Services → OpenMPTCProuter
3. **Enter details** from the web page or credentials file:
   - Server IP: (your VPS public IP)
   - Port: 65500
   - Password: (generated during installation)
   - Encryption: Shadowsocks (chacha20-ietf-poly1305)
4. **Save & Apply**
5. **Verify**: Status → OpenMPTCProuter (should show "Connected")

## Supported Operating Systems

- ✅ Debian 11 (Bullseye)
- ✅ Debian 12 (Bookworm)
- ✅ Debian 13 (Trixie)
- ✅ Ubuntu 20.04 LTS
- ✅ Ubuntu 22.04 LTS
- ✅ Ubuntu 24.04 LTS

## VPS Requirements

- **RAM:** Minimum 1GB (2GB recommended)
- **CPU:** 1 core minimum (2+ recommended for high throughput)
- **Disk:** 10GB minimum
- **Network:** Public IP address required
- **Root access:** Required for installation

## Other Installation Methods

### Full Installation Script

For advanced users who want more control:

```bash
bash omr-vps-install.sh
```

This is the comprehensive installation script with all features.

### Easy Install Wrapper

Alternative method that downloads and runs the full installer:

```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/main/scripts/easy-install.sh | sudo bash
```

## Troubleshooting

### Installation Fails

1. **Check OS compatibility**: Run `cat /etc/os-release`
2. **Verify root access**: Run `sudo -v`
3. **Check internet**: Run `ping -c 3 8.8.8.8`
4. **Update package lists**: Run `apt-get update`

### Can't Access Web Interface

1. **Check firewall**: Ensure port 8080 is open
   ```bash
   sudo iptables -L -n | grep 8080
   ```

2. **Verify service is running**:
   ```bash
   sudo systemctl status omr-setup-web
   ```

3. **Restart service**:
   ```bash
   sudo systemctl restart omr-setup-web
   ```

### Router Won't Connect

1. **Verify VPS IP**: Check public IP matches configuration
2. **Test connectivity**: From router, ping VPS IP
3. **Check credentials**: Ensure password matches exactly
4. **Review firewall**: Port 65500 must be open on VPS
5. **Check logs**: 
   ```bash
   # On VPS
   sudo systemctl status shadowsocks-libev-server@config
   
   # On router
   logread | grep -i openmptcprouter
   ```

## Files Created During Installation

```
/etc/openmptcprouter/
  └── config.json                        # Main configuration file

/etc/shadowsocks-libev/
  └── config.json                        # Shadowsocks configuration

/etc/sysctl.d/
  └── 99-openmptcprouter.conf           # Kernel parameters

/etc/modules-load.d/
  └── openmptcprouter.conf              # Kernel modules

/etc/iptables/
  └── rules.v4                          # Firewall rules

/root/
  ├── openmptcprouter_credentials.txt   # Saved credentials
  └── iptables-backup-*.rules           # Firewall backup

/var/www/omr-setup/
  └── index.html                        # Web setup interface

/etc/systemd/system/
  └── omr-setup-web.service             # Web interface service
```

## Security Notes

- 🔐 All passwords are generated using cryptographically secure random sources
- 🔒 Credential files have 600 permissions (root only)
- 🛡️ Firewall is configured to allow only necessary ports
- 🔑 SSH access is preserved (port 22)
- ⚠️ **Never share your credentials publicly**

## What's Configured

### Network Optimization
- MPTCP enabled for multi-WAN bonding
- BBR2 congestion control
- Optimized TCP buffer sizes
- Connection tracking for high throughput

### VPN Services
- Shadowsocks (chacha20-ietf-poly1305)
- WireGuard ready
- Support for Glorytun, MLVPN, V2Ray, Xray

### Firewall
- Drop policy on INPUT and FORWARD
- SSH access (port 22)
- VPN ports (65500, 65510, 65520)
- Web interface (port 8080)
- ICMP (ping) allowed

## Support

- 📖 [Complete Setup Guide](../SETUP_GUIDE.md)
- 💬 [Community Discussions](https://github.com/spotty118/openmptcprouter/discussions)
- 🐛 [Report Issues](https://github.com/spotty118/openmptcprouter/issues)
- ⭐ [Star the Project](https://github.com/spotty118/openmptcprouter)

## License

GPL-3.0 - See [LICENSE](../LICENSE) for details.

## Credits

- Original OpenMPTCProuter by [Ysurac](https://github.com/Ysurac/openmptcprouter)
- Optimized fork by [spotty118](https://github.com/spotty118/openmptcprouter)
