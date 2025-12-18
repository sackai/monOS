#ifndef FS_H
#define FS_H

#include <stdint.h>

#define FS_MAX_FILES   128
#define FS_MAX_NAME    16
#define FS_MAX_DATA    512

typedef struct {
    char     name[FS_MAX_NAME];
    uint32_t size;
    int      used;
    char     data[FS_MAX_DATA];
} fs_file_t;

// Initialize FS
void fs_init(void);

// Create empty file (fails if exists or full)
int fs_create(const char* name);

// Write (overwrite) file contents
int fs_write(const char* name, const char* data);

// Get pointer to file struct (or NULL)
fs_file_t* fs_get(const char* name);

// Delete file
int fs_delete(const char* name);

// List all files with sizes
void fs_list(void);
void fs_load_directory(void);
void fs_save_directory(void);


#endif
