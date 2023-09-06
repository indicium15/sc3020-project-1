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

#ifndef RECORD_H
#define RECORD_H

#include <string>

using namespace std;

class Record
{
public:
    int gameDate;
    unsigned short int teamID;
    unsigned short int pts, ast, reb;
    float fgPct, ftPct, fg3Pct;
    bool homeTeamWins;
    // Constructor functions for the record
    Record(int date, unsigned short int team, unsigned short int points, unsigned short int assists,
           unsigned short int rebounds, float fgPercentage, float ftPercentage, float fg3Percentage,
           bool homeWins);
    Record(string gameDateStr, int teamID, unsigned short int pts, unsigned short int reb, unsigned short int ast, float fgPct, float ftPct, float fg3Pct, int homeTeamWins);
    // Functions to manage conversion of data between console output and database storage
    int dateToOffset(const string &dateString);
    string offsetToDate(int offsetInDays);
    unsigned short int teamIDToOffset(int fileTeamID);
    int offsetToTeamID(unsigned short int offset);
    bool winsToBool(int homeTeamWins);
    int boolWinsToInt(bool wins);
    void print() const;
};

#endif