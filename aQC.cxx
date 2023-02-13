// aQC.cxx
// David Grund, Jan 18, 2023
// MFT asynchronous Quality Control

// cpp
#include <vector>
#include <fstream>
// root
#include "TSystem.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

bool rewriteFiles = false;
bool oldPath = false;

vector<int>* ReadInput(string sIn, string &period, string &pass1, string &pass2, string &pass3)
{
    std::vector<int>* runList = new std::vector<int>;
    ifstream ifs;
    ifs.open(sIn);
    int nRuns;
    ifs >> period
        >> pass1
        >> pass2
        >> pass3
        >> nRuns;
    for(int i = 0; i < nRuns; i++) {
        int run;
        ifs >> run;
        runList->push_back(run);
    }
    cout << "## Input: ##\n"
         << "period: " << period << "\n"
         << "pass1:  " << pass1 << "\n"
         << "pass2:  " << pass2 << "\n"
         << "pass3:  " << pass3 << "\n"
         << "# runs: " << nRuns << "\n";
    for(int i = 0; i < nRuns; i++) cout << Form("%03i -> ",i+1) << runList->at(i) << "\n";
    return runList;
}

template <typename TH>
TH* LoadHisto(string path, string histName, int runNo, string pass)
{
    o2::ccdb::CcdbApi api;
    if(pass == "passMC") api.init("ccdb-test.cern.ch:8080");
    else                 api.init("ali-qcdb-gpn.cern.ch:8083");
    map<string, string> metadata;
    metadata["RunNumber"] = std::to_string(runNo);
    if(pass != "online") metadata["PassName"] = pass;
    TH* h = api.retrieveFromTFileAny<TH>(path+histName,metadata,-1); // timestamp: -1
    if(h) return h;
    else  return NULL;
}

string RenameHisto(string oldName)
{
    string newName = oldName;
    char slash = '/';
    size_t index = newName.find_last_of(slash);
    if (index < string::npos) {
        // erase everything up to 'index'
        newName.erase(0,index+1);
    }
    return newName;
}

void SaveHistos(string period, int runNo, string pass)
{
    gSystem->Exec(Form("mkdir -p results/%s/runsRootFiles/",period.data()));
    // online QC?
    bool online = false;
    if(pass == "online") online = true;
    // MC?
    bool isMC = false;
    if(pass == "passMC") isMC = true;
    // check if the file already exists
    string sFile = Form("results/%s/runsRootFiles/%i_%s.root",period.data(),runNo,pass.data());
    bool fileExists = !gSystem->AccessPathName(sFile.data());
    if(fileExists && !rewriteFiles) {
        cout << sFile << ": already downloaded -> skipping...\n";
    } else {
        cout << sFile << ": will be downloaded now.\n";
        TFile* f = new TFile(sFile.data(),"recreate");
        string sPath = "";
        if(isMC) {
            if(!online) sPath += "qc_mc/";
            else { 
                cout << "Online QC not available for MC runs -> terminating...\n";
                return;
            }
        } else {
            if(!online) sPath += "qc_async/";
            else        sPath += "qc/";
        }    
        sPath += "MFT/MO/";
        string subfldr = "Tracks/";
        if(oldPath) subfldr = "QcMFTAsync/";
        if(online) subfldr = "MFTAsyncTask/";
        std::vector<string> sNamesTH2 = {
            // track position
            subfldr+"tracks/mMFTTrackEtaPhi_5_MinClusters",
            subfldr+"tracks/mMFTTrackXY_5_MinClusters",
            subfldr+"tracks/mMFTTrackEtaPhi_7_MinClusters",
            subfldr+"tracks/mMFTTrackXY_7_MinClusters",
            subfldr+"tracks/mMFTTrackEtaPhi_8_MinClusters",
            subfldr+"tracks/mMFTTrackXY_8_MinClusters"
        };
        if(isMC) {
            sNamesTH2.push_back("Digits/mDigitOccupancySummary");
            sNamesTH2.push_back("Clusters/mClusterOccupancySummary");
        }
        std::vector<string> sNamesTH1 = {
            // tracks
            subfldr+"tracks/mMFTTrackEta",
            subfldr+"tracks/mMFTTrackNumberOfClusters",
            subfldr+"tracks/mMFTTrackPhi",
            subfldr+"tracks/mMFTTrackTanl",
            subfldr+"tracks/mNOfTracksTime",
            subfldr+"tracks/mMFTTrackInvQPt",
            subfldr+"tracks/CA/mMFTCATrackPt",
            subfldr+"tracks/LTF/mMFTLTFTrackPt",
            subfldr+"tracks/CA/mMFTCATrackEta",
            subfldr+"tracks/LTF/mMFTLTFTrackEta",
            subfldr+"tracks/mMFTTracksBC",
            // clusters
            subfldr+"clusters/mMFTClusterPatternIndex",
            subfldr+"clusters/mMFTClusterSensorIndex",
            subfldr+"clusters/mMFTClustersROFSize",
            subfldr+"clusters/mNOfClustersTime"
        };
        if(isMC) {
            sNamesTH1.push_back("Digits/mDigitChipOccupancy");
            sNamesTH1.push_back("Digits/mDigitsBC");
            sNamesTH1.push_back("Clusters/mClusterOccupancy");
            sNamesTH1.push_back("Clusters/mClusterSizeSummary");
            sNamesTH1.push_back("Clusters/mClusterZ");
            sNamesTH1.push_back("Clusters/mGroupedClusterSizeSummary");
        }
        for(int i = 0; i < sNamesTH2.size(); i++) {
            TH2F* h = LoadHisto<TH2F>(sPath,sNamesTH2[i],runNo,pass);
            if(h) {
                cout << "run " << runNo << ", " << pass << ": " << h->GetName() << " loaded\n";
                f->cd();
                h->Write(RenameHisto(h->GetName()).data());
            }
        }
        for(int i = 0; i < sNamesTH1.size(); i++) {
            TH1F* h = LoadHisto<TH1F>(sPath,sNamesTH1[i],runNo,pass);
            if(h) {
                cout << "run " << runNo << ", " << pass << ": " << h->GetName() << " loaded\n";
                f->cd();
                h->Write(RenameHisto(h->GetName()).data());
            }
        }
        f->Write("",TObject::kWriteDelete);
        f->Close();
    }
    return;
}

void aQC(string sIn)
{
    string period;
    string pass1;
    string pass2;
    string pass3;
    std::vector<int>* runList = ReadInput(sIn,period,pass1,pass2,pass3);
    int nRuns = runList->size();
    // QC of the first pass
    for(int i = 0; i < nRuns; i++) SaveHistos(period.data(),runList->at(i),pass1);
    // QC of the second pass (if any)
    if(pass2 != "none") for(int i = 0; i < nRuns; i++) SaveHistos(period.data(),runList->at(i),pass2);
    // if not MC -> online QC
    if(pass3 != "none") for(int i = 0; i < nRuns; i++) SaveHistos(period.data(),runList->at(i),pass3);

    cout << "Done.\n\n";
    return;
}