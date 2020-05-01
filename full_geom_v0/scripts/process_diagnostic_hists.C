#define ANATREE_CXX
#include "AnaTree.h"
#include "Constants.hh"

#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

void attachFiles(TChain* tree, const char* fname, int batchNo, int Nruns,
		 const char* basedir = "data/full_geom_v0_4classes");

void process_diagnostic_hists(STR fname, STR outpref, int batchNo = 4, int Nruns = 10,
			      const char* basedir = "data/full_geom_v0_4classes")
{
    // input tree
    auto tree = new TChain("events");
    attachFiles(tree, fname, batchNo, Nruns, basedir);
    auto evt = new AnaTree::Event_t;
    AnaTree::resetEvent(*evt);
    AnaTree::registerTree(tree, *evt);


    // allow only a subset of branches
    std::vector<TString> allowed = {
    	"n_tpc",
    	"n_rfr",
    	"n_skin",
    	"n_gdls",
    	"n_wt",
    };
    tree->SetBranchStatus("*", 0);
    for (auto allow: allowed)
    	tree->SetBranchStatus(allow, 1);



    //**************************************************************************
    // output hists

    // hists of number of depositions
    enum {
	Ntpc = 0,
	Nrfr,
	Nskin,
	Ngdls,
	Nwt,
	Nhists,
    };
    TH1* hists[Nhists] = {};

    const char* names[] = {"TPC", "RFR", "Skin", "GdLS", "WT"};


#define H1(name) hists[name] = new TH1F(#name, Form("Number of energy deposits in %s clustered in 100 ns" \
						    "; Number of deposits", names[name]), 100, 0, 100);	\

    H1(Ntpc);
    H1(Nrfr);
    H1(Nskin);
    H1(Ngdls);
    H1(Nwt);

    //**************************************************************************

    // output file
    auto outf = TFile::Open(Form("%sdiagnostic_hists.root", outpref), "UPDATE");

    // main loop
    unsigned int size = tree->GetEntries();

#ifdef DEBUG
    size = 20; // just for debugging
#endif
    //size = 1000000;
    int step = size / 50;

    cout << "Will process "<<size<<" entries."<<endl;
    cout<<"|                                                  |\r|";
    cout.flush();
    // loop over tree entries
    FOR(ientry, size) {
	if (step && (ientry+1) % step == 0 ) {
	    cout<<"-";
	    cout.flush();
	}
	tree->GetEntry(ientry);

	for (int i = Ntpc; i < Nhists; i++) {
	    hists[i]->Fill( (&evt->n_tpc)[i] );
	}
    }
    cout<<"|"<<endl;

    // write out output histograms into the output file
    FOR (i, Nhists) {
	auto h = hists[i];
	h->Write(h->GetName(), TObject::kOverwrite);
    }

    cout<<"Saving histograms in "<<outf->GetName()<<endl;
    outf->Close();
}



void attachFiles(TChain* tree, const char* fname, int batchNo, int Nruns, const char* basedir) {
    if (!strcmp(fname, "")) { // no input given
        cout<<"Will add production files from batch "
            <<batchNo<<" to the chain (unchecked)."<<endl;
        TString batch = Form("200%02d", batchNo);
        TString topdir = Form("%s/batch_", basedir);
        topdir += batch + "00/";
        for (int j = 0; j<Nruns; j++) {
            TString fname = topdir +
                Form("200%04d/sim.root", j + batchNo*100);
            int status = tree->Add(fname, -1);
        }
    } else { // input given
        cout<<"Adding "<<fname<<" to the chain."<<endl;
        int status = tree->Add(fname, -1);
        if (!status) { // try to look in the base dir
            tree->SetName("anatree");
            status = tree->Add(fname, -1);
        }
        cout<<"Status: "<<status<<endl;
    }
}
