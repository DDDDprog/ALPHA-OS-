/**
 * Alpha OS - Device Management
 * 
 * Device driver interface and management
 */

#ifndef DEVICE_H
#define DEVICE_H

#include "../kernel/types.h"

/* ============================================================
 * Device Types
 * ============================================================ */

#define DEVICE_TYPE_CHAR     1
#define DEVICE_TYPE_BLOCK   2
#define DEVICE_TYPE_NETWORK 3
#define DEVICE_TYPE_VIDEO   4
#define DEVICE_TYPE_AUDIO   5
#define DEVICE_TYPE_INPUT   6

/* Device classes */
#define DEVICE_CLASS_MEMORY      1
#define DEVICE_CLASS_STORAGE     2
#define DEVICE_CLASS_NETWORK     3
#define DEVICE_CLASS_DISPLAY     4
#define DEVICE_CLASS_INPUT       5
#define DEVICE_CLASS_SERIAL      6
#define DEVICE_CLASS_RTC        7

/* Device flags */
#define DEVICE_FLAG_READ    0x01
#define DEVICE_FLAG_WRITE   0x02
#define DEVICE_FLAG_IRQ     0x04
#define DEVICE_FLAG_DMA     0x08
#define DEVICE_FLAG_BLOCKING 0x10

/* ============================================================
 * Device Structure
 * ============================================================ */

typedef struct device device_t;

struct device {
    /* Identification */
    char          name[32];
    char          path[64];
    uint32_t      type;
    uint32_t      class;
    uint32_t      flags;
    uint32_t      id;
    
    /* Hardware info */
    uint32_t      base_addr;    /* I/O base address */
    uint32_t      irq;          /* IRQ number */
    uint32_t      dma;          /* DMA channel */
    uint32_t      memory_size;  /* For memory-mapped devices */
    
    /* Driver callbacks */
    int  (*init)(device_t* dev);
    int  (*open)(device_t* dev, int flags);
    int  (*close)(device_t* dev);
    int  (*read)(device_t* dev, void* buf, size_t count);
    int  (*write)(device_t* dev, const void* buf, size_t count);
    int  (*ioctl)(device_t* dev, uint32_t cmd, void* arg);
    int  (*poll)(device_t* dev, int events);
    
    /* Device-specific data */
    void*         driver_data;
    
    /* State */
    bool          initialized;
    bool          opened;
    uint32_t      ref_count;
    
    /* Links */
    device_t*     next;
    device_t*     prev;
};

/* ============================================================
 * Driver Structure
 * ============================================================ */

typedef struct driver {
    char     name[32];
    uint32_t class;
    uint32_t type;
    
    /* Driver callbacks */
    int (*init)(void);
    int (*probe)(device_t* dev);
    int (*remove)(device_t* dev);
    
    /* Driver data */
    void*   data;
    
    struct driver* next;
} driver_t;

/* ============================================================
 * Major/Minor Numbers (for character/block devices)
 * ============================================================ */

#define MAJOR_DEVICES  256
#define MINOR_DEVICES  256

/* Standard major numbers */
#define MAJOR_NULL     1
#define MAJOR_ZERO     1
#define MAJOR_TTY      4
#define MAJOR_HD       3
#define MAJOR_NET      30

/* ============================================================
 * Device Operations
 * ============================================================ */

/* Device manager */
int device_init(void);
int device_shutdown(void);

/* Device registration */
int device_register(device_t* dev);
int device_unregister(device_t* dev);
device_t* device_get_by_name(const char* name);
device_t* device_get_by_id(uint32_t id);
device_t* device_get_by_path(const char* path);

/* Device operations */
int device_open(const char* path, int flags);
int device_close(int fd);
int device_read(int fd, void* buf, size_t count);
int device_write(int fd, const void* buf, size_t count);
int device_ioctl(int fd, uint32_t cmd, void* arg);

/* Device management */
int device_enable(device_t* dev);
int device_disable(device_t* dev);
int device_reset(device_t* dev);

/* Driver management */
int driver_register(driver_t* drv);
int driver_unregister(driver_t* drv);
driver_t* driver_get_by_name(const char* name);
driver_t* driver_get_by_class(uint32_t class);

/* ============================================================
 * Device Drivers (to be implemented)
 * ============================================================ */

/* Console/TTY */
int tty_init(void);
int tty_register(void);

/* Disk/Storage */
int disk_init(void);
int disk_register(void);

/* Network */
int network_driver_init(void);

/* RTC */
int rtc_init(void);
int rtc_register(void);

/* PCI */
int pci_init(void);
device_t* pci_get_device(uint16_t vendor, uint16_t device);

/* I/O Port */
uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);
void     outb(uint16_t port, uint8_t val);
void     outw(uint16_t port, uint16_t val);
void     outl(uint16_t port, uint32_t val);

/* Memory-mapped I/O */
uint8_t  mmio_read8(uint32_t addr);
uint16_t mmio_read16(uint32_t addr);
uint32_t mmio_read32(uint32_t addr);
void     mmio_write8(uint32_t addr, uint8_t val);
void     mmio_write16(uint32_t addr, uint16_t val);
void     mmio_write32(uint32_t addr, uint32_t val);

#endif /* DEVICE_H */
