/**
 * Alpha OS - User Authentication System
 * Login, password, user management
 */

#ifndef LOGIN_H
#define LOGIN_H

#include "../kernel/types.h"

/* ============================================================
 * User Constants
 * ============================================================ */

#define MAX_USERS          8
#define MAX_USERNAME_LEN   32
#define MAX_PASSWORD_LEN   64
#define MAX_FULLNAME_LEN  64

/* ============================================================
 * User Structure
 * ============================================================ */

typedef struct {
    char username[MAX_USERNAME_LEN];
    char password_hash[64];      /* SHA-256 hash */
    char fullname[MAX_FULLNAME_LEN];
    uint32_t uid;
    uint32_t gid;
    bool is_admin;
    bool is_locked;
    uint32_t login_count;
    uint32_t failed_attempts;
    uint32_t last_login;
    bool session_active;
    uint32_t session_start;
} user_t;

/* ============================================================
 * Session
 * ============================================================ */

typedef struct {
    char username[MAX_USERNAME_LEN];
    uint32_t uid;
    uint32_t session_id;
    uint32_t login_time;
    bool active;
} session_t;

/* ============================================================
 * Auth Result
 * ============================================================ */

typedef enum {
    AUTH_SUCCESS = 0,
    AUTH_INVALID_USER,
   _AUTH_INVALID_PASSWORD,
    AUTH_ACCOUNT_LOCKED,
    AUTH_SESSION_EXISTS,
    AUTH_MAX_USERS
} auth_result_t;

/* ============================================================
 * API
 * ============================================================ */

/* Initialize auth system */
int auth_init(void);

/* User management */
int user_add(const char* username, const char* password, const char* fullname, bool is_admin);
int user_remove(const char* username);
int user_set_password(const char* username, const char* new_password);
int user_set_shell(const char* username, const char* shell);
bool user_exists(const char* username);
int user_get_info(const char* username, user_t* info);
int user_list(char* buffer, size_t len);

/* Authentication */
auth_result_t login(const char* username, const char* password);
int logout(const char* username);
int logout_all(void);

/* Session */
session_t* get_session(const char* username);
session_t* get_current_session(void);
bool is_logged_in(void);
const char* get_current_user(void);
uint32_t get_current_uid(void);

/* Boot theme / First boot */
bool is_first_boot(void);
int setup_first_user(const char* username, const char* password, const char* fullname);

/* Password hashing */
void hash_password(const char* password, char* hash_out);
bool verify_password(const char* password, const char* hash);

/* Boot time theme */
void show_login_theme(void);
void show_banner(void);

/* Lock/Unlock */
int lock_account(const char* username);
int unlock_account(const char* username);

#endif /* LOGIN_H */
