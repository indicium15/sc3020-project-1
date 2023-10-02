// NBA Data Record
// Size of one record written to database:
// recordID (uint8_t) = 1 byte
// game_date (int) = 4 bytes
// team_id_home (uint8_t) = 1 byte
// pts_home (uint8_t) = 1 byte
// ast_home (uint8_t) = 1 byte
// reb_home (uint8_t) = 1 byte
// fg_pct_home (float) = 4 bytes
// ft_pct_home (float) = 4 bytes
// fg3_pct_home (float) = 4 bytes
// home_team_wins (bool) = 1 byte
// Total: 22 bytes

#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <cstdint>

using namespace std;

class Record
{
public:
    float fgPct, ftPct, fg3Pct;
    int gameDate;
    unsigned short int recordID;
    uint8_t teamID, pts, ast, reb;
    bool homeTeamWins;

    // Constructor functions for the record
    Record(unsigned short int recordID, int date, uint8_t team, uint8_t points, uint8_t rebounds, uint8_t assists,
           float fgPercentage, float ftPercentage, float fg3Percentage,
           bool homeWins);
    Record(unsigned short int recordID, string gameDateStr, int teamID, uint8_t pts, uint8_t reb, uint8_t ast, float fgPct, float ftPct, float fg3Pct, int homeTeamWins);
    
    // Functions to manage conversion of data between console output and database storage
    int dateToOffset(const string &dateString);
    string offsetToDate(int offsetInDays);
    uint8_t teamIDToOffset(int fileTeamID);
    int offsetToTeamID(uint8_t offset);
    bool winsToBool(int homeTeamWins);
    int boolWinsToInt(bool wins);
    void print() const;
};

#endif