/* ============================================================================
 * Copyright (c) 2018 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <memory>

#include "RodriguesConvertor.h"

#include <cmath>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"


#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

class RodriguesConvertorImpl
{
public:
  RodriguesConvertorImpl(RodriguesConvertor* filter, float* inputRod, float* outputRod)
  : m_Filter(filter)
  , m_Input(inputRod)
  , m_Output(outputRod)
  {
  }
  RodriguesConvertorImpl(const RodriguesConvertorImpl&) = default; // Copy Constructor
  RodriguesConvertorImpl(RodriguesConvertorImpl&&) = delete;      // Move Constructor Not Implemented
  RodriguesConvertorImpl& operator=(const RodriguesConvertorImpl&) = delete; // Copy Assignment Not Implemented
  RodriguesConvertorImpl& operator=(RodriguesConvertorImpl&&) = delete;      // Move Assignment Not Implemented

  virtual ~RodriguesConvertorImpl() = default;

  void convert(size_t start, size_t end) const
  {
     for(size_t i = start; i < end; i++)
    {
      if(m_Filter->getCancel()) { return; }
      float r0 = m_Input[i*3];
      float r1 = m_Input[i*3 + 1];
      float r2 = m_Input[i*3 + 2];
      float length = sqrtf(r0 * r0 + r1 * r1 + r2 * r2);

      m_Output[i*4] = r0/length;
      m_Output[i*4+1] = r1/length;
      m_Output[i*4+2] = r2/length;
      m_Output[i*4+3] = length;
    }
    
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif
private:
  RodriguesConvertor* m_Filter = nullptr;
  float* m_Input;
  float* m_Output;
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RodriguesConvertor::RodriguesConvertor() :
m_DeleteOriginalData(true)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RodriguesConvertor::~RodriguesConvertor() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RodriguesConvertor::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RodriguesConvertor::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataArraySelectionFilterParameter::RequirementType dasReq;
  std::vector<std::vector<size_t>> comp;
  comp.push_back(std::vector<size_t>(1, 3));
  dasReq.componentDimensions = comp;
  dasReq.daTypes = { SIMPL::TypeNames::Float };
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Rodrigues Array", RodriguesDataArrayPath, FilterParameter::Parameter, RodriguesConvertor, dasReq));
  DataArrayCreationFilterParameter::RequirementType dacReq;
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Output Data Array Path", OutputDataArrayPath, FilterParameter::CreatedArray, RodriguesConvertor, dacReq));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Delete Original Data", DeleteOriginalData, FilterParameter::Parameter, RodriguesConvertor));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RodriguesConvertor::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> cDims(1, 1);
  cDims[0] = 3;
  m_RodriguesVectorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getRodriguesDataArrayPath(), cDims);
  if(nullptr != m_RodriguesVectorsPtr.lock())                                                                       
  {
    m_RodriguesVectors = m_RodriguesVectorsPtr.lock()->getPointer(0);
  }
  
  cDims[0] = 4;
  m_OutputRodriguesVectorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getOutputDataArrayPath(), 0, cDims);                 /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_OutputRodriguesVectorsPtr.lock())   
  {
    m_OutputRodriguesVectors = m_OutputRodriguesVectorsPtr.lock()->getPointer(0);
  }
  
  if(getDeleteOriginalData() && getInPreflight())
  {
    AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(getRodriguesDataArrayPath());
    if(am)
    {
      am->removeAttributeArray(getRodriguesDataArrayPath().getDataArrayName());
    }
  }
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RodriguesConvertor::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RodriguesConvertor::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if (getCancel()) { return; }

  size_t totalPoints = m_RodriguesVectorsPtr.lock()->getNumberOfTuples();


#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif
  
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), RodriguesConvertorImpl(this, m_RodriguesVectors, m_OutputRodriguesVectors), tbb::auto_partitioner());
  }
  else
#endif
  {
    RodriguesConvertorImpl serial(this, m_RodriguesVectors, m_OutputRodriguesVectors);
    serial.convert(0, totalPoints);
  }
  
  /* Do not forget to remove the original array if requested */
  if(getDeleteOriginalData())
  {
    getDataContainerArray()->getAttributeMatrix(getRodriguesDataArrayPath())->removeAttributeArray(getRodriguesDataArrayPath().getDataArrayName());
  }
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RodriguesConvertor::newFilterInstance(bool copyFilterParameters) const
{
  RodriguesConvertor::Pointer filter = RodriguesConvertor::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RodriguesConvertor::getCompiledLibraryName() const
{ 
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RodriguesConvertor::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RodriguesConvertor::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RodriguesConvertor::getGroupName() const
{ 
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RodriguesConvertor::getSubGroupName() const
{ 
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RodriguesConvertor::getHumanLabel() const
{ 
  return "Rodrigues Convertor"; 
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RodriguesConvertor::getUuid() const
{
  return QUuid("{a2b62395-1a7d-5058-a840-752d8f8e2430}");
}

// -----------------------------------------------------------------------------
RodriguesConvertor::Pointer RodriguesConvertor::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RodriguesConvertor> RodriguesConvertor::New()
{
  struct make_shared_enabler : public RodriguesConvertor
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RodriguesConvertor::getNameOfClass() const
{
  return QString("_SUPERRodriguesConvertor");
}

// -----------------------------------------------------------------------------
QString RodriguesConvertor::ClassName()
{
  return QString("_SUPERRodriguesConvertor");
}

// -----------------------------------------------------------------------------
void RodriguesConvertor::setRodriguesDataArrayPath(const DataArrayPath& value)
{
  m_RodriguesDataArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RodriguesConvertor::getRodriguesDataArrayPath() const
{
  return m_RodriguesDataArrayPath;
}

// -----------------------------------------------------------------------------
void RodriguesConvertor::setOutputDataArrayPath(const DataArrayPath& value)
{
  m_OutputDataArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RodriguesConvertor::getOutputDataArrayPath() const
{
  return m_OutputDataArrayPath;
}

// -----------------------------------------------------------------------------
void RodriguesConvertor::setDeleteOriginalData(bool value)
{
  m_DeleteOriginalData = value;
}

// -----------------------------------------------------------------------------
bool RodriguesConvertor::getDeleteOriginalData() const
{
  return m_DeleteOriginalData;
}
