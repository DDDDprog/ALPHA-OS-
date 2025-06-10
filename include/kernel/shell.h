#ifndef SHELL_H
#define SHELL_H

#include "types.h"

#define SHELL_MAX_COMMAND_LENGTH 512
#define SHELL_MAX_ARGS 32
#define SHELL_HISTORY_SIZE 50
#define SHELL_MAX_PATH_LENGTH 512
#define SHELL_PROMPT_LENGTH 256

// Command handler function type
typedef void (*command_handler_t)(int argc, char* argv[]);

// Shell command structure
typedef struct {
    char* name;
    command_handler_t handler;
    char* help;
    char* usage;
} shell_command_t;

// Initialize the shell
void shell_init(void);

// Process a command
void shell_process_command(const char* command);

// Main shell loop
void shell_run(void);

// Register a command
void shell_register_command(const char* name, command_handler_t handler, const char* help, const char* usage);

// Print shell prompt
void shell_print_prompt(void);

// Get current directory
const char* shell_get_current_dir(void);

// Set current directory
void shell_set_current_dir(const char* dir);

// Command history functions
void shell_add_to_history(const char* command);
const char* shell_get_history(int index);
int shell_get_history_count(void);

// Auto-completion
void shell_autocomplete(char* buffer, size_t buffer_size);

// Enhanced prompt with colors and info
void shell_print_enhanced_prompt(void);

#endif // SHELL_H
