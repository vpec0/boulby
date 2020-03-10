#ifndef AnaManager_h
#define AnaManager_h 1

#include "G4ThreeVector.hh"

#include "globals.hh"

#include "AnaTree.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TFile;
class TTree;
class TH1D;

const G4int kMaxHisto = 4;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class AnaManager
{
public:
    enum {
	kEm=0,
	kNonEm
    };

    enum {
	kTpc=0,
	kRfr,
	kGdls,
	kWt,
	kNdetectors
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
    void SetEdep(Double_t, Double_t, Double_t, Int_t);
    //Double_t* GetEdep(Int_t, Int_t);

public:
    static AnaManager* GetManager() {return fgManager;}


  private:
    TString  fFileName;
    TFile*   fRootFile;
    TH1D*    fHisto[kMaxHisto];
    TTree*   fTree;


    G4double fEabs;
    G4double fEgap;
    G4double fLabs;
    G4double fLgap;

public:
    AnaTree::Event_t fEvent;
    int fWarningMessageCount;

private:
    static AnaManager* fgManager;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
