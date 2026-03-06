/**
 * Common Architecture Implementation
 * Provides architecture-agnostic interfaces
 */

#include "../kernel/types.h"
#include "../kernel/console.h"
#include "../kernel/memory.h"
#include "../kernel/cpu.h"
#include "arch.h"

#ifdef TEST_MODE
#include <stdio.h>
#include <string.h>
#else

/* Architecture-specific implementations */
#if ARCH == ARCH_X86
#include "../../kernel/cpu.c"  /* x86 implementation */
#endif

#endif

/* Architecture name strings */
static const char* arch_names[] = {
    "x86",
    "ARM",
    "ARM64",
    "RISC-V"
};

/* Get architecture name */
const char* arch_get_name(void) {
    if (ARCH < sizeof(arch_names)/sizeof(arch_names[0])) {
        return arch_names[ARCH];
    }
    return "Unknown";
}

/* Get architecture ID */
uint32_t arch_get_id(void) {
    return ARCH;
}

/* Initialize architecture */
void arch_init(void) {
#ifdef TEST_MODE
    printf("[ARCH] Initializing %s architecture\n", arch_get_name());
#else
    /* Architecture-specific init would go here */
#endif
}

/* Enable paging */
void arch_enable_paging(void) {
#ifdef TEST_MODE
    printf("[ARCH] Paging enabled\n");
#else
    #if ARCH == ARCH_X86
    cpu_enable_paging();
    #elif ARCH == ARCH_ARM
    arm_mmu_enable();
    #endif
#endif
}

/* Disable paging */
void arch_disable_paging(void) {
#ifdef TEST_MODE
    printf("[ARCH] Paging disabled\n");
#else
    #if ARCH == ARCH_X86
    cpu_disable_paging();
    #elif ARCH == ARCH_ARM
    arm_mmu_disable();
    #endif
#endif
}

/* Check if paging is enabled */
bool arch_is_paging_enabled(void) {
#ifdef TEST_MODE
    return false;
#else
    #if ARCH == ARCH_X86
    return memory_is_paging_enabled();
    #else
    return false;
    #endif
#endif
}

/* Get CPU info */
void arch_get_cpu_info(void* info) {
#ifdef TEST_MODE
    printf("[ARCH] Getting CPU info for %s\n", arch_get_name());
    if (info) {
        memset(info, 0, sizeof(cpu_info_t));
    }
#else
    #if ARCH == ARCH_X86
    cpu_get_info((cpu_info_t*)info);
    #elif ARCH == ARCH_ARM
    arm_cpu_detect((arm_cpu_info_t*)info);
    #endif
#endif
}

/* Enable MMU */
void arch_enable_mmu(void) {
#ifdef TEST_MODE
    printf("[ARCH] MMU enabled\n");
#else
    #if ARCH == ARCH_ARM
    arm_mmu_enable();
    #endif
#endif
}

/* Invalidate TLB */
void arch_invalidate_tlb(void) {
#ifdef TEST_MODE
    /* No-op in test mode */
#else
    #if ARCH == ARCH_X86
    cache_flush_tlb();
    #elif ARCH == ARCH_ARM
    arm_tlb_invalidate();
    #endif
#endif
}

/* Invalidate cache */
void arch_invalidate_cache(void) {
#ifdef TEST_MODE
    /* No-op in test mode */
#else
    #if ARCH == ARCH_ARM
    arm_icache_invalidate();
    arm_dcache_invalidate();
    #endif
#endif
}

/* Initialize timer */
void arch_timer_init(uint32_t freq) {
#ifdef TEST_MODE
    printf("[ARCH] Timer initialized at %u Hz\n", freq);
#else
    #if ARCH == ARCH_X86
    timer_init(freq);
    #elif ARCH == ARCH_ARM
    arm_timer_init(freq);
    #elif ARCH == ARCH_RISCV
    riscv_timer_init(freq);
    #endif
#endif
}

/* Get timer ticks */
uint64_t arch_timer_get_ticks(void) {
#ifdef TEST_MODE
    return 0;
#else
    #if ARCH == ARCH_X86
    return timer_get_ticks();
    #elif ARCH == ARCH_ARM
    return arm_timer_get_ticks();
    #elif ARCH == ARCH_RISCV
    return riscv_timer_get_ticks();
    #endif
#endif
}

/* Wait for milliseconds */
void arch_timer_wait(uint32_t ms) {
#ifdef TEST_MODE
    #ifdef TEST_MODE
    #include <unistd.h>
    usleep(ms * 1000);
    #endif
#else
    #if ARCH == ARCH_X86
    timer_sleep(ms);
    #elif ARCH == ARCH_ARM
    arm_timer_wait(ms);
    #elif ARCH == ARCH_RISCV
    riscv_timer_wait(ms);
    #endif
#endif
}

/* Initialize serial */
void arch_serial_init(void) {
#ifdef TEST_MODE
    printf("[ARCH] Serial port initialized\n");
#endif
}

/* Serial putchar */
void arch_serial_putchar(char c) {
    (void)c;
#ifdef TEST_MODE
    putchar(c);
#endif
}

/* Serial getchar */
char arch_serial_getchar(void) {
#ifdef TEST_MODE
    return getchar();
#else
    return 0;
#endif
}

/* Platform init */
void platform_init(void) {
#ifdef TEST_MODE
    printf("[PLATFORM] Platform initialized\n");
#endif
}

/* Platform halt */
void platform_halt(void) {
#ifdef TEST_MODE
    printf("[PLATFORM] System halted\n");
    exit(0);
#else
    arch_halt();
#endif
}

/* Platform reboot */
void platform_reboot(void) {
#ifdef TEST_MODE
    printf("[PLATFORM] Reboot requested\n");
#else
    system_reboot();
#endif
}
