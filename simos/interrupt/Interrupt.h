#ifndef INTERRUPT_H
#define INTERRUPT_H
#include <queue>
#if __cplusplus < 201103L
	#error "This program requires C++ 11 or later"
#    endif

#    include <iostream>


// 应在初始化时生成一个唯一的interrupt类
// 请使用interrupt::getInstance()调用public函数
// 如：interrupt::getInstance().check_switch()
//
class interrupt {
    enum ITRTYPE { input, output, soft, none = 0 };//TODO 讨论中断类型
class intVec {
   
public:
    intVec(int processnum=-1,ITRTYPE itrtype=none, int device=-1):process(processnum),itrpt_type(itrtype),device_num(device){}    
     int process; //进程号
    ITRTYPE itrpt_type = none;
    int device_num=-1;
    };
    private:
    static std::queue<intVec> itrpt_que;
    static bool                itrpt_switch; //中断开关
    interrupt() =default;
	~interrupt() = default;
	interrupt(const interrupt&);
	interrupt& operator=(const interrupt&);
        bool       valid(const intVec &input);
        void work(intVec& input);
public:
    bool check_switch() { return itrpt_switch; }
    int check_que_len(){return itrpt_que.size();}
    void add_itrpt(int process_num = -1, ITRTYPE itr_type = none,
                   int deviceInfo = 0,memory::Pointer<char> ptr=nullptr);
    void process();//固定时间执行，检查是否有中断需要处理。如果有，执行中断处理程序；如果没有，跳过。

    static interrupt &getInstance() {
        static interrupt instance;
        return instance;
    }
}
;



#endif









