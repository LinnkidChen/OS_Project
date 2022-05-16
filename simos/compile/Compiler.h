#pragma once
#include <iostream>
#include <regex>
#include<queue>
#include <string>
#include <vector>
#include "../memory/MemoryManagement.h"
class PCB;
class memory_block {

    memory::Pointer<char> ptr;
    int            size;

public:
    memory_block(int size_, const memory::Pointer<char>ptr_) :size(size_){ptr=ptr_;};
    int get_size() { return size; }
    memory::Pointer<char>get_ptr(){return ptr;}
};
class Intepreter {//输入指令并执行 
public:
    PCB const * cur_PCB;
    void run(std::string instruction);//instruction使用请参照文档

    // static std::queue<memory_block> memry_que;//TODO 写进PCB
    //不是很确定const*memryque的形式有没有写对

};


// Intepreter A;
// A.cur_pcb = pcb;
// A.run(instruction);

/*
	时钟周期
*/

class clock {
	private:
		static unsigned int clockNum;	//时钟周期数

	public:
		static void increase() {	//消耗时钟周期
			clockNum++;
		}
};
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
class commandC {
public:
        PCB             const * pcb;
    commandC(PCB const * pcb_):pcb(pcb_){}
  static std::regex mode;
  static std::regex searchMode;

  static void init() {
    mode = std::regex("^C\\s[0-9]*\\s*$");
    searchMode = std::regex("[0-9]*");
  }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work(std::string str) {
    int clockNeed; //所需周期
    std::smatch matched;

    std::regex_search(str, matched, searchMode);
    clockNeed = std::stoi(matched.str(0));
    for (; clockNeed > 0; clockNeed--) {
      clock::increase();
      //进程管理.减少该进程时间片();
    }
    return 0;
  }
};
class commandK {
public:
    PCB             const * pcb;
    commandK(PCB const * pcb_):pcb(pcb_){}
  static std::regex mode;
  static std::regex searchMode;

  static void init() {
    mode = std::regex(R"(^K\s[0-9]*\s[0-9]*\s*$)");
    searchMode = std::regex("[0-9]*");
    ;
  }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work(std::string str) ;
};
class commandP {
public:
    PCB             const * pcb;
    commandP(PCB const * pcb_):pcb(pcb_){}
  static std::regex mode;
  static std::regex searchMode;

  static void init() {
    mode = std::regex(R"(^P\s[0-9]*\s[0-9]*\s*$)");
    searchMode = std::regex("[0-9]*");
    ;
  }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work(std::string str) ;
};
class commandR {
public:
    PCB
        const * pcb;
    commandR(PCB const * pcb_):pcb(pcb_){}
  static std::regex mode;
  static std::regex searchModeFileDir;
  static std::regex searchModeNumber;

  static void init() {
    mode = std::regex("^R\\s\"[^\"]*\"\\s[0-9]*\\s(-1|[0-9]*)\\s*$");
    searchModeFileDir = std::regex("\"[^\"]*\"");
    searchModeNumber = std::regex("-?[0-9]*");
  }
  static bool match(std::string str) { return std::regex_match(str, mode); }
 int work(std::string str) ;
  
};
class commandW {
public:
        PCB             const * pcb;
    commandW(PCB const * pcb_):pcb(pcb_){}
  static std::regex mode;
  static std::regex searchModeFileDir;
  static std::regex searchModeNumber;

  static void init() {
    mode = std::regex("^W\\s\"[^\"]*\"\\s[0-9]*\\s(-1|[0-9]*)\\s*$");
    searchModeFileDir = std::regex("\"[^\"]*\"");
    searchModeNumber = std::regex("-?[0-9]*");
  }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work(std::string str) ;
};
class commandM {
public:
        PCB             const * pcb;
    commandM(PCB const * pcb_):pcb(pcb_){}
  static std::regex mode;

  static void init() { mode = std::regex("^M\\s*$"); }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work();
};
class commandY {
public:
    PCB             const * pcb;
    commandY(PCB const * pcb_):pcb(pcb_){}
  static std::regex mode;

  static void init() { mode = std::regex("^Y\\s*$"); }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work() ;
};
class commandQ {
public:
    PCB             const * pcb;
    commandQ(PCB const * pcb_):pcb(pcb_){}
  static std::regex mode;

  static void init() { mode = std::regex("^Q\\s*$"); }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work() ;
};