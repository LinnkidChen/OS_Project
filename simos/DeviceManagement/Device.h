#ifndef DEVICE_H
#define DEVICE_H

#include "DeviceDict.h"

//******************************外部接口******************************

class Device_Interface : public DeviceDict //设备接口
{
public:
    virtual int getID();                    //获取设备ID<用于设备管理>
    virtual DeviceType getType();           //获取设备类型
    virtual std::string getName();          //获取设备名称
    virtual DeviceStateType getState();     //获取设备状态
    virtual std::string getIDString();      //获取设备ID字符串<用于显示ID>
    virtual void setName(std::string name); //设置设备名称（设备新名称）
};

//******************************抽象设备******************************

class Device : public DeviceDict
{
private:
    int ID;          //设备ID
    DeviceType type; //设备类型

    std::string name;      //设备名称
    DeviceStateType state; //设备状态

public:
    Device() = default;
    Device(int ID, DeviceType type)
    {
        this->ID = ID;
        this->type = type;

        this->name += DeviceDict::deviceTypeString(this->getType());
        this->name += this->getIDString();
        this->state = DeviceStateType::free;
    }

    int getID() { return ID; }
    DeviceType getType() { return type; }
    std::string getName() { return name; }
    DeviceStateType getState() { return state; }
    std::string getIDString()
    {
        char IDStr[11];
        snprintf(IDStr, 11, "%010d", ID);
        return IDStr;
    }
    void setName(std::string name) { this->name = name; }

    void setSate(DeviceStateType state) { this->state = state; }
};

//******************************实际设备******************************

class Device_Disk : public Device //硬盘设备
{
private:
    static std::map<int, Device_Disk> diskList; //硬盘列表

public:
    Device_Disk() = default;
    Device_Disk(int ID) : Device(ID, DeviceType::disk) { diskList[ID] = *this; }

    static void removeDevice(int deviceID) { diskList.erase(deviceID); }

    static std::map<int, Device> getDeviceList()
    {
        std::map<int, Device> deviceList;
        for (auto &iter : diskList)
            deviceList[iter.first] = iter.second;
        return deviceList;
    }

    static int disk_write(int address, std::string content)
    {
        if (diskList.empty() != true)
        {
            if (diskList.begin()->second.getState() != DeviceDict::DeviceStateType::busy)
            {
                diskList.begin()->second.setSate(DeviceDict::DeviceStateType::busy);
                return diskList.begin()->first;
            }
            return -1;
        }
        return -2;
    }
    static int disk_read(int address, int size_byte)
    {
        if (diskList.empty() != true)
        {
            if (diskList.begin()->second.getState() != DeviceDict::DeviceStateType::busy)
            {
                diskList.begin()->second.setSate(DeviceDict::DeviceStateType::busy);
                return diskList.begin()->first;
            }
            return -1;
        }
        return -2;
    }

    static int disk_write(int deviceID, int address, std::string content)
    {
        if (diskList.count(deviceID) == 1)
        {
            if (diskList[deviceID].getState() != DeviceDict::DeviceStateType::busy)
            {
                diskList[deviceID].setSate(DeviceDict::DeviceStateType::busy);
                return deviceID;
            }
            return -1;
        }
        return -2;
    }
    static int disk_read(int deviceID, int address, int size_byte)
    {
        if (diskList.count(deviceID) == 1)
        {
            if (diskList[deviceID].getState() != DeviceDict::DeviceStateType::busy)
            {
                diskList[deviceID].setSate(DeviceDict::DeviceStateType::busy);
                return deviceID;
            }
            return -1;
        }
        return -2;
    }
};

class Device_Keyboard : public Device //键盘设备
{
private:
    static std::map<int, Device_Keyboard> keyboardList; //键盘列表

public:
    Device_Keyboard() = default;
    Device_Keyboard(int ID) : Device(ID, DeviceType::keyboard)
    {
        keyboardList[ID] = *this;
    }

    static void removeDevice(int deviceID) { keyboardList.erase(deviceID); }

    static std::map<int, Device> getDeviceList()
    {
        std::map<int, Device> deviceList;
        for (auto &iter : keyboardList)
            deviceList[iter.first] = iter.second;
        return deviceList;
    }

    static int keyboard_read()
    {
        if (keyboardList.empty() != true)
        {
            // if (rand() % 2 == 0)
            //     char temp = (char)('0' + rand() % 9);
            // else
            //     char temp = (char)('a' + rand() % 26);
            if (keyboardList.begin()->second.getState() != DeviceDict::DeviceStateType::busy)
            {
                keyboardList.begin()->second.setSate(DeviceDict::DeviceStateType::busy);
                return keyboardList.begin()->first;
            }
            return -1;
        }
        else
            return -2;
    }
    static int keyboard_read(int deviceID)
    {
        if (keyboardList.count(deviceID) == 1)
        {
            // if (rand() % 2 == 0)
            //     char temp = (char)('0' + rand() % 9);
            // else
            //     char temp = (char)('a' + rand() % 26);
            if (keyboardList[deviceID].getState() != DeviceDict::DeviceStateType::busy)
            {
                keyboardList[deviceID].setSate(DeviceDict::DeviceStateType::busy);
                return deviceID;
            }
            return -1;
        }
        else
            return -2;
    }
};

class Device_Printer : public Device //打印机设备
{
private:
    static std::map<int, Device_Printer> printerList; //打印机列表

public:
    Device_Printer() = default;
    Device_Printer(int ID) : Device(ID, DeviceType::printer)
    {
        printerList[ID] = *this;
    }

    void write(std::string content)
    {
        // TODO
        w->printPrinter(QString::fromStdString(content));
    }

    static void removeDevice(int deviceID) { printerList.erase(deviceID); }

    static std::map<int, Device> getDeviceList()
    {
        std::map<int, Device> deviceList;
        for (auto &iter : printerList)
            deviceList[iter.first] = iter.second;
        return deviceList;
    }

    static int printer_write(std::string content)
    {
        if (printerList.empty() != true)
        {
            if (printerList.begin()->second.getState() !=
                DeviceStateType::busy)
            {
                Device_Printer temp = printerList.begin()->second;
                temp.write(content);
                temp.setSate(DeviceStateType::busy);
                return printerList.begin()->first;
            }
            return -1;
        }
        else
            return -2;
    }

    static int printer_write(int deviceID, std::string content)
    {
        if (printerList.count(deviceID) == 1)
        {
            if (printerList[deviceID].getState() != DeviceStateType::busy)
            {
                printerList[deviceID].write(content);
                printerList[deviceID].setSate(DeviceStateType::busy);
                return deviceID;
            }
            return -1;
        }
        else
            return -2;
    }
};

class Device_Terminal : public Device //显示器设备
{
private:
    static std::map<int, Device_Terminal> terminalList; //显示器列表

public:
    Device_Terminal() = default;
    Device_Terminal(int ID) : Device(ID, DeviceType::terminal)
    {
        terminalList[ID] = *this;
    }

    void write(std::string content)
    {
        w->printTerminal(QString::fromStdString(content));
    }

    static void removeDevice(int deviceID) { terminalList.erase(deviceID); }

    static std::map<int, Device> getDeviceList()
    {
        std::map<int, Device> deviceList;
        for (auto &iter : terminalList)
            deviceList[iter.first] = iter.second;
        return deviceList;
    }

    static int terminal_write(std::string content)
    {
        if (terminalList.empty() != true)
        {
            if (terminalList.begin()->second.getState() !=
                DeviceStateType::busy)
            {
                Device_Terminal temp = terminalList.begin()->second;
                temp.write(content);
                temp.setSate(DeviceStateType::busy);
                return terminalList.begin()->first;
            }
            return -1;
        }
        else
            return -2;
    }

    static int terminal_write(int deviceID, std::string content)
    {
        if (terminalList.count(deviceID) == 1)
        {
            if (terminalList[deviceID].getState() != DeviceStateType::busy)
            {
                terminalList[deviceID].write(content);
                terminalList[deviceID].setSate(DeviceStateType::busy);
                return deviceID;
            }
            return -1;
        }
        else
            return -2;
    }
};

#endif // DEVICE_H
