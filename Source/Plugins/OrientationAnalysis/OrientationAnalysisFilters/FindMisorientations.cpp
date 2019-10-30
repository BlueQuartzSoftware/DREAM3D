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

#include "FindMisorientations.h"

#include <cmath>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMisorientations::FindMisorientations()
: m_NeighborListArrayPath("", "", "")
, m_MisorientationListArrayName(SIMPL::FeatureData::MisorientationList)
, m_AvgQuatsArrayPath("", "", "")
, m_FeaturePhasesArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
, m_AvgMisorientationsArrayName(SIMPL::FeatureData::AvgMisorientations)
, m_FindAvgMisors(false)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();

  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_MisorientationList = NeighborList<float>::NullPointer();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMisorientations::~FindMisorientations() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMisorientations::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  QStringList linkedProps("AvgMisorientationsArrayName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Find Average Misorientation Per Feature", FindAvgMisors, FilterParameter::Parameter, FindMisorientations, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::NeighborList, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Neighbor List", NeighborListArrayPath, FilterParameter::RequiredArray, FindMisorientations, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Quaternions", AvgQuatsArrayPath, FilterParameter::RequiredArray, FindMisorientations, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, FindMisorientations, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindMisorientations, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Misorientation List", MisorientationListArrayName, NeighborListArrayPath, NeighborListArrayPath, FilterParameter::CreatedArray, FindMisorientations));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Average Misorientations", AvgMisorientationsArrayName, FeaturePhasesArrayPath, FeaturePhasesArrayPath, FilterParameter::CreatedArray, FindMisorientations));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMisorientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAvgMisorientationsArrayName(reader->readString("AvgMisorientationsArrayName", getAvgMisorientationsArrayName()));
  setMisorientationListArrayName(reader->readString("MisorientationListArrayName", getMisorientationListArrayName()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath()));
  setFindAvgMisors(reader->readValue("FindAvgMisors", getFindAvgMisors()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMisorientations::initialize()
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_MisorientationList = NeighborList<float>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMisorientations::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  DataArrayPath tempPath;

  std::vector<size_t> cDims(1, 4);

  QVector<DataArrayPath> dataArrayPaths;

  // Feature Data
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(),
                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock())                                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getAvgQuatsArrayPath());
  }

  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getFeaturePhasesArrayPath());
  }

  // New Feature Data
  if(m_FindAvgMisors)
  {
    tempPath.update(m_FeaturePhasesArrayPath.getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getAvgMisorientationsArrayName());
    m_AvgMisorientationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0, cDims);                      /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_AvgMisorientationsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_AvgMisorientations = m_AvgMisorientationsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Ensemble Data
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter>(this, getNeighborListArrayPath(), cDims);
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getNeighborListArrayPath());
  }

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), getNeighborListArrayPath().getAttributeMatrixName(), getMisorientationListArrayName());
  m_MisorientationList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims, "", DataArrayID31);

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMisorientations::preflight()
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
void FindMisorientations::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int32_t>& neighborlist = *(m_NeighborList.lock());

  std::vector<std::vector<float>> misorientationlists;

  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  float w = 0.0f;
  size_t tempMisoList = 0;

  uint32_t xtalType1 = 0, xtalType2 = 0;
  int32_t nname = 0;

  misorientationlists.resize(totalFeatures);
  for(size_t i = 1; i < totalFeatures; i++)
  {
    QuatF q1(m_AvgQuats + i * 4);
    xtalType1 = m_CrystalStructures[m_FeaturePhases[i]];
    NeighborList<int32_t>::VectorType& featureNeighborList = neighborlist[i];

    misorientationlists[i].assign(featureNeighborList.size(), -1.0);

    for(size_t j = 0; j < featureNeighborList.size(); j++)
    {
      w = std::numeric_limits<float>::max();
      nname = featureNeighborList[j];
      QuatF q2(m_AvgQuats + nname * 4);
      xtalType2 = m_CrystalStructures[m_FeaturePhases[nname]];
      tempMisoList = featureNeighborList.size();
      if(xtalType1 == xtalType2 && static_cast<int64_t>(xtalType1) < static_cast<int64_t>(m_OrientationOps.size()))
      {
        w = m_OrientationOps[xtalType1]->getMisoQuat(q1, q2, n1, n2, n3);
        misorientationlists[i][j] = w * SIMPLib::Constants::k_180OverPi;
        if(m_FindAvgMisors)
        {
          m_AvgMisorientations[i] += misorientationlists[i][j];
        }
      }
      else
      {
        if(m_FindAvgMisors)
        {
          tempMisoList--;
        }
        misorientationlists[i][j] = NAN;
      }
    }
    if(m_FindAvgMisors)
    {
      if(tempMisoList != 0)
      {
        m_AvgMisorientations[i] /= tempMisoList;
      }
      else
      {
        m_AvgMisorientations[i] = NAN;
      }
      tempMisoList = 0;
    }
  }

  for(size_t i = 1; i < totalFeatures; i++)
  {
    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType misoL(new std::vector<float>);
    misoL->assign(misorientationlists[i].begin(), misorientationlists[i].end());
    m_MisorientationList.lock()->setList(static_cast<int32_t>(i), misoL);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindMisorientations::newFilterInstance(bool copyFilterParameters) const
{
  FindMisorientations::Pointer filter = FindMisorientations::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMisorientations::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMisorientations::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMisorientations::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMisorientations::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindMisorientations::getUuid() const
{
  return QUuid("{286dd493-4fea-54f4-b59e-459dd13bbe57}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMisorientations::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMisorientations::getHumanLabel() const
{
  return "Find Feature Neighbor Misorientations";
}

// -----------------------------------------------------------------------------
FindMisorientations::Pointer FindMisorientations::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindMisorientations> FindMisorientations::New()
{
  struct make_shared_enabler : public FindMisorientations
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindMisorientations::getNameOfClass() const
{
  return QString("FindMisorientations");
}

// -----------------------------------------------------------------------------
QString FindMisorientations::ClassName()
{
  return QString("FindMisorientations");
}

// -----------------------------------------------------------------------------
void FindMisorientations::setNeighborListArrayPath(const DataArrayPath& value)
{
  m_NeighborListArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindMisorientations::getNeighborListArrayPath() const
{
  return m_NeighborListArrayPath;
}

// -----------------------------------------------------------------------------
void FindMisorientations::setMisorientationListArrayName(const QString& value)
{
  m_MisorientationListArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindMisorientations::getMisorientationListArrayName() const
{
  return m_MisorientationListArrayName;
}

// -----------------------------------------------------------------------------
void FindMisorientations::setAvgQuatsArrayPath(const DataArrayPath& value)
{
  m_AvgQuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindMisorientations::getAvgQuatsArrayPath() const
{
  return m_AvgQuatsArrayPath;
}

// -----------------------------------------------------------------------------
void FindMisorientations::setFeaturePhasesArrayPath(const DataArrayPath& value)
{
  m_FeaturePhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindMisorientations::getFeaturePhasesArrayPath() const
{
  return m_FeaturePhasesArrayPath;
}

// -----------------------------------------------------------------------------
void FindMisorientations::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindMisorientations::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void FindMisorientations::setAvgMisorientationsArrayName(const QString& value)
{
  m_AvgMisorientationsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindMisorientations::getAvgMisorientationsArrayName() const
{
  return m_AvgMisorientationsArrayName;
}

// -----------------------------------------------------------------------------
void FindMisorientations::setFindAvgMisors(bool value)
{
  m_FindAvgMisors = value;
}

// -----------------------------------------------------------------------------
bool FindMisorientations::getFindAvgMisors() const
{
  return m_FindAvgMisors;
}
