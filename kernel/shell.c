#include "../include/kernel/shell.h"
#include "../include/kernel/fs.h"
#include "../include/kernel/keyboard.h"
#include "../include/kernel/console.h"
#include "../include/kernel/memory.h"
#include "../include/libc/stdio.h"
#include "../include/libc/stdlib.h"
#include "../include/libc/string.h"

#define MAX_COMMANDS 64

static shell_command_t commands[MAX_COMMANDS];
static int num_commands = 0;
static char current_dir[SHELL_MAX_PATH_LENGTH] = "/";
static char command_history[SHELL_HISTORY_SIZE][SHELL_MAX_COMMAND_LENGTH];
static int history_count = 0;
static char username[32] = "user";
static char hostname[32] = "alphaos";
static bool is_root = false;

// Forward declarations of built-in commands
static void cmd_help(int argc, char* argv[]);
static void cmd_ls(int argc, char* argv[]);
static void cmd_cd(int argc, char* argv[]);
static void cmd_cat(int argc, char* argv[]);
static void cmd_echo(int argc, char* argv[]);
static void cmd_mkdir(int argc, char* argv[]);
static void cmd_touch(int argc, char* argv[]);
static void cmd_rm(int argc, char* argv[]);
static void cmd_clear(int argc, char* argv[]);
static void cmd_pwd(int argc, char* argv[]);
static void cmd_stat(int argc, char* argv[]);
static void cmd_mem(int argc, char* argv[]);
static void cmd_history(int argc, char* argv[]);
static void cmd_tree(int argc, char* argv[]);
static void cmd_info(int argc, char* argv[]);
static void cmd_whoami(int argc, char* argv[]);
static void cmd_hostname(int argc, char* argv[]);
static void cmd_uptime(int argc, char* argv[]);
static void cmd_calc(int argc, char* argv[]);
static void cmd_date(int argc, char* argv[]);
static void cmd_banner(int argc, char* argv[]);
static void cmd_su(int argc, char* argv[]);
static void cmd_root_shell(int argc, char* argv[]);
static void cmd_exit_root(int argc, char* argv[]);

void shell_init(void) {
    // Register built-in commands with usage information
    shell_register_command("help", cmd_help, "Display help information", "help [command]");
    shell_register_command("ls", cmd_ls, "List directory contents", "ls [-l] [directory]");
    shell_register_command("cd", cmd_cd, "Change directory", "cd <directory>");
    shell_register_command("cat", cmd_cat, "Display file contents", "cat <filename>");
    shell_register_command("echo", cmd_echo, "Display a line of text", "echo <text>");
    shell_register_command("mkdir", cmd_mkdir, "Create a directory", "mkdir <directory>");
    shell_register_command("touch", cmd_touch, "Create a file", "touch <filename>");
    shell_register_command("rm", cmd_rm, "Remove a file or directory", "rm <filename>");
    shell_register_command("clear", cmd_clear, "Clear the screen", "clear");
    shell_register_command("pwd", cmd_pwd, "Print working directory", "pwd");
    shell_register_command("stat", cmd_stat, "Display file system statistics", "stat");
    shell_register_command("mem", cmd_mem, "Display memory statistics", "mem");
    shell_register_command("history", cmd_history, "Show command history", "history");
    shell_register_command("tree", cmd_tree, "Show directory tree", "tree [directory]");
    shell_register_command("info", cmd_info, "Show system information", "info");
    shell_register_command("whoami", cmd_whoami, "Display current user", "whoami");
    shell_register_command("hostname", cmd_hostname, "Display or set hostname", "hostname [name]");
    shell_register_command("uptime", cmd_uptime, "Show system uptime", "uptime");
    shell_register_command("calc", cmd_calc, "Simple calculator", "calc <expression>");
    shell_register_command("date", cmd_date, "Show current date/time", "date");
    shell_register_command("banner", cmd_banner, "Display Alpha OS banner", "banner");
    shell_register_command("su", cmd_su, "Switch user", "su [username]");
    shell_register_command("root", cmd_root_shell, "Enter root shell", "root");
    shell_register_command("exit", cmd_exit_root, "Exit root shell", "exit");
    
    // Display welcome banner
    cmd_banner(0, NULL);
    
    console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("Alpha Shell v2.0 initialized\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Type 'help' for available commands or 'info' for system information\n");
    printf("Type 'root' to enter root shell\n");
}

void shell_register_command(const char* name, command_handler_t handler, const char* help, const char* usage) {
    if (num_commands >= MAX_COMMANDS) {
        printf("Error: Maximum number of commands reached\n");
        return;
    }
    
    commands[num_commands].name = strdup(name);
    commands[num_commands].handler = handler;
    commands[num_commands].help = strdup(help);
    commands[num_commands].usage = strdup(usage);
    num_commands++;
}

void shell_print_enhanced_prompt(void) {
    // Get short directory name
    const char* short_dir = current_dir;
    if (strlen(current_dir) > 20) {
        short_dir = "...";
        short_dir = current_dir + strlen(current_dir) - 17;
    }
    
    // Color-coded prompt: user@hostname:dir$
    if (is_root) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);  // Red for root like Linux
    } else {
        console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    }
    printf("%s", username);
    
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    printf("@");
    console_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printf("%s", hostname);
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    printf(":");
    console_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    printf("%s", short_dir);
    console_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    // Different prompt for root vs user - Linux style
    if (is_root) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("# ");
    } else {
        console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        printf("$ ");
    }
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void shell_print_prompt(void) {
    shell_print_enhanced_prompt();
}

const char* shell_get_current_dir(void) {
    return current_dir;
}

void shell_set_current_dir(const char* dir) {
    strncpy(current_dir, dir, SHELL_MAX_PATH_LENGTH - 1);
    current_dir[SHELL_MAX_PATH_LENGTH - 1] = '\0';
}

void shell_add_to_history(const char* command) {
    if (strlen(command) > 0) {
        strncpy(command_history[history_count % SHELL_HISTORY_SIZE], 
                command, 
                SHELL_MAX_COMMAND_LENGTH - 1);
        command_history[history_count % SHELL_HISTORY_SIZE][SHELL_MAX_COMMAND_LENGTH - 1] = '\0';
        history_count++;
    }
}

const char* shell_get_history(int index) {
    if (index >= 0 && index < SHELL_HISTORY_SIZE && index < history_count) {
        return command_history[index];
    }
    return NULL;
}

int shell_get_history_count(void) {
    return history_count < SHELL_HISTORY_SIZE ? history_count : SHELL_HISTORY_SIZE;
}

void shell_process_command(const char* command) {
    // Add to history
    shell_add_to_history(command);
    
    // Parse command and arguments
    char cmd_copy[SHELL_MAX_COMMAND_LENGTH];
    strncpy(cmd_copy, command, SHELL_MAX_COMMAND_LENGTH - 1);
    cmd_copy[SHELL_MAX_COMMAND_LENGTH - 1] = '\0';
    
    char* argv[SHELL_MAX_ARGS];
    int argc = 0;
    
    // Simple tokenization
    char* token = strtok(cmd_copy, " \t\n");
    while (token != NULL && argc < SHELL_MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    
    if (argc == 0) {
        return; // Empty command
    }
    
    // Find and execute command
    for (int i = 0; i < num_commands; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].handler(argc, argv);
            return;
        }
    }
    
    console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    printf("alphaos: command not found: %s\n", argv[0]);
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Type 'help' for a list of available commands\n");
}

void shell_run(void) {
    char command[SHELL_MAX_COMMAND_LENGTH];
    
    while (1) {
        shell_print_prompt();
        
        // Get command from user
        int len = keyboard_get_line(command, SHELL_MAX_COMMAND_LENGTH);
        
        if (len > 0) {
            shell_process_command(command);
        }
    }
}

// Enhanced command implementations

static void cmd_banner(int argc, char* argv[]) {
    console_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printf("    _    _       _             ___  ____  \n");
    printf("   / \\  | |_ __ | |__   __ _  / _ \\/ ___| \n");
    printf("  / _ \\ | | '_ \\| '_ \\ / _` || | | \\___ \\ \n");
    printf(" / ___ \\| | |_) | | | | (_| || |_| |___) |\n");
    printf("/_/   \\_\\_| .__/|_| |_|\\__,_| \\___/|____/ \n");
    printf("          |_|                            \n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Alpha Operating System - Advanced Learning Platform\n");
    printf("Version 1.0 | Built with passion for education | 2025\n\n");
}

static void cmd_help(int argc, char* argv[]) {
    if (argc > 1) {
        // Show help for specific command
        for (int i = 0; i < num_commands; i++) {
            if (strcmp(argv[1], commands[i].name) == 0) {
                console_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
                printf("Command: %s\n", commands[i].name);
                console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
                printf("Description: %s\n", commands[i].help);
                printf("Usage: %s\n", commands[i].usage);
                return;
            }
        }
        printf("Unknown command: %s\n", argv[1]);
        return;
    }
    
    console_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    printf("Alpha OS Shell Commands\n");
    printf("=======================\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    printf("\nFile System Commands:\n");
    printf("  %-12s - %s\n", "ls", "List directory contents");
    printf("  %-12s - %s\n", "cd", "Change directory (supports .. for parent)");
    printf("  %-12s - %s\n", "pwd", "Print working directory");
    printf("  %-12s - %s\n", "mkdir", "Create directory");
    printf("  %-12s - %s\n", "touch", "Create file");
    printf("  %-12s - %s\n", "cat", "Display file contents");
    printf("  %-12s - %s\n", "rm", "Remove file or directory");
    printf("  %-12s - %s\n", "tree", "Show directory tree");
    
    printf("\nSystem Commands:\n");
    printf("  %-12s - %s\n", "clear", "Clear screen");
    printf("  %-12s - %s\n", "info", "Show system information");
    printf("  %-12s - %s\n", "stat", "File system statistics");
    printf("  %-12s - %s\n", "mem", "Memory statistics");
    printf("  %-12s - %s\n", "uptime", "System uptime");
    printf("  %-12s - %s\n", "date", "Current date/time");
    
    printf("\nUser Management:\n");
    printf("  %-12s - %s\n", "whoami", "Current user");
    printf("  %-12s - %s\n", "su", "Switch user");
    printf("  %-12s - %s\n", "root", "Enter root shell");
    printf("  %-12s - %s\n", "exit", "Exit root shell");
    
    printf("\nUtility Commands:\n");
    printf("  %-12s - %s\n", "echo", "Display text");
    printf("  %-12s - %s\n", "calc", "Simple calculator");
    printf("  %-12s - %s\n", "history", "Command history");
    printf("  %-12s - %s\n", "hostname", "System hostname");
    printf("  %-12s - %s\n", "banner", "Display Alpha OS banner");
    printf("  %-12s - %s\n", "help", "This help message");
    
    printf("\nTip: Use 'help <command>' for detailed information about a command\n");
}

static void cmd_root_shell(int argc, char* argv[]) {
    if (is_root) {
        console_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
        printf("Already in root shell\n");
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return;
    }
    
    console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    printf("Entering root shell...\n");
    printf("Password: ");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    char password[64];
    keyboard_get_line(password, sizeof(password));
    
    if (strcmp(password, "root") == 0 || strcmp(password, "admin") == 0) {
        is_root = true;
        strcpy(username, "root");
        strcpy(current_dir, "/root");
        
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("\n*** ROOT SHELL ACTIVATED ***\n");
        printf("You now have administrative privileges.\n");
        printf("Type 'exit' to return to user shell.\n");
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    } else {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("\nAccess denied. Incorrect password.\n");
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
}

static void cmd_exit_root(int argc, char* argv[]) {
    if (!is_root) {
        printf("Not in root shell\n");
        return;
    }
    
    is_root = false;
    strcpy(username, "user");
    strcpy(current_dir, "/home/user");
    
    console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("Exited root shell. Returned to user mode.\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

static void cmd_su(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: su [username]\n");
        printf("Available users: root, user\n");
        return;
    }
    
    if (strcmp(argv[1], "root") == 0) {
        cmd_root_shell(0, NULL);
    } else if (strcmp(argv[1], "user") == 0) {
        if (is_root) {
            cmd_exit_root(0, NULL);
        } else {
            printf("Already user\n");
        }
    } else {
        printf("Unknown user: %s\n", argv[1]);
        printf("Available users: root, user\n");
    }
}

static void cmd_ls(int argc, char* argv[]) {
    bool detailed = false;
    char* dir = current_dir;
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            detailed = true;
        } else {
            dir = argv[i];
        }
    }
    
    char absolute_path[FS_MAX_FILENAME_LENGTH];
    fs_get_absolute_path(dir, current_dir, absolute_path, sizeof(absolute_path));
    
    char buffer[FS_MAX_FILES * FS_MAX_FILENAME_LENGTH];
    int result = fs_list_directory(absolute_path, buffer, sizeof(buffer));
    
    if (result < 0) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("ls: cannot access '%s': No such directory\n", dir);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return;
    }
    
    if (buffer[0] == '\0') {
        printf("Directory is empty\n");
        return;
    }
    
    if (detailed) {
        printf("total files in %s:\n", absolute_path);
        // Parse buffer and show detailed info
        char* line = strtok(buffer, "\n");
        while (line != NULL) {
            char full_path[FS_MAX_FILENAME_LENGTH];
            snprintf(full_path, sizeof(full_path), "%s/%s", absolute_path, line);
            fs_normalize_path(full_path, full_path, sizeof(full_path));
            
            char perms[16];
            fs_get_permissions_string(full_path, perms, sizeof(perms));
            
            size_t size = fs_file_size(full_path);
            const char* type = fs_get_file_type_string(full_path);
            
            if (strstr(line, "/")) {
                console_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            } else {
                console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            }
            
            printf("%s %8zu %s\n", perms, size, line);
            line = strtok(NULL, "\n");
        }
    } else {
        // Simple listing with colors
        char* line = strtok(buffer, "\n");
        int count = 0;
        while (line != NULL) {
            if (strstr(line, "/")) {
                console_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            } else {
                console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            }
            
            printf("%-20s", line);
            count++;
            if (count % 4 == 0) printf("\n");
            
            line = strtok(NULL, "\n");
        }
        if (count % 4 != 0) printf("\n");
    }
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

static void cmd_cd(int argc, char* argv[]) {
    if (argc < 2) {
        // Go to appropriate home directory
        if (is_root) {
            strcpy(current_dir, "/root");
        } else {
            strcpy(current_dir, "/home/user");
        }
        return;
    }
    
    if (fs_change_directory(argv[1], current_dir, SHELL_MAX_PATH_LENGTH) != 0) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("cd: %s: No such directory\n", argv[1]);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
}

static void cmd_cat(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: cat <filename>\n");
        return;
    }
    
    char path[FS_MAX_FILENAME_LENGTH];
    fs_get_absolute_path(argv[1], current_dir, path, sizeof(path));
    
    char buffer[FS_MAX_FILE_SIZE];
    int bytes_read = fs_read_file(path, buffer, sizeof(buffer) - 1);
    
    if (bytes_read < 0) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("cat: %s: No such file or directory\n", argv[1]);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return;
    }
    
    buffer[bytes_read] = '\0';
    printf("%s", buffer);
    
    if (bytes_read > 0 && buffer[bytes_read - 1] != '\n') {
        printf("\n");
    }
}

static void cmd_echo(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

static void cmd_mkdir(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: mkdir <directory>\n");
        return;
    }
    
    char path[FS_MAX_FILENAME_LENGTH];
    fs_get_absolute_path(argv[1], current_dir, path, sizeof(path));
    
    // Ensure path ends with '/'
    size_t path_len = strlen(path);
    if (path_len > 0 && path[path_len - 1] != '/') {
        if (path_len < FS_MAX_FILENAME_LENGTH - 1) {
            path[path_len] = '/';
            path[path_len + 1] = '\0';
        }
    }
    
    if (fs_create_file(path, 1) != 0) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("mkdir: cannot create directory '%s'\n", argv[1]);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    } else {
        console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        printf("Directory '%s' created successfully\n", argv[1]);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
}

static void cmd_touch(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: touch <filename>\n");
        return;
    }
    
    char path[FS_MAX_FILENAME_LENGTH];
    fs_get_absolute_path(argv[1], current_dir, path, sizeof(path));
    
    if (fs_create_file(path, 0) != 0) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("touch: cannot create file '%s'\n", argv[1]);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    } else {
        console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        printf("File '%s' created successfully\n", argv[1]);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
}

static void cmd_rm(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: rm <filename>\n");
        return;
    }
    
    char path[FS_MAX_FILENAME_LENGTH];
    fs_get_absolute_path(argv[1], current_dir, path, sizeof(path));
    
    if (fs_delete_file(path) != 0) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf("rm: cannot remove '%s': No such file or directory\n", argv[1]);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    } else {
        console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        printf("'%s' removed successfully\n", argv[1]);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
}

static void cmd_clear(int argc, char* argv[]) {
    console_clear();
}

static void cmd_pwd(int argc, char* argv[]) {
    console_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    printf("%s\n", current_dir);
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

static void cmd_stat(int argc, char* argv[]) {
    uint32_t total_files, total_size, free_size;
    fs_get_stats(&total_files, &total_size, &free_size);
    
    console_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    printf("Alpha OS File System Statistics\n");
    printf("===============================\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Total files:     %u\n", total_files);
    printf("Used space:      %u bytes (%u KB)\n", total_size, total_size / 1024);
    printf("Free space:      %u bytes (%u KB)\n", free_size, free_size / 1024);
    printf("Total capacity:  %u bytes (%u KB)\n", total_size + free_size, (total_size + free_size) / 1024);
    printf("Usage:           %.1f%%\n", (float)total_size / (total_size + free_size) * 100);
}

static void cmd_mem(int argc, char* argv[]) {
    size_t total, used, free;
    memory_get_stats(&total, &used, &free);
    
    console_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    printf("Alpha OS Memory Statistics\n");
    printf("==========================\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("Total memory:    %zu bytes (%zu KB)\n", total, total / 1024);
    printf("Used memory:     %zu bytes (%zu KB)\n", used, used / 1024);
    printf("Free memory:     %zu bytes (%zu KB)\n", free, free / 1024);
    printf("Usage:           %.1f%%\n", (float)used / total * 100);
    printf("Heap integrity:  %s\n", memory_check_integrity() ? "OK" : "CORRUPTED");
}

static void cmd_history(int argc, char* argv[]) {
    int count = shell_get_history_count();
    
    console_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    printf("Command History\n");
    printf("===============\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    if (count == 0) {
        printf("No commands in history\n");
        return;
    }
    
    int start = (history_count > SHELL_HISTORY_SIZE) ? 
                (history_count - SHELL_HISTORY_SIZE) : 0;
    
    for (int i = 0; i < count; i++) {
        int index = (start + i) % SHELL_HISTORY_SIZE;
        const char* cmd = shell_get_history(index);
        if (cmd) {
            printf("%3d  %s\n", start + i + 1, cmd);
        }
    }
}

static void cmd_tree(int argc, char* argv[]) {
    printf("Directory tree feature coming soon!\n");
    printf("Use 'ls' to list directory contents for now.\n");
}

static void cmd_info(int argc, char* argv[]) {
    console_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printf("Alpha OS System Information\n");
    printf("===========================\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printf("OS Name:         Alpha OS\n");
    printf("Version:         1.0.0\n");
    printf("Kernel:          AlphaKernel\n");
    printf("Shell:           AlphaShell v2.0\n");
    printf("Architecture:    x86 (32-bit)\n");
    printf("Compiler:        GCC\n");
    printf("Build Date:      2025\n");
    printf("Features:        File System, Memory Management, Shell\n");
    
    size_t total_mem, used_mem, free_mem;
    memory_get_stats(&total_mem, &used_mem, &free_mem);
    printf("Memory:          %zu KB total, %zu KB free\n", total_mem / 1024, free_mem / 1024);
    
    uint32_t total_files, total_size, free_size;
    fs_get_stats(&total_files, &total_size, &free_size);
    printf("File System:     %u files, %u KB used\n", total_files, total_size / 1024);
    
    printf("Current User:    %s", username);
    if (is_root) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printf(" (ROOT)");
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
    printf("\n");
}

static void cmd_whoami(int argc, char* argv[]) {
    if (is_root) {
        console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    } else {
        console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    }
    printf("%s", username);
    if (is_root) {
        printf(" (root privileges)");
    }
    printf("\n");
    console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

static void cmd_hostname(int argc, char* argv[]) {
    if (argc > 1) {
        if (!is_root) {
            console_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            printf("Permission denied: Only root can change hostname\n");
            console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            return;
        }
        
        strncpy(hostname, argv[1], sizeof(hostname) - 1);
        hostname[sizeof(hostname) - 1] = '\0';
        console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        printf("Hostname set to: %s\n", hostname);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    } else {
        console_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        printf("%s\n", hostname);
        console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
}

static void cmd_uptime(int argc, char* argv[]) {
    uint32_t uptime = system_get_uptime();
    printf("System uptime: %u ticks\n", uptime);
}

static void cmd_calc(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: calc <expression>\n");
        printf("Examples:\n");
        printf("  calc 2 + 3\n");
        printf("  calc 10 - 4\n");
        printf("  calc 6 * 7\n");
        printf("  calc 15 / 3\n");
        return;
    }
    
    if (argc >= 4) {
        int a = atoi(argv[1]);
        char op = argv[2][0];
        int b = atoi(argv[3]);
        int result = 0;
        bool valid = true;
        
        switch (op) {
            case '+':
                result = a + b;
                break;
            case '-':
                result = a - b;
                break;
            case '*':
                result = a * b;
                break;
            case '/':
                if (b != 0) {
                    result = a / b;
                } else {
                    printf("Error: Division by zero\n");
                    valid = false;
                }
                break;
            default:
                printf("Error: Unknown operator '%c'\n", op);
                valid = false;
        }
        
        if (valid) {
            console_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            printf("%d %c %d = %d\n", a, op, b, result);
            console_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    } else {
        printf("Invalid expression. Use: calc <num1> <op> <num2>\n");
    }
}

static void cmd_date(int argc, char* argv[]) {
    uint32_t time = system_get_uptime();
    printf("System time: %u ticks since boot\n", time);
    printf("Build year: 2025\n");
    printf("Note: Real-time clock not implemented yet\n");
}
