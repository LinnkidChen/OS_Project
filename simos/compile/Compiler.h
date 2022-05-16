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
  static std::regex mode;
  static std::regex searchMode;

  static void init() {
    mode = std::regex("^K\\s[0-9]*\\s[0-9]*\\s*$");
    searchMode = std::regex("[0-9]*");
    ;
  }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work(std::string str) {
    int time; //使用的周期
    int size; //文件大小
    std::smatch matched;

    std::regex_search(str, matched, searchMode);
    time = std::stoi(matched.str(0));
    size = std::stoi(matched.str(1));

    for (; time > 0; time--) {
      clock::increase();
    }
    /*
            设备管理.键盘写入内存(内核.获得缓冲区地址(), size);
            进程管理.阻塞该进程(中断条件_键盘写入完成);
    */
    return 0;
  }
};
class commandP {
public:
  static std::regex mode;
  static std::regex searchMode;

  static void init() {
    mode = std::regex("^P\\s[0-9]*\\s[0-9]*\\s*$");
    searchMode = std::regex("[0-9]*");
    ;
  }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work(std::string str) {
    int size;   //文件大小
    int offset; //偏移量
    std::smatch matched;

    std::regex_search(str, matched, searchMode);
    offset = std::stoi(matched.str(0));
    size = std::stoi(matched.str(1));

    clock::increase();
    /*
            设备管理.打印内容(进程管理.获得进程起始地址() + offset, size);
            进程管理.阻塞该进程(中断条件_打印完成);
    */
  }
};
class commandR {
public:
  static std::regex mode;
  static std::regex searchModeFileDir;
  static std::regex searchModeNumber;

  static void init() {
    mode = std::regex("^R\\s\"[^\"]*\"\\s[0-9]*\\s(-1|[0-9]*)\\s*$");
    searchModeFileDir = std::regex("\"[^\"]*\"");
    searchModeNumber = std::regex("-?[0-9]*");
  }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work(std::string str) {
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

    clock::increase();
    /*
            设备管理.文件写入内存(内核.获得系统缓冲区地址(), fileDir, offset,
       size); 进程管理.阻塞该进程(中断条件_文件读入完成);
    */
  }
};
class commandW {
public:
  static std::regex mode;
  static std::regex searchModeFileDir;
  static std::regex searchModeNumber;

  static void init() {
    mode = std::regex("^W\\s\"[^\"]*\"\\s[0-9]*\\s(-1|[0-9]*)\\s*$");
    searchModeFileDir = std::regex("\"[^\"]*\"");
    searchModeNumber = std::regex("-?[0-9]*");
  }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work(std::string str) {
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

    clock::increase();
    /*
            设备管理.内存写入文件(内核.获得系统缓冲区地址(), fileDir, offset,
       size); 进程管理.阻塞该进程(中断条件_文件写入完成);
    */
  }
};
class commandM {
public:
  static std::regex mode;

  static void init() { mode = std::regex("^M\\s*$"); }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work() {
    clock::increase();
    /*
            return 进程管理.查询该进程占用内存();
    */
  }
};
class commandY {
public:
  static std::regex mode;

  static void init() { mode = std::regex("^Y\\s*$"); }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work() {
    clock::increase();
    /*
            return 进程管理.查询该进程优先级();
    */
  }
};
class commandQ {
public:
  static std::regex mode;

  static void init() { mode = std::regex("^Q\\s*$"); }
  static bool match(std::string str) { return std::regex_match(str, mode); }
  static int work() {
    clock::increase();
    /*
            进程管理.销毁该进程();
    */
  }
};