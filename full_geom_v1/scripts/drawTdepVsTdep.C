#define FOR(i, size) for (int i = 0; i < size; i++)

void doXlog(TH1* h, int which = 0)
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

TH2* drawHist(TTree* tree, const char* det1, const char* det2,
	      int nbinst, float tlow, float thigh, int nbinse, float elow, float ehigh)
{
    TString name = Form("%s_%s", det1, det2);
    TString formula = Form("Tdep_%s:Tdep_%s", det1, det2);


    auto hET = new TH2F(Form("hTT_%s", name.Data()),
			Form("%s;T_{dep,%s} [#mus];T_{dep,%s} [#mus]", name.Data(), det1, det2),
			nbinst, tlow, thigh, nbinse, elow, ehigh);
    hET->SetDirectory(gROOT);
    doXlog(hET);
    doXlog(hET, 1);

    tree->Draw(Form("%s>>+%s", formula.Data(), hET->GetName()), "", "colz");

    return hET;
}

void attachFiles(TChain* tree, const char* basedir, int batch, int nruns)
{
    TString path  = Form("%s/batch_%d", basedir, batch);
    FOR(i, nruns)
	tree->AddFile(Form("%s/%d/sim.root", path.Data(), batch+i));
}

void drawTdepVsTdep(const char* basedir, int batch, int nruns, const char* outpref) {

    gStyle->SetTitleOffset(1.1,"x");
    gStyle->SetTitleOffset(1.3,"y");
    gStyle->SetNdivisions(505,"x");
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadBottomMargin(0.15);
    gStyle->SetOptStat(0);
    gStyle->SetNumberContours(255);

    auto tree = new TChain("events");
    //tree->Add(fname);
    attachFiles(tree, basedir, batch, nruns);
    cout<<"Entries: "<<tree->GetEntries()<<endl;

    auto c = new TCanvas("c", "", 1200, 1200);
    c->Divide(4,4, 0.001, 0.001);
    FOR(i, 16) {
	auto pad = c->cd(i+1);
	pad->SetLogx();
	pad->SetLogy();
	pad->SetLogz();
    }

    const char* detectors[4] = {"tpc", "skin", "gdls", "wt"};

    for (int i = 0; i < 3; i++) {
	for (int j = i+1; j < 4; j++) {
	    c->cd(i*4 + j + 1);
	    drawHist(tree,
		     detectors[i], detectors[j],
		     100, 0.001, 1e3, 100, 0.001, 1e3);
	}
    }


    c->SaveAs(Form("%sTdep_vs_Tdep.pdf", outpref));

    return;
}
