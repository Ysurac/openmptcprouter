# Security Policy

## Supported Versions

Security fixes are provided for the latest stable release and the current development branch.

| Version                     | Supported          |
| --------------------------- | ------------------ |
| 0.65 snapshot (`develop`)   | :white_check_mark: |
| 0.64 (latest stable)        | :white_check_mark: |
| < 0.64                      | :x:                |

If you are running an older release, please upgrade to the latest stable version before reporting an issue.

## Reporting a Vulnerability

**Please do not report security vulnerabilities through public GitHub issues, discussions or pull requests.**

Report them privately using one of the following:

* GitHub private vulnerability reporting: [Report a vulnerability](https://github.com/Ysurac/openmptcprouter/security/advisories/new)
* E-mail: contact@openmptcprouter.com

Please include as much of the following as possible:

* OpenMPTCProuter version and target/platform (e.g. `x86_64`, `bpi-r4`, `rpi4`)
* Affected component (router image, VPS script, a specific package, LuCI interface, ...)
* Description of the issue and its potential impact
* Steps to reproduce, proof of concept or exploit code if available
* Any suggested fix or mitigation

## What to Expect

* An acknowledgement of your report within 7 days.
* An assessment of the issue and, if confirmed, a plan for a fix.
* Updates on progress until the issue is resolved.
* Credit in the release notes once a fix is published, unless you prefer to remain anonymous.

Please give us reasonable time to release a fix before any public disclosure.

## Scope

This repository contains the build system for OpenMPTCProuter router images. Related components:

* Packages: [openmptcprouter-feeds](https://github.com/Ysurac/openmptcprouter-feeds)
* VPS side: [openmptcprouter-vps](https://github.com/Ysurac/openmptcprouter-vps)

Reports for any of these components can be sent through the channels above.

Vulnerabilities in upstream projects (OpenWrt, Linux kernel, MPTCP, Shadowsocks, Glorytun, OpenVPN, ...) should also be reported to the upstream project. If an upstream issue affects OpenMPTCProuter, let us know so we can ship the fix.
