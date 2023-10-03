#include "Storage.h"
#include <iostream>
using namespace std;

// Global variables.
uint diskCap = 500000000; // Allocated disk size
uint blkSize = 500;
// uint blkSize = 200;                 // Change according to Experiment
uint availBlks = diskCap / blkSize; // Total available block remaining that we can use
uint currBlkID = 0;                 // Keeps track of the number of blocks used
uint currentBlkCap = 0;             // Keeps track of whether current block has enough space left
float totalRecSize = 0.0;           // Sum of sizes of records that are packed into blocks.

// Storage Constructor
Storage::Storage(uint diskCap, uint blkSize)
{
    this->diskCap = diskCap;
    this->blkSize = blkSize;
    this->availBlks = diskCap / blkSize;
    this->currBlkID = 0;
    this->currentBlkCap = 0;
    this->totalRecSize = 0;
}

uint insertRec(uint recSize)
{

    // Situation 1: Insufficient space for record in the current block, but there are free block(s) available to pack the record into.
    if ((currentBlkCap + recSize) > blkSize)
    {
        availBlks--;
        currBlkID++;
        currentBlkCap = 0;
        currentBlkCap += recSize;
    }

    // Situation 2: Insufficient space for record in the current block and there are no free blocks to pack the record into.
    else if (((currentBlkCap + recSize) > blkSize) && (availBlks == currBlkID))
    {
        cout << "Cannot store record as there is not sufficient space on the disk." << endl;
    }

    // Situation 3: Sufficient space to pack the record into the current block.
    else
    {
        currentBlkCap += recSize;
    }
    totalRecSize += recSize;
    return currBlkID;
}

uint databaseSize()
{
    return totalRecSize / 1000000.0;
}

uint availableBlk()
{
    return availBlks;
}

uint numOfBlks()
{
    return currBlkID + 1;
}

/*
//Not sure if we need this. Don't uncomment this, program cannot run with this code.
void deleteRec(uint recAddress, vector<tuple <uint8_t, tuple <void *, uint8_t>>> addMap, uint offset, uint size) {
    for (int i = 0; i < addMap.size(); i++) {
        if (recAddress == get<0>(addMap[i])) {
            get<0>(get<1>(addMap[i])) = nullptr;
            currentBlkCap += size;
            totalRecSize -= size;
        }
    }
}
*/
