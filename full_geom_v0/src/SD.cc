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
  int pdg = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

  // if (pdg == 2112) {
  //     // sum energy of secondaries
  //     auto secondaries = aStep->GetSecondaryInCurrentStep();
  //     G4double totalKine = 0.;
  //     for (auto trk: *secondaries){
  // 	  totalKine += trk->GetKineticEnergy();
  //     }
  //     G4int secpdg = secondaries->at(0)->GetParticleDefinition()->GetPDGEncoding();

  //     G4double nonionidep = aStep->GetNonIonizingEnergyDeposit();
  //     G4double startE = aStep->GetPreStepPoint()->GetTotalEnergy();
  //     G4double endE = aStep->GetPostStepPoint()->GetTotalEnergy();
  //     std::cout<<pdg<<" hit at ("
  // 	       <<aStep->GetPostStepPoint()->GetPosition().x()<<","
  // 	       <<aStep->GetPostStepPoint()->GetPosition().y()<<","
  // 	       <<aStep->GetPostStepPoint()->GetPosition().z()<<")"
  // 	       <<", startE = "<<startE
  // 	       <<", endE = "<<endE
  // 	       <<", Edep = "<<edep
  // 	       <<", EdepNonIon = "<<nonionidep
  // 	       <<", N secondaries = "<<aStep->GetNumberOfSecondariesInCurrentStep()
  // 	       <<", 1st secondary pdg = "<<secpdg
  // 	       <<", DE = "<<(startE-endE)
  // 	       <<", Total KinE = "<<totalKine
  // 	       <<std::endl;
  // }


  if (edep==0.) return false;

  AHit* newHit = new AHit();

  newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
  newHit->SetEdep(edep);
  newHit->SetPos (aStep->GetPostStepPoint()->GetPosition());
  newHit->SetTime (aStep->GetPostStepPoint()->GetGlobalTime());
  // G4StepPoint::fGlobalTime: Time since event is created
  // G4StepPoint::fLocalTime: Time since track is created
  newHit->SetPdg( pdg );

  fHitsCollection->insert( newHit );

  // fill the histogram
  fAnaM->FillPDGEdep(fDetector, pdg, edep);

  // G4double nonionidep = aStep->GetNonIonizingEnergyDeposit();
  // std::cout<<pdg<<" hit at ("
  // 	   <<aStep->GetPostStepPoint()->GetPosition().x()<<","
  // 	   <<aStep->GetPostStepPoint()->GetPosition().y()<<","
  // 	   <<aStep->GetPostStepPoint()->GetPosition().z()<<")"
  // 	   <<", Edep = "<<edep
  // 	   <<", EdepNonIon = "<<nonionidep
  // 	   <<std::endl;

  // if (pdg == 2112) {
  //     std::cout<<"neutron hit at ("
  // 	       <<aStep->GetPostStepPoint()->GetPosition().x()<<","
  // 	       <<aStep->GetPostStepPoint()->GetPosition().y()<<","
  // 	       <<aStep->GetPostStepPoint()->GetPosition().z()<<")"
  // 	       <<", Edep = "<<edep<<std::endl;
  // }
  //newHit->Print();

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SD::EndOfEvent(G4HCofThisEvent*)
{
    const G4double gateWindow = 0.1*CLHEP::us; // how to group hits, in microseconds

    if ( verboseLevel>1 ) {
	G4int nofHits = fHitsCollection->entries();
	G4cout << G4endl
	       << "-------->Hits Collection: in this event they are " << nofHits
	       << " hits in the tracker chambers: " << G4endl;
	for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
    }

    std::vector<G4double> edeps[kNDepositionClasses];

    std::vector<G4double> times;

    G4double total[kNDepositionClasses] = {};

    G4double	gate = 0.;
    G4double	time = 0.;
    G4double	edep = 0.;
    G4int	pdg  = 0;
    EDepositionClass depcls;
    // process hit collections and pass data to ana manager
    for (auto hit: *(fHitsCollection->GetVector())) {
	time = hit->GetTime();
	edep = hit->GetEdep();
	pdg = hit->GetPdg();
	if (time > gate) {
	    if ( gate > 0. && (total[0] > 0. || total[1] > 0. ||
			       total[2] > 0. || total[3] > 0.) ) {
		edeps[0].push_back(total[0]  / CLHEP::MeV);
		edeps[1].push_back(total[1]  / CLHEP::MeV);
		edeps[2].push_back(total[2]  / CLHEP::MeV);
		edeps[3].push_back(total[3]  / CLHEP::MeV);
		times.push_back((gate - gateWindow) / CLHEP::us);
	    }
	    gate = time+gateWindow;
	    memset(total, 0, sizeof(total)) ;
	}
	if (edep == 0.) continue;
	if (pdg == 22 || pdg == 11 || pdg == -1)
	    depcls = kEm;
	else if (pdg == -13 || pdg == 13)
	    depcls = kMu;
	else if (pdg > 1000540000 && pdg < 1000550000)
	    depcls = kXe;
	else
	    depcls = kOther;
	total[depcls] += edep;
    }


    // fill in last recorded hits
    if (total[0] > 0. || total[1] > 0. ||
	total[2] > 0. || total[3] > 0.) {

	edeps[0].push_back(total[0]  / CLHEP::MeV);
	edeps[1].push_back(total[1]  / CLHEP::MeV);
	edeps[2].push_back(total[2]  / CLHEP::MeV);
	edeps[3].push_back(total[3]  / CLHEP::MeV);
	times.push_back((gate - gateWindow) / CLHEP::us);
    }

    // G4cout<<"End of event in det "<<fDetector<<", ";
    // if (edeps[0].size()) {
    // 	for (int i = 0; i < kNDepositionClasses; i++)
    // 	    G4cout<<edeps[i].at(0)<<", ";
    // } else
    // 	G4cout<<"no depositions"<<G4endl;
    // G4cout<<G4endl;

    //     std::cout<<" Energy deposited in detector "<<fDetector<<std::endl
    // 	       <<"  EM: "<<total_em<<std::endl
    // 	       <<"  nonEM: "<<total_non_em<<std::endl;
    for (int i = 0; i < times.size(); ++i)
	fAnaM->SetEdep(times[i], edeps[0][i], edeps[1][i], edeps[2][i], edeps[3][i], fDetector);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
