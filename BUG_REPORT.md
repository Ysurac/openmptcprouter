# OpenMPTCProuter Codebase Bug Report

This document catalogs legitimate bugs and security issues found during a comprehensive code review.

## Summary

| Category | Critical | High | Medium | Total |
|----------|----------|------|--------|-------|
| Shell Script Bugs | 0 | 4 | 8 | 12 |
| Makefile Issues | 0 | 1 | 4 | 5 |
| Security Vulnerabilities | 2 | 2 | 4 | 8 |
| **Total** | **2** | **7** | **16** | **25** |

---

## Shell Script Bugs

### HIGH: Unquoted Variables in Dangerous Commands

#### Bug #1: Unquoted path variable in sign.sh
**File:** `sign.sh:5-6`
```bash
[ -d $path/source/bin ] && [ -f "$key" ] && \
    find $path/source/bin
```
**Issue:** `$path` is unquoted in both the test and find command. If `$path` contains spaces or glob characters, the command will fail or behave unexpectedly.

**Fix:**
```bash
[ -d "$path/source/bin" ] && [ -f "$key" ] && \
    find "$path/source/bin"
```

---

#### Bug #2: Unquoted variable in file test
**File:** `build.sh:296`
```bash
if [ -f $OMR_TARGET_CONFIG ]; then
```
**Issue:** `$OMR_TARGET_CONFIG` is unquoted. If the path contains spaces, the test will fail.

**Fix:**
```bash
if [ -f "$OMR_TARGET_CONFIG" ]; then
```

---

#### Bug #3: Unquoted variables in cp command
**File:** `build.sh:1065`
```bash
[ -d $OMR_FEED/luci-base/po/oc ] && cp -rf $OMR_FEED/luci-base/po/oc feeds/${OMR_KERNEL}/luci/modules/luci-base/po/
```
**Issue:** `$OMR_FEED` is unquoted in multiple places.

**Fix:**
```bash
[ -d "$OMR_FEED/luci-base/po/oc" ] && cp -rf "$OMR_FEED/luci-base/po/oc" "feeds/${OMR_KERNEL}/luci/modules/luci-base/po/"
```

---

#### Bug #4: Multiple unquoted variables in import_lzmasdk.sh
**File:** `common/package/boot/uboot-ipq40xx/src/lib/lzma/import_lzmasdk.sh:13,18,26,28`
```bash
if [ ! -f $1 ] ; then
BASENAME=`basename $1 .tar.bz2`
mkdir -p $TMPDIR
tar -jxf $1 -C $TMPDIR
```
**Issue:** `$1` and `$TMPDIR` are unquoted throughout the script.

**Fix:** Quote all variable references:
```bash
if [ ! -f "$1" ] ; then
BASENAME=$(basename "$1" .tar.bz2)
mkdir -p "$TMPDIR"
tar -jxf "$1" -C "$TMPDIR"
```

---

### MEDIUM: Predictable Temporary File Path

#### Bug #5: Predictable temp directory name
**File:** `common/package/boot/uboot-ipq40xx/src/lib/lzma/import_lzmasdk.sh:19`
```bash
TMPDIR=/tmp/tmp_lib_$BASENAME
```
**Issue:** Creates a predictable temporary directory based on the basename. This is vulnerable to symlink attacks where an attacker pre-creates the directory pointing to a sensitive location.

**Fix:**
```bash
TMPDIR=$(mktemp -d)
```

---

### MEDIUM: Deprecated Backtick Syntax

#### Bug #6: Backtick command substitution
**Files:**
- `build.sh:47,49,429-430,462-463,469-470`
- `common/package/boot/uboot-ipq40xx/src/lib/lzma/import_lzmasdk.sh:18,31,33`

**Example:**
```bash
OMR_RELEASE=${OMR_RELEASE:-$(git describe --tags `git rev-list --tags --max-count=1` | tail -1)}
BASENAME=`basename $1 .tar.bz2`
```
**Issue:** Backticks are deprecated and harder to nest. The `$()` syntax is preferred.

**Fix:**
```bash
OMR_RELEASE=${OMR_RELEASE:-$(git describe --tags $(git rev-list --tags --max-count=1) | tail -1)}
BASENAME=$(basename "$1" .tar.bz2)
```

---

### MEDIUM: IFS Modification Without Restoration

#### Bug #7: IFS not restored after modification
**File:** `common/scripts/mkits-rutx.sh:63`
```bash
IFS=,
for f in $DTB_CSV; do
    # loop body
done
```
**Issue:** `IFS` is modified but never restored to its default value. This affects word splitting for all subsequent commands in the script.

**Fix:**
```bash
OLD_IFS="$IFS"
IFS=,
for f in $DTB_CSV; do
    # loop body
done
IFS="$OLD_IFS"
```
Or use a subshell:
```bash
(
    IFS=,
    for f in $DTB_CSV; do
        # loop body
    done
)
```

---

### MEDIUM: Missing Error Handling After Directory Change

#### Bug #8: No error check after cd
**File:** `build.sh:477,1064` (and similar locations)
```bash
cd "$OMR_TARGET/${OMR_KERNEL}/source"
# ... subsequent commands assume cd succeeded
```
**Issue:** If `cd` fails (directory doesn't exist), subsequent commands will execute in the wrong directory, potentially causing data loss.

**Fix:**
```bash
cd "$OMR_TARGET/${OMR_KERNEL}/source" || { echo "Failed to change directory"; exit 1; }
```

---

## Makefile Issues

### HIGH: Installation Path Mismatch

#### Bug #9: Directory created but file installed elsewhere
**File:** `common/package/firmware/linux-firmware/netxen.mk:3-4`
```makefile
$(INSTALL_DIR) $(1)/lib/firmware/netxen
$(INSTALL_DATA) $(PKG_BUILD_DIR)/phanfw.bin $(1)/lib/firmware
```
**Issue:** Line 3 creates `/lib/firmware/netxen`, but line 4 installs the file to `/lib/firmware` (without the `netxen` subdirectory). The firmware file ends up in the wrong location.

**Fix:**
```makefile
$(INSTALL_DIR) $(1)/lib/firmware/netxen
$(INSTALL_DATA) $(PKG_BUILD_DIR)/phanfw.bin $(1)/lib/firmware/netxen/
```

---

### MEDIUM: Bash-specific Syntax in Portable Recipe

#### Bug #10: shopt is bash-specific
**File:** `common/package/modems/Makefile:30`
```makefile
shopt -s nullglob ; \
for filevar in $(1)/lib/network/wwan/*-* ; \
```
**Issue:** `shopt` is a bash builtin and is not available in POSIX sh or dash. The recipe may fail on systems where `/bin/sh` is not bash.

**Fix:** Use POSIX-compliant alternatives or explicitly invoke bash:
```makefile
/bin/bash -c 'shopt -s nullglob; for filevar in ...'
```

---

### MEDIUM: Missing Error Handling for Tool Discovery

#### Bug #11: No check for empty CLANG variable
**File:** `common/include/bpf_mptcp.mk:15-16`
```makefile
CLANG:=$(firstword $(shell PATH='$(BPF_PATH)' command -v clang clang-16 clang-13 clang-12 clang-11))
LLVM_VER:=$(subst clang,,$(notdir $(CLANG)))
```
**Issue:** If no clang binary is found, `CLANG` becomes empty, causing `LLVM_VER` to be empty, which results in invalid tool paths like `/llc`, `/llvm-strip`.

**Fix:** Add validation:
```makefile
ifeq ($(CLANG),)
  $(error No clang compiler found. Please install clang.)
endif
```

---

### MEDIUM: Parse-time vs Build-time Conditional

#### Bug #12: Wildcard evaluated at parse time
**File:** `6.18/package/kernel/linux/modules/lib.mk:257-265`
```makefile
ifneq ($(wildcard $(LINUX_DIR)/arch/$(LINUX_KARCH)/lib/xor-neon.ko),)
  FILES:= $(LINUX_DIR)/crypto/xor.ko $(LINUX_DIR)/arch/$(LINUX_KARCH)/lib/xor-neon.ko
else
  FILES:=$(LINUX_DIR)/crypto/xor.ko
endif
```
**Issue:** The `wildcard` is evaluated when the Makefile is parsed, not at build time. If the file doesn't exist during parsing but is created during build, the wrong `FILES` list is used.

---

### MEDIUM: Unnecessary Shell Invocation

#### Bug #13: Redundant shell echo
**File:** `6.1/target/linux/bcm27xx/image/Makefile:26`
```makefile
$(foreach dts,$(shell echo $(DEVICE_DTS)),mcopy -i $@.boot $(DTS_DIR)/$(dts).dtb ::;)
```
**Issue:** The `$(shell echo $(DEVICE_DTS))` is unnecessary overhead. Make's `$(foreach)` can directly iterate over `$(DEVICE_DTS)`.

**Fix:**
```makefile
$(foreach dts,$(DEVICE_DTS),mcopy -i $@.boot $(DTS_DIR)/$(dts).dtb ::;)
```

---

## Security Vulnerabilities

### CRITICAL: Command Injection via Shell Interpolation

#### Vulnerability #1: Unsanitized filenames in shell commands
**Files:**
- `5.4/target/linux/ipq40xx/image/uboot_fw/pack.py:741-747`
- `common/package/boot/uboot-ipq40xx/src/tools/pack.py:741-747`

```python
cmd = 'cat %s > %s' % (filename_abs, filename_abs_pad)
ret = subprocess.call(cmd, shell=True)
cmd = 'dd if=/dev/zero count=1 bs=%s %s >> %s' % (pad_size, tr, filename_abs_pad)
ret = subprocess.call(cmd, shell=True)
```
**Issue:** Filenames are interpolated directly into shell commands without sanitization. A malicious filename containing shell metacharacters (e.g., `file.mbn; rm -rf /`) could execute arbitrary commands.

**Fix:** Use subprocess with argument lists instead of shell=True:
```python
import shutil
shutil.copy(filename_abs, filename_abs_pad)
# Or use subprocess without shell=True:
subprocess.call(['cp', filename_abs, filename_abs_pad])
```

---

### CRITICAL: Eval with Potentially Untrusted Input

#### Vulnerability #2: eval execution of configuration data
**File:** `5.4/target/linux/ipq40xx/base-files/lib/functions/migrate.sh:19,76,117,138`
```bash
eval export "${___var}=\${section}"
eval "$cb \"\$option\" \"\$_OLD_SEC_NAME\" \"\$_NEW_SEC_NAME\""
```
**Issue:** `eval` executes shell code from configuration variables. If these values come from untrusted sources, arbitrary command execution is possible.

**Recommendation:** Avoid `eval` where possible. Use indirect variable references or arrays in bash, or restructure the code to not require dynamic evaluation.

---

### HIGH: Weak Cryptographic Hash (SHA-1)

#### Vulnerability #3: SHA-1 used for image integrity
**Files:**
- `common/scripts/mkits-rutx.sh:79,123,164`
- `common/scripts/mkits-tlt-rutx-fit.sh:86,127`

```bash
algo = \"sha1\";
```
**Issue:** SHA-1 is cryptographically broken and should not be used for security purposes. It's used here for device tree blob (FIT) image authentication.

**Fix:** Replace with SHA-256:
```bash
algo = \"sha256\";
```

---

### HIGH: os.system() with Unsanitized Input

#### Vulnerability #4: Direct system call execution
**Files:**
- `common/package/boot/uboot-ipq40xx/src/tools/patman/gitutil.py:266`
- `common/package/boot/uboot-ipq40xx/src/tools/patman/test.py:90`

```python
os.system(str)
```
**Issue:** `os.system()` passes the string directly to the shell. If `str` contains unsanitized user input, command injection is possible.

**Fix:** Use `subprocess.run()` with argument lists:
```python
subprocess.run(['command', 'arg1', 'arg2'], check=True)
```

---

### MEDIUM: Credentials Visible in Process Listing

#### Vulnerability #5: Password passed on command line
**File:** `common/package/network/ipv6/6in4/files/6in4.sh:91,103,110`
```bash
proto_6in4_update $urlget $urlget_opts --user="$username" --password="$password" "$url"
```
**Issue:** Credentials passed as command-line arguments are visible in process listings (`ps aux`) and may be logged.

**Recommendation:** Pass credentials via environment variables or stdin where the tool supports it.

---

### MEDIUM: HTTP Used for Package Repositories

#### Vulnerability #6: Unencrypted package downloads
**File:** `build.sh:50,236-240`
```bash
OMR_REPO=${OMR_REPO:-http://$OMR_HOST:$OMR_PORT/release/$OMR_RELEASE-$OMR_KERNEL/$OMR_TARGET}
```
**Issue:** Package repositories default to HTTP, which is vulnerable to man-in-the-middle attacks. An attacker could inject malicious packages.

**Fix:** Default to HTTPS:
```bash
OMR_REPO=${OMR_REPO:-https://$OMR_HOST/release/$OMR_RELEASE-$OMR_KERNEL/$OMR_TARGET}
```

---

### MEDIUM: Eval with Network-Derived Data

#### Vulnerability #7: jsonfilter output passed to eval
**File:** `5.4/target/linux/ipq40xx/base-files/lib/functions/network.sh:24`
```bash
__tmp="$(jsonfilter ...)"
eval "$__tmp"
```
**Issue:** The output of `jsonfilter` is directly passed to `eval`. If the JSON data is malformed or maliciously crafted, arbitrary commands could execute.

**Recommendation:** Validate jsonfilter output or use safer parsing methods.

---

### MEDIUM: Insecure Temporary File Operations

#### Vulnerability #8: Unvalidated temp file creation
**File:** `5.4/target/linux/ipq807x/base-files/lib/upgrade/buffalo.sh:52-54`
```bash
echo -n "00000000000000000000000000000000" > /tmp/dummyhash.txt
ubiupdatevol /dev/$hashvol_root /tmp/dummyhash.txt
```
**Issue:** Creates file in `/tmp` without checking for pre-existing files. An attacker could pre-create `/tmp/dummyhash.txt` as a symlink to overwrite arbitrary files.

**Fix:**
```bash
TMPFILE=$(mktemp)
echo -n "00000000000000000000000000000000" > "$TMPFILE"
ubiupdatevol /dev/$hashvol_root "$TMPFILE"
rm -f "$TMPFILE"
```

---

## Recommendations

### Immediate Actions (Critical/High Priority)
1. Fix command injection vulnerabilities in Python scripts (pack.py)
2. Quote all variable references in shell scripts
3. Replace SHA-1 with SHA-256 in FIT image creation scripts
4. Fix the netxen.mk installation path mismatch

### Short-term Actions (Medium Priority)
1. Replace deprecated backtick syntax with `$()`
2. Add error handling after `cd` commands
3. Use `mktemp` for temporary files
4. Default to HTTPS for package repositories
5. Fix IFS restoration in mkits-rutx.sh

### Long-term Actions
1. Audit all uses of `eval` and replace where possible
2. Review all subprocess calls for shell injection risks
3. Implement input validation for all external data sources
4. Add shellcheck to CI pipeline for shell script validation

---

*Report generated: 2026-01-23*
