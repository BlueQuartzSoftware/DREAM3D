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

#include "FindBoundaryElementFractions.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryElementFractions::FindBoundaryElementFractions()
: m_FeatureIdsArrayPath("", "", "")
, m_BoundaryCellsArrayPath("", "", "")
, m_BoundaryCellFractionsArrayPath("", "", "")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryElementFractions::~FindBoundaryElementFractions() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::setupFilterParameters()
{
  FilterParameterVectorType parameters;
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
  clearErrorCode();
  clearWarningCode();

  QVector<DataArrayPath> dataArrayPaths;

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getFeatureIdsArrayPath());
  }

  m_BoundaryCellsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getBoundaryCellsArrayPath(),
                                                                                                          cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_BoundaryCellsPtr.lock())                                                                        /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_BoundaryCells = m_BoundaryCellsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getBoundaryCellsArrayPath());
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);

  m_BoundaryCellFractionsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getBoundaryCellFractionsArrayPath(), 0, cDims, "", DataArrayID31);
  if(nullptr != m_BoundaryCellFractionsPtr.lock())          /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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

  FloatArrayType::Pointer m_SurfVoxCounts = FloatArrayType::CreateArray(numfeatures, "_INTERNAL_USE_ONLY_SurfVoxCounts", true);
  m_SurfVoxCounts->initializeWithZeros();
  float* surfvoxcounts = m_SurfVoxCounts->getPointer(0);
  FloatArrayType::Pointer m_VoxCounts = FloatArrayType::CreateArray(numfeatures, "_INTERNAL_USE_ONLY_VoxCounts", true);
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  find_surface_voxel_fractions();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindBoundaryElementFractions::newFilterInstance(bool copyFilterParameters) const
{
  FindBoundaryElementFractions::Pointer filter = FindBoundaryElementFractions::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryElementFractions::getCompiledLibraryName() const
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryElementFractions::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryElementFractions::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryElementFractions::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindBoundaryElementFractions::getUuid() const
{
  return QUuid("{6357243e-41a6-52c4-be2d-2f6894c39fcc}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryElementFractions::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryElementFractions::getHumanLabel() const
{
  return "Find Feature Boundary Element Fractions";
}

// -----------------------------------------------------------------------------
FindBoundaryElementFractions::Pointer FindBoundaryElementFractions::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindBoundaryElementFractions> FindBoundaryElementFractions::New()
{
  struct make_shared_enabler : public FindBoundaryElementFractions
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindBoundaryElementFractions::getNameOfClass() const
{
  return QString("FindBoundaryElementFractions");
}

// -----------------------------------------------------------------------------
QString FindBoundaryElementFractions::ClassName()
{
  return QString("FindBoundaryElementFractions");
}

// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindBoundaryElementFractions::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::setBoundaryCellsArrayPath(const DataArrayPath& value)
{
  m_BoundaryCellsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindBoundaryElementFractions::getBoundaryCellsArrayPath() const
{
  return m_BoundaryCellsArrayPath;
}

// -----------------------------------------------------------------------------
void FindBoundaryElementFractions::setBoundaryCellFractionsArrayPath(const DataArrayPath& value)
{
  m_BoundaryCellFractionsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindBoundaryElementFractions::getBoundaryCellFractionsArrayPath() const
{
  return m_BoundaryCellFractionsArrayPath;
}
