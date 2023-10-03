#include "b+tree.hpp"
#include <queue>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

/**
 * @brief
 * Remove key from tree.
 * @param x Key to be removed.
 * @return int Number of removed nodes.
 */

int BPTree::removeData(data_keys x)
{
  // need to check root is not null, if not null then we can assign current node to root and start traversing to leaf nodes
  if (root != nullptr)
  {
    Node *parent;
    Node *current = root;
    //
    int lSPtr, rSPtr;

    /*Once we assigned the root node to current, we can start traversing the tree.
    We do this by setting a parent node to current Node and increase the size until you reach the leaf Node
    */
    while (!current->isLeaf)
    {
      parent = current;
      int j;
      if (x.key_value >= current->key[current->size - 1].key_value)
      {
        j = current->size;
      }
      else
      {
        j = 0;
        while (x.key_value >= current->key[j].key_value)
        {
          j++;
        }
      }
      lSPtr = j - 1;
      rSPtr = j + 1;
      // go down to the child node until you reach a leaf node
      current = current->children[j];
    }

    // Can we replace this by the search query fn? We search for the key value if not found key does not exist. int k represents the position of the current node
    int k = 0;
    while (x.key_value > current->key[k].key_value)
      k += 1;
    // Key does not exist in the tree.
    if (x.key_value != current->key[k].key_value)
    {
      ////cout << "key is not found!" << endl;
      return 0;
    }

    // Delete the key by shifting all elements that are above the key down.
    for (int i = k; i < current->size; i++)
    {
      current->key[i] = current->key[i + 1];
    }
    current->size--;

    if (current == root) // If it is root node, make all ptr null.
    {
      ////cout << "Deleted " << x.key_value << " "
      //     << "from leaf node successfully" << endl;
      numOfNodes--;
      int m = 0;
      while (m < maxDegree + 1)
      {
        current->children[m] = nullptr;
      }

      if (current->size == 0) // If all keys are deleted.
      {
        ////cout << "Tree is deleted!" << endl;
        delete[] current->key;
        delete[] current->children;
        delete current;
        root = nullptr;
        return 1;
      }
      return 0;
    }

    // Shift the pointer to the adjacent leaf node.
    current->children[current->size + 1] = nullptr;
    current->children[current->size] = current->children[current->size + 1];
    // cout << "Deleted " << x.key_value << " "
    //<< " from leaf node successfully" //<< endl;
    numOfNodes -= 1;

    // Update parent if necessary (When position of key in leaf node to be removed is 0).
    if (k == 0)
    {
      // cout << "Update parent!" << endl;
      updateParent(current, current->key[0]);
    }
    // There are sufficient keys in node.
    if (current->size >= (maxDegree + 1) / 2)
    {
      // cout << "Sufficient nodes!" << endl;
      return 1;
    }

    // cout << "Underflow condition!" << endl;
    //  Underflow condition.
    //  Try to transfer a key from sibling node.
    //  Check if left sibling exists.
    if (lSPtr >= 0)
    {
      Node *lNode = parent->children[lSPtr];
      // cout << "Shift from left sibling!" << endl;
      //  Check if it is possible to transfer.
      if (lNode->size >= (maxDegree + 1) / 2 + 1)
      {
        // Create space for transferring key.
        for (int i = current->size; i > 0; i--)
        {
          current->key[i] = current->key[i - 1];
        }
        // Update pointer to the next node.
        current->size++;
        current->children[current->size] = current->children[current->size - 1];
        current->children[current->size - 1] = nullptr;

        // Transfer key from left sibling to node.
        current->key[0] = lNode->key[lNode->size - 1];

        // Update pointer to the next node.
        lNode->size--;
        lNode->children[lNode->size] = current;
        lNode->children[lNode->size + 1] = nullptr;

        // Update parent.
        parent->key[lSPtr] = current->key[0];
        return 0;
      }
    }
    // Check if right sibling exists.
    if (rSPtr <= parent->size)
    {
      Node *rNode = parent->children[rSPtr];
      // cout << "Shift keys from right node!" << endl;
      //  Check if it is possible to transfer.
      if (rNode->size >= (maxDegree + 1) / 2 + 1)
      {

        // Shift pointer that points to the next leaf.
        current->size++;
        current->children[current->size] = current->children[current->size - 1];
        current->children[current->size - 1] = nullptr;

        // Transfer key from right sibling.
        current->key[current->size - 1] = rNode->key[0];

        // Update pointer of right sibling.
        rNode->size--;
        rNode->children[rNode->size] = rNode->children[rNode->size + 1];
        rNode->children[rNode->size + 1] = nullptr;

        // Shift keys of right sibling down by 1.
        // No need to shift ptr cause they are null.
        for (int i = 0; i < rNode->size; i++)
        {
          rNode->key[i] = rNode->key[i + 1];
        }
        // Update parent key.
        parent->key[rSPtr - 1] = rNode->key[0];
        return 0;
      }
    }
    // If left sibling exists, merge with it.
    if (lSPtr >= 0)
    {
      // cout << "We merged with left sibling!" << endl;
      Node *lNode = parent->children[lSPtr];

      // Transfer all keys to left sibling.
      int s = current->size;
      int n = 0;
      while (n < lNode->size)
      {
        current->key[s] = lNode->key[n];
        s += 1;
        n += 1;
      }
      lNode->children[lNode->size] = nullptr;
      lNode->size += current->size;

      // Transfer pointer to adjacent leaf node.
      lNode->children[lNode->size] = current->children[current->size];

      // Delete parent node key of deleted current. Calls removeInternal
      int numNodeMerged = removeInternal(parent->key[lSPtr], parent, current);

      delete[] current->key;
      delete[] current->children;
      delete current;
      // cout << "Deleted 1" << endl;
      //  Returns total deleted = 1 + those deleted when merging
      return 1 + numNodeMerged;
    }
    // Another case: Merge with right sibling only if the right node exists
    else if (rSPtr <= parent->size)
    {
      // create a right node to the current node that is represented by
      // cout << "Merged with right sibling!" << endl;
      Node *rNode = parent->children[rSPtr];

      // Transfer all keys to current.
      int s = current->size;
      int n = 0;
      while (n < rNode->size)
      {
        current->key[s] = rNode->key[n];
        s += 1;
        n += 1;
      }

      // Transfer pointer to adjacent leaf node.
      current->children[current->size] = nullptr;
      current->size += rNode->size;
      current->children[current->size] = rNode->children[rNode->size];

      // Delete parent node key.
      int numNodesMerged = removeInternal(parent->key[rSPtr - 1], parent, rNode);
      delete[] rNode->key;
      delete[] rNode->children;
      delete rNode;
      // cout << "Deleted right node (1)" << endl;
      return 1 + numNodesMerged;
    }
  }

  // When root does not exist, tree does not exist
  else
  {
    throw std::logic_error("Tree is empty!");
    // return -1;
  }
  return 0;
}

/*this function removes specified internal nodes to be deleted using vectors implementation.
data_keys d refers to the internal nodes to be deleted
Node *current refers to the node at which we are currently at and need to move the current to the node to
be deleted in order to remove the specified nodes
Node *child refers to the child of the current node
*/
int BPTree::removeInternal(data_keys x, Node *current, Node *currentChild)
{
  /*basic idea: if current node is equal to the root and we need to delete the root, the b+ tree will not have a root,
  therefore the child node becomes the root
  We also need to check if the ptr beside the first key point to the child node as the pointer that becomes the new root cannot
  be pointing to the child node.
  We create the new root by assigning the pointer that was not pointing the to the node to be deleted to the root  */
  if (current == root && current->size == 1)
  {
    if (current->children[0] == currentChild)
    {
      if (current->children[1] == currentChild)
      {
        // Set new root.//figure out how to expand this list comprehension thing
        root = current->children[0] == currentChild ? current->children[1] : current->children[0];

        /*Once we created the new root of the tree, we can now delete the keys and ptr of the child node and also delete the node itself.
        We also need to keep in mind that the number of nodes also need to reduced by one as we removed the child node
        */
        delete[] currentChild->key;
        delete[] currentChild->children;
        delete currentChild;
        numOfNodes--;

        // cout << "Deleted 1" << endl;

        // Delete old root node.
        delete[] current->key;
        delete[] current->children;
        delete current;
        numOfNodes--;
        // cout << "Deleted 1" << endl;

        // cout << "Root node changed." << endl;
        return 0;
      }
    }
  }
  // In order to delete a key, we need to determine key position
  int a = 0; // represents the position of the key
  while (a < current->size && x.key_value != current->key[a].key_value)
  {
    a++;
  }
  // Once we located the key, we can implicitly delete it by shifting all the keys forward
  int i = a;
  while (i < current->size - 1) // current->size - 1 ??
  {
    current->key[i] = current->key[i + 1];
    i++;
  }

  // In order to delete a pointer, we need to determine position position
  int b = 0; // represents position of pointer
  while (b < current->size + 1 && current->children[b] != currentChild)
    b++;

  // Once we located the pointer, we can implicitly delete it by shifting each pointer to the left
  i = b;
  while (i < current->size) // current->size - 1 ??
  {
    current->children[i] = current->children[i + 1];
    i++;
  }

  // As we have deleted the key, need to decrease the size of the tree by 1
  current->size--;

  /*Need to ensure that current number of keys after deletion satisfies the minimum no. of keys per node.
  However if there is only the root, then size is not constrained*/
  if (current->size >= (maxDegree + 1) / 2 - 1)
  {
    // cout << "Deleted " << x.key_value << " "
    //   << " from internal node successfully" << endl;
    numOfNodes--;
    return 0;
  }
  else if (current == root)
  {
    // cout << "Deleted " << x.key_value << " "
    //<< " from internal node successfully" //<< endl;
    numOfNodes--;
    return 0;
  }
  else
  {
    // cout << "Deletion of" << x.key_value << " "
    //<< " does not satisy size requirement" << endl;
  }
  // if new node size does not satisfy the requirement, we need to transfer from sibling nodes.
  Node *parent = findParent(root, current);
  // need pointers to the left sibling node and the right sibling node as well as nodes lNode and rNode
  int lSPtr, rSPtr;

  // Get index of pointer to the node in parent node.
  int k = 0;
  while (k < parent->size + 1 && parent->children[k] != current)
    k++;
  lSPtr = k - 1;
  rSPtr = k + 1;

  // MERGING WITH LEFT SIBLING:
  // general idea: check if there is left/right sibling -->
  // check if the size requirement will still be satisfied-->
  // increase the size of the current node and pointers
  if (lSPtr >= 0)
  {
    // cout << "Merge with left sibling!" << endl;
    Node *lNode = parent->children[lSPtr];

    if (lNode->size >= (maxDegree + 1) / 2)
    {
      /*e represents the size of the current node and after transferring the keys over,
      need to decrease the size until all is transferred over*/
      int e = current->size;
      while (e > 0)
      {
        current->key[e] = current->key[e - 1];
        e--;
      }
      int f = current->size + 1;
      while (f > 0)
      {
        current->children[f] = current->children[f - 1];
        f--;
      }
      // Transfer key from left sibling.
      current->key[0] = parent->key[lSPtr]; // lowest key for the current subtree.

      // leftNode->key[leftNode->size - 1] is the smallest key of the node subtree that is being transferred.
      parent->key[lSPtr] = lNode->key[lNode->size - 1];

      // Transfer pointer from left sibling.
      current->children[0] = lNode->children[lNode->size];

      // Update sizes of nodes.
      current->size++;
      lNode->size--;
      // cout << "Transferred " << current->key[0].key_value
      //<< "from left sibling of node!" << endl;
      return 0;
    }
  }

  // MERGING WITH RIGHT SIBLING:
  // k represents the position of pointer to the current node
  // general idea: check if there is left/right sibling -->
  // check if the size requirement will still be satisfied-->
  // but the difference between merging with left sibling vs right sibling is
  // that we are not required to make space for the transferred key as it is the largest in the current node.
  if (rSPtr <= parent->size)
  {
    // cout << "Merge with right sibling!" << endl;
    Node *rNode = parent->children[rSPtr];

    if (rNode->size >= (maxDegree + 1) / 2)
    {
      current->key[current->size] = parent->key[k]; // lowest key of the right sibling's subtree

      // Update parent key for right sibling.
      parent->key[k] = rNode->key[0];

      // Tranfer right sibling keys to the left to remove that key.
      for (int i = 0; i < rNode->size - 1; i++)
      {
        rNode->key[i] = rNode->key[i + 1];
      }

      // Move first pointer of right sibling to current's last pointer.
      current->children[current->size + 1] = rNode->children[0];

      // Shift right sibling ptr to the left.
      int i = 0;
      while (i < rNode->size)
      {
        rNode->children[i] = rNode->children[i + 1];
        i++;
      }

      // Update sizes of nodes.
      rNode->size--;
      current->size++;
      // cout << "Transferred  " << current->key[0].key_value
      //<< " from right sibling of node!" << endl;
      return 0;
    }
  }
  // Not possible to transfer from sibling nodes, merge with sibling node.
  if (lSPtr >= 0)
  {
    // cout << "Merge with left sibling!" << endl;
    Node *lNode = parent->children[lSPtr];

    // Merge current into left sibling.
    // parent->key lSPtr] is the smallest key in the current subtree to be merged in.
    lNode->key[lNode->size] = parent->key[lSPtr];

    // Add each remaining key into left sibling.
    int j = 0;
    while (j < current->size)
    {
      lNode->key[i] = current->key[j];
      j++;
    }

    int i = lNode->size + 1;
    j = 0;
    while (j <= current->size)
    {
      lNode->children[i] = current->children[j];
      current->children[j] = nullptr;
      j++;
    }

    // Update sizes.
    // This is current + 1 due to the additional key that is added which was previously the smallest key of the current.
    lNode->size += current->size + 1;
    current->size = 0;

    // Passes the key of the current (Key to be removed).
    int numNodesMerged = removeInternal(parent->key[lSPtr], parent, current);
    return 1 + numNodesMerged;
  }
  // Try to merge right sibling.
  else if (rSPtr <= parent->size)
  {
    // cout << "Merged with right sibling!" << endl;

    // Merge right sibling into current.
    Node *rNode = parent->children[rSPtr];

    // parent->key[rSPtr - 1] is the smallest key in the right subtree to be merged in.
    current->key[current->size] = parent->key[rSPtr - 1];

    // Add each right sibling key into current
    int j = 0;
    i = current->size + 1;
    while (j < rNode->size)
    {
      current->key[i] = rNode->key[j];
      j++;
    }

    // Add each right sibling pointer into current
    j = 0;
    while (j <= rNode->size)
    {
      current->children[i] = rNode->children[j];
      rNode->children[j] = NULL;
      j++;
    }
    // Update sizes.
    // This is right node + 1 due to the additional key that is added which was previously the smallest key of the right node.
    current->size += rNode->size + 1;
    rNode->size = 0;

    // Passes the key of the right sibling (Key to be removed).
    int numNodesMerged = removeInternal(parent->key[rSPtr - 1], parent,
                                        rNode);
    return 1 + numNodesMerged;
  }
  return 0;
}

/**
 * @brief
 * If the first key of the leaf node is deleted, you have to update one of the index nodes whose key
 * that ultimately points to that leaf node.
 * However, if the smallest pointer of that index node leads to the leaf node, you have to continue
 * finding the appropriate parent.
 * @param child
 * @param key
 */
void BPTree::updateParent(Node *child, data_keys d)
{
  // cout << "Entered update fn" << endl;
  Node *parent;
  Node *current = root;
  if (child->isLeaf)
  {
    while (current->isLeaf == false)
    {
      // cout << 2;
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
  }
  else
  {
    parent = findParent(root, child);
  }

  // cout << "Parent found FUNCTION STOPS HERE WHY" << parent->size << endl;
  int pl = 0; // pointer position
  while (pl <= parent->size && parent->children[pl] != child)
  {
    // increment pointer
    pl++;
  }
  // If the first pointer is pointing to the child, find the previous parent.
  if (pl != 0)
  {
    parent->key[pl - 1] = d;
    // cout << "key value of parent:" << parent->key[pl - 1].key_value << endl;
    return;
  }
  else
  {
    if (parent == root)
    {
      updateParent(parent, d);
    }
    else
    {
      parent->key[pl - 1] = d;
      // cout << "key value of parent:" << parent->key[pl - 1].key_value << endl;
      return;
    }
  }
}