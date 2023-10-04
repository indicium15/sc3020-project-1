#ifndef STORAGE_H
#define STORAGE_H
#include <string>
#include <iostream>
#include <unordered_map>
#include "record.h"
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
    // TODO: Write a function to parse file data and return record object
    // Allocate memory to a block
    bool allocateBlock();
    // Allocate memory to a record
    bool allocateRecord(Record record);
    // Function to find available block while allocating records
    uchar *findAvailableBlock(int size);
    // Function to read a block
    uchar *readBlock(int blockID);
    // Function to print the content of blockRecords
    void printBlockRecords();
    // Function to print the content for a block
    vector<Record> readRecordsFromBlock(int blockID);
    vector<Record> readAllRecords();
    // Return the number of records for a blockID by looking up the structure
    int recordsInBlock(int blockID);
    //Return the header memory address for a block ID
    uchar* getBlockAddress(int blockID);
    // Getter functions for private attributes
    int getRecordsStored();
    int getBlocksUsed();
    int getAvailableBlocks();
    int getBlockSize();
};

#endif
