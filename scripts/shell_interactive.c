#include "../include/kernel/console.h"
#include "../include/kernel/fs.h"
#include "../include/kernel/keyboard.h"
#include "../include/kernel/memory.h"
#include "../include/kernel/shell.h"
#include "../include/libc/stdio.h"

#ifdef TEST_MODE
extern void keyboard_cleanup(void);
#endif

int main(void) {
    printf("MyOS Interactive Shell Test\n");
    printf("===========================\n\n");
    
    // Initialize all components
    memory_init();
    console_init();
    keyboard_init();
    fs_init();
    shell_init();
    
    printf("Interactive shell is now running!\n");
    printf("Try commands like: help, ls, cat welcome.txt, mkdir test, etc.\n");
    printf("Type 'exit' to quit.\n\n");
    
    // Run the shell
    shell_run();
    
#ifdef TEST_MODE
    keyboard_cleanup();
#endif
    
    return 0;
}
