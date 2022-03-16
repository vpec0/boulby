#define ANATREE_CXX
#include "AnaTree.h"
#include "Constants.hh"

#define FOR(i, size) for (int i = 0; i < size; i++)

#include "common.icc"


TH2* createHist(const char* name,
		int nbinst, float tlow, float thigh, int nbinse, float elow, float ehigh)
{
    auto hET = new TH2F(Form("hET_%s", name), Form("%s;Tdep [#mus];Edep [MeV]",name),
			nbinst, tlow, thigh, nbinse, elow, ehigh);
    hET->SetDirectory(gROOT);
    doXlog(hET);
    doXlog(hET, 1);

    //tree->Draw(Form("%s>>+%s", formula, hET->GetName()), "", "colz");

    return hET;
}

void attachFiles(TChain* tree, const char* basedir, int batch, int nruns)
{
    TString path  = Form("%s/batch_%d", basedir, batch);
    FOR(i, nruns)
	tree->AddFile(Form("%s/%d/sim.root", path.Data(), batch+i));
}

void drawEdepVsTdep(const char* basedir, int batchNo, int Nruns, const char* outpref) {

    enum Detectors_t {
	kTpc = 0,
	kRfr,
	kSkin,
	kGdls,
	kWt
    };


    vector<TH2*> hists;
    hists.push_back(createHist("tpc_xe", 100, 0.001, 1e20, 100, 1e-9, 100.));
    hists.push_back(createHist("tpc_em", 100, 0.001, 1e32, 100, 1e-4, 1.e6));
    hists.push_back(createHist("tpc_mu", 100, 0.001, 1e2, 100, 1., 1.e4));
    hists.push_back(createHist("tpc_other", 100, 0.01, 1e32, 100, 1e-10, 1e-3));
    hists.push_back(createHist("rfr_xe", 100, 0.001, 1e20, 100, 1e-9, 100.));
    hists.push_back(createHist("rfr_em", 100, 0.001, 1e32, 100, 1e-4, 1.e6));
    hists.push_back(createHist("rfr_mu", 100, 0.001, 1e2, 100, 1., 1.e4));
    hists.push_back(createHist("rfr_other", 100, 0.01, 1e32, 100, 1e-10, 1e-3));
    hists.push_back(createHist("skin_xe", 100, 0.001, 1e20, 100, 1e-9, 100.));
    hists.push_back(createHist("skin_em", 100, 0.001, 1e32, 100, 1e-4, 1.e6));
    hists.push_back(createHist("skin_mu", 100, 0.001, 1e2, 100, 1., 1.e4));
    hists.push_back(createHist("skin_other", 100, 0.01, 1e32, 100, 1e-10, 1));
    hists.push_back(createHist("gdls_xe", 100, 0.001, 1e20, 100, 1e-9, 100.));
    hists.push_back(createHist("gdls_em", 100, 0.001, 1e32, 100, 1e-4, 1.e6));
    hists.push_back(createHist("gdls_mu", 100, 0.001, 1e2, 100, 1., 1.e4));
    hists.push_back(createHist("gdls_other", 100, 0.01, 1e32, 100, 1e-10, 1e5));
    hists.push_back(createHist("wt_xe", 100, 0.001, 1e20, 100, 1e-9, 100.));
    hists.push_back(createHist("wt_em", 100, 0.001, 1e32, 100, 1e-4, 1.e6));
    hists.push_back(createHist("wt_mu", 100, 0.001, 1e2, 100, 1., 1.e4));
    hists.push_back(createHist("wt_other", 100, 0.01, 1e32, 100, 1e-11, 1e5));


    // input tree
    auto tree = new TChain("events");
    size_t size = attachFiles(tree, "", batchNo, Nruns, 0, basedir);
    auto evt = new AnaTree::Event_t;
    AnaTree::resetEvent(*evt);
    AnaTree::registerTree(tree, *evt);

    cout<<"Number of files attached to the chain: "<<tree->GetNtrees()<<endl;


    // allow only a subset of branches
    std::vector<TString> allowed = {
    	"n_tpc",
    	"n_rfr",
    	"n_skin",
    	"n_gdls",
    	"n_wt",
    	"Edep_tpc",
    	"Edep_rfr",
    	"Edep_skin",
    	"Edep_gdls",
    	"Edep_wt",
    	"Tdep_tpc",
    	"Tdep_rfr",
    	"Tdep_skin",
    	"Tdep_gdls",
    	"Tdep_wt"
    };
    tree->SetBranchStatus("*", 0);
    for (auto allow: allowed)
    	tree->SetBranchStatus(allow, 1);


    double etemp = 0.;

    cout << "Will process "<<size<<" entries."<<endl;
    cout<<"|                                                  |\r|";
    cout.flush();
    // loop over tree entries
    size_t ientry = 0;
    size_t step = size / 50;
    while ( tree->GetEntry(ientry) && ientry < size ) {
	if (step && (ientry+1) % step == 0 ) {
	    cout<<"-";
	    cout.flush();
	}

		// loop over different sensitive detectors
	for (int i = 0; i < AnaTree::Ndetectors; ++i) {
	    int& n = *(&evt->n_tpc + i);
	    double* edep = (double*)evt->Edep_tpc + i*AnaTree::MAX_DEPOSITIONS*kNDepositionClasses;
	    double* tdep = (double*)evt->Tdep_tpc + i*AnaTree::MAX_DEPOSITIONS;

	    for ( int j = 0; j < n; ++j ) {
		// Sum all deposition types
		for ( int k = 0; k < kNDepositionClasses; ++k ) {
		    etemp = edep[j*kNDepositionClasses + k];
		    if (etemp == 0.) continue;
		    hists[i*kNDepositionClasses + k]->Fill(tdep[j], etemp);
		}
	    }

	}


	++ientry;
    } // tree while loop
    cout<<"|"<<endl;


    gStyle->SetTitleOffset(1.1,"x");
    gStyle->SetTitleOffset(1.3,"y");
    gStyle->SetNdivisions(505,"x");
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadBottomMargin(0.15);
    gStyle->SetOptStat(0);
    gStyle->SetNumberContours(255);


    auto c = new TCanvas("c", "", 1200, 1500);
    c->Divide(4,5, 0.001, 0.001);
    FOR(i, 20) {
	auto pad = c->cd(i+1);
	pad->SetLogx();
	pad->SetLogy();
	pad->SetLogz();

	hists[i]->Draw("colz");
    }


    c->SaveAs(Form("%sEdep_vs_Tdep.pdf", outpref));

    return;
}
