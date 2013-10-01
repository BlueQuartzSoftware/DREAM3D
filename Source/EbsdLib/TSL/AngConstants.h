/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */




#ifndef ANGCONSTANTS_H_
#define ANGCONSTANTS_H_

/**
 * @file AngConstants.h
 * @brief This file contains both c style macro definitions and C++ style const
 * declarations of TSL OIM variables and names. If you are dealing with a .ang
 * file from TSL instrumentation then you MUST use the defines from this file.
 */

// -----------------------------------------------------------------------------
//  These defines are needed for the pure "C" parser
// -----------------------------------------------------------------------------
#define ANG_TEM_PIXPERUM     "TEM_PIXperUM"
#define ANG_X_STAR           "x-star"
#define ANG_Y_STAR           "y-star"
#define ANG_Z_STAR           "z-star"
#define ANG_WORKING_DISTANCE "WorkingDistance"
#define ANG_PHASE             "Phase"
#define ANG_MATERIAL_NAME     "MaterialName"
#define ANG_FORMULA           "Formula"
#define ANG_INFO              "Info"
#define ANG_SYMMETRY          "Symmetry"
#define ANG_LATTICE_CONSTANTS "LatticeConstants"
#define ANG_NUMBER_FAMILIES   "NumberFamilies"
#define ANG_HKL_FAMILIES      "hklFamilies"
#define ANG_CATEGORIES        "Categories"
#define ANG_GRID              "GRID"
#define ANG_X_STEP            "XSTEP"
#define ANG_Y_STEP            "YSTEP"
#define ANG_NCOLS_ODD         "NCOLS_ODD"
#define ANG_NCOLS_EVEN        "NCOLS_EVEN"
#define ANG_NROWS             "NROWS"
#define ANG_OPERATOR          "OPERATOR"
#define ANG_SAMPLE_ID         "SAMPLEID"
#define ANG_SCAN_ID           "SCANID"

#define ANG_SQUARE_GRID       "SqrGrid"
#define ANG_HEX_GRID          "HexGrid"


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



namespace Ebsd {

 namespace Ang
 {
   const std::string Manufacturer("TSL");

/* These are courtesy of TSL */
// LAUE Symmetry Identifiers

#define OH  43        // cubic            Oh         a=b=c     a=b=g=90

#define TH  23        // tetrahedral      Th         a=b=c     a=b=g=90
#define D4H 42        // ditetragonal     D4h        a=b!=c    a=b=g=90
#define C4H 4         // tetragonal       C4h        a=b!=c    a=b=g=90

#define D2H 22        // orthrohombic     D2h        a!=b!=c   a=b=g=90

#define C2H_c 2       // monoclinic       C2h        a!=b!=c   a=b=90!=g
#define C2H_b 20      // monoclinic       C2h        a!=b!=c   a=g=90!=b
#define C2H_a 21      // monoclinic       C2h        a!=b!=c   b=g=90!=a

#define D6H 62        // dihexagonal      D6h        a=b!=c    a=b=90 g=120
#define C6H 6         // hexagonal        C6h        a=b! =c   a=b=90 g=120

#define D3D 32        // ditrigonal       D3d        a=b=c     a=b=g!=90
#define C3I 3         // trigonal         C3i        a=b=c     a=b=g!=90

#define CIs 1         // triclinic        Ci         a!=b!=c  a!=b!=g!=90

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
     const unsigned int Triclinic =CIs;
     const unsigned int UnknownSymmetry = 999;
   }



   const std::string FileExt("ang");
   const std::string TEMPIXPerUM(ANG_TEM_PIXPERUM);
   const std::string XStar(ANG_X_STAR  );
   const std::string YStar(ANG_Y_STAR);
   const std::string ZStar(ANG_Z_STAR);
   const std::string WorkingDistance(ANG_WORKING_DISTANCE);
   const std::string Phase(ANG_PHASE);
   const std::string MaterialName(ANG_MATERIAL_NAME);
   const std::string Formula(ANG_FORMULA);
   const std::string Info(ANG_INFO);
   const std::string Symmetry(ANG_SYMMETRY);
   const std::string LatticeConstants(ANG_LATTICE_CONSTANTS);
   const std::string NumberFamilies(ANG_NUMBER_FAMILIES);
   const std::string HKLFamilies(ANG_HKL_FAMILIES);
   const std::string Categories(ANG_CATEGORIES);
   const std::string Grid(ANG_GRID);
   const std::string XStep(ANG_X_STEP);
   const std::string YStep(ANG_Y_STEP);
   const std::string NColsOdd(ANG_NCOLS_ODD);
   const std::string NColsEven(ANG_NCOLS_EVEN);
//   const std::string NCols(ANG_NCOLS);
   const std::string NRows(ANG_NROWS);
   const std::string Operator(ANG_OPERATOR);
   const std::string SampleId(ANG_SAMPLE_ID);
   const std::string ScanId(ANG_SCAN_ID);
   const std::string Phi1(ANG_PHI1);
   const std::string Phi(ANG_PHI);
   const std::string Phi2(ANG_PHI2);
   const std::string ImageQuality(ANG_IMAGE_QUALITY);
   const std::string ConfidenceIndex(ANG_CONFIDENCE_INDEX);
   const std::string PhaseData(ANG_PHASE_DATA);
   const std::string XPosition(ANG_X_POSITION);
   const std::string YPosition(ANG_Y_POSITION);
   const std::string SEMSignal(ANG_SEM_SIGNAL);
   const std::string Fit(ANG_FIT);

   const std::string SquareGrid(ANG_SQUARE_GRID);
   const std::string HexGrid(ANG_HEX_GRID);

   const std::string ZStep(ANG_Z_STEP);
   const std::string ZPos(ANG_Z_POS);
   const std::string ZMax(ANG_Z_MAX);

 }
}

#if 1
// -----------------------------------------------------------------------------
//  These are the lower case versions of the constants for the ANG file
// -----------------------------------------------------------------------------
#define ANG_TEM_PIXPERUM_LOWER "teDREAM3D::Constants::k_Pixperum"
#define ANG_X_STAR_LOWER "x-star"
#define ANG_Y_STAR_LOWER "y-star"
#define ANG_Z_STAR_LOWER "z-star"
#define ANG_WORKING_DISTANCE_LOWER "workingdistance"

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


namespace Ebsd {


namespace Ang
 {
// These are the Lower Case versions of the constants
   const std::string FileExtLower("ang");
   const std::string TEMPIXPerUMLower(ANG_TEM_PIXPERUM_LOWER);
   const std::string XStarLower(ANG_X_STAR_LOWER);
   const std::string YStarLower(ANG_Y_STAR_LOWER);
   const std::string ZStarLower(ANG_Z_STAR_LOWER);
   const std::string WorkingDistanceLower(ANG_WORKING_DISTANCE_LOWER);
   const std::string PhaseLower(ANG_PHASE_LOWER);
   const std::string MaterialNameLower(ANG_MATERIAL_NAME_LOWER);
   const std::string FormulaLower(ANG_FORMULA_LOWER);
   const std::string InfoLower(ANG_INFO_LOWER);
   const std::string SymmetryLower(ANG_SYMMETRY_LOWER);
   const std::string LatticeConstantsLower(ANG_LATTICE_CONSTANTS_LOWER);
   const std::string NumberFamiliesLower(ANG_NUMBER_FAMILIES_LOWER);
   const std::string HKLFamiliesLower(ANG_HKL_FAMILIES_LOWER);
   const std::string CategoriesLower(ANG_CATEGORIES_LOWER);
   const std::string GridLower(ANG_GRID_LOWER);
   const std::string XStepLower(ANG_X_STEP_LOWER);
   const std::string YStepLower(ANG_Y_STEP_LOWER);
   const std::string NColsOddLower(ANG_NCOLS_ODD_LOWER);
   const std::string NColsEvenLower(ANG_NCOLS_EVEN_LOWER);
//   const std::string NColsLower(ANG_NCOLS_LOWER);
   const std::string NRowsLower(ANG_NROWS_LOWER);
   const std::string OperatorLower(ANG_OPERATOR_LOWER);
   const std::string SampleIdLower(ANG_SAMPLE_ID_LOWER);
   const std::string ScanIdLower(ANG_SCAN_ID_LOWER);

   const std::string Phi1Lower(ANG_PHI1_LOWER);
   const std::string PhiLower(ANG_PHI_LOWER);
   const std::string Phi2Lower(ANG_PHI2_LOWER);
   const std::string ImageQualityLower(ANG_IMAGE_QUALITY_LOWER);
   const std::string ConfidenceIndexLower(ANG_CONFIDENCE_INDEX_LOWER);
   const std::string PhaseDataLower(ANG_PHASE_DATA_LOWER);
   const std::string XPositionLower(ANG_X_POSITION_LOWER);
   const std::string YPositionLower(ANG_Y_POSITION_LOWER);
   const std::string SEMSignalLower(ANG_SEM_SIGNAL_LOWER);
   const std::string FitLower(ANG_FIT_LOWER);
 }

}
#endif

#endif /* ANGCONSTANTS_H_ */
