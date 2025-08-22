#include "../include/kernel/keyboard.h"
#include "../include/kernel/system.h"
#include "../include/kernel/console.h" 
#include "../include/libc/string.h"

#ifdef TEST_MODE
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static struct termios old_termios;
static bool termios_saved = false;

void keyboard_init(void) {
    // Set terminal to raw mode for character-by-character input
    if (tcgetattr(STDIN_FILENO, &old_termios) == 0) {
        termios_saved = true;
        struct termios new_termios = old_termios;
        new_termios.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
        
        // Set non-blocking mode
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }
}

char keyboard_read(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        return c;
    }
    return 0;
}

bool keyboard_available(void) {
    char c;
    int result = read(STDIN_FILENO, &c, 1);
    if (result == 1) {
        // Put the character back (simple implementation)
        ungetc(c, stdin);
        return true;
    }
    return false;
}

int keyboard_get_line(char* buffer, size_t buffer_size) {
    size_t pos = 0;
    char c;
    
    while (pos < buffer_size - 1) {
        c = getchar();
        
        if (c == '\n' || c == '\r') {
            break;
        } else if (c == '\b' || c == 127) { // Backspace or DEL
            if (pos > 0) {
                pos--;
                printf("\b \b"); // Erase character on screen
                fflush(stdout);
            }
        } else if (c >= ' ' && c <= '~') { // Printable characters
            buffer[pos++] = c;
            putchar(c);
            fflush(stdout);
        }
    }
    
    buffer[pos] = '\0';
    putchar('\n');
    return pos;
}

void keyboard_clear_buffer(void) {
    // Clear any pending input
    while (keyboard_available()) {
        keyboard_read();
    }
}

void keyboard_poll(void) {
    // In test mode, polling is handled by the terminal
}

// Cleanup function for test mode
void keyboard_cleanup(void) {
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    }
}

#else
// Kernel mode implementation

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 256

// Keyboard buffer
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int keyboard_buffer_head = 0;
static int keyboard_buffer_tail = 0;

// US keyboard layout
static const char keyboard_us[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void keyboard_init(void) {
    keyboard_buffer_head = 0;
    keyboard_buffer_tail = 0;
}

char keyboard_read(void) {
    if (keyboard_buffer_head == keyboard_buffer_tail) {
        return 0; // Buffer empty
    }
    
    char c = keyboard_buffer[keyboard_buffer_tail];
    keyboard_buffer_tail = (keyboard_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

bool keyboard_available(void) {
    return keyboard_buffer_head != keyboard_buffer_tail;
}

int keyboard_get_line(char* buffer, size_t buffer_size) {
    size_t pos = 0;
    char c;
    
    while (pos < buffer_size - 1) {
        // Poll for input
        while (!keyboard_available()) {
            keyboard_poll();
        }
        
        c = keyboard_read();
        
        if (c == '\n' || c == '\r') {
            break;
        } else if (c == '\b') { // Backspace
            if (pos > 0) {
                pos--;
                // Echo backspace to console
                console_putchar('\b');
                console_putchar(' ');
                console_putchar('\b');
            }
        } else if (c >= ' ' && c <= '~') { // Printable characters
            buffer[pos++] = c;
            console_putchar(c); // Echo character
        }
    }
    
    buffer[pos] = '\0';
    console_putchar('\n');
    return pos;
}

void keyboard_clear_buffer(void) {
    keyboard_buffer_head = keyboard_buffer_tail;
}

void keyboard_poll(void) {
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    
    if (status & 0x01) {
        uint8_t scancode = inb(KEYBOARD_DATA_PORT);
        
        // Only process key press events (not key release)
        if (!(scancode & 0x80)) {
            char c = keyboard_us[scancode];
            
            if (c) {
                int next_head = (keyboard_buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
                if (next_head != keyboard_buffer_tail) {
                    keyboard_buffer[keyboard_buffer_head] = c;
                    keyboard_buffer_head = next_head;
                }
            }
        }
    }
}

#endif
