#define ANATREE_CXX
#include "AnaTree.h"
#include "common.icc"

void count_total_137Xe(const char* fname, int  batchNo = 0, int Nruns = 10,
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
	"runNo",
	"eventNo",
	"n137Xe"
    };
    tree->SetBranchStatus("*", 0);
    for (auto allow: allowed)
    	tree->SetBranchStatus(allow, 1);


    long int total_137xe = 0;

    // loop over tree entries
    size_t ientry = 0;
    while ( tree->GetEntry(ientry) && ientry < size ) {
	if (evt->n137Xe > 0) {
	    total_137xe += evt->n137Xe;
	    cout<<"137Xe: "<<evt->runNo<<" "<<evt->eventNo<<" "<<evt->n137Xe<<endl;
	}

	++ientry;
    } // tree while loop


    cout<<"Counted total "<<total_137xe<<" of 137Xe activated."<<endl;

}
