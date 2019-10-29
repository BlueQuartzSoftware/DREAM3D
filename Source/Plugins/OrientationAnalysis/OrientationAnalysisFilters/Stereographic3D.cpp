/* ============================================================================
 * Copyright (c) 2016 BlueQuartz Software, LLC
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

#include "Stereographic3D.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
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

  virtual ~GenerateCoordinatesImpl() = default;

  void convert(size_t start, size_t end) const
  {

    for(size_t i = start; i < end; i++)
    {
      size_t coordIndex = i * 3;
      size_t quatIndex = i * 4;
      if(m_Filter->getCancel())
      {
        return;
      }
      if(m_Quats[quatIndex + 3] <= -1.0f)
      {
        m_Filter->setCancel(true);
        QString ss = QObject::tr("The scalar value of a quaterion was <= -1.0. The value was %1").arg(m_Quats[quatIndex + 3]);
        m_Filter->setErrorCondition(-95000, ss);
        return;
      }
      if(m_Quats[quatIndex + 3] != 0.0f)
      {
        float q_x = m_Quats[quatIndex + 0];
        float q_y = m_Quats[quatIndex + 1];
        float q_z = m_Quats[quatIndex + 2];
        float q_w = m_Quats[quatIndex + 3];

        q_x = q_x / (1.0f + q_w);
        q_y = q_y / (1.0f + q_w);
        q_z = q_z / (1.0f + q_w);

        m_Coordinates[coordIndex + 0] = q_x;
        m_Coordinates[coordIndex + 1] = q_y;
        m_Coordinates[coordIndex + 2] = q_z;
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
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
: m_QuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats)
, m_CoordinatesArrayName("Coordinates")
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Stereographic3D::~Stereographic3D() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Stereographic3D::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Stereographic3D::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::Any, IGeometry::Type::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::RequiredArray, Stereographic3D, req));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Coordinates", CoordinatesArrayName, QuatsArrayPath, QuatsArrayPath, FilterParameter::CreatedArray, Stereographic3D));

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
  clearErrorCode();
  clearWarningCode();
  QVector<DataArrayPath> dataArrayPaths;

  std::vector<size_t> cDims(1, 1);

  cDims[0] = 4;
  m_QuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_QuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dataArrayPaths.push_back(getQuatsArrayPath());

  cDims[0] = 3;
  DataArrayPath path = getQuatsArrayPath();
  path.setDataArrayName(getCoordinatesArrayName());
  m_CellCoordinatesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter, float>(this, path, 0, cDims, "", DataArrayID31);
  if(nullptr != m_CellCoordinatesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  size_t totalPoints = m_QuatsPtr.lock()->getNumberOfTuples();

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), GenerateCoordinatesImpl(this, m_Quats, m_CellCoordinates), tbb::auto_partitioner());
  }
  else
#endif
  {
    GenerateCoordinatesImpl serial(this, m_Quats, m_CellCoordinates);
    serial.convert(0, totalPoints);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer Stereographic3D::newFilterInstance(bool copyFilterParameters) const
{
  Stereographic3D::Pointer filter = Stereographic3D::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Stereographic3D::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Stereographic3D::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Stereographic3D::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Stereographic3D::getGroupName() const
{
  return SIMPL::FilterGroups::Utilities;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid Stereographic3D::getUuid() const
{
  return QUuid("{3630623e-724b-5154-a060-a5fca4ecfff5}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Stereographic3D::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Stereographic3D::getHumanLabel() const
{
  return "Stereographic 3D Coordinates";
}

// -----------------------------------------------------------------------------
Stereographic3D::Pointer Stereographic3D::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<Stereographic3D> Stereographic3D::New()
{
  struct make_shared_enabler : public Stereographic3D
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString Stereographic3D::getNameOfClass() const
{
  return QString("Stereographic3D");
}

// -----------------------------------------------------------------------------
QString Stereographic3D::ClassName()
{
  return QString("Stereographic3D");
}

// -----------------------------------------------------------------------------
void Stereographic3D::setQuatsArrayPath(const DataArrayPath& value)
{
  m_QuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath Stereographic3D::getQuatsArrayPath() const
{
  return m_QuatsArrayPath;
}

// -----------------------------------------------------------------------------
void Stereographic3D::setCoordinatesArrayName(const QString& value)
{
  m_CoordinatesArrayName = value;
}

// -----------------------------------------------------------------------------
QString Stereographic3D::getCoordinatesArrayName() const
{
  return m_CoordinatesArrayName;
}
