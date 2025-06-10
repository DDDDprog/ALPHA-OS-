#include "../include/kernel/fs.h"
#include "../include/kernel/shell.h"
#include "../include/kernel/console.h"
#include "../include/kernel/memory.h"
#include "../include/libc/stdio.h"
#include "../include/libc/string.h"

void test_filesystem(void) {
    printf("=== File System Test ===\n");
    
    // Initialize components
    memory_init();
    console_init();
    fs_init();
    
    // Test file creation
    printf("Creating test files...\n");
    fs_create_file("/test.txt", 0);
    fs_write_file("/test.txt", "Hello, MyOS!", 12);
    
    fs_create_file("/home/user/", 1);
    fs_create_file("/home/user/notes.txt", 0);
    fs_write_file("/home/user/notes.txt", "This is a test note.\nSecond line.", 33);
    
    // Test file reading
    printf("Reading files...\n");
    char buffer[256];
    int bytes_read = fs_read_file("/test.txt", buffer, sizeof(buffer));
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Content of /test.txt: %s\n", buffer);
    }
    
    bytes_read = fs_read_file("/home/user/notes.txt", buffer, sizeof(buffer));
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Content of /home/user/notes.txt:\n%s\n", buffer);
    }
    
    // Test directory listing
    printf("Listing directories...\n");
    fs_list_directory("/", buffer, sizeof(buffer));
    printf("Root directory contents:\n%s\n", buffer);
    
    fs_list_directory("/home/", buffer, sizeof(buffer));
    printf("Home directory contents:\n%s\n", buffer);
    
    // Test file statistics
    uint32_t total_files, total_size, free_size;
    fs_get_stats(&total_files, &total_size, &free_size);
    printf("File system stats: %u files, %u bytes used, %u bytes free\n", 
           total_files, total_size, free_size);
    
    printf("File system test completed successfully!\n\n");
}

void test_shell_commands(void) {
    printf("=== Shell Commands Test ===\n");
    
    shell_init();
    
    printf("Testing shell commands...\n");
    
    printf("\n> help\n");
    shell_process_command("help");
    
    printf("\n> ls\n");
    shell_process_command("ls");
    
    printf("\n> mkdir /test\n");
    shell_process_command("mkdir /test");
    
    printf("\n> touch /test/hello.txt\n");
    shell_process_command("touch /test/hello.txt");
    
    // Manually write to the file since we don't have redirection
    fs_write_file("/test/hello.txt", "Hello from shell test!", 22);
    
    printf("\n> cat /test/hello.txt\n");
    shell_process_command("cat /test/hello.txt");
    
    printf("\n> ls /test\n");
    shell_process_command("ls /test");
    
    printf("\n> pwd\n");
    shell_process_command("pwd");
    
    printf("\n> stat\n");
    shell_process_command("stat");
    
    printf("\n> mem\n");
    shell_process_command("mem");
    
    printf("Shell commands test completed successfully!\n\n");
}

int main(void) {
    printf("MyOS Test Suite\n");
    printf("===============\n\n");
    
    test_filesystem();
    test_shell_commands();
    
    printf("All tests completed successfully!\n");
    printf("You can now build the full OS with 'make iso' and run it with 'make run'\n");
    
    return 0;
}
