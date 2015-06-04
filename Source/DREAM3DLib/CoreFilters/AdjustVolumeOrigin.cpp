/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "AdjustVolumeOrigin.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"

/**
 * @brief The UpdateVerticesImpl class implements a threaded algorithm that adjusts the
 * positions of a set of nodes relative to a new origin
 */
class UpdateVerticesImpl
{
    float* m_Nodes;
    float* m_Delta;

  public:
    UpdateVerticesImpl(float* nodes, float* delta) :
      m_Nodes(nodes),
      m_Delta(delta)
    {
    }

    virtual ~UpdateVerticesImpl() {}

    void generate(size_t start, size_t end) const
    {

      for (size_t i = start; i < end; i++)
      {
        m_Nodes[3*i] -= m_Delta[0];
        m_Nodes[3*i+1] -= m_Delta[1];
        m_Nodes[3*i+2] -= m_Delta[2];
      }

    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdjustVolumeOrigin::AdjustVolumeOrigin() :
  AbstractFilter(),
  m_DataContainerName(""),
  m_SurfaceDataContainerName(""),
  m_ApplyToVoxelVolume(true),
  m_ApplyToSurfaceMesh(true)
{
  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdjustVolumeOrigin::~AdjustVolumeOrigin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOrigin::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Origin", "Origin", FilterParameterWidgetType::FloatVec3Widget, getOrigin(), false, ""));
  QStringList linkedProps("DataContainerName");
  parameters.push_back(LinkedBooleanFilterParameter::New("Apply To Voxel Volume", "ApplyToVoxelVolume", getApplyToVoxelVolume(), linkedProps, true));
  parameters.push_back(FilterParameter::New("Data Container To Apply To", "DataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getDataContainerName(), true));
  linkedProps.clear();
  linkedProps << "SurfaceDataContainerName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Apply to Surface Mesh", "ApplyToSurfaceMesh", getApplyToSurfaceMesh(), linkedProps, true));
  parameters.push_back(FilterParameter::New("Surface Data Container To Apply To", "SurfaceDataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getSurfaceDataContainerName(), true));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOrigin::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setApplyToVoxelVolume( reader->readValue("ApplyToVoxelVolume", getApplyToVoxelVolume()) );
  setApplyToSurfaceMesh( reader->readValue("ApplyToSurfaceMesh", getApplyToSurfaceMesh()) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setSurfaceDataContainerName(reader->readString("SurfaceDataContainerName", getSurfaceDataContainerName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AdjustVolumeOrigin::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(Origin)
  DREAM3D_FILTER_WRITE_PARAMETER(ApplyToVoxelVolume)
  DREAM3D_FILTER_WRITE_PARAMETER(ApplyToSurfaceMesh)
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceDataContainerName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOrigin::dataCheck()
{
  setErrorCondition(0);

  if (m_ApplyToVoxelVolume == true)
  {
    getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getDataContainerName());
  }

  if (m_ApplyToSurfaceMesh == true)
  {
    getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry2D, AbstractFilter>(this, getSurfaceDataContainerName());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOrigin::preflight()
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
void AdjustVolumeOrigin::updateSurfaceMesh()
{
  setErrorCondition(0);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  IGeometry2D::Pointer geom2D = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName())->getGeometryAs<IGeometry2D>();
  float* nodes = geom2D->getVertexPointer(0);

  // First get the min/max coords.

  float min[3] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };

  int64_t count = geom2D->getNumberOfVertices();
  for (int64_t i = 0; i < count; i++)
  {
    if (nodes[3*i] < min[0])
    {
      min[0] = nodes[i];
    }
    if (nodes[3*i+1] < min[1])
    {
      min[1] = nodes[i+1];
    }
    if (nodes[3*i+2] < min[2])
    {
      min[2] = nodes[i+2];
    }
  }

  float delta[3] = {min[0] - m_Origin.x, min[1] - m_Origin.y, min[2] - m_Origin.z };
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      UpdateVerticesImpl(nodes, delta), tbb::auto_partitioner());
  }
  else
#endif
  {
    UpdateVerticesImpl serial(nodes, delta);
    serial.generate(0, count);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOrigin::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Set the Voxel Volume First, since this is easy
  if (m_ApplyToVoxelVolume == true)
  {
    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
    ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
    image->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
  }

  if (m_ApplyToSurfaceMesh == true)
  {
    updateSurfaceMesh();
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AdjustVolumeOrigin::newFilterInstance(bool copyFilterParameters)
{
  AdjustVolumeOrigin::Pointer filter = AdjustVolumeOrigin::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AdjustVolumeOrigin::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AdjustVolumeOrigin::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AdjustVolumeOrigin::getSubGroupName()
{ return DREAM3D::FilterSubGroups::SpatialFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AdjustVolumeOrigin::getHumanLabel()
{ return "Change Origin of Volume"; }
