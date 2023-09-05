#ifndef STORAGE_H
#define STORAGE_H
#include <string>

// NBA Data Record
// Size of one record written to database:
// game_date (int) = 4 bytes
// team_id_home (unsigned short int) = 2 bytes
// pts_home (unsigned short int) = 2 bytes
// ast_home (unsigned short int) = 2 bytes
// reb_home (unsigned short int) = 2 bytes
// fg_pct_home (float) = 4 bytes
// ft_pct_home (float) = 4 bytes
// fg3_pct_home (float) = 4 bytes
// home_team_wins (bool) = 1 byte
// Total: 25 bytes
struct Record
{
    int recordID;
    int gameDate;
    unsigned short int teamID;
    unsigned short int pts, ast, reb;
    float fgPct, ftPct, fg3Pct;
    bool homeTeamWins;
};

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
    // Function to read a block
    uchar *readBlock(int blockID);
    // Function to find available block while allocating records
    uchar *findAvailableBlock(int size);
};

#endif
