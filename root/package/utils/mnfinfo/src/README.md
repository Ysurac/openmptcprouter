# libmnfinfo

How it works: depending on the platform, selected in the menuconfig, a different platform flag is defined during compilation. Depending on that flag, a header file with platform-specific MTD field defines is included. Here are all the defines, (currently) used for these headers:

* `MAX_SIM_ID` - maximum allowed SIM index on the platform.
* `MTD_CONFIG_RO` - absolute path of a read-only MTD partition. Used for most field reads.
* `MTD_CONFIG_RW` - absolute path of a read/write MTD partition. Used for SIM PIN value reads/writes.
* `MAC_OFFSET` - MAC field offset in the `MTD_CONFIG_RO` partition.
* `MAC_LENGTH` - MAC field length in the `MTD_CONFIG_RO` partition.
* `NAME_OFFSET` - product name field offset in the `MTD_CONFIG_RO` partition.
* `NAME_LENGTH` - product name field length in the `MTD_CONFIG_RO` partition.
* `WPS_OFFSET` - WPS field offset in the `MTD_CONFIG_RO` partition.
* `WPS_LENGTH` - WPS field length in the `MTD_CONFIG_RO` partition.
* `SERIAL_OFFSET` - product serial code field offset in the `MTD_CONFIG_RO` partition.
* `SERIAL_LENGTH` - product serial code field length in the `MTD_CONFIG_RO` partition.
* `BATCH_OFFSET` - batch index field offset in the `MTD_CONFIG_RO` partition.
* `BATCH_LENGTH` - batch index field length in the `MTD_CONFIG_RO` partition.
* `HWVER_OFFSET` - hardware version index field offset in the `MTD_CONFIG_RO` partition.
* `HWVER_LENGTH` - hardware version index field length in the `MTD_CONFIG_RO` partition.
* `SIMPIN_OFFSET` - SIM card PIN code field offset in the `MTD_CONFIG_RW` partition.
* `SIMPIN_LENGTH` - SIM card PIN code field length in the `MTD_CONFIG_RW` partition.
* `WIFI_OFFSET` - Wi-Fi password field offset in the `MTD_CONFIG_RO` partition.
* `WIFI_LENGTH` - Wi-Fi password field length in the `MTD_CONFIG_RO` partition.

Only defines with actual values should be written. All other defines should be removed from the platform-specific header file.
