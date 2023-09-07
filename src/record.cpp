#include "record.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <string>
#include <typeinfo>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;
// Used for calculating the date offset in the function dateToOffset()
using days = chrono::duration<int, ratio<60 * 60 * 24>>;

Record::Record(int date, unsigned short int team, unsigned short int points, unsigned short int rebounds,
               unsigned short int assists, float fgPercentage, float ftPercentage, float fg3Percentage,
               bool homeWins)
    : gameDate(date), teamID(team), pts(points), ast(assists), reb(rebounds),
      fgPct(fgPercentage), ftPct(ftPercentage), fg3Pct(fg3Percentage), homeTeamWins(homeWins){};

/**
 * @brief Constructor from record that converts the data read from a file format to a Record object
 * 
 * @param gameDateStr 
 * @param teamID 
 * @param pts 
 * @param reb 
 * @param ast 
 * @param fgPct 
 * @param ftPct 
 * @param fg3Pct 
 * @param homeTeamWins 
 */
Record::Record(string gameDateStr, int teamID, unsigned short int pts, unsigned short int reb, unsigned short int ast, float fgPct, float ftPct, float fg3Pct, int homeTeamWins)
{
    int gameDateOffset = dateToOffset(gameDateStr);
    unsigned short int teamIDOffset = teamIDToOffset(teamID);
    bool homeTeamWinsBoolean = winsToBool(homeTeamWins);
    this->gameDate = gameDateOffset;
    this->teamID = teamIDOffset;
    this->pts = pts;
    this->reb = reb;
    this->ast = ast;
    this->fgPct = fgPct;
    this->ftPct = ftPct;
    this->fg3Pct = fg3Pct;
    this->homeTeamWins = homeTeamWinsBoolean;
}

/**
 * @brief Function that converts date string found in data to an integer offset from 01/01/2000 for storage
 *
 * @param dateString Input string for conversion
 * @return int Date offset from 01/01/2000
 */
int Record::dateToOffset(const string &dateString)
{
    // Set starting date 01/01/2000
    chrono::system_clock::time_point startingDate = chrono::system_clock::from_time_t(946684800); // UNIX timestamp for January 1, 2000

    // Parse input date string with the format given in the data file
    tm tmDate = {};
    istringstream dateStream(dateString);
    dateStream >> get_time(&tmDate, "%d/%m/%Y");
    chrono::system_clock::time_point inputDate = chrono::system_clock::from_time_t(mktime(&tmDate));

    // Calculate the date offset from starting date
    days dateOffset = chrono::duration_cast<days>(inputDate - startingDate);
    int daysSinceEpoch = dateOffset.count();
    return daysSinceEpoch;
}

/**
 * @brief Function to convert an offset to a date string for displaying information
 *
 * @param offsetInDays Integer value stored in database
 * @return string Date string used for display
 */
string Record::offsetToDate(int offsetInDays)
{
    // Set starting date 01/01/2000
    chrono::system_clock::time_point startingDate = chrono::system_clock::from_time_t(946684800); // UNIX timestamp for January 1, 2000

    // Calculate date from offset in database
    chrono::system_clock::time_point calculatedDate = startingDate + days(offsetInDays);

    // Convert date to a string with same format as input file
    time_t calculatedTime = chrono::system_clock::to_time_t(calculatedDate);
    tm tmDate = *localtime(&calculatedTime);
    ostringstream dateStream;
    dateStream << put_time(&tmDate, "%d/%m/%Y");
    return dateStream.str();
}

/**
 * @brief Function that converts team id to a offset from the lowest team ID found in the dataset for storage
 *
 * @param teamID read from file
 * @return unsigned short int
 */
unsigned short int Record::teamIDToOffset(int fileTeamID)
{
    int difference = fileTeamID - 1610612736;
    unsigned short int offset = static_cast<unsigned short int>(difference);
    return (offset);
}

/**
 * @brief Function that converts team id offset to integer value for displaying
 *
 * @param offset read from database
 * @return int
 */
int Record::offsetToTeamID(unsigned short int offset)
{
    int difference = static_cast<int>(offset);
    return (1610612736 + difference);
}

/**
 * @brief Function that converts int homeTeamWins to a boolean for database storage
 *
 * @param homeTeamWins read from file
 * @return true if value == 1
 * @return false if value == 0
 */
bool Record::winsToBool(int homeTeamWins)
{
    return (homeTeamWins == 1);
}

/**
 * @brief Function that converts boolean homeTeamWins from database to int for display
 *
 * @param wins fread from database
 * @return int
 */
int Record::boolWinsToInt(bool wins)
{
    return (wins ? 1 : 0);
}

void Record::print() const
{
    std::cout << "GAME_DATE_EST: " << gameDate << " TEAM_ID_home: " << teamID << " PTS_home: " << pts << " FG_PCT_home: " << fgPct << " FT_PCT_home: " << ftPct << " FG3_PCT_home: " << fg3Pct << " AST_home: " << ast << " REB_home: " << reb << " HOME_TEAM_WINS: " << homeTeamWins << endl;

}