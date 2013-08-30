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

#ifndef _EbsdConstants_h_
#define _EbsdConstants_h_

#include <string>

/**
 * @file EbsdConstants.h
 * @brief This file contains many constants that are generic to the EBSD library
 */


namespace Ebsd
{
  enum Manufacturer
  {
    TSL = 0,
    HKL = 1,
    HEDM = 2,
    UnknownManufacturer
  };


  namespace H5 {
    const std::string OriginalHeader("OriginalHeader");
    const std::string ZStartIndex("ZStartIndex");
    const std::string ZEndIndex("ZEndIndex");
    const std::string ZResolution("Z Resolution");
    const std::string StackingOrder("Stacking Order");
    const std::string SampleTransformationAngle("SampleTransformationAngle");
    const std::string SampleTransformationAxis("SampleTransformationAxis");
    const std::string EulerTransformationAngle("EulerTransformationAngle");
    const std::string EulerTransformationAxis("EulerTransformationAxis");

    const std::string Manufacturer("Manufacturer");
    const std::string OriginalFile("OriginalFile");
    const std::string Index("Index");
    const std::string Header("Header");
    const std::string Phases("Phases");
    const std::string Data("Data");


    // Each Manufacturer has their own naming scheme for these variables but for
    // DREAM.3D we are going to settle on using these names for consistency
    const std::string XResolution("X Resolution");
    const std::string YResolution("Y Resolution");

    // We store the Maximum number of X and Y Points for the given volume. This
    // allows us to store slices that have different XY voxel dimensions.
    const std::string XPoints("Max X Points");
    const std::string YPoints("Max Y Points");

    const std::string FileVersionStr("FileVersion");
    const unsigned int FileVersion = 5;
    const std::string EbsdLibVersionStr("EbsdLibVersion");
  }

  /** @brief Constants defined for the 5 orientation options */


  enum RefFrameZDir
  {
     LowtoHigh = 0,
     HightoLow = 1,
     UnknownRefFrameZDirection
  };

  enum EbsdToSampleCoordinateMapping
  {
    TSLdefault = 0,
    HKLdefault = 1,
    HEDMdefault = 2,
    UnknownCoordinateMapping = 3
  };

  namespace StackingOrder {
    const std::string LowToHigh("Low To High");
    const std::string HighToLow("High To Low");
    const std::string UnknownStackingOrder("Unknown Stacking Order");

    class Utils
    {
      public:
        static std::string getStringForEnum(RefFrameZDir v)
        {
          if (LowtoHigh == v) return Ebsd::StackingOrder::LowToHigh;
          if (HightoLow == v) return Ebsd::StackingOrder::HighToLow;
          return Ebsd::StackingOrder::UnknownStackingOrder;
        }

        static RefFrameZDir getEnumForString(const std::string &v)
        {
          if (Ebsd::StackingOrder::LowToHigh.compare(v) == 0) return LowtoHigh;
          if (Ebsd::StackingOrder::HighToLow.compare(v) == 0) return HightoLow;
          return Ebsd::UnknownRefFrameZDirection;
        }
    };
  }




  /**
   * @brief IF YOU CHANGE THE VALUES THERE ARE DEEP RAMIFICATIONS IN THE CODE BUT
   * MOSTLY IN THE HDF5 FILES WHICH ARE WRITTEN USING THE ENUMERATIONS.
   */
  namespace CrystalStructure
  {

    const unsigned int Triclinic = 4; //!< TriClinic -1
    const unsigned int Monoclinic = 5; //!< Monoclinic 2/m
    const unsigned int OrthoRhombic = 6; //!< OrthoRhombic mmm
    const unsigned int Tetragonal_Low = 7; //!< Tetragonal-Low 4/m
    const unsigned int Tetragonal_High = 8; //!< Tetragonal-High 4/mmm
    const unsigned int Trigonal_Low = 9; //!< Trigonal-Low -3
    const unsigned int Trigonal_High = 10; //!< Trignal-High -3m

    const unsigned int Hexagonal_Low = 2; //!< Hexagonal-Low 6/m
    const unsigned int Hexagonal_High = 0; //!< Hexagonal-High 6/mmm
    const unsigned int Cubic_Low = 3; //!< Cubic Cubic-Low m3 (Tetrahedral)
    const unsigned int Cubic_High = 1; //!< Cubic Cubic-High m3m

    const unsigned int LaueGroupEnd = 11; //!< The end of the Laue groups
    const unsigned int UnknownCrystalStructure = 999; //!< UnknownCrystalStructure
  }

  namespace BravaisLattice
  {
    const std::string Unknown("Unknown");
    const std::string Cubic("Cubic");
    const std::string Hexagonal("Hexagonal");
  }

  enum NumType {
    Int8 = 0,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double,
    UnknownNumType
  };
}



#endif /* _EbsdConstants.h_  */
