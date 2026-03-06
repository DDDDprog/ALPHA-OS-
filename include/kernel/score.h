/**
 * Alpha OS - Scoring/Reward System
 * Gamification for user achievements
 */

#ifndef SCORE_H
#define SCORE_H

#include "../kernel/types.h"
#include "login.h"

/* ============================================================
 * Achievement Categories
 * ============================================================ */

#define ACHV_MAX_NAME    32
#define ACHV_MAX_DESC    64
#define ACHV_MAX         32

/* ============================================================
 * Achievement Types
 * ============================================================ */

typedef enum {
    ACHV_BOOT,           /* First boot */
    ACHV_LOGIN,          /* Login streak */
    ACHV_COMMAND,        /* Commands executed */
    ACHV_UPTIME,        /* Long uptime */
    ACHV_EXPLORE,        /* Explore system */
    ACHV_NETWORK,        /* Network achievements */
    ACHV_FILE,           /* File operations */
    ACHV_CUSTOM          /* Custom achievements */
} achv_type_t;

/* ============================================================
 * Achievement Structure
 * ============================================================ */

typedef struct {
    char id[16];
    char name[ACHV_MAX_NAME];
    char description[ACHV_MAX_DESC];
    achv_type_t type;
    uint32_t points;
    bool unlocked;
    uint32_t unlocked_time;
    char unlocked_by[MAX_USERNAME_LEN];
} achievement_t;

/* ============================================================
 * User Score
 * ============================================================ */

typedef struct {
    char username[MAX_USERNAME_LEN];
    uint32_t total_points;
    uint32_t level;
    uint32_t commands_run;
    uint32_t login_streak;
    uint32_t achievements[ACHV_MAX];
    uint32_t achv_count;
    uint32_t session_commands;
    uint32_t session_start;
} user_score_t;

/* ============================================================
 * Score Events
 * ============================================================ */

typedef enum {
    SCORE_EVENT_LOGIN,
    SCORE_EVENT_LOGOUT,
    SCORE_EVENT_COMMAND,
    SCORE_EVENT_BOOT,
    SCORE_EVENT_EXPLORE,
    SCORE_EVENT_NETWORK,
    SCORE_EVENT_FILE
} score_event_t;

/* ============================================================
 * API
 * ============================================================ */

/* Initialize scoring system */
int score_init(void);

/* User scores */
int score_add(const char* username, uint32_t points);
int score_get(const char* username, user_score_t* score);
int score_level_up(const char* username);

/* Achievements */
int achievement_unlock(const char* username, const char* achv_id);
bool achievement_unlocked(const char* username, const char* achv_id);
int achievement_list(const char* username, char* buffer, size_t len);
int achievement_show_all(char* buffer, size_t len);

/* Score events */
int score_on_event(const char* username, score_event_t event, void* data);

/* Leaderboard */
int leaderboard_get(char* buffer, size_t len, size_t max_entries);

/* Points lookup */
uint32_t get_points_for_level(uint32_t level);
uint32_t get_level_from_points(uint32_t points);

/* Display */
void show_score_summary(const char* username);
void show_achievement_unlocked(achievement_t* achv);

#endif /* SCORE_H */
