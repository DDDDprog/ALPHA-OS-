#include "../include/kernel/console.h"
#include "../include/kernel/fs.h"
#include "../include/kernel/keyboard.h"
#include "../include/kernel/memory.h"
#include "../include/kernel/shell.h"
#include "../include/kernel/system.h"
#include "../include/libc/stdio.h"
#include "../include/libc/string.h"

#ifndef TEST_MODE
// Only define kernel_main for actual kernel compilation
void kernel_main(void) {
    // Initialize kernel components
    console_init();
    memory_init();
    keyboard_init();
    system_init();
    
    // Display welcome message
    console_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printf("Alpha OS - Advanced Learning Platform\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Version 1.0.0 | AlphaKernel | Build 2025\n");
    printf("Built with standard GCC for educational purposes\n\n");
    
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
    printf("Welcome to Alpha OS 2025! Type 'help' for commands or 'root' for admin access.\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("\n");
    
    // Run the shell
    shell_run();
    
    // This point should never be reached
    printf("Alpha OS kernel halted.\n");
    system_halt();
}
#endif
