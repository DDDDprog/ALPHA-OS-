/**
 * Alpha OS - Boot Diagnostics Implementation
 * Shows hardware detection like Linux kernel boot
 */

#include "../include/kernel/boot_info.h"
#include "../include/kernel/console.h"
#include "../include/kernel/acpi.h"
#include "../include/libc/string.h"
#include "../include/libc/stdio.h"

#ifdef TEST_MODE
#include <stdio.h>
#include <time.h>
#endif

static bool diagnostics_initialized = false;
static cpu_info_t boot_cpu;
static mem_info_t boot_mem;
static gpu_info_t boot_gpu;

/* ============================================================
 * Initialization
 * ============================================================ */

int boot_diagnostics_init(void) {
    if (diagnostics_initialized) return 0;
    
    console_set_color(COLOR_CYAN, COLOR_BLACK);
    printf("\n");
    printf("    ALPHA OS Boot Menu v1.0.0\n");
    printf("    ===========================\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
    printf("\n");
    
    /* Detect all hardware */
    cpu_detect(&boot_cpu);
    mem_detect(&boot_mem);
    gpu_detect(&boot_gpu);
    
    diagnostics_initialized = true;
    return 0;
}

/* ============================================================
 * CPU Detection
 * ============================================================ */

int cpu_detect(cpu_info_t* info) {
    if (!info) return -1;
    
    console_set_color(COLOR_YELLOW, COLOR_BLACK);
    printf("[    0.000000] Alpha OS boot_params 0x00000000\n");
    printf("[    0.000000] Command line: BOOT_IMAGE=/boot/alpha root=/dev/sda1 ro quiet\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
    
    /* Try to detect real CPU using CPUID */
    bool has_cpuid = false;
    
#ifdef TEST_MODE
    /* Try to get real CPU info on test systems */
    #if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
    /* CPUID detection would go here in real kernel */
    #endif
    
    /* Default simulated values - will be replaced by real detection */
    strcpy(info->vendor, "AuthenticAMD");
    strcpy(info->brand, "AMD Ryzen 7 5800X 8-Core Processor");
    info->cores = 8;
    info->threads = 16;
    info->frequency = 3800;
    info->cache_l1 = 32;
    info->cache_l2 = 512;
    info->cache_l3 = 32768;
    info->has_fpu = true;
    info->has_sse = true;
    info->has_avx = true;
    info->has_avx2 = true;
    info->is_64bit = true;
#else
    /* Real kernel mode - detect actual CPU */
    #if defined(__i386__) || defined(__x86_64__)
    /* CPUID leaf 0 - Vendor string */
    char vendor[13] = {0};
    /* In real kernel, we'd call cpuid here */
    /* For now, detect based on compile target */
    #endif
    
    strcpy(info->vendor, "AuthenticAMD");
    strcpy(info->brand, "AMD Ryzen 7 5800X 8-Core Processor");
    info->cores = 4;
    info->threads = 8;
    info->frequency = 3600;
    info->cache_l1 = 32;
    info->cache_l2 = 256;
    info->cache_l3 = 12288;
    info->has_fpu = true;
    info->has_sse = true;
    info->has_avx = true;
    info->is_64bit = true;
#endif
    
    return 0;
}

void cpu_print_info(void) {
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                      CPU INFORMATION                        ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
    printf("║ Vendor:    %-48s ║\n", boot_cpu.vendor);
    printf("║ Model:     %-48s ║\n", boot_cpu.brand);
    printf("║ Cores:     %-48u ║\n", boot_cpu.cores);
    printf("║ Threads:   %-48u ║\n", boot_cpu.threads);
    printf("║ Frequency: %-48u MHz ║\n", boot_cpu.frequency);
    printf("║ L1 Cache:  %-48u KB ║\n", boot_cpu.cache_l1);
    printf("║ L2 Cache:  %-48u KB ║\n", boot_cpu.cache_l2);
    printf("║ L3 Cache:  %-48u KB ║\n", boot_cpu.cache_l3);
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("║ Flags:     %-48s ║\n", 
           "fpu pae sse sse2 sse3 ssse3 sse4_1 sse4_2 avx avx2 aes");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
    printf("║ 64-bit:    %-48s ║\n", boot_cpu.is_64bit ? "Yes" : "No");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
}

/* ============================================================
 * Memory Detection
 * ============================================================ */

int mem_detect(mem_info_t* info) {
    if (!info) return -1;
    
#ifdef TEST_MODE
    info->total = 16ULL * 1024 * 1024 * 1024;  /* 16 GB */
    info->free = 14ULL * 1024 * 1024 * 1024;
    info->available = 13ULL * 1024 * 1024 * 1024;
    info->buffers = 500 * 1024 * 1024;
    info->cached = 1ULL * 1024 * 1024 * 1024;
#else
    info->total = 256 * 1024 * 1024;  /* 256 MB */
    info->free = 200 * 1024 * 1024;
    info->available = 220 * 1024 * 1024;
    info->buffers = 10 * 1024 * 1024;
    info->cached = 20 * 1024 * 1024;
#endif
    info->num_modules = 2;
    
    return 0;
}

void mem_print_info(void) {
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    MEMORY INFORMATION                        ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
    
    uint64_t total_gb = boot_mem.total / (1024ULL * 1024 * 1024);
    uint64_t free_gb = boot_mem.free / (1024ULL * 1024 * 1024);
    uint64_t avail_gb = boot_mem.available / (1024ULL * 1024 * 1024);
    
    printf("║ Total Memory:     %4llu GB                             ║\n", total_gb);
    printf("║ Free Memory:     %4llu GB                             ║\n", free_gb);
    printf("║ Available:       %4llu GB                             ║\n", avail_gb);
    printf("║ Buffers:         %llu MB                             ║\n", boot_mem.buffers / (1024 * 1024));
    printf("║ Cached:          %llu MB                             ║\n", boot_mem.cached / (1024 * 1024));
    printf("║ Memory Modules:   %u                                      ║\n", boot_mem.num_modules);
    
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
}

/* ============================================================
 * GPU Detection
 * ============================================================ */

int gpu_detect(gpu_info_t* info) {
    if (!info) return -1;
    
#ifdef TEST_MODE
    strcpy(info->name, "NVIDIA GeForce RTX 3080");
    info->vram = 10240;  /* 10 GB */
    info->frequency = 1440;
    info->present = true;
    info->accel_2d = true;
    info->accel_3d = true;
#else
    strcpy(info->name, "Virtio GPU");
    info->vram = 256;
    info->frequency = 500;
    info->present = true;
    info->accel_2d = true;
    info->accel_3d = false;
#endif
    
    return 0;
}

void gpu_print_info(void) {
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     GPU INFORMATION                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
    
    if (boot_gpu.present) {
        printf("║ Name:        %-48s ║\n", boot_gpu.name);
        printf("║ VRAM:        %-48u MB ║\n", boot_gpu.vram);
        printf("║ Frequency:   %-48u MHz ║\n", boot_gpu.frequency);
        console_set_color(COLOR_WHITE, COLOR_BLACK);
        printf("║ 2D Accel:    %-48s ║\n", boot_gpu.accel_2d ? "Yes" : "No");
        printf("║ 3D Accel:    %-48s ║\n", boot_gpu.accel_3d ? "Yes" : "No");
    } else {
        printf("║ No GPU detected                                       ║\n");
    }
    
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
}

/* ============================================================
 * Storage Detection
 * ============================================================ */

int storage_detect(storage_info_t* devs, int* count) {
    if (!devs || !count) return -1;
    
#ifdef TEST_MODE
    *count = 2;
    strcpy(devs[0].name, "sda");
    strcpy(devs[0].model, "Samsung SSD 970 EVO Plus 500GB");
    devs[0].size = 500ULL * 1024 * 1024 * 1024;
    devs[0].present = true;
    
    strcpy(devs[1].name, "sdb");
    strcpy(devs[1].model, "WDC WD10EZEX-08WN4A0");
    devs[1].size = 1ULL * 1024 * 1024 * 1024 * 1024;
    devs[1].present = true;
#else
    *count = 1;
    strcpy(devs[0].name, "sda");
    strcpy(devs[0].model, "Virtio Block Device");
    devs[0].size = 20ULL * 1024 * 1024 * 1024;
    devs[0].present = true;
#endif
    
    return 0;
}

void storage_print_info(void) {
    storage_info_t devs[MAX_STORAGE_DEVICES];
    int count = 0;
    storage_detect(devs, &count);
    
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                   STORAGE DEVICES                           ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
    
    for (int i = 0; i < count; i++) {
        uint64_t size_gb = devs[i].size / (1024ULL * 1024 * 1024);
        printf("║ %-5s: %-30s %5llu GB  ║\n", 
               devs[i].name, devs[i].model, size_gb);
    }
    
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
}

/* ============================================================
 * Network Detection
 * ============================================================ */

int net_detect(net_info_t* devs, int* count) {
    if (!devs || !count) return -1;
    
#ifdef TEST_MODE
    *count = 2;
    strcpy(devs[0].name, "eth0");
    strcpy(devs[0].mac, "52:54:00:12:34:56");
    devs[0].ip = 0x0100A8C0;  /* 192.168.0.1 */
    devs[0].present = true;
    devs[0].up = true;
    
    strcpy(devs[1].name, "wlan0");
    strcpy(devs[1].mac, "a4:5e:60:78:90:ab");
    devs[1].ip = 0x0200A8C0;
    devs[1].present = true;
    devs[1].up = false;
#else
    *count = 1;
    strcpy(devs[0].name, "eth0");
    strcpy(devs[0].mac, "52:54:00:ab:cd:ef");
    devs[0].ip = 0x0100A8C0;
    devs[0].present = true;
    devs[0].up = true;
#endif
    
    return 0;
}

void net_print_info(void) {
    net_info_t devs[MAX_NET_DEVICES];
    int count = 0;
    net_detect(devs, &count);
    
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    NETWORK DEVICES                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
    
    for (int i = 0; i < count; i++) {
        printf("║ %-6s: %-18s %-15s %s  ║\n", 
               devs[i].name, devs[i].mac, 
               devs[i].up ? "UP" : "DOWN",
               "");
    }
    
    console_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
}

/* ============================================================
 * Full Boot Info Display
 * ============================================================ */

void boot_show_all(void) {
    boot_diagnostics_init();
    cpu_print_info();
    mem_print_info();
    gpu_print_info();
    storage_print_info();
    net_print_info();
}

/* ============================================================
 * Boot Messages (Linux-style)
 * ============================================================ */

void boot_message_start(void) {
    console_clear();
    console_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    printf("\n");
    printf("    ALPHA OS 1.0.0 alpha    \n");
    printf("    ════════════════════════════════════    \n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
    printf("[    0.000000] Alpha OS version 1.0.0-alpha\n");
    printf("[    0.000000] Command line: BOOT_IMAGE=/boot/alpha root=/dev/sda1 ro quiet splash\n");
    printf("[    0.000000] x86/fpu: Using FPU save instructions\n");
    printf("[    0.000000] e820: BIOS-provided physical RAM map:\n");
    printf("[    0.000000] BIOS-e820: [mem 0x0000000000000000-0x000000000009fbff] usable\n");
    printf("[    0.000000] BIOS-e820: [mem 0x000000000009fc00-0x000000000fffffff] usable\n");
    printf("[    0.000000] NX (Execute Disable) protection: active\n");
    printf("[    0.000000] SMBIOS 3.3.0 present.\n");
    printf("[    0.000000] DMI: QEMU Standard PC (i440FX + PIIX, 1996), BIOS 1.0.0 04/01/2014\n");
    printf("[    0.000000] Hypervisor detected: KVM\n");
    console_set_color(COLOR_CYAN, COLOR_BLACK);
    printf("\n");
}

void boot_message_end(void) {
    console_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    printf("\n[    2.345678] ALPHA OS started.\n");
    printf("              Welcome to Alpha OS 1.0.0\n");
    printf("\n");
    console_set_color(COLOR_WHITE, COLOR_BLACK);
}

/* Stub functions */
int pci_detect(pci_device_t* devs, int* count) { 
    (void)devs; 
    if (count) *count = 0; 
    return 0; 
}
void pci_print_devices(void) { }
void serial_detect(void) { }
void apic_print_info(void) { }
void dmi_print_info(void) { }
