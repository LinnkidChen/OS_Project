#pragma once

class Intepreter {//输入指令并执行 
    PCB * const cur_PCB;
    queue<memory_block> memry_que;
public:
    void run(string instruction);//instruction使用请参照文档
// TODO ⬇️小组讨论实现
    void save(); //将memoryqueue以及相关的数据结构保存进内存中 
    void read();//将相关的数据结构从内存中读取出来 
    
    };
class memory_block {

    memory_pointer ptr;
    int            size;

public:
    memory_block(int size_, memory_pointer ptr_) : prt(ptr_), size(size_){};
    
};