#ifndef _FCB_H
#define _FCB_H

#include "disk.h"
#include "header.h"

#define INDEX_NUM 4

class FCB {
public:
    FCB(String name, bool isDir, bool isRunable);
    FCB(String str);

    FCB(const FCB &other);
    FCB &operator=(const FCB &other) = delete;

    int         check_size();
    inline void set_index(Addr, int, bool);
    inline int  file_size();
    inline bool is_dir();
    inline bool is_runable();
    inline Addr direct_index(int);
    inline Addr indirect_index(int);
    String      to_string();
    String      create(String);

private:
    int  filesize;                 // 文件大小
    bool dir;                      // 是否为文件夹
    bool runable;                  // 是否为可执行文件
    Addr directIndex[INDEX_NUM];   // 直接索引块
    Addr indirectIndex[INDEX_NUM]; // 间接索引块

public:
    const String filename; // 文件名，最大长度固定
};

inline int FCB::file_size() { return this->filesize; }

inline bool FCB::is_dir() { return this->dir; }

inline bool FCB::is_runable() { return this->runable; }

inline Addr FCB::direct_index(int i) { return this->directIndex[i]; }

inline Addr FCB::indirect_index(int i) { return this->indirectIndex[i]; }

inline void FCB::set_index(Addr addr, int i, bool is_direct) {
    if (is_direct)
        directIndex[i] = addr;
    else
        indirectIndex[i] = addr;
}

// root@0@1@0@-1@-1@-1@-1@-1@-1@-1@-1
// namenamenamename@504000@0@0@10101010@10101010@10101010@10101010@10101010@10101010@10101010@10101010

#endif