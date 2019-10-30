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

#include "RotateEulerRefFrame.h"

#include <QtCore/QTextStream>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationLib/Core/OrientationTransformation.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

/**
 * @brief The RotateEulerRefFrameImpl class implements a threaded algorithm that rotates an array of Euler
 * angles about the supplied axis-angle pair.
 */
class RotateEulerRefFrameImpl
{
  float* m_CellEulerAngles;
  float angle;
  float axis[3];

public:
  RotateEulerRefFrameImpl(float* data, float rotAngle, float rotAxis[3])
  : m_CellEulerAngles(data)
  , angle(rotAngle)
  {
    axis[0] = rotAxis[0];
    axis[1] = rotAxis[1];
    axis[2] = rotAxis[2];
  }
  virtual ~RotateEulerRefFrameImpl() = default;

  void convert(size_t start, size_t end) const
  {
    float rotMat[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    OrientationTransformation::ax2om<OrientationF, OrientationF>(OrientationF(axis[0], axis[1], axis[2], angle)).toGMatrix(rotMat);

    float ea1 = 0, ea2 = 0, ea3 = 0;
    float g[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float gNew[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    for(size_t i = start; i < end; i++)
    {
      ea1 = m_CellEulerAngles[3 * i + 0];
      ea2 = m_CellEulerAngles[3 * i + 1];
      ea3 = m_CellEulerAngles[3 * i + 2];
      OrientationTransformation::eu2om<OrientationF, OrientationF>(OrientationF(ea1, ea2, ea3)).toGMatrix(g);

      MatrixMath::Multiply3x3with3x3(g, rotMat, gNew);
      MatrixMath::Normalize3x3(gNew);
      // Because we are going to simply wrap the m_CellEulerAngles array, the new
      // Euler angles will be directly written to the m_CellEulerAngles array
      // at the proper spot
      OrientationF eu(m_CellEulerAngles + (3 * i), 3);
      eu = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(gNew));
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateEulerRefFrame::RotateEulerRefFrame()
: m_RotationAngle(0.0f)
, m_CellEulerAnglesArrayPath("", "", "")
{
  m_RotationAxis[0] = 0.0f;
  m_RotationAxis[1] = 0.0f;
  m_RotationAxis[2] = 1.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateEulerRefFrame::~RotateEulerRefFrame() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Rotation Angle (Degrees)", RotationAngle, FilterParameter::Parameter, RotateEulerRefFrame));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Rotation Axis (ijk)", RotationAxis, FilterParameter::Parameter, RotateEulerRefFrame));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::RequiredArray, RotateEulerRefFrame, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setRotationAxis(reader->readFloatVec3("RotationAxis", getRotationAxis()));
  setRotationAngle(reader->readValue("RotationAngle", getRotationAngle()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> cDims(1, 3);
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellEulerAnglesPtr.lock())                                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::preflight()
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
void RotateEulerRefFrame::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  size_t totalPoints = m_CellEulerAnglesPtr.lock()->getNumberOfTuples();

  float rotAngle = m_RotationAngle * SIMPLib::Constants::k_Pi / 180.0f;
  float rotAxis[3] = {m_RotationAxis[0], m_RotationAxis[1], m_RotationAxis[2]};
  MatrixMath::Normalize3x1(rotAxis);

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), RotateEulerRefFrameImpl(m_CellEulerAngles, rotAngle, rotAxis), tbb::auto_partitioner());
  }
  else
#endif
  {
    RotateEulerRefFrameImpl serial(m_CellEulerAngles, rotAngle, rotAxis);
    serial.convert(0, totalPoints);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RotateEulerRefFrame::newFilterInstance(bool copyFilterParameters) const
{
  RotateEulerRefFrame::Pointer filter = RotateEulerRefFrame::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateEulerRefFrame::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateEulerRefFrame::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateEulerRefFrame::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateEulerRefFrame::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RotateEulerRefFrame::getUuid() const
{
  return QUuid("{ef9420b2-8c46-55f3-8ae4-f53790639de4}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateEulerRefFrame::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateEulerRefFrame::getHumanLabel() const
{
  return "Rotate Euler Reference Frame";
}

// -----------------------------------------------------------------------------
RotateEulerRefFrame::Pointer RotateEulerRefFrame::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RotateEulerRefFrame> RotateEulerRefFrame::New()
{
  struct make_shared_enabler : public RotateEulerRefFrame
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RotateEulerRefFrame::getNameOfClass() const
{
  return QString("RotateEulerRefFrame");
}

// -----------------------------------------------------------------------------
QString RotateEulerRefFrame::ClassName()
{
  return QString("RotateEulerRefFrame");
}

// -----------------------------------------------------------------------------
void RotateEulerRefFrame::setRotationAxis(const FloatVec3Type& value)
{
  m_RotationAxis = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type RotateEulerRefFrame::getRotationAxis() const
{
  return m_RotationAxis;
}

// -----------------------------------------------------------------------------
void RotateEulerRefFrame::setRotationAngle(float value)
{
  m_RotationAngle = value;
}

// -----------------------------------------------------------------------------
float RotateEulerRefFrame::getRotationAngle() const
{
  return m_RotationAngle;
}

// -----------------------------------------------------------------------------
void RotateEulerRefFrame::setCellEulerAnglesArrayPath(const DataArrayPath& value)
{
  m_CellEulerAnglesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RotateEulerRefFrame::getCellEulerAnglesArrayPath() const
{
  return m_CellEulerAnglesArrayPath;
}
