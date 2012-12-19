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
/*
 * Your License or Copyright Information can go here
 */

#ifndef _CalculateTriangleCurvature_H_
#define _CalculateTriangleCurvature_H_

#include <string>




#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

class SurfaceMeshDataContainer;

#if DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include <tbb/task.h>
#endif


/**
 * @class CalculateTriangleCurvature CalculateTriangleCurvature.h /SurfaceMeshFilters/CalculateTriangleCurvature.h
 * @brief Calculates the Curvature at a particular triangle
 * @author Mike Jackson (BlueQuartz Software)s
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT CalculateTriangleCurvature
#if defined (DREAM3D_USE_PARALLEL_ALGORITHMS)
: public tbb::task
#endif
{
  public:
    CalculateTriangleCurvature(int nring,
                                std::vector<int> triangleIds,
                                int grainId,
                                NodeTrianglesMap_t* node2Triangle,
                                SurfaceMeshDataContainer* sm);

    virtual ~CalculateTriangleCurvature();

#if defined (DREAM3D_USE_PARALLEL_ALGORITHMS)
    tbb::task*
#else
    void
#endif
    execute();

  protected:
    CalculateTriangleCurvature();


    DataArray<double>::Pointer extractPatchData(UniqueTriangleIds_t &triPatch,
                                                double* data,
                                                const std::string &name);
  private:
    int m_NRing;
    std::vector<int> m_TriangleIds;
    int m_GrainId;
    NodeTrianglesMap_t* m_NodeTrianglesMap;
    SurfaceMeshDataContainer* m_SurfaceMeshDataContainer;

};

#endif /* _CalculateTriangleCurvature_H_ */


/*
 *
 ** If you want to call this from a TBB Task Group the following is a possibility
 *
std::string indent("  ");

#if defined (OpenMBIR_USE_PARALLEL_ALGORITHMS)
  tbb::task_scheduler_init init;
  //   int m_NumThreads = init.default_num_threads();
#else
  //   int m_NumThreads = 1;
#endif

  notify("Starting Forward Projection", 10, Observable::UpdateProgressValueAndMessage);
  START_TIMER;
  // This next section looks crazy with all the #if's but this makes sure we are
  // running the exact same code whether in parallel or serial.

#if OpenMBIR_USE_PARALLEL_ALGORITHMS
  tbb::task_group* g = new tbb::task_group;
  if(getVerbose())
  {
    std::cout << "Default Number of Threads to Use: " << init.default_num_threads() << std::endl;
    std::cout << "Forward Projection Running in Parallel." << std::endl;
  }
#else
  if(getVerbose())
  {
    std::cout << "Forward Projection Running in Serial." << std::endl;
  }
#endif
  // Queue up a thread for each z layer of the Geometry. The threads will only be
  // run as hardware resources open up so this will not just fire up a gazillion
  // threads.
  for (uint16_t t = 0; t < geometry->N_z; t++)
  {
#if OpenMBIR_USE_PARALLEL_ALGORITHMS
    g->run(ForwardProject(sinogram.get(), geometry.get(), tempCol, voxelLineResponse, yEstimate, this, t, this));
#else
    ForwardProject fp(sinogram.get(), geometry.get(), tempCol, voxelLineResponse, yEstimate, NuisanceParams.get(), t, this);
    //fp.setObservers(getObservers());
    fp();
#endif
  }
#if OpenMBIR_USE_PARALLEL_ALGORITHMS
  g->wait(); // Wait for all the threads to complete before moving on.
  delete g;
#endif

  STOP_TIMER;
  PRINT_TIME("Forward Project Time");


  */
