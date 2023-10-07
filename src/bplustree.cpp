#include "bplustree.h"
#include "storage.h"
#include <iostream>
#include "types.h"

using namespace std;

Node::Node(int maxKeys, bool isLeaf)
{
    this->keys = new float[maxKeys];
    this->children = vector<vector<Address>>(maxKeys + 1);
    // Initialize all elements to 0.0
    for (int i = 0; i < maxKeys; i++)
    {
        this->keys[i] = 0.0;
        this->children[i] = vector<Address>{Address(nullptr, 0)};
    }
    this->children[maxKeys] = vector<Address>{Address(nullptr, 0)};
    this->isLeaf = isLeaf;
    this->numKeys = 0;
}

BPlusTree::BPlusTree(int numberOfRecords)
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
    while ((numberOfRecords % calculatedCapacity) < ((calculatedCapacity + 1) / 2))
    {
        calculatedCapacity--;
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
void BPlusTree::displayTree(Node *cursor, int level)
{
    // If tree exists, display all nodes
    if (cursor != nullptr)
    {
        for (int i = 0; i < level; i++)
        {
            cout << "   ";
        }
        cout << " level " << level << ": ";
        // cout << " Node Address: " << cursor << ": ";
        displayNode(cursor);
    }
    if (cursor->getIsLeaf() != true)
    {
        for (int i = 0; i < cursor->numKeys + 1; i++)
        {
            // Load node in from disk to main memory.
            Node *newChild = static_cast<Node *>(cursor->getChild(i, 0).blockAddress);

            displayTree(newChild, level + 1);
        }
    }
}

// Recursive function to display a node and its children
void BPlusTree::displayNode(Node *node)
{
    int i = 0;
    cout << "|";
    for (int i = 0; i < node->getNumKeys(); i++)
    {
        cout << node->getChild(i, 0).blockAddress << " | ";
        cout << node->getKey(i) << " | ";
    }

    // Print last filled pointer
    if (node->getChild(node->getNumKeys(), 0).blockAddress == nullptr)
    {
        cout << " Null |";
    }
    else
    {
        cout << node->getChild(node->getNumKeys(), 0).blockAddress << "|";
    }

    for (int i = node->getNumKeys(); i < maxKeys; i++)
    {
        cout << " x |";      // Remaining empty keys
        cout << "  Null  |"; // Remaining empty pointers
    }

    cout << endl;
}

int BPlusTree::insert(float key, const vector<Address> value)
{
    // No root node exists, create one
    cout << "Inserting key: " << key << endl;
    cout << "Inserting value: " << value[0].blockAddress << endl;
    if (this->rootNode == nullptr)
    {
        Node *root = new Node(this->maxKeys, true);
        root->setKey(0, key);
        cout << "KEY STORED: " << root->getKey(root->numKeys) << endl;
        root->setChildren(0, value);
        cout << "ADDRESS STORED: " << root->getChildren(root->numKeys)[0].blockAddress << endl;
        root->setNumKeys(root->getNumKeys() + 1);
        this->rootNode = root;
        this->nodesStored++;
        this->keysStored++;
        this->levels++;
        return 1;
    }
    else
    {
        cout << "Root node exists" << endl;
        Node *cursor = rootNode;
        Node *parent = nullptr;
        Node *oldRootNode = rootNode;
        while (cursor->getIsLeaf() == false)
        {
            parent = cursor;
            cout << "Cursor Address: " << cursor << endl;
            cout << "CURSOR GET LEAF: " << cursor->getIsLeaf() << endl;
            bool found = false;
            // cout << "Num Keys: " << cursor->getNumKeys() << endl;
            // cout << "First key of cursor: " << cursor->getKey(0) << endl;
            //  cout << "Keys in the cursor: " << cursor->numKeys << endl;
            for (int i = 0; i < cursor->getNumKeys(); i++)
            {
                cout << "Exploring Key " << cursor->getKey(i) << endl;
                if (key < cursor->getKey(i))
                {
                    cout << "Expanding key " << cursor->getKey(i) << endl;
                    cursor = static_cast<Node *>(cursor->getChild(0, 0).blockAddress);
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                // cout << "CURSOR -> GETNUMKEYS() " << cursor->getNumKeys() << endl;
                // cout << "Expanding last key " << cursor->getKey(cursor->getNumKeys()) << endl;
                // cout << "Memory address being accessed: " << cursor->getChild(cursor->getNumKeys(),0).blockAddress << endl;
                cursor = static_cast<Node *>(cursor->children[cursor->getNumKeys()][0].blockAddress);
                // cout << "Cursor Address: " << cursor << endl;
                // cout << "CURSOR GET LEAF IN IF !FOUND: " << cursor->getIsLeaf() << endl;
                // cout << "CURSOR KEYS =======" << cursor->getNumKeys() << endl;
            }
        }
        // At this point we should have reached a leaf node
        //  Reaching a leaf node and it has space to add more nodes
        if (cursor->getNumKeys() < this->maxKeys)
        {
            // cout << "Reached a leaf node : " << (cursor->getIsLeaf()) << endl;
            int i = 0;
            // Find the first last stored key smaller than the current key to insert
            while (key > cursor->getKey(i) && i < cursor->getNumKeys())
                i++;
            // cout << "Position for Insertion: " << i << endl;
            //  Swap all keys after this point to make space for the new key
            for (int j = cursor->getNumKeys(); j > i; j--)
            {
                // cout << "Swapping j " << j << " with " << j - 1 << endl;
                cursor->setKey(j, cursor->getKey(j - 1));
                cursor->setChildren(j, cursor->getChildren(j - 1));
            }
            // Insert the keys into the correct point i
            cout << "Inserting key into position " << i << endl;
            cursor->setKey(i, key);
            cout << "Inserting children into position " << i << endl;
            cursor->setChildren(i, value);
            // cout << "Incrementing numKeys. New Num keys = " << cursor->getNumKeys() << endl;
            cursor->setNumKeys(cursor->getNumKeys() + 1);
            // cout << "Incrementing keysStored" << endl;
            this->keysStored++;
            return 1;
        }
        // If there is no more space, create a new node
        else if (cursor->numKeys >= maxKeys) // TODO FIXME MAYBE: Insert internal being called too many times
        {
            cout << "Maximum keys exceeded in current node, creating new node" << endl;
            // Initialize new node
            Node *newNode = new Node(maxKeys, true);
            // Link the last pointer of the old node to the pointer of the new leaf node
            cursor->setChildren(cursor->getNumKeys(), vector<Address>{Address(newNode, 0)});
            newNode->setIsLeaf(true);
            // cout << "NEWNODE IS A LEAF: " << newNode->getIsLeaf() << endl;
            // Move cursor to next node after linking the two leaf nodes together
            cursor = newNode;
            cursor->setKey(0, key);
            cursor->setNumKeys(cursor->getNumKeys() + 1);
            cursor->setChildren(0, value);
            this->nodesStored++;
            this->keysStored++;
            cout << "NODES STORED AFTER CREATING NEW NODE: " << this->nodesStored << endl;
            // cout << "PARENT == ROOTNODE: " << (parent == rootNode) << endl;
            // cout << "PARENT MEMORY ADDRESS: " << parent << endl;
            // cout << "ROOTNODE MEMORY ADDRESS: " << rootNode << endl;

            // TODO FIXME MAYBE: Insert internal being called too many times
            if (oldRootNode == rootNode && nodesStored == 2)
            {
                // TODO: manually insert a parent node if the initial cursor is on root
                cout << "Manually insert new root node when there are two leaf nodes" << endl;
                Node *newParentNode = new Node(maxKeys, false);
                // Address *newChildNodeAddress = new Address(&newNode, 0);
                // newParentNode->keys[0] = newNode->keys[0]; //0.477
                // First value in parent is the left bound of the right node
                newParentNode->setKey(0, newNode->getKey(0));
                cout << "First value in parent [left bound of the right node]? : " << (newParentNode->getKey(0) == newNode->getKey(0)) << endl;
                // Second pointer in parent is the memory address of the new node
                // newParentNode->children[0].push_back(Address(oldRootNodeAddress->blockAddress, oldRootNodeAddress->offset)); //All values less than 0.477
                // newParentNode->children[1].push_back(Address(newNodeAddress->blockAddress, newNodeAddress->offset)); //All values greater than or equal to 0.477
                // TODO: undo these lines of code if not working
                // newParentNode->setChildren(0, vector<Address>{Address(rootNode,0)}) ; //All values less than 0.477
                // newParentNode->setChildren(1, vector<Address>{Address(newNode,0)}) ; //All values greater than or equal to 0.477
                newParentNode->setChild(0, Address(rootNode, 0)); // All values less than 0.477
                newParentNode->setChild(1, Address(newNode, 0));  // All values greater than or equal to 0.477
                // cout << "Actual pointer of root node in memory " << (rootNode) << endl;
                // cout << "Actual pointer of new node in memory " << (newNode) << endl;
                // cout << "First pointer in parent: " << newParentNode->getChild(0,0).blockAddress << endl;
                // cout << "Second pointer in parent: " << newParentNode->getChild(1,0).blockAddress << endl;
                //  Set first value of children to be the old root node
                newParentNode->setNumKeys(1);
                newParentNode->setIsLeaf(false);
                // Update old root node to now be a leaf node
                // cout << "Address of old root node " << static_cast<Node *>(this->rootNode) << endl;
                // Update root node variable to the new parent node
                // TODO: HARDCODED PARENT! NEED TO CHANGE
                parent = newParentNode;
                this->rootNode = newParentNode;
                cout << "Address of new root node " << static_cast<Node *>(this->rootNode) << endl;
                this->nodesStored++;
                this->levels++;
                return 1;
            }
            else
            {
                // TODO: helper function to manage parent node creation / updating in middle layers
                // FIXME: Insert internal being called too many times
                cout << "Calling insertInternal on parent address " << parent << endl;
                cout << "Storing child address in parent:  " << newNode << endl;
                insertInternal(key, parent, newNode);
                return 1;
            }
        }
    }
    return 0;
}

int BPlusTree::insertInternal(float key, Node *parent, Node *child)
{
    Node *cursor = parent;
    cout << "Cursor has " << cursor->getNumKeys() << " keys" << endl;
    cout << "Cursor has " << maxKeys << " max keys" << endl;
    // Check if the value can be inserted in the existing parent
    if ((cursor->getNumKeys() + 1) <= maxKeys)
    {
        // Find the correct position for inserting
        cout << "Space in parent, trying to find the location" << endl;
        cursor->setKey(cursor->getNumKeys(), key);
        cursor->setNumKeys(cursor->getNumKeys() + 1);
        cursor->setChildren(cursor->getNumKeys(), vector<Address>{Address(child, 0)});
        // //Keep incrementing i until you find the index where the key is smaller than current key stored
        // while(key > cursor->getKey(i) && i < cursor->getNumKeys()){
        //     i++;
        // }
        // cout << "Inserting in Parent Node at " << i << endl;
        // //Shift all keys by one step to create space for the change
        // for(int j = cursor->getNumKeys(); j>i; j--){
        //     cursor->setKey(j,cursor->getKey(j-1));
        // }
        // //Shift all pointers by one step to create space for the change
        // for(int j = cursor->getNumKeys()+1; j>i; j--){
        //     cursor->setChildren(j,cursor->getChildren(j-1));
        // }
        // Insert new key and pointer to parent
    }
    // Splitting the parent node into two
    else
    {
        Node *newSibling = new Node(maxKeys, false);
        newSibling->setIsLeaf(false);
        // Temp list of keys and addresses to insert into the split nodes
        float tempKeysList[maxKeys + 1];
        vector<Address> tempAddressList;
        //(maxKeys+2) = {nullptr, 0};
        // Copy all keys into temp list
        for (int i = 0; i < maxKeys; i++)
        {
            tempKeysList[i] = cursor->getKey(i);
        }
        tempKeysList[maxKeys] = key;
        // Copy all addresses into temp list
        for (int i = 0; i < maxKeys + 1; i++)
        {
            tempAddressList.push_back(cursor->getChild(i, 0));
        }
        tempAddressList.push_back(Address(child, 0));
        // Split the nodes into two
        // TODO: Debug this - ceiling working?
        cursor->setNumKeys((maxKeys + 1) / 2);
        newSibling->setNumKeys((maxKeys - (maxKeys + 1) / 2));
        // Reassign keys and pointers to cursor
        for (int i = 0; i < cursor->getNumKeys(); i++)
        {
            cursor->setKey(i, tempKeysList[i]);
        }

        // Assign keys and pointers to newSibling
        for (int i = 0, j = cursor->getNumKeys() + 1; i < newSibling->getNumKeys(); i++, j++)
        {
            newSibling->setKey(i, tempKeysList[j]);
        }

        // Assign pointers into the new parent node
        for (int i = 0, j = cursor->getNumKeys() + 1; i < newSibling->getNumKeys() + 1; i++, j++)
        {
            newSibling->setChild(i, tempAddressList[j]);
        }

        // Remove remaining cursor keys from cursor
        for (int i = cursor->getNumKeys(); i < cursor->getMaxKeys(); i++)
        {
            cursor->setKey(i, 0.0);
        }

        // Remove remaining cursor pointers from cursor
        for (int i = cursor->getNumKeys() + 1; i < cursor->getMaxKeys() + 1; i++)
        {
            Address nullAddress{nullptr, 0};
            cursor->setChild(i, nullAddress);
        }

        // Assign new child to original parent
        cursor->setChild(cursor->getNumKeys(), Address{child, 0});

        if (cursor == this->rootNode)
        {
            Node *newRoot = new Node(maxKeys, false);
            newRoot->setIsLeaf(false);
            // Set first key of new root node to be the rightmost key of original parent
            newRoot->setKey(0, cursor->getKey(cursor->getNumKeys()));
            newRoot->setNumKeys(newRoot->getNumKeys() + 1);
            // Assign pointers to new root node
            newRoot->setChild(0, Address{parent, 0});
            newRoot->setChild(1, Address{newSibling, 0});
            // Update class variables
            this->rootNode = newRoot;
            this->nodesStored++;
            this->levels++;
        }
        else
        {
            // TODO: write a function to get the parent of the current parent
            Node *parentNode = findParent(this->rootNode, parent, cursor->getKey(0));
            // insertInternal(cursor->numKeys, parent of cursor, newSibling);
            insertInternal(tempKeysList[cursor->getNumKeys()], parentNode, newSibling);
        }
    }
    return 1;
}

Node *BPlusTree::findParent(Node *rootNode, Node *childNode, float lowerBoundKey)
{
    Node *cursor = rootNode;
    if (cursor->getIsLeaf())
    {
        return nullptr;
    }
    Node *parent = cursor;
    while (!cursor->getIsLeaf())
    {
        // Check through all pointers of current cursor to find pointer to childNode
        for (int i = 0; i < cursor->getNumKeys() + 1; i++)
        {
            if (cursor->getChild(i, 0).blockAddress == childNode)
            {
                return parent;
            }
        }
        // We cannot find a direct match, so look for the pointer to expand
        for (int i = 0; i < cursor->getNumKeys(); i++)
        {
            if (lowerBoundKey < cursor->getKey(i))
            {
                parent = cursor;
                cursor = static_cast<Node *>(cursor->getChild(i, 0).blockAddress);
                break;
            }
            if (i == cursor->getNumKeys() - 1)
            {
                parent = cursor;
                cursor = static_cast<Node *>(cursor->getChild(i + 1, 0).blockAddress);
                break;
            }
        }
    }
    return nullptr;
}

vector<Address> BPlusTree::searchKey(float key)
{
    if (this->rootNode == nullptr)
    {
        // Error case: tree is empty
        cerr << "Tree is empty" << endl;
    }
    Node *cursor = this->rootNode;
    int indexNodesAccessed = 1; // Tracking the total number of index nodes accessed
    displayNode(cursor);
    while (!cursor->getIsLeaf())
    {
        for (int i = 0; i < cursor->getNumKeys(); i++)
        {
            // Find the key in the current node
            if (key < cursor->getKey(i))
            {
                cursor = static_cast<Node *>(cursor->getChild(i, 0).blockAddress);
                displayNode(cursor);
                indexNodesAccessed++;
                break;
            }
            // If we cannot find in any of the current keys, go to the last pointer
            if (i == cursor->getNumKeys() - 1)
            {
                cursor = static_cast<Node *>(cursor->getChild(i + 1, 0).blockAddress);
                displayNode(cursor);
                indexNodesAccessed++;
                break;
            }
        }
    }
    // Found a leaf node,traverse through it to find the key
    for (int i = 0; i < cursor->getNumKeys(); i++)
    {
        if (cursor->getKey(i) == key)
        {
            cout << ": " << indexNodesAccessed << endl;
            return cursor->getChildren(i);
        }
    }
    return vector<Address>{};
}

vector<vector<Address>> BPlusTree::searchRange(float low, float high)
{
    if (this->rootNode == nullptr)
    {
        // Error case: tree is empty
        cerr << "Tree is empty" << endl;
    }
    vector<vector<Address>> results;
    Node *cursor = this->rootNode;
    int indexNodesAccessed = 1; // Tracking the total number of index nodes accessed
    displayNode(cursor);
    while (!cursor->getIsLeaf())
    {
        for (int i = 0; i < cursor->getNumKeys(); i++)
        {
            // Find the key in the current node
            if (low < cursor->getKey(i))
            {
                cursor = static_cast<Node *>(cursor->getChild(i, 0).blockAddress);
                displayNode(cursor);
                indexNodesAccessed++;
                break;
            }
            // If we cannot find in any of the current keys, go to the last pointer
            if (i == cursor->getNumKeys() - 1)
            {
                cursor = static_cast<Node *>(cursor->getChild(i + 1, 0).blockAddress);
                displayNode(cursor);
                indexNodesAccessed++;
                break;
            }
        }
    }
    // Found a leaf node,traverse through it to find the range
    bool flag = false;
    Node *temp;
    while (cursor != nullptr && flag != true)
    {
        for (int i = 0; i < cursor->getNumKeys(); i++)
        {
            if (low <= cursor->getKey(i) && cursor->getKey(i) <= high)
            {
                results.push_back(cursor->getChildren(i));
            }
            if (cursor->getKey(i) > high)
            {
                flag = true;
            }
        }
        temp = static_cast<Node *>(cursor->getChild(cursor->getNumKeys(), 0).blockAddress);
        cursor = temp;
    }
    cout << "Number of Index Nodes accessed: " << indexNodesAccessed << endl;
    return results;
}

int BPlusTree::deleteNode(float key)
{
    // Check if tree is empty
    if (this->rootNode == nullptr)
    {
        cerr << "Tree is empty" << endl;
    }
    const int minimumKeysForLeafNode = (maxKeys + 1) / 2;
    Node *cursor = this->rootNode;
    Node *parent;
    int leftSibling, rightSibling; // Index of the left and the right leaf node that we need to borrow from

    // Finding the leaf node with the key to delete
    while (!cursor->getIsLeaf())
    {
        parent = cursor;

        // Look for all the keys in the current node
        for (int i = 0; i < cursor->getNumKeys(); i++)
        {
            leftSibling = i - 1;
            rightSibling = i + 1;

            // Find the key that we have to delete
            if (key < cursor->getKey(i))
            {
                cursor = static_cast<Node *>(cursor->getChild(i, 0).blockAddress);
                break;
            }
            // If we cannot find, go to the last pointer in the node
            if (i == cursor->getNumKeys() - 1)
            {
                leftSibling = i;
                rightSibling = i + 2;
                cursor = static_cast<Node *>(cursor->getChild(i + 1, 0).blockAddress);
                break;
            }
        }
    }

    // Cursor now points to the leaf node that contains the key that needs to be deleted

    bool found = false;
    int indexToBeDeleted;

    // Traversing the leaf node to find the index of the specified key
    for (int i = 0; i < cursor->getNumKeys(); i++)
    {
        if (cursor->getKey(i) == key)
        {
            found = true;
            indexToBeDeleted = i;
            break;
        }
    }

    if (found == false)
    {
        cerr << "Can't find the key in the tree!" << endl;
    }

    // Removing the specified key and its pointer in the memory from the node
    for (int i = indexToBeDeleted; i < cursor->getNumKeys(); i++)
    {
        cursor->setKey(i, cursor->getKey(i + 1));
        cursor->setChildren(i, cursor->getChildren(i + 1));
    }
    // Decrement number of keys stored in the current node
    cursor->setNumKeys(cursor->getNumKeys() - 1);
    // Manually set the last node since it is not covered by for loop range
    cursor->setChildren(cursor->getNumKeys(), cursor->getChildren(cursor->getNumKeys() + 1));

    for (int i = cursor->getNumKeys() + 1; i < maxKeys + 1; i++)
    {
        cursor->setChildren(i, vector<Address>{Address{nullptr, 0}});
    }

    // Deleting a key from the root
    if (cursor == this->rootNode)
    {
        if (cursor->getNumKeys() == 0)
        {
            cout << "Entire index has been deleted" << endl;
            rootNode = nullptr;
        }
        cout << "Successfully deleted " << key << endl;
        return 1;
    }

    // If we do not need to borrow from other nodes, end function
    if (cursor->getNumKeys() >= minimumKeysForLeafNode)
    {
        cout << "Successfully deleted " << key << endl;
        return 1;
    }

    // If we don't have enough keys for a balanced tree, we try to take a key from the left sibling
    if (leftSibling >= 0)
    {
        Node *leftNode = static_cast<Node *>(parent->getChild(leftSibling, 0).blockAddress);

        // We check if leftNode has enough keys to lend one and still be a valid leaf node
        if (leftNode->getNumKeys() >= minimumKeysForLeafNode + 1)
        {

            // Shift the last pointer by one
            cursor->setChildren(cursor->getNumKeys() + 1, cursor->getChildren(cursor->getNumKeys()));

            // Shift all the remaining keys and pointer back by one
            for (int i = cursor->getNumKeys(); i > 0; i--)
            {
                cursor->setKey(i, cursor->getKey(i - 1));
                cursor->setChildren(i, cursor->getChildren(i - 1));
            }

            // Transfer the borrowed key from leftNode to current Node
            cursor->setKey(0, leftNode->getKey(leftNode->getNumKeys() - 1));
            cursor->setChildren(0, leftNode->getChildren(leftNode->getNumKeys() - 1));
            cursor->setNumKeys(cursor->getNumKeys() + 1);     // to account for the increase in key
            leftNode->setNumKeys(leftNode->getNumKeys() - 1); // to account for the decrease in key

            // Move the last pointer in the left node back by one to account for the deletion
            leftNode->setChildren(cursor->getNumKeys(), cursor->getChildren(cursor->getNumKeys() + 1));
            leftNode->setChildren(cursor->getNumKeys() + 1, vector<Address>{Address{nullptr, 0}});

            // Updating parent with the new key at the beginning of current node
            parent->setKey(leftSibling, cursor->getKey(0));

            if (cursor->getNumKeys() >= minimumKeysForLeafNode)
            {
                cout << "Successfully deleted " << key << endl;
            }

            return 1;
        }
    }

    // If we can't take a key from left sibling, we check the right sibling
    if (rightSibling <= parent->getNumKeys())
    {
        Node *rightNode = static_cast<Node *>(parent->getChild(rightSibling, 0).blockAddress);

        // We check if rightNode has enough keys to lend one and still be a valid leaf node
        if (rightNode->getNumKeys() >= minimumKeysForLeafNode + 1)
        {

            // Shifting the last pointer by one to make space to add new key and pointer
            // Ensure that the last pointer still points to the right sibling
            cursor->setChildren(cursor->getNumKeys() + 1, cursor->getChildren(cursor->getNumKeys()));

            // Transfer borrowed key and pointer
            cursor->setKey(cursor->getNumKeys(), rightNode->getKey(0));
            cursor->setChildren(cursor->getNumKeys(), rightNode->getChildren(0));
            cursor->setNumKeys(cursor->getNumKeys() + 1);
            rightNode->setNumKeys(rightNode->getNumKeys() - 1);

            // Shift all the keys and pointers in rightNode left by one
            for (int i = 0; i < rightNode->getNumKeys(); i++)
            {
                rightNode->setKey(i, rightNode->getKey(i + 1));
                rightNode->setChildren(i, rightNode->getChildren(i + 1));
            }

            // TODO: Explain the logic behind using cursor->numKey and not rightNode->numKey+1
            // Shift right sibling's last pointer left by one

            rightNode->setChildren(cursor->getNumKeys(), rightNode->getChildren(cursor->getNumKeys() + 1));
            rightNode->setChildren(cursor->getNumKeys() + 1, vector<Address>{Address{nullptr, 0}}); // TODO: check if this statement is right. delete TODO after check

            // Updating the parent node with the new key at the beginning of the right sibling
            parent->setKey(rightSibling - 1, rightNode->getKey(0));

            if (cursor->getNumKeys() >= minimumKeysForLeafNode)
            {
                cout << "Successfully deleted " << key << endl;
            }

            return 1;
        }
    }

    
}