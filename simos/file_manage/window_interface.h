#ifndef _WINDOW_INTERFACE_H
#define _WINDOW_INTERFACE_H

#include "header.h"
#include "disk.h"
#include "disk_manage.h"
#include "FCB.h"
#include <fstream>

typedef std::vector<std::vector<String>> MAP;

std::vector<std::vector<String>> win_iter_dir();
String win_get_file(std::vector<String>);
int win_del_file(std::vector<String>);
int win_save_file(std::vector<String>, String, size_t);

#endif
