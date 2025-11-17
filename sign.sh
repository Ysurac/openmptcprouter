#!/bin/sh
# Image signing script for OpenMPTCProuter
# Usage: ./sign.sh [key-file] [target-path]

set -e

# Get and validate parameters
key="${1:-key-build}"
path="${2:-x86_64}"

# Validate key file exists
if [ ! -f "$key" ]; then
    echo "Error: Key file '$key' not found" >&2
    exit 1
fi

# Validate path is a directory
if [ ! -d "$path" ]; then
    echo "Error: Path '$path' is not a directory" >&2
    exit 1
fi

# Validate source/bin directory exists
if [ ! -d "$path/source/bin" ]; then
    echo "Error: Directory '$path/source/bin' does not exist" >&2
    exit 1
fi

# Validate usign binary exists
usign_bin="$path/source/staging_dir/host/bin/usign"
if [ ! -x "$usign_bin" ]; then
    echo "Error: usign binary not found at '$usign_bin'" >&2
    exit 1
fi

echo "Signing images in $path/source/bin with key $key..."

# Sign all images and package files
find "$path/source/bin" \
    \( -name '*.img.gz' -or -name 'Packages' \) \
    -exec "$usign_bin" -S -m {} -s "$key" \;

echo "Signing complete!"
