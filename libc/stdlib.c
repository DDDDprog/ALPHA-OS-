#include "../include/libc/stdlib.h"
#include "../include/kernel/memory.h"
#include "../include/libc/string.h"

// Random number generation
static unsigned int next = 1;

int atoi(const char* nptr) {
    int result = 0;
    int sign = 1;
    
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n') {
        nptr++;
    }
    
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }
    
    while (*nptr >= '0' && *nptr <= '9') {
        result = result * 10 + (*nptr - '0');
        nptr++;
    }
    
    return result * sign;
}

long atol(const char* nptr) {
    long result = 0;
    int sign = 1;
    
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n') {
        nptr++;
    }
    
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }
    
    while (*nptr >= '0' && *nptr <= '9') {
        result = result * 10 + (*nptr - '0');
        nptr++;
    }
    
    return result * sign;
}

double atof(const char* nptr) {
    double result = 0.0;
    int sign = 1;
    double fraction = 1.0;
    
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n') {
        nptr++;
    }
    
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }
    
    while (*nptr >= '0' && *nptr <= '9') {
        result = result * 10.0 + (*nptr - '0');
        nptr++;
    }
    
    if (*nptr == '.') {
        nptr++;
        while (*nptr >= '0' && *nptr <= '9') {
            fraction /= 10.0;
            result += (*nptr - '0') * fraction;
            nptr++;
        }
    }
    
    return result * sign;
}

int rand(void) {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) {
    next = seed;
}

void exit(int status) {
#ifdef TEST_MODE
    exit(status);
#else
    // In kernel mode, halt the system
    while (1) {
        __asm__ volatile ("hlt");
    }
#endif
}

void abort(void) {
#ifdef TEST_MODE
    abort();
#else
    // In kernel mode, halt the system
    while (1) {
        __asm__ volatile ("hlt");
    }
#endif
}

int abs(int j) {
    return (j < 0) ? -j : j;
}

long labs(long j) {
    return (j < 0) ? -j : j;
}

char* itoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;
    
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);
    
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    
    return str;
}

char* ltoa(long value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    long tmp_value;
    
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);
    
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    
    return str;
}
