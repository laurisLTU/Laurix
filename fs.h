#ifndef FS_H
#define FS_H

#include <stdint.h>

/*
    LaurixFS – simple custom filesystem
    Supports:
      - save
      - load
      - delete
      - list files
      - custom disk size (set in fs_init)
*/

/* Initialize filesystem (detect disk, load table, etc.) */
void fs_init();

/* Save a file (overwrite if exists) */
int fs_save(const char* name, const char* data, uint32_t size);

/* Load a file (returns pointer to internal buffer or NULL) */
const char* fs_load(const char* name);

/* Delete a file (0 = OK, -1 = not found) */
int fs_delete(const char* name);

/* List files: 
      fs_list_start() returns number of files
      fs_list_next(name_out, max) returns 1 if next file exists, 0 if done
*/
int fs_list_start();
int fs_list_next(char* name_out, int max);

#endif
