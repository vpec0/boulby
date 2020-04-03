#define ANATREE_CXX
#include "AnaTree.h"
#include "Constants.hh"

#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

void doXlog(TH1*, int which = 0);
void attachFiles(TChain* tree, const char* fname, int batchNo, int Nruns);

void process(STR fname, STR outpref, int batchNo = 4, int Nruns = 10)
{
    // input tree
    auto tree = new TChain("events");
    attachFiles(tree, fname, batchNo, Nruns);
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
    	"Edep_tpc",
    	"Edep_rfr",
    	"Edep_skin",
    	"Edep_gdls",
    	"Edep_wt"
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
	EdepTpcNoGdls,
	EdepTpcNoGdlsWt,
	EdepTpcNoWt,

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

    H1(EdepTpcNoWt, "Energy Deposited in TPC, WT veto"
       ";Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpcNoWt]);

    H1(EdepTpcNoGdls, "Energy Deposited in TPC, Scint veto"
       ";Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpcNoGdls]);

    H1(EdepTpcNoGdlsWt, "Energy Deposited in TPC, Scint+WT veto"
       ";Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpcNoGdlsWt]);

    H1(EdepTpcNoSkinGdls, "Energy Deposited in TPC, Skin+Scint veto"
       ";Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpcNoSkinGdls]);

    H1(EdepTpcNoSkinGdlsWt, "Energy Deposited in TPC, Skin+Scint+WT veto"
       ";Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpcNoSkinGdlsWt]);

    H1(EdepTpcNoSkin, "Energy Deposited in TPC, Skin veto"
       ";Energy [MeV];[MeV^{-1}]", 360, 0.001, 1e6);
    doXlog(edephists[EdepTpcNoSkin]);


    // histograms by deposition types
    TH1* hists[EDepNhists][kNDepositionClasses] = {};
#undef H1
#define H1(depclass, name, title) hists[name][depclass] =		\
	new TH1F(Form("%s_%s", #name, DepositionClassNames[depclass]),	\
		 Form("Energy Deposited by %s in TPC, %s;Energy [MeV];[MeV^{-1}]", \
		      DepositionClassNames[depclass], title),		\
		 360, 0.001, 1e6)					\

    FOR(i, kNDepositionClasses) {
	H1(i, EdepTpcNoWt, "WT veto");
	doXlog(hists[EdepTpcNoWt][i]);

	H1(i, EdepTpcNoGdls, "Scint veto");
	doXlog(hists[EdepTpcNoGdls][i]);

	H1(i, EdepTpcNoGdlsWt, "Scint+WT veto");
	doXlog(hists[EdepTpcNoGdlsWt][i]);

	H1(i, EdepTpcNoSkinGdls, "Skin+Scint veto");
	doXlog(hists[EdepTpcNoSkinGdls][i]);

	H1(i, EdepTpcNoSkinGdlsWt, "Skin+Scint+WT veto");
	doXlog(hists[EdepTpcNoSkinGdlsWt][i]);

	H1(i, EdepTpcNoSkin, "Skin veto");
	doXlog(hists[EdepTpcNoSkin][i]);

    }


    //**************************************************************************



    // output file
    auto outf = TFile::Open(Form("%shists.root", outpref), "UPDATE");

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
    // loop over tree entries
    FOR(ientry, size) {
	if (step && (ientry+1) % step == 0 ) {
	    cout<<"-";
	    cout.flush();
	}
	tree->GetEntry(ientry);

	double etot[EDepNhists] = {};
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

	    if(etot[i] > 0.)
		edephists[i]->Fill(etot[i]);
	}

	double	skin_veto = 0.1;	// 100 keV
	double	gdls_veto = 0.2;	// 200 keV
	double	wt_veto	  = 200.;	// 200 MeV

	if (etot[EdepTpc] > 0.) {
	    if (etot[EdepSkin] < skin_veto) {
		edephists[EdepTpcNoSkin]->Fill(etot[EdepTpc]);
		if (etot[EdepGdls] < gdls_veto) {
		    edephists[EdepTpcNoSkinGdls]->Fill(etot[EdepTpc]);
		    if (etot[EdepWt] < wt_veto) {
			edephists[EdepTpcNoSkinGdlsWt]->Fill(etot[EdepTpc]);
		    }
		}
	    }

	    if (etot[EdepGdls] < gdls_veto) {
		edephists[EdepTpcNoGdls]->Fill(etot[EdepTpc]);
		if (etot[EdepWt] < wt_veto) {
		    edephists[EdepTpcNoGdlsWt]->Fill(etot[EdepTpc]);
		}
	    }

	    if (etot[EdepWt] < wt_veto) {
		edephists[EdepTpcNoWt]->Fill(etot[EdepTpc]);
	    }
	}

	FOR(i, kNDepositionClasses) {
	    if (etot_class[EdepTpc][i] > 0.) {
		if (etot[EdepSkin] < skin_veto) {
		    hists[EdepTpcNoSkin][i]->Fill(etot_class[EdepTpc][i]);
		    if (etot[EdepGdls] < gdls_veto) {
			hists[EdepTpcNoSkinGdls][i]->Fill(etot_class[EdepTpc][i]);
			if (etot[EdepWt] < wt_veto) {
			    hists[EdepTpcNoSkinGdlsWt][i]->Fill(etot_class[EdepTpc][i]);
			}
		    }
		}

		if (etot[EdepGdls] < gdls_veto) {
		    hists[EdepTpcNoGdls][i]->Fill(etot_class[EdepTpc][i]);
		    if (etot[EdepWt] < wt_veto) {
			hists[EdepTpcNoGdlsWt][i]->Fill(etot_class[EdepTpc][i]);
		    }
		}

		if (etot[EdepWt] < wt_veto) {
		    hists[EdepTpcNoWt][i]->Fill(etot_class[EdepTpc][i]);
		}
	    }
	}
    }
    cout<<"|"<<endl;

    // write out output histograms into the output file
    FOR (i, EDepNhists) {
	auto h = edephists[i];
	h->Write(h->GetName(), TObject::kOverwrite);
    }

    for (int i = EdepTpcNoSkin; i < EDepNhists; ++i) {
	FOR(j, kNDepositionClasses) {
	    auto h = hists[i][j];
	    h->Write(h->GetName(), TObject::kOverwrite);
	}
    }

    cout<<"Saving histograms in "<<outf->GetName()<<endl;
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


void attachFiles(TChain* tree, const char* fname, int batchNo, int Nruns) {
    if (!strcmp(fname, "")) { // no input given
        cout<<"Will add production files from batch "
            <<batchNo<<" to the chain (unchecked)."<<endl;
        TString batch = Form("200%02d", batchNo);
        TString topdir = "data/full_geom_v0_4classes/batch_";
        topdir += batch + "00/";
        for (int j = 0; j<Nruns; j++) {
            TString fname = topdir +
                Form(batch + "%02d/sim.root", j);
            int status = tree->Add(fname, -1);
        }
    } else { // input given
        cout<<"Adding "<<fname<<" to the chain."<<endl;
        int status = tree->Add(fname, -1);
        if (!status) { // try to look in the base dir
            tree->SetName("anatree");
            status = tree->Add(fname, -1);
        }
        cout<<"Status: "<<status<<endl;
    }
}
