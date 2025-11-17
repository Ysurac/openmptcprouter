# Emergency Recovery Guide

## Overview

If you've locked yourself out of the router and can't access the web interface or SSH, this guide shows you how to recover access using hardware methods.

## Recovery Methods

### Method 1: Reset Button (Recommended)

**Physical reset button on the router case**

#### Short Press (< 5 seconds)
- **What it does:** Restores at least one LAN port
- **When to use:** You can't access the web UI
- **How:**
  1. Press and hold the reset button
  2. Release after 2-3 seconds
  3. Wait 30 seconds for recovery
  4. Connect to any LAN port
  5. Access http://192.168.2.1

#### Long Press (5+ seconds)
- **What it does:** Complete factory reset - all ports to LAN
- **When to use:** Total lockout or want fresh start
- **How:**
  1. Press and hold the reset button
  2. Keep holding for 5+ seconds
  3. Release when LED starts flashing
  4. Wait 60 seconds for full reset
  5. Connect to any port (all are LAN now)
  6. Access http://192.168.2.1

**Visual indicators:**
- LED flashing: Reset in progress
- LED solid: Reset complete

### Method 2: Serial Console (Advanced)

**Connect via serial port (UART/TTL)**

#### Equipment needed:
- USB-to-TTL serial adapter (3.3V)
- 3 jumper wires
- Serial console software (PuTTY, screen, minicom)

#### Connection:
```
Router GND  → Adapter GND
Router TX   → Adapter RX
Router RX   → Adapter TX
(Do NOT connect VCC/3.3V)
```

#### Settings:
- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1
- Flow control: None

#### Recovery steps:
1. Connect serial adapter
2. Open serial console
3. Power on router
4. Watch boot messages
5. Press Enter when you see login prompt
6. Login as root
7. Run recovery command:
   ```bash
   /usr/bin/emergency-lan-restore.sh
   ```
8. Wait for completion
9. Disconnect serial, connect ethernet
10. Access http://192.168.2.1

### Method 3: Safety Monitor (Automatic)

**No action required - automatic recovery**

#### How it works:
- Safety monitor checks every 30 seconds
- Detects if LAN is inaccessible
- Automatically restores at least one LAN port
- Takes back a WAN port if needed

#### Timeline:
- **0s:** User makes configuration error
- **30s:** Safety monitor detects issue
- **40s:** Analysis and port selection
- **50s:** Network reconfigured
- **60s:** LAN accessible at 192.168.2.1

#### When it triggers:
- All ports assigned to WAN
- LAN set to DHCP (causes APIPA)
- No physical ports in LAN bridge
- APIPA address detected

#### What it does:
1. Finds first available port not in WAN
2. If all ports are WAN, takes last one back
3. Creates LAN bridge with that port
4. Sets LAN to 192.168.2.1 (static)
5. Enables DHCP server
6. Logs recovery action

#### Checking logs:
```bash
# Via serial console
logread | grep network-safety

# After recovery
ssh root@192.168.2.1
logread | grep network-safety
```

### Method 4: Power Cycle + Serial

**For devices without reset button**

1. Power off router
2. Connect serial console
3. Power on router
4. Watch boot sequence
5. When you see:
   ```
   Press any key to stop autoboot:  3
   ```
6. Press any key to enter bootloader (if available)
7. Or wait for Linux to boot
8. Login via serial console
9. Run emergency restore:
   ```bash
   /usr/bin/emergency-lan-restore.sh
   ```

## What Gets Reset

### Short Press / Emergency Restore
- **Changed:** One port moved to LAN
- **Preserved:** WAN configurations, WiFi settings, all other configs
- **Result:** Minimal disruption, just restores access

### Long Press / Factory Reset
- **Changed:** All network configuration
- **Reset to:** All ports in LAN, first boot will auto-configure
- **Preserved:** None
- **Result:** Fresh start

### Safety Monitor Recovery
- **Changed:** One port moved to LAN, LAN set to static
- **Preserved:** Other WANs, WiFi, most configs
- **Result:** Automatic with minimal changes

## Scenarios and Solutions

### Scenario 1: "I assigned all ports to WAN and can't access the router"

**Solution A (Easiest):** Wait 60 seconds
- Safety monitor will automatically recover
- One port will be moved back to LAN
- Access restored at 192.168.2.1

**Solution B (Fastest):** Press reset button (short)
- Recovery in 30 seconds
- One port restored to LAN
- Access restored at 192.168.2.1

**Solution C (Most reliable):** Use serial console
- Connect via UART
- Login and run emergency-lan-restore.sh
- Access restored immediately

### Scenario 2: "Router has 169.254.x.x address and I can't connect"

**This means LAN was set to DHCP (incorrect)**

**Solution:** Wait 30 seconds
- Safety monitor detects APIPA
- Changes LAN back to static 192.168.2.1
- Access restored automatically

### Scenario 3: "I changed LAN IP and forgot what I set it to"

**Solution A:** Press and hold reset button (long, 5+ seconds)
- Factory reset
- LAN restored to 192.168.2.1
- All ports in LAN

**Solution B:** Use serial console
- Login via serial
- Check current IP: `uci get network.lan.ipaddr`
- Reset to default:
  ```bash
  uci set network.lan.ipaddr='192.168.2.1'
  uci commit network
  /etc/init.d/network restart
  ```

### Scenario 4: "Router won't boot / Stuck in boot loop"

**Solution:** Serial console + emergency restore
1. Connect serial console
2. Watch boot messages for errors
3. If it boots to login, run emergency-lan-restore.sh
4. If it doesn't boot, may need firmware reflash

### Scenario 5: "I have no reset button and no serial adapter"

**Prevention:**
- Don't assign all ports to WAN
- Keep one port as LAN
- Note: Safety monitor should still auto-recover

**Recovery if prevention failed:**
1. Wait 2 minutes for safety monitor
2. Try connecting to different ports
3. Check if any port responds to ping 192.168.2.1
4. If nothing works, you'll need:
   - Serial adapter ($5-10 online), OR
   - Firmware reflash via recovery mode, OR
   - Return to factory defaults via bootloader

## Prevention Tips

### Best Practices

1. **Keep at least one LAN port**
   - Don't assign all ports to WAN
   - Safety monitor will recover, but better to avoid

2. **Never set LAN to DHCP**
   - Always use static IP
   - Safety monitor prevents this, but don't try

3. **Use USB modems for additional WANs**
   - Physical ports can stay as LAN
   - USB modems auto-configure as WAN
   - Better multi-WAN strategy

4. **Test before saving**
   - Make changes in web UI
   - Apply but don't save permanently
   - Test access first
   - Then save if it works

5. **Document your changes**
   - Write down custom IPs
   - Note which ports are WAN/LAN
   - Keep recovery info handy

### Configuration Guidelines

**Safe LAN configuration:**
```
Protocol: static
IP: 192.168.2.1 (or any private IP)
Netmask: 255.255.255.0
Ports: At least one physical port
DHCP: Enabled
```

**Safe WAN configuration:**
```
Maximum: N-1 physical ports (keep one for LAN)
Protocol: DHCP or static (for WAN)
Multipath: on (for bonding)
```

**Safe multi-WAN strategy:**
```
Physical port 1: WAN (ethernet)
Physical ports 2-4: LAN
USB modems: wan2, wan3, etc. (auto-configured)
Result: Can never lock yourself out
```

## Recovery Command Reference

### Via Serial Console or SSH (if accessible)

```bash
# Emergency LAN restore (minimal change)
/usr/bin/emergency-lan-restore.sh

# Full recovery tool (interactive)
omr-recovery

# Check network configuration
uci show network | grep -E "lan|wan"

# Check current LAN IP
uci get network.lan.ipaddr

# Reset LAN to default
uci set network.lan.proto='static'
uci set network.lan.ipaddr='192.168.2.1'
uci set network.lan.netmask='255.255.255.0'
uci commit network
/etc/init.d/network restart

# Check safety monitor status
logread | grep network-safety | tail -20

# Manually trigger port auto-config
rm -f /etc/port-autoconfig-applied
/usr/bin/port-autoconfig.sh
```

## Technical Details

### Reset Button Handler

**File:** `/etc/rc.button/reset`

**How it works:**
- Listens for button press events
- Short press (< 5s): Calls emergency-lan-restore.sh
- Long press (≥ 5s): Factory resets network config
- Uses LED to indicate progress

### Emergency LAN Restore Script

**File:** `/usr/bin/emergency-lan-restore.sh`

**Logic:**
1. Find physical ethernet ports
2. Check which are assigned to WAN
3. Take first port not in WAN
4. If all ports in WAN, take last WAN port
5. Create LAN bridge with that port
6. Set LAN to 192.168.2.1 static
7. Enable DHCP server
8. Restart network

### Safety Monitor

**File:** `/usr/bin/network-safety-monitor.sh`

**Monitoring:**
- Runs continuously (every 30s)
- Checks LAN protocol (must be static)
- Checks for APIPA addresses
- Checks physical ports in LAN
- Triggers recovery if issues detected

**Recovery actions:**
- Force LAN to static if DHCP
- Take back WAN port if no LAN ports
- Fix APIPA situations immediately

## FAQ

**Q: How long does automatic recovery take?**
A: Safety monitor checks every 30 seconds, so up to 60 seconds total.

**Q: Will I lose my WAN configurations?**
A: Short press/emergency restore: No, only one port moved
Long press/factory reset: Yes, all network config reset

**Q: Can I disable the safety monitor?**
A: Not recommended, but: `/etc/init.d/network-safety stop && /etc/init.d/network-safety disable`

**Q: What if reset button doesn't work?**
A: Use serial console or wait for automatic safety monitor recovery.

**Q: Can I change the default LAN IP from 192.168.2.1?**
A: Yes, via web UI. But remember it! If you forget, use reset button.

**Q: Will USB modems prevent lockouts?**
A: Yes! USB modems are WAN, physical ports can stay LAN. Best practice!

**Q: What if I don't have a serial adapter?**
A: $5-10 online (FTDI FT232RL or CP2102). Essential for router tinkering!

**Q: Does reset button affect other settings?**
A: Short press: Only network. Long press: Only network. Everything else preserved.

**Q: How do I test if recovery works?**
A: Assign all ports to WAN, wait 60s, check if you can access 192.168.2.1

## Summary

You have multiple layers of protection:

1. **Automatic (60s):** Safety monitor auto-recovers
2. **Physical (30s):** Reset button short press
3. **Nuclear (60s):** Reset button long press
4. **Manual:** Serial console + emergency script
5. **Advanced:** Serial console + custom commands

**Bottom line:** It's nearly impossible to permanently lock yourself out!
