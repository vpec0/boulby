#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4SDManager.hh"


#include "Geometry.hh"
#include "SD.hh"
#include "SDExternal.hh"


using namespace Geometry;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0)
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
    G4bool checkOverlaps = true;

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
    // Rock
    //
    G4Material* rock_mat = nist->FindOrBuildMaterial(material_map.at(ROCK_MATERIAL));
    G4Box* solidRock =
	new G4Box("Rock",
		  0.5*ROCK_W, 0.5*ROCK_W, 0.5*ROCK_H );
    G4LogicalVolume* logicRock =
	new G4LogicalVolume(solidRock,          //its solid
			    rock_mat,           //its material
			    "Rock");            //its name
    G4ThreeVector posRock(ROCK_X,ROCK_Y,ROCK_Z);
    G4VPhysicalVolume* physRock =
	new G4PVPlacement(0,                     //no rotation
			  posRock,       //at (0,0,0)
			  logicRock,            //its logical volume
			  "Rock",               //its name
			  logicWorld,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"ROCK_X : "<<ROCK_X
	     <<", ROCK_Y : "<<ROCK_Y
	     <<", ROCK_Z : "<<ROCK_Z
	     <<std::endl;

    std::cout<<"ROCK_W : "<<ROCK_W<<", ROCK_H : "<<ROCK_H<<std::endl;

    // print rock thickness on all sides
    std::cout<<"Rock Top : "<<ROCK_SPACE_TOP
	     <<", Rock Bottom : "<<ROCK_SPACE_BOTTOM
	     <<", Rock Side : "<<ROCK_SPACE_SIDE
	     <<std::endl;

    //
    // Hall
    //
    G4Material* hall_mat = nist->FindOrBuildMaterial(material_map.at(HALL_MATERIAL));
    G4Tubs* solidHall =
	new G4Tubs("Hall",
		   0., 0.5*HALL_D, 0.5*HALL_H, 0., 360. );
    G4LogicalVolume* logicHall =
	new G4LogicalVolume(solidHall,          //its solid
			    hall_mat,           //its material
			    "Hall");            //its name
    G4ThreeVector posHall(HALL_X-ROCK_X,HALL_Y-ROCK_Y,HALL_Z-ROCK_Z);
    G4VPhysicalVolume* physHall =
	new G4PVPlacement(0,                     //no rotation
			  posHall,       //at (0,0,0)
			  logicHall,            //its logical volume
			  "Hall",               //its name
			  logicRock,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"HALL_X : "<<HALL_X
	     <<", HALL_Y : "<<HALL_Y
	     <<", HALL_Z : "<<HALL_Z
	     <<std::endl;
    std::cout<<"HALL_D : "<<HALL_D<<", HALL_H : "<<HALL_H<<std::endl;

    std::cout<<"Space above WT: "<<(HALL_Z+0.5*HALL_H - WT_Z - 0.5*WT_H)<<std::endl
	     <<"Space on the sides of WT - X: "
	     <<(HALL_X+0.5*HALL_D - WT_X - 0.5*WT_D)<<std::endl
	     <<"Space on the sides of WT - Y: "
	     <<(HALL_Y+0.5*HALL_D - WT_Y - 0.5*WT_D)<<std::endl;


    //
    // steel support shielding
    //
    G4Material* steel_mat = nist->FindOrBuildMaterial(material_map.at(STEEL_MATERIAL));
    G4Tubs* solidSteel =
	new G4Tubs("Steel",
		   0., 0.5*STEEL_D, 0.5*STEEL_H-ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicSteel =
	new G4LogicalVolume(solidSteel,          //its solid
			    steel_mat,           //its material
			    "Steel");            //its name
    G4ThreeVector posSteel(STEEL_X-HALL_X,STEEL_Y-HALL_Y,STEEL_Z-HALL_Z);
    G4VPhysicalVolume* physSteel =
	new G4PVPlacement(0,                     //no rotation
			  posSteel,       //at (0,0,0)
			  logicSteel,            //its logical volume
			  "Steel",               //its name
			  logicHall,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"STEEL_Z : "<<STEEL_Z<<", STEEL_H : "<<STEEL_H<<", STEEL_D : "<<STEEL_D<<std::endl;


    //
    // Water Tank
    //
    G4Material* wt_mat = nist->FindOrBuildMaterial(material_map.at(WT_MATERIAL));
    G4Tubs* solidWt =
	new G4Tubs("WT",
		   0., 0.5*WT_D, 0.5*WT_H, 0., 360.*deg );
    G4LogicalVolume* logicWt =
	new G4LogicalVolume(solidWt,          //its solid
			    wt_mat,           //its material
			    "WT");            //its name
    G4ThreeVector posWt(WT_X-HALL_X,WT_Y-HALL_Y,WT_Z-HALL_Z);
    G4VPhysicalVolume* physWt =
	new G4PVPlacement(0,                     //no rotation
			  posWt,       //at (0,0,0)
			  logicWt,            //its logical volume
			  "WT",               //its name
			  logicHall,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"WT_X : "<<WT_X
	     <<", WT_Y : "<<WT_Y
	     <<", WT_Z : "<<WT_Z
	     <<std::endl;
    std::cout<<"WT_H : "<<WT_H<<", WT_D : "<<WT_D<<std::endl;

    //
    // Gd Doped Liquid Scintillator
    //
    G4Material* gdls_mat = nist->FindOrBuildMaterial(material_map.at(GDLS_MATERIAL));
    G4Tubs* solidGdls =
	new G4Tubs("GdLS",
		   0., 0.5*GDLS_D, 0.5*GDLS_H, 0., 360.*deg );
    G4LogicalVolume* logicGdls =
	new G4LogicalVolume(solidGdls,          //its solid
			    gdls_mat,           //its material
			    "GdLS");            //its name
    G4ThreeVector posGdls(GDLS_X-WT_X,GDLS_Y-WT_Y,GDLS_Z-WT_Z);
    G4VPhysicalVolume* physGdls =
	new G4PVPlacement(0,                     //no rotation
			  posGdls,       //at (0,0,0)
			  logicGdls,            //its logical volume
			  "GdLS",               //its name
			  logicWt,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"GDLS_Z : "<<GDLS_Z<<", GDLS_H : "<<GDLS_H<<", GDLS_D : "<<GDLS_D<<std::endl;


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
    G4ThreeVector posOcv(OCV_X-GDLS_X,OCV_Y-GDLS_Y,OCV_Z-GDLS_Z);
    G4VPhysicalVolume* physOcv =
	new G4PVPlacement(0,                     //no rotation
			  posOcv,       //at (0,0,0)
			  logicOcv,            //its logical volume
			  "OCV",               //its name
			  logicGdls,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"OCV_Z : "<<OCV_Z<<", OCV_H : "<<OCV_H<<", OCV_D : "<<OCV_D<<std::endl;


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
    G4ThreeVector posVac(VAC_X-OCV_X,VAC_Y-OCV_Y,VAC_Z-OCV_Z);
    G4VPhysicalVolume* physVac =
	new G4PVPlacement(0,                     //no rotation
			  posVac,       //at (0,0,0)
			  logicVac,            //its logical volume
			  "VAC",               //its name
			  logicOcv,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"VAC_Z : "<<VAC_Z<<", VAC_H : "<<VAC_H<<", VAC_D : "<<VAC_D<<std::endl;

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
    G4ThreeVector posIcv(ICV_X-VAC_X,ICV_Y-VAC_Y,ICV_Z-VAC_Z);
    G4VPhysicalVolume* physIcv =
	new G4PVPlacement(0,                     //no rotation
			  posIcv,       //at (0,0,0)
			  logicIcv,            //its logical volume
			  "ICV",               //its name
			  logicVac,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"ICV_Z : "<<ICV_Z<<", ICV_H : "<<ICV_H<<", ICV_D : "<<ICV_D<<std::endl;


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
    G4ThreeVector posId_Env(ID_ENV_X-ICV_X,ID_ENV_Y-ICV_Y,ID_ENV_Z-ICV_Z);
    G4VPhysicalVolume* physId_Env =
	new G4PVPlacement(0,                     //no rotation
			  posId_Env,
			  logicId_Env,            //its logical volume
			  "ID_ENV",               //its name
			  logicIcv,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"ID_ENV_Z : "<<ID_ENV_Z<<", ID_ENV_H : "<<ID_ENV_H<<", ID_ENV_D : "<<ID_ENV_D<<std::endl;

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
    G4ThreeVector posDome(DOME_X-ID_ENV_X,DOME_Y-ID_ENV_Y,DOME_Z-ID_ENV_Z);
    G4VPhysicalVolume* physDome =
	new G4PVPlacement(0,                     //no rotation
			  posDome,
			  logicDome,            //its logical volume
			  "Dome",               //its name
			  logicId_Env,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"DOME_Z : "<<DOME_Z<<", DOME_H : "<<DOME_H<<std::endl;

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
    G4ThreeVector posPmt_T(PMT_T_X-DOME_X,PMT_T_Y-DOME_Y,PMT_T_Z-DOME_Z);
    G4VPhysicalVolume* physTop_Pmt =
	new G4PVPlacement(0,                     //no rotation
			  posPmt_T,
			  logicTop_Pmt,            //its logical volume
			  "Top_PMT",               //its name
			  logicDome,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"PMT_T_Z : "<<PMT_T_Z<<", PMT_ARR_H : "<<PMT_ARR_H<<std::endl;

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
    G4ThreeVector posPtfe_T(PTFE_T_X-DOME_X,PTFE_T_Y-DOME_Y,PTFE_T_Z-DOME_Z);
    G4VPhysicalVolume* physTop_Ptfe =
	new G4PVPlacement(0,                     //no rotation
			  posPtfe_T,
			  logicTop_Ptfe,            //its logical volume
			  "Top_PTFE",               //its name
			  logicDome,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checkin
    std::cout<<"PTFE_T_Z : "<<PTFE_T_Z<<", PTFE_T_H : "<<PTFE_T_H<<std::endl;


    //
    // LXe Skin
    //
    G4Material* skin_mat = nist->FindOrBuildMaterial(material_map.at(SKIN_MATERIAL));
    //std::cout<<skin_mat<<std::endl;
    G4Tubs* solidSkin =
	new G4Tubs("Skin",
		   0., 0.5*SKIN_D - ENV_SPACING, 0.5*SKIN_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicSkin =
	new G4LogicalVolume(solidSkin,          //its solid
			    skin_mat,           //its material
			    "Skin");            //its name
    G4ThreeVector posSkin(SKIN_X-ID_ENV_X,SKIN_Y-ID_ENV_Y,SKIN_Z-ID_ENV_Z);
    G4VPhysicalVolume* physSkin =
	new G4PVPlacement(0,                     //no rotation
			  posSkin,
			  logicSkin,            //its logical volume
			  "Skin",               //its name
			  logicId_Env,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"SKIN_Z : "<<SKIN_Z<<", SKIN_H : "<<SKIN_H<<std::endl;

    //
    // Tpc_Env
    //
    G4Material* tpc_env_mat = nist->FindOrBuildMaterial(material_map.at(TPC_ENV_MATERIAL));
    G4Tubs* solidTpc_Env =
	new G4Tubs("TPC_ENV",
		   0., 0.5*TPC_ENV_D, 0.5*TPC_ENV_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicTpc_Env =
	new G4LogicalVolume(solidTpc_Env,          //its solid
			    tpc_env_mat,           //its material
			    "TPC_ENV");            //its name
    G4ThreeVector posTpc_Env(TPC_ENV_X-SKIN_X,TPC_ENV_Y-SKIN_Y,TPC_ENV_Z-SKIN_Z);
    G4VPhysicalVolume* physTpc_Env =
	new G4PVPlacement(0,                     //no rotation
			  posTpc_Env,
			  logicTpc_Env,            //its logical volume
			  "TPC_ENV",               //its name
			  logicSkin,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"TPC_ENV_X : "<<TPC_ENV_X
	     <<", TPC_ENV_Y : "<<TPC_ENV_Y
	     <<", TPC_ENV_Z : "<<TPC_ENV_Z
	     <<std::endl;
    std::cout<<"TPC_ENV_H : "<<TPC_ENV_H
	     <<", TPC_ENV_D : "<<TPC_ENV_D<<std::endl;


    //
    // Side PTFE
    //
    G4Material* ptfe_S_mat = nist->FindOrBuildMaterial(material_map.at(PTFE_MATERIAL));
    G4Tubs* solidSide_PTFE =
	new G4Tubs("Side_PTFE",
		   0.5*PTFE_S_D_INNER + ENV_SPACING,
		   0.5*PTFE_S_D_OUTER - ENV_SPACING,
		   0.5*PTFE_S_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicSide_PTFE =
	new G4LogicalVolume(solidSide_PTFE,          //its solid
			    ptfe_S_mat,           //its material
			    "Side_PTFE");            //its name
    G4ThreeVector posSide_PTFE(PTFE_S_X-TPC_ENV_X,PTFE_S_Y-TPC_ENV_Y,PTFE_S_Z-TPC_ENV_Z);
    G4VPhysicalVolume* physSide_PTFE =
	new G4PVPlacement(0,                     //no rotation
			  posSide_PTFE,
			  logicSide_PTFE,            //its logical volume
			  "Side_PTFE",               //its name
			  logicTpc_Env,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"PTFE_S_Z : "<<PTFE_S_Z<<", PTFE_S_H : "<<PTFE_S_H<<std::endl;


    //
    // TPC
    //
    G4Material* tpc_mat = nist->FindOrBuildMaterial(material_map.at(TPC_MATERIAL));
    G4Tubs* solidTpc =
	new G4Tubs("TPC",
		   0., 0.5*TPC_D - ENV_SPACING, 0.5*TPC_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicTpc =
	new G4LogicalVolume(solidTpc,          //its solid
			    tpc_mat,           //its material
			    "TPC");            //its name
    G4ThreeVector posTpc(TPC_X-TPC_ENV_X,TPC_Y-TPC_ENV_Y,TPC_Z-TPC_ENV_Z);
    G4VPhysicalVolume* physTpc =
	new G4PVPlacement(0,                     //no rotation
			  posTpc,
			  logicTpc,            //its logical volume
			  "TPC",               //its name
			  logicTpc_Env,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    std::cout<<"TPC_X : "<<TPC_X
	     <<", TPC_Y : "<<TPC_Y
	     <<", TPC_Z : "<<TPC_Z
	     <<std::endl;
    std::cout<<"TPC_H : "<<TPC_H
	     <<", TPC_D : "<<TPC_D<<std::endl;


    //
    // RFR
    //
    G4Material* rfr_mat = nist->FindOrBuildMaterial(material_map.at(RFR_MATERIAL));
    G4Tubs* solidRfr =
	new G4Tubs("RFR",
		   0., 0.5*RFR_D - ENV_SPACING, 0.5*RFR_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicRfr =
	new G4LogicalVolume(solidRfr,          //its solid
			    rfr_mat,           //its material
			    "RFR");            //its name
    G4ThreeVector posRfr(RFR_X-TPC_ENV_X,RFR_Y-TPC_ENV_Y,RFR_Z-TPC_ENV_Z);
    G4VPhysicalVolume* physRfr =
	new G4PVPlacement(0,                     //no rotation
			  posRfr,
			  logicRfr,            //its logical volume
			  "RFR",               //its name
			  logicTpc_Env,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking

    //
    // PTFE
    //
    G4Material* ptfe_b_mat = nist->FindOrBuildMaterial(material_map.at(PTFE_MATERIAL));
    G4Tubs* solidBottom_Ptfe =
	new G4Tubs("Bottom_PTFE",
		   0., 0.5*PTFE_B_D - ENV_SPACING, 0.5*PTFE_B_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicBottom_Ptfe =
	new G4LogicalVolume(solidBottom_Ptfe,          //its solid
			    ptfe_b_mat,           //its material
			    "Bottom_PTFE");            //its name
    G4ThreeVector posBottom_Ptfe(PTFE_B_X-TPC_ENV_X,PTFE_B_Y-TPC_ENV_Y,PTFE_B_Z-TPC_ENV_Z);
    G4VPhysicalVolume* physBottom_Ptfe =
	new G4PVPlacement(0,                     //no rotation
			  posBottom_Ptfe,
			  logicBottom_Ptfe,            //its logical volume
			  "Bottom_PTFE",               //its name
			  logicTpc_Env,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking
    //
    // bottom PMT
    //
    G4Material* bottom_pmt_mat = nist->FindOrBuildMaterial(material_map.at(PMT_ARR_MATERIAL));
    G4Tubs* solidBottom_Pmt =
	new G4Tubs("Bottom_PMT",
		   0., 0.5*PMT_ARR_D - ENV_SPACING, 0.5*PMT_ARR_H - ENV_SPACING, 0., 360.*deg );
    G4LogicalVolume* logicBottom_Pmt =
	new G4LogicalVolume(solidBottom_Pmt,          //its solid
			    bottom_pmt_mat,           //its material
			    "Bottom_PMT");            //its name
    G4ThreeVector posBottom_Pmt(PMT_B_X-TPC_ENV_X,PMT_B_Y-TPC_ENV_Y,PMT_B_Z-TPC_ENV_Z);
    G4VPhysicalVolume* physBottom_Pmt =
	new G4PVPlacement(0,                     //no rotation
			  posBottom_Pmt,
			  logicBottom_Pmt,            //its logical volume
			  "Bottom_PMT",               //its name
			  logicTpc_Env,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking


    ConstructExternalXeTank(logicHall, checkOverlaps, nist);


    // print the table of materials
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;

    //
    //always return the physical World
    //
    return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
    SD* sd_tpc = new SD("sdTpc","tpc", AnaManager::kTpc);
    G4SDManager::GetSDMpointer()->AddNewDetector(sd_tpc);
    SetSensitiveDetector("TPC", sd_tpc);

    SD* sd_rfr = new SD("sdRfr","rfr", AnaManager::kRfr);
    G4SDManager::GetSDMpointer()->AddNewDetector(sd_rfr);
    SetSensitiveDetector("RFR", sd_rfr);

    SD* sd_skin = new SD("sdSkin","skin", AnaManager::kSkin);
    G4SDManager::GetSDMpointer()->AddNewDetector(sd_skin);
    SetSensitiveDetector("Skin", sd_skin);

    SD* sd_gdls = new SD("sdGdls","gdls", AnaManager::kGdls);
    G4SDManager::GetSDMpointer()->AddNewDetector(sd_gdls);
    SetSensitiveDetector("GdLS", sd_gdls);

    SD* sd_wt = new SD("sdWt","wt", AnaManager::kWt);
    G4SDManager::GetSDMpointer()->AddNewDetector(sd_wt);
    SetSensitiveDetector("WT", sd_wt);

    SDExternal* sd_extxe = new SDExternal("sdExternal_Xe");
    G4SDManager::GetSDMpointer()->AddNewDetector(sd_extxe);
    SetSensitiveDetector("External_Xe", sd_extxe);

}


G4VPhysicalVolume* DetectorConstruction::ConstructExternalXeTank(G4LogicalVolume* logicHall,
								 G4bool checkOverlaps,
								 G4NistManager* nist)
{
    // place an external tank somewhere in the hall
    // Goal is to measure Xe exposure to cosmogenic neutrons and activation of 1XXXe

    // PE shielding
    G4Material* pe_mat = nist->FindOrBuildMaterial(material_map.at(PE_MATERIAL));
    G4Tubs* solidPE =
	new G4Tubs("PE",
		   0., 0.5*EXTERNAL_PE_D, 0.5*EXTERNAL_PE_H, 0., 360.*deg );
    G4LogicalVolume* logicPE =
	new G4LogicalVolume(solidPE,          //its solid
			    pe_mat,           //its material
			    "External_PE");            //its name
    G4ThreeVector posPE(EXTERNAL_PE_X-HALL_X,EXTERNAL_PE_Y-HALL_Y,EXTERNAL_PE_Z-HALL_Z);
    G4VPhysicalVolume* physPE =
	new G4PVPlacement(0,                     //no rotation
			  posPE,
			  logicPE,            //its logical volume
			  "External_PE",               //its name
			  logicHall,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking


    // Inner Xe containter
    G4Material* xe_mat = nist->FindOrBuildMaterial(material_map.at(EXTERNAL_XE_MATERIAL));
    G4Tubs* solidXe =
	new G4Tubs("Xe",
		   0., 0.5*EXTERNAL_XE_D, 0.5*EXTERNAL_XE_H, 0., 360.*deg );
    G4LogicalVolume* logicXe =
	new G4LogicalVolume(solidXe,          //its solid
			    xe_mat,           //its material
			    "External_Xe");            //its name
    G4ThreeVector posXe(EXTERNAL_XE_X-EXTERNAL_PE_X,EXTERNAL_XE_Y-EXTERNAL_PE_Y,EXTERNAL_XE_Z-EXTERNAL_PE_Z);
    G4VPhysicalVolume* physXe =
	new G4PVPlacement(0,                     //no rotation
			  posXe,
			  logicXe,            //its logical volume
			  "External_Xe",               //its name
			  logicPE,                     //its mother  volume
			  false,                 //no boolean operation
			  0,                     //copy number
			  checkOverlaps);        //overlaps checking

    std::cout<<"EXTERNAL_XE_X : "<<EXTERNAL_XE_X
	     <<", EXTERNAL_XE_Y : "<<EXTERNAL_XE_Y
	     <<", EXTERNAL_XE_Z : "<<EXTERNAL_XE_Z
	     <<std::endl;
    std::cout<<"EXTERNAL_XE_H : "<<EXTERNAL_XE_H
	     <<", EXTERNAL_XE_D : "<<EXTERNAL_XE_D<<std::endl;



    return physPE;

}



void DetectorConstruction::PrepareMaterials(G4NistManager* nist)
{
    // create PTFE
    const std::vector<G4String> elements = {"F", "C"};
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
    StdRock->AddElement(nist->FindOrBuildElement("O"),  52.*perCent);
    StdRock->AddElement(nist->FindOrBuildElement("Ca"), 27.*perCent);
    StdRock->AddElement(nist->FindOrBuildElement("C"),  12.*perCent);
    StdRock->AddElement(nist->FindOrBuildElement("Mg"),  9.*perCent);

    auto NaCl = new G4Material("NaCl",2.17*g/cm3, 2, kStateSolid );
    NaCl->AddElement(nist->FindOrBuildElement("Na"), 1);
    NaCl->AddElement(nist->FindOrBuildElement("Cl"), 1);


    // define GdLS
    // FIXME: inspired by Daya Bay, the fractions may not be correct, need to find a proper definition
    auto GdLS = new G4Material("GdLS",0.8602*g/cm3,4, kStateLiquid );
    GdLS->AddElement(nist->FindOrBuildElement("C"),  87.736*perCent);
    GdLS->AddElement(nist->FindOrBuildElement("H"), 12.02*perCent);
    GdLS->AddElement(nist->FindOrBuildElement("O"),  .109*perCent);
    GdLS->AddElement(nist->FindOrBuildElement("Gd"),  .103*perCent);

    //G4Material* gxe = nist->FindOrBuildMaterial("G4_Xe");
    nist->BuildMaterialWithNewDensity("Xe_External", "G4_Xe", 11.8*0.001*g/cm3);

    // define polyhalite
    auto polyhalite = new G4Material("polyhalite",2.78*g/cm3, 6, kStateSolid );
    // K2Ca2Mg(SO4)4·2H2O
    // Counts of element atoms:
    //   - K   2x
    //   - Ca  2x
    //   - Mg  1x
    //   - S   4x
    //   - O  18x
    //   - H   4x
    polyhalite->AddElement(nist->FindOrBuildElement("K"),  2);
    polyhalite->AddElement(nist->FindOrBuildElement("Ca"), 2);
    polyhalite->AddElement(nist->FindOrBuildElement("Mg"), 1);
    polyhalite->AddElement(nist->FindOrBuildElement("S"),  4);
    polyhalite->AddElement(nist->FindOrBuildElement("O"), 18);
    polyhalite->AddElement(nist->FindOrBuildElement("H"),  4);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
