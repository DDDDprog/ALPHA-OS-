#include "../include/kernel/keyboard.h"
#include "../include/kernel/system.h"
#include "../include/kernel/console.h" 
#include "../include/libc/string.h"
#include "../include/libc/stdio.h"

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
        } else if (c == 9) { // Tab - tab completion
            if (pos > 0) {
                buffer[pos] = '\0';
                keyboard_do_tab_completion(buffer, buffer, buffer_size);
                pos = strlen(buffer);
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

// Command history for shell (up/down arrows)
#define KEYBOARD_HISTORY_SIZE 32
static char keyboard_history[KEYBOARD_HISTORY_SIZE][256];
static int keyboard_history_count = 0;
static int keyboard_history_index = -1;
static char current_input[256];


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
    int history_pos = -1;
    current_input[0] = '\0';

    while (pos < buffer_size - 1) {
        while (!keyboard_available()) {
            keyboard_poll();
        }

        c = keyboard_read();

        if (c == '\x1b') {
            while (!keyboard_available()) keyboard_poll();
            char next = keyboard_read();
            if (next == '[') {
                while (!keyboard_available()) keyboard_poll();
                char key = keyboard_read();

                if (key == 'A' && keyboard_history_count > 0) {
                    if (history_pos == -1) {
                        strncpy(current_input, buffer, 255);
                        history_pos = keyboard_history_count - 1;
                    } else if (history_pos > 0) {
                        history_pos--;
                    }
                    while (pos > 0) { console_putchar('\b'); console_putchar(' '); console_putchar('\b'); pos--; }
                    const char* hist = keyboard_history[history_pos % KEYBOARD_HISTORY_SIZE];
                    size_t len = strlen(hist);
                    if (len > buffer_size - 1) len = buffer_size - 1;
                    memcpy(buffer, hist, len);
                    buffer[len] = '\0';
                    pos = len;
                    console_write(buffer);
                } else if (key == 'B' && history_pos != -1) {
                    history_pos++;
                    if (history_pos >= keyboard_history_count) {
                        history_pos = -1;
                        while (pos > 0) { console_putchar('\b'); console_putchar(' '); console_putchar('\b'); pos--; }
                        buffer[0] = '\0';
                        if (strlen(current_input) > 0) {
                            size_t len = strlen(current_input);
                            if (len > buffer_size - 1) len = buffer_size - 1;
                            memcpy(buffer, current_input, len);
                            buffer[len] = '\0';
                            pos = len;
                            console_write(buffer);
                        }
                    } else {
                        while (pos > 0) { console_putchar('\b'); console_putchar(' '); console_putchar('\b'); pos--; }
                        const char* hist = keyboard_history[history_pos % KEYBOARD_HISTORY_SIZE];
                        size_t len = strlen(hist);
                        if (len > buffer_size - 1) len = buffer_size - 1;
                        memcpy(buffer, hist, len);
                        buffer[len] = '\0';
                        pos = len;
                        console_write(buffer);
                    }
                }
            }
            continue;
        }

        if (c == '\n' || c == '\r') {
            break;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                console_putchar('\b');
                console_putchar(' ');
                console_putchar('\b');
            }
        } else if (c == 9) { // Tab - tab completion
            if (pos > 0) {
                buffer[pos] = '\0';
                keyboard_do_tab_completion(buffer, buffer, buffer_size);
                pos = strlen(buffer);
            }
        } else if (c >= ' ' && c <= '~') {
            buffer[pos++] = c;
            console_putchar(c);
            history_pos = -1;
        }
    }

    buffer[pos] = '\0';
    console_putchar('\n');
    if (pos > 0) keyboard_add_history(buffer);
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

// Add to history - call this after command is executed
void keyboard_add_history(const char* cmd) {
    if (cmd && strlen(cmd) > 0) {
        // Don't add duplicate of last command
        if (keyboard_history_count == 0 || strcmp(cmd, keyboard_history[keyboard_history_count - 1]) != 0) {
            strncpy(keyboard_history[keyboard_history_count % KEYBOARD_HISTORY_SIZE], cmd, 255);
            keyboard_history[keyboard_history_count % KEYBOARD_HISTORY_SIZE][255] = '\0';
            keyboard_history_count++;
        }
        keyboard_history_index = keyboard_history_count;
    }
}

// Get history count
int keyboard_get_history_count(void) {
    return keyboard_history_count;
}

// Get history item
const char* keyboard_get_history(int index) {
    if (index >= 0 && index < keyboard_history_count) {
        return keyboard_history[index % KEYBOARD_HISTORY_SIZE];
    }
    return NULL;
}

// Tab completion function
void keyboard_do_tab_completion(const char* partial, char* buffer, size_t buffer_size) {
    if (!partial || strlen(partial) == 0) return;
    
    // Get current command state from shell
    extern int shell_get_command_count(void);
    extern const char* shell_get_command_name(int index);
    
    int partial_len = strlen(partial);
    char matches[10][64];
    int match_count = 0;
    
    // Check commands
    for (int i = 0; i < shell_get_command_count(); i++) {
        const char* cmd = shell_get_command_name(i);
        if (cmd && strncmp(cmd, partial, partial_len) == 0) {
            if (match_count < 10) {
                strncpy(matches[match_count], cmd, 63);
                matches[match_count][63] = '\0';
                match_count++;
            }
        }
    }
    
    // Single match - complete it
    if (match_count == 1) {
        const char* match = matches[0];
        size_t match_len = strlen(match);
        if (match_len > (size_t)partial_len && match_len < buffer_size) {
            memcpy(buffer + partial_len, match + partial_len, match_len - partial_len);
            buffer[match_len] = '\0';
            // Echo completed
            for (size_t j = partial_len; j < match_len; j++) {
                console_putchar(buffer[j]);
            }
        }
    } else if (match_count > 1) {
        // Show all matches
        console_putchar('\n');
        for (int i = 0; i < match_count; i++) {
            console_write(matches[i]); console_write("  ");
        }
        console_putchar('\n');
        // Print prompt again (need to call shell)
        extern void shell_print_prompt(void);
        shell_print_prompt();
        console_write(buffer);
    }
}
