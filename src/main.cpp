#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <map>
#include <string>
#include <cstdint>
#include <typeinfo>
#include <chrono>
#include <ctime>
#include <algorithm>
#include "storage.h"
#include "record.h"
#include "types.h"
#include "bplustree.h"
// #include "b+tree_search.cpp"

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
    // Comapre fgPct of games
    if (a.fgPct < b.fgPct)
    {
        return true;
    }
    // If fgPct values are the same, compare by teamID
    else if (a.fgPct == b.fgPct)
    {
        return a.teamID < b.teamID;
    }
    return false;
}

int main()
{
    
    string outputFileName = "output.txt";
    ofstream outputFile(outputFileName);
    if (!outputFile.is_open()) {
        cerr << "Failed to open the file for writing: " << outputFileName << endl;
        return 1; // Exit with an error code
    }
    streambuf* coutBuffer = cout.rdbuf();
    cout.rdbuf(outputFile.rdbuf());

    vector<Record> records;
    map<float, vector<Address>> recordMap;
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
        unsigned short int recordNumber = static_cast<unsigned short int>(lineNumber - 1);
        // Skip the first line of the file (column headers)
        if (lineNumber == 1)
        {
            continue;
        }
        // Comment out for final demonstration
        // if (lineNumber == 500)
        // {
        //     break;
        // }
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
        std::cout << "-----------------------------" << endl;
        std::cout << "Read from File: " << endl;
        std::cout << "Line Number: " << lineNumber << endl;
        std::cout << "Game Date: " << gameDateStr << endl;
        std::cout << "Team ID: " << +teamID << endl;
        std::cout << "PTS: " << +pts << endl;
        std::cout << "FG Pct: " << fgPct << endl;
        std::cout << "FT Pct: " << ftPct << endl;
        std::cout << "FG3 Pct: " << fg3Pct << endl;
        std::cout << "AST: " << +ast << endl;
        std::cout << "REB: " << +reb << endl;
        std::cout << "Home Team Wins: " << homeTeamWins << endl;
        try
        {
            Record recordToInsert = Record(recordNumber, gameDateStr, teamID, pts, reb, ast, fgPct, ftPct, fg3Pct, homeTeamWins, 0, 0);
            std::cout << "-----------------------------" << endl;
            std::cout << "Record Information" << endl;
            std::cout << "Line Number: " << lineNumber << endl;
            std::cout << "Game Date Written to Database: " << recordToInsert.gameDate << endl;
            // cout << "Game Date Reading from Database: " << recordToInsert.offsetToDate(recordToInsert.gameDate) << endl;
            std::cout << "Team ID Written to Database: " << +recordToInsert.teamID << endl;
            // cout << "Team ID Reading from Database: " << recordToInsert.offsetToTeamID(recordToInsert.teamID) << endl;
            std::cout << "PTS: " << +recordToInsert.pts << endl;
            std::cout << "FG Pct: " << recordToInsert.fgPct << endl;
            std::cout << "FT Pct: " << recordToInsert.ftPct << endl;
            std::cout << "FG3 Pct: " << recordToInsert.fg3Pct << endl;
            std::cout << "AST: " << +recordToInsert.ast << endl;
            std::cout << "REB: " << +recordToInsert.reb << endl;
            std::cout << "Home Team Wins Written to Database: " << recordToInsert.homeTeamWins << endl;
            // cout << "Home Team Wins Reading from Database: " << recordToInsert.boolWinsToInt(recordToInsert.homeTeamWins) << endl;
            records.push_back(recordToInsert);
        }
        catch (...)
        {
            cerr << "Error parsing line number: " << lineNumber << endl;
        }
    }
    inputFile.close();
    cout << "------------------------------------------" << endl;
    cout << "Record Offsets" << endl;
    cout << "fgPct: " << offsetof(Record, fgPct) << endl;
    cout << "ftPct: " << offsetof(Record, ftPct) << endl;
    cout << "fg3Pct: " << offsetof(Record, fg3Pct) << endl;
    cout << "gameDate: " << offsetof(Record, gameDate) << endl;
    cout << "blockID: " << offsetof(Record, blockAddress) << endl;
    cout << "offset: " << offsetof(Record, offset) << endl;
    cout << "recordID: " << offsetof(Record, recordID) << endl;
    cout << "teamID: " << offsetof(Record, teamID) << endl;
    cout << "pts: " << offsetof(Record, pts) << endl;
    cout << "ast: " << offsetof(Record, ast) << endl;
    cout << "reb: " << offsetof(Record, reb) << endl;
    cout << "homeTeamWins: " << offsetof(Record, homeTeamWins) << endl;
    cout << "------------------------------------------" << endl;
    cout << "Sorted Records" << endl;
    sort(records.begin(), records.end(), compareRecords);
    for (Record record : records)
    {
        if (storage.allocateRecord(record))
        {
            // cout << "Record allocated " << endl;
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
    cout << "Reading Block 0 From Database" << endl;
    // recordsRead = storage.readRecordsFromBlock(0);
    recordsRead = storage.readAllRecords();
    cout << "------------------------------------------" << endl;
    cout << "Experiment 1" << endl;
    int recordsStored = storage.getRecordsStored();
    cout << "Number of Records: " << recordsStored << endl;
    cout << "Size of Record: " << sizeof(Record) << " bytes" << endl;
    cout << "Number of Records Per Block : " << (storage.getBlockSize() / sizeof(Record)) << endl;
    cout << "Number of Blocks: " << storage.getBlocksUsed() << endl;
    cout << "Number of Records Stored Per Block: " << endl;
    storage.printBlockRecords();
    cout << "All records:" << endl;
    for (const Record record : recordsRead)
    {
        if (recordMap.find(record.fgPct) == recordMap.end())
        {
            //If value is not found, create a vector and add it to the hash map
            vector<Address> addresses;
            addresses.push_back(Address(record.blockAddress, record.offset));
            recordMap[record.fgPct] = addresses;
        }
        else
        {
            //Else append the value to the existing vector for the key
            recordMap[record.fgPct].push_back(Address(record.blockAddress, record.offset));
        }
        record.print();
    }
    cout << "-----------------PRINTING RECORD MAP-------------------------" << endl;
    // Printing record map structure
    for (const auto &pair : recordMap)
    {
        std::cout << "Key: " << pair.first << ", Value: " << endl;
        for (const Address &address : pair.second)
        {
            if (address.blockAddress != nullptr)
            {
                std::cout << "Block Address: " << address.blockAddress << std::endl;
                std::cout << "Offset: " << address.offset << std::endl;
            }
            else
            {
                std::cout << "Block Address is null." << std::endl;
            }
        }
    }
    cout << "------------------------------------------" << endl;
    cout << "Experiment 2 " << endl;
    // Initialize B+ Tree Object
    BPlusTree tree = BPlusTree();
    cout << "Maximum Keys in a node: " << tree.maxKeys << endl;
    cout << "Map size: " << recordMap.size() << endl;
    int count = 0;
    for (const auto &pair : recordMap)
    {
        count++;
        cout << "Count: " << count << endl;
        std::cout << "Key: " << pair.first << ", Value: " << &pair.second << endl;
        tree.insert(pair.first, pair.second);
        // cout << "Content of the root node: " << endl;
        for (int i = 0; i < tree.rootNode->getNumKeys(); i++)
        {
            cout << tree.rootNode->getChild(i, 0).blockAddress << " | "<< tree.rootNode->getKey(i) << " | ";
        }
        if(tree.rootNode->getChild(tree.rootNode->getNumKeys(), 0).blockAddress != nullptr)
            cout << tree.rootNode->getChild(tree.rootNode->getNumKeys(), 0).blockAddress << endl;
        else
            cout << "NULL" << endl;
        cout << "Number of nodes: " << tree.nodesStored << endl;
        cout << "Number of keys: " << tree.keysStored << endl;
        cout << "Number of levels: " << tree.levels << endl;
    }
    cout << "Number of nodes: " << tree.nodesStored << endl;
    cout << "Number of keys: " << tree.keysStored << endl;
    cout << "Number of levels: " << tree.levels << endl;
    tree.displayTree(tree.rootNode,1);
    
    //Experiment 3
    //TODO: Write the average of Field Goal 3 percentage home
    //TODO: Running time retrieval of b+ tree
    //TODO: Number of data blocks accessed by a brute force scan
    cout << "------------------------------------------" << endl;
    cout << "Experiment 3" << endl;
    vector<Address> results = tree.searchKey(0.5);
    int resultsLength = results.size();
    cout << "Number of Records with FG_PCT 0.5: " << resultsLength << endl;

    //Experiment 4
    cout << "------------------------------------------" << endl;
    cout << "Experiment 4" << endl;
    vector<vector<Address>> rangedResults = tree.searchRange(0.6,1);
    int rangedResultsSize = 0;
    for(vector<Address> vector: rangedResults){
        rangedResultsSize += vector.size();
    }
    cout << "Number of Records with FG_PCT between 0.6 and 1 (inclusive): " << rangedResultsSize << endl;
    // Restore the original std::cout buffer
    cout.rdbuf(coutBuffer);
    // Close the output file
    outputFile.close();
    return 1;
}
