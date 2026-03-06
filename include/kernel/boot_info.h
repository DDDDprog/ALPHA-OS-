/**
 * Alpha OS - Boot Diagnostics & System Info
 * Shows hardware detection like Linux kernel boot
 */

#ifndef BOOT_INFO_H
#define BOOT_INFO_H

#include "../kernel/types.h"

/* ============================================================
 * CPU Information
 * ============================================================ */

#define CPU_VENDOR_LEN  16
#define CPU_BRAND_LEN   64
#define CPU_MAX_CORES   32

typedef struct {
    char vendor[CPU_VENDOR_LEN];
    char brand[CPU_BRAND_LEN];
    uint32_t family;
    uint32_t model;
    uint32_t stepping;
    uint32_t cores;
    uint32_t threads;
    uint32_t frequency;  /* MHz */
    uint32_t cache_l1;   /* KB */
    uint32_t cache_l2;   /* KB */
    uint32_t cache_l3;   /* KB */
    uint64_t flags;
    bool has_fpu;
    bool has_pae;
    bool has_sse;
    bool has_avx;
    bool is_64bit;
} cpu_info_t;

/* ============================================================
 * Memory Information  
 * ============================================================ */

typedef struct {
    uint64_t total;       /* bytes */
    uint64_t free;
    uint64_t available;
    uint64_t buffers;
    uint64_t cached;
    uint32_t num_modules;
} mem_info_t;

/* ============================================================
 * GPU Information
 * ============================================================ */

#define GPU_NAME_LEN   32

typedef struct {
    char name[GPU_NAME_LEN];
    uint32_t vram;        /* MB */
    uint32_t frequency;
    bool present;
    bool accel_2d;
    bool accel_3d;
} gpu_info_t;

/* ============================================================
 * Storage Devices
 * ============================================================ */

#define MAX_STORAGE_DEVICES  8

typedef struct {
    char name[16];
    char model[32];
    uint64_t size;        /* bytes */
    bool present;
} storage_info_t;

/* ============================================================
 * Network Devices
 * ============================================================ */

#define MAX_NET_DEVICES  4

typedef struct {
    char name[16];
    char mac[18];
    uint32_t ip;
    bool present;
    bool up;
} net_info_t;

/* ============================================================
 * PCI Devices
 * ============================================================ */

#define MAX_PCI_DEVICES  32

typedef struct {
    uint16_t vendor;
    uint16_t device;
    uint8_t class_code;
    uint8_t subclass;
    char vendor_name[16];
    char device_name[24];
} pci_device_t;

/* ============================================================
 * Boot Parameters
 * ============================================================ */

#define BOOT_CMDLINE_LEN  256

typedef struct {
    uint32_t boot_flags;
    uint32_t boot_reason;
    uint32_t uptime_seconds;
    char cmdline[BOOT_CMDLINE_LEN];
} boot_info_t;

/* ============================================================
 * API
 * ============================================================ */

/* Initialize boot diagnostics */
int boot_diagnostics_init(void);

/* CPU */
int cpu_detect(cpu_info_t* info);
void cpu_print_info(void);

/* Memory */
int mem_detect(mem_info_t* info);
void mem_print_info(void);

/* GPU */
int gpu_detect(gpu_info_t* info);
void gpu_print_info(void);

/* Storage */
int storage_detect(storage_info_t* devs, int* count);
void storage_print_info(void);

/* Network */
int net_detect(net_info_t* devs, int* count);
void net_print_info(void);

/* PCI */
int pci_detect(pci_device_t* devs, int* count);
void pci_print_devices(void);

/* Full boot info display */
void boot_show_all(void);

/* Boot messages like Linux */
void boot_message_start(void);
void boot_message_end(void);

/* Serial/COM ports */
void serial_detect(void);

/* APIC info */
void apic_print_info(void);

/* DMI/SMBIOS info */
void dmi_print_info(void);

#endif /* BOOT_INFO_H */
