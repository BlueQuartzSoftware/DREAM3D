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





#ifndef _angconstants_h_
#define _angconstants_h_

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
#define ANG_PATTERN_CENTER_CALIBRATION "Pattern Center Calibration"
#define ANG_WORKINGDISTANCE   "WorkingDistance"
#define ANG_WORKING_DISTANCe  "Working Distance"
#define ANG_PHASE             "Phase"
#define ANG_MATERIAL_NAME     "MaterialName"
#define ANG_FORMULA           "Formula"
#define ANG_INFO              "Info"
#define ANG_SYMMETRY          "Symmetry"
#define ANG_LATTICE_CONSTANTS "LatticeConstants"
#define ANG_NUMBER_FAMILIES   "NumberFamilies"
#define ANG_HKLFAMILIES       "hklFamilies"
#define ANG_HKL_FAMILIES      "hkl Families"
#define ANG_CATEGORIES        "Categories"
#define ANG_GRID              "GRID"
#define ANG_GRID_TYPE         "Grid Type"
#define ANG_X_STEP            "XSTEP"
#define ANG_Y_STEP            "YSTEP"
#define ANG_STEP_X            "Step X"
#define ANG_STEP_Y            "Step Y"
#define ANG_NCOLS_ODD         "NCOLS_ODD"
#define ANG_NCOLS_EVEN        "NCOLS_EVEN"
#define ANG_NROWS             "NROWS"
#define ANG_nColumns          "nColumns"
#define ANG_nROWS             "nRows"
#define ANG_OPERATOR          "OPERATOR"
#define ANG_SAMPLE_ID         "SAMPLEID"
#define ANG_SCAN_ID           "SCANID"
#define ANG_Scan_ID            "Scan ID"

#define ANG_Operator          "Operator"
#define ANG_Sample_ID         "Sample ID"

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
#define ANG_CI "CI"
#define ANG_IQ "IQ"
#define ANG_PATTERN_DATA "Pattern"
#define ANG_PATTERN_WIDTH "Pattern Width"
#define ANG_PATTERN_HEIGHT "Pattern Height"



namespace Ebsd
{

  namespace Ang
  {
    const QString Manufacturer("TSL");

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
      const unsigned int Triclinic = CIs;
      const unsigned int UnknownSymmetry = 999;
    }

    const QString TEMPIXPerUM(ANG_TEM_PIXPERUM);
    const QString XStar(ANG_X_STAR);
    const QString YStar(ANG_Y_STAR);
    const QString ZStar(ANG_Z_STAR);
    const QString WorkingDistance(ANG_WORKINGDISTANCE);
    const QString Working_Distance(ANG_WORKING_DISTANCe);
    const QString Phase(ANG_PHASE);
    const QString MaterialName(ANG_MATERIAL_NAME);
    const QString Formula(ANG_FORMULA);
    const QString Info(ANG_INFO);
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

  }
}

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
    const QString H5FileExt("h5");
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
  }

  namespace AngFile
  {
    const QString Phases("Phases");
    const QString EulerAngles("EulerAngles");
    const QString CrystalStructures("CrystalStructures");
    const QString MaterialName("MaterialName");
    const QString LatticeConstants("LatticeConstants");
    const QString BravaisLattice("BravaisLattice");
  }

}
#endif

#endif /* ANGCONSTANTS_H_ */

