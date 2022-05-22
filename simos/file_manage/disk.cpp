#include "disk.h"

#include <iostream>

namespace {

class FileWrapper {
public:
    FileWrapper() noexcept : m_file(nullptr) {}
    FileWrapper(FILE *f) noexcept : m_file(f) {}
    ~FileWrapper() {
        if (m_file)
            fclose(m_file);
    }

    FileWrapper(const FileWrapper &) = delete;
    FileWrapper &operator=(const FileWrapper &) = delete;

    FileWrapper(FileWrapper &&other) noexcept : m_file(other.m_file) {
        other.m_file = nullptr;
    }

    FileWrapper &operator=(FileWrapper &&other) noexcept {
        Release();
        m_file       = other.m_file;
        other.m_file = nullptr;
        return *this;
    }

    FileWrapper &operator=(FILE *f) noexcept {
        Release();
        m_file = f;
        return *this;
    }

    void Release() noexcept {
        if (m_file != nullptr) {
            fclose(m_file);
            m_file = nullptr;
        }
    }

    operator FILE *() const noexcept { return m_file; }

private:
    FILE *m_file = nullptr;
};

} // namespace

size_t disk_write(Addr addr, const char *str, size_t length) {
    // String disk_name = "DISK/" + std::to_string(addr / BLOCK_SIZE) + ".disk";
    // FILE *disk = fopen(disk_name.c_str(), "r+");
    FileWrapper disk = fopen(DISK(addr).c_str(), "r+");
    addr %= BLOCK_SIZE;
    fseek(disk, addr * sizeof(char), SEEK_SET);
    if (addr + length > BLOCK_SIZE)
        length = BLOCK_SIZE - addr;
    // printf("debug-disk_write: %d %d\n",ftell(disk),addr);
    length = fwrite(str, sizeof(char), length, disk);
    return length;
}

size_t disk_read(Addr addr, char *str, size_t length) {
    FileWrapper disk = fopen(DISK(addr).c_str(), "r");
    addr %= BLOCK_SIZE;
    fseek(disk, addr * sizeof(char), SEEK_SET);
    if (addr + length > BLOCK_SIZE)
        length = BLOCK_SIZE - addr;
    length = fread(str, sizeof(char), length, disk);
    return length;
}

int disk_size(Addr addr) {
    FileWrapper disk = fopen(DISK(addr).c_str(), "r");

    fseek(disk, 0, SEEK_END);
    int size = ftell(disk);
    // printf("debug-disk_size: %d\n", size);
    return size;
}

size_t disk_writeline(Addr addr, String str, size_t length, int line) {
    FileWrapper disk;
    if (line == -1) {
        disk   = fopen(DISK(addr).c_str(), "a");
        length = fwrite(str.c_str(), sizeof(char), length, disk);
        fwrite("#", sizeof(char), 1, disk);
    } else {
        int line_begin = 0, line_end;
        disk           = fopen(DISK(addr).c_str(), "r+");
        for (int line_count = 0; line_count < line; line_begin++) {
            if (fgetc(disk) == '#') {
                line_count++;
            }
        }

        for (line_end = line_begin;; line_end++) {
            if (fgetc(disk) == '#')
                break;
        }

        if (line_end - line_begin == length) {
            fseek(disk, line_begin, SEEK_SET);
            fwrite(str.c_str(), sizeof(char), length, disk);
        } else {
            line_end++;
            fseek(disk, 0, SEEK_END);
            int   end         = ftell(disk);
            int   left_len    = end - line_end;
            char *left_string = (char *)malloc(sizeof(char) * left_len + 1);
            fseek(disk, line_end, SEEK_SET);
            fread(left_string, sizeof(char), left_len, disk);

            fseek(disk, line_begin, SEEK_SET);
            fwrite(str.c_str(), sizeof(char), length, disk);
            fwrite("#", sizeof(char), 1, disk);

            fwrite(left_string, sizeof(char), left_len, disk);
        }
    }
    // printf("debug-disk_writeline: ftell:%ld\n", ftell(disk));
    return length;
}

size_t disk_writeline(Addr addr, int str, int line) {
    String in = std::to_string(str);
    return disk_writeline(addr, in, in.size(), line);
}

String disk_readline(Addr addr, int line) {
    FileWrapper disk       = fopen(DISK(addr).c_str(), "r");
    int         line_begin = 0;
    for (int line_count = 0; line_count < line; line_begin++) {
        if (feof(disk)) {
            return "error";
        }
        if (fgetc(disk) == '#')
            line_count++;
    }
    int str_len;
    for (str_len = 0;; str_len++) {
        if (fgetc(disk) == '#')
            break;
    }
    // printf("debug-disk_readline: str_len %d\n", str_len);
    fseek(disk, line_begin, SEEK_SET);
    char *str = (char *)malloc(sizeof(char) * str_len + 1);
    fread(str, sizeof(char), str_len, disk);
    str[str_len] = '\0';
    String ret(str);
    return ret;
}

int disk_searchline(Addr addr, String name) {
    int match_name(FILE * disk, String name);

    printf("debug-disk_searchline: str %s\n", name.c_str());

    if (disk_size(addr) == 0) {
        return -1;
    }

    // std::cerr << __FILE_NAME__ << ":" << __LINE__ << " " << __FUNCTION__
    //           << " - "
    //           << "Filename: " << DISK(addr) << '\n';

    FileWrapper disk = fopen(DISK(addr).c_str(), "r");
    // char        buffer[1024]{};
    // fscanf(disk, "%s", buffer);
    // std::cerr << __FILE_NAME__ << ":" << __LINE__ << " " << __FUNCTION__
    //           << " - " << buffer << '\n';
    // fseek(disk, 0, SEEK_SET);

    if (match_name(disk, name)) {
        return 0;
    }

    // std::cerr << __FILE_NAME__ << ":" << __LINE__ << " " << __FUNCTION__
    //           << " - "
    //           << "match_name end" << '\n';

    char in = fgetc(disk);
    for (int line = 0; feof(disk) == 0;) {
        if (in == '#') {
            line++;
            if (match_name(disk, name)) {
                return line;
            }
        }
        in = fgetc(disk);
    }
    return -1;
}

int match_name(FILE *disk, String name) {
    int flag = 1;
    for (char ch : name) {
        char c = fgetc(disk);
        // std::cerr << __FILE_NAME__ << ":" << __LINE__ << " " << __FUNCTION__
        //           << " - "
        //           << "c: " << c << ", ch: " << ch << '\n';
        if (c != ch) {
            flag = 0;
            break;
        }
    }
    if (fgetc(disk) != '@')
        flag = 0;
    return flag;
}