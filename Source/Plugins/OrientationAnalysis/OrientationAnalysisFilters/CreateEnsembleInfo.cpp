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

#include "CreateEnsembleInfo.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Filtering/ThresholdFilterHelper.h"

#include "OrientationAnalysis/FilterParameters/EnsembleInfoFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/EbsdConstants.h"

// Include the MOC generated file for this class
#include "moc_CreateEnsembleInfo.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateEnsembleInfo::CreateEnsembleInfo()
: AbstractFilter()
, m_DataContainerName("")
, m_CellEnsembleAttributeMatrixName("EnsembleAttributeMatrix")
, m_Ensemble()
, m_CrystalStructuresArrayName(SIMPL::EnsembleData::CrystalStructures)
, m_PhaseTypesArrayName(SIMPL::EnsembleData::PhaseTypes)
, m_PhaseNamesArrayName("PhaseNames")
, m_CrystalStructures(nullptr)
, m_PhaseTypes(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateEnsembleInfo::~CreateEnsembleInfo()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateEnsembleInfo::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container", DataContainerName, FilterParameter::RequiredArray, CreateEnsembleInfo, req));
  }
  {
    EnsembleInfoFilterParameter::Pointer parameter = EnsembleInfoFilterParameter::New();
    parameter->setHumanLabel("Created Ensemble Info");
    parameter->setPropertyName("Ensemble");

    parameter->setShowOperators(true);
    parameter->setCategory(FilterParameter::Parameter);
    parameter->setSetterCallback(SIMPL_BIND_SETTER(CreateEnsembleInfo, this, Ensemble));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(CreateEnsembleInfo, this, Ensemble));
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, CreateEnsembleInfo));
  parameters.push_back(SIMPL_NEW_STRING_FP("Crystal Structures", CrystalStructuresArrayName, FilterParameter::CreatedArray, CreateEnsembleInfo));
  parameters.push_back(SIMPL_NEW_STRING_FP("Phase Types", PhaseTypesArrayName, FilterParameter::CreatedArray, CreateEnsembleInfo));
  parameters.push_back(SIMPL_NEW_STRING_FP("Phase Names", PhaseNamesArrayName, FilterParameter::CreatedArray, CreateEnsembleInfo));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateEnsembleInfo::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setPhaseTypesArrayName(reader->readString("PhaseTypesArrayName", getPhaseTypesArrayName()));
  setPhaseNamesArrayName(reader->readString("PhaseNamesArrayName", getPhaseTypesArrayName()));
  setCrystalStructuresArrayName(reader->readString("CrystalStructuresArrayName", getCrystalStructuresArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateEnsembleInfo::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateEnsembleInfo::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(m_Ensemble.size() == 0)
  {
    setErrorCondition(-15000);
    notifyErrorMessage(getHumanLabel(), "You must add at least 1 ensemble phase.", getErrorCondition());
  }

  int numPhases = m_Ensemble.size();

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> tDims(1, numPhases + 1);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble);
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> cDims(1, 1);
  DataArrayPath tempPath;
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName());
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(
      this, tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get())                           /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getPhaseTypesArrayName());
  m_PhaseTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(
      this, tempPath, static_cast<PhaseType::EnumType>(PhaseType::Type::Unknown), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_PhaseTypesPtr.lock().get())                                             /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getPhaseNamesArrayName());
  m_PhaseNamesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<StringDataArray, AbstractFilter, QString>(this, tempPath, "_PHASE_NAME_",
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateEnsembleInfo::preflight()
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
void CreateEnsembleInfo::execute()
{
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer cellensembleAttrMat = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName());

  int32_t numPhases = m_Ensemble.size();

  if(0 == numPhases) // Either the group name "EnsembleInfo" is incorrect or 0 was entered as the Number_Phases
  {
    QString ss = QObject::tr("Check the group name EnsembleInfo and that the number of phases > 0");
    setErrorCondition(-10003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataArray<EnsembleInfo::CrystalStructureType>::Pointer crystalStructures = m_Ensemble.getCrystalStructureArray();
  DataArray<PhaseType::EnumType>::Pointer phaseTypes = m_Ensemble.getPhaseTypeArray();
  StringDataArray::Pointer phaseNames = m_Ensemble.getPhaseNameArray();

  for(int i = 0; i < numPhases; i++)
  {
    // Crystal Structure
    uint32_t crystalStructure = crystalStructures->getValue(i);
    if(crystalStructure == Ebsd::CrystalStructure::UnknownCrystalStructure)
    {
      QString ss = QObject::tr("Incorrect crystal structure name '%1'").arg(crystalStructure);
      setErrorCondition(-10006);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    m_CrystalStructures[i + 1] = crystalStructure;

    // Phase Type
    uint32_t phaseType = phaseTypes->getValue(i);
    if(static_cast<PhaseType::Type>(phaseType) == PhaseType::Type::Unknown)
    {
      QString ss = QObject::tr("Incorrect phase type '%1'").arg(phaseType); // The phase type name was not found in the lookup table
      setErrorCondition(-10007);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    m_PhaseTypes[i + 1] = phaseType;

    // Phase Name
    QString phaseName = phaseNames->getValue(i);
    if(phaseName.isEmpty())
    {
      QString ss = QObject::tr("Phase name cannot be empty'"); // The phase name was not found
      setErrorCondition(-10008);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    m_PhaseNamesPtr.lock()->setValue(i + 1, phaseName);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateEnsembleInfo::newFilterInstance(bool copyFilterParameters)
{
  CreateEnsembleInfo::Pointer filter = CreateEnsembleInfo::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateEnsembleInfo::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateEnsembleInfo::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateEnsembleInfo::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateEnsembleInfo::getGroupName()
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateEnsembleInfo::getSubGroupName()
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateEnsembleInfo::getHumanLabel()
{
  return "Create Ensemble Info";
}
