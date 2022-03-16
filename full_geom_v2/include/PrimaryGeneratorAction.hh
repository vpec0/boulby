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
/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "AnaManager.hh"

class G4ParticleGun;
class G4Event;
class G4Box;

/// The primary generator action class with particle gun.
///
/// The default kinematic is a 6 MeV gamma, randomly distribued
/// in front of the phantom across 80% of the (X,Y) phantom size.

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    PrimaryGeneratorAction(const char* fname, G4int nevents, G4int startevent);

    virtual ~PrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

    // setters and getters
    void  SetNevents(G4int n) { fNevents = n; }
    G4int GetNevents() { return fNevents; }

    void  SetStartEvent(G4int n) { fStartEvent = n; }
    G4int GetStartEvent() { return fStartEvent; }

    void  SetEventOffset(G4int n) { fEventOffset = n; }
    G4int GetEventOffset() { return fEventOffset; }

    void     SetMuonFileName(G4String s) { fMuonFileName = s; }
    G4String GetMuonFileName() { return fMuonFileName; }

    void ReadInMuonFile();

private:
    struct Muon_t {
	int entry, pdg;
	double energy,
	    posX, posY, posZ,
	    cosX, cosY, cosZ;
    };


private:
    G4int fNevents;
    G4int fEventOffset; /// offset in global G4Event ID
    G4int fStartEvent; /// Where to start when reading in muons from input muon file
    G4String fMuonFileName;
    std::ifstream* fMuonFile;

    G4ParticleGun*  fParticleGun; /// pointer a to G4 gun class
    G4Box* fEnvelopeBox;
    std::vector<Muon_t> fMuons;

    G4ParticleTable* fParticleTable;

    AnaManager* fAnaM;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
