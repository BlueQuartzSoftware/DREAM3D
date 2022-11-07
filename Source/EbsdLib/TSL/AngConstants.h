/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <QtCore/QString>

/**
 * @file AngConstants.h
 * @brief This file contains both c style macro definitions and C++ style const
 * declarations of TSL OIM variables and names. If you are dealing with a .ang
 * file from TSL instrumentation then you MUST use the defines from this file.
 */

// -----------------------------------------------------------------------------
//  These defines are needed for the pure "C" parser
// -----------------------------------------------------------------------------
#define ANG_TEM_PIXPERUM "TEM_PIXperUM"
#define ANG_X_STAR "x-star"
#define ANG_Y_STAR "y-star"
#define ANG_Z_STAR "z-star"
#define ANG_PATTERN_CENTER_CALIBRATION "Pattern Center Calibration"
#define ANG_WORKINGDISTANCE "WorkingDistance"
#define ANG_WORKING_DISTANCe "Working Distance"
#define ANG_PHASE "Phase"
#define ANG_PHASE_NAME "PhaseName"
#define ANG_MATERIAL_NAME "MaterialName"
#define ANG_FORMULA "Formula"
#define ANG_INFO "Info"
#define ANG_SYMMETRY "Symmetry"
#define ANG_LATTICE_CONSTANTS "LatticeConstants"
#define ANG_NUMBER_FAMILIES "NumberFamilies"
#define ANG_HKLFAMILIES "hklFamilies"
#define ANG_HKL_FAMILIES "hkl Families"
#define ANG_CATEGORIES "Categories"
#define ANG_GRID "GRID"
#define ANG_GRID_TYPE "Grid Type"
#define ANG_X_STEP "XSTEP"
#define ANG_Y_STEP "YSTEP"
#define ANG_STEP_X "Step X"
#define ANG_STEP_Y "Step Y"
#define ANG_NCOLS_ODD "NCOLS_ODD"
#define ANG_NCOLS_EVEN "NCOLS_EVEN"
#define ANG_NROWS "NROWS"
#define ANG_nColumns "nColumns"
#define ANG_nROWS "nRows"
#define ANG_OPERATOR "OPERATOR"
#define ANG_SAMPLE_ID "SAMPLEID"
#define ANG_SCAN_ID "SCANID"
#define ANG_Scan_ID "Scan ID"

#define ANG_Operator "Operator"
#define ANG_Sample_ID "Sample ID"

#define ANG_SQUARE_GRID "SqrGrid"
#define ANG_HEX_GRID "HexGrid"

#define ANG_PHI1 "Phi1"
#define ANG_PHI "Phi"
#define ANG_PHI2 "Phi2"
#define ANG_IMAGE_QUALITY "Image Quality"
#define ANG_CONFIDENCE_INDEX "Confidence Index"
#define ANG_PHASE_DATA "PhaseData"
#define ANG_PHASES "Phases"
#define ANG_X_POSITION "X Position"
#define ANG_Y_POSITION "Y Position"
#define ANG_SEM_SIGNAL "SEM Signal"
#define ANG_FIT "Fit"
#define ANG_Z_STEP "ZStep"
#define ANG_Z_POS "ZPos"
#define ANG_Z_MAX "ZMax"
#define ANG_CI "CI"
#define ANG_IQ "IQ"
#define ANG_PATTERN_DATA "Pattern"
#define ANG_PATTERN_WIDTH "Pattern Width"
#define ANG_PATTERN_HEIGHT "Pattern Height"

enum ANG_READ_FLAG
{
  ANG_FULL_FILE,
  ANG_HEADER_ONLY
};

namespace Ebsd
{

namespace H5OIM
{
const QString EDAX("EDAX");
const QString Manufacturer(" Manufacturer");
const std::string Version7(" Version");
const std::string Version8("Version");

const QString H5FileExt("h5");

const QString OriginalHeader("OriginalHeader");
const QString OriginalFile("OriginalFile");
const QString Index("Index");
const QString Header("Header");
const QString Phases("Phases");
const QString Phase("Phase");
const QString Data("Data");
const QString EBSD("EBSD");
const QString SEM_PRIAS_Images("SEM-PRIAS Images");
const QString PatternCenterCalibration("Pattern Center Calibration");
const QString SEM("SEM");

const QString FileVersionStr("FileVersion");
const unsigned int FileVersion = 5;
const QString EbsdLibVersionStr("EbsdLibVersion");

const std::string OIMAnalysisVersion("OIM Analysis");
const std::string OIMAnalysisVersion7("OIM Analysis 7");
const std::string OIMAnalysisVersion8("OIM Analysis 8");

} // namespace H5OIM

namespace Ang
{
const QString Manufacturer("TSL");
const QString EDAX("EDAX");

/* These are courtesy of TSL */
// LAUE Symmetry Identifiers

// Old Style LAUE Symmetry Identifiers
#define CIs 1    // triclinic      (S2, Ci) [-1]                 a!=b!=c  a!=b!=g!=90
#define D2H 22   // orthrohombic   (D2h) [mmm]                   a!=b!=c  a=b=g=90
#define C4H 4    // tetragonal     (C4h) [4/m]                   a=b!=c   a=b=g=90
#define D4H 42   // ditetragonal   (D4h) [4/mmm]                 a=b!=c   a=b=g=90
#define C3I 3    // trigonal       (S6, C3i) [-3]                a=b=c    a=b=g!=90
#define D3D 32   // ditrigonal     (D3d) [-3m]                   a=b=c    a=b=g!=90
#define C6H 6    // hexagonal      (C6h) [6/m]                   a=b!=c   a=b=90 g=120
#define D6H 62   // dihexagonal    (D6h) [6/mmm]                 a=b!=c   a=b=90 g=120
#define TH 23    // tetrahedral    (Th) [m3]                     a=b=c    a=b=g=90
#define OH 43    // cubic          (Oh) [m3m]                    a=b=c    a=b=g=90
#define C2H_c 2  // monoclinic c   (C2h) [2/m]                   a!=b!=c  a=b=90!=g
#define C2H_b 20 // monoclinic b   (C2h) [2/m]                   a!=b!=c  a=g=90!=b
#define C2H_a 21 // monoclinic a   (C2h) [2/m]                   a!=b!=c  b=g=90!=a

// New Style Full Point Group Symmetry Identifiers
#define Sym_C1 100 // Triclinic (C1) [1]
#define Sym_S2 101 // Triclinic (S2, Ci) [-1]

#define Sym_D2 105  // Orthorhombic (D2) [222]
#define Sym_C2v 106 // Orthorhombic (C2v) [mm2]
#define Sym_D2h 107 // Orthorhombic (D2h) [mmm]

#define Sym_C4 108  // Tetragonal (C4) [4]
#define Sym_S4 109  // Tetragonal (S4) [-4]
#define Sym_C4h 110 // Tetragonal (C4h) [4/m]

#define Sym_D4 111  // Tetragonal (D4) [422]
#define Sym_C4v 112 // Tetragonal (C4v) [4mm]
#define Sym_D2d 113 // Tetragonal (D2d) [-42m]
#define Sym_D4h 114 // Tetragonal (D4h) [4/mmm]

#define Sym_C3 115 // Trigonal (C3) [3]
#define Sym_S6 116 // Trigonal (S6, C3i) [-3]

#define Sym_D3 117  // Trigonal (D3) [32]
#define Sym_C3v 118 // Trigonal (C3v) [3m]
#define Sym_D3d 119 // Trigonal (D3d) [-3m]

#define Sym_C6 120  // Hexagonal (C6) [6]
#define Sym_C3h 121 // Hexagonal (C3h) [-6]
#define Sym_C6h 122 // Hexagonal (C6h) [6/m]

#define Sym_D6 123  // Hexagonal (D6) [622]
#define Sym_C6v 124 // Hexagonal (C6v) [6mm]
#define Sym_D3h 125 // Hexagonal (D3h) [-6m2]
#define Sym_D6h 126 // Hexagonal (D6h) [6/mmm]

#define Sym_T 127  // Cubic (T) [23]
#define Sym_Th 128 // Cubic (Th) [m-3]

#define Sym_O 129  // Cubic (O) [432]
#define Sym_Td 130 // Cubic (Td) [-43m]
#define Sym_Oh 131 // Cubic (Oh) [m3m]

#define Sym_C2_c 102  // Monoclinic c (C2)[2]
#define Sym_C1h_c 103 // Monoclinic c (C1h, Cs) [m]
#define Sym_C2h_c 104 // Monoclinic c (C2h) [2/m]
#define Sym_C2_b 132  // Monoclinic b (C2)[2]
#define Sym_C1h_b 133 // Monoclinic b (C1h, Cs) [m]
#define Sym_C2h_b 134 // Monoclinic b (C2h) [2/m]
#define Sym_C2_a 135  // Monoclinic a (C2)[2]
#define Sym_C1h_a 136 // Monoclinic a (C1h, Cs) [m]
#define Sym_C2h_a 137 // Monoclinic a (C2h) [2/m]

namespace PhaseSymmetry
{
const unsigned int Cubic = OH;
const unsigned int Tetrahedral = TH;
const unsigned int DiTetragonal = D4H;
const unsigned int Tetragonal = C4H;
const unsigned int Orthorhombic = D2H;
const unsigned int Monoclinic_c = C2H_c;
const unsigned int Monoclinic_b = C2H_b;
const unsigned int Monoclinic_a = C2H_a;
const unsigned int DiHexagonal = D6H;
const unsigned int Hexagonal = C6H;
const unsigned int DiTrigonal = D3D;
const unsigned int Trigonal = C3I;
const unsigned int Triclinic = CIs;

const unsigned int k_Sym_C1 = Sym_C1; // Triclinic (C1) [1]
const unsigned int k_Sym_S2 = Sym_S2; // Triclinic (S2, Ci) [-1]

const unsigned int k_Sym_D2 = Sym_D2;   // Orthorhombic (D2) [222]
const unsigned int k_Sym_C2v = Sym_C2v; // Orthorhombic (C2v) [mm2]
const unsigned int k_Sym_D2h = Sym_D2h; // Orthorhombic (D2h) [mmm]

const unsigned int k_Sym_C4 = Sym_C4;   // Tetragonal (C4) [4]
const unsigned int k_Sym_S4 = Sym_S4;   // Tetragonal (S4) [-4]
const unsigned int k_Sym_C4h = Sym_C4h; // Tetragonal (C4h) [4/m]

const unsigned int k_Sym_D4 = Sym_D4;   // Tetragonal (D4) [422]
const unsigned int k_Sym_C4v = Sym_C4v; // Tetragonal (C4v) [4mm]
const unsigned int k_Sym_D2d = Sym_D2d; // Tetragonal (D2d) [-42m]
const unsigned int k_Sym_D4h = Sym_D4h; // Tetragonal (D4h) [4/mmm]

const unsigned int k_Sym_C3 = Sym_C3; // Trigonal (C3) [3]
const unsigned int k_Sym_S6 = Sym_S6; // Trigonal (S6, C3i) [-3]

const unsigned int k_Sym_D3 = Sym_D3;   // Trigonal (D3) [32]
const unsigned int k_Sym_C3v = Sym_C3v; // Trigonal (C3v) [3m]
const unsigned int k_Sym_D3d = Sym_D3d; // Trigonal (D3d) [-3m]

const unsigned int k_Sym_C6 = Sym_C6;   // Hexagonal (C6) [6]
const unsigned int k_Sym_C3h = Sym_C3h; // Hexagonal (C3h) [-6]
const unsigned int k_Sym_C6h = Sym_C6h; // Hexagonal (C6h) [6/m]

const unsigned int k_Sym_D6 = Sym_D6;   // Hexagonal (D6) [622]
const unsigned int k_Sym_C6v = Sym_C6v; // Hexagonal (C6v) [6mm]
const unsigned int k_Sym_D3h = Sym_D3h; // Hexagonal (D3h) [-6m2]
const unsigned int k_Sym_D6h = Sym_D6h; // Hexagonal (D6h) [6/mmm]

const unsigned int k_Sym_T = Sym_T;   // Cubic (T) [23]
const unsigned int k_Sym_Th = Sym_Th; // Cubic (Th) [m-3]  HIGH

const unsigned int k_Sym_O = Sym_O;   // Cubic (O) [432]
const unsigned int k_Sym_Td = Sym_Td; // Cubic (Td) [-43m]
const unsigned int k_Sym_Oh = Sym_Oh; // Cubic (Oh) [m3m]

const unsigned int k_Sym_C2_c = Sym_C2_c;   // Monoclinic c (C2)[2]
const unsigned int k_Sym_C1h_c = Sym_C1h_c; // Monoclinic c (C1h, Cs) [m]
const unsigned int k_Sym_C2h_c = Sym_C2h_c; // Monoclinic c (C2h) [2/m]
const unsigned int k_Sym_C2_b = Sym_C2_b;   // Monoclinic b (C2)[2]
const unsigned int k_Sym_C1h_b = Sym_C1h_b; // Monoclinic b (C1h, Cs) [m]
const unsigned int k_Sym_C2h_b = Sym_C2h_b; // Monoclinic b (C2h) [2/m]
const unsigned int k_Sym_C2_a = Sym_C2_a;   // Monoclinic a (C2)[2]
const unsigned int k_Sym_C1h_a = Sym_C1h_a; // Monoclinic a (C1h, Cs) [m]
const unsigned int k_Sym_C2h_a = Sym_C2h_a; // Monoclinic a (C2h) [2/m]

const unsigned int UnknownSymmetry = 999;
} // namespace PhaseSymmetry

const QString TEMPIXPerUM(ANG_TEM_PIXPERUM);
const QString XStar(ANG_X_STAR);
const QString YStar(ANG_Y_STAR);
const QString ZStar(ANG_Z_STAR);
const QString WorkingDistance(ANG_WORKINGDISTANCE);
const QString Working_Distance(ANG_WORKING_DISTANCe);
const QString Phase(ANG_PHASE);
const QString PhaseName(ANG_PHASE_NAME);
const QString MaterialName(ANG_MATERIAL_NAME);
const QString Formula(ANG_FORMULA);
// const QString Info(ANG_INFO);
const QString Symmetry(ANG_SYMMETRY);
const QString LatticeConstants(ANG_LATTICE_CONSTANTS);
const QString NumberFamilies(ANG_NUMBER_FAMILIES);
const QString HKLFamilies(ANG_HKLFAMILIES);
const QString HKL_Families(ANG_HKL_FAMILIES);
const QString Categories(ANG_CATEGORIES);
const QString Grid(ANG_GRID);
const QString GridType(ANG_GRID_TYPE);
const QString XStep(ANG_X_STEP);
const QString YStep(ANG_Y_STEP);
const QString StepX(ANG_STEP_X);
const QString StepY(ANG_STEP_Y);
const QString NColsOdd(ANG_NCOLS_ODD);
const QString NColsEven(ANG_NCOLS_EVEN);
const QString nColumns(ANG_nColumns);
const QString NRows(ANG_NROWS);
const QString nRows(ANG_nROWS);
const QString Operator(ANG_Operator);
const QString SampleID(ANG_Sample_ID);
const QString OPERATOR(ANG_OPERATOR);
const QString SAMPLEID(ANG_SAMPLE_ID);
const QString SCANID(ANG_SCAN_ID);
const QString ScanID(ANG_Scan_ID);
const QString Phi1(ANG_PHI1);
const QString Phi(ANG_PHI);
const QString Phi2(ANG_PHI2);
const QString ImageQuality(ANG_IMAGE_QUALITY);
const QString ConfidenceIndex(ANG_CONFIDENCE_INDEX);
const QString CI(ANG_CI);
const QString IQ(ANG_IQ);
const QString PhaseData(ANG_PHASE_DATA);
const QString XPosition(ANG_X_POSITION);
const QString YPosition(ANG_Y_POSITION);
const QString SEMSignal(ANG_SEM_SIGNAL);
const QString Fit(ANG_FIT);
const QString PatternData(ANG_PATTERN_DATA);
const QString PatternWidth(ANG_PATTERN_WIDTH);
const QString PatternHeight(ANG_PATTERN_HEIGHT);

const QString SquareGrid(ANG_SQUARE_GRID);
const QString HexGrid(ANG_HEX_GRID);

const QString ZStep(ANG_Z_STEP);
const QString ZPos(ANG_Z_POS);
const QString ZMax(ANG_Z_MAX);

} // namespace Ang
} // namespace Ebsd

#if 1
// -----------------------------------------------------------------------------
//  These are the lower case versions of the constants for the ANG file
// -----------------------------------------------------------------------------
#define ANG_TEM_PIXPERUM_LOWER "teSIMPLib::Constants::k_Pixperum"
#define ANG_X_STAR_LOWER "x-star"
#define ANG_Y_STAR_LOWER "y-star"
#define ANG_Z_STAR_LOWER "z-star"
#define ANG_WORKINGDISTANCE_LOWER "workingdistance"

#define ANG_PHASE_LOWER "phase"
#define ANG_MATERIAL_NAME_LOWER "materialname"
#define ANG_FORMULA_LOWER "formula"
#define ANG_INFO_LOWER "info"
#define ANG_SYMMETRY_LOWER "symmetry"
#define ANG_LATTICE_CONSTANTS_LOWER "latticeconstants"
#define ANG_NUMBER_FAMILIES_LOWER "numberfamilies"
#define ANG_HKL_FAMILIES_LOWER "hklfamilies"
#define ANG_CATEGORIES_LOWER "categories"

#define ANG_GRID_LOWER "grid"
#define ANG_X_STEP_LOWER "xstep"
#define ANG_Y_STEP_LOWER "ystep"
#define ANG_NCOLS_ODD_LOWER "ncols_odd"
#define ANG_NCOLS_EVEN_LOWER "ncols_even"
//#define ANG_NCOLS_LOWER "ncols"
#define ANG_NROWS_LOWER "nrows"

#define ANG_OPERATOR_LOWER "operator"
#define ANG_SAMPLE_ID_LOWER "sampleid"
#define ANG_SCAN_ID_LOWER "scanid"
#define ANG_SQUARE_GRID_LOWER "sqrgrid"
#define ANG_HEX_GRID_LOWER "hexgrid"

// Constants  to be used by all classes
#define ANG_PHI1_LOWER "phi1"
#define ANG_PHI_LOWER "phi"
#define ANG_PHI2_LOWER "phi2"
#define ANG_IMAGE_QUALITY_LOWER "image quality"
#define ANG_CONFIDENCE_INDEX_LOWER "confidence index"
#define ANG_PHASE_DATA_LOWER "phase"
#define ANG_X_POSITION_LOWER "x position"
#define ANG_Y_POSITION_LOWER "y position"
#define ANG_SEM_SIGNAL_LOWER "sem signal"
#define ANG_FIT_LOWER "fit"

namespace Ebsd
{

namespace Ang
{
// These are the Lower Case versions of the constants
const QString FileExtLower("ang");
const QString FileExt("ang");
const QString TEMPIXPerUMLower(ANG_TEM_PIXPERUM_LOWER);
const QString XStarLower(ANG_X_STAR_LOWER);
const QString YStarLower(ANG_Y_STAR_LOWER);
const QString ZStarLower(ANG_Z_STAR_LOWER);
const QString WorkingDistanceLower(ANG_WORKINGDISTANCE_LOWER);
const QString PhaseLower(ANG_PHASE_LOWER);
const QString MaterialNameLower(ANG_MATERIAL_NAME_LOWER);
const QString FormulaLower(ANG_FORMULA_LOWER);
const QString InfoLower(ANG_INFO_LOWER);
const QString SymmetryLower(ANG_SYMMETRY_LOWER);
const QString LatticeConstantsLower(ANG_LATTICE_CONSTANTS_LOWER);
const QString NumberFamiliesLower(ANG_NUMBER_FAMILIES_LOWER);
const QString HKLFamiliesLower(ANG_HKL_FAMILIES_LOWER);
const QString CategoriesLower(ANG_CATEGORIES_LOWER);
const QString GridLower(ANG_GRID_LOWER);
const QString XStepLower(ANG_X_STEP_LOWER);
const QString YStepLower(ANG_Y_STEP_LOWER);
const QString NColsOddLower(ANG_NCOLS_ODD_LOWER);
const QString NColsEvenLower(ANG_NCOLS_EVEN_LOWER);
//   const QString NColsLower(ANG_NCOLS_LOWER);
const QString NRowsLower(ANG_NROWS_LOWER);
const QString OperatorLower(ANG_OPERATOR_LOWER);
const QString SampleIdLower(ANG_SAMPLE_ID_LOWER);
const QString ScanIdLower(ANG_SCAN_ID_LOWER);

const QString Phi1Lower(ANG_PHI1_LOWER);
const QString PhiLower(ANG_PHI_LOWER);
const QString Phi2Lower(ANG_PHI2_LOWER);
const QString ImageQualityLower(ANG_IMAGE_QUALITY_LOWER);
const QString ConfidenceIndexLower(ANG_CONFIDENCE_INDEX_LOWER);
const QString PhaseDataLower(ANG_PHASE_DATA_LOWER);
const QString XPositionLower(ANG_X_POSITION_LOWER);
const QString YPositionLower(ANG_Y_POSITION_LOWER);
const QString SEMSignalLower(ANG_SEM_SIGNAL_LOWER);
const QString FitLower(ANG_FIT_LOWER);

const QString LatticeConstantA("Lattice Constant a");
const QString LatticeConstantB("Lattice Constant b");
const QString LatticeConstantC("Lattice Constant c");
const QString LatticeConstantAlpha("Lattice Constant alpha");
const QString LatticeConstantBeta("Lattice Constant beta");
const QString LatticeConstantGamma("Lattice Constant gamma");
} // namespace Ang

namespace AngFile
{
const QString Phases("Phases");
const QString EulerAngles("EulerAngles");
const QString CrystalStructures("CrystalStructures");
const QString MaterialName("MaterialName");
const QString LatticeConstants("LatticeConstants");
const QString BravaisLattice("BravaisLattice");
} // namespace AngFile

} // namespace Ebsd
#endif
