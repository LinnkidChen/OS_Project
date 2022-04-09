#include "Compiler.h"
#include <regex>

void Intepreter::run(std ::string instruction) {
    if(std::regex_match(instruction,std::regex("^enter ")))
}

//使用嵌套类的方法内置指令实行函数
//嵌套类与外类无相互访问权限 需要通过传参实现