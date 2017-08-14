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

#include "FindBoundaryElementFractions.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

// Include the MOC generated file for this class
#include "moc_FindBoundaryElementFractions.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryElementFractions::FindBoundaryElementFractions()
: AbstractFilter()
, m_FeatureIdsArrayPath("", "", "")
, m_BoundaryCellsArrayPath("", "", "")
, m_BoundaryCellFractionsArrayPath("", "", "")
, m_FeatureIds(nullptr)
, m_BoundaryCells(nullptr)
, m_BoundaryCellFractions(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryElementFractions::~FindBoundaryElementFractions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindBoundaryElementFractions, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int8, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Surface Elements", BoundaryCellsArrayPath, FilterParameter::RequiredArray, FindBoundaryElementFractions, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Surface Element Fractions", BoundaryCellFractionsArrayPath, FilterParameter::CreatedArray, FindBoundaryElementFractions, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setBoundaryCellFractionsArrayPath(reader->readDataArrayPath("BoundaryCellFractionsArrayPath", getBoundaryCellFractionsArrayPath()));
  setBoundaryCellsArrayPath(reader->readDataArrayPath("BoundaryCellsArrayPath", getBoundaryCellsArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getFeatureIdsArrayPath());
  }

  m_BoundaryCellsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getBoundaryCellsArrayPath(),
                                                                                                          cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_BoundaryCellsPtr.lock().get())                                                                  /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_BoundaryCells = m_BoundaryCellsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getBoundaryCellsArrayPath());
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);

  m_BoundaryCellFractionsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, getBoundaryCellFractionsArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_BoundaryCellFractionsPtr.lock().get())    /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_BoundaryCellFractions = m_BoundaryCellFractionsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::preflight()
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
void FindBoundaryElementFractions::find_surface_voxel_fractions()
{
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t numfeatures = m_BoundaryCellFractionsPtr.lock()->getNumberOfTuples();

  FloatArrayType::Pointer m_SurfVoxCounts = FloatArrayType::CreateArray(numfeatures, "_INTERNAL_USE_ONLY_SurfVoxCounts");
  m_SurfVoxCounts->initializeWithZeros();
  float* surfvoxcounts = m_SurfVoxCounts->getPointer(0);
  FloatArrayType::Pointer m_VoxCounts = FloatArrayType::CreateArray(numfeatures, "_INTERNAL_USE_ONLY_VoxCounts");
  m_VoxCounts->initializeWithZeros();
  float* voxcounts = m_VoxCounts->getPointer(0);

  for(size_t j = 0; j < totalPoints; j++)
  {
    int32_t gnum = m_FeatureIds[j];
    voxcounts[gnum]++;
    if(m_BoundaryCells[j] > 0)
    {
      surfvoxcounts[gnum]++;
    }
  }
  for(size_t i = 1; i < numfeatures; i++)
  {
    m_BoundaryCellFractions[i] = surfvoxcounts[i] / voxcounts[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  find_surface_voxel_fractions();

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindBoundaryElementFractions::newFilterInstance(bool copyFilterParameters)
{
  FindBoundaryElementFractions::Pointer filter = FindBoundaryElementFractions::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryElementFractions::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryElementFractions::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryElementFractions::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryElementFractions::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryElementFractions::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryElementFractions::getHumanLabel()
{
  return "Find Feature Boundary Element Fractions";
}
