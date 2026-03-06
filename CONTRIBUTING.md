# Contributing to Alpha OS

Thank you for your interest in contributing to Alpha OS!

## 📋 Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [提交 Pull Request](#pull-requests)
- [Reporting Bugs](#reporting-bugs)

## 📜 Code of Conduct

Please be respectful and professional when contributing. We follow the standard open-source code of conduct.

## 🚀 Getting Started

### Prerequisites

- GCC or Clang compiler
- NASM assembler
- QEMU (for testing)
- Git

### Clone and Build

```bash
git clone https://github.com/DDDDprog/ALPHA-OS-.git
cd ALPHA-OS-
make                    # Build kernel
make test             # Run tests
make run              # Run in QEMU
```

### Building for Different Architectures

```bash
make ARCH=x86     # Default x86
make ARCH=arm     # ARM
make ARCH=arm64   # ARM64
make ARCH=riscv   # RISC-V
```

## 🔧 Development Workflow

1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature/amazing-feature`
3. **Make** your changes
4. **Test** your changes: `make test`
5. **Commit** your changes: `git commit -m 'Add amazing feature'`
6. **Push** to the branch: `git push origin feature/amazing-feature`
7. **Open** a Pull Request

## 📝 Coding Standards

### Code Style

We follow the Linux kernel coding style with some modifications:

- Use 4 spaces for indentation (no tabs)
- Maximum line width: 100 characters
- Braces on the same line for functions, new line for control structures
- Use meaningful variable names
- Comment complex code

### File Organization

```
kernel/          - Core kernel code
libc/            - Standard C library
include/         - Header files
arch/            - Architecture-specific code
scripts/         - Build and test scripts
```

### Commit Messages

- Use imperative mood: "Add feature" not "Added feature"
- First line: 50 characters or less
- Detailed description after blank line
- Reference issues: "Fixes #123"

### Example Commit

```
Add process scheduling support

Implemented round-robin scheduler with process control blocks.
Supports process creation, destruction, and context switching.

Fixes #42
```

## 🔍 Pull Requests

### Before Submitting

1. Test your changes on all supported architectures
2. Run the test suite: `make test`
3. Check code formatting: Use `.clang-format`
4. Ensure no compiler warnings

### PR Description

- **Title**: Clear and concise
- **Description**: Explain what and why
- **Tests**: Describe how to test
- **Screenshots**: If applicable

## 🐛 Reporting Bugs

Use GitHub Issues to report bugs. Include:

- Description of the bug
- Steps to reproduce
- Expected behavior
- Actual behavior
- Environment (OS, compiler version, etc.)

## 💬 Getting Help

- GitHub Discussions
- Open an issue for bugs
- Email: (add your contact)

---

Thank you for contributing to Alpha OS! 🎉
