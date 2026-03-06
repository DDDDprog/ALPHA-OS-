/**
 * RISC-V CPU definitions for Alpha OS
 */

#ifndef RISCV_CPU_H
#define RISCV_CPU_H

#include "../../kernel/types.h"

/* RISC-V Specific Features */
#define RISCV_FEATURE_I      (1 << 0)  /* Integer */
#define RISCV_FEATURE_E      (1 << 1)  /* Embedded */
#define RISCV_FEATURE_M      (1 << 2)  /* Multiply/Divide */
#define RISCV_FEATURE_A      (1 << 3)  /* Atomics */
#define RISCV_FEATURE_F      (1 << 4)  /* Single Precision FP */
#define RISCV_FEATURE_D      (1 << 5)  /* Double Precision FP */
#define RISCV_FEATURE_C      (1 << 6)  /* Compressed */
#define RISCV_FEATURE_U      (1 << 7)  /* User mode */
#define RISCV_FEATURE_S      (1 << 8)  /* Supervisor mode */
#define RISCV_FEATURE_H      (1 << 9)  /* Hypervisor */
#define RISCV_FEATURE_V      (1 << 10) /* Vector */
#define RISCV_FEATURE_RV32E  (1 << 11) /* RV32E (embedded) */
#define RISCV_FEATURE_64BIT  (1 << 12) /* RV64 */

/* RISC-V CSR Registers */
#define RISCV_CSR_MSTATUS   0x300
#define RISCV_CSR_MIE       0x304
#define RISCV_CSR_MTVEC     0x305
#define RISCV_CSR_MEPC      0x341
#define RISCV_CSR_MCAUSE    0x342
#define RISCV_CSR_MTVAL     0x343
#define RISCV_CSR_MIP       0x344

/* RISC-V CPU Information */
typedef struct {
    char vendor[32];
    char name[64];
    uint32_t architecture;  /* Base ISA */
    uint32_t extensions;    /* Enabled extensions */
    uint32_t mvendorid;     /* Vendor ID */
    uint32_t marchid;       /* Architecture ID */
    uint32_t mimpid;        /* Implementation ID */
    uint32_t hartid;        /* Hardware thread ID */
    uint32_t num_harts;     /* Number of harts */
    uint32_t mem_size;      /* Memory size */
    uint64_t freq_hz;       /* CPU frequency in Hz */
    bool     virtualized;   /* Running in VM */
} riscv_cpu_info_t;

/* RISC-V Specific inline functions */

/* Read CSR */
#define riscv_read_csr(csr) ({ \
    uint32_t __val; \
    __asm__ volatile("csrr %0, " #csr : "=r"(__val)); \
    __val; \
})

/* Write CSR */
#define riscv_write_csr(csr, val) ({ \
    __asm__ volatile("csrw " #csr ", %0" : : "r"(val)); \
})

/* Set bits in CSR */
#define riscv_set_csr(csr, val) ({ \
    uint32_t __old = riscv_read_csr(csr); \
    riscv_write_csr(csr, __old | val); \
})

/* Clear bits in CSR */
#define riscv_clear_csr(csr, val) ({ \
    uint32_t __old = riscv_read_csr(csr); \
    riscv_write_csr(csr, __old & ~val); \
})

/* Get current privilege mode */
static inline uint8_t riscv_get_mode(void) {
    return riscv_read_csr(0xC00) & 3;
}

/* Enable machine interrupts */
static inline void riscv_enable_irq(void) {
    riscv_set_csr(RISCV_CSR_MSTATUS, 0x8);  /* MIE bit */
}

/* Disable machine interrupts */
static inline void riscv_disable_irq(void) {
    riscv_clear_csr(RISCV_CSR_MSTATUS, 0x8);
}

/* Memory barriers */
static inline void riscv_mb(void) {
    __asm__ volatile("fence" ::: "memory");
}

static inline void riscv_rmb(void) {
    __asm__ volatile("fence r, r" ::: "memory");
}

static inline void riscv_wmb(void) {
    __asm__ volatile("fence w, w" ::: "memory");
}

/* Get current stack pointer */
static inline void* riscv_get_sp(void) {
    void* sp;
    __asm__ volatile("mv %0, sp" : "=r"(sp));
    return sp;
}

/* Set stack pointer */
static inline void riscv_set_sp(void* sp) {
    __asm__ volatile("mv sp, %0" : : "r"(sp));
}

/* Get global pointer */
static inline void* riscv_get_gp(void) {
    void* gp;
    __asm__ volatile("mv %0, gp" : "=r"(gp));
    return gp;
}

/* Get thread pointer */
static inline void* riscv_get_tp(void) {
    void* tp;
    __asm__ volatile("mv %0, tp" : "=r"(tp));
    return tp;
}

/* Get return address */
static inline void* riscv_get_ra(void) {
    void* ra;
    __asm__ volatile("mv %0, ra" : "=r"(ra));
    return ra;
}

/* RISC-V CPU functions */
void riscv_cpu_init(void);
void riscv_cpu_detect(riscv_cpu_info_t* info);
void riscv_mmu_init(void);
void riscv_mmu_enable(void);
void riscv_mmu_disable(void);

/* RISC-V Timer */
void riscv_timer_init(uint32_t freq);
uint64_t riscv_timer_get_ticks(void);
void riscv_timer_wait(uint32_t ms);

/* RISC-V CLINT */
void riscv_clint_init(void);
void riscv_clint_set_mtimecmp(uint64_t value);
uint64_t riscv_clint_get_mtime(void);

#endif /* RISCV_CPU_H */
