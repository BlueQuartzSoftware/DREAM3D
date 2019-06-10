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

#include "SIMPLib/Common/Constants.h"


/**
 * @file EbsdConstants.h
 * @brief This file contains many constants that are generic to the EBSD library
 */
namespace Ebsd
{

namespace H5Ebsd
{
const QString Manufacturer("Manufacturer");
const QString Header("Header");
const QString Phases("Phases");
const QString Phase("Phase");
const QString Data("Data");
const QString Index("Index");

const QString ZStartIndex("ZStartIndex");
const QString ZEndIndex("ZEndIndex");
const QString ZResolution("Z Resolution");
const QString StackingOrder("Stacking Order");
const QString SampleTransformationAngle("SampleTransformationAngle");
const QString SampleTransformationAxis("SampleTransformationAxis");
const QString EulerTransformationAngle("EulerTransformationAngle");
const QString EulerTransformationAxis("EulerTransformationAxis");

// Each Manufacturer has their own naming scheme for these variables but for
// DREAM.3D we are going to settle on using these names for consistency
const QString XResolution("X Resolution");
const QString YResolution("Y Resolution");

// We store the Maximum number of X and Y Points for the given volume. This
// allows us to store slices that have different XY voxel dimensions.
const QString XPoints("Max X Points");
const QString YPoints("Max Y Points");

const QString FileVersionStr("FileVersion");
const unsigned int FileVersion = 5;
const QString EbsdLibVersionStr("EbsdLibVersion");
} // namespace H5Ebsd

using EnumType = int32_t;
enum class OEM : EnumType
{
  EDAX = 0,
  Oxford = 1,
  Bruker = 2,
  HEDM = 3,
  Zeiss = 4,
  Phillips = 5,
  ThermoFisher = 6,
  DREAM3D = 7,
  Unknown = 8
};

namespace CellData
{
const QString EulerAngles("EulerAngles");
const QString Phases("Phases");
} // namespace CellData

  enum EbsdToSampleCoordinateMapping
  {
    TSLdefault = 0,
    HKLdefault = 1,
    HEDMdefault = 2,
    UnknownCoordinateMapping = 3
  };

  namespace StackingOrder
  {
    const QString LowToHigh("Low To High");
    const QString HighToLow("High To Low");
    const QString UnknownStackingOrder("Unknown Stacking Order");

    class Utils
    {
      public:
        static QString getStringForEnum(unsigned int v)
        {
          if (SIMPL::RefFrameZDir::LowtoHigh == v) { return Ebsd::StackingOrder::LowToHigh; }
          if (SIMPL::RefFrameZDir::HightoLow == v) { return Ebsd::StackingOrder::HighToLow; }
          return Ebsd::StackingOrder::UnknownStackingOrder;
        }

        static int getEnumForString(const QString& v)
        {
          if (Ebsd::StackingOrder::LowToHigh.compare(v) == 0) { return SIMPL::RefFrameZDir::LowtoHigh; }
          if (Ebsd::StackingOrder::HighToLow.compare(v) == 0) { return SIMPL::RefFrameZDir::HightoLow; }
          return SIMPL::RefFrameZDir::UnknownRefFrameZDirection;
        }
    };
  }




  /**
   * @brief IF YOU CHANGE THE VALUES THERE ARE DEEP RAMIFICATIONS IN THE CODE BUT
   * MOSTLY IN THE HDF5 FILES WHICH ARE WRITTEN USING THE ENUMERATIONS.
   */
  namespace CrystalStructure
  {

    const unsigned int Triclinic = 4; //!< Triclinic -1
    const unsigned int Monoclinic = 5; //!< Monoclinic 2/m
    const unsigned int OrthoRhombic = 6; //!< Orthorhombic mmm
    const unsigned int Tetragonal_Low = 7; //!< Tetragonal-Low 4/m
    const unsigned int Tetragonal_High = 8; //!< Tetragonal-High 4/mmm
    const unsigned int Trigonal_Low = 9; //!< Trigonal-Low -3
    const unsigned int Trigonal_High = 10; //!< Trigonal-High -3m

    const unsigned int Hexagonal_Low = 2; //!< Hexagonal-Low 6/m
    const unsigned int Hexagonal_High = 0; //!< Hexagonal-High 6/mmm
    const unsigned int Cubic_Low = 3; //!< Cubic Cubic-Low m3 (Tetrahedral)
    const unsigned int Cubic_High = 1; //!< Cubic Cubic-High m3m

    const unsigned int LaueGroupEnd = 11; //!< The end of the Laue groups
    const unsigned int UnknownCrystalStructure = 999; //!< UnknownCrystalStructure
  }

  namespace BravaisLattice
  {
    const QString Unknown("Unknown");
    const QString Cubic("Cubic");
    const QString Hexagonal("Hexagonal");
  }

  enum NumType
  {
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

  namespace AngleRepresentation
  {
    static const int Radians = 0;
    static const int Degrees = 1;
    static const int Invalid = 2;
  };
}



