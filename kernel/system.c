#include "../include/kernel/system.h"
#include "../include/libc/stdio.h"

static uint32_t system_uptime = 0;

void system_init(void) {
    system_uptime = 0;
    
#ifndef TEST_MODE
    // In kernel mode, we would initialize interrupts, timers, etc.
    printf("System components initialized\n");
#endif
}

uint32_t system_get_uptime(void) {
    return ++system_uptime; // Simple increment for now
}

void system_delay(uint32_t ms) {
#ifdef TEST_MODE
    // In test mode, use system sleep
    usleep(ms * 1000);
#else
    // Simple busy wait (not accurate, but works for demonstration)
    for (volatile uint32_t i = 0; i < ms * 1000; i++) {
        // Busy wait
    }
#endif
}
