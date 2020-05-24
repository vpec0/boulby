#include "GenMuonEvent.h"


void ReadMuonFile(const char* fname, const char* outfname = "")
{
    /// File format:
    ///
    /// Seq number
    /// -13 mu+ or 13 mu-
    ///  Energy in GeV (assuming ultrarelativistic muons so E = Ekin)
    ///  3 positions in x, y z in cm
    ///  3 direction cosines (the last one is always negative since all muons are going down).

    TString aoutfname = fname;
    aoutfname += ".root";

    if (strcmp(outfname, ""))
	aoutfname = outfname;

    // output file
    auto outf = new TFile(aoutfname, "recreate");

    // output tree
    auto* evt = new GenMuonEvent::Event_t;
    TTree* tree = GenMuonEvent::createTree("gen_muon_tree", *evt);



    ifstream muon_file;
    muon_file.open(fname);
    if( !muon_file.is_open() ) { cout<<"couldn't find muon file... bye "<<endl; return 0;}
    else cout<<"Loading this muon file: "<<fname<<endl;

    long int size = -1;

    size = 1e6;
    long int entry = 0;
    while(true){
	if (size > 0 && entry >= size) break;
	++entry;
	muon_file>>evt->entry>>evt->pdg>>evt->energy
		 >>evt->posX>>evt->posY>>evt->posZ
		 >>evt->cosX>>evt->cosY>>evt->cosZ;
	if( muon_file.eof() ) break;

	// fill the output tree
	tree->Fill();
    }


    outf->Write();
    outf->Close();
}
