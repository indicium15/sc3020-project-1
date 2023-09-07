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
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include "storage.h"
#include "record.h"

using namespace std;
// Custom definition for days offset calculation
using days = chrono::duration<int, ratio<60 * 60 * 24>>;
typedef unsigned int uint;

/**
 * @brief Sorting function used for putting records in chronological order. If the game dates are same, use teamID to order the records
 *
 * @param a
 * @param b
 * @return true
 * @return false
 */
bool compareRecords(const Record &a, const Record &b)
{
    // Comapre dates of games
    if (a.gameDate < b.gameDate)
    {
        return true;
    }
    // If dates are the same, compare by teamID
    else if (a.gameDate == b.gameDate)
    {
        return a.teamID < b.teamID;
    }
    return false;
}

int main()
{
    vector<Record> records;
    Storage storage(100000, 400);
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
        // Skip the first line of the file (column headers)
        if (lineNumber == 1)
        {
            continue;
        }
        // Comment out for final demonstration
        if (lineNumber == 18)
        {
            break;
        }
        istringstream iss(line);
        vector<string> fields;
        string field;
        // Split the line into an array of strings separated by the \t delimiter
        while (getline(iss, field, '\t'))
        {
            fields.push_back(field);
        }
        // Assign values to each variable for the column before creating a Record object
        string gameDateStr = fields[0];
        int teamID = stoi(fields[1]);
        unsigned short int pts = stoi(fields[2]), ast = stoi(fields[6]), reb = stoi(fields[7]);
        int homeTeamWins = stoi(fields[8]);
        float fgPct = stof(fields[3]), ftPct = stof(fields[4]), fg3Pct = stof(fields[5]);
        cout << "-----------------------------" << endl;
        cout << "Read from File: " << endl;
        cout << "Line Number: " << lineNumber << endl;
        cout << "Game Date: " << gameDateStr << endl;
        cout << "Team ID: " << teamID << endl;
        cout << "PTS: " << pts << endl;
        cout << "FG Pct: " << fgPct << endl;
        cout << "FT Pct: " << ftPct << endl;
        cout << "FG3 Pct: " << fg3Pct << endl;
        cout << "AST: " << ast << endl;
        cout << "REB: " << reb << endl;
        cout << "REB: " << reb << endl;
        cout << "Home Team Wins: " << homeTeamWins << endl;
        try
        {
            Record recordToInsert(gameDateStr, teamID, pts, reb, ast, fgPct, ftPct, fg3Pct, homeTeamWins);
            cout << "-----------------------------" << endl;
            cout << "Record Information" << endl;
            cout << "Line Number: " << lineNumber << endl;
            cout << "Game Date Written to Database: " << recordToInsert.gameDate << endl;
            // cout << "Game Date Reading from Database: " << recordToInsert.offsetToDate(recordToInsert.gameDate) << endl;
            cout << "Team ID Written to Database: " << recordToInsert.teamID << endl;
            // cout << "Team ID Reading from Database: " << recordToInsert.offsetToTeamID(recordToInsert.teamID) << endl;
            cout << "PTS: " << recordToInsert.pts << endl;
            cout << "FG Pct: " << recordToInsert.fgPct << endl;
            cout << "FT Pct: " << recordToInsert.ftPct << endl;
            cout << "FG3 Pct: " << recordToInsert.fg3Pct << endl;
            cout << "AST: " << recordToInsert.ast << endl;
            cout << "REB: " << recordToInsert.reb << endl;
            cout << "Home Team Wins Written to Database: " << recordToInsert.homeTeamWins << endl;
            // cout << "Home Team Wins Reading from Database: " << recordToInsert.boolWinsToInt(recordToInsert.homeTeamWins) << endl;
            records.push_back(recordToInsert);
        }
        catch (...)
        {
            cerr << "Error parsing line number: " << lineNumber << endl;
        }
    }
    inputFile.close();
    cout << "Sorted Records" << endl;
    sort(records.begin(), records.end(), compareRecords);
    for (const Record &record : records)
    {
        record.print();
        if (storage.allocateRecord(record))
        {
            // cout << "Record allocated sucessfully." << endl;
        }
        else
        {
            cerr << "An error occured while storing record" << endl;
        }
    }
    storage.printBlockRecords();
    std::vector<Record> recordsRead;
    uchar *dataBlock0 = storage.readBlock(0);
    int block0Records = storage.getNumberOfRecords(0);
    cout << "Block 0 Records: " << block0Records << endl;
    uchar *dataBlock1 = storage.readBlock(1);
    int block1Records = storage.getNumberOfRecords(1);
    cout << "Block 1 Records: " << block1Records << endl;
    cout << "Reading Block 0 From Database" << endl;
    for (int i = 0; i < block0Records; ++i)
    {
        // Calculate the offset within dataBlock0 for each record
        int offset = i * sizeof(Record);

        // Create a new Record object from the data at the calculated offset
        //TODO FIGURE OUT WHY THE ASSISTS AND REBOUNDS HAVE SWITCHED PLACES
        Record record(
            *reinterpret_cast<int *>(dataBlock0 + offset),                     // gameDate
            *reinterpret_cast<unsigned short int *>(dataBlock0 + offset + 4),  // teamID
            *reinterpret_cast<unsigned short int *>(dataBlock0 + offset + 6),  // points
            *reinterpret_cast<unsigned short int *>(dataBlock0 + offset + 10),  // rebounds
            *reinterpret_cast<unsigned short int *>(dataBlock0 + offset + 8), // assists
            *reinterpret_cast<float *>(dataBlock0 + offset + 12),              // fgPercentage
            *reinterpret_cast<float *>(dataBlock0 + offset + 16),              // ftPercentage
            *reinterpret_cast<float *>(dataBlock0 + offset + 20),              // fg3Percentage
            *reinterpret_cast<bool *>(dataBlock0 + offset + 24)                // fg3Percentage
        );

        recordsRead.push_back(record);
        // Add the record to the vector
    }
    // cout << "Reading Block 1 From Database" << endl;
    // for (int i = 0; i < block1Records; ++i)
    // {
    //     // Calculate the offset within dataBlock0 for each record
    //     int offset = i * sizeof(Record);

    //     // Create a new Record object from the data at the calculated offset
    //     Record record(
    //         *reinterpret_cast<int *>(dataBlock1 + offset),                     // gameDate
    //         *reinterpret_cast<unsigned short int *>(dataBlock1 + offset + 4),  // teamID
    //         *reinterpret_cast<unsigned short int *>(dataBlock1 + offset + 6),  // points
    //         *reinterpret_cast<unsigned short int *>(dataBlock1 + offset + 8),  // rebounds
    //         *reinterpret_cast<unsigned short int *>(dataBlock1 + offset + 10), // assists
    //         *reinterpret_cast<float *>(dataBlock1 + offset + 12),              // fgPercentage
    //         *reinterpret_cast<float *>(dataBlock1 + offset + 16),              // ftPercentage
    //         *reinterpret_cast<float *>(dataBlock1 + offset + 20),              // fg3Percentage
    //         *reinterpret_cast<bool *>(dataBlock1 + offset + 24)                // fg3Percentage
    //     );

    //     recordsRead.push_back(record);
    //     // Add the record to the vector
    // }
    for (const Record &record : recordsRead)
    {
        record.print();
    }

    return 1;
}
