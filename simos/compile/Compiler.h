#pragma once
#include <iostream>
#include <regex>
#include <string>
#include<vector>
class Intepreter {//输入指令并执行 
    PCB * const cur_PCB;
    queue<memory_block> memry_que;
    std::vector <std::regex> instru_reg_list;//指令正则化的表达集合
public:
    void run(std::string instruction);//instruction使用请参照文档
// TODO ⬇️小组讨论实现
    void save(); //将memoryqueue以及相关的数据结构保存进内存中
    void read(); //将相关的数据结构从内存中读取出来

    Intepreter() {
        std::vector<std::string> string_list = {"^c\\s[1-9][0-9]*$", "^enter "};
        for (auto i : string_list) {
            instru_reg_list.emplace_back(i);
        }
    }
};



class memory_block {

    memory_pointer ptr;
    int            size;

public:
    memory_block(int size_, memory_pointer ptr_) : prt(ptr_), size(size_){};
    int get_size() { return size; }
    memory_pointer get_ptr(){return ptr;}
};
class enter {
public:
	std::string mode = "^enter ";
	
};
