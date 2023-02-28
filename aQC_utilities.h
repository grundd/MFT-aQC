// aQC_utilities.h
// David Grund, Feb 27, 2023
// MFT asynchronous Quality Control

// cpp
#include <vector>
#include <fstream>

vector<int>* readInput(string sIn, string &period, 
    string &pass0, string &pass1, string &pass2, string &pass3, string &pass4)
{
    std::vector<int>* runList = new std::vector<int>;
    ifstream ifs;
    ifs.open(sIn);
    int nRuns;
    ifs >> period
        >> pass0
        >> pass1
        >> pass2
        >> pass3
        >> pass4
        >> nRuns;
    for(int i = 0; i < nRuns; i++) {
        int run;
        ifs >> run;
        runList->push_back(run);
    }
    cout << "\nInput:\n"
         << "period: " << period << "\n"
         << "pass 0: " << pass0 << "\n"
         << "pass 1: " << pass1 << "\n"
         << "pass 2: " << pass2 << "\n"
         << "pass 3: " << pass3 << "\n"
         << "pass 4: " << pass4 << "\n"
         << "# runs: " << nRuns << "\n";
    for(int i = 0; i < nRuns; i++) cout << Form("%03i -> ",i+1) << runList->at(i) << "\n";
    return runList;
}