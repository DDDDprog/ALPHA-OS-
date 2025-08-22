#include "../include/kernel/memory.h"
#include "../include/libc/string.h"

#ifdef TEST_MODE
#include <stdlib.h>
#include <stdio.h>  

// In test mode, use system malloc/free
void memory_init(void) {
    // Nothing to do in test mode
}

void memory_get_stats(size_t* total, size_t* used, size_t* free) {
    if (total) *total = HEAP_SIZE;
    if (used) *used = 0; // Can't easily track in test mode
    if (free) *free = HEAP_SIZE;
}

bool memory_check_integrity(void) {
    return true; // Assume system malloc is working
}

#else
// Kernel mode implementation

// Heap memory
static uint8_t heap[HEAP_SIZE] __attribute__((aligned(16)));
static block_header_t* heap_start = NULL;
static size_t total_memory = HEAP_SIZE;
static size_t free_memory = HEAP_SIZE;

void memory_init(void) {
    // Initialize the heap with a single free block
    heap_start = (block_header_t*)heap;
    heap_start->magic = BLOCK_MAGIC;
    heap_start->size = HEAP_SIZE - sizeof(block_header_t);
    heap_start->is_free = 1;
    heap_start->next = NULL;
    
    free_memory = heap_start->size;
}

static block_header_t* find_free_block(size_t size) {
    block_header_t* current = heap_start;
    
    while (current) {
        if (current->magic != BLOCK_MAGIC) {
            return NULL; // Heap corruption
        }
        
        if (current->is_free && current->size >= size) {
            return current;
        }
        
        current = current->next;
    }
    
    return NULL;
}

static void split_block(block_header_t* block, size_t size) {
    if (block->size > size + sizeof(block_header_t) + 16) {
        block_header_t* new_block = (block_header_t*)((uint8_t*)block + sizeof(block_header_t) + size);
        new_block->magic = BLOCK_MAGIC;
        new_block->size = block->size - size - sizeof(block_header_t);
        new_block->is_free = 1;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

static void merge_free_blocks(void) {
    block_header_t* current = heap_start;
    
    while (current && current->next) {
        if (current->magic != BLOCK_MAGIC || current->next->magic != BLOCK_MAGIC) {
            return; // Heap corruption
        }
        
        if (current->is_free && current->next->is_free) {
            current->size += sizeof(block_header_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void* malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    // Align size to 8 bytes
    size = (size + 7) & ~7;
    
    block_header_t* block = find_free_block(size);
    if (!block) {
        return NULL; // Out of memory
    }
    
    split_block(block, size);
    block->is_free = 0;
    free_memory -= block->size;
    
    return (void*)((uint8_t*)block + sizeof(block_header_t));
}

void free(void* ptr) {
    if (!ptr) {
        return;
    }
    
    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    
    if (block->magic != BLOCK_MAGIC) {
        return; // Invalid pointer
    }
    
    block->is_free = 1;
    free_memory += block->size;
    
    merge_free_blocks();
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    
    if (block->magic != BLOCK_MAGIC) {
        return NULL; // Invalid pointer
    }
    
    if (block->size >= size) {
        return ptr; // Current block is large enough
    }
    
    void* new_ptr = malloc(size);
    if (!new_ptr) {
        return NULL;
    }
    
    memcpy(new_ptr, ptr, block->size);
    free(ptr);
    
    return new_ptr;
}

void* calloc(size_t nmemb, size_t size) {
    size_t total_size = nmemb * size;
    void* ptr = malloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void memory_get_stats(size_t* total, size_t* used, size_t* free) {
    if (total) *total = total_memory;
    if (used) *used = total_memory - free_memory;
    if (free) *free = free_memory;
}

bool memory_check_integrity(void) {
    block_header_t* current = heap_start;
    
    while (current) {
        if (current->magic != BLOCK_MAGIC) {
            return false;
        }
        current = current->next;
    }
    
    return true;
}

#endif
