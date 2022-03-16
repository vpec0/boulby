#include "ProperCoincTree_v1.h"

#define FOR(i, size) for (int i = 0; i < size; i++)

#define in_coincidence(veto_t1, veto_t2, window_t1, window_t2) (veto_t1 < window_t2 && veto_t2 > window_t1)

struct veto_index_t {
    int i0, i1, i2;
};

struct combined_t {
    int i1;
    int i2;
    int dep1;
    int dep2;
    int dep;
};

void merge_veto_deps(int n1, double* t1, double* dep1, int n2, double* t2, double* dep2, vector<combined_t>& result);


void print_signal_like_events_simple_v1(const char* fname)
{
    auto f = TFile::Open(fname, "read");
    auto tree = (TTree*)f->Get("proper_coincidence");

    ProperCoincTree::Event_t evt;
    ProperCoincTree::registerBranches(tree, &evt);

    size_t total = 0;
    size_t nols = 0;

    TEntryList total_list;
    TEntryList nols_list;
    vector<veto_index_t> veto_list, veto_nols_list;

    auto size = tree->GetEntries();
    size_t step = size / 50;
    cout << "Will process "<<size<<" entries."<<endl;
    cout<<"|                                                  |\r|";
    cout.flush();
    for (size_t entry = 0; entry < size; entry++) {
	if (step && (entry+1) % step == 0 ) {
	    cout<<"-";
	    cout.flush();
	}

	tree->GetEntry(entry);

	auto xe_thrld = evt.tpc_deps[0] > 1e-3;
	auto other_deps = evt.tpc_deps[1] + evt.tpc_deps[2] + evt.tpc_deps[3]/10. < 10e-3; // depositoins by other means then Xe recoil need to be below 10 keV

	// check for veto signals
	veto_index_t which_veto = {-1,-1,-1}; // Skin, GdLS, WT
	bool veto[3] = {true, true, true}; // true if no signal over threshold
	float veto_threshold[3] = {
	    0.1, //skin
	    0.2, // GdLS
	    200. //WT
	};

	const float veto_window = 500.; // no signal in veto detectors 0.5 ms before and after signal in TPC
	float coinc_low = evt.tpc_t1 - veto_window;
	float coinc_hi  = evt.tpc_t2 + veto_window;

	// find signals in each veto system
	FOR(iveto, 3) {
	    double largest_veto = 0.;
	    FOR(i, evt.n_veto_deps[iveto]) {
		// if (entry == 47339) {
		//     cout << "Veto "<<iveto<<" T1 = "<< evt.veto_t1[iveto][i]
		// 	 << " T2 = "<< evt.veto_t2[iveto][i]
		// 	 << " Coincidence low = "<< coinc_low
		// 	 << " hi = "<< coinc_hi
		// 	 << " dep = " << evt.veto_deps[iveto][i] << endl;
		// }
		if ( in_coincidence(evt.veto_t1[iveto][i], evt.veto_t2[iveto][i], coinc_low, coinc_hi) ) {
		    if (evt.veto_deps[iveto][i] > largest_veto) {
			((int*)&which_veto.i0)[iveto] = i;
			largest_veto = evt.veto_deps[iveto][i];
		    }
		    if ( evt.veto_deps[iveto][i] > veto_threshold[iveto] ) {
			veto[iveto] = false;
			break;
		    }
		}
	    }
	    // if (entry == 47339) {
	    // 	cout << "Which veto " << iveto << ": " << ((int*)&which_veto.i0)[iveto] << endl;
	    // }
	}

	// deal with combined veto (LS+WT=WT) separately -- check depositions within the same time window
	veto_index_t which_veto_nols = {-1,-1,-1}; //  Skin, GdLS, WT
	vector<combined_t> combined_deps;
	// merge LS and WT depositions -- considers only depositions which are within 1 us coincidence
	merge_veto_deps(evt.n_veto_deps[1], evt.veto_t1[1], evt.veto_deps[1],
			evt.n_veto_deps[2], evt.veto_t1[2], evt.veto_deps[2], combined_deps);
	auto nols_wt_veto = true;
	int idx1, idx2;
	double largest_veto = 0.;
	which_veto_nols.i0 = which_veto.i0;
	for (auto dep: combined_deps) {
	    idx1 = dep.i1;
	    idx2 = dep.i2;
	    if( in_coincidence(evt.veto_t1[1][idx1], evt.veto_t2[1][idx1], coinc_low, coinc_hi)
		|| in_coincidence(evt.veto_t1[2][idx2], evt.veto_t2[2][idx2], coinc_low, coinc_hi)
		) {
		if ( dep.dep > largest_veto) {
		    which_veto_nols.i1 = idx1;
		    which_veto_nols.i2 = idx2;
		    largest_veto = dep.dep;
		}

		if (dep.dep > veto_threshold[2]) { // WT threshold
		    nols_wt_veto = false;
		    break;
		}
	    }
	}

	// Full veto events
	if (xe_thrld && other_deps && veto[2] && veto[1] && veto[0]) {
	    total ++;
	    total_list.Enter(entry);
	    veto_list.push_back(which_veto);
	}
	// no-LS veto events
	else if (xe_thrld && other_deps && nols_wt_veto && veto[0]) {
	    nols ++;
	    nols_list.Enter(entry);
	    veto_nols_list.push_back(which_veto_nols);
	}

    }
    cout<<"|"<<endl;

    cout<<"Number of events passing selection (Skin+LS+WT): "<<total<<endl;
    cout<<"Additional events passing selection (Skin+WT): "<<nols<<endl;


    // total_list.Print("all");
    // nols_list.Print("all");
    cout<<"Run Event E_TPC_Xe [MeV] TPC_T [us] E_TPC_EM E_TPC_mu E_TPC_Other E_Skin E_LS E_WT"<<endl;

    cout<<"Full veto:"<<endl;
    auto veto = veto_list.begin();
    for (int i = 0; i < total_list.GetN(); i++, veto++) {
	tree->GetEntry(total_list.Next());
	cout<<setprecision(10);
	cout<<evt.run<<" "<<evt.event<<" "
	    <<evt.tpc_deps[0]<<" "<<evt.tpc_t1<<" "
	    <<evt.tpc_deps[1]<<" "<<evt.tpc_deps[2]<<" "<<evt.tpc_deps[3]<<" "
	    <<((veto->i0 >= 0)?evt.veto_deps[0][veto->i0]:0)<<" "
	    <<((veto->i1 >= 0)?evt.veto_deps[1][veto->i1]:0)<<" "
	    <<((veto->i2 >= 0)?evt.veto_deps[2][veto->i2]:0)<<endl;
    }

    cout<<"No LS veto:"<<endl;
    veto = veto_nols_list.begin();
    for (int i = 0; i < nols_list.GetN(); i++, veto++) {
	tree->GetEntry(nols_list.Next());
	cout<<setprecision(10);
	cout<<evt.run<<" "<<evt.event<<" "
	    <<evt.tpc_deps[0]<<" "<<evt.tpc_t1<<" "
	    <<evt.tpc_deps[1]<<" "<<evt.tpc_deps[2]<<" "<<evt.tpc_deps[3]<<" "
	    <<((veto->i0 >= 0)?evt.veto_deps[0][veto->i0]:0)<<" "
	    <<((veto->i1 >= 0)?evt.veto_deps[1][veto->i1]:0)<<" "
	    <<((veto->i2 >= 0)?evt.veto_deps[2][veto->i2]:0)<<endl;
    }

}

void merge_veto_deps(int n1, double* t1, double* dep1, int n2, double* t2, double* dep2, vector<combined_t>& result)
{
    // only merge overlapping signals, solitairs are treated in the Full Veto (Skin+LS+WT)
    int i1 = 0, i2 = 0;
    combined_t combined;

    while (i1 < n1 && i2 < n2) {
	if ( abs(t1[i1] - t2[i2]) < 1. ) { // deps within 1 us of each other
	    combined.i1 = i1;
	    combined.i2 = i2;
	    combined.dep1 = dep1[i1];
	    combined.dep2 = dep2[i2];
	    combined.dep = combined.dep1 + combined.dep2;
	    result.push_back(combined);
	    ++i1; ++i2;
	} else if ( t1[i1] < t2[i2] )
	    ++i1;
	else
	    ++i2;

    }
}
