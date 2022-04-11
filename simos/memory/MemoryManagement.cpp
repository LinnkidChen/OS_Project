#include "MemoryManagement.h"

namespace memory {

MemoryPool &MemoryPool::GetInstance() noexcept {
    static MemoryPool memoryPool;
    return memoryPool;
}

auto MemoryPool::GetBelongingPageId(Pointer<void> ptr) noexcept -> PageId {
    if (ptr.Get() < std::addressof(pages[0]))
        return INVALID_PAGE_ID;

    auto byte_ptr   = static_cast<uint8_t *>(ptr.Get());
    auto page_start = reinterpret_cast<uint8_t *>(std::addressof(pages[0]));

    // 根据偏移量计算所属的页编号
    size_t diff = byte_ptr - page_start;
    PageId id   = diff / MEMORY_PAGE_SIZE;

    if (id >= pages.size())
        return INVALID_PAGE_ID;

    return id;
}

auto MemoryPool::GetFreeGroupId() noexcept -> PageGroupId {
    if (freeGroupIdList.empty())
        return INVALID_PAGE_GROUP_ID;

    PageGroupId id = freeGroupIdList.front();
    freeGroupIdList.pop();
    return id;
}

void MemoryPool::ReleaseGroupId(PageGroupId id) noexcept {
    freeGroupIdList.push(id);
}

auto MemoryPool::GetFreePages(size_t num) noexcept -> std::vector<PageId> {
    if (freeList.size() < num)
        return {};

    std::vector<PageId> ret{num};
    for (size_t i = 0; i < num; ++i) {
        ret.push_back(freeList.front());
        freeList.pop();
    }

    return ret;
}

void MemoryPool::ReleasePages(const std::vector<PageId> &pageList) noexcept {
    for (const auto &id : pageList) {
        assert(id >= 0 && id < pages.size());
        freeList.push(id);
    }
}

size_t MemoryPool::RoundUpPageNum(size_t memorySize) noexcept {
    return ((memorySize + MEMORY_PAGE_SIZE - 1) / MEMORY_PAGE_SIZE);
}

MemoryPool::MemoryPool() {
    for (size_t i = 0; i < MEMORY_PAGE_NUM; ++i) {
        freeList.push(i);
        freeGroupIdList.push(i);
    }
}

Pointer<void> MemoryPool::Allocate(size_t size) {
    // 申请页组编号
    PageGroupId currentGroupId = GetFreeGroupId();
    if (currentGroupId == INVALID_PAGE_GROUP_ID)
        return nullptr;

    // 申请页面
    size_t realSize        = RoundUpPageNum(size);
    auto   currentPageList = GetFreePages(realSize);
    if (currentPageList.empty()) {
        ReleaseGroupId(currentGroupId);
        return nullptr;
    }

    void *resultPage = pages[currentPageList[0]].Get(0);

    for (const auto &id : currentPageList) {
        pageGroupId[id] = currentGroupId;
    }
    pageGroups[currentGroupId] = std::move(currentPageList);

    return {resultPage};
}

void MemoryPool::Deallocate(Pointer<void> pointer) {
    PageId currentPageId = GetBelongingPageId(pointer);
    if (currentPageId == INVALID_PAGE_ID)
        return;

    // 归还页面
    PageGroupId currentGroupId = pageGroupId[currentPageId];
    ReleasePages(pageGroups[currentGroupId]);
    pageGroups[currentGroupId].clear();

    // 归还页面组号
    ReleaseGroupId(currentGroupId);
}

} // namespace memory
