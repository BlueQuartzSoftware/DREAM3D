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

#include <memory>

#include "EstablishMatrixPhase.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,

  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishMatrixPhase::EstablishMatrixPhase()
: m_OutputCellAttributeMatrixPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "")
, m_OutputCellFeatureAttributeMatrixName(SIMPL::Defaults::CellFeatureAttributeMatrixName)
, m_OutputCellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, m_CellPhasesArrayName(SIMPL::CellData::Phases)
, m_FeaturePhasesArrayName(SIMPL::FeatureData::Phases)
, m_NumFeaturesArrayName(SIMPL::EnsembleData::NumFeatures)
, m_MaskArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
, m_UseMask(false)
, m_InputStatsArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::Statistics)
, m_InputPhaseTypesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseTypes)
, m_InputPhaseNamesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseName)
{
  m_StatsDataArray = StatsDataArray::NullPointer();

  firstMatrixFeature = 1;
  sizex = sizey = sizez = totalvol = 0.0f;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishMatrixPhase::~EstablishMatrixPhase() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  QStringList linkedProps("MaskArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Mask", UseMask, FilterParameter::Parameter, EstablishMatrixPhase, linkedProps));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", OutputCellAttributeMatrixPath, FilterParameter::RequiredArray, EstablishMatrixPhase, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Feature Ids", FeatureIdsArrayName, OutputCellAttributeMatrixPath, OutputCellAttributeMatrixPath, FilterParameter::CreatedArray, EstablishMatrixPhase));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Phases", CellPhasesArrayName, OutputCellAttributeMatrixPath, OutputCellAttributeMatrixPath, FilterParameter::CreatedArray, EstablishMatrixPhase));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", MaskArrayPath, FilterParameter::RequiredArray, EstablishMatrixPhase, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Feature Attribute Matrix", OutputCellFeatureAttributeMatrixName, OutputCellAttributeMatrixPath, FilterParameter::CreatedArray, EstablishMatrixPhase));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Phases", FeaturePhasesArrayName, OutputCellAttributeMatrixPath, OutputCellFeatureAttributeMatrixName, FilterParameter::CreatedArray, EstablishMatrixPhase));

  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::StatsDataArray, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Statistics", InputStatsArrayPath, FilterParameter::RequiredArray, EstablishMatrixPhase, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Types", InputPhaseTypesArrayPath, FilterParameter::RequiredArray, EstablishMatrixPhase, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::StringArray, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Names", InputPhaseNamesArrayPath, FilterParameter::RequiredArray, EstablishMatrixPhase, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Ensemble Attribute Matrix", OutputCellEnsembleAttributeMatrixName, OutputCellAttributeMatrixPath, FilterParameter::CreatedArray, EstablishMatrixPhase));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Number of Features", NumFeaturesArrayName, OutputCellAttributeMatrixPath, OutputCellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, EstablishMatrixPhase));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputCellAttributeMatrixPath(reader->readDataArrayPath("OutputCellAttributeMatrixPath", getOutputCellAttributeMatrixPath()));
  setOutputCellFeatureAttributeMatrixName(reader->readString("OutputCellFeatureAttributeMatrixName", getOutputCellFeatureAttributeMatrixName()));
  setOutputCellEnsembleAttributeMatrixName(reader->readString("OutputCellEnsembleAttributeMatrixName", getOutputCellEnsembleAttributeMatrixName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
  setCellPhasesArrayName(reader->readString("CellPhasesArrayName", getCellPhasesArrayName()));
  setUseMask(reader->readValue("UseMask", getUseMask()));
  setMaskArrayPath(reader->readDataArrayPath("MaskArrayPath", getMaskArrayPath()));
  setFeaturePhasesArrayName(reader->readString("FeaturePhasesArrayName", getFeaturePhasesArrayName()));
  setNumFeaturesArrayName(reader->readString("NumFeaturesArrayName", getNumFeaturesArrayName()));
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath()));
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::updateFeatureInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_FeaturePhasesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::initialize()
{
  firstMatrixFeature = 1;
  sizex = sizey = sizez = totalvol = 0.0f;

  m_StatsDataArray = StatsDataArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getOutputCellAttributeMatrixPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_PhaseTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), cDims);
  if(nullptr != m_PhaseTypesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_StatsDataArray = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), cDims);
  if(m_StatsDataArray.lock() == nullptr)
  {
    QString ss = QObject::tr("Statistics array is not initialized correctly. The path is %1").arg(getInputStatsArrayPath().serialize());
    setErrorCondition(-308, ss);
  }

  cDims[0] = 1;
  // Cell Data
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellAttributeMatrixPath().getAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, -1, cDims);             /* Assigns the shared_ptr<>(this, tempPath, -1, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellAttributeMatrixPath().getAttributeMatrixName(), getCellPhasesArrayName());
  m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);              /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_UseMask)
  {
    m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), cDims);
    if(nullptr != m_MaskPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_Mask = m_MaskPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  std::vector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix(this, getOutputCellFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == cellFeatureAttrMat.get())
  {
    return;
  }

  AttributeMatrix::Pointer outEnsembleAttrMat = AttributeMatrix::NullPointer();
  if(m->doesAttributeMatrixExist(getOutputCellEnsembleAttributeMatrixName()))
  {
    outEnsembleAttrMat = m->getPrereqAttributeMatrix(this, getOutputCellEnsembleAttributeMatrixName(), -350);
  }
  else
  {
    tDims[0] = m_PhaseTypesPtr.lock()->getNumberOfTuples();
    outEnsembleAttrMat = m->createNonPrereqAttributeMatrix(this, getOutputCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble, AttributeMatrixID22);
  }

  tempPath = getOutputCellAttributeMatrixPath();
  tempPath.setAttributeMatrixName(getOutputCellEnsembleAttributeMatrixName());
  tempPath.setDataArrayName(SIMPL::EnsembleData::PhaseName);
  m_PhaseNamesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<StringDataArray, AbstractFilter, QString>(this, tempPath, nullptr, cDims, "", DataArrayID31);

  // Feature Data
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getFeaturePhasesArrayName());
  m_FeaturePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);                 /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellEnsembleAttributeMatrixName(), getNumFeaturesArrayName());
  m_NumFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);               /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NumFeaturesPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::preflight()
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
void EstablishMatrixPhase::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  establish_matrix();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer ensembleAttrMat = getDataContainerArray()->getAttributeMatrix(getInputPhaseNamesArrayPath());
  IDataArray::Pointer inputPhaseNames = ensembleAttrMat->getAttributeArray(getInputPhaseNamesArrayPath().getDataArrayName());
  if(inputPhaseNames.get() != nullptr)
  {
    AttributeMatrix::Pointer cellEnsembleAttrMat = m->getAttributeMatrix(m_OutputCellEnsembleAttributeMatrixName);
    IDataArray::Pointer outputPhaseNames = inputPhaseNames->deepCopy();
    cellEnsembleAttrMat->insertOrAssign(outputPhaseNames);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::establish_matrix()
{
  notifyStatusMessage("Establishing Matrix");
  SIMPL_RANDOMNG_NEW()

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  FloatVec3Type spacing = m->getGeometryAs<ImageGeom>()->getSpacing();

  sizex = dims[0] * spacing[0];
  sizey = dims[1] * spacing[1];
  sizez = dims[2] * spacing[2];
  totalvol = sizex * sizey * sizez;

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t currentnumfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  std::vector<size_t> tDims(1, 1);
  if(currentnumfeatures == 0)
  {
    m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
    currentnumfeatures = 1;
  }
  firstMatrixFeature = currentnumfeatures;
  float random = 0.0f;
  float totalmatrixfractions = 0.0f;

  for(size_t i = 1; i < numensembles; ++i)
  {
    if(m_PhaseTypes[i] == static_cast<PhaseType::EnumType>(PhaseType::Type::Matrix))
    {
      MatrixStatsData::Pointer mp = std::dynamic_pointer_cast<MatrixStatsData>(statsDataArray[i]);
      if(nullptr == mp)
      {
        QString ss = QObject::tr("Tried to cast a statsDataArray[%1].get() to a MatrixStatsData* "
                                 "pointer but this resulted in a nullptr pointer. The value at m_PhaseTypes[%2] = %3 does not match up "
                                 "with the type of pointer stored in the StatsDataArray (MatrixStatsData)\n")
                         .arg(i)
                         .arg(i)
                         .arg(m_PhaseTypes[i]);
        setErrorCondition(-666, ss);
        return;
      }
      matrixphases.push_back(i);
      matrixphasefractions.push_back(mp->getPhaseFraction());
      totalmatrixfractions = totalmatrixfractions + mp->getPhaseFraction();
    }
  }
  for(size_t i = 0; i < matrixphases.size(); i++)
  {
    matrixphasefractions[i] = matrixphasefractions[i] / totalmatrixfractions;
    if(i > 0)
    {
      matrixphasefractions[i] = matrixphasefractions[i] + matrixphasefractions[i - 1];
    }
  }
  size_t j = 0;
  for(size_t i = 0; i < totalPoints; ++i)
  {
    if((!m_UseMask && m_FeatureIds[i] <= 0) || (m_UseMask && m_Mask[i] && m_FeatureIds[i] <= 0))
    {
      random = static_cast<float>(rg.genrand_res53());
      j = 0;
      while(random > matrixphasefractions[j])
      {
        j++;
      }
      if(m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples() <= (firstMatrixFeature + j))
      {
        tDims[0] = (firstMatrixFeature + j) + 1;
        m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
        updateFeatureInstancePointers();
        m_NumFeatures[j] = 1;
      }
      m_FeatureIds[i] = (firstMatrixFeature + j);
      m_CellPhases[i] = matrixphases[j];
      m_FeaturePhases[(firstMatrixFeature + j)] = matrixphases[j];
    }
    else if(m_UseMask && !m_Mask[i])
    {
      m_FeatureIds[i] = 0;
      m_CellPhases[i] = 0;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EstablishMatrixPhase::newFilterInstance(bool copyFilterParameters) const
{
  EstablishMatrixPhase::Pointer filter = EstablishMatrixPhase::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getCompiledLibraryName() const
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getBrandingString() const
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getGroupName() const
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid EstablishMatrixPhase::getUuid() const
{
  return QUuid("{28910d1c-4309-500a-9508-e3ef1612e1f8}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::PackingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getHumanLabel() const
{
  return "Establish Matrix Phase";
}

// -----------------------------------------------------------------------------
EstablishMatrixPhase::Pointer EstablishMatrixPhase::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<EstablishMatrixPhase> EstablishMatrixPhase::New()
{
  struct make_shared_enabler : public EstablishMatrixPhase
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getNameOfClass() const
{
  return QString("EstablishMatrixPhase");
}

// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::ClassName()
{
  return QString("EstablishMatrixPhase");
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setOutputCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_OutputCellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath EstablishMatrixPhase::getOutputCellAttributeMatrixPath() const
{
  return m_OutputCellAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setOutputCellFeatureAttributeMatrixName(const QString& value)
{
  m_OutputCellFeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getOutputCellFeatureAttributeMatrixName() const
{
  return m_OutputCellFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setOutputCellEnsembleAttributeMatrixName(const QString& value)
{
  m_OutputCellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getOutputCellEnsembleAttributeMatrixName() const
{
  return m_OutputCellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setCellPhasesArrayName(const QString& value)
{
  m_CellPhasesArrayName = value;
}

// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getCellPhasesArrayName() const
{
  return m_CellPhasesArrayName;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setFeaturePhasesArrayName(const QString& value)
{
  m_FeaturePhasesArrayName = value;
}

// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getFeaturePhasesArrayName() const
{
  return m_FeaturePhasesArrayName;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setNumFeaturesArrayName(const QString& value)
{
  m_NumFeaturesArrayName = value;
}

// -----------------------------------------------------------------------------
QString EstablishMatrixPhase::getNumFeaturesArrayName() const
{
  return m_NumFeaturesArrayName;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setMaskArrayPath(const DataArrayPath& value)
{
  m_MaskArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath EstablishMatrixPhase::getMaskArrayPath() const
{
  return m_MaskArrayPath;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setUseMask(bool value)
{
  m_UseMask = value;
}

// -----------------------------------------------------------------------------
bool EstablishMatrixPhase::getUseMask() const
{
  return m_UseMask;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setInputStatsArrayPath(const DataArrayPath& value)
{
  m_InputStatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath EstablishMatrixPhase::getInputStatsArrayPath() const
{
  return m_InputStatsArrayPath;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setInputPhaseTypesArrayPath(const DataArrayPath& value)
{
  m_InputPhaseTypesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath EstablishMatrixPhase::getInputPhaseTypesArrayPath() const
{
  return m_InputPhaseTypesArrayPath;
}

// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setInputPhaseNamesArrayPath(const DataArrayPath& value)
{
  m_InputPhaseNamesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath EstablishMatrixPhase::getInputPhaseNamesArrayPath() const
{
  return m_InputPhaseNamesArrayPath;
}
