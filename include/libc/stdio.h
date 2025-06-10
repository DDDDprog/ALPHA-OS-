#ifndef STDIO_H
#define STDIO_H

#include "../kernel/types.h"

#ifdef TEST_MODE
#include <stdarg.h>
#else
// Simple va_list implementation for kernel mode
typedef __builtin_va_list va_list;
#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)
#endif

// Standard I/O functions
int putchar(int c);
int puts(const char* str);
int printf(const char* format, ...);
int vprintf(const char* format, va_list args);
int sprintf(char* buffer, const char* format, ...);
int vsprintf(char* buffer, const char* format, va_list args);
int snprintf(char* buffer, size_t size, const char* format, ...);

// Simple file operations for testing
#ifdef TEST_MODE
#include <stdio.h>
char* fgets(char* s, int size, FILE* stream);
#else
// Stub implementations for kernel mode
typedef struct FILE FILE;
extern FILE* stdin;
char* fgets(char* s, int size, FILE* stream);
#endif

#endif // STDIO_H
