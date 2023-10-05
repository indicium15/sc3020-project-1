#pragma once

using namespace std;

#include <iostream>
#include <tuple>
#include <vector>
#include <queue>
#include <cstring>
#include <unordered_map>
#include <cmath>

struct data_keys
{
    float key_value;
    vector<void *> add;
};

class Node
{
    int size;
    bool isLeaf;    // true if its a leaf node
    data_keys *key; // to hold the array of keys in the node
    friend class BPTree;
    Node **children; // ptr to an array of children. If leaf node children ptr consist of data or next node ptr

public:
    Node(int maxDegree)
    {
        this->isLeaf = true;
        this->key = new data_keys[maxDegree];
        this->children = new Node *[maxDegree + 1];
    }
    // need to remove this function. Here for hard-coding test
    void setDataKey(data_keys d, int i)
    {
        this->key[i] = d;
    }
    void setChildren(Node *child, int i)
    {
        this->children[i] = child;
    }
    data_keys getDataKey(int i)
    {
        return this->key[i];
    }
    Node *getChildren(int i)
    {
        return this->children[i];
    }
};

class BPTree
{
    void insertInternal(data_keys, Node *, Node *);
    void updateParent(Node *, data_keys);
    int removeInternal(data_keys, Node *, Node *);
    Node *findParent(Node *, Node *);

private:
    Node *root;
    int maxDegree;
    int numOfNodes = 0;
    int blkSize;

public:
    BPTree(int blkSize)
    {
        this->root = NULL;
        this->blkSize = blkSize;
        maxDegree = getMaxDegree(blkSize);
    }
    void insertData(data_keys);
    int removeData(data_keys);
    Node *getFirstLeaf(Node *current);
    void displayTree(Node *, bool);
    Node *search(float, bool, float);
    int treeHeight(Node *);
    int height(Node *);
    // get new key to add to parent
    data_keys getNewKey(Node *cursor);

    // basic setters
    void setRoot(Node *_root)
    {
        this->root = _root;
    }
    // basic getters
    Node *getRoot()
    {
        return this->root;
    }
    int getMaxDegree(int blkSize)
    {
        return floor((blkSize - 8) / 26);
    }

    int getnumOfNodes()
    {
        return numOfNodes;
    }
    
};