#!/bin/sh

key=${1:-key-build}
path=${2:-x86_64}
[ -d $path/source/bin ] && [ -f "$key" ] && \
	find $path/source/bin \
	\( -name '*.img.gz' -or -name 'Packages' \) \
	-exec $path/source/staging_dir/host/bin/usign -S -m {} -s "$key" \;
