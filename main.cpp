#include <iostream>
#include "memory.h"


int main() {
    LRUCache* cache = new LRUCache(2);
    cache->put(1,1);
    cache->put(2,2);
    std::cout << "get:" << cache->get(1) << std::endl;
    cache->put(3,3);
    std::cout << "get:" << cache->get(2) << std::endl;
    cache->put(4,4);
    std::cout << "get:" << cache->get(1) << std::endl;
    std::cout << "get:" << cache->get(3) << std::endl;
    std::cout << "get:" << cache->get(4) << std::endl;
    return 0;
}
