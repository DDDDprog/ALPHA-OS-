#include "../include/kernel/cpu.h"
#include "../include/libc/stdio.h"
#include "../include/libc/string.h"

#ifndef TEST_MODE

// CPU Information
static cpu_info_t cpu_info;
static bool interrupts_enabled = false;

// CPUID function
void cpu_cpuid(uint32_t func, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    uint32_t a, b, c, d;
    __asm__ volatile ("cpuid" 
        : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
        : "a"(func), "c"(0));
    if (eax) *eax = a;
    if (ebx) *ebx = b;
    if (ecx) *ecx = c;
    if (edx) *edx = d;
}

// Read MSR
uint64_t cpu_read_msr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" 
        : "=a"(low), "=d"(high)
        : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

// Write MSR
void cpu_write_msr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    __asm__ volatile ("wrmsr"
        : 
        : "a"(low), "d"(high), "c"(msr));
}

// Get vendor string
const char* cpu_get_vendor(void) {
    return cpu_info.vendor;
}

// Get CPU features
uint32_t cpu_get_features(void) {
    return cpu_info.features;
}

// Detect CPU features
void cpu_detect(void) {
    uint32_t eax, ebx, ecx, edx;
    
    // Get vendor string
    cpu_cpuid(0, &eax, &ebx, &ecx, &edx);
    
    // Store vendor string (12 characters)
    memcpy(cpu_info.vendor, &ebx, 4);
    memcpy(cpu_info.vendor + 4, &ecx, 4);
    memcpy(cpu_info.vendor + 8, &edx, 4);
    cpu_info.vendor[12] = '\0';
    
    // Get CPU features (eax=1)
    cpu_cpuid(1, &eax, &ebx, &ecx, &edx);
    
    cpu_info.features = edx;
    cpu_info.flags = ecx;
    
    // Extract family, model, stepping
    cpu_info.family = ((eax >> 8) & 0xF) + ((eax >> 20) & 0xFF);
    cpu_info.model = ((eax >> 4) & 0xF) | ((eax >> 12) & 0xF0);
    cpu_info.stepping = eax & 0xF;
    
    // Detect number of cores
    cpu_info.cores_per_cpu = 1;
    cpu_info.threads_per_core = 1;
    
    // Get brand string
    if (eax >= 0x80000004) {
        char brand[48] = {0};
        cpu_cpuid(0x80000002, (uint32_t*)(brand + 0), 
                  (uint32_t*)(brand + 4), 
                  (uint32_t*)(brand + 8), 
                  (uint32_t*)(brand + 12));
        cpu_cpuid(0x80000003, (uint32_t*)(brand + 16), 
                  (uint32_t*)(brand + 20), 
                  (uint32_t*)(brand + 24), 
                  (uint32_t*)(brand + 28));
        cpu_cpuid(0x80000004, (uint32_t*)(brand + 32), 
                  (uint32_t*)(brand + 36), 
                  (uint32_t*)(brand + 40), 
                  (uint32_t*)(brand + 44));
        
        // Clean up brand string
        int j = 0;
        for (int i = 0; i < 48 && brand[i]; i++) {
            if (brand[i] != ' ') {
                cpu_info.brand[j++] = brand[i];
            }
        }
        cpu_info.brand[j] = '\0';
    }
    
    // Check for specific features
    cpu_info.virtualized = false;
    
    // Default cache sizes
    cpu_info.cache_l1_size = 32;
    cpu_info.cache_l2_size = 256;
    cpu_info.cache_l3_size = 0;
    cpu_info.microcode_version = 0;
    cpu_info.cpu_freq_mhz = 1000;  // Default 1GHz
    cpu_info.bus_freq_mhz = 100;
}

// Get CPU info
void cpu_get_info(cpu_info_t* info) {
    if (info) {
        memcpy(info, &cpu_info, sizeof(cpu_info_t));
    }
}

// Initialize CPU
void cpu_init(void) {
    cpu_detect();
    
    // Set default features
    cpu_set_features(cpu_info.features);
    
    // Disable interrupts initially
    cpu_disable_interrupts();
}

// Set CPU features
void cpu_set_features(uint32_t features) {
    cpu_info.features = features;
}

// Enable paging
void cpu_enable_paging(void) {
    uint64_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= CR0_PG;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));
}

// Disable paging
void cpu_disable_paging(void) {
    uint64_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~CR0_PG;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));
}

// Enable interrupts
void cpu_enable_interrupts(void) {
    __asm__ volatile ("sti");
    interrupts_enabled = true;
}

// Disable interrupts
void cpu_disable_interrupts(void) {
    __asm__ volatile ("cli");
    interrupts_enabled = false;
}

// Check if interrupts are enabled
bool cpu_are_interrupts_enabled(void) {
    return interrupts_enabled;
}

// Halt CPU
void cpu_halt(void) {
    __asm__ volatile ("hlt");
}

// Idle CPU
void cpu_idle(void) {
    __asm__ volatile ("hlt");
}

// Relax CPU (hint for spin-wait loops)
void cpu_relax(void) {
    __asm__ volatile ("pause");
}

// Control register operations
uint64_t cpu_read_cr0(void) {
    uint64_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    return cr0;
}

void cpu_write_cr0(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr0" : : "r"(val));
}

uint64_t cpu_read_cr2(void) {
    uint64_t cr2;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
    return cr2;
}

uint64_t cpu_read_cr3(void) {
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

void cpu_write_cr3(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr3" : : "r"(val));
}

uint64_t cpu_read_cr4(void) {
    uint64_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    return cr4;
}

void cpu_write_cr4(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr4" : : "r"(val));
}

// Read RFLAGS
uint64_t cpu_read_rflags(void) {
    uint64_t rflags;
    __asm__ volatile ("pushf; pop %0" : "=r"(rflags));
    return rflags;
}

// IDT management (stub implementations)
static idt_entry_t idt[256];

void idt_init(void) {
    memset(idt, 0, sizeof(idt));
}

void idt_set_entry(uint8_t vector, uint64_t handler, uint8_t ist, uint8_t type_attr) {
    idt[vector].offset_low = handler & 0xFFFF;
    idt[vector].offset_mid = (handler >> 16) & 0xFFFF;
    idt[vector].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[vector].selector = 0x08;  // Kernel code segment
    idt[vector].ist = ist;
    idt[vector].type_attr = type_attr;
    idt[vector].reserved = 0;
}

void interrupt_register_handler(uint8_t vector, interrupt_handler_t handler) {
    // Would set up the handler in IDT
    (void)vector;
    (void)handler;
}

void interrupt_enable(uint8_t vector) {
    (void)vector;
}

void interrupt_disable(uint8_t vector) {
    (void)vector;
}

// APIC (stub)
void apic_init(void) {}
void apic_enable(void) {}
void apic_disable(void) {}
void apic_send_eoi(void) {}
void apic_send_ipi(uint8_t cpu, uint8_t vector) {
    (void)cpu;
    (void)vector;
}

// System timer (stub)
static uint64_t timer_ticks = 0;

void timer_init(uint32_t frequency) {
    (void)frequency;
    timer_ticks = 0;
}

void timer_set_handler(void (*handler)(void)) {
    (void)handler;
}

uint64_t timer_get_ticks(void) {
    return ++timer_ticks;
}

void timer_sleep(uint32_t ms) {
    // Simple delay
    for (volatile uint32_t i = 0; i < ms * 1000; i++) {
        __asm__ volatile ("nop");
    }
}

// Performance monitoring (stub)
void perf_init(void) {}

uint64_t perf_get_cycles(void) {
    uint64_t cycles;
    __asm__ volatile ("rdtsc" : "=A"(cycles));
    return cycles;
}

uint64_t perf_get_timestamp(void) {
    return perf_get_cycles();
}

#else

// Test mode implementations
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void cpu_init(void) {
    printf("[TEST] CPU subsystem initialized\n");
}

void cpu_detect(void) {}
void cpu_set_features(uint32_t features) {
    (void)features;
}
uint32_t cpu_get_features(void) { return 0; }
const char* cpu_get_vendor(void) { return "TestCPU"; }
void cpu_get_info(cpu_info_t* info) {
    if (info) {
        strcpy(info->vendor, "TestCPU");
        strcpy(info->brand, "Test CPU");
    }
}
void cpu_enable_paging(void) {}
void cpu_disable_paging(void) {}
void cpu_enable_interrupts(void) {}
void cpu_disable_interrupts(void) {}
bool cpu_are_interrupts_enabled(void) { return true; }
void cpu_halt(void) { exit(0); }
void cpu_idle(void) { usleep(1000); }
void cpu_relax(void) {}
void cpu_cpuid(uint32_t func, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    (void)func;
    if (eax) *eax = 0;
    if (ebx) *ebx = 0;
    if (ecx) *ecx = 0;
    if (edx) *edx = 0;
}
uint64_t cpu_read_cr0(void) { return 0; }
void cpu_write_cr0(uint64_t val) { (void)val; }
uint64_t cpu_read_cr2(void) { return 0; }
uint64_t cpu_read_cr3(void) { return 0; }
void cpu_write_cr3(uint64_t val) { (void)val; }
uint64_t cpu_read_cr4(void) { return 0; }
void cpu_write_cr4(uint64_t val) { (void)val; }
uint64_t cpu_read_rflags(void) { return 0; }
uint64_t cpu_read_msr(uint32_t msr) { (void)msr; return 0; }
void cpu_write_msr(uint32_t msr, uint64_t value) { (void)msr; (void)value; }

void idt_init(void) {}
void idt_set_entry(uint8_t vector, uint64_t handler, uint8_t ist, uint8_t type_attr) {
    (void)vector; (void)handler; (void)ist; (void)type_attr;
}
void interrupt_register_handler(uint8_t vector, interrupt_handler_t handler) {
    (void)vector; (void)handler;
}
void interrupt_enable(uint8_t vector) { (void)vector; }
void interrupt_disable(uint8_t vector) { (void)vector; }

void apic_init(void) {}
void apic_enable(void) {}
void apic_disable(void) {}
void apic_send_eoi(void) {}
void apic_send_ipi(uint8_t cpu, uint8_t vector) { (void)cpu; (void)vector; }

static uint64_t test_ticks = 0;
void timer_init(uint32_t frequency) { (void)frequency; test_ticks = 0; }
void timer_set_handler(void (*handler)(void)) { (void)handler; }
uint64_t timer_get_ticks(void) { return ++test_ticks; }
void timer_sleep(uint32_t ms) { usleep(ms * 1000); }

void perf_init(void) {}
uint64_t perf_get_cycles(void) { return 0; }
uint64_t perf_get_timestamp(void) { return 0; }

#endif
