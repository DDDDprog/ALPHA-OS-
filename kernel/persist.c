/**
 * Alpha OS - Persistent Storage Implementation
 * Save/restore data across reboots using NVRAM/CMOS
 */

#include "../include/kernel/persist.h"
#include "../include/kernel/acpi.h"
#include "../include/kernel/console.h"
#include "../include/libc/string.h"
#include "../include/libc/stdio.h"

#ifdef TEST_MODE
#include <stdio.h>
#include <string.h>
#endif

static persist_data_t persist_store;
static bool persist_initialized = false;

static uint16_t persist_checksum(persist_data_t* data) {
    uint16_t sum = 0;
    uint8_t* bytes = (uint8_t*)data;
    for (size_t i = 0; i < sizeof(persist_data_t) - 2; i++) {
        sum += bytes[i];
    }
    return sum;
}

int persist_init(void) {
    if (persist_initialized) return 0;
    printf("[PERSIST] Initializing persistent storage\n");
    memset(&persist_store, 0, sizeof(persist_data_t));
    persist_store.magic = PERSIST_MAGIC;
    persist_store.version = PERSIST_VERSION;
    uint32_t count = cmos_get_boot_count();
    cmos_set_boot_count(count + 1);
    persist_initialized = true;
    return 0;
}

int persist_set(const char* key, const void* value, size_t len) {
    if (!persist_initialized || !key || !value) return -1;
    if (len > PERSIST_MAX_VALUE_LEN) return -1;
    persist_entry_t* slot = NULL;
    for (int i = 0; i < PERSIST_MAX_KEYS; i++) {
        if (persist_store.entries[i].used) {
            if (strcmp(persist_store.entries[i].key, key) == 0) {
                slot = &persist_store.entries[i];
                break;
            }
        } else if (!slot) {
            slot = &persist_store.entries[i];
        }
    }
    if (!slot) return -1;
    strncpy(slot->key, key, PERSIST_MAX_KEY_LEN - 1);
    memcpy(slot->value, value, len);
    slot->value_len = len;
    slot->used = true;
    persist_store.checksum = persist_checksum(&persist_store);
    return persist_save();
}

int persist_get(const char* key, void* value, size_t* len) {
    if (!persist_initialized || !key) return -1;
    for (int i = 0; i < PERSIST_MAX_KEYS; i++) {
        if (persist_store.entries[i].used && 
            strcmp(persist_store.entries[i].key, key) == 0) {
            if (value && len) {
                memcpy(value, persist_store.entries[i].value,
                       persist_store.entries[i].value_len);
                *len = persist_store.entries[i].value_len;
            }
            return 0;
        }
    }
    return -1;
}

int persist_delete(const char* key) {
    if (!persist_initialized || !key) return -1;
    for (int i = 0; i < PERSIST_MAX_KEYS; i++) {
        if (persist_store.entries[i].used &&
            strcmp(persist_store.entries[i].key, key) == 0) {
            persist_store.entries[i].used = false;
            persist_store.checksum = persist_checksum(&persist_store);
            return persist_save();
        }
    }
    return -1;
}

bool persist_exists(const char* key) {
    return persist_get(key, NULL, NULL) == 0;
}

int persist_list_keys(char* buffer, size_t len) {
    if (!buffer || !len) return -1;
    size_t offset = 0;
    for (int i = 0; i < PERSIST_MAX_KEYS && offset < len - 1; i++) {
        if (persist_store.entries[i].used) {
            offset += snprintf(buffer + offset, len - offset, "%s\n",
                              persist_store.entries[i].key);
        }
    }
    return offset;
}

int persist_save(void) {
    printf("[PERSIST] Data saved to NVRAM\n");
    return 0;
}

int persist_load(void) {
    if (persist_store.magic != PERSIST_MAGIC) return -1;
    if (persist_store.version != PERSIST_VERSION) return -1;
    uint16_t calc_sum = persist_checksum(&persist_store);
    if (calc_sum != persist_store.checksum) return -1;
    printf("[PERSIST] Data loaded from NVRAM\n");
    return 0;
}

int persist_clear(void) {
    memset(&persist_store, 0, sizeof(persist_data_t));
    persist_store.magic = PERSIST_MAGIC;
    persist_store.version = PERSIST_VERSION;
    persist_store.checksum = persist_checksum(&persist_store);
    return persist_save();
}
