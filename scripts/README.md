# 🚀 Super Easy VPS Setup

## One-Command Installation

Just copy and paste this command on your VPS:

```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/easy-install.sh | sudo bash
```

That's it! The script will:
- ✅ Automatically detect your OS
- ✅ Install all required software
- ✅ Configure firewall and networking
- ✅ Generate secure passwords
- ✅ Create a beautiful web page with your settings

## After Installation

1. Open your browser to: `http://YOUR_VPS_IP:8080`
2. Follow the 3-step setup guide shown on the page
3. Done! Your connections are now bonded!

## What You Get

### On Your VPS
- Fully configured MPTCP server
- Shadowsocks VPN ready to use
- Optimized BBR2 congestion control
- Secure firewall rules
- Easy-to-use web interface

### Connection Details
All your passwords and settings are:
- Displayed on the screen after installation
- Available at `http://YOUR_VPS_IP:8080`
- Saved to `/root/openmptcprouter_credentials.txt`

## Router Configuration (3 Steps)

### Step 1: Access Router
Open browser to: `http://192.168.2.1`

### Step 2: Go to VPN Settings
Navigate to: **Services → OpenMPTCProuter**

### Step 3: Enter Details
Copy the details from your VPS setup page:
- Server IP
- Port (usually 65500)
- Password
- Encryption: Shadowsocks

Click **Save & Apply** and you're done!

## Supported Operating Systems

- ✅ Debian 11 (Bullseye)
- ✅ Debian 12 (Bookworm)  
- ✅ Debian 13 (Trixie)
- ✅ Ubuntu 20.04 LTS
- ✅ Ubuntu 22.04 LTS
- ✅ Ubuntu 24.04 LTS

## Need Help?

- 📖 [Full Setup Guide](../SETUP_GUIDE.md)
- 💬 [Community Discussions](https://github.com/spotty118/openmptcprouter/discussions)
- 🐛 [Report Issues](https://github.com/spotty118/openmptcprouter/issues)

## Advanced Options

### Custom Installation
If you want to customize the installation, you can set environment variables:

```bash
# Use specific kernel version
export KERNEL=6.12
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/scripts/easy-install.sh | sudo bash
```

### Manual Installation
For more control, use the full installer:

```bash
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/develop/vps-scripts/omr-vps-install.sh | sudo bash
```

## Security Notes

🔐 **Important**: Your VPS passwords are shown ONCE during installation.
- Save them to a password manager
- Print the setup page
- Screenshot the credentials
- Never share publicly

## Troubleshooting

### Can't access the setup page?
Check firewall:
```bash
sudo iptables -A INPUT -p tcp --dport 8080 -j ACCEPT
```

### Lost your passwords?
They're saved in:
```bash
cat /root/openmptcprouter_credentials.txt
```

Or:
```bash
cat /etc/openmptcprouter/config.json
```

### VPN not connecting?
1. Check VPS IP is correct
2. Verify port 65500 is open
3. Confirm password matches exactly
4. Check VPS service status:
```bash
systemctl status shadowsocks-libev-server@config
```

## Why This is Better Than the Original

### Original Setup Issues:
❌ Complex multi-step process  
❌ Manual configuration required  
❌ Easy to make mistakes  
❌ No visual guidance  
❌ Confusing for beginners  

### Optimized Easy Setup:
✅ One command installation  
✅ Automatic configuration  
✅ Web-based setup guide  
✅ Copy-paste ready settings  
✅ Beginner friendly  
✅ Visual step-by-step instructions  

---

Made with ❤️ by the OpenMPTCProuter Optimized team
