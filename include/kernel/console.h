#ifndef CONSOLE_H
#define CONSOLE_H

#include "types.h"

// VGA text mode color constants
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,  // This is actually yellow
    VGA_COLOR_WHITE = 15,
};

// Color aliases for better readability
#define VGA_COLOR_YELLOW VGA_COLOR_BROWN
#define VGA_COLOR_LIGHT_YELLOW VGA_COLOR_LIGHT_BROWN

// Initialize the console
void console_init(void);

// Clear the console
void console_clear(void);

// Set the foreground and background colors
void console_set_color(enum vga_color fg, enum vga_color bg);

// Write a single character to the console
void console_putchar(char c);

// Write a string to the console
void console_write(const char* data);

// Write a string with a specified length to the console
void console_write_size(const char* data, size_t size);

// Get current cursor position
void console_get_cursor(size_t* row, size_t* col);

// Set cursor position
void console_set_cursor(size_t row, size_t col);

#endif // CONSOLE_H
