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
int dateToOffset(const string& dateString) {
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
string offsetToDate(int offsetInDays) {
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

int main(){
    //Read the input file
    ifstream inputFile("../games.txt");
    if(!inputFile.is_open()){
        cerr << "Error: unable to open the file./n";
        return 1;
    }
    string line;
    int lineNumber = 0;
    while (getline(inputFile, line)){
        lineNumber++;
        //Skip the first line of the file
        if(lineNumber == 1){
            continue;
        }
        //Remove code for final 
        if(lineNumber==50){
            break;
        }
        istringstream iss(line);
        string gameDateStr, teamID;
        int pts, ast, reb, homeTeamWins;
        double fgPct, ftPct, fg3Pct;
        if(iss>>gameDateStr>>teamID>>pts>>fgPct>>ftPct>>fg3Pct>>ast>>reb>>homeTeamWins){
            int gameDateOffset = dateToOffset(gameDateStr);
            cout << "-----------------------------" << endl;
            cout << "Line Number: " << lineNumber << endl;
            cout << "Game Date: " << offsetToDate(gameDateOffset) << endl;
            cout << "Team ID: " << teamID << endl;
            cout << "PTS: " << pts << endl;
            cout << "FG Pct: " << fgPct << endl;
            cout << "FT Pct: " << ftPct << endl;
            cout << "FG3 Pct: " << fg3Pct << endl;
            cout << "AST: " << ast << endl;
            cout << "REB: " << reb << endl;
            cout << "Home Team Wins: " << homeTeamWins << endl;
        } else{
            cerr << "Error parsing line number: " << lineNumber << endl;
        }
    }
    inputFile.close();
    return 0;
}
