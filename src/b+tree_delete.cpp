#include "bplustree.h"
#include "storage.h"
#include <iostream>
#include "types.h"

// Node class implementation
Node::Node(int maxKeys, bool isLeaf) {
    this->keys = new float[maxKeys];
    this->children = vector<vector<Address>>(maxKeys + 1);
    this->isLeaf = isLeaf;
    this->numKeys = 0;
}

Node::~Node() {
    delete[] keys;
}

// BPlusTree class implementation
BPlusTree::BPlusTree() {
    this->rootNode = nullptr;
    this->keysStored = 0;
    this->blockSize = 400;
    this->nodeSize = blockSize;
    this->nodesStored = 0;
    int calculatedCapacity, count = 0;
    while (blockSize >= count + sizeof(Address) + sizeof(float)) {
        count += sizeof(Address) + sizeof(float);
        calculatedCapacity++;
    }
    if (calculatedCapacity == 0) {
        cerr << "Error: block size is too small" << endl;
    }
    this->maxKeys = calculatedCapacity;
}

BPlusTree::~BPlusTree() {
    // Implement destructor to free memory
    // Call the recursive destructor function starting from the root
    destroyTree(rootNode);
}

// Recursive destructor function to destroy the B+ tree
void BPlusTree::destroyTree(Node* currentNode) {
    if (currentNode == nullptr) {
        return; // Base case: Reached a null node
    }

    // Recursively destroy child nodes (for non-leaf nodes)
    if (!currentNode->isLeaf) {
        for (int i = 0; i <= currentNode->numKeys; i++) {
            Node* childNode = static_cast<Node*>(currentNode->children[i][0].blockAddress);
            destroyTree(childNode);
        }
    }

    // Delete keys and the current node
    delete[] currentNode->keys;
    delete currentNode;
}

// Deletion code (complete implementation)
bool BPlusTree::remove(float key) {
    if (rootNode == nullptr) {
        return false;  // Tree is empty, nothing to remove
    }

    // Call the recursive remove function starting from the root
    bool keyRemoved = removeRec(rootNode, key);

    // If the root node is empty after removal, update the root
    if (rootNode->numKeys == 0) {
        Node *newRoot = static_cast<Node*>(rootNode->children[0][0].blockAddress);
        this->nodesStored--;  // Decrement the node count
        delete rootNode;
        rootNode = newRoot;
    }

    return keyRemoved;
}

// BPlusTree::removeRec function for recursive removal
bool BPlusTree::removeRec(Node *currentNode, float key) {
    // Find the key index in the current node (if it exists)
    int keyIndex = -1;
    for (int i = 0; i < currentNode->numKeys; i++) {
        if (currentNode->keys[i] == key) {
            keyIndex = i;
            break;
        }
    }

    // Keep track of left and right siblings to borrow from
    Node* leftSibling = nullptr;
    Node* rightSibling = nullptr;

    // While not a leaf, keep following the nodes to correct key
    while (!currentNode->isLeaf) {
        // Set the parent of the node (in case we need to assign new child later)
        Node* parentNode = currentNode;
        Address parentDiskAddress = cursorDiskAddress;
        cursorDiskAddress = getAddress(currentNode);

        // Check through all keys of the node to find key and pointer to follow downwards
        int childIndex = 0;
        while (childIndex < currentNode->numKeys && key > currentNode->keys[childIndex]) {
            childIndex++;
        }

        // Keep track of left and right to borrow
        if (childIndex > 0) {
            leftSibling = static_cast<Node*>(parentNode->children[childIndex - 1][0].blockAddress);
        }
        if (childIndex < parentNode->numKeys) {
            rightSibling = static_cast<Node*>(parentNode->children[childIndex + 1][0].blockAddress);
        }

        // If key is lesser than current key, go to the left pointer's node
        if (childIndex > 0) {
            // Load node in from disk to main memory
            currentNode = loadNode(leftSibling);
            // Update cursorDiskAddress to maintain address in disk if we need to update nodes
            cursorDiskAddress = getAddress(leftSibling);
        }
        // Else if key larger than all keys in the node, go to last pointer's node (rightmost)
        else {
            // Load node in from disk to main memory
            currentNode = loadNode(rightSibling);
            // Update cursorDiskAddress to maintain address in disk if we need to update nodes
            cursorDiskAddress = getAddress(rightSibling);
        }
    }

    // Now that we have found the leaf node that might contain the key, we will try and find the position of the key here (if it exists)
    // Search if the key to be deleted exists in this bplustree
    // Also works for duplicates
    bool keyFound = false;
    int pos = -1;  // Position where we found the key

    for (int i = 0; i < currentNode->numKeys; i++) {
        if (currentNode->keys[i] == key) {
            keyFound = true;
            pos = i;
            break;
        }
    }

    // If key to be deleted does not exist in the tree, return error
    if (!keyFound) {
        return false;
    }

    // Update numNodes and numNodesDeleted after deletion
    numNodesDeleted++;

    // We must delete the entire linked-list before we delete the key, otherwise we lose access to the linked list head
    // Delete the linked list stored under the key

    // Now, we can delete the key. Move all keys/pointers forward to replace its values
    // Change the key removed to empty float
    for (int i = pos; i < currentNode->numKeys - 1; i++) {
        currentNode->keys[i] = currentNode->keys[i + 1];
    }

    // Move the last pointer forward (if any)
    currentNode->children[currentNode->numKeys - 1][0] = nullptr;

    // Set all forward pointers from numKeys onwards to nullptr
    for (int i = currentNode->numKeys; i < maxKeys; i++) {
        currentNode->children[i][0] = nullptr;
    }

    // If current node is root, check if tree still has keys
    if (currentNode == rootNode) {
        if (currentNode->numKeys == 0) {
            // Delete the entire root node and deallocate it
            delete currentNode;
            // Deallocate block used to store root node
            storageManager->deallocateBlock(rootNodeDiskAddress);
            // Reset root pointers in the B+ Tree
            rootNode = nullptr;
        }
    } else {
        // Update numNodes and numNodesDeleted after deletion
        numNodes--;

        // Save to disk
        saveNode(currentNode);
    }

    // If we didn't delete from root, we check if we have minimum keys ⌊(n+1)/2⌋ for leaf
    if (currentNode->numKeys >= (maxKeys + 1) / 2) {
        // No underflow, so we're done
        return true;
    }

    // Update numNodes and numNodesDeleted after deletion
    numNodesDeleted++;

    // Save to disk
    saveNode(currentNode);

    // If we reach here, it means we have underflow (not enough keys for a balanced tree)
    // Try to take from left sibling (node on the same level) first
    if (leftSibling != nullptr && leftSibling->numKeys > (maxKeys / 2)) {
        // Check if left sibling even exists
        if (leftSibling != nullptr) {
            // Load in left sibling from disk
            leftSibling = loadNode(leftSibling);

            // Check if we can steal (ahem, borrow) a key without underflow
            if (leftSibling->numKeys > (maxKeys / 2)) {
                // We will insert this borrowed key into the rightmost of the current node (larger)
                // Shift last pointer back by one first
                for (int i = currentNode->numKeys; i > 0; i--) {
                    currentNode->children[i][0] = currentNode->children[i - 1][0];
                }
                // No need to shift remaining pointers and keys since we are inserting on the rightmost
                // Transfer borrowed key and pointer (leftmost of the right node) over to the rightmost of the current node
                currentNode->keys[currentNode->numKeys] = leftSibling->keys[leftSibling->numKeys - 1];
                currentNode->children[currentNode->numKeys][0] = leftSibling->children[leftSibling->numKeys][0];
                leftSibling->numKeys--;

                // Update parent node's key to be the new lower bound of the right sibling
                parentNode->keys[childIndex - 1] = leftSibling->keys[leftSibling->numKeys - 1];

                // Save parent to disk
                saveNode(parentNode);

                // Save right sibling to disk
                saveNode(leftSibling);

                // Save current node to disk
                saveNode(currentNode);

                return true;
            }
        }
    }

    // If we reach here, it means no sibling we can steal from
    // To resolve underflow, we must merge nodes

    // If left sibling exists, merge with it
    if (leftSibling != nullptr) {
        // Load in left sibling from disk
        leftSibling = loadNode(leftSibling);

        // Make the left node's upper bound to be the cursor's lower bound
        leftSibling->keys[leftSibling->numKeys] = parentNode->keys[childIndex - 1];

        // Transfer all keys and pointers from the current node to the left node
        for (int i = 0; i < currentNode->numKeys; i++) {
            leftSibling->keys[leftSibling->numKeys + 1 + i] = currentNode->keys[i];
            leftSibling->children[leftSibling->numKeys + 1 + i][0] = currentNode->children[i][0];
        }

        // Note: Merging will always succeed due to ⌊(n)/2⌋ (left) + ⌊(n-1)/2⌋ (current)
        // Transfer all pointers too
        leftSibling->children[leftSibling->numKeys + 1 + currentNode->numKeys][0] = currentNode->children[currentNode->numKeys][0];

        // Update variables, make the left node's last pointer point to the next leaf node pointed to by the current node
        leftSibling->children[leftSibling->numKeys + 1 + currentNode->numKeys][0] = currentNode->children[currentNode->numKeys][0];

        // Save left node to disk
        saveNode(leftSibling);

        // We need to update the parent in order to fully remove the current node
        // Find the key to delete in the parent based on the child's lower bound key
        for (int i = 0; i < parentNode->numKeys; i++) {
            if (parentNode->children[i][0].blockAddress == currentNode) {
                // Delete the key by shifting all keys forward
                for (int j = i; j < parentNode->numKeys - 1; j++) {
                    parentNode->keys[j] = parentNode->keys[j + 1];
                }

                // Search for the pointer to delete in the parent
                // Remember pointers are on the RIGHT for non-leaf nodes
                // Now move all pointers from that point on forward by one to delete it
                for (int j = i + 1; j < parentNode->numKeys; j++) {
                    parentNode->children[j][0] = parentNode->children[j + 1][0];
                }

                // Update numKeys
                parentNode->numKeys--;

                // Check if there's underflow in the parent
                // No underflow, life is good
                if (parentNode->numKeys >= (maxKeys + 1) / 2) {
                    // Save parent to disk
                    saveNode(parentNode);

                    return true;
                }
            }
        }

        // If we reach here, it means there's underflow in the parent's keys
        // Try to steal some from neighboring nodes
        // If we are the root, we are screwed, just give up
        if (parentNode == rootNode) {
            return true;
        }

        // If not, we need to find the parent of this parent to get our siblings
        Address grandparentAddress = parentNode->parent;
        Node *grandparentNode = loadNode(grandparentAddress);

        // Pass in the lower bound key of our child to search for it
        for (int i = 0; i < grandparentNode->numKeys + 1; i++) {
            if (grandparentNode->children[i][0].blockAddress == parentNode) {
                // Load parent into main memory
                parentNode = loadNode(grandparentNode->children[i][0]);

                // Find left and right sibling of cursor, iterate through pointers
                for (int j = i - 1; j >= 0; j--) {
                    if (grandparentNode->children[j][0].blockAddress != nullptr) {
                        leftSibling = loadNode(grandparentNode->children[j][0]);
                        break;
                    }
                }

                for (int j = i + 1; j <= grandparentNode->numKeys; j++) {
                    if (grandparentNode->children[j][0].blockAddress != nullptr) {
                        rightSibling = loadNode(grandparentNode->children[j][0]);
                        break;
                    }
                }

                // Try to borrow a key from either the left or right sibling
                // Check if left sibling exists. If so, try to borrow
                if (leftSibling != nullptr) {
                    // Load in left sibling from disk
                    leftSibling = loadNode(leftSibling);

                    // Check if we can steal (ahem, borrow) a key without underflow
                    // Non-leaf nodes require a minimum of ⌊n/2⌋
                    if (leftSibling->numKeys > (maxKeys / 2)) {
                        // We will insert this borrowed key into the leftmost of the current node (smaller)
                        // Shift all remaining keys and pointers back by one
                        for (int k = currentNode->numKeys; k > 0; k--) {
                            currentNode->keys[k] = currentNode->keys[k - 1];
                            currentNode->children[k][0] = currentNode->children[k - 1][0];
                        }

                        // Transfer borrowed key and pointer to the cursor from the left node
                        currentNode->keys[0] = leftSibling->keys[leftSibling->numKeys - 1];
                        currentNode->children[0][0] = leftSibling->children[leftSibling->numKeys][0];
                        leftSibling->numKeys--;

                        // Basically, duplicate cursor lower bound key to keep pointers correct
                        // Move all pointers back to fit the new one
                        // Add pointers to the cursor from the left node
                        for (int k = currentNode->numKeys + 1; k > 0; k--) {
                            currentNode->children[k][0] = currentNode->children[k - 1][0];
                        }

                        // Change key numbers
                        currentNode->children[0][0] = leftSibling->children[leftSibling->numKeys + 1][0];

                        // Update left sibling (shift pointers left)
                        saveNode(leftSibling);

                        // Save parent to disk
                        saveNode(parentNode);

                        // Save current node to disk
                        saveNode(currentNode);

                        return true;
                    }
                }
            }
        }
    }
}

// If left sibling doesn't exist, try to merge with the right sibling
if (rightSibling != nullptr) {
    // Load in right sibling from disk
    rightSibling = loadNode(rightSibling);

    // Note we are moving right node's stuff into ours
    // Transfer all keys and pointers from the right node into the current node
    for (int i = 0; i < rightSibling->numKeys; i++) {
        currentNode->keys[currentNode->numKeys + 1 + i] = rightSibling->keys[i];
        currentNode->children[currentNode->numKeys + 1 + i][0] = rightSibling->children[i][0];
    }

    // Note: Merging will always succeed due to ⌊(n)/2⌋ (left) + ⌊(n-1)/2⌋ (current)
    // Transfer all pointers from the right node into the current node
    currentNode->children[currentNode->numKeys + 1 + rightSibling->numKeys][0] = rightSibling->children[rightSibling->numKeys][0];

    // Update variables
    // Make the current node's last pointer point to the next leaf node pointed to by the right node
    currentNode->children[currentNode->numKeys + 1 + rightSibling->numKeys][0] = rightSibling->children[rightSibling->numKeys + 1][0];

    // Save current node to disk
    saveNode(currentNode);

    // We need to update the parent in order to fully remove the right node
    // Find the key to delete in the parent based on the child's lower bound key
    for (int i = 0; i < parentNode->numKeys; i++) {
        if (parentNode->children[i][0].blockAddress == currentNode) {
            // Delete the key by shifting all keys forward
            for (int j = i; j < parentNode->numKeys - 1; j++) {
                parentNode->keys[j] = parentNode->keys[j + 1];
            }

            // Search for the pointer to delete in the parent
            // Remember pointers are on the RIGHT for non-leaf nodes
            // Now move all pointers from that point on forward by one to delete it
            for (int j = i + 1; j < parentNode->numKeys; j++) {
                parentNode->children[j][0] = parentNode->children[j + 1][0];
            }

            // Update numKeys
            parentNode->numKeys--;

            // Save parent to disk
            saveNode(parentNode);

            // Save right sibling to disk
            saveNode(rightSibling);

            // Delete right node
            delete rightSibling;

            return true;
        }
    }
}

return true;
}

// Helper function to delete a child from a parent node
void BPlusTree::deleteChild(Node *parentNode, Address childAddress) {
    // Load in the cursor (parent) and child from disk to get the latest copy
    parentNode = loadNode(parentNode);
    Node *childNode = loadNode(childAddress);

    // Check if the cursor is root via disk address
    if (parentNode == rootNode) {
        // Get the address of the child to delete
        Address childToDelete = childAddress;

        if (parentNode->children[1][0].blockAddress != nullptr) {
            childToDelete = parentNode->children[1][0];
        }

        // If the current parent is root
        if (parentNode == rootNode) {
            // If we have to remove all keys in the root, we reset the root node to be the first and only child (new root)
            // This is why we look at the leftmost child first
            if (parentNode->numKeys == 0) {
                // Get the new root
                rootNode = loadNode(childToDelete);
                // Deallocate the old root node
                storageManager->deallocateBlock(cursorDiskAddress);
                // Update the cursor address to the new root address
                cursorDiskAddress = getAddress(rootNode);
            }
        }
    }

    // Else, if the cursor is not root, we can delete child from parent
    for (int i = 0; i < parentNode->numKeys + 1; i++) {
        if (parentNode->children[i][0].blockAddress == childNode) {
            // Delete the child pointer by shifting all pointers forward
            for (int j = i; j < parentNode->numKeys; j++) {
                parentNode->children[j][0] = parentNode->children[j + 1][0];
            }
            // Update numKeys
            parentNode->numKeys--;

            // Save parent to disk
            saveNode(parentNode);

            // If we are deleting a leaf node, we also need to deallocate its block
            if (childNode->isLeaf) {
                storageManager->deallocateBlock(childAddress);
                // Update the number of nodes deleted
                numNodesDeleted++;
            }

            // Delete the child node
            delete childNode;
            break;
        }
    }
}

// Helper function to load a node from disk and return it in main memory
Node *BPlusTree::loadNode(Address diskAddress) {
    Node *node = static_cast<Node *>(storageManager->readBlock(diskAddress));
    return node;
}

// Helper function to save a node to disk
void BPlusTree::saveNode(Node *node) {
    storageManager->writeBlock(cursorDiskAddress, node);
}

// Helper function to get the disk address of a node
Address BPlusTree::getAddress(Node *node) {
    return cursorDiskAddress;
}

// Other BPlusTree member functions

int BPlusTree::getHeight() const {
    // Calculate the height of the tree by traversing from the root to a leaf node
    int height = 0;
    Node *currentNode = rootNode;
    while (currentNode != nullptr && !currentNode->isLeaf) {
        currentNode = static_cast<Node *>(currentNode->children[0][0].blockAddress);
        height++;
    }
    return height;
}

int BPlusTree::getMaxKeys() const {
    return maxKeys;
}

int BPlusTree::getKeysStored() const {
    return keysStored;
}

int BPlusTree::getNodesStored() const {
    return nodesStored;
}

int BPlusTree::getNodesDeleted() const {
    return numNodesDeleted;
}

int BPlusTree::getNodesCount() const {
    return numNodes;
}

// Other utility functions
// (You can add these functions to the BPlusTree class if needed)

