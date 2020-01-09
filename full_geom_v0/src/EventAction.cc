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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

#include "AnaManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction, AnaManager* anam)
: G4UserEventAction(),
  fRunAction(runAction),
  fEdep(0.),
  mAnaM(anam)
{
    fRunVerboseLevel = G4RunManager::GetRunManager()->GetVerboseLevel();
    G4cout<<"EventAction: Setting Run Verbose Level to: "<<fRunVerboseLevel<<G4endl;
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
  mAnaM->Reset();
  mAnaM->mEvent.runNo = fRunAction->mRunID;
  mAnaM->mEvent.eventNo = evt->GetEventID();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event*)
{
  // accumulate statistics in run action
  fRunAction->AddEdep(fEdep);
  if (mAnaM->mEvent.n_neutrons > 0)
      mAnaM -> FillTree();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
