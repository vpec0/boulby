#ifndef GENERALSD_HH
#define GENERALSD_HH

#include "G4VSensitiveDetector.hh"
#include "AnaManager.h"


class VGeneralSD : public G4VSensitiveDetector
{
public:
    VGeneralSD(const G4String& name);
    virtual ~VGeneralSD();

    virtual void Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

private:
    AnaManager* mAnaM;
};

#endif
