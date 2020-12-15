#ifndef SDEXTERNAL_HH
#define SDEXTERNAL_HH

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

class SDExternal : public G4VSensitiveDetector
{
  public:
    SDExternal(const G4String& name);
    virtual ~SDExternal();

    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    AnaManager*		fAnaM;
    Int_t		fN137Xe;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
