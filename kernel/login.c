/**
 * Alpha OS - User Authentication Implementation
 */

#include "../include/kernel/login.h"
#include "../include/kernel/console.h"
#include "../include/kernel/persist.h"
#include "../include/libc/string.h"
#include "../include/libc/stdio.h"

#ifdef TEST_MODE
#include <stdio.h>
#include <time.h>
#endif

/* User database */
static user_t users[MAX_USERS];
static session_t current_session;
static bool auth_initialized = false;
static bool first_boot = false;

/* Simple hash */
static void simple_hash(const char* input, char* output) {
    uint32_t hash = 5381;
    int c;
    while ((c = *input++)) {
        hash = ((hash << 5) + hash) + c;
    }
    for (int i = 0; i < 16; i++) {
        sprintf(output + i * 2, "%02x", (hash >> (i * 2)) & 0xFF);
    }
    output[32] = '\0';
}

void hash_password(const char* password, char* hash_out) {
    simple_hash(password, hash_out);
}

bool verify_password(const char* password, const char* hash) {
    char computed[33];
    simple_hash(password, computed);
    return strcmp(computed, hash) == 0;
}

int auth_init(void) {
    if (auth_initialized) return 0;
    printf("[AUTH] Initializing authentication system\n");
    
    char test_user[32];
    size_t len = sizeof(test_user);
    if (persist_get("firstuser", test_user, &len) != 0) {
        first_boot = true;
        printf("[AUTH] First boot detected - setup required\n");
    } else {
        first_boot = false;
    }
    
    memset(users, 0, sizeof(users));
    memset(&current_session, 0, sizeof(session_t));
    auth_initialized = true;
    return 0;
}

bool is_first_boot(void) { return first_boot; }

int setup_first_user(const char* username, const char* password, const char* fullname) {
    if (!username || !password) return -1;
    int ret = user_add(username, password, fullname ? fullname : username, true);
    if (ret == 0) {
        char marker[] = "yes";
        persist_set("firstuser", marker, 3);
        first_boot = false;
        login(username, password);
    }
    return ret;
}

int user_add(const char* username, const char* password, const char* fullname, bool is_admin) {
    if (!username || !password) return -1;
    int slot = -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] == '\0') { slot = i; break; }
    }
    if (slot < 0) return -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] != '\0' && strcmp(users[i].username, username) == 0) return -1;
    }
    user_t* u = &users[slot];
    strncpy(u->username, username, MAX_USERNAME_LEN - 1);
    hash_password(password, u->password_hash);
    strncpy(u->fullname, fullname ? fullname : username, MAX_FULLNAME_LEN - 1);
    u->uid = 1000 + slot;
    u->gid = 1000;
    u->is_admin = is_admin;
    u->is_locked = false;
    u->login_count = 0;
    u->failed_attempts = 0;
    persist_set("users", users, sizeof(users));
    printf("[AUTH] User '%s' created (UID: %u)\n", username, u->uid);
    return 0;
}

int user_remove(const char* username) {
    if (!username) return -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] != '\0' && strcmp(users[i].username, username) == 0) {
            memset(&users[i], 0, sizeof(user_t));
            persist_set("users", users, sizeof(users));
            return 0;
        }
    }
    return -1;
}

int user_set_password(const char* username, const char* new_password) {
    if (!username || !new_password) return -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] != '\0' && strcmp(users[i].username, username) == 0) {
            hash_password(new_password, users[i].password_hash);
            persist_set("users", users, sizeof(users));
            return 0;
        }
    }
    return -1;
}

int user_set_shell(const char* username, const char* shell) { (void)username; (void)shell; return 0; }

bool user_exists(const char* username) {
    if (!username) return false;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] != '\0' && strcmp(users[i].username, username) == 0) return true;
    }
    return false;
}

int user_get_info(const char* username, user_t* info) {
    if (!username || !info) return -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] != '\0' && strcmp(users[i].username, username) == 0) {
            memcpy(info, &users[i], sizeof(user_t)); return 0;
        }
    }
    return -1;
}

int user_list(char* buffer, size_t len) {
    if (!buffer || !len) return -1;
    size_t offset = 0;
    for (int i = 0; i < MAX_USERS && offset < len - 1; i++) {
        if (users[i].username[0] != '\0') {
            offset += snprintf(buffer + offset, len - offset, "%s:%u:%s\n",
                             users[i].username, users[i].uid, users[i].is_admin ? "admin" : "user");
        }
    }
    return offset;
}

auth_result_t login(const char* username, const char* password) {
    if (!username || !password) return AUTH_INVALID_USER;
    user_t* u = NULL;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] != '\0' && strcmp(users[i].username, username) == 0) { u = &users[i]; break; }
    }
    if (!u) return AUTH_INVALID_USER;
    if (u->is_locked) { printf("[AUTH] Account '%s' is locked\n", username); return AUTH_ACCOUNT_LOCKED; }
    if (!verify_password(password, u->password_hash)) {
        u->failed_attempts++;
        printf("[AUTH] Login failed for '%s' (attempt %u)\n", username, u->failed_attempts);
        if (u->failed_attempts >= 5) { u->is_locked = true; printf("[AUTH] Account '%s' locked\n", username); }
        return AUTH_INVALID_USER;
    }
    u->login_count++;
    u->failed_attempts = 0;
    strncpy(current_session.username, username, MAX_USERNAME_LEN - 1);
    current_session.uid = u->uid;
    current_session.session_id = u->login_count;
    current_session.active = true;
    u->session_active = true;
    persist_set("users", users, sizeof(users));
    printf("[AUTH] User '%s' logged in (UID: %u)\n", username, u->uid);
    return AUTH_SUCCESS;
}

int logout(const char* username) {
    if (!username) return -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] != '\0' && strcmp(users[i].username, username) == 0) users[i].session_active = false;
    }
    if (strcmp(current_session.username, username) == 0) memset(&current_session, 0, sizeof(session_t));
    printf("[AUTH] User '%s' logged out\n", username);
    return 0;
}

int logout_all(void) {
    for (int i = 0; i < MAX_USERS; i++) users[i].session_active = false;
    memset(&current_session, 0, sizeof(session_t));
    printf("[AUTH] All users logged out\n");
    return 0;
}

session_t* get_session(const char* username) {
    if (!username) return NULL;
    if (strcmp(current_session.username, username) == 0) return &current_session;
    return NULL;
}

session_t* get_current_session(void) { return current_session.active ? &current_session : NULL; }
bool is_logged_in(void) { return current_session.active; }
const char* get_current_user(void) { return current_session.active ? current_session.username : NULL; }
uint32_t get_current_uid(void) { return current_session.active ? current_session.uid : 0; }

int lock_account(const char* username) {
    if (!username) return -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] != '\0' && strcmp(users[i].username, username) == 0) {
            users[i].is_locked = true; return 0;
        }
    }
    return -1;
}

int unlock_account(const char* username) {
    if (!username) return -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username[0] != '\0' && strcmp(users[i].username, username) == 0) {
            users[i].is_locked = false; users[i].failed_attempts = 0; return 0;
        }
    }
    return -1;
}

/* Boot theme */
void show_login_theme(void) {
    console_clear();
    console_set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    printf("\n");
    printf("  ╔═══════════════════════════════════════════════════════════╗\n");
    printf("  ║                                                           ║\n");
    printf("  ║     ██╗    ██╗███████╗██╗      ██████╗ ██████╗ ██╗   ██╗  ║\n");
    printf("  ║     ██║    ██║██╔════╝██║     ██╔═══██╗██╔══██╗╚██╗ ██╔╝  ║\n");
    printf("  ║     ██║ █╗ ██║█████╗  ██║     ██║   ██║██████╔╝ ╚████╔╝   ║\n");
    printf("  ║     ██║███╗██║██╔══╝  ██║     ██║   ██║██╔══██╗  ╚██╔╝    ║\n");
    printf("  ║     ╚███╔███╔╝███████╗███████╗╚██████╔╝██║  ██║   ██║     ║\n");
    printf("  ║      ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝     ║\n");
    printf("  ║                                                           ║\n");
    printf("  ║           ALPHA OS - Advanced Operating System            ║\n");
    printf("  ║                    Version 1.0.0                          ║\n");
    printf("  ║                                                           ║\n");
    printf("  ╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void show_banner(void) {
    console_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  ALPHA OS - Boot Complete                              ║\n");
    printf("║  Copyright (c) 2024 Alpha OS Team                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}
