# Alpha OS - Docker Build Environment
# This container provides all necessary tools to build Alpha OS

FROM ubuntu:22.04

LABEL maintainer="Alpha OS Team"
LABEL description="Build environment for Alpha OS"

# ============================================================
# Environment Variables
# ============================================================
ENV DEBIAN_FRONTEND=noninteractive
ENV MAKEFLAGS=-j$(nproc)

# ============================================================
# Install Build Tools
# ============================================================
RUN apt-get update && apt-get install -y \
    # Core build tools
    build-essential \
    cmake \
    ninja-build \
    \
    # Assembly
    nasm \
    \
    # Version control
    git \
    \
    # Documentation
    doxygen \
    graphviz \
    \
    # Testing
    qemu-system-x86 \
    qemu-system-arm \
    qemu-system-aarch64 \
    qemu-system-riscv64 \
    \
    # Code quality
    clang-format \
    clang-tidy \
    cppcheck \
    \
    # Cross-compilers
    gcc-arm-none-eabi \
    gcc-aarch64-linux-gnu \
    gcc-riscv64-unknown-elf \
    \
    # Cleanup
    && rm -rf /var/lib/apt/lists/*

# ============================================================
# Create Build Directory
# ============================================================
WORKDIR /build

# Default command
CMD ["/bin/bash"]
