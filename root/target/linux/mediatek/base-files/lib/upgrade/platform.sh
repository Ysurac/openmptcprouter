platform_do_upgrade() {
	local board=$(board_name)
	case "$board" in
	"unielec,u7623"*)
		#Keep the persisten random mac address (if it exists)
		mkdir -p /tmp/recovery
		mount -o rw,noatime /dev/mmcblk0p1 /tmp/recovery
		[ -f "/tmp/recovery/mac_addr" ] && \
			mv -f /tmp/recovery/mac_addr /tmp/
		umount /tmp/recovery

		#1310720 is the offset in bytes from the start of eMMC and to
		#the location of the kernel (2560 512 byte sectors)
		get_image "$1" | dd of=/dev/mmcblk0 bs=1310720 seek=1 conv=fsync

		mount -o rw,noatime /dev/mmcblk0p1 /tmp/recovery
		[ -f "/tmp/mac_addr" ] && mv -f /tmp/mac_addr /tmp/recovery
		sync
		umount /tmp/recovery
		;;
	bananapi,bpi-r2)
		local tar_file="$1"

		echo "flashing kernel"
		tar xf $tar_file sysupgrade-7623n-bananapi-bpi-r2/kernel -O | mtd write - kernel

		echo "flashing rootfs"
		tar xf $tar_file sysupgrade-7623n-bananapi-bpi-r2/root -O | mtd write - rootfs
		;;
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}

PART_NAME=firmware

platform_check_image() {
	local board=$(board_name)

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	bananapi,bpi-r2)
		local tar_file="$1"
		local kernel_length=`(tar xf $tar_file sysupgrade-7623n-bananapi-bpi-r2/kernel -O | wc -c) 2> /dev/null`
		local rootfs_length=`(tar xf $tar_file sysupgrade-7623n-bananapi-bpi-r2/root -O | wc -c) 2> /dev/null`
		[ "$kernel_length" = 0 -o "$rootfs_length" = 0 ] && {
			echo "The upgrade image is corrupt."
			return 1
		}
		;;
	"unielec,u7623"*)
		local magic="$(get_magic_long "$1")"
		[ "$magic" != "27051956" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;

	*)
		echo "Sysupgrade is not supported on your board yet."
		return 1
		;;
	esac

	return 0
}

platform_copy_config_emmc() {
	mkdir -p /recovery
	mount -o rw,noatime /dev/mmcblk0p1 /recovery
	cp -af "$CONF_TAR" /recovery/
	sync
	umount /recovery
}

platform_copy_config() {
	case "$(board_name)" in
	"unielec,u7623"*)
		platform_copy_config_emmc
		;;
	esac
}
