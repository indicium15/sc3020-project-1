#include "bplustree.h"
#include "storage.h"
#include <iostream>
#include "types.h"

// Address::Address(Record record){
//     this->blockAddress = record.blockAddress;
//     this->offset = record.offset;
// }

// Address::Address(){
//     this->blockAddress = 0;
//     this->offset = 0;
// }

Node::Node(int maxKeys, bool isLeaf)
{
    this->keys = new float[maxKeys];
    // Initialize all elements to 0.0
    for (int i = 0; i < maxKeys; i++)
    {
        this->keys[i] = 0.0;
    }
    this->children = vector<vector<Address>>(maxKeys + 1);
    this->isLeaf = isLeaf;
    this->numKeys = 0;
}

BPlusTree::BPlusTree()
{
    this->rootNode = nullptr;
    this->keysStored = 0;
    this->blockSize = 400;
    this->nodeSize = blockSize;
    this->nodesStored = 0;
    this->levels = 0;
    int calculatedCapacity = 0, count = 0;
    while ((count + sizeof(Address) + sizeof(float)) <= (nodeSize - 2 * sizeof(uint8_t) - sizeof(bool)))
    {
        count += sizeof(Address) + sizeof(float);
        calculatedCapacity++;
    }
    if (calculatedCapacity == 0)
    {
        cerr << "Error: block size is too small" << endl;
    }
    this->maxKeys = calculatedCapacity;
}

// void BPlusTree::displayTree(Node *node, int level){
//     Node *cursor = node;
//     while(!(cursor == nullptr)){
//         for(int j = 0; j < level; j++){
//             cout << "  " ;
//         }
//         for(int j = 0; j < cursor->numKeys; j++){
//             cout << cursor->children[j][0].blockAddress << " | " << cursor->keys[j] << " | ";
//         }

//         if (cursor->children[cursor->numKeys][0].blockAddress == nullptr) {
//             std::cout << " Null |";
//         }
//         else {
//             std::cout << cursor->children[cursor->numKeys][0].blockAddress << "|";
//         }

//         for(int j = cursor->numKeys; j < maxKeys; j++){
//             cout << " x |";      // Remaining empty keys
//             cout << "  Null  |"; // Remaining pointers
//         }
//         cout << endl;
//     }
// }

BPlusTree::~BPlusTree()
{
    // delete rootNode;
}

Node::~Node()
{
    // delete keys;
    // delete &children;
}

// Function to display the B+ tree
void BPlusTree::displayTree()
{
    if (rootNode != nullptr)
    {
        displayNode(rootNode, 0);
    }
    else
    {
        cout << "Tree is empty" << endl;
    }
}

// Recursive function to display a node and its children
void BPlusTree::displayNode(Node *node, int level)
{
    if (node == nullptr)
    {
        return;
    }
    for (int i = 0; i < level; i++)
    {
        cout << "   ";
    }
    // cout << "Level " << level << ": ";
    for (int j = 0; j < node->numKeys; j++)
    {
        cout << node->children[j][0].blockAddress << " | " << node->keys[j] << " | ";
    }
    if (node->children[node->numKeys][0].blockAddress == nullptr)
    {
        std::cout << " Null |";
    }
    else
    {
        std::cout << node->children[node->numKeys][0].blockAddress << "|";
    }
    for (int j = node->numKeys; j < maxKeys; j++)
    {
        cout << " x |";      // Remaining empty keys
        cout << "  Null  |"; // Remaining pointers
    }
    cout << endl;
    if (!node->isLeaf)
    {
        for (int i = 0; i < node->numKeys + 1; i++)
        {
            displayNode(static_cast<Node *>(node->children[i][0].blockAddress), level + 1); // TODO: UNSURE ABOUT THE STATIC CASTING PART
        }
    }
}

int BPlusTree::insert(float key, const vector<Address> &value)
{
    // No root node exists, create one
    // cout << "Inserting key: " << key << endl;
    // cout << "Inserting value: " << value[0].blockAddress << endl;
    if (this->rootNode == nullptr)
    {
        Node *root = new Node(this->maxKeys, true);
        root->keys[0] = key;
        root->children.push_back(value);
        root->numKeys++;
        this->rootNode = root;
        this->nodesStored++;
        this->keysStored++;
        this->levels++;
    }
    else
    {
        cout << "Root node exists" << endl;
        Node *cursor = rootNode;
        Node *parent = cursor; //= rootNode; // TODO: Just added it now not sure if this works!
        // TODO: if root node is not a leaf
        while ((cursor->isLeaf) == false)
        {
            // Non-leaf node
            // 0 index -> first node
            // 1 index -> second node
            // cout << "Cursor not at leaf node" << endl;
            parent = cursor;
            bool found = false;
            cout << "Num Keys: " << cursor->getNumKeys() << endl;
            cout << "First key of cursor: " << cursor->getKey(0) << endl;
            // cout << "Keys in the cursor: " << cursor->numKeys << endl;
            for (int i = 0; i < cursor->getNumKeys(); i++)
            {
                cout << "Exploring Key" << cursor->keys[i] << endl;
                if (key < cursor->getKey(i))
                {
                    cout << "Expanding key " << cursor->keys[i] << endl;
                    // parent = cursor; //TODO: check if this logic is right. parent variable keeps being stored with the parent of the cursor
                    cursor = static_cast<Node *>(cursor->children[i][0].blockAddress);
                    found = true;
                    break;
                }
            }
            if(!found){
                cout << "CURSOR -> GETNUMKEYS() " << cursor->getNumKeys() << endl;
                cout << "Expanding last key " << cursor->keys[cursor->getNumKeys()] << endl;
                // parent = cursor;//TODO: check if this logic is right. parent variable keeps being stored with the parent of the cursor
                cout << "Memory address being accessed: " << &cursor->children[cursor->getNumKeys()][0].blockAddress;
                cursor = static_cast<Node *>(cursor->children[cursor->getNumKeys()][0].blockAddress);
            }
        }
        // Reaching a leaf node and it has space to add more nodes
        if (cursor->getNumKeys() < maxKeys)
        {
            cout << "Reached a leaf node" << endl;
            int i = 0;
            //Find the first last stored key smaller than the current key to insert
            while (key > cursor->keys[i] && i < cursor->getNumKeys())
                i++;
            cout << "Position for Insertion: " << i << endl;
            // Swap all keys after this point to make space for the new key
            for (int j = cursor->getNumKeys(); j > i; j--)
            {
                cout << "Swapping j " << j << " with " << j - 1 << endl;
                cursor->keys[j] = cursor->keys[j - 1];
                cursor->children[j] = cursor->children[j - 1];
            }
            // Insert the keys into the correct point i
            cout << "Inserting key into position " << i << endl;
            cursor->keys[i] = key;
            cout << "Inserting children into position " << i << endl;
            cursor->children[i].insert(cursor->children[i].end(), value.begin(), value.end());
            cout << "Incrementing numKeys. New Num keys = " << cursor->getNumKeys() << endl;
            cursor->numKeys++;
            cout << "Incrementing keysStored" << endl;
            this->keysStored++;
        }
        // If there is no more space, create a new node
        else if (cursor->numKeys >= maxKeys)
        {
            cout << "Maximum keys exceeded in current node, creating new node" << endl;
            Node *newNode = new Node(maxKeys, true);
            // TODO: Need to confirm that there is no problem not using a pointer here
            Address *newNodeAddress = new Address(&newNode, 0);
            cursor->children[cursor->getNumKeys()].push_back((Address(&newNode, 0)));
            cursor = newNode;
            cursor->keys[0] = key;
            cursor->numKeys = 1;
            cursor->children[0] = value;
            this->nodesStored++;
            this->keysStored++;
            
            if (parent == rootNode && nodesStored == 2)
            {
                // TODO: manually insert a parent node if the initial cursor is on root
                cout << "Manually insert new root node when there are two leaf nodes" << endl;
                Node *newParentNode = new Node(maxKeys, false);
                Address *oldRootNodeAddress = new Address(&rootNode, 0);
                // Address *newChildNodeAddress = new Address(&newNode, 0);
                // First value in parent is the left bound of the right node
                newParentNode->keys[0] = newNode->keys[0]; //0.477
                cout << "First value in parent [left bound of the right node]: " << newParentNode->keys[0] << endl;
                // Second pointer in parent is the memory address of the new node
                // newParentNode->children[0].push_back(Address(oldRootNodeAddress->blockAddress, oldRootNodeAddress->offset)); //All values less than 0.477
                // newParentNode->children[1].push_back(Address(newNodeAddress->blockAddress, newNodeAddress->offset)); //All values greater than or equal to 0.477
                // TODO: undo these lines of code if not working
                newParentNode->children[0].push_back(Address(static_cast<void*>(&rootNode),0)) ; //All values less than 0.477
                newParentNode->children[1].push_back(Address(static_cast<void*>(&newNode),0)) ; //All values less than 0.477
                cout << "Actual pointer of root node in memory " << static_cast<void*>(&rootNode) << endl;
                cout << "Actual pointer of new node in memory " << static_cast<void*>(&newNode) << endl;
                cout << "First pointer in parent: " << &newParentNode->children[0][0].blockAddress << endl;
                cout << "Second pointer in parent: " << &newParentNode->children[1][0].blockAddress << endl;
                // Set first value of children to be the old root node
                newParentNode->numKeys = 1;
                // Update old root node to now be a leaf node
                this->rootNode->isLeaf = true;
                cout << "Address of old root node " << static_cast<Node *>(this->rootNode) << endl;
                // Update root node variable to the new parent node
                this->rootNode = newParentNode;
                cout << "Address of new root node " << static_cast<Node *>(this->rootNode) << endl;
                this->nodesStored++;
                this->levels++;
            }
            else
            {
                // TODO: helper function to manage parent node creation / updating in middle layers
            }
        }
    }
}