# Makefile for MyOS - Standard GCC Version

# Compiler and tools
CC = gcc
AS = nasm
LD = ld

# Directories
ARCH_DIR = arch/x86
KERNEL_DIR = kernel
LIBC_DIR = libc
INCLUDE_DIR = include
BUILD_DIR = build
ISO_DIR = iso
SCRIPTS_DIR = scripts

# Compiler flags for kernel
KERNEL_CFLAGS = -m32 -std=gnu99 -ffreestanding -fno-builtin -fno-stack-protector \
                -nostdlib -nodefaultlibs -Wall -Wextra -Wno-implicit-function-declaration \
                -I$(INCLUDE_DIR) -O2

# Compiler flags for test programs
TEST_CFLAGS = -std=gnu99 -Wall -Wextra -I$(INCLUDE_DIR) -DTEST_MODE -g

# Linker flags
LDFLAGS = -m elf_i386 -nostdlib

# Source files
ARCH_SOURCES = $(ARCH_DIR)/boot.asm
KERNEL_SOURCES = $(wildcard $(KERNEL_DIR)/*.c)
LIBC_SOURCES = $(wildcard $(LIBC_DIR)/*.c)

# Object files
ARCH_OBJECTS = $(BUILD_DIR)/boot.o
KERNEL_OBJECTS = $(KERNEL_SOURCES:$(KERNEL_DIR)/%.c=$(BUILD_DIR)/kernel_%.o)
LIBC_OBJECTS = $(LIBC_SOURCES:$(LIBC_DIR)/%.c=$(BUILD_DIR)/libc_%.o)

ALL_OBJECTS = $(ARCH_OBJECTS) $(KERNEL_OBJECTS) $(LIBC_OBJECTS)

# Test object files (compiled differently)
TEST_KERNEL_OBJECTS = $(KERNEL_SOURCES:$(KERNEL_DIR)/%.c=$(BUILD_DIR)/test_kernel_%.o)
TEST_LIBC_OBJECTS = $(LIBC_SOURCES:$(LIBC_DIR)/%.c=$(BUILD_DIR)/test_libc_%.o)

# Targets
.PHONY: all clean iso run test install-deps help

all: $(BUILD_DIR)/myos.bin

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Assemble boot.asm
$(BUILD_DIR)/boot.o: $(ARCH_DIR)/boot.asm | $(BUILD_DIR)
	$(AS) -f elf32 $< -o $@

# Compile kernel sources for OS
$(BUILD_DIR)/kernel_%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

# Compile libc sources for OS
$(BUILD_DIR)/libc_%.o: $(LIBC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

# Compile kernel sources for testing
$(BUILD_DIR)/test_kernel_%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# Compile libc sources for testing
$(BUILD_DIR)/test_libc_%.o: $(LIBC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# Link the kernel
$(BUILD_DIR)/myos.bin: $(ALL_OBJECTS) $(ARCH_DIR)/linker.ld
	$(LD) $(LDFLAGS) -T $(ARCH_DIR)/linker.ld -o $@ $(ALL_OBJECTS)

# Create ISO image
iso: $(BUILD_DIR)/myos.bin
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BUILD_DIR)/myos.bin $(ISO_DIR)/boot/
	cp $(ISO_DIR)/boot/grub/grub.cfg $(ISO_DIR)/boot/grub/ 2>/dev/null || true
	grub-mkrescue -o $(BUILD_DIR)/myos.iso $(ISO_DIR) 2>/dev/null || \
	echo "Warning: grub-mkrescue not available. ISO creation skipped."

# Run in QEMU
run: iso
	qemu-system-i386 -cdrom $(BUILD_DIR)/myos.iso 2>/dev/null || \
	echo "QEMU not available. Please install qemu-system-x86 to run the OS."

# Test the file system and shell (native compilation)
test: $(BUILD_DIR)/test_fs_shell $(BUILD_DIR)/test_console
	@echo "Running file system and shell tests..."
	$(BUILD_DIR)/test_fs_shell
	@echo "Running console tests..."
	$(BUILD_DIR)/test_console

# Build test programs
$(BUILD_DIR)/test_fs_shell: $(SCRIPTS_DIR)/test_fs_shell.c $(TEST_KERNEL_OBJECTS) $(TEST_LIBC_OBJECTS) | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) $^ -o $@

$(BUILD_DIR)/test_console: $(SCRIPTS_DIR)/test_console.c $(TEST_KERNEL_OBJECTS) $(TEST_LIBC_OBJECTS) | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) $^ -o $@

# Interactive shell test
shell-test: $(BUILD_DIR)/shell_interactive
	$(BUILD_DIR)/shell_interactive

$(BUILD_DIR)/shell_interactive: $(SCRIPTS_DIR)/shell_interactive.c $(TEST_KERNEL_OBJECTS) $(TEST_LIBC_OBJECTS) | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) $^ -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR)/*
	rm -f $(ISO_DIR)/boot/myos.bin

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y build-essential nasm qemu-system-x86 grub-pc-bin xorriso
	@echo "Dependencies installed successfully!"

# Help
help:
	@echo "MyOS Build System"
	@echo "=================="
	@echo "Available targets:"
	@echo "  all          - Build the kernel binary"
	@echo "  iso          - Create bootable ISO image"
	@echo "  run          - Run the OS in QEMU"
	@echo "  test         - Run file system and shell tests"
	@echo "  shell-test   - Run interactive shell test"
	@echo "  clean        - Clean build files"
	@echo "  install-deps - Install build dependencies"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Quick start:"
	@echo "  make install-deps  # Install dependencies"
	@echo "  make test          # Test the implementation"
	@echo "  make shell-test    # Try the interactive shell"
	@echo "  make iso           # Build bootable ISO"
	@echo "  make run           # Run in QEMU"
