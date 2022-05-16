#include "Compiler.h"
#include <regex>
//string instr: instruction
void Intepreter::run(std ::string instr) {
    //Command C
    auto c_cmd_ptr = new commandC;
    c_cmd_ptr->init();
    if (c_cmd_ptr->match(instr))
    {   c_cmd_ptr->work(instr);
        delete c_cmd_ptr;
        c_cmd_ptr=nullptr;}
    else
        delete c_cmd_ptr;
    //Command K
    auto k_cmd_ptr = new commandK;
    k_cmd_ptr->init();
    if (k_cmd_ptr->match(instr))
        k_cmd_ptr->work(instr);
    else
        delete k_cmd_ptr;

    //Command P
    auto p_cmd_ptr = new commandK;
    p_cmd_ptr->init();
    if (p_cmd_ptr->match(instr))
        p_cmd_ptr->work(instr);
    else
        delete p_cmd_ptr;
    
    //Command R
    auto r_cmd_ptr = new commandK;
    r_cmd_ptr->init();
    if (r_cmd_ptr->match(instr))
        r_cmd_ptr->work(instr);
    else
        delete r_cmd_ptr;
    // Command W
    
    auto w_cmd_ptr = new commandK;
    w_cmd_ptr->init();
    if ( w_cmd_ptr->match(instr))
        w_cmd_ptr->work(instr);
    else
        delete w_cmd_ptr;
    // Command M
    
    auto m_cmd_ptr = new commandK;
    m_cmd_ptr->init();
    if ( m_cmd_ptr->match(instr))
        m_cmd_ptr->work(instr);
    else
        delete m_cmd_ptr;
    // Command Y
    
    auto y_cmd_ptr = new commandK;
    y_cmd_ptr->init();
    if ( y_cmd_ptr->match(instr))
        y_cmd_ptr->work(instr);
    else
        delete y_cmd_ptr;
    // Command Q
    
    auto q_cmd_ptr = new commandK;
    q_cmd_ptr->init();
    if ( q_cmd_ptr->match(instr))
        q_cmd_ptr->work(instr);
    else
        delete q_cmd_ptr;
}

//使用嵌套类的方法内置指令实行函数
//嵌套类与外类无相互访问权限 需要通过传参实现