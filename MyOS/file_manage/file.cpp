#include "file.h"
#include "file_manage.h"

#include <cstring>
#include <iostream>
#include <memory>

std::vector<o_file *> filelist;

Handle add(const FCB &fcb, int permission, Addr address, int line) {
    auto *f = (o_file *)malloc(sizeof(o_file));
    memset(f, 0, sizeof(o_file));

    // std::cerr << __FILE_NAME__ << ":" << __LINE__ << ":" << __FUNCTION__
    //           << " - "
    //           << "FCB: filename: " << fcb.filename << '\n';

    f->fcb        = std::make_shared<FCB>(fcb);
    f->permission = permission;
    f->address    = address;
    f->line       = line;

    // std::cerr << __FILE_NAME__ << ":" << __LINE__ << ":" << __FUNCTION__
    //           << " - "
    //           << "FCB: filename: " << f->fcb->filename << '\n';

    // 载入disks
    f->disks = new std::vector<Addr>;
    f->fcb->check_size();
    int block_num = 0;
    for (int i = 0; i < INDEX_NUM; i++) {
        if (f->fcb->direct_index(i) == -1)
            break;
        else
            block_num += 1;
    }
    Addr addr;
    for (int i = 0; i < INDEX_NUM; i++) {
        addr = f->fcb->indirect_index(i);
        if (addr == -1)
            break;
        else {
            for (int j = 0;; j++) {
                if (disk_readline(addr, j) == "error")
                    break;
                else
                    block_num += 1;
            }
        }
    }
    // printf("debug-add: block_num %d\n", block_num);
    if (block_num <= INDEX_NUM) {
        for (int i = 0; i < block_num; i++) {
            if (f->fcb->direct_index(i) == -1) {
                break;
            } else {
                f->disks->push_back(f->fcb->direct_index(i));
            }
        }
    } else {
        for (int i = 0; i < INDEX_NUM; i++)
            f->disks->push_back(f->fcb->direct_index(i));
        String str;
        Addr   addr;
        for (int i = 0; i < block_num - INDEX_NUM; i++) {
            addr = f->fcb->indirect_index(i / ADDR_PER_BLOCK);
            if (addr == -1)
                break;
            str = disk_readline(addr, i % ADDR_PER_BLOCK);
            f->disks->push_back(std::stoi(str));
        }
    }

    if (permission & F_ADDEND)
        f->ptr = f->fcb->file_size();
    else
        f->ptr = 0;

    f->Handle = filelist.size();
    filelist.push_back(f);

    return f->Handle;
}

Handle file_open(const char *filename, int permission) {
    if (filelist.size() == 0) {
        int line = disk_searchline(1000, "system");
        FCB system(disk_readline(1000, line));
        add(system, F_READ | F_WRITE, 1000, line);
    }

    o_file *sys = filelist[0];
    String  name(filename);
    int     line = -1;
    Addr    addr;
    for (Addr addr_tem : *(sys->disks)) {
        line = disk_searchline(addr_tem, name);
        std::cerr << __FILE_NAME__ << ":" << __LINE__ << ":" << __FUNCTION__
                  << " - "
                  << "addr_tem: " << addr_tem << ", searchline: " << line << '\n';

        printf("debug-file_open: line %d\n", line);
        if (line != -1) {
            addr = addr_tem;
            break;
        }
    }
    // printf("debug-file_open: run\n");
    if (line == -1) {
        if (permission & F_CREATE) {
            FCB    new_file(name, false, false);
            String fcb_str = new_file.to_string();
            if (disk_size(sys->disks->back()) + fcb_str.size() > BLOCK_SIZE) {
                expand_a_block(sys);
            }
            disk_writeline(sys->disks->back(), fcb_str, fcb_str.size(), -1);
            line = disk_searchline(sys->disks->back(), name);
            addr = sys->disks->back();
            sys->fcb->check_size();
        } else {
            return -1;
        }
    }

    std::cerr << __FILE_NAME__ << ":" << __LINE__ << ":" << __FUNCTION__
              << " - "
              << "addr: " << addr << ", line: " << line << '\n';
    FCB fcb(disk_readline(addr, line));
    std::cerr << __FILE_NAME__ << ":" << __LINE__ << ":" << __FUNCTION__
              << " - "
              << "FCB: filename: " << fcb.filename << '\n';
    Handle h = add(fcb, permission, addr, line);
    printf("debug-file_open: name %s\n", filelist[h]->fcb->filename.c_str());
    if (permission & F_TRUNC) {
        printf("debug-file_open: trunc\n");
        for (Addr addr : *filelist[h]->disks) {
            recycle_block(addr);
        }
        for (int i = 0; i < INDEX_NUM; i++) {
            recycle_block(fcb.direct_index(i));
            recycle_block(fcb.indirect_index(i));
            fcb.set_index(-1, i, true);
            fcb.set_index(-1, i, false);
        }
        filelist[h]->ptr = 0;
        fcb.check_size();
    }
    printf("debug-file_open: name %s\n", filelist[h]->fcb->filename.c_str());
    printf("debug-handle: %d\n", h);
    return h;
}

int file_seek(Handle h, int offset, int mode) {
    o_file *f = filelist[h];
    if (f == nullptr)
        return -1;
    if (mode == FILE_SEEK_SET) {
        f->ptr = offset;
        return f->ptr;
    } else if (mode == FILE_SEEK_CUR) {
        f->ptr += offset;
        return f->ptr;
    } else if (mode == FILE_SEEK_END) {
        f->ptr = f->fcb->file_size() + offset;
        return f->ptr;
    } else
        return -1;
}

int file_write(Handle h, size_t length, const void *str) {
    auto    ptr = static_cast<const char *>(str);
    o_file *f   = filelist[h];
    for (int i = 0; i < 4; i++)
        printf("debug-file_write %d\n", f->fcb->direct_index(i));
    if ((f->permission & F_WRITE) == 0)
        return -1;

    int block_num_new = (f->ptr + length + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (block_num_new > f->disks->size()) {
        for (int i = 0; i < (block_num_new - f->disks->size()); i++) {
            expand_a_block(f);
        }
    }

    printf("debug-file_write: run\n");
    Addr addr = locate(f);

    for (size_t l = 0, len = length; len > 0;) {
        l = disk_write(addr, ptr, len);
        f->ptr += l;
        addr = locate(f);
        ptr += l;
        len -= l;
    }

    f->fcb->check_size();
    return length;
}

int file_read(Handle h, size_t length, void *str) {
    auto    ptr = static_cast<char *>(str);
    o_file *f   = filelist[h];
    if ((f->permission & F_READ) == 0)
        return -1;

    Addr addr = locate(f);
    if (f->ptr + length > f->fcb->file_size())
        length = (size_t)f->fcb->file_size() - f->ptr;

    for (size_t l = 0, len = length; len > 0;) {
        l = disk_read(addr, ptr, len);
        f->ptr += l;
        addr = locate(f);
        ptr += l;
        len -= l;
    }

    return length;
}

int file_close(Handle h) {
    free(filelist[h]);
    return 1;
}

Addr locate(o_file *f) {
    int block_No     = f->ptr / BLOCK_SIZE;
    int block_offset = f->ptr % BLOCK_SIZE;
    return f->disks->at(block_No) + block_offset;
}

void expand_a_block(o_file *f) {
    Addr addr = assign_block();
    f->disks->push_back(addr);
    // printf("debug-expand_a_block: name %s\n", f->fcb->filename.c_str());
    if (f->disks->size() <= INDEX_NUM) {
        f->fcb->set_index(addr, f->disks->size() - 1, true);
    } else {
        int block_indrt = f->disks->size() - INDEX_NUM;
        int indrt_num   = block_indrt / ADDR_PER_BLOCK;
        if (block_indrt % ADDR_PER_BLOCK == 1) {
            f->fcb->set_index(assign_block(), indrt_num, false);
        }
        disk_writeline(f->fcb->indirect_index(indrt_num), addr,
                       block_indrt % ADDR_PER_BLOCK);
    }
    printf("debug-expand_a_block: run\n");
    printf("debug-expand_a_block: name %s\n", f->fcb->filename.c_str());
    String str = f->fcb->to_string();
    disk_writeline(f->address, str, str.length(), f->line);
    for (Addr a : *f->disks)
        printf("debug-expand_a_block: %s disks %d\n",
               f->fcb->to_string().c_str(), a);
}
