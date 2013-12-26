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
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_InputFile(""),
  m_CreateCellLevelArrays(true),
  m_RenumberFeatures(true),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeatureEulerAngles(NULL),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
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
void FeatureInfoReader::dataCheck()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FeatureInfoReader>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix* cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist.");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (m_CreateCellLevelArrays)
  {
    m_CellPhasesPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_CellPhasesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 3;
    m_CellEulerAnglesPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_CellEulerAnglesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  dims[0] = 1;
  m_ActivePtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this,  m_ActiveArrayName, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeaturePhasesPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_FeaturePhasesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_FeatureEulerAnglesPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_FeatureEulerAnglesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::preflight()
{
  dataCheck();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureInfoReader::readHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureInfoReader::readFile()
{
  dataCheck();

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  std::ifstream inFile;
  inFile.open(getInputFile().toLatin1().data(), std::ios_base::binary);
  if(!inFile)
  {
    QString ss = QObject::tr("Failed to open: %1").arg(getInputFile());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    return -1;
  }
  int numfeatures;
  int gnum, phase;
  int maxphase = 0;
  float ea1, ea2, ea3;
  inFile >> numfeatures;

  // Create and initialize the Feature Active Array with a default value of true
  BoolArrayType::Pointer featureActive = BoolArrayType::CreateArray(numfeatures + 1, DREAM3D::FeatureData::Active);
  featureActive->initializeWithValue(true);

  // Initialize arrays to hold the data for the Euler Data
  QVector<size_t> dims(1, 3);
  FloatArrayType::Pointer featureEulerData = FloatArrayType::CreateArray(numfeatures + 1, dims, DREAM3D::FeatureData::EulerAngles);
  featureEulerData->initializeWithZeros();

  // Create and initialize the Feature Phase Array with a default value of the "Unkown Phase Type"
  Int32ArrayType::Pointer featurePhaseData = Int32ArrayType::CreateArray(numfeatures + 1, DREAM3D::FeatureData::Phases);
  featurePhaseData->initializeWithValue(999);
  for(int i = 0; i < numfeatures; i++)
  {
    inFile >> gnum >> phase >> ea1 >> ea2 >> ea3;
    if(gnum >= featureActive->getSize())
    {
      featureActive->resize(gnum + 1);
      featurePhaseData->resize(gnum + 1);
      featureEulerData->resize(gnum + 1);
    }
    featureEulerData->setValue(3 * gnum, ea1);
    featureEulerData->setValue(3 * gnum + 1, ea2);
    featureEulerData->setValue(3 * gnum + 2, ea3);
    featurePhaseData->setValue(gnum, phase);
    if(phase > maxphase) { maxphase = phase; }
  }
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(DREAM3D::FeatureData::EulerAngles, featureEulerData);
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(DREAM3D::FeatureData::Phases, featurePhaseData);
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(DREAM3D::FeatureData::Active, featureActive);

  if (m_CreateCellLevelArrays == true)
  {
    int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
    QVector<size_t> dims(1, 3);
    FloatArrayType::Pointer cellEulerData = FloatArrayType::CreateArray(totalPoints, dims, DREAM3D::FeatureData::EulerAngles);
    cellEulerData->initializeWithZeros();
    Int32ArrayType::Pointer cellPhaseData = Int32ArrayType::CreateArray(totalPoints, DREAM3D::FeatureData::Phases);
    cellPhaseData->initializeWithValue(999);
    for(int i = 0; i < totalPoints; i++)
    {
      gnum = m_FeatureIds[i];
      cellEulerData->setValue(3 * i, featureEulerData->getValue(3 * gnum));
      cellEulerData->setValue(3 * i + 1, featureEulerData->getValue(3 * gnum + 1));
      cellEulerData->setValue(3 * i + 2, featureEulerData->getValue(3 * gnum + 2));
      cellPhaseData->setValue(i, featurePhaseData->getValue(gnum));
    }
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(DREAM3D::CellData::EulerAngles, cellEulerData);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(DREAM3D::CellData::Phases, cellPhaseData);
  }

  if (m_RenumberFeatures == true)
  {
    int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
    size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
    if (0 == totalFeatures)
    {
      notifyErrorMessage(getHumanLabel(), "The number of features is Zero and should be greater than Zero", -600);
      return -999;
    }

    dataCheck();

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
    connect(renum.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
            this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
    renum->setMessagePrefix(getMessagePrefix());
    renum->execute();
    setErrorCondition(renum->getErrorCondition());

  }

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}


