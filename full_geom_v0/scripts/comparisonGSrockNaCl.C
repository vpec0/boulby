
const char* base_dir[2] = {
    "plots/full_geom",
    "plots/full_geom_nacl"};

const double scale[2] = { 1., 120e6/199902600.};

const size_t colors[2] = {kRed, kBlue};

const char* titles[2] = {"GS rock", "NaCl"};


const TString outpref("plots/compare_grr_nacl_");

struct HistInfo_t {
    double min;
    double max;
    bool logX;
    TH1* hists[2];
};

using HistMap_t = map<TString, HistInfo_t>;


void plotComparisons(HistMap_t& hists);

void comparisonGSrockNaCl()
{
    gROOT->ForceStyle(1);
    gStyle->SetHistFillStyle(0);
    gStyle->SetOptStat(0);
    gStyle->SetTitleW(1.);

    HistMap_t hists;

    // Ndep plots
    for (int i = 0; i < 2; ++i) {
	TString fname = base_dir[i];
	fname += "/merged_diagnostic_hists.root";

	auto f = TFile::Open(fname, "read");

	auto list = f->GetListOfKeys();

	for (auto next: *list) {
	    auto key = (TKey*)next;
	    if (strstr(key->GetClassName(), "TH1")) {
		auto h = (TH1*)key->ReadObj();

		HistInfo_t &hi =  hists[h->GetName()];
		hi.min = 0; hi.max = 20.; hi.logX = false; hi.hists[i] = h;

		h->SetDirectory(gROOT); // keep histogram in memory after closing the input file
	    }
	}
	f->Close();
    }

    // Edep plots
    for (int i = 0; i < 2; ++i) {
    	TString fname = base_dir[i];
    	fname += "/merged_LSon_hists.root";

    	auto f = TFile::Open(fname, "read");

    	auto list = f->GetListOfKeys();

    	for (auto next: *list) {
    	    auto key = (TKey*)next;
    	    if (strstr(key->GetClassName(), "TH1")) {
    		auto h = (TH1*)key->ReadObj();

		HistInfo_t &hi =  hists[h->GetName()];
		hi.min = 1e-4; hi.max = 1e6; hi.logX = true; hi.hists[i] = h;

    		h->SetDirectory(gROOT); // keep histogram in memory after closing the input file

		// rebin if a vetoed histogram
		if (strstr(h->GetName(),"TpcNo")) {
		    h->Rebin(5);
		    //cout<<"Rebinning "<<h->GetName()<<endl;
		}
    	    }
    	}
    	f->Close();
    }


    // for (auto hpair: hists) {
    // 	cout<<hpair.first<<": ";
    // 	for (auto h: hpair.second.hists)
    // 	    cout<<h->GetName()<<" ";
    // 	cout<<endl;
    // }


    plotComparisons(hists);

}

void plotComparisons(HistMap_t& hists) {

    auto c = new TCanvas("c","",1);
    c->SetGrid();
    vector<THStack*> vhs;
    for (auto hpair: hists) {
	bool logX = hpair.second.logX;
	c->SetLogx(logX);
	c->SetLogy();
	TString title = hpair.second.hists[0]->GetTitle();

	title += ";";
	title += hpair.second.hists[0]->GetXaxis()->GetTitle();
	auto hs = new THStack(hpair.first, title);
	auto leg = new TLegend(0.3,0.85,0.7,0.93);
	leg->SetNColumns(2);

	for (size_t i = 0; i < 2; ++i) {
	    auto h = hpair.second.hists[i];
	    h->SetLineColor(colors[i]);
	    h->Scale(scale[i]);
	    hs->Add(h,"hist");
	    leg->AddEntry(h, titles[i], "L");
	}

	hs->Draw("nostack");
	c->Update();
	hs->GetHistogram()->SetAxisRange(hpair.second.min, hpair.second.max);
	vhs.push_back(hs);

	leg->Draw();

	c->Print(outpref+hs->GetName()+".pdf");

	// create ratio of the histograms
	c->SetLogy(0);
	auto h = (TH1*)hpair.second.hists[0]->Clone(Form("%s_ratio",hs->GetName()));
	h->Divide(hpair.second.hists[1]);

	h->SetAxisRange(hpair.second.min, hpair.second.max);
	h->Draw();
	h->GetYaxis()->SetTitle("Ratio GS rock/NaCl");

	// draw line at ratio=1.
	//c->Update();
	auto xaxis = h->GetXaxis();
	TLine l(xaxis->GetXmin(), 1., xaxis->GetXmax(), 1.);
	l.SetLineColor(kGreen+1);
	l.SetLineWidth(2);
	l.Draw();
	// redraw the hist over the line
	h->Draw("same");
	h->SetMaximum(2.);
	h->SetMinimum(0.);

	c->Print(outpref+h->GetName()+".pdf");
    }

}
