#include "Process.h"
#include "Cpu.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>

static constexpr size_t NUM_INST_PER_LEVEL = 5;

CpuInstruction ProcessScheduler::GetNextInstruction() noexcept {
    for (size_t i = 0; i < std::size(m_queues); ++i) {
        if (m_queues[i].empty())
            continue;
        assert(m_current_process == nullptr);

        // get process
        m_current_process = m_queues[i].front();
        m_queues[i].pop_front();
        assert(m_current_process != nullptr);
        m_current_process->m_pcb.m_state = ProcessState::Running;

        CpuInstruction ret{m_current_process, i, {}};
        bool result = m_current_process->GetNextInstruction(ret.m_inst);
        (void)result;
        assert(result);

        return ret;
    }

    return CpuInstruction{nullptr, {}, {}};
}

void ProcessScheduler::WakeupProcess(Process *proc) noexcept {
    auto it = m_blocked.find(proc);
    assert(it != m_blocked.end());
    assert(it->first == proc);
    auto queue_id = it->second;
    m_blocked.erase(it);
    proc->m_pcb.m_time_left = NUM_INST_PER_LEVEL * queue_id;
    m_queues[queue_id].push_back(proc);
}

void ProcessScheduler::InstructionDone(CpuInstruction &inst,
                                       int64_t         time_spent,
                                       bool            is_blocked) noexcept {
    m_current_process = nullptr;
    auto process      = inst.m_from_process;
    m_last_process    = process;

    // this process finished executing
    if (process->m_next_instruction >= process->m_process_instructions.size()) {
        m_processes.erase(process->m_pcb.m_pid);
        m_last_process = nullptr;
    } else {
        process->m_pcb.m_time_left -= time_spent;
        if (is_blocked) {
            process->m_pcb.m_state = ProcessState::Blocked;
            assert(m_blocked.find(process) == m_blocked.end());
            m_blocked.emplace(process, inst.m_last_queue);
        } else {
            if (process->m_pcb.m_time_left > 0)
                m_queues[inst.m_last_queue].push_front(process);
            else {
                process->m_pcb.m_state = ProcessState::Waiting;
                size_t next_queue_id   = inst.m_last_queue + 1;
                if (next_queue_id >= std::size(m_queues))
                    next_queue_id = std::size(m_queues) - 1;
                process->m_pcb.m_time_left =
                    (next_queue_id + 1) * NUM_INST_PER_LEVEL;
                m_queues[next_queue_id].push_back(inst.m_from_process);
            }
        }
    }
}

void ProcessScheduler::KillProcess(int64_t pid) noexcept {
    auto it = m_processes.find(pid);
    if (it == m_processes.end())
        return;

    it->second.m_pcb.m_state = ProcessState::Destroying;

    if (m_last_process == std::addressof(it->second))
        m_last_process = nullptr;
    if (m_current_process == std::addressof(it->second))
        m_current_process = nullptr;
    m_processes.erase(it);
}

int64_t ProcessScheduler::AllocatePID() const noexcept {
    int64_t ret = 0;
    while (m_processes.find(ret) != m_processes.end())
        ++ret;
    return ret;
}

Process *
ProcessScheduler::StartProcess(std::string              name,
                               std::vector<Instruction> program) noexcept {
    Process proc;
    // TODO: add program start time
    proc.m_process_instructions = std::move(program);
    proc.m_next_instruction     = 0;
    proc.m_pcb.m_pid            = AllocatePID();
    proc.m_pcb.m_start_time     = Cpu::GetInstance().GetCurrentTime();
    proc.m_pcb.m_time_left      = NUM_INST_PER_LEVEL;
    proc.m_pcb.m_name           = std::move(name);

    int64_t pid      = proc.m_pcb.m_pid;
    auto    result   = m_processes.emplace(pid, std::move(proc));
    auto    proc_ptr = std::addressof(result.first->second);
    m_queues[0].push_back(proc_ptr);
    return proc_ptr;
}

ProcessScheduler &ProcessScheduler::GetInstance() noexcept {
    static ProcessScheduler instance;
    return instance;
}
