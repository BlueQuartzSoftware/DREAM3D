/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "EstablishMatrixPhase.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"
#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"

// Include the MOC generated file for this class
#include "moc_EstablishMatrixPhase.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishMatrixPhase::EstablishMatrixPhase() :
  AbstractFilter(),
  m_OutputCellAttributeMatrixPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_OutputCellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_OutputCellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_NumFeaturesArrayName(DREAM3D::EnsembleData::NumFeatures),
  m_MaskArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Mask),
  m_UseMask(false), m_InputStatsArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::Statistics),
  m_InputPhaseTypesArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::PhaseTypes),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_Mask(NULL),
  m_FeaturePhases(NULL),
  m_NumFeatures(NULL),
  m_PhaseTypes(NULL)
{
  m_StatsDataArray = StatsDataArray::NullPointer();

  firstMatrixFeature = 1;
  sizex = sizey = sizez = totalvol = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishMatrixPhase::~EstablishMatrixPhase()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps("MaskArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Mask", "UseMask", getUseMask(), linkedProps, FilterParameter::Parameter));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Attribute Matrix", "OutputCellAttributeMatrixPath", getOutputCellAttributeMatrixPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Feature Ids", "FeatureIdsArrayName", getFeatureIdsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Phases", "CellPhasesArrayName", getCellPhasesArrayName(), FilterParameter::CreatedArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Mask", "MaskArrayPath", getMaskArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Feature Attribute Matrix", "OutputCellFeatureAttributeMatrixName", getOutputCellFeatureAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Phases", "FeaturePhasesArrayName", getFeaturePhasesArrayName(), FilterParameter::CreatedArray));

  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::StatsDataArray, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::Defaults::AnyGeometry);
    QVector<uint32_t> geomTypes;
    geomTypes.push_back(DREAM3D::GeometryType::ImageGeometry);
    geomTypes.push_back(DREAM3D::GeometryType::UnknownGeometry);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(DataArraySelectionFilterParameter::New("Statistics", "InputStatsArrayPath", getInputStatsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::UInt32, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::Defaults::AnyGeometry);
    QVector<uint32_t> geomTypes;
    geomTypes.push_back(DREAM3D::GeometryType::ImageGeometry);
    geomTypes.push_back(DREAM3D::GeometryType::UnknownGeometry);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(DataArraySelectionFilterParameter::New("Phase Types", "InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Ensemble Attribute Matrix", "OutputCellEnsembleAttributeMatrixName", getOutputCellEnsembleAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Number of Features", "NumFeaturesArrayName", getNumFeaturesArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputCellAttributeMatrixPath( reader->readDataArrayPath("OutputCellAttributeMatrixPath", getOutputCellAttributeMatrixPath() ) );
  setOutputCellFeatureAttributeMatrixName( reader->readString("OutputCellFeatureAttributeMatrixName", getOutputCellFeatureAttributeMatrixName() ) );
  setOutputCellEnsembleAttributeMatrixName( reader->readString("OutputCellEnsembleAttributeMatrixName", getOutputCellEnsembleAttributeMatrixName() ) );
  setFeatureIdsArrayName( reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setCellPhasesArrayName( reader->readString("CellPhasesArrayName", getCellPhasesArrayName() ) );
  setUseMask(reader->readValue("UseMask", getUseMask()));
  setMaskArrayPath(reader->readDataArrayPath("MaskArrayPath", getMaskArrayPath()));
  setFeaturePhasesArrayName(reader->readString("FeaturePhasesArrayName", getFeaturePhasesArrayName()));
  setNumFeaturesArrayName( reader->readString("NumFeaturesArrayName", getNumFeaturesArrayName() ) );
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath() ) );
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EstablishMatrixPhase::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(OutputCellAttributeMatrixPath)
  SIMPL_FILTER_WRITE_PARAMETER(OutputCellFeatureAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(OutputCellEnsembleAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(CellPhasesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(FeaturePhasesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(NumFeaturesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(InputStatsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(UseMask)
  SIMPL_FILTER_WRITE_PARAMETER(MaskArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(InputPhaseTypesArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getOutputCellAttributeMatrixPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_PhaseTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), cDims);
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_StatsDataArray = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), cDims);
  if(m_StatsDataArray.lock() == NULL)
  {
    QString ss = QObject::tr("Statistics array is not initialized correctly. The path is %1").arg(getInputStatsArrayPath().serialize());
    setErrorCondition(-308);
    notifyErrorMessage(getHumanLabel(), ss, -308);
  }

  cDims[0] = 1;
  // Cell Data
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellAttributeMatrixPath().getAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, cDims); /* Assigns the shared_ptr<>(this, tempPath, -1, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellAttributeMatrixPath().getAttributeMatrixName(), getCellPhasesArrayName() );
  m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (m_UseMask == true)
  {
    m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), cDims);
    if (NULL != m_MaskPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Mask = m_MaskPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0 || NULL == cellFeatureAttrMat.get()) { return; }
  tDims[0] = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0 || NULL == cellEnsembleAttrMat.get()) { return; }

  // Feature Data
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getFeaturePhasesArrayName() );
  m_FeaturePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellEnsembleAttributeMatrixName(), getNumFeaturesArrayName() );
  m_NumFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  establish_matrix();
  if(getErrorCondition() < 0) { return; }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  EstablishMatrixPhase::establish_matrix()
{
  notifyStatusMessage(getHumanLabel(), "Establishing Matrix");
  SIMPL_RANDOMNG_NEW()

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  size_t udims[3] =
  { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  sizex = dims[0] * m->getGeometryAs<ImageGeom>()->getXRes();
  sizey = dims[1] * m->getGeometryAs<ImageGeom>()->getYRes();
  sizez = dims[2] * m->getGeometryAs<ImageGeom>()->getZRes();
  totalvol = sizex * sizey * sizez;

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t currentnumfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  QVector<size_t> tDims(1, 1);
  if (currentnumfeatures == 0)
  {
    m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
    currentnumfeatures = 1;
  }
  firstMatrixFeature = currentnumfeatures;
  float random = 0.0f;
  float totalmatrixfractions = 0.0f;

  for (size_t i = 1; i < numensembles; ++i)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::MatrixPhase)
    {
      MatrixStatsData* mp = MatrixStatsData::SafePointerDownCast(statsDataArray[i].get());
      if (NULL == mp)
      {
        QString ss = QObject::tr("Tried to cast a statsDataArray[%1].get() to a MatrixStatsData* "
                                 "pointer but this resulted in a NULL pointer. The value at m_PhaseTypes[%2] = %3 does not match up "
                                 "with the type of pointer stored in the StatsDataArray (MatrixStatsData)\n")
                     .arg(i).arg(i).arg(m_PhaseTypes[i]);
        setErrorCondition(-666);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      matrixphases.push_back(i);
      matrixphasefractions.push_back(mp->getPhaseFraction());
      totalmatrixfractions = totalmatrixfractions + mp->getPhaseFraction();
    }
  }
  for (int32_t i = 0; i < matrixphases.size(); i++)
  {
    matrixphasefractions[i] = matrixphasefractions[i] / totalmatrixfractions;
    if (i > 0) { matrixphasefractions[i] = matrixphasefractions[i] + matrixphasefractions[i - 1]; }
  }
  size_t j = 0;
  for (size_t i = 0; i < totalPoints; ++i)
  {
    if ((m_UseMask == false && m_FeatureIds[i] <= 0) || (m_UseMask == true && m_Mask[i] == true && m_FeatureIds[i] <= 0))
    {
      random = static_cast<float>( rg.genrand_res53() );
      j = 0;
      while (random > matrixphasefractions[j])
      {
        j++;
      }
      if (m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples() <= (firstMatrixFeature + j))
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
    else if (m_UseMask == true && m_Mask[i] == false)
    {
      m_FeatureIds[i] = 0;
      m_CellPhases[i] = 0;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EstablishMatrixPhase::newFilterInstance(bool copyFilterParameters)
{
  EstablishMatrixPhase::Pointer filter = EstablishMatrixPhase::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishMatrixPhase::getCompiledLibraryName()
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishMatrixPhase::getBrandingString()
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishMatrixPhase::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishMatrixPhase::getGroupName()
{ return DREAM3D::FilterGroups::SyntheticBuildingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishMatrixPhase::getSubGroupName()
{ return DREAM3D::FilterSubGroups::PackingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishMatrixPhase::getHumanLabel()
{ return "Establish Matrix Phase"; }
