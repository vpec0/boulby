#include "SD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SD::SD(const G4String& name,
       const G4String& hitsCollectionName, Int_t detector)
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL),
   fDetector(detector)
{
  collectionName.insert(hitsCollectionName);
  fAnaM = AnaManager::GetManager();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SD::~SD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SD::Initialize(G4HCofThisEvent* hce)
{
    // std::cout<<"Constructing sensitive detector "<<fDetector
    // 	     <<" "<<SensitiveDetectorName<<std::endl
    // 	     <<"Creating hit collection "<<collectionName[0]<<std::endl;

    // Create hits collection

  fHitsCollection
    = new AHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce

  G4int hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SD::ProcessHits(G4Step* aStep,
                                     G4TouchableHistory*)
{
  // energy deposit
  G4double edep = aStep->GetTotalEnergyDeposit();

  if (edep==0.) return false;

  AHit* newHit = new AHit();

  newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
  newHit->SetEdep(edep);
  newHit->SetPos (aStep->GetPostStepPoint()->GetPosition());
  newHit->SetTime (aStep->GetPostStepPoint()->GetLocalTime());

  int pdg = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
  newHit->SetIsElmag ( pdg == 22 || pdg == 11 || pdg == -11);

  fHitsCollection->insert( newHit );

  //newHit->Print();

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) {
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event they are " << nofHits
            << " hits in the tracker chambers: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }

  std::vector<G4double> em_deps;
  std::vector<G4double> non_em_deps;

  std::vector<G4double> times;

  G4double total_em = 0.;
  G4double total_non_em = 0.;

  G4double gate = 0.;
  G4double time = 0.;
  G4double edep = 0.;
  // process hit collections and pass data to ana manager
  for (auto hit: *(fHitsCollection->GetVector())) {
      time = hit->GetTime()/CLHEP::us;
      edep = hit->GetEdep() / CLHEP::MeV;
      if (time > gate) {
	  if (gate > 0. && (total_em > 0. || total_non_em > 0.)) {
	      em_deps.push_back(total_em);
	      non_em_deps.push_back(total_non_em);
	      times.push_back(gate - 1.);
	  }
	  gate = time+1.; // set signal gate to 1 us
	  total_em = 0.;
	  total_non_em = 0.;
      }
      if (hit->IsElmag())
	  total_em += edep;
      else
	  total_non_em += edep;
  }
  // fill in last recorded hits
  if (total_em > 0. || total_non_em > 0.) {
      em_deps.push_back(total_em);
      non_em_deps.push_back(total_non_em);
      times.push_back(gate - 1.);
  }

  //     std::cout<<" Energy deposited in detector "<<fDetector<<std::endl
  // 	       <<"  EM: "<<total_em<<std::endl
  // 	       <<"  nonEM: "<<total_non_em<<std::endl;
  for (int i = 0; i < times.size(); ++i)
      fAnaM->SetEdep(times[i], em_deps[i], non_em_deps[i], fDetector);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
