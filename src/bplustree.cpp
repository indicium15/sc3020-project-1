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
    else{
        Node *cursor = rootNode;
        Node *parent;
        //TODO: if root node is not a leaf
        while(!(cursor->isLeaf)){
            for(int i=0; i < cursor->numKeys; i++){
                if(key < cursor->keys[i]){
                    cursor = static_cast<Node*>(cursor->children[i][0].blockAddress);
                    break;
                }
                if(i == cursor->numKeys-1){
                    cursor = static_cast<Node*>(cursor->children[i+1][0].blockAddress);
                    break;
                }
            }
        }
        //Reaching a leaf node and it has space to add more nodes
        if(cursor->numKeys < maxKeys){
            cursor->keys[cursor->numKeys-1] = key;
            cursor->numKeys++;
            cursor->children[cursor->numKeys-1].push_back(value);
        }
        //If there is no more space, create a new node
        if(cursor->numKeys >= maxKeys){
            Node newNode = Node(maxKeys, true);
            //TODO: Need to confirm that there is no problem not using a pointer here
            Address newNodeAddress = Address(&newNode, 0); 
            cursor->children[cursor->maxKeys].push_back(newNodeAddress);
            this->nodesStored++;
            cursor = &newNode;
            cursor->keys[0] = key;
            cursor->numKeys = 1;
            cursor->children[0][0] = value;
            if(parent==rootNode && nodesStored == 2){
                //TODO: manually insert a parent node if the initial cursor is on root
                Node newParentNode = Node(maxKeys, false);
                Address newParentNodeAddress = Address(&newParentNode, 0);
                Address oldRootNodeAddress = Address(&rootNode, 0);
                //First value in parent is the left bound of the right node
                newParentNode.keys[0] = newNode.keys[0];
                //Second pointer in parent is the memory address of the new node
                newParentNode.children[1][0] = newParentNodeAddress;
                //Set first value of children to be the old root node
                newParentNode.children[0][0] = oldRootNodeAddress;
                //Update old root node to now be a leaf node
                this->rootNode->isLeaf = true;
                //Update root node variable to the new parent node
                this->rootNode = &newParentNode;
                this->nodesStored++;
            }
            else{ 
            //TODO: helper function to manage parent node creation / updating in middle layers
            }
        }
        
    }
}