# Alpha OS 2.0 - Modern Educational Operating System

<p align="center">
  <img src="https://img.shields.io/badge/OS-AlphaOS-7F3FBF?style=for-the-badge&logo=linux&logoColor=white" alt="Alpha OS Logo" />
  <img src="https://img.shields.io/badge/Version-2.0.0-blue?style=for-the-badge" alt="Version" />
  <img src="https://img.shields.io/badge/Build-Year-2026-orange?style=for-the-badge" />
</p>

<h1 align="center">💻 Alpha OS 2.0 - Modern Kernel 🚀</h1>

> **Where Security Meets Innovation!** 🌟
> *Built with passion for education and modern OS development practices.*

---

## 🧠 What is Alpha OS 2.0?

**Alpha OS 2.0** is a modern, educational operating system built with **GCC**, featuring:

- 🔒 Secure root access and permission-based operations
- 🎨 Linux-style color-coded shell interface
- 🛠️ Comprehensive command set
- 📁 Advanced file system
- ⚡ **Process Scheduling & Multitasking**
- 🧠 **Virtual Memory & Paging**
- 🔍 **CPU Detection & Features**
- 📊 **Advanced Memory Management (SLAB allocator)**

---

## ✨ Features at a Glance

| Feature                    | Description                                                |
|---------------------------|------------------------------------------------------------|
| 🧑‍💻 Root Shell Access      | Enter administrative mode with red Linux-style prompts     |
| 🔐 Enhanced Security       | User vs. root separation, password protection              |
| 📂 Advanced File System    | Shadow files, permission-based ops, structured hierarchy   |
| 💬 Smart Shell Commands    | User management, filesystem nav, system info & utilities   |
| 🎨 Visual Design           | Inspired by Linux - green user, red root, smart path UX    |
| ⚡ Process Scheduling      | Round-robin scheduler, process creation/destruction        |
| 🧠 Virtual Memory         | Page tables, page fault handling, memory mapping           |
| 🔍 CPU Detection          | CPUID, MSR access, APIC, interrupts                       |
| 📊 SLAB Allocator         | Fast kernel object allocation                              |

---

## 🆕 What's New in 2.0

- **Process Scheduling**: Full multitasking support with process control blocks
- **Virtual Memory**: Paging, page tables, memory mapping
- **CPU Subsystem**: CPUID detection, control registers, APIC support
- **Memory Management**: SLAB allocator, buddy system ready
- **System Calls**: Extended syscalls for process/memory management
- **Enhanced Debugging**: Panic handler, register dump

---

## 🔧 Getting Started

```bash
make install-deps     # Install dependencies
make test             # Run tests
make shell-test       # Try the interactive shell
make iso              # Build Alpha OS
make run              # Run in QEMU
```

## 📋 New Commands

| Command | Description |
|---------|-------------|
| `sysinfo` | Display system information |
| `meminfo` | Display memory statistics |
| `cpuinfo` | Display CPU information |
| `ps` | List running processes |
| `kill <pid>` | Terminate a process |
| `reboot` | Reboot the system |

---

## 🏗️ Architecture

```
ALPHA-OS/
├── arch/x86/         # x86 architecture code
│   ├── boot.asm      # Boot sector
│   └── linker.ld     # Linker script
├── kernel/           # Kernel source
│   ├── cpu.c         # CPU detection & management
│   ├── memory.c      # Memory management
│   ├── scheduler.c   # Process scheduling
│   ├── system.c      # System functions
│   └── ...
├── libc/             # Standard C library
├── include/          # Header files
├── scripts/          # Build scripts
└── Makefile          # Build system
```

---

Built with ❤️ for OS development education.

---

## 🏗️ Multi-Architecture Support

Alpha OS 2.0 supports multiple CPU architectures:

| Architecture | Support Level | QEMU Target |
|--------------|---------------|-------------|
| x86 (32-bit) | Full | qemu-system-i386 |
| ARMv7 | Basic | qemu-system-arm |
| ARM64 (AArch64) | Basic | qemu-system-aarch64 |
| RISC-V (64-bit) | Basic | qemu-system-riscv64 |

### Building for Different Architectures

```bash
# Default x86
make                    # Build x86 kernel
make run               # Run in QEMU

# ARM
make ARCH=arm          # Build ARM kernel
make ARCH=arm run      # Run ARM in QEMU

# ARM64
make ARCH=arm64        # Build ARM64 kernel

# RISC-V
make ARCH=riscv       # Build RISC-V kernel
```

### Project Structure

```
ALPHA-OS/
├── arch/
│   ├── common/        # Architecture abstraction layer
│   │   ├── arch.h    # Common interface definitions
│   │   └── arch.c    # Implementation
│   ├── x86/          # x86 (32-bit) specific code
│   │   ├── boot.asm  # Boot loader
│   │   └── linker.ld # Linker script
│   ├── arm/          # ARM specific code
│   │   ├── boot.S    # Boot loader
│   │   ├── cpu.h     # ARM CPU definitions
│   │   └── linker.ld # Linker script
│   └── riscv/        # RISC-V specific code
│       ├── boot.S    # Boot loader
│       ├── cpu.h     # RISC-V CPU definitions
│       └── linker.ld # Linker script
├── kernel/           # Kernel source (architecture-independent)
├── libc/             # Standard C library
├── include/          # Header files
├── Makefile          # Multi-arch build system
└── README.md
```
