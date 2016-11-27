/* ============================================================================
 * Copyright (c) 2016 BlueQuartz Softwae, LLC
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

#include "Stereographic3D.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// Include the MOC generated file for this class
#include "moc_Stereographic3D.cpp"

/**
 * @brief The GenerateCoordinatesImpl class implements a threaded algorithm that computes the IPF
 * colors for each element in a geometry
 */
class GenerateCoordinatesImpl
{
public:
  GenerateCoordinatesImpl(AbstractFilter* filter, float* quats, float* coordinates)
  : m_Filter(filter)
  , m_Quats(quats)
  , m_Coordinates(coordinates)
  {
  }

  virtual ~GenerateCoordinatesImpl()
  {
  }

  void convert(size_t start, size_t end) const
  {

    size_t index = 0;

    for(size_t i = start; i < end; i++)
    {
      if(m_Filter->getCancel())
      {
        return;
      }
      index = i * 3;
      if(m_Quats[index + 3] <= -1.0)
      {
        m_Filter->setCancel(true);
        QString ss = QObject::tr("The scalar value of a quaterion was <= -1.0. The value was").arg(m_Quats[index + 3]);
        m_Filter->setErrorCondition(-95000);
        m_Filter->notifyErrorMessage(m_Filter->getHumanLabel(), ss, m_Filter->getErrorCondition());
        return;
      }

      m_Coordinates[index + 0] = m_Quats[index + 0] / (1.0 + m_Quats[index + 3]);
      m_Coordinates[index + 1] = m_Quats[index + 1] / (1.0 + m_Quats[index + 3]);
      m_Coordinates[index + 2] = m_Quats[index + 2] / (1.0 + m_Quats[index + 3]);
    }
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif
private:
  AbstractFilter* m_Filter;
  float* m_Quats;
  float* m_Coordinates;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Stereographic3D::Stereographic3D()
: AbstractFilter()
, m_QuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats)
, m_CoordinatesArrayName("Coordinates")
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Stereographic3D::~Stereographic3D()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Stereographic3D::initialize()
{
  setErrorCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Stereographic3D::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::RequiredArray, Stereographic3D, req));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Coordinates", CoordinatesArrayName, FilterParameter::CreatedArray, Stereographic3D));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Stereographic3D::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath()));
  setCoordinatesArrayName(reader->readString("CoordinatesArrayName", getCoordinatesArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Stereographic3D::dataCheck()
{
  setErrorCondition(0);
  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);

  cDims[0] = 4;
  m_QuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_QuatsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dataArrayPaths.push_back(getQuatsArrayPath());

  cDims[0] = 3;
  DataArrayPath path = getQuatsArrayPath();
  path.setDataArrayName(getCoordinatesArrayName());
  m_CellCoordinatesPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter, float>(this, path, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellCoordinatesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellCoordinates = m_CellCoordinatesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dataArrayPaths.push_back(path);

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Stereographic3D::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Stereographic3D::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(getCancel() == true)
  {
    return;
  }

  size_t totalPoints = m_QuatsPtr.lock()->getNumberOfTuples();

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if(doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), GenerateCoordinatesImpl(this, m_Quats, m_CellCoordinates), tbb::auto_partitioner());
  }
  else
#endif
  {
    GenerateCoordinatesImpl serial(this, m_Quats, m_CellCoordinates);
    serial.convert(0, totalPoints);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer Stereographic3D::newFilterInstance(bool copyFilterParameters)
{
  Stereographic3D::Pointer filter = Stereographic3D::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Stereographic3D::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Stereographic3D::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Stereographic3D::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Stereographic3D::getGroupName()
{
  return SIMPL::FilterGroups::OrientationAnalysisFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Stereographic3D::getSubGroupName()
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Stereographic3D::getHumanLabel()
{
  return "Stereographic 3D Coordinates";
}
