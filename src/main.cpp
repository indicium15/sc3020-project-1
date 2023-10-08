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
    
    string outputFileName1 = "Output.txt";
    string outputFileName2 = "Experiments.txt";
    ofstream outputFile1(outputFileName1);
    ofstream outputFile2(outputFileName2);
    if (!outputFile1.is_open() || !outputFile1.is_open()) {
        cerr << "Failed to open or more files for writing " << endl;
        return 1; // Exit with an error code
    }
    streambuf* coutBuffer = cout.rdbuf();
    cout.rdbuf(outputFile1.rdbuf());

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
    cout << "blockAddress: " << offsetof(Record, blockAddress) << endl;
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
    cout.rdbuf(outputFile2.rdbuf());
    cout << "Experiment 1" << endl;
    int recordsStored = storage.getRecordsStored();
    cout << "Number of Records: " << recordsStored << endl;
    cout << "Size of a Record: " << sizeof(Record) << " bytes" << endl;
    cout << "Number of Records in a Block : " << (storage.getBlockSize() / sizeof(Record)) << endl;
    cout << "Number of Blocks for Storing Data : " << storage.getBlocksUsed() << endl;
    cout.rdbuf(outputFile1.rdbuf());
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
    BPlusTree tree = BPlusTree(recordMap.size());
    cout << "Maximum Keys in a node: " << tree.maxKeys << endl;
    // cout << "Map size: " << recordMap.size() << endl;
    int count = 0;
    for (const auto &pair : recordMap)
    {
        count++;
        cout << "Count: " << count << endl;
        std::cout << "Key: " << pair.first << ", Value: " << &pair.second << endl;
        tree.insert(pair.first, pair.second);
        tree.displayTree(tree.rootNode, 1);
        // cout << "Content of the root node: " << endl;
        // for (int i = 0; i < tree.rootNode->getNumKeys(); i++)
        // {
        //     cout << tree.rootNode->getChild(i, 0).blockAddress << " | "<< tree.rootNode->getKey(i) << " | ";
        // }
        // if(tree.rootNode->getChild(tree.rootNode->getNumKeys(), 0).blockAddress != nullptr)
        //     cout << tree.rootNode->getChild(tree.rootNode->getNumKeys(), 0).blockAddress << endl;
        // else
        //     cout << "NULL" << endl;
        cout << "Number of nodes: " << tree.nodesStored << endl;
        cout << "Number of keys: " << tree.keysStored << endl;
        cout << "Number of levels: " << tree.levels << endl;
    }
    cout.rdbuf(outputFile2.rdbuf());
    cout << "------------------------------------------" << endl;
    cout << "Experiment 2 " << endl;
    // cout << "Number of keys stored in B+ Tree: " << tree.keysStored << endl;
    cout << "Parameter 'n' of B+ Tree: " << tree.maxKeys << endl;
    cout << "Number of nodes of B+ Tree: " << tree.nodesStored << endl;
    cout << "Number of levels of B+ Tree: " << tree.levels << endl;
    cout << "Content of Root Node:" << endl;
    tree.displayNode(tree.rootNode);
    
    cout.rdbuf(outputFile1.rdbuf());
    cout << "Content of Tree:" << endl;
    tree.displayTree(tree.rootNode,1);
    
    //Experiment 3
    cout.rdbuf(outputFile2.rdbuf());
    cout << "------------------------------------------" << endl;
    cout << "Experiment 3" << endl;
    auto experimentThreeStart = chrono::high_resolution_clock::now();
    vector<Address> results = tree.searchKey(0.5);
    vector<Record> resultRecords = storage.readRecordsfromAddresses(results);
    auto experimentThreeEnd = chrono::high_resolution_clock::now();
    chrono::nanoseconds experimentThreeRuntime = chrono::duration_cast<chrono::nanoseconds>(experimentThreeEnd - experimentThreeStart);
    int resultsLength = results.size();
    int resultRecordsLength = resultRecords.size();
    cout << "Number of Records with FG_PCT 0.5: " << resultsLength << endl;
    cout << "Linear Search for Records with FG_PCT 0.5" << endl;
    int linearSearchResultsLength = 0;
    int linearSearchDataBlocksAccessed = 0;
    bool flag = false;

    auto experimentThreeLinearSeachStart = chrono::high_resolution_clock::now();
    for(int i = 0; i<storage.getBlocksUsed(); i++){
        linearSearchDataBlocksAccessed++;
        vector<Record>recordsInBlock = storage.readRecordsFromBlock(i);
        for(Record record: recordsInBlock){
            if(record.fgPct == 0.5){
                linearSearchResultsLength++;
            }
            if(record.fgPct > 0.5){
                flag = true;
                break;
            }
        }
        if(flag){
            break;
        }
    }
    auto experimentThreeLinearSeachEnd = chrono::high_resolution_clock::now();
    chrono::nanoseconds experimentThreeLinearSearchRuntime = chrono::duration_cast<chrono::nanoseconds>(experimentThreeLinearSeachEnd - experimentThreeLinearSeachStart);
    //Finding average of fg3pct of Record
    float sum = 0.0f;
    for(Record record: resultRecords){
        sum+=record.fg3Pct;
    }
    float experimentThreeAverage = sum / static_cast<float>(resultRecords.size());
    cout << "Average of FG3_PCT_HOME for Records Returned: " << experimentThreeAverage << endl;
    cout << "Number of Records with FG_PCT 0.5 with Linear Search: " << linearSearchResultsLength << endl;
    cout << "Number of Data Blocks accessed for Query with Linear Search: " << linearSearchDataBlocksAccessed << endl;
    cout << "Runtime of Search and Retrieval: " << experimentThreeRuntime.count() << " nanoseconds" << endl;
    cout << "Runtime of Linear Search of Data Blocks: " << experimentThreeLinearSearchRuntime.count() << " nanoseconds" << endl;
    
    //Experiment 4
    cout << "------------------------------------------" << endl;
    cout << "Experiment 4" << endl;
    auto experimentFourStart = chrono::high_resolution_clock::now();
    vector<vector<Address>> rangedResults = tree.searchRange(0.6,1);
    vector<Record> rangedResultsRecords = storage.readRecordsfromNestedAddresses(rangedResults);
    auto experimentFourEnd = chrono::high_resolution_clock::now();
    chrono::nanoseconds experimentFourRuntime = chrono::duration_cast<chrono::nanoseconds>(experimentFourEnd - experimentFourStart);
    int rangedResultsSize = 0;
    for(vector<Address> vector: rangedResults){
        rangedResultsSize += vector.size();
    }
    // cout << "Number of Records (Address Structure) with FG_PCT between 0.6 and 1 (inclusive): " << rangedResultsSize << endl;
    // cout << "Number of Records (Record Structure) with FG_PCT between 0.6 and 1 (inclusive): " << rangedResultsRecords.size() << endl;
    int rangedLinearSeachResultsLength = 0;
    int rangedLinearSearchDataBlocksAccessed = 0;
    
    auto experimentFourLinearSeachStart = chrono::high_resolution_clock::now();
    for(int i = 0; i<storage.getBlocksUsed(); i++){
        rangedLinearSearchDataBlocksAccessed++;
        vector<Record>recordsInBlock = storage.readRecordsFromBlock(i);
        for(Record record: recordsInBlock){
            if(record.fgPct >= 0.6 && record.fgPct <= 1){
                rangedLinearSeachResultsLength++;
            }
        }
    }
    auto experimentFourLinearSeachEnd = chrono::high_resolution_clock::now();
    chrono::nanoseconds experimentFourLinearSearchRuntime = chrono::duration_cast<chrono::nanoseconds>(experimentFourLinearSeachEnd - experimentFourLinearSeachStart);
    sum = 0.0f;
    for(Record record: rangedResultsRecords){
        sum+=record.fg3Pct;
    } 
    float experimentFourAverage = sum / static_cast<float>(rangedResultsRecords.size());
    cout << "Average of FG3_PCT_HOME for Records Returned: " << experimentFourAverage << endl;
    cout << "Number of Records with FG_PCT between 0.6 and 1 (inclusive) with Linear Search: " << rangedLinearSeachResultsLength << endl;
    cout << "Number of Data Blocks accessed for Ranged Query with Linear Search: " << rangedLinearSearchDataBlocksAccessed << endl;
    cout << "Runtime of Search and Retrieval: " << experimentFourRuntime.count() << " nanoseconds" << endl; 
    cout << "Runtime of Linear Search of Data Blocks: " << experimentFourLinearSearchRuntime.count() << " nanoseconds" << endl;
    
    tree.displayTree(tree.rootNode, 1); 
    cout.rdbuf(outputFile1.rdbuf());
    //Experiment 5
    cout << "------------------------------------------" << endl;
    cout << "Experiment 5" << endl;
    
    // tree.displayTree(tree.rootNode, 1);
    int deletionLinearSearchResultsLength = 0;
    int deletionLinearSearchDataBlocksAccessed = 0;
    auto experimentFiveLinearSearchStart = chrono::high_resolution_clock::now();
    for(int i = 0; i<storage.getBlocksUsed(); i++){
        deletionLinearSearchDataBlocksAccessed++;
        vector<Record>recordsInBlock = storage.readRecordsFromBlock(i);
        for(Record record: recordsInBlock){
            if(record.fgPct <= 0.35){
                deletionLinearSearchResultsLength++;
            }
            else{
                break;
            }
        }
    }
    auto experimentFiveLinearSearchEnd = chrono::high_resolution_clock::now();
    chrono::nanoseconds experimentFiveLinearSearchRuntime = chrono::duration_cast<chrono::nanoseconds>(experimentFiveLinearSearchEnd - experimentFiveLinearSearchStart);
    //Measure start
    auto experimentFiveBPlusTreeSearchStart = chrono::high_resolution_clock::now();
    vector<vector<Address>> addressesToDelete = tree.searchRange(0,0.35);
    auto experimentFiveBPlusTreeSearchEnd = chrono::high_resolution_clock::now();
    chrono::nanoseconds experimentFiveBPlusTreeSearchRuntime = chrono::duration_cast<chrono::nanoseconds>(experimentFiveBPlusTreeSearchEnd - experimentFiveBPlusTreeSearchStart);
    //Call method to remove these addresses from the disk
    auto recordDeletionStart = chrono::high_resolution_clock::now();
    storage.removeRecordsfromNestedAddresses(addressesToDelete);
    auto recordDeletionEnd = chrono::high_resolution_clock::now();
    chrono::nanoseconds recordDeletionRuntime = chrono::duration_cast<chrono::nanoseconds>(recordDeletionEnd - recordDeletionStart);
    //Measure end
    vector<Record> testDeletion = storage.readAllRecords();
    for(Record test: testDeletion){
        test.print();
    }
    // Tree Deletion
    auto deletionStart = chrono::high_resolution_clock::now();
    for(auto pair: recordMap){
        if(pair.first <= 0.35){
            // Measure the runtime for each individual insertion call
            cout << "Deleting Record " << pair.first << endl;
            tree.deleteNode(pair.first);
        }
        else break;
    }
    auto deletionEnd = chrono::high_resolution_clock::now();
    chrono::nanoseconds bPlusTreeDeletionRuntime = chrono::duration_cast<chrono::nanoseconds>(deletionEnd - deletionStart);
    // tree.displayTree(tree.rootNode, 1);
    //Using search query to get address vectors of all records that need to be removed from the disk
    cout.rdbuf(outputFile2.rdbuf());
    cout << "------------------------------------------" << endl;
    cout << "Experiment 5" << endl;
    cout << "Number of nodes of the updated B+ tree: " << tree.nodesStored << endl;
    cout << "Number of levels of the updated B+ tree: " << tree.levels << endl;
    cout << "Content of the Root Node: " << endl;
    tree.displayNode(tree.rootNode);
    cout << "Runtime of Linear Search Query for Keys Between 0 and 0.35: " << experimentFiveLinearSearchRuntime.count() << " nanoseconds" << endl;
    cout << "Number of Data Blocks Accessed for Linear Search: " << deletionLinearSearchDataBlocksAccessed << endl;
    cout << "Runtime of B+ Tree Query for Keys Between 0 and 0.35: " << experimentFiveBPlusTreeSearchRuntime.count() << " nanoseconds" << endl;
    cout << "Runtime of B+ Tree Deletion of Keys Between 0 and 0.35: " << bPlusTreeDeletionRuntime.count() << " nanoseconds" << endl;
    cout << "Runtime of Database Record Deletion of Keys Between 0 and 0.35: " << recordDeletionRuntime.count() << " nanoseconds" << endl;
    
    tree.displayTree(tree.rootNode, 1);
    // Restore the original std::cout buffer
    cout.rdbuf(coutBuffer);
    // // Close the output file
    outputFile1.close();
    outputFile2.close();
    return 1;
}
