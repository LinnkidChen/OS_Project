#include "DeviceManager.h"
#include "Device.h"

//******************************类初始化******************************

// Device
// std::map<int, Device_Disk>     Device_Disk::diskList;
// std::map<int, Device_Keyboard> Device_Keyboard::keyboardList;
// std::map<int, Device_Printer>  Device_Printer::printerList;
// std::map<int, Device_Terminal> Device_Terminal::terminalList;

// DeviceManager
int                   DeviceManager::deviceIDPool = 0;
std::map<int, Device> DeviceManager::deviceList;

//******************************测试部分******************************
//using namespace std;

//int main(void)
//{
//    Device_Disk disk(22);
//    cout << "ID:\t\t" << disk.getID() << endl;
//    cout << "name:\t\t" << disk.getName() << endl;
//    cout << "type:\t\t" << DeviceDict::deviceTypeString(disk.getType()) << endl;
//    cout << "state:\t\t" << DeviceDict::deviceStateTypeString(disk.getState()) << endl;
//    cout << "IDstring:\t" << disk.getIDString() << endl;

//    DeviceManager::addDevice(DeviceDict::disk);

//    return 0;
//}
