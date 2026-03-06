#ifndef MEMORY_H
#define MEMORY_H

// Use standard integer types
#ifdef TEST_MODE
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#else
#include "types.h"
#include "cpu.h"
#endif

// Memory Constants
#define PAGE_SIZE         4096
#define PAGE_SHIFT        12
#define PAGE_MASK         0xFFFFF000
#define PAGE_OFFSET_MASK  0x00000FFF

#define MEMORY_BITMAP_SIZE  (32 * 1024)  // Support up to 128MB
#define HEAP_SIZE (8 * 1024 * 1024) // 8 MB heap (upgraded from 1MB)
#define BLOCK_MAGIC 0x1234ABCD

// Memory Regions
#define KERNEL_VMA         0xC0000000   // Kernel virtual memory address
#define KERNEL_PHYS_BASE   0x00100000    // Kernel physical base
#define KERNEL_HEAP_SIZE   (8 * 1024 * 1024)  // 8MB kernel heap

// Page Table Entries
#define PTE_PRESENT    0x01
#define PTE_WRITABLE   0x02
#define PTE_USER       0x04
#define PTE_WRITE_THROUGH  0x08
#define PTE_CACHE_DISABLE  0x10
#define PTE_ACCESSED    0x20
#define PTE_DIRTY       0x40
#define PTE_HUGE        0x80
#define PTE_GLOBAL      0x100

// Memory Region Types
typedef enum {
    MEMORY_REGION_USABLE = 0,
    MEMORY_REGION_RESERVED,
    MEMORY_REGION_ACPI,
    MEMORY_REGION_NVS,
    MEMORY_REGION_BADMEM,
    MEMORY_REGION_KERNEL
} memory_region_type_t;

// Memory Region Structure
typedef struct {
    uint64_t base;
    uint64_t size;
    memory_region_type_t type;
} memory_region_t;

// Physical Memory Block
typedef struct phys_block {
    uint32_t page_num;
    uint32_t order;
    struct phys_block* next;
} phys_block_t;

// Page Directory/Table Entry
typedef uint32_t page_entry_t;

// Page Table Structure
typedef struct {
    page_entry_t entries[1024];
} page_table_t;

typedef page_table_t page_directory_t;

// Memory Statistics
typedef struct {
    uint64_t total_pages;
    uint64_t free_pages;
    uint64_t used_pages;
    uint64_t reserved_pages;
    uint64_t kernel_pages;
    uint64_t user_pages;
    uint32_t total_physical_mb;
    uint32_t largest_free_block;
    uint32_t page_fault_count;
    uint32_t allocation_count;
    uint32_t free_count;
} memory_stats_t;

// Virtual Memory Areas
typedef struct vma {
    uint32_t start;
    uint32_t end;
    uint32_t flags;
    const char* name;
    struct vma* next;
} vma_t;

// Memory block header
typedef struct block_header {
    uint32_t magic;
    size_t size;
    uint8_t is_free;
    struct block_header* next;
} block_header_t;

// Memory Management Flags
#define VMA_READ     0x01
#define VMA_WRITE    0x02
#define VMA_EXECUTE  0x04
#define VMA_SHARED   0x08
#define VMA_PRIVATE  0x10

// Buddy System Constants
#define MAX_ORDER 11  // Supports up to 4MB blocks

// Initialize memory management
void memory_init(void);

// Detect physical memory
void memory_detect(void);

// Setup paging
void memory_setup_paging(void);

// Physical Memory Management
void* physical_alloc(void);
void physical_free(void* page);
uint32_t physical_get_free_count(void);

// Virtual Memory Management
void* vmalloc(size_t size);
void vmfree(void* addr, size_t size);
void* vmap(uint32_t phys_addr, size_t size);
void vunmap(void* addr, size_t size);

// Page Table Operations
void page_table_init(page_directory_t* pd);
void page_directory_switch(page_directory_t* pd);
page_directory_t* page_directory_get_current(void);
void page_map(page_directory_t* pd, uint32_t virt, uint32_t phys, uint32_t flags);
void page_unmap(page_directory_t* pd, uint32_t virt);
uint32_t page_lookup(page_directory_t* pd, uint32_t virt);

// Allocate memory
void* malloc(size_t size);

// Free allocated memory
void free(void* ptr);

// Reallocate memory
void* realloc(void* ptr, size_t size);

// Allocate and zero memory
void* calloc(size_t nmemb, size_t size);

// Kernel memory allocation with flags
void* kmalloc(size_t size, uint32_t flags);
void kfree(void* ptr);

// Get memory statistics
void memory_get_stats(size_t* total, size_t* used, size_t* free);

// Check heap integrity
bool memory_check_integrity(void);

// Extended memory statistics
void memory_get_extended_stats(memory_stats_t* stats);
void memory_dump_info(void);

// Memory Region Management
int memory_get_regions(memory_region_t* regions, int max_count);
void memory_enable_paging(void);
void memory_disable_paging(void);
bool memory_is_paging_enabled(void);

// Page Fault Handling
void page_fault_handler(uint32_t fault_addr, uint32_t error_code);

// Cache Operations
void cache_flush_tlb(void);
void cache_flush_page(uint32_t addr);
void cache_invalidate_icache(void);

// SLAB Allocator
typedef struct slab_cache {
    const char* name;
    size_t object_size;
    uint32_t page_count;
    void* pages;
    struct slab_cache* next;
} slab_cache_t;

slab_cache_t* slab_create(const char* name, size_t size);
void* slab_alloc(slab_cache_t* cache);
void slab_free(slab_cache_t* cache, void* obj);
void slab_destroy(slab_cache_t* cache);

// Test mode functions
void memory_test(void);

#endif // MEMORY_H
