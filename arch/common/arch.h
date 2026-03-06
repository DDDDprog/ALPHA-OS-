/**
 * Architecture Abstraction Layer
 * Provides common interfaces for different CPU architectures
 */

#ifndef ARCH_H
#define ARCH_H

#include "../kernel/types.h"

/* Architecture types */
#define ARCH_X86    0
#define ARCH_ARM    1
#define ARCH_ARM64  2
#define ARCH_RISCV  3

/* Detect current architecture */
#if defined(__i386__) || defined(__x86_64__)
    #define ARCH ARCH_X86
    #include "x86/cpu.h"
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__aarch64__)
        #define ARCH ARCH_ARM64
    #else
        #define ARCH ARCH_ARM
    #endif
    #include "arm/cpu.h"
#elif defined(__riscv)
    #define ARCH ARCH_RISCV
    #include "riscv/cpu.h"
#else
    #define ARCH ARCH_X86  /* Default */
#endif

/* Common types for all architectures */

/* Physical and virtual addresses */
typedef uint32_t phys_addr_t;
typedef uint32_t virt_addr_t;

#ifdef ARCH_ARM64
typedef uint64_t phys_addr_t;
typedef uint64_t virt_addr_t;
#endif

/* Register sets - architecture independent view */
typedef struct {
    uint32_t r0, r1, r2, r3;
    uint32_t r4, r5, r6, r7;
    uint32_t r8, r9, r10, r11;
    uint32_t r12, sp, lr, pc;
    uint32_t cpsr;
} arch_regs_t;

/* GDT/IDT entry count */
#define MAX_INTERRUPTS 256

/* Page size options */
#define PAGE_SIZE_4KB   4096
#define PAGE_SIZE_16KB  16384
#define PAGE_SIZE_64KB  65536
#define PAGE_SIZE_1MB   1048576
#define PAGE_SIZE_2MB   2097152
#define PAGE_SIZE_4MB   4194304

/* Default page size */
#ifndef PAGE_SIZE
#define PAGE_SIZE PAGE_SIZE_4KB
#endif

/* Cache line size for memory barriers */
#define CACHE_LINE_SIZE 64

/* Memory barrier macros */
#if defined(ARCH_X86)
    #define mb() __asm__ volatile("mfence" ::: "memory")
    #define rmb() __asm__ volatile("lfence" ::: "memory")
    #define wmb() __asm__ volatile("sfence" ::: "memory")
#elif defined(ARCH_ARM) || defined(ARCH_ARM64)
    #define mb() __asm__ volatile("dmb sy" ::: "memory")
    #define rmb() __asm__ volatile("dmb sy" ::: "memory")
    #define wmb() __asm__ volatile("dmb st" ::: "memory")
#else
    #define mb() __asm__ volatile("" ::: "memory")
    #define rmb() __asm__ volatile("" ::: "memory")
    #define wmb() __asm__ volatile("" ::: "memory")
#endif

/* CPU pause/hint for spin loops */
#if defined(ARCH_X86) || defined(ARCH_X86_64)
    #define cpu_relax() __asm__ volatile("pause" ::: "memory")
#elif defined(ARCH_ARM) || defined(ARCH_ARM64)
    #define cpu_relax() __asm__ volatile("yield" ::: "memory")
#else
    #define cpu_relax() __asm__ volatile("" ::: "memory")
#endif

/* Interrupt enable/disable */
#if defined(ARCH_X86)
    #define arch_enable_interrupts() __asm__ volatile("sti")
    #define arch_disable_interrupts() __asm__ volatile("cli")
#elif defined(ARCH_ARM)
    #define arch_enable_interrupts() do { uint32_t temp; __asm__ volatile("mrs %0, cpsr; bic %0, %0, #0x80; msr cpsr_c, %0" : "=r"(temp)); } while(0)
    #define arch_disable_interrupts() do { uint32_t temp; __asm__ volatile("mrs %0, cpsr; orr %0, %0, #0x80; msr cpsr_c, %0" : "=r"(temp)); } while(0)
#elif defined(ARCH_ARM64)
    #define arch_enable_interrupts() __asm__ volatile("msr daifclr, #3" ::: "memory")
    #define arch_disable_interrupts() __asm__ volatile("msr daifset, #3" ::: "memory")
#else
    #define arch_enable_interrupts() ((void)0)
    #define arch_disable_interrupts() ((void)0)
#endif

/* Halt CPU */
#if defined(ARCH_X86)
    #define arch_halt() __asm__ volatile("hlt")
#elif defined(ARCH_ARM) || defined(ARCH_ARM64)
    #define arch_halt() __asm__ volatile("wfi")
#else
    #define arch_halt() for(;;) { }
#endif

/* Wait for interrupt */
#if defined(ARCH_ARM) || defined(ARCH_ARM64)
    #define arch_wfi() __asm__ volatile("wfi")
#else
    #define arch_wfi() arch_halt()
#endif

/* Architecture initialization */
void arch_init(void);
void arch_enable_paging(void);
void arch_disable_paging(void);
bool arch_is_paging_enabled(void);

/* Architecture-specific CPU info */
const char* arch_get_name(void);
uint32_t arch_get_id(void);
void arch_get_cpu_info(void* info);

/* Memory management */
void arch_enable_mmu(void);
void arch_invalidate_tlb(void);
void arch_invalidate_cache(void);

/* Timer */
void arch_timer_init(uint32_t freq);
uint64_t arch_timer_get_ticks(void);
void arch_timer_wait(uint32_t ms);

/* Serial I/O */
void arch_serial_init(void);
void arch_serial_putchar(char c);
char arch_serial_getchar(void);

/* Platform initialization */
void platform_init(void);
void platform_halt(void);
void platform_reboot(void);

#endif /* ARCH_H */
