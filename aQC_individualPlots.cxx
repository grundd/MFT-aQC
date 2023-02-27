// aQC_individualPlots.cxx
// David Grund, Feb 24, 2023
// MFT asynchronous Quality Control

// cpp
#include <vector>
// root
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

int run = 529009;
string period = "LHC22q";
string pass1 = "apass2";
string pass2 = "apass3";

void aQC_individualPlots()
{
    gStyle->SetOptStat(0);
    gSystem->Exec("mkdir -p results/individualPlots/");
    string s1 = Form("results/%s/runsRootFiles/%i_%s.root",period.data(),run,pass1.data());
    string s2 = Form("results/%s/runsRootFiles/%i_%s.root",period.data(),run,pass2.data());
    TFile* f1 = new TFile(s1.data(),"read");
    TFile* f2 = new TFile(s2.data(),"read");
    //f1->ls();
    TLatex* l = new TLatex();
    l->SetTextSize(0.04);
    l->SetTextAlign(21);
    l->SetNDC();
    // go over the histograms and print them
    // 2d 
    std::vector<string> sNamesTH2 = {
        "mMFTTrackEtaPhi_5_MinClusters",
        "mMFTTrackXY_5_MinClusters",
        "mMFTTrackEtaPhi_7_MinClusters",
        "mMFTTrackXY_7_MinClusters"
    };
    for(int i = 0; i < sNamesTH2.size(); i++) {
        TH2F* h1 = (TH2F*)f1->Get(sNamesTH2[i].data());
        TH2F* h2 = (TH2F*)f2->Get(sNamesTH2[i].data());
        // print for pass1
        TCanvas c1("c1","",800,700);
        c1.SetRightMargin(0.12);
        if(h1) {
            h1->GetXaxis()->SetDecimals(1);
            h1->GetZaxis()->SetMaxDigits(3);
            h1->Draw("COLZ");
        } 
        l->DrawLatex(0.9,0.95,pass1.data());
        c1.Print(Form("results/individualPlots/%i_%s_%s.pdf",run,pass1.data(),sNamesTH2[i].data()));
        // print for pass2
        TCanvas c2("c2","",800,700);
        c2.SetRightMargin(0.12);
        if(h2) {
            h2->GetXaxis()->SetDecimals(1);
            h2->GetZaxis()->SetMaxDigits(3);
            h2->Draw("COLZ");
        }
        l->DrawLatex(0.9,0.95,pass2.data());
        c2.Print(Form("results/individualPlots/%i_%s_%s.pdf",run,pass2.data(),sNamesTH2[i].data()));
    }
    // 1d
    std::vector<string> sNamesTH1 = {
        "mMFTTrackEta",
        "mMFTTrackNumberOfClusters",
        "mMFTTrackPhi",
        "mMFTTrackTanl",
        "mMFTTrackInvQPt"
    };
    for(int i = 0; i < sNamesTH1.size(); i++) {
        TH1F* h1 = (TH1F*)f1->Get(sNamesTH1[i].data());
        TH1F* h2 = (TH1F*)f2->Get(sNamesTH1[i].data());
        TCanvas c("c","",800,700);
        c.SetRightMargin(0.04);
        if(h1) { 
            h1->GetYaxis()->SetMaxDigits(3);
            h1->GetYaxis()->SetDecimals(1);
            h1->GetYaxis()->SetRangeUser(0.,h1->GetMaximum()*1.08);
            h1->GetXaxis()->SetDecimals(1);
            h1->SetLineWidth(2);
            h1->SetLineColor(kBlue);
            h1->Draw();
        }
        if(h2) {
            h2->SetLineWidth(2);
            h2->SetLineColor(kRed);
            h2->Draw("SAME");
        }
        TLegend l(0.80,0.91,0.98,0.99);
        l.AddEntry(h1,pass1.data(),"L");
        l.AddEntry(h2,pass2.data(),"L");
        l.SetTextSize(0.04);
        l.SetBorderSize(0);
        l.SetFillStyle(0);
        l.SetMargin(0.30);
        l.Draw();
        c.Print(Form("results/individualPlots/%i_%s_%s_%s.pdf",run,pass1.data(),pass2.data(),sNamesTH1[i].data()));
    }
}