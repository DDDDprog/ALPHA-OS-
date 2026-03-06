/**
 * ARM CPU definitions for Alpha OS
 */

#ifndef ARM_CPU_H
#define ARM_CPU_H

#include "../../kernel/types.h"

/* ARM Specific Features */
#define ARM_FEATURE_VFP       (1 << 0)
#define ARM_FEATURE_NEON      (1 << 1)
#define ARM_FEATURE_THUMB     (1 << 2)
#define ARM_FEATURE_MMU       (1 << 3)
#define ARM_FEATURE_L1CACHE   (1 << 4)
#define ARM_FEATURE_L2CACHE   (1 << 5)
#define ARM_FEATURE_TZ        (1 << 6)  /* TrustZone */
#define ARM_FEATURE_SMP       (1 << 7)
#define ARM_FEATURE_VIRT      (1 << 8)  /* Virtualization */

/* ARM Processor Modes */
#define ARM_MODE_USR 0x10
#define ARM_MODE_FIQ 0x11
#define ARM_MODE_IRQ 0x12
#define ARM_MODE_SVC 0x13
#define ARM_MODE_ABT 0x17
#define ARM_MODE_UND 0x1B
#define ARM_MODE_SYS 0x1F

/* ARM Coprocessor Registers */
#define ARM_CPREG_C1_CONTROL  0
#define ARM_CPREG_C1_ECTLR    1
#define ARM_CPREG_C1_CPACR    2

/* ARM CPU Information */
typedef struct {
    char vendor[32];
    char name[64];
    uint32_t implementer;   /* ARM implementer code */
    uint32_t variant;       /* Variant number */
    uint32_t architecture;  /* Architecture version */
    uint32_t part;          /* Part number */
    uint32_t revision;      /* Revision number */
    uint32_t features;      /* Feature flags */
    uint32_t core_count;    /* Number of cores */
    uint32_t cpu_id;       /* CPU ID register */
    uint32_t cache_line_size;
    uint32_t dcache_size;   /* L1 data cache size */
    uint32_t icache_size;   /* L1 instruction cache size */
    uint32_t l2cache_size;  /* L2 cache size */
    uint32_t freq_hz;       /* CPU frequency in Hz */
} arm_cpu_info_t;

/* ARM Specific inline functions */

/* Read CP15 register */
static inline uint32_t arm_read_c0(uint32_t reg) {
    uint32_t val;
    __asm__ volatile("mrc p15, 0, %0, c0, c0, 0" : "=r"(val));
    return val;
}

static inline uint32_t arm_read_c1(void) {
    uint32_t val;
    __asm__ volatile("mrc p15, 0, %0, c1, c0, 0" : "=r"(val));
    return val;
}

static inline void arm_write_c1(uint32_t val) {
    __asm__ volatile("mcr p15, 0, %0, c1, c0, 0" : : "r"(val));
}

/* Read ARM register */
static inline uint32_t arm_get_cpsr(void) {
    uint32_t val;
    __asm__ volatile("mrs %0, cpsr" : "=r"(val));
    return val;
}

static inline void arm_set_cpsr(uint32_t val) {
    __asm__ volatile("msr cpsr, %0" : : "r"(val));
}

/* Get current mode */
static inline uint8_t arm_get_mode(void) {
    return arm_get_cpsr() & 0x1F;
}

/* Set CPU mode */
static inline void arm_set_mode(uint8_t mode) {
    uint32_t val = arm_get_cpsr();
    val = (val & ~0x1F) | (mode & 0x1F);
    arm_set_cpsr(val);
}

/* Enable/disable interrupts in CPSR */
static inline void arm_enable_irq(void) {
    uint32_t val = arm_get_cpsr();
    val &= ~0x80;  /* Clear I bit */
    arm_set_cpsr(val);
}

static inline void arm_disable_irq(void) {
    uint32_t val = arm_get_cpsr();
    val |= 0x80;   /* Set I bit */
    arm_set_cpsr(val);
}

static inline void arm_enable_fiq(void) {
    uint32_t val = arm_get_cpsr();
    val &= ~0x40;  /* Clear F bit */
    arm_set_cpsr(val);
}

static inline void arm_disable_fiq(void) {
    uint32_t val = arm_get_cpsr();
    val |= 0x40;   /* Set F bit */
    arm_set_cpsr(val);
}

/* Data memory barrier */
static inline void arm_dmb(void) {
    __asm__ volatile("dmb" ::: "memory");
}

/* Data sync barrier */
static inline void arm_dsb(void) {
    __asm__ volatile("dsb" ::: "memory");
}

/* Instruction sync barrier */
static inline void arm_isb(void) {
    __asm__ volatile("isb" ::: "memory");
}

/* Invalidate instruction cache */
static inline void arm_icache_invalidate(void) {
    __asm__ volatile("mcr p15, 0, %0, c7, c5, 0" : : "r"(0));
}

/* Invalidate data cache */
static inline void arm_dcache_invalidate(void) {
    __asm__ volatile("mcr p15, 0, %0, c7, c6, 0" : : "r"(0));
}

/* Clean and invalidate cache */
static inline void arm_cache_clean_invalidate(void) {
    __asm__ volatile("mcr p15, 0, %0, c7, c14, 0" : : "r"(0));
}

/* Get current stack pointer */
static inline void* arm_get_sp(void) {
    void* sp;
    __asm__ volatile("mov %0, sp" : "=r"(sp));
    return sp;
}

/* Set stack pointer */
static inline void arm_set_sp(void* sp) {
    __asm__ volatile("mov sp, %0" : : "r"(sp));
}

/* Get link register (return address) */
static inline void* arm_get_lr(void) {
    void* lr;
    __asm__ volatile("mov %0, lr" : "=r"(lr));
    return lr;
}

/* ARM CPU functions */
void arm_cpu_init(void);
void arm_cpu_detect(arm_cpu_info_t* info);
void arm_mmu_init(void);
void arm_mmu_enable(void);
void arm_mmu_disable(void);
void arm_tlb_invalidate(void);
void arm_tlb_invalidate_all(void);

/* ARM Timer */
void arm_timer_init(uint32_t freq);
uint64_t arm_timer_get_ticks(void);
void arm_timer_wait(uint32_t ms);

#endif /* ARM_CPU_H */
