
void print_signal_like_events(const char* fname)
{
    auto f = TFile::Open(fname, "read");
    auto tree = (TTree*)f->Get("proper_coincidence");

    TCut xe_thrld = "tpc_deps[0] > 1e-3";
    TCut other_deps = "tpc_deps[1] + tpc_deps[2] + tpc_deps[3]/10. < 10e-3"; // depositoins by other means then Xe recoil need to be below 1 keV
    TCut wt_veto = "veto_deps[2] < 200";
    TCut gdls_veto = "veto_deps[1] < 0.2";
    TCut skin_veto = "veto_deps[0] < 0.1";
    TCut nols_wt_veto = "veto_deps[1] + veto_deps[2] < 200";

    auto total = tree->GetEntries(xe_thrld && other_deps && wt_veto && gdls_veto && skin_veto);

    auto total_nols = tree->GetEntries(xe_thrld && other_deps && nols_wt_veto && skin_veto);

    cout<<"Number of events passing selection (Skin+LS+WT): "<<total<<endl;
    cout<<"Number of events passing selection (Skin+WT): "<<total<<endl;
    tree->Scan("run:event:tpc_deps[0]:tpc_t1:tpc_t2"
	       ":tpc_deps[1]:tpc_deps[2]:tpc_deps[3]"
	       ":veto_deps[0]:veto_deps[1]:veto_deps[2]",
	       xe_thrld && other_deps && wt_veto && gdls_veto && skin_veto);
}
