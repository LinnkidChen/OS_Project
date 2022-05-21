#include "MemoryManagement.h"
#include<iostream>

memory::MemoryPool::MemoryPool() {

    /*LRU*/
    head = new Node(-1);
    tail = new Node(-1);
    head->right = tail;
    tail->left = head;

    /*物理内存*/
    m_pages.fill(page_type());
    for(size_t i = 0; i < m_pages.size(); i++)
    {
        m_pages[i].page_num = i;
    }

    for (auto &page : m_pages) {
        m_free_pages.push(&page);
    }

    /*已经分配的内存*/
    m_groups.fill(group_type());

    for (auto &group : m_groups) {
        m_free_groups.push(&group);
    }

    /*虚拟内存*/
    virtual_pages.fill(page_type());
    for(size_t i = 0; i < virtual_pages.size(); i++)
    {
        virtual_pages[i].page_num = i;
    }
    for(auto &page: virtual_pages)
    {
        free_virtual_pages.push(&page);
    }

    /*交换文件*/
    swap_file.fill(page_type());
    for(size_t i = 0; i < swap_file.size(); i++)
    {
        swap_file[i].page_num = i;
    }
    for(auto &page: swap_file)
    {
        free_swap_file.push(&page);
    }

    /*页表初始化为-1*/
    for(auto &page : m_pages)
    {
        page_table.insert(std::make_pair(page.page_num,-1));
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
        replacePage(num_page - m_free_pages.size());
    }


    group_type *group = m_free_groups.front();
    m_free_groups.pop();
    group->clear();

    for (size_t i = 0; i < num_page; ++i) {
        page_type *page = m_free_pages.front();
        m_free_pages.pop();

        page_type *v_page = free_virtual_pages.front();
        free_virtual_pages.pop();

        get(page->page_num);
        page_table[page->page_num] = v_page->page_num;

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

    size_t virtual_page_num = page_table[page->page_num];
    page_table[page->page_num] = -1;

    auto group = static_cast<group_type *>(page->m_group);
    for (auto &i : *group) {
        i->m_group = nullptr;
        m_free_pages.push(i);
    }


//    auto v_page = &virtual_pages[virtual_page_num];
//    auto v_group = static_cast<group_type *>(v_page->m_group);
//    for (auto &i : *v_group) {
//        i->m_group = nullptr;
//        free_virtual_pages.push(i);
//    }



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


void memory::MemoryPool::removeNode(Node* p)
{
    p->left->right = p->right;
    p->right->left = p->left;
}

void memory::MemoryPool::insertNode(Node* p)
{
    p->right = head->right;
    p->left = head;
    head->right->left = p;
    head->right = p;
}

int memory::MemoryPool::get(int p_num)
{
    if(hash.count(p_num) == 0)return -1;
    Node* tmp = hash[p_num];
    removeNode(tmp);
    insertNode(tmp);
    return tmp->page_num;
}

void memory::MemoryPool::put(int p_num) {
    if (hash.count(p_num)) {
        Node *tmp = hash[p_num];
        removeNode(tmp);
        insertNode(tmp);
    } else {
        if (hash.size() == VIRTUAL_PAGE_NUM) {
            Node *tmp = tail->left;
            removeNode(tmp);
            hash.erase(tmp->page_num);
            delete tmp;
        }
        Node *tmp = new Node(p_num);
        hash[p_num] = tmp;
        insertNode(tmp);
    }
}

size_t memory::MemoryPool::replacePage(size_t needSize) noexcept
{
    size_t replace_page_num;
    size_t replace_virtual_page_num;
    for(size_t i = 0; i < needSize; i++)
    {
        replace_page_num = tail->left->page_num;
        replace_virtual_page_num = page_table[replace_page_num];
        if(page_table[replace_page_num] != -1)
        {
            page_type *vpage = free_swap_file.front();
            free_swap_file.pop();

            swap_file[replace_page_num] = m_pages[replace_page_num];
            m_free_pages.push(&m_pages[replace_page_num]);

            removeNode(hash[replace_page_num]);

            //调入virtual
            replace_virtual_page_num = vpage->page_num;
        }

    }
    return replace_page_num;

}

void memory::MemoryPool::test(size_t size) {
    for(auto it = page_table.begin(); it != page_table.end(); ++it)
    {
        std::cout<<it->first<<" "<<it->second<<std::endl;
    }


}



