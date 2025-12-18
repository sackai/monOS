// fs.c -- Disk-backed simple filesystem (Stage 2)
// Supports:
//   - 128 files max
//   - Single-sector files (<= 512 bytes)
//   - Directory stored on disk in 32 sectors (16 KB)
//   - Only first 4 KB used for 128 entries (safe)

#include "fs.h"
#include "fs_disk.h"
#include "ata.h"
#include "console.h"
#include "string.h"
#include <stdint.h>

#define FS_SCAN_LIMIT_SECTOR 30000

// ---- In-memory directory ----
// Each entry is 32 bytes, and FS_MAX_FILES = 128 → total 4096 bytes
static fs_disk_entry_t directory[FS_MAX_FILES];

// RAM file view returned by fs_get()
static fs_file_t ram_file_view;

// -------------------- Helpers --------------------

static void fs_zero_directory(void) {
    for (int i = 0; i < FS_MAX_FILES; ++i) {
        directory[i].name[0] = 0;
        directory[i].used = 0;
        directory[i].start_sector = 0;
        directory[i].size = 0;
        for (int j = 0; j < 7; ++j) directory[i].reserved[j] = 0;
    }
}

int find_free_directory_entry(void) {
    for (int i = 0; i < FS_MAX_FILES; ++i) {
        if (!directory[i].used || directory[i].name[0] == 0)
            return i;
    }
    return -1;
}

int find_free_sector(void) {
    uint8_t buffer[512];

    for (uint32_t sec = FS_SECTOR_DATA_START; sec < FS_SCAN_LIMIT_SECTOR; ++sec) {
        ata_read_sector(sec, buffer);

        int used = 0;
        for (int i = 0; i < 512; ++i)
            if (buffer[i] != 0) { used = 1; break; }

        if (!used) return sec;
    }
    return -1;
}

// -------------------- Directory Load / Save --------------------
// NOTE: Disk directory area = 32 sectors = 16384 bytes
// But RAM directory = only 4096 bytes (128 entries × 32 bytes)
// So we load/save ONLY the first 4096 bytes!

/*void fs_load_directory(void) {
    uint8_t buffer[512];

    int bytes_needed = FS_MAX_FILES * sizeof(fs_disk_entry_t); // 4096 bytes
    int bytes_loaded = 0;

    for (int i = 0; i < FS_SECTOR_DIR_COUNT && bytes_loaded < bytes_needed; ++i) {

        ata_read_sector(FS_SECTOR_DIR_START + i, buffer);

        int chunk = bytes_needed - bytes_loaded;
        if (chunk > 512) chunk = 512;

        memcpy(((uint8_t*)directory) + bytes_loaded, buffer, chunk);

        bytes_loaded += chunk;
    }

    // Detect fresh disk (all zero)
    int any_used = 0;
    for (int i = 0; i < FS_MAX_FILES; ++i) {
        if (directory[i].used || directory[i].name[0]) {
            any_used = 1;
            break;
        }
    }

    if (!any_used) {
        fs_zero_directory();
        fs_save_directory();   // write empty directory to disk
    }
}*/
void fs_load_directory(void) {
    uint8_t buffer[512];

    int bytes_needed = FS_MAX_FILES * sizeof(fs_disk_entry_t); // 4096
    int bytes_loaded = 0;

    for (int i = 0; i < FS_SECTOR_DIR_COUNT && bytes_loaded < bytes_needed; ++i) {

        ata_read_sector(FS_SECTOR_DIR_START + i, buffer);

        int chunk = 512;
        if (bytes_loaded + chunk > bytes_needed)
            chunk = bytes_needed - bytes_loaded;

        memcpy(((uint8_t*)directory) + bytes_loaded, buffer, chunk);

        bytes_loaded += chunk;
    }

    // check if directory empty → format it
    int used = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (directory[i].used)
            used = 1;
    }

    if (!used) {
        memset(directory, 0, bytes_needed);
        fs_save_directory();
    }
}





/*void fs_save_directory(void) {
    uint8_t buffer[512];

    int bytes_needed = FS_MAX_FILES * sizeof(fs_disk_entry_t); // 4096 bytes
    int bytes_saved = 0;

    for (int i = 0; i < FS_SECTOR_DIR_COUNT && bytes_saved < bytes_needed; ++i) {

        int chunk = bytes_needed - bytes_saved;
        if (chunk > 512) chunk = 512;

        memset(buffer, 0, 512);
        memcpy(buffer, ((uint8_t*)directory) + bytes_saved, chunk);

        ata_write_sector(FS_SECTOR_DIR_START + i, buffer);

        bytes_saved += chunk;
    }
}*/


void fs_save_directory(void) {
    uint8_t buffer[512];

    int bytes_needed = FS_MAX_FILES * sizeof(fs_disk_entry_t);  // 4096
    int bytes_saved = 0;

    for (int i = 0; i < FS_SECTOR_DIR_COUNT && bytes_saved < bytes_needed; ++i) {

        int chunk = 512;
        if (bytes_saved + chunk > bytes_needed)
            chunk = bytes_needed - bytes_saved;

        memcpy(buffer, ((uint8_t*)directory) + bytes_saved, chunk);

        ata_write_sector(FS_SECTOR_DIR_START + i, buffer);

        bytes_saved += chunk;
    }
}



// -------------------- Public FS API --------------------

void fs_init(void) {
    fs_load_directory();
}

void fs_list(void) {
    kprint("Files:\n");
    char tmp[32];

    for (int i = 0; i < FS_MAX_FILES; ++i) {
        if (directory[i].used && directory[i].name[0]) {

            kprint("  ");
            kprint(directory[i].name);
            kprint(" (");

            extern void itoa(int, char*);
            itoa((int)directory[i].size, tmp);
            kprint(tmp);
            kprint(" bytes)\n");
        }
    }
}

int fs_write(const char* name, const char* text) {
    if (!name || !name[0] || !text) return 0;
    if (strlen(name) >= FS_FILENAME_LEN) return 0;

    // 1. Find existing or free slot
    int idx = -1;
    for (int i = 0; i < FS_MAX_FILES; ++i) {
        if (directory[i].used && strcmp(directory[i].name, name) == 0) {
            idx = i;
            break;
        }
    }
    if (idx < 0) {
        idx = find_free_directory_entry();
        if (idx < 0) return 0;
    }

    // 2. Prepare buffer
    uint8_t buf[512];
    memset(buf, 0, 512);

    int len = strlen(text);
    if (len > 511) len = 511;
    strcpy((char*)buf, text);

    // 3. Find a free data sector
    int sector = find_free_sector();
    if (sector < 0) return 0;

    // 4. Write sector
    if (!ata_write_sector((uint32_t)sector, buf)) return 0;

    // 5. Update directory entry
    directory[idx].used = 1;
    memset(directory[idx].name, 0, FS_FILENAME_LEN);
    strcpy(directory[idx].name, name);
    directory[idx].start_sector = (uint32_t)sector;
    directory[idx].size = (uint32_t)len;

    // 6. Persist directory
    fs_save_directory();
    return 1;
}

fs_file_t* fs_get(const char* name) {
    if (!name) return 0;

    int idx = -1;
    for (int i = 0; i < FS_MAX_FILES; ++i) {
        if (directory[i].used && strcmp(directory[i].name, name) == 0) {
            idx = i;
            break;
        }
    }
    if (idx < 0) return 0;

    uint8_t buf[512];
    ata_read_sector(directory[idx].start_sector, buf);

    // Build RAM view
    ram_file_view.used = 1;
    ram_file_view.size = directory[idx].size;

    memset(ram_file_view.name, 0, FS_MAX_NAME);
    strcpy(ram_file_view.name, directory[idx].name);

    int copy_len = (ram_file_view.size < FS_MAX_DATA - 1)
                   ? ram_file_view.size : (FS_MAX_DATA - 1);

    for (int i = 0; i < copy_len; ++i)
        ram_file_view.data[i] = (char)buf[i];
    ram_file_view.data[copy_len] = 0;

    return &ram_file_view;
}

int fs_delete(const char* name) {
    if (!name) return 0;

    int idx = -1;
    for (int i = 0; i < FS_MAX_FILES; ++i) {
        if (directory[i].used && strcmp(directory[i].name, name) == 0) {
            idx = i;
            break;
        }
    }
    if (idx < 0) return 0;

    // Zero data sector
    uint8_t zero[512];
    memset(zero, 0, 512);

    ata_write_sector(directory[idx].start_sector, zero);

    // Clear directory entry
    directory[idx].used = 0;
    directory[idx].name[0] = 0;
    directory[idx].size = 0;
    directory[idx].start_sector = 0;

    fs_save_directory();
    return 1;
}
