#!/bin/sh

set -e

umask 0022
unset GREP_OPTIONS SED

_get_repo() {
	git clone "$2" "$1" 2>/dev/null || true
	git -C "$1" remote set-url origin "$2"
	git -C "$1" fetch origin
	git -C "$1" fetch origin --tags
	git -C "$1" checkout "origin/$3" -B "build" || git -C "$1" checkout "$3" -B "build"
}

OMR_DIST=${OMR_DIST:-omr}
OMR_HOST=${OMR_HOST:-$(curl -sS ipaddr.ovh)}
OMR_PORT=${OMR_PORT:-8000}
OMR_REPO=${OMR_REPO:-http://$OMR_HOST:$OMR_PORT/$OMR_PATH}

OMR_TARGET=${OMR_TARGET:-x86_64}
OMR_TARGET_CONFIG="config-$OMR_TARGET"

OMR_FEED_URL="${OMR_FEED_URL:-https://github.com/ysurac/openmptcprouter-feeds}"
OMR_FEED_SRC="${OMR_FEED_SRC:-master}"

if [ ! -f "$OMR_TARGET_CONFIG" ]; then
	echo "Target $OMR_TARGET not found !"
	exit 1
fi

_get_repo source https://github.com/ysurac/openmptcprouter "master"
_get_repo feeds/packages https://github.com/openwrt/packages "master"
_get_repo feeds/luci https://github.com/openwrt/luci "for-15.05"

if [ -z "$OMR_FEED" ]; then
	OMR_FEED=feeds/openmptcprouter
	_get_repo "$OMR_FEED" "$OMR_FEED_URL" "$OMR_FEED_SRC"
fi

if [ -n "$1" ] && [ -f "$OMR_FEED/$1/Makefile" ]; then
	OMR_DIST=$1
	shift 1
fi

rm -rf source/bin source/files source/tmp
cp -rf root source/files

cat >> source/files/etc/banner <<EOF
-----------------------------------------------------
 PACKAGE:     $OMR_DIST
 VERSION:     $(git describe --tag --always)

 BUILD REPO:  $(git config --get remote.origin.url)
 BUILD DATE:  $(date -u)
-----------------------------------------------------
EOF

cat > source/feeds.conf <<EOF
src-link packages $(readlink -f feeds/packages)
src-link luci $(readlink -f feeds/luci)
src-link openmptcprouter $(readlink -f "$OMR_FEED")
EOF

cat "$OMR_TARGET_CONFIG" config -> source/.config <<EOF
CONFIG_IMAGEOPT=y
CONFIG_VERSIONOPT=y
CONFIG_VERSION_DIST="$OMR_DIST"
CONFIG_VERSION_REPO="$OMR_REPO"
CONFIG_VERSION_NUMBER="$(git describe --tag --always)"
CONFIG_VERSION_CODE="$(git -C "$OMR_FEED" describe --tag --always)"
CONFIG_PACKAGE_$OMR_DIST=y
CONFIG_PACKAGE_${OMR_DIST}-full=m
EOF

echo "Building $OMR_DIST for the target $OMR_TARGET"

cd source

cp .config .config.keep
scripts/feeds clean
scripts/feeds update -a
scripts/feeds install -a -d y -f -p openmptcprouter
cp .config.keep .config

make defconfig
make "$@"
