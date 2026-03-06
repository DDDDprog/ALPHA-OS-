/**
 * Alpha OS - ACPI and Power Management
 * 
 * ACPI tables, power states, and hardware control
 */

#ifndef ACPI_H
#define ACPI_H

#include "../kernel/types.h"

/* ============================================================
 * ACPI Tables
 * ============================================================ */

#define ACPI_SDT_HEADER_SIGNATURE  0x20544350  /* "APIC" */
#define ACPI_RSDP_SIGNATURE        0x20525344  /* "RSD " */
#define ACPI_XSDP_SIGNATURE        0x20534458  /* "XSDT" */

/* ACPI Table Types */
#define ACPI_TABLE_RSDP    0
#define ACPI_TABLE_RSDT    1
#define ACPI_TABLE_XSDT    2
#define ACPI_TABLE_FACP    3
#define ACPI_TABLE_MADT    4
#define ACPI_TABLE_SSDT    5
#define ACPI_TABLE_HPET    6

/* ============================================================
 * ACPI Structures
 * ============================================================ */

/* RSDP (Root System Description Pointer) */
typedef struct {
    char     signature[8];    /* "RSD PTR " */
    uint8_t  checksum;
    char     oem_id[6];
    uint8_t  revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t  extended_checksum;
    char     reserved[3];
} __attribute__((packed)) acpi_rsdp_t;

/* ACPI Generic Address Structure */
typedef struct {
    uint8_t  address_space_id;
    uint8_t  register_bit_width;
    uint8_t  register_bit_offset;
    uint8_t  reserved;
    uint64_t address;
} __attribute__((packed)) acpi_gas_t;

/* FADT (Fixed ACPI Description Table) */
typedef struct {
    uint32_t signature;        /* "FACP" */
    uint32_t length;
    uint8_t  revision;
    uint8_t  checksum;
    char     oem_id[6];
    char     oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
    
    uint32_t firmware_ctrl;
    uint32_t dsdt;
    uint8_t  reserved1;
    uint8_t  preferred_pm_profile;
    uint16_t sci_int;
    uint32_t smi_cmd;
    uint8_t  acpi_enable;
    uint8_t  acpi_disable;
    uint8_t  s4bios_req;
    uint8_t  pstate_cnt;
    uint32_t pm1a_evt_blk;
    uint32_t pm1b_evt_blk;
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
    uint32_t pm2_cnt_blk;
    uint32_t pm_tmr_blk;
    uint32_t gpe0_blk;
    uint32_t gpe1_blk;
    uint8_t  pm1_evt_len;
    uint8_t  pm1_cnt_len;
    uint8_t  pm2_cnt_len;
    uint8_t  pm_tmr_len;
    uint8_t  gpe0_len;
    uint8_t  gpe1_len;
    uint8_t  gpe1_base;
    uint8_t  cstate_cnt;
    uint16_t c2_latency;
    uint16_t c3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t  duty_offset;
    uint8_t  duty_width;
    uint8_t  day_alrm;
    uint8_t  month_alrm;
    uint8_t  century;
    uint16_t ia_pc_boot_arch;
    uint8_t  reserved2;
    uint32_t flags;
    
    /* ACPI 2.0+ fields */
    acpi_gas_t reset_reg;
    uint8_t  reset_value;
    uint16_t arm_boot_arch;
    uint8_t  fadt_minor;
    uint32_t x_firmware_ctrl;
    uint64_t x_dsdt;
    acpi_gas_t x_pm1a_evt_blk;
    acpi_gas_t x_pm1b_evt_blk;
    acpi_gas_t x_pm1a_cnt_blk;
    acpi_gas_t x_pm1b_cnt_blk;
    acpi_gas_t x_pm2_cnt_blk;
    acpi_gas_t x_pm_tmr_blk;
    acpi_gas_t x_gpe0_blk;
    acpi_gas_t x_gpe1_blk;
} __attribute__((packed)) acpi_fadt_t;

/* ============================================================
 * Power States (S-states)
 * ============================================================ */

#define ACPI_S0   0   /* Working */
#define ACPI_S1   1   /* Sleep (CPU context preserved) */
#define ACPI_S2   2   /* Sleep (CPU lost, cache flushed) */
#define ACPI_S3   3   /* Suspend to RAM (S3) */
#define ACPI_S4   4   /* Suspend to Disk (hibernate) */
#define ACPI_S5   5   /* Soft off */

/* ============================================================
 * Sleep States
 * ============================================================ */

#define PM1_STS_BIT      0x01
#define PM1_EN_BIT       0x02
#define PM1_TMR_BIT      0x04

/* PM1 Control */
#define PM1_SCI_EN       (1 << 0)
#define PM1_SLEEP_TYPE   (7 << 2)
#define PM1_SLEEP_ENABLE (1 << 5)

/* ============================================================
 * RTC/NVRAM
 * ============================================================ */

#define RTC_PORT_INDEX   0x70
#define RTC_PORT_DATA    0x71

/* RTC Registers */
#define RTC_SECOND       0x00
#define RTC_MINUTE       0x01
#define RTC_HOUR         0x02
#define RTC_DAY          0x03
#define RTC_MONTH        0x04
#define RTC_YEAR         0x05
#define RTC_REG_A        0x0A
#define RTC_REG_B        0x0B
#define RTC_REG_C        0x0C
#define RTC_REG_D        0x0D

/* RTC Register B flags */
#define RTC_24H          0x02
#define RTC_BIN          0x04
#define RTC_SQUARE       0x08
#define RTC_UPDATE       0x80

/* NVRAM Size (128 bytes standard) */
#define NVRAM_SIZE       128

/* ============================================================
 * Power Management API
 * ============================================================ */

/* ACPI */
int acpi_init(void);
int acpi_shutdown(void);
bool acpi_enabled(void);

/* RSDP */
acpi_rsdp_t* acpi_get_rsdp(void);
bool acpi_validate_rsdp(acpi_rsdp_t* rsdp);

/* Sleep */
int acpi_sleep(uint8_t state);
int acpi_prepare_sleep(uint8_t state);
int acpi_wake_from_sleep(uint8_t state);

/* Power button */
void acpi_power_button_init(void);
void acpi_power_button_handler(void);

/* ACPI Timer */
uint32_t acpi_get_timer_ticks(void);
void acpi_wait_ms(uint32_t ms);

/* ============================================================
 * RTC API
 * ============================================================ */

int rtc_init(void);
void rtc_read_time(uint8_t* hour, uint8_t* min, uint8_t* sec);
void rtc_read_date(uint16_t* year, uint8_t* month, uint8_t* day);
void rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec);
void rtc_set_date(uint16_t year, uint8_t month, uint8_t day);

/* NVRAM */
int nvram_read(uint8_t offset, uint8_t* data, size_t len);
int nvram_write(uint8_t offset, const uint8_t* data, size_t len);
uint8_t nvram_read_byte(uint8_t offset);
void nvram_write_byte(uint8_t offset, uint8_t data);

/* RTC Alarm */
void rtc_set_alarm(uint8_t hour, uint8_t min, uint8_t sec);
void rtc_enable_alarm(void);
void rtc_disable_alarm(void);

/* ============================================================
 * Power Control API
 * ============================================================ */

typedef enum {
    POWER_OFF,
    POWER_REBOOT,
    POWER_SLEEP,
    POWER_HIBERNATE
} power_action_t;

/* Power management */
int power_init(void);
int power_shutdown(power_action_t action);
int power_reboot(void);
int power_off(void);
int power_sleep(uint8_t state);
int power_hibernate(void);

/* Battery (if available) */
typedef struct {
    uint16_t voltage;        /* mV */
    uint8_t  percent;       /* 0-100 */
    bool     charging;
    bool     present;
} battery_info_t;

int battery_get_info(battery_info_t* info);
bool battery_present(void);

/* ============================================================
 * CMOS Memory (for data persistence)
 * ============================================================ */

#define CMOS_MEMORY_SIZE  256

/* CMOS Layout */
#define CMOS_BOOT_COUNT     0
#define CMOS_LAST_SHUTDOWN  1
#define CMOS_SHUTDOWN_MAGIC  0x5A
#define CMOS_WAKE_ALARM     10
#define CMOS_FLAGS          20
#define CMOS_DATA_START     32  /* User data starts here */

/* Data persistence */
int cmos_save_data(const void* data, size_t len, size_t offset);
int cmos_load_data(void* data, size_t len, size_t offset);
int cmos_set_boot_count(uint32_t count);
uint32_t cmos_get_boot_count(void);
int cmos_set_last_shutdown(uint8_t reason);
uint8_t cmos_get_last_shutdown(void);

#endif /* ACPI_H */
