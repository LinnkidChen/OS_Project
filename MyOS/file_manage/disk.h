#ifndef _DISK_H
#define _DISK_H

#include "header.h"

#define BLOCK_SIZE 1000

#define BLOCK_NUM 100

// #define BLOCK_NUM 64000

#define ADDR_PER_BLOCK 125

#define DISK(X) ("DISK/" + std::to_string((X) / BLOCK_SIZE) + ".disk")

// 写入磁盘块，返回成功写入的长度（块容量不超过块大小）
size_t disk_write(Addr addr, const char *str, size_t length);

// 读出内容，返回成功读取的长度（块容量不超过块大小）
size_t disk_read(Addr addr, char *str, size_t length);

// 返回块当前已用空间
int disk_size(Addr addr);

// 写入（覆盖）一行内容，line为行号，-1代表追加
size_t disk_writeline(Addr addr, String str, size_t length, int line);
size_t disk_writeline(Addr addr, int str, int line);

// 读取一行内容，line为行号
String disk_readline(Addr addr, int line);

// 搜索文件名
int disk_searchline(Addr addr, String name);

#endif