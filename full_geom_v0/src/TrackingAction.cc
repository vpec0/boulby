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
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "TrackingAction.hh"

#include "DetectorConstruction.hh"
#include "AnaManager.hh"

#include "G4RunManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4Track.hh"
#include "G4Positron.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(DetectorConstruction* det, AnaManager* anam)
    :G4UserTrackingAction(),detector(det), mAnaM(anam)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track* trk)
{
    // Check if this is a neutron
    if (trk->GetParticleDefinition()->GetPDGEncoding() != 2112) return;

    if ( mAnaM->mEvent.n_neutrons >= AnaTree::MAX_NEUTRONS ) {
	// we don't have enough capacity allocated to store more
	// print out a warning message and don't record the neutron
	if (mAnaM->mWarningMessageCount < 1) {
	    std::cerr<<"WARNING: TrackingAction::PreUserTrackingAction(): "
		     <<"run "<<mAnaM->mEvent.runNo
		     <<" event "<<mAnaM->mEvent.eventNo
		     <<"reached a hardcoded limit of neutrons to be stored: "
		     << AnaTree::MAX_NEUTRONS << std::endl
		     <<"         Will not store any more neutrons in this event ("
		     <<mAnaM->mEvent.runNo<<", "
		     <<mAnaM->mEvent.eventNo<<")." << std::endl;
	    mAnaM->mWarningMessageCount++;
	}
	return;
    }

    // count the neutron and record its initial energy, start
    // position, end position, creation process, and parent id
    int n = mAnaM->mEvent.n_neutrons;
    mAnaM->mEvent.n_energy[n] = trk->GetKineticEnergy() / CLHEP::MeV;
    mAnaM->mEvent.start_xyz[n][0] = trk->GetVertexPosition().getX()/CLHEP::cm;
    mAnaM->mEvent.start_xyz[n][1] = trk->GetVertexPosition().getY()/CLHEP::cm;
    mAnaM->mEvent.start_xyz[n][2] = trk->GetVertexPosition().getZ()/CLHEP::cm;

    mAnaM->mEvent.process->push_back(trk->GetCreatorProcess()->GetProcessName());
    mAnaM->mEvent.trackId[n] = trk->GetTrackID();
    mAnaM->mEvent.parentId[n] = trk->GetParentID();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* trk)
{
    // Check if this is a neutron
    if (trk->GetParticleDefinition()->GetPDGEncoding() != 2112) return;
    if ( mAnaM->mEvent.n_neutrons >= AnaTree::MAX_NEUTRONS ) {
	// we don't have enough capacity allocated to store more
	mAnaM->mEvent.n_neutrons_total++;
	return;
    }

    int n = mAnaM->mEvent.n_neutrons;

    mAnaM->mEvent.end_xyz[n][0] = trk->GetPosition().getX()/CLHEP::cm;
    mAnaM->mEvent.end_xyz[n][1] = trk->GetPosition().getY()/CLHEP::cm;
    mAnaM->mEvent.end_xyz[n][2] = trk->GetPosition().getZ()/CLHEP::cm;

    mAnaM->mEvent.n_neutrons_total++;
    mAnaM->mEvent.n_neutrons++;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
