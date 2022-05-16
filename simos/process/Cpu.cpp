#include "Cpu.h"
#include "Process.h"

void Cpu::Run(bool step) noexcept {
    if (step) {
        auto &proc = ProcessScheduler::GetInstance();
        auto  inst = proc.GetNextInstruction();
        if (inst.m_from_process != nullptr) {
            bool blocked = inst.m_inst.instruction(*(inst.m_from_process));
            proc.InstructionDone(inst, 1, blocked);
        }
        m_circles++;

        while (!m_timers.empty() && m_timers.top().trigger_at >= m_circles) {
            m_timers.top().callback();
            m_timers.pop();
        }
    } else {
        auto &proc = ProcessScheduler::GetInstance();
        while (proc.HasProcess()) {
            auto inst = proc.GetNextInstruction();
            if (inst.m_from_process != nullptr) {
                bool blocked = inst.m_inst.instruction(*(inst.m_from_process));
                m_circles++;
                proc.InstructionDone(inst, 1, blocked);
            }

            while (!m_timers.empty() &&
                   m_timers.top().trigger_at >= m_circles) {
                m_timers.top().callback();
                m_timers.pop();
            }
        }
    }
}

Cpu &Cpu::GetInstance() noexcept {
    static Cpu instance;
    return instance;
}
