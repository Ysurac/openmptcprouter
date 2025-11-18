# Contributing to OpenMPTCProuter Optimized

Thank you for your interest in contributing to OpenMPTCProuter Optimized! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Development Setup](#development-setup)
- [Coding Standards](#coding-standards)
- [Commit Message Guidelines](#commit-message-guidelines)
- [Pull Request Process](#pull-request-process)
- [Testing Requirements](#testing-requirements)
- [Documentation](#documentation)

---

## Code of Conduct

Please read and follow our [Code of Conduct](CODE_OF_CONDUCT.md). We are committed to providing a welcoming and inclusive environment for all contributors.

---

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check existing issues to avoid duplicates. When creating a bug report, include:

**Required Information:**
- OpenMPTCProuter version and kernel (e.g., "6.12")
- Router model and hardware specs
- VPS operating system and version
- Steps to reproduce the issue
- Expected vs. actual behavior
- Relevant logs (use `logread` on router)

**Template:**
```markdown
## Bug Description
Brief description of the issue

## Environment
- Router Model: Banana Pi R4
- OMR Version: v1.2.3
- Kernel: 6.12
- VPS OS: Ubuntu 22.04

## Steps to Reproduce
1. Step one
2. Step two
3. ...

## Expected Behavior
What should happen

## Actual Behavior
What actually happens

## Logs
```
Paste relevant logs here
```

## Additional Context
Screenshots, configuration files, etc.
```

### Suggesting Enhancements

Enhancement suggestions are welcome! Please provide:
- Clear description of the feature
- Use cases and benefits
- Potential implementation approach
- Any drawbacks or considerations

### Your First Code Contribution

New to the project? Look for issues labeled:
- `good-first-issue` - Simple, self-contained tasks
- `help-wanted` - Areas where we need assistance
- `documentation` - Documentation improvements

---

## Development Setup

### Prerequisites

**System Requirements:**
- Linux-based OS (Ubuntu 20.04+ or Debian 11+ recommended)
- 30GB+ free disk space
- 8GB+ RAM (16GB recommended)
- Multi-core CPU (faster builds)

**Required Software:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential asciidoc binutils bzip2 gawk \
    gettext git libncurses5-dev libz-dev patch unzip zlib1g-dev \
    lib32gcc-s1 libc6-dev-i386 subversion flex uglifyjs git-core \
    gcc-multilib p7zip p7zip-full msmtp libssl-dev texinfo \
    libglib2.0-dev xmlto qemu-utils upx libelf-dev autoconf \
    automake libtool autopoint device-tree-compiler python3-pyelftools \
    llvm clang shellcheck
```

### Forking and Cloning

1. Fork the repository on GitHub
2. Clone your fork:
   ```bash
   git clone https://github.com/YOUR_USERNAME/openmptcprouter.git
   cd openmptcprouter
   ```
3. Add upstream remote:
   ```bash
   git remote add upstream https://github.com/spotty118/openmptcprouter.git
   ```

### Building from Source

**Quick build for testing:**
```bash
OMR_TARGET=x86_64 OMR_KERNEL=6.12 ./build.sh
```

**Build specific target:**
```bash
OMR_TARGET=bpi-r4 OMR_KERNEL=6.12 ./build.sh
```

**Available targets:** See `.github/workflows/build.yml` for the full list.

### Development Workflow

1. **Create a feature branch:**
   ```bash
   git checkout -b feature/my-new-feature
   ```

2. **Make changes and test:**
   ```bash
   # Make your changes
   nano build.sh

   # Test build
   OMR_TARGET=x86_64 OMR_KERNEL=6.12 ./build.sh
   ```

3. **Commit changes:**
   ```bash
   git add .
   git commit -m "feat: add new feature"
   ```

4. **Push to your fork:**
   ```bash
   git push origin feature/my-new-feature
   ```

5. **Create Pull Request** on GitHub

---

## Coding Standards

### Shell Scripts

**Style Guide:**
- Use `#!/bin/sh` for portability (not `#!/bin/bash`)
- Set `set -e` to exit on errors
- Quote all variables: `"$VAR"` not `$VAR`
- Use `${VAR}` for clarity when needed
- Add comments for complex logic

**Example:**
```bash
#!/bin/sh
set -e

# Function to validate IP address
validate_ip() {
    local ip="$1"

    if ! echo "$ip" | grep -Eq '^([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
        echo "Error: Invalid IP address format"
        return 1
    fi

    return 0
}

# Main script
VPS_IP="${1:-}"
if [ -z "$VPS_IP" ]; then
    echo "Usage: $0 <vps_ip>"
    exit 1
fi

if validate_ip "$VPS_IP"; then
    echo "Valid IP: $VPS_IP"
fi
```

**Run shellcheck before committing:**
```bash
shellcheck scripts/*.sh
```

### Makefiles

- Use tabs (not spaces) for indentation
- Add comments explaining non-obvious commands
- Follow OpenWrt package Makefile conventions
- Test with `make -n` (dry-run) first

### Configuration Files

- Use consistent formatting
- Add comments for complex settings
- Follow OpenWrt UCI syntax
- Validate with appropriate tools

### Documentation

- Use Markdown for all documentation
- Follow existing formatting style
- Keep line length ≤100 characters
- Add table of contents for long documents
- Use code blocks with language specifiers

**Example:**
````markdown
## Section Title

Brief description of what this section covers.

### Subsection

Instructions with code example:

```bash
# Run this command
./build.sh
```

**Note:** Important information here.
````

---

## Commit Message Guidelines

We follow the [Conventional Commits](https://www.conventionalcommits.org/) specification.

### Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Types

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, no logic change)
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding or updating tests
- `build`: Build system changes
- `ci`: CI/CD changes
- `chore`: Maintenance tasks

### Scope (Optional)

- `build`: Build system (build.sh, Makefiles)
- `scripts`: Shell scripts (VPS, client setup)
- `workflow`: GitHub Actions
- `kernel`: Kernel configurations
- `docs`: Documentation
- `deps`: Dependencies

### Examples

**Good commit messages:**
```
feat(build): add parallel build support with -j$(nproc)

Enable automatic parallel builds by default to reduce build time.
Falls back to single-threaded build if parallel build fails.

Closes #123
```

```
fix(scripts): prevent sed command injection in easy-install.sh

Escape special characters in password before using in sed command.
Replace `/` delimiter with `|` to avoid common injection pattern.

SECURITY: Fixes critical command injection vulnerability
```

```
docs: create comprehensive FAQ.md

Add FAQ covering:
- Installation and setup
- Configuration
- Troubleshooting
- Performance optimization
- Security and privacy

Resolves #45
```

**Bad commit messages:**
```
fix stuff                    # Too vague
updated files                # No description
WIP                          # Work in progress, not final
Fixed bug in build.sh        # Should start with lowercase, use 'fix' type
```

### Commit Message Body

- Explain **what** and **why**, not **how**
- Wrap lines at 72 characters
- Use bullet points for multiple changes
- Reference issues and PRs

---

## Pull Request Process

### Before Submitting

- [ ] Code follows style guidelines
- [ ] All tests pass (if applicable)
- [ ] Documentation updated
- [ ] Commit messages follow guidelines
- [ ] shellcheck passes (for scripts)
- [ ] No merge conflicts with main branch

### PR Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Testing
Describe how you tested your changes:
- Build targets tested: x86_64, bpi-r4
- VPS OS tested: Ubuntu 22.04
- Manual testing steps:
  1. Step 1
  2. Step 2

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings generated
- [ ] Tests added/updated (if applicable)

## Related Issues
Fixes #123
Closes #456
```

### Review Process

1. **Automated Checks:** CI/CD runs automatically
2. **Code Review:** Maintainer reviews changes
3. **Revisions:** Address review feedback
4. **Approval:** Maintainer approves PR
5. **Merge:** PR merged to main branch

**Review Criteria:**
- Code quality and style
- Functionality and correctness
- Test coverage
- Documentation completeness
- Security considerations
- Performance impact

### After Merge

- Delete your feature branch
- Pull latest changes from upstream:
  ```bash
  git checkout main
  git pull upstream main
  ```

---

## Testing Requirements

### Build Testing

**Minimum:** Test your changes on at least one target:
```bash
OMR_TARGET=x86_64 OMR_KERNEL=6.12 ./build.sh
```

**Recommended:** Test on multiple targets if changes affect multiple platforms:
```bash
for target in x86_64 bpi-r4 rpi4; do
    OMR_TARGET=$target OMR_KERNEL=6.12 ./build.sh
done
```

### Script Testing

For script changes:
1. Run shellcheck: `shellcheck scripts/yourscript.sh`
2. Test in clean environment (VM or container)
3. Verify error handling (invalid inputs, missing dependencies)
4. Check for security issues (injection, privilege escalation)

### Manual Testing

For feature changes:
1. Flash image to hardware or VM
2. Run through setup wizard
3. Test all affected functionality
4. Check logs for errors
5. Verify performance

### Integration Testing

For major changes:
1. Test full setup: VPS installation + router configuration
2. Verify all VPN protocols work
3. Test WAN aggregation
4. Check failover behavior
5. Monitor for memory leaks or crashes

---

## Documentation

### When to Update Documentation

Update documentation when:
- Adding new features
- Changing existing behavior
- Fixing bugs that affect user experience
- Adding or removing dependencies
- Changing configuration options

### Documentation Files

- **README.md** - Project overview, quick start
- **SETUP_GUIDE.md** - Detailed setup instructions
- **FAQ.md** - Common questions and answers
- **CONTRIBUTING.md** - This file
- **EMERGENCY_RECOVERY.md** - Recovery procedures
- **Script README files** - Script-specific documentation

### Documentation Style

- Use clear, concise language
- Include code examples
- Add screenshots for UI changes (optional but helpful)
- Keep formatting consistent
- Test all commands/instructions

---

## Getting Help

**Need help contributing?**
- [GitHub Discussions](https://github.com/spotty118/openmptcprouter/discussions) - Ask questions
- [Existing Issues](https://github.com/spotty118/openmptcprouter/issues) - See what others are working on
- [Pull Requests](https://github.com/spotty118/openmptcprouter/pulls) - Learn from ongoing work

**Maintainer Contact:**
- GitHub: [@spotty118](https://github.com/spotty118)
- Issues: Tag maintainer in issue comments

---

## License

By contributing, you agree that your contributions will be licensed under the GPL-3.0 License.

All contributions must:
- Be your original work or properly attributed
- Not violate any third-party licenses
- Include appropriate license headers

---

## Recognition

Contributors are recognized in:
- GitHub contributor list
- Release notes for significant contributions
- Project documentation (for major features)

Thank you for contributing to OpenMPTCProuter Optimized! 🎉
