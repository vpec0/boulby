#include "ProperCoincTree.h"


void print_signal_like_events_simple(const char* fname)
{
    auto f = TFile::Open(fname, "read");
    auto tree = (TTree*)f->Get("proper_coincidence");

    ProperCoincTree::Event_t evt;
    ProperCoincTree::registerBranches(tree, &evt);

    size_t total = 0;
    size_t nols = 0;

    TEntryList total_list;
    TEntryList nols_list;

    auto size = tree->GetEntries();
    for (size_t entry = 0; entry < size; entry++) {
	tree->GetEntry(entry);

	auto xe_thrld = evt.tpc_deps[0] > 1e-3;
	auto other_deps = evt.tpc_deps[1] + evt.tpc_deps[2] + evt.tpc_deps[3]/10. < 10e-3; // depositoins by other means then Xe recoil need to be below 10 keV
	auto wt_veto = evt.veto_deps[2] < 200.;
	auto gdls_veto = evt.veto_deps[1] < 0.2;
	auto skin_veto = evt.veto_deps[0] < 0.1;
	auto nols_wt_veto = evt.veto_deps[1] + evt.veto_deps[2] < 200.;

	// Full veto events
	if (xe_thrld && other_deps && wt_veto && gdls_veto && skin_veto) {
	    total ++;
	    total_list.Enter(entry);
	}
	// no-LS veto events
	else if (xe_thrld && other_deps && nols_wt_veto && skin_veto) {
	    nols ++;
	    nols_list.Enter(entry);
	}

    }

    cout<<"Number of events passing selection (Skin+LS+WT): "<<total<<endl;
    cout<<"Additional events passing selection (Skin+WT): "<<nols<<endl;


    // total_list.Print("all");
    // nols_list.Print("all");
    cout<<"Run Event E_TPC_Xe [MeV] TPC_T [us] E_TPC_EM E_TPC_mu E_TPC_Other E_Skin E_LS E_WT"<<endl;

    cout<<"Full veto:"<<endl;
    for (int i = 0; i < total_list.GetN(); i++) {
	tree->GetEntry(total_list.Next());
	cout<<setprecision(10);
	cout<<evt.run<<" "<<evt.event<<" "
	    <<evt.tpc_deps[0]<<" "<<evt.tpc_t1<<" "
	    <<evt.tpc_deps[1]<<" "<<evt.tpc_deps[2]<<" "<<evt.tpc_deps[3]<<" "
	    <<evt.veto_deps[0]<<" "<<evt.veto_deps[1]<<" "<<evt.veto_deps[2]<<endl;
    }

    cout<<"No LS veto:"<<endl;
    for (int i = 0; i < nols_list.GetN(); i++) {
	tree->GetEntry(nols_list.Next());
	cout<<setprecision(10);
	cout<<evt.run<<" "<<evt.event<<" "
	    <<evt.tpc_deps[0]<<" "<<evt.tpc_t1<<" "
	    <<evt.tpc_deps[1]<<" "<<evt.tpc_deps[2]<<" "<<evt.tpc_deps[3]<<" "
	    <<evt.veto_deps[0]<<" "<<evt.veto_deps[1]<<" "<<evt.veto_deps[2]<<endl;
    }

}
