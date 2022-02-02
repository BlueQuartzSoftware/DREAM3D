/* ============================================================================
* Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ConvertQuaternion.h"

#include <array>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"


#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#endif


#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
// enum createdPathID : RenameDataPath::DataID_t
// {
//   DataArrayID30 = 30,
//   DataArrayID31 = 31,
// };

class ConvertQuaternionImpl
{
public:
  ConvertQuaternionImpl(ConvertQuaternion* filter, float* inputQuat, float* outputQuat)
  : m_Filter(filter)
  , m_Input(inputQuat)
  , m_Output(outputQuat)
  {
  }
  ConvertQuaternionImpl(const ConvertQuaternionImpl&) = default;           // Copy Constructor
  ConvertQuaternionImpl(ConvertQuaternionImpl&&) = delete;                 // Move Constructor Not Implemented
  ConvertQuaternionImpl& operator=(const ConvertQuaternionImpl&) = delete; // Copy Assignment Not Implemented
  ConvertQuaternionImpl& operator=(ConvertQuaternionImpl&&) = delete;      // Move Assignment Not Implemented

  virtual ~ConvertQuaternionImpl() = default;

  void convert(size_t start, size_t end) const
  {
    // Let's assume k_ToScalarVector which means the incoming quaternions are Vector-Scalar
    // <x,y,z> w  ---> w <x,y,z>
    std::array<size_t, 4> mapping = {{1, 2, 3, 0}};

    if(m_Filter->getConversionType() == ConvertQuaternion::k_ToVectorScalar)
    {
      // w <x,y,z>  ---> <x,y,z> w
      mapping = {{3, 0, 1, 2}};
    }

    std::array<float, 4> temp = {0.0f, 0.0f, 0.0f, 0.0f};
    for(size_t i = start; i < end; i++)
    {
      if(m_Filter->getCancel())
      {
        return;
      }

      temp[mapping[0]] = m_Input[i * 4];
      temp[mapping[1]] = m_Input[i * 4 + 1];
      temp[mapping[2]] = m_Input[i * 4 + 2];
      temp[mapping[3]] = m_Input[i * 4 + 3];

      m_Output[i * 4] = temp[0];
      m_Output[i * 4 + 1] = temp[1];
      m_Output[i * 4 + 2] = temp[2];
      m_Output[i * 4 + 3] = temp[3];
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif
private:
  ConvertQuaternion* m_Filter = nullptr;
  float* m_Input;
  float* m_Output;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertQuaternion::ConvertQuaternion()
: m_QuaternionDataArrayPath("", "", "")
, m_OutputDataArrayPath("", "", "OutQuats")
, m_DeleteOriginalData(false)
, m_ConversionType(k_ToScalarVector)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertQuaternion::~ConvertQuaternion() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertQuaternion::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertQuaternion::setupFilterParameters()
{
  FilterParameterVector parameters;
  DataArraySelectionFilterParameter::RequirementType dasReq;
  QVector<QVector<size_t>> comp;
  comp.push_back(QVector<size_t>(1, 4));
  dasReq.componentDimensions = comp;
  dasReq.daTypes = {SIMPL::TypeNames::Float};
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternion Array", QuaternionDataArrayPath, FilterParameter::Parameter, ConvertQuaternion, dasReq));
  DataArrayCreationFilterParameter::RequirementType dacReq;
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Output Data Array Path", OutputDataArrayPath, FilterParameter::CreatedArray, ConvertQuaternion, dacReq));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Delete Original Data", DeleteOriginalData, FilterParameter::Parameter, ConvertQuaternion));
  QVector<QString> choices = {"To Scalar Vector ( w, [x, y, z] )", "To Vector Scalar ( [x, y, z], w )"};
  parameters.push_back(SIMPL_NEW_CHOICE_FP("Conversion Type", ConversionType, FilterParameter::Parameter, ConvertQuaternion, choices, false));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertQuaternion::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QVector<size_t> cDims(1, 1);
  cDims[0] = 4;
  m_QuaternionsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuaternionDataArrayPath(), cDims);
  if(nullptr != m_QuaternionsPtr.lock())
  {
    m_Quaternions = m_QuaternionsPtr.lock()->getPointer(0);
  }

  cDims[0] = 4;
  m_OutputQuaternionsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getOutputDataArrayPath(), 0, cDims, "");
  if(nullptr != m_OutputQuaternionsPtr.lock())
  {
    m_OutputQuaternions = m_OutputQuaternionsPtr.lock()->getPointer(0);
  }

  if(getDeleteOriginalData() && getInPreflight())
  {
    AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(getQuaternionDataArrayPath());
    if(am)
    {
      am->removeAttributeArray(getQuaternionDataArrayPath().getDataArrayName());
    }
  }

  if(m_ConversionType < 0 || m_ConversionType > 1)
  {
    QString ss = QObject::tr("The conversion type must be either 0 (ToScalarVector) or 1 (ToVectorScalar). DREAM.3D expects Quaternions to be  in the Vector-Scalar form, i.e., <x,y,z> w");
    setErrorCondition(-52500);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertQuaternion::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  Q_EMIT preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  Q_EMIT updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  Q_EMIT preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertQuaternion::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t totalPoints = m_QuaternionsPtr.lock()->getNumberOfTuples();

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), ConvertQuaternionImpl(this, m_Quaternions, m_OutputQuaternions), tbb::auto_partitioner());
  }
  else
#endif
  {
    ConvertQuaternionImpl serial(this, m_Quaternions, m_OutputQuaternions);
    serial.convert(0, totalPoints);
  }

  /* Do not forget to remove the original array if requested */
  if(getDeleteOriginalData())
  {
    getDataContainerArray()->getAttributeMatrix(getQuaternionDataArrayPath())->removeAttributeArray(getQuaternionDataArrayPath().getDataArrayName());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertQuaternion::newFilterInstance(bool copyFilterParameters) const
{
  ConvertQuaternion::Pointer filter = ConvertQuaternion::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertQuaternion::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertQuaternion::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertQuaternion::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertQuaternion::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertQuaternion::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertQuaternion::getHumanLabel() const
{
  return "Convert Quaternion Order";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ConvertQuaternion::getUuid()
{
  return QUuid("{439e31b7-3198-5d0d-aef6-65a9e9c1a016}");
}

