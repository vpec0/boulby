/*************************************************************
 * Processes simulated events. Takes into account proper coincidence,
 * and isolation of events.
 *
 * Skin, GdLS and WT are searched for anti-coincidence in a window of
 * 500 us before and 500 us after.
 *
 **************************************************************/


#define ANATREE_CXX

#include "AnaTree.h"
#include "Constants.hh"
#include "ProperCoincTree.h"


#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

//void doXlog(TH1*, int which = 0);

#include "common.icc"

enum {
    kTpc=0,
    kRfr,
    kSkin,
    kGdls,
    kWt,
    kNdetectors
};


struct deposition_t {
    float T1;
    float T2;
    float E;
};

typedef vector<deposition_t> deposition_vector;

void fillDepositionVector(AnaTree::Event_t* evt, int detector, deposition_vector *tpc_deps, double window_us, int class_separately = 0);
void infoPrintouts(int, AnaTree::Event_t* , deposition_vector* , deposition_vector& , deposition_vector& , deposition_vector& );
vector<int> getIsolatedDepositions(deposition_vector*);
deposition_t testOtherDep(deposition_vector&, double, double, double, double);
void printDepositions(deposition_vector& deps);
void printDeposition(deposition_t& dep);


void process_proper_coinc_simple_veto1us(const char* basedir = "data/full_geom_v0_4classes",
					 int batchNo = 4, int Nruns = 10,
					 const char* outfname = "")
{

    // input tree
    auto tree = new TChain("events");
    size_t size = attachFiles(tree, "", batchNo, Nruns, 0, basedir);
    auto evt = new AnaTree::Event_t;
    AnaTree::resetEvent(*evt);
    AnaTree::registerTree(tree, *evt);

    cout<<"Number of files attached to the chain: "<<tree->GetNtrees()<<endl;


    // output tree
    TString oname = outfname;
    if (oname == "")
	oname = "test_proper_coincidence.root";
    auto outf = TFile::Open(oname, "recreate");
    auto outtree = new TTree("proper_coincidence","");
    auto outevt = new ProperCoincTree::Event_t;
    ProperCoincTree::createBranches(outtree, outevt);
    ProperCoincTree::Event_t empty_evt = {};

    // const double	skin_veto = 0.1;	// 100 keV
    // double gdls_veto		  = 0.2;	// 200 keV, default LS treated as LS
    // const double	wt_veto	  = 200.;	// 200 MeV
    // if (strstr("LSoff", LS))
    // 	gdls_veto = wt_veto;	// 200 MeV, treat LS as WT

    // cout<<"Using following threshold for veto detectors:"<<endl
    // 	<<"  Skin: "<<skin_veto<<endl
    // 	<<"  GdLS: "<<gdls_veto<<endl
    // 	<<"    WT: "<<wt_veto<<endl;



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

    // main loop

#ifdef DEBUG
    size = 20; // just for debugging
    size = 4000;
#endif
    size_t step = size / 50;

    cout << "Will process "<<size<<" entries."<<endl;
    cout<<"|                                                  |\r|";
    cout.flush();
    // loop over tree entries
    size_t ientry = 0;
    while ( tree->GetEntry(ientry) && ientry < size ) {
	if (step && (ientry+1) % step == 0 ) {
	    cout<<"-";
	    cout.flush();
	}


	// group depositions in all detectors (by deposition class in TPC, sum everywhere else
	deposition_vector tpc_deps[4];
	deposition_vector skin_deps;
	deposition_vector gdls_deps;
	deposition_vector wt_deps;

	fillDepositionVector(evt, kTpc, tpc_deps, 1000., 1); // sum over 1 ms
	fillDepositionVector(evt, kSkin, &skin_deps, 1., 0); // sum over 1 us
	fillDepositionVector(evt, kGdls, &gdls_deps, 1., 0); // sum over 1 us
	fillDepositionVector(evt, kWt, &wt_deps, 1., 0); // sum over 1 us


	const double before = 500.; // veto window before signal; us
	const double after = 500.; // veto window after signal
	for (size_t idep = 0; idep < tpc_deps[0].size(); idep++) {
	    auto tpc_dep = tpc_deps[0][idep];
	    if (tpc_dep.E < 1e-3)
		continue;

	    // check if signal in other parts
	    deposition_t depSkin = testOtherDep(skin_deps, tpc_dep.T1, tpc_dep.T2, before, after);
	    deposition_t depGdls = testOtherDep(gdls_deps, tpc_dep.T1, tpc_dep.T2, before, after);
	    deposition_t depWt = testOtherDep(wt_deps, tpc_dep.T1, tpc_dep.T2, before, after);

	    outevt->run = evt->runNo;
	    outevt->event = evt->eventNo;
	    for (int i = 0; i < 4; i++) {
		outevt->tpc_deps[i] = tpc_deps[i][idep].E;
	    }
	    outevt->tpc_t1 = tpc_dep.T1;
	    outevt->tpc_t2 = tpc_dep.T2;

	    outevt->veto_deps[0] = depSkin.E;
	    outevt->veto_deps[1] = depGdls.E;
	    outevt->veto_deps[2] = depWt.E;

	    outevt->veto_t1[0] = depSkin.T1;
	    outevt->veto_t1[1] = depGdls.T1;
	    outevt->veto_t1[2] = depWt.T1;
	    outevt->veto_t2[0] = depSkin.T2;
	    outevt->veto_t2[1] = depGdls.T2;
	    outevt->veto_t2[2] = depWt.T2;

#ifdef DEBUG
	    cout<<idep<<": Depositions in other detectors:"<<endl;
	    cout<<"  Skin: "; printDeposition(depSkin);
	    cout<<"  Gdls: "; printDeposition(depGdls);
	    cout<<"  Wt: "; printDeposition(depWt);
#endif

	    outtree->Fill();
	    *outevt = empty_evt;
	}
	++ientry;
    } // tree while loop
    cout<<"|"<<endl;

    outf->Write();
    outf->Close();
}

void fillDepositionVector(AnaTree::Event_t* evt, int detector, deposition_vector *deps, double window_us, int class_separately)
{
    int& n = *(&evt->n_tpc + detector);
    if (n == 0)
	return;
    double* edep = (double*)evt->Edep_tpc + detector*AnaTree::MAX_DEPOSITIONS*kNDepositionClasses;
    double* tdep = (double*)evt->Tdep_tpc + detector*AnaTree::MAX_DEPOSITIONS;

    // loop over number of depositions within 1st us
    double stop_looking = tdep[0] + window_us; // stop looking 1 ms after first deposition
    const deposition_t empty = {};
    deposition_t tmp[kNDepositionClasses] = {};
    deposition_t tmp_sum = {};

    tmp_sum.T1 = tdep[0];
    for ( int j = 0; j < n; ++j ) {
	// if (tdep[j] > 1.e6)
	//     break;// don't store depositions 1 second after muon

	if (tdep[j] > stop_looking) {
	    // write depositions
	    if (tmp_sum.E > 0.) {

		if (class_separately) {
		    FOR(i, 4) {
			tmp[i].T1 = tmp_sum.T1;
			tmp[i].T2 = tmp_sum.T2;

			deps[i].push_back(tmp[i]);
		    }
		} else {
		    deps->push_back(tmp_sum);
		}
	    }

	    // clear tmp deposition
	    FOR(i, kNDepositionClasses) tmp[i] = empty;
	    tmp_sum = empty;
	    // set first time
	    tmp_sum.T1 = tdep[j];
	    stop_looking = tdep[j] + window_us;
	}

	// within the window, add deposition and update last dep. time
	tmp_sum.T2 = tdep[j];

	FOR(i, kNDepositionClasses) {
	    tmp[i].E += edep[j*kNDepositionClasses + i];
	    tmp_sum.E += edep[j*kNDepositionClasses + i];
	}
    }

    // finish job if there is unstored deposition
    if (tmp_sum.E > 0.) {
	if (class_separately) {
	    FOR(i, 4) {
		tmp[i].T1 = tmp_sum.T1;
		tmp[i].T2 = tmp_sum.T2;

		deps[i].push_back(tmp[i]);
	    }
	} else {
	    deps->push_back(tmp_sum);
	}
    }

}


void infoPrintouts(int ientry,
		   AnaTree::Event_t* evt,
		   deposition_vector* tpc_deps,
		   deposition_vector& skin_deps,
		   deposition_vector& gdls_deps,
		   deposition_vector& wt_deps)
{
    //if (evt->n_tpc < 2 ) return;
    cout<<endl;
    cout<<"===> Run: "<<evt->runNo<<" Event "<<evt->eventNo<<" Entry: "<<ientry<<" <==="<<endl
	<<"deposition counts:"<<endl
	<<"  TPC: "<<tpc_deps[0].size()<<endl
	<<"  Skin: "<<skin_deps.size()<<endl
	<<"  GdLS: "<<gdls_deps.size()<<endl
	<<"  WT: "<<wt_deps.size()<<endl;
    // print depositions in the event and then depositions in the vector

    cout<<"TPC"<<endl;
    cout<<"     Tdep      Edep0      Edep1      Edep2      Edep3"<<endl;
    FOR(j, evt->n_tpc) {
	printf(" %9.5g",evt->Tdep_tpc[j]);
	FOR(i, kNDepositionClasses) {
	    printf(" %9.5g",evt->Edep_tpc[j][i]);
	}
	cout<<endl;
    }

    cout<<"       T1        T2    Edep0      Edep1      Edep2      Edep3"<<endl;
    FOR(j, tpc_deps[0].size()) {
	printf(" %9.4g", tpc_deps[0][j].T1);
	printf(" %9.4g", tpc_deps[0][j].T2);
	FOR(i, kNDepositionClasses) {
	    printf(" %9.4g",tpc_deps[i][j].E);
	}
	cout<<endl;
    }
    cout<<endl;

}


deposition_t testOtherDep(deposition_vector& deps, double t1, double t2, double before, double after)
{
    // see if any depositions within the time before and after the T1 and T2, respectively
    deposition_t result = {};
    result.T1 = 1e10;
    result.T2 = -1e10;
    for (auto dep: deps) {
	if ( dep.T1 < (t2+after) && dep.T2 > (t1-before) ) {
	    result.E += dep.E;
	    if (dep.T1 < result.T1)
		result.T1 = dep.T1;
	    if (dep.T2 > result.T2)
		result.T2 = dep.T2;
	}
    }

    return result;
}


void printDeposition(deposition_t& dep)
{
    cout<<"  "<<dep.T1<<".."<<dep.T2<<": "<<dep.E<<endl;
}

void printDepositions(deposition_vector& deps)
{
    for (auto dep: deps)
	printDeposition(dep);
}
