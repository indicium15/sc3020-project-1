#include "storage.h"
#include "record.h"
#include "types.h"
#include <iostream>
#include <map>
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

bool Storage::deleteBlock(int blockID)
{
    uchar *cursor = baseAddress;
    // Navigate to the first memory address of a block
    cursor += (blockID * blockSize);
    uchar zeroBytes[blockSize];              // Create an array of 400 null bytes
    memset(zeroBytes, 0, sizeof(zeroBytes)); // Initialize it to zeros
    if (memcpy(cursor, zeroBytes, sizeof(zeroBytes)))
    {
        this->recordsStored -= (currentBlockSize / sizeof(Record));
        this->availableBlocks++;
        return true; // Copy to the memory pointed by cursor
    }
    return false;
}

bool Storage::deleteRecord(int blockID, int offset)
{
    uchar *cursor = baseAddress;
    cursor += (blockID * blockSize) + offset;
    uchar zeroBytes[sizeof(Record)];         // Create an array of 40 null bytes
    memset(zeroBytes, 0, sizeof(zeroBytes)); // Initialize it to zeros
    if (memcpy(cursor, zeroBytes, sizeof(zeroBytes))){
        // Decrement the tracker of number of records stored
        this->recordsStored--;
        // If there is only one record on a block, increment the number of available blocks
        if(this->recordsInBlock(blockID) == 1){
            this->availableBlocks++;
        }
        // Update the blockRecords data structure 
        this->setRecordsInBlock(blockID, this->recordsInBlock(blockID)-1);
        return true; 
    }
    return false;
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
    uchar *blockAddress = findAvailableBlock(sizeof(Record));
    if (!blockAddress)
    {
        cerr << "Failed to find an available block" << endl;
        return false;
    }
    // Update record header
    record.setBlockAddress(this->getBlockAddress(this->currentBlock));
    int offset = currentBlockSize;
    record.setOffset(offset);
    record.print();
    // Copy the record data to the memory address
    memcpy(blockAddress, &record, sizeof(Record));
    // cout << "Current block size " << currentBlockSize << endl;
    currentBlockSize += sizeof(Record);
    // Update blockRecords value to keep track
    auto find = blockRecords.find(currentBlock);
    if (find != blockRecords.end())
    {
        find->second += 1;
    }
    recordsStored++;
    // Move the cursor forward by the amount of memory allocated
    databaseCursor = (blockAddress + sizeof(Record));
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
    return nullptr;
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
    uchar nullBytes[sizeof(Record)];
    memset(nullBytes, 0, sizeof(Record));
    uchar *blockCursor = baseAddress + (blockID * blockSize); // starting memory address of the particular block
    int numRecordsInBlock = recordsInBlock(blockID);
    if (numRecordsInBlock == 0)
    {
        return records;
    }
    for (int i = 0; i < numRecordsInBlock; i++)
    {
        int offset = i * sizeof(Record);
        if (memcmp(blockCursor + offset + sizeof(Record), nullBytes, sizeof(Record)) == 0)
        {
            continue;
        }
        else
        {
            float fgPct = *reinterpret_cast<float *>(blockCursor + offset + offsetof(Record, fgPct));
            float ftPct = *reinterpret_cast<float *>(blockCursor + offset + offsetof(Record, ftPct));
            float fg3Pct = *reinterpret_cast<float *>(blockCursor + offset + offsetof(Record, fg3Pct));
            int gameDate = *reinterpret_cast<int *>(blockCursor + offset + offsetof(Record, gameDate));
            uchar *blockAddress = *reinterpret_cast<uchar **>(blockCursor + offset + offsetof(Record, blockAddress));
            int recordOffset = *reinterpret_cast<int *>(blockCursor + offset + offsetof(Record, offset));
            unsigned short int recordID = *reinterpret_cast<unsigned short int *>(blockCursor + offset + offsetof(Record, recordID));
            uint8_t teamID = *reinterpret_cast<uint8_t *>(blockCursor + offset + offsetof(Record, teamID));
            uint8_t pts = *reinterpret_cast<uint8_t *>(blockCursor + offset + offsetof(Record, pts));
            uint8_t ast = *reinterpret_cast<uint8_t *>(blockCursor + offset + offsetof(Record, ast));
            uint8_t reb = *reinterpret_cast<uint8_t *>(blockCursor + offset + offsetof(Record, reb));
            bool homeTeamWins = *reinterpret_cast<bool *>(blockCursor + offset + offsetof(Record, homeTeamWins));
            Record record = Record(fgPct, ftPct, fg3Pct, gameDate, blockAddress, recordOffset, recordID, teamID, pts, ast, reb, homeTeamWins);
            records.push_back(record);
        }
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

void Storage::setRecordsInBlock(int blockID, int value)
{
    auto find = blockRecords.find(blockID);
    if (find != blockRecords.end())
    {
        find->second = value;
    }
    else
    {
        blockRecords[blockID] = value;
    }
}

vector<Record> Storage::readRecordsfromAddresses(vector<Address> addresses)
{
    // Initialize a map to store the indexes we want to read for each blockID
    int blockAccessCount = 0;
    map<int, vector<int>> indexMap;
    for (int i = 0; i < addresses.size(); i++)
    {
        int blockID = getBlockID(addresses[i].blockAddress);
        int index = addresses[i].offset / sizeof(Record);
        if (indexMap.find(blockID) != indexMap.end())
        {
            // Push index to existing vector if the key already exists
            indexMap[blockID].push_back(index);
        }
        else
        {
            // Create and push a vector with index value if the key does not exist
            indexMap[blockID] = vector<int>{index};
            // indexMap.insert(blockID, vector<int>{index});
        }
    }
    // Value to return, vector of Records
    vector<Record> results;
    // Iterate through all keys of hashmap (block ID we want to read)
    for (const auto &pair : indexMap)
    {
        // Perform a unit reading of each block
        vector<Record> recordsFromBlock = readRecordsFromBlock(pair.first);
        blockAccessCount++;
        // Get the indexes we want to read from each block
        vector<int> indexes = pair.second;
        for (int index : indexes)
        {
            // Push each index into the results
            results.push_back(recordsFromBlock.at(index));
        }
    }
    cout << "Number of Data Blocks Accessed for Reading Records from Database: " << blockAccessCount << endl;
    return results;
}


vector<Record> Storage::readRecordsfromNestedAddresses(vector<vector<Address>> addresses)
{
    int blockAccessCount = 0;
    map<int, vector<int>> indexMap;
    for (int i = 0; i < addresses.size(); i++)
    {
        vector<Address> cursor = addresses[i];
        for (int j = 0; j < cursor.size(); j++)
        {
            int blockID = getBlockID(cursor[j].blockAddress);
            int index = cursor[j].offset / sizeof(Record);
            if (indexMap.find(blockID) != indexMap.end())
            {
                // Push index to existing vector if the key already exists
                indexMap[blockID].push_back(index);
            }
            else
            {
                // Create and push a vector with index value if the key does not exist
                indexMap[blockID] = vector<int>{index};
                // indexMap.insert(blockID, vector<int>{index});
            }
        }
    }
    // Value to return, vector of Records
    vector<Record> results;
    // Iterate through all keys of hashmap (block ID we want to read)
    for (const auto &pair : indexMap)
    {
        // Perform a unit reading of each block
        vector<Record> recordsFromBlock = readRecordsFromBlock(pair.first);
        blockAccessCount++;
        // Get the indexes we want to read from each block
        vector<int> indexes = pair.second;
        for (int index : indexes)
        {
            // Push each index into the results
            results.push_back(recordsFromBlock.at(index));
        }
    }
    cout << "Number of Data Blocks Accessed: " << blockAccessCount << endl;
    return results;
}

int Storage::removeRecordsfromNestedAddresses(vector<vector<Address>> addresses)
{
    int blockAccessCount = 0;
    map<int, vector<int>> indexMap;
    for (int i = 0; i < addresses.size(); i++)
    {
        vector<Address> cursor = addresses[i];
        for (int j = 0; j < cursor.size(); j++)
        {
            int blockID = getBlockID(cursor[j].blockAddress);
            int index = cursor[j].offset / sizeof(Record);
            if (indexMap.find(blockID) != indexMap.end())
            {
                // Push index to existing vector if the key already exists
                indexMap[blockID].push_back(index);
            }
            else
            {
                // Create and push a vector with index value if the key does not exist
                indexMap[blockID] = vector<int>{index};
                // indexMap.insert(blockID, vector<int>{index});
            }
        }
    }
    cout << "Printing blocks and indexes to be deleted" << endl;
    for (const auto &pair : indexMap)
    {
        cout << "Block Number: " << pair.first << ", Indexes : ";
        vector<int> indexes = pair.second;
        // Check if the length is the full block, then just delete the whole block
        if (indexes.size() == (blockSize / sizeof(Record)))
        {
            cout << "Trying to delete block" << endl;
            if (deleteBlock(pair.first) == 1)
            {
                cout << "Block ID " << pair.first << "sucessfully deleted" << endl;
            }
        }
        for (int index : indexes)
        {
            // Push each index into the results
            if(deleteRecord(pair.first,index*sizeof(Record)) == 1){
                cout << "Record deleted" << endl;
            };
        }
        cout << endl;
    }
    return 1;
}

uchar *Storage::getBlockAddress(int blockID)
{
    return (baseAddress + (blockID * blockSize));
}

int Storage::getBlockID(void *blockAddress)
{
    return (static_cast<uchar *>(blockAddress) - baseAddress) / blockSize;
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