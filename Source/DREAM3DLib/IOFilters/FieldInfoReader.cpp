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

#include "MXA/Utilities/MXAFileInfo.h"

#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FieldInfoReader::FieldInfoReader() :
  FileReader(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_InputFile(""),
  m_CreateCellLevelArrays(true),
  m_RenumberGrains(true),
  m_GrainIds(NULL),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_FieldPhases(NULL),
  m_FieldEulerAngles(NULL),
  m_Active(NULL)
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
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    option->setFileExtension("*.txt");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Create Cell Level Arrays");
    option->setPropertyName("CreateCellLevelArrays");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Renumber Grains");
    option->setPropertyName("RenumberGrains");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldInfoReader::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldInfoReader::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("CreateCellLevelArrays", getCreateCellLevelArrays() );
  writer->writeValue("RenumberGrains", getRenumberGrains() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldInfoReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if (getInputFile().empty() == true)
  {
    ss << ClassName() << " needs the Input File Set and it was not.";
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else if (MXAFileInfo::exists(getInputFile()) == false)
  {
    ss << "The input file does not exist.";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)

  if (m_CreateCellLevelArrays)
  {
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, float, FloatArrayType, 0, voxels, 3)
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, int32_t, Int32ArrayType, 0, voxels, 1)
  }
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)
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
int  FieldInfoReader::readHeader()
{
  return 0;
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
  inFile.open(getInputFile().c_str(), std::ios_base::binary);
  if(!inFile)
  {
    std::stringstream ss;
    ss << "Failed to open: " << getInputFile();
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return -1;
  }
  int numgrains;
  int gnum, phase;
  int maxphase = 0;
  float ea1, ea2, ea3;
  inFile >> numgrains;

  // Now that we know how many unique grains are in the file initialize the Field Map to the proper size:
  int64_t totalPoints = m->getTotalPoints();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains + 1, totalEnsembles);

  // Create and initialize the Field Active Array with a default value of true
  BoolArrayType::Pointer fieldActive = BoolArrayType::CreateArray(numgrains+1, DREAM3D::FieldData::Active);
  fieldActive->initializeWithValues(true);

  // Initialize arrays to hold the data for the Euler Data
  FloatArrayType::Pointer fieldEulerData = FloatArrayType::CreateArray(numgrains+1, 3, DREAM3D::FieldData::EulerAngles);
  fieldEulerData->SetNumberOfComponents(3);
  fieldEulerData->initializeWithZeros();

  // Create and initialize the Field Phase Array with a default value of the "Unkown Phase Type"
  Int32ArrayType::Pointer fieldPhaseData = Int32ArrayType::CreateArray(numgrains+1, DREAM3D::FieldData::Phases);
  fieldPhaseData->initializeWithValues(999);
  for(int i=0;i<numgrains;i++)
  {
    inFile >> gnum >> phase >> ea1 >> ea2 >> ea3;
    if(gnum >= fieldActive->GetSize())
    {
      fieldActive->Resize(gnum+1);
      fieldPhaseData->Resize(gnum+1);
      fieldEulerData->Resize(gnum+1);
    }
    fieldEulerData->SetValue(3*gnum, ea1);
    fieldEulerData->SetValue(3*gnum+1, ea2);
    fieldEulerData->SetValue(3*gnum+2, ea3);
    fieldPhaseData->SetValue(gnum, phase);
    if(phase > maxphase) maxphase = phase;
  }
  m->addFieldData(DREAM3D::FieldData::EulerAngles, fieldEulerData);
  m->addFieldData(DREAM3D::FieldData::Phases, fieldPhaseData);
  m->addFieldData(DREAM3D::FieldData::Active, fieldActive);

  if (m_CreateCellLevelArrays == true)
  {
    FloatArrayType::Pointer cellEulerData = FloatArrayType::CreateArray(totalPoints, 3, DREAM3D::FieldData::EulerAngles);
    cellEulerData->initializeWithZeros();
    Int32ArrayType::Pointer cellPhaseData = Int32ArrayType::CreateArray(totalPoints, DREAM3D::FieldData::Phases);
    cellPhaseData->initializeWithValues(999);
    for(int i=0;i<totalPoints;i++)
    {
      gnum = m_GrainIds[i];
      cellEulerData->SetValue(3*i, fieldEulerData->GetValue(3*gnum));
      cellEulerData->SetValue(3*i+1, fieldEulerData->GetValue(3*gnum+1));
      cellEulerData->SetValue(3*i+2, fieldEulerData->GetValue(3*gnum+2));
      cellPhaseData->SetValue(i, fieldPhaseData->GetValue(gnum));
    }
    m->addCellData(DREAM3D::CellData::EulerAngles, cellEulerData);
    m->addCellData(DREAM3D::CellData::Phases, cellPhaseData);
  }

  if (m_RenumberGrains == true)
  {
    totalPoints = m->getTotalPoints();
    size_t totalFields = m->getNumFieldTuples();
    if (0 == totalFields)
    {
      notifyErrorMessage("The number of grains is Zero and should be greater than Zero", -600);
      notifyStatusMessage("Completed");
      return -999;
    }
    dataCheck(false, totalPoints, totalFields, m->getNumEnsembleTuples());

    std::stringstream ss;

    // Find the unique set of grain ids
    for (size_t i = 1; i < totalFields; ++i)
    {
      m_Active[i] = false;
    }
    for (int64_t i = 0; i < totalPoints; ++i)
    {
      m_Active[m_GrainIds[i]] = true;
    }

    RenumberGrains::Pointer renum = RenumberGrains::New();
    renum->setVoxelDataContainer(m);
    renum->setObservers(getObservers());
    renum->setMessagePrefix(getMessagePrefix());
    renum->execute();
    setErrorCondition(renum->getErrorCondition());
    addErrorMessages(renum->getPipelineMessages());
  }

  notifyStatusMessage("Complete");
  return 0;
}

