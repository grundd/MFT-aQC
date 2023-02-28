// aQC_downloadHistos.cxx
// David Grund, Feb 27, 2023
// MFT asynchronous Quality Control

// root
#include "TSystem.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
// my headers
#include "aQC_utilities.h"

bool rewriteFiles = false;
bool oldPath = false;

template <typename TH>
TH* loadHisto(string path, string histName, int runNo, string pass)
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

void saveHistos(string period, int runNo, string pass)
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
            TH2F* h = loadHisto<TH2F>(sPath,sNamesTH2[i],runNo,pass);
            if(h) {
                cout << "run " << runNo << ", " << pass << ": " << h->GetName() << " loaded\n";
                f->cd();
                h->Write(renameHisto(h->GetName()).data());
            }
        }
        for(int i = 0; i < sNamesTH1.size(); i++) {
            TH1F* h = loadHisto<TH1F>(sPath,sNamesTH1[i],runNo,pass);
            if(h) {
                cout << "run " << runNo << ", " << pass << ": " << h->GetName() << " loaded\n";
                f->cd();
                h->Write(renameHisto(h->GetName()).data());
            }
        }
        f->Write("",TObject::kWriteDelete);
        f->Close();
    }
    return;
}

void aQC_downloadHistos(string sIn)
{
    string period;
    string pass[5];
    std::vector<int>* runList = readInput(sIn,period,pass[0],pass[1],pass[2],pass[3],pass[4]);
    int nRuns = runList->size();

    for(int r = 0; r < nRuns; r++) {
        for(int p = 0; p < 5; p++) {
            if(pass[p] != "none") saveHistos(period,runList->at(r),pass[p]);
        }
    }

    cout << "Done.\n\n";
    return;
}