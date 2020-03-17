#ifndef HIT_HH
#define HIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"


#include "Constants.hh"

class AHit : public G4VHit
{
  public:
    AHit();
    AHit(const AHit&);
    virtual ~AHit();

    // operators
    const AHit& operator=(const AHit&);
    G4bool operator==(const AHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetTrackID  (G4int track)      { fTrackID = track; };
    void SetEdep     (G4double de)      { fEdep	   = de; };
    void SetPos      (G4ThreeVector xyz){ fPos	   = xyz; };
    void SetTime     (G4double time)    { fTime	   = time; };
    void SetPdg      (G4int pdg)        { fPdg	   = pdg; };

    // Get methods
    G4int GetTrackID() const     { return fTrackID; };
    G4double GetEdep() const     { return fEdep; };
    G4ThreeVector GetPos() const { return fPos; };
    G4double GetTime() const { return fTime; };
    G4double GetPdg() const { return fPdg; };

  private:

    G4int		fTrackID;
    G4double		fEdep;
    G4ThreeVector	fPos;
    G4double		fTime;
    G4int		fPdg;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<AHit> AHitsCollection;

extern G4ThreadLocal G4Allocator<AHit>* AHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* AHit::operator new(size_t)
{
  if(!AHitAllocator)
      AHitAllocator = new G4Allocator<AHit>;
  return (void *) AHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void AHit::operator delete(void *hit)
{
  AHitAllocator->FreeSingle((AHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
