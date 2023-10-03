//
// Created by Chloe To on 26/9/22.
//

#ifndef CE4031_PROJECT_1_STORAGE_H
#define CE4031_PROJECT_1_STORAGE_H

#include <iostream>
#include <vector>
#include <cstring>
using namespace std;

typedef unsigned int uint;

struct Record
{
    string gameDateStr;
    uint teamID, pts, ast, reb;
    float fgPct, ftPct, fg3Pct;
    int homeTeamWins;
};

class Storage
{



private:
    uint diskCap;
    uint blkSize;
    uint availBlks;
    uint currBlkID;
    uint blkNum;
    uint currentBlkCap;
    float totalRecSize;

public:
    Storage(uint diskCap, uint blkSize);
    uint insertRec(uint recSize);
    void deleteRec(uint recAddress, vector<tuple<uint, tuple<void *, uint>>> mappingTable, uint offset, uint size);
    uint numOfBlks();
    uint databaseSize();
};

#endif // CE4031_PROJECT_1_STORAGE_H
