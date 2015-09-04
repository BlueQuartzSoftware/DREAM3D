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


#include "FindSizes.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "Statistics/StatisticsConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::FindSizes() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_NumCellsArrayName(DREAM3D::FeatureData::NumCells),
  m_FeatureIds(NULL),
  m_Volumes(NULL),
  m_EquivalentDiameters(NULL),
  m_NumCells(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::~FindSizes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Equivalent Diameters", "EquivalentDiametersArrayName", getEquivalentDiametersArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Number of Cells", "NumCellsArrayName", getNumCellsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Volumes", "VolumesArrayName", getVolumesArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setNumCellsArrayName(reader->readString("NumCellsArrayName", getNumCellsArrayName() ) );
  setEquivalentDiametersArrayName(reader->readString("EquivalentDiametersArrayName", getEquivalentDiametersArrayName() ) );
  setVolumesArrayName(reader->readString("VolumesArrayName", getVolumesArrayName() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSizes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(NumCellsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(EquivalentDiametersArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(VolumesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getVolumesArrayName() );
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getEquivalentDiametersArrayName() );
  m_EquivalentDiametersPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getNumCellsArrayName() );
  m_NumCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumCells = m_NumCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::preflight()
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
void FindSizes::find_sizes()
{
  float radcubed = 0.0f;
  float diameter = 0.0f;

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t numfeatures = m_VolumesPtr.lock()->getNumberOfTuples();

  DataArray<float>::Pointer m_FeatureCounts = DataArray<float>::CreateArray(numfeatures, "_INTERNAL_USE_ONLY_FeatureCounts");
  m_FeatureCounts->initializeWithZeros();
  float* featurecounts = m_FeatureCounts->getPointer(0);

  for (size_t j = 0; j < totalPoints; j++)
  {
    int32_t gnum = m_FeatureIds[j];
    featurecounts[gnum]++;
  }
  float res_scalar = m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getYRes() * m->getGeometryAs<ImageGeom>()->getZRes();
  float vol_term = (4.0f / 3.0f) * SIMPLib::Constants::k_Pi;
  for (size_t i = 1; i < numfeatures; i++)
  {
    m_NumCells[i] = static_cast<int32_t>( featurecounts[i] );
    m_Volumes[i] = (featurecounts[i] * res_scalar);
    radcubed = m_Volumes[i] / vol_term;
    diameter = 2.0f * powf(radcubed, 0.3333333333f);
    m_EquivalentDiameters[i] = diameter;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::find_sizes2D()
{
  float radsquared = 0.0f;
  float diameter = 0.0f;

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t numfeatures = m_VolumesPtr.lock()->getNumberOfTuples();

  DataArray<float>::Pointer m_FeatureCounts = DataArray<float>::CreateArray(numfeatures, "_INTERNAL_USE_ONLY_FeatureCounts");
  m_FeatureCounts->initializeWithZeros();
  float* featurecounts = m_FeatureCounts->getPointer(0);

  for (size_t j = 0; j < totalPoints; j++)
  {
    int32_t gnum = m_FeatureIds[j];
    featurecounts[gnum]++;
  }
  float res_scalar = 0.0f;
  if (m->getGeometryAs<ImageGeom>()->getXPoints() == 1) { res_scalar = m->getGeometryAs<ImageGeom>()->getYRes() * m->getGeometryAs<ImageGeom>()->getZRes(); }
  else if (m->getGeometryAs<ImageGeom>()->getYPoints() == 1) { res_scalar = m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getZRes(); }
  else if (m->getGeometryAs<ImageGeom>()->getZPoints() == 1) { res_scalar = m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getYRes(); }
  for (size_t i = 1; i < numfeatures; i++)
  {
    m_NumCells[i] = static_cast<int32_t>( featurecounts[i] );
    m_Volumes[i] = (featurecounts[i] * res_scalar);
    radsquared = m_Volumes[i] / SIMPLib::Constants::k_Pi;
    diameter = (2 * sqrtf(radsquared));
    m_EquivalentDiameters[i] = diameter;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  if(m->getGeometryAs<ImageGeom>()->getXPoints() > 1 && m->getGeometryAs<ImageGeom>()->getYPoints() > 1 && m->getGeometryAs<ImageGeom>()->getZPoints() > 1) { find_sizes(); }
  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1 || m->getGeometryAs<ImageGeom>()->getYPoints() == 1 || m->getGeometryAs<ImageGeom>()->getZPoints() == 1) { find_sizes2D(); }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSizes::newFilterInstance(bool copyFilterParameters)
{
  FindSizes::Pointer filter = FindSizes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getCompiledLibraryName()
{ return StatisticsConstants::StatisticsBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MorphologicalFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getHumanLabel()
{ return "Find Feature Sizes"; }
