#!/bin/sh

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
OMR_PORT=${OMR_PORT:-8000}
OMR_REPO=${OMR_REPO:-http://$OMR_HOST:$OMR_PORT/release/$OMR_KERNEL}
OMR_KEEPBIN=${OMR_KEEPBIN:-no}
OMR_IMG=${OMR_IMG:-yes}
#OMR_UEFI=${OMR_UEFI:-yes}
OMR_ALL_PACKAGES=${OMR_ALL_PACKAGES:-no}
OMR_TARGET=${OMR_TARGET:-x86_64}
OMR_TARGET_CONFIG="config-$OMR_TARGET"
OMR_KERNEL=${OMR_KERNEL:-5.4}

OMR_FEED_URL="${OMR_FEED_URL:-https://github.com/ysurac/openmptcprouter-feeds}"
OMR_FEED_SRC="${OMR_FEED_SRC:-develop}"

OMR_OPENWRT=${OMR_OPENWRT:-default}

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
	OMR_REAL_TARGET="arm_cortex-a9_vfpv3"
elif [ "$OMR_TARGET" = "wrt32x" ]; then
	OMR_REAL_TARGET="arm_cortex-a9_vfpv3"
elif [ "$OMR_TARGET" = "bpi-r2" ]; then
	OMR_REAL_TARGET="arm_cortex-a7_neon-vfpv4"
elif [ "$OMR_TARGET" = "x86" ]; then
	OMR_REAL_TARGET="i386_pentium4"
else
	OMR_REAL_TARGET=${OMR_TARGET}
fi

#_get_repo source https://github.com/ysurac/openmptcprouter-source "master"
if [ "$OMR_OPENWRT" = "default" ]; then
	_get_repo "$OMR_TARGET/source" https://github.com/openwrt/openwrt "eb17ee294ca8e2b59051a4337779af7a3f4812d5"
	_get_repo feeds/packages https://github.com/openwrt/packages "a22fff844db6927bbe77570d21a7e08d5f5424f3"
	_get_repo feeds/luci https://github.com/openwrt/luci "750e6c1df9624198f4f11e0675de428ec33fd564"
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
 VERSION:     $(git -C "$OMR_FEED" describe --tag --always)

 BUILD REPO:  $(git config --get remote.origin.url)
 BUILD DATE:  $(date -u)
-----------------------------------------------------
EOF

cat > "$OMR_TARGET/source/feeds.conf" <<EOF
src-link packages $(readlink -f feeds/packages)
src-link luci $(readlink -f feeds/luci)
src-link openmptcprouter $(readlink -f "$OMR_FEED")
EOF

cat > "$OMR_TARGET/source/package/system/opkg/files/customfeeds.conf" <<EOF
src/gz openwrt_luci http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/luci
src/gz openwrt_packages http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/packages
src/gz openwrt_base http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/base
src/gz openwrt_routing http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/routing
src/gz openwrt_telephony http://downloads.openwrt.org/snapshots/packages/${OMR_REAL_TARGET}/telephony
EOF
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
	CONFIG_VERSION_NUMBER="$(git -C "$OMR_FEED" describe --tag --always)"
	CONFIG_PACKAGE_${OMR_DIST}-full=y
	EOF
else
	cat config -> "$OMR_TARGET/source/.config" <<-EOF
	CONFIG_IMAGEOPT=y
	CONFIG_VERSIONOPT=y
	CONFIG_VERSION_DIST="$OMR_DIST"
	CONFIG_VERSION_REPO="$OMR_REPO"
	CONFIG_VERSION_NUMBER="$(git -C "$OMR_FEED" describe --tag --always)"
	CONFIG_PACKAGE_${OMR_DIST}-full=y
	EOF
fi
if [ "$OMR_ALL_PACKAGES" = "yes" ]; then
	echo 'CONFIG_ALL=y' >> "$OMR_TARGET/source/.config"
	echo 'CONFIG_ALL_NONSHARED=y' >> "$OMR_TARGET/source/.config"
fi
if [ "$OMR_IMG" = "yes" ] && [ "$OMR_TARGET" = "x86_64" ]; then 
	echo 'CONFIG_VDI_IMAGES=y' >> "$OMR_TARGET/source/.config"
	echo 'CONFIG_VMDK_IMAGES=y' >> "$OMR_TARGET/source/.config"
	echo 'CONFIG_VHDX_IMAGES=y' >> "$OMR_TARGET/source/.config"
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

echo "Checking if opkg install arguement too long patch is set or not"
if ! patch -Rf -N -p1 -s --dry-run < ../../patches/package-too-long.patch; then
	echo "apply..."
	patch -N -p1 -s < ../../patches/package-too-long.patch
fi
echo "Done"

echo "Download via IPv4"
if ! patch -Rf -N -p1 -s --dry-run < ../../patches/download-ipv4.patch; then
	patch -N -p1 -s < ../../patches/download-ipv4.patch
fi
echo "Done"

if [ -f target/linux/mediatek/patches-5.4/0999-hnat.patch ]; then
	rm -f target/linux/mediatek/patches-5.4/0999-hnat.patch
fi

#echo "Patch protobuf wrong hash"
#patch -N -R -p1 -s < ../../patches/protobuf_hash.patch
#echo "Done"

#echo "Remove gtime dependency"
#if ! patch -Rf -N -p1 -s --dry-run < ../../patches/gtime.patch; then
#	patch -N -p1 -s < ../../patches/gtime.patch
#fi
#echo "Done"

if [ "$OMR_KERNEL" = "5.4" ]; then
	echo "Set to kernel 5.4 for rpi arch"
	find target/linux/bcm27xx -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=4.19%KERNEL_PATCHVER:=5.4%g' {} \;
	echo "Done"
	echo "Set to kernel 5.4 for x86 arch"
	find target/linux/x86 -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=4.19%KERNEL_PATCHVER:=5.4%g' {} \;
	echo "Done"
	echo "Set to kernel 5.4 for mvebu arch (WRT)"
	find target/linux/mvebu -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=4.19%KERNEL_PATCHVER:=5.4%g' {} \;
	echo "Done"
	echo "Set to kernel 5.4 for mediatek arch (BPI-R2)"
	find target/linux/mediatek -type f -name Makefile -exec sed -i 's%KERNEL_PATCHVER:=4.19%KERNEL_PATCHVER:=5.4%g' {} \;
	echo "Done"
fi

# Remove patch that can make BPI-R2 slow
rm -rf target/linux/mediatek/patches-4.14/0027-*.patch

echo "Update feeds index"
cp .config .config.keep
scripts/feeds clean
scripts/feeds update -a

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
scripts/feeds install -a -d y -f -p openmptcprouter
cp .config.keep .config
echo "Done"

if [ ! -f "../../$OMR_TARGET_CONFIG" ]; then
	echo "Target $OMR_TARGET not found ! You have to configure and compile your kernel manually."
	exit 1
fi

echo "Building $OMR_DIST for the target $OMR_TARGET"
make defconfig
make IGNORE_ERRORS=m "$@"
echo "Done"
