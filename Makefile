# Alpha OS 2.0 - Multi-Architecture Makefile
ARCH ?= x86

# x86 (default)
CC_x86 = gcc
AS_x86 = nasm
LD_x86 = ld

# ARM
CC_arm = arm-none-eabi-gcc
AS_arm = arm-none-eabi-as
LD_arm = arm-none-eabi-ld

# ARM64
CC_arm64 = aarch64-linux-gnu-gcc
AS_arm64 = aarch64-linux-gnu-as
LD_arm64 = aarch64-linux-gnu-ld

# RISC-V
CC_riscv = riscv64-unknown-elf-gcc
AS_riscv = riscv64-unknown-elf-as
LD_riscv = riscv64-unknown-elf-ld

CC = $(CC_$(ARCH))
AS = $(AS_$(ARCH))
LD = $(LD_$(ARCH))

BUILD_DIR = build/$(ARCH)
TEST_CFLAGS = -std=gnu99 -Wall -Wextra -Iinclude -DTEST_MODE -g -fno-pie -no-pie
TEST_LDFLAGS = -no-pie
KERNEL_CFLAGS = -m32 -std=gnu99 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra -Wno-implicit-function-declaration -Iinclude -O2 -fno-pie

.PHONY: all clean test help info

# All kernel objects
KERNEL_OBJS = build/x86/boot.o \
              build/x86/kernel_main.o \
              build/x86/kernel_console.o \
              build/x86/kernel_memory.o \
              build/x86/kernel_cpu.o \
              build/x86/kernel_keyboard.o \
              build/x86/kernel_fs.o \
              build/x86/kernel_shell.o \
              build/x86/kernel_system.o \
              build/x86/kernel_scheduler.o \
              build/x86/kernel_acpi.o \
              build/x86/kernel_boot_info.o \
              build/x86/kernel_device.o \
              build/x86/kernel_login.o \
              build/x86/kernel_network.o \
              build/x86/kernel_persist.o \
              build/x86/kernel_score.o \
              build/x86/kernel_syscall.o \
              build/x86/libc_string.o \
              build/x86/libc_stdio.o \
              build/x86/libc_stdlib.o

all: $(BUILD_DIR)/myos.bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/kernel_main.o: kernel/main.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel_%.o: kernel/%.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/libc_%.o: libc/%.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/boot.o: arch/$(ARCH)/boot.asm | $(BUILD_DIR)
	$(AS) -f elf32 $< -o $@

$(BUILD_DIR)/myos.bin: $(KERNEL_OBJS) arch/$(ARCH)/linker.ld
	$(LD) -m elf_i386 -nostdlib -T arch/$(ARCH)/linker.ld -o $@ $(KERNEL_OBJS)

test: $(BUILD_DIR)/test_fs_shell
	$(BUILD_DIR)/test_fs_shell

$(BUILD_DIR)/test_kernel_%.o: kernel/%.c | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_libc_%.o: libc/%.c | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_fs_shell: scripts/test_fs_shell.c
	$(CC) $(TEST_CFLAGS) scripts/test_fs_shell.c -o $@ $(TEST_LDFLAGS)

clean:
	rm -rf build

help:
	@echo "Alpha OS 2.0 - Multi-Arch Build System"
	@echo "Usage: make [target] ARCH=<arch>"
	@echo "Architectures: x86 (default), arm, arm64, riscv"

info:
	@echo "ARCH=$(ARCH) CC=$(CC) AS=$(AS) LD=$(LD)"
