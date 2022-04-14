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

// 页大小，单位：字节
    static constexpr const size_t MEMORY_PAGE_SIZE = 4096; // byte
// 页的数量
    static constexpr const size_t MEMORY_PAGE_NUM = 4096;
// 内存总大小
    static constexpr const size_t MEMORY_POOL_SIZE =
            MEMORY_PAGE_NUM * MEMORY_PAGE_SIZE;

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
            return std::addressof(pageData[pos]);
        }

        /// 获取这个页第pos个字节。pos应当总是小于页面大小。
        const void *Get(size_t pos) const noexcept {
            assert(pos < MEMORY_PAGE_SIZE);
            return std::addressof(pageData[pos]);
        }

    private:
        std::array<uint8_t, MEMORY_PAGE_SIZE> pageData;
    };

/// 指针异常，当指针操作发生非法访问时抛出。
/// NOTE: 直接访问进程管理杀死当前进程而非抛出异常。
    class PointerException final : public std::exception {
    public:
        PointerException() noexcept                         = default;
        PointerException(const PointerException &) noexcept = default;
        ~PointerException() noexcept override = default;
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
        explicit Pointer(const Pointer<U> &other) noexcept {
            pointer = other.pointer;
        }

        /// ptr[i]
        T &operator[](ptrdiff_t i);

        const T &operator[](ptrdiff_t i) const;

        /// *ptr
        T &operator*() { return operator[](0); }

        /// *ptr
        const T &operator*() const { return operator[](0); }

        /// ptr->
        T &operator->() { return this->operator*(); }

        /// ptr->
        const T &operator->() const { return this->operator*(); }

        /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
        T *Get() noexcept { return pointer; }
        /// 注意：获取空指针不要直接使用算数操作获取地址，这是不安全的操作。
        const T *Get() const noexcept { return pointer; }

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

    private:
        T *pointer = nullptr;
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

/// void *类型的指针，可以与其他任意类型的指针转换。
/// 不能进行算数运算，不能获取内容。
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
        // 页号
        using PageId = size_t;
        // 组号
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

        // 考虑到Pointer可能需要MemoryPool中组号相关的信息来计算偏移量，
        // 故允许Pointer类为友元类。
        template <typename T>
        friend class Pointer;


    private:
        /// 如果ptr非法，则返回INVALID_PAGE_ID，否则返回ptr从属的页的编号。
        PageId GetBelongingPageId(Pointer<void> ptr) noexcept;

        /// 如果没有空闲的group id，返回INVALID_PAGE_GROUP_ID。
        PageGroupId GetFreeGroupId() noexcept;

        /// 释放该group id。
        void ReleaseGroupId(PageGroupId id) noexcept;

        /// 获取num个空闲页。如果剩余的空闲页不足，则返回一个空列表。
        std::vector<PageId> GetFreePages(size_t num) noexcept;

        /// 释放pageList中所有的页。
        void ReleasePages(const std::vector<PageId> &pageList) noexcept;

        /// 用于计算需要分配申请多少页的内存。
        static size_t RoundUpPageNum(size_t memorySize) noexcept;

    private:
        // 当前空闲的页的编号。
        ConstQueue<PageId, MEMORY_PAGE_NUM> freeList;
        // 所有的内存页。
        std::array<MemoryPage, MEMORY_PAGE_NUM> pages;

        // 空闲的组的编号。保证GroupId <= MEMORY_PAGE_NUM恒成立。
        ConstQueue<PageGroupId, MEMORY_PAGE_NUM> freeGroupIdList;
        // 用于存储每个组都有哪些页。
        std::array<std::vector<PageId>, MEMORY_PAGE_NUM> pageGroups;
        // 用于标记每个页属于哪个组。
        std::array<PageGroupId, MEMORY_PAGE_NUM> pageGroupId;
    };

    template <typename T>
    T &Pointer<T>::operator[](ptrdiff_t i) {
        // TODO: Implement this.
        PointerException pointer_Exception;
        if(i < 0 || i > MEMORY_POOL_SIZE){
            return pointer_Exception.what();
        }
        else {
            MemoryPool pool = MemoryPool::GetInstance();
            auto ptr = std::addressof(pointer);//该指针地址
            auto pageNum = pool.GetBelongingPageId(pointer);//该指针对应的页号
            auto pageStart = std::addressof(pool.pages[pageNum]);//该页的开始地址
            if(ptr + i <= pageStart + MEMORY_PAGE_SIZE)//不跨页
            {
                return pointer+i;
            }
            else
            {
                auto nextPageNum = pageNum + (i+ptr-pageStart)/MEMORY_PAGE_SIZE;//偏移后的页号
                assert(nextPageNum >= 0 && nextPageNum < MEMORY_PAGE_NUM);
                auto nextPageStart = std::addressof(pool.pages[nextPageNum]);//偏移后的页号的开始地址
                return nextPageStart + (i+ptr-pageStart)%MEMORY_PAGE_SIZE;
            }

        // 注意需要考虑地址非法的情况、地址为负数的情况和跨页的情况。
    }

    template <typename T>
    const T &Pointer<T>::operator[](ptrdiff_t i) const {
        // TODO: Implement this.
        PointerException pointer_Exception;
        if(i < 0 || i > MEMORY_POOL_SIZE){
            return pointer_Exception.what();
        }
        else {
            MemoryPool pool = MemoryPool::GetInstance();
            auto ptr = std::addressof(pointer);//该指针地址
            auto pageNum = pool.GetBelongingPageId(pointer);//该指针对应的页号
            auto pageStart = std::addressof(pool.pages[pageNum]);//该页的开始地址
            if(ptr + i <= pageStart + MEMORY_PAGE_SIZE)//不跨页
            {
                return pointer+i;
            }
            else
            {
                auto nextPageNum = pageNum + (i+ptr-pageStart)/MEMORY_PAGE_SIZE;//偏移后的页号
                assert(nextPageNum >= 0 && nextPageNum < MEMORY_PAGE_NUM);
                auto nextPageStart = std::addressof(pool.pages[nextPageNum]);//偏移后的页号的开始地址
                return nextPageStart + (i+ptr-pageStart)%MEMORY_PAGE_SIZE;
            }
        }
        // 注意需要考虑地址非法的情况、地址为负数的情况和跨页的情况。
        // 实现完毕后直接复制上面的代码即可。
    }

    template <typename T>
    auto Pointer<T>::operator+=(ptrdiff_t offset) -> Pointer & {
        // TODO: Implement this.
        // 注意，处理偏移时需要考虑跨页的情况和offset为负数的情况。
        MemoryPool pool = MemoryPool::GetInstance();
        auto ptr = std::addressof(pointer);//该指针地址
        auto pageNum = pool.GetBelongingPageId(pointer);//该指针对应的页号
        auto pageStart = std::addressof(pool.pages[pageNum]);//该页的开始地址
        if(offset >= 0){
            if(ptr + offset <= pageStart + MEMORY_PAGE_SIZE)//不跨页
            {
                pointer += offset;
            }
            else
            {
                auto nextPageNum = pageNum + (offset+ptr-pageStart)/MEMORY_PAGE_SIZE;//偏移后的页号
                assert(nextPageNum >= 0 && nextPageNum < MEMORY_PAGE_NUM);
                auto nextPageStart = std::addressof(pool.pages[nextPageNum]);//偏移后的页号的开始地址
                pointer = nextPageStart + (offset+ptr-pageStart)%MEMORY_PAGE_SIZE;
            }
        }
        else{
            if(ptr + offset <= pageStart)//不跨页
            {
                pointer += offset;
            }
            else
            {
                auto nextPageNum = pageNum - (-offset+ptr-pageStart)/MEMORY_PAGE_SIZE;
                assert(nextPageNum >= 0 && nextPageNum < MEMORY_PAGE_NUM);
                auto nextPageStart = std::addressof(pool.pages[nextPageNum]);
                pointer = nextPageStart + (-offset+ptr-pageStart)%MEMORY_PAGE_SIZE + MEMORY_PAGE_SIZE;
            }
        }

    }

} // namespace memory
