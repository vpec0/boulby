#ifndef PROPERCOINCTREE_H
#define PROPERCOINCTREE_H

namespace ProperCoincTree {

    struct Event_t {
	int run;
	int event;

	double tpc_deps[4]; //[Xe, EM, muon, other]
	double tpc_t1;
	double tpc_t2;

	double veto_deps[3]; // [skin,gdls,wt][Xe, EM, muon, other]
	double veto_t1[3];
	double veto_t2[3];
    };


    void createBranches(TTree* tree, Event_t* evt) {
	tree->Branch("run", &evt->run, "run/I");
	tree->Branch("event", &evt->event, "event/I");

	tree->Branch("tpc_deps", evt->tpc_deps, "tpc_deps[4]/D");
	tree->Branch("tpc_t1", &evt->tpc_t1, "tpc_t1/D");
	tree->Branch("tpc_t2", &evt->tpc_t2, "tpc_t2/D");

	tree->Branch("veto_deps", evt->veto_deps, "veto_deps[3]/D");
	tree->Branch("veto_t1", evt->veto_t1, "veto_t1[3]/D");
	tree->Branch("veto_t2", evt->veto_t2, "veto_t2[3]/D");
    }
}
#endif
