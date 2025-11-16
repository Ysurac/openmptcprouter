#!/bin/bash
#
# OpenMPTCProuter Optimized - One-Command Easy Install
# Just run: curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/main/scripts/easy-install.sh | bash
#

set -e

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

clear

echo -e "${CYAN}"
cat << 'EOF'
╔═══════════════════════════════════════════════════════════╗
║                                                           ║
║   ██████╗ ███╗   ███╗██████╗                            ║
║  ██╔═══██╗████╗ ████║██╔══██╗                           ║
║  ██║   ██║██╔████╔██║██████╔╝                           ║
║  ██║   ██║██║╚██╔╝██║██╔══██╗                           ║
║  ╚██████╔╝██║ ╚═╝ ██║██║  ██║                           ║
║   ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝                           ║
║                                                           ║
║           OPTIMIZED - Easy Setup Wizard                   ║
║                                                           ║
╚═══════════════════════════════════════════════════════════╝
EOF
echo -e "${NC}"

echo -e "${GREEN}Welcome to OpenMPTCProuter Optimized - The EASY Way!${NC}"
echo ""
echo -e "${YELLOW}This wizard will set up your VPS in just a few minutes.${NC}"
echo -e "${YELLOW}No technical knowledge required!${NC}"
echo ""

# Check root
if [ "$(id -u)" -ne 0 ]; then 
    echo -e "${RED}Please run as root or with sudo:${NC}"
    echo -e "${CYAN}curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/main/scripts/easy-install.sh | sudo bash${NC}"
    exit 1
fi

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
else
    echo -e "${RED}Cannot detect your operating system.${NC}"
    exit 1
fi

echo -e "${BLUE}┌─────────────────────────────────────┐${NC}"
echo -e "${BLUE}│  System Information                 │${NC}"
echo -e "${BLUE}├─────────────────────────────────────┤${NC}"
echo -e "${BLUE}│${NC} OS: ${GREEN}$PRETTY_NAME${NC}"
echo -e "${BLUE}│${NC} Kernel: ${GREEN}$(uname -r)${NC}"

# Get public IP
echo -e "${BLUE}│${NC} Detecting your public IP..."
VPS_IP=$(curl -4 -s --max-time 5 ifconfig.me 2>/dev/null || curl -4 -s --max-time 5 icanhazip.com 2>/dev/null || echo "Unable to detect")

if [ "$VPS_IP" = "Unable to detect" ]; then
    echo -e "${YELLOW}Could not auto-detect IP. Please enter manually:${NC}"
    read -p "VPS Public IP: " VPS_IP
fi

echo -e "${BLUE}│${NC} Public IP: ${GREEN}$VPS_IP${NC}"
echo -e "${BLUE}└─────────────────────────────────────┘${NC}"
echo ""

# Simple yes/no prompt
echo -e "${PURPLE}═══════════════════════════════════════${NC}"
echo -e "${YELLOW}Ready to install OpenMPTCProuter Optimized?${NC}"
echo ""
echo "What this will do:"
echo "  ✓ Install and configure VPN server"
echo "  ✓ Set up multiple aggregation protocols"
echo "  ✓ Configure firewall automatically"
echo "  ✓ Generate secure passwords"
echo "  ✓ Create easy router configuration"
echo "  ✓ Generate setup webpage"
echo ""
read -p "Continue? (Y/n): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]] && [[ ! -z $REPLY ]]; then
    echo -e "${YELLOW}Installation cancelled.${NC}"
    exit 0
fi

echo ""
echo -e "${GREEN}┌────────────────────────────────────────┐${NC}"
echo -e "${GREEN}│  Starting Installation...              │${NC}"
echo -e "${GREEN}└────────────────────────────────────────┘${NC}"
echo ""

# Create temp directory for downloads
TEMP_DIR=$(mktemp -d)
cd $TEMP_DIR

# Download and run the full installer
echo -e "${CYAN}[1/3]${NC} Downloading installer..."
curl -sSL https://raw.githubusercontent.com/spotty118/openmptcprouter/main/vps-scripts/omr-vps-install.sh -o installer.sh
chmod +x installer.sh

echo -e "${CYAN}[2/3]${NC} Running installation (this may take 5-10 minutes)..."
echo ""

# Run installer with environment variables
export VPS_PUBLIC_IP="$VPS_IP"
export DEBIAN_FRONTEND=noninteractive
./installer.sh

echo ""
echo -e "${CYAN}[3/3]${NC} Generating easy setup page..."
echo ""

# Create web-based configuration page
mkdir -p /var/www/omr-setup

cat > /var/www/omr-setup/index.html << 'ENDHTML'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>OMR Setup - Easy Configuration</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        .header h1 { font-size: 2em; margin-bottom: 10px; }
        .header p { opacity: 0.9; }
        .content { padding: 30px; }
        .section {
            margin-bottom: 30px;
            padding: 20px;
            background: #f8f9fa;
            border-radius: 10px;
            border-left: 4px solid #667eea;
        }
        .section h2 {
            color: #667eea;
            margin-bottom: 15px;
            font-size: 1.3em;
        }
        .config-item {
            margin: 10px 0;
            padding: 10px;
            background: white;
            border-radius: 5px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .config-label {
            font-weight: 600;
            color: #333;
        }
        .config-value {
            font-family: 'Courier New', monospace;
            color: #667eea;
            font-weight: bold;
            cursor: pointer;
            padding: 5px 10px;
            background: #f0f0f0;
            border-radius: 5px;
        }
        .config-value:hover {
            background: #e0e0e0;
        }
        .copy-btn {
            background: #667eea;
            color: white;
            border: none;
            padding: 8px 15px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 0.9em;
            margin-left: 10px;
        }
        .copy-btn:hover {
            background: #5568d3;
        }
        .step {
            background: white;
            padding: 15px;
            margin: 10px 0;
            border-radius: 5px;
            border-left: 3px solid #28a745;
        }
        .step-number {
            display: inline-block;
            background: #28a745;
            color: white;
            width: 30px;
            height: 30px;
            line-height: 30px;
            text-align: center;
            border-radius: 50%;
            margin-right: 10px;
            font-weight: bold;
        }
        .success-badge {
            display: inline-block;
            background: #28a745;
            color: white;
            padding: 5px 15px;
            border-radius: 20px;
            font-size: 0.9em;
            margin-left: 10px;
        }
        .warning {
            background: #fff3cd;
            border-left-color: #ffc107;
            color: #856404;
        }
        .important {
            background: #f8d7da;
            border-left-color: #dc3545;
            color: #721c24;
        }
        @media print {
            body { background: white; }
            .no-print { display: none; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🚀 OMR Optimized Setup</h1>
            <p>Your server is ready! Follow these simple steps to connect your router.</p>
        </div>
        
        <div class="content">
            <div class="section important">
                <h2>⚠️ Important: Save This Page!</h2>
                <p>Print or screenshot this page. You'll need these settings to configure your router.</p>
                <button onclick="window.print()" class="copy-btn" style="margin-top:10px">🖨️ Print This Page</button>
            </div>

            <div class="section">
                <h2>📋 Your Server Details</h2>
                <div class="config-item">
                    <span class="config-label">Server IP:</span>
                    <div>
                        <span class="config-value" id="server-ip">REPLACE_IP</span>
                        <button class="copy-btn" onclick="copyValue('server-ip')">Copy</button>
                    </div>
                </div>
                <div class="config-item">
                    <span class="config-label">Server Port:</span>
                    <div>
                        <span class="config-value">65500</span>
                        <button class="copy-btn" onclick="copyText('65500')">Copy</button>
                    </div>
                </div>
                <div class="config-item">
                    <span class="config-label">Password:</span>
                    <div>
                        <span class="config-value" id="password">REPLACE_PASSWORD</span>
                        <button class="copy-btn" onclick="copyValue('password')">Copy</button>
                    </div>
                </div>
                <div class="config-item">
                    <span class="config-label">Encryption:</span>
                    <div>
                        <span class="config-value">Shadowsocks (chacha20-ietf-poly1305)</span>
                    </div>
                </div>
            </div>

            <div class="section">
                <h2>📱 Quick Router Setup (3 Steps)</h2>
                
                <div class="step">
                    <span class="step-number">1</span>
                    <strong>Access your router:</strong><br>
                    Connect to your router's WiFi or via Ethernet, then open browser to:<br>
                    <code style="background:#f0f0f0;padding:5px;border-radius:3px;display:inline-block;margin-top:5px">http://192.168.100.1</code>
                </div>

                <div class="step">
                    <span class="step-number">2</span>
                    <strong>Go to VPN Settings:</strong><br>
                    Navigate to: <strong>Services → OpenMPTCProuter</strong>
                </div>

                <div class="step">
                    <span class="step-number">3</span>
                    <strong>Enter these details:</strong><br>
                    <ul style="margin-top:10px;margin-left:40px">
                        <li>Server IP: <strong><span id="server-ip-2">REPLACE_IP</span></strong></li>
                        <li>Port: <strong>65500</strong></li>
                        <li>Password: <strong><span id="password-2">REPLACE_PASSWORD</span></strong></li>
                        <li>Encryption: <strong>Shadowsocks</strong></li>
                        <li>Click <strong>"Save & Apply"</strong></li>
                    </ul>
                </div>
            </div>

            <div class="section">
                <h2>✅ Testing Your Connection</h2>
                <div class="step">
                    <span class="step-number">✓</span>
                    After saving settings, wait 30 seconds, then check:<br>
                    <strong>Status → OpenMPTCProuter</strong><br>
                    You should see: <span class="success-badge">Connected</span>
                </div>
            </div>

            <div class="section warning">
                <h2>🔐 Security Reminder</h2>
                <p><strong>Keep your password secure!</strong> Anyone with this password can use your VPS.</p>
                <p style="margin-top:10px">Never share these credentials publicly.</p>
            </div>

            <div class="section">
                <h2>📚 Need Help?</h2>
                <ul style="margin-left:20px;line-height:1.8">
                    <li><a href="https://github.com/spotty118/openmptcprouter/blob/main/SETUP_GUIDE.md" target="_blank">📖 Full Setup Guide</a></li>
                    <li><a href="https://github.com/spotty118/openmptcprouter/issues" target="_blank">🐛 Report Issues</a></li>
                    <li><a href="https://github.com/spotty118/openmptcprouter/discussions" target="_blank">💬 Community Support</a></li>
                </ul>
            </div>
        </div>
    </div>

    <script>
        function copyValue(elementId) {
            const text = document.getElementById(elementId).textContent;
            copyText(text);
        }
        
        function copyText(text) {
            navigator.clipboard.writeText(text).then(() => {
                alert('Copied to clipboard!');
            });
        }
    </script>
</body>
</html>
ENDHTML

# Replace placeholders
sed -i "s/REPLACE_IP/$VPS_IP/g" /var/www/omr-setup/index.html
if [ -f /etc/openmptcprouter/config.json ]; then
    PASSWORD=$(jq -r '.credentials.shadowsocks_password' /etc/openmptcprouter/config.json 2>/dev/null || echo "check /root/openmptcprouter_credentials.txt")
    sed -i "s/REPLACE_PASSWORD/$PASSWORD/g" /var/www/omr-setup/index.html
fi

# Install and configure simple web server
echo -e "${CYAN}Setting up easy access web page...${NC}"
apt-get install -y -qq python3 jq > /dev/null 2>&1

# Create systemd service for web interface
cat > /etc/systemd/system/omr-setup-web.service << 'ENDSERVICE'
[Unit]
Description=OMR Setup Web Interface
After=network.target

[Service]
Type=simple
WorkingDirectory=/var/www/omr-setup
ExecStart=/usr/bin/python3 -m http.server 8080
Restart=always

[Install]
WantedBy=multi-user.target
ENDSERVICE

systemctl daemon-reload
systemctl enable omr-setup-web > /dev/null 2>&1
systemctl restart omr-setup-web

# Cleanup
cd /
rm -rf $TEMP_DIR

clear

# Final success message
echo -e "${GREEN}"
cat << 'EOF'
╔════════════════════════════════════════════════════════════╗
║                                                            ║
║          ✓ INSTALLATION SUCCESSFUL!                        ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝
EOF
echo -e "${NC}"

echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}       Your VPS is ready for OpenMPTCProuter!${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════${NC}"
echo ""

if [ -f /root/openmptcprouter_credentials.txt ]; then
    echo -e "${YELLOW}📋 Quick Connection Info:${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    
    echo -e "${CYAN}Server IP:${NC}     ${GREEN}$VPS_IP${NC}"
    echo -e "${CYAN}Port:${NC}          ${GREEN}65500${NC}"
    
    if [ -f /etc/openmptcprouter/config.json ]; then
        PASSWORD=$(jq -r '.credentials.shadowsocks_password' /etc/openmptcprouter/config.json 2>/dev/null)
        if [ -n "$PASSWORD" ] && [ "$PASSWORD" != "null" ]; then
            echo -e "${CYAN}Password:${NC}      ${GREEN}$PASSWORD${NC}"
        fi
    fi
    
    echo -e "${CYAN}Encryption:${NC}    ${GREEN}Shadowsocks (chacha20-ietf-poly1305)${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
fi

echo ""
echo -e "${PURPLE}🌐 Easy Setup Page (RECOMMENDED):${NC}"
echo -e "   ${GREEN}http://$VPS_IP:8080${NC}"
echo ""
echo -e "   ${YELLOW}Open this in your browser for:${NC}"
echo -e "   • Step-by-step router configuration"
echo -e "   • Copy-paste ready settings"
echo -e "   • Printable setup guide"
echo ""

echo -e "${PURPLE}📁 Credentials saved to:${NC}"
echo -e "   ${CYAN}/root/openmptcprouter_credentials.txt${NC}"
echo ""

echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}Next Steps:${NC}"
echo -e "  ${CYAN}1.${NC} Open ${GREEN}http://$VPS_IP:8080${NC} in your browser"
echo -e "  ${CYAN}2.${NC} Follow the 3-step setup instructions"
echo -e "  ${CYAN}3.${NC} Done! You're bonding multiple connections!"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

echo -e "${GREEN}🎉 Enjoy your optimized multi-WAN connection!${NC}"
echo ""
echo -e "${BLUE}Support: ${CYAN}https://github.com/spotty118/openmptcprouter${NC}"
echo ""
