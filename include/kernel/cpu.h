#ifndef CPU_H
#define CPU_H

// Use standard integer types
#ifdef TEST_MODE
#include <stdint.h>
#include <stdbool.h>
#else
#include "../kernel/types.h"
#endif

// CPU Features
#define CPU_FEATURE_FPU       (1 << 0)
#define CPU_FEATURE_APIC      (1 << 1)
#define CPU_FEATURE_PAE       (1 << 2)
#define CPU_FEATURE_PGE       (1 << 3)
#define CPU_FEATURE_PAT       (1 << 4)
#define CPU_FEATURE_SEP       (1 << 5)
#define CPU_FEATURE_MTRR      (1 << 6)
#define CPU_FEATURE_XSAVE     (1 << 7)
#define CPU_FEATURE_AVX       (1 << 8)
#define CPU_FEATURE_SSE       (1 << 9)
#define CPU_FEATURE_SSE2      (1 << 10)
#define CPU_FEATURE_SSE3      (1 << 11)
#define CPU_FEATURE_SSSE3     (1 << 12)
#define CPU_FEATURE_SSE4_1    (1 << 13)
#define CPU_FEATURE_SSE4_2    (1 << 14)

// CPU Vendor IDs
#define CPU_VENDOR_INTEL     "GenuineIntel"
#define CPU_VENDOR_AMD       "AuthenticAMD"
#define CPU_VENDOR_UNKNOWN   "Unknown"

// CPU States
typedef enum {
    CPU_STATE_RUNNING = 0,
    CPU_STATE_IDLE = 1,
    CPU_STATE_HALTED = 2,
    CPU_STATE_SLEEP = 3
} cpu_state_t;

// CPU Information Structure
typedef struct {
    char vendor[16];
    char brand[64];
    uint32_t family;
    uint32_t model;
    uint32_t stepping;
    uint32_t features;
    uint32_t flags;
    uint32_t cores_per_cpu;
    uint32_t threads_per_core;
    uint32_t cpu_freq_mhz;
    uint32_t bus_freq_mhz;
    uint32_t cache_l1_size;
    uint32_t cache_l2_size;
    uint32_t cache_l3_size;
    uint8_t  microcode_version;
    bool     virtualized;
} cpu_info_t;

// CPU Registers
typedef struct {
    uint32_t eax, ebx, ecx, edx;
} cpu_registers_t;

// Interrupt Descriptor Table Entry
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

// Interrupt Handler Type
typedef void (*interrupt_handler_t)(void);

// CPU Control Registers
#define CR0_PE  (1 << 0)   // Protection Enable
#define CR0_MP  (1 << 1)   // Monitor Coprocessor
#define CR0_EM  (1 << 2)   // Emulation
#define CR0_NE  (1 << 5)   // Numeric Error
#define CR0_WP  (1 << 16)  // Write Protect
#define CR0_AM  (1 << 18)  // Alignment Mask
#define CR0_NW  (1 << 29)  // Not Write-through
#define CR0_CD  (1 << 30)  // Cache Disable
#define CR0_PG  (1 << 31)  // Paging

#define CR4_PAE   (1 << 5)  // Physical Address Extension
#define CR4_PGE   (1 << 7)  // Page Global Enable
#define CR4_OSFXSR (1 << 9) // OS FXSAVE Support
#define CR4_OSXMMEXCPT (1 << 10) // OS XMM Exception
#define CR4_XSAVE (1 << 18) // XSAVE Support

// CPU Functions
void cpu_init(void);
void cpu_detect(void);
void cpu_set_features(uint32_t features);
uint32_t cpu_get_features(void);
const char* cpu_get_vendor(void);
void cpu_get_info(cpu_info_t* info);
void cpu_enable_paging(void);
void cpu_disable_paging(void);
void cpu_enable_interrupts(void);
void cpu_disable_interrupts(void);
bool cpu_are_interrupts_enabled(void);
void cpu_halt(void);
void cpu_idle(void);
void cpu_relax(void);
void cpu_cpuid(uint32_t func, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
uint64_t cpu_read_cr0(void);
void cpu_write_cr0(uint64_t val);
uint64_t cpu_read_cr2(void);
uint64_t cpu_read_cr3(void);
void cpu_write_cr3(uint64_t val);
uint64_t cpu_read_cr4(void);
void cpu_write_cr4(uint64_t val);
uint64_t cpu_read_rflags(void);
uint64_t cpu_read_msr(uint32_t msr);
void cpu_write_msr(uint32_t msr, uint64_t value);

// Interrupt Management
void idt_init(void);
void idt_set_entry(uint8_t vector, uint64_t handler, uint8_t ist, uint8_t type_attr);
void interrupt_register_handler(uint8_t vector, interrupt_handler_t handler);
void interrupt_enable(uint8_t vector);
void interrupt_disable(uint8_t vector);

// APIC Functions
void apic_init(void);
void apic_enable(void);
void apic_disable(void);
void apic_send_eoi(void);
void apic_send_ipi(uint8_t cpu, uint8_t vector);

// System Timer
void timer_init(uint32_t frequency);
void timer_set_handler(void (*handler)(void));
uint64_t timer_get_ticks(void);
void timer_sleep(uint32_t ms);

// Performance Monitoring
void perf_init(void);
uint64_t perf_get_cycles(void);
uint64_t perf_get_timestamp(void);

#endif // CPU_H
