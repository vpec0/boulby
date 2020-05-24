// Geometry.hh ---
//
// Filename: Geometry.hh
// Description:
// Author: Viktor Pec
// Maintainer:
// Created: Thu Jan  9 13:09:02 2020 (+0000)
// Version:
// Package-Requires: ()
// Last-Updated:
//           By:
//     Update #: 0
// URL:
// Doc URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
// All geometry specific variables needed for detector construction.
// Materials are defined in a separate header file.
//

// Change Log:
//
// 24/05/2020 - start changing to cylindrical geometry
//
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Emacs.  If not, see <https://www.gnu.org/licenses/>.
//
//

// Code:
#ifndef GEOMETRY_HH
#define GEOMETRY_HH

#include "G4SystemOfUnits.hh"

namespace Geometry {
    using namespace std;

    /// Cryostat
    /// Defined properties

    // Active region
    const G4String  TPC_MATERIAL = "LXe";
    const double TPC_H = 2.5 * m; // height
    const double TPC_D = 3.5 * m; // diameter
    // position - active volume centered
    const double TPC_X = 0.;
    const double TPC_Y = 0.;
    const double TPC_Z = 0.;


    // reflective cover
    const G4String  PTFE_MATERIAL  = "PTFE";
    const double PTFE_THICKNESS = 3*cm;

    // reverse field region
    const G4String RFR_MATERIAL = "LXe";
    const double RFR_H       = 20*cm;

    // approximation of PMT array
    const G4String PMT_ARR_MATERIAL = "Steel5";
    const double PMT_ARR_H = 40*cm;

    // TPC envelope volume to hold volumes described above; should
    // fill ~0.5 mm space between volumes so that they do not touch
    const G4String TPC_ENV_MATERIAL = "Vacuum";

    // outer Xe skin
    const G4String SKIN_MATERIAL = "LXe";
    const double SKIN_THICK_SIDE = 8*cm;
    const double SKIN_THICK_BOTTOM = 70*cm;

    // top dome
    const G4String DOME_MATERIAL = "GXe";
    const double DOME_HH = 50*cm; // height above top PMT array

    // Inner detector envelope - to hold skin and dome volumes; should
    // fill ~0.5 mm space between volumes so that they do not touch
    const G4String ID_ENV_MATERIAL = "Vacuum";

    // inner cryostat vessel
    const G4String ICV_MATERIAL = "Titanium";
    const double ICV_THICKNESS = 2*cm;

    // cryo vacuum
    const G4String VAC_MATERIAL = "Vacuum";
    const double VAC_THICKNESS = 5*cm;

    // outer cryostat vessel
    const G4String  OCV_MATERIAL = "Titanium";
    const double OCV_THICKNESS = 2*cm;


    // Scintillator - LS, LAB, Gd
    const G4String  GDLS_MATERIAL = "GdLS";
    const double GDLS_THICKNESS = 50*cm;

    // Water tank
    const G4String  WT_MATERIAL = "Water";
    const double WT_THICKNESS_TOP = 3*m;
    const double WT_THICKNESS_BOTTOM = 1.5*m;
    const double WT_THICKNESS_SIDE = 4*m;

    // Bottom steel shielding
    const G4String  STEEL_MATERIAL = "Steel";
    const double STEEL_H = 30*cm;
    const double STEEL_OVERLAP = 1*m;

    // Hall
    const G4String  HALL_MATERIAL = "Air";
    const double HALL_D = 30*m;
    const double HALL_H = 30*m;
    //const double HALL_SPACE_TOP = 8*m;
    //const double HALL_SPACE_BOTTOM = 1*m;
    //const double HALL_SPACE_SIDE = 3*m;

    // Rock
    const G4String  ROCK_MATERIAL = "Rock";
    const double ROCK_SPACE_TOP = 7*m;
    const double ROCK_SPACE_SIDE = 5*m;
    const double ROCK_SPACE_BOTTOM = 3*m;


    // World
    const G4String  WORLD_MATERIAL = "Vacuum";

    // helper variable
    // envelope spacing
    const double ENV_SPACING = 0.5*mm;


    /// ========== Derived properties ==========
    /// - necessary to build the whole hierarchy of G4 volumes

    // bottom PTFE plate
    const double PTFE_B_H = PTFE_THICKNESS;
    const double PTFE_B_D = TPC_D + 2*PTFE_THICKNESS;
    // positioning in absolute coordinates (TPC placed at 0.)
    const double PTFE_B_X = TPC_X;
    const double PTFE_B_Y = TPC_Y;
    const double PTFE_B_Z = TPC_Z - 0.5*TPC_H - RFR_H - 0.5*PTFE_THICKNESS;


    // top PTFE plate
    const double PTFE_T_H = PTFE_THICKNESS;
    const double PTFE_T_D = TPC_D + 2*PTFE_THICKNESS;
    const double PTFE_T_X = TPC_X;
    const double PTFE_T_Y = TPC_Y;
    const double PTFE_T_Z = TPC_Z + 0.5*TPC_H + 0.5*PTFE_THICKNESS;

    // side PTFE tube
    const double PTFE_S_D_INNER = TPC_D;
    const double PTFE_S_D_OUTER = TPC_D + 2*PTFE_THICKNESS;
    const double PTFE_S_H = TPC_H + RFR_H;
    const double PTFE_S_X = TPC_X;
    const double PTFE_S_Y = TPC_Y;
    const double PTFE_S_Z = TPC_Z - 0.5*RFR_H; // off-set by RFR


    // RFR cylinder
    const double RFR_D = TPC_D;
    const double RFR_X = TPC_X;
    const double RFR_Y = TPC_Y;
    const double RFR_Z = TPC_Z - 0.5*TPC_H - 0.5*RFR_H;

    // PMT array cylinder
    const double PMT_ARR_D = PTFE_S_D_OUTER;
    // top PMT array
    const double PMT_T_X = TPC_X;
    const double PMT_T_Y = TPC_Y;
    const double PMT_T_Z = TPC_Z + 0.5*TPC_H + PTFE_T_H + 0.5*PMT_ARR_H;
    // bottom PMT array
    const double PMT_B_X = TPC_X;
    const double PMT_B_Y = TPC_Y;
    const double PMT_B_Z = PTFE_B_Z - 0.5*PTFE_B_H - 0.5*PMT_ARR_H;

    // Tpc envelope size
    const double TPC_ENV_H = TPC_H + PTFE_B_H + RFR_H + PMT_ARR_H; // TPC_ENV is in LXe, LXe level starts below top PTFE
    const double TPC_ENV_D = PTFE_S_D_OUTER;
    const double TPC_ENV_X = TPC_X;
    const double TPC_ENV_Y = TPC_Y;
    const double TPC_ENV_Z = PTFE_S_Z - 0.5*PMT_ARR_H - 0.5*PTFE_B_H;


    // skin layer (cylinder)
    const double SKIN_H = TPC_ENV_H + SKIN_THICK_BOTTOM;
    const double SKIN_D = TPC_ENV_D + 2*SKIN_THICK_SIDE;
    const double SKIN_X = TPC_X;
    const double SKIN_Y = TPC_Y;
    const double SKIN_Z = TPC_ENV_Z - 0.5*SKIN_THICK_BOTTOM;

    // top dome (cylinder)
    //
    // stretches from top level of LXe, contains
    // top PTFE layer, top PMT array, and to top dome spanning over
    // the detector
    const double DOME_H = DOME_HH + PMT_ARR_H + PTFE_THICKNESS;
    const double DOME_D = SKIN_D;
    const double DOME_X = TPC_X;
    const double DOME_Y = TPC_Y;
    const double DOME_Z = TPC_ENV_Z + 0.5*TPC_ENV_H + 0.5*DOME_H;

    // Inner detector
    const double ID_ENV_D = SKIN_D;
    const double ID_ENV_H = SKIN_H + DOME_H;
    const double ID_ENV_X = TPC_X;
    const double ID_ENV_Y = TPC_Y;
    const double ID_ENV_Z = SKIN_Z + 0.5*DOME_H;


    // ICV
    const double ICV_D = ID_ENV_D + 2*ICV_THICKNESS;
    const double ICV_H = ID_ENV_H + 2*ICV_THICKNESS;
    const double ICV_X = TPC_X;
    const double ICV_Y = TPC_Y;
    const double ICV_Z = ID_ENV_Z;

    // Cryo vacuum
    const double VAC_D = ICV_D + 2*VAC_THICKNESS;
    const double VAC_H = ICV_H + 2*VAC_THICKNESS;
    const double VAC_X = TPC_X;
    const double VAC_Y = TPC_Y;
    const double VAC_Z = ICV_Z;

    /// OCV
    const double OCV_D = VAC_D + 2*OCV_THICKNESS;
    const double OCV_H = VAC_H + 2*OCV_THICKNESS;
    const double OCV_X = TPC_X;
    const double OCV_Y = TPC_Y;
    const double OCV_Z = VAC_Z;

    /// GdLS - scintillator around the cryostat
    const double GDLS_D = OCV_D + 2*GDLS_THICKNESS;
    const double GDLS_H = OCV_H + 2*GDLS_THICKNESS;
    const double GDLS_X = TPC_X;
    const double GDLS_Y = TPC_Y;
    const double GDLS_Z = OCV_Z;

    /// Water tank
    const double WT_D = GDLS_D + 2*WT_THICKNESS_SIDE;
    const double WT_H = GDLS_H + WT_THICKNESS_TOP + WT_THICKNESS_BOTTOM;
    const double WT_X = TPC_X;
    const double WT_Y = TPC_Y;
    const double WT_Z = GDLS_Z + 0.5*WT_THICKNESS_TOP - 0.5*WT_THICKNESS_BOTTOM;

    // Bottom steel shielding
    const double STEEL_D = GDLS_D + 2*STEEL_OVERLAP;
    const double STEEL_X = TPC_X;
    const double STEEL_Y = TPC_Y;
    const double STEEL_Z = WT_Z - 0.5*WT_H - 0.5*STEEL_H;


    /// Hall
    // const double HALL_D = WT_D + 2*HALL_SPACE_SIDE;
    // const double HALL_H = WT_H + HALL_SPACE_TOP + STEEL_H;
    const double HALL_X = TPC_X;
    const double HALL_Y = TPC_Y;
    const double HALL_Z = WT_Z + - 0.5*WT_H - 0.5*STEEL_H + 0.5*HALL_H;

    // Rock
    const double ROCK_W = HALL_D + 2*ROCK_SPACE_SIDE;
    const double ROCK_H = HALL_H + ROCK_SPACE_TOP + ROCK_SPACE_BOTTOM;
    const double ROCK_X = TPC_X;
    const double ROCK_Y = TPC_Y;
    const double ROCK_Z = HALL_Z + 0.5*ROCK_SPACE_TOP - 0.5*ROCK_SPACE_BOTTOM;

    // World
    const double WORLD_W = 2*ROCK_W;
    const double WORLD_H = 2*ROCK_H;


    // Definition of materials

    const std::map<const G4String,const G4String> material_map = {
	{"LXe", "G4_lXe"},
	{"PTFE", "PTFE"},
	{"Steel5", "Steel5"},
	{"Vacuum", "G4_Galactic"},
	{"GXe", "G4_Xe"},
	{"Titanium", "G4_Ti"},
	{"Air", "G4_AIR"},
	// {"Rock", "StdRock"},
	{"Rock", "NaCl"},
	{"Water", "G4_WATER"},
	{"GdLS", "GdLS"},
	{"Steel", "G4_STAINLESS-STEEL"}
    };


} // end of namespace Geometry
#endif

//
// Geometry.hh ends here
