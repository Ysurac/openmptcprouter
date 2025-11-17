#!/bin/sh
# Display helpful first-boot message on console login

cat << 'BANNER'
 
 ╔═══════════════════════════════════════════════════════════════╗
 ║         OpenMPTCProuter Optimized - Quick Start Guide        ║
 ╚═══════════════════════════════════════════════════════════════╝
 
 📡 Web Interface:  http://192.168.2.1
 🔐 Default Login:   root (no password initially)
 
 📊 Quick Commands:
   omr-status       - Show all WAN connections and bonding status
   omr-recovery     - Emergency recovery if locked out
   
 📋 Network Configuration:
   • Physical ports auto-detected on first boot
   • USB modems automatically configured as additional WANs
   • All WANs have MPTCP bonding enabled
   • Customize everything via web interface
   
 🔧 Troubleshooting:
   • Can't access web UI? Run: omr-recovery
   • Need to check connections? Run: omr-status
   • Locked out? Safety monitor will auto-recover in 60 seconds
   
 💡 Tips:
   • Plug in USB modems for automatic multi-WAN bonding
   • All WANs are bonded together for increased speed
   • System prevents you from locking yourself out
   • LAN is always 192.168.2.1 (never changes to 169.254.x.x)
   
BANNER

# Show WiFi password if it exists
if [ -f /etc/wifi-password.txt ]; then
    echo " 📶 WiFi Password:"
    cat /etc/wifi-password.txt | sed 's/^/   /'
    echo ""
fi

# Show current status
if [ -x /usr/bin/omr-status ]; then
    echo " 📈 Current Status:"
    /usr/bin/omr-status --plain 2>/dev/null | head -20 | sed 's/^/   /'
    echo ""
fi

echo " ═══════════════════════════════════════════════════════════════"
echo ""
