#include <stdint.h>
#include "fs.h"
#include "ata.h"

#define FS_MAGIC          0x4C465331   // "LFS1"
#define FS_HEADER_SECTOR  0
#define FS_TABLE_START    1
#define FS_TABLE_SECTORS  32
#define FS_MAX_FILES      32
#define FS_DATA_START     (FS_TABLE_START + FS_TABLE_SECTORS)

#define SECTOR_SIZE 512

typedef struct {
    uint32_t magic;
    uint32_t disk_size_sectors;
} FSHeader;

typedef struct {
    char     name[32];
    uint32_t start;
    uint32_t size;
    uint8_t  used;
    uint8_t  _pad[3];
} FileEntry;

static FSHeader  header;
static FileEntry table[FS_MAX_FILES];
static int       list_index = 0;

/* ---------------- low-level I/O ---------------- */

static void read_sector(uint32_t lba, void* buf) {
    ata_read_sector(lba, (uint8_t*)buf);
}

static void write_sector(uint32_t lba, const void* buf) {
    ata_write_sector(lba, (const uint8_t*)buf);
}

/* ---------------- table load/save ---------------- */

static void load_table() {
    uint8_t buf[SECTOR_SIZE];

    for (int i = 0; i < FS_TABLE_SECTORS; i++) {
        read_sector(FS_TABLE_START + i, buf);
        for (int j = 0; j < SECTOR_SIZE / sizeof(FileEntry); j++) {
            int idx = i * (SECTOR_SIZE / sizeof(FileEntry)) + j;
            if (idx < FS_MAX_FILES)
                table[idx] = ((FileEntry*)buf)[j];
        }
    }
}

static void save_table() {
    uint8_t buf[SECTOR_SIZE];

    for (int i = 0; i < FS_TABLE_SECTORS; i++) {
        for (int j = 0; j < SECTOR_SIZE / sizeof(FileEntry); j++) {
            int idx = i * (SECTOR_SIZE / sizeof(FileEntry)) + j;
            if (idx < FS_MAX_FILES)
                ((FileEntry*)buf)[j] = table[idx];
            else
                ((FileEntry*)buf)[j].used = 0;
        }
        write_sector(FS_TABLE_START + i, buf);
    }
}

/* ---------------- init ---------------- */

void fs_init() {
    uint8_t buf[SECTOR_SIZE];
    read_sector(FS_HEADER_SECTOR, buf);
    header = *(FSHeader*)buf;

    if (header.magic != FS_MAGIC) {
        header.magic = FS_MAGIC;
        header.disk_size_sectors = 2048; // 1MB default

        for (int i = 0; i < FS_MAX_FILES; i++)
            table[i].used = 0;

        write_sector(FS_HEADER_SECTOR, &header);
        save_table();
    } else {
        load_table();
    }
}

/* ---------------- name helpers ---------------- */

static int names_equal(const char* a, const char* b) {
    for (int i = 0; i < 32; i++) {
        if (a[i] != b[i])
            return 0;
        if (a[i] == 0 && b[i] == 0)
            return 1;
    }
    return 1;
}

static void name_copy(char* dst, const char* src) {
    for (int i = 0; i < 32; i++)
        dst[i] = 0;
    for (int i = 0; i < 32; i++) {
        dst[i] = src[i];
        if (src[i] == 0) break;
    }
}

/* ---------------- lookup ---------------- */

static int find_file(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (table[i].used && names_equal(table[i].name, name))
            return i;
    }
    return -1;
}

static int find_free_entry() {
    for (int i = 0; i < FS_MAX_FILES; i++)
        if (!table[i].used)
            return i;
    return -1;
}

/* ---------------- allocation ---------------- */

static uint32_t allocate_sectors(uint32_t size) {
    uint32_t start = FS_DATA_START;

    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (table[i].used) {
            uint32_t end = table[i].start +
                (table[i].size + SECTOR_SIZE - 1) / SECTOR_SIZE;
            if (end > start)
                start = end;
        }
    }

    return start;
}

/* ---------------- save ---------------- */

int fs_save(const char* name, const char* data, uint32_t size) {
    int idx = find_file(name);
    if (idx < 0) idx = find_free_entry();
    if (idx < 0) return -1;

    uint32_t start   = allocate_sectors(size);
    uint32_t sectors = (size + SECTOR_SIZE - 1) / SECTOR_SIZE;

    uint8_t buf[SECTOR_SIZE];

    for (uint32_t i = 0; i < sectors; i++) {
        for (int j = 0; j < SECTOR_SIZE; j++) {
            uint32_t pos = i * SECTOR_SIZE + j;
            buf[j] = (pos < size) ? (uint8_t)data[pos] : 0;
        }
        write_sector(start + i, buf);
    }

    name_copy(table[idx].name, name);
    table[idx].start = start;
    table[idx].size  = size;
    table[idx].used  = 1;

    save_table();
    return 0;
}

/* ---------------- load ---------------- */

const char* fs_load(const char* name) {
    static char buffer[8192];


    int idx = find_file(name);
    if (idx < 0) return 0;

    uint32_t size    = table[idx].size;
    uint32_t start   = table[idx].start;
    uint32_t sectors = (size + SECTOR_SIZE - 1) / SECTOR_SIZE;

    uint8_t  buf[SECTOR_SIZE];
    uint32_t pos = 0;

    for (uint32_t i = 0; i < sectors; i++) {
        read_sector(start + i, buf);
        for (int j = 0; j < SECTOR_SIZE && pos < size; j++)
            buffer[pos++] = buf[j];
    }

    if (size < sizeof(buffer))
        buffer[size] = 0;
    else
        buffer[sizeof(buffer) - 1] = 0;

    return buffer;
}

/* ---------------- delete ---------------- */

int fs_delete(const char* name) {
    int idx = find_file(name);
    if (idx < 0) return -1;

    table[idx].used = 0;
    save_table();
    return 0;
}

/* ---------------- list ---------------- */

int fs_list_start() {
    list_index = 0;
    int count = 0;
    for (int i = 0; i < FS_MAX_FILES; i++)
        if (table[i].used)
            count++;
    return count;
}

int fs_list_next(char* name_out, int max) {
    while (list_index < FS_MAX_FILES) {
        if (table[list_index].used) {
            for (int i = 0; i < max; i++) {
                name_out[i] = table[list_index].name[i];
                if (table[list_index].name[i] == 0) break;
            }
            list_index++;
            return 1;
        }
        list_index++;
    }
    return 0;
}
