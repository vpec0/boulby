#ifndef SD_HH
#define SD_HH

#include "G4VSensitiveDetector.hh"

#include "AHit.hh"

#include "AnaManager.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// B2Tracker sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero
/// energy deposit.

class SD : public G4VSensitiveDetector
{
  public:
    SD(const G4String& name,
       const G4String& hitsCollectionName,
       Int_t detector);
    virtual ~SD();

    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    AHitsCollection*	fHitsCollection;
    AnaManager*		fAnaM;
    Int_t		fDetector;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
