#!/bin/sh

key=${1:-key-build}

[ -d source/bin ] && [ -f "$key" ] && \
	find source/bin \
	\( -name '*.img.gz' -or -name 'Packages' \) \
	-exec source/staging_dir/host/bin/usign -S -m {} -s "$key" \;
