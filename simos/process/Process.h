#ifndef PROCESS_H
#define PROCESS_H

#include "../memory/MemoryManagement.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

struct Process;

struct Instruction {
    std::string context; // instruction name
    // return whether this process should be blocked
    std::function<bool(Process &)> instruction;
};

enum class ProcessState {
    Creating,
    Blocked,
    Waiting,
    Running,
    Destroying,
};

struct ProcessMeta {
    // PCB
    int64_t      m_pid;   // pid, should be no less than 0
    ProcessState m_state; // state of current process
    std::string  m_name;
    int64_t      m_start_time; // process start time(clock circle)
    int64_t      m_time_left;  // time slice left
};

struct ProcessResource {
    struct AllocatedMemory {
        memory::Pointer<char> ptr;
        size_t                size = 0;
    };

    std::string                  m_keyboard_input;
    std::vector<AllocatedMemory> m_memory;
};

struct Process {
    using InstructionList = std::vector<Instruction>;

    ProcessMeta m_pcb;

    // Program content
    InstructionList m_process_instructions;
    size_t          m_next_instruction;

    ProcessResource m_resource;

    bool GetNextInstruction(Instruction &inst) noexcept {
        if (m_next_instruction >= m_process_instructions.size())
            return false;
        inst = m_process_instructions[m_next_instruction];
        m_next_instruction++;
        return true;
    }
};

struct CpuInstruction {
    Process *const m_from_process;
    const size_t   m_last_queue;
    Instruction    m_inst;
    bool           m_add_cycle;
};

class ProcessScheduler {
public:
    ProcessScheduler() noexcept  = default;
    ~ProcessScheduler() noexcept = default;

    ProcessScheduler(const ProcessScheduler &) = delete;
    void operator=(const ProcessScheduler &) = delete;

    Process *GetProcessInfo(int64_t pid) noexcept {
        auto it = m_processes.find(pid);
        if (it == m_processes.end())
            return nullptr;
        return std::addressof(it->second);
    }

    const Process *GetProcessInfo(int64_t pid) const noexcept {
        auto it = m_processes.find(pid);
        if (it == m_processes.end())
            return nullptr;
        return std::addressof(it->second);
    }

    Process *GetCurrentProcessInfo() noexcept { return m_current_process; }

    const Process *GetCurrentProcessInfo() const noexcept {
        return m_current_process;
    }

    CpuInstruction GetNextInstruction() noexcept;

    void InstructionDone(CpuInstruction &inst, int64_t time_spent,
                         bool is_blocked) noexcept;

    bool HasProcess() const noexcept { return !m_processes.empty(); }

    bool HasRunningProcess() const noexcept {
        return (!m_queues[0].empty() || !m_queues[1].empty() ||
                !m_queues[2].empty());
    }

    bool HasBlockedProcess() const noexcept { return !m_blocked.empty(); }

    void WakeupProcess(Process *proc) noexcept;

    Process *StartProcess(std::string              name,
                          std::vector<Instruction> program) noexcept;

    const std::map<int64_t, Process> &GetAllProcesses() const noexcept {
        return m_processes;
    }

    Process *GetCurrentProcess() const noexcept { return m_current_process; }

    Process *GetLastProcess() const noexcept { return m_last_process; }

    /// Never use the process again once you call this method.
    void KillProcess(int64_t pid) noexcept;

    /// \return
    ///   -1 if process doesn't exists or process has been blocked.
    int32_t GetProcessPriority(Process &proc) const noexcept;

    static ProcessScheduler &GetInstance() noexcept;

private:
    int64_t AllocatePID() noexcept;

private:
    Process *m_last_process    = nullptr;
    Process *m_current_process = nullptr;

    std::map<int64_t, Process> m_processes;
    std::deque<Process *>      m_queues[3]; // schedule queue

    std::map<Process *, size_t> m_blocked; // blocked processes
    int64_t                     m_max_pid = 0;
};

#endif // PROCESS_H
