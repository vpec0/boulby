#define ANATREE_CXX
#include "AnaTree.h"


#define FOR(i, size) for (unsigned int i = 0; i < size; ++i)
typedef const char* STR;

void doXlog(TH1*);

void process(STR fname, STR outpref, STR charge = "default", STR material = "default", STR energy = "default")
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
	"start_xyz",
	"end_xyz",
	"trackId",
	"parentId"
    };
    tree->SetBranchStatus("*", 0);
    for (auto allow: allowed)
	tree->SetBranchStatus(allow, 1);



    //**************************************************************************
    // output hists
    enum {
	kE = 0,
	kElog,
	kN,
	kNlog,
	kZ,
	kXY
    };
    const unsigned N_HISTS = 6;

    TH1* hists[2][N_HISTS]; // 1st dim: before and after spatial cut; 2nd dim: particular histograms
    STR cuts[2] = {"before", "after"};
    FOR(i, 2) {
	auto hE = new TH1F(Form("%s_energy_%s", charge, cuts[i]),"", 5000, 0, 5000);
	hists[i][kE] = hE;
	auto hElog = new TH1F(Form("%s_energy_log_%s", charge, cuts[i]),"", 700, 0.1, 1e6);
	hists[i][kElog] = hElog;
	doXlog(hElog);

	auto hN = new TH1F(Form("%s_ncount_%s", charge, cuts[i]),"", 4096, 0, 4096);
	hists[i][kN] = hN;
	auto hNlog = new TH1F(Form("%s_ncount_log_%s", charge, cuts[i]),"", 400, 1, 1e4);
	hists[i][kNlog] = hNlog;
	doXlog(hNlog);

	auto hZ = new TH1F(Form("%s_z_%s", charge, cuts[i]),"", 2200, -22e2, 22e2);
	hists[i][kZ] = hZ;
	auto hXY = new TH2F(Form("%s_xy_%s", charge, cuts[i]),"", 200, -10e2, 10e2, 200, -10e2, 10e2);
	hists[i][kXY] = hXY;
    }

    //**************************************************************************

    // output file
    auto outf = TFile::Open(Form("%shists.root", outpref), "UPDATE");
    if (!outf->cd(material))
	outf->mkdir(material)->cd();
    if (!gDirectory->cd(energy))
	gDirectory->mkdir(energy)->cd();
    if (!gDirectory->cd("neutrons_info"))
	gDirectory->mkdir("neutrons_info")->cd();

    // main loop
    unsigned int size = tree->GetEntries();

#ifdef DEBUG
    size = 20; // just for debugging
#endif

    cout << "Will process "<<size<<" entries."<<endl;
    FOR(ientry, size) {
	tree->GetEntry(ientry);

	// select hists to be filled before spatial cuts
	auto hE = hists[0][kE];
	auto hN = hists[0][kN];
	// after the cut
	auto hEcut = hists[1][kE];
	auto hNcut = hists[1][kN];


	int neutron_count = 0;
	int neutron_count_cut = 0;
	std::map<int, std::vector<int> > stack; // maps index of parent to indices of its daughters
	std::map<int, std::vector<int> > stack_cut; // same thing, just for neutrons passing the fidutial cut
	// FIXME: if the parent did not pass the cut, do not discard
	// the highest-energy daughter!

	// maps index of parent neutron to index and energy of its daughter with the largest energy
	std::map<int, std::pair<int, double> > max_energy_map;

#ifdef DEBUG
	const int MAX_NEUTRONS_DEBUG = 15;

	if (evt->n_neutrons < MAX_NEUTRONS_DEBUG )
	    cout<<"--------------------------------------------------"<<endl
		<<"Total number of read neutrons: "<<evt->n_neutrons<<endl
		<<"Neutron track id's (* neutron stored): ";
#endif

	FOR(i, evt->n_neutrons) {
	    if (i > AnaTree::MAX_NEUTRONS) break;

	    // is it produced within the volume of interest?
	    // +- 10 m in Z direction, length is in cm
	    bool passed = false;
	    if (evt->start_xyz[i][2] > -10e2 &&  evt->start_xyz[i][2] < 10e2)
		passed = true;

	    double energy = evt->n_energy[i];

#ifdef DEBUG
	    if (evt->n_neutrons < MAX_NEUTRONS_DEBUG ) {
		cout<<evt->trackId[i];
		if (passed)
		    cout<<"+";
	    }
#endif

	    // see if parent is a neutron
	    int parent_is_neutron = 0;
	    FOR(j, evt->n_neutrons) {
		if (i > AnaTree::MAX_NEUTRONS) break;
		if ( evt->parentId[i] != evt->trackId[j] )
		    continue;
		// its parent IS a neutron
		// store this neutron in a list of the parents descendants and store its energy
		stack[j].push_back(i);
		auto iit = max_energy_map.find(j);
		if ( iit != max_energy_map.end() || iit->second.second < energy)
		    iit->second = {i, energy};
		else
		    max_energy_map[j] = {i, energy};

		if (passed)
		    stack_cut[j].push_back(i);

		parent_is_neutron = 1;
		break; // stop looking for the parent
	    }

	    if (parent_is_neutron) {
#ifdef DEBUG
		if (evt->n_neutrons < MAX_NEUTRONS_DEBUG )
		    cout<<",";
#endif
		continue;
	    }

#ifdef DEBUG
	    if (evt->n_neutrons < MAX_NEUTRONS_DEBUG )
		cout<<"*,";
#endif

	    // store this neutrons info immediately
	    hE->Fill(energy);
	    hists[0][kElog]->Fill(energy);
	    hists[0][kZ]->Fill(evt->start_xyz[i][2]);
	    hists[0][kXY]->Fill(evt->start_xyz[i][0], evt->start_xyz[i][1]);
	    neutron_count++;
	    if (passed) {
		hEcut->Fill(energy);
		hists[1][kElog]->Fill(energy);
		hists[1][kZ]->Fill(evt->start_xyz[i][2]);
		hists[1][kXY]->Fill(evt->start_xyz[i][0], evt->start_xyz[i][1]);
		neutron_count_cut++;
	    }
	}

#ifdef DEBUG
	if (evt->n_neutrons < MAX_NEUTRONS_DEBUG )
	    cout<<endl; // finish line with neutron track id's

	if (evt->n_neutrons < MAX_NEUTRONS_DEBUG )
	    cout<<"Descendants (* highest energy/skipped):"<<endl;
#endif

	// loop over stacked neutrons which have neutrons as their parents
	for (auto& list: stack) {
#ifdef DEBUG
	    if (evt->n_neutrons < MAX_NEUTRONS_DEBUG )
		cout<<"  "<<evt->trackId[list.first]<<", "<<evt->n_energy[list.first]<<" MeV, "
		    <<"("<<evt->end_xyz[list.first][0]<<","<<evt->end_xyz[list.first][1]<<","<<evt->end_xyz[list.first][2]<<"):";
#endif

	    neutron_count += list.second.size()-1;
	    // find maximum energy element
	    auto j = max_energy_map[list.first].first;
	    // fill in all energies which are not the maximum energy
	    for (auto i: list.second) {
#ifdef DEBUG
		if (evt->n_neutrons < MAX_NEUTRONS_DEBUG ) {
		    cout<<"["<<evt->trackId[i];
		    if (i==j)
			cout<<"*";
		    cout<<", "<<evt->n_energy[i]<<" MeV, "
			<<"("<<evt->start_xyz[i][0]<<","<<evt->start_xyz[i][1]<<","<<evt->start_xyz[i][2]<<")], ";
		}
#endif

		if (i == j )
		    continue;
		hE->Fill(evt->n_energy[i]);
		hists[0][kElog]->Fill(evt->n_energy[i]);
		hists[0][kZ]->Fill(evt->start_xyz[i][2]);
		hists[0][kXY]->Fill(evt->start_xyz[i][0], evt->start_xyz[i][1]);
	    }
#ifdef DEBUG
	    if (evt->n_neutrons < MAX_NEUTRONS_DEBUG )
		cout<<endl;
#endif
	}

	// loop over stacked neutrons which have neutrons as their parents
	// for neutrons passing the spatial cut
	for (auto& list: stack_cut) {
	    neutron_count_cut += list.second.size()-1;
	    // find maximum energy element
	    auto j = max_energy_map[list.first].first;
	    // fill in all energies which are not the maximum energy
	    for (auto i: list.second) {
		if (i == j )
		    continue;
		hEcut->Fill(evt->n_energy[i]);
		hists[1][kElog]->Fill(evt->n_energy[i]);
		hists[1][kZ]->Fill(evt->start_xyz[i][2]);
		hists[1][kXY]->Fill(evt->start_xyz[i][0], evt->start_xyz[i][1]);
	    }
	}

#ifdef DEBUG
	if (evt->n_neutrons < MAX_NEUTRONS_DEBUG )
	    cout<<"===> neutrons number stored: "<<neutron_count<<", of which "<<neutron_count_cut<<" passed the Z cut."<<endl;
#endif

	hN    -> Fill(neutron_count);
	hNcut -> Fill(neutron_count_cut);
	hists[0][kNlog] -> Fill(neutron_count);
	hists[1][kNlog] -> Fill(neutron_count_cut);
    }

    // hists["energy"] -> Draw();
    // new TCanvas();
    // hists["ncount"] -> Draw();


    // write out output histograms into the output file
    FOR(i,2) {
	FOR (j, N_HISTS) {
	    auto h = hists[i][j];
	    h->Write(h->GetName(), TObject::kOverwrite);
	}
    }

    outf->Close();
}


void doXlog(TH1* h)
// redo scales for x-log hists
{
    double start = TMath::Log10(h->GetXaxis()->GetXmin());
    double stop = TMath::Log10(h->GetXaxis()->GetXmax());
    double range = stop - start;
    int nbins = h->GetXaxis()->GetNbins();
    double binwidth = range / nbins;

    double *bins = new double[nbins+1];
    FOR(i, (nbins+1)) {
	bins[i] = TMath::Power(10, start + i*binwidth);
    }

    h->GetXaxis()->Set(nbins, bins);

    delete[] bins;
}
