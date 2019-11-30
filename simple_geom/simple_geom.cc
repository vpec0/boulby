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
/// \file examplesimple_geom_.cc
/// \brief Main program of the simple_geom_ example

#include "simple_geom_DetectorConstruction.hh"
#include "simple_geom_ActionInitialization.hh"

#undef G4MULTITHREADED
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "Shielding.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

#include "AnaManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
/**
 * Input parameters:
 *   1: string;  run macro file
 *   2: string;  output file
 *   3: integer; Start run
 *   4: string;  active material
 **/
{
  std::cout<<">>> simple_geom: begin."<<std::endl;
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  std::cout<<">>> simple_geom: deal with random number settings..."<<std::endl;
  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  G4Random::showEngineStatus();
  int startRun = 2000010;
  if (argc>3)
      sscanf(argv[3], "%d", &startRun);
  std::cout<<">>> simple_geom: Will use "<<startRun<<" start number in run sequence."<<std::endl;


  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif


  runManager->SetRunIDCounter(startRun);

  // Set mandatory initialization classes
  //
  // Detector construction
  G4String material = "polyethylene_standard";
  if (argc > 4)
      material = argv[4];
  simple_geom_DetectorConstruction* detconst = new simple_geom_DetectorConstruction(material);
  runManager->SetUserInitialization(detconst);

  // Physics list
  G4VModularPhysicsList* physicsList = new Shielding;
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);

  TString outfname("");
  AnaManager* anam = 0;
  if (argc>2) {
      outfname = argv[2];
      anam = new AnaManager(outfname);
  } else
      anam = new AnaManager();

  anam->Book();

  // User action initialization
  runManager->SetUserInitialization(new simple_geom_ActionInitialization(detconst, anam));

  // Initialize visualization
  //
  // G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  if (ui) { // this is an interactive mode, allow visualisation
      G4VisManager* visManager = new G4VisExecutive("Quiet");
      visManager->Initialize();
  }

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  anam->Save();

  delete anam;
  //delete visManager;
  delete runManager;
  std::cout<<">>> simple_geom: end."<<std::endl;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
