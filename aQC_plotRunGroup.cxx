// aQC_plotRunGroup.cxx
// David Grund, Feb 27, 2023
// MFT asynchronous Quality Control

// root headers
#include "TSystem.h"
#include "TROOT.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TH1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
// my headers
#include "aQC_utilities.h"

std::vector<int>* runList;
int nRuns;

Color_t colorTable[] = {
    kBlue,
    kBlue+1,
    kBlue+2,
    kRed,
    kRed+1,
    kRed+2,
    kGreen,
    kGreen+1,
    kGreen+2,
    kViolet,
    kViolet+1,
    kViolet+2,
    kCyan,
    kCyan+1,
    kCyan+2,
    kYellow+1,
    kYellow+2,
    kYellow+3,
    kMagenta,
    kMagenta+1,
    kMagenta+2,
    kOrange,
    kOrange+1,
    kOrange+2,
    kGray,
    kGray+1,
    kGray+2,
    kTeal,
    kTeal+1,
    kTeal+2,
    kSpring,
    kSpring+1,
    kSpring+2,
    kPink,
    kPink+1,
    kPink+2
};
string histosNames[5] = {
    "mMFTTrackEta",
    "mMFTTrackNumberOfClusters",
    "mMFTTrackPhi",
    "mMFTTrackTanl",
    "mMFTTrackInvQPt"
};
string histosTitles[5] = {
    "Track #eta",
    "# clusters per track",
    "Track #phi",
    "Track tan #lambda",
    "Track q/p_{T}"
};

void setAxes(TH1F* h)
{
    // font sizes
    int sizePixels = 18;
    h->GetXaxis()->SetTitleFont(63);
    h->GetXaxis()->SetTitleSize(sizePixels);
    h->GetYaxis()->SetTitleFont(63);
    h->GetYaxis()->SetTitleSize(sizePixels);
    h->GetXaxis()->SetLabelFont(63);
    h->GetXaxis()->SetLabelSize(sizePixels);
    h->GetYaxis()->SetLabelFont(63);
    h->GetYaxis()->SetLabelSize(sizePixels);
    // other
    h->GetXaxis()->SetDecimals(1);
    h->GetXaxis()->SetTitleOffset(1.05);
    h->GetYaxis()->SetTitleOffset(1.6);
    h->GetYaxis()->SetMaxDigits(3);
    return;
}

void plotRunGroup(string period, string pass, string histName, string title, int refRun = 529009)
{
    TObjArray* arr = new TObjArray(nRuns);
    arr->SetOwner();
    float yMin = 1e10;
    float yMax = 0.;
    float xMin = 1e10;
    float xMax = 0;
    for(int r = 0; r < nRuns; r++) {
        string s = Form("results/%s/runsRootFiles/%i_%s.root",period.data(),runList->at(r),pass.data());
        TFile* f = TFile::Open(s.data(),"read");
        TH1F* h = (TH1F*)f->Get(histName.data());
        if(h) {
            gROOT->cd();
            TH1F* hClone = (TH1F*)h->Clone(Form("h_%i",r));
            hClone->Scale(1./h->Integral());
            if(hClone->GetMaximum() > yMax) yMax = hClone->GetMaximum();
            if(hClone->GetMinimum() < yMin) yMin = hClone->GetMinimum();
            xMin = hClone->GetBinLowEdge(1);
            xMax = hClone->GetBinLowEdge(hClone->GetNbinsX()+1);
            arr->AddAt(hClone, r);
        }
        f->Close();
    }
    // reference run
    TH1F* hRefRun = NULL;
    if(refRun != -1) {
        string s = Form("results/%s/runsRootFiles/%i_%s.root",period.data(),refRun,pass.data());
        TFile* f = TFile::Open(s.data(),"read");
        gROOT->cd();
        TH1F* h = (TH1F*)f->Get(histName.data());
        hRefRun = (TH1F*)h->Clone("hRefRun");
        f->Close();
        hRefRun->Scale(1./hRefRun->Integral());
    }
    // make the plot
    TCanvas c(histName.data(),"",800,600);
    float x = 0.84;
    float y = 0.40;
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TLatex* l = new TLatex();
    l->SetTextFont(63);
    l->SetTextSize(20);
    l->SetTextAlign(21);
    l->SetNDC();
    // upper pad: distributions
    TPad p1("p1","",0.0,y,x,1.0);
    p1.SetTopMargin(0.09);
    p1.SetBottomMargin(0.);
    p1.SetRightMargin(0.025);
    p1.SetLeftMargin(0.10);
    //p1.SetFillColor(kRed);
    p1.Draw();
    p1.cd();
    TH1F* hAxisUpp = gPad->DrawFrame(xMin,yMin,xMax,yMax*1.02);
    setAxes(hAxisUpp);
    hAxisUpp->GetYaxis()->SetTitle("# entries (normalized to integral)");
    hAxisUpp->Draw("AXIS");
    for(int r = 0; r < nRuns; r++) {
        TH1F* h = (TH1F*)arr->At(r);
        if(h) {
            h->SetLineColor(colorTable[r]);
            h->SetLineStyle(1 + (r % 3));
            h->SetLineWidth(2);
            h->Draw("HIST SAME");
        }
    }
    l->DrawLatex(0.5,0.94,Form("%s %s: %s",period.data(),pass.data(),title.data()));
    // lower pad: ratios
    c.cd();
    TPad p2("p2","",0.0,0.0,x,y);
    p2.SetTopMargin(0.);
    p2.SetBottomMargin(0.18);
    p2.SetRightMargin(0.025);
    p2.SetLeftMargin(0.10);
    //p2.SetFillColor(kBlue);
    p2.Draw();
    p2.cd();
    TH1F* hAxisLow = gPad->DrawFrame(xMin,0.5,xMax,1.9);
    setAxes(hAxisLow);
    hAxisLow->GetYaxis()->SetTitle(Form("ratio to run %i",refRun));
    hAxisLow->GetXaxis()->SetTitle(hRefRun->GetXaxis()->GetTitle());
    hAxisLow->Draw("AXIS");
    for(int r = 0; r < nRuns; r++) {
        TH1F* h = (TH1F*)arr->At(r);
        if(h) {
            TH1F* hRatio = (TH1F*)h->Clone(Form("hRatio_%i",r));
            //hRatio->Sumw2();
            hRatio->Divide(hRefRun);
            hRatio->Draw("HIST SAME");
        }
    }
    // panel on the right: legends
    c.cd();
    TPad p3("p3","",x,0.0,1.0,1.0);
    //p3.SetFillColor(kGreen);
    p3.Draw();
    p3.cd();
    float ytop = 0.98;
    TLegend leg(0.0,ytop-(nRuns)*0.027,0.95,ytop);
    for(int r = 0; r < nRuns; r++) {
        TH1F* h = (TH1F*)arr->At(r);
        if(h) leg.AddEntry(h,Form("%i",runList->at(r)),"L");
        else leg.AddEntry((TObject*)0,Form("%i: n/a",runList->at(r)),"");
    }
    leg.SetTextFont(63);
    leg.SetTextSize(16);
    leg.SetBorderSize(0);
    leg.SetFillStyle(0);
    leg.SetMargin(0.28);
    leg.Draw();
    // print the plot
    c.Print(Form("results/plotRunGroup/%s_%s.pdf",pass.data(),histName.data()));
    cout << "This pass done.\n";
    return;
}

void aQC_plotRunGroup(string sIn)
{   
    gSystem->Exec("mkdir -p results/plotRunGroup/");
    string period;
    string pass[5];
    runList = readInput(sIn,period,pass[0],pass[1],pass[2],pass[3],pass[4]);
    nRuns = runList->size();

    for(int p = 0; p < 5; p++) { // loop over passes
        if(pass[p] != "none") {
            for(int h = 0; h < 5; h++) { // loop over histograms
                cout << "Processing " << pass[p] << ", " << histosNames[h] << "\n";
                plotRunGroup(period,pass[p],histosNames[h],histosTitles[h]);
            }
        }
    }

    cout << "Done.\n";
    return;
}