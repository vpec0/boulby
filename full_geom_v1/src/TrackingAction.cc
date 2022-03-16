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

TrackingAction::TrackingAction()
    :G4UserTrackingAction()
{
    detector = (DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    fAnaM = AnaManager::GetManager();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track* trk)
{
    // Check if this is a neutron
    if (trk->GetParticleDefinition()->GetPDGEncoding() != 2112) return;

    if ( fAnaM->fEvent.n_neutrons >= AnaTree::MAX_NEUTRONS ) {
	// we don't have enough capacity allocated to store more
	// print out a warning message and don't record the neutron
	if (fAnaM->fWarningMessageCount < 1) {
	    std::cerr<<"WARNING: TrackingAction::PreUserTrackingAction(): "
		     <<"run "<<fAnaM->fEvent.runNo
		     <<" event "<<fAnaM->fEvent.eventNo
		     <<"reached a hardcoded limit of neutrons to be stored: "
		     << AnaTree::MAX_NEUTRONS << std::endl
		     <<"         Will not store any more neutrons in this event ("
		     <<fAnaM->fEvent.runNo<<", "
		     <<fAnaM->fEvent.eventNo<<")." << std::endl;
	    fAnaM->fWarningMessageCount++;
	}
	return;
    }

    // count the neutron and record its initial energy, start
    // position, end position, creation process, and parent id
    int n = fAnaM->fEvent.n_neutrons;
    fAnaM->fEvent.n_energy[n] = trk->GetKineticEnergy() / CLHEP::MeV;
    fAnaM->fEvent.start_xyz[n][0] = trk->GetVertexPosition().getX()/CLHEP::cm;
    fAnaM->fEvent.start_xyz[n][1] = trk->GetVertexPosition().getY()/CLHEP::cm;
    fAnaM->fEvent.start_xyz[n][2] = trk->GetVertexPosition().getZ()/CLHEP::cm;

    //std::cout<<"Neutron process vector size: "<<fAnaM->fEvent.process->size()<<std::endl;
    std::string process_name = "primary";
    if (trk->GetParentID() != 0)
	process_name = trk->GetCreatorProcess()->GetProcessName();
    fAnaM->fEvent.process->push_back(process_name);
    fAnaM->fEvent.trackId[n] = trk->GetTrackID();
    fAnaM->fEvent.parentId[n] = trk->GetParentID();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* trk)
{
    int pdg = trk->GetParticleDefinition()->GetPDGEncoding();
    int trkId = trk->GetTrackID();
    int mother = trk->GetParentID();
    // store primary muon last position
    if (mother == 0) {
	// this is a primary particle
	fAnaM->fEvent.mu_endx = trk->GetPosition().getX()/CLHEP::cm;
	fAnaM->fEvent.mu_endy = trk->GetPosition().getY()/CLHEP::cm;
	fAnaM->fEvent.mu_endz = trk->GetPosition().getZ()/CLHEP::cm;

	fAnaM->fEvent.mu_len = trk->GetTrackLength()/CLHEP::cm;
    }

    // Check if this is a neutron
    if (pdg != 2112) return;
    if ( fAnaM->fEvent.n_neutrons >= AnaTree::MAX_NEUTRONS ) {
	// we don't have enough capacity allocated to store more
	fAnaM->fEvent.n_neutrons_total++;
	return;
    }

    int n = fAnaM->fEvent.n_neutrons;

    fAnaM->fEvent.end_xyz[n][0] = trk->GetPosition().getX()/CLHEP::cm;
    fAnaM->fEvent.end_xyz[n][1] = trk->GetPosition().getY()/CLHEP::cm;
    fAnaM->fEvent.end_xyz[n][2] = trk->GetPosition().getZ()/CLHEP::cm;

    fAnaM->fEvent.n_neutrons_total++;
    fAnaM->fEvent.n_neutrons++;


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
