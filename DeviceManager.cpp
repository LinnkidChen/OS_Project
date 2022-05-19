#include "Device.cpp"

//******************************外部接口******************************

class DeviceManager_Interface : public DeviceDict //设备管理接口
{
public:
    //--------------------设备管理--------------------

    static Device getDevice(int deviceID);       //获取设备（设备ID）
    static int addDevice(DeviceType deviceType); //添加新设备（设备类型）
    // 0，添加失败<类型为all会导致添加失败>
    // 1，添加成功
    static int removeDevice(int deviceID); //移除设备（设备ID）
    //-1，移除失败：设备ID不存在
    // 0，移除失败：设备状态繁忙
    // 1，移除成功
    static std::map<int, Device> getDeviceList(DeviceType deviceType); //获取设备列表

    //--------------------驱动接口--------------------

    static int disk_write(int address, std::string content);               //磁盘写（地址，内容）
    static int disk_write(int deviceID, int address, std::string content); //磁盘写（设备ID,地址，内容）
    // -1，没有磁盘设备
    // 0，磁盘繁忙
    // 1，存储成功
    static std::string disk_read(int address, int size_byte);               //磁盘读（地址，内容）
    static std::string disk_read(int deviceID, int address, int size_byte); //磁盘读（设备ID,地址，内容）
    // ""，没有磁盘设备
    static char keyboard_read();             //获取键盘输入
    static char keyboard_read(int deviceID); //获取键盘输入(设备ID)
    // '\0'，没有键盘设备
    static int printer_write(std::string content);               //打印机打印（内容）
    static int printer_write(int deviceID, std::string content); //打印机打印（设备ID,内容）
    // -1，没有打印机设备
    // 0，打印机繁忙
    // 1，打印成功
    static int terminal_write(std::string content);               //终端打印（内容）
    static int terminal_write(int deviceID, std::string content); //终端打印（设备ID,内容）
    // -1，没有终端设备
    // 0，终端繁忙
    // 1，打印成功
};

//******************************设备管理******************************

class DeviceManager : public DeviceDict
{
private:
    static int deviceIDPool;                 //设备ID分配池
    static std::map<int, Device> deviceList; //设备列表

public:
    static Device getDevice(int deviceID)
    {
        if (deviceList.count(deviceID) == 1)
        {
            return deviceList[deviceID];
        }
    }

    static int addDevice(DeviceType deviceType)
    {
        int deviceID = -1;
        //为新设备分配ID
        if (deviceList.size() < INT_MAX)
        {
            if (deviceList.size() == deviceIDPool)
            {
                deviceID = deviceIDPool + 1;
                deviceIDPool++;
            }
            else
            {
                for (int i = 0; i < deviceIDPool; i++)
                {
                    if (deviceList.count(i) == 0)
                    {
                        deviceID = i;
                        break;
                    }
                }
            }
        }
        else
            return 0;
        //创建新设备
        switch (deviceType)
        {
        case DeviceType::disk:
        {
            Device_Disk disk(deviceID);
            deviceList[deviceID] = disk;
            break;
        }
        case DeviceType::keyboard:
        {
            Device_Keyboard keyboard(deviceID);
            deviceList[deviceID] = keyboard;
            break;
        }
        case DeviceType::printer:
        {
            Device_Printer printer(deviceID);
            deviceList[deviceID] = printer;
            break;
        }
        case DeviceType::terminal:
        {
            Device_Terminal terminal(deviceID);
            deviceList[deviceID] = terminal;
            break;
        }
        default:
            deviceIDPool--;
            return 0;
            break;
        }
        return 1;
    }

    static int removeDevice(int deviceID)
    {
        if (deviceList.count(deviceID) == 1)
        {
            if (deviceList[deviceID].getState() != DeviceStateType::busy)
            {
                switch (deviceList[deviceID].getType())
                {
                case DeviceType::disk:
                    Device_Disk::removeDevice(deviceID);
                    break;
                case DeviceType::keyboard:
                    Device_Keyboard::removeDevice(keyboard);
                    break;
                case DeviceType::printer:
                    Device_Printer::removeDevice(printer);
                    break;
                case DeviceType::terminal:
                    Device_Terminal::removeDevice(terminal);
                    break;
                }
                deviceList.erase(deviceID);
                return 1;
            }
            else
                return 0;
        }
        return -1;
    }

    static std::map<int, Device> getDeviceList(DeviceType deviceType)
    {
        switch (deviceType)
        {
        case DeviceType::all:
            return deviceList;
            break;
        case DeviceType::disk:
            return Device_Disk::getDeviceList();
            break;
        case DeviceType::keyboard:
            return Device_Keyboard::getDeviceList();
            break;
        case DeviceType::printer:
            return Device_Printer::getDeviceList();
            break;
        case DeviceType::terminal:
            return Device_Terminal::getDeviceList();
            break;
        }
    }

    static int disk_write(int address, std::string content)
    {
        return Device_Disk::disk_write(address, content);
    }
    static std::string disk_read(int address, int size_byte)
    {
        return Device_Disk::disk_read(address, size_byte);
    }
    static char keyboard_read()
    {
        return Device_Keyboard::keyboard_read();
    }
    static int printer_write(std::string content)
    {
        return Device_Printer::printer_write(content);
    }
    static int terminal_write(std::string content)
    {
        return Device_Terminal::terminal_write(content);
    }

    static int disk_write(int deviceID, int address, std::string content)
    {
        return Device_Disk::disk_write(deviceID, address, content);
    }
    static std::string disk_read(int deviceID, int address, int size_byte)
    {
        return Device_Disk::disk_read(address, size_byte);
    }
    static char keyboard_read(int deviceID)
    {
        return Device_Keyboard::keyboard_read(deviceID);
    }
    static int printer_write(int deviceID, std::string content)
    {
        return Device_Printer::printer_write(deviceID, content);
    }
    static int terminal_write(int deviceID, std::string content)
    {
        return Device_Terminal::terminal_write(deviceID, content);
    }
};

//******************************类初始化******************************

// Device
std::map<int, Device_Disk> Device_Disk::diskList;
std::map<int, Device_Keyboard> Device_Keyboard::keyboardList;
std::map<int, Device_Printer> Device_Printer::printerList;
std::map<int, Device_Terminal> Device_Terminal::terminalList;

// DeviceManager
int DeviceManager::deviceIDPool = 0;
std::map<int, Device> DeviceManager::deviceList;

//******************************测试部分******************************
using namespace std;

int main(void)
{
    Device_Disk disk(22);
    cout << "ID:\t\t" << disk.getID() << endl;
    cout << "name:\t\t" << disk.getName() << endl;
    cout << "type:\t\t" << DeviceDict::deviceTypeString(disk.getType()) << endl;
    cout << "state:\t\t" << DeviceDict::deviceStateTypeString(disk.getState()) << endl;
    cout << "IDstring:\t" << disk.getIDString() << endl;

    DeviceManager::addDevice(DeviceDict::disk);

    return 0;
}
