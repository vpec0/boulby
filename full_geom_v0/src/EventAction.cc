#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

#include "AnaManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
: G4UserEventAction(),
  fEdep(0.)
{
    fRunVerboseLevel = G4RunManager::GetRunManager()->GetVerboseLevel();
    G4cout<<"EventAction: Setting Run Verbose Level to: "<<fRunVerboseLevel<<G4endl;

    fRunAction = (RunAction*)G4RunManager::GetRunManager()->GetUserRunAction();

    fAnaM = AnaManager::GetManager();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* evt)
{

    // Set current random number seeds based on the current run and event
    long seeds[200] = {};
    G4Random::getTheTableSeeds(seeds, 0);
    //std::cout<<"Original seeds from the table: "<<seeds[0]<<", "<<seeds[1]<<std::endl;
    // const long* s = G4Random::getTheSeeds();
    // std::cout<<"Original event seeds: "<<s[0]<<", "<<s[1]<<std::endl;
    unsigned runId = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
    unsigned evtId = evt->GetEventID();
    /// Temporary debugging vvvvvvvvvvvvvvvvvvvv
    // runId = 20000;
    // evtId = 7452;
    /// end of temporary debugging ^^^^^^^^^^
    unsigned long hash = (runId << 16) | (evtId & 0xffff);
    unsigned long cantor0 = (((seeds[0] + hash)*(seeds[0] + hash))>>1) + seeds[0];
    unsigned long cantor1 = (((seeds[1] + hash)*(seeds[1] + hash))>>1) + seeds[1];
    // std::cout<<"runId: "<<runId<<", evtId:" <<evtId
    // 	     <<", cantor0: "<<cantor0<<", cantor1: "<<cantor1<<std::endl;
    seeds[0] = cantor0>>2;
    seeds[1] = cantor1>>2;
    // std::cout<<"Prepared seeds: "<<seeds[0]<<", "<<seeds[1]<<", "<<seeds[2]<<std::endl;
    G4Random::setTheSeeds(seeds);

    // s = G4Random::getTheSeeds();
    // std::cout<<"Current event seeds: "<<s[0]<<", "<<s[1]<<std::endl;
    // std::cout<<"Compared to: "<<seeds[0]<<", "<<seeds[1]<<std::endl;


    // if (evt->GetEventID() == 0) {
    // 	fRunVerboseLevel = G4RunManager::GetRunManager()->GetVerboseLevel();
    // }

    // if (fRunVerboseLevel>1) {
    // 	G4cout<<"Starting event number "<<evt->GetEventID()<<G4endl;
    // }
    //G4RunManager::GetRunManager()->RestoreRandomNumberStatus("currentEvent.rndm");

    fEdep = 0.;
    fAnaM->Reset();
    fAnaM->fEvent.runNo = fRunAction->fRunID;
    fAnaM->fEvent.eventNo = evt->GetEventID();

    // store primary particle info
    const G4PrimaryParticle* primary = evt->GetPrimaryVertex()->GetPrimary();
    fAnaM->fEvent.mu_en = primary->GetTotalEnergy() / CLHEP::GeV;
    G4ThreeVector mom_dir = primary->GetMomentumDirection();
    fAnaM->fEvent.mu_cosz = mom_dir.cosTheta();
    fAnaM->fEvent.mu_phi = mom_dir.phi();

    G4ThreeVector pos = evt->GetPrimaryVertex()->GetPosition();
    pos /= CLHEP::cm;
    fAnaM->fEvent.mu_startx = pos.x();
    fAnaM->fEvent.mu_starty = pos.y();
    fAnaM->fEvent.mu_startz = pos.z();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event*)
{
  // accumulate neutron statistics in run action
  fRunAction->AddEdep(fEdep);
  fAnaM -> FillTree();

  // process SD hit collections

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
