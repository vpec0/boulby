#include "GenMuonEvent.h"

#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

void doXlog(TH1*, int which = 0);

void ProcessGenMuons(const char* fname, const char* outpref)
{
    TChain* tree = new TChain("gen_muon_tree");
    int nfiles = tree->Add(fname);
    cout<<"Added "<<nfiles<<" files to the chain."<<endl;

    auto evt = new GenMuonEvent::Event_t;
    GenMuonEvent::registerTree(tree, *evt);


    // allow only a subset of branches
    std::vector<TString> allowed = {
	"cosX",
	"cosY",
	"cosZ",
	"energy",
    };
    tree->SetBranchStatus("*", 0);
    for (auto allow: allowed)
	tree->SetBranchStatus(allow, 1);

    //**************************************************************************
    // output hists

    // Energy deposit hists
    enum {
	Energy = 0,
	Zenith,
	Azimuth,
	cosZ,
	dirPhi,
	Nhists,
    };
    TH1* hists[Nhists] = {};

#define H1(name, title, nbins, low, high) hists[name] = new TH1F(#name, title, nbins, low, high)
    H1(Energy, "Muon Energy;Energy [GeV];[GeV^{-1}]", 120, 0.1, 1e5);
    doXlog(hists[Energy]);

    H1(Zenith, "Muon Zenith Angle; Zenith [#circ];", 180, 0., 90.);
    H1(Azimuth, "Muon Azimuth Angle; Azimuth [#circ];", 180, 0., 360.);
    H1(cosZ, "Muon Direction Cosine; cosZ;", 200, -1., 0.);
    H1(dirPhi, "Muon Direction Azimuth; Azimuth [#circ];", 180, -180., 180.);


    long int size = tree->GetEntries();

    int step = size / 50;

    cout << "Will process "<<size<<" entries."<<endl;
    cout<<"|                                                  |\r|";
    cout.flush();

    double zenith_avg = 0.;

    for (int ientry = 0; ientry < size; ++ientry) {
	if (step && (ientry+1) % step == 0 ) {
            cout<<"-";
            cout.flush();
        }
	tree->GetEntry(ientry);

	double zenith = TMath::ACos(-evt->cosZ)*180./TMath::Pi();
	hists[Zenith]->Fill(zenith);

	hists[cosZ]->Fill(evt->cosZ);

	double dirphi = TMath::ATan2(evt->cosY, evt->cosX)*180./TMath::Pi();
	hists[dirPhi]->Fill(dirphi);
	hists[Azimuth]->Fill(dirphi+180);

	hists[Energy]->Fill(evt->energy);

	zenith_avg += TMath::ACos(evt->cosZ);
    }
    cout<<"|"<<endl;

    zenith_avg /= size;
    zenith_avg *= 180./TMath::Pi();


    cout<<"Average zenith angle: "<<zenith_avg<<" deg and module 180 deg: "<<(180-zenith_avg)<<endl;

    // output file
    auto outf = TFile::Open(Form("%shists.root", outpref), "UPDATE");
    // write out output histograms into the output file
    FOR (i, Nhists) {
	auto h = hists[i];
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
