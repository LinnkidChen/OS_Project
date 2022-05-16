#include "ProcessWindow.h"
#include "Cpu.h"
#include "Process.h"
#include "ui_ProcessWindow.h"

#include <cstdio>

ProcessWindow::ProcessWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::ProcessWindow) {
    ui->setupUi(this);

    connect(ui->do_all_btn, &QPushButton::clicked, [this]() {
        Cpu::GetInstance().Run(false);
        this->UpdateContents();
    });

    connect(ui->step_in_btn, &QPushButton::clicked, [this]() {
        Cpu::GetInstance().Run(true);
        this->UpdateContents();
    });
}

ProcessWindow::~ProcessWindow() { delete ui; }

void ProcessWindow::UpdateContents() {
    auto &process_scheduler = ProcessScheduler::GetInstance();
    {
        auto last_proc = process_scheduler.GetLastProcess();
        ui->pcb_info->clear();
        if (last_proc != nullptr) {
            char buffer[512]{};
            snprintf(buffer,
                     sizeof(buffer),
                     "PID: %lld\nName: %s\nState: %d\nStart Time: %lld\nTime "
                     "Slice: %lld\n",
                     last_proc->m_pcb.m_pid,
                     last_proc->m_pcb.m_name.c_str(),
                     static_cast<int>(last_proc->m_pcb.m_state),
                     last_proc->m_pcb.m_start_time,
                     last_proc->m_pcb.m_time_left);
            ui->pcb_info->setText(QString(buffer));
        }
    }
    {
        ui->current_inst->clear();
        auto &inst = Cpu::GetInstance().GetLastInstructionName();
        ui->current_inst->setText(inst.c_str());
    }
    {
        auto last_proc = process_scheduler.GetLastProcess();
        ui->inst_table->clear();
        if (last_proc != nullptr) {
            for (size_t i = 0; i < last_proc->m_process_instructions.size();
                 ++i) {
                std::string ret;
                if (i == last_proc->m_next_instruction)
                    ret = "-> " + last_proc->m_process_instructions[i].context;
                else
                    ret = "   " + last_proc->m_process_instructions[i].context;

                ui->inst_table->append(ret.c_str());
            }
        }
    }
    {
        ui->process_table->clear();
        auto &proc_list = process_scheduler.GetAllProcesses();
        for (auto &proc : proc_list) {
            char buf[128]{};
            snprintf(buf,
                     sizeof(buf),
                     "PID: %lld, Name: %s, state: %d, inst left: %zu\n",
                     proc.second.m_pcb.m_pid,
                     proc.second.m_pcb.m_name.c_str(),
                     static_cast<int>(proc.second.m_pcb.m_state),
                     proc.second.m_process_instructions.size() -
                         proc.second.m_next_instruction);
            ui->process_table->append(buf);
        }
    }
}
