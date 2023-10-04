#include "bplustree.h";
#include "storage.h";
#include <iostream>;

Address::Address(Record record){
    this->blockAddress = record.blockAddress;
    this->offset = record.offset;
}

Address::Address(){
    this->blockAddress = 0;
    this->offset = 0;
}

Node::Node(int maxKeys, bool isLeaf){
    this->keys = new float[maxKeys + 1];
    this->children = new Address[maxKeys + 1];
    this->isLeaf = isLeaf;
    this->numKeys = 0;
}

BPlusTree::BPlusTree(){
    this->rootNode = nullptr;
    this->keysStored = 0;
    this->blockSize = 400;
    this->nodeSize = blockSize;
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

int BPlusTree::insert(Record record){
    Address toInsert = Address(record);
    float key = record.fgPct;
    //No root node exists, create one
    if(rootNode == nullptr){
        Node root = Node(this->maxKeys, true);
        root.keys[0] = key;
        root.numKeys = 1;
        root.children[0] = toInsert;
        this->rootNode = &root;
    }
    else{
        Node *cursor = rootNode;
        Node *parent;
        //TODO: if root node is not a leaf
        while(!(cursor->isLeaf)){
            for(int i=0; i < cursor->numKeys; i++){
                if(key < cursor->keys[i]){
                    cursor = static_cast<Node*>(cursor->children[i].blockAddress);
                    break;
                }
                if(i == cursor->numKeys-1){
                    cursor = static_cast<Node*>(cursor->children[i+1].blockAddress);
                    break;
                }
            }
        }
        //Reaching a leaf node and it has space to add one more node
        if(cursor->numKeys < maxKeys){
            while()
        }

    }
}