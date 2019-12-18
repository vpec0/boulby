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
/// \file simple_geom_DetectorConstruction.cc
/// \brief Implementation of the simple_geom_DetectorConstruction class

#include "simple_geom_DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

simple_geom_DetectorConstruction::simple_geom_DetectorConstruction(G4String material_name)
: G4VUserDetectorConstruction(),
  fScoringVolume(0),
  fMaterial(material_name)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

simple_geom_DetectorConstruction::~simple_geom_DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* simple_geom_DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Geometrical parameters for active regions
  //
  G4double box_sizeXY = 20*m;
  G4double box_sizeZ = 40*m;

  G4double density = 1.0*g/cm3;
  G4Material* box_mat = 0;

  std::map<G4String, G4int> sw_map = {
      {"polyethylene_standard", 1},
      {"plastic_sc", 2},
      {"ch2", 3},
      {"water", 4},
      {"lead", 5},
      {"toluene", 6},
      {"salt", 7},
      {"iron", 8},
      {"copper", 9},
      {"mangan", 10},
      {"magnesium", 11}
  };

  switch(sw_map[fMaterial]) {
  case 3 :
      box_mat = new G4Material("CH2", density, 2);
      box_mat->AddElement(nist->FindOrBuildElement("C"), 1);
      box_mat->AddElement(nist->FindOrBuildElement("H"), 2);
      break;

  case 2 :
      box_mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
      break;

  case 1 :
      box_mat = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
      break;

  case 4 :
      box_mat = nist->FindOrBuildMaterial("G4_WATER");
      break;

  case 5:
      box_mat = nist->FindOrBuildMaterial("G4_Pb");
      // box_mat = new G4Material("Lead", density, 2);
      // box_mat->AddElement(nist->FindOrBuildElement("G4_Pb"), 1);
      break;

  case 7 :
      box_mat = new G4Material("Salt", density, 2);
      box_mat->AddElement(nist->FindOrBuildElement("Na"), 1);
      box_mat->AddElement(nist->FindOrBuildElement("Cl"), 1);
      break;

  case 8:
      box_mat = nist->FindOrBuildMaterial("G4_Fe");
      break;

  case 9:
      box_mat = nist->FindOrBuildMaterial("G4_Cu");
      break;

  case 10:
      box_mat = nist->FindOrBuildMaterial("G4_Mn");
      break;

  case 11:
      box_mat = nist->FindOrBuildMaterial("G4_Mg");
      break;

  default:
      std::cout<<"WARNING: simple_geom_DetectorConstruction::Construct(): Material "
	       <<fMaterial<<" not defined, will use polyethylene."<<std::endl;
      box_mat = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  }

  box_mat = new G4Material("Custom", density, box_mat);



  // Envelope parameters
  //
  G4double env_sizeXY = 1.1*box_sizeXY, env_sizeZ = 1.1*box_sizeZ;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_Galactic");

  // World parameters
  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");


  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //

  G4Box* solidWorld =
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size

  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name

  G4VPhysicalVolume* physWorld =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  //
  // Envelope
  //
  G4Box* solidEnv =
    new G4Box("Envelope",                    //its name
        0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ); //its size

  G4LogicalVolume* logicEnv =
    new G4LogicalVolume(solidEnv,            //its solid
                        env_mat,             //its material
                        "Envelope");         //its name

  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking

  //
  // Shape 1
  //
  G4Box* solidBox =
    new G4Box("Box",
	      0.5*box_sizeXY, 0.5*box_sizeXY, 0.5*box_sizeZ);


  // Defining the active region box

  G4ThreeVector pos1 = G4ThreeVector();

  G4LogicalVolume* logicBox =
    new G4LogicalVolume(solidBox,         //its solid
                        box_mat,          //its material
                        "Box");           //its name

  new G4PVPlacement(0,                       //no rotation
                    pos1,                    //at position
                    logicBox,             //its logical volume
                    "Box",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking

  // Set Shape2 as scoring volume
  //
  fScoringVolume = logicBox;

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
