#ifndef STORAGE_H
#define STORAGE_H

//NBA Data Record
struct Record{

};

struct Block{

};

typedef unsigned int uint;

//Structure of a Database
//Fields are packed into records, records are packed into blocks
class Storage{
    private:
        //Disk capacity between 100-500MB
        uint diskCapacity;
        //Block size of 400 bytes
        uint blockSize = 400;
    public:
        //Constructor
        Storage(uint diskCapacity, uint blockSize);
        //Destructor
        ~Storage();
        //Allocate memory to a block
        bool allocateBlock();
        //Allocate memory to a record
        bool allocateRecord();
}
#endif
