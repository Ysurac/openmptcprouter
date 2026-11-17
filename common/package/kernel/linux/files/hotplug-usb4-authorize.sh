#!/bin/sh
# No boltd on OpenWrt: approve each new Thunderbolt device, else the
# controller keeps its PCIe tunnel down (security level "user"/"secure").

[ "$ACTION" = add ] || exit 0
[ "$(cat "/sys$DEVPATH/authorized" 2>/dev/null)" = 0 ] || exit 0
echo 1 > "/sys$DEVPATH/authorized" && logger -t thunderbolt "authorized ${DEVPATH##*/}"
