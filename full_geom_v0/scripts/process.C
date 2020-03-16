#define ANATREE_CXX
#include "AnaTree.h"


#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

void doXlog(TH1*, int which = 0);

void process(STR fname, STR outpref, STR charge = "default", STR material = "default", STR energy = "default")
{
    // input tree
    auto tree = new TChain("events");
    tree->SetMakeClass(1);
    int status = tree->Add(fname);
    if (!status) {
	cerr<< "Can't get events tree from file "<<fname<<endl;
	return;
    }
    auto evt = new AnaTree::Event_t;
    AnaTree::resetEvent(*evt);
    AnaTree::registerTree(tree, *evt);


    // allow only a subset of branches
    std::vector<TString> allowed = {
	"n_tpc",
	"n_rfr",
	"n_skin",
	"n_gdls",
	"n_wt",
	"Edep_tpc_em",
	"Edep_rfr_em",
	"Edep_skin_em",
	"Edep_gdls_em",
	"Edep_wt_em",
	"Edep_tpc_nonem",
	"Edep_rfr_nonem",
	"Edep_skin_nonem",
	"Edep_gdls_nonem",
	"Edep_wt_nonem"
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

	EdepTpcNoWt,
	EdepTpcNoGdls,
	EdepTpcNoWtGdls,

	EDepNhists,
    };
    TH1* edephists[EDepNhists] = {};

#define H1(name, title, nbins, low, high) edephists[name] = new TH1F(#name, title, nbins, low, high)

    H1(EdepTpc, "Energy Deposited in TPC;Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpc]);
    H1(EdepRfr, "Energy Deposited in RFR;Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepRfr]);
    H1(EdepSkin, "Energy Deposited in Skin;Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepSkin]);
    H1(EdepGdls, "Energy Deposited in GdLS;Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepGdls]);
    H1(EdepWt, "Energy Deposited in WT;Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepWt]);

    H1(EdepTpcNoWt, "Energy Deposited in TPC (WT deposits < 20 MeV);Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpcNoWt]);
    H1(EdepTpcNoGdls, "Energy Deposited in TPC (GdLS deposits < 10 MeV);Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpcNoGdls]);
    H1(EdepTpcNoWtGdls, "Energy Deposited in TPC (WT deposits < 20 MeV AND GdLS deposits < 10 MeV)"
       ";Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpcNoWtGdls]);
    //**************************************************************************

    // output file
    auto outf = TFile::Open(Form("%shists.root", outpref), "UPDATE");
    // if (!outf->cd(material))
    // 	outf->mkdir(material)->cd();
    // if (!gDirectory->cd(energy))
    // 	gDirectory->mkdir(energy)->cd();
    // if (!gDirectory->cd("neutrons_info"))
    // 	gDirectory->mkdir("neutrons_info")->cd();

    // main loop
    unsigned int size = tree->GetEntries();

#ifdef DEBUG
    size = 20; // just for debugging
#endif
    //size = 1000000;
    int step = size / 50;

    cout << "Will process "<<size<<" entries."<<endl;
    cout<<"|                                                  |\r|";
    cout.flush();
    FOR(ientry, size) {
	if (step && (ientry+1) % step == 0 ) {
            cout<<"-";
            cout.flush();
        }
	tree->GetEntry(ientry);

	double etot[EDepNhists] = {};
	for (int i = 0; i < EDepNhists; ++i) {
	    int& n = *(&evt->n_tpc + i);
	    double* edep_em = evt->Edep_tpc_em + i*AnaTree::MAX_DEPOSITIONS;
	    double* edep_nonem = evt->Edep_tpc_nonem + i*AnaTree::MAX_DEPOSITIONS;

	    // cout<<"i = "<<i
	    // 	<<", n = "<<n<<endl;
	    for (int j = 0; j < n; ++j) {
		etot[i] += edep_em[j] + edep_nonem[j];
	    }
	    if(etot[i] > 0.)
		edephists[i]->Fill(etot[i]);
	}

	if (etot[EdepTpc] > 0.) {
	    if (etot[EdepWt] < 20.)
		edephists[EdepTpcNoWt]->Fill(etot[EdepTpc]);
	    if (etot[EdepGdls] < 10.)
		edephists[EdepTpcNoGdls]->Fill(etot[EdepTpc]);
	    if (etot[EdepWt] < 20. && etot[EdepGdls] < 10.)
		edephists[EdepTpcNoWtGdls]->Fill(etot[EdepTpc]);
	}
    }
    cout<<"|"<<endl;

    // write out output histograms into the output file
    FOR (i, EDepNhists) {
	auto h = edephists[i];
	h->Write(h->GetName(), TObject::kOverwrite);
    }

    outf->Close();
}


void doXlog(TH1* h, int which)
// redo scales for x-log hists
{
    TAxis* axis = (which==0)?h->GetXaxis():h->GetYaxis();

    double start = TMath::Log10(axis->GetXmin());
    double stop = TMath::Log10(axis->GetXmax());
    double range = stop - start;
    int nbins = axis->GetNbins();
    double binwidth = range / nbins;

    double *bins = new double[nbins+1];
    FOR(i, (nbins+1)) {
	bins[i] = TMath::Power(10, start + i*binwidth);
    }

    axis->Set(nbins, bins);

    delete[] bins;
}
