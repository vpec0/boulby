#ifndef AnaManager_h
#define AnaManager_h 1

#include "G4ThreeVector.hh"

#include "globals.hh"

#include "AnaTree.h"

#include <TH1F.h>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TFile;
class TTree;
class TH1D;

const G4int kMaxHisto = 4;
const G4int kMaxG4Tracks = 200000;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class AnaManager
{
public:
    enum {
	kTpc=0,
	kRfr,
	kSkin,
	kGdls,
	kWt,
	kNdetectors
    };

    const char* DetectorNames[kNdetectors] = {
	"TPC", "RFR", "Skin", "Scint", "WT"
    };

  public:
    AnaManager();
    AnaManager(const char*);
   ~AnaManager();

    void Book();
    void Save();

    void FillHisto(G4int id, G4double bin, G4double weight = 1.0);
    void Normalize(G4int id, G4double fac);

    void FillTree();
    void PrintStatistic();
    void Reset();

    // setters and getters
    // set or get energy deposit by EM or NonEM, and by detector
    void SetEdep(Double_t, Double_t, Double_t, Double_t, Double_t, Int_t);
    //Double_t* GetEdep(Int_t, Int_t);
    void FillPDGEdep(Int_t detector, Int_t pdg, Double_t Edep) {
	fHisto[detector]->Fill(Form("%d", pdg), Edep); }


    void SetN137Xe(Int_t n137xe);

    void AddTrkPdg(G4int trkId, G4int pdg) { fTrkPdg[trkId] = pdg; }
    G4int GetTrkPdg(G4int trkId) { return fTrkPdg[trkId]; }

public:
    static AnaManager* GetManager() {return fgManager;}


  private:
    TString  fFileName;
    TFile*   fRootFile;
    TH1F*    fHisto[kNdetectors];
    TTree*   fTree;


    G4double fEabs;
    G4double fEgap;
    G4double fLabs;
    G4double fLgap;

    G4int fTrkPdg[kMaxG4Tracks];


public:
    AnaTree::Event_t fEvent;
    int fWarningMessageCount;

private:
    static AnaManager* fgManager;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
