[![CircleCI](https://circleci.com/gh/Ysurac/openmptcprouter-feeds.svg?style=svg)](https://circleci.com/gh/Ysurac/openmptcprouter-feeds)

# OpenMPTCProuter

OpenMPTCProuter is an open source solution to aggregate and encrypt multiple internet connections and terminates it over any VPS which make clients benefit security, reliability, net neutrality, as well as dedicated public IP.

The aggregation is based on Multipath TCP (MPTCP), which is ISP, WAN type, and latency independent "whether it was Fiber, VDSL, SHDSL, ADSL or even 4G", different scenarios can be configured to have either aggregation or failover based on MPTCP.

The solution takes advantage of the OpenWRT/LEDE system, which is user friendly and also adds the possibility of installing other packages like VPN, QoS, routing protocols, monitoring, etc. through web-interface or terminal.

## Install from pre-compiled images

You can download precompiled images from [https://www.openmptcprouter.com/](https://www.openmptcprouter.com/)

Then copy it to a sdcard:

```sh
gunzip omr-*.img.gz
dd bs=4M if=omr-*.img of=/dev/sdX conv=fsync
```

## Install from source

### Dependencies

You need a classical build environment like `build-essential` on Debian and `git`.
Some feeds might not available over `git` but only via `subversion` or `mercurial`.

On Debian you'll need to install the following:

```sh
sudo apt install build-essential git unzip ncurses-dev libz-dev libssl-dev
  python subversion gettext gawk wget curl rsync perl
```

### Prepare and build

```sh
git clone https://github.com/ysurac/openmptcprouter.git
cd openmptcprouter
./build.sh
```

The script `build.sh` accepts all `make` arguments (like `-j`).
When finished, files are located in the directory `source/bin`.

### Custom arch build

By default the build script will create the packages for the `x86_64` architecture.
You can specify a custom build target by adding a `OMR_TARGET` environment variable to the build and the corresponding `config-$OMR_TARGET` file.

To build the project for the raspberry pi 3:

```sh
OMR_TARGET="rpi3" ./build.sh
```

## Credits

Our solution is mainly based on:

* [OverTheBox](https://www.ovhtelecom.fr/overthebox/)
* [OpenWRT](https://openwrt.org)
* [LEDE](https://lede-project.org)
* [MultiPath TCP (MPTCP)](https://multipath-tcp.org)
* [Shadowsocks](https://shadowsocks.org)
