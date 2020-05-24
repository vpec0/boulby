#include "Constants.hh"

void plotEdepHists(const char* fname, const char* outpref)
{
    gROOT->ForceStyle();

    gStyle->SetPadGridX(1);
    gStyle->SetPadGridY(1);

    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadRightMargin(0.15);
    gStyle->SetPadTopMargin(0.15);
    gStyle->SetPadBottomMargin(0.15);

    gStyle->SetTitleOffset(1.4,"xy");

    const char* suffix = ".pdf";


    auto c = new TCanvas("c","",2);
    c->SetLogx(1);
    c->SetLogy(1);


    auto f = TFile::Open(fname, "read");

    auto list = f->GetListOfKeys();

    for (auto next: *list) {
	auto key = (TKey*)next;
	if (strstr(key->GetClassName(), "TH1")) {
	    auto h = (TH1*)key->ReadObj();

	    // non-scaled version
	    h->Draw("hist");
	    // remove misleading title on y axis
	    h->GetYaxis()->SetTitle("");
	    c->SaveAs(Form("%s%s%s", outpref, h->GetName(), suffix));

	    // scaled version -- normalized to bin width
	    int entries = h->GetEntries();
	    double stats[10] = {};
	    h->GetStats(stats);

	    h->Scale(1., "width");
	    h->Draw("hist");
	    h->GetYaxis()->SetTitle("[MeV^{-1}]");

	    h->SetEntries(entries);
	    h->PutStats(stats);

	    c->SaveAs(Form("%s%s_bwscaled%s", outpref, h->GetName(), suffix));
	}
    }

}
