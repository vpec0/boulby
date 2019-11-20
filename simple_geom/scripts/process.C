#define ANATREE_CXX
#include "AnaTree.h"


#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

void process(STR fname, STR outpref)
{
    // input tree
    auto tree = new TChain("events");
    int status = tree->Add(fname);
    if (!status) {
	cerr<< "Can't get events tree from file "<<fname<<endl;
	return;
    }
    auto evt = new AnaTree::Event_t;
    AnaTree::registerTree(tree, *evt);


    // allow only a subset of branches
    std::vector<TString> allowed = {
	"n_neutrons",
	"n_energy",
    };
    tree->SetBranchStatus("*", 0);
    for (auto allow: allowed)
	tree->SetBranchStatus(allow, 1);



    // output hists
    std::map<TString, TH1*> hists;
    auto hE = new TH1I("energy","", 100, 0,-1);
    hists["energy"] = hE;
    auto hN = new TH1I("ncount","", 100, 0,-1);
    hists["ncount"] = hN;


    // main loop
    unsigned int size = tree->GetEntries();

    cout << "Will process "<<size<<" entries."<<endl;
    FOR(ientry, size) {
	tree->GetEntry(ientry);

	int neutron_count = 0;
	std::map<int, std::vector<double> > stack;
	FOR(i, evt->n_neutrons) {
	    if (i > AnaTree::MAX_NEUTRONS) break;

	    // see if parent is a neutron
	    int parent_is_neutron = 0;
	    FOR(j, evt->n_neutrons) {
		if (i > AnaTree::MAX_NEUTRONS) break;
		if ( evt->parentId[i] != evt->trackId[j] )
		    continue;
		// its parent IS a neutron
		// store this neutron in a list of the parents descendants and store its energy
		stack[evt->trackId[j]].push_back(evt->n_energy[i]);
		parent_is_neutron = 1;
		break; // stop looking for the parent
	    }

	    if (!parent_is_neutron) {
		// store this neutrons info immediately
		hE->Fill(evt->n_energy[i]);
		neutron_count++;
	    }
	}

	// loop over stacked neutrons which have neutrons as their parents
	for (auto& list: stack) {
	    neutron_count += list.second.size()-1;
	    // find maximum energy element
	    auto it = std::max_element(list.second.begin(), list.second.end());
	    // fill in all energies which are not the maximum energy
	    for (auto energy: list.second)
		if (energy != *it )
		    hE->Fill(energy);
	}

	hN -> Fill(neutron_count);
    }



    hists["energy"] -> Draw();
    new TCanvas();
    hists["ncount"] -> Draw();
}
