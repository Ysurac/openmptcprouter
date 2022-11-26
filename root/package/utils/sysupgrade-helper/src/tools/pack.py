#!/usr/bin/env python
#
# Copyright (c) 2013-2015 The Linux Foundation. All rights reserved.
#
"""
Script to create a U-Boot flashable multi-image blob.

This script creates a multi-image blob, from a bunch of images, and
adds a U-Boot shell script to the blob, that can flash the images from
within U-Boot. The procedure to use this script is listed below.

  1. Create an images folder. Ex: my-pack

  2. Copy all the images to be flashed into the folder.

  3. Copy the partition MBN file into the folder. The file should be
     named 'partition.mbn'. This is used to determine the offsets for
     each of the named partitions.

  4. Create a flash configuration file, specifying the images be
     flashed, and the partition in which the images is to be
     flashed. The flash configuration file can be specified using the
     -f option, default is flash.conf.

  5. Invoke 'pack' with the folder name as argument, pass flash
     parameters as arguments if required. A single image file will
     be created, out side the images folder, with .img suffix. Ex:
     my-pack.img

  6. Transfer the file into a valid SDRAM address and invoke the
     following U-Boot command to flash the images. Replace 0x41000000,
     with address location where the image has been loaded. The script
     expects the variable 'imgaddr' to be set.

     u-boot> imgaddr=0x41000000 source $imgaddr:script

Host-side Pre-req

  * Python >= 2.6
  * ordereddict >= 1.1 (for Python 2.6)
  * mkimage >= 2012.07
  * dtc >= 1.2.0

Target-side Pre-req

The following U-Boot config macros should be enabled, for the
generated flashing script to work.

  * CONFIG_FIT -- FIT image format support
  * CONFIG_SYS_HUSH_PARSER -- bash style scripting support
  * CONFIG_SYS_NULLDEV -- redirecting command output support
  * CONFIG_CMD_XIMG -- extracting sub-images support
  * CONFIG_CMD_NAND -- NAND Flash commands support
  * CONFIG_CMD_NAND_YAFFS -- NAND YAFFS2 write support
  * CONFIG_CMD_SF -- SPI Flash commands support
"""

from ConfigParser import ConfigParser
from ConfigParser import Error as ConfigParserError
from os.path import getsize
from getopt import getopt
from getopt import GetoptError
from collections import namedtuple
from string import Template
from unittest import TestCase
from tempfile import mkdtemp
from shutil import rmtree

import os
import sys
import os.path
import subprocess
import struct
import re
import hashlib

version = "1.1"

#
# Python 2.6 and earlier did not have OrderedDict use the backport
# from ordereddict package. If that is not available report error.
#
try:
    from collections import OrderedDict
except ImportError:
    try:
        from ordereddict import OrderedDict
    except ImportError:
        print "error: this script requires the 'ordereddict' class."
        print "Try 'pip install --user ordereddict'"
        print "Or  'easy_install --user ordereddict'"
        sys.exit(1)

__all__ = []

KB = 1024
MB = 1024 * KB

def error(msg, ex=None):
    """Print an error message and exit.

    msg -- string, the message to print
    ex -- exception, the associate exception, if any
    """

    sys.stderr.write("pack: %s" % msg)
    if ex != None: sys.stderr.write(": %s" % str(ex))
    sys.stderr.write("\n")
    sys.exit(1)

FlashInfo = namedtuple("FlashInfo", "type pagesize blocksize chipsize")
ImageInfo = namedtuple("ProgInfo", "name filename type")
PartInfo = namedtuple("PartInfo", "name offset length")

def roundup(value, roundto):
    """Return the next largest multiple of 'roundto'."""

    return ((value + roundto - 1) // roundto) * roundto

class GPT(object):
    GPTheader = namedtuple("GPTheader", "signature revision header_size"
                            " crc32 current_lba backup_lba first_usable_lba"
                            " last_usable_lba disk_guid start_lba_part_entry"
                            " num_part_entry part_entry_size part_crc32")
    GPT_SIGNATURE = 'EFI PART'
    GPT_REVISION = '\x00\x00\x01\x00'
    GPT_HEADER_SIZE = 0x5C
    GPT_HEADER_FMT = "<8s4sLL4xQQQQ16sQLLL"

    GPTtable = namedtuple("GPTtable", "part_type unique_guid first_lba"
                           " last_lba attribute_flag part_name")
    GPT_TABLE_FMT = "<16s16sQQQ72s"

    def __init__(self, filename, pagesize, blocksize, chipsize):
        self.filename = filename
        self.pagesize = pagesize
        self.blocksize = blocksize
        self.chipsize = chipsize
        self.__partitions = OrderedDict()

    def __validate_and_read_parts(self, part_fp):
        """Validate the GPT and read the partition"""
        part_fp.seek(self.blocksize, os.SEEK_SET)
        gptheader_str = part_fp.read(struct.calcsize(GPT.GPT_HEADER_FMT))
        gptheader = struct.unpack(GPT.GPT_HEADER_FMT, gptheader_str)
        gptheader = GPT.GPTheader._make(gptheader)

        if gptheader.signature != GPT.GPT_SIGNATURE:
            error("Invalid signature")

        if gptheader.revision != GPT.GPT_REVISION:
            error("Unsupported GPT Revision")

        if gptheader.header_size != GPT.GPT_HEADER_SIZE:
            error("Invalid Header size")

        # Adding GPT partition info. This has to be flashed first.
        # GPT Header starts at LBA1 so (current_lba -1) will give the
        # starting of primary GPT.
        # blocksize will equal to gptheader.first_usuable_lba - current_lba + 1

        name = "0:GPT"
        block_start = gptheader.current_lba - 1
        block_count = gptheader.first_usable_lba - gptheader.current_lba + 1
        part_info = PartInfo(name, block_start, block_count)
        self.__partitions[name] = part_info

        part_fp.seek(2 * self.blocksize, os.SEEK_SET)

        for i in range(gptheader.num_part_entry):
            gpt_table_str = part_fp.read(struct.calcsize(GPT.GPT_TABLE_FMT))
            gpt_table = struct.unpack(GPT.GPT_TABLE_FMT, gpt_table_str)
            gpt_table = GPT.GPTtable._make(gpt_table)

            block_start = gpt_table.first_lba
            block_count = gpt_table.last_lba - gpt_table.first_lba + 1

            part_name = gpt_table.part_name.strip(chr(0))
            name = part_name.replace('\0','')
            part_info = PartInfo(name, block_start, block_count)
            self.__partitions[name] = part_info

        # Adding the GPT Backup partition.
        # GPT header backup_lba gives block number where the GPT backup header will be.
        # GPT Backup header will start from offset of 32 blocks before
        # the GPTheader.backup_lba. Backup GPT size is 33 blocks.
        name = "0:GPTBACKUP"
        block_start = gptheader.backup_lba - 32
        block_count = 33
        part_info = PartInfo(name, block_start, block_count)
        self.__partitions[name] = part_info

    def get_parts(self):
        """Returns a list of partitions present in the GPT."""

        try:
            with open(self.filename, "r") as part_fp:
                self.__validate_and_read_parts(part_fp)
        except IOError, e:
            error("error opening %s" % self.filename, e)

        return self.__partitions

class MIBIB(object):
    Header = namedtuple("Header", "magic1 magic2 version age")
    HEADER_FMT = "<LLLL"
    HEADER_MAGIC1 = 0xFE569FAC
    HEADER_MAGIC2 = 0xCD7F127A
    HEADER_VERSION = 4

    Table = namedtuple("Table", "magic1 magic2 version numparts")
    TABLE_FMT = "<LLLL"
    TABLE_MAGIC1 = 0x55EE73AA
    TABLE_MAGIC2 = 0xE35EBDDB
    TABLE_VERSION = 3

    Entry = namedtuple("Entry", "name offset length"
                        " attr1 attr2 attr3 which_flash")
    ENTRY_FMT = "<16sLLBBBB"

    def __init__(self, filename, pagesize, blocksize, chipsize):
        self.filename = filename
        self.pagesize = pagesize
        self.blocksize = blocksize
        self.chipsize = chipsize
        self.__partitions = OrderedDict()

    def __validate(self, part_fp):
        """Validate the MIBIB by checking for magic bytes."""

        mheader_str = part_fp.read(struct.calcsize(MIBIB.HEADER_FMT))
        mheader = struct.unpack(MIBIB.HEADER_FMT, mheader_str)
        mheader = MIBIB.Header._make(mheader)

        if (mheader.magic1 != MIBIB.HEADER_MAGIC1
            or mheader.magic2 != MIBIB.HEADER_MAGIC2):
            error("invalid partition table, magic byte not present")

        if mheader.version != MIBIB.HEADER_VERSION:
            error("unsupport mibib version")

    def __read_parts(self, part_fp):
        """Read the partitions from the MIBIB."""

        part_fp.seek(self.pagesize, os.SEEK_SET)
        mtable_str = part_fp.read(struct.calcsize(MIBIB.TABLE_FMT))
        mtable = struct.unpack(MIBIB.TABLE_FMT, mtable_str)
        mtable = MIBIB.Table._make(mtable)

        if (mtable.magic1 != MIBIB.TABLE_MAGIC1
            or mtable.magic2 != MIBIB.TABLE_MAGIC2):
            error("invalid sys part. table, magic byte not present")

        if mtable.version != MIBIB.TABLE_VERSION:
            error("unsupported partition table version")

        for i in range(mtable.numparts):
            mentry_str = part_fp.read(struct.calcsize(MIBIB.ENTRY_FMT))
            mentry = struct.unpack(MIBIB.ENTRY_FMT, mentry_str)
            mentry = MIBIB.Entry._make(mentry)

            byte_offset = mentry.offset * self.blocksize

            if mentry.length == 0xFFFFFFFF:
                byte_length = self.chipsize - byte_offset
            else:
                byte_length = mentry.length * self.blocksize

            part_name = mentry.name.strip(chr(0))
            part_info = PartInfo(part_name, byte_offset, byte_length)
            self.__partitions[part_name] = part_info

    def __write_header(self, part_fp):
        """Write the MIBIB header, used for unit testing purposes."""

        header = MIBIB.Header(magic1=MIBIB.HEADER_MAGIC1,
                              magic2=MIBIB.HEADER_MAGIC2,
                              version=MIBIB.HEADER_VERSION,
                              age=1)
        header_str = struct.pack(MIBIB.HEADER_FMT, *header)
        part_fp.write(header_str)

    def __write_parts(self, part_fp):
        """Write the MIBIB partitions, used for unit testing purposes."""

        part_fp.seek(self.pagesize, os.SEEK_SET)
        table = MIBIB.Table(magic1=MIBIB.TABLE_MAGIC1,
                            magic2=MIBIB.TABLE_MAGIC2,
                            version=MIBIB.TABLE_VERSION,
                            numparts=len(self.__partitions))
        table_str = struct.pack(MIBIB.TABLE_FMT, *table)
        part_fp.write(table_str)
        for name, offset, length in self.__partitions.itervalues():
            block_offset = offset / self.blocksize

            if length == None:
                block_length = 0xFFFFFFFF
            else:
                block_length = length / self.blocksize

            entry = MIBIB.Entry(name, block_offset, block_length,
                                attr1=0, attr2=0, attr3=0, which_flash=0)
            entry_str = struct.pack(MIBIB.ENTRY_FMT, *entry)
            part_fp.write(entry_str)

    def get_parts(self):
        """Returns a list of partitions present in the MIBIB."""

        try:
            with open(self.filename, "r") as part_fp:
                self.__validate(part_fp)
                self.__read_parts(part_fp)
        except IOError, e:
            error("error opening %s" % self.filename, e)

        return self.__partitions

    def add_part(self, part_info):
        """Add a partition for writing to MIBIB."""

        self.__partitions[part_info.name] = part_info

    def write(self):
        """Write the MIBIB to file, for unit testing purposes."""

        try:
            with open(self.filename, "w") as part_fp:
                self.__write_header(part_fp)
                self.__write_parts(part_fp)
        except IOError, e:
            error("error opening %s" % self.filename, e)

class FlashScript(object):
    """Base class for creating flash scripts."""

    def __init__(self, flinfo):
        self.pagesize = flinfo.pagesize
        self.blocksize = flinfo.blocksize
        self.script = []
        self.parts = []
        self.curr_stdout = "serial"
        self.activity = None

        self.script.append('if test "x$verbose" = "x"; then\n')
        self.script.append("failedmsg='[failed]'\n")
        self.script.append('else\n')
        self.script.append("failedmsg='%s Failed'\n" % ("#" * 40))
        self.script.append('fi\n')

    def append(self, cmd, fatal=True):
        """Add a command to the script.

        Add additional code, to terminate on error. This can be
        supressed by passing 'fatal' as False.
        """

        if fatal:
            self.script.append(cmd
                               + ' || setenv stdout serial'
                               + ' && echo "$failedmsg"'
                               + ' && exit 1\n')
        else:
            self.script.append(cmd + "\n")

    def check_isset(self, var):
        """Check if a variable is set."""
        self.append('test "x$%s" != "x"' % var)

    def dumps(self):
        """Return the created script as a string."""
        return "".join(self.script)

    def redirect(self, dev):
        """Generate code, to redirect command output to a device."""

        if self.curr_stdout == dev:
            return

        self.append("setenv stdout %s" % dev, fatal=False)
        self.curr_stdout = dev

    def start_activity(self, activity):
        """Generate code, to indicate start of an activity."""

        self.script.append('if test "x$verbose" = "x"; then\n')
        self.echo("'%-40.40s'" % activity, nl=False)
        self.script.append('else\n')
        self.echo("'%s %s Started'" % ("#" * 40, activity), verbose=True)
        self.script.append('fi\n')
        self.activity = activity

    def finish_activity(self):
        """Generate code, to indicate end of an activity."""

        self.script.append('if test "x$verbose" = "x"; then\n')
        self.echo("'[ done ]'")
        self.redirect("serial")
        self.script.append('else\n')
        self.echo("'%s %s Done'" % ("#" * 40, self.activity), verbose=True)
        self.script.append('fi\n')

    def imxtract(self, part):
        """Generate code, to extract image location, from a multi-image blob.

        part -- string, name of the sub-image

        Sets the $fileaddr environment variable, to point to the
        location of the sub-image.
        """

        self.append("imxtract $imgaddr %s" % part)

    def echo(self, msg, nl=True, verbose=False):
        """Generate code, to print a message.

        nl -- bool, indicates whether newline is to be printed
        verbose -- bool, indicates whether printing in verbose mode
        """

        if not verbose:
            self.redirect("serial")

        if nl:
            self.append("echo %s" % msg, fatal=False)
        else:
            self.append("echo %s%s" % (r"\\c", msg), fatal=False)

        if not verbose:
            self.redirect("nulldev")

    def end(self):
        """Generate code, to indicate successful completion of script."""

        self.append("exit 0\n", fatal=False)

    def start_if(self, var, value):
        """Generate code, to check an environment variable.

        var -- string, variable to check
        value -- string, the value to compare with
        """

        self.append('if test "$%s" = "%s"; then\n' % (var, value),
                    fatal=False)

    def end_if(self):
        """Generate code, to end if statement."""

        self.append('fi\n', fatal=False)

class NandScript(FlashScript):
    """Class for creating NAND flash scripts."""

    def __init__(self, flinfo, ipq_nand):
        FlashScript.__init__(self, flinfo)
        self.ipq_nand = ipq_nand

    def erase(self, offset, size):
        """Generate code, to erase the specified partition."""

        size = roundup(size, self.blocksize)
        self.append("nand erase 0x%08x 0x%08x" % (offset, size))

    def write(self, offset, size, yaffs):
        """Generate code, to write to a partition."""

        if size > 0:
            if yaffs:
                self.append("nand write.yaffs $fileaddr 0x%08x 0x%08x"
                        % (offset, size))
            else:
                size = roundup(size, self.pagesize)
                self.append("nand write $fileaddr 0x%08x 0x%08x" % (offset, size))

    def switch_layout(self, layout):
        """Generate code, to switch between sbl/linux layouts."""

        self.append("ipq_nand %s" % layout)

class NorScript(FlashScript):
    """Class for creating NAND flash scripts."""

    def __init__(self, flinfo):
        FlashScript.__init__(self, flinfo)

    def erase(self, offset, size):
        """Generate code, to erase the specified partition."""

        size = roundup(size, self.blocksize)
        self.append("sf erase 0x%08x +0x%08x" % (offset, size))

    def write(self, offset, size, yaffs):
        """Generate code, to write to a partition."""

        if size > 0:
            self.append("sf write $fileaddr 0x%08x 0x%08x" % (offset, size))

    def nand_write(self, offset, part_size, img_size):
        """Handle the NOR + NAND case
           All binaries upto HLOS will go to NOR and Root FS will go to NAND
           Assumed all nand page sizes are less than are equal to 8KB
           """
        self.append("nand device 0 && nand erase 0x%08x 0x%08x" % (offset, part_size))
        if img_size > 0:
            self.append("nand write $fileaddr 0x%08x 0x%08x" % (offset, img_size))

    def switch_layout(self, layout):
        pass

class EmmcScript(FlashScript):
    """Class for creating EMMC scripts."""

    def __init__(self, flinfo):
        FlashScript.__init__(self, flinfo)

    def erase(self, offset, size):
        """Generate code, to erase the specified partition."""

        self.append("mmc erase 0x%08x %x" % (offset, size))

    def write(self, offset, size, yaffs):
        """Generate code, to write to a partition."""
        if size > 0:
           size = roundup(size, self.blocksize)
           blk_cnt = size / self.blocksize
           self.append("mmc write $fileaddr 0x%08x %x" % (offset, blk_cnt))

    def switch_layout(self, layout):
        pass

its_tmpl = Template("""
/dts-v1/;

/ {
        description = "${desc}";
        images {
${images}
        };
};
""")

its_image_tmpl = Template("""
                ${name} {
                        description = "${desc}";
                        data = /incbin/("./${fname}");
                        type = "${imtype}";
                        arch = "arm";
                        compression = "none";
                        hash@1 { algo = "crc32"; };
                };
""")


def sha1(message):
    """Returns SHA1 digest in hex format of the message."""

    m = hashlib.sha1()
    m.update(message)
    return m.hexdigest()


class Pack(object):
    """Class to create a flashable, multi-image blob.

    Combine multiple images present in a directory, and generate a
    U-Boot script to flash the images.
    """
    # The maximum rootfs size is 64MB
    norplusnand_rootfs_img_size = (64 * 1024 * 1024)

    def __init__(self):
        self.flinfo = None
        self.images_dname = None
        self.ipq_nand = None
        self.partitions = {}

        self.fconf_fname = None
        self.scr_fname = None
        self.its_fname = None
        self.img_fname = None

    def __get_yaffs(self, info, section):
        """Get the yaffs flag for a section.

        info -- ConfigParser object, containing image flashing info
        section -- section to check if yaffs flag is set
        """
        try:
            yaffs = info.get(section, "yaffs")
            if yaffs.lower() in ["0", "no"]:
                yaffs = False
            elif yaffs.lower() in ["1", "yes"]:
                yaffs = True
            else:
                error("invalid value for 'yaffs' in '%s'" % section)
        except ConfigParserError, e:
            yaffs = False

        if self.flinfo.type == "nor" and yaffs == True:
            error("yaffs cannot be used with NOR flash type")

        return yaffs

    def __get_layout(self, info, section):
        """Get the layout for a section.

        info -- ConfigParser object, containing image flashing info
        section - section to retreive the layout from
        """
        try:
            layout = info.get(section, "layout")
        except ConfigParserError, e:
            layout = None

        if self.ipq_nand and layout == None:
            error("layout not specified for IPQ device")

        if not self.ipq_nand and layout != None:
            error("layout specified for a non IPQ device")

        if layout not in ("sbl", "linux", None):
            error("invalid layout in '%s'" % section)

        return layout

    def __get_machid(self, info, section):
        """Get the machid for a section.

        info -- ConfigParser object, containing image flashing info
        section -- section to retreive the machid from
        """
        try:
            machid = info.get(section, "if_machid")
            machid = int(machid, 0)
            machid = "%x" % machid
        except ConfigParserError, e:
            machid = None
        except ValueError, e:
            error("invalid value for machid, should be integer")

        return machid

    def __get_img_size(self, filename):
        """Get the size of the image to be flashed

        filaneme -- string, filename of the image to be flashed
        """

        if filename.lower() == "none":
            return 0
        try:
            return getsize(os.path.join(self.images_dname, filename))
        except OSError, e:
            error("error getting image size '%s'" % filename, e)

    def __get_part_info(self, partition):
        """Return partition info for the specified partition.

        partition -- string, partition name
        """
        try:
            return self.partitions[partition]
        except KeyError, e:
            return None

    def __gen_flash_script(self, info, script, flinfo):
        """Generate the script to flash the images.

        info -- ConfigParser object, containing image flashing info
        script -- Script object, to append commands to
        """
        count = 0

        for section in info.sections():
            try:
                filename = info.get(section, "filename")
                partition = info.get(section, "partition")
                include = info.get(section, "include")
            except ConfigParserError, e:
                error("error getting image info in section '%s'" % section, e)

            if include.lower() in ["0", "no"]:
                continue

            machid = self.__get_machid(info, section)
            layout = self.__get_layout(info, section)
            yaffs = self.__get_yaffs(info, section)

            img_size = self.__get_img_size(filename)
            part_info = self.__get_part_info(partition)

            if self.flinfo.type == 'nand':
                size = roundup(img_size, flinfo.pagesize)
                tr = ' | tr \"\\000\" \"\\377\"'

            if self.flinfo.type == 'emmc':
                size = roundup(img_size, flinfo.blocksize)
                tr = ''

            if ((self.flinfo.type == 'nand' or self.flinfo.type == 'emmc') and (size != img_size)):
                pad_size = size - img_size
                filename_abs = os.path.join(self.images_dname, filename)
                filename_abs_pad = filename_abs + ".padded"
                cmd = 'cat %s > %s' % (filename_abs, filename_abs_pad)
                ret = subprocess.call(cmd, shell=True)
                if ret != 0:
                    error("failed to copy image")
                cmd = 'dd if=/dev/zero count=1 bs=%s %s >> %s' % (pad_size, tr, filename_abs_pad)
                cmd = '(' + cmd + ') 1>/dev/null 2>/dev/null'
                ret = subprocess.call(cmd, shell=True)
                if ret != 0:
                    error("failed to create padded image from script")

            if self.flinfo.type != "emmc":
               if part_info == None:
                   if self.flinfo.type == 'norplusnand':
                       if count > 2:
                           error("More than 2 NAND images for NOR+NAND is not allowed")
               elif img_size > part_info.length:
                   error("img size is larger than part. len in '%s'" % section)
            else:
                if part_info != None:
                    if (img_size > 0):
                        if img_size > (part_info.length * self.flinfo.blocksize):
                            error("img size is larger than part. len in '%s'" % section)

            if part_info == None and self.flinfo.type != 'norplusnand':
                continue

            if machid:
                script.start_if("machid", machid)

            script.start_activity("Flashing %s:" % section)
            if self.ipq_nand: script.switch_layout(layout)
            if img_size > 0:
                if ((self.flinfo.type == 'nand' or self.flinfo.type == 'emmc') and (size != img_size)):
                    filename_pad = filename + ".padded"
                    script.imxtract(section + "-" + sha1(filename_pad))
                else:
                    script.imxtract(section + "-" + sha1(filename))

            if part_info == None:
                if self.flinfo.type == 'norplusnand':
                    offset = count * Pack.norplusnand_rootfs_img_size
                    part_size = Pack.norplusnand_rootfs_img_size
                    script.nand_write(offset, part_size, img_size)
                    count = count + 1
            else:
                offset = part_info.offset
                script.erase(offset, part_info.length)
                script.write(offset, img_size, yaffs)
            script.finish_activity()

            if machid:
                script.end_if()

        if part_info == None and self.flinfo.type != 'norplusnand':
            print "Flash type is norplusemmc"
        else:
            script.end()

    def __gen_script(self, script_fp, info_fp, script, images, flinfo):
        """Generate the script to flash the multi-image blob.

        script_fp -- file object, to write script to
        info_fp -- file object, to read flashing information from
        script -- Script object, to append the commands to
        images -- list of ImageInfo, appended to, based on images in config
        """
        try:
            info = ConfigParser({"include": "yes"})
            info.readfp(info_fp)
        except ConfigParserError, e:
            error("error parsing info file '%s'" % self.fconf_fname, e)

        self.__gen_flash_script(info, script, flinfo)

        for section in info.sections():
            if info.get(section, "include").lower() in ["0", "no"]:
                continue

            partition = info.get(section, "partition")
            part_info = self.__get_part_info(partition)

            if part_info == None and self.flinfo.type != 'norplusnand':
                continue

            filename = info.get(section, "filename")
            if self.flinfo.type == 'nand':
                img_size = self.__get_img_size(filename)
                size = roundup(img_size, flinfo.pagesize)
                if ( size != img_size ):
                    filename = filename + ".padded"
            if self.flinfo.type == 'emmc':
                img_size = self.__get_img_size(filename)
                size = roundup(img_size, flinfo.blocksize)
                if ( size != img_size ):
                    filename = filename + ".padded"
            image_info = ImageInfo(section + "-" + sha1(filename),
                                   filename, "firmware")
            if filename.lower() != "none":
               if image_info not in images:
                   images.append(image_info)

    def __its_escape(self, string):
        """Return string with ITS special characters escaped.

        string -- string to be escape.

        String in ITS files, consider 'slash' as special
        character. Escape them by prefixing them with a slash.
        """
        return string.replace("\\", "\\\\")

    def __mkimage(self, images):
        """Create the multi-image blob.

        images -- list of ImageInfo, containing images to be part of the blob
        """
        try:
            its_fp = open(self.its_fname, "wb")
        except IOError, e:
            error("error opening its file '%s'" % self.its_fname, e)

        desc = "Flashing %s %x %x"
        desc = desc % (self.flinfo.type, self.flinfo.pagesize,
                       self.flinfo.blocksize)

        image_data = []
        for (section, fname, imtype) in images:
            fname = self.__its_escape(fname)
            subs = dict(name=section, desc=fname, fname=fname, imtype=imtype)
            image_data.append(its_image_tmpl.substitute(subs))

        image_data = "".join(image_data)
        its_data = its_tmpl.substitute(desc=desc, images=image_data)

        its_fp.write(its_data)
        its_fp.close()

        try:
            cmd = ["mkimage", "-f", self.its_fname, self.img_fname]
            ret = subprocess.call(cmd)
            if ret != 0:
                error("failed to create u-boot image from script")
        except OSError, e:
            error("error executing mkimage", e)

    def __create_fnames(self):
        """Populate the filenames."""

        self.scr_fname = os.path.join(self.images_dname, "flash.scr")
        self.its_fname = os.path.join(self.images_dname, "flash.its")

    def __gen_board_script(self, board_section, machid, flinfo,
                           part_fname, fconf_fname, images):
        """Generate the flashing script for one board.

        board_section -- string, board section in board config file
        machid -- string, board machine ID in hex format
        flinfo -- FlashInfo object, contains board specific flash params
        part_fname -- string, partition file specific to the board
        fconf_fname -- string, flash config file specific to the board
        images -- list of ImageInfo, append images used by the board here
        """
        script_fp = open(self.scr_fname, "a")

        try:
            fconf_fp = open(fconf_fname)
        except IOError, e:
            error("error opening flash config file '%s'" % fconf_fname, e)

        if flinfo.type != "emmc":
            mibib = MIBIB(part_fname, flinfo.pagesize, flinfo.blocksize,
                          flinfo.chipsize)
            self.partitions = mibib.get_parts()
        else:
            gpt = GPT(part_fname, flinfo.pagesize, flinfo.blocksize, flinfo.chipsize)
            self.partitions = gpt.get_parts()

        self.flinfo = flinfo
        if flinfo.type == "nand":
            self.ipq_nand = True
            script = NandScript(flinfo, self.ipq_nand)
        elif flinfo.type == "nor" or flinfo.type == "norplusnand":
            self.ipq_nand = False
            script = NorScript(flinfo)
        elif flinfo.type == "emmc":
            self.ipq_nand = False
            script = EmmcScript(flinfo)
        else:
            error("error, flash type unspecified.")

        script.start_if("machid", machid)
        self.__gen_script(script_fp, fconf_fp, script, images, flinfo)
        script.end_if()

        try:
            script_fp.write(script.dumps())
        except IOError, e:
            error("error writing to script '%s'" % script_fp.name, e)

        script_fp.close()

    def __process_board_flash_emmc(self, ftype, board_section, machid, images):
        """Extract board info from config and generate the flash script.

        ftype -- string, flash type 'emmc'
        board_section -- string, board section in config file
        machid -- string, board machine ID in hex format
        images -- list of ImageInfo, append images used by the board here
        """

        pagesize_param = "%s_pagesize" % ftype
        blocksize_param = "%s_blocksize" % "emmc"
        blocks_per_chip_param = "%s_total_blocks" % "emmc"
        part_fname_param = "%s_partition_mbn" % ftype
        fconf_fname_param = "%s_flash_conf" % ftype

        if ftype == "norplusemmc":
            part_fname_param = "%s_gpt_bin" % ftype

        try:
            pagesize = int(self.bconf.get(board_section, pagesize_param))
            blocksize = int(self.bconf.get(board_section, blocksize_param))
            chipsize = int(self.bconf.get(board_section,
                                                 blocks_per_chip_param))
        except ConfigParserError, e:
            error("missing flash info in section '%s'" % board_section, e)
        except ValueError, e:
            error("invalid flash info in section '%s'" % board_section, e)

        if ftype == "norplusemmc":
            ftype = "emmc"

        flinfo = FlashInfo(ftype, pagesize, blocksize, chipsize)

        try:
            part_fname = self.bconf.get(board_section, part_fname_param)
            if not os.path.isabs(part_fname):
                part_fname = os.path.join(self.images_dname, part_fname)
        except ConfigParserError, e:
            error("missing partition file in section '%s'" % board_section, e)

        try:
            fconf_fname = self.bconf.get(board_section, fconf_fname_param)
            if not os.path.isabs(fconf_fname):
                fconf_fname = os.path.join(self.images_dname, fconf_fname)
        except ConfigParserError, e:
            error("missing NAND config in section '%s'" % board_section, e)

        self.__gen_board_script(board_section, machid, flinfo,
                                part_fname, fconf_fname, images)

    def __process_board_flash(self, ftype, board_section, machid, images):
        """Extract board info from config and generate the flash script.

        ftype -- string, flash type 'nand' or 'nor' or 'emmc' or 'norplusnand'
        board_section -- string, board section in config file
        machid -- string, board machine ID in hex format
        images -- list of ImageInfo, append images used by the board here
        """

        pagesize_param = "%s_pagesize" % ftype
        pages_per_block_param = "%s_pages_per_block" % ftype
        blocks_per_chip_param = "%s_total_blocks" % ftype
        part_fname_param = "%s_partition_mbn" % ftype
        fconf_fname_param = "%s_flash_conf" % ftype

        if ftype == "norplusnand" or ftype == "norplusemmc":
            pagesize_param = "%s_pagesize" % "nor"
            pages_per_block_param = "%s_pages_per_block" % "nor"
            blocks_per_chip_param = "%s_total_blocks" % "nor"

        try:
            pagesize = int(self.bconf.get(board_section, pagesize_param))
            pages_per_block = int(self.bconf.get(board_section,
                                                 pages_per_block_param))
            blocks_per_chip = int(self.bconf.get(board_section,
                                                 blocks_per_chip_param))
        except ConfigParserError, e:
            error("missing flash info in section '%s'" % board_section, e)
        except ValueError, e:
            error("invalid flash info in section '%s'" % board_section, e)

        blocksize = pages_per_block * pagesize
        chipsize = blocks_per_chip * blocksize
        if ftype == "norplusemmc":
            ftype = "nor"

        flinfo = FlashInfo(ftype, pagesize, blocksize, chipsize)

        try:
            part_fname = self.bconf.get(board_section, part_fname_param)
            if not os.path.isabs(part_fname):
                part_fname = os.path.join(self.images_dname, part_fname)
        except ConfigParserError, e:
            error("missing partition file in section '%s'" % board_section, e)

        try:
            fconf_fname = self.bconf.get(board_section, fconf_fname_param)
            if not os.path.isabs(fconf_fname):
                fconf_fname = os.path.join(self.images_dname, fconf_fname)
        except ConfigParserError, e:
            error("missing NAND config in section '%s'" % board_section, e)

        self.__gen_board_script(board_section, machid, flinfo,
                                part_fname, fconf_fname, images)

    def __process_board(self, board_section, images):
        try:
            machid = int(self.bconf.get(board_section, "machid"), 0)
            machid = "%x" % machid
        except ConfigParserError, e:
            error("missing machid in section '%s'" % board_section, e)
        except ValueError, e:
            error("invalid machid in section '%s'" % board_section, e)

        try:
            available = self.bconf.get(board_section, "nand_available")
            if available == "true" and self.flash_type == "nand":
                self.__process_board_flash("nand", board_section, machid, images)
        except ConfigParserError, e:
            error("error getting board info in section '%s'" % board_section, e)

        try:
            available = self.bconf.get(board_section, "nor_available")
            if available == "true" and self.flash_type == "nor":
                self.__process_board_flash("nor", board_section, machid, images)
        except ConfigParserError, e:
            error("error getting board info in section '%s'" % board_section, e)

        try:
            available = self.bconf.get(board_section, "emmc_available")
            if available == "true" and self.flash_type == "emmc":
                self.__process_board_flash_emmc("emmc", board_section, machid, images)
        except ConfigParserError, e:
            error("error getting board info in section '%s'" % board_section, e)

        try:
            available = self.bconf.get(board_section, "norplusnand_available")
            if available == "true" and self.flash_type == "norplusnand":
                self.__process_board_flash("norplusnand", board_section, machid, images)
        except ConfigParserError, e:
            error("error getting board info in section '%s'" % board_section, e)

        try:
            available = self.bconf.get(board_section, "norplusemmc_available")
            if available == "true" and self.flash_type == "norplusemmc":
                self.__process_board_flash("norplusemmc", board_section, machid, images)
                self.__process_board_flash_emmc("norplusemmc", board_section, machid, images)
        except ConfigParserError, e:
            error("error getting board info in section '%s'" % board_section, e)



    def main_bconf(self, flash_type, images_dname, out_fname, brdconfig):
        """Start the packing process, using board config.

        flash_type -- string, indicates flash type, 'nand' or 'nor' or 'emmc' or 'norplusnand'
        images_dname -- string, name of images directory
        out_fname -- string, output file path
        """
        self.flash_type = flash_type
        self.images_dname = images_dname
        self.img_fname = out_fname

        self.__create_fnames()

        try:
            os.unlink(self.scr_fname)
        except OSError, e:
            pass

        try:
            bconf_fname = os.path.join(images_dname, brdconfig)
            bconf_fp = open(bconf_fname)
            self.bconf = ConfigParser()
            self.bconf.readfp(bconf_fp)
            bconf_fp.close()
        except IOError, e:
            error("error reading board configuration file", e)
        except ConfigParserError, e:
            error("error parsing board configuration file", e)

        images = []
        for section in self.bconf.sections():
            self.__process_board(section, images)

        images.insert(0, ImageInfo("script", "flash.scr", "script"))
        self.__mkimage(images)

    def main(self, flinfo, images_dname, out_fname, part_fname, fconf_fname, ipq_nand):
        """Start the packing process.

        flinfo -- FlashInfo object, containing flash parameters
        images_dname -- string, name of images directory
        out_fname -- string, output file path
        part_fname -- string, partition file path
        fconf_fname -- string, flash confing file path
        ipq_nand -- bool, indicates whether this is an IPQ device
        """
        self.flinfo = flinfo
        self.images_dname = images_dname
        self.img_fname = out_fname
        self.ipq_nand = ipq_nand

        self.__create_fnames()

        if self.flinfo.type == "nand":
            script = NandScript(self.flinfo, self.ipq_nand)
        elif self.flinfo.type == "nor" or self.flinfo.type == "norplusnand":
            script = NorScript(self.flinfo)
        elif self.flinfo.type == "emmc":
            script = EmmcScript(self.flinfo)
        else:
            error("Invalid flash type specified. It should be 'nand' or 'nor' or 'norplusnand'")

        if not os.path.isabs(part_fname):
            part_fname = os.path.join(self.images_dname, part_fname)

        if not os.path.isabs(fconf_fname):
            self.fconf_fname = os.path.join(self.images_dname, fconf_fname)

        mibib = MIBIB(part_fname, self.flinfo.pagesize, self.flinfo.blocksize,
                      self.flinfo.chipsize)
        self.partitions = mibib.get_parts()

        script.echo("", verbose=True)

        script.start_activity("Check environment:")
        script.check_isset("imgaddr")
        script.check_isset("machid")
        script.finish_activity()

        try:
            info_fp = open(self.fconf_fname)
        except IOError, e:
            error("error opening info file '%s'" % self.fconf_fname, e)

        try:
            scr_fp = open(self.scr_fname, "wb")
        except IOError, e:
            error("error opening script file '%s'" % self.scr_fname, e)

        images = []
        self.__gen_script(scr_fp, info_fp, script, images)
        try:
            scr_fp.write(script.dumps())
        except IOError, e:
            error("error writing to script '%s'" % script_fp.name, e)
        scr_fp.close() # Flush out all written commands

        images.insert(0, ImageInfo("script", "flash.scr", "script"))
        self.__mkimage(images)

class UsageError(Exception):
    """Indicates error in command arguments."""
    pass

class ArgParser(object):
    """Class to parse command-line arguments."""

    DEFAULT_PAGESIZE = 4096
    DEFAULT_PAGES_PER_BLOCK = 64
    DEFAULT_BLOCKS_PER_CHIP = 1024
    DEFAULT_TYPE = "nand"
    DEFAULT_PART_FNAME = "partition.mbn"
    DEFAULT_FCONF_FNAME = "flash.conf"

    def __init__(self):
        self.__pagesize = None
        self.__pages_per_block = None
        self.__blocksize = None
        self.__chipsize = None
        self.__flash_type = None

        self.flash_info = None
        self.images_dname = None
        self.ipq_nand = False
        self.bconf = False
        self.bconf_fname = "boardconfig"
        self.part_fname = None
        self.fconf_fname = None
        self.genitb_fname = None

    def __init_pagesize(self, pagesize):
        """Set the pagesize, from the command line argument.

        pagesize -- string, flash page size

        Raise UsageError, if pagesize is invalid
        """
        if pagesize == None:
            self.__pagesize = ArgParser.DEFAULT_PAGESIZE
        else:
            try:
                self.__pagesize = int(pagesize)
            except ValueError:
                raise UsageError("invalid page size '%s'" % pagesize)

    def __init_blocksize(self, pages_per_block):
        """Set the blocksize, from the command line argument.

        pages_per_block -- string, no. of pages in a flash block

        Raise UsageError, if pages_per_block is invalid
        """
        if pages_per_block == None:
            self.__blocksize = (self.__pagesize
                                * ArgParser.DEFAULT_PAGES_PER_BLOCK)
        else:
            try:
                self.__blocksize = self.__pagesize * int(pages_per_block)
            except ValueError:
                raise UsageError("invalid block size '%s'" % self.__blocksize)

    def __init_chipsize(self, blocks_per_chip):
        """Set the chipsize, from the command line argument.

        blocks_per_chip -- string, no. of blocks in a flash chip

        Raise UsageError, if chips_per_block is invalid
        """
        if blocks_per_chip == None:
            self.__chipsize = (self.__blocksize
                               * ArgParser.DEFAULT_BLOCKS_PER_CHIP)
        else:
            try:
                self.__chipsize = self.__blocksize * int(blocks_per_chip)
            except ValueError:
                raise UsageError("invalid chip size '%s'" % self.__chipsize)

    def __init_flash_info(self):
        """Set flash_info from the parsed flash paramaters."""

        self.flash_info = FlashInfo(self.__flash_type,
                                    self.__pagesize,
                                    self.__blocksize,
                                    self.__chipsize)

    def __init_flash_type(self, flash_type):
        """Set the flash_type, from the command line argument.

        flash_type -- string, nand or nor

        Raise UsageError, if flash_type is invalid
        """

        if flash_type == None:
            self.__flash_type = ArgParser.DEFAULT_TYPE
        elif flash_type in [ "nand", "nor", "emmc", "norplusnand", "norplusemmc" ]:
            self.__flash_type = flash_type
        else:
            raise UsageError("invalid flash type '%s'" % flash_type)

    def __init_part_fname(self, part_fname):
        """Set the partition filename from command line argument

        part_fname -- string, the partition filename
        """

        if part_fname == None:
            self.part_fname = ArgParser.DEFAULT_PART_FNAME
        else:
            self.part_fname = part_fname

    def __init_fconf_fname(self, fconf_fname):
        """Set the flash configuration filename from command line argument

        fconf_fname -- string, the flash configuration filename
        """

        if fconf_fname == None:
            self.fconf_fname = ArgParser.DEFAULT_FCONF_FNAME
        else:
            self.fconf_fname = fconf_fname

    def __init_out_fname(self, out_fname, images_dname, flash_type,
                         pagesize, pages_per_block, blocks_per_chip,
                         bconf):
        """Set the out_fname from the command line argument.

        out_fname -- string, the output filename
        images_dname -- string, the images dirname
        flash_type -- string, the flash type
        pagesize -- int, the flash page size in bytes
        pages_per_block -- int, the flash pages per block
        blocks_per_chip -- int, the flash blocks per chip
        bconf -- bool, whether is board config mode
        """

        if out_fname == None:
            images_dname_norm = os.path.abspath(images_dname)
            if bconf:
                fmt = "%s-%s%simg"
                self.out_fname = fmt % (images_dname_norm, flash_type,
                                        os.path.extsep)
            else:
                fmt = "%s-%s-%d-%d-%d%simg"
                self.out_fname = fmt % (images_dname_norm, flash_type,
                                        pagesize, pages_per_block,
                                        blocks_per_chip, os.path.extsep)
        else:
            if os.path.isabs(out_fname):
                self.out_fname = out_fname
            else:
                images_dname_parent = os.path.dirname(images_dname)
                self.out_fname = os.path.join(images_dname_parent, out_fname)

    def __init_images_dname(self, args):
        """Set the images_dname from the command line argument.

        args -- list of string, command line args after stripping options
        """
        self.images_dname = args[0]

    def parse(self, argv):
        """Start the parsing process, and populate members with parsed value.

        argv -- list of string, the command line arguments
        """
        flash_type = None
        pagesize = None
        pages_per_block = None
        blocks_per_chip = None
        ipq_nand = False
        out_fname = None
        part_fname = None
        fconf_fname = None
        bconf = False
        bconf_fname = None
        genitb_fname = None

        try:
            opts, args = getopt(argv[1:], "Bib:hp:t:o:c:m:f:F:M:")
        except GetoptError, e:
            raise UsageError(e.msg)

        for option, value in opts:
            if option == "-t":
                flash_type = value
            elif option == "-i":
                ipq_nand = True
            elif option == "-p":
                pagesize = value
            elif option == "-b":
                pages_per_block = value
            elif option == '-c':
                blocks_per_chip = value
            elif option == "-o":
                out_fname = value
            elif option == "-m":
                part_fname = value
            elif option == "-f":
                fconf_fname = value
            elif option == "-B":
                bconf = True
            elif option == "-F":
                bconf_fname = value
            elif option == "-M":
                genitb_fname= value

        if len(args) != 1:
            raise UsageError("insufficient arguments")

        self.__init_flash_type(flash_type)
        self.__init_pagesize(pagesize)
        self.__init_blocksize(pages_per_block)
        self.__init_chipsize(blocks_per_chip)
        self.__init_flash_info()
        self.__init_images_dname(args)
        self.__init_out_fname(out_fname, self.images_dname,
                              self.__flash_type, self.__pagesize,
                              self.__blocksize / self.__pagesize,
                              self.__chipsize / self.__blocksize,
                              bconf)
        self.__init_part_fname(part_fname)
        self.__init_fconf_fname(fconf_fname)

        self.ipq_nand = ipq_nand
        self.bconf = bconf
        if bconf_fname != None:
            self.bconf_fname = bconf_fname

        self.genitb_fname = genitb_fname

    def usage(self, msg):
        """Print error message and command usage information.

        msg -- string, the error message
        """
        print "pack: %s" % msg
        print
        print "Usage: pack [options] IDIR"
        print
        print "where IDIR is the path containing the images."
        print
        print "   -t TYPE    specifies partition type, 'nand' or 'nor',"
        print "              default is '%s'." % ArgParser.DEFAULT_TYPE
        print "   -p SIZE    specifies the page size in bytes,"
        print "              default is %d." % ArgParser.DEFAULT_PAGESIZE
        print "   -b COUNT   specifies the pages per block,"
        print "              default is %d." % ArgParser.DEFAULT_PAGES_PER_BLOCK
        print "   -c COUNT   specifies the no. of blocks per chip"
        print "              default is %d." % ArgParser.DEFAULT_BLOCKS_PER_CHIP
        print "   -i         specifies IPQ processor specific NAND layout"
        print "              switch, default disabled."
        print "   -m FILE    specifies the partition filename"
        print "              default is '%s'." % ArgParser.DEFAULT_PART_FNAME
        print "   -f FILE    specifies the flash configuration filename"
        print "              default is '%s'." % ArgParser.DEFAULT_FCONF_FNAME
        print "   -o FILE    specifies the output filename"
        print "              default is IDIR-TYPE-SIZE-COUNT.img"
        print "              if the filename is relative, it is relative"
        print "              to the parent of IDIR."
        print "   -M         specifies script name to build single ITB with multiple dtb's,"
        print "              default disabled."
        print
        print "NOTE: The above invocation method of pack is deprecated."
        print "The new pack invocation uses a board config file to retrieve"
        print "the board specific parameters. The new invocation is provided"
        print "below."
        print
        print "Usage: pack [options] IDIR"
        print
        print "where IDIR is the path containing the images."
        print
        print "   -t TYPE    specifies partition type, 'nand' or 'nor', or 'emmc'"
        print "              default is '%s'." % ArgParser.DEFAULT_TYPE
        print "   -B         specifies board config should be used, for"
        print "              flash parameters."
        print "   -F         specifies board config file name should be used, for"
        print "              flash parameters. Defaults to 'boardconfig'"
        print "   -o FILE    specifies the output filename"
        print "              default is IDIR-TYPE.img"
        print "              if the filename is relative, it is relative"
        print "              to the parent of IDIR."
        print
        print "Pack Version: %s" % version


def main():
    """Main script entry point.

    Created to avoid polluting the global namespace.
    """
    try:
        parser = ArgParser()
        parser.parse(sys.argv)
    except UsageError, e:
        parser.usage(e.args[0])
        sys.exit(1)

    if parser.genitb_fname:
       prc = subprocess.Popen(['sh', parser.genitb_fname])
       prc.wait()

       if prc.returncode != 0:
          print 'ERROR: unable to create ITB'
          return prc.returncode
       else:
          print '...ITB binary created'


    pack = Pack()
    if parser.bconf:
        pack.main_bconf(parser.flash_info.type, parser.images_dname,
                        parser.out_fname, parser.bconf_fname)
    else:
        pack.main(parser.flash_info, parser.images_dname,
                  parser.out_fname, parser.part_fname,
                  parser.fconf_fname, parser.ipq_nand)

class ArgParserTestCase(TestCase):
    def setUp(self):
        self.parser = ArgParser()

    def test_defaults(self):
        self.parser.parse(["pack.py", "itest"])
        self.assertEqual(self.parser.images_dname, "itest")

        fmt = "itest-%s-%d-%d-%d.img"
        expected_fname = fmt % (ArgParser.DEFAULT_TYPE,
                                ArgParser.DEFAULT_PAGESIZE,
                                ArgParser.DEFAULT_PAGES_PER_BLOCK,
                                ArgParser.DEFAULT_BLOCKS_PER_CHIP)
        self.assertEqual(self.parser.out_fname, expected_fname)
        self.assertEqual(self.parser.ipq_nand, False)
        self.assertEqual(self.parser.flash_info.type,
                         ArgParser.DEFAULT_TYPE)
        self.assertEqual(self.parser.flash_info.pagesize,
                         ArgParser.DEFAULT_PAGESIZE)
        self.assertEqual(self.parser.flash_info.blocksize,
                         ArgParser.DEFAULT_PAGES_PER_BLOCK
                         * ArgParser.DEFAULT_PAGESIZE)
        self.assertEqual(self.parser.flash_info.chipsize,
                         ArgParser.DEFAULT_BLOCKS_PER_CHIP
                         * ArgParser.DEFAULT_PAGES_PER_BLOCK
                         * ArgParser.DEFAULT_PAGESIZE)

    def test_ipq_flag(self):
        self.parser.parse(["pack.py", "-i", "itest"])
        self.assertEqual(self.parser.ipq_nand, True)

    def test_invalid_flag(self):
        self.assertRaises(UsageError, self.parser.parse,
                          ["pack.py", "-x", "itest"])

    def test_type_option(self):
        self.parser.parse(["pack.py", "-t", "nor", "itest"])
        self.assertEqual(self.parser.flash_info.type, "nor")

    def test_invalid_type_option(self):
        self.assertRaises(UsageError, self.parser.parse,
                          ["pack.py", "-t", "abcd", "itest"])

    def test_pagesize_option(self):
        self.parser.parse(["pack.py", "-p", "2048", "itest"])
        self.assertEqual(self.parser.flash_info.pagesize, 2048)

    def test_invalid_pagesize_option(self):
        self.assertRaises(UsageError, self.parser.parse,
                          ["pack.py", "-p", "abcd", "itest"])

    def test_pages_per_block_option(self):
        self.parser.parse(["pack.py", "-b", "32", "itest"])
        self.assertEqual(self.parser.flash_info.blocksize,
                         ArgParser.DEFAULT_PAGESIZE * 32)

    def test_invalid_pages_per_block_option(self):
        self.assertRaises(UsageError, self.parser.parse,
                          ["pack.py", "-b", "abcd", "itest"])

    def test_blocks_per_chip_option(self):
        self.parser.parse(["pack.py", "-c", "512", "itest"])
        self.assertEqual(self.parser.flash_info.chipsize,
                         ArgParser.DEFAULT_PAGESIZE
                         * ArgParser.DEFAULT_PAGES_PER_BLOCK
                         * 512)

    def test_out_fname_rel_option(self):
        self.parser.parse(["pack.py", "-o", "abcd", "/tmp/test/itest"])
        self.assertEqual(self.parser.out_fname, "/tmp/test/abcd")

    def test_out_fname_abs_option(self):
        self.parser.parse(["pack.py", "-o", "/tmp/abcd", "/tmp/test/itest"])
        self.assertEqual(self.parser.out_fname, "/tmp/abcd")

    def test_partition_option(self):
        self.parser.parse(["pack.py", "-m", "abcd", "/tmp/test/itest"])
        self.assertEqual(self.parser.part_fname, "abcd")

    def test_flash_conf_option(self):
        self.parser.parse(["pack.py", "-f", "abcd", "/tmp/test/itest"])
        self.assertEqual(self.parser.fconf_fname, "abcd")

class PackTestCase(TestCase):
    def setUp(self):
        self.pack = Pack()
        blocksize = (ArgParser.DEFAULT_PAGESIZE
                     * ArgParser.DEFAULT_PAGES_PER_BLOCK)
        chipsize = blocksize * ArgParser.DEFAULT_BLOCKS_PER_CHIP

        self.flinfo = FlashInfo(ArgParser.DEFAULT_TYPE,
                                ArgParser.DEFAULT_PAGESIZE,
                                blocksize, chipsize)
        self.img_dname = mkdtemp()
        self.img_fname = self.img_dname + ".img"
        self.part_fname = "partition.mbn"
        self.fconf_fname = "flash.conf"

        sbl1_fp = open(os.path.join(self.img_dname, "sbl1.mbn"), "w")
        sbl1_fp.write("#" * blocksize * 2)
        sbl1_fp.close()

        self.__create_partition_mbn(blocksize, chipsize)

    def __create_partition_mbn(self, blocksize, chipsize):
        part_fname = os.path.join(self.img_dname, self.part_fname)

        mibib = MIBIB(part_fname, ArgParser.DEFAULT_PAGESIZE, blocksize,
                      chipsize)

        offset = 0
        part_size = 2 * blocksize
        mibib.add_part(PartInfo("0:SBL1", offset, part_size))

        offset += part_size
        part_size = 2 * blocksize
        mibib.add_part(PartInfo("0:MIBIB", offset, part_size))

        offset += part_size
        part_size = 1 * blocksize
        mibib.add_part(PartInfo("0:SBL2", offset, part_size))

        offset += part_size
        part_size = None
        mibib.add_part(PartInfo("0:FS", offset, part_size))

        mibib.write()

    def __mkconf(self, conf_str):
        conf_fname = os.path.join(self.img_dname, self.fconf_fname)
        conf_fp = open(conf_fname, "w")
        conf_fp.write(conf_str)
        conf_fp.close()

    def tearDown(self):
        rmtree(self.img_dname)
        try:
            os.remove(self.img_fname)
        except OSError:
            pass

    def test_simple(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
""")

        self.pack.main(self.flinfo, self.img_dname, self.img_fname,
                       self.part_fname, self.fconf_fname, ipq_nand=False)
        self.assertEqual(os.path.exists(self.img_fname), True)

    def test_missing_conf(self):
        self.assertRaises(SystemExit,
                          self.pack.main,
                          self.flinfo,
                          self.img_dname,
                          self.img_fname,
                          self.part_fname,
                          self.fconf_fname,
                          ipq_nand=False)

    def test_nand_layout(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
layout = sbl
""")
        self.pack.main(self.flinfo, self.img_dname, self.img_fname,
                       self.part_fname, self.fconf_fname, ipq_nand=True)
        self.assertEqual(os.path.exists(self.img_fname), True)

    def test_invalid_layout(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
layout = abcd
""")
        self.assertRaises(SystemExit,
                          self.pack.main,
                          self.flinfo,
                          self.img_dname,
                          self.img_fname,
                          self.part_fname,
                          self.fconf_fname,
                          ipq_nand=True)

    def test_inconsistent_layout(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
layout = sbl
""")
        self.assertRaises(SystemExit,
                          self.pack.main,
                          self.flinfo,
                          self.img_dname,
                          self.img_fname,
                          self.part_fname,
                          self.fconf_fname,
                          ipq_nand=False)

    def test_invalid_filename(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl10.mbn
""")
        self.assertRaises(SystemExit,
                          self.pack.main,
                          self.flinfo,
                          self.img_dname,
                          self.img_fname,
                          self.part_fname,
                          self.fconf_fname,
                          ipq_nand=False)

    def test_special_chars_in_filename(self):
        self.__mkconf("""
[slash]
partition = 0:SBL1
filename = sb\\l1.mbn
""")

        sbl1_fp = open(os.path.join(self.img_dname, "sb\\l1.mbn"), "w")
        sbl1_fp.write("abcdef")
        sbl1_fp.close()

        self.pack.main(self.flinfo, self.img_dname, self.img_fname,
                       self.part_fname, self.fconf_fname, ipq_nand=False)
        self.assertEqual(os.path.exists(self.img_fname), True)

    def __get_images(self):
        mkimage_output = subprocess.check_output(["mkimage", "-l", self.img_fname])
        return re.findall(r"Image \d+ \((.*)\)", mkimage_output)

    def test_multi_image(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn

[sbl2]
partition = 0:MIBIB
filename = partition.mbn
""")

        self.pack.main(self.flinfo, self.img_dname, self.img_fname,
                       self.part_fname, self.fconf_fname, ipq_nand=False)
        count = len(self.__get_images())
        self.assertEqual(count, 3)

    def test_include(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
include = no

[sbl2]
partition = 0:MIBIB
filename = partition.mbn
""")

        self.pack.main(self.flinfo, self.img_dname, self.img_fname,
                       self.part_fname, self.fconf_fname, ipq_nand=False)
        images = self.__get_images()
        print images
        self.assertTrue("sbl2" in images)
        self.assertTrue("sbl1" not in images)

    def test_yaffs_yes(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
yaffs = yes
""")
        self.pack.main(self.flinfo, self.img_dname, self.img_fname,
                       self.part_fname, self.fconf_fname, ipq_nand=False)

    def test_yaffs_no(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
yaffs = no
""")
        self.pack.main(self.flinfo, self.img_dname, self.img_fname,
                       self.part_fname, self.fconf_fname, ipq_nand=False)

    def test_yaffs_invalid(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
yaffs = abcd
""")
        self.assertRaises(SystemExit,
                          self.pack.main,
                          self.flinfo,
                          self.img_dname,
                          self.img_fname,
                          self.part_fname,
                          self.fconf_fname,
                          ipq_nand=False)

    def test_invalid_partition(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL5
filename = sbl1.mbn
""")
        self.assertRaises(SystemExit,
                          self.pack.main,
                          self.flinfo,
                          self.img_dname,
                          self.img_fname,
                          self.part_fname,
                          self.fconf_fname,
                          ipq_nand=False)

    def test_img_larger_than_partition(self):
        self.__mkconf("""
[sbl2]
partition = 0:SBL2
filename = sbl1.mbn
""")
        self.assertRaises(SystemExit,
                          self.pack.main,
                          self.flinfo,
                          self.img_dname,
                          self.img_fname,
                          self.part_fname,
                          self.fconf_fname,
                          ipq_nand=False)

    def test_machid_in_hex(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
if_machid = 0x152
""")
        self.pack.main(self.flinfo, self.img_dname, self.img_fname,
                       self.part_fname, self.fconf_fname, ipq_nand=False)
        self.assertEqual(os.path.exists(self.img_fname), True)

    def test_machid_in_dec(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
if_machid = 256
""")
        self.pack.main(self.flinfo, self.img_dname, self.img_fname,
                       self.part_fname, self.fconf_fname, ipq_nand=False)
        self.assertEqual(os.path.exists(self.img_fname), True)

    def test_machid_invalid(self):
        self.__mkconf("""
[sbl1]
partition = 0:SBL1
filename = sbl1.mbn
if_machid = xyz
""")
        self.assertRaises(SystemExit,
                          self.pack.main,
                          self.flinfo,
                          self.img_dname,
                          self.img_fname,
                          self.part_fname,
                          self.fconf_fname,
                          ipq_nand=False)

if __name__ == "__main__":
    main()
