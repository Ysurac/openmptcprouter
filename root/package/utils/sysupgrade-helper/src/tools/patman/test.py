#
# Copyright (c) 2011 The Chromium OS Authors.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

import os
import tempfile
import unittest

import checkpatch
import gitutil
import patchstream
import series


class TestPatch(unittest.TestCase):
    """Test this program

    TODO: Write tests for the rest of the functionality
    """

    def testBasic(self):
        """Test basic filter operation"""
        data='''

From 656c9a8c31fa65859d924cd21da920d6ba537fad Mon Sep 17 00:00:00 2001
From: Simon Glass <sjg@chromium.org>
Date: Thu, 28 Apr 2011 09:58:51 -0700
Subject: [PATCH (resend) 3/7] Tegra2: Add more clock support

This adds functions to enable/disable clocks and reset to on-chip peripherals.

BUG=chromium-os:13875
TEST=build U-Boot for Seaboard, boot

Change-Id: I80fe1d0c0b7dd10aa58ce5bb1d9290b6664d5413

Review URL: http://codereview.chromium.org/6900006

Signed-off-by: Simon Glass <sjg@chromium.org>
---
 arch/arm/cpu/armv7/tegra2/Makefile         |    2 +-
 arch/arm/cpu/armv7/tegra2/ap20.c           |   57 ++----
 arch/arm/cpu/armv7/tegra2/clock.c          |  163 +++++++++++++++++
'''
        expected='''

From 656c9a8c31fa65859d924cd21da920d6ba537fad Mon Sep 17 00:00:00 2001
From: Simon Glass <sjg@chromium.org>
Date: Thu, 28 Apr 2011 09:58:51 -0700
Subject: [PATCH (resend) 3/7] Tegra2: Add more clock support

This adds functions to enable/disable clocks and reset to on-chip peripherals.

Signed-off-by: Simon Glass <sjg@chromium.org>
---
 arch/arm/cpu/armv7/tegra2/Makefile         |    2 +-
 arch/arm/cpu/armv7/tegra2/ap20.c           |   57 ++----
 arch/arm/cpu/armv7/tegra2/clock.c          |  163 +++++++++++++++++
'''
        out = ''
        inhandle, inname = tempfile.mkstemp()
        infd = os.fdopen(inhandle, 'w')
        infd.write(data)
        infd.close()

        exphandle, expname = tempfile.mkstemp()
        expfd = os.fdopen(exphandle, 'w')
        expfd.write(expected)
        expfd.close()

        patchstream.FixPatch(None, inname, series.Series(), None)
        rc = os.system('diff -u %s %s' % (inname, expname))
        self.assertEqual(rc, 0)

        os.remove(inname)
        os.remove(expname)

    def GetData(self, data_type):
        data='''
From 4924887af52713cabea78420eff03badea8f0035 Mon Sep 17 00:00:00 2001
From: Simon Glass <sjg@chromium.org>
Date: Thu, 7 Apr 2011 10:14:41 -0700
Subject: [PATCH 1/4] Add microsecond boot time measurement

This defines the basics of a new boot time measurement feature. This allows
logging of very accurate time measurements as the boot proceeds, by using
an available microsecond counter.

%s
---
 README              |   11 ++++++++
 common/bootstage.c  |   50 ++++++++++++++++++++++++++++++++++++
 include/bootstage.h |   71 +++++++++++++++++++++++++++++++++++++++++++++++++++
 include/common.h    |    8 ++++++
 5 files changed, 141 insertions(+), 0 deletions(-)
 create mode 100644 common/bootstage.c
 create mode 100644 include/bootstage.h

diff --git a/README b/README
index 6f3748d..f9e4e65 100644
--- a/README
+++ b/README
@@ -2026,6 +2026,17 @@ The following options need to be configured:
		example, some LED's) on your board. At the moment,
		the following checkpoints are implemented:

+- Time boot progress
+		CONFIG_BOOTSTAGE
+
+		Define this option to enable microsecond boot stage timing
+		on supported platforms. For this to work your platform
+		needs to define a function timer_get_us() which returns the
+		number of microseconds since reset. This would normally
+		be done in your SOC or board timer.c file.
+
+		You can add calls to bootstage_mark() to set time markers.
+
 - Standalone program support:
		CONFIG_STANDALONE_LOAD_ADDR

diff --git a/common/bootstage.c b/common/bootstage.c
new file mode 100644
index 0000000..2234c87
--- /dev/null
+++ b/common/bootstage.c
@@ -0,0 +1,50 @@
+/*
+ * Copyright (c) 2011, Google Inc. All rights reserved.
+ *
+ * See file CREDITS for list of people who contributed to this
+ * project.
+ *
+ * This program is free software; you can redistribute it and/or
+ * modify it under the terms of the GNU General Public License as
+ * published by the Free Software Foundation; either version 2 of
+ * the License, or (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+ * MA 02111-1307 USA
+ */
+
+
+/*
+ * This module records the progress of boot and arbitrary commands, and
+ * permits accurate timestamping of each. The records can optionally be
+ * passed to kernel in the ATAGs
+ */
+
+#include <common.h>
+
+
+struct bootstage_record {
+	uint32_t time_us;
+	const char *name;
+};
+
+static struct bootstage_record record[BOOTSTAGE_COUNT];
+
+uint32_t bootstage_mark(enum bootstage_id id, const char *name)
+{
+	struct bootstage_record *rec = &record[id];
+
+	/* Only record the first event for each */
+%sif (!rec->name) {
+		rec->time_us = (uint32_t)timer_get_us();
+		rec->name = name;
+	}
+%sreturn rec->time_us;
+}
--
1.7.3.1
'''
        signoff = 'Signed-off-by: Simon Glass <sjg@chromium.org>\n'
        tab = '	'
        if data_type == 'good':
            pass
        elif data_type == 'no-signoff':
            signoff = ''
        elif data_type == 'spaces':
            tab = '   '
        else:
            print 'not implemented'
        return data % (signoff, tab, tab)

    def SetupData(self, data_type):
        inhandle, inname = tempfile.mkstemp()
        infd = os.fdopen(inhandle, 'w')
        data = self.GetData(data_type)
        infd.write(data)
        infd.close()
        return inname

    def testCheckpatch(self):
        """Test checkpatch operation"""
        inf = self.SetupData('good')
        result, problems, err, warn, lines, stdout = checkpatch.CheckPatch(inf)
        self.assertEqual(result, True)
        self.assertEqual(problems, [])
        self.assertEqual(err, 0)
        self.assertEqual(warn, 0)
        self.assertEqual(lines, 67)
        os.remove(inf)

        inf = self.SetupData('no-signoff')
        result, problems, err, warn, lines, stdout = checkpatch.CheckPatch(inf)
        self.assertEqual(result, False)
        self.assertEqual(len(problems), 1)
        self.assertEqual(err, 1)
        self.assertEqual(warn, 0)
        self.assertEqual(lines, 67)
        os.remove(inf)

        inf = self.SetupData('spaces')
        result, problems, err, warn, lines, stdout = checkpatch.CheckPatch(inf)
        self.assertEqual(result, False)
        self.assertEqual(len(problems), 2)
        self.assertEqual(err, 0)
        self.assertEqual(warn, 2)
        self.assertEqual(lines, 67)
        os.remove(inf)


if __name__ == "__main__":
    unittest.main()
    gitutil.RunTests()
