#pragma once

#include "../memory/MemoryManagement.h"
#include "../process/Process.h"

#include <string>
#include <vector>

class Intepreter { //输入指令并执行
public:
    static auto interpret(const std::string &src) -> std::vector<Instruction>;
    static auto interpret(const std::string *src_list, size_t num_src)
        -> std::vector<Instruction>;

    // static std::queue<memory_block> memry_que;//TODO 写进PCB
    //不是很确定const*memryque的形式有没有写对
};

// Intepreter A;
// A.cur_pcb = pcb;
// A.run(instruction);

/*
        时钟周期
*/

/*
        指令系统

        ------------

        static std::regex mode;			匹配模式

        static void init();					初始化模式
        static bool match(std::string str);	是否匹配模式
        static int work(std::string str);	执行指令

        ------------

        * 使用以下类前请务必先初始化模式
        * 使用以下类执行指令前请先测试确保能够匹配该类的模式
*/
