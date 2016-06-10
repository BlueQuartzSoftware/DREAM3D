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

#include "FindSlipTransmissionMetrics.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

// Include the MOC generated file for this class
#include "moc_FindSlipTransmissionMetrics.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlipTransmissionMetrics::FindSlipTransmissionMetrics() :
  AbstractFilter(),
  m_NeighborListArrayPath("", "", ""),
  m_F1ListArrayName(SIMPL::FeatureData::F1List),
  m_F1sptListArrayName(SIMPL::FeatureData::F1sptList),
  m_F7ListArrayName(SIMPL::FeatureData::F7List),
  m_mPrimeListArrayName(SIMPL::FeatureData::mPrimeList),
  m_AvgQuatsArrayPath("", "", ""),
  m_FeaturePhasesArrayPath("", "", ""),
  m_CrystalStructuresArrayPath("", "", ""),
  m_FeaturePhases(NULL),
  m_AvgQuats(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  m_F1List = NeighborList<float>::NullPointer();
  m_F1sptList = NeighborList<float>::NullPointer();
  m_F7List = NeighborList<float>::NullPointer();
  m_mPrimeList = NeighborList<float>::NullPointer();
  m_NeighborList = NeighborList<int32_t>::NullPointer();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlipTransmissionMetrics::~FindSlipTransmissionMetrics()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::NeighborList, 1, SIMPL::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Neighbor List", "NeighborListArrayPath", getNeighborListArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 4, SIMPL::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Average Quaternions", "AvgQuatsArrayPath", getAvgQuatsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, SIMPL::AttributeMatrixObjectType::Ensemble);
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("F1 List", "F1ListArrayName", getF1ListArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("F1spt List", "F1sptListArrayName", getF1sptListArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("F7 List", "F7ListArrayName", getF7ListArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("mPrime List", "mPrimeListArrayName", getmPrimeListArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setF1ListArrayName(reader->readString("F1ListArrayName", getF1ListArrayName() ) );
  setF1sptListArrayName(reader->readString("F1sptListArrayName", getF1sptListArrayName() ) );
  setF7ListArrayName(reader->readString("F7ListArrayName", getF7ListArrayName() ) );
  setmPrimeListArrayName(reader->readString("mPrimeListArrayName", getmPrimeListArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSlipTransmissionMetrics::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(NeighborListArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(F1ListArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(F1sptListArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(F7ListArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(mPrimeListArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getAvgQuatsArrayPath()); }

  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeaturePhasesArrayPath()); }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter>(this, getNeighborListArrayPath(), cDims);
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getNeighborListArrayPath()); }

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), m_NeighborListArrayPath.getAttributeMatrixName(), getF1ListArrayName() );
  m_F1List = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), m_NeighborListArrayPath.getAttributeMatrixName(), getF1sptListArrayName() );
  m_F1sptList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), m_NeighborListArrayPath.getAttributeMatrixName(), getF7ListArrayName() );
  m_F7List = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  tempPath.update(m_NeighborListArrayPath.getDataContainerName(), m_NeighborListArrayPath.getAttributeMatrixName(), getmPrimeListArrayName() );
  m_mPrimeList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

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
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int32_t>& neighborlist = *(m_NeighborList.lock());

  std::vector<std::vector<float> > F1lists;
  std::vector<std::vector<float> > F1sptlists;
  std::vector<std::vector<float> > F7lists;
  std::vector<std::vector<float> > mPrimelists;

  float mprime = 0.0f, F1 = 0.0f, F1spt = 0.0f, F7 = 0.0f;
  int32_t nname;
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float LD[3] = { 0.0f, 0.0f, 1.0f };

  F1lists.resize(totalFeatures);
  F1sptlists.resize(totalFeatures);
  F7lists.resize(totalFeatures);
  mPrimelists.resize(totalFeatures);

  for (size_t i = 1; i < totalFeatures; i++)
  {
    F1lists[i].assign(neighborlist[i].size(), 0.0f);
    F1sptlists[i].assign(neighborlist[i].size(), 0.0f);
    F7lists[i].assign(neighborlist[i].size(), 0.0f);
    mPrimelists[i].assign(neighborlist[i].size(), 0.0f);
    for (size_t j = 0; j < neighborlist[i].size(); j++)
    {
      nname = neighborlist[i][j];
      QuaternionMathF::Copy(avgQuats[i], q1);
      QuaternionMathF::Copy(avgQuats[nname], q1);
      if (m_CrystalStructures[m_FeaturePhases[i]] == m_CrystalStructures[m_FeaturePhases[nname]] && m_FeaturePhases[i] > 0)
      {
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[i]]]->getmPrime(q1, q2, LD, mprime);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[i]]]->getF1(q1, q2, LD, true, F1);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[i]]]->getF1spt(q1, q2, LD, true, F1spt);
        m_OrientationOps[m_CrystalStructures[m_FeaturePhases[i]]]->getF7(q1, q2, LD, true, F7);
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

  for (size_t i = 1; i < totalFeatures; i++)
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

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSlipTransmissionMetrics::newFilterInstance(bool copyFilterParameters)
{
  FindSlipTransmissionMetrics::Pointer filter = FindSlipTransmissionMetrics::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSlipTransmissionMetrics::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSlipTransmissionMetrics::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSlipTransmissionMetrics::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSlipTransmissionMetrics::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSlipTransmissionMetrics::getSubGroupName()
{ return SIMPL::FilterSubGroups::CrystallographicFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSlipTransmissionMetrics::getHumanLabel()
{ return "Find Neighbor Slip Transmission Metrics"; }
