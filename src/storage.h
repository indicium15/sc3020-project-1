#ifndef STORAGE_H
#define STORAGE_H
#include <string>
#include <iostream>
#include <unordered_map>
#include "record.h"
#include "bplustree.h"
typedef unsigned int uint;
typedef unsigned char uchar;

// Structure of a Database
// Fields are packed into records, records are packed into blocks
class Storage
{
private:
    // Disk capacity between 100-500MB
    uint diskCapacity;
    // Block size of 400 bytes
    uint blockSize = 400;
    // Current block number that data is being written to
    int currentBlock;
    int currentBlockSize;
    // Number of blocks that are available in the database
    int availableBlocks;
    uchar *baseAddress;
    uchar *databaseCursor;
    // Dictionary to store the number of records in each block
    unordered_map<int,int> blockRecords;
    // Keep track of number of records stored for Experiment 1
    int recordsStored;

public:
    // Constructor
    Storage(uint diskCapacity, uint blockSize);
    // Destructor
    ~Storage();
    // Delete contents of a block
    bool deleteBlock(int blockID);
    // Allocate memory to a record
    bool allocateRecord(Record record);
    // Delete a record from the database
    bool deleteRecord(int blockID, int offset);
    // Function to find available block while allocating records
    uchar *findAvailableBlock(int size);
    // Function to read a block
    uchar *readBlock(int blockID);
    // Function to print the content of blockRecords
    void printBlockRecords();
    // Function to print the content for a block
    vector<Record> readRecordsFromBlock(int blockID);
    vector<Record> readAllRecords();
    vector<Record> readRecordsfromAddresses(vector<Address> address);
    vector<Record> readRecordsfromNestedAddresses(vector<vector<Address>> address);
    int removeRecordsfromNestedAddresses(vector<vector<Address>> address);
    // Return the number of records for a blockID by looking up the structure
    int recordsInBlock(int blockID);
    //Return the header memory address for a block ID
    uchar* getBlockAddress(int blockID);
    int getBlockID(void* blockAddress);
    // Getter functions for private attributes
    int getRecordsStored();
    int getBlocksUsed();
    int getAvailableBlocks();
    int getBlockSize();
    // Setter function for blockRecords
    void setRecordsInBlock(int blockID, int value);
};

#endif
