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


#include "FeatureInfoReader.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include <QtCore/QFileInfo>

#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/GenericFilters/RenumberFeatures.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureInfoReader::FeatureInfoReader() :
  FileReader(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_InputFile(""),
  m_CreateCellLevelArrays(true),
  m_RenumberFeatures(true),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_FeaturePhases(NULL),
  m_FeatureEulerAngles(NULL),
  m_Active(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureInfoReader::~FeatureInfoReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Feature Info File");
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
    option->setHumanLabel("Renumber Features");
    option->setPropertyName("RenumberFeatures");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setCreateCellLevelArrays( reader->readValue("CreateCellLevelArrays", getCreateCellLevelArrays()) );
  setRenumberFeatures( reader->readValue("RenumberFeatures", getRenumberFeatures()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureInfoReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("CreateCellLevelArrays", getCreateCellLevelArrays() );
  writer->writeValue("RenumberFeatures", getRenumberFeatures() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{

  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist.");
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<int> dims(1, 1);
  m_FeatureIds = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims);

  if (m_CreateCellLevelArrays)
  {
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, int32_t, Int32ArrayType, 0, voxels, dims)
    dims[0] = 3;
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, float, FloatArrayType, 0, voxels, dims)
  }
  dims[0] = 1;
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFeatureData, Active, bool, BoolArrayType, true, features, dims)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFeatureData, FeaturePhases, int32_t, Int32ArrayType, 0, features, dims)
  dims[0] = 3;
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFeatureData, FeatureEulerAngles, float, FloatArrayType, 0, features, dims)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
    vdc->setName(getDataContainerName());
    getDataContainerArray()->pushBack(vdc);
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  }

  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int  FeatureInfoReader::readHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int  FeatureInfoReader::readFile()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  std::ifstream inFile;
  inFile.open(getInputFile().toLatin1().data(), std::ios_base::binary);
  if(!inFile)
  {

    QString ss = QObject::tr("Failed to open: %1").arg(getInputFile());
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss, -1);
    return -1;
  }
  int numfeatures;
  int gnum, phase;
  int maxphase = 0;
  float ea1, ea2, ea3;
  inFile >> numfeatures;

  // Now that we know how many unique features are in the file initialize the Feature Map to the proper size:
  int64_t totalPoints = m->getTotalPoints();
  size_t totalEnsembles = m->getNumCellEnsembleTuples();
  dataCheck(false, totalPoints, numfeatures + 1, totalEnsembles);

  // Create and initialize the Feature Active Array with a default value of true
  BoolArrayType::Pointer featureActive = BoolArrayType::CreateArray(numfeatures + 1, DREAM3D::FeatureData::Active);
  featureActive->initializeWithValues(true);

  // Initialize arrays to hold the data for the Euler Data
  QVector<int> dims(1, 3);
  FloatArrayType::Pointer featureEulerData = FloatArrayType::CreateArray(numfeatures + 1, dims, DREAM3D::FeatureData::EulerAngles);
  featureEulerData->initializeWithZeros();

  // Create and initialize the Feature Phase Array with a default value of the "Unkown Phase Type"
  Int32ArrayType::Pointer featurePhaseData = Int32ArrayType::CreateArray(numfeatures + 1, DREAM3D::FeatureData::Phases);
  featurePhaseData->initializeWithValues(999);
  for(int i = 0; i < numfeatures; i++)
  {
    inFile >> gnum >> phase >> ea1 >> ea2 >> ea3;
    if(gnum >= featureActive->GetSize())
    {
      featureActive->Resize(gnum + 1);
      featurePhaseData->Resize(gnum + 1);
      featureEulerData->Resize(gnum + 1);
    }
    featureEulerData->SetValue(3 * gnum, ea1);
    featureEulerData->SetValue(3 * gnum + 1, ea2);
    featureEulerData->SetValue(3 * gnum + 2, ea3);
    featurePhaseData->SetValue(gnum, phase);
    if(phase > maxphase) { maxphase = phase; }
  }
  m->addCellFeatureData(DREAM3D::FeatureData::EulerAngles, featureEulerData);
  m->addCellFeatureData(DREAM3D::FeatureData::Phases, featurePhaseData);
  m->addCellFeatureData(DREAM3D::FeatureData::Active, featureActive);

  if (m_CreateCellLevelArrays == true)
  {
    QVector<int> dims(1, 3);
    FloatArrayType::Pointer cellEulerData = FloatArrayType::CreateArray(totalPoints, dims, DREAM3D::FeatureData::EulerAngles);
    cellEulerData->initializeWithZeros();
    Int32ArrayType::Pointer cellPhaseData = Int32ArrayType::CreateArray(totalPoints, DREAM3D::FeatureData::Phases);
    cellPhaseData->initializeWithValues(999);
    for(int i = 0; i < totalPoints; i++)
    {
      gnum = m_FeatureIds[i];
      cellEulerData->SetValue(3 * i, featureEulerData->GetValue(3 * gnum));
      cellEulerData->SetValue(3 * i + 1, featureEulerData->GetValue(3 * gnum + 1));
      cellEulerData->SetValue(3 * i + 2, featureEulerData->GetValue(3 * gnum + 2));
      cellPhaseData->SetValue(i, featurePhaseData->GetValue(gnum));
    }
    m->addCellData(DREAM3D::CellData::EulerAngles, cellEulerData);
    m->addCellData(DREAM3D::CellData::Phases, cellPhaseData);
  }

  if (m_RenumberFeatures == true)
  {
    totalPoints = m->getTotalPoints();
    size_t totalFeatures = m->getNumCellFeatureTuples();
    if (0 == totalFeatures)
    {
      notifyErrorMessage("The number of features is Zero and should be greater than Zero", -600);
      notifyStatusMessage("Completed");
      return -999;
    }
    dataCheck(false, totalPoints, totalFeatures, m->getNumCellEnsembleTuples());



    // Find the unique set of feature ids
    for (size_t i = 1; i < totalFeatures; ++i)
    {
      m_Active[i] = false;
    }
    for (int64_t i = 0; i < totalPoints; ++i)
    {
      m_Active[m_FeatureIds[i]] = true;
    }

    RenumberFeatures::Pointer renum = RenumberFeatures::New();
    renum->setDataContainerArray(getDataContainerArray());
    renum->setObservers(getObservers());
    renum->setMessagePrefix(getMessagePrefix());
    renum->execute();
    setErrorCondition(renum->getErrorCondition());
    addErrorMessages(renum->getPipelineMessages());
  }

  notifyStatusMessage("Complete");
  return 0;
}

