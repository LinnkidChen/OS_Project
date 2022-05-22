#ifndef _DISK_MANAGE_H
#define _DISK_MANAGE_H

#include "header.h"
#include "disk.h"

// 返回一个未被占据的块的地址，并标记为已占用，-1代表出错
Addr assign_block();

// 回收一个块
int recycle_block(Addr addr);

#endif