#!/bin/sh
#
# Licensed under the terms of the GNU GPL License version 2 or later.
#
# Author: Peter Tyser <ptyser@xes-inc.com>
#
# U-Boot firmware supports the booting of images in the Flattened Image
# Tree (FIT) format.  The FIT format uses a device tree structure to
# describe a kernel image, device tree blob, ramdisk, etc.  This script
# creates an Image Tree Source (.its file) which can be passed to the
# 'mkimage' utility to generate an Image Tree Blob (.itb file).  The .itb
# file can then be booted by U-Boot (or other bootloaders which support
# FIT images).  See doc/uImage.FIT/howto.txt in U-Boot source code for
# additional information on FIT images.
#

usage() {
	printf "Usage: %s -A arch -C comp -a addr -e entry" "$(basename "$0")"
	printf " -v version -k kernel [-D name -n address -d dtb] -o its_file"

	printf "\n\t-A ==> set architecture to 'arch'"
	printf "\n\t-C ==> set compression type 'comp'"
	printf "\n\t-c ==> set config name 'config'"
	printf "\n\t-a ==> set load address to 'addr' (hex)"
	printf "\n\t-e ==> set entry point to 'entry' (hex)"
	printf "\n\t-v ==> set kernel version to 'version'"
	printf "\n\t-k ==> include kernel image 'kernel'"
	printf "\n\t-D ==> human friendly Device Tree Blob 'name'"
	printf "\n\t-n ==> fdt unit-address 'address'"
	printf "\n\t-d ==> include Device Tree Blob 'dtb'"
	printf "\n\t-o ==> create output file 'its_file'\n"
	exit 1
}

FDTNUM=1

while getopts ":A:a:c:C:D:d:e:k:n:o:v:" OPTION
do
	case $OPTION in
		A ) ARCH=$OPTARG;;
		a ) LOAD_ADDR=$OPTARG;;
		c ) CONFIG=$OPTARG;;
		C ) COMPRESS=$OPTARG;;
		D ) DEVICE=$OPTARG;;
		d ) DTB=$OPTARG;;
		e ) ENTRY_ADDR=$OPTARG;;
		k ) KERNEL=$OPTARG;;
		n ) FDTNUM=$OPTARG;;
		o ) OUTPUT=$OPTARG;;
		v ) VERSION=$OPTARG;;
		* ) echo "Invalid option passed to '$0' (options:$*)"
		usage;;
	esac
done

# Make sure user entered all required parameters
if [ -z "${ARCH}" ] || [ -z "${COMPRESS}" ] || [ -z "${LOAD_ADDR}" ] || \
	[ -z "${ENTRY_ADDR}" ] || [ -z "${VERSION}" ] || [ -z "${KERNEL}" ] || \
	[ -z "${OUTPUT}" ] || [ -z "${CONFIG}" ]; then
	usage
fi

ARCH_UPPER=$(echo "$ARCH" | tr '[:lower:]' '[:upper:]')

# Conditionally create fdt information
if [ -n "${DTB}" ]; then
	if [ -f "$DTB" ]; then
		FDT_NODE="
			fdt@$FDTNUM {
				description = \"${ARCH_UPPER} OpenWrt ${DEVICE} device tree blob\";
				data = /incbin/(\"${DTB}\");
				type = \"flat_dt\";
				arch = \"${ARCH}\";
				compression = \"none\";
				hash@1 {
					algo = \"crc32\";
				};
				hash@2 {
					algo = \"sha1\";
				};
			};
"

		CONFIG_NODE="
			${CONFIG} {
			description = \"OpenWrt\";
			kernel = \"kernel@1\";
			fdt = \"fdt@$FDTNUM\";
		};
"
	else
		FDTNUM=0
		DEFAULT_CONFIG=$FDTNUM

		# for f in $(eval echo $DTB); do <= doesn't work on dash, here comes a workaround:
		DTB_DIR="${DTB%%{*}"
		DTB="${DTB##*{}"
		DTB_CSV="${DTB##*{}"
		DTB_CSV="${DTB%\}*}"
		IFS=,
		for f in $DTB_CSV; do
			FDTNUM=$((FDTNUM+1))

			ff="${DTB_DIR}/${f}"
			[ -f "${ff}" ] || {
				echo "ERROR: no DTBs found" >&2
				rm -f "${OUTPUT}"
				exit 1
			}

			FDT_NODE="${FDT_NODE}

				fdt@$FDTNUM {
					description = \"${f}\";
					data = /incbin/(\"${ff}\");
					type = \"flat_dt\";
					arch = \"${ARCH}\";
					compression = \"none\";
					hash@1 {
						algo = \"crc32\";
					};
					hash@2 {
						algo = \"sha1\";
					};
				};
"
			# extract XYZ from image-qcom-ipq4018-rutx-XYZ.dtb
			f=${f##*-}
			f=${f%.*}

			CONFIG_NODE="${CONFIG_NODE}

				conf_mdtb@${FDTNUM} {
				description = \"${f}\";
				kernel = \"kernel@1\";
				fdt = \"fdt@$FDTNUM\";
			};
"
		done
	fi
fi

# Create a default, fully populated DTS file
DATA="/dts-v1/;

/ {
	description = \"${ARCH_UPPER} OpenWrt FIT (Flattened Image Tree)\";
	#address-cells = <1>;

	images {
		kernel@1 {
			description = \"${ARCH_UPPER} OpenWrt Linux-${VERSION}\";
			data = /incbin/(\"${KERNEL}\");
			type = \"kernel\";
			arch = \"${ARCH}\";
			os = \"linux\";
			compression = \"${COMPRESS}\";
			load = <${LOAD_ADDR}>;
			entry = <${ENTRY_ADDR}>;
			hash@1 {
				algo = \"crc32\";
			};
			hash@2 {
				algo = \"sha1\";
			};
		};
${FDT_NODE}
	};

	configurations {
		default = \"${DEFAULT_CONFIG}\";
${CONFIG_NODE}
	};
};"

# Write .its file to disk
echo "$DATA" > "${OUTPUT}"
