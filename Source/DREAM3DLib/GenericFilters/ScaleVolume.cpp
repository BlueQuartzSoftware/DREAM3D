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

#include "ScaleVolume.h"

#include <limits>


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

/**
 * @brief The CalculateCentroidsImpl class
 */
class UpdateVerticesImpl
{
    DREAM3D::Mesh::VertListPointer_t m_Nodes;
    float* m_Min;
    FloatVec3Widget_t m_ScaleFactor;

  public:
    UpdateVerticesImpl(DREAM3D::Mesh::VertListPointer_t nodes, float* min, FloatVec3Widget_t scale) :
      m_Nodes(nodes),
      m_Min(min),
      m_ScaleFactor(scale)
    {
    }
    virtual ~UpdateVerticesImpl(){}

    void generate(size_t start, size_t end) const
    {

      DREAM3D::Mesh::Vert_t* nodes = m_Nodes->GetPointer(0);
      for (size_t i = start; i < end; i++)
      {
        nodes[i].pos[0] = m_Min[0] + (nodes[i].pos[0] - m_Min[0])*m_ScaleFactor.x;
        nodes[i].pos[1] = m_Min[1] + (nodes[i].pos[1] - m_Min[1])*m_ScaleFactor.y;
        nodes[i].pos[2] = m_Min[2] + (nodes[i].pos[2] - m_Min[2])*m_ScaleFactor.z;
      }

    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScaleVolume::ScaleVolume() :
  AbstractFilter(),
  m_ApplyToVoxelVolume(true),
  m_ApplyToSurfaceMesh(true),
  m_ApplyToSolidMesh(false)
{

  m_ScaleFactor.x = 1.0f;
  m_ScaleFactor.y = 1.0f;
  m_ScaleFactor.z = 1.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScaleVolume::~ScaleVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScaleVolume::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Apply to Voxel Volume");
    option->setPropertyName("ApplyToVoxelVolume");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Apply to Surface Mesh");
    option->setPropertyName("ApplyToSurfaceMesh");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
#if 0
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Apply to Solid Mesh");
    option->setPropertyName("ApplyToSolidMesh");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
#endif

  {
    FilterParameter::Pointer option = FilterParameter::New();

    option->setHumanLabel("Scaling Factor");
    option->setPropertyName("ScaleFactor");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
 //   option->setUnits("Microns");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScaleVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setApplyToVoxelVolume( reader->readValue("ApplyToVoxelVolume", getApplyToVoxelVolume()) );
  setApplyToSurfaceMesh( reader->readValue("ApplyToSurfaceMesh", getApplyToSurfaceMesh()) );
  setScaleFactor( reader->readValue("ScaleFactor", getScaleFactor() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ScaleVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ScaleFactor", getScaleFactor() );
  writer->writeValue("ApplyToVoxelVolume", getApplyToVoxelVolume() );
  writer->writeValue("ApplyToSurfaceMesh", getApplyToSurfaceMesh() );
  writer->writeValue("ApplyToSolidMesh", getApplyToSolidMesh() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScaleVolume::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  if (m_ApplyToVoxelVolume == true)
  {
    VolumeDataContainer* m = getVolumeDataContainer();
    if(NULL == m)
    {
      setErrorCondition(-383);
      addErrorMessage(getHumanLabel(), "VolumeDataContainer is missing", getErrorCondition());
    }
  }

  if (m_ApplyToSurfaceMesh == true)
  {
    SurfaceDataContainer* sm = getSurfaceDataContainer();
    if(NULL == sm)
    {
      setErrorCondition(-383);
      addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", getErrorCondition());
    }
    else
    {
      // We MUST have Nodes
      if(sm->getVertices().get() == NULL)
      {
        setErrorCondition(-384);
        addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScaleVolume::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScaleVolume::execute()
{
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  std::stringstream ss;


  if (m_ApplyToVoxelVolume ==true)
  {
    float resolution[3];
    m->getResolution(resolution);
    resolution[0] *= m_ScaleFactor.x;
    resolution[1] *= m_ScaleFactor.y;
    resolution[2] *= m_ScaleFactor.z;
  }

  if (m_ApplyToSurfaceMesh == true)
  {
    updateSurfaceMesh();
  }

  if (m_ApplyToSolidMesh == true)
  {
    updatesSolidMesh();
  }

  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScaleVolume::updateSurfaceMesh()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceDataContainer* m = getSurfaceDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  DREAM3D::Mesh::VertListPointer_t nodesPtr = getSurfaceDataContainer()->getVertices();
  DREAM3D::Mesh::Vert_t* nodes = nodesPtr->GetPointer(0);

  // First get the min/max coords.

  float min[3] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
  float max[3] = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

  size_t count = nodesPtr->GetNumberOfTuples();
  for (size_t i = 0; i < count; i++)
  {
    if (nodes[i].pos[0] > max[0]) { max[0] = nodes[i].pos[0]; }
    if (nodes[i].pos[1] > max[1]) { max[1] = nodes[i].pos[1]; }
    if (nodes[i].pos[2] > max[2]) { max[2] = nodes[i].pos[2]; }

    if (nodes[i].pos[0] < min[0]) { min[0] = nodes[i].pos[0]; }
    if (nodes[i].pos[1] < min[1]) { min[1] = nodes[i].pos[1]; }
    if (nodes[i].pos[2] < min[2]) { min[2] = nodes[i].pos[2]; }
  }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      UpdateVerticesImpl(nodesPtr, min, m_ScaleFactor), tbb::auto_partitioner());

  }
  else
#endif
  {
    UpdateVerticesImpl serial(nodesPtr, min, m_ScaleFactor);
    serial.generate(0, count);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScaleVolume::updatesSolidMesh()
{
  BOOST_ASSERT(false);
}
