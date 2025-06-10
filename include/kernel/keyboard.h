#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#define KEYBOARD_BUFFER_SIZE 256

// Special key codes
#define KEY_BACKSPACE 0x08
#define KEY_TAB       0x09
#define KEY_ENTER     0x0A
#define KEY_ESCAPE    0x1B
#define KEY_DELETE    0x7F

// Initialize the keyboard
void keyboard_init(void);

// Read a character from the keyboard buffer
char keyboard_read(void);

// Check if a character is available in the keyboard buffer
bool keyboard_available(void);

// Get a line of input from the keyboard
int keyboard_get_line(char* buffer, size_t buffer_size);

// Clear the keyboard buffer
void keyboard_clear_buffer(void);

// Poll the keyboard (call this regularly)
void keyboard_poll(void);

#endif // KEYBOARD_H
