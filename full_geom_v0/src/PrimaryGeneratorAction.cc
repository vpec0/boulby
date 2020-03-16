#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0),
      fEnvelopeBox(0),
      fNevents(-1), // read in all events in the file
      fStartEvent(0), // start from 1st entry
      fMuonFileName(""),
      fMuonFile(0)
{
    fAnaM = AnaManager::GetManager();

    G4int n_particle = 1;
    fParticleGun  = new G4ParticleGun(n_particle);

    fParticleTable = G4ParticleTable::GetParticleTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(const char* fname, G4int nevents, G4int startevent)
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0),
      fEnvelopeBox(0),
      fNevents(nevents), // read in all events in the file
      fStartEvent(startevent), // start from 1st entry
      fMuonFileName(fname),
      fMuonFile(0)
{
    fAnaM = AnaManager::GetManager();

    ReadInMuonFile();

    G4int n_particle = 1;
    fParticleGun  = new G4ParticleGun(n_particle);

    fParticleTable = G4ParticleTable::GetParticleTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::ReadInMuonFile()
{
    fMuonFile = new ifstream;
    fMuonFile->open(fMuonFileName);


    if( !fMuonFile->is_open() ) { cout<<"couldn't find muon file... bye "<<endl; return;}
    else cout<<"Loading this muon file: "<<fMuonFileName<<endl;

    cout<<"Starting at line: "<<fStartEvent<<endl;
    cout<<"Will read ";
    if (fNevents >= 0)
	cout<<fNevents;
    else
	cout<<"all";
    cout<<" entries in the file."<<endl;

    // skip entries if asked for
    if (fStartEvent > 0) {
	char buffer[256];
	for (int i = 0; i < fStartEvent; ++i) {
	    fMuonFile->getline(buffer, 255);
	}
    }


    Muon_t muon;
    G4int read_entries = 0;
    while(true){
	*fMuonFile>>muon.entry>>muon.pdg>>muon.energy
		  >>muon.posX>>muon.posY>>muon.posZ
		  >>muon.cosX>>muon.cosY>>muon.cosZ;
	if( fMuonFile->eof() ) break;

	fMuons.push_back(muon);
	++read_entries;
	if (fNevents >= 0 && read_entries >= fNevents)
	    break;
    }

    cout<<"Read "<<read_entries<<" entries."<<endl;
    delete fMuonFile;
    fMuonFile = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

    G4int eventNo = anEvent->GetEventID();


    if (!fMuonFile) {
	G4ThreeVector pos(0.,0.,0.);

	fParticleGun->
	    SetParticleDefinition(fParticleTable->FindParticle(2112));

	fParticleGun->SetParticlePosition(pos);
	fParticleGun->SetParticleEnergy(20.*CLHEP::MeV);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,-1));

	fParticleGun->GeneratePrimaryVertex(anEvent);

	return;
    }

    Muon_t muon;

    G4int size = fMuons.size();
    G4int entry = eventNo % size;

    muon = fMuons[entry];


    G4ThreeVector pos(muon.posX, muon.posY, muon.posZ);
    pos = pos * CLHEP::cm;

    fParticleGun->
	SetParticleDefinition(fParticleTable->FindParticle(muon.pdg));

    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleEnergy(muon.energy*CLHEP::GeV);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(muon.cosX, muon.cosY, muon.cosZ));

    fParticleGun->GeneratePrimaryVertex(anEvent);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
