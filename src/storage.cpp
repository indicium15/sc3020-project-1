#include "storage.h"
#include "record.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <cstdint>

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
    this->recordsStored = 0;
}

/**
 * @brief Function that is used to store a record in the database object.
 *
 * @param record Record to allocate
 * @return true if the allocation is succesfull
 * @return false if there is an error in allocation
 */
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
    // Update record header
    record.setBlockAddress(this->getBlockAddress(this->currentBlock));
    record.setOffset(currentBlockSize);
    record.print(); 
    // Copy the record data to the memory address
    memcpy(blockAddress, &record, sizeof(Record));
    // cout << "Current block size " << currentBlockSize << endl;
    currentBlockSize += sizeof(record);
    // Update blockRecords value to keep track
    auto find = blockRecords.find(currentBlock);
    if (find != blockRecords.end())
    {
        find->second += 1;
    }
    recordsStored++;
    // Move the cursor forward by the amount of memory allocated
    databaseCursor = (blockAddress + sizeof(record));
    return true;
}

/**
 * @brief Helper function that is used to find the memory address of the block with capacity for a record, or create a new block.
 *
 * @param recordSize size in bytes of the record to be allocated
 * @return uchar* memory pointer to the available space in the database
 */
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

/**
 * @brief Function that copies a block to "RAM" and returns the memory address of the copied area
 *
 * @param blockID index of the block to copy
 * @return uchar* pointer to the copy of the block data
 */
uchar *Storage::readBlock(int blockID)
{
    // Calculate starting memory address of block from base address of database and block size
    uchar *blockCursor = baseAddress + (blockID * blockSize);
    uchar *copy = new uchar[400];
    memcpy(copy, blockCursor, blockSize);
    return copy;
}

/**
 * @brief Function that prints the contents of the blockRecords attribute in the database
 */
void Storage::printBlockRecords()
{
    for (const auto &pair : blockRecords)
    {
        cout << "Block ID: " << pair.first << ", Num Records: " << pair.second << endl;
    }
}

/**
 * @brief Function that reads all the records in a database object
 *
 * @return vector<Record> array of all records read from memory
 */
vector<Record> Storage::readAllRecords()
{
    vector<Record> records;
    for (int blockID = 0; blockID <= currentBlock; ++blockID)
    {
        vector<Record> blockRecords = readRecordsFromBlock(blockID);
        records.insert(records.end(), blockRecords.begin(), blockRecords.end());        
    }
    return records;
}

/**
 * @brief Function that reads all records from a given blockID
 *
 * @param blockID block ID to read records from
 * @return vector<Record> of all records inside a block
 */
vector<Record> Storage::readRecordsFromBlock(int blockID)
{
    vector<Record> records;
    uchar *blockCursor = baseAddress + (blockID * blockSize); // starting memory address of the particular block
    int numRecordsInBlock = recordsInBlock(blockID);
    if (numRecordsInBlock == 0)
    {
        return records;
    }
    for (int i = 0; i < numRecordsInBlock; ++i)
    {
        int offset = i * sizeof(Record);
        //TODO: Fix offset not being read
        Record record(
                *reinterpret_cast<float *>(blockCursor + offset),     // fgPct
                *reinterpret_cast<float *>(blockCursor + offset + 4),     // ftPct
                *reinterpret_cast<float *>(blockCursor + offset + 8), // fg3Pct
                *reinterpret_cast<int *>(blockCursor + offset + 12), // gameDate
                *reinterpret_cast<uchar **>(blockCursor + offset + 16), // blockAddress
                *reinterpret_cast<int *>(blockCursor + offset + 24), // offset
                *reinterpret_cast<unsigned short int *>(blockCursor + offset + 28), // recordID
                *reinterpret_cast<uint8_t *>(blockCursor + offset + 30), // teamID
                *reinterpret_cast<uint8_t *>(blockCursor + offset + 31),   // pts
                *reinterpret_cast<uint8_t *>(blockCursor + offset + 32),   // ast
                *reinterpret_cast<uint8_t *>(blockCursor + offset + 33),   // reb
                *reinterpret_cast<bool *>(blockCursor + offset + 34)    // homeTeamWins
        );
        records.push_back(record);
    }
    return records;
}

/**
 * @brief Returns the number of records stored in a block by looking up the blockRecords map
 *
 * @param blockID to find number of records for
 * @return int number of records in block
 */
int Storage::recordsInBlock(int blockID)
{
    auto find = blockRecords.find(blockID);
    if (find != blockRecords.end())
    {
        return find->second;
    }
    return 0;
}

uchar* Storage::getBlockAddress(int blockID){
    return(baseAddress + (blockID * blockSize));
}

// Getter functions for private class attributes

int Storage::getRecordsStored()
{
    return recordsStored;
}

int Storage::getBlocksUsed()
{
    return (currentBlock + 1);
}

int Storage::getAvailableBlocks()
{
    return availableBlocks;
}

int Storage::getBlockSize()
{
    return blockSize;
}

Storage::~Storage()
{
    free(baseAddress);
}