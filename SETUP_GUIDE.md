# OpenMPTCProuter Optimized - Easy Setup Guide

This guide will help you quickly set up OpenMPTCProuter Optimized for bonding/aggregating multiple internet connections between your client router and VPS server.

## Table of Contents
- [Quick Start](#quick-start)
- [Client Setup](#client-setup)
- [Server Setup](#server-setup)
- [Testing Your Connection](#testing-your-connection)
- [Troubleshooting](#troubleshooting)

## Quick Start

### What You Need
1. **Client Device**: Your OpenMPTCProuter Optimized router (installed with image from this repo)
2. **VPS Server**: A remote server with public IP (DigitalOcean, Vultr, AWS, etc.)
3. **Multiple Internet Connections**: 2+ WAN connections (DSL, Cable, 4G/5G, etc.)

### Overview
```
[Your Network] ---> [OMR Client Router] ---> [Multiple WANs] ---> [Internet] ---> [VPS Server]
                         |                                                            |
                         +-------- Bonded/Aggregated MPTCP Connection ---------------+
```

## Client Setup

### Step 1: Flash Your Device
1. Download the appropriate image for your hardware from the releases
2. Flash the image to your device:
   ```bash
   # For SD card devices (Raspberry Pi, Banana Pi, etc.)
   gunzip omr-*.img.gz
   dd bs=4M if=omr-*.img of=/dev/sdX conv=fsync
   
   # For x86/x64 devices
   # Use Etcher, Rufus, or dd to write the image
   ```

### Step 2: Initial Configuration
1. Connect to the router via Ethernet
2. Access the web interface at `http://192.168.2.1`
3. Default credentials:
   - Username: `root`
   - Password: (none - press Enter)

### Step 3: Configure WAN Interfaces
1. Go to **Network → Interfaces**
2. Configure each WAN connection:
   - WAN1 (Primary): Your main internet connection
   - WAN2, WAN3, etc.: Additional connections
3. Set up each interface with appropriate settings (DHCP/Static/PPPoE)
4. Save & Apply

### Step 4: VPS Connection Settings
1. Go to **Services → OpenMPTCProuter**
2. Enter your VPS details:
   - **Server IP**: Your VPS public IP address
   - **Server Port**: Default is 65500 (or your custom port)
   - **Username**: vpn username (default: openmptcprouter)
   - **Password**: Your secure password
   - **Encryption**: Choose Shadowsocks, Glorytun, or MLVPN

### Step 5: Enable MPTCP
1. Go to **Network → MPTCP**
2. Enable MPTCP kernel module
3. Select **fullmesh** mode (allows all WANs to bond together)
4. Save & Apply

## Server Setup

### Option 1: Automated Setup (Recommended)
Use the automated VPS setup script:

```bash
# On your VPS, run:
wget -O - https://raw.githubusercontent.com/Ysurac/openmptcprouter-vps/master/install.sh | sh
```

Follow the prompts to configure your VPS.

### Option 2: Manual Configuration

#### Prerequisites
Your VPS should have:
- Ubuntu 20.04/22.04 or Debian 11/12
- Public IP address
- At least 1GB RAM
- Root access

#### Quick Manual Setup
```bash
# Update system
apt-get update && apt-get upgrade -y

# Install required packages
apt-get install -y shadowsocks-libev glorytun mptcp-tools iptables

# Configure kernel for MPTCP
echo "net.mptcp.mptcp_enabled=1" >> /etc/sysctl.conf
echo "net.ipv4.ip_forward=1" >> /etc/sysctl.conf
sysctl -p

# Configure firewall
iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
iptables -A FORWARD -i tun0 -o eth0 -j ACCEPT
iptables -A FORWARD -i eth0 -o tun0 -m state --state RELATED,ESTABLISHED -j ACCEPT

# Save iptables rules
iptables-save > /etc/iptables/rules.v4
```

### Server Connection Settings

Create a configuration file `/etc/openmptcprouter/config.json`:

```json
{
  "server_ip": "YOUR_VPS_PUBLIC_IP",
  "server_port": 65500,
  "username": "openmptcprouter",
  "password": "YOUR_SECURE_PASSWORD",
  "encryption": "shadowsocks",
  "shadowsocks": {
    "method": "chacha20-ietf-poly1305",
    "timeout": 600
  }
}
```

## Easy Configuration Helper Script

We provide a helper script to make setup even easier. Run this on your VPS:

```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/easy-setup.sh | bash
```

This script will:
1. Detect your VPS configuration
2. Install required packages
3. Configure MPTCP kernel settings
4. Set up VPN tunnels (Shadowsocks/Glorytun)
5. Configure firewall rules
6. Generate client configuration

## Testing Your Connection

### Step 1: Verify Tunnel is Up
On the client router:
```bash
# SSH to router
ssh root@192.168.2.1

# Check tunnel status
ip addr show tun0
# Should show an IP address

# Check MPTCP status
cat /proc/net/mptcp_net/mptcp
```

### Step 2: Test Bandwidth
1. Go to **Status → OpenMPTCProuter**
2. Click **Test Bandwidth**
3. You should see combined bandwidth from all WANs

### Step 3: Verify Aggregation
```bash
# On client router, check active subflows
ss -tin | grep -i mptcp

# You should see multiple subflows (one per WAN)
```

## Connection Modes

### Fullmesh (Recommended for Bonding)
All WAN interfaces bond together for maximum throughput.

**When to use**: 
- Maximum bandwidth needed
- All WANs have good latency
- High bandwidth applications (streaming, downloads)

### Master/Backup (Failover Mode)
Primary WAN is used, others are backup.

**When to use**:
- Reliability over speed
- One WAN is significantly faster
- Cost savings (limited data on backup WANs)

### Round-Robin
Connections distributed evenly across all WANs.

**When to use**:
- Multiple similar WANs
- Load balancing
- VoIP/Gaming alongside downloads

## Troubleshooting

### Client Can't Connect to Server
1. Check VPS firewall allows port 65500 (or your custom port)
2. Verify VPS public IP is correct in client settings
3. Check VPS has net.ipv4.ip_forward enabled
4. Test basic connectivity: `ping YOUR_VPS_IP`

### Slow Speeds Despite Multiple WANs
1. Verify all WANs are actually connected (check interface status)
2. Check MPTCP is enabled: `lsmod | grep mptcp`
3. Verify fullmesh mode is selected
4. Check for packet loss on individual WANs
5. Test each WAN individually to identify slow links

### One WAN Not Being Used
1. Check WAN interface is up: `ifconfig`
2. Verify routing table: `ip route show`
3. Check MPTCP status: `ip mptcp endpoint show`
4. Review firewall rules aren't blocking interface

### VPN Tunnel Drops
1. Check VPS is running and accessible
2. Verify credentials match on both sides
3. Review logs: `logread | grep -i openmptcprouter`
4. Try different encryption method (Shadowsocks vs Glorytun)

## Advanced Configuration

### Custom Port Configuration
If you need to use a different port (e.g., due to ISP restrictions):

**On VPS:**
```bash
# Edit server config
vi /etc/shadowsocks-libev/config.json
# Change "server_port": 65500 to your desired port

# Update firewall
iptables -A INPUT -p tcp --dport YOUR_PORT -j ACCEPT
```

**On Client:**
1. Go to Services → OpenMPTCProuter
2. Change Server Port to match VPS

### Multiple Servers (Load Balancing)
You can configure multiple VPS servers for redundancy:
1. Set up multiple servers using the same steps
2. On client, configure backup servers in Services → OpenMPTCProuter
3. Enable automatic failover

### QoS Configuration
Prioritize traffic types for better performance:
1. Go to Network → QoS
2. Enable SQM (Smart Queue Management)
3. Set bandwidth limits per WAN
4. Configure priority rules (VoIP > Web > Bulk)

## Quick Reference Commands

### Client Router Commands
```bash
# Check connection status
omr-status

# Restart VPN tunnel
/etc/init.d/openmptcprouter restart

# Check MPTCP connections
ss -tin | grep -i mptcp

# View logs
logread | grep -i omr
```

### Server Commands
```bash
# Check tunnel status
ip addr show tun0

# Check connected clients
ss -tn | grep :65500

# View MPTCP kernel status
cat /proc/net/mptcp_net/mptcp

# Restart services
systemctl restart shadowsocks-libev
```

## Getting Help

- **Documentation**: [OpenMPTCProuter Wiki](https://github.com/Ysurac/openmptcprouter/wiki)
- **Forum**: [Community Support](https://github.com/spotty118/openmptcprouter/discussions)
- **Issues**: [Report Bugs](https://github.com/spotty118/openmptcprouter/issues)

## Security Best Practices

1. **Change Default Passwords**: Always use strong, unique passwords
2. **Keep Updated**: Regularly update both client and server
3. **Use Encryption**: Always enable VPN encryption (Shadowsocks recommended)
4. **Firewall Rules**: Only open necessary ports on VPS
5. **Monitor Logs**: Regularly check for unusual activity

---

**Need more help?** Check out the [FAQ](FAQ.md) or open an issue on GitHub.
