#include "DeviceManager.h"

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

class Device : public Device_Interface
{
private:
    int ID;          //设备ID
    DeviceType type; //设备类型

    std::string name;      //设备名称
    DeviceStateType state; //设备状态

public:
    Device() {}
    Device(int ID, DeviceType type)
    {
        this->ID = ID;
        this->type = type;

        this->name += this->getType();
        this->name += this->getIDString();
        this->state = DeviceStateType::free;
    }

    int getID()
    {
        return ID;
    }
    DeviceType getType()
    {
        return type;
    }
    std::string getName()
    {
        return name;
    }
    DeviceStateType getState()
    {
        return state;
    }
    std::string getIDString()
    {
        char IDStr[11];
        snprintf(IDStr, 11, "%010d", ID);
        return IDStr;
    }
    void setName(std::string name)
    {
        this->name = name;
    }

    void setSate(DeviceStateType state)
    {
        this->state = state;
    }
};

//******************************实际设备******************************

class Device_Disk : public Device //硬盘设备
{
private:
    std::map<int, std::string> buffer;
    static std::map<int, Device_Disk> diskList; //硬盘列表

public:
    Device_Disk() {}
    Device_Disk(int ID) : Device(ID, DeviceType::disk) {}

    void write(int address, std::string content)
    {
        buffer[address] = content;
    }

    static void addDevice(Device_Disk device)
    {
        diskList[device.getID()] = device;
    }

    static void removeDevice(int deviceID)
    {
        diskList.erase(deviceID);
    }

    static std::map<int, Device> getDeviceList()
    {
        std::map<int, Device> deviceList;
        for (std::map<int, Device_Disk>::iterator iter = diskList.begin(); iter != diskList.end(); iter++)
            deviceList[iter->first] = iter->second;
        return deviceList;
    }

    static int disk_write(int address, std::string content)
    {
        if (diskList.empty() != 1)
        {
            for (std::map<int, Device_Disk>::iterator iter = diskList.begin(); iter != diskList.end(); iter++)
            {
                if (iter->second.getState() != DeviceStateType::busy)
                {
                    Device_Disk &temp = diskList.begin()->second;
                    temp.write(address, content);
                    temp.setSate(DeviceStateType::busy);
                    // Cpu::GetInstance().RegisterTimer(content.length() + 1, [&temp](){temp.setSate(DeviceStateType::free);});
                    return 1;
                }
            }
            return 0;
        }
        else
            return -1;
    }

    static std::string disk_read(int address, int size_byte)
    {
        if (diskList.empty() != 1)
        {
            std::string temp;
            if (rand() % 2 == 0)
            {
                for (int i = 0; i < size_byte; i++)
                    temp += (char)('0' + rand() % 9);
            }
            else
            {
                for (int i = 0; i < size_byte; i++)
                    temp += (char)('a' + rand() % 26);
            }
            // cpu.RegisterTimer(size_byte % 10 + 1, [](){});
            // return temp;
        }
        else
            return "";
    }
};

class Device_Keyboard : public Device //键盘设备
{
private:
    static std::map<int, Device_Keyboard> keyboardList; //键盘列表

public:
    Device_Keyboard() {}
    Device_Keyboard(int ID) : Device(ID, DeviceType::keyboard) {}

    static void addDevice(Device_Keyboard device)
    {
        keyboardList[device.getID()] = device;
    }

    static void removeDevice(int deviceID)
    {
        keyboardList.erase(deviceID);
    }

    static std::map<int, Device> getDeviceList()
    {
        std::map<int, Device> deviceList;
        for (std::map<int, Device_Keyboard>::iterator iter = keyboardList.begin(); iter != keyboardList.end(); iter++)
            deviceList[iter->first] = iter->second;
        return deviceList;
    }

    static char keyboard_read()
    {
        if (keyboardList.empty() != 1)
        {
            if (rand() % 2 == 0)
            {
                char temp = (char)('0' + rand() % 9);
                // cpu.RegisterTimer(1, [](){});
                // return temp;
            }
            else
            {
                char temp = (char)('a' + rand() % 26);
                // cpu.RegisterTimer(1, [](){});
                // return temp;
            }
        }
        else
            return '\0';
    }
};

class Device_Printer : public Device //打印机设备
{
private:
    std::string buffer;
    static std::map<int, Device_Printer> printerList; //打印机列表

public:
    Device_Printer() {}
    Device_Printer(int ID) : Device(ID, DeviceType::printer) {}

    std::string getBuffer()
    {
        return buffer;
    }

    void write(std::string content)
    {
        buffer += content;
    }

    static void addDevice(Device_Printer device)
    {
        printerList[device.getID()] = device;
    }

    static void removeDevice(int deviceID)
    {
        printerList.erase(deviceID);
    }

    static std::map<int, Device> getDeviceList()
    {
        std::map<int, Device> deviceList;
        for (std::map<int, Device_Printer>::iterator iter = printerList.begin(); iter != printerList.end(); iter++)
            deviceList[iter->first] = iter->second;
        return deviceList;
    }

    static int printer_write(std::string content)
    {
        if (printerList.empty() != 1)
        {
            for (std::map<int, Device_Printer>::iterator iter = printerList.begin(); iter != printerList.end(); iter++)
            {
                if (iter->second.getState() != DeviceStateType::busy)
                {
                    Device_Printer temp = printerList.begin()->second;
                    temp.write(content);
                    temp.setSate(DeviceStateType::busy);
                    // cpu.RegisterTimer(content.length() % 10 + 1, [temp](){temp.setSate(DeviceStateType::free);});
                    return 1;
                }
            }
            return 0;
        }
        else
            return -1;
    }
};

class Device_Terminal : public Device //显示器设备
{
private:
    std::string buffer;
    static std::map<int, Device_Terminal> terminalList; //显示器列表

public:
    Device_Terminal() {}
    Device_Terminal(int ID) : Device(ID, DeviceType::terminal) {}

    std::string getBuffer()
    {
        return buffer;
    }

    void write(std::string content)
    {
        buffer += content;
    }

    static void addDevice(Device_Terminal device)
    {
        terminalList[device.getID()] = device;
    }

    static void removeDevice(int deviceID)
    {
        terminalList.erase(deviceID);
    }

    static std::map<int, Device> getDeviceList()
    {
        std::map<int, Device> deviceList;
        for (std::map<int, Device_Terminal>::iterator iter = terminalList.begin(); iter != terminalList.end(); iter++)
            deviceList[iter->first] = iter->second;
        return deviceList;
    }

    static int terminal_write(std::string content)
    {
        if (terminalList.empty() != 1)
        {
            for (std::map<int, Device_Terminal>::iterator iter = terminalList.begin(); iter != terminalList.end(); iter++)
            {
                if (iter->second.getState() != DeviceStateType::busy)
                {
                    Device_Terminal temp = terminalList.begin()->second;
                    temp.write(content);
                    temp.setSate(DeviceStateType::busy);
                    // cpu.RegisterTimer(content.length() + 1, [temp](){temp.setSate(DeviceStateType::free);});
                    return 1;
                }
            }
            return 0;
        }
        else
            return -1;
    }
};