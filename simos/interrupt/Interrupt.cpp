#include "Interrupt.h"
void interrupt::add_itrpt(int process_num , ITRTYPE itr_type , int deviceInfo,memory::Pointer<char> ptr=nullptr ) {
    itrpt_que.emplace(process_num, itr_type, deviceInfo);
    itrpt_switch = true;
    
}
void interrupt:: process() {
    if (itrpt_que.empty()) {
        itrpt_switch = false;
        return;
    }
    while (!itrpt_que.empty()) {
        work(itrpt_que.front());
        itrpt_que.pop();
    }
    itrpt_switch = false;
}

void interrupt::work(intVec& Input) {
    if (!valid(Input))
        return;
    if (Input.itrpt_type == input) {
        //call keyboard input
    }
    if (Input.itrpt_type == output) {
        //call Monitor output
    }
}

bool interrupt::valid(const intVec &input) {
    if (input.device_num >= 0 && input.itrpt_type != none && input.process >= 0)
        return true;
    else return false;
}