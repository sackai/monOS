#ifndef FS_DISK_H
#define FS_DISK_H

#include <stdint.h>

// ---- Sector layout ----
// LBA 2  = Superblock
// LBA 3–34 = Directory table (32 sectors = 16 KB)
// LBA 35+ = File storage area

#define FS_SECTOR_SUPERBLOCK 2
#define FS_SECTOR_DIR_START  3
#define FS_SECTOR_DIR_COUNT  8
#define FS_SECTOR_DATA_START (FS_SECTOR_DIR_START + FS_SECTOR_DIR_COUNT)

#define FS_MAX_FILES 128
#define FS_FILENAME_LEN 16

// Directory entry stored ON DISK (must be 32 bytes)
typedef struct {
    char name[FS_FILENAME_LEN];  // file name
    uint32_t start_sector;       // where file data begins
    uint32_t size;               // file size in bytes
    uint8_t used;                // 1 = file exists
    uint8_t reserved[7];         // padding to make entry 32 bytes
} fs_disk_entry_t;

#endif
