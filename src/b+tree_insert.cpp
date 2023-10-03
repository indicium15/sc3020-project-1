#include <iostream>
#include "b+tree.hpp"
#include <cmath>

using namespace std;

void BPTree::insertData(data_keys d)
{
    // if no root, create a new B+ tree root
    if (root == NULL)
    {
        root = new Node(maxDegree);
        root->key[0] = d;
        root->isLeaf = true;
        root->size = 1;
        numOfNodes++;
    }
    // else, traverse through the nodes to insert
    else
    {
        Node *current = root;
        Node *parent = nullptr;

        // check for repeated numVotes value
        Node *searchRep;
        searchRep = search(d.key_value, false, 0);
        if (searchRep != nullptr)
        {
            for (int i = 0; i < searchRep->size; i++)
            {
                // if repeated add to end of vector
                if (searchRep->key[i].key_value == d.key_value)
                {
                    searchRep->key[i].add.push_back(d.add[0]);
                    break;
                }
            }
            return;
        }
        // if currently pointed node is not a leaf node
        while (current->isLeaf == false)
        {
            parent = current;
            for (int i = 0; i < current->size; i++)
            {
                // if we found where we want to insert the data
                if (d.key_value < current->key[i].key_value)
                {
                    current = current->children[i];
                    break;
                }
                // if we reached the end
                if (i == current->size - 1)
                {
                    current = current->children[i + 1];
                    break;
                }
            }
        }

        // insert keys while there is space
        if (current->size < maxDegree)
        {
            int i = 0;
            while (d.key_value > current->key[i].key_value && i < current->size)
                i++;
            for (int j = current->size; j > i; j--)
            {
                current->key[j] = current->key[j - 1];
            }

            current->key[i] = d; // insert new key here
            current->size++;
            current->children[current->size] = current->children[current->size - 1];
            current->children[current->size - 1] = NULL;
        }

        // else need to split nodes
        else
        {
            // create a new leaf node
            Node *newLeaf = new Node(maxDegree);
            numOfNodes++;
            data_keys tempNode[maxDegree + 1];

            // point current to tempNode
            for (int i = 0; i < maxDegree; i++)
            {
                tempNode[i] = current->key[i];
            }
            int i = 0, j;
            // find where new node is inserted
            while (d.key_value > tempNode[i].key_value && i < maxDegree)
            {
                i++;
            }
            // update current tempNode to previous
            for (int j = maxDegree; j > i; j--)
            {
                tempNode[j] = tempNode[j - 1];
                // tempNode[j - 1] = tempNode[j - 2];
            }
            tempNode[i] = d; // insert key into tempNode
            newLeaf->isLeaf = true;
            // split into two leaf nodes
            current->size = ceil(((float)maxDegree + 1) / 2);
            newLeaf->size = floor((maxDegree + 1) / 2);

            current->children[current->size] = newLeaf;
            newLeaf->children[newLeaf->size] = current->children[maxDegree];
            current->children[maxDegree] = NULL;

            // update current to previous tempNode
            for (i = 0; i < current->size; i++)
            {
                current->key[i] = tempNode[i];
            }
            // update new key to tempNode
            for (i = 0, j = current->size; i < newLeaf->size; i++, j++)
            {
                newLeaf->key[i] = tempNode[j];
            }

            // if current pointer is root
            if (current == root)
            {
                // create a new node
                Node *firstLeaf = getFirstLeaf(newLeaf);
                Node *newRoot = new Node(maxDegree);
                numOfNodes++;
                newRoot->key[0] = newLeaf->key[0];
                newRoot->children[0] = current;
                newRoot->children[1] = newLeaf;
                newRoot->isLeaf = false;
                newRoot->size = 1;
                root = newRoot;
            }
            else
            {
                insertInternal(newLeaf->key[0], parent, newLeaf); // insert new parent
            }
        }
    }
}

// update parent node to point at child nodes, and adds a parent if needed
void BPTree::insertInternal(data_keys data, Node *current, Node *child)
{
    if (current->size < maxDegree) // if internal not full
    {
        int i = 0;
        while (data.key_value > current->key[i].key_value && i < current->size)
            i++;

        // insert accordingly
        for (int j = current->size; j > i; j--)
        {
            current->key[j] = current->key[j - 1];
        }
        for (int j = current->size + 1; j > i + 1; j--)
        {
            current->children[j] = current->children[j - 1];
        }
        current->key[i] = data;
        current->size++;
        current->children[i + 1] = child;
    }
    else
    { // if full, split internal node and create new internal node
        Node *newInt = new Node(maxDegree);
        numOfNodes++;

        data_keys tempKey[maxDegree + 1];
        Node *tempPtr[maxDegree + 2];
        for (int i = 0; i < maxDegree; i++)
        {
            tempKey[i] = current->key[i];
        }
        for (int i = 0; i < maxDegree + 1; i++)
        {
            tempPtr[i] = current->children[i];
        }
        int i = 0, j;
        while (data.key_value > tempKey[i].key_value && i < maxDegree)
            i++;

        for (int j = maxDegree; j > i; j--)
        {
            tempKey[j] = tempKey[j - 1];
        }
        tempKey[i] = data;
        for (int j = maxDegree + 1; j > i + 1; j--)
        {
            tempPtr[j] = tempPtr[j - 1];
        }
        tempPtr[i + 1] = child;
        // internal node cannot be leaf node
        newInt->isLeaf = false;

        // split into two nodes
        current->size = ceil(float(maxDegree) / 2);
        newInt->size = floor(float(maxDegree) / 2);

        for (int i = 0; i < current->size; i++)
        {
            current->key[i] = tempKey[i];
        }

        for (int i = 0; i <= current->size; i++)
        {
            current->children[i] = tempPtr[i];
        }

        // insert node as new internal node
        for (i = 0, j = current->size + 1; i < newInt->size; i++, j++)
        {
            newInt->key[i] = tempKey[j];
        }
        for (i = 0, j = current->size + 1; i < newInt->size + 1; i++, j++)
        {
            newInt->children[i] = tempPtr[j];
        }
        // if currently pointing to root node
        if (current == root)
        {
            Node *firstLeaf = getFirstLeaf(newInt);
            Node *newRoot = new Node(maxDegree);
            numOfNodes++;
            newRoot->key[0] = firstLeaf->key[0];
            newRoot->children[0] = current;
            newRoot->children[1] = newInt;
            newRoot->isLeaf = false;
            newRoot->size = 1;
            root = newRoot;
        }
        else
        { // split and make new internal node recursively
            // find parent of current cursor
            //  Node *parent = findParent(root, current);
            //  std::cout << parent->key[0].key_value;
            insertInternal(tempKey[current->size], findParent(root, current), newInt);
        }
    }
}

// find parent node
Node *BPTree::findParent(Node *current, Node *child)
{
    Node *parent = current;

    if (current->isLeaf) // node is not internal/parent node
    {
        return NULL;
    }
    else if ((current->children[0])->isLeaf)
    {
        return NULL;
    }
    for (int i = 0; i < current->size + 1; i++)
    { // traverse through the nodes
        if (current->children[i] == child)
        {
            parent = current;
            return parent;
        }
        else
        {
            parent = findParent(current->children[i], child);
            if (parent != NULL)
                return parent;
        }
    }
    return parent;
}

Node *BPTree::getFirstLeaf(Node *current)
{
    if (current == NULL)
    {
        return NULL;
    }
    else
    {
        while (!current->isLeaf)
        {
            current = current->getChildren(0);
        }
        return (current);
    }
}
