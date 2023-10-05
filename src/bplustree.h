#ifndef BPLUSTREE
#define BPLUSTREE

#include <cstdint>
#include "record.h"

using namespace std;

// class Address{
//     private:
//         void* blockAddress;
//         int offset;
//     public:
//         Address(Record record);
//         Address();
//         ~Address();
// };

class Node{
    private:
        float *keys;
        uint8_t numKeys;
        uint8_t maxKeys;
        vector<vector<Address>> children;
        bool isLeaf;
        friend class BPlusTree;
    public:
        Node(int maxKeys, bool isLeaf);
        ~Node();
};

class BPlusTree{
    public:
        Node* rootNode;
        int keysStored;
        int maxKeys;
        int levels;
        int nodesStored;
        int nodeSize;
        int blockSize;
        int insert(float key, Address value);
        int remove(float key);
        void displayTree();
        void displayNode();
        BPlusTree();
        ~BPlusTree();
};

#endif