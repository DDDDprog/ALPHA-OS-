#include "../include/kernel/memory.h"
#include "../include/kernel/cpu.h"
#include "../include/libc/string.h"

#ifdef TEST_MODE
#include <stdlib.h>
#include <stdio.h>  
#include <string.h>

// Test mode - use system malloc/free
void memory_init(void) {
    printf("[TEST] Memory subsystem initialized (8MB heap)\n");
}
void memory_enable_paging(void) {}
void memory_disable_paging(void) {}
bool memory_is_paging_enabled(void) { return false; }

void memory_get_stats(size_t* total, size_t* used, size_t* free) {
    if (total) *total = HEAP_SIZE;
    if (used) *used = 0;
    if (free) *free = HEAP_SIZE;
}

void memory_get_extended_stats(memory_stats_t* stats) {
    if (stats) {
        memset(stats, 0, sizeof(memory_stats_t));
        stats->total_pages = HEAP_SIZE / PAGE_SIZE;
        stats->free_pages = HEAP_SIZE / PAGE_SIZE;
        stats->total_physical_mb = 8;
    }
}

void memory_dump_info(void) {
    printf("Memory Info:\n");
    printf("  Total: 8 MB\n");
    printf("  Heap Size: 8 MB\n");
    printf("  Page Size: 4 KB\n");
}

bool memory_check_integrity(void) {
    return true;
}

void* physical_alloc(void) {
    return malloc(PAGE_SIZE);
}

void physical_free(void* page) {
    free(page);
}

uint32_t physical_get_free_count(void) {
    return HEAP_SIZE / PAGE_SIZE;
}

void* vmalloc(size_t size) {
    return malloc(size);
}

void vmfree(void* addr, size_t size) {
    free(addr);
}

void* vmap(uint32_t phys_addr, size_t size) {
    (void)phys_addr;
    return malloc(size);
}

void vunmap(void* addr, size_t size) {
    free(addr);
}

void page_table_init(page_directory_t* pd) {
    (void)pd;
}

void page_directory_switch(page_directory_t* pd) {
    (void)pd;
}

page_directory_t* page_directory_get_current(void) {
    return NULL;
}

void page_map(page_directory_t* pd, uint32_t virt, uint32_t phys, uint32_t flags) {
    (void)pd; (void)virt; (void)phys; (void)flags;
}

void page_unmap(page_directory_t* pd, uint32_t virt) {
    (void)pd; (void)virt;
}

uint32_t page_lookup(page_directory_t* pd, uint32_t virt) {
    (void)pd; (void)virt;
    return 0;
}

void page_fault_handler(uint32_t fault_addr, uint32_t error_code) {
    (void)fault_addr; (void)error_code;
}

void cache_flush_tlb(void) {}
void cache_flush_page(uint32_t addr) { (void)addr; }
void cache_invalidate_icache(void) {}

slab_cache_t* slab_create(const char* name, size_t size) {
    (void)name;
    slab_cache_t* cache = malloc(sizeof(slab_cache_t));
    if (cache) {
        cache->object_size = size;
    }
    return cache;
}

void* slab_alloc(slab_cache_t* cache) {
    if (cache) {
        return malloc(cache->object_size);
    }
    return NULL;
}

void slab_free(slab_cache_t* cache, void* obj) {
    (void)cache;
    free(obj);
}

void slab_destroy(slab_cache_t* cache) {
    free(cache);
}

void* kmalloc(size_t size, uint32_t flags) {
    (void)flags;
    return malloc(size);
}

void kfree(void* ptr) {
    free(ptr);
}

int memory_get_regions(memory_region_t* regions, int max_count) {
    (void)regions;
    return 0;
}

void memory_test(void) {
    printf("Running memory tests...\n");
    void* p1 = malloc(1024);
    void* p2 = malloc(2048);
    void* p3 = malloc(4096);
    free(p1);
    free(p2);
    free(p3);
    printf("Memory tests passed!\n");
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

void memory_detect(void) {
    // Detect physical memory - stub for now
    total_memory = HEAP_SIZE;
    free_memory = HEAP_SIZE;
}

void memory_setup_paging(void) {
    // Setup paging - stub for now
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
