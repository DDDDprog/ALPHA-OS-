#ifndef SYSTEM_H
#define SYSTEM_H

#include "types.h"
#include "cpu.h"

#ifdef TEST_MODE
// Test mode - use regular system calls
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#define system_exit(code) exit(code)
#define system_halt() exit(0)
#else
// Kernel mode - use inline assembly

// I/O port operations
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Wait a small amount of time
static inline void io_wait(void) {
    outb(0x80, 0);
}

// Disable interrupts
static inline void cli(void) {
    __asm__ volatile ("cli");
}

// Enable interrupts
static inline void sti(void) {
    __asm__ volatile ("sti");
}

// Halt the CPU
static inline void hlt(void) {
    __asm__ volatile ("hlt");
}

// Save and restore interrupts
static inline uint32_t save_flags(void) {
    uint32_t flags;
    __asm__ volatile ("pushf; pop %0" : "=r"(flags));
    return flags;
}

static inline void restore_flags(uint32_t flags) {
    __asm__ volatile ("push %0; popf" : : "r"(flags));
}

// Enable/disable interrupts atomically
#define ENABLE_INTERRUPTS() __asm__ volatile ("sti")
#define DISABLE_INTERRUPTS() __asm__ volatile ("cli")

// Memory barriers
static inline void mb(void) {
    __asm__ volatile ("mfence");
}

static inline void rmb(void) {
    __asm__ volatile ("lfence");
}

static inline void wmb(void) {
    __asm__ volatile ("sfence");
}

// Read timestamp counter
static inline uint64_t rdtsc(void) {
    uint32_t low, high;
    __asm__ volatile ("rdtsc" : "=a"(low), "=d"(high));
    return ((uint64_t)high << 32) | low;
}

#define system_exit(code) do { while(1) hlt(); } while(0)
#define system_halt() do { while(1) hlt(); } while(0)
#endif

// System initialization
void system_init(void);

// Get system uptime in ticks
uint32_t system_get_uptime(void);

// Simple delay function
void system_delay(uint32_t ms);

// Power management
typedef enum {
    POWER_STATE_ON = 0,
    POWER_STATE_SLEEP = 1,
    POWER_STATE_SUSPEND = 2,
    POWER_STATE_HIBERNATE = 3,
    POWER_STATE_OFF = 4
} power_state_t;

void system_power_set_state(power_state_t state);
power_state_t system_power_get_state(void);
void system_reboot(void);
void system_shutdown(void);

// System information
typedef struct {
    char kernel_version[64];
    char build_date[32];
    char build_time[32];
    uint32_t uptime_seconds;
    uint32_t cpu_count;
    uint32_t memory_total_mb;
    uint32_t boot_count;
} system_info_t;

void system_get_info(system_info_t* info);

// Performance counters
typedef struct {
    uint64_t context_switches;
    uint64_t interrupts;
    uint64_t page_faults;
    uint64_t cpu_cycles;
} system_stats_t;

void system_get_stats(system_stats_t* stats);

// Watchdog timer
void watchdog_init(uint32_t timeout_ms);
void watchdog_feed(void);
void watchdog_disable(void);

// System panic
void panic(const char* msg);
void dump_registers(void);

#endif // SYSTEM_H
