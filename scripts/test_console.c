#include "../include/kernel/console.h"
#include "../include/kernel/memory.h"
#include "../include/libc/stdio.h"

int main(void) {
    printf("=== Console Test ===\n");
    
    memory_init();
    console_init();
    
    printf("Testing console colors...\n");
    
    console_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
    printf("Red text\n");
    
    console_set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    printf("Green text\n");
    
    console_set_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK);
    printf("Blue text\n");
    
    console_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    printf("Yellow text\n");
    
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Back to normal\n");
    
    printf("Testing special characters:\n");
    printf("Tab:\tTabbed text\n");
    printf("Newline:\nNew line\n");
    
    printf("Console test completed!\n");
    
    return 0;
}
