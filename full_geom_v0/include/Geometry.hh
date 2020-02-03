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
//
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
    const double DOME_H = 50*cm;

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


    // Hall
    const G4String  HALL_MATERIAL = "Air";
    const double HALL_SPACE_TOP = 2*m;
    const double HALL_SPACE_BOTTOM = 1*m;
    const double HALL_SPACE_SIDE = 2*m;


    // World - Rock
    const G4String  WORLD_MATERIAL = "Rock";
    const double WORLD_SPACE = 3*m;


    // helper variable
    // envelope spacing
    const double ENV_SPACING = 1*mm;


    /// ========== Derived properties ==========
    /// - necessary to build the whole hierarchy of G4 volumes

    // bottom PTFE plate
    const double PTFE_B_H = PTFE_THICKNESS;
    const double PTFE_B_D = TPC_D + 2*PTFE_THICKNESS;

    // top PTFE plate
    const double PTFE_T_H = PTFE_THICKNESS;
    const double PTFE_T_D = TPC_D + 2*PTFE_THICKNESS;

    // side PTFE tube
    const double PTFE_S_D_INNER = TPC_D; // Do this during the
					 // construction instead -> +
					 // 0.5*mm; // keep space
					 // between PTFE and active
					 // volume
    const double PTFE_S_D_OUTER = TPC_D + 2*PTFE_THICKNESS;
    const double PTFE_H = TPC_H;

    // RFR cylinder
    const double RFR_D = PTFE_S_D_OUTER;

    // PMT array cylinder
    const double PMT_ARR_D = PTFE_S_D_OUTER;

    // TPC envelope size
    const double TPC_ENV_H = TPC_H + PTFE_T_H + PTFE_B_H + RFR_H + PMT_ARR_H;
    const double TPC_ENV_D = PTFE_S_D_OUTER;


    // skin layer (cylinder)
    const double SKIN_H = TPC_ENV_H + SKIN_THICK_BOTTOM;
    const double SKIN_D = TPC_ENV_D + SKIN_THICK_SIDE;

    // top dome (cylinder)
    const double DOME_D = SKIN_D;

    // Inner detector
    const double ID_ENV_D = SKIN_D;
    const double ID_ENV_H = SKIN_H + DOME_H;

    // ICV
    const double ICV_D = ID_ENV_D + ICV_THICKNESS;
    const double ICV_H = ID_ENV_H + ICV_THICKNESS;

    // Cryo vacuum
    const double VAC_D = ICV_D + VAC_THICKNESS;
    const double VAC_H = ICV_H + VAC_THICKNESS;

    /// OCV
    const double OCV_D = VAC_D + OCV_THICKNESS;
    const double OCV_H = VAC_H + OCV_THICKNESS;

    /// Pit



    /// Hall
    const double HALL_W = OCV_D + 2*HALL_SPACE_SIDE;
    const double HALL_H = OCV_H + HALL_SPACE_TOP + HALL_SPACE_BOTTOM;

    // World
    const double WORLD_W = HALL_W + 2*WORLD_SPACE;
    const double WORLD_H = HALL_H + 2*WORLD_SPACE;


    // Definition of materials

    const std::map<const G4String,const G4String> material_map = {
	{"LXe", "G4_lXe"},
	{"PTFE", "PTFE"},
	{"Steel5", "Steel5"},
	{"Vacuum", "G4_Galactic"},
	{"GXe", "G4_Xe"},
	{"Titanium", "G4_Ti"},
	{"Air", "G4_AIR"},
	{"Rock", "StdRock"},
    };


} // end of namespace Geometry
#endif

//
// Geometry.hh ends here
