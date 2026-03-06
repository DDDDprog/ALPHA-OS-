/**
 * Alpha OS - Device Management Implementation
 */

#include "../include/kernel/device.h"
#include "../include/kernel/console.h"
#include "../include/libc/string.h"
#include "../include/libc/stdio.h"

#ifdef TEST_MODE
#include <stdlib.h>
#include <string.h>
#else
#endif

/* Device and driver lists */
static device_t* device_list = NULL;
static driver_t* driver_list = NULL;
static uint32_t next_device_id = 1;

/* Device file descriptor table */
#define MAX_OPEN_DEVICES 64
static device_t* fd_table[MAX_OPEN_DEVICES] = {NULL};

/* ============================================================
 * Initialization
 * ============================================================ */

#ifdef TEST_MODE

int device_init(void) {
    printf("[DEV] Device manager initialized\n");
    
    /* Register default devices */
    tty_register();
    rtc_register();
    
    return 0;
}

int device_shutdown(void) {
    printf("[DEV] Device manager shutting down\n");
    /* Cleanup devices */
    return 0;
}

#else

int device_init(void) {
    return 0;
}

int device_shutdown(void) {
    return 0;
}

#endif

/* ============================================================
 * Device Registration
 * ============================================================ */

int device_register(device_t* dev) {
    if (!dev) return -1;
    
    dev->id = next_device_id++;
    dev->initialized = false;
    dev->opened = false;
    dev->ref_count = 0;
    
    /* Add to device list */
    dev->next = device_list;
    if (device_list) {
        device_list->prev = dev;
    }
    device_list = dev;
    
    /* Initialize device if driver provides init */
    if (dev->init) {
        int ret = dev->init(dev);
        if (ret == 0) {
            dev->initialized = true;
        }
        return ret;
    }
    
    return 0;
}

int device_unregister(device_t* dev) {
    if (!dev) return -1;
    
    /* Remove from list */
    if (dev->prev) {
        dev->prev->next = dev->next;
    } else {
        device_list = dev->next;
    }
    
    if (dev->next) {
        dev->next->prev = dev->prev;
    }
    
    free(dev);
    return 0;
}

device_t* device_get_by_name(const char* name) {
    device_t* dev = device_list;
    while (dev) {
        if (strcmp(dev->name, name) == 0) {
            return dev;
        }
        dev = dev->next;
    }
    return NULL;
}

device_t* device_get_by_id(uint32_t id) {
    device_t* dev = device_list;
    while (dev) {
        if (dev->id == id) {
            return dev;
        }
        dev = dev->next;
    }
    return NULL;
}

device_t* device_get_by_path(const char* path) {
    device_t* dev = device_list;
    while (dev) {
        if (strcmp(dev->path, path) == 0) {
            return dev;
        }
        dev = dev->next;
    }
    return NULL;
}

/* ============================================================
 * Device Operations
 * ============================================================ */

int device_open(const char* path, int flags) {
    device_t* dev = device_get_by_path(path);
    if (!dev) return -1;
    
    /* Find free file descriptor */
    int fd = -1;
    for (int i = 0; i < MAX_OPEN_DEVICES; i++) {
        if (fd_table[i] == NULL) {
            fd = i;
            break;
        }
    }
    
    if (fd < 0) return -1;  /* No free descriptors */
    
    /* Call device open */
    if (dev->open) {
        int ret = dev->open(dev, flags);
        if (ret != 0) return ret;
    }
    
    dev->opened = true;
    dev->ref_count++;
    fd_table[fd] = dev;
    
    return fd;
}

int device_close(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_DEVICES) return -1;
    
    device_t* dev = fd_table[fd];
    if (!dev) return -1;
    
    /* Call device close */
    if (dev->close) {
        dev->close(dev);
    }
    
    dev->ref_count--;
    if (dev->ref_count == 0) {
        dev->opened = false;
    }
    
    fd_table[fd] = NULL;
    return 0;
}

int device_read(int fd, void* buf, size_t count) {
    if (fd < 0 || fd >= MAX_OPEN_DEVICES) return -1;
    
    device_t* dev = fd_table[fd];
    if (!dev || !dev->read) return -1;
    
    return dev->read(dev, buf, count);
}

int device_write(int fd, const void* buf, size_t count) {
    if (fd < 0 || fd >= MAX_OPEN_DEVICES) return -1;
    
    device_t* dev = fd_table[fd];
    if (!dev || !dev->write) return -1;
    
    return dev->write(dev, buf, count);
}

int device_ioctl(int fd, uint32_t cmd, void* arg) {
    if (fd < 0 || fd >= MAX_OPEN_DEVICES) return -1;
    
    device_t* dev = fd_table[fd];
    if (!dev || !dev->ioctl) return -1;
    
    return dev->ioctl(dev, cmd, arg);
}

/* ============================================================
 * Device Management
 * ============================================================ */

int device_enable(device_t* dev) {
    if (!dev) return -1;
    dev->initialized = true;
    return 0;
}

int device_disable(device_t* dev) {
    if (!dev) return -1;
    dev->initialized = false;
    return 0;
}

int device_reset(device_t* dev) {
    if (!dev || !dev->init) return -1;
    dev->init(dev);
    return 0;
}

/* ============================================================
 * Driver Management
 * ============================================================ */

int driver_register(driver_t* drv) {
    if (!drv) return -1;
    
    /* Add to driver list */
    drv->next = driver_list;
    driver_list = drv;
    
    /* Initialize driver */
    if (drv->init) {
        return drv->init();
    }
    
    return 0;
}

int driver_unregister(driver_t* drv) {
    if (!drv) return -1;
    
    /* Remove from list */
    driver_t** ptr = &driver_list;
    while (*ptr) {
        if (*ptr == drv) {
            *ptr = drv->next;
            break;
        }
        ptr = &(*ptr)->next;
    }
    
    free(drv);
    return 0;
}

driver_t* driver_get_by_name(const char* name) {
    driver_t* drv = driver_list;
    while (drv) {
        if (strcmp(drv->name, name) == 0) {
            return drv;
        }
        drv = drv->next;
    }
    return NULL;
}

driver_t* driver_get_by_class(uint32_t class) {
    driver_t* drv = driver_list;
    while (drv) {
        if (drv->class == class) {
            return drv;
        }
        drv = drv->next;
    }
    return NULL;
}

/* ============================================================
 * I/O Operations (inline for kernel)
 * ============================================================ */

#ifndef TEST_MODE

uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

uint16_t inw(uint16_t port) {
    uint16_t val;
    __asm__ volatile ("inw %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

uint32_t inl(uint16_t port) {
    uint32_t val;
    __asm__ volatile ("inl %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

#else

/* Test mode stubs */
uint8_t inb(uint16_t port) { (void)port; return 0; }
uint16_t inw(uint16_t port) { (void)port; return 0; }
uint32_t inl(uint16_t port) { (void)port; return 0; }
void outb(uint16_t port, uint8_t val) { (void)port; (void)val; }
void outw(uint16_t port, uint16_t val) { (void)port; (void)val; }
void outl(uint16_t port, uint32_t val) { (void)port; (void)val; }

#endif

/* MMIO stubs */
uint8_t mmio_read8(uint32_t addr) {
    return *(volatile uint8_t*)addr;
}

uint16_t mmio_read16(uint32_t addr) {
    return *(volatile uint16_t*)addr;
}

uint32_t mmio_read32(uint32_t addr) {
    return *(volatile uint32_t*)addr;
}

void mmio_write8(uint32_t addr, uint8_t val) {
    *(volatile uint8_t*)addr = val;
}

void mmio_write16(uint32_t addr, uint16_t val) {
    *(volatile uint16_t*)addr = val;
}

void mmio_write32(uint32_t addr, uint32_t val) {
    *(volatile uint32_t*)addr = val;
}

/* ============================================================
 * Default Device Drivers
 * ============================================================ */

int tty_init(void) {
    return 0;
}

int tty_register(void) {
    device_t* tty = malloc(sizeof(device_t));
    if (!tty) return -1;
    
    memset(tty, 0, sizeof(device_t));
    strcpy(tty->name, "tty");
    strcpy(tty->path, "/dev/tty");
    tty->type = DEVICE_TYPE_CHAR;
    tty->class = DEVICE_CLASS_SERIAL;
    tty->flags = DEVICE_FLAG_READ | DEVICE_FLAG_WRITE;
    
    device_register(tty);
    return 0;
}

int rtc_init(void) {
    return 0;
}

int rtc_register(void) {
    device_t* rtc = malloc(sizeof(device_t));
    if (!rtc) return -1;
    
    memset(rtc, 0, sizeof(device_t));
    strcpy(rtc->name, "rtc");
    strcpy(rtc->path, "/dev/rtc");
    rtc->type = DEVICE_TYPE_CHAR;
    rtc->class = DEVICE_CLASS_RTC;
    rtc->flags = DEVICE_FLAG_READ;
    
    device_register(rtc);
    return 0;
}

int disk_init(void) { return 0; }
int disk_register(void) { return 0; }
int network_driver_init(void) { return 0; }
int pci_init(void) { return 0; }
device_t* pci_get_device(uint16_t vendor, uint16_t device) { (void)vendor; (void)device; return NULL; }
