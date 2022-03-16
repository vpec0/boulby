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
/// \file example.cc
/// \brief Main program of the  example

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

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
 * Input parameters (if macro given, then generator file needs to be supplied):
 *   1: string;  run macro file
 *   2: string;  generator file
 *   3: string;  output file
 *   4: integer; Start run
 *   5: integer; start event from which to read in from muon flux file
 *   6: integer; number of events to read in from the flux file
 *   7: integer; set an event offset to be able to start in the middle of the same run, for reproducibility
 **/
{
  std::cout<<">>> simple_geom: begin."<<std::endl;
  // Detect interactive mode (if no arguments) and define UI session
  //
  bool doVis = false;
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
      //doVis = true;
      ui = new G4UIExecutive(argc, argv);
      // std::cout<<"### Opening UI "<<std::endl;
  }

  if (argc == 2) { //
      std::cerr<<"error, when macro file supplied, path to muon file needs to be provided too"<<std::endl;
      return -1;
  }

  G4String muon_file_name = "";
  if (argc > 2) {
      muon_file_name = argv[2];
  }

  std::cout<<">>> full_geom_v0: deal with random number settings..."<<std::endl;
  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  G4Random::showEngineStatus();
  int startRun = 2000010;
  if (argc>4)
      sscanf(argv[4], "%d", &startRun);
  std::cout<<">>> full_geom_v0: Will use "<<startRun<<" start number in run sequence."<<std::endl;


  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
  std::cout<<"Running in multi-thread mode."<<std::endl;
#else
  G4RunManager* runManager = new G4RunManager;
  std::cout<<"Running in single-thread mode."<<std::endl;
#endif


  runManager->SetRunIDCounter(startRun);

  // Set mandatory initialization classes
  //
  DetectorConstruction* detconst = new DetectorConstruction();
  runManager->SetUserInitialization(detconst);

  // Physics list
  G4VModularPhysicsList* physicsList = new Shielding;
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);

  TString outfname("");
  AnaManager* anam = 0;
  if (argc>3) {
      outfname = argv[3];
      anam = new AnaManager(outfname);
  } else
      anam = new AnaManager();

  anam->Book();

  // User action initialization
  auto uai = new ActionInitialization(detconst, muon_file_name);
  // set parameters for reading in from the muon flux file
  G4int startEvent = 0;
  if (argc > 5)
      sscanf(argv[5], "%d", &startEvent);
  uai->SetStartEvent(startEvent);
  G4int nEvents = -1; // default read all
  if (argc > 6)
      sscanf(argv[6], "%d", &nEvents);
  uai->SetNevents(nEvents);
  G4int eventOffset = 0;
  if (argc > 7)
      sscanf(argv[7], "%d", &eventOffset);
  uai->SetEventOffset(eventOffset);

  runManager->SetUserInitialization(uai);


  // Initialize visualization
  //
  // G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  if (ui) { // this is an interactive mode, allow visualisation
  //if (doVis) { // this is an interactive mode, allow visualisation
      G4cout<<"### Creating vis manager"<<G4endl;
      G4VisManager* visManager = new G4VisExecutive();
      visManager->Initialize();
  }

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) {
      //if ( ! doVis ) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode
    G4cout<<"### About to execute init_vis.mac"<<G4endl;
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
