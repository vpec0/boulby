/**
 * This does not seem to be finished
 *
 * */

#define ANATREE_CXX
#include "AnaTree.h"
#include "Constants.hh"

#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

//void doXlog(TH1*, int which = 0);

#include "common.icc"

void process_timing(STR fname, STR outpref, int batchNo = 4, int Nruns = 10,
		    const char* basedir = "data/full_geom_v0_4classes")
{
    // input tree
    auto tree = new TChain("events");
    size_t size = attachFiles(tree, fname, batchNo, Nruns, 0, basedir);
    auto evt = new AnaTree::Event_t;
    AnaTree::resetEvent(*evt);
    AnaTree::registerTree(tree, *evt);


    const double	skin_veto = 0.1;	// 100 keV
    double gdls_veto		  = 0.2;	// 200 keV, default LS treated as LS
    const double	wt_veto	  = 200.;	// 200 MeV
    if (strstr("LSoff", LS))
	gdls_veto = wt_veto;	// 200 MeV, treat LS as WT

    cout<<"Using following threshold for veto detectors:"<<endl
	<<"  Skin: "<<skin_veto<<endl
	<<"  GdLS: "<<gdls_veto<<endl
	<<"    WT: "<<wt_veto<<endl;



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



    //**************************************************************************
    // output hists

    enum {
	T0dep,
	DT10dep,

	Nhists,
    };


    TH1* hists[Nhists] = {};

#define H1(name, title) hists[name] = new TH1D(#name,			\
					       Form("%s;Time [#mus]",	\
						    title), 400, 0, 10) \


    H1(T0dep, "T0");
    H1(DT10dep, "#DeltaT_{10}");

    //**************************************************************************



    // output file
    auto outf = TFile::Open(Form("%shists.root", outpref), "UPDATE");

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


	double etot[EDepNhists] = {};
	double tdepfirst[EDepNhists] = {};
	double etot_class[EDepNhists][kNDepositionClasses] = {};
	// loop over different sensitive detectors
	for (int i = 0; i < AnaTree::Ndetectors; ++i) {
	    int& n = *(&evt->n_tpc + i);
	    double* edep = (double*)evt->Edep_tpc + i*AnaTree::MAX_DEPOSITIONS*kNDepositionClasses;
	    double* tdep = (double*)evt->Tdep_tpc + i*AnaTree::MAX_DEPOSITIONS;

	    // loop over number of depositions within 1st us
	    double stop_looking = tdep[0] + 1.; // stop looking 1 us after first deposition
	    for ( int j = 0; j < n; ++j ) {
		if (tdep[j] > stop_looking) break;
		// Sum all deposition types
		for ( int k = 0; k < kNDepositionClasses; ++k ) {
		    etot[i] += edep[j*kNDepositionClasses + k];
		    etot_class[i][k] += edep[j*kNDepositionClasses + k];
		}
	    }
	    tdepfirst[i] = tdep[0];

	    if(etot[i] > 0.)
		edephists[i]->Fill(etot[i]);
	}


	if (etot[EdepTpc] > 0.) {
	    if (etot[EdepSkin] < skin_veto) {
		edephists[EdepTpcNoSkin]->Fill(etot[EdepTpc]);
		if (etot[EdepGdls] < gdls_veto) {
		    edephists[EdepTpcNoSkinGdls]->Fill(etot[EdepTpc]);
		    //cout<<"Filling EdepTpcNoSkinGdls: "<<etot[EdepTpc]<<endl;
		    if (etot[EdepWt] < wt_veto) {
			edephists[EdepTpcNoSkinGdlsWt]->Fill(etot[EdepTpc]);
		    }
		}
		if (etot[EdepWt] < wt_veto) {
		    edephists[EdepTpcNoSkinWt]->Fill(etot[EdepTpc]);
		}
	    }

	    if (etot[EdepGdls] < gdls_veto) {
		edephists[EdepTpcNoGdls]->Fill(etot[EdepTpc]);
		if (etot[EdepWt] < wt_veto) {
		    edephists[EdepTpcNoGdlsWt]->Fill(etot[EdepTpc]);
		}
	    }

	    if (etot[EdepWt] < wt_veto) {
		edephists[EdepTpcNoWt]->Fill(etot[EdepTpc]);
	    }
	}

	// only Xe doposition in TPC
	if (etot_class[EdepTpc][kEm]+etot_class[EdepTpc][kMu]+etot_class[EdepTpc][kOther] == 0.) {
	    //no other depositions
	    if (etot_class[EdepTpc][kXe] > 0.) {
		hists_tpc_xeonly[EdepTpc]->Fill(etot_class[EdepTpc][kXe]);
		if (etot[EdepSkin] < skin_veto) {
		    hists_tpc_xeonly[EdepTpcNoSkin]->Fill(etot_class[EdepTpc][kXe]);
		    if (etot[EdepGdls] < gdls_veto) {
			hists_tpc_xeonly[EdepTpcNoSkinGdls]->Fill(etot_class[EdepTpc][kXe]);
			if (etot[EdepWt] < wt_veto) {
			    hists_tpc_xeonly[EdepTpcNoSkinGdlsWt]->Fill(etot_class[EdepTpc][kXe]);
			}
		    }
		    if (etot[EdepWt] < wt_veto) {
			hists_tpc_xeonly[EdepTpcNoSkinWt]->Fill(etot_class[EdepTpc][kXe]);
		    }
		}

		if (etot[EdepGdls] < gdls_veto) {
		    hists_tpc_xeonly[EdepTpcNoGdls]->Fill(etot_class[EdepTpc][kXe]);
		    if (etot[EdepWt] < wt_veto) {
			hists_tpc_xeonly[EdepTpcNoGdlsWt]->Fill(etot_class[EdepTpc][kXe]);
		    }
		}

		if (etot[EdepWt] < wt_veto) {
		    hists_tpc_xeonly[EdepTpcNoWt]->Fill(etot_class[EdepTpc][kXe]);
		}
	    }
	}

	FOR(i, kNDepositionClasses) {
	    for (int j = 0; j < AnaTree::Ndetectors; ++j) {
		if(etot_class[j][i] > 0.)
		    hists[j][i]->Fill(etot_class[j][i]);
	    }

	    if (etot_class[EdepTpc][i] > 0.) {
		if (etot[EdepSkin] < skin_veto) {
		    hists[EdepTpcNoSkin][i]->Fill(etot_class[EdepTpc][i]);
		    if (etot[EdepGdls] < gdls_veto) {
			hists[EdepTpcNoSkinGdls][i]->Fill(etot_class[EdepTpc][i]);
			if (etot[EdepWt] < wt_veto) {
			    hists[EdepTpcNoSkinGdlsWt][i]->Fill(etot_class[EdepTpc][i]);
			}
		    }
		    if (etot[EdepWt] < wt_veto) {
			hists[EdepTpcNoSkinWt][i]->Fill(etot_class[EdepTpc][i]);
		    }
		}

		if (etot[EdepGdls] < gdls_veto) {
		    hists[EdepTpcNoGdls][i]->Fill(etot_class[EdepTpc][i]);
		    if (etot[EdepWt] < wt_veto) {
			hists[EdepTpcNoGdlsWt][i]->Fill(etot_class[EdepTpc][i]);
		    }
		}

		if (etot[EdepWt] < wt_veto) {
		    hists[EdepTpcNoWt][i]->Fill(etot_class[EdepTpc][i]);
		}
	    }
	}

	++ientry;
    } // tree while loop
    cout<<"|"<<endl;

    // write out output histograms into the output file
    FOR (i, EDepNhists) {
	auto h = edephists[i];
	h->Write(h->GetName(), TObject::kOverwrite);
    }

    for (int i = 0; i < EDepNhists; ++i) {
	FOR(j, kNDepositionClasses) {
	    auto h = hists[i][j];
	    h->Write(h->GetName(), TObject::kOverwrite);
	}
    }

    auto h = hists_tpc_xeonly[EdepTpc];
    h->Write(h->GetName(), TObject::kOverwrite);
    for (int i = EdepTpcNoSkin; i < EDepNhists; ++i) {
	auto h = hists_tpc_xeonly[i];
	h->Write(h->GetName(), TObject::kOverwrite);
    }



    cout<<"Saving histograms in "<<outf->GetName()<<endl;
    outf->Close();
}


// void attachFiles(TChain* tree, const char* fname, int batchNo, int Nruns) {
//     if (!strcmp(fname, "")) { // no input given
//         cout<<"Will add production files from batch "
//             <<batchNo<<" to the chain (unchecked)."<<endl;
//         TString batch = Form("200%02d", batchNo);
//         TString topdir = "data/full_geom_v0_4classes/batch_";
//         topdir += batch + "00/";
//         for (int j = 0; j<Nruns; j++) {
//             TString fname = topdir +
//                 Form(batch + "%02d/sim.root", j);
//             int status = tree->Add(fname, -1);
//         }
//     } else { // input given
//         cout<<"Adding "<<fname<<" to the chain."<<endl;
//         int status = tree->Add(fname, -1);
//         if (!status) { // try to look in the base dir
//             tree->SetName("anatree");
//             status = tree->Add(fname, -1);
//         }
//         cout<<"Status: "<<status<<endl;
//     }
// }
