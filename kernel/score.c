/**
 * Alpha OS - Scoring System Implementation
 */

#include "../include/kernel/score.h"
#include "../include/kernel/login.h"
#include "../include/kernel/console.h"
#include "../include/libc/string.h"
#include "../include/libc/stdio.h"

#ifdef TEST_MODE
#include <stdio.h>
#endif

static user_score_t current_user_score;
static achievement_t achievements[ACHV_MAX];
static int achv_count = 0;
static bool score_initialized = false;

static void init_achievements(void) {
    achv_count = 0;
    achievement_t* a = &achievements[achv_count++];
    strcpy(a->id, "first_boot"); strcpy(a->name, "Welcome!");
    strcpy(a->description, "Boot Alpha OS"); a->type = ACHV_BOOT; a->points = 10;
    
    a = &achievements[achv_count++];
    strcpy(a->id, "first_login"); strcpy(a->name, "Getting Started");
    strcpy(a->description, "Log in first time"); a->type = ACHV_LOGIN; a->points = 20;
    
    a = &achievements[achv_count++];
    strcpy(a->id, "command_10"); strcpy(a->name, "Explorer");
    strcpy(a->description, "Run 10 commands"); a->type = ACHV_COMMAND; a->points = 30;
    
    a = &achievements[achv_count++];
    strcpy(a->id, "command_100"); strcpy(a->name, "Power User");
    strcpy(a->description, "Run 100 commands"); a->type = ACHV_COMMAND; a->points = 100;
    
    a = &achievements[achv_count++];
    strcpy(a->id, "streak_3"); strcpy(a->name, "Dedicated");
    strcpy(a->description, "Log in 3 days"); a->type = ACHV_LOGIN; a->points = 50;
}

int score_init(void) {
    if (score_initialized) return 0;
    printf("[SCORE] Initializing scoring system\n");
    init_achievements();
    memset(&current_user_score, 0, sizeof(user_score_t));
    score_initialized = true;
    return 0;
}

uint32_t get_points_for_level(uint32_t level) { return level * 100 + level * level * 10; }
uint32_t get_level_from_points(uint32_t points) { uint32_t level = 1; while (get_points_for_level(level) <= points) level++; return level; }

int score_on_event(const char* username, score_event_t event, void* data) {
    if (!username) return -1;
    const char* user = get_current_user();
    if (!user || strcmp(user, username) != 0) return -1;
    switch (event) {
        case SCORE_EVENT_LOGIN:
            current_user_score.login_streak++;
            achievement_unlock(username, "first_login");
            score_add(username, 5);
            break;
        case SCORE_EVENT_COMMAND:
            current_user_score.commands_run++;
            score_add(username, 1);
            if (current_user_score.commands_run >= 10) achievement_unlock(username, "command_10");
            if (current_user_score.commands_run >= 100) achievement_unlock(username, "command_100");
            break;
        case SCORE_EVENT_BOOT: achievement_unlock(username, "first_boot"); break;
        default: break;
    }
    return 0;
}

int score_add(const char* username, uint32_t points) {
    if (!username) return -1;
    if (strcmp(current_user_score.username, username) != 0) {
        strncpy(current_user_score.username, username, MAX_USERNAME_LEN - 1);
    }
    current_user_score.total_points += points;
    current_user_score.level = get_level_from_points(current_user_score.total_points);
    return 0;
}

int score_get(const char* username, user_score_t* score) {
    if (!username || !score) return -1;
    if (strcmp(current_user_score.username, username) == 0) { memcpy(score, &current_user_score, sizeof(user_score_t)); return 0; }
    return -1;
}

int achievement_unlock(const char* username, const char* achv_id) {
    if (!username || !achv_id) return -1;
    for (int i = 0; i < achv_count; i++) {
        if (strcmp(achievements[i].id, achv_id) == 0) {
            if (!achievements[i].unlocked) {
                achievements[i].unlocked = true;
                strncpy(achievements[i].unlocked_by, username, MAX_USERNAME_LEN - 1);
                score_add(username, achievements[i].points);
                console_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
                printf("\n[ACHIEVEMENT] %s - %s (+%u pts)\n", achievements[i].name, achievements[i].description, achievements[i].points);
                console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            }
            return 0;
        }
    }
    return -1;
}

bool achievement_unlocked(const char* username, const char* achv_id) {
    (void)username;
    if (!achv_id) return false;
    for (int i = 0; i < achv_count; i++) {
        if (strcmp(achievements[i].id, achv_id) == 0) return achievements[i].unlocked;
    }
    return false;
}

int achievement_list(const char* username, char* buffer, size_t len) {
    if (!buffer || !len) return -1;
    size_t offset = 0;
    for (int i = 0; i < achv_count && offset < len - 1; i++) {
        if (achievements[i].unlocked && strcmp(achievements[i].unlocked_by, username) == 0) {
            offset += snprintf(buffer + offset, len - offset, "* %s\n", achievements[i].name);
        }
    }
    return offset;
}

int achievement_show_all(char* buffer, size_t len) {
    if (!buffer || !len) return -1;
    size_t offset = 0;
    for (int i = 0; i < achv_count && offset < len - 1; i++) {
        if (achievements[i].unlocked) offset += snprintf(buffer + offset, len - offset, "* %s: %s\n", achievements[i].name, achievements[i].description);
        else offset += snprintf(buffer + offset, len - offset, "* %s (Locked)\n", achievements[i].name);
    }
    return offset;
}

int leaderboard_get(char* buffer, size_t len, size_t max_entries) {
    (void)max_entries;
    if (!buffer || !len) return -1;
    return snprintf(buffer, len, "Level %u | %u pts | %u cmds\n", current_user_score.level, current_user_score.total_points, current_user_score.commands_run);
}

void show_score_summary(const char* username) {
    if (!username) return;
    console_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    printf("\n=== SCORE ===\nUser: %s\nLevel: %u\nPoints: %u\nCommands: %u\nStreak: %u\n============\n",
           username, current_user_score.level, current_user_score.total_points, current_user_score.commands_run, current_user_score.login_streak);
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void show_achievement_unlocked(achievement_t* achv) {
    if (!achv) return;
    console_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    printf("\n*** ACHIEVEMENT: %s ***\n%s (+%u pts)\n", achv->name, achv->description, achv->points);
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}
