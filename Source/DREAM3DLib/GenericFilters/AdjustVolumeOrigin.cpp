/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "AdjustVolumeOrigin.h"

#include <limits>

#include <boost/assert.hpp>


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

namespace Detail
{
  /**
   * @brief The CalculateCentroidsImpl class
   */
  class UpdateVerticesImpl
  {
      VertexArray::Pointer m_Nodes;
      float* m_Delta;

    public:
      UpdateVerticesImpl(VertexArray::Pointer nodes, float* delta) :
        m_Nodes(nodes),
        m_Delta(delta)
      {
      }

      virtual ~UpdateVerticesImpl() {}

      void generate(size_t start, size_t end) const
      {

        VertexArray::Vert_t* nodes = m_Nodes->getPointer(0);
        for (size_t i = start; i < end; i++)
        {
          nodes[i].pos[0] = nodes[i].pos[0] - m_Delta[0];
          nodes[i].pos[1] = nodes[i].pos[1] - m_Delta[1];
          nodes[i].pos[2] = nodes[i].pos[2] - m_Delta[2];
        }

      }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
      void operator()(const tbb::blocked_range<size_t>& r) const
      {
        generate(r.begin(), r.end());
      }
#endif


  };

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdjustVolumeOrigin::AdjustVolumeOrigin() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
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
  parameters.push_back(FilterParameter::New("Origin", "Origin", FilterParameterWidgetType::FloatVec3Widget, getOrigin(), false, "Microns"));
  parameters.push_back(FilterParameter::New("Data Container To Apply To", "DataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getDataContainerName(), false));
  parameters.back()->setConditional(true);
  parameters.back()->setConditionalProperty("ApplyToVoxelVolume");
  parameters.back()->setConditionalLabel("Apply to Voxel Volume");
  parameters.push_back(FilterParameter::New("Surface Data Container To Apply To", "SurfaceDataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getSurfaceDataContainerName(), false));
  parameters.back()->setConditional(true);
  parameters.back()->setConditionalProperty("ApplyToSurfaceMesh");
  parameters.back()->setConditionalLabel("Apply to Surface Mesh");
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOrigin::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setApplyToVoxelVolume( reader->readValue("ApplyToVoxelVolume", false) );
  setApplyToSurfaceMesh( reader->readValue("ApplyToSurfaceMesh", false) );
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
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("ApplyToVoxelVolume", getApplyToVoxelVolume() );
  writer->writeValue("ApplyToSurfaceMesh", getApplyToSurfaceMesh() );
  writer->writeValue("DataContainerName", getDataContainerName());
  writer->writeValue("SurfaceDataContainerName", getSurfaceDataContainerName());
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
    VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
    if(getErrorCondition() < 0) { return; }
  }

  if (m_ApplyToSurfaceMesh == true)
  {
    SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName());
    if(getErrorCondition() < 0) { return; }
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      setErrorCondition(-384);
      notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdjustVolumeOrigin::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
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
    VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
    m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
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
void AdjustVolumeOrigin::updateSurfaceMesh()
{
  int err = 0;
  setErrorCondition(err);

  notifyStatusMessage(getHumanLabel(), "Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  VertexArray::Pointer nodesPtr = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getVertices();
  VertexArray::Vert_t* nodes = nodesPtr->getPointer(0);

  // First get the min/max coords.

  float min[3] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };

  size_t count = nodesPtr->getNumberOfTuples();
  for (size_t i = 0; i < count; i++)
  {
    if (nodes[i].pos[0] < min[0]) { min[0] = nodes[i].pos[0]; }
    if (nodes[i].pos[1] < min[1]) { min[1] = nodes[i].pos[1]; }
    if (nodes[i].pos[2] < min[2]) { min[2] = nodes[i].pos[2]; }
  }

  float delta[3] = {min[0] - m_Origin.x, min[1] - m_Origin.y, min[2] - m_Origin.z };
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      Detail::UpdateVerticesImpl(nodesPtr, delta), tbb::auto_partitioner());
  }
  else
#endif
  {
    Detail::UpdateVerticesImpl serial(nodesPtr, delta);
    serial.generate(0, count);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AdjustVolumeOrigin::newFilterInstance(bool copyFilterParameters)
{
  /*
  * ApplyToVoxelVolume
  * ApplyToSurfaceMesh
  * Origin
  */
  AdjustVolumeOrigin::Pointer filter = AdjustVolumeOrigin::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
