/*******************************************************************************
 * New version of the event structure:
 * - adding multiple veto signals (will sum depositions over 1 us and
 *     record all 1 us depositions over -1 s .. 10 s)
 *******************************************************************************/

#ifndef PROPERCOINCTREE_V1_H
#define PROPERCOINCTREE_V1_H

namespace ProperCoincTree {

    const int MAX_VETO_DEPS = 1024;

    struct Event_t {
	int run;
	int event;

	double tpc_deps[4]; //[Xe, EM, muon, other]
	double tpc_t1; // in us
	double tpc_t2;

	int n_veto_deps[3];
	double veto_deps[3][MAX_VETO_DEPS]; // [skin,gdls,wt]
	double veto_t1[3][MAX_VETO_DEPS]; // in us
	double veto_t2[3][MAX_VETO_DEPS];
    };


    void createBranches(TTree* tree, Event_t* evt) {
	tree->Branch("run", &evt->run, "run/I");
	tree->Branch("event", &evt->event, "event/I");

	tree->Branch("tpc_deps", evt->tpc_deps, "tpc_deps[4]/D");
	tree->Branch("tpc_t1", &evt->tpc_t1, "tpc_t1/D");
	tree->Branch("tpc_t2", &evt->tpc_t2, "tpc_t2/D");

	tree->Branch("n_veto_deps", evt->n_veto_deps, "n_veto_deps[3]/I");
	tree->Branch("veto_deps", evt->veto_deps, Form("veto_deps[3][%d]/D", MAX_VETO_DEPS));
	tree->Branch("veto_t1", evt->veto_t1, Form("veto_t1[3][%d]/D", MAX_VETO_DEPS));
	tree->Branch("veto_t2", evt->veto_t2, Form("veto_t2[3][%d]/D", MAX_VETO_DEPS));
    }

    void registerBranches(TTree* tree, Event_t* evt) {
	tree->SetBranchAddress("run", &evt->run);
	tree->SetBranchAddress("event", &evt->event);

	tree->SetBranchAddress("tpc_deps", evt->tpc_deps);
	tree->SetBranchAddress("tpc_t1", &evt->tpc_t1);
	tree->SetBranchAddress("tpc_t2", &evt->tpc_t2);

	tree->SetBranchAddress("n_veto_deps", evt->n_veto_deps);
	tree->SetBranchAddress("veto_deps", evt->veto_deps);
	tree->SetBranchAddress("veto_t1", evt->veto_t1);
	tree->SetBranchAddress("veto_t2", evt->veto_t2);
    }
}
#endif
