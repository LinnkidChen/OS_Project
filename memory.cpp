//
// Created by 86134 on 2022/4/5.
//

#include "memory.h"


void LRUCache::removeNode(Node* p)
{
    p->left->right = p->right;
    p->right->left = p->left;
}

void LRUCache::insertNode(Node* p)
{
    p->right = head->right;
    p->left = head;
    head->right->left = p;
    head->right = p;
}

LRUCache::LRUCache (int capacity)
{
    n = capacity;
    head = new Node(-1,-1);
    tail = new Node(-1,-1);
    head->right = tail;
    tail->left = head;
}
int LRUCache::get(int key)
{
    if(hash.count(key) == 0)return -1;
    Node* tmp = hash[key];
    removeNode(tmp);
    insertNode(tmp);
    return tmp->value;
}

void LRUCache::put(int key, int value)
{
    if(hash.count(key))
    {
        Node* tmp = hash[key];
        tmp->value = value;
        removeNode(tmp);
        insertNode(tmp);
    }
    else{
        if(hash.size() == n)
        {
            Node* tmp = tail->left;
            removeNode(tmp);
            hash.erase(tmp->key);
            delete tmp;
        }
        Node* tmp = new Node(key,value);
        hash[key] = tmp;
        insertNode(tmp);
    }
}

