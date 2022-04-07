#pragma once

#include "ConstQueue.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <map>
#include <mutex>
#include <vector>

namespace memory {

static constexpr const size_t MEMORY_PAGE_SIZE = 4096; // byte
static constexpr const size_t MEMORY_PAGE_NUM  = 4096;
static constexpr const size_t MEMORY_POOL_SIZE =
    MEMORY_PAGE_NUM * MEMORY_PAGE_SIZE;

class MemoryPage final {
public:
    MemoryPage() noexcept                   = default;
    MemoryPage(const MemoryPage &) noexcept = default;
    ~MemoryPage()                           = default;

    void *Get(size_t pos) noexcept {
        assert(pos < MEMORY_PAGE_SIZE);
        return std::addressof(pageData[pos]);
    }

    const void *Get(size_t pos) const noexcept {
        assert(pos < MEMORY_PAGE_SIZE);
        return std::addressof(pageData[pos]);
    }

private:
    std::array<uint8_t, MEMORY_PAGE_SIZE> pageData;
};

class PointerException final : public std::exception {
public:
    PointerException() noexcept                         = default;
    PointerException(const PointerException &) noexcept = default;

    ~PointerException() noexcept override = default;

    const char *what() const noexcept override { return "segmentation fault."; }
};

template <typename T>
class Pointer final {
public:
    Pointer() noexcept                = default;
    Pointer(const Pointer &) noexcept = default;
    Pointer &operator=(const Pointer &) noexcept = default;

    Pointer(std::nullptr_t) noexcept : Pointer() {}

    template <typename U>
    explicit Pointer(const Pointer<U> &other) noexcept {
        pointer = other.pointer;
    }

    /// 如果pointer地址非法，则抛出memory::PointerException异常。
    /// 抛出异常主要是用于通知进程调度发生了非法内存访问，终止当前进程。
    T &operator&();

    /// 如果pointer地址非法，则抛出memory::PointerException异常。
    /// 抛出异常主要是用于通知进程调度发生了非法内存访问，终止当前进程。
    const T &operator&() const;

    /// 如果pointer地址非法，则抛出memory::PointerException异常。
    /// 抛出异常主要是用于通知进程调度发生了非法内存访问，终止当前进程。
    T &operator->();

    /// 如果pointer地址非法，则抛出memory::PointerException异常。
    /// 抛出异常主要是用于通知进程调度发生了非法内存访问，终止当前进程。
    const T &operator->() const;

    /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
    T *Get() noexcept { return pointer; }
    /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
    const T *Get() const noexcept { return pointer; }

private:
    T *pointer = nullptr;
};

template <>
class Pointer<void> final {
public:
    Pointer() noexcept                = default;
    Pointer(const Pointer &) noexcept = default;
    Pointer &operator=(const Pointer &) noexcept = default;

    Pointer(std::nullptr_t) noexcept {}

    template <typename U>
    Pointer(const Pointer<U> &other) noexcept {
        pointer = other.pointer;
    }

    /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
    void *Get() noexcept { return pointer; }
    /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
    const void *Get() const noexcept { return pointer; }

    friend class MemoryPool;

private:
    Pointer(void *ptr) noexcept : pointer(ptr) {}

    void *pointer = nullptr;
};

template <typename T>
bool operator==(Pointer<T> lhs, Pointer<T> rhs) noexcept {
    return lhs.Get() == rhs.Get();
}

template <typename T>
bool operator!=(Pointer<T> lhs, Pointer<T> rhs) noexcept {
    return lhs.Get() != rhs.Get();
}

class MemoryPool final {
public:
    using PageId      = size_t;
    using PageGroupId = size_t;

    static constexpr const PageId INVALID_PAGE_ID       = -1;
    static constexpr const PageId INVALID_PAGE_GROUP_ID = -1;

    MemoryPool();

    /// 申请size byte内存。如果内存不足，返回nullptr。
    /// 虚拟内存还没实现，需要依赖文件管理。调页算法待虚拟内存实现以后再说。
    Pointer<void> Allocate(size_t size);

    /// 释放pointer。
    void Deallocate(Pointer<void> pointer);

    static MemoryPool &GetInstance() noexcept;

    template <typename T>
    friend class Pointer;

private:
    /// 如果ptr非法，则返回INVALID_PAGE_ID，否则返回ptr从属的页的编号。
    PageId GetBelongingPageId(Pointer<void> ptr) noexcept;

    /// 如果没有空闲的group id，返回INVALID_PAGE_GROUP_ID。
    PageGroupId GetFreeGroupId() noexcept;
    void        ReleaseGroupId(PageGroupId id) noexcept;

    /// 如果剩余的空闲页不足，则返回一个空列表。
    std::vector<PageId> GetFreePages(size_t num) noexcept;
    void ReleasePages(const std::vector<PageId> &pageList) noexcept;

    /// 用于计算需要分配申请多少页的内存。
    static size_t RoundUpPageNum(size_t memorySize) noexcept;

private:
    ConstQueue<PageId, MEMORY_PAGE_NUM>     freeList;
    std::array<MemoryPage, MEMORY_PAGE_NUM> pages;

    // 保证GroupId <= MEMORY_PAGE_NUM恒成立。
    ConstQueue<PageGroupId, MEMORY_PAGE_NUM>         freeGroupIdList;
    std::array<std::vector<PageId>, MEMORY_PAGE_NUM> pageGroups;
    std::array<PageGroupId, MEMORY_PAGE_NUM>         pageGroupId;
};

} // namespace memory
