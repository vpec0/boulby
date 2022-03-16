#define ANATREE_CXX
#include "AnaTree.h"
#include "Constants.hh"

#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

#include "common.icc"

//#define DEBUG

void print_bkgevents_new(STR fname, int batchNo = 2002000, int Nruns = 10, int startrun = 0,
			 const char* LS = "LSon",
			 const char* basedir = "data/full_geom_v0_nacl_4classes")
{

    // input tree
    auto tree = new TChain("events");
    size_t size = attachFiles(tree, fname, batchNo, Nruns, startrun, basedir);
    auto evt = new AnaTree::Event_t;
    AnaTree::resetEvent(*evt);
    AnaTree::registerTree(tree, *evt);


    double skin_veto = 0.1;	// 100 keV
    double gdls_veto = 0.2;	// 200 keV, default LS treated as LS
    double wt_veto   = 200.;	// 200 MeV
    if (strstr("LSoff", LS))
	gdls_veto = wt_veto;	// 200 MeV, treat LS as WT

#ifdef DEBUG
    skin_veto = 1000;
    gdls_veto = 1000;
    wt_veto   = 10000;
#endif

    cout<<"Using following threshold for veto detectors:"<<endl
	<<"  Skin: "<<skin_veto<<endl
	<<"  GdLS: "<<gdls_veto<<endl
	<<"    WT: "<<wt_veto<<endl;



    // allow only a subset of branches
    std::vector<TString> allowed = {
	"runNo",
	"eventNo",
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

    // Energy deposit hists
    enum {
	EdepTpc = 0,
	EdepRfr,
	EdepSkin,
	EdepGdls,
	EdepWt,

	EdepTpcNoSkin,
	EdepTpcNoSkinGdls,
	EdepTpcNoSkinGdlsWt,
	EdepTpcNoSkinWt,
	EdepTpcNoGdls,
	EdepTpcNoGdlsWt,
	EdepTpcNoWt,

	EDepNhists,
    };
    //**************************************************************************



    // containers to store run and event numbers, amount of energy stored by only Xe in TPC, and when it occured
    vector<int> run_full;
    vector<int> event_full;
    vector<float> edep_full;
    vector<float> tdep_full;
    vector<float> edep_skin_full;
    vector<float> edep_gdls_full;
    vector<float> edep_wt_full;

    vector<int> run_nogdls;
    vector<int> event_nogdls;
    vector<float> edep_nogdls;
    vector<float> tdep_nogdls;
    vector<float> edep_skin_nogdls;
    vector<float> edep_gdls_nogdls;
    vector<float> edep_wt_nogdls;


    // output file
    // auto outf = TFile::Open(Form("%shists.root", outpref), "UPDATE");

    // main loop

#ifdef DEBUG
    size = 1000000; // just for debugging
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

#ifdef DEBUG
	if (ientry >= size) break;
#endif

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
	}

	// only Xe doposition in TPC
	if (etot_class[EdepTpc][kEm]+etot_class[EdepTpc][kMu]+etot_class[EdepTpc][kOther] == 0.) {
	    //no other depositions
	    if (etot_class[EdepTpc][kXe] > 0.) {
		if (etot[EdepSkin] < skin_veto) {
		    if (etot[EdepWt] < wt_veto) {
			if (etot[EdepGdls] < gdls_veto) {
			    // full veto
			    run_full.push_back(evt->runNo);
			    event_full.push_back(evt->eventNo);
			    edep_full.push_back(etot_class[EdepTpc][kXe]);
			    tdep_full.push_back(tdepfirst[EdepTpc]);
			    edep_skin_full.push_back(etot[EdepSkin]);
			    edep_gdls_full.push_back(etot[EdepGdls]);
			    edep_wt_full.push_back(etot[EdepWt]);
			} else if ( etot[EdepWt] + etot[EdepGdls] < wt_veto ) {
			    run_nogdls.push_back(evt->runNo);
			    event_nogdls.push_back(evt->eventNo);
			    edep_nogdls.push_back(etot_class[EdepTpc][kXe]);
			    tdep_nogdls.push_back(tdepfirst[EdepTpc]);
			    edep_skin_nogdls.push_back(etot[EdepSkin]);
			    edep_gdls_nogdls.push_back(etot[EdepGdls]);
			    edep_wt_nogdls.push_back(etot[EdepWt]);
			}
		    }
		}
	    }
	}


	++ientry;
    } // tree while loop
    cout<<"|"<<endl;

    cout<<"Processed "<<ientry<<" entries"<<endl;


    cout<<"Run Event E_TPC_Xe [MeV] TPC_T [us] E_Skin E_LS E_WT"<<endl;

    cout<<"Full veto:"<<endl;
    auto run = run_full.begin();
    auto event = event_full.begin();
    auto edep = edep_full.begin();
    auto tdep = tdep_full.begin();
    auto skin = edep_skin_full.begin();
    auto gdls = edep_gdls_full.begin();
    auto wt = edep_wt_full.begin();
    for (;run != run_full.end(); run++, event++, edep++, tdep++, skin++, gdls++, wt++) {
	cout<< *run <<" "<< *event
	    <<" "<< *edep
	    <<" "<< *tdep
	    <<" "<< *skin
	    <<" "<< *gdls
	    <<" "<< *wt
	    <<endl;
    }


    cout<<"No LS veto:"<<endl;
    run = run_nogdls.begin();
    event = event_nogdls.begin();
    edep = edep_nogdls.begin();
    tdep = tdep_nogdls.begin();
    skin = edep_skin_nogdls.begin();
    gdls = edep_gdls_nogdls.begin();
    wt = edep_wt_nogdls.begin();
    for (;run != run_nogdls.end(); run++, event++, edep++, tdep++, skin++, gdls++, wt++) {
	cout<< *run <<" "<< *event
	    <<" "<< *edep
	    <<" "<< *tdep
	    <<" "<< *skin
	    <<" "<< *gdls
	    <<" "<< *wt
	    <<endl;
    }
}
