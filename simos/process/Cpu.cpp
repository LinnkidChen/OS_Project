#include "Cpu.h"
#include "Process.h"

void Cpu::Run(bool step, std::function<void()> flush_window) noexcept {
    if (step) {
        m_timer.stop();
        Tick();
        if (flush_window)
            flush_window();
    } else {
        m_timer.callOnTimeout([this, flush_window]() {
            auto &proc = ProcessScheduler::GetInstance();
            if (proc.HasProcess()) {
                this->Tick();
                if (flush_window)
                    flush_window();
                if (proc.HasProcess() && !this->m_timer.isActive())
                    this->m_timer.start(100);
            } else {
                this->m_timer.stop();
            }
        });
        m_timer.start(200);
    }
}

void Cpu::Tick() noexcept {
    auto &proc = ProcessScheduler::GetInstance();
    auto  inst = proc.GetNextInstruction();

    if (inst.m_from_process != nullptr) {
        bool blocked = inst.m_inst.instruction(*(inst.m_from_process));
        proc.InstructionDone(inst, 1, blocked);
    }

    if (inst.m_add_cycle)
        m_circles++;

    while (!m_timers.empty() && m_timers.top().trigger_at < m_circles) {
        m_timers.top().callback();
        m_timers.pop();
    }
}

Cpu &Cpu::GetInstance() noexcept {
    static Cpu instance;
    return instance;
}
