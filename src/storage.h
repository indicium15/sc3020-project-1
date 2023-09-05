#ifndef STORAGE_H
#define STORAGE_H

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
    int gameDate;
    unsigned short int teamID;
    unsigned short int pts, ast, reb;
    float fgPct, ftPct, fg3Pct;
    bool homeTeamWins;
};

struct Block
{
    int blockID;
    Record records[10];
};

typedef unsigned int uint;

// Structure of a Database
// Fields are packed into records, records are packed into blocks
class Storage
{
private:
    // Disk capacity between 100-500MB
    uint diskCapacity;
    // Block size of 400 bytes
    uint blockSize = 400;

public:
    // Constructor
    Storage(uint diskCapacity, uint blockSize);
    // Destructor
    ~Storage();
    // Allocate memory to a block
    bool allocateBlock();
    // Allocate memory to a record
    bool allocateRecord();
};

#endif
