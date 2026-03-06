/**
 * Alpha OS - Persistent Storage Module
 * 
 * Save/restore data across reboots using NVRAM/CMOS
 */

#ifndef PERSIST_H
#define PERSIST_H

#include "../kernel/types.h"

/* ============================================================
 * Persistent Storage Configuration
 * ============================================================ */

#define PERSIST_MAX_KEYS      32
#define PERSIST_MAX_KEY_LEN   16
#define PERSIST_MAX_VALUE_LEN 64

/* ============================================================
 * Persistent Key-Value Store
 * ============================================================ */

typedef struct {
    char key[PERSIST_MAX_KEY_LEN];
    uint8_t value[PERSIST_MAX_VALUE_LEN];
    uint8_t value_len;
    bool used;
} persist_entry_t;

typedef struct {
    persist_entry_t entries[PERSIST_MAX_KEYS];
    uint32_t magic;
    uint16_t version;
    uint16_t checksum;
} persist_data_t;

/* Magic number for validation */
#define PERSIST_MAGIC    0x414C5048  /* "ALPH" */
#define PERSIST_VERSION  1

/* ============================================================
 * API
 * ============================================================ */

/* Initialize persistent storage */
int persist_init(void);

/* Set a value */
int persist_set(const char* key, const void* value, size_t len);

/* Get a value */
int persist_get(const char* key, void* value, size_t* len);

/* Delete a key */
int persist_delete(const char* key);

/* Check if key exists */
bool persist_exists(const char* key);

/* List all keys */
int persist_list_keys(char* buffer, size_t len);

/* Save to NVRAM */
int persist_save(void);

/* Load from NVRAM */
int persist_load(void);

/* Clear all data */
int persist_clear(void);

/* ============================================================
 * Predefined Keys
 * ============================================================ */

#define PERSIST_KEY_HOSTNAME   "hostname"
#define PERSIST_KEY_USER       "username"
#define PERSIST_KEY_LANGUAGE   "lang"
#define PERSIST_KEY_TIMEZONE  "timezone"
#define PERSIST_KEY_BOOTCOUNT  "bootcnt"
#define PERSIST_KEY_LASTERR    "lasterr"
#define PERSIST_KEY_CONFIG     "config"

#endif /* PERSIST_H */
