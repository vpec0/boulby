#include "SDExternal.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "G4VProcess.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SDExternal::SDExternal(const G4String& name)
    : G4VSensitiveDetector(name),
      fN137Xe(0)
{
  fAnaM = AnaManager::GetManager();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SDExternal::~SDExternal()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SDExternal::Initialize(G4HCofThisEvent* hce)
{
    fN137Xe = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SDExternal::ProcessHits(G4Step* aStep,
                                     G4TouchableHistory*)
{
    auto track = aStep->GetTrack();
    // Find if initial step
    if ( track->GetCurrentStepNumber() != 1 ) // not a 1st step in the track
	return true;

    // if ( !track->GetCreatorProcess() )
    // 	return true;

    // // looking for nCapture process
    // if ( strcmp(track->GetCreatorProcess()->GetProcessName(),"nCapture") )
    // 	return true;

    // Check if nucleus
    // Check if 137Xe created
    if ( track->GetParticleDefinition()->GetAtomicNumber() != 54)
	return true;
    if ( track->GetParticleDefinition()->GetAtomicMass() != 137)
	return true;

    std::cout<<track->GetParticleDefinition()->GetParticleName()
	     <<", 1st step in vol.: "<<aStep->IsFirstStepInVolume()
	     <<", at: "<<aStep->GetPreStepPoint()->GetPosition()
	     <<", atomic mass: "<<track->GetParticleDefinition()->GetAtomicMass();
    std::cout<<", creator process: "<<track->GetCreatorProcess()->GetProcessName();
    std::cout<<std::endl;

    // Increment count for this event
    fN137Xe++;

    return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SDExternal::EndOfEvent(G4HCofThisEvent*)
{
    // pass total count on to AnaManager
    fAnaM->SetN137Xe(fN137Xe);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
