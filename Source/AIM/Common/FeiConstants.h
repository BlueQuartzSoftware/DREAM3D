///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Mike Jackson,
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __FeiConstants_H
#define __FeiConstants_H

/**
 *  @file FEI/FeiConstants.h
 *  @brief This file contains string constants for use with Fei instrument code
 *
 *  @author by Michael A Jackson on 4/11/06.
 *  Copyright 2006 IMTS. All rights reserved.
 *  @version $Revision: 1.2 $
 */

//-- C++ includes
#include <string>

// -----------------------------------------------------------------------------
//  Defines for Fei Instrument. Use the C++ const std::strings in your code
// -----------------------------------------------------------------------------

#define FEI_SLICE_NAME           "FIB Slice"
#define FEI_SLICE_ALT_NAME       "FIB Slice"
#define FEI_SLICE_INDEX           0

#define FEI_ISE                  "Ion Induced SEI"
#define FEI_SEI                  "Secondary Electron Image"
#define FEI_EBSD                 "OIM"
#define FEI_IMAGE_QUALITY        "Image Quality"
#define FEI_CONFIDENCEINDEX      "Confidence Index"
#define FEI_PHASE                "Phase"
#define FEI_EULER_ANGLES         "Euler Angles"
#define FEI_PHI_1                "phi1"
#define FEI_PHI                  "phi"
#define FEI_PHI_2                "phi2"

// -----------------------------------------------------------------------------
//  Paths for the Data Records
// -----------------------------------------------------------------------------
#define FEI_OIM_PATH              "OIM"
#define FEI_IMAGE_QUALITY_PATH     "OIM/Image Quality"
#define FEI_CONFIDENCEINDEX_PATH   "OIM/Confidence Index"
#define FEI_PHASE_PATH             "OIM/Phase"

#define FEI_EULER_ANGLES_PATH      "OIM/Euler Angles"
#define FEI_PHI_1_PATH             "OIM/Euler Angles/phi1"
#define FEI_PHI_PATH               "OIM/Euler Angles/phi"
#define FEI_PHI_2_PATH             "OIM/Euler Angles/phi2"

// -----------------------------------------------------------------------------
// Group Types if needed
// -----------------------------------------------------------------------------
#define GROUP_TYPE_SLICE        "Slice Data"
#define GROUP_TYPE_SERIES_DATA  "Tilt Data"
#define GROUP_TYPE_EULER_ANGLES "Euler Angle Data"
#define GROUP_TYPE_EBSD         "OIM Data"


// -----------------------------------------------------------------------------
// These are the Data Record GUIDS for the OIM + FIB Datasets
// -----------------------------------------------------------------------------
#define FEI_SEI_GUID             0
#define FEI_EBSD_GUID            1
#define FEI_IMAGE_QUALITY_GUID   2
#define FEI_CONFIDENCEINDEX_GUID  3
#define FEI_PHASE_GUID           4
#define FEI_EULER_ANGLES_GUID     5
#define FEI_PHI_1_GUID            6
#define FEI_PHI_GUID             7
#define FEI_PHI_2_GUID            8

#define FEI_SEI_LUID             0
#define FEI_EBSD_LUID            1
#define FEI_IMAGE_QUALITY_LUID   0
#define FEI_CONFIDENCEINDEX_LUID 1
#define FEI_PHASE_LUID           2
#define FEI_EULER_ANGLES_LUID    3
#define FEI_PHI_1_LUID           0
#define FEI_PHI_LUID             1
#define FEI_PHI_2_LUID           2

#define DUMMY_DATA    "Dummy Data"

#define FEI_ATTR_SLICE          "Slice"
#define FEI_ATTR_FILENAME       "Filename"
#define FEI_SCALE               "Scale"
#define FEI_ATTR_DESCRIPTION     "Description"

// -----------------------------------------------------------------------------
// Constants for .ang files
// -----------------------------------------------------------------------------
#define OIM_TEM_PIXPERUM     "TEM_PIXperUM"
#define OIM_X_STAR           "X-Star"
#define OIM_Y_STAR           "Y-Star"
#define OIM_Z_STAR           "Z-Star"
#define OIM_WORKING_DISTANCE "Working Distance"

#define OIM_PHASE             "Phase"
#define OIM_MATERIAL_NAME     "Material Name"
#define OIM_FORMULA           "Formula"
#define OIM_INFO              "Info"
#define OIM_SYMMETRY          "Symmetry"
#define OIM_LATTICE_CONSTANTS "Lattice Constants"
#define OIM_NUMBER_FAMILIES   "Number Families"
#define OIM_HKL_FAMILIES      "hkl Families"
#define OIM_CATEGORIES        "Categories"
#define OIM_GRID              "Grid"
#define OIM_X_STEP            "x-Step"
#define OIM_Y_STEP            "y-Step"
#define OIM_NCOLS_ODD         "Ncols Odd"
#define OIM_NCOLS_EVEN        "Ncols Even"
#define OIM_NROWS             "NRows"
#define OIM_OPERATOR          "Operator"
#define OIM_SAMPLE_ID         "Sampleid"
#define OIM_SCAN_ID           "Scan ID"

namespace Fei
{
  const std::string SliceName(FEI_SLICE_NAME);

  const std::string AttrSlice(FEI_ATTR_SLICE);
  const std::string AttrFilename(FEI_ATTR_FILENAME);
  const std::string Scale(FEI_SCALE);
  const std::string AttrDescription(FEI_ATTR_DESCRIPTION);

// -----------------------------------------------------------------------------
//  Constants Related to the Data Model
// -----------------------------------------------------------------------------

  // Define the Names of the Data Records
  const std::string SEI (FEI_SEI);
  const std::string EBSD(FEI_EBSD);
  const std::string ImageQuality(FEI_IMAGE_QUALITY);
  const std::string ConfidenceIndex(FEI_CONFIDENCEINDEX);
  const std::string Phase(FEI_PHASE);
  const std::string EulerAngles(FEI_EULER_ANGLES);
  const std::string Phi1(FEI_PHI_1);
  const std::string Phi(FEI_PHI);
  const std::string Phi2(FEI_PHI_2);

  // Define the Data Record Paths within the Model
  const std::string OimPath(FEI_OIM_PATH);
  const std::string ImageQualityPath(FEI_IMAGE_QUALITY_PATH);
  const std::string ConfidenceIndexPath(FEI_CONFIDENCEINDEX_PATH);
  const std::string PhasePath(FEI_PHASE_PATH);
  const std::string EulerAnglesPath(FEI_EULER_ANGLES_PATH);
  const std::string Phi1Path(FEI_PHI_1_PATH);
  const std::string PhiPath(FEI_PHI_PATH);
  const std::string Phi2Path(FEI_PHI_2_PATH);

  namespace Oim
  {
// -----------------------------------------------------------------------------
//  Constants related to the ANG file Header Information
// -----------------------------------------------------------------------------
    const std::string FileExt(".ang");
    const std::string TEMPIXPerUM (OIM_TEM_PIXPERUM);
    const std::string XStar (OIM_X_STAR   );
    const std::string YStar (OIM_Y_STAR);
    const std::string ZStar (OIM_Z_STAR);
    const std::string WorkingDistance (OIM_WORKING_DISTANCE);
    const std::string Phase (OIM_PHASE);
    const std::string MaterialName (OIM_MATERIAL_NAME);
    const std::string Formula (OIM_FORMULA);
    const std::string Info (OIM_INFO);
    const std::string Symmetry (OIM_SYMMETRY);
    const std::string LatticeConstants (OIM_LATTICE_CONSTANTS);
    const std::string NumberFamilies (OIM_NUMBER_FAMILIES);
    const std::string HKLFamilies (OIM_HKL_FAMILIES);
    const std::string Categories (OIM_CATEGORIES);
    const std::string Grid (OIM_GRID );
    const std::string XStep (OIM_X_STEP );
    const std::string YStep (OIM_Y_STEP );
    const std::string NColsOdd (OIM_NCOLS_ODD);
    const std::string NColsEven (OIM_NCOLS_EVEN );
    const std::string NRows (OIM_NROWS );
    const std::string Operator (OIM_OPERATOR);
    const std::string SampleId (OIM_SAMPLE_ID);
    const std::string ScanId (OIM_SCAN_ID);
  }


}


#endif
