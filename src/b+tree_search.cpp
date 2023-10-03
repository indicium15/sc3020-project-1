#include <iostream>
#include <typeinfo>
#include "b+tree.hpp"

using namespace std;
// toPrint only prints internal node traversed
Node *BPTree::search(float keyTofind, bool toPrint, float range)
{
    bool rootPtr = true;
    bool printFormat = true;
    int numOfIndexNodes = 0;
    int numOfDup = 0;
    float totalRating = 0;
    int numOfDataNodes = 0;
    int nodePrintingLimit = 5; // only need to print first 5 index nodes or data blocks
    vector<void *> v;
    Node *current = root;
    Node *start;
    if (!current)
    {
        cout << "Empty root" << endl;
        return NULL;
    }
    // traversing nodes in the BPTree
    while (current->isLeaf == false)
    {
        // traverse the keys in the node
        for (int i = 0; i < current->size; i++)
        {
            if (keyTofind < current->key[i].key_value)
            {
                // check if need to print the internal node contents
                if (toPrint)
                {
                    // if current is pointing to root, skip it
                    if (rootPtr)
                    {
                        rootPtr = !rootPtr;
                        current = current->getChildren(i);
                        break;
                    }
                    if (nodePrintingLimit > 0)
                    {
                        // print the whole node content out
                        for (int j = 0; j < current->size; j++)
                        {
                            if (printFormat)
                            {
                                cout << "| Contents in this index node: ";
                                printFormat = false;
                            }
                            cout << current->getDataKey(j).key_value << " ";
                        }
                        cout << endl;
                        nodePrintingLimit--;
                    }
                }
                current = current->getChildren(i);
                if (rootPtr)
                {
                    rootPtr = false;
                }
                else
                {
                    numOfIndexNodes++;
                }
                break;
            }
            // key to search is > last key index
            else if (i == current->size - 1)
            {
                // check if need to print the internal node contents
                if (toPrint)
                {
                    // if current is pointing to root, skip it
                    if (rootPtr)
                    {
                        rootPtr = !rootPtr;
                        current = current->getChildren(i + 1);
                        break;
                    }
                    if (nodePrintingLimit > 0)
                    {
                        // print the whole node content out
                        for (int j = 0; j < current->size; j++)
                        {
                            if (printFormat)
                            {
                                cout << "| Contents in this index node: ";
                                printFormat = false;
                            }
                            cout << current->getDataKey(j).key_value << " ";
                        }
                        cout << endl;
                        nodePrintingLimit--;
                    }
                }
                current = current->getChildren(i + 1);
                if (rootPtr)
                {
                    rootPtr = false;
                }
                else
                {
                    numOfIndexNodes++;
                }
                break;
            }
        }
        printFormat = true;
    }
    // reset variables
    printFormat = true;
    nodePrintingLimit = 5;
    int count = 5;

    // process to find node with key
    for (int i = 0; i < current->size; i++)
    {
        // not finding range of values
        if (range == 0)
        {
            if (current->getDataKey(i).key_value == keyTofind)
            {
                start = current;
                if (toPrint)
                {

                    while (current->getDataKey(i).key_value == keyTofind)
                    {
                        for (int j = 0; j < current->getDataKey(i).add.size(); j++)
                        {
                            if (count == 5)
                            {
                                cout << "| Contents of data blocks: " << endl;
                                cout << "| GAME_DATE_EST	TEAM_ID_home	PTS_home	FG_PCT_home	FT_PCT_home	FG3_PCT_home	AST_home	REB_home	HOME_TEAM_WINS" << endl;
                            }
                            if (count > 0)
                            {
                                cout << "| " << (*(Record *)current->getDataKey(i).add[j]).gameDateStr << "      "
                                     << (*(Record *)current->getDataKey(i).add[j]).teamID << "      "
                                     << (*(Record *)current->getDataKey(i).add[j]).pts << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).fgPct << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).ftPct << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).fg3Pct << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).ast << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).reb << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).homeTeamWins << "     "
                                     << endl;
                                count--;
                            }
                            totalRating += (*(Record *)current->getDataKey(i).add[j]).fg3Pct;
                            v.push_back((Record *)current->getDataKey(i).add[j]);
                            numOfDup++;
                        }
                        numOfDataNodes++;
                        i++;
                        // move to next node if
                        if (i > current->size)
                        {
                            current = current->getChildren(-1);
                            i = 0;
                        }
                    }
                    cout << "| Total number of index nodes accessed: " << numOfIndexNodes << endl;
                    cout << "| Total number of data nodes accessed: " << numOfDataNodes << endl;
                    cout << "| Number of data blocks accessed: " << numOfDup << endl;
                    cout << "| Total average ratings: " << to_string(totalRating / numOfDup) << endl;
                }
                return start;
            }
        }
        // finding a range of values
        else
        {
            // if last element is still smaller than target-- move to next node
            if (i == current->size - 1 && current->getDataKey(i).key_value < keyTofind)
            {
                current = current->children[current->size];
                i = 0;
            }
            if (current->getDataKey(i).key_value >= keyTofind)
            {
                start = current;
                if (toPrint)
                {
                    while (keyTofind <= current->getDataKey(i).key_value && current->getDataKey(i).key_value <= keyTofind + range)
                    {
                        for (int j = 0; j < current->getDataKey(i).add.size(); j++)
                        {
                            if (count == 5)
                            {
                                cout << "| Contents of data blocks: " << endl;
                                cout << "|GAME_DATE_EST	TEAM_ID_home	PTS_home	FG_PCT_home	FT_PCT_home	FG3_PCT_home	AST_home	REB_home	HOME_TEAM_WINS" << endl;
                            }
                            if (count > 0)
                            {
                                cout << "| " << (*(Record *)current->getDataKey(i).add[j]).gameDateStr << "      "
                                     << (*(Record *)current->getDataKey(i).add[j]).teamID << "      "
                                     << (*(Record *)current->getDataKey(i).add[j]).pts << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).fgPct << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).ftPct << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).fg3Pct << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).ast << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).reb << "     "
                                     << (*(Record *)current->getDataKey(i).add[j]).homeTeamWins << "     "
                                     << endl;
                            }
                            totalRating += (*(Record *)current->getDataKey(i).add[j]).fg3Pct;
                            numOfDup++;
                        }
                        numOfDataNodes++;

                        if (i == current->size - 1)
                        {
                            if (current->children[current->size] != nullptr)
                            {
                                printFormat = true;
                                current = current->children[current->size];
                                i = 0;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            i++;
                        }
                    }
                    cout << "| Total number of index nodes accessed: " << numOfIndexNodes << endl;
                    cout << "| Total number of data nodes accessed: " << numOfDataNodes << endl;
                    cout << "| Number of data blocks accessed: " << numOfDup << endl;
                    cout << "| Total average ratings: " << to_string(totalRating / numOfDup) << endl;
                }
                return start;
            }
        }
    }
    if (toPrint)
    {
        cout << "Key not found" << endl;
    }
    return nullptr;
}
