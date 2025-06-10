#ifndef FS_H
#define FS_H

#include "types.h"

#define FS_MAX_FILENAME_LENGTH 128
#define FS_MAX_FILES 256
#define FS_MAX_FILE_SIZE 16384
#define FS_TOTAL_DATA_SIZE (FS_MAX_FILES * FS_MAX_FILE_SIZE)
#define FS_MAX_PATH_DEPTH 32

typedef struct {
    char filename[FS_MAX_FILENAME_LENGTH];
    uint32_t size;
    uint32_t data_offset;
    uint8_t is_directory;
    uint32_t created_time;
    uint32_t modified_time;
    uint32_t permissions;  // rwx permissions
    uint32_t parent_index; // Index of parent directory
} fs_entry_t;

typedef struct {
    fs_entry_t entries[FS_MAX_FILES];
    uint32_t num_entries;
    uint8_t data[FS_TOTAL_DATA_SIZE];
    uint32_t data_used;
    char current_path[FS_MAX_FILENAME_LENGTH];
} filesystem_t;

// Initialize the file system
void fs_init(void);

// File operations
int fs_create_file(const char* filename, uint8_t is_directory);
int fs_write_file(const char* filename, const void* data, size_t size);
int fs_read_file(const char* filename, void* buffer, size_t buffer_size);
int fs_delete_file(const char* filename);
int fs_file_exists(const char* filename);
size_t fs_file_size(const char* filename);

// Directory operations
int fs_list_directory(const char* dirname, char* buffer, size_t buffer_size);
int fs_change_directory(const char* dirname, char* current_dir, size_t dir_size);
int fs_get_parent_directory(const char* current_dir, char* parent_dir, size_t dir_size);
int fs_create_directory_tree(const char* path);

// Path utilities
void fs_normalize_path(const char* path, char* normalized, size_t size);
void fs_get_absolute_path(const char* relative_path, const char* current_dir, char* absolute_path, size_t size);
const char* fs_get_filename(const char* path);
void fs_get_directory(const char* path, char* directory, size_t size);

// Utility functions
void fs_get_stats(uint32_t* total_files, uint32_t* total_size, uint32_t* free_size);
const char* fs_get_file_type_string(const char* filename);
void fs_get_permissions_string(const char* filename, char* perms, size_t size);

// Get the global filesystem instance
filesystem_t* fs_get_instance(void);

#endif // FS_H
