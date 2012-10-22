/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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


#include "FieldInfoReader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "DREAM3DLib/Common/DataArray.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FieldInfoReader::FieldInfoReader() :
FileReader(),
  m_InputInfoFile(""),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_GrainIds(NULL),
m_CellPhases(NULL),
m_CellEulerAngles(NULL),
m_FieldPhases(NULL),
m_FieldEulerAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FieldInfoReader::~FieldInfoReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldInfoReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Field Info File");
    option->setPropertyName("InputInfoFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldInfoReader::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("InputInfoFile", getInputInfoFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldInfoReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if (getInputInfoFile().empty() == true)
  {
    std::stringstream ss;
    ss << ClassName() << " needs the Input Grain Info File Set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -4);
    setErrorCondition(-387);
  }
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, float, FloatArrayType, 0, voxels, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, int32_t, Int32ArrayType, 0, voxels, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, int32_t, Int32ArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, float, FloatArrayType, 0, fields, 3)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldInfoReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int  FieldInfoReader::readFile()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    std::stringstream ss;
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "("<<__LINE__<<")";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-1);
    return -1;
  }

  std::ifstream inFile;
  inFile.open(getInputInfoFile().c_str(), std::ios_base::binary);
  if(!inFile)
  {
    std::stringstream ss;
    ss << "Failed to open: " << getInputInfoFile();
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return -1;
  }
  int numgrains;
  int gnum, phase;
  int maxphase = 0;
  float ea1, ea2, ea3;
  inFile >> numgrains;
  FloatArrayType::Pointer m_FieldEulerData = FloatArrayType::CreateArray(3*(numgrains+1), DREAM3D::FieldData::EulerAngles);
  m_FieldEulerData->SetNumberOfComponents(3);
  Int32ArrayType::Pointer m_FieldPhaseData = Int32ArrayType::CreateArray(numgrains+1, DREAM3D::FieldData::Phases);
  for(int i=0;i<numgrains;i++)
  {
    inFile >> gnum >> phase >> ea1 >> ea2 >> ea3;
    m_FieldEulerData->SetValue(3*gnum, ea1);
    m_FieldEulerData->SetValue(3*gnum+1, ea2);
    m_FieldEulerData->SetValue(3*gnum+2, ea3);
    m_FieldPhaseData->SetValue(gnum, phase);
    if(phase > maxphase) maxphase = phase;
  }
  m->addFieldData(DREAM3D::FieldData::EulerAngles, m_FieldEulerData);
  m->addFieldData(DREAM3D::FieldData::Phases, m_FieldPhaseData);

  int64_t totalPoints = m->getTotalPoints();
  FloatArrayType::Pointer m_CellEulerData = FloatArrayType::CreateArray(3*totalPoints, DREAM3D::FieldData::EulerAngles);
  m_CellEulerData->SetNumberOfComponents(3);
  Int32ArrayType::Pointer m_CellPhaseData = Int32ArrayType::CreateArray(totalPoints, DREAM3D::FieldData::Phases);
  for(int i=0;i<totalPoints;i++)
  {
    gnum = m_GrainIds[i];
    m_CellEulerData->SetValue(3*i, m_FieldEulerData->GetValue(3*gnum));
    m_CellEulerData->SetValue(3*i+1, m_FieldEulerData->GetValue(3*gnum+1));
    m_CellEulerData->SetValue(3*i+2, m_FieldEulerData->GetValue(3*gnum+2));
    m_CellPhaseData->SetValue(i, m_FieldPhaseData->GetValue(gnum));
  }
  m->addCellData(DREAM3D::CellData::EulerAngles, m_CellEulerData);
  m->addCellData(DREAM3D::CellData::Phases, m_CellPhaseData);

  notifyStatusMessage("Complete");
  return 0;
}
