#ifndef _FILE_H
#define _FILE_H

#include "FCB.h"
#include "disk.h"
#include "header.h"

#include <memory>

struct o_file {
    std::shared_ptr<FCB> fcb;
    std::vector<Addr>   *disks;
    int                  Handle;
    int                  ptr;
    int                  permission;
    Addr                 address;
    int                  line;
};
using o_file = struct o_file;

Handle add(const FCB &fcb, int permission, Addr address, int line);

// 根据ptr定位磁盘块
Addr locate(o_file *f);

// 扩张一个块
void expand_a_block(o_file *f);

#endif