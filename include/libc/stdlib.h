#ifndef STDLIB_H
#define STDLIB_H

#include "../kernel/types.h"

// Memory allocation (implemented in memory.c)
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

// String conversion
int atoi(const char* nptr);
long atol(const char* nptr);
double atof(const char* nptr);

// Random number generation
int rand(void);
void srand(unsigned int seed);
#define RAND_MAX 32767

// Process control
void exit(int status);
void abort(void);

// Integer arithmetic
int abs(int j);
long labs(long j);

// Utility functions
char* itoa(int value, char* str, int base);
char* ltoa(long value, char* str, int base);

#endif // STDLIB_H
