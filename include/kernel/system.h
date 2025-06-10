#ifndef SYSTEM_H
#define SYSTEM_H

#include "types.h"

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

#define system_exit(code) do { while(1) hlt(); } while(0)
#define system_halt() do { while(1) hlt(); } while(0)
#endif

// System initialization
void system_init(void);

// Get system uptime in ticks
uint32_t system_get_uptime(void);

// Simple delay function
void system_delay(uint32_t ms);

#endif // SYSTEM_H
