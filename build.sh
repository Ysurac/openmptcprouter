#!/bin/sh
#
# Copyright (C) 2017 OVH OverTheBox
# Copyright (C) 2017-2020 Ycarus (Yannick Chabanois) <ycarus@zugaina.org> for OpenMPTCProuter project
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.
#

set -e

umask 0022
unset GREP_OPTIONS SED

_get_repo() (
	mkdir -p "$1"
	cd "$1"
	[ -d .git ] || git init
	if git remote get-url origin >/dev/null 2>/dev/null; then
		git remote set-url origin "$2"
	else
		git remote add origin "$2"
	fi
	git fetch origin -f
	git fetch origin --tags -f
	git checkout -f "origin/$3" -B "build" 2>/dev/null || git checkout "$3" -B "build"
)

OMR_DIST=${OMR_DIST:-openmptcprouter}
OMR_HOST=${OMR_HOST:-$(curl -sS ifconfig.co)}
OMR_PORT=${OMR_PORT:-80}
OMR_KEEPBIN=${OMR_KEEPBIN:-no}
OMR_IMG=${OMR_IMG:-yes}
#OMR_UEFI=${OMR_UEFI:-yes}
OMR_PACKAGES=${OMR_PACKAGES:-full}
OMR_ALL_PACKAGES=${OMR_ALL_PACKAGES:-no}
OMR_TARGET=${OMR_TARGET:-x86_64}
OMR_TARGET_CONFIG="config-$OMR_TARGET"
UPSTREAM=${UPSTREAM:-no}
OMR_KERNEL=${OMR_KERNEL:-5.4}
SHORTCUT_FE=${SHORTCUT_FE:-no}
#OMR_RELEASE=${OMR_RELEASE:-$(git describe --tags `git rev-list --tags --max-count=1` | sed 's/^\([0-9.]*\).*/\1/')}
#OMR_RELEASE=${OMR_RELEASE:-$(git tag --sort=committerdate | tail -1)}
OMR_RELEASE=${OMR_RELEASE:-$(git describe --tags `git rev-list --tags --max-count=1` | tail -1 | cut -d '-' -f1)}
OMR_REPO=${OMR_REPO:-http://$OMR_HOST:$OMR_PORT/release/$OMR_RELEASE/$OMR_TARGET}

OMR_FEED_URL="${OMR_FEED_URL:-https://github.com/suyuan168/openmptcprouter-feeds}"
OMR_FEED_SRC="${OMR_FEED_SRC:-develop}"

CUSTOM_FEED_URL="${CUSTOM_FEED_URL}"

OMR_OPENWRT=${OMR_OPENWRT:-default}

OMR_FORCE_DSA=${OMR_FORCE_DSA:-0}

if [ ! -f "$OMR_TARGET_CONFIG" ]; then
	echo "Target $OMR_TARGET not found !"
	#exit 1
fi

if [ "$OMR_TARGET" = "rpi3" ]; then
	OMR_REAL_TARGET="aarch64_cortex-a53"
elif [ "$OMR_TARGET" = "rpi4" ]; then
	OMR_REAL_TARGET="aarch64_cortex-a72"
elif [ "$OMR_TARGET" = "rpi2" ]; then
	OMR_REAL_TARGET="arm_cortex-a7_neon-vfpv4"
elif [ "$OMR_TARGET" = "wrt3200acm" ]; then
	OMR_REAL_TARGET="arm_cortex-a9_vfpv3-d16"
elif [ "$OMR_TARGET" = "wrt32x" ]; then
	OMR_REAL_TARGET="arm_cortex-a9_vfpv3-d16"
elif [ "$OMR_TARGET" = "bpi-r1" ]; then
	OMR_REAL_TARGET="arm_cortex-a7_neon-vfpv4"
elif [ "$OMR_TARGET" = "bpi-r2" ]; then
	OMR_REAL_TARGET="arm_cortex-a7_neon-vfpv4"
elif [ "$OMR_TARGET" = "bpi-r64" ]; then
	OMR_REAL_TARGET="aarch64_cortex-a53"
elif [ "$OMR_TARGET" = "espressobin" ]; then
	OMR_REAL_TARGET="aarch64_cortex-a53"
elif [ "$OMR_TARGET" = "x86" ]; then
	OMR_REAL_TARGET="i386_pentium4"
elif [ "$OMR_TARGET" = "r2s" ]; then
	OMR_REAL_TARGET="aarch64_generic"
elif [ "$OMR_TARGET" = "r4s" ]; then
	OMR_REAL_TARGET="aarch64_generic"
elif [ "$OMR_TARGET" = "ubnt-erx" ]; then
	OMR_REAL_TARGET="mipsel_24kc"
else
	OMR_REAL_TARGET=${OMR_TARGET}
fi

#_get_repo source https://github.com/ysurac/openmptcprouter-source "master"
if [ "$OMR_OPENWRT" = "default" ]; then
	_get_repo "$OMR_TARGET/source" https://github.com/openwrt/openwrt "585cef5f1a9c1c3aecd7d231364618e96d03ab65"
	_get_repo feeds/packages https://github.com/openwrt/packages "e2055b5433da245e6ff8fb060d018d036499cf38"
	_get_repo feeds/luci https://github.com/openwrt/luci "7c943a1d6bcf449019ca8a43e800e51f269bb8f6"
elif [ "$OMR_OPENWRT" = "master" ]; then
	_get_repo "$OMR_TARGET/source" https://github.com/openwrt/openwrt "master"
	_get_repo feeds/packages https://github.com/openwrt/packages "master"
	_get_repo feeds/luci https://github.com/openwrt/luci "master"
else
	_get_repo "$OMR_TARGET/source" https://github.com/openwrt/openwrt "${OMR_OPENWRT}"
	_get_repo feeds/packages https://github.com/openwrt/packages "${OMR_OPENWRT}"
	_get_repo feeds/luci https://github.com/openwrt/luci "${OMR_OPENWRT}"
fi

if [ -z "$OMR_FEED" ]; then
	OMR_FEED=feeds/openmptcprouter
	_get_repo "$OMR_FEED" "$OMR_FEED_URL" "$OMR_FEED_SRC"
fi

if [ -n "$CUSTOM_FEED_URL" ] && [ -z "$CUSTOM_FEED" ]; then
	CUSTOM_FEED=feeds/${OMR_DIST}
	_get_repo "$CUSTOM_FEED" "$CUSTOM_FEED_URL" "master"
fi

if [ -n "$1" ] && [ -f "$OMR_FEED/$1/Makefile" ]; then
	OMR_DIST=$1
	shift 1
fi

if [ "$OMR_KEEPBIN" = "no" ]; then 
	rm -rf "$OMR_TARGET/source/bin"
fi
rm -rf "$OMR_TARGET/source/files" "$OMR_TARGET/source/tmp"
#rm -rf "$OMR_TARGET/source/target/linux/mediatek/patches-4.14"
cp -rf root/* "$OMR_TARGET/source"

cat >> "$OMR_TARGET/source/package/base-files/files/etc/banner" <<EOF
-----------------------------------------------------
 PACKAGE:     $OMR_DIST
 VERSION:     $(git -C "$OMR_FEED" tag --sort=committerdate | tail -1)
 TARGET:      $OMR_TARGET
 ARCH:        $OMR_REAL_TARGET

 BUILD REPO:  $(git config --get remote.origin.url)
 BUILD DATE:  $(date -u)
-----------------------------------------------------
EOF

cat > "$OMR_TARGET/source/feeds.conf" <<EOF
src-link packages $(readlink -f feeds/packages)
src-link luci $(readlink -f feeds/luci)
src-link openmptcprouter $(readlink -f "$OMR_FEED")
EOF

if [ -n "$CUSTOM_FEED" ]; then
	echo "src-link ${OMR_DIST} $(readlink -f ${CUSTOM_FEED})" >> "$OMR_TARGET/source/feeds.conf"
fi

if [ "$OMR_DIST" = "openmptcprouter" ]; then
	cat > "$OMR_TARGET/source/package/system/opkg/files/customfeeds.conf" <<-EOF
	src/gz openwrt_luci http://packages.openmptcprouter.com/${OMR_RELEASE}/${OMR_REAL_TARGET}/luci
	src/gz openwrt_packages http://packages.openmptcprouter.com/${OMR_RELEASE}/${OMR_REAL_TARGET}/packages
	src/gz openwrt_base http://packages.openmptcprouter.com/${OMR_RELEASE}/${OMR_REAL_TARGET}/base
	src/gz openwrt_routing http://packages.openmptcprouter.com/${OMR_RELEASE}/${OMR_REAL_TARGET}/routing
	src/gz openwrt_telephony http://packages.openmptcprouter.com/${OMR_RELEASE}/${OMR_REAL_TARGET}/telephony
	EOF
elif [ -n "$OMR_PACKAGES_URL" ]; then
	cat > "$OMR_TARGET/source/package/system/opkg/files/customfeeds.conf" <<-EOF
	src/gz openwrt_luci ${OMR_PACKAGES_URL}/${OMR_RELEASE}/${OMR_REAL_TARGET}/luci
	src/gz openwrt_packages ${OMR_PACKAGES_URL}/${OMR_RELEASE}/${OMR_REAL_TARGET}/packages
	src/gz openwrt_base ${OMR_PACKAGES_URL}/${OMR_RELEASE}/${OMR_REAL_TARGET}/base
	src/gz openwrt_routing ${OMR_PACKAGES_URL}/${OMR_RELEASE}/${OMR_REAL_TARGET}/routing
	src/gz openwrt_telephony ${OMR_PACKAGES_URL}/${OMR_RELEASE}/${OMR_REAL_TARGET}/telephony
	EOF
else
	cat > "$OMR_TARGET/source/package/system/opkg/files/customfeeds.conf" <<-EOF
	src/gz openwrt_luci http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/luci
	src/gz openwrt_packages http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/packages
	src/gz openwrt_base http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/base
	src/gz openwrt_routing http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/routing
	src/gz openwrt_telephony http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/telephony
	EOF
fi
#cat > "$OMR_TARGET/source/package/system/opkg/files/customfeeds.conf" <<EOF
#src/gz openwrt_luci http://downloads.openwrt.org/releases/18.06.0/packages/${OMR_REAL_TARGET}/luci
#src/gz openwrt_packages http://downloads.openwrt.org/releases/18.06.0/packages/${OMR_REAL_TARGET}/packages
#src/gz openwrt_base http://downloads.openwrt.org/releases/18.06.0/packages/${OMR_REAL_TARGET}/base
#src/gz openwrt_routing http://downloads.openwrt.org/releases/18.06.0/packages/${OMR_REAL_TARGET}/routing
#src/gz openwrt_telephony http://downloads.openwrt.org/releases/18.06.0/packages/${OMR_REAL_TARGET}/telephony
#EOF

if [ -f "$OMR_TARGET_CONFIG" ]; then
	cat "$OMR_TARGET_CONFIG" config -> "$OMR_TARGET/source/.config" <<-EOF
	CONFIG_IMAGEOPT=y
	CONFIG_VERSIONOPT=y
	CONFIG_VERSION_DIST="$OMR_DIST"
	CONFIG_VERSION_REPO="$OMR_REPO"
	CONFIG_VERSION_NUMBER="$(git -C "$OMR_FEED" tag --sort=committerdate | tail -1)"
	EOF
else
	cat config -> "$OMR_TARGET/source/.config" <<-EOF
	CONFIG_IMAGEOPT=y
	CONFIG_VERSIONOPT=y
	CONFIG_VERSION_DIST="$OMR_DIST"
	CONFIG_VERSION_REPO="$OMR_REPO"
	CONFIG_VERSION_NUMBER="$(git -C "$OMR_FEED" tag --sort=committerdate | tail -1)-$(git -C "$OMR_FEED" rev-parse --short HEAD)"
	EOF
fi
#if [ "$OMR_KERNEL" = "5.14" ]; then
#	echo 'CONFIG_KERNEL_GIT_CLONE_URI="https://github.com/multipath-tcp/mptcp_net-next.git"' >> "$OMR_TARGET/source/.config"
#	echo 'CONFIG_KERNEL_GIT_REF="78828adaef8fe9b69f9a8c4b60f74b01c5a31c7a"' >> "$OMR_TARGET/source/.config"
#fi
if [ "$OMR_ALL_PACKAGES" = "yes" ]; then
	echo 'CONFIG_ALL=y' >> "$OMR_TARGET/source/.config"
	echo 'CONFIG_ALL_NONSHARED=y' >> "$OMR_TARGET/source/.config"
fi
if [ "$OMR_IMG" = "yes" ] && [ "$OMR_TARGET" = "x86_64" ]; then 
	echo 'CONFIG_VDI_IMAGES=y' >> "$OMR_TARGET/source/.config"
	echo 'CONFIG_VMDK_IMAGES=y' >> "$OMR_TARGET/source/.config"
	echo 'CONFIG_VHDX_IMAGES=y' >> "$OMR_TARGET/source/.config"
fi

if [ "$OMR_PACKAGES" = "full" ]; then
	echo "CONFIG_PACKAGE_${OMR_DIST}-full=y" >> "$OMR_TARGET/source/.config"
fi
if [ "$OMR_PACKAGES" = "mini" ]; then
	echo "CONFIG_PACKAGE_${OMR_DIST}-mini=y" >> "$OMR_TARGET/source/.config"
fi

if [ "$SHORTCUT_FE" = "yes" ] && [ "$OMR_KERNEL" != "5.14" ]; then
	echo "# CONFIG_PACKAGE_kmod-fast-classifier is not set" >> "$OMR_TARGET/source/.config"
	echo "CONFIG_PACKAGE_kmod-fast-classifier-noload=y" >> "$OMR_TARGET/source/.config"
	echo "CONFIG_PACKAGE_kmod-shortcut-fe-cm=y" >> "$OMR_TARGET/source/.config"
	echo "CONFIG_PACKAGE_kmod-shortcut-fe=y" >> "$OMR_TARGET/source/.config"
else
	echo "# CONFIG_PACKAGE_kmod-fast-classifier is not set" >> "$OMR_TARGET/source/.config"
	echo "# CONFIG_PACKAGE_kmod-fast-classifier-noload is not set" >> "$OMR_TARGET/source/.config"
	echo "# CONFIG_PACKAGE_kmod-shortcut-fe-cm is not set" >> "$OMR_TARGET/source/.config"
	echo "# CONFIG_PACKAGE_kmod-shortcut-fe is not set" >> "$OMR_TARGET/source/.config"
fi

if [ "$OMR_TARGET" = "bpi-r1" -a "$OMR_OPENWRT" = "master" ]; then
	# We disable mc in master, because it leads to unknown compilation errors on bpi-r1 target
	# No time to check this, now, cause i am focused on make this target work
	# Maybe someone can do this later	
	echo -n "Disabling error causing midnight commander (mc) package..."
	sed -i "s/CONFIG_PACKAGE_mc=y/# CONFIG_PACKAGE_mc is not set/" "$OMR_TARGET/source/.config"
	sed -i "s/CONFIG_MC_EDITOR=y/# CONFIG_MC_EDITOR is not set/" "$OMR_TARGET/source/.config"
	sed -i "s/CONFIG_MC_SUBSHELL=y/# CONFIG_MC_SUBSHELL is not set/" "$OMR_TARGET/source/.config"
	sed -i "s/CONFIG_MC_CHARSET=y/# CONFIG_MC_CHARSET is not set/" "$OMR_TARGET/source/.config"
	sed -i "s/CONFIG_MC_VFS=y/# CONFIG_MC_VFS is not set/" "$OMR_TARGET/source/.config"	
	echo "done"

	# 2021-03-05 Oliver Welter <oliver@welter.rocks>
fi

if [ "$OMR_TARGET" = "bpi-r1" ]; then
	# Check kernel version
	if [ "$OMR_KERNEL" != "5.4" ]; then
		echo "Sorry, but for now kernel 5.4 is the only supported one."
		exit 1
	fi
	
	# Remove the 310-Revert-ARM-dts-sun7i-Add-BCM53125-switch-nodes-to-th patch
	echo -n "Removing unwanted patches from kernel $OMR_KERNEL..."
	rm -f "$OMR_TARGET/source/target/linux/sunxi/patches-$OMR_KERNEL/310-Revert-ARM-dts-sun7i-Add-BCM53125-switch-nodes-to-th.patch" >/dev/null 2>&1
	echo "done"
	
	if [ "$OMR_FORCE_DSA" = "1" ]; then 
		# Remove support for swconfig
		echo -n "Removing swconfig support from openwrt config..."
		for i in DEFAULT_swconfig PACKAGE_swconfig PACKAGE_kmod-swconfig; do
			sed -i "s/CONFIG_${i}/# CONFIG_${i} is not set/" "$OMR_TARGET/source/.config"
		done
		echo "done"
		echo -n "Removing B53 swconfig support from kernel $OMR_KERNEL..."
		for i in SWCONFIG_B53 SWCONFIG_B53_PHY_DRIVER SWCONFIG_LEDS LED_TRIGGER_PHY SWCONFIG_B53_PHY_FIXUP SWCONFIG_B53_SPI_DRIVER SWCONFIG_B53_MMAP_DRIVER SWCONFIG_B53_SRAB_DRIVER; do
			sed -i "s/CONFIG_${i}/# CONFIG_${i} is not set/" "$OMR_TARGET/source/target/linux/sunxi/config-$OMR_KERNEL"
			sed -i "s/CONFIG_${i}/# CONFIG_${i} is not set/" "$OMR_TARGET/source/target/linux/sunxi/cortexa7/config-$OMR_KERNEL"
		done
		echo "done"

		# Add support for distributed switch architecture
		echo -n "Adding B53 DSA support to kernel $OMR_KERNEL..."		
		for i in B53 B53_MDIO_DRIVER BRIDGE_VLAN_FILTERING MDIO_BUS_MUX_MULTIPLEXER NET_DSA NET_DSA_TAG_8021Q NET_DSA_TAG_BRCM NET_DSA_TAG_BRCM_PREPEND; do
			check_sunxi_config=`grep "CONFIG_${i}=y" "$OMR_TARGET/source/target/linux/sunxi/config-$OMR_KERNEL" || true`
			check_cortexa7_config=`grep "CONFIG_${i}=y" "$OMR_TARGET/source/target/linux/sunxi/cortexa7/config-$OMR_KERNEL" || true`
			
			[ "$check_sunxi_config" = "" -a "$check_cortexa7_config" = "" ] && echo "CONFIG_${i}=y" >> "$OMR_TARGET/source/target/linux/sunxi/cortexa7/config-$OMR_KERNEL"
		done
		echo "done"

		# Create DSA port map file (will be filled on first boot, by uci-defaults and tells the system, that it is in DSA mode)
		touch "$OMR_TARGET/source/target/linux/sunxi/base-files/etc/dsa.map"
		
		# Remove the b53 hack in preinit
		rm -f "$OMR_TARGET/source/target/linux/sunxi/base-files/lib/preinit/03_b53_hack.sh"
	else
		# Remove ip-bridge
		echo -n "Removing ip-bridge support from openwrt config..."
		for i in PACKAGE_ip-bridge; do
			sed -i "s/CONFIG_${i}/# CONFIG_${i} is not set/" "$OMR_TARGET/source/.config"
		done
		echo "done"

		# Remove swconfig parts
		echo -n "Removing unneeded B53 swconfig parts from kernel $OMR_KERNEL..."
		for i in SWCONFIG_B53_PHY_FIXUP SWCONFIG_B53_SPI_DRIVER SWCONFIG_B53_MMAP_DRIVER SWCONFIG_B53_SRAB_DRIVER; do
			sed -i "s/CONFIG_${i}/# CONFIG_${i} is not set/" "$OMR_TARGET/source/target/linux/sunxi/config-$OMR_KERNEL"
			sed -i "s/CONFIG_${i}/# CONFIG_${i} is not set/" "$OMR_TARGET/source/target/linux/sunxi/cortexa7/config-$OMR_KERNEL"
		done
		echo "done"
	fi
		
	# Add led support
	echo -n "Adding LED TRIGGER support to kernel $OMR_KERNEL..."
	if [ "$OMR_FORCE_DSA" != "1" ]; then
		for i in SWCONFIG_LEDS LED_TRIGGER_PHY; do
			check_sunxi_config=`grep "CONFIG_${i}=y" "$OMR_TARGET/source/target/linux/sunxi/config-$OMR_KERNEL" || true`
			check_cortexa7_config=`grep "CONFIG_${i}=y" "$OMR_TARGET/source/target/linux/sunxi/cortexa7/config-$OMR_KERNEL" || true`

			[ "$check_sunxi_config" = "" -a "$check_cortexa7_config" = "" ] && echo "CONFIG_${i}=y" >> "$OMR_TARGET/source/target/linux/sunxi/cortexa7/config-$OMR_KERNEL"
		done
	fi
	for i in TIMER ONESHOT DISK MTD HEARTBEAT BACKLIGHT CPU ACTIVITY GPIO DEFAULT_ON TRANSIENT CAMERA PANIC NETDEV PATTERN AUDIO; do
		check_sunxi_config=`grep "CONFIG_LEDS_TRIGGER_${i}=y" "$OMR_TARGET/source/target/linux/sunxi/config-$OMR_KERNEL" || true`
		check_cortexa7_config=`grep "CONFIG_LEDS_TRIGGER_${i}=y" "$OMR_TARGET/source/target/linux/sunxi/cortexa7/config-$OMR_KERNEL" || true`

		[ "$check_sunxi_config" = "" -a "$check_cortexa7_config" = "" ] && echo "CONFIG_LEDS_TRIGGER_${i}=y" >> "$OMR_TARGET/source/target/linux/sunxi/cortexa7/config-$OMR_KERNEL"
	done
	echo "done"	
fi

cd "$OMR_TARGET/source"

#if [ "$OMR_UEFI" = "yes" ] && [ "$OMR_TARGET" = "x86_64" ]; then 
#	echo "Checking if UEFI patch is set or not"
#	if [ "$(grep 'EFI_IMAGES' target/linux/x86/image/Makefile)" = "" ]; then
#		patch -N -p1 -s < ../../patches/uefi.patch
#	fi
#	echo "Done"
#else
#	if [ "$(grep 'EFI_IMAGES' target/linux/x86/image/Makefile)" != "" ]; then
#		patch -N -R -p1 -s < ../../patches/uefi.patch
#	fi
#fi

#if [ "$OMR_TARGET" = "x86_64" ]; then 
#	echo "Checking if Hyper-V patch is set or not"
#	if ! patch -Rf -N -p1 -s --dry-run < ../../patches/images.patch; then
#		patch -N -p1 -s < ../../patches/images.patch
#	fi
#	echo "Done"
#fi

echo "Checking if No check patch is set or not"
if ! patch -Rf -N -p1 -s --dry-run < ../../patches/nocheck.patch; then
	echo "apply..."
	patch -N -p1 -s < ../../patches/nocheck.patch
fi
echo "Done"

echo "Checking if Nanqinlang patch is set or not"
if ! patch -Rf -N -p1 -s --dry-run < ../../patches/nanqinlang.patch; then
	echo "apply..."
	patch -N -p1 -s < ../../patches/nanqinlang.patch
fi
echo "Done"

#echo "Checking if remove_abi patch is set or not"
#if ! patch -Rf -N -p1 -s --dry-run < ../../patches/remove_abi.patch; then
#	echo "apply..."
#	patch -N -p1 -s < ../../patches/remove_abi.patch
#fi
#echo "Done"

# Add BBR2 patch, only working on 64bits images for now
if [ "$OMR_KERNEL" != "5.14" ] && ([ "$OMR_TARGET" = "x86_64" ] || [ "$OMR_TARGET" = "bpi-r64" ] || [ "$OMR_TARGET" = "rpi4" ] || [ "$OMR_TARGET" = "espressobin" ] || [ "$OMR_TARGET" = "r2s" ] || [ "$OMR_TARGET" = "r4s" ] || [ "$OMR_TARGET" = "rpi3" ]); then
	echo "Checking if BBRv2 patch is set or not"
	if ! patch -Rf -N -p1 -s --dry-run < ../../patches/bbr2.patch; then
		echo "apply..."
		patch -N -p1 -s < ../../patches/bbr2.patch
	fi
	echo "Done"
fi

echo "Checking if smsc75xx patch is set or not"
if ! patch -Rf -N -p1 -s --dry-run < ../../patches/smsc75xx.patch; then
	echo "apply..."
	patch -N -p1 -s < ../../patches/smsc75xx.patch
fi
echo "Done"

#echo "Checking if ipt-nat patch is set or not"
#if ! patch -Rf -N -p1 -s --dry-run < ../../patches/ipt-nat6.patch; then
#	echo "apply..."
#	patch -N -p1 -s < ../../patches/ipt-nat6.patch
#fi
#echo "Done"

#echo "Checking if mvebu patch is set or not"
#if [ ! -d target/linux/mvebu/patches-5.4 ]; then
#	echo "apply..."
#	patch -N -p1 -s < ../../patches/mvebu-5.14.patch
#fi
#echo "Done"

#echo "Checking if opkg install arguement too long patch is set or not"
#if ! patch -Rf -N -p1 -s --dry-run < ../../patches/package-too-long.patch; then
#	echo "apply..."
#	patch -N -p1 -s < ../../patches/package-too-long.patch
#fi
#echo "Done"

echo "Download via IPv4"
if ! patch -Rf -N -p1 -s --dry-run < ../../patches/download-ipv4.patch; then
	patch -N -p1 -s < ../../patches/download-ipv4.patch
fi
echo "Done"

#echo "Remove check rsync"
#if [ "$(grep rsync include/prereq-build.mk)" != "" ]; then
#	patch -N -p1 -s < ../../patches/check-rsync.patch
#fi
#echo "Done"

if [ -f target/linux/mediatek/patches-5.4/0999-hnat.patch ]; then
	rm -f target/linux/mediatek/patches-5.4/0999-hnat.patch
fi

if [ -f target/linux/ipq40xx/patches-5.4/100-GPIO-add-named-gpio-exports.patch ]; then
	rm -f target/linux/ipq40xx/patches-5.4/100-GPIO-add-named-gpio-exports.patch
fi

if [ -f package/boot/uboot-rockchip/patches/100-rockchip-rk3328-Add-support-for-FriendlyARM-NanoPi-R.patch ]; then
	rm -f package/boot/uboot-rockchip/patches/100-rockchip-rk3328-Add-support-for-FriendlyARM-NanoPi-R.patch
fi

#echo "Patch protobuf wrong hash"
#patch -N -R -p1 -s < ../../patches/protobuf_hash.patch
#echo "Done"

#echo "Remove gtime dependency"
#if ! patch -Rf -N -p1 -s --dry-run < ../../patches/gtime.patch; then
#	patch -N -p1 -s < ../../patches/gtime.patch
#fi
#echo "Done"

#if [ -f target/linux/generic/backport-5.4/370-netfilter-nf_flow_table-fix-offloaded-connection-tim.patch ]; then
#	rm -f target/linux/generic/backport-5.4/370-netfilter-nf_flow_table-fix-offloaded-connection-tim.patch
#fi
#if [ -f target/linux/generic/pending-5.4/640-netfilter-nf_flow_table-add-hardware-offload-support.patch ]; then
#	rm -f target/linux/generic/pending-5.4/640-netfilter-nf_flow_table-add-hardware-offload-support.patch
#fi
#if [ -f target/linux/generic/pending-5.4/641-netfilter-nf_flow_table-support-hw-offload-through-v.patch ]; then
#	rm -f target/linux/generic/pending-5.4/641-netfilter-nf_flow_table-support-hw-offload-through-v.patch
#fi
#if [ -f target/linux/generic/pending-5.4/642-net-8021q-support-hardware-flow-table-offload.patch ]; then
#	rm -f target/linux/generic/pending-5.4/642-net-8021q-support-hardware-flow-table-offload.patch
#fi
#if [ -f target/linux/generic/pending-5.4/643-net-bridge-support-hardware-flow-table-offload.patch ]; then
#	rm -f target/linux/generic/pending-5.4/643-net-bridge-support-hardware-flow-table-offload.patch
#fi
#if [ -f target/linux/generic/pending-5.4/644-net-pppoe-support-hardware-flow-table-offload.patch ]; then
#	rm -f target/linux/generic/pending-5.4/644-net-pppoe-support-hardware-flow-table-offload.patch
#fi
#if [ -f target/linux/generic/pending-5.4/645-netfilter-nf_flow_table-rework-hardware-offload-time.patch ]; then
#	rm -f target/linux/generic/pending-5.4/645-netfilter-nf_flow_table-rework-hardware-offload-time.patch
#fi
#if [ -f target/linux/generic/pending-5.4/647-net-dsa-support-hardware-flow-table-offload.patch ]; then
#	rm -f target/linux/generic/pending-5.4/647-net-dsa-support-hardware-flow-table-offload.patch
#fi
#if [ -f target/linux/generic/hack-5.4/650-netfilter-add-xt_OFFLOAD-target.patch ]; then
#	rm -f target/linux/generic/hack-5.4/650-netfilter-add-xt_OFFLOAD-target.patch
#fi
#if [ -f target/linux/generic/pending-5.4/690-net-add-support-for-threaded-NAPI-polling.patch ]; then
#	rm -f target/linux/generic/pending-5.4/690-net-add-support-for-threaded-NAPI-polling.patch
#fi
#if [ -f target/linux/generic/hack-5.4/647-netfilter-flow-acct.patch ]; then
#	rm -f target/linux/generic/hack-5.4/647-netfilter-flow-acct.patch
#fi
#if [ -f target/linux/generic/hack-5.4/953-net-patch-linux-kernel-to-support-shortcut-fe.patch ]; then
#	rm -f target/linux/generic/hack-5.4/953-net-patch-linux-kernel-to-support-shortcut-fe.patch
#fi
#if [ -f target/linux/bcm27xx/patches-5.4/950-1031-net-lan78xx-Ack-pending-PHY-ints-when-resetting.patch ]; then
#	rm -f target/linux/bcm27xx/patches-5.4/950-1031-net-lan78xx-Ack-pending-PHY-ints-when-resetting.patch
#fi
#if [ -f target/linux/generic/pending-5.4/770-16-net-ethernet-mediatek-mtk_eth_soc-add-flow-offloadin.patch ]; then
#	rm -f target/linux/generic/pending-5.4/770-16-net-ethernet-mediatek-mtk_eth_soc-add-flow-offloadin.patch
#fi

if [ "$OMR_KERNEL" = "5.4" ]; then
	echo "Set to kernel 5.4 for rpi arch"
	find target/linux/bcm27xx -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER=5.10%KERNEL_PATCHVER:=5.4%g' {} \;
	find target/linux/bcm27xx -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.10%KERNEL_PATCHVER:=5.4%g' {} \;
	echo "Done"
	echo "Set to kernel 5.4 for x86 arch"
	find target/linux/x86 -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.10%KERNEL_PATCHVER:=5.4%g' {} \;
	echo "Done"
	echo "Set to kernel 5.4 for mvebu arch (WRT)"
	find target/linux/mvebu -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.10%KERNEL_PATCHVER:=5.4%g' {} \;
	echo "Done"
	echo "Set to kernel 5.4 for mediatek arch (BPI-R2)"
	find target/linux/mediatek -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.10%KERNEL_PATCHVER:=5.4%g' {} \;
	echo "Done"
fi
if [ "$OMR_KERNEL" = "5.10" ]; then
	echo "Set to kernel 5.10 for rpi arch"
	find target/linux/bcm27xx -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.4%KERNEL_PATCHVER:=5.10%g' {} \;
	find target/linux/bcm27xx -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER=5.4%KERNEL_PATCHVER:=5.10%g' {} \;
	echo "Done"
	echo "Set to kernel 5.10 for x86 arch"
	find target/linux/x86 -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.4%KERNEL_PATCHVER:=5.10%g' {} \;
	echo "Done"
	echo "Set to kernel 5.10 for mvebu arch (WRT)"
	find target/linux/mvebu -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.4%KERNEL_PATCHVER:=5.10%g' {} \;
	echo "Done"
	echo "Set to kernel 5.10 for mediatek arch (BPI-R2)"
	find target/linux/mediatek -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.4%KERNEL_PATCHVER:=5.10%g' {} \;
	echo "Done"
fi
if [ "$OMR_KERNEL" = "5.14" ]; then
	echo "Set to kernel 5.14 for rpi arch"
	find target/linux/bcm27xx -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.10%KERNEL_PATCHVER:=5.14%g' {} \;
	find target/linux/bcm27xx -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER=5.10%KERNEL_PATCHVER:=5.14%g' {} \;
	echo "Done"
	echo "Set to kernel 5.14 for x86 arch"
	find target/linux/x86 -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.10%KERNEL_PATCHVER:=5.14%g' {} \;
	echo "Done"
	echo "Set to kernel 5.14 for mvebu arch (WRT)"
	find target/linux/mvebu -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.10%KERNEL_PATCHVER:=5.14%g' {} \;
	echo "Done"
	echo "Set to kernel 5.14 for mediatek arch (BPI-R2)"
	find target/linux/mediatek -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.10%KERNEL_PATCHVER:=5.14%g' {} \;
	echo "Done"
	echo "Set to kernel 5.14 for rockchip arch (R2S/R4S)"
	find target/linux/rockchip -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.4%KERNEL_PATCHVER:=5.14%g' {} \;
	echo "Done"
	echo "Set to kernel 5.14 for ramips"
	find target/linux/ramips -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=5.4%KERNEL_PATCHVER:=5.14%g' {} \;
	echo "Done"
	rm -rf /target/linux/generic/files/drivers/net/phy/b53
fi

#rm -rf feeds/packages/libs/libwebp
cd "../.."
rm -rf feeds/luci/modules/luci-mod-network
[ -d feeds/${OMR_DIST}/luci-mod-status ] && rm -rf feeds/luci/modules/luci-mod-status
[ -d feeds/${OMR_DIST}/luci-app-statistics ] && rm -rf feeds/luci/applications/luci-app-statistics
[ -d feeds/${OMR_DIST}/luci-proto-modemmanager ] && rm -rf feeds/luci/protocols/luci-proto-modemmanager

echo "Add Occitan translation support"
if ! patch -Rf -N -p1 -s --dry-run < patches/luci-occitan.patch; then
	patch -N -p1 -s < patches/luci-occitan.patch
	#sh feeds/luci/build/i18n-add-language.sh oc
fi
[ -d $OMR_FEED/luci-base/po/oc ] && cp -rf $OMR_FEED/luci-base/po/oc feeds/luci/modules/luci-base/po/
echo "Done"

cd "$OMR_TARGET/source"
echo "Update feeds index"
cp .config .config.keep
scripts/feeds clean
scripts/feeds update -a
scripts/feeds install -a

#cd -
#echo "Checking if fullconenat-luci patch is set or not"
##if ! patch -Rf -N -p1 -s --dry-run < patches/fullconenat-luci.patch; then
#	echo "apply..."
#	patch -N -p1 -s < patches/fullconenat-luci.patch
#fi
#echo "Done"
#cd "$OMR_TARGET/source"

if [ "$OMR_ALL_PACKAGES" = "yes" ]; then
	scripts/feeds install -a -d m -p packages
	scripts/feeds install -a -d m -p luci
fi
if [ -n "$CUSTOM_FEED" ]; then
	scripts/feeds install -a -d m -p openmptcprouter
	scripts/feeds install -a -d y -f -p ${OMR_DIST}
else
	scripts/feeds install -a -d y -f -p openmptcprouter
fi
cp .config.keep .config
scripts/feeds install kmod-macremapper
echo "Done"

if [ ! -f "../../$OMR_TARGET_CONFIG" ]; then
	echo "Target $OMR_TARGET not found ! You have to configure and compile your kernel manually."
	exit 1
fi

echo "Building $OMR_DIST for the target $OMR_TARGET with kernel $OMR_KERNEL"
make defconfig
make IGNORE_ERRORS=m "$@"
echo "Done"