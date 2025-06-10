#include "../include/kernel/fs.h"
#include "../include/kernel/system.h"
#include "../include/libc/string.h"
#include "../include/libc/stdio.h"

static filesystem_t filesystem;
static uint32_t system_time = 0;

// Simple time function
static uint32_t get_time(void) {
    return ++system_time;
}

void fs_init(void) {
    memset(&filesystem, 0, sizeof(filesystem_t));
    strcpy(filesystem.current_path, "/");
    
    // Create root directory
    fs_create_file("/", 1);
    
    // Create initial directories with Linux-like structure
    fs_create_file("/bin", 1);
    fs_create_file("/home", 1);
    fs_create_file("/etc", 1);
    fs_create_file("/tmp", 1);
    fs_create_file("/usr", 1);
    fs_create_file("/var", 1);
    fs_create_file("/dev", 1);
    fs_create_file("/proc", 1);
    fs_create_file("/root", 1);
    
    // Create user directories
    fs_create_file("/home/user", 1);
    
    // Create some system files
    const char* welcome_msg = "Welcome to Alpha OS 2025!\n\nAlpha OS is a modern, lightweight operating system with:\n- Advanced file system with directory navigation\n- Enhanced shell with command history and root access\n- Memory management\n- Colorful interface with Linux-style prompts\n- Root shell functionality\n\nType 'help' for available commands.\nType 'root' to enter root shell (password: root or admin).\nType 'info' for system information.\n";
    fs_write_file("/welcome.txt", welcome_msg, strlen(welcome_msg));
    
    const char* readme = "Alpha OS File System 2025\n=========================\n\nFeatures:\n- Linux-like directory structure\n- Parent directory navigation with 'cd ..'\n- File permissions\n- Path normalization\n- Root shell access\n- Linux-style colored prompts\n\nCommands:\n- ls [-l] [path]  : List files (with -l for detailed view)\n- cd <path>       : Change directory (supports .. and absolute paths)\n- pwd             : Print working directory\n- mkdir <path>    : Create directory\n- touch <file>    : Create file\n- cat <file>      : Display file contents\n- rm <file>       : Remove file\n- root            : Enter root shell\n- exit            : Exit root shell\n- su <user>       : Switch user\n";
    fs_write_file("/readme.txt", readme, strlen(readme));
    
    const char* version = "Alpha OS v1.0\nBuild: 2025.01\nKernel: AlphaKernel\nShell: AlphaShell v2.0\nFeatures: Root Access, Enhanced Security\n";
    fs_write_file("/etc/version", version, strlen(version));
    
    const char* motd = "Welcome to Alpha OS 2025!\nA modern operating system for learning and development.\nType 'root' for administrative access.\n";
    fs_write_file("/etc/motd", motd, strlen(motd));
    
    const char* passwd = "# Alpha OS User Database 2025\nroot:x:0:0:root:/root:/bin/sh\nuser:x:1000:1000:user:/home/user:/bin/sh\n";
    fs_write_file("/etc/passwd", passwd, strlen(passwd));
    
    const char* shadow = "# Alpha OS Shadow File 2025\nroot:$1$root$encrypted:0:0:99999:7:::\nuser:$1$user$encrypted:0:0:99999:7:::\n";
    fs_write_file("/etc/shadow", shadow, strlen(shadow));
}

void fs_normalize_path(const char* path, char* normalized, size_t size) {
    char temp[FS_MAX_FILENAME_LENGTH];
    char* components[FS_MAX_PATH_DEPTH];
    int component_count = 0;
    
    // Copy input path
    strncpy(temp, path, FS_MAX_FILENAME_LENGTH - 1);
    temp[FS_MAX_FILENAME_LENGTH - 1] = '\0';
    
    // Split path into components
    char* token = strtok(temp, "/");
    while (token != NULL && component_count < FS_MAX_PATH_DEPTH) {
        if (strcmp(token, ".") == 0) {
            // Skip current directory
        } else if (strcmp(token, "..") == 0) {
            // Go to parent directory
            if (component_count > 0) {
                component_count--;
            }
        } else {
            components[component_count++] = token;
        }
        token = strtok(NULL, "/");
    }
    
    // Rebuild normalized path
    if (component_count == 0) {
        strcpy(normalized, "/");
    } else {
        normalized[0] = '\0';
        for (int i = 0; i < component_count; i++) {
            strcat(normalized, "/");
            strcat(normalized, components[i]);
        }
    }
}

void fs_get_absolute_path(const char* relative_path, const char* current_dir, char* absolute_path, size_t size) {
    if (relative_path[0] == '/') {
        // Already absolute
        fs_normalize_path(relative_path, absolute_path, size);
    } else {
        // Make it absolute
        char temp[FS_MAX_FILENAME_LENGTH];
        snprintf(temp, sizeof(temp), "%s/%s", current_dir, relative_path);
        fs_normalize_path(temp, absolute_path, size);
    }
}

int fs_get_parent_directory(const char* current_dir, char* parent_dir, size_t dir_size) {
    if (strcmp(current_dir, "/") == 0) {
        // Already at root
        strcpy(parent_dir, "/");
        return 0;
    }
    
    char temp[FS_MAX_FILENAME_LENGTH];
    strncpy(temp, current_dir, FS_MAX_FILENAME_LENGTH - 1);
    temp[FS_MAX_FILENAME_LENGTH - 1] = '\0';
    
    // Remove trailing slash if present
    size_t len = strlen(temp);
    if (len > 1 && temp[len - 1] == '/') {
        temp[len - 1] = '\0';
    }
    
    // Find last slash
    char* last_slash = strrchr(temp, '/');
    if (last_slash == NULL) {
        strcpy(parent_dir, "/");
    } else if (last_slash == temp) {
        strcpy(parent_dir, "/");
    } else {
        *last_slash = '\0';
        strncpy(parent_dir, temp, dir_size - 1);
        parent_dir[dir_size - 1] = '\0';
    }
    
    return 0;
}

const char* fs_get_filename(const char* path) {
    const char* last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        return path;
    }
    return last_slash + 1;
}

void fs_get_directory(const char* path, char* directory, size_t size) {
    const char* last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        strcpy(directory, ".");
    } else if (last_slash == path) {
        strcpy(directory, "/");
    } else {
        size_t len = last_slash - path;
        if (len >= size) len = size - 1;
        strncpy(directory, path, len);
        directory[len] = '\0';
    }
}

int fs_create_file(const char* filename, uint8_t is_directory) {
    if (filesystem.num_entries >= FS_MAX_FILES) {
        return -1; // No space for new files
    }
    
    if (fs_file_exists(filename)) {
        return -2; // File already exists
    }
    
    fs_entry_t* entry = &filesystem.entries[filesystem.num_entries];
    strncpy(entry->filename, filename, FS_MAX_FILENAME_LENGTH - 1);
    entry->filename[FS_MAX_FILENAME_LENGTH - 1] = '\0';
    entry->size = 0;
    entry->data_offset = filesystem.data_used;
    entry->is_directory = is_directory;
    entry->created_time = get_time();
    entry->modified_time = entry->created_time;
    entry->permissions = 0755; // Default permissions
    entry->parent_index = 0; // Will be set properly later
    
    filesystem.num_entries++;
    return 0;
}

int fs_write_file(const char* filename, const void* data, size_t size) {
    if (size > FS_MAX_FILE_SIZE) {
        return -1; // File too large
    }
    
    if (filesystem.data_used + size > FS_TOTAL_DATA_SIZE) {
        return -2; // Not enough space
    }
    
    // Find existing file
    for (uint32_t i = 0; i < filesystem.num_entries; i++) {
        if (strcmp(filesystem.entries[i].filename, filename) == 0) {
            if (filesystem.entries[i].is_directory) {
                return -3; // Cannot write to directory
            }
            
            // Update existing file
            filesystem.entries[i].size = size;
            filesystem.entries[i].modified_time = get_time();
            memcpy(filesystem.data + filesystem.entries[i].data_offset, data, size);
            return 0;
        }
    }
    
    // File doesn't exist, create it
    if (fs_create_file(filename, 0) != 0) {
        return -4; // Failed to create file
    }
    
    fs_entry_t* entry = &filesystem.entries[filesystem.num_entries - 1];
    entry->size = size;
    entry->data_offset = filesystem.data_used;
    entry->modified_time = get_time();
    memcpy(filesystem.data + entry->data_offset, data, size);
    filesystem.data_used += size;
    
    return 0;
}

int fs_read_file(const char* filename, void* buffer, size_t buffer_size) {
    for (uint32_t i = 0; i < filesystem.num_entries; i++) {
        if (strcmp(filesystem.entries[i].filename, filename) == 0) {
            if (filesystem.entries[i].is_directory) {
                return -1; // Cannot read directory as file
            }
            
            size_t size_to_copy = filesystem.entries[i].size;
            if (size_to_copy > buffer_size) {
                size_to_copy = buffer_size;
            }
            
            memcpy(buffer, filesystem.data + filesystem.entries[i].data_offset, size_to_copy);
            return size_to_copy;
        }
    }
    
    return -2; // File not found
}

int fs_delete_file(const char* filename) {
    for (uint32_t i = 0; i < filesystem.num_entries; i++) {
        if (strcmp(filesystem.entries[i].filename, filename) == 0) {
            // Don't allow deletion of root directory
            if (strcmp(filename, "/") == 0) {
                return -2; // Cannot delete root
            }
            
            // Simple deletion by swapping with the last entry
            if (i < filesystem.num_entries - 1) {
                filesystem.entries[i] = filesystem.entries[filesystem.num_entries - 1];
            }
            filesystem.num_entries--;
            return 0;
        }
    }
    
    return -1; // File not found
}

int fs_file_exists(const char* filename) {
    for (uint32_t i = 0; i < filesystem.num_entries; i++) {
        if (strcmp(filesystem.entries[i].filename, filename) == 0) {
            return 1;
        }
    }
    return 0;
}

size_t fs_file_size(const char* filename) {
    for (uint32_t i = 0; i < filesystem.num_entries; i++) {
        if (strcmp(filesystem.entries[i].filename, filename) == 0) {
            return filesystem.entries[i].size;
        }
    }
    return 0;
}

int fs_list_directory(const char* dirname, char* buffer, size_t buffer_size) {
    size_t offset = 0;
    char normalized_dirname[FS_MAX_FILENAME_LENGTH];
    
    // Normalize the directory name
    fs_normalize_path(dirname, normalized_dirname, sizeof(normalized_dirname));
    
    // Ensure dirname ends with '/' for comparison
    size_t dirname_len = strlen(normalized_dirname);
    if (dirname_len > 0 && normalized_dirname[dirname_len - 1] != '/') {
        if (dirname_len < FS_MAX_FILENAME_LENGTH - 1) {
            normalized_dirname[dirname_len] = '/';
            normalized_dirname[dirname_len + 1] = '\0';
            dirname_len++;
        }
    }
    
    for (uint32_t i = 0; i < filesystem.num_entries; i++) {
        const char* filename = filesystem.entries[i].filename;
        
        // Skip the directory itself
        if (strcmp(filename, normalized_dirname) == 0) {
            continue;
        }
        
        // Check if file is in the requested directory
        if (strncmp(filename, normalized_dirname, dirname_len) == 0) {
            const char* remaining = filename + dirname_len;
            
            // Check if this is a direct child (no more '/' in remaining path)
            if (strchr(remaining, '/') == NULL && strlen(remaining) > 0) {
                size_t name_len = strlen(remaining);
                
                // Check buffer space
                if (offset + name_len + 2 > buffer_size) {
                    break; // Buffer full
                }
                
                // Copy filename
                strcpy(buffer + offset, remaining);
                offset += name_len;
                
                // Add directory indicator
                if (filesystem.entries[i].is_directory) {
                    buffer[offset++] = '/';
                }
                
                buffer[offset++] = '\n';
            }
        }
    }
    
    if (offset > 0) {
        buffer[offset - 1] = '\0'; // Replace last newline with null terminator
    } else {
        buffer[0] = '\0';
    }
    
    return offset;
}

int fs_change_directory(const char* dirname, char* current_dir, size_t dir_size) {
    char new_path[FS_MAX_FILENAME_LENGTH];
    
    // Handle special cases
    if (strcmp(dirname, "..") == 0) {
        return fs_get_parent_directory(current_dir, current_dir, dir_size);
    }
    
    if (strcmp(dirname, ".") == 0) {
        return 0; // Stay in current directory
    }
    
    // Get absolute path
    fs_get_absolute_path(dirname, current_dir, new_path, sizeof(new_path));
    
    // Ensure path ends with '/' for directory check
    size_t path_len = strlen(new_path);
    if (path_len > 0 && new_path[path_len - 1] != '/') {
        if (path_len < FS_MAX_FILENAME_LENGTH - 1) {
            new_path[path_len] = '/';
            new_path[path_len + 1] = '\0';
        }
    }
    
    // Check if directory exists
    if (!fs_file_exists(new_path)) {
        return -1; // Directory not found
    }
    
    // Update current directory
    strncpy(current_dir, new_path, dir_size - 1);
    current_dir[dir_size - 1] = '\0';
    
    return 0;
}

void fs_get_stats(uint32_t* total_files, uint32_t* total_size, uint32_t* free_size) {
    if (total_files) *total_files = filesystem.num_entries;
    if (total_size) *total_size = filesystem.data_used;
    if (free_size) *free_size = FS_TOTAL_DATA_SIZE - filesystem.data_used;
}

const char* fs_get_file_type_string(const char* filename) {
    for (uint32_t i = 0; i < filesystem.num_entries; i++) {
        if (strcmp(filesystem.entries[i].filename, filename) == 0) {
            return filesystem.entries[i].is_directory ? "directory" : "file";
        }
    }
    return "unknown";
}

void fs_get_permissions_string(const char* filename, char* perms, size_t size) {
    for (uint32_t i = 0; i < filesystem.num_entries; i++) {
        if (strcmp(filesystem.entries[i].filename, filename) == 0) {
            uint32_t p = filesystem.entries[i].permissions;
            snprintf(perms, size, "%c%c%c%c%c%c%c%c%c",
                filesystem.entries[i].is_directory ? 'd' : '-',
                (p & 0400) ? 'r' : '-',
                (p & 0200) ? 'w' : '-',
                (p & 0100) ? 'x' : '-',
                (p & 0040) ? 'r' : '-',
                (p & 0020) ? 'w' : '-',
                (p & 0010) ? 'x' : '-',
                (p & 0004) ? 'r' : '-',
                (p & 0002) ? 'w' : '-'
            );
            return;
        }
    }
    strcpy(perms, "---------");
}

filesystem_t* fs_get_instance(void) {
    return &filesystem;
}
