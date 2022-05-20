#ifndef DEVICEDICT_H
#define DEVICEDICT_H

#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

//******************************类型定义******************************
class DeviceDict //设备词典
{
public:
    enum DeviceType //设备类型
    {
        all,      //全部
        disk,     //硬盘
        keyboard, //键盘
        printer,  //打印机
        terminal  //显示器
    };
    enum DeviceStateType //设备状态
    {
        busy, //占用
        free  //空闲
    };

    static std::string
    deviceTypeString(DeviceType deviceType) //获得设备类型字符串（设备类型）
    {
        switch (deviceType) {
        case DeviceType::all:
            return "all";
            break;
        case DeviceType::disk:
            return "disk";
            break;
        case DeviceType::keyboard:
            return "keyboard";
            break;
        case DeviceType::printer:
            return "printer";
            break;
        case DeviceType::terminal:
            return "terminal";
            break;
        }
    }
    static std::string deviceStateTypeString(
        DeviceStateType deviceStateType) //获得设备状态字符串（设备状态）
    {
        switch (deviceStateType) {
        case DeviceStateType::busy:
            return "busy";
            break;
        case DeviceStateType::free:
            return "free";
            break;
        }
    }
};

#endif // DEVICEDICT_H
