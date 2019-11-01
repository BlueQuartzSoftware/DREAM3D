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

#include "FindSlipTransmissionMetrics.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"
#include "OrientationLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
  DataArrayID33 = 33,
  DataArrayID34 = 34,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlipTransmissionMetrics::FindSlipTransmissionMetrics()
: m_NeighborListArrayPath("", "", "")
, m_F1ListArrayName(SIMPL::FeatureData::F1List)
, m_F1sptListArrayName(SIMPL::FeatureData::F1sptList)
, m_F7ListArrayName(SIMPL::FeatureData::F7List)
, m_mPrimeListArrayName(SIMPL::FeatureData::mPrimeList)
, m_AvgQuatsArrayPath("", "", "")
, m_FeaturePhasesArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
{

  m_F1List = NeighborList<float>::NullPointer();
  m_F1sptList = NeighborList<float>::NullPointer();
  m_F7List = NeighborList<float>::NullPointer();
  m_mPrimeList = NeighborList<float>::NullPointer();
  m_NeighborList = NeighborList<int32_t>::NullPointer();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlipTransmissionMetrics::~FindSlipTransmissionMetrics() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::NeighborList, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Neighbor List", NeighborListArrayPath, FilterParameter::RequiredArray, FindSlipTransmissionMetrics, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Quaternions", AvgQuatsArrayPath, FilterParameter::RequiredArray, FindSlipTransmissionMetrics, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, FindSlipTransmissionMetrics, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindSlipTransmissionMetrics, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("F1 List", F1ListArrayName, NeighborListArrayPath, NeighborListArrayPath, FilterParameter::CreatedArray, FindSlipTransmissionMetrics));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("F1spt List", F1sptListArrayName, NeighborListArrayPath, NeighborListArrayPath, FilterParameter::CreatedArray, FindSlipTransmissionMetrics));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("F7 List", F7ListArrayName, NeighborListArrayPath, NeighborListArrayPath, FilterParameter::CreatedArray, FindSlipTransmissionMetrics));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("mPrime List", mPrimeListArrayName, NeighborListArrayPath, NeighborListArrayPath, FilterParameter::CreatedArray, FindSlipTransmissionMetrics));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setF1ListArrayName(reader->readString("F1ListArrayName", getF1ListArrayName()));
  setF1sptListArrayName(reader->readString("F1sptListArrayName", getF1sptListArrayName()));
  setF7ListArrayName(reader->readString("F7ListArrayName", getF7ListArrayName()));
  setmPrimeListArrayName(reader->readString("mPrimeListArrayName", getmPrimeListArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::initialize()
{
  m_F1List = NeighborList<float>::NullPointer();
  m_F1sptList = NeighborList<float>::NullPointer();
  m_F7List = NeighborList<float>::NullPointer();
  m_mPrimeList = NeighborList<float>::NullPointer();
  m_NeighborList = NeighborList<int32_t>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArrayPaths;

  std::vector<size_t> cDims(1, 4);
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

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter>(this, getNeighborListArrayPath(), cDims);
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getNeighborListArrayPath());
  }

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), m_NeighborListArrayPath.getAttributeMatrixName(), getF1ListArrayName());
  m_F1List = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims, "", DataArrayID31);

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), m_NeighborListArrayPath.getAttributeMatrixName(), getF1sptListArrayName());
  m_F1sptList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims, "", DataArrayID32);

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), m_NeighborListArrayPath.getAttributeMatrixName(), getF7ListArrayName());
  m_F7List = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims, "", DataArrayID33);

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), m_NeighborListArrayPath.getAttributeMatrixName(), getmPrimeListArrayName());
  m_mPrimeList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims, "", DataArrayID34);

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::preflight()
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
void FindSlipTransmissionMetrics::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
  QVector<LaueOps::Pointer> m_OrientationOps = LaueOps::getOrientationOpsQVector();

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int32_t>& neighborlist = *(m_NeighborList.lock());

  std::vector<std::vector<float>> F1lists;
  std::vector<std::vector<float>> F1sptlists;
  std::vector<std::vector<float>> F7lists;
  std::vector<std::vector<float>> mPrimelists;

  double mprime = 0.0f, F1 = 0.0f, F1spt = 0.0f, F7 = 0.0f;
  int32_t nname;

  // QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  FloatArrayType::Pointer avgQuatPtr = m_AvgQuatsPtr.lock();

  double LD[3] = {0.0f, 0.0f, 1.0f};

  F1lists.resize(totalFeatures);
  F1sptlists.resize(totalFeatures);
  F7lists.resize(totalFeatures);
  mPrimelists.resize(totalFeatures);

  for(size_t i = 1; i < totalFeatures; i++)
  {
    F1lists[i].assign(neighborlist[i].size(), 0.0f);
    F1sptlists[i].assign(neighborlist[i].size(), 0.0f);
    F7lists[i].assign(neighborlist[i].size(), 0.0f);
    mPrimelists[i].assign(neighborlist[i].size(), 0.0f);
    for(size_t j = 0; j < neighborlist[i].size(); j++)
    {
      nname = neighborlist[i][j];
      float* avgQuat = m_AvgQuats + i * 4;
      QuatType q1(avgQuat[0], avgQuat[1], avgQuat[2], avgQuat[3]);
      avgQuat = m_AvgQuats + nname * 4;
      QuatType q2(avgQuat[0], avgQuat[1], avgQuat[2], avgQuat[3]);

      if(m_CrystalStructures[m_FeaturePhases[i]] == m_CrystalStructures[m_FeaturePhases[nname]] && m_FeaturePhases[i] > 0)
      {
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[i]]]->getmPrime(q1, q2, LD);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[i]]]->getF1(q1, q2, LD, true);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[i]]]->getF1spt(q1, q2, LD, true);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[i]]]->getF7(q1, q2, LD, true);
      }
      else
      {
        mprime = 0.0f;
        F1 = 0.0f;
        F1spt = 0.0f;
        F7 = 0.0f;
      }
      mPrimelists[i][j] = mprime;
      F1lists[i][j] = F1;
      F1sptlists[i][j] = F1spt;
      F7lists[i][j] = F7;
    }
  }

  for(size_t i = 1; i < totalFeatures; i++)
  {
    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType f1L(new std::vector<float>);
    f1L->assign(F1lists[i].begin(), F1lists[i].end());
    m_F1List.lock()->setList(static_cast<int32_t>(i), f1L);

    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType f1sptL(new std::vector<float>);
    f1sptL->assign(F1sptlists[i].begin(), F1sptlists[i].end());
    m_F1sptList.lock()->setList(static_cast<int32_t>(i), f1sptL);

    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType f7L(new std::vector<float>);
    f7L->assign(F7lists[i].begin(), F7lists[i].end());
    m_F7List.lock()->setList(static_cast<int32_t>(i), f7L);

    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType mPrimeL(new std::vector<float>);
    mPrimeL->assign(mPrimelists[i].begin(), mPrimelists[i].end());
    m_mPrimeList.lock()->setList(static_cast<int32_t>(i), mPrimeL);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSlipTransmissionMetrics::newFilterInstance(bool copyFilterParameters) const
{
  FindSlipTransmissionMetrics::Pointer filter = FindSlipTransmissionMetrics::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindSlipTransmissionMetrics::getUuid() const
{
  return QUuid("{97523038-5fb2-5e82-9177-ed3e8b24b4bd}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getHumanLabel() const
{
  return "Find Neighbor Slip Transmission Metrics";
}

// -----------------------------------------------------------------------------
FindSlipTransmissionMetrics::Pointer FindSlipTransmissionMetrics::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindSlipTransmissionMetrics> FindSlipTransmissionMetrics::New()
{
  struct make_shared_enabler : public FindSlipTransmissionMetrics
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getNameOfClass() const
{
  return QString("FindSlipTransmissionMetrics");
}

// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::ClassName()
{
  return QString("FindSlipTransmissionMetrics");
}

// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setNeighborListArrayPath(const DataArrayPath& value)
{
  m_NeighborListArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindSlipTransmissionMetrics::getNeighborListArrayPath() const
{
  return m_NeighborListArrayPath;
}

// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setF1ListArrayName(const QString& value)
{
  m_F1ListArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getF1ListArrayName() const
{
  return m_F1ListArrayName;
}

// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setF1sptListArrayName(const QString& value)
{
  m_F1sptListArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getF1sptListArrayName() const
{
  return m_F1sptListArrayName;
}

// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setF7ListArrayName(const QString& value)
{
  m_F7ListArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getF7ListArrayName() const
{
  return m_F7ListArrayName;
}

// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setmPrimeListArrayName(const QString& value)
{
  m_mPrimeListArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSlipTransmissionMetrics::getmPrimeListArrayName() const
{
  return m_mPrimeListArrayName;
}

// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setAvgQuatsArrayPath(const DataArrayPath& value)
{
  m_AvgQuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindSlipTransmissionMetrics::getAvgQuatsArrayPath() const
{
  return m_AvgQuatsArrayPath;
}

// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setFeaturePhasesArrayPath(const DataArrayPath& value)
{
  m_FeaturePhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindSlipTransmissionMetrics::getFeaturePhasesArrayPath() const
{
  return m_FeaturePhasesArrayPath;
}

// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindSlipTransmissionMetrics::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}
