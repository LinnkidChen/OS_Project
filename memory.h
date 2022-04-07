//
// Created by 86134 on 2022/4/5.
//

#ifndef MEMORY_MEMORY_H
#define MEMORY_MEMORY_H

#include<unordered_map>
#include<stdlib.h>

class LRUCache{
public:
    struct Node{
        int key,value;
        Node *left,*right;
        Node(int _key,int _value): key(_key),value(_value),left(NULL),right(NULL){}
    };
    int n;
    std::unordered_map<int,Node*> hash;
    struct Node *head,*tail;
    void removeNode(Node* p);
    void insertNode(Node* p);
    LRUCache(int capacity);
    int get(int key);
    void put(int key, int value);

};



#endif //MEMORY_MEMORY_H
