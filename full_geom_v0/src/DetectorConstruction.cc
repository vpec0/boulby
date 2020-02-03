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
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"

#include "Geometry.hh"

using namespace Geometry;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction(G4String material_name)
: G4VUserDetectorConstruction(),
  fScoringVolume(0),
  fMaterial(material_name)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Get nist material manager
    G4NistManager* nist = G4NistManager::Instance();
    // Option to switch on/off checking of volumes overlaps
    //
    G4bool checkOverlaps = false;

    // prepare materials which are not in the nist tables by default
    PrepareMaterials(nist);

    //
    // World
    //
    G4Material* world_mat = nist->FindOrBuildMaterial(material_map.at(WORLD_MATERIAL));
    G4Box* solidWorld =
	new G4Box("World",
		  0.5*WORLD_W, 0.5*WORLD_W, 0.5*WORLD_H );
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
    // Hall
    //
    G4Material* hall_mat = nist->FindOrBuildMaterial(material_map.at(HALL_MATERIAL));
    G4Box* solidHall =
	new G4Box("Hall",
		  0.5*HALL_W, 0.5*HALL_W, 0.5*HALL_H );
    G4LogicalVolume* logicHall =
	new G4LogicalVolume(solidHall,          //its solid
			    hall_mat,           //its material
			    "Hall");            //its name
    G4VPhysicalVolume* physHall =
	new G4PVPlacement(0,                     //no rotation
			  G4ThreeVector(),       //at (0,0,0)
			  logicHall,            //its logical volume
			  "Hall",               //its name
			  logicWorld,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking


    //
    // Outer Cryostat Vessel
    //

    G4Material* ocv_mat = nist->FindOrBuildMaterial(material_map.at(OCV_MATERIAL));
    G4Tubs* solidOcv =
	new G4Tubs("OCV",
		   0., 0.5*OCV_D, 0.5*OCV_H, 0., 360.*deg );
    G4LogicalVolume* logicOcv =
	new G4LogicalVolume(solidOcv,          //its solid
			    ocv_mat,           //its material
			    "OCV");            //its name
    G4VPhysicalVolume* physOcv =
	new G4PVPlacement(0,                     //no rotation
			  G4ThreeVector(),       //at (0,0,0)
			  logicOcv,            //its logical volume
			  "OCV",               //its name
			  logicHall,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking


    //
    // Vacuum between cryostat walls
    //

    G4Material* vac_mat = nist->FindOrBuildMaterial("G4_Ti");
    G4Tubs* solidVac =
	new G4Tubs("VAC",
		   0., 0.5*VAC_D, 0.5*VAC_H, 0., 360.*deg );
    G4LogicalVolume* logicVac =
	new G4LogicalVolume(solidVac,          //its solid
			    vac_mat,           //its material
			    "VAC");            //its name
    G4VPhysicalVolume* physVac =
	new G4PVPlacement(0,                     //no rotation
			  G4ThreeVector(),       //at (0,0,0)
			  logicVac,            //its logical volume
			  "VAC",               //its name
			  logicOcv,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking

    //
    // Inner Cryostat Vessel
    //

    G4Material* icv_mat = nist->FindOrBuildMaterial(material_map.at(ICV_MATERIAL));
    G4Tubs* solidIcv =
	new G4Tubs("ICV",
		   0., 0.5*ICV_D, 0.5*ICV_H, 0., 360.*deg );
    G4LogicalVolume* logicIcv =
	new G4LogicalVolume(solidIcv,          //its solid
			    icv_mat,           //its material
			    "ICV");            //its name
    G4VPhysicalVolume* physIcv =
	new G4PVPlacement(0,                     //no rotation
			  G4ThreeVector(),       //at (0,0,0)
			  logicIcv,            //its logical volume
			  "ICV",               //its name
			  logicVac,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking


    //
    // Inner detector envelope
    // - contains upper "dome" part filled with GXe and lower "skin" part with LXe
    //

    G4Material* id_env_mat = nist->FindOrBuildMaterial(material_map.at(ID_ENV_MATERIAL));
    G4Tubs* solidId_Env =
	new G4Tubs("ID_ENV",
		   0., 0.5*ID_ENV_D, 0.5*ID_ENV_H, 0., 360.*deg );
    G4LogicalVolume* logicId_Env =
	new G4LogicalVolume(solidId_Env,          //its solid
			    id_env_mat,           //its material
			    "ID_ENV");            //its name
    G4VPhysicalVolume* physId_Env =
	new G4PVPlacement(0,                     //no rotation
			  G4ThreeVector(),       //at (0,0,0)
			  logicId_Env,            //its logical volume
			  "ID_ENV",               //its name
			  logicHall,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking

    //
    // Dome - GXe top part in the cryostat
    //
    G4Material* dome_mat = nist->FindOrBuildMaterial(material_map.at(DOME_MATERIAL));
    G4Tubs* solidDome =
	new G4Tubs("Dome",
		   0., 0.5*DOME_D - ENV_SPACING, 0.5*DOME_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicDome =
	new G4LogicalVolume(solidDome,          //its solid
			    dome_mat,           //its material
			    "Dome");            //its name
    G4VPhysicalVolume* physDome =
	new G4PVPlacement(0,                     //no rotation
			  G4ThreeVector(),       //at (0,0,0)
			  logicDome,            //its logical volume
			  "Dome",               //its name
			  logicId_Env,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking

    //
    // top PMT array
    //

    G4Material* top_pmt_mat = nist->FindOrBuildMaterial(material_map.at(PMT_ARR_MATERIAL));
    G4Tubs* solidTop_Pmt =
	new G4Tubs("Top_PMT",
		   0., 0.5*PMT_ARR_D, 0.5*PMT_ARR_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicTop_Pmt =
	new G4LogicalVolume(solidTop_Pmt,          //its solid
			    top_pmt_mat,           //its material
			    "Top_PMT");            //its name
    G4VPhysicalVolume* physTop_Pmt =
	new G4PVPlacement(0,                     //no rotation
			  G4ThreeVector(),       //at (0,0,0)
			  logicTop_Pmt,            //its logical volume
			  "Top_PMT",               //its name
			  logicDome,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking

    //
    // top PTFE
    //

    G4Material* top_ptfe_mat = nist->FindOrBuildMaterial(material_map.at(PTFE_MATERIAL));
    G4Tubs* solidTop_Ptfe =
	new G4Tubs("Top_PTFE",
		   0., 0.5*PTFE_T_D, 0.5*PTFE_T_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicTop_Ptfe =
	new G4LogicalVolume(solidTop_Ptfe,          //its solid
			    top_ptfe_mat,           //its material
			    "Top_PTFE");            //its name
    G4VPhysicalVolume* physTop_Ptfe =
	new G4PVPlacement(0,                     //no rotation
			  G4ThreeVector(),       //at (0,0,0)
			  logicTop_Ptfe,            //its logical volume
			  "Top_PTFE",               //its name
			  logicDome,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking


    //
    // LXe Skin
    //
    G4Material* skin_mat = nist->FindOrBuildMaterial("G4_lXe");
    G4Tubs* solidSkin =
	new G4Tubs("Skin",
		   0., 0.5*SKIN_D - ENV_SPACING, 0.5*SKIN_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicSkin =
	new G4LogicalVolume(solidSkin,          //its solid
			    skin_mat,           //its material
			    "Skin");            //its name
    G4VPhysicalVolume* physSkin =
	new G4PVPlacement(0,                     //no rotation
			  G4ThreeVector(),       //at (0,0,0)
			  logicSkin,            //its logical volume
			  "Skin",               //its name
			  logicId_Env,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking


  //
  //always return the physical World
  //
  return physWorld;
}


void DetectorConstruction::PrepareMaterials(G4NistManager* nist)
{
    // create PTFE
    const std::vector<G4String> elements = {"G4_F", "G4_C"};
    const std::vector<G4int> nAtoms = {4, 2};
    const G4double density = 2.2*g/cm3;
    nist->ConstructNewMaterial("PTFE", elements, nAtoms, density);

    // Steel5 - 5% of original steel density (0.05 * 8 g/cm3)
    G4Material* steel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    nist->BuildMaterialWithNewDensity("Steel5", "G4_STAINLESS-STEEL", 0.05*steel->GetDensity());

    // taken from https://github.com/claudiok/clsim/blob/master/private/geant4/TrkDetectorConstruction.cxx
    //Standard Rock (approximation with a composition of individual elements.
    //Geant4 does not play well with average atomic weights, etc.)
    //This is not too different from Gran Sasso rock, which
    //in turn should not be too different from standard rock. The density of 2.65g/cm^3
    //is the original "standard rock" density given by the full PDG report.
    auto StdRock = new G4Material("StdRock",2.65*g/cm3,4, kStateSolid );
    StdRock->AddElement(nist->FindOrBuildElement("G4_O"),  52.*perCent);
    StdRock->AddElement(nist->FindOrBuildElement("G4_Ca"), 27.*perCent);
    StdRock->AddElement(nist->FindOrBuildElement("G4_C"),  12.*perCent);
    StdRock->AddElement(nist->FindOrBuildElement("G4_Mg"),  9.*perCent);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
