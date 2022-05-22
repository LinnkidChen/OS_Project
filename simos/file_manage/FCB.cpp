#include "FCB.h"

namespace {

String separate_tail(String &str) {
    for (int i = str.length() - 1; i >= 0; i--) {
        if (str[i] == '@') {
            String ret = str.substr(i + 1);
            str.erase(i);
            return ret;
        }
    }
    return str;
}

} // namespace

FCB::FCB(String name, bool isDir, bool isRunable)
    : filesize(0),
      dir(isDir),
      runable(isRunable),
      directIndex{},
      indirectIndex{},
      filename(std::move(name)) {

    memset(directIndex, -1, sizeof(directIndex));
    memset(indirectIndex, -1, sizeof(indirectIndex));
}

String FCB::create(String str) {
    if (strlen(str.c_str()) == 0)
        return {};

    for (int i = INDEX_NUM - 1; i >= 0; i--) {
        this->indirectIndex[i] = std::stoi(separate_tail(str));
    }
    for (int i = INDEX_NUM - 1; i >= 0; i--) {
        this->directIndex[i] = std::stoi(separate_tail(str));
    }

    this->runable  = std::stoi(separate_tail(str));
    this->dir      = std::stoi(separate_tail(str));
    this->filesize = std::stoi(separate_tail(str));

    return str;
}

FCB::FCB(String str)
    : filesize(),
      dir(false),
      runable(false),
      directIndex{},
      indirectIndex{},
      filename(create(str)) {
}

FCB::FCB(const FCB &other)
    : filesize(other.filesize),
      dir(other.dir),
      runable(other.runable),
      directIndex{},
      indirectIndex{},
      filename(other.filename) {
    memcpy(directIndex, other.directIndex, sizeof(directIndex));
    memcpy(indirectIndex, other.indirectIndex, sizeof(indirectIndex));
}

String FCB::to_string() {
    String str(this->filename);
    str += ('@' + std::to_string(this->filesize));
    str += ('@' + std::to_string(this->dir));
    str += ('@' + std::to_string(this->runable));
    for (int &a : this->directIndex) {
        str += ('@' + std::to_string(a));
    }
    for (int &a : this->indirectIndex) {
        str += ('@' + std::to_string(a));
    }
    return str;
}

int FCB::check_size() {
    int size = 0;
    for (Addr addr : directIndex) {
        if (addr != -1) {
            size += disk_size(addr);
        } else {
            break;
        }
    }
    String str;
    for (Addr addr : indirectIndex) {
        if (addr != -1) {
            for (int line = 0;; line++) {
                str = disk_readline(addr, line);
                if (str == "error")
                    break;
                else
                    size += disk_size(std::stoi(str));
            }
        } else {
            break;
        }
    }
    if (size != filesize) {
        filesize = size;
        return -1;
    } else {
        return 1;
    }
}
