/**
 * Alpha OS - ACPI and Power Management Implementation
 */

#include "../include/kernel/acpi.h"
#include "../include/kernel/console.h"
#include "../include/libc/string.h"

#ifdef TEST_MODE
#include <stdio.h>
#include <time.h>
#else
#include "../include/kernel/device.h"
#endif

/* ACPI state */
static bool acpi_initialized = false;
static bool acpi_present = false;
static void* rsdp = NULL;

/* PM registers */
static uint32_t pm1a_cnt = 0;
static uint32_t pm1b_cnt = 0;

/* Initialization */
#ifdef TEST_MODE

int acpi_init(void) {
    if (acpi_initialized) return 0;
    printf("[ACPI] Initializing ACPI\n");
    acpi_present = true;
    rtc_init();
    acpi_initialized = true;
    printf("[ACPI] ACPI initialized (simulation mode)\n");
    return 0;
}

int acpi_shutdown(void) {
    printf("[ACPI] ACPI shutdown\n");
    return 0;
}

#else

int acpi_init(void) {
    if (acpi_initialized) return 0;
    acpi_initialized = true;
    printf("[ACPI] ACPI initialized\n");
    return 0;
}

int acpi_shutdown(void) {
    return 0;
}

#endif

bool acpi_enabled(void) { return acpi_present; }
void* acpi_get_rsdp(void) { return rsdp; }
bool acpi_validate_rsdp(void* ptr) { (void)ptr; return false; }

int acpi_sleep(uint8_t state) {
    printf("[ACPI] Entering S%d\n", state);
    cmos_set_last_shutdown(state);
    return 0;
}

int acpi_prepare_sleep(uint8_t state) { (void)state; return 0; }
int acpi_wake_from_sleep(uint8_t state) { (void)state; return 0; }

void acpi_power_button_init(void) { }
void acpi_power_button_handler(void) { printf("[ACPI] Power button pressed!\n"); }
uint32_t acpi_get_timer_ticks(void) { return 0; }
void acpi_wait_ms(uint32_t ms) { (void)ms; }

#ifdef TEST_MODE

int rtc_init(void) {
    printf("[RTC] Real-time clock initialized\n");
    return 0;
}

void rtc_read_time(uint8_t* hour, uint8_t* min, uint8_t* sec) {
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    if (hour) *hour = tm->tm_hour;
    if (min)  *min = tm->tm_min;
    if (sec)  *sec = tm->tm_sec;
}

void rtc_read_date(uint16_t* year, uint8_t* month, uint8_t* day) {
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    if (year)  *year = tm->tm_year + 1900;
    if (month) *month = tm->tm_mon + 1;
    if (day)   *day = tm->tm_mday;
}

#else

int rtc_init(void) { return 0; }

void rtc_read_time(uint8_t* hour, uint8_t* min, uint8_t* sec) {
    (void)hour; (void)min; (void)sec;
}

void rtc_read_date(uint16_t* year, uint8_t* month, uint8_t* day) {
    (void)year; (void)month; (void)day;
}

#endif

void rtc_set_time(uint8_t hour, uint8_t min, uint8_t sec) {
    (void)hour; (void)min; (void)sec;
}

void rtc_set_date(uint16_t year, uint8_t month, uint8_t day) {
    (void)year; (void)month; (void)day;
}

int nvram_read(uint8_t offset, uint8_t* data, size_t len) {
    (void)offset; (void)data; (void)len;
    return -1;
}

int nvram_write(uint8_t offset, const uint8_t* data, size_t len) {
    (void)offset; (void)data; (void)len;
    return -1;
}

uint8_t nvram_read_byte(uint8_t offset) { (void)offset; return 0; }
void nvram_write_byte(uint8_t offset, uint8_t data) { (void)offset; (void)data; }

void rtc_set_alarm(uint8_t hour, uint8_t min, uint8_t sec) {
    (void)hour; (void)min; (void)sec;
}

void rtc_enable_alarm(void) { }
void rtc_disable_alarm(void) { }

int power_init(void) {
    printf("[POWER] Power management initialized\n");
    acpi_init();
    return 0;
}

int power_reboot(void) {
    printf("[POWER] Rebooting...\n");
    return 0;
}

int power_off(void) {
    printf("[POWER] Powering off...\n");
    cmos_set_last_shutdown(5);
    return 0;
}

int power_sleep(uint8_t state) {
    printf("[POWER] Entering sleep state S%d\n", state);
    return acpi_sleep(state);
}

int power_hibernate(void) {
    printf("[POWER] Hibernating...\n");
    return acpi_sleep(4);
}

int power_shutdown(power_action_t action) {
    switch (action) {
        case POWER_OFF: return power_off();
        case POWER_REBOOT: return power_reboot();
        case POWER_SLEEP: return power_sleep(3);
        case POWER_HIBERNATE: return power_hibernate();
        default: return -1;
    }
}

int cmos_save_data(const void* data, size_t len, size_t offset) {
    (void)data; (void)len; (void)offset;
    return -1;
}

int cmos_load_data(void* data, size_t len, size_t offset) {
    (void)data; (void)len; (void)offset;
    return -1;
}

int cmos_set_boot_count(uint32_t count) {
    (void)count;
    return 0;
}

uint32_t cmos_get_boot_count(void) { return 0; }

int cmos_set_last_shutdown(uint8_t reason) {
    (void)reason;
    return 0;
}

uint8_t cmos_get_last_shutdown(void) { return 0xFF; }

int battery_get_info(void* info) {
    (void)info;
    return -1;
}

bool battery_present(void) { return false; }
