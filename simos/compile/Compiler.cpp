#include "Compiler.h"
#include "../DeviceManagement/DeviceManager.h"
#include "../file_manage/file_manage.h"
#include "../memory/MemoryManagement.h"
#include "../process/Cpu.h"

#include <QDebug>

#include <iostream>
#include <queue>
#include <regex>

namespace {

uint32_t Log2I(int i) noexcept {
    uint32_t ret = 0;
    while (i) {
        i >>= 1;
        ret++;
    }
    return ret;
}

class commandC {
public:
    static const std::regex mode;
    static const std::regex searchMode;

    static bool match(const std::string &str) {
        return std::regex_match(str, mode);
    }

    static auto interpret(const std::string &src) -> std::vector<Instruction>;
};

class commandK {
public:
    static const std::regex mode;
    static const std::regex searchMode;
    static const std::regex searchModeString;

    static bool match(const std::string &str) {
        return std::regex_match(str, mode);
    }

    static auto interpret(const std::string &src) -> std::vector<Instruction>;
};

class commandP {
public:
    static const std::regex mode;
    static const std::regex searchMode;

    static bool match(const std::string &str) {
        return std::regex_match(str, mode);
    }

    static auto interpret(const std::string &src) -> std::vector<Instruction>;
};

class commandR {
public:
    static const std::regex mode;
    static const std::regex searchModeFileDir;
    static const std::regex searchModeNumber;

    static bool match(const std::string &str) {
        return std::regex_match(str, mode);
    }

    static auto interpret(const std::string &src) -> std::vector<Instruction>;
};

class commandW {
public:
    static const std::regex mode;
    static const std::regex searchModeFileDir;
    static const std::regex searchModeNumber;

    static bool match(const std::string &str) {
        return std::regex_match(str, mode);
    }

    static auto interpret(const std::string &src) -> std::vector<Instruction>;
};

class commandM {
public:
    static const std::regex mode;

    static bool match(const std::string &str) {
        return std::regex_match(str, mode);
    }

    static auto interpret(const std::string &src) -> std::vector<Instruction>;
};

class commandY {
public:
    static const std::regex mode;

    static bool match(const std::string &str) {
        return std::regex_match(str, mode);
    }

    static auto interpret(const std::string &src) -> std::vector<Instruction>;
};

class commandQ {
public:
    static const std::regex mode;

    static bool match(const std::string &str) {
        return std::regex_match(str, mode);
    }

    static auto interpret(const std::string &src) -> std::vector<Instruction>;
};

const std::regex commandC::mode{"^C\\s*[1-9][0-9]*\\s*$"};
const std::regex commandC::searchMode{"[1-9][0-9]*"};
const std::regex commandK::mode{R"(^K\s*[1-9][0-9]*\s*.+)"};
const std::regex commandK::searchMode{"[1-9][0-9]*"};
const std::regex commandK::searchModeString{"[^\\s]+$"};
const std::regex commandP::mode{R"(^P\s*(0|[1-9][0-9]*)\s*[1-9][0-9]*\s*$)"};
const std::regex commandP::searchMode{"[0-9]+"};
const std::regex commandR::mode{
    R"(^R\s*"[^"]*"\s*(0|[1-9][0-9]*)\s*[1-9][0-9]*\s*$)"};
const std::regex commandR::searchModeFileDir{R"("[^"]*")"};
const std::regex commandR::searchModeNumber{"[0-9]+"};
const std::regex commandW::mode{
    R"(^W\s*"[^"]*"\s*(0|[1-9][0-9]*)\s*[1-9][0-9]*\s*$)"};
const std::regex commandW::searchModeFileDir{R"("[^"]*")"};
const std::regex commandW::searchModeNumber{"[0-9]+"};
const std::regex commandM::mode{"^M\\s*$"};
const std::regex commandY::mode{"^Y\\s*$"};
const std::regex commandQ::mode{"^Q\\s*$"};

/// \brief
///   Clock指令，模拟使用CPU进行一定时间的计算。
auto commandC::interpret(const std::string &src) -> std::vector<Instruction> {
    int         clock_need = 0;
    std::smatch matched;
    std::regex_search(src, matched, searchMode);
    clock_need = std::stoi(matched.str(0));
    std::vector<Instruction> inst_list;

    for (int i = 0; i < clock_need; ++i) {
        Instruction inst;
        inst.context     = src + ' ' + std::to_string(i);
        inst.instruction = [](Process &) { return false; };
        inst_list.emplace_back(std::move(inst));
    }
    return inst_list;
}

//使用嵌套类的方法内置指令实行函数
//嵌套类与外类无相互访问权限 需要通过传参实现

// int commandK::work(std::string str) {
//     int         time;        //使用的周期
//     int         size = 1024; //文件大小
//     std::smatch matched;

//     std::regex_search(str, matched, searchMode);
//     time = std::stoi(matched.str(0));
//     size = std::stoi(matched.str(1));

//     memory::Pointer<char>
//     ptr(memory::MemoryPool::GetInstance().Allocate(size)); if (ptr.Get() ==
//     nullptr) {
//         // terminate process
//     }

//     pcb->memry_queue.emplace(size, ptr);
//     /*

//             设备管理.键盘写入内存(内核.获得缓冲区地址(), size);
//             进程管理.阻塞该进程(中断条件_键盘写入完成);
//     */
//     for (; time > 0; time--) {
//         clock::increase();
//     }
//     return 0;
// }

auto commandK::interpret(const std::string &src) -> std::vector<Instruction> {
    int         time = 0;
    std::smatch matched;

    std::regex_search(src, matched, searchMode);
    time                  = std::stoi(matched.str(0));
    std::string key_input = matched.suffix();

    Instruction inst;
    inst.context = src;
    inst.instruction =
        [time, key_input = std::move(key_input)](Process &proc) -> bool {
        int64_t pid = proc.m_pcb.m_pid;
        auto   &cpu = Cpu::GetInstance();
        cpu.RegisterTimer(
            static_cast<uint64_t>(time),
            [pid, key_input = std::move(key_input)]() {
                auto &proc_mgn = ProcessScheduler::GetInstance();
                auto  proc     = proc_mgn.GetProcessInfo(pid);
                if (proc == nullptr)
                    return;

                proc_mgn.WakeupProcess(proc);

                // Allocate memory and write keyboard input to it
                auto ptr = memory::MemoryPool::GetInstance().Allocate(
                    key_input.size() + 1);

                memory::Pointer<char> str = memory::Pointer<char>(ptr);
                for (size_t i = 0; i < key_input.size(); ++i) {
                    str[i] = key_input[i];
                }
                str[key_input.size()] = '\0';

                // Mount memory resource to process
                ProcessResource::AllocatedMemory mem_resource;
                mem_resource.ptr  = str;
                mem_resource.size = key_input.size() + 1;
                proc->m_resource.m_memory.push_back(std::move(mem_resource));

                qDebug() << "Memory resource size: " << mem_resource.size;
            });
        return true; // Block current thread
    };

    return {inst};
}

// int commandP::work(std::string str) {
//     int         size;   //文件大小
//     int         offset; //偏移量
//     std::smatch matched;

//     std::regex_search(str, matched, searchMode);
//     offset               = std::stoi(matched.str(0));
//     size                 = std::stoi(matched.str(1));
//     memory_block m_block = pcb->memry_queue.front();
//     pcb->memry_queue.pop();

//     clock::increase();
//     /*
//             设备管理.打印内容(进程管理.获得进程起始地址() + offset, size);
//             进程管理.阻塞该进程(中断条件_打印完成);
//     */
// }

auto commandP::interpret(const std::string &src) -> std::vector<Instruction> {
    int         size   = 0;
    int         offset = 0;
    std::smatch matched;

    std::regex_search(src, matched, searchMode);
    offset = std::stoi(matched.str(0));
    std::regex_search(src, matched, searchMode);
    size = std::stoi(matched.str(0));

    Instruction inst;
    inst.context     = src;
    inst.instruction = [size, offset](Process &proc) -> bool {
        int64_t pid = proc.m_pcb.m_pid;
        auto   &cpu = Cpu::GetInstance();

        // Wakeup after Log2(size) cpu cycles.
        cpu.RegisterTimer(Log2I(size), [pid, off = offset, size]() mutable {
            auto  offset   = off;
            auto &proc_mgn = ProcessScheduler::GetInstance();
            auto  proc     = proc_mgn.GetProcessInfo(pid);
            if (proc == nullptr)
                return;

            proc_mgn.WakeupProcess(proc);
            for (const auto &i : proc->m_resource.m_memory) {
                if (i.size <= offset) {
                    offset -= i.size;
                    continue;
                } else {
                    auto        ptr = memory::Pointer<char>(i.ptr);
                    std::string buffer(i.size, '\0');
                    for (size_t j = 0; j < i.size; ++j)
                        buffer[j] = ptr[j];
                    std::string_view sv = buffer;
                    if (sv.size() >= static_cast<size_t>(offset))
                        sv.remove_prefix(offset);
                    else
                        sv = std::string_view();
                    DeviceManager::printer_write(std::string(sv));
                    break;
                }
            }
        });
        return true;
    };

    return {inst};
}

// int commandR::work(std::string str) {
//     int         size;    //文件大小
//     int         offset;  //偏移量
//     std::string fileDir; //文件目录
//     std::smatch matched;

//     std::regex_search(str, matched, searchModeFileDir);
//     fileDir = matched.str(0);
//     fileDir = fileDir.substr(1, fileDir.length() - 2); //?

//     std::regex_search(str, matched, searchModeNumber);
//     offset = std::stoi(matched.str(0));
//     size   = std::stoi(matched.str(1));

//     clock::increase();

//     memory::Pointer<char>
//     ptr(memory::MemoryPool::GetInstance().Allocate(size)); if (ptr.Get() ==
//     nullptr) {
//         // terminate process
//     }
//     pcb->memry_queue.emplace(size, ptr);

//     /*
//             设备管理.文件写入内存(内核.获得系统缓冲区地址(), fileDir, offset,
//        size); 进程管理.阻塞该进程(中断条件_文件读入完成);
//     */
// }

auto commandR::interpret(const std::string &src) -> std::vector<Instruction> {
    int         size   = 0;
    int         offset = 0;
    std::string fileDir;
    std::smatch matched;

    std::regex_search(src, matched, searchModeFileDir);
    fileDir = matched.str(0);
    fileDir = fileDir.substr(1, fileDir.length() - 2); // ?

    std::regex_search(src, matched, searchModeNumber);
    offset = std::stoi(matched.str(0));
    std::regex_search(src, matched, searchModeNumber);
    size = std::stoi(matched.str(0));

    Instruction inst;
    inst.context = src;
    inst.instruction =
        [file_dir = std::move(fileDir), offset, size](Process &proc) -> bool {
        int64_t pid = proc.m_pcb.m_pid;
        auto   &cpu = Cpu::GetInstance();
        cpu.RegisterTimer(
            Log2I(size), [file_dir = std::move(file_dir), offset, size, pid]() {
                auto &proc_mgn = ProcessScheduler::GetInstance();
                auto  proc     = proc_mgn.GetProcessInfo(pid);
                // Test if this process has been killed
                if (proc == nullptr)
                    return;

                auto file = file_open(file_dir.c_str(), F_READ);
                if (file < 0) {
                    qDebug() << "commandR: Failed to open file: "
                             << file_dir.c_str();
                } else {
                    file_seek(file, static_cast<int>(offset), FILE_SEEK_SET);
                    std::string buffer(size, '\0');
                    file_read(file, static_cast<size_t>(size), buffer.data());
                    auto buf = memory::Pointer<char>(
                        memory::MemoryPool::GetInstance().Allocate(size + 1));
                    for (size_t i = 0; i < size; ++i)
                        buf[i] = buffer[i];

                    //                    qDebug() << __FILE_NAME__ << ':' <<
                    //                    __LINE__ << ' '
                    //                             << "Read from file " <<
                    //                             file_dir.c_str() << ": "
                    //                             << buffer.c_str();

                    file_close(file);
                }

                // TODO: call file API and memory API
                proc_mgn.WakeupProcess(proc);
            });
        return true;
    };

    return {inst};
}

// int commandW::work(std::string str) {
//     int         size;    //文件大小
//     int         offset;  //偏移量
//     std::string fileDir; //文件目录
//     std::smatch matched;

//     std::regex_search(str, matched, searchModeFileDir);
//     fileDir = matched.str(0);
//     fileDir = fileDir.substr(1, fileDir.length() - 2);

//     std::regex_search(str, matched, searchModeNumber);
//     offset               = std::stoi(matched.str(0));
//     size                 = std::stoi(matched.str(1));
//     memory_block m_block = pcb->memry_queue.front();
//     pcb->memry_queue.pop();

//     clock::increase();
//     /*
//             设备管理.内存写入文件(内核.获得系统缓冲区地址(), fileDir, offset,
//        size); 进程管理.阻塞该进程(中断条件_文件写入完成);
//     */
// }

auto commandW::interpret(const std::string &src) -> std::vector<Instruction> {
    int         size   = 0;
    int         offset = 0;
    std::string fileDir;
    std::smatch matched;

    std::regex_search(src, matched, searchModeFileDir);
    fileDir = matched.str(0);
    fileDir = fileDir.substr(1, fileDir.length() - 2);

    std::regex_search(src, matched, searchModeNumber);
    offset = std::stoi(matched.str(0));
    std::regex_search(src, matched, searchModeNumber);
    size = std::stoi(matched.str(0));

    Instruction inst;
    inst.context = src;
    inst.instruction =
        [file_dir = std::move(fileDir), offset, size](Process &proc) -> bool {
        int64_t pid = proc.m_pcb.m_pid;
        auto   &cpu = Cpu::GetInstance();
        cpu.RegisterTimer(
            Log2I(size), [file_dir = std::move(file_dir), offset, size, pid]() {
                auto &proc_mgn = ProcessScheduler::GetInstance();
                auto  proc     = proc_mgn.GetProcessInfo(pid);
                // Test if this process has been killed
                if (proc == nullptr)
                    return;

                auto file =
                    file_open(file_dir.c_str(), F_WRITE | F_TRUNC | F_CREATE);
                if (file >= 0 && !proc->m_resource.m_memory.empty()) {
                    auto &mem = proc->m_resource.m_memory.back();
                    char  buffer[4096]{};
                    for (size_t i = 0; i < mem.size; ++i) {
                        buffer[i] = mem.ptr[i];
                    }

                    file_seek(file, static_cast<int>(offset), FILE_SEEK_SET);
                    file_write(file, mem.size, buffer);
                    file_close(file);
                }

                // TODO: call file API and memory API
                proc_mgn.WakeupProcess(proc);
            });
        return true;
    };

    return {inst};
}

// int commandM::work() {
//     clock::increase();
//     /*
//             return 进程管理.查询该进程占用内存();
//     */
//     int sum = 0;
//     for (auto &i : pcb->memry_que) {
//         sum += i.size;
//     }
//     return sum;
// }

auto commandM::interpret(const std::string &src) -> std::vector<Instruction> {
    Instruction inst;
    inst.context     = src;
    inst.instruction = [](Process &proc) {
        size_t total = 0;
        for (const auto &m : proc.m_resource.m_memory)
            total += m.size;
        DeviceManager::terminal_write(
            "PID: " + std::to_string(proc.m_pcb.m_pid) +
            ", Memory usage: " + std::to_string(total) + '\n');
        return false;
    };
    return {inst};
}

// int commandY::work() {
//     clock::increase();
//     return pcb->priority;
//     /*
//             return 进程管理.查询该进程优先级();
//     */
// }

auto commandY::interpret(const std::string &src) -> std::vector<Instruction> {
    Instruction inst;
    inst.context     = src;
    inst.instruction = [](Process &proc) {
        auto &proc_mgn = ProcessScheduler::GetInstance();
        auto  priority = proc_mgn.GetProcessPriority(proc);
        DeviceManager::terminal_write(
            "PID: " + std::to_string(proc.m_pcb.m_pid) +
            ", Process priority: " + std::to_string(priority) + '\n');
        return false;
    };
    return {inst};
}

auto commandQ::interpret(const std::string &src) -> std::vector<Instruction> {
    Instruction inst;
    inst.context     = src;
    inst.instruction = [](Process &proc) {
        auto &proc_mgn = ProcessScheduler::GetInstance();
        proc_mgn.KillProcess(proc.m_pcb.m_pid);
        return false;
    };
    return {inst};
}

// int commandQ::work() {
//     clock::increase();
//     /*
//             进程管理.销毁该进程();
//     */
//     Terminate();
// }

} // namespace

auto Intepreter::interpret(const std::string &src) -> std::vector<Instruction> {
    if (commandC::match(src)) {
        return commandC::interpret(src);
    } else if (commandK::match(src)) {
        return commandK::interpret(src);
    } else if (commandP::match(src)) {
        return commandP::interpret(src);
    } else if (commandR::match(src)) {
        return commandR::interpret(src);
    } else if (commandW::match(src)) {
        return commandW::interpret(src);
    } else if (commandM::match(src)) {
        return commandM::interpret(src);
    } else if (commandY::match(src)) {
        return commandY::interpret(src);
    } else if (commandQ::match(src)) {
        return commandQ::interpret(src);
    }
    return {};
}

auto Intepreter::interpret(const std::string *src_list, size_t num_src)
    -> std::vector<Instruction> {
    std::vector<Instruction> ret;
    for (size_t i = 0; i < num_src; ++i) {
        auto temp = interpret(src_list[i]);
        for (auto &j : temp)
            ret.emplace_back(std::move(j));
    }
    return ret;
}

//*****标准样例
// using namespace std;

// int main(void)
// {
//     cout << commandC::match("C 99") << "\t\"C 99\""
//          << "\t\t\t\t\"C time\"" << endl;
//     cout << commandK::match("K 99 1024") << "\t\"K 99 1024\""
//          << "\t\t\t\"K time size\"" << endl;
//     cout << commandP::match("P 0 1024") << "\t\"P 0 1024\""
//          << "\t\t\t\"P offset size\"" << endl;
//     cout << commandR::match("R \"root//dir//doc\" 0 1024") << "\t\"R
//     \"root//dir//doc\" 0 1024\""
//          << "\t\"R dir offset size\"" << endl;
//     cout << commandW::match("W \"root//dir//doc\" 0 1024") << "\t\"W
//     \"root//dir//doc\" 0 1024\""
//          << "\t\"R dir offset size\"" << endl;
//     cout << commandM::match("M") << "\t\"M\""
//          << "\t\t\t\t\"M\"" << endl;
//     cout << commandY::match("Y") << "\t\"Y\""
//          << "\t\t\t\t\"Y\"" << endl;
//     cout << commandQ::match("Q") << "\t\"Q\""
//          << "\t\t\t\t\"Q\"" << endl;
//     return 0;
// }

//*****搜索方法
// using namespace std;

// int main(void)
// {
//     std::smatch matched;

//     std::string str = "K 10 1024";
//     std::string::const_iterator iter_begin = str.begin();
//     std::string::const_iterator iter_end = str.end();

//     while(regex_search(iter_begin, iter_end, matched, commandK::searchMode))
//     {
//         cout << matched[0] << endl;
//         iter_begin = matched[0].second;
//     }

//     return 0;
// }
