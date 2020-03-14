[![CircleCI](https://circleci.com/gh/Ysurac/openmptcprouter-feeds.svg?style=svg)](https://circleci.com/gh/Ysurac/openmptcprouter-feeds)
[![Financial Contributors on Open Collective](https://opencollective.com/openmptcprouter/all/badge.svg?label=financial+contributors)](https://opencollective.com/openmptcprouter) ![Latest tag](https://img.shields.io/github/tag/ysurac/openmptcprouter-feeds.svg)
[![Paypal](https://www.openmptcprouter.com/img/donate-PayPal-green.svg)](https://www.paypal.me/ycarus)
[![Flattr](https://www.openmptcprouter.com/img/donate-flattr-yellow.svg)](https://flattr.com/@ycarus)
[![Liberapay](https://img.shields.io/liberapay/patrons/Moul.svg?logo=liberapay)](https://liberapay.com/Ycarus/)
[![LinkedIn](https://www.openmptcprouter.com/img/linkedin.png)](https://www.linkedin.com/in/yannick-chabanois-550330146/)
[![Twitter](https://www.openmptcprouter.com/img/twitter.jpg)](https://twitter.com/OpenMPTCProuter)
[![Atom](https://www.openmptcprouter.com/img/feed.png)](https://www.openmptcprouter.com/atom)

# OpenMPTCProuter

OpenMPTCProuter is an open source solution to aggregate and encrypt multiple internet connections and terminates it over any VPS which make clients benefit security, reliability, net neutrality, as well as dedicated public IP.

The aggregation is based on Multipath TCP (MPTCP), which is ISP, WAN type, and latency independent "whether it was Fiber, VDSL, SHDSL, ADSL or even 4G", different scenarios can be configured to have either aggregation or failover based on MPTCP.

Aggregation via [Multi-link VPN (MLVPN)](https://github.com/markfoodyburton/MLVPN/commits/new-reorder) and [Glorytun UDP](https://github.com/angt/glorytun) with multipath support are also supported.

The solution takes advantage of the OpenWRT/LEDE system, which is user friendly and also adds the possibility of installing other packages like VPN, QoS, routing protocols, monitoring, etc. through web-interface or terminal.


Main website: [https://www.openmptcprouter.com/](https://www.openmptcprouter.com/)

Packages made for OpenMPTCProuter are available here: [https://github.com/Ysurac/openmptcprouter-feeds](https://github.com/Ysurac/openmptcprouter-feeds)

OpenMPTCProuter VPS script part: [https://github.com/Ysurac/openmptcprouter-vps](https://github.com/Ysurac/openmptcprouter-vps)


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
sudo apt install build-essential git unzip ncurses-dev libz-dev libssl-dev openssl-1.0-dev
  python python3-dev python3.5 libelf-dev subversion gettext gawk wget curl rsync perl
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

* [OpenWRT](https://openwrt.org)
* [MultiPath TCP (MPTCP)](https://multipath-tcp.org)
* [Shadowsocks](https://shadowsocks.org)
* [Glorytun](https://github.com/angt/glorytun)
## Contributors

### Code Contributors

This project exists thanks to all the people who contribute. [[Contribute](CONTRIBUTING.md)].
<a href="https://github.com/Ysurac/openmptcprouter/graphs/contributors"><img src="https://opencollective.com/openmptcprouter/contributors.svg?width=890&button=false" /></a>

### Financial Contributors

Become a financial contributor and help us sustain our community. [[Contribute](https://opencollective.com/openmptcprouter/contribute)]

#### Individuals

<a href="https://opencollective.com/openmptcprouter"><img src="https://opencollective.com/openmptcprouter/individuals.svg?width=890"></a>

#### Organizations

Support this project with your organization. Your logo will show up here with a link to your website. [[Contribute](https://opencollective.com/openmptcprouter/contribute)]

<a href="https://opencollective.com/openmptcprouter/organization/0/website"><img src="https://opencollective.com/openmptcprouter/organization/0/avatar.svg"></a>
<a href="https://opencollective.com/openmptcprouter/organization/1/website"><img src="https://opencollective.com/openmptcprouter/organization/1/avatar.svg"></a>
<a href="https://opencollective.com/openmptcprouter/organization/2/website"><img src="https://opencollective.com/openmptcprouter/organization/2/avatar.svg"></a>
<a href="https://opencollective.com/openmptcprouter/organization/3/website"><img src="https://opencollective.com/openmptcprouter/organization/3/avatar.svg"></a>
<a href="https://opencollective.com/openmptcprouter/organization/4/website"><img src="https://opencollective.com/openmptcprouter/organization/4/avatar.svg"></a>
<a href="https://opencollective.com/openmptcprouter/organization/5/website"><img src="https://opencollective.com/openmptcprouter/organization/5/avatar.svg"></a>
<a href="https://opencollective.com/openmptcprouter/organization/6/website"><img src="https://opencollective.com/openmptcprouter/organization/6/avatar.svg"></a>
<a href="https://opencollective.com/openmptcprouter/organization/7/website"><img src="https://opencollective.com/openmptcprouter/organization/7/avatar.svg"></a>
<a href="https://opencollective.com/openmptcprouter/organization/8/website"><img src="https://opencollective.com/openmptcprouter/organization/8/avatar.svg"></a>
<a href="https://opencollective.com/openmptcprouter/organization/9/website"><img src="https://opencollective.com/openmptcprouter/organization/9/avatar.svg"></a>
