# USA Carrier APN Configuration

This document describes how to configure carrier-specific APNs for cellular modems in OpenMPTCProuter.

## Overview

OpenMPTCProuter now supports automatic APN configuration for major USA cellular carriers and MVNOs. The system includes a comprehensive database of carrier APNs and provides tools to easily configure your modem to use the correct settings.

## Quick Start

### Set Your Carrier Globally

To configure all modems to use a specific carrier's APN:

```bash
set-carrier verizon
```

### Set Carrier for Specific Interface

To configure a specific modem interface (e.g., wan1):

```bash
set-carrier -i wan1 tmobile
```

### List Available Carriers

To see all supported carriers:

```bash
set-carrier --list
```

### View Current Configuration

```bash
set-carrier --show
```

## Supported Carriers

### Major Carriers

- **Verizon**: `verizon`, `verizon_lte`, `verizon_prepaid`, `verizon_business`
- **AT&T**: `att`, `att_lte`, `att_prepaid`, `att_reseller`, `att_isp`, `att_hotspot`
- **T-Mobile**: `tmobile`, `tmobile_lte`, `tmobile_prepaid`, `tmobile_business`, `tmobile_iot`
- **Sprint**: `sprint`, `sprint_lte` (now merged with T-Mobile)
- **US Cellular**: `uscellular`, `uscellular_lte`

### MVNOs (Mobile Virtual Network Operators)

#### Verizon Network MVNOs
- **Visible**: `visible`
- **Xfinity Mobile**: `xfinity`
- **Spectrum Mobile**: `spectrum`
- **Total Wireless**: `totalwireless`
- **Optimum Mobile**: `optimum`
- **Straight Talk** (Verizon): `straighttalk_verizon`
- **TracFone** (Verizon): `tracfone_verizon`
- **Net10** (Verizon): `net10_verizon`
- **Red Pocket** (Verizon): `redpocket_verizon`
- **Ting** (Verizon): `ting_verizon`
- **Twigby** (Verizon): `twigby_verizon`
- **US Mobile** (Super LTE): `usmobile_super_lte`

#### AT&T Network MVNOs
- **Cricket Wireless**: `cricket`, `cricket_lte`
- **H2O Wireless**: `h2o`
- **Consumer Cellular** (AT&T): `consumer_att`
- **Wing**: `wing`
- **Straight Talk** (AT&T): `straighttalk_att`
- **TracFone** (AT&T): `tracfone_att`
- **Net10** (AT&T): `net10_att`
- **Red Pocket** (AT&T): `redpocket_att`

#### T-Mobile Network MVNOs
- **Metro by T-Mobile**: `metro`, `metro_lte`
- **Mint Mobile**: `mint`, `mint_lte`
- **Boost Mobile**: `boost`, `boost_lte`
- **Simple Mobile**: `simplemobile`
- **Tello**: `tello`
- **Ultra Mobile**: `ultramobile`
- **Lycamobile**: `lycamobile`
- **Republic Wireless**: `republic`
- **Consumer Cellular** (T-Mobile): `consumer_tmobile`
- **Ting** (T-Mobile): `ting_tmobile`
- **Straight Talk** (T-Mobile): `straighttalk_tmobile`
- **TracFone** (T-Mobile): `tracfone_tmobile`
- **Net10** (T-Mobile): `net10_tmobile`
- **Red Pocket** (T-Mobile): `redpocket_tmobile`
- **Twigby** (T-Mobile): `twigby_tmobile`
- **FreedomPop** (T-Mobile): `freedompop_tmobile`
- **US Mobile** (GSM): `usmobile_gsm`

#### Multi-Network
- **Google Fi**: `googlefi`, `googlefi_lte`
- **FreedomPop** (AT&T): `freedompop_att`

## Configuration Methods

### Method 1: Using set-carrier Command (Recommended)

The `set-carrier` command is the easiest way to configure your carrier:

```bash
# Set carrier globally
set-carrier verizon

# Set carrier for specific interface
set-carrier -i wan1 att

# List all carriers
set-carrier --list

# Show current settings
set-carrier --show
```

After setting the carrier, apply the changes:

```bash
/etc/init.d/network reload
```

Or reconfigure modems:

```bash
/usr/bin/usb-modem-autoconfig.sh
```

### Method 2: Using UCI Directly

For advanced users, you can configure carriers using UCI commands:

#### Set Global Carrier

```bash
uci set network.globals.carrier='tmobile'
uci commit network
/etc/init.d/network reload
```

#### Set Per-Interface Carrier

```bash
uci set network.wan1.carrier='att'
uci commit network
/etc/init.d/network reload
```

### Method 3: Using Environment Variable

You can also set the carrier using an environment variable before running the modem auto-configuration:

```bash
USA_CARRIER=verizon /usr/bin/usb-modem-autoconfig.sh
```

## Priority Order

The system checks for carrier configuration in the following order:

1. **Per-interface setting**: `uci get network.wan1.carrier`
2. **Global setting**: `uci get network.globals.carrier`
3. **Environment variable**: `$USA_CARRIER`
4. **Default**: If no carrier is specified, uses generic APN "internet"

## How It Works

### Automatic Configuration

When a USB modem is detected, the `usb-modem-autoconfig.sh` script:

1. Detects the modem (QMI or MBIM protocol)
2. Checks for carrier configuration
3. Looks up the carrier in the USA carrier APN database (`/etc/usa-carrier-apns.conf`)
4. Configures the modem with the correct APN, username, password, and auth type
5. Creates the network interface and brings it up

### APN Database

The carrier APN database is stored in `/etc/usa-carrier-apns.conf` and includes:

- **APN name**: The access point name for data connections
- **Username**: Authentication username (if required)
- **Password**: Authentication password (if required)
- **Auth type**: Authentication type (PAP, CHAP, or both)

## Troubleshooting

### Check Modem Status

View configured modems and their settings:

```bash
set-carrier --show
```

Or check UCI configuration directly:

```bash
uci show network | grep -E '(wan|carrier|apn)'
```

### View Modem Logs

Check the system log for modem configuration messages:

```bash
logread | grep usb-modem-autoconfig
```

### Test Carrier APN

You can test if a carrier APN is in the database:

```bash
. /etc/usa-carrier-apns.conf
get_carrier_apn verizon
```

### Manual APN Configuration

If your carrier is not in the database or you need custom settings, you can manually configure the APN:

```bash
uci set network.wan1.apn='custom.apn.name'
uci set network.wan1.username='myusername'
uci set network.wan1.password='mypassword'
uci set network.wan1.auth='PAP'
uci commit network
/etc/init.d/network reload
```

### Reconfigure Modems

If you change carrier settings, reconfigure modems:

```bash
/usr/bin/usb-modem-autoconfig.sh
```

## Adding New Carriers

To add a new carrier to the database, edit `/etc/usa-carrier-apns.conf`:

```bash
# Add new carrier
USA_CARRIER_APNS["mycarrier"]="custom.apn:username:password:CHAP"
```

Format: `APN:USERNAME:PASSWORD:AUTH_TYPE`

- Leave username/password empty if not required
- AUTH_TYPE can be: PAP, CHAP, or both

Example:
```bash
USA_CARRIER_APNS["mycarrier"]="internet.mycell.com:::PAP"
```

## Network Reload

After any configuration changes, reload the network:

```bash
/etc/init.d/network reload
```

## Examples

### Example 1: Verizon Modem

```bash
# Set carrier
set-carrier verizon

# Reload network
/etc/init.d/network reload

# Check configuration
set-carrier --show
```

Result: Modem configured with APN "vzwinternet" using PAP authentication.

### Example 2: Multiple Modems with Different Carriers

```bash
# First modem on T-Mobile
set-carrier -i wan1 tmobile

# Second modem on Verizon
set-carrier -i wan2 verizon

# Apply changes
/etc/init.d/network reload
```

### Example 3: AT&T Prepaid

```bash
set-carrier att_prepaid
/etc/init.d/network reload
```

Result: Modem configured with APN "phone" using CHAP authentication.

## Security Notes

- APN credentials are stored in UCI configuration
- Configuration files have restricted permissions (umask 077)
- Modem status files are stored in `/var/run/modem-status/` with secure permissions
- All input is validated to prevent command injection

## Support

For issues or questions:

1. Check logs: `logread | grep usb-modem-autoconfig`
2. Verify carrier database: `set-carrier --list`
3. Test configuration: `set-carrier --show`
4. Check modem status: `ls -l /var/run/modem-status/`

## File Locations

- Carrier database: `/etc/usa-carrier-apns.conf`
- Configuration script: `/usr/bin/usb-modem-autoconfig.sh`
- Helper command: `/usr/bin/set-carrier`
- Documentation: `/etc/usa-carrier-apns.md`
- Modem status: `/var/run/modem-status/`
