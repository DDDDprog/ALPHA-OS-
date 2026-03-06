#ifndef TYPES_H
#define TYPES_H

// Basic type definitions - only for kernel mode
#ifndef TEST_MODE

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef uint32_t size_t;
typedef int32_t ssize_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef EOF
#define EOF (-1)
#endif

// Boolean type
typedef enum {
    false = 0,
    true = 1
} bool;

#else
// In test mode, use standard library types
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#endif

#endif // TYPES_H
#ifndef TIME_T
#define TIME_T
typedef unsigned long time_t;
#endif
typedef unsigned long sigset_t;
typedef long suseconds_t;
typedef long off_t;

/* Network byte order */
unsigned short ntohs(unsigned short x);
