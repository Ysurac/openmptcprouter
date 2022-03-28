#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#

import sys
import struct

def create_header(base, size, img_type):
    """Returns a packed MBN header image with the specified base and size.

    @arg base: integer, specifies the image load address in RAM
    @arg size: integer, specifies the size of the image
    @arg img_type: integer, specifies the image type
    @returns: string, the MBN header
    """

    header = [
        img_type,         # Type: APPSBL
        0x3,         # Version: 3
        0x0,         # Image source pointer
        base,        # Image destination pointer
        size,        # Code Size + Cert Size + Signature Size
        size,        # Code Size
        base + size, # Destination + Code Size
        0x0,         # Signature Size
        base + size, # Destination + Code Size + Signature Size
        0x0,         # Cert Size
    ]

    header_packed = struct.pack('<10I', *header)
    return header_packed

def mkheader(base_addr, img_type, infname, outfname):
    """Prepends the image with the MBN header.

    @arg base_addr: integer, specifies the image load address in RAM
    @arg img_type: integer, specifies the image type
    @arg infname: string, image filename
    @arg outfname: string, output image with header prepended
    @raises IOError: if reading/writing input/output file fails
    """
    with open(infname, "rb") as infp:
        image = infp.read()
        insize = len(image)

    if base_addr > 0xFFFFFFFF:
        raise ValueError("invalid base address")

    if base_addr + insize > 0xFFFFFFFF:
        raise ValueError("invalid destination range")

    header = create_header(base_addr, insize, img_type)
    with open(outfname, "wb") as outfp:
        outfp.write(header)
        outfp.write(image)

def usage(msg=None):
    """Print command usage.

    @arg msg: string, error message if any (default: None)
    """
    if msg != None:
        sys.stderr.write("mkheader: %s\n" % msg)

    print "Usage: mkheader.py <base-addr> <img_type> <input-file> <output-file>"

    if msg != None:
        exit(1)

def main():
    """Main entry function"""

    if len(sys.argv) != 5:
        usage("incorrect no. of arguments")

    try:
        base_addr = int(sys.argv[1], 0)
        img_type = int(sys.argv[2], 0)
        infname = sys.argv[3]
        outfname = sys.argv[4]
    except ValueError as e:
        sys.stderr.write("mkheader: invalid base address '%s'\n" % sys.argv[1])
        exit(1)

    try:
        mkheader(base_addr, img_type,  infname, outfname)
    except IOError as e:
        sys.stderr.write("mkheader: %s\n" % e)
        exit(1)
    except ValueError as e:
        sys.stderr.write("mkheader: %s\n" % e)
        exit(1)

if __name__ == "__main__":
    main()

