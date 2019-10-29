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

#include "FindBasalLoadingFactor.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationLib/Core/OrientationTransformation.hpp"

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
FindBasalLoadingFactor::FindBasalLoadingFactor()
: m_AvgQuatsArrayPath("", "", "")
, m_BasalLoadingFactorArrayPath("", "", "")
{
  m_LoadingDirection[0] = 1.0f;
  m_LoadingDirection[1] = 1.0f;
  m_LoadingDirection[2] = 1.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBasalLoadingFactor::~FindBasalLoadingFactor() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::setupFilterParameters()
{
  FilterParameterVectorType parameters;

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
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> dims(1, 4);
  m_AvgQuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  m_BasalLoadingFactorPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getBasalLoadingFactorArrayPath(), 0, dims, "", DataArrayID31);
  if(nullptr != m_BasalLoadingFactorPtr.lock())         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  size_t totalFeatures = m_BasalLoadingFactorPtr.lock()->getNumberOfTuples();

  QuatF q1;
  //  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float sampleLoading[3];
  float w;
  float g1[3][3];
  float g1t[3][3];
  float caxis[3] = {0, 0, 1};
  float c1[3];

  sampleLoading[0] = m_LoadingDirection[0];
  sampleLoading[1] = m_LoadingDirection[1];
  sampleLoading[2] = m_LoadingDirection[2];
  MatrixMath::Normalize3x1(sampleLoading);

  for(size_t i = 1; i < totalFeatures; i++)
  {
    QuatF q1(m_AvgQuats + i * 4);
    OrientationTransformation::qu2om<QuatF, OrientationF>(q1).toGMatrix(g1);
    // transpose the g matricies so when caxis is multiplied by it
    // it will give the sample direction that the caxis is along
    MatrixMath::Transpose3x3(g1, g1t);
    MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
    // normalize so that the magnitude is 1
    MatrixMath::Normalize3x1(c1);
    if(c1[2] < 0)
    {
      MatrixMath::Multiply3x1withConstant(c1, -1.0f);
    }
    w = GeometryMath::CosThetaBetweenVectors(c1, sampleLoading);
    w = acos(w);
    w *= SIMPLib::Constants::k_180OverPi;
    m_BasalLoadingFactor[i] = w;
  }

  notifyStatusMessage("FindBasalLoadingFactor Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindBasalLoadingFactor::newFilterInstance(bool copyFilterParameters) const
{
  FindBasalLoadingFactor::Pointer filter = FindBasalLoadingFactor::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBasalLoadingFactor::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBasalLoadingFactor::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBasalLoadingFactor::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBasalLoadingFactor::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindBasalLoadingFactor::getUuid() const
{
  return QUuid("{4a8cf012-7ce0-5479-970e-3f5e2052396e}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBasalLoadingFactor::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBasalLoadingFactor::getHumanLabel() const
{
  return "Find Basal Loading Factors";
}

// -----------------------------------------------------------------------------
FindBasalLoadingFactor::Pointer FindBasalLoadingFactor::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindBasalLoadingFactor> FindBasalLoadingFactor::New()
{
  struct make_shared_enabler : public FindBasalLoadingFactor
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindBasalLoadingFactor::getNameOfClass() const
{
  return QString("FindBasalLoadingFactor");
}

// -----------------------------------------------------------------------------
QString FindBasalLoadingFactor::ClassName()
{
  return QString("FindBasalLoadingFactor");
}

// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::setAvgQuatsArrayPath(const DataArrayPath& value)
{
  m_AvgQuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindBasalLoadingFactor::getAvgQuatsArrayPath() const
{
  return m_AvgQuatsArrayPath;
}

// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::setBasalLoadingFactorArrayPath(const DataArrayPath& value)
{
  m_BasalLoadingFactorArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindBasalLoadingFactor::getBasalLoadingFactorArrayPath() const
{
  return m_BasalLoadingFactorArrayPath;
}

// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::setLoadingDirection(const FloatVec3Type& value)
{
  m_LoadingDirection = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type FindBasalLoadingFactor::getLoadingDirection() const
{
  return m_LoadingDirection;
}
