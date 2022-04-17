#include "MemoryManagement.h"

memory::MemoryPool::MemoryPool() {
    m_pages.fill(page_type());
    for (auto &page : m_pages) {
        m_free_pages.push(&page);
    }

    m_groups.fill(group_type());
    for (auto &group : m_groups) {
        m_free_groups.push(&group);
    }
}

static size_t GetPageNum(size_t size) noexcept {
    return ((size + memory::MEMORY_PAGE_SIZE - 1) / memory::MEMORY_PAGE_SIZE);
}

auto memory::MemoryPool::GetInstance() noexcept -> MemoryPool & {
    static MemoryPool instance{};
    return instance;
}

auto memory::MemoryPool::Allocate(size_t size) -> Pointer<void> {
    size_t num_page = GetPageNum(size);

    if (m_free_groups.empty()) {
        return nullptr;
    }

    if (m_free_pages.size() < num_page) {
        return nullptr;
    }

    group_type *group = m_free_groups.front();
    m_free_groups.pop();

    group->clear();

    for (size_t i = 0; i < num_page; ++i) {
        page_type *page = m_free_pages.front();
        m_free_pages.pop();

        page->m_group = group;
        group->push_back(page);
    }

    if (!group->empty()) {
        Pointer<void> ret;
        ret.m_ptr = group->front()->Get(0);
        return ret;
    } else {
        return nullptr;
    }
}

auto memory::MemoryPool::GetBelongingPage(memory::Pointer<void> ptr) noexcept
    -> page_type * {
    auto byte_ptr = static_cast<uint8_t *>(ptr.Get());

    ptrdiff_t offset  = byte_ptr - static_cast<uint8_t *>(m_pages[0].Get(0));
    int32_t   page_id = offset / sizeof(MemoryPage);

    if (page_id < 0 || page_id >= static_cast<int32_t>(MEMORY_PAGE_NUM)) {
        return nullptr;
    }

    return &m_pages[page_id];
}

void memory::MemoryPool::Deallocate(Pointer<void> pointer) {
    page_type *page = GetBelongingPage(pointer);
    if (page == nullptr) {
        return;
    }

    auto group = static_cast<group_type *>(page->m_group);
    for (auto &i : *group) {
        i->m_group = nullptr;
        m_free_pages.push(i);
    }

    group->clear();
    m_free_groups.push(group);
}

auto memory::MemoryPool::GetPageAfter(page_type *page,
                                      int64_t    offset) const noexcept
    -> page_type * {
    assert(page != nullptr);
    auto    group      = static_cast<group_type *>(page->m_group);
    int64_t page_index = -1;

    for (size_t i = 0; i < group->size(); ++i) {
        if ((*group)[i] == page) {
            page_index = static_cast<int64_t>(i);
            break;
        }
    }

    if (page_index == -1)
        return nullptr;

    page_index += offset;
    if (page_index < 0 || page_index >= static_cast<int64_t>(group->size()))
        return nullptr;

    return (*group)[page_index];
}
