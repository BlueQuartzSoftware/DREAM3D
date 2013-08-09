/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "GenerateRodriguesColors.h"



#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/CubicLowOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalLowOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/OrientationOps/MonoclinicOps.h"
#include "DREAM3DLib/OrientationOps/TriclinicOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateRodriguesColors::GenerateRodriguesColors() :
  AbstractFilter(),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CellRodriguesColorsArrayName(DREAM3D::CellData::RodriguesColor),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL),
  m_CellRodriguesColors(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateRodriguesColors::~GenerateRodriguesColors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if (NULL == m)
  {
    ss.str("");
    ss << getHumanLabel() << "The VoxelDataContainer was NULL and this is NOT allowed. There is an error in the programming. Please contact the developers";
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return;
  }

  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType,  voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -300, float, FloatArrayType, voxels, 3)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellRodriguesColors, ss, uint8_t, UInt8ArrayType, 0, voxels, 3)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  bool* m_GoodVoxels;
  BoolArrayType* goodVoxels = NULL;
  bool missingGoodVoxels = false;
  IDataArray::Pointer gvPtr = m->getCellData(m_GoodVoxelsArrayName);

  if (m->getCellData(m_GoodVoxelsArrayName).get() == NULL)
  {
    missingGoodVoxels = true;
  }
  else
  {
    goodVoxels = BoolArrayType::SafePointerDownCast(gvPtr.get());
    m_GoodVoxels = goodVoxels->GetPointer(0);
  }



  int phase;
  size_t index = 0;
  float r1, r2, r3;

  // Write the IPF Coloring Cell Data
  for (int64_t i = 0; i < totalPoints; i++)
  {
    phase = m_CellPhases[i];
    index = i * 3;
    m_CellRodriguesColors[index] = 0;
    m_CellRodriguesColors[index + 1] = 0;
    m_CellRodriguesColors[index + 2] = 0;

    if(missingGoodVoxels == true || m_GoodVoxels[i] == true)
    {
      OrientationMath::EulertoRod(r1, r2, r3, m_CellEulerAngles[index], m_CellEulerAngles[index + 1], m_CellEulerAngles[index + 2]);
      if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Cubic_High)
      {
        CubicOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Cubic_Low)
      {
        CubicLowOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Hexagonal_High)
      {
        HexagonalOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Hexagonal_Low)
      {
        HexagonalLowOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Trigonal_High)
      {
        TrigonalOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Trigonal_Low)
      {
        TrigonalLowOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Tetragonal_High)
      {
        TetragonalOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Tetragonal_Low)
      {
        TetragonalLowOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::OrthoRhombic)
      {
        OrthoRhombicOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Monoclinic)
      {
        MonoclinicOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Triclinic)
      {
        TriclinicOps ops;
        ops.generateRodriguesColor(r1, r2, r3, m_CellRodriguesColors+index);
      }
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
