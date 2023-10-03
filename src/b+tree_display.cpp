#include <iostream>
#include "b+tree.hpp"

using namespace std;

void BPTree::displayTree(Node *root, bool fullTree)
{
    Node *current;

    if (root == NULL)
    {
        cout << "B+ Tree is empty!!!" << endl;
    }
    // if partial tree -- print only root and first child content
    if (!fullTree)
    {
        // printing root contents
        cout << "| Root node contains: ";
        for (int i = 0; i < root->size; i++)
        {
            cout << root->getDataKey(i).key_value << " ";
        }
        cout << endl;
        // printing 1st child contents
        if (!root->isLeaf)
        {
            current = root->getChildren(0);
            cout << "| First child node contains: ";
            for (int j = 0; j < current->size; j++)
            {
                cout << current->getDataKey(j).key_value << " ";
            }
            cout << endl;
        }
    }
    else
    {
        current = root;
        if (current != NULL)
        {
            for (int i = 0; i < current->size; i++)
            {
                cout << current->getDataKey(i).key_value << " | ";
            }

            cout << endl;
            if (!current->isLeaf)
            {
                for (int j = 0; j < current->size + 1; j++)
                {
                    displayTree(current->getChildren(j), true);
                }
            }
        }
    }
}

// get the height of the BPTree by traversing to leftmost child
int BPTree::treeHeight(Node *root)
{
    if (root->isLeaf)
    {
        return 1;
    }
    return treeHeight(root->getChildren(0)) + 1;
}