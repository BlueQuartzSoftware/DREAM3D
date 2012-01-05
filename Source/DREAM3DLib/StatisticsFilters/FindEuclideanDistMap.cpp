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

#if AIM_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/atomic.h>
#include <tbb/tick_count.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#endif



class FindEuclideanMap : public AbstractFilter
{
    DataContainer* m;
    int loop;

  public:
    /**
     *
     * @param recon
     */
    FindEuclideanMap(DataContainer* datacontainer, int l)  :
      AbstractFilter(),
      m(datacontainer),
      loop (l)
    {}

    virtual ~FindEuclideanMap() {}

    void operator()() const
    {
      std::cout << "  FindEuclideanMap: Loop = " << loop << std::endl;
      int64_t totalPoints = m->totalPoints();
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG(m, Voxel, DREAM3D::VoxelData::NearestNeighbors, Int32ArrayType, int32_t, (totalPoints*3), nearestneighbors);
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG(m, Voxel, DREAM3D::VoxelData::NearestNeighborDistances, FloatArrayType, float, (totalPoints*3), nearestneighbordistances);

      int nearestneighbordistance = 0;
      int count = 1;
//      int good = 1;
//      double x, y, z;
      int neighpoint;
      int nearestneighbor;
      int neighbors[6];
      int xpoints = m->getXPoints();
      int ypoints = m->getYPoints();
      int zpoints = m->getZPoints();
      double resx = m->getXRes();
      double resy = m->getYRes();
      double resz = m->getZRes();

      neighbors[0] = -m->getXPoints() * m->getYPoints();
      neighbors[1] = -m->getXPoints();
      neighbors[2] = -1;
      neighbors[3] = 1;
      neighbors[4] = m->getXPoints();
      neighbors[5] = m->getXPoints() * m->getYPoints();
      int* voxel_NearestNeighbor = new int[totalPoints];
      double* voxel_NearestNeighborDistance = new double[totalPoints];
      nearestneighbordistance = 0;
      for (int a = 0; a < (totalPoints); ++a)
      {
        voxel_NearestNeighbor[a] = nearestneighbors[a*3 + loop];
        voxel_NearestNeighborDistance[a] = nearestneighbordistances[a*3 + loop];
      }
      count = 1;
      int i;
      char mask[6] = {0,0,0,0,0,0};
      while (count != 0)
      {
        count = 0;
        nearestneighbordistance++;

        for (int z = 0; z < zpoints; ++z)
        {
          mask[0] = mask[5] = 1;
          if (z == 0 ) { mask[0] = 0; }
          if (z == zpoints - 1) { mask[5] = 0; }

          for (int y = 0; y < ypoints; ++y)
          {
            mask[1] = mask[4] = 1;
            if (y == 0 ) { mask[1] = 0; }
            if (y == ypoints - 1) { mask[4] = 0; }

            for (int x = 0; x < xpoints; ++x)
            {
              mask[2] = mask[3] = 1;
              if (x == 0 ) { mask[2] = 0; }
              if (x == xpoints - 1) { mask[3] = 0; }

              i = (z * xpoints*ypoints) + (y*xpoints) + x;
              if (voxel_NearestNeighbor[i] == -1)
              {
                count++;
                for (int j = 0; j < 6; j++)
                {
                  neighpoint = i + neighbors[j];
                  if (mask[j] == 1)
                  {
                    if (voxel_NearestNeighborDistance[neighpoint] != -1.0)
                    {
                      voxel_NearestNeighbor[i] = voxel_NearestNeighbor[neighpoint];
                    }
                  }
                }
              }
            }
          }
        }
        for (int j = 0; j < (totalPoints); ++j)
        {
          if (voxel_NearestNeighbor[j] != -1 && voxel_NearestNeighborDistance[j] == -1)
          {
            voxel_NearestNeighborDistance[j] = nearestneighbordistance;
          }
        }
      }
      double x1, x2, y1, y2, z1, z2;
      double dist;
      for (int j = 0; j < (totalPoints); j++)
      {
        nearestneighbor = voxel_NearestNeighbor[j];
        x1 = resx * double(j % xpoints); // find_xcoord(j);
        y1 = resy * double((j / xpoints) % ypoints);// find_ycoord(j);
        z1 = resz * double(j / (xpoints * ypoints)); // find_zcoord(j);
        x2 = resx * double(nearestneighbor % xpoints); // find_xcoord(nearestneighbor);
        y2 = resy * double((nearestneighbor / xpoints) % ypoints); // find_ycoord(nearestneighbor);
        z2 = resz * double(nearestneighbor / (xpoints * ypoints)); // find_zcoord(nearestneighbor);
        dist = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
        dist = sqrt(dist);
        voxel_NearestNeighborDistance[j] = dist + (0.5 * resx);
      }
      for (int a = 0; a < (totalPoints); ++a)
      {
        nearestneighbors[a*3 + loop] = voxel_NearestNeighbor[a];
        nearestneighbordistances[a*3 + loop] = voxel_NearestNeighborDistance[a];
      }
      delete[] voxel_NearestNeighbor;
      delete[] voxel_NearestNeighborDistance;
    }


};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindEuclideanDistMap::FindEuclideanDistMap() :
            AbstractFilter()
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

  int64_t totalPoints = m->totalPoints();
    int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == grain_indicies) { return; }

  int32_t* nearestneighbors = m->createVoxelData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::NearestNeighbors, totalPoints*3, 1, this);
  if (nearestneighbors == NULL) { return; }

  float* nearestneighbordistances = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::NearestNeighborDistances, totalPoints*3, 1, this);
  if (NULL == nearestneighbordistances) {return;}


  for (int i = 0; i < totalPoints*3; i++)
  {
    nearestneighbors[i] = -1;
    nearestneighbordistances[i] = -1;
  }

  size_t column, row, plane;
  int add = 1;
  int good = 0;
  int grain, neighbor;
  std::vector<int> coordination;

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  int neighbors[6];
  neighbors[0] = -dims[0]*dims[1];
  neighbors[1] = -dims[0];
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = dims[0];
  neighbors[5] = dims[0]*dims[1];


  for (int64_t a = 0; a < (totalPoints); ++a)
  {
	grain = grain_indicies[a];
	if(grain > 0)
	{
	  coordination.resize(0);
	  column = a % m->getXPoints();
	  row = (a / m->getXPoints()) % m->getYPoints();
	  plane = a / (m->getXPoints() * m->getYPoints());
	  for (size_t k = 0; k < 6; k++)
	  {
		good = 1;
		neighbor = a + neighbors[k];
		if(k == 0 && plane == 0) good = 0;
		if(k == 5 && plane == (m->getZPoints() - 1)) good = 0;
		if(k == 1 && row == 0) good = 0;
		if(k == 4 && row == (m->getYPoints() - 1)) good = 0;
		if(k == 2 && column == 0) good = 0;
		if(k == 3 && column == (m->getXPoints() - 1)) good = 0;
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
