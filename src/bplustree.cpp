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
        cout << " Level " << level << ": ";
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
    cout << "| ";
    for (int i = 0; i < node->getNumKeys(); i++)
    {
        // cout << node->getChild(i, 0).blockAddress << " | ";
        if (i == node->getNumKeys() - 1)
        {
            cout << node->getKey(i) << " |";
        }
        else
        {
            cout << node->getKey(i) << " | ";
        }
    }

    // // Print last filled pointer
    // if (node->getChild(node->getNumKeys(), 0).blockAddress == nullptr)
    // {
    //     cout << " Null |";
    // }
    // else
    // {
    //     cout << node->getChild(node->getNumKeys(), 0).blockAddress << "|";
    // }

    for (int i = node->getNumKeys(); i < maxKeys; i++)
    {
        cout << " x |"; // Remaining empty keys
        // cout << "  Null |"; // Remaining empty pointers
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
        else if (cursor->numKeys >= maxKeys) 
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

            if (oldRootNode == rootNode && nodesStored == 2)
            {
                cout << "Manually insert new root node when there are two leaf nodes" << endl;
                Node *newParentNode = new Node(maxKeys, false);
                // Address *newChildNodeAddress = new Address(&newNode, 0);
                // newParentNode->keys[0] = newNode->keys[0]; //0.477
                // First value in parent is the left bound of the right node
                newParentNode->setKey(0, newNode->getKey(0));
                cout << "First value in parent [left bound of the right node]? : " << (newParentNode->getKey(0) == newNode->getKey(0)) << endl;
                // Second pointer in parent is the memory address of the new node
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
                parent = newParentNode;
                this->rootNode = newParentNode;
                cout << "Address of new root node " << static_cast<Node *>(this->rootNode) << endl;
                this->nodesStored++;
                this->levels++;
                return 1;
            }
            else
            {
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
        tempAddressList.push_back(Address{child, 0});
        // Split the nodes into two
        cursor->setNumKeys((maxKeys + 1) / 2); //2
        newSibling->setNumKeys((maxKeys - ((maxKeys + 1) / 2))); //2
        // Reassign keys and pointers to cursor
        // i = 0-1
        for (int i = 0; i < cursor->getNumKeys(); i++)
        {
            //0 and 1 here
            cursor->setKey(i, tempKeysList[i]);
        }

        // Assign keys and pointers to newSibling
        // i = 0-1, j = 2-3
        for (int i = 0, j = cursor->getNumKeys()+1; i < newSibling->getNumKeys(); i++, j++)
        {
            newSibling->setKey(i, tempKeysList[j]);
        }

        // newSibling->setKey(newSibling->getNumKeys(), tempKeysList[maxKeys]);

        // Assign pointers into the new parent node
        for (int i = 0, j = cursor->getNumKeys()+1; i < newSibling->getNumKeys() + 1; i++, j++)
        {
            newSibling->setChild(i, tempAddressList[j]);
        }

        // Remove remaining cursor keys from cursor
        // float newRootValue = cursor->getKey(cursor->getNumKeys());
        for (int i = cursor->getNumKeys(); i < cursor->getMaxKeys(); i++)
        {
            cursor->setKey(i, float());
        }

        // Remove remaining cursor pointers from cursor
        for (int i = cursor->getNumKeys() + 1; i < cursor->getMaxKeys() + 1; i++)
        {
            Address nullAddress{nullptr, 0};
            cursor->setChild(i, nullAddress);
        }

        // Assign new child to original parent
        newSibling->setChild(newSibling->getNumKeys(), Address{child, 0});

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
                cursor = static_cast<Node *>(cursor->getChild(i, 0).blockAddress);
                parent = cursor;
                break;
            }
            if (i == cursor->getNumKeys() - 1)
            {
                cursor = static_cast<Node *>(cursor->getChild(i + 1, 0).blockAddress);
                parent = cursor;
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
    // displayNode(cursor);
    while (!cursor->getIsLeaf())
    {
        for (int i = 0; i < cursor->getNumKeys(); i++)
        {
            // Find the key in the current node
            if (key < cursor->getKey(i))
            {
                cursor = static_cast<Node *>(cursor->getChild(i, 0).blockAddress);
                // displayNode(cursor);
                indexNodesAccessed++;
                break;
            }
            // If we cannot find in any of the current keys, go to the last pointer
            if (i == cursor->getNumKeys() - 1)
            {
                cursor = static_cast<Node *>(cursor->getChild(i + 1, 0).blockAddress);
                // displayNode(cursor);
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
            cout << "Index Nodes Accessed while Searching B+ Key: " << indexNodesAccessed << endl;
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
    // displayNode(cursor);
    while (!cursor->getIsLeaf())
    {
        for (int i = 0; i < cursor->getNumKeys(); i++)
        {
            // Find the key in the current node
            if (low < cursor->getKey(i))
            {
                cursor = static_cast<Node *>(cursor->getChild(i, 0).blockAddress);
                // displayNode(cursor);
                indexNodesAccessed++;
                break;
            }
            // If we cannot find in any of the current keys, go to the last pointer
            if (i == cursor->getNumKeys() - 1)
            {
                cursor = static_cast<Node *>(cursor->getChild(i + 1, 0).blockAddress);
                // displayNode(cursor);
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
    cout << "Number of Index Nodes Accessed in B+ Tree Search: " << indexNodesAccessed << endl;
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

            // Shift right sibling's last pointer left by one

            rightNode->setChildren(cursor->getNumKeys(), rightNode->getChildren(cursor->getNumKeys() + 1));
            rightNode->setChildren(cursor->getNumKeys() + 1, vector<Address>{Address{nullptr, 0}});

            // Updating the parent node with the new key at the beginning of the right sibling
            parent->setKey(rightSibling - 1, rightNode->getKey(0));

            if (cursor->getNumKeys() >= minimumKeysForLeafNode)
            {
                cout << "Successfully deleted " << key << endl;
            }

            return 1;
        }
    }

    // Reaching this point means that we cannot borrow keys from the siblings
    // In this case, we must merge nodes

    // If current node has a left sibling, merge with it
    if (leftSibling >= 0)
    {

        // Left sibling
        Node *leftNode = static_cast<Node *>(parent->getChild(leftSibling, 0).blockAddress);

        // Transfering all keys and pointers from current node to left sibling
        for (int i = leftNode->getNumKeys(), j = 0; j < cursor->getNumKeys(); i++, j++)
        {
            leftNode->setKey(i, cursor->getKey(j));
            leftNode->setChildren(i, cursor->getChildren(j));
        }

        // Updating numKeys and making sure that the last pointer in the left node points to the node after the current node
        leftNode->setNumKeys(leftNode->getNumKeys() + cursor->getNumKeys());
        leftNode->setChildren(leftNode->getNumKeys(), cursor->getChildren(cursor->getNumKeys()));


        deleteInternal(parent->getKey(leftSibling), parent, cursor);
    }
    // If merge with left sibling does not work, we try merging with the right sibling
    else if (rightSibling <= parent->getNumKeys())
    {
        // Right sibling
        Node *rightNode = static_cast<Node *>(parent->getChild(rightSibling, 0).blockAddress);

        // Transfering all keys and pointers from right node to current node
        for (int i = cursor->getNumKeys(), j = 0; j < rightNode->getNumKeys(); i++, j++)
        {
            cursor->setKey(i, rightNode->getKey(j));
            cursor->setChildren(i, rightNode->getChildren(j));
        }

        // Updating numkeys and making sure that the last pointer in the current node points to the node after the right node
        cursor->setNumKeys(cursor->getNumKeys() + rightNode->getNumKeys());
        cursor->setChildren(cursor->getNumKeys(), rightNode->getChildren(rightNode->getNumKeys()));

        // We need to update the parent in order to fully remove the right node.
        deleteInternal(parent->getKey(rightSibling - 1), parent, rightNode);
    }
    this->nodesStored--;
    return 1;
}

int BPlusTree::deleteInternal(float key, Node *parent, Node *child)
{
    Node *cursor = parent;

    // If current parent is root
    if (cursor == this->rootNode)
    {

        // If we have to remove all keys in root (parent), we change the new root to its child
        if (cursor->getNumKeys() == 1)
        {

            // If the second pointer contains the child to delete, we make the first pointer the new root
            if (cursor->getChild(1, 0).blockAddress == child)
            {
                this->rootNode = static_cast<Node *>(cursor->getChild(0, 0).blockAddress);
                cout << "Root node changed!" << endl;

                return 1;
            }
            // if the first pointer contains the child to delete, we make the second pointer the new root
            else if (cursor->getChild(0, 0).blockAddress == child)
            {
                this->rootNode = static_cast<Node *>(cursor->getChild(1, 0).blockAddress);
                cout << "Root node changed!" << endl;
                return 1;
            }
        }
    }

    int positionToDelete; // position of the internal node to be deleted

    // Search for key to delete in parent based on the first key of the child to delete
    for (int i = 0; i < cursor->getNumKeys(); i++)
    {
        if (cursor->getKey(i) == key)
        {
            positionToDelete = i;
            break;
        }
    }

    // Deleting the key from the root
    for (int i = positionToDelete; i < cursor->getNumKeys(); i++)
    {
        cursor->setKey(i, cursor->getKey(i + 1));
    }

    // Search for pointer to delete in parent based on the key
    for (int i = 0; i < cursor->getNumKeys() + 1; i++)
    {
        if (cursor->getChild(i, 0).blockAddress == child)
        {
            positionToDelete = i;
            break;
        }
    }
    // Deleting the pointer from the root
    for (int i = positionToDelete; i < cursor->getNumKeys() + 1; i++)
    {
        cursor->setChild(i, cursor->getChild(i + 1, 0));
    }

    // Updating the number of keys in the parent
    cursor->setNumKeys(cursor->getNumKeys() - 1);

    // Check if the node size is valid according to the requirements
    // if (cursor->getNumKeys() >= ((maxKeys + 1) / 2 - 1))
    if (cursor->getNumKeys() >= ((maxKeys) / 2))
    {
        return 1;
    }

    // Satisfying this condition means that the parent is a root node and it does not matter if it doesn't satisfy the minimum keys condition for a non leaf node
    if (parent == this->rootNode)
    {
        return 1;
    }

    // We find the parent of the parent node to find the parent's siblings
    // Node *grandParent = findParent(this->rootNode, parent, parent->getKey(0));
    Node *grandParent = findParent(this->rootNode, cursor, cursor->getKey(0));
    int leftSibling, rightSibling;

    // Find the left and right sibling of the parent
    for (int i = 0; i < grandParent->getNumKeys() + 1; i++)
    {
        if (grandParent->getChild(i, 0).blockAddress == parent)
        {
            leftSibling = i - 1;
            rightSibling = i + 1;
            positionToDelete = i;
            break;
        }
    }

    // Check if left sibling exists
    if (leftSibling >= 0)
    {
        Node *leftNode = static_cast<Node *>(grandParent->getChild(leftSibling, 0).blockAddress);
        // Check if it is possible to take a key from the left sibling
        if (leftNode->getNumKeys() >= ((maxKeys) / 2))
        {
            // Making space at the beginning of the current parent to fit the incoming key from left sibling
            for (int i = cursor->getNumKeys(); i > 0; i--)
            {
                cursor->setKey(i, cursor->getKey(i - 1));
            }
            // Transfer borrowed key and cursor to pointer from left node
            cursor->setKey(0, grandParent->getKey(leftSibling));
            // Node* leftSibling = static_cast<Node*>(grandParent->getChild(leftSibling,0).blockAddress);
            grandParent->setKey(leftSibling, leftNode->getKey(leftNode->getNumKeys() - 1));

            // Move all pointers back in the cursor to fit a pointer
            for (int i = cursor->getNumKeys() + 1; i > 0; i--)
            {
                cursor->setChild(i, cursor->getChild(i - 1, 0));
            }

            cursor->setChild(0, leftNode->getChild(leftNode->getNumKeys(), 0));

            // Updating the number of keys in the node
            cursor->setNumKeys(cursor->getNumKeys() + 1);
            leftNode->setNumKeys(leftNode->getNumKeys() - 1);

            // Shift left sibling's last pointer left by one
            leftNode->setChild(cursor->getNumKeys(), leftNode->getChild(cursor->getNumKeys() + 1, 0));

            return 1;
        }
    }

    // Check if right sibling exists
    if (rightSibling <= grandParent->getNumKeys())
    {

        Node *rightNode = static_cast<Node *>(grandParent->getChild(rightSibling, 0).blockAddress);

        // Check if it is possible to take a key from the right sibling
        if (rightNode->getNumKeys() >= ((maxKeys) / 2))
        {

            // Transfer leftmost key and pointer from right node to current node
            cursor->setKey(cursor->getNumKeys(), grandParent->getKey(positionToDelete));
            grandParent->setKey(positionToDelete, rightNode->getKey(0));

            // Shift the keys in the right node left by one
            for (int i = 0; i < rightNode->getNumKeys() - 1; i++)
            {
                rightNode->setKey(i, rightNode->getKey(i + 1));
            }

            // Transfer first pointer from right node to cursor
            cursor->setChild(cursor->getNumKeys() + 1, rightNode->getChild(0, 0));

            // Shift pointers left for right node and delete the first pointer
            for (int i = 0; i < rightNode->getNumKeys(); ++i)
            {
                rightNode->setChild(i, rightNode->getChild(i + 1, 0));
            }

            // Updating the number of keys inside the nodes
            cursor->setNumKeys(cursor->getNumKeys() + 1);
            rightNode->setNumKeys(rightNode->getNumKeys() - 1);

            return 1;
        }
    }

    // Case 3 : Reaching here means that we cannot borrow keys from siblings so we must merge nodes
    // If left sibling exists we merge with it
    if (leftSibling >= 0)
    {
        Node *leftNode = static_cast<Node *>(grandParent->getChild(leftSibling, 0).blockAddress);

        // Making left node's upper bound to be cursor's lower bound
        leftNode->setKey(leftNode->getNumKeys(), grandParent->getKey(leftSibling));

        // Transfer all keys from current node to left node
        for (int i = leftNode->getNumKeys() + 1, j = 0; j < cursor->getNumKeys(); j++)
        {
            leftNode->setKey(i, cursor->getKey(j));
        }

        // Transfer all pointers from current node to left node
        for (int i = leftNode->numKeys + 1, j = 0; j < cursor->numKeys + 1; j++)
        {
            leftNode->setChild(i, cursor->getChild(j, 0));
            cursor->setChild(j, Address{nullptr, 0});
        }

        // Update numkeys
        leftNode->setNumKeys(leftNode->getNumKeys() + cursor->getNumKeys() + 1);
        cursor->setNumKeys(0);
        this->nodesStored--;

        // We need to update the parent in order to fully remove the current node
        deleteInternal(grandParent->getKey(leftSibling), grandParent, parent);
    }
    else if (rightSibling <= grandParent->getNumKeys())
    {
        Node *rightNode = static_cast<Node *>(grandParent->getChild(rightSibling, 0).blockAddress);

        // Upper bound of cursor is lower bound of right sibling
        cursor->setKey(cursor->getNumKeys(), grandParent->getKey(rightSibling - 1));

        // Transfer all keys from right node into current
        for (int i = cursor->getNumKeys() + 1, j = 0; j < rightNode->getNumKeys(); j++)
        {
            cursor->setKey(i, rightNode->getKey(j));
        }

        // Transfer all pointers from right node into current
        for (int i = cursor->getNumKeys() + 1, j = 0; j < rightNode->getNumKeys() + 1; j++)
        {
            cursor->setChild(i, rightNode->getChild(j, 0));
            rightNode->setChild(j, Address{nullptr, 0});
        }

        // Update variables
        cursor->setNumKeys(cursor->getNumKeys() + rightNode->getNumKeys() + 1);
        rightNode->setNumKeys(0);
        this->nodesStored--;

        // We need to update the parent in order to fully remove the right node
        Node *newRightNode = static_cast<Node *>(grandParent->getChild(rightSibling, 0).blockAddress);
        deleteInternal(grandParent->getKey(rightSibling - 1), grandParent, newRightNode);
    }
    return 1;
}