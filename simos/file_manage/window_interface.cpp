#include "window_interface.h"

#include <QDebug>

void build_dir(MAP &res, std::vector<String> path, FCB dir);

std::vector<std::vector<String>> win_iter_dir() {
    MAP res;
    std::vector<String> path;
    FCB root(disk_readline(0, 0));
    build_dir(res, path, root);
    return res;
}

String win_get_file(std::vector<String> path) {
    Addr addr = 0;
    int line = 0;
    for (String name : path) {
        line = disk_searchline(addr, name);
        FCB f(disk_readline(addr, line));
        addr = f.direct_index(0);
        if (!f.is_dir()) {
            addr = f.direct_index(0);
            std::ifstream in(DISK(addr));
            std::istreambuf_iterator<char> begin(in);
            std::istreambuf_iterator<char> end;
            String str(begin, end);
            return str;
        }
    }
}

int win_del_file(std::vector<String> path) {
    Addr addr = 0;
    int line = 0;
    for (String name : path) {
        line = disk_searchline(addr, name);
        FCB f(disk_readline(addr, line));

        if (f.is_dir()) {
            addr = f.direct_index(0);
        } else {
            disk_writeline(addr, "deleted@", 8, line);
            recycle_block(f.direct_index(0));
        }
    }
    return 1;
}

int win_save_file(std::vector<String> path, String content, size_t length) {
    Addr addr = 0;
    int line = 0;
    for (String name : path) {
        line = disk_searchline(addr, name);
        FCB f(disk_readline(addr, line));
        addr = f.direct_index(0);
        if (!f.is_dir()) {
            FILE *f = fopen(DISK(addr).c_str(), "w");
            fwrite(content.c_str(), sizeof(char), length, f);
            fclose(f);
        }
    }
    return 1;
}

void build_dir(MAP &res, std::vector<String> path, FCB dir) {
    qDebug() << __FILE_NAME__ << ":" << __LINE__ << " " << __FUNCTION__
             << " is called.";
    path.push_back(dir.filename);
    Addr addr = dir.direct_index(0);
    String name;
    for (int i = 0;; i++) {
        name = disk_readline(addr, i);
        if (name == "error")
            break;
        if (name == "deleted@") {
            continue;
        }
        FCB fcb(name);
        if (fcb.is_dir()) {
            if (fcb.direct_index(0) != -1) {
                std::vector<String> path_n(path);
                // path_n.push_back(fcb.filename);
                build_dir(res, path_n, fcb);
            } else {
                path.push_back(fcb.filename);
                path.push_back("#");
                std::vector<String> path_n(path);
                res.push_back(path_n);
            }
        } else if (!fcb.filename.empty()) {
            path.push_back(fcb.filename);
            std::vector<String> path_n(path);
            res.push_back(path_n);
        }
    }
    return;
}
