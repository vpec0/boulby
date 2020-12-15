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

TH2* drawHist(TTree* tree, const char* name, const char* formula,
	      int nbinst, float tlow, float thigh, int nbinse, float elow, float ehigh)
{
    auto hET = new TH2F(Form("hET_%s", name), Form("%s;Tdep [#mus];Edep [GeV]",name),
			nbinst, tlow, thigh, nbinse, elow, ehigh);
    hET->SetDirectory(gROOT);
    doXlog(hET);
    doXlog(hET, 1);

    tree->Draw(Form("%s>>+%s", formula, hET->GetName()), "", "colz");

    return hET;
}

void attachFiles(TChain* tree, const char* basedir, int batch, int nruns)
{
    TString path  = Form("%s/batch_%d", basedir, batch);
    FOR(i, nruns)
	tree->AddFile(Form("%s/%d/sim.root", path.Data(), batch+i));
}

void drawEdepVsTdep(const char* basedir, int batch, int nruns, const char* outpref) {

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

    int ipad = 1;
    c->cd(ipad++);
    drawHist(tree, "wt_xe", "Edep_wt[][0]:Tdep_wt", 100, 0.001, 1e20, 100, 1e-9, 100.);
    c->cd(ipad++);
    drawHist(tree, "wt_mu", "Edep_wt[][2]:Tdep_wt", 100, 0.001, 1e2, 100, 1., 1.e4);
    c->cd(ipad++);
    drawHist(tree, "wt_em", "Edep_wt[][1]:Tdep_wt", 100, 0.001, 1e32, 100, 1e-4, 1.e6);
    c->cd(ipad++);
    drawHist(tree, "wt_other", "Edep_wt[][3]:Tdep_wt", 100, 0.01, 1e32, 100, 1e-11, 1e5);

    c->cd(ipad++);
    drawHist(tree, "gdls_xe", "Edep_gdls[][0]:Tdep_gdls", 100, 0.001, 1e20, 100, 1e-9, 100.);
    c->cd(ipad++);
    drawHist(tree, "gdls_mu", "Edep_gdls[][2]:Tdep_gdls", 100, 0.001, 1e2, 100, 1., 1.e4);
    c->cd(ipad++);
    drawHist(tree, "gdls_em", "Edep_gdls[][1]:Tdep_gdls", 100, 0.001, 1e32, 100, 1e-4, 1.e6);
    c->cd(ipad++);
    drawHist(tree, "gdls_other", "Edep_gdls[][3]:Tdep_gdls", 100, 0.01, 1e32, 100, 1e-10, 1e5);

    c->cd(ipad++);
    drawHist(tree, "skin_xe", "Edep_skin[][0]:Tdep_skin", 100, 0.001, 1e20, 100, 1e-9, 100.);
    c->cd(ipad++);
    drawHist(tree, "skin_mu", "Edep_skin[][2]:Tdep_skin", 100, 0.001, 1e2, 100, 1., 1.e4);
    c->cd(ipad++);
    drawHist(tree, "skin_em", "Edep_skin[][1]:Tdep_skin", 100, 0.001, 1e32, 100, 1e-4, 1.e6);
    c->cd(ipad++);
    drawHist(tree, "skin_other", "Edep_skin[][3]:Tdep_skin", 100, 0.01, 1e32, 100, 1e-10, 1);

    c->cd(ipad++);
    drawHist(tree, "tpc_xe", "Edep_tpc[][0]:Tdep_tpc", 100, 0.001, 1e20, 100, 1e-9, 100.);
    c->cd(ipad++);
    drawHist(tree, "tpc_mu", "Edep_tpc[][2]:Tdep_tpc", 100, 0.001, 1e2, 100, 1., 1.e4);
    c->cd(ipad++);
    drawHist(tree, "tpc_em", "Edep_tpc[][1]:Tdep_tpc", 100, 0.001, 1e32, 100, 1e-4, 1.e6);
    c->cd(ipad++);
    drawHist(tree, "tpc_other", "Edep_tpc[][3]:Tdep_tpc", 100, 0.01, 1e32, 100, 1e-10, 1e-3);


    c->SaveAs(Form("%sEdep_vs_Tdep.pdf", outpref));

    return;
}
