#include "../include/kernel/console.h"
#include "../include/kernel/fs.h"
#include "../include/kernel/keyboard.h"
#include "../include/kernel/memory.h"
#include "../include/kernel/shell.h"
#include "../include/kernel/system.h"
#include "../include/kernel/cpu.h"
#include "../include/kernel/scheduler.h"
#include "../include/kernel/boot_info.h"
#include "../include/libc/stdio.h"
#include "../include/libc/string.h"

#ifndef TEST_MODE
// Only define kernel_main for actual kernel compilation
void kernel_main(void) {
    // Show boot messages like real kernel
    boot_message_start();
    
    // Initialize kernel components
    console_init();
    memory_init();
    memory_detect();
    memory_setup_paging();
    cpu_init();
    keyboard_init();
    system_init();
    scheduler_init();
    
    // Run hardware detection - this shows all the boot messages
    boot_diagnostics_init();
    boot_show_all();
    
    // Show boot complete message
    boot_message_end();
    
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
    printf("Welcome to Alpha OS 2.0! Type 'help' for commands.\n");
    console_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printf("New Features: Boot Diagnostics | Login | Scoring\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("\n");
    
    // Run the shell
    shell_run();
    
    // This point should never be reached
    printf("Alpha OS kernel halted.\n");
    system_halt();
}
#endif
