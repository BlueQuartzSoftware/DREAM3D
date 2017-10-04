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

#include "FindBasalLoadingFactor.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// Include the MOC generated file for this class
#include "moc_FindBasalLoadingFactor.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBasalLoadingFactor::FindBasalLoadingFactor()
: AbstractFilter()
, m_AvgQuatsArrayPath("", "", "")
, m_BasalLoadingFactorArrayPath("", "", "")
, m_BasalLoadingFactor(nullptr)
, m_AvgQuats(nullptr)
{
  m_LoadingDirection.x = 1.0f;
  m_LoadingDirection.y = 1.0f;
  m_LoadingDirection.z = 1.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBasalLoadingFactor::~FindBasalLoadingFactor()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Loading Direction", LoadingDirection, FilterParameter::Parameter, FindBasalLoadingFactor));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("AvgQuats", AvgQuatsArrayPath, FilterParameter::RequiredArray, FindBasalLoadingFactor, req));
  }

  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Basal Loading Factor", BasalLoadingFactorArrayPath, FilterParameter::CreatedArray, FindBasalLoadingFactor, req));
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setBasalLoadingFactorArrayPath(reader->readDataArrayPath("BasalLoadingFactorArrayPath", getBasalLoadingFactorArrayPath()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setLoadingDirection(reader->readFloatVec3("LoadingDirection", getLoadingDirection()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QVector<size_t> dims(1, 4);
  m_AvgQuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  m_BasalLoadingFactorPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, getBasalLoadingFactorArrayPath(), 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_BasalLoadingFactorPtr.lock().get())   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_BasalLoadingFactor = m_BasalLoadingFactorPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::preflight()
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
void FindBasalLoadingFactor::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t totalFeatures = m_BasalLoadingFactorPtr.lock()->getNumberOfTuples();

  // int ss = 0;
  QuatF q1;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float sampleLoading[3];
  // typedef DataArray<unsigned int> XTalType;

  float w;
  float g1[3][3];
  float g1t[3][3];
  float caxis[3] = {0, 0, 1};
  float c1[3];

  sampleLoading[0] = m_LoadingDirection.x;
  sampleLoading[1] = m_LoadingDirection.y;
  sampleLoading[2] = m_LoadingDirection.z;
  MatrixMath::Normalize3x1(sampleLoading);

  for(size_t i = 1; i < totalFeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    FOrientArrayType om(9);
    FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
    om.toGMatrix(g1);
    // transpose the g matricies so when caxis is multiplied by it
    // it will give the sample direction that the caxis is along
    MatrixMath::Transpose3x3(g1, g1t);
    MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
    // normalize so that the magnitude is 1
    MatrixMath::Normalize3x1(c1);
    if(c1[2] < 0)
    {
      MatrixMath::Multiply3x1withConstant(c1, -1);
    }
    w = GeometryMath::CosThetaBetweenVectors(c1, sampleLoading);
    w = acos(w);
    w *= SIMPLib::Constants::k_180OverPi;
    m_BasalLoadingFactor[i] = w;
  }

  notifyStatusMessage(getHumanLabel(), "FindBasalLoadingFactor Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindBasalLoadingFactor::newFilterInstance(bool copyFilterParameters)
{
  FindBasalLoadingFactor::Pointer filter = FindBasalLoadingFactor::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBasalLoadingFactor::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBasalLoadingFactor::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBasalLoadingFactor::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBasalLoadingFactor::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBasalLoadingFactor::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBasalLoadingFactor::getHumanLabel()
{
  return "Find Basal Loading Factors";
}
