#include "../include/kernel/system.h"
#include "../include/libc/stdio.h"
#include "../include/libc/string.h"

static uint32_t system_uptime = 0;
static power_state_t current_power_state = POWER_STATE_ON;
static uint32_t boot_count = 0;

// System statistics
static system_stats_t system_stats = {0};

void system_init(void) {
    system_uptime = 0;
    current_power_state = POWER_STATE_ON;
    boot_count++;
    
#ifndef TEST_MODE
    // Initialize CPU and interrupt subsystems
    cpu_init();
    
    // Initialize timer
    timer_init(100);  // 100 Hz
    
    printf("System initialized\n");
    printf("  Boot count: %u\n", boot_count);
#else
    printf("[TEST] System subsystem initialized\n");
#endif
}

uint32_t system_get_uptime(void) {
    return ++system_uptime;
}

void system_delay(uint32_t ms) {
#ifdef TEST_MODE
    usleep(ms * 1000);
#else
    for (volatile uint32_t i = 0; i < ms * 1000; i++) {
        __asm__ volatile ("nop");
    }
#endif
}

// Power management
void system_power_set_state(power_state_t state) {
    current_power_state = state;
}

power_state_t system_power_get_state(void) {
    return current_power_state;
}

void system_reboot(void) {
#ifndef TEST_MODE
    // Try to reboot via keyboard controller
    uint8_t status;
    
    // Disable interrupts
    cli();
    
    // Wait for keyboard controller
    for (int i = 0; i < 10; i++) {
        io_wait();
    }
    
    // Send reboot command
    outb(0x64, 0xFE);
    
    // If that fails, halt
    hlt();
#else
    printf("System reboot requested (test mode)\n");
#endif
}

void system_shutdown(void) {
#ifndef TEST_MODE
    cli();
    printf("System shutting down...\n");
    hlt();
#else
    printf("System shutdown requested (test mode)\n");
#endif
}

// System information
void system_get_info(system_info_t* info) {
    if (info) {
        strcpy(info->kernel_version, "AlphaOS 2.0");
        strcpy(info->build_date, __DATE__);
        strcpy(info->build_time, __TIME__);
        info->uptime_seconds = system_uptime;
        info->cpu_count = 1;
        info->memory_total_mb = 8;
        info->boot_count = boot_count;
    }
}

// System statistics
void system_get_stats(system_stats_t* stats) {
    if (stats) {
        memcpy(stats, &system_stats, sizeof(system_stats_t));
    }
}

// Watchdog timer (stub)
void watchdog_init(uint32_t timeout_ms) {
    (void)timeout_ms;
}

void watchdog_feed(void) {
}

void watchdog_disable(void) {
}

// System panic
void panic(const char* msg) {
#ifndef TEST_MODE
    cli();
    printf("\n*** KERNEL PANIC ***\n");
    printf("Message: %s\n", msg);
    printf("System halted.\n");
    dump_registers();
    hlt();
#else
    printf("\n*** PANIC (test mode) ***\n");
    printf("Message: %s\n", msg);
#endif
}

void dump_registers(void) {
#ifdef TEST_MODE
    printf("Register dump not available in test mode\n");
#else
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    uint32_t esi = 0, edi = 0, ebp = 0, esp = 0, eip = 0, eflags = 0;
    
    // Read CR0-CR4
    __asm__ volatile ("mov %%cr0, %%eax" : "=a"(eax));
    __asm__ volatile ("mov %%cr2, %%eax" : "=a"(ecx));
    __asm__ volatile ("mov %%cr3, %%eax" : "=a"(edx));
    __asm__ volatile ("mov %%cr4, %%eax" : "=a"(esi));
    
    printf("Control Registers:\n");
    printf("  CR0=0x%08X CR2=0x%08X CR3=0x%08X CR4=0x%08X\n", eax, ecx, edx, esi);
#endif
}
