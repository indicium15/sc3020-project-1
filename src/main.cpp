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
// Custom definition for days offset calculation
using days = chrono::duration<int, ratio<60 * 60 * 24>>;

/**
 * @brief Function that converts date string found in data to an integer offset from 01/01/2000 for storage
 *
 * @param dateString Input string for conversion
 * @return int Date offset from 01/01/2000
 */
int dateToOffset(const string &dateString)
{
    // Set starting date 01/01/2000
    chrono::system_clock::time_point startingDate = chrono::system_clock::from_time_t(946684800); // UNIX timestamp for January 1, 2000

    // Parse input date string
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
//
string offsetToDate(int offsetInDays)
{
    // Set starting date 01/01/2000
    chrono::system_clock::time_point startingDate = chrono::system_clock::from_time_t(946684800); // UNIX timestamp for January 1, 2000

    // Calculate date from offset
    chrono::system_clock::time_point calculatedDate = startingDate + days(offsetInDays);

    // Convert date to a string
    time_t calculatedTime = chrono::system_clock::to_time_t(calculatedDate);
    tm tmDate = *localtime(&calculatedTime);
    ostringstream dateStream;
    dateStream << put_time(&tmDate, "%d/%m/%Y");
    return dateStream.str();
}

/**
 * @brief Function that converts team id to a offset from the lowest team ID found in the dataset for storage.
 *
 * @param teamID read from file
 * @return unsigned short int
 */
unsigned short int teamIDToOffset(int teamID)
{
    return (teamID - 1610612736);
}

/**
 * @brief Function that converts team id offset to integer value for displaying
 *
 * @param offset read from database
 * @return int
 */
int offsetToTeamID(unsigned short int offset)
{
    return (1610612736 + offset);
}

/**
 * @brief Function that converts int homeTeamWins to a boolean for database storage.
 *
 * @param homeTeamWins read from file
 * @return true if value == 1
 * @return false if value == 0
 */
bool winsToBool(int homeTeamWins)
{
    return (homeTeamWins == 1 ? true : false);
}

/**
 * @brief Function that converts boolean homeTeamWins from database to int for display
 *
 * @param wins fread from database
 * @return int
 */
int boolWinstoInt(bool wins)
{
    return (wins == 1 ? true : false);
}

int main()
{
    // Read the input file
    ifstream inputFile("../games.txt");
    if (!inputFile.is_open())
    {
        cerr << "Error: unable to open the file./n";
        return 1;
    }
    string line;
    int lineNumber = 0;
    while (getline(inputFile, line))
    {
        lineNumber++;
        // Skip the first line of the file
        if (lineNumber == 1)
        {
            continue;
        }
        // Remove code for final
        if (lineNumber == 5)
        {
            break;
        }
        istringstream iss(line);
        vector<string> fields;
        string field;
        // Split the line into an array separated by the \t delimiter
        while (getline(iss, field, '\t'))
        {
            fields.push_back(field);
        }
        // Assign values to each variable for the column before storing to database
        // TODO: Add constructor for record here 
        string gameDateStr = fields[0], teamID = fields[1];
        unsigned short int pts = stoi(fields[2]), ast = stoi(fields[6]), reb = stoi(fields[7]);
        int homeTeamWins = stoi(fields[8]);
        float fgPct = stof(fields[3]), ftPct = stof(fields[4]), fg3Pct = stof(fields[5]);
       
        if (!gameDateStr.empty() && !teamID.empty() && pts != 0 && fgPct != 0.0 && ftPct != 0.0 && fg3Pct != 0.0 && ast != 0 && reb != 0 && homeTeamWins != 0)
        {
            int gameDateOffset = dateToOffset(gameDateStr);
            unsigned short int teamIDOffset = teamIDToOffset(stoi(teamID));
            bool homeTeamWinsBool = winsToBool(homeTeamWins);
            cout << "-----------------------------" << endl;
            cout << "Line Number: " << lineNumber << endl;
            cout << "Game Date Written to Database: " << gameDateOffset << endl;
            cout << "Game Date Reading from Database: " << offsetToDate(gameDateOffset) << endl;
            cout << "Team ID Written to Database: " << teamIDOffset << endl;
            cout << "Team ID Reading from Database: " << offsetToTeamID(teamIDOffset) << endl;
            cout << "PTS: " << pts << endl;
            cout << "FG Pct: " << fgPct << endl;
            cout << "FT Pct: " << ftPct << endl;
            cout << "FG3 Pct: " << fg3Pct << endl;
            cout << "AST: " << ast << endl;
            cout << "REB: " << reb << endl;
            cout << "Home Team Wins Written to Database: " << homeTeamWinsBool << endl;
            cout << "Home Team Wins Reading from Database: " << boolWinstoInt(homeTeamWinsBool) << endl;
        }
        else
        {
            cerr << "Error parsing line number: " << lineNumber << endl;
        }
    }
    inputFile.close();
    return 0;
}
