#pragma once
// device 唯一标识
#include <cstddef>
#include <string>
#include<vector>
class DCB{
    int Did=0;
    enum type {NONE=0, INPUT, OUTPUT, DISK };
    type kind = NONE;
    std::string d_name; // device name
    bool  updated;// 状态更新 需要更新到文件
    void       *device_ptr = nullptr; //指向具体device类的指针
};
class Device_ptr {
public:
    void *ptr = nullptr;
    enum type {NONE=0, INPUT, OUTPUT, DISK };
    type kind = NONE;
};
class Device_Manage {
    static std::vector<DCB> DiskList;
    static std::string      FilePath;
    
public:
    int initiate(std::string FilePath_);//传入路径 根据路径中的文件初始化DCBlist 具体实现时可以加默认值
    int update(int Did=-1);//更新实时文件状态到设备。-1表示更新所有;
    int delete_device(int Did = -1); //删除设备，-1表示删除所有
    Device_ptr get_device(int Did);//请求设备的指针
};
