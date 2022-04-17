#pragma once

#include "ConstQueue.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <vector>

namespace memory {

// 页大小，单位：字节
static constexpr const size_t MEMORY_PAGE_SIZE = 4096;
// 页的数量
static constexpr const size_t MEMORY_PAGE_NUM = 4096;
// 内存总大小
static constexpr const size_t MEMORY_POOL_SIZE =
    MEMORY_PAGE_NUM * MEMORY_PAGE_SIZE;

class MemoryPool;

/// 页面类，仅用于表示一个页的存储空间，这个类的大小为MEMORY_PAGE_SIZE字节。
/// 可平凡拷贝，可平凡析构，可以安全地直接获取并访问该类的地址。
class MemoryPage final {
public:
    MemoryPage() noexcept                   = default;
    MemoryPage(const MemoryPage &) noexcept = default;
    ~MemoryPage()                           = default;

    /// 获取这个页第pos个字节。pos应当总是小于页面大小。
    void *Get(size_t pos) noexcept {
        assert(pos < MEMORY_PAGE_SIZE);
        return std::addressof(m_page_data[pos]);
    }

    /// 获取这个页第pos个字节。pos应当总是小于页面大小。
    const void *Get(size_t pos) const noexcept {
        assert(pos < MEMORY_PAGE_SIZE);
        return std::addressof(m_page_data[pos]);
    }

    friend class MemoryPool;

private:
    std::array<uint8_t, MEMORY_PAGE_SIZE> m_page_data;
    void                                 *m_group = nullptr;
};

/// 指针异常，当指针操作发生非法访问时抛出。
/// NOTE: 直接访问进程管理杀死当前进程而非抛出异常。
class PointerException final : public std::exception {
public:
    PointerException() noexcept                         = default;
    PointerException(const PointerException &) noexcept = default;
    ~PointerException() noexcept override               = default;
    const char *what() const noexcept override { return "segmentation fault."; }
};

/// 指针类，指向一块模拟内存。
///
/// 该类模拟裸指针设计，设计上允许像裸指针一样访问。
/// 比如，使用Pointer<char>表示指向char的指针。
///
/// Pointer<char> ptr应当支持以下操作，->后面为返回类型：
/// 获取内容：
/// - *ptr   -> char &
/// - ptr[i] -> char &
/// - ptr->  -> char &（注意，第一个->是运算符，当T是struct或者class时有用）
/// 偏移：
/// - ptr++         -> Pointer<char>
/// - ptr--         -> Pointer<char>
/// - ++ptr         -> Pointer<char> &
/// - --ptr         -> Pointer<char> &
/// - ptr + offset  -> Pointer<char>，其中offset可以是任意整数类型
/// - ptr - offset  -> Pointer<char>，其中offset可以是任意整数类型
/// - ptr += offset -> Pointer<char> &，其中offset可以是任意整数类型
/// - ptr -= offset -> Pointer<char> &，其中offset可以是任意整数类型
/// 比较运算：
/// - ptr == ptr2 -> bool，其中ptr2是Pointer<char>，Pointer<void>类型或者nullptr
/// - ptr > ptr2 -> bool
/// - ptr < ptr2 -> bool
/// 以及其他指针支持的比较运算符。
///
/// WARN:
/// 如果T存储时需要跨页访问，可能会出现错误，因此尽可能使用char或uint8_t类型的Pointer。
template <typename T>
class Pointer final {
public:
    Pointer() noexcept                = default;
    Pointer(const Pointer &) noexcept = default;
    Pointer &operator=(const Pointer &) noexcept = default;

    Pointer(std::nullptr_t) noexcept : Pointer() {}

    template <typename U>
    explicit Pointer(Pointer<U> other) noexcept {
        m_ptr = reinterpret_cast<T *>(other.m_ptr);
    }

    /// ptr[i]
    T       &operator[](ptrdiff_t i);
    const T &operator[](ptrdiff_t i) const;

    /// *ptr
    T &operator*() { return operator[](0); }

    /// *ptr
    const T &operator*() const { return operator[](0); }

    /// ptr->
    T &operator->() { return this->operator*(); }

    /// ptr->
    const T &operator->() const { return this->operator*(); }

    // ptr += offset
    Pointer &operator+=(ptrdiff_t offset);

    // ptr -= offset
    Pointer &operator-=(ptrdiff_t offset) { return operator+=(-offset); }

    // ++ptr
    Pointer &operator++() {
        operator+=(1);
        return (*this);
    }

    // ptr++
    Pointer operator++(int) {
        auto ret = *this;
        ++(*this);
        return ret;
    }

    // --ptr
    Pointer &operator--() {
        operator-=(1);
        return (*this);
    }

    // ptr--
    Pointer operator--(int) {
        auto ret = *this;
        --(*this);
        return ret;
    }

    /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
    T *Get() noexcept { return m_ptr; }
    /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
    const T *Get() const noexcept { return m_ptr; }

    template <typename>
    friend class Pointer;

private:
    T *m_ptr = nullptr;
};

template <>
class Pointer<void> {
public:
    Pointer() noexcept                = default;
    Pointer(const Pointer &) noexcept = default;
    Pointer &operator=(const Pointer &) noexcept = default;

    Pointer(std::nullptr_t) noexcept : Pointer() {}

    template <typename U>
    Pointer(Pointer<U> other) noexcept {
        m_ptr = other.m_ptr;
    }

    /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
    void *Get() noexcept { return m_ptr; }
    /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
    const void *Get() const noexcept { return m_ptr; }

    friend class MemoryPool;

    template <typename>
    friend class Pointer;

private:
    void *m_ptr = nullptr;
};

template <typename T>
Pointer<T> operator+(Pointer<T> lhs, ptrdiff_t rhs) {
    lhs += rhs;
    return lhs;
}

template <typename T>
Pointer<T> operator-(Pointer<T> lhs, ptrdiff_t rhs) {
    lhs -= rhs;
    return lhs;
}

template <typename T>
bool operator==(Pointer<T> lhs, Pointer<T> rhs) noexcept {
    return lhs.Get() == rhs.Get();
}

template <typename T>
bool operator!=(Pointer<T> lhs, Pointer<T> rhs) noexcept {
    return lhs.Get() != rhs.Get();
}

template <typename T>
bool operator<(Pointer<T> lhs, Pointer<T> rhs) noexcept {
    return lhs.Get() < rhs.Get();
}

template <typename T>
bool operator>(Pointer<T> lhs, Pointer<T> rhs) noexcept {
    return lhs.Get() > rhs.Get();
}

template <typename T>
bool operator<=(Pointer<T> lhs, Pointer<T> rhs) noexcept {
    return lhs.Get() <= rhs.Get();
}

template <typename T>
bool operator>=(Pointer<T> lhs, Pointer<T> rhs) noexcept {
    return lhs.Get() >= rhs.Get();
}

/// 内存池，实际上的内存管理类。
///
/// 内存管理方法：
///     调用者调用Allocate(size)方法申请内存，申请内存时会先判断有没有足够的内存用来分配。
/// 如果没有足够的内存分配，则返回nullptr。如果有足够的内存，则从空闲列表中划出足够的内存，
/// 然后从空闲组号中取一个组号，将这些内存标记为一组，表示一次分配。同时，标记这些内存块对应的
/// 组号。
///     释放内存时，首先根据传入的指针，计算相对于内存池的偏移量，判断这个指针属于哪个内存块。
/// 然后根据内存块找到所属的组，将整个组的内存一起标记为释放。释放时，先将内存中所有的页归还到
/// freeList中，然后归还组号。
///
/// MemoryPool类是Singleton类，全局只有一个MemoryPool即可。
/// 使用MemoryPool时，调用MemoryPool::GetInstance()方法来获取实例。
class MemoryPool final {
public:
    using page_type  = MemoryPage;
    using group_type = std::vector<page_type *>;

    MemoryPool();

    MemoryPool(const MemoryPool &) = delete;
    MemoryPool &operator=(const MemoryPool &) = delete;

    MemoryPool(MemoryPool &&) = delete;
    MemoryPool &operator=(MemoryPool &&) = delete;

    ~MemoryPool() = default;

    /// 申请size byte内存。如果内存不足，返回nullptr。
    /// 虚拟内存还没实现，需要依赖文件管理。调页算法待虚拟内存实现以后再说。
    Pointer<void> Allocate(size_t size);

    /// 释放pointer。
    void Deallocate(Pointer<void> pointer);

    static MemoryPool &GetInstance() noexcept;

    template <typename T>
    friend class Pointer;

private:
    page_type *GetPageAfter(page_type *page, int64_t offset) const noexcept;

    page_type *GetBelongingPage(Pointer<void> ptr) noexcept;

private:
    std::array<page_type, MEMORY_PAGE_NUM>   m_pages;
    ConstQueue<page_type *, MEMORY_PAGE_NUM> m_free_pages;

    std::array<group_type, MEMORY_PAGE_NUM>   m_groups;
    ConstQueue<group_type *, MEMORY_PAGE_NUM> m_free_groups;
};

template <typename T>
T &Pointer<T>::operator[](ptrdiff_t index) {
    auto      byte_ptr   = reinterpret_cast<uint8_t *>(Get());
    ptrdiff_t byte_index = index * sizeof(T);

    MemoryPage *page       = MemoryPool::GetInstance().GetBelongingPage(*this);
    auto        page_start = static_cast<uint8_t *>(page->Get(0));

    // 位于同一页
    if (byte_ptr + byte_index >= page_start &&
        byte_ptr + byte_index < page_start + MEMORY_PAGE_SIZE) {
        byte_ptr += byte_index;
        return *reinterpret_cast<T *>(byte_ptr);
    } else {
        byte_index += (byte_ptr - page_start);
        int32_t page_after = byte_index / MEMORY_PAGE_SIZE;

        if (byte_index < 0)
            --page_after;

        auto dest_page =
            MemoryPool::GetInstance().GetPageAfter(page, page_after);
        if (dest_page == nullptr) {
            throw PointerException();
        }

        ptrdiff_t dest_page_offset =
            ((byte_index % MEMORY_PAGE_SIZE) + MEMORY_PAGE_SIZE) %
            MEMORY_PAGE_SIZE;

        return *static_cast<T *>(dest_page->Get(dest_page_offset));
    }
}

template <typename T>
const T &Pointer<T>::operator[](ptrdiff_t index) const {
    auto      byte_ptr   = reinterpret_cast<uint8_t *>(Get());
    ptrdiff_t byte_index = index * sizeof(T);

    MemoryPage *page       = MemoryPool::GetInstance().GetBelongingPage(*this);
    auto        page_start = static_cast<uint8_t *>(page->Get(0));

    // 位于同一页
    if (byte_ptr + byte_index >= page_start &&
        byte_ptr + byte_index < page_start + MEMORY_PAGE_SIZE) {
        byte_ptr += byte_index;
        return *reinterpret_cast<T *>(byte_ptr);
    } else {
        byte_index += (m_ptr - page_start);
        int32_t page_after = byte_index / MEMORY_PAGE_SIZE;

        if (byte_index < 0)
            --page_after;

        auto dest_page =
            MemoryPool::GetInstance().GetPageAfter(page, page_after);

        if (dest_page == nullptr)
            throw PointerException();

        ptrdiff_t dest_page_offset =
            ((byte_index % MEMORY_PAGE_SIZE) + MEMORY_PAGE_SIZE) %
            MEMORY_PAGE_SIZE;

        return *static_cast<const T *>(dest_page->Get(dest_page_offset));
    }
}

template <typename T>
Pointer<T> &Pointer<T>::operator+=(ptrdiff_t offset) {
    try {
        m_ptr = &(this->operator[](offset));
    } catch (PointerException) {
        m_ptr = nullptr;
    }

    return (*this);
}

} // namespace memory
