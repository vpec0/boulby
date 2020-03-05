#ifndef GENERALSD_HH
#define GENERALSD_HH

#include "G4VSensitiveDetector.hh"

class GeneralSD : public G4VSensitiveDetector
{
public:
    GeneralSD(const G4String& name);
    virtual ~GeneralSD();

    virtual void Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

private:

};

#endif
