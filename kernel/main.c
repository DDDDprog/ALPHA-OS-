#include "../include/kernel/console.h"
#include "../include/kernel/fs.h"
#include "../include/kernel/keyboard.h"
#include "../include/kernel/memory.h"
#include "../include/kernel/shell.h"
#include "../include/kernel/system.h"
#include "../include/kernel/cpu.h"
#include "../include/kernel/scheduler.h"
#include "../include/kernel/boot_info.h"
#include "../include/kernel/login.h"
#include "../include/kernel/persist.h"
#include "../include/libc/stdio.h"
#include "../include/libc/string.h"

#ifndef TEST_MODE

// Forward declarations
static void setup_first_user_wizard(void);
static void do_login_prompt(void);

// Only define kernel_main for actual kernel compilation
void kernel_main(void) {
    // Show boot messages like real kernel
    boot_message_start();

    // Initialize kernel components
    console_init();
    memory_init();
    memory_detect();
    memory_setup_paging();
    cpu_init();
    keyboard_init();
    system_init();
    scheduler_init();

    // Run hardware detection - this shows all the boot messages
    boot_diagnostics_init();
    boot_show_all();

    // Show boot complete message
    boot_message_end();

    // Initialize file system
    console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("[OK] ");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Initializing Alpha File System...\n");
    fs_init();

    // Initialize authentication
    console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("[OK] ");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Initializing Authentication System...\n");
    auth_init();
    persist_init();

    // Initialize shell
    console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("[OK] ");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Initializing Alpha Shell...\n");
    shell_init();

    printf("\n");

    // First boot setup or login
    if (is_first_boot()) {
        setup_first_user_wizard();
    } else {
        do_login_prompt();
    }

    printf("\n");
    console_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    printf("Welcome to Alpha OS 2.0! Type 'help' for commands.\n");
    console_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printf("New Features: Boot Diagnostics | Login | Scoring\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("\n");

    // Run the shell
    shell_run();

    // This point should never be reached
    printf("Alpha OS kernel halted.\n");
    system_halt();
}

/* First boot user setup wizard */
static void setup_first_user_wizard(void) {
    char username[64] = {0};
    char password[64] = {0};
    char confirm[64] = {0};
    char fullname[128] = {0};

    console_clear();
    show_login_theme();
    
    console_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    printf("  First Boot Setup - Create Your Account\n");
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    printf("\n");
    printf("  This wizard will help you create the first user account.\n");
    printf("  This account will have administrator privileges.\n");
    printf("\n");

    // Get username
    console_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    printf("  Enter username: ");
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    keyboard_get_line(username, sizeof(username));
    trim(username);
    
    // Validate username
    if (username[0] == '\0') {
        strcpy(username, "alpha");
    }

    // Get full name
    console_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    printf("  Enter full name (or press Enter for '%s'): ", username);
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    keyboard_get_line(fullname, sizeof(fullname));
    trim(fullname);

    // Get password
    console_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    printf("  Enter password: ");
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    keyboard_get_line(password, sizeof(password));
    
    // Confirm password
    console_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    printf("  Confirm password: ");
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    keyboard_get_line(confirm, sizeof(confirm));

    // Verify passwords match
    if (strcmp(password, confirm) != 0) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("\n  Passwords do not match! Using default password.\n");
        console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        strcpy(password, "alpha123");
    }

    // Create the user
    console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("\n  Creating user '%s'...\n", username);
    
    if (setup_first_user(username, password, fullname[0] ? fullname : NULL) == 0) {
        console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        printf("  User created successfully!\n"); shell_set_user(username, true);
    } else {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("  Failed to create user!\n");
        console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        setup_first_user("alpha", "alpha123", "Alpha User");
    }

    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    printf("\n  Press any key to continue...");
    while (!keyboard_available()) { keyboard_poll(); }
    keyboard_read();
}

/* Login prompt */
static void do_login_prompt(void) {
    char username[64] = {0};
    char password[64] = {0};
    int attempts = 0;
    const int max_attempts = 5;

    while (attempts < max_attempts) {
        console_clear();
        show_login_theme();
        
        console_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        printf("  Alpha OS Login\n");
        console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        printf("\n");

        if (attempts > 0) {
            console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            printf("  Invalid username or password (%d/%d attempts)\n", attempts, max_attempts);
            console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            printf("\n");
        }

        // Get username
        console_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
        printf("  Login: ");
        console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        keyboard_get_line(username, sizeof(username));
        trim(username);

        if (username[0] == '\0') continue;

        // Get password
        console_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
        printf("  Password: ");
        console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        keyboard_get_line(password, sizeof(password));

        // Try to login
        if (login(username, password) == AUTH_SUCCESS) {
            console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            printf("\n  Login successful!\n"); shell_set_user(username, true);
            console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            printf("\n  Press any key to continue...");
            while (!keyboard_available()) { keyboard_poll(); }
            keyboard_read();
            return;
        }

        attempts++;
    }

    console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    printf("\n  Too many failed attempts. System will reboot...\n");
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    system_reboot();
}

#endif
