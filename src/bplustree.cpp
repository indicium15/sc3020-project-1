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
using namespace std;

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
    if (rootNode != nullptr)
    {
        deleteNode(rootNode);
    }
}

Node::~Node()
{
    // delete keys;
    delete[] keys;
    // delete &children; // we dont need to do this right?
}

// Function to display the B+ tree
void BPlusTree::displayTree()
{
    if (this->rootNode != nullptr)
    {
        displayNode(rootNode, 0);
    }
    else
    {
        cout << "Tree is empty" << endl;
    }
}

void BPlusTree::deleteNode(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    // Recursively delete child nodes
    if (!node->getIsLeaf())
    {
        for (int i = 0; i <= node->getNumKeys(); i++)
        {
            Node *childNode = static_cast<Node *>(node->getChildren(i)[0].blockAddress);
            deleteNode(childNode);
        }
    }

    // Delete the current node
    delete node;
}

// Recursive function to display a node and its children
void BPlusTree::displayNode(Node *node, int level)
{
    cout << "Display Tree Node: " << node << endl;
    cout << "Display Tree Node Keys: " << node->getNumKeys() << endl;
    cout << "Display Tree Node Key 1: " << node->getKey(0) << endl;
    // cout << "Display Tree Node Address 1: " << node->getChildren(0)[0].blockAddress << endl;
    if (node == nullptr)
    {
        return;
    }
    // cout << "Iterating through levels" << endl;
    for (int i = 0; i < level; i++)
    {
        cout << "   ";
    }
    // cout << "Level " << level << ": ";
    // cout << "Iterating through keys" << endl;
    for (int j = 0; j < node->getNumKeys(); j++)
    {
        cout << "J Value: " << j << endl;
        // cout << node->getChildren(j)[0].blockAddress << " | " << node->getKey(j) << " | ";
        cout << "Key Value: " << node->getKey(j) << endl;
        cout << "Block Address Value: " << node->getChildren(j)[0].blockAddress << endl;

    }
    if (node->getChildren(node->getNumKeys())[0].blockAddress == nullptr)
    {
        cout << " Null |";
    }
    else
    {
        cout << node->getChildren(node->getNumKeys())[0].blockAddress << "|";
    }
    for (int j = node->getNumKeys(); j < maxKeys; j++)
    {
        cout << " x |";      // Remaining empty keys
        cout << "  Null  |"; // Remaining pointers
    }
    cout << endl;
    if (!node->getIsLeaf())
    {
        for (int i = 0; i < node->getNumKeys() + 1; i++)
        {
            cout << "Display Tree going to : " << (node->getChildren(i)[0].blockAddress) << endl;  
            displayNode(static_cast<Node *>(node->getChildren(i)[0].blockAddress), level + 1); 
            // TODO: UNSURE ABOUT THE STATIC CASTING PART

        }
    }
}
 
int BPlusTree::remove(float key)
{
    if (rootNode == nullptr)
    {
        // Tree is empty, thus there is nothing to remove
        return -1; 
    }

    // Search for the node containing the key to be removed
    Node* nodeContainingKey = findNodeForKey(key, rootNode);

    if (nodeContainingKey == nullptr)
    {
        // Key is not found in the tree
        return -1; 
    }

    // Remove the key from the node containing it
    int result = removeInternal(key, nodeContainingKey, nullptr);

    // If the root has only one child (it became empty), replace it with the child
    if (rootNode->getNumKeys() == 0 && !rootNode->getIsLeaf())
    {
        Node* newRoot = static_cast<Node*>(rootNode->getChildren(0)[0].blockAddress);
        delete rootNode;
        rootNode = newRoot;
    }

    // Update the number of stored keys
    if (result == 0)
    {
        keysStored--;
    }

    return result;
}
int BPlusTree::findNodeForKey(float key, Node* currentNode)
{
    if (currentNode == nullptr)
    {
        // Tree is empty, so key is not found
        return -1;
    }

    int index = 0;
    while (index < currentNode->getNumKeys() && key > currentNode->getKey(index))
    {
        index++;
    }

    // If the key matches, return a success indicator
    if (index < currentNode->getNumKeys() && key == currentNode->getKey(index))
    {
        return 0; // Key found
    }

    // If it's an internal node, recursively search in the appropriate child
    if (!currentNode->getIsLeaf())
    {
        return findNodeForKey(key, static_cast<Node*>(currentNode->getChildren(index)[0].blockAddress));
    }

    // Key not found
    return -1;
}

int BPlusTree::removeInternal(float key, Node* parent, Node* child)
{
    if (parent == nullptr)
    {
        // Key is not found in the tree
        return -1; 
    }
    int index = -1;
    // Checking whether key index is in parent node
    for (int i = 0; i < parent->getNumKeys(); i++)
    {
        if (key == parent->getKey(i))
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        // Key is not present in the current node
        return -1; 
    }

    if (parent->getIsLeaf())
    {
        // Removing the key from the leaf node
        for (int i = index; i < parent->getNumKeys() - 1; i++)
        {
            parent->setKey(i, parent->getKey(i + 1));
        }
        parent->setNumKeys(parent->getNumKeys() - 1);

        if (parent->getNumKeys() < maxKeys / 2)
        {
            redistributeLeafNodes(parent);
            mergeLeafNodes(parent);
        }

        return 0; // Key has been successfully removed
    }
    else
    {
        // Removing the key from an internal node
        for (int i = index; i < parent->getNumKeys() - 1; i++)
        {
            parent->setKey(i, parent->getKey(i + 1));
            // Update the child pointers accordingly
            parent->setChildren(i + 1, parent->getChildren(i + 2));
        }
        parent->setNumKeys(parent->getNumKeys() - 1);
       
        if (parent->getNumKeys() < maxKeys / 2)
        {
            redistributeInternalNodes(parent);
            mergeInternalNodes(parent);
        }


        Node* childNode = static_cast<Node*>(parent->getChildren(index + 1)[0].blockAddress);
        int result = removeInternal(key, childNode, parent);

        if (childNode->getNumKeys() < maxKeys / 2)
        {
            redistributeLeafNodes(childNode);
            mergeLeafNodes(childNode);
        }

        return result;
    }
}
void BPlusTree::redistributeLeafNodes(Node* node)
{
    if (node == nullptr)
        return;

    // Find the index of the node within its parent
    int indexOfNode = -1;
    Node* parentNode = nullptr;
    for (int i = 0; i < rootNode->getNumKeys() + 1; i++)
    {
        if (rootNode->getChildren(i)[0].blockAddress == node)
        {
            indexOfNode = i;
            break;
        }
    }

    if (indexOfNode == -1)
    {
        // Unable to find the node's index in its parent
        return;
    }
    // Attempt redistribution with left sibling
    if (indexOfNode > 0)
    {
        Node* leftSibling = static_cast<Node*>(rootNode->getChildren(indexOfNode - 1)[0].blockAddress);
        if (leftSibling->getNumKeys() > maxKeys / 2)
        {
            // Move a key from the left sibling to the current node
            float keyToMove = leftSibling->getKey(leftSibling->getNumKeys() - 1);
            leftSibling->setNumKeys(leftSibling->getNumKeys() - 1);
            node->setKey(0, keyToMove);

            // Update parent keys
            rootNode->setKey(indexOfNode - 1, keyToMove);

            return;
        }
    }

    // Attempt redistribution with right sibling
    if (indexOfNode < rootNode->getNumKeys())
    {
        Node* rightSibling = static_cast<Node*>(rootNode->getChildren(indexOfNode + 1)[0].blockAddress);
        if (rightSibling->getNumKeys() > maxKeys / 2)
        {
            // Move a key from the right sibling to the current node
            float keyToMove = rightSibling->getKey(0);
            rightSibling->setNumKeys(rightSibling->getNumKeys() - 1);
            node->setKey(node->getNumKeys(), keyToMove);

            // Update parent keys
            rootNode->setKey(indexOfNode, rightSibling->getKey(0));

            return;
        }
    }

}
void BPlusTree::mergeLeafNodes(Node* node)
{
    if (node == nullptr)
        return;

    // Find the index of the node within its parent
    int indexOfNode = -1;
    Node* parentNode = nullptr;
    for (int i = 0; i < rootNode->getNumKeys() + 1; i++)
    {
        if (rootNode->getChildren(i)[0].blockAddress == node)
        {
            indexOfNode = i;
            break;
        }
    }

    if (indexOfNode == -1)
    {
        // Unable to find the node's index in its parent
        return;
    }

    // Merge with left sibling if possible
    if (indexOfNode > 0)
    {
        Node* leftSibling = static_cast<Node*>(rootNode->getChildren(indexOfNode - 1)[0].blockAddress);
        if (leftSibling->getNumKeys() + node->getNumKeys() <= maxKeys)
        {
            // Merge the current node into the left sibling
            leftSibling->setKey(leftSibling->getNumKeys(), node->getKey(0));
            leftSibling->setNumKeys(leftSibling->getNumKeys() + 1);

            for (int i = 0; i < node->getNumKeys(); i++)
            {
                leftSibling->setKey(leftSibling->getNumKeys(), node->getKey(i));
                leftSibling->setNumKeys(leftSibling->getNumKeys() + 1);
            }

            // Update parent keys and pointers
            for (int i = indexOfNode - 1; i < rootNode->getNumKeys() - 1; i++)
            {
                rootNode->setKey(i, rootNode->getKey(i + 1));
                rootNode->setChildren(i + 1, rootNode->getChildren(i + 2));
            }
            rootNode->setNumKeys(rootNode->getNumKeys() - 1);
            delete node;
            mergeLeafNodes(rootNode);

            return;
        }
    }

    // Merge with right sibling if possible
    if (indexOfNode < rootNode->getNumKeys())
    {
        Node* rightSibling = static_cast<Node*>(rootNode->getChildren(indexOfNode + 1)[0].blockAddress);
        if (rightSibling->getNumKeys() + node->getNumKeys() <= maxKeys)
        {
            // Merge the right sibling into the current node
            node->setKey(node->getNumKeys(), rightSibling->getKey(0));
            node->setNumKeys(node->getNumKeys() + 1);

            for (int i = 0; i < rightSibling->getNumKeys(); i++)
            {
                node->setKey(node->getNumKeys(), rightSibling->getKey(i));
                node->setNumKeys(node->getNumKeys() + 1);
            }

            // Update parent keys and pointers
            for (int i = indexOfNode; i < rootNode->getNumKeys() - 1; i++)
            {
                rootNode->setKey(i, rootNode->getKey(i + 1));
                rootNode->setChildren(i + 1, rootNode->getChildren(i + 2));
            }
            rootNode->setNumKeys(rootNode->getNumKeys() - 1);

            // Release the memory of the right sibling node
            delete rightSibling;

            // Recursively check if the parent node needs merging
            mergeLeafNodes(rootNode);

            return;
        }
    }
    // Merging was not possible
}
void BPlusTree::redistributeInternalNodes(Node* node)
{
    if (node == nullptr)
        return;

    // Find the index of the node within its parent
    int indexOfNode = -1;
    Node* parentNode = nullptr;
    for (int i = 0; i < rootNode->getNumKeys() + 1; i++)
    {
        if (rootNode->getChildren(i)[0].blockAddress == node)
        {
            indexOfNode = i;
            break;
        }
    }

    if (indexOfNode == -1)
    {
        return;
    }
    if (indexOfNode > 0)
    {
        Node* leftSibling = static_cast<Node*>(rootNode->getChildren(indexOfNode - 1)[0].blockAddress);
        if (leftSibling->getNumKeys() > maxKeys / 2)
        {
            // Move a key from the left sibling to the current node
            float keyToMove = leftSibling->getKey(leftSibling->getNumKeys() - 1);
            leftSibling->setNumKeys(leftSibling->getNumKeys() - 1);
            node->setKey(0, rootNode->getKey(indexOfNode - 1));
            rootNode->setKey(indexOfNode - 1, keyToMove);

            return;
        }
    }

    if (indexOfNode < rootNode->getNumKeys())
    {
        Node* rightSibling = static_cast<Node*>(rootNode->getChildren(indexOfNode + 1)[0].blockAddress);
        if (rightSibling->getNumKeys() > maxKeys / 2)
        {
            // Move a key from the right sibling to the current node
            float keyToMove = rightSibling->getKey(0);
            rightSibling->setNumKeys(rightSibling->getNumKeys() - 1);
            node->setKey(node->getNumKeys(), rootNode->getKey(indexOfNode));
            rootNode->setKey(indexOfNode, keyToMove);

            return;
        }
    }

    // Redistribution was not possible, consider merging
}
void BPlusTree::mergeInternalNodes(Node* node)
{
    if (node == nullptr)
        return;

    // Find the index of the node within its parent
    int indexOfNode = -1;
    Node* parentNode = nullptr;
    for (int i = 0; i < rootNode->getNumKeys() + 1; i++)
    {
        if (rootNode->getChildren(i)[0].blockAddress == node)
        {
            indexOfNode = i;
            break;
        }
    }

    if (indexOfNode == -1)
    {
        // Unable to find the node's index in its parent
        return;
    }

    // Merge with left sibling if possible
    if (indexOfNode > 0)
    {
        Node* leftSibling = static_cast<Node*>(rootNode->getChildren(indexOfNode - 1)[0].blockAddress);
        if (leftSibling->getNumKeys() + node->getNumKeys() <= maxKeys)
        {
            // Merge the current node into the left sibling
            leftSibling->setKey(leftSibling->getNumKeys(), rootNode->getKey(indexOfNode - 1));
            leftSibling->setNumKeys(leftSibling->getNumKeys() + 1);

            for (int i = 0; i < node->getNumKeys(); i++)
            {
                leftSibling->setKey(leftSibling->getNumKeys(), node->getKey(i));
                leftSibling->setNumKeys(leftSibling->getNumKeys() + 1);
                leftSibling->setChildren(leftSibling->getNumKeys(), node->getChildren(i + 1));
            }

            // Update parent keys and pointers
            for (int i = indexOfNode - 1; i < rootNode->getNumKeys() - 1; i++)
            {
                rootNode->setKey(i, rootNode->getKey(i + 1));
                rootNode->setChildren(i + 1, rootNode->getChildren(i + 2));
            }
            rootNode->setNumKeys(rootNode->getNumKeys() - 1);

            // Release the memory of the current node
            delete node;

            // Recursively check if the parent node needs merging
            mergeInternalNodes(rootNode);

            return;
        }
    }

    // Merge with right sibling if possible
    if (indexOfNode < rootNode->getNumKeys())
    {
        Node* rightSibling = static_cast<Node*>(rootNode->getChildren(indexOfNode + 1)[0].blockAddress);
        if (rightSibling->getNumKeys() + node->getNumKeys() <= maxKeys)
        {
            // Merge the right sibling into the current node
            node->setKey(node->getNumKeys(), rootNode->getKey(indexOfNode));
            node->setNumKeys(node->getNumKeys() + 1);

            for (int i = 0; i < rightSibling->getNumKeys(); i++)
            {
                node->setKey(node->getNumKeys(), rightSibling->getKey(i));
                node->setNumKeys(node->getNumKeys() + 1);
                node->setChildren(node->getNumKeys(), rightSibling->getChildren(i + 1));
            }

            // Update the parent keys and pointers
            for (int i = indexOfNode; i < rootNode->getNumKeys() - 1; i++)
            {
                rootNode->setKey(i, rootNode->getKey(i + 1));
                rootNode->setChildren(i + 1, rootNode->getChildren(i + 2));
            }
            rootNode->setNumKeys(rootNode->getNumKeys() - 1);
            delete rightSibling;
            mergeInternalNodes(rootNode);

            return;
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
        while (cursor->getIsLeaf() == false)
        {
            // Non-leaf node
            // 0 index -> first node
            // 1 index -> second node
            // cout << "Cursor not at leaf node" << endl;
            cout << "Cursor Address: " << cursor << endl;
            cout << "CURSOR GET LEAF: " << cursor->getIsLeaf() << endl;
            parent = cursor;
            bool found = false;
            cout << "Num Keys: " << cursor->getNumKeys() << endl;
            cout << "First key of cursor: " << cursor->getKey(0) << endl;
            // cout << "Keys in the cursor: " << cursor->numKeys << endl;
            for (int i = 0; i < cursor->getNumKeys(); i++)
            {
                cout << "Exploring Key " << cursor->getKey(i) << endl;
                if (key < cursor->getKey(i))
                {
                    cout << "Expanding key " << cursor->getKey(i) << endl;
                    // parent = cursor; //TODO: check if this logic is right. parent variable keeps being stored with the parent of the cursor
                    cursor = static_cast<Node*>(cursor->children[i][0].blockAddress);
                    found = true;
                    break;
                }
            }
            if(!found){
                cout << "CURSOR -> GETNUMKEYS() " << cursor->getNumKeys() << endl;
                cout << "Expanding last key " << cursor->keys[cursor->getNumKeys()] << endl;
                // parent = cursor;//TODO: check if this logic is right. parent variable keeps being stored with the parent of the cursor
                cout << "Memory address being accessed: " << cursor->children[cursor->getNumKeys()][0].blockAddress << endl;
                cursor = static_cast<Node *>(cursor->children[cursor->getNumKeys()][0].blockAddress);
                cout << "Cursor Address: " << cursor << endl;
                cout << "CURSOR GET LEAF IN IF !FOUND: " << cursor->getIsLeaf() << endl;
                cout << "CURSOR KEYS =======" << cursor->getNumKeys() << endl;
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
            newNode->setIsLeaf(true);
            cout << "NEWNODE IS A LEAF: " << newNode->getIsLeaf() << endl;
            // TODO: Need to confirm that there is no problem not using a pointer here
            cursor->children[cursor->getNumKeys()].push_back(Address(&newNode, 0));
            //Move cursor to next node after linking the two leaf nodes together
            cursor = newNode;

            cursor->setKey(0,key);
            cursor->setNumKeys(1);
            cursor->setChildren(0, value);
            this->nodesStored++;
            this->keysStored++;
            
            if (parent == rootNode && nodesStored == 2)
            {
                // TODO: manually insert a parent node if the initial cursor is on root
                cout << "Manually insert new root node when there are two leaf nodes" << endl;
                Node *newParentNode = new Node(maxKeys, false);
                // Address *newChildNodeAddress = new Address(&newNode, 0);
                // First value in parent is the left bound of the right node
                // newParentNode->keys[0] = newNode->keys[0]; //0.477
                newParentNode->setKey(0, newNode->getKey(0));
                cout << "First value in parent [left bound of the right node]: " << newParentNode->keys[0] << endl;
                // Second pointer in parent is the memory address of the new node
                // newParentNode->children[0].push_back(Address(oldRootNodeAddress->blockAddress, oldRootNodeAddress->offset)); //All values less than 0.477
                // newParentNode->children[1].push_back(Address(newNodeAddress->blockAddress, newNodeAddress->offset)); //All values greater than or equal to 0.477
                // TODO: undo these lines of code if not working
                newParentNode->children[0].push_back(Address(rootNode,0)) ; //All values less than 0.477
                newParentNode->children[1].push_back(Address(newNode,0)) ; //All values less than 0.477
                cout << "Actual pointer of root node in memory " << (rootNode) << endl;
                cout << "Actual pointer of new node in memory " << (newNode) << endl;
                cout << "First pointer in parent: " << newParentNode->children[0][0].blockAddress << endl;
                cout << "Second pointer in parent: " << newParentNode->children[1][0].blockAddress << endl;
                // Set first value of children to be the old root node
                newParentNode->setNumKeys(1);
                newParentNode->setIsLeaf(false);
                // Update old root node to now be a leaf node
                cout << "Address of old root node " << static_cast<Node *>(this->rootNode) << endl;
                // Update root node variable to the new parent node
                this->rootNode = newParentNode;
                parent = newParentNode; //TODO: HARDCODED PARENT! NEED TO CHANGE
                cout << "Address of new root node " << static_cast<Node *>(this->rootNode) << endl;
                this->nodesStored++;
                this->levels++;
            }
            else
            {
                // TODO: helper function to manage parent node creation / updating in middle layers
                cout << "Calling insertInternal" << endl;
                insertInternal(key, parent, newNode);
            }
        }
    }
}

// int BPlusTree::insertInternal(float key, Node*parent ,Node* child){
//     Node* cursor = parent;
//     //Check if the value can be inserted in the existing parent
//     if(cursor->getNumKeys() + 1 < cursor->getMaxKeys()){
//         //Find the correct position for inserting
//         int i = 0;
//         //Keep incrementing i until you find the index where the key is smaller than current key stored
//         while(key > cursor->getKey(i) && i < cursor->getNumKeys()){
//             i++;
//         }
//         //Shift all keys by one step to create space for the change
//         for(int j = cursor->getNumKeys(); j>i; j--){
//             cursor->setKey(j,cursor->getKey(j-1));
//         }
//         //Shift all pointers by one step to create space for the change
//         for(int j = cursor->getNumKeys()+1; j>i; j--){
//             cursor->setChildren(j,cursor->getChildren(j-1));
//         }
//         //Insert new key and pointer to parent
//         cursor->setKey(i, key);
//         int newKeys = cursor->getNumKeys();
//         newKeys++;
//         cursor->setNumKeys(newKeys);
//         cursor->children[i+1][0] = Address(child, 0);
//     }
//     //Splitting the parent node into two
//     else{
//         // Node* newParent = new Node(maxKeys, false);
//         // newParent->setIsLeaf(false);
//         // //Temp list of keys and addresses to insert into the split nodes
//         // float tempKeysList[maxKeys];
//         // vector<Address> tempAddressList(maxKeys+1);
//         // //Copy all keys into temp list
//         // for(int i = 0; i < maxKeys; i++){
//         //     tempKeysList[i] = cursor->getKey(i);
//         // }
//         // // Copy all addresses into temp list
//         // for(int i = 0; i < maxKeys+1; i++){
//         //     tempAddressList.push_back(Address(cursor->getChildren(i)[0].blockAddress, 0));
//         // }
//     }
// }

