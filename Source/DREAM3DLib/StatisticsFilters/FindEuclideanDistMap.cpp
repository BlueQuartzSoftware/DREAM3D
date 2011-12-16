/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "FindEuclideanDistMap.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/StatisticsFilters/Algo.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindEuclideanDistMap::FindEuclideanDistMap()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindEuclideanDistMap::~FindEuclideanDistMap()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::execute()
{
  setErrorCondition(0);

  find_euclideandistmap();
  notify("FindEuclideanDistMap Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::find_euclideandistmap()
{
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  GET_NAMED_ARRAY_SIZE_CHK(m, DREAM3D::VoxelData::GrainIds, Int32ArrayType, int32_t, (m->totalpoints), grain_indicies);


  INITIALIZE_NAMED_ARRAY_TO_PTR(m, DREAM3D::VoxelData::NearestNeighbors, Int32ArrayType, int32_t, (m->totalpoints*3), nearestneighbors, 3);
  INITIALIZE_NAMED_ARRAY_TO_PTR(m, DREAM3D::VoxelData::NearestNeighborDistances, FloatArrayType, float, (m->totalpoints*3), nearestneighbordistances, 3);



  for (int i = 0; i < m->totalpoints*3; i++)
  {
    nearestneighbors[i] = -1;
    nearestneighbordistances[i] = -1;
  }
//  int neighpoint;
//  int nearestneighbor;
  int neighbors[6];
  int column, row, plane;
  int add = 1;
  int good = 0;
  int grain, neighbor;
  std::vector<int> coordination;

  neighbors[0] = -m->xpoints * m->ypoints;
  neighbors[1] = -m->xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = m->xpoints;
  neighbors[5] = m->xpoints * m->ypoints;
  for (int a = 0; a < (m->totalpoints); ++a)
  {
	grain = grain_indicies[a];
	if(grain > 0)
	{
	  coordination.resize(0);
	  column = a % m->xpoints;
	  row = (a / m->xpoints) % m->ypoints;
	  plane = a / (m->xpoints * m->ypoints);
	  for (int k = 0; k < 6; k++)
	  {
		good = 1;
		neighbor = a + neighbors[k];
		if(k == 0 && plane == 0) good = 0;
		if(k == 5 && plane == (m->zpoints - 1)) good = 0;
		if(k == 1 && row == 0) good = 0;
		if(k == 4 && row == (m->ypoints - 1)) good = 0;
		if(k == 2 && column == 0) good = 0;
		if(k == 3 && column == (m->xpoints - 1)) good = 0;
		if(good == 1 && grain_indicies[neighbor] != grain && grain_indicies[neighbor] > 0)
		{
			add = 1;
			for(size_t i=0;i<coordination.size();i++)
			{
				if(grain_indicies[neighbor] == coordination[i]) add = 0;
			}
			if(add == 1) coordination.push_back(grain_indicies[neighbor]);
		}
	  }
	}
	if(coordination.size() > 2) nearestneighbordistances[a*3+0] = 0, nearestneighbordistances[a*3+1] = 0, nearestneighbordistances[a*3+2] = 0, nearestneighbors[a*3+0] = coordination[0], nearestneighbors[a*3+1] = coordination[0], nearestneighbors[a*3+2] = coordination[0];
	if(coordination.size() == 2) nearestneighbordistances[a*3+0] = 0, nearestneighbordistances[a*3+1] = 0, nearestneighbordistances[a*3+2] = -1, nearestneighbors[a*3+0] = coordination[0], nearestneighbors[a*3+1] = coordination[0], nearestneighbors[a*3+2] = -1;
	if(coordination.size() == 1) nearestneighbordistances[a*3+0] = 0, nearestneighbordistances[a*3+1] = -1, nearestneighbordistances[a*3+2] = -1, nearestneighbors[a*3+0] = coordination[0], nearestneighbors[a*3+1] = -1, nearestneighbors[a*3+2] = -1;
	if(coordination.size() == 0) nearestneighbordistances[a*3+0] = -1, nearestneighbordistances[a*3+1] = -1, nearestneighbordistances[a*3+2] = -1, nearestneighbors[a*3+0] = -1, nearestneighbors[a*3+1] = -1, nearestneighbors[a*3+2] = -1;
 }
#if AIM_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  tbb::task_group* g = new tbb::task_group;
  g->run(FindEuclideanMap(this, 0));
  g->run(FindEuclideanMap(this, 1));
  g->run(FindEuclideanMap(this, 2));
  g->wait();
  delete g;
  #else
  for (int loop = 0; loop < 3; loop++)
  {
    FindEuclideanMap f(m, loop);
    f();
  }
#endif
}
