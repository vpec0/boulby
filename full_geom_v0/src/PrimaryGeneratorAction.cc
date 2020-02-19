//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

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

    Muon_t muon;

    G4int size = fMuons.size();
    G4int entry = eventNo % size;

    muon = fMuons[entry];


    G4ThreeVector pos(muon.posX, muon.posY, muon.posZ);


    fParticleGun->
	SetParticleDefinition(fParticleTable->FindParticle(muon.pdg));

    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleEnergy(muon.energy);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(muon.cosX, muon.cosZ, muon.cosZ));

    fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
