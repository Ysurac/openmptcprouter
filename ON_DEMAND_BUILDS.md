# On-Demand Build Guide

## Overview

OpenMPTCProuter Optimized now supports **on-demand builds** via GitHub Actions. You can trigger builds for specific platforms and kernel versions without waiting for a full repository build.

## Building Client Images

### Step 1: Navigate to Actions

1. Go to https://github.com/spotty118/openmptcprouter/actions
2. Click on "Build OpenMPTCProuter Optimized Images" workflow

### Step 2: Trigger the Build

1. Click the "Run workflow" button (top right)
2. Select your branch (usually `main`)
3. Choose your options:

   **Target Platform:**
   - Select `all` to build all platforms (default)
   - Or choose a specific platform:
     - `bpi-r4` - Banana Pi R4
     - `bpi-r4-poe` - Banana Pi R4 PoE
     - `rpi4` - Raspberry Pi 4
     - `rpi5` - Raspberry Pi 5
     - `x86_64` - 64-bit x86 systems
     - And many more...

   **Kernel Version:**
   - Select `all` to build both kernel versions (default)
   - Or choose a specific version:
     - `6.6` - Kernel 6.6 (stable)
     - `6.12` - Kernel 6.12 (latest)

4. Click "Run workflow" to start

### Step 3: Download Your Image

1. Wait for the build to complete (typically 30-90 minutes)
2. Click on the completed workflow run
3. Scroll down to "Artifacts"
4. Download the artifact for your platform (e.g., `bpi-r4-6.12`)
5. Extract and flash the image to your device

## Building VPS Images

### Step 1: Navigate to VPS Workflow

1. Go to https://github.com/spotty118/openmptcprouter/actions
2. Click on "Build VPS Images" workflow

### Step 2: Trigger the Build

1. Click "Run workflow"
2. Select VPS distribution type:
   - `debian` - Debian-based VPS
   - `ubuntu` - Ubuntu-based VPS
   - `all` - All distributions

3. Click "Run workflow"

### Step 3: Download VPS Package

1. Wait for build completion (usually < 5 minutes)
2. Download artifacts:
   - `vps-install-package` - Installation scripts
   - `vps-documentation` - Setup instructions

## Example Use Cases

### Building for a Single Device

**Scenario:** You only have a Raspberry Pi 4 and want the latest kernel.

1. Select workflow: "Build OpenMPTCProuter Optimized Images"
2. Run workflow with:
   - Target: `rpi4`
   - Kernel: `6.12`
3. Build time: ~45 minutes (instead of 6+ hours for all platforms)
4. Download: `rpi4-6.12` artifact

### Testing a Specific Configuration

**Scenario:** You want to test BPI-R4 with both kernel versions.

1. Select workflow: "Build OpenMPTCProuter Optimized Images"
2. Run workflow with:
   - Target: `bpi-r4`
   - Kernel: `all`
3. Build time: ~90 minutes
4. Download both artifacts: `bpi-r4-6.6` and `bpi-r4-6.12`

### Setting up a VPS

**Scenario:** You need VPS installation scripts.

1. Select workflow: "Build VPS Images"
2. Run workflow with:
   - VPS type: `ubuntu` (or your VPS distribution)
3. Build time: ~2 minutes
4. Download and deploy to your VPS

## Automatic Builds

Builds are also triggered automatically on every push to the repository. This ensures:

- Latest code is always built
- All platforms are tested
- Release artifacts are up-to-date

## Build Matrix

When you select "all", the following combinations are built:

### Platforms
- **Banana Pi**: R1, R2, R3, R3-Mini, R4, R4-PoE, R64
- **Raspberry Pi**: RPi2, RPi3, RPi4, RPi5
- **Rockchip**: R2S, R4S, R5C, R5S
- **x86**: x86, x86_64
- **GL.iNet**: MT2500, MT3000, MT6000
- **Other**: WRT3200ACM, WRT32X, UBNT-ERX, R7800, and more

### Kernel Versions
- **6.6**: Stable, well-tested
- **6.12**: Latest features, newer hardware support

### Total Combinations
- 31 platforms × 2 kernel versions = **62 build jobs**

## Build Status

You can monitor build progress:

1. Click on the running workflow
2. View individual job status in the matrix
3. Check logs for any specific platform
4. Failed builds don't stop other platforms (continue-on-error)

## Artifacts

Each build produces:

**Client Images:**
- OpenWrt image files (`.img`, `.bin`, etc.)
- Kernel modules
- Package feed

**VPS Images:**
- Installation scripts
- Configuration files
- Documentation

Artifacts are stored for **90 days** and can be downloaded anytime.

## Advanced: API Triggering

You can also trigger builds via GitHub API:

```bash
curl -X POST \
  -H "Authorization: token YOUR_GITHUB_TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/spotty118/openmptcprouter/actions/workflows/build.yml/dispatches \
  -d '{"ref":"main","inputs":{"target":"bpi-r4","kernel":"6.12"}}'
```

## Troubleshooting

### Build Failed

1. Check the logs in the failed job
2. Common issues:
   - Disk space (automatically cleaned)
   - Compiler errors (usually upstream)
   - Network timeout (retry the build)

### Artifact Not Available

1. Ensure build completed successfully
2. Check artifact expiration (90 days)
3. Re-run the workflow if needed

### Wrong Platform Downloaded

1. Check artifact name matches your device
2. Verify kernel version in filename
3. Read the banner in the image after flashing

## Tips

1. **Build Only What You Need**: Select specific platform/kernel to save time
2. **Monitor Progress**: Check Actions tab regularly
3. **Test Before Deploying**: Always test new builds in a safe environment
4. **Keep Artifacts**: Download important builds before 90-day expiration
5. **Use Latest Kernel**: Kernel 6.12 has newest features and hardware support

## Support

For issues with builds:

1. Check workflow logs
2. Review [build.yml](.github/workflows/build.yml) configuration
3. Open an issue with build logs and details
4. Ask in [Discussions](https://github.com/spotty118/openmptcprouter/discussions)
