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

#include "FeatureInfoReader.h"

#include <fstream>

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

// Include the MOC generated file for this class
#include "moc_FeatureInfoReader.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureInfoReader::FeatureInfoReader()
: FileReader()
, m_CellAttributeMatrixName("", "", "")
, m_CellFeatureAttributeMatrixName("FeatureAttributeMatrix")
, m_InputFile("")
, m_CreateCellLevelArrays(true)
, m_RenumberFeatures(true)
, m_FeatureIdsArrayPath("", "", "")
, m_CellPhasesArrayName(SIMPL::CellData::Phases)
, m_CellEulerAnglesArrayName(SIMPL::CellData::EulerAngles)
, m_FeaturePhasesArrayName(SIMPL::FeatureData::Phases)
, m_FeatureEulerAnglesArrayName(SIMPL::FeatureData::EulerAngles)
, m_FeatureIds(nullptr)
, m_CellPhases(nullptr)
, m_CellEulerAngles(nullptr)
, m_FeaturePhases(nullptr)
, m_FeatureEulerAngles(nullptr)
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
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input Feature Info File", InputFile, FilterParameter::Parameter, FeatureInfoReader, "*.txt"));
  QStringList linkedProps;
  linkedProps << "CellPhasesArrayName"
              << "CellEulerAnglesArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Element Level Arrays", CreateCellLevelArrays, FilterParameter::Parameter, FeatureInfoReader, linkedProps));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Renumber Features", RenumberFeatures, FilterParameter::Parameter, FeatureInfoReader));
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    AttributeMatrix::Types amTypes;
    amTypes.push_back(AttributeMatrix::Type::Cell);
    amTypes.push_back(AttributeMatrix::Type::Edge);
    amTypes.push_back(AttributeMatrix::Type::Face);
    amTypes.push_back(AttributeMatrix::Type::Vertex);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Element Attribute Matrix", CellAttributeMatrixName, FilterParameter::RequiredArray, FeatureInfoReader, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FeatureInfoReader, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Phases", CellPhasesArrayName, FilterParameter::CreatedArray, FeatureInfoReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Euler Angles", CellEulerAnglesArrayName, FilterParameter::CreatedArray, FeatureInfoReader));
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Feature Attribute Matrix", CellFeatureAttributeMatrixName, FilterParameter::CreatedArray, FeatureInfoReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Phases", FeaturePhasesArrayName, FilterParameter::CreatedArray, FeatureInfoReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Euler Angles", FeatureEulerAnglesArrayName, FilterParameter::CreatedArray, FeatureInfoReader));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixName(reader->readDataArrayPath("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setFeatureEulerAnglesArrayName(reader->readString("FeatureEulerAnglesArrayName", getFeatureEulerAnglesArrayName()));
  setFeaturePhasesArrayName(reader->readString("FeaturePhasesArrayName", getFeaturePhasesArrayName()));
  setCellEulerAnglesArrayName(reader->readString("CellEulerAnglesArrayName", getCellEulerAnglesArrayName()));
  setCellPhasesArrayName(reader->readString("CellPhasesArrayName", getCellPhasesArrayName()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  setCreateCellLevelArrays(reader->readValue("CreateCellLevelArrays", getCreateCellLevelArrays()));
  setRenumberFeatures(reader->readValue("RenumberFeatures", getRenumberFeatures()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::updateFeatureInstancePointers()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(nullptr != m_FeaturePhasesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  }                                                   /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_FeatureEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> tDims(1, 0);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature);

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The input file must be set").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_CellFeatureAttributeMatrixName.isEmpty() == true)
  {
    QString ss = QObject::tr("Feature Attribute Matrix name must be set");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_CreateCellLevelArrays)
  {
    tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getCellPhasesArrayName());
    m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
        this, tempPath, 0, cDims);              /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CellPhasesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    cDims[0] = 3;
    tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getCellEulerAnglesArrayName());
    m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0, cDims);                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CellEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  cDims[0] = 1;
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), getCellFeatureAttributeMatrixName(), getFeaturePhasesArrayName());
  m_FeaturePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);                 /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), getCellFeatureAttributeMatrixName(), getFeatureEulerAnglesArrayName());
  m_FeatureEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                      /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t FeatureInfoReader::readHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t FeatureInfoReader::readFile()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return getErrorCondition();
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixName());

  std::ifstream inFile;
  inFile.open(getInputFile().toLatin1().data(), std::ios_base::binary);
  if(!inFile)
  {
    QString ss = QObject::tr("Error opening input file: %1").arg(getInputFile());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }

  int32_t numfeatures = 0;
  int32_t gnum = 0, phase = 0;
  int32_t maxphase = 0;
  float ea1 = 0.0f, ea2 = 0.0f, ea3 = 0.0f;
  inFile >> numfeatures;
  if(0 == numfeatures)
  {
    QString ss = QObject::tr("The number of Features (%1) specified in the file must be greater than zero").arg(numfeatures);
    setErrorCondition(-600);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  int32_t maxFeatureId = 0;
  for(size_t i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > maxFeatureId)
    {
      maxFeatureId = m_FeatureIds[i];
    }
  }

  if(numfeatures != maxFeatureId)
  {
    QString ss =
        QObject::tr("The number of Features (%1) specified in the file does not correspond to the maximum Feature Id (%2) in the selected Feature Ids array").arg(numfeatures).arg(maxFeatureId);
    setErrorCondition(-600);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }

  QVector<size_t> tDims(1, numfeatures + 1);
  cellFeatureAttrMat->setTupleDimensions(tDims);
  updateFeatureInstancePointers();

  for(int32_t i = 0; i < numfeatures; i++)
  {
    inFile >> gnum >> phase >> ea1 >> ea2 >> ea3;
    if(gnum > maxFeatureId)
    {
      QString ss = QObject::tr("A Feature Id (%1) specified in the file is larger than the maximum Feature Id (%2) in the selected Feature Ids array").arg(numfeatures).arg(maxFeatureId);
      setErrorCondition(-600);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return getErrorCondition();
    }
    m_FeatureEulerAngles[3 * gnum] = ea1;
    m_FeatureEulerAngles[3 * gnum + 1] = ea2;
    m_FeatureEulerAngles[3 * gnum + 2] = ea3;
    m_FeaturePhases[gnum] = phase;
    if(phase > maxphase)
    {
      maxphase = phase;
    }
  }

  if(m_CreateCellLevelArrays == true)
  {
    for(size_t i = 0; i < totalPoints; i++)
    {
      gnum = m_FeatureIds[i];
      m_CellEulerAngles[3 * i] = m_FeatureEulerAngles[3 * gnum];
      m_CellEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * gnum + 1];
      m_CellEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * gnum + 2];
      m_CellPhases[i] = m_FeaturePhases[gnum];
    }
  }

  if(m_RenumberFeatures == true)
  {
    size_t totalFeatures = cellFeatureAttrMat->getNumberOfTuples();

    // Find the unique set of feature ids
    QVector<bool> activeObjects(totalFeatures, false);
    for(size_t i = 0; i < totalPoints; ++i)
    {
      activeObjects[m_FeatureIds[i]] = true;
    }
    cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FeatureInfoReader::newFilterInstance(bool copyFilterParameters)
{
  FeatureInfoReader::Pointer filter = FeatureInfoReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureInfoReader::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureInfoReader::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureInfoReader::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureInfoReader::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureInfoReader::getSubGroupName()
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureInfoReader::getHumanLabel()
{
  return "Import Feature Info File";
}
