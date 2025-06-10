#include "../include/libc/stdio.h"
#include "../include/kernel/console.h"
#include "../include/libc/string.h"

#ifdef TEST_MODE
// In test mode, use system stdio
#include <stdio.h>
#include <stdarg.h>

int putchar(int c) {
    return fputc(c, stdout);  // Changed from putchar(c) to avoid recursion
}

int puts(const char* str) {
    return fputs(str, stdout);  // Changed from puts(str) to avoid recursion
}

#else
// For kernel mode, we need our own va_list
typedef __builtin_va_list va_list;
#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)

// Kernel mode implementation

int putchar(int c) {
    console_putchar((char)c);
    return c;
}

int puts(const char* str) {
    console_write(str);
    console_putchar('\n');
    return 0;
}

#endif

// Common implementation for both modes
int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

int vprintf(const char* format, va_list args) {
    char buffer[1024];
    int result = vsprintf(buffer, format, args);
    
#ifdef TEST_MODE
    printf("%s", buffer);
#else
    console_write(buffer);
#endif
    
    return result;
}

int sprintf(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsprintf(buffer, format, args);
    va_end(args);
    return result;
}

int snprintf(char* buffer, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // Use vsprintf and then truncate if necessary
    int result = vsprintf(buffer, format, args);
    va_end(args);
    
    // Simple truncation if needed
    if (result >= (int)size && size > 0) {
        buffer[size - 1] = '\0';
        result = size - 1;
    }
    
    return result;
}

int vsprintf(char* buffer, const char* format, va_list args) {
    int written = 0;
    
    while (*format && written < 1023) {
        if (*format != '%') {
            buffer[written++] = *format++;
            continue;
        }
        
        format++; // Skip '%'
        
        switch (*format) {
            case 'c': {
                char c = (char)va_arg(args, int);
                buffer[written++] = c;
                break;
            }
            case 's': {
                const char* s = va_arg(args, const char*);
                if (!s) s = "(null)";
                while (*s && written < 1023) {
                    buffer[written++] = *s++;
                }
                break;
            }
            case 'd':
            case 'i': {
                int value = va_arg(args, int);
                if (value < 0) {
                    buffer[written++] = '-';
                    value = -value;
                }
                
                char temp[32];
                int temp_pos = 0;
                do {
                    temp[temp_pos++] = '0' + (value % 10);
                    value /= 10;
                } while (value > 0);
                
                while (temp_pos > 0 && written < 1023) {
                    buffer[written++] = temp[--temp_pos];
                }
                break;
            }
            case 'u': {
                unsigned int value = va_arg(args, unsigned int);
                
                char temp[32];
                int temp_pos = 0;
                do {
                    temp[temp_pos++] = '0' + (value % 10);
                    value /= 10;
                } while (value > 0);
                
                while (temp_pos > 0 && written < 1023) {
                    buffer[written++] = temp[--temp_pos];
                }
                break;
            }
            case 'x':
            case 'X': {
                unsigned int value = va_arg(args, unsigned int);
                const char* digits = (*format == 'x') ? "0123456789abcdef" : "0123456789ABCDEF";
                
                char temp[32];
                int temp_pos = 0;
                do {
                    temp[temp_pos++] = digits[value & 0xF];
                    value >>= 4;
                } while (value > 0);
                
                while (temp_pos > 0 && written < 1023) {
                    buffer[written++] = temp[--temp_pos];
                }
                break;
            }
            case '%': {
                buffer[written++] = '%';
                break;
            }
            default: {
                buffer[written++] = '%';
                if (written < 1023) {
                    buffer[written++] = *format;
                }
                break;
            }
        }
        
        format++;
    }
    
    buffer[written] = '\0';
    return written;
}

#ifdef TEST_MODE
char* fgets(char* s, int size, FILE* stream) {
    return fgets(s, size, stream);
}
#else
// Stub implementation for kernel mode
typedef struct FILE FILE;
FILE* stdin = NULL;

char* fgets(char* s, int size, FILE* stream) {
    // Simple implementation using keyboard input
    return NULL; // Not implemented in kernel mode
}
#endif
