///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef ANGCONSTANTS_H_
#define ANGCONSTANTS_H_

/**
 * @file AngConstants.h
 * @brief This file contains both c style macro definitions and C++ style const
 * declarations of TSL OIM variables and names. If you are dealing with a .ang
 * file from TSL instrumentation then you MUST use the defines from this file.
 */

#include <string>
// -----------------------------------------------------------------------------
//
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
//#define ANG_NCOLS             "NCOLS"
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
#define ANG_PHASE_DATA "Phase"
#define ANG_X_POSITION "X Position"
#define ANG_Y_POSITION "Y Position"
#define ANG_Z_STEP "ZStep"
#define ANG_Z_POS "ZPos"
#define ANG_Z_MAX "ZMax"




namespace TSL {
 namespace OIM
 {
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
#define ANG_TEM_PIXPERUM_LOWER "tem_pixperum"
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


namespace TSL {


namespace OIM
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
 }

}
#endif

#endif /* ANGCONSTANTS_H_ */
