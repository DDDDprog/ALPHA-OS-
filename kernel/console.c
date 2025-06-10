#include "../include/kernel/console.h"
#include "../include/kernel/system.h"
#include "../include/libc/string.h"

#ifdef TEST_MODE
#include <stdio.h>
#include <stdlib.h>

// Test mode - use standard I/O
static enum vga_color current_fg = VGA_COLOR_LIGHT_GREY;
static enum vga_color current_bg = VGA_COLOR_BLACK;

void console_init(void) {
    printf("\033[2J\033[H"); // Clear screen and move cursor to home
}

void console_clear(void) {
    printf("\033[2J\033[H"); // Clear screen and move cursor to home
}

void console_set_color(enum vga_color fg, enum vga_color bg) {
    current_fg = fg;
    current_bg = bg;
    // Set ANSI color codes
    printf("\033[%d;%dm", 30 + fg, 40 + bg);
}

void console_putchar(char c) {
    putchar(c);
    fflush(stdout);
}

void console_write(const char* data) {
    printf("%s", data);
    fflush(stdout);
}

void console_write_size(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        putchar(data[i]);
    }
    fflush(stdout);
}

void console_get_cursor(size_t* row, size_t* col) {
    *row = 0;
    *col = 0;
}

void console_set_cursor(size_t row, size_t col) {
    printf("\033[%zu;%zuH", row + 1, col + 1);
}

#else
// Kernel mode - use VGA text mode

// Forward declaration
void console_putchar(char c);

// VGA text mode constants
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

// Console state
static size_t console_row;
static size_t console_column;
static uint8_t console_color;
static uint16_t* console_buffer;

// Create a VGA entry
static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) c | (uint16_t) color << 8;
}

// Create a VGA color
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

void console_init(void) {
    console_row = 0;
    console_column = 0;
    console_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    console_buffer = VGA_MEMORY;
    console_clear();
}

void console_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            console_buffer[index] = vga_entry(' ', console_color);
        }
    }
    console_row = 0;
    console_column = 0;
}

void console_set_color(enum vga_color fg, enum vga_color bg) {
    console_color = vga_entry_color(fg, bg);
}

// Scroll the console up one line
static void console_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t dst_index = y * VGA_WIDTH + x;
            const size_t src_index = (y + 1) * VGA_WIDTH + x;
            console_buffer[dst_index] = console_buffer[src_index];
        }
    }
    
    // Clear the last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        console_buffer[index] = vga_entry(' ', console_color);
    }
}

void console_putchar(char c) {
    if (c == '\n') {
        console_column = 0;
        if (++console_row == VGA_HEIGHT) {
            console_scroll();
            console_row = VGA_HEIGHT - 1;
        }
        return;
    }
    
    if (c == '\r') {
        console_column = 0;
        return;
    }
    
    if (c == '\t') {
        console_column = (console_column + 4) & ~3;
        if (console_column >= VGA_WIDTH) {
            console_column = 0;
            if (++console_row == VGA_HEIGHT) {
                console_scroll();
                console_row = VGA_HEIGHT - 1;
            }
        }
        return;
    }
    
    if (c == '\b') {
        if (console_column > 0) {
            console_column--;
            const size_t index = console_row * VGA_WIDTH + console_column;
            console_buffer[index] = vga_entry(' ', console_color);
        }
        return;
    }
    
    const size_t index = console_row * VGA_WIDTH + console_column;
    console_buffer[index] = vga_entry(c, console_color);
    
    if (++console_column == VGA_WIDTH) {
        console_column = 0;
        if (++console_row == VGA_HEIGHT) {
            console_scroll();
            console_row = VGA_HEIGHT - 1;
        }
    }
}

void console_write(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        console_putchar(data[i]);
    }
}

void console_write_size(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        console_putchar(data[i]);
    }
}

void console_get_cursor(size_t* row, size_t* col) {
    *row = console_row;
    *col = console_column;
}

void console_set_cursor(size_t row, size_t col) {
    if (row < VGA_HEIGHT && col < VGA_WIDTH) {
        console_row = row;
        console_column = col;
    }
}

#endif
