#include <TH1D.h>
#include <TFile.h>
#include <TTree.h>
#include <CLHEP/Units/SystemOfUnits.h>

#define ANATREE_CXX 1
#include "AnaManager.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AnaManager* AnaManager::fgManager = 0;


AnaManager::AnaManager(const char* filename):
    fFileName(filename),
    fRootFile(0),
    fHisto {},
    fTree(0),
    fEabs(0.), fEgap(0.) ,fLabs(0.), fLgap(0.), fEvent(),
    fWarningMessageCount(0),
    fTrkPdg {}
{
    fgManager = this;

  // histograms
  //for (G4int k=0; k<kMaxHisto; k++) fHisto[k] = 0;

}

AnaManager::AnaManager():
    fFileName("anatree.root"),
    fRootFile(0),
    fHisto {},
    fTree(0),
    fEabs(0.), fEgap(0.) ,fLabs(0.), fLgap(0.), fEvent(),
    fTrkPdg {}
{
    fgManager = this;

  // histograms
  //for (G4int k=0; k<kMaxHisto; k++) fHisto[k] = 0;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AnaManager::~AnaManager()
{
    if (fRootFile) delete fRootFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::Book()
{
  // Creating a tree container to handle histograms and ntuples.
  // This tree is associated to an output file.
  //
  fRootFile = new TFile(fFileName,"RECREATE");
  if (! fRootFile) {
    G4cout << " AnaManager::Book :"
           << " problem creating the ROOT TFile "
           << G4endl;
    return;
  }

  // create 1st ntuple
  fTree = AnaTree::createTree("events", fEvent);
  // have the tree saved every 20kB
  fTree->SetAutoSave(-20000);

  G4cout << "\n----> Output file is open in " << fFileName << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::Save()
{
  if (! fRootFile) return;

  fRootFile->Write();       // Writing the histograms to the file
  fRootFile->Close();       // and closing the tree (and the file)

  G4cout << "\n----> Histograms and ntuples are saved\n" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::FillHisto(G4int ih, G4double xbin, G4double weight)
{
  // if (ih >= kMaxHisto) {
  //   G4cerr << "---> warning from AnaManager::FillHisto() : histo " << ih
  //          << " does not exist. (xbin=" << xbin << " weight=" << weight << ")"
  //          << G4endl;
  //   return;
  // }
  // if  (fHisto[ih]) { fHisto[ih]->Fill(xbin, weight); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::Normalize(G4int ih, G4double fac)
{
  // if (ih >= kMaxHisto) {
  //   G4cout << "---> warning from AnaManager::Normalize() : histo " << ih
  //          << " does not exist. (fac=" << fac << ")" << G4endl;
  //   return;
  // }
  // if (fHisto[ih]) fHisto[ih]->Scale(fac);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void AnaManager::FillTree()
{
    // do any event filtering here
    fTree->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::PrintStatistic()
{
  // G4cout << "\n ----> print histograms statistic \n" << G4endl;
  // for ( G4int i=0; i<kMaxHisto; ++i ) {
  //   TH1D* h1 = fHisto[i];
  //   const G4String name = h1->GetName();

  //   G4String unitCategory;
  //   if (name[0] == 'E' ) unitCategory = "Energy";
  //   if (name[0] == 'L' ) unitCategory = "Length";

  //   G4cout << name
  //          << ": mean = " << G4BestUnit(h1->GetMean(), unitCategory)
  //          << " rms = " << G4BestUnit(h1->GetRMS(), unitCategory )
  //          << G4endl;
  // }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::Reset()
{
    AnaTree::resetEvent(fEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// Setters and getters

void AnaManager::SetEdep(Double_t time, Double_t Edep_em, Double_t Edep_nonem, Int_t detector)
{
    Int_t& n = *(&fEvent.n_tpc + detector);

    Double_t* times = fEvent.Tdep_tpc
	+ detector*AnaTree::MAX_DEPOSITIONS;
    times[n] = time;

    Double_t* Edeps = fEvent.Edep_tpc_em
	+ detector*AnaTree::MAX_DEPOSITIONS;
    Edeps[n] = Edep_em;
    // std::cout<<"Added Edep_em "<<Edeps[n]<<" at position"<<n<<std::endl;

    Edeps = fEvent.Edep_tpc_nonem
	+ detector*AnaTree::MAX_DEPOSITIONS;
    Edeps[n] = Edep_nonem;
    // std::cout<<"Added Edep_nonem "<<Edeps[n]<<" at position"<<n<<std::endl;

    ++n;

    // std::cout<<"Event "<<fEvent.eventNo<<std::endl;
    // std::cout<<"TPC N: "<<fEvent.n_tpc<<", EM E: "<<fEvent.Edep_tpc_em[fEvent.n_tpc-1]
    // 	     <<", RFR N: "<<fEvent.n_rfr<<", EM E: "<<fEvent.Edep_rfr_em[fEvent.n_rfr-1]
    // 	     <<", GDLS N: "<<fEvent.n_gdls<<", EM E: "<<fEvent.Edep_gdls_em[fEvent.n_gdls-1]
    // 	     <<", WT N: "<<fEvent.n_wt<<", EM E: "<<fEvent.Edep_wt_em[fEvent.n_wt-1]<<std::endl;
}


// Double_t AnaManager::GetEdep(Int_t em, Int_t detector)
// {
//     Int_t& n = *(&fEvent.n_tpc_em + 4*(em==kEm) + detector);

//     Double_t* Edeps = fEvent.Edep_tpc_em
// 	+ 4*(em==kEm)*AnaTree::MAX_DEPOSITIONS
// 	+ detector*AnaTree::MAX_DEPOSITIONS;

//     return Edeps[n];
// }
