#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

#define HEAP_SIZE (1024 * 1024) // 1 MB heap
#define BLOCK_MAGIC 0x1234ABCD

// Memory block header
typedef struct block_header {
    uint32_t magic;
    size_t size;
    uint8_t is_free;
    struct block_header* next;
} block_header_t;

// Initialize memory management
void memory_init(void);

// Allocate memory
void* malloc(size_t size);

// Free allocated memory
void free(void* ptr);

// Reallocate memory
void* realloc(void* ptr, size_t size);

// Allocate and zero memory
void* calloc(size_t nmemb, size_t size);

// Get memory statistics
void memory_get_stats(size_t* total, size_t* used, size_t* free);

// Check heap integrity
bool memory_check_integrity(void);

#endif // MEMORY_H
