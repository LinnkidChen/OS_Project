#ifndef _FILE_MANAGE_H
#define _FILE_MANAGE_H

#include "header.h"
#include "disk_manage.h"
#include "file.h"

enum FILE_PERMISSION {
    F_CREATE = 1,
    F_TRUNC = 2,
    F_ADDEND = 4,
    F_READ = 8,
    F_WRITE = 16
};

enum SEEK_MODE { FILE_SEEK_SET = 0, FILE_SEEK_CUR = 1, FILE_SEEK_END = 2 };

Handle file_open(const char *filename, int permisson);
int file_seek(Handle h, int offset, int mode);
int file_write(Handle h, size_t length, const void *str);
int file_read(Handle h, size_t length, void *str);
int file_close(Handle h);

#endif