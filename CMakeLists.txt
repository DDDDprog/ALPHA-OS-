cmake_minimum_required(VERSION 3.16)
project(AlphaOS VERSION 2.0.0 LANGUAGES C ASM)

# ============================================================
# Alpha OS 2.0 - Modern CMake Build System
# ============================================================

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# Default architecture
set(TARGET_ARCH "x86" CACHE STRING "Target architecture: x86, arm, arm64, riscv")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/kernel
    ${PROJECT_SOURCE_DIR}/include/libc
)

# ============================================================
# Compiler Flags
# ============================================================

# Common flags for all targets
set(COMMON_FLAGS
    -Wall -Wextra -Wpedantic
    -Wno-unused-parameter
    -ffreestanding
    -fno-builtin
    -fno-stack-protector
    -nostdlib
)

# Architecture-specific settings
if(TARGET_ARCH STREQUAL "x86")
    set(ARCH_FLAGS "-m32 -march=i386")
    set(ASM_FORMAT "elf32")
    set(ARCH_DIR "x86")
    set(LINKER_FORMAT "elf_i386")
elseif(TARGET_ARCH STREQUAL "arm")
    set(ARCH_FLAGS "-march=armv7-a")
    set(ASM_FORMAT "elf32")
    set(ARCH_DIR "arm")
    set(LINKER_FORMAT "")
elseif(TARGET_ARCH STREQUAL "arm64")
    set(ARCH_FLAGS "-march=armv8-a")
    set(ASM_FORMAT "elf64")
    set(ARCH_DIR "arm")
    set(LINKER_FORMAT "elf64ltsfip")
elseif(TARGET_ARCH STREQUAL "riscv")
    set(ARCH_FLAGS "-march=rv64gc")
    set(ASM_FORMAT "elf64")
    set(ARCH_DIR "riscv")
    set(LINKER_FORMAT "elf64lriscv")
endif()

# Kernel build flags
set(KERNEL_FLAGS
    ${COMMON_FLAGS}
    ${ARCH_FLAGS}
    -D__KERNEL__
)

# Test build flags
set(TEST_FLAGS
    -Wall -Wextra
    -DTEST_MODE
    -g
)

# ============================================================
# Build Options
# ============================================================

option(ENABLE_TESTS "Build test programs" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Treat warnings as errors
if(ENABLE_WARNINGS_AS_ERRORS)
    list(APPEND KERNEL_FLAGS -Werror)
    list(APPEND TEST_FLAGS -Werror)
endif()

# Code coverage
if(ENABLE_COVERAGE)
    list(APPEND TEST_FLAGS --coverage -fprofile-arcs -ftest-coverage)
endif()

# ============================================================
# Build Targets
# ============================================================

# Kernel sources
file(GLOB KERNEL_SOURCES
    ${PROJECT_SOURCE_DIR}/kernel/*.c
)

# LibC sources
file(GLOB LIBC_SOURCES
    ${PROJECT_SOURCE_DIR}/libc/*.c
)

# All kernel objects
set(KERNEL_OBJECTS "")
set(LIBC_OBJECTS "")

# Create kernel object files
foreach(src ${KERNEL_SOURCES})
    get_filename_component(name ${src} NAME)
    string(REPLACE ".c" "_kernel" obj_name ${name})
    add_object_library(${obj_name} ${src})
    target_compile_options(${obj_name} PRIVATE ${KERNEL_FLAGS})
    list(APPEND KERNEL_OBJECTS $<TARGET_OBJECTS:${obj_name}>)
endforeach()

# Create libc object files
foreach(src ${LIBC_SOURCES})
    get_filename_component(name ${src} NAME)
    string(REPLACE ".c" "_libc" obj_name ${name})
    add_object_library(${obj_name} ${src})
    target_compile_options(${obj_name} PRIVATE ${KERNEL_FLAGS})
    list(APPEND LIBC_OBJECTS $<TARGET_OBJECTS:${obj_name}>)
endforeach()

# Bootloader
set(BOOT_SOURCE ${PROJECT_SOURCE_DIR}/arch/${ARCH_DIR}/boot.asm)
set(BOOT_OBJECT ${CMAKE_BINARY_DIR}/boot.o)

# Custom command for NASM assembly
add_custom_command(
    OUTPUT ${BOOT_OBJECT}
    COMMAND nasm -f ${ASM_FORMAT} ${BOOT_SOURCE} -o ${BOOT_OBJECT}
    DEPENDS ${BOOT_SOURCE}
    COMMENT "Assembling bootloader for ${TARGET_ARCH}"
)

# Kernel binary
add_custom_command(
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myos.bin
    COMMAND ${CMAKE_LINKER} -T ${PROJECT_SOURCE_DIR}/arch/${ARCH_DIR}/linker.ld -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myos.bin ${BOOT_OBJECT} ${KERNEL_OBJECTS} ${LIBC_OBJECTS}
    DEPENDS ${BOOT_OBJECT} ${KERNEL_OBJECTS} ${LIBC_OBJECTS}
    COMMENT "Linking kernel for ${TARGET_ARCH}"
)

add_custom_target(kernel ALL DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myos.bin)

# ============================================================
# Test Programs
# ============================================================

if(ENABLE_TESTS)
    # Create test executable
    add_executable(test_fs_shell
        ${PROJECT_SOURCE_DIR}/scripts/test_fs_shell.c
    )
    target_compile_options(test_fs_shell PRIVATE ${TEST_FLAGS})
    target_link_libraries(test_fs_shell PRIVATE ${KERNEL_OBJECTS} ${LIBC_OBJECTS})

    add_test(NAME test_fs_shell COMMAND test_fs_shell)
endif()

# ============================================================
# Installation
# ============================================================

install(TARGETS kernel DESTINATION bin)
install(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myos.bin DESTINATION bin)

# ============================================================
# Build Information
# ============================================================

message(STATUS "========================================")
message(STATUS "  Alpha OS 2.0 Build Configuration")
message(STATUS "========================================")
message(STATUS "  Version:      ${PROJECT_VERSION}")
message(STATUS "  Architecture: ${TARGET_ARCH}")
message(STATUS "  Build Type:  ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler:     ${CMAKE_C_COMPILER}")
message(STATUS "  Assembler:    ${CMAKE_ASM_NASM_COMPILER}")
message(STATUS "  Linker:       ${CMAKE_LINKER}")
message(STATUS "========================================")

# Print configuration summary
message(STATUS "")
message(STATUS "Kernel flags: ${KERNEL_FLAGS}")
message(STATUS "")
