#include "bplustree.h";
#include "storage.h";
#include <iostream>;
#include "types.h";

// Address::Address(Record record){
//     this->blockAddress = record.blockAddress;
//     this->offset = record.offset;
// }

// Address::Address(){
//     this->blockAddress = 0;
//     this->offset = 0;
// }

Node::Node(int maxKeys, bool isLeaf){
    this->keys = new float[maxKeys];
    this->children = vector<vector<Address>>(maxKeys+1);
    this->isLeaf = isLeaf;
    this->numKeys = 0;
}

BPlusTree::BPlusTree(){
    this->rootNode = nullptr;
    this->keysStored = 0;
    this->blockSize = 400;
    this->nodeSize = blockSize;
    this->nodesStored = 0;
    int calculatedCapacity, count = 0;
    while(blockSize >= count + sizeof(Address) + sizeof(float)){
        count += sizeof(Address) + sizeof(float);
        calculatedCapacity++;
    }
    if(calculatedCapacity == 0){
        cerr << "Error: block size is too small" << endl;
    }
    this->maxKeys = calculatedCapacity;
}

int BPlusTree::insert(float key, Address value){
    //No root node exists, create one
    if(rootNode == nullptr){
        Node root = Node(this->maxKeys, true);
        root.keys[0] = key;
        root.numKeys = 1;
        root.children[0][0] = value;
        this->rootNode = &root;
        this->nodesStored++;
    }