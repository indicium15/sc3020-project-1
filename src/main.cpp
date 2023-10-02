#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <typeinfo>
#include <chrono>
#include <ctime>
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
    uint databaseSize = 100 * 1024 * 1024;
    Storage storage(databaseSize, 400);
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
        uint8_t recordNumber = static_cast<uint8_t>(lineNumber - 1);
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
        uint8_t teamID = stoi(fields[1]);
        uint8_t pts, ast, reb;
        float fgPct, ftPct, fg3Pct;
        try
        {
            pts = stoi(fields[2]);
            ast = stoi(fields[6]);
            reb = stoi(fields[7]);
            fgPct = stof(fields[3]);
            ftPct = stof(fields[4]);
            fg3Pct = stof(fields[5]);
        }
        catch (invalid_argument &e)
        {
            pts = 0;
            ast = 0;
            reb = 0;
            fgPct = 0.0f;
            ftPct = 0.0f; 
            fg3Pct = 0.0f;
        }
        int homeTeamWins = stoi(fields[8]);
        cout << "-----------------------------" << endl;
        cout << "Read from File: " << endl;
        cout << "Line Number: " << lineNumber << endl;
        cout << "Game Date: " << gameDateStr << endl;
        cout << "Team ID: " << +teamID << endl;
        cout << "PTS: " << +pts << endl;
        cout << "FG Pct: " << fgPct << endl;
        cout << "FT Pct: " << ftPct << endl;
        cout << "FG3 Pct: " << fg3Pct << endl;
        cout << "AST: " << +ast << endl;
        cout << "REB: " << +reb << endl;
        cout << "Home Team Wins: " << homeTeamWins << endl;
        try
        {
            Record recordToInsert(recordNumber,gameDateStr, teamID, pts, reb, ast, fgPct, ftPct, fg3Pct, homeTeamWins);
            cout << "-----------------------------" << endl;
            cout << "Record Information" << endl;
            cout << "Line Number: " << lineNumber << endl;
            cout << "Game Date Written to Database: " << recordToInsert.gameDate << endl;
            // cout << "Game Date Reading from Database: " << recordToInsert.offsetToDate(recordToInsert.gameDate) << endl;
            cout << "Team ID Written to Database: " << +recordToInsert.teamID << endl;
            // cout << "Team ID Reading from Database: " << recordToInsert.offsetToTeamID(recordToInsert.teamID) << endl;
            cout << "PTS: " << +recordToInsert.pts << endl;
            cout << "FG Pct: " << recordToInsert.fgPct << endl;
            cout << "FT Pct: " << recordToInsert.ftPct << endl;
            cout << "FG3 Pct: " << recordToInsert.fg3Pct << endl;
            cout << "AST: " << +recordToInsert.ast << endl;
            cout << "REB: " << +recordToInsert.reb << endl;
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
    vector<Record> recordsRead;
    uchar *dataBlock0 = storage.readBlock(0);
    int block0Records = storage.recordsInBlock(0);
    cout << "Block 0 Records: " << block0Records << endl;
    uchar *dataBlock1 = storage.readBlock(1);
    int block1Records = storage.recordsInBlock(1);
    cout << "Block 1 Records: " << block1Records << endl;
    cout << "Reading Block 0 From Database" << endl;
    recordsRead = storage.readRecordsFromBlock(0);
    cout << "------------------------------------------" << endl;
    cout << "Experiment 1" << endl;
    int recordsStored = storage.getRecordsStored();
    cout << "Number of Records: " << recordsStored << endl;
    cout << "Size of Record: " << sizeof(Record)  << " bytes" << endl;
    cout << "Number of Records Per Block : " << (storage.getBlockSize() / sizeof(Record)) << endl;
    cout << "Number of Blocks: " << storage.getBlocksUsed() << endl;
    // cout << "Number of Records Stored Per Block: " << endl;
    // storage.printBlockRecords();
    cout << "All records:" << endl;
    for (const Record &record : recordsRead)
    {
        record.print();
    }
    return 1;
}
