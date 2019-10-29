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

#include "ReverseTriangleWinding.h"

#include <QtCore/QTextStream>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"

#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#endif

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

/**
 * @brief The ReverseWindingImpl class implements a threaded algorithm that reverses the node
 * windings for a set of triangles
 */
class ReverseWindingImpl
{
  SharedTriList::Pointer m_Triangles;

public:
  ReverseWindingImpl(SharedTriList::Pointer triangles)
  : m_Triangles(triangles)
  {
  }
  virtual ~ReverseWindingImpl() = default;

  void generate(size_t start, size_t end) const
  {
    MeshIndexType* triangles = m_Triangles->getPointer(0);

    for(size_t i = start; i < end; i++)
    {
      // Swap the indices
      int64_t nId0 = triangles[i * 3 + 0];
      int64_t nId2 = triangles[i * 3 + 2];

      triangles[i * 3 + 0] = nId2;
      triangles[i * 3 + 2] = nId0;
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  /**
   * @brief operator () This is called from the TBB stye of code
   * @param r The range to compute the values
   */
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    generate(r.begin(), r.end());
  }
#endif
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReverseTriangleWinding::ReverseTriangleWinding()
: m_SurfaceDataContainerName(SIMPL::Defaults::TriangleDataContainerName)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReverseTriangleWinding::~ReverseTriangleWinding() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReverseTriangleWinding::setupFilterParameters()
{
  SurfaceMeshFilter::setupFilterParameters();
  FilterParameterVectorType parameters;
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = IGeometry::Types(1, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container", SurfaceDataContainerName, FilterParameter::RequiredArray, ReverseTriangleWinding, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReverseTriangleWinding::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceDataContainerName(reader->readDataArrayPath("SurfaceDataContainerName", getSurfaceDataContainerName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReverseTriangleWinding::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReverseTriangleWinding::dataCheck()
{
  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceDataContainerName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReverseTriangleWinding::preflight()
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
void ReverseTriangleWinding::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, triangleGeom->getNumberOfTris()), ReverseWindingImpl(triangleGeom->getTriangles()), tbb::auto_partitioner());
  }
  else
#endif
  {
    ReverseWindingImpl serial(triangleGeom->getTriangles());
    serial.generate(0, triangleGeom->getNumberOfTris());
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReverseTriangleWinding::newFilterInstance(bool copyFilterParameters) const
{
  ReverseTriangleWinding::Pointer filter = ReverseTriangleWinding::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReverseTriangleWinding::getCompiledLibraryName() const
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReverseTriangleWinding::getBrandingString() const
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReverseTriangleWinding::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReverseTriangleWinding::getGroupName() const
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ReverseTriangleWinding::getUuid() const
{
  return QUuid("{9b9fb9e1-074d-54b6-a6ce-0be21ab4496d}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReverseTriangleWinding::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConnectivityArrangementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReverseTriangleWinding::getHumanLabel() const
{
  return "Reverse Triangle Winding";
}

// -----------------------------------------------------------------------------
ReverseTriangleWinding::Pointer ReverseTriangleWinding::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ReverseTriangleWinding> ReverseTriangleWinding::New()
{
  struct make_shared_enabler : public ReverseTriangleWinding
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ReverseTriangleWinding::getNameOfClass() const
{
  return QString("ReverseTriangleWinding");
}

// -----------------------------------------------------------------------------
QString ReverseTriangleWinding::ClassName()
{
  return QString("ReverseTriangleWinding");
}

// -----------------------------------------------------------------------------
void ReverseTriangleWinding::setSurfaceDataContainerName(const DataArrayPath& value)
{
  m_SurfaceDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ReverseTriangleWinding::getSurfaceDataContainerName() const
{
  return m_SurfaceDataContainerName;
}
