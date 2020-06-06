#define ANATREE_CXX
#include "AnaTree.h"
#include "Constants.hh"

#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

int attachFiles(TChain* tree, const char* fname, int batchNo, int Nruns,
		 const char* basedir = "data/full_geom_v0_4classes");

#include "common.icc"


void process_diagnostic_hists(STR fname, STR outpref, int batchNo = 2002000, int Nruns = 10,
			      const char* basedir = "data/full_geom_v0_4classes")
{
    // input tree
    auto tree = new TChain("events");
    size_t size = attachFiles(tree, fname, batchNo, Nruns, 0, basedir);
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
    	"Edep_tpc",
    	"Edep_rfr",
    	"Edep_skin",
    	"Edep_gdls",
    	"Edep_wt",
    	"Tdep_tpc",
    	"Tdep_rfr",
    	"Tdep_skin",
    	"Tdep_gdls",
    	"Tdep_wt",
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
	Tdep_tpc,
	Tdep_rfr,
	Tdep_skin,
	Tdep_gdls,
	Tdep_wt,
	Nhists,
    };
    TH1* hists[Nhists] = {};
    TH1* hists_byClass[Nhists][kNDepositionClasses] = {};

    const char* names[] = {"TPC", "RFR", "Skin", "GdLS", "WT",
			   "Tdep_TPC", "Tdep_RFR", "Tdep_Skin", "Tdep_GdLS", "Tdep_WT"
    };


#define H1(name) hists[name] = new TH1D(#name, Form("Number of energy deposits in %s clustered in 100 ns" \
						    "; Number of deposits", names[name]), 260, 0, 260) \

    H1(Ntpc);
    H1(Nrfr);
    H1(Nskin);
    H1(Ngdls);
    H1(Nwt);
#undef H1

#define H1(name) hists[name] = new TH1D(#name, Form("Times of energy depositions in %s clustered in 100 ns" \
						    "; Time [#mus]", names[name-Tdep_tpc]), 200, 0.01, 1e8); \
    doXlog(hists[name])							\

    H1(Tdep_tpc);
    H1(Tdep_rfr);
    H1(Tdep_skin);
    H1(Tdep_gdls);
    H1(Tdep_wt);
#undef H1

    // Tdeps by deposition type
#define H1(depclass, name) hists_byClass[name - Tdep_tpc][depclass] =		\
	new TH1D(Form("%s_%s", #name, DepositionClassNames[depclass]),	\
		 Form("Times of energy depositions by %s in %s clustered in 100 ns" \
		      "; Time [#mus]", DepositionClassNames[depclass],	\
		      names[name-Tdep_tpc]), 200, 0.01, 1e8);		\
    doXlog(hists_byClass[name - Tdep_tpc][depclass])			\

    FOR(i, kNDepositionClasses) {
	H1(i, Tdep_tpc);
	H1(i, Tdep_rfr);
	H1(i, Tdep_skin);
	H1(i, Tdep_gdls);
	H1(i, Tdep_wt);
    }
#undef H1


    //**************************************************************************

    // output file
    auto outf = TFile::Open(Form("%sdiagnostic_hists.root", outpref), "UPDATE");

    // main loop
#ifdef DEBUG
    size = 20; // just for debugging
#endif
    //size = 1000000;
    size_t step = size / 50;

    cout << "Will process "<<size<<" entries."<<endl;
    cout<<"|                                                  |\r|";
    cout.flush();
    // loop over tree entries
    size_t ientry = 0;
    while ( tree->GetEntry(ientry) ) {
	if (step && (ientry+1) % step == 0 ) {
	    cout<<"-";
	    cout.flush();
	}


	for (int i = Ntpc; i < Tdep_tpc; i++) {
	    size_t n = (&evt->n_tpc)[i];
	    hists[i]->Fill( n );
	    Double_t* tdeps = (&evt->Tdep_tpc)[i];
	    Double_t* edeps = (Double_t*)(&evt->Edep_tpc)[i];
	    for (int j = 0; j < n; ++j) {
		hists[i + Tdep_tpc]->Fill(tdeps[j]);
		for (int k = 0; k < kNDepositionClasses; ++k) {
		    if (edeps[j*kNDepositionClasses + k] > 0.)
			hists_byClass[i][k]->Fill(tdeps[j]);
		}
	    }
	}
	++ientry;
    }
    cout<<"|"<<endl;

    // write out output histograms into the output file
    FOR (i, Nhists) {
	auto h = hists[i];
	h->Write(h->GetName(), TObject::kOverwrite);
    }
    FOR (i, Nhists-Tdep_tpc) {
	FOR (j, kNDepositionClasses) {
	    auto h = hists_byClass[i][j];
	    h->Write(h->GetName(), TObject::kOverwrite);
	}
    }

    cout<<"Saving histograms in "<<outf->GetName()<<endl;
    outf->Close();
}
