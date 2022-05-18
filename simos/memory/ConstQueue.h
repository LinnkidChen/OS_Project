#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace memory {

/// 大小固定的queue，用法和std::queue差不多，只不过多了一个full()方法用来判断
/// 队列是否已满。
    template <typename T, size_t SIZE>
    class ConstQueue {
    public:
        using container_type  = std::array<T, SIZE + 1>;
        using value_type      = typename container_type::value_type;
        using size_type       = typename container_type::size_type;
        using reference       = typename container_type::reference;
        using const_reference = typename container_type::const_reference;

        ConstQueue() : ConstQueue(container_type()) {}
        explicit ConstQueue(const container_type &cont) : queueData(cont) {}

        ConstQueue(const ConstQueue &) noexcept(
        std::is_nothrow_copy_constructible<T>::value) = default;

        ConstQueue &operator=(const ConstQueue &) noexcept(
        std::is_nothrow_copy_assignable<T>::value) = default;

        ConstQueue(ConstQueue &&) noexcept(
        std::is_nothrow_move_constructible<T>::value) = default;

        ConstQueue &operator                           =(ConstQueue &&) noexcept(
        std::is_nothrow_move_assignable<T>::value) = default;

        ~ConstQueue() = default;

        reference front() {
            assert(!empty());
            return queueData[head];
        }

        const_reference front() const {
            assert(!empty());
            return queueData[head];
        }

        reference back() {
            assert(!empty());
            return queueData[(tail + queueData.size() - 1) % SIZE];
        }

        const_reference back() const {
            assert(!empty());
            return queueData[(tail + queueData.size() - 1) % SIZE];
        }

        bool empty() const noexcept { return size() == 0; }

        bool full() const noexcept { return size() >= SIZE; }

        size_t size() const noexcept {
            return (tail + queueData.size() - head) % queueData.size();
        }

        void push(const value_type &value) { emplace(value); }
        void push(value_type &&value) { emplace(std::move(value)); }

        template <typename... Args>
        void emplace(Args &&...args) noexcept(
        std::is_nothrow_constructible<T, decltype(args)...>::value) {
            assert(!full());
            new (std::addressof(queueData[tail]))
                    value_type(std::forward<Args>(args)...);
            tail = (tail + 1) % queueData.size();
        }

        void pop() {
            if (head != tail) {
                queueData[head].~value_type();
                head = (head + 1) % queueData.size();
            }
        }

    private:
        size_t         head = 0;
        size_t         tail = 0;
        container_type queueData;
    };

} // namespace memory
