#include "record.h"
#include "record.cpp"
#include "Storage.h"
#include "Storage.cpp"
#include "b+tree.hpp"
#include "b+tree_insert.cpp"
#include "b+tree_search.cpp"
#include "b+tree_display.cpp"
#include "b+tree_delete.cpp"
#include <string>
#include <cstring>
#include <sstream>
#include <tuple>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    int blk = 0;       // blkPos Keep track of size of all blocks created
    int prevBlkID = 0; // prevBlockID Keep track of previous block ID
    int size = 0;      // prev Keep track of size of record inserted and help update offset
    int offset = 0;
    int count = 0; // logicCount Keep track of number of entry in the address map
    int tracker = 0;

    unsigned char *disk = nullptr;
    disk = new unsigned char[diskCap];
    unsigned char *blkPtr = nullptr;
    Record games;
    vector<Record> gamesVector;

    // blkSize = 200; // Change according to Experiment
    blkSize = 400;
    blkPtr = disk;

    // Map address
    vector<tuple<unsigned char, void *>> addMap;
    string line;

    // Import data
    ifstream filename("games.txt");
    filename.ignore(10000, '\n'); // Remove header

    // Read data line by line
    while (getline(filename, line))
    {

        stringstream linestream(line);
        string r;
        getline(linestream, r, '\t');
        // strcpy(games.gameDateStr, r.c_str());
        linestream >> games.gameDateStr >> games.teamID >> games.pts >> games.fgPct >> games.ftPct >> games.fg3Pct >> games.ast >> games.reb >> games.homeTeamWins;
        // Insert record to storage
        int currBlkID = insertRec(sizeof(games));

        gamesVector.push_back(games);

        ////Logics to keep track of pointers
        // New block is created. Set offset to 0 and update prevBlkID
        if (prevBlkID != currBlkID)
        {
            offset = 0;
            size = sizeof(games);
            ;
            prevBlkID = currBlkID;
            tracker = 0;
        }
        else
        {
            if (tracker == 0)
            {
                offset = 0;
                size = sizeof(games);
            }
            else
            { // Update offset
                offset += size;
            }
        }

        blk = (currBlkID * blkSize); // Get current size of all block
        blkPtr = (unsigned char *)disk + blk;
        tuple<void *, unsigned char> blockTable(&blkPtr, offset);

        // Add the offset to the physical address
        void *physicalAddress = blkPtr + offset;

        // Update Mapping Table
        tuple<unsigned char, void *> tableEntry(count, physicalAddress);
        addMap.push_back(tableEntry);
        count++;

        // Store Record to physical address
        memcpy(physicalAddress, &games, sizeof(games));

        tracker++;
    }

    filename.close();

    vector<tuple<unsigned char, void *>>::iterator iterator;

    // Experiment 1//
    cout << "=============================================" << endl;
    cout << "| Experiment 1" << endl;
    cout << "| -------------------" << endl;
    cout << "| Fixed block size: " + to_string(blkSize) + "B" << endl;
    cout << "| Number of available blocks: " + to_string(availableBlk()) << endl;
    cout << "| Size of database: " + to_string(databaseSize()) + "MB" << endl;
    cout << "| Number of blocks used:  " + to_string(numOfBlks()) << endl;
    cout << "=============================================" << endl;

    // Experiment 2//
    BPTree bplustree = BPTree(blkSize);
    for (iterator = addMap.begin(); iterator != addMap.end(); ++iterator)
    {
        void *blkAdd = get<1>(*iterator);
        float fgPct = (*(Record *)blkAdd).fgPct;
        data_keys record;
        record.key_value = fgPct;
        record.add.push_back(blkAdd);
        bplustree.insertData(record);
    }
    cout << "| Experiment 2" << endl;
    cout << "| -------------------" << endl;
    cout << "| Parameter n of B+ Tree: " + to_string(bplustree.getMaxDegree(blkSize)) << endl;
    cout << "| Number of nodes:  " + to_string(bplustree.getnumOfNodes()) << endl;
    cout << "| Height of B+ Tree  " + to_string(bplustree.treeHeight(bplustree.getRoot())) << endl;
    bplustree.displayTree(bplustree.getRoot(), false); // if false, print only the root and first node
    cout << "=============================================" << endl;

    // Experiment 3//
    cout << "| Experiment 3" << endl;
    cout << "| -------------------" << endl;
    bplustree.search(0.5, true, 0);
    cout << "=============================================" << endl;

    // Experiment 4//
    cout << "| Experiment 4" << endl;
    cout << "| -------------------" << endl;
    // bplustree.search(0.6, true, 0.4);
    cout << "=============================================" << endl;

    // Experiment 5//
    data_keys recordToDelete;
    recordToDelete.key_value = 0.35;
    cout << "| Experiment 5:" << endl;
    cout << "| -------------------" << endl;
    cout << "| Deleting records where fgPct = " << recordToDelete.key_value << endl;
    int numNodeBeforeRemoval = bplustree.getnumOfNodes();
    cout << "| Number of nodes before removal: " << numNodeBeforeRemoval << endl;
    int numNodeMerged = bplustree.removeData(recordToDelete);
    int numNodeAfterRemoval = bplustree.getnumOfNodes();
    int numNodeRemoved = numNodeBeforeRemoval - numNodeAfterRemoval;
    cout << "| Number of Nodes deleted: " << numNodeRemoved << endl;
    cout << "| Number of Nodes in updated B+ Tree: " << numNodeAfterRemoval << endl;
    cout << "| Height of updated B+ Tree: " << bplustree.treeHeight(bplustree.getRoot()) << endl;
    bplustree.displayTree(bplustree.getRoot(), false);
    cout << "=============================================" << endl;

    // clean up
    disk = NULL;
    blkPtr = nullptr;
    gamesVector.clear();
    addMap.clear();
    delete disk;
    delete blkPtr;
}
