/**
 * Alpha OS Version Information
 * 
 * This file contains version information used throughout the kernel.
 * Update these values when releasing a new version.
 */

#ifndef VERSION_H
#define VERSION_H

// ============================================================
// Version Information
// ============================================================

#define ALPHA_OS_NAME         "Alpha OS"
#define ALPHA_OS_VERSION      "2.0.0"
#define ALPHA_OS_VERSION_MAJOR 2
#define ALPHA_OS_VERSION_MINOR 0
#define ALPHA_OS_VERSION_PATCH 0

#define ALPHA_OS_BUILD_DATE   __DATE__
#define ALPHA_OS_BUILD_TIME   __TIME__

// Codename for this release
#define ALPHA_OS_CODENAME    "Phoenix"

// ============================================================
// Kernel Information
// ============================================================

#define KERNEL_NAME          "AlphaKernel"
#define KERNEL_VERSION       ALPHA_OS_VERSION
#define KERNEL_ARCH          "x86/ARM/RISC-V"
#define KERNEL_BUILD_TYPE    ""

// Set build type based on compiler flags
#if defined(DEBUG)
    #define KERNEL_BUILD_TYPE "Debug"
#elif defined(NDEBUG)
    #define KERNEL_BUILD_TYPE "Release"
#else
    #define KERNEL_BUILD_TYPE "Unknown"
#endif

// ============================================================
// Feature Flags
// ============================================================

// Memory features
#define FEATURE_PAGING         1
#define FEATURE_VIRTUAL_MEM    1
#define FEATURE_SLAB_ALLOCATOR 1
#define FEATURE_BUDDY_SYSTEM  0

// CPU features
#define FEATURE_CPUID         1
#define FEATURE_APIC          1
#define FEATURE_MP            0
#define FEATURE_MMU           1

// Process features
#define FEATURE_SCHEDULER     1
#define FEATURE_PROCESSES     1
#define FEATURE_THREADS       0
#define FEATURE_SIGNALS       0

// File system features
#define FEATURE_FS            1
#define FEATURE_FAT12         1
#define FEATURE_EXT2          0

// Network features (future)
#define FEATURE_NETWORK       0
#define FEATURE_TCPIP         0

// Debug features
#define FEATURE_DEBUG         1
#define FEATURE_ASSERT        1
#define FEATURE_LOGGING       1

// ============================================================
// System Limits
// ============================================================

// Memory limits
#define KERNEL_HEAP_SIZE      (8 * 1024 * 1024)  // 8 MB
#define MAX_PHYS_PAGES        32768               // 128 MB
#define PAGE_SIZE             4096

// Process limits
#define MAX_PROCESSES          64
#define MAX_THREADS_PER_PROC   16
#define DEFAULT_TIME_SLICE     10  // milliseconds

// File system limits
#define MAX_FILES              256
#define MAX_OPEN_FILES         32
#define MAX_PATH_LENGTH        256

// Stack sizes
#define KERNEL_STACK_SIZE      4096
#define USER_STACK_SIZE        65536

// ============================================================
// Build Configuration
// ============================================================

// Endianness
#define __LITTLE_ENDIAN__  1
#define __BIG_ENDIAN__     0

#if defined(__LITTLE_ENDIAN) || defined(_WIN32)
    #define OS_ENDIANNESS __LITTLE_ENDIAN__
#else
    #define OS_ENDIANNESS __BIG_ENDIAN__
#endif

// Word size
#if defined(__x86_64__) || defined(__aarch64__) || defined(__riscv64)
    #define OS_64BIT 1
    #define OS_32BIT 0
    #define POINTER_SIZE 8
#else
    #define OS_64BIT 0
    #define OS_32BIT 1
    #define POINTER_SIZE 4
#endif

// ============================================================
// Version String Helpers
// ============================================================

#define VERSION_STRING(a, b, c) #a "." #b "." #c
#define VERSION_STRING_FULL(a, b, c) VERSION_STRING(a, b, c)

// ============================================================
// Copyright
// ============================================================

#define COPYRIGHT_YEAR        "2025-2026"
#define COPYRIGHT_HOLDER     "Alpha OS Contributors"
#define LICENSE              "MIT License"

#endif // VERSION_H
