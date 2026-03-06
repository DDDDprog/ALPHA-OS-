#include "../include/kernel/console.h"
#include "../include/kernel/fs.h"
#include "../include/kernel/keyboard.h"
#include "../include/kernel/memory.h"
#include "../include/kernel/shell.h"
#include "../include/kernel/system.h"
#include "../include/kernel/cpu.h"
#include "../include/kernel/scheduler.h"
#include "../include/libc/stdio.h"
#include "../include/libc/string.h"

#ifndef TEST_MODE
// Only define kernel_main for actual kernel compilation
void kernel_main(void) {
    // Initialize kernel components
    console_init();
    memory_init();
    memory_detect();
    memory_setup_paging();
    cpu_init();
    keyboard_init();
    system_init();
    scheduler_init();
    
    // Display welcome message
    console_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printf("======================================\n");
    printf("   Alpha OS 2.0 - Modern Kernel\n");
    printf("======================================\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Version 2.0.0 | AlphaKernel | Build 2026\n");
    printf("Built with modern GCC for educational purposes\n\n");
    
    // Display CPU info
    cpu_info_t cpu_info;
    cpu_get_info(&cpu_info);
    console_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    printf("[CPU] Vendor: %s\n", cpu_info.vendor);
    printf("[CPU] Features: 0x%08X\n", cpu_info.features);
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    // Display memory info
    memory_stats_t mem_stats;
    memory_get_extended_stats(&mem_stats);
    console_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    printf("[MEM] Total Pages: %u\n", mem_stats.total_pages);
    printf("[MEM] Free Pages: %u\n", mem_stats.free_pages);
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    // Initialize file system
    console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("[OK] ");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Initializing Alpha File System...\n");
    fs_init();
    
    // Initialize shell
    console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("[OK] ");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Initializing Alpha Shell...\n");
    shell_init();
    
    printf("\n");
    console_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    printf("Welcome to Alpha OS 2.0! Type 'help' for commands or 'root' for admin access.\n");
    console_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printf("New Features: Process Scheduling | Virtual Memory | CPU Detection\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("\n");
    
    // Run the shell
    shell_run();
    
    // This point should never be reached
    printf("Alpha OS kernel halted.\n");
    system_halt();
}
#endif
