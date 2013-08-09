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
#include "GenerateIPFColors.h"


#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/Math/MatrixMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIPFColors::GenerateIPFColors() :
  AbstractFilter(),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CellIPFColorsArrayName(DREAM3D::CellData::IPFColor),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL),
  m_CellIPFColors(NULL)
{

  m_ReferenceDir.x = 0.0f;
  m_ReferenceDir.y = 0.0f;
  m_ReferenceDir.z = 1.0f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIPFColors::~GenerateIPFColors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();

    option->setHumanLabel("Reference Direction");
    option->setPropertyName("ReferenceDir");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("ReferenceDir", getReferenceDir() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellIPFColors, ss, uint8_t, UInt8ArrayType, 0, voxels, 3)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::execute()
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

  // Make sure we are dealing with a unit 1 vector.
  MatrixMath::NormalizeVector(m_ReferenceDir.x, m_ReferenceDir.y, m_ReferenceDir.z);

  uint8_t hkl[3] = { 0, 0, 0 };
  // Write the IPF Coloring Cell Data
  for (int64_t i = 0; i < totalPoints; i++)
  {
    phase = m_CellPhases[i];
    index = i * 3;
    m_CellIPFColors[index] = 0;
    m_CellIPFColors[index + 1] = 0;
    m_CellIPFColors[index + 2] = 0;

    if(missingGoodVoxels == true || m_GoodVoxels[i] == true)
    {
      if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Cubic_High)
      {
        EbsdColoring::GenerateCubicIPFColor(m_CellEulerAngles[index], m_CellEulerAngles[index + 1], m_CellEulerAngles[index + 2],
            m_ReferenceDir.x, m_ReferenceDir.y, m_ReferenceDir.z, m_CellIPFColors + index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Hexagonal_High)
      {
        EbsdColoring::GenerateHexIPFColor(m_CellEulerAngles[index], m_CellEulerAngles[index + 1], m_CellEulerAngles[index + 2],
            m_ReferenceDir.x, m_ReferenceDir.y, m_ReferenceDir.z, m_CellIPFColors + index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Trigonal_High)
      {
        EbsdColoring::GenerateTrigIPFColor(m_CellEulerAngles[index], m_CellEulerAngles[index + 1], m_CellEulerAngles[index + 2],
            m_ReferenceDir.x, m_ReferenceDir.y, m_ReferenceDir.z, m_CellIPFColors + index);
      }
      else if(m_CrystalStructures[phase] == Ebsd::CrystalStructure::Tetragonal_High)
      {
        EbsdColoring::GenerateTetraIPFColor(m_CellEulerAngles[index], m_CellEulerAngles[index + 1], m_CellEulerAngles[index + 2],
            m_ReferenceDir.x, m_ReferenceDir.y, m_ReferenceDir.z, m_CellIPFColors + index);
      }
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
