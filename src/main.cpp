#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <string>
#include <typeinfo>

using namespace std;

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
        string gameDate, teamID;
        int pts, ast, reb, homeTeamWins;
        double fgPct, ftPct, fg3Pct;
        if(iss>>gameDate>>teamID>>pts>>fgPct>>ftPct>>fg3Pct>>ast>>reb>>homeTeamWins){
            cout << "-----------------------------" << endl;
            cout << "Line Number: " << lineNumber << endl;
            cout << "Game Date: " << gameDate << endl;
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
