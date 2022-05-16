#include "Compiler.h"
#include <regex>
//string instr: instruction
void Intepreter::run(std ::string instr) {
    bool matched=false;
    //Command C
    auto c_cmd_ptr = new commandC(cur_PCB);
    c_cmd_ptr->init();
    if (c_cmd_ptr->match(instr))
    {   c_cmd_ptr->work(instr);
        delete c_cmd_ptr;
        c_cmd_ptr = nullptr;
        matched=true;}
    else
        delete c_cmd_ptr;
    //Command K
    auto k_cmd_ptr = new commandK(cur_PCB);
    k_cmd_ptr->init();
    if (k_cmd_ptr->match(instr)) {
        k_cmd_ptr->work(instr);
        matched=true;}
    else
        delete k_cmd_ptr;

    //Command P
    auto p_cmd_ptr = new commandP(cur_PCB);
    p_cmd_ptr->init();
    if (p_cmd_ptr->match(instr)) {
        p_cmd_ptr->work(instr);
        matched=true;}
    else
        delete p_cmd_ptr;
    
    //Command R
    auto r_cmd_ptr = new commandR(cur_PCB);
    r_cmd_ptr->init();
    if (r_cmd_ptr->match(instr))
        {r_cmd_ptr->work(instr);matched=true;}
    else
        delete r_cmd_ptr;
    // Command W
    
    auto w_cmd_ptr = new commandW(cur_PCB);
    w_cmd_ptr->init();
    if ( w_cmd_ptr->match(instr))
        {w_cmd_ptr->work(instr);matched=true;}
    else
        delete w_cmd_ptr;
    // Command M
    
    auto m_cmd_ptr = new commandM(cur_PCB);
    m_cmd_ptr->init();
    if ( m_cmd_ptr->match(instr))
       {matched=true; m_cmd_ptr->work();}
    else
        delete m_cmd_ptr;
    // Command Y
    
    auto y_cmd_ptr = new commandY(cur_PCB);
    y_cmd_ptr->init();
    if ( y_cmd_ptr->match(instr))
       { y_cmd_ptr->work();matched=true;}
    else
        delete y_cmd_ptr;
    // Command Q
    
    auto q_cmd_ptr = new commandQ(cur_PCB);
    q_cmd_ptr->init();
    if ( q_cmd_ptr->match(instr))
        {q_cmd_ptr->work();matched=true;}
    else
        delete q_cmd_ptr;

    if(!matched);//错误处理
}

//使用嵌套类的方法内置指令实行函数
//嵌套类与外类无相互访问权限 需要通过传参实现

int commandK::work(std::string str) {
    int time; //使用的周期
    int size=1024; //文件大小
    std::smatch matched;

    std::regex_search(str, matched, searchMode);
    time = std::stoi(matched.str(0));
    size = std::stoi(matched.str(1));

    memory::Pointer<char> ptr(memory::MemoryPool::GetInstance().Allocate(size));
    if (ptr.Get() == nullptr) {
        //terminate process
    }

    pcb->memry_queue.emplace(size,ptr);
    /*
            
            设备管理.键盘写入内存(内核.获得缓冲区地址(), size);
            进程管理.阻塞该进程(中断条件_键盘写入完成);
    */
    for (; time > 0; time--) {
      clock::increase();
    }
    return 0;
}

 int commandP:: work(std::string str) {
    int size;   //文件大小
    int offset; //偏移量
    std::smatch matched;

    std::regex_search(str, matched, searchMode);
    offset = std::stoi(matched.str(0));
    size = std::stoi(matched.str(1));
    memory_block m_block = pcb->memry_queue.front();
    pcb->memry_queue.pop();

    
    clock::increase();
    /*
            设备管理.打印内容(进程管理.获得进程起始地址() + offset, size);
            进程管理.阻塞该进程(中断条件_打印完成);
    */
 }

int commandR:: work(std::string str) {
    int size;            //文件大小
    int offset;          //偏移量
    std::string fileDir; //文件目录
    std::smatch matched;

    std::regex_search(str, matched, searchModeFileDir);
    fileDir = matched.str(0);
    fileDir = fileDir.substr(1, fileDir.length() - 2);//?

    std::regex_search(str, matched, searchModeNumber);
    offset = std::stoi(matched.str(0));
    size = std::stoi(matched.str(1));

    clock::increase();

 memory::Pointer<char> ptr(memory::MemoryPool::GetInstance().Allocate(size));
    if (ptr.Get() == nullptr) {
        //terminate process
    }
    pcb->memry_queue.emplace(size,ptr);

    /*
            设备管理.文件写入内存(内核.获得系统缓冲区地址(), fileDir, offset,
       size); 进程管理.阻塞该进程(中断条件_文件读入完成);
    */
}
int commandW::work(std::string str) {
    int size;            //文件大小
    int offset;          //偏移量
    std::string fileDir; //文件目录
    std::smatch matched;

    std::regex_search(str, matched, searchModeFileDir);
    fileDir = matched.str(0);
    fileDir = fileDir.substr(1, fileDir.length() - 2);

    std::regex_search(str, matched, searchModeNumber);
    offset = std::stoi(matched.str(0));
    size = std::stoi(matched.str(1));
    memory_block m_block = pcb->memry_queue.front();
    pcb->memry_queue.pop();

    clock::increase();
    /*
            设备管理.内存写入文件(内核.获得系统缓冲区地址(), fileDir, offset,
       size); 进程管理.阻塞该进程(中断条件_文件写入完成);
    */
}
  int commandM::work() {
    clock::increase();
    /*
            return 进程管理.查询该进程占用内存();
    */
    int sum = 0;
    for (auto &i : pcb->memry_que) {
        sum += i.size;
    }
    return sum;
  }
 int commandY:: work() {
     clock::increase();
     return pcb->priority;
    /*
            return 进程管理.查询该进程优先级();
    */
 }
 int commandQ::work() {
    clock::increase();
    /*
            进程管理.销毁该进程();
    */
    Terminate();
  }


 