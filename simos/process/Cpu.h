#ifndef CPU_H
#define CPU_H

#include <QTimer>

#include <cstdint>
#include <functional>
#include <queue>
#include <string>
#include <vector>

class Cpu {
public:
    struct TimerCallback {
        int64_t               trigger_at;
        std::function<void()> callback;
    };

    Cpu() noexcept = default;

    Cpu(const Cpu &) = delete;
    Cpu &operator=(const Cpu &) = delete;

    Cpu(Cpu &&)  = delete;
    Cpu &operator=(Cpu &&) = delete;

    void Run(bool step, std::function<void()> flush_window) noexcept;

    int64_t GetCurrentTime() const noexcept { return m_circles; }

    const std::string &GetLastInstructionName() const noexcept {
        return m_last_inst;
    }

    void RegisterTimer(uint64_t after, std::function<void()> func) noexcept {
        m_timers.push(TimerCallback{m_circles + static_cast<int64_t>(after),
                                    std::move(func)});
    }

    static Cpu &GetInstance() noexcept;

private:
    void Tick() noexcept;

    struct TimerCallbackCompare {
        bool operator()(const TimerCallback &lhs,
                        const TimerCallback &rhs) const noexcept {
            return lhs.trigger_at < rhs.trigger_at;
        }
    };

private:
    int64_t     m_circles = 0;
    std::string m_last_inst;
    std::priority_queue<TimerCallback, std::vector<TimerCallback>,
                        TimerCallbackCompare>
        m_timers;

    QTimer m_timer;
};

#endif // CPU_H
