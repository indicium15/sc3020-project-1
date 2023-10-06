#ifndef BPLUSTREE
#define BPLUSTREE

#include <cstdint>
#include "record.h"
#include "types.h"

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

        float getKey(int index){
            return keys[index];
        };
        int getNumKeys(){
            return (int)(numKeys);
        };
        int getMaxKeys(){
            return (int)(maxKeys);
        };
        vector<Address> getChildren(int index){
            return children[index];
        };
        Address getChild(int index, int childIndex){
            if(children[index][childIndex].blockAddress != nullptr){
                return children[index][childIndex];
            }
            else
                return Address(nullptr, 0);
        };
        int getIsLeaf(){
            return isLeaf;
        };
        void setKey(int index, float key){
            keys[index] = key;
        };
        void setNumKeys(int numKeys){
            this->numKeys = numKeys;
        };
        void setChildren(int index, vector<Address> children){
            this->children[index] = children;
        };
        void setIsLeaf(bool isLeaf){
            this->isLeaf = isLeaf;
        };
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
        int insert(float key, const vector<Address> value);
        int insertInternal(float key, Node* parent, Node* child);
        int remove(float key);
        void displayTree(Node *cursor, int level);
        void displayNode(Node* node);
        
        BPlusTree();
        ~BPlusTree();
};

#endif