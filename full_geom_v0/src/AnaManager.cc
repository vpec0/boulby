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
/// \file analysis/AnaEx02/src/AnaManager.cc
/// \brief Implementation of the AnaManager class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <TH1D.h>
#include <TFile.h>
#include <TTree.h>
#include <CLHEP/Units/SystemOfUnits.h>

#define ANATREE_CXX 1
#include "AnaManager.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AnaManager::AnaManager(const char* filename):
        fFileName(filename),
	fRootFile(0),
	fHisto {},
	fTree(0),
	fEabs(0.), fEgap(0.) ,fLabs(0.), fLgap(0.), mEvent(),
	mWarningMessageCount(0)
{

  // histograms
  //for (G4int k=0; k<kMaxHisto; k++) fHisto[k] = 0;

}

AnaManager::AnaManager():
    fFileName("anatree.root"),
    fRootFile(0),
    fHisto {},
    fTree(0),
    fEabs(0.), fEgap(0.) ,fLabs(0.), fLgap(0.), mEvent()
{

  // histograms
  //for (G4int k=0; k<kMaxHisto; k++) fHisto[k] = 0;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AnaManager::~AnaManager()
{
    //if (fRootFile) delete fRootFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::Book()
{
  // Creating a tree container to handle histograms and ntuples.
  // This tree is associated to an output file.
  //
  // fRootFile = new TFile(fFileName,"RECREATE");
  // if (! fRootFile) {
  //   G4cout << " AnaManager::Book :"
  //          << " problem creating the ROOT TFile "
  //          << G4endl;
  //   return;
  // }

  // // create 1st ntuple
  // fTree = AnaTree::createTree("events", mEvent);
  // // have the tree saved every 20kB
  // fTree->SetAutoSave(-20000);

  // G4cout << "\n----> Output file is open in " << fFileName << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::Save()
{
  // if (! fRootFile) return;

  // fRootFile->Write();       // Writing the histograms to the file
  // fRootFile->Close();       // and closing the tree (and the file)

  // G4cout << "\n----> Histograms and ntuples are saved\n" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::FillHisto(G4int ih, G4double xbin, G4double weight)
{
  // if (ih >= kMaxHisto) {
  //   G4cerr << "---> warning from AnaManager::FillHisto() : histo " << ih
  //          << " does not exist. (xbin=" << xbin << " weight=" << weight << ")"
  //          << G4endl;
  //   return;
  // }
  // if  (fHisto[ih]) { fHisto[ih]->Fill(xbin, weight); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::Normalize(G4int ih, G4double fac)
{
  // if (ih >= kMaxHisto) {
  //   G4cout << "---> warning from AnaManager::Normalize() : histo " << ih
  //          << " does not exist. (fac=" << fac << ")" << G4endl;
  //   return;
  // }
  // if (fHisto[ih]) fHisto[ih]->Scale(fac);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void AnaManager::FillTree()
{
    // do any event filtering here

    // fTree->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::PrintStatistic()
{
  // G4cout << "\n ----> print histograms statistic \n" << G4endl;
  // for ( G4int i=0; i<kMaxHisto; ++i ) {
  //   TH1D* h1 = fHisto[i];
  //   const G4String name = h1->GetName();

  //   G4String unitCategory;
  //   if (name[0] == 'E' ) unitCategory = "Energy";
  //   if (name[0] == 'L' ) unitCategory = "Length";

  //   G4cout << name
  //          << ": mean = " << G4BestUnit(h1->GetMean(), unitCategory)
  //          << " rms = " << G4BestUnit(h1->GetRMS(), unitCategory )
  //          << G4endl;
  // }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnaManager::Reset()
{
    AnaTree::resetEvent(mEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
