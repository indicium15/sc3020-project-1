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
            return static_cast<int>(numKeys);
        };
        int getMaxKeys(){
            return static_cast<int>(maxKeys);
        };
        vector<Address> getChildren(int index){
            return children[index];
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
        int insert(float key, const vector<Address>& value);
        int insertInternal(float key, Node* parent, Node* child);
        int remove(float key);
        int findNodeForKey(float key, Node* currentNode);
        int removeInternal(float key, Node* parent, Node* child);
        void redistributeLeafNodes(Node* node);
        void redistributeInternalNodes(Node* node);
        void deleteNode(Node *node);
        void mergeLeafNodes(Node* node);
        void mergeInternalNodes(Node* node);
        void displayTree();
        void displayNode(Node* node, int level);
        BPlusTree();
        ~BPlusTree();
};

#endif