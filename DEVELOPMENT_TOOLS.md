# 🛠️ Development Tools & QoL Enhancements

OpenMPTCProuter Optimized includes several quality of life tools to make building and managing the project easier.

## 🎯 Quick Reference

```bash
# Interactive guided build
./scripts/omr-build-helper.sh --interactive

# Check system health
./scripts/omr-health-check.sh

# Backup configuration before changes
./scripts/omr-config-manager.sh backup

# Clean up disk space
./scripts/omr-clean.sh analyze
```

## 📦 Available Tools

### 🏗️ Build Tools

#### **omr-build-helper.sh** - Enhanced Build Wrapper
Enhanced build wrapper with progress tracking, time measurement, and statistics.

```bash
# Interactive platform selection
./scripts/omr-build-helper.sh --interactive

# Build specific target with logging
./scripts/omr-build-helper.sh --target x86_64 --kernel 6.12 --log

# View build statistics
./scripts/omr-build-helper.sh --stats
```

**Features:**
- ⏱️ Build time tracking
- 📊 Build history and statistics
- ✅ Pre-build validation
- 📝 Optional build logging
- 🎯 Success/failure tracking

---

#### **omr-select-platform.sh** - Interactive Platform Selection
User-friendly menu for selecting build platform and kernel version.

```bash
# Interactive selection with categorized platforms
./scripts/omr-select-platform.sh

# List all available platforms
./scripts/omr-select-platform.sh --list

# Pre-select kernel version
./scripts/omr-select-platform.sh --kernel 6.12
```

**Features:**
- 📋 Categorized platform display (ARM SBC, x86, MediaTek, Rockchip, etc.)
- 📝 Platform descriptions
- 🎛️ Kernel version selection
- 🔗 Direct build integration

---

#### **omr-clean.sh** - Build Cleanup Utility
Smart cleanup utility to free up disk space with multiple cleanup levels.

```bash
# Analyze disk usage first
./scripts/omr-clean.sh analyze

# Light cleanup - safe, removes tmp files (~1-5GB)
./scripts/omr-clean.sh light

# Moderate cleanup - removes builds, keeps downloads (~5-15GB)
./scripts/omr-clean.sh moderate

# Deep cleanup - removes everything (~15-30GB)
./scripts/omr-clean.sh deep

# Clean specific kernel only
./scripts/omr-clean.sh moderate --kernel 6.12

# Dry run to see what would be deleted
./scripts/omr-clean.sh deep --dry-run
```

**Cleanup Levels:**
- **light**: Temporary files, caches, old logs (safe)
- **moderate**: Build artifacts, staging directories
- **deep**: Downloads, feeds, all build files
- **nuclear**: Complete clean like fresh checkout

---

### ⚙️ Configuration Management

#### **omr-config-manager.sh** - Configuration Backup & Restore
Manage build configurations with backup/restore functionality.

```bash
# Create backup of current configuration
./scripts/omr-config-manager.sh backup

# List all available backups
./scripts/omr-config-manager.sh list

# Restore a specific backup
./scripts/omr-config-manager.sh restore 20250118_143022

# Compare current config with backup
./scripts/omr-config-manager.sh diff 20250118_143022

# Export backup to shareable file
./scripts/omr-config-manager.sh export 20250118_143022

# Import backup from file
./scripts/omr-config-manager.sh import omr-config-backup.tar.gz
```

**What Gets Backed Up:**
- ✅ All `config-*` platform configurations
- ✅ `.config` files from build directories
- ✅ `feeds.conf` customizations
- ✅ Custom patches

---

### 🏥 System Diagnostics

#### **omr-health-check.sh** - Build Environment Health Check
Comprehensive health check for your build environment.

```bash
# Full health check
./scripts/omr-health-check.sh

# Quick check (essentials only)
./scripts/omr-health-check.sh --quick

# Attempt to fix common issues automatically
./scripts/omr-health-check.sh --fix
```

**Checks Performed:**
- ✅ System dependencies (git, gcc, python3, etc.)
- ✅ Disk space availability (30GB minimum)
- ✅ Git repository status
- ✅ Build environment setup
- ✅ Kernel directories
- ✅ Script permissions
- ✅ Documentation files

---

### 📚 Shared Library

#### **omr-lib.sh** - Common Functions Library
Shared library providing consistent UX across all scripts.

**Features:**
- 🎨 Color-coded output functions
- 📊 Progress indicators
- ❓ User input helpers
- ✅ Validation utilities
- 📏 Formatting functions

**Usage in your scripts:**
```bash
#!/bin/sh
. "$(dirname "$0")/omr-lib.sh"

omr_log_header "My Script"
omr_log_success "Operation completed!"
omr_log_error "Something went wrong"
omr_log_warning "Be careful here"
omr_log_info "Just FYI"

if omr_ask_yes_no "Continue?" "y"; then
    omr_progress_start "Doing something"
    # ... your code ...
    omr_progress_done
fi
```

---

## 🚀 Recommended Workflows

### First-Time Setup
```bash
# 1. Check if your system is ready
./scripts/omr-health-check.sh

# 2. Fix any issues found
./scripts/omr-health-check.sh --fix

# 3. Start an interactive build
./scripts/omr-build-helper.sh --interactive
```

### Before Making Configuration Changes
```bash
# 1. Backup current configuration
./scripts/omr-config-manager.sh backup

# 2. Make your changes
vim config-x86_64

# 3. If something goes wrong, restore
./scripts/omr-config-manager.sh restore <backup-name>
```

### Regular Maintenance
```bash
# Check disk usage
./scripts/omr-clean.sh analyze

# View build history
./scripts/omr-build-helper.sh --stats

# List configuration backups
./scripts/omr-config-manager.sh list

# Clean up old builds
./scripts/omr-clean.sh moderate
```

### Typical Build Workflow
```bash
# 1. Check system health
./scripts/omr-health-check.sh --quick

# 2. Backup current config
./scripts/omr-config-manager.sh backup

# 3. Check disk space
./scripts/omr-clean.sh analyze

# 4. Clean if needed
./scripts/omr-clean.sh moderate --kernel 6.12

# 5. Start build with tracking and logging
./scripts/omr-build-helper.sh --target x86_64 --kernel 6.12 --log
```

---

## 💡 Tips & Tricks

### Disk Space Management
- Run `omr-clean.sh analyze` regularly to monitor space usage
- Use `moderate` cleanup level to free space while keeping downloads
- The `deep` cleanup removes everything but takes longer to rebuild

### Configuration Safety
- Always backup before experimenting: `./scripts/omr-config-manager.sh backup`
- Use `diff` command to see what changed: `./scripts/omr-config-manager.sh diff <backup>`
- Export backups for sharing: `./scripts/omr-config-manager.sh export <backup>`

### Build Optimization
- Use `--log` flag to save build output for troubleshooting
- Check build stats to identify slow builds: `./scripts/omr-build-helper.sh --stats`
- Pre-build validation catches issues early

### Interactive Mode
- New users should use `--interactive` flag for guided experience
- Platform selection shows helpful descriptions
- Kernel selection explains the differences

---

## 🎯 Environment Variables

These tools respect standard OpenMPTCProuter environment variables:

```bash
# Set target platform
export OMR_TARGET=x86_64

# Set kernel version
export OMR_KERNEL=6.12

# Keep binary files after build
export OMR_KEEPBIN=yes

# Build firmware images
export OMR_IMG=yes

# Package selection
export OMR_PACKAGES=full
```

---

## 🔧 For Script Developers

When creating new scripts for this project:

1. **Use the shared library** for consistent output:
   ```bash
   . "$(dirname "$0")/omr-lib.sh"
   ```

2. **Include help text** with `--help` option

3. **Add validation** using helper functions:
   ```bash
   omr_require_command git
   omr_validate_directory /path/to/dir
   ```

4. **Use color-coded output**:
   ```bash
   omr_log_success "Success message"
   omr_log_error "Error message"
   omr_log_warning "Warning message"
   ```

5. **Make scripts executable**:
   ```bash
   chmod +x your-script.sh
   ```

6. **Test with shellcheck**:
   ```bash
   ./scripts/validate-scripts.sh
   ```

---

## 📖 Related Documentation

- [Main README](README.md) - Project overview
- [Setup Guide](SETUP_GUIDE.md) - Installation instructions
- [VPS Setup](scripts/README.md) - VPS installation guide
- [FAQ](FAQ.md) - Frequently asked questions
- [Contributing](CONTRIBUTING.md) - Contribution guidelines

---

## 🆘 Getting Help

If you encounter issues with these tools:

1. Run health check: `./scripts/omr-health-check.sh`
2. Try the fix option: `./scripts/omr-health-check.sh --fix`
3. Check the [FAQ](FAQ.md)
4. Open an issue on [GitHub](https://github.com/spotty118/openmptcprouter/issues)

---

## ⭐ Features Summary

| Tool | Purpose | Key Benefit |
|------|---------|-------------|
| omr-build-helper.sh | Enhanced build wrapper | Time tracking, statistics, validation |
| omr-select-platform.sh | Platform selection | User-friendly guided selection |
| omr-clean.sh | Disk space management | Smart cleanup with multiple levels |
| omr-config-manager.sh | Config backup/restore | Safe experimentation |
| omr-health-check.sh | System diagnostics | Catch issues early |
| omr-lib.sh | Shared functions | Consistent UX across tools |

---

**Note:** All these enhancements are **optional** and **non-breaking**. Existing workflows continue to work exactly as before. These tools are here to make your life easier! 🎉
