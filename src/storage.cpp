#include "storage.h"
#include "record.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>

using namespace std;
typedef unsigned int uint;
typedef unsigned char uchar;

Storage::Storage(uint diskCapacity, uint blockSize)
{
    this->diskCapacity = diskCapacity;
    this->blockSize = blockSize;
    this->currentBlock = 0;
    this->currentBlockSize = 0;
    this->availableBlocks = diskCapacity / blockSize;
    this->baseAddress = static_cast<uchar *>(malloc(diskCapacity));
    this->databaseCursor = baseAddress;
    this->blockRecords[0] = 0;
}

bool Storage::allocateRecord(Record record)
{
    // Error case: no blocks are available
    if (availableBlocks == 0)
    {
        return false;
    }
    // Try to find an available block and get the address of it
    uchar *blockAddress = findAvailableBlock(sizeof(record));
    if (!blockAddress)
    {
        cerr << "Failed to find an available block" << endl;
        return false;
    }
    // Copy the record data to the memory address
    memcpy(blockAddress, &record, sizeof(record));
    currentBlockSize += sizeof(record);
    // Update blockRecords value to keep track
    auto find = blockRecords.find(currentBlock);
    if (find != blockRecords.end())
    {
        find->second += 1;
    }
    // Move the cursor forward by the amount of memory allocated
    databaseCursor = (blockAddress + sizeof(record));
    return true;
}

uchar *Storage::findAvailableBlock(int recordSize)
{
    // Error case: no blocks are available
    if (currentBlockSize + recordSize > blockSize && availableBlocks == 0)
    {
        return nullptr;
    }
    // Case 1: The new record can fit in an existing block
    if ((currentBlockSize + recordSize) <= blockSize)
    {
        return databaseCursor;
    }
    // Case 2: A new block has to be allocated for the new record
    if (currentBlockSize + recordSize > blockSize && availableBlocks > 0)
    {
        // Unspanned implementation, go past the remaining fields
        databaseCursor += (blockSize - currentBlockSize);
        // Change internal variables to acknowledge the new block
        availableBlocks--;
        currentBlock++;
        currentBlockSize = 0;
        blockRecords[currentBlock] = 0;
        return databaseCursor;
    }
}

uchar *Storage::readBlock(int blockID)
{
    // Calculate starting memory address of block from base address of database and block size
    uchar *blockCursor = baseAddress + (blockID * blockSize);
    uchar *copy = new uchar[400];
    std::memcpy(copy, blockCursor, blockSize);
    return copy;
}

void Storage::printBlockRecords()
{
    for (const auto &pair : blockRecords)
    {
        std::cout << "Block ID: " << pair.first << ", Num Records: " << pair.second << std::endl;
    }
}

int Storage::getNumberOfRecords(int blockID){
    auto find = blockRecords.find(blockID);
    if (find != blockRecords.end())
    {
        return find->second;
    }
    return 0;
}

Storage::~Storage()
{
    free(baseAddress);
}