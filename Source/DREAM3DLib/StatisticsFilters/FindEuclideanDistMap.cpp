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

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/atomic.h>
#include <tbb/tick_count.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#endif



class FindEuclideanMap
{
    VolumeDataContainer* m;
    int mapType;

  public:
    /**
     * @brief
     * @param datacontainer
     */
    FindEuclideanMap(VolumeDataContainer* datacontainer, int type) :
      m(datacontainer),
      mapType(type)
    {
    }
    virtual ~FindEuclideanMap()
    {
    }

    void operator()() const
    {
      // std::cout << "  FindEuclideanMap: Loop = " << loop << std::endl;
      int64_t totalPoints = m->getTotalPoints();
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG(m, Cell, DREAM3D::CellData::GrainIds, Int32ArrayType, int32_t, (totalPoints), m_GrainIds);
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG(m, Cell, DREAM3D::CellData::NearestNeighbors, Int32ArrayType, int32_t, (totalPoints * 3), m_NearestNeighbors);
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG(m, Cell, DREAM3D::CellData::GBEuclideanDistances, FloatArrayType, float, totalPoints, m_GBEuclideanDistances);
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG(m, Cell, DREAM3D::CellData::TJEuclideanDistances, FloatArrayType, float, totalPoints, m_TJEuclideanDistances);
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG(m, Cell, DREAM3D::CellData::QPEuclideanDistances, FloatArrayType, float, totalPoints, m_QPEuclideanDistances);

      int euclideanDistance = 0;
      int count = 1;
      //      int good = 1;
      //      double x, y, z;
      int neighpoint;
      int nearestneighbor;
      int neighbors[6];
      int xpoints = static_cast<int>(m->getXPoints());
      int ypoints = static_cast<int>(m->getYPoints());
      int zpoints = static_cast<int>(m->getZPoints());
      double resx = m->getXRes();
      double resy = m->getYRes();
      double resz = m->getZRes();

      neighbors[0] = -xpoints * ypoints;
      neighbors[1] = -xpoints;
      neighbors[2] = -1;
      neighbors[3] = 1;
      neighbors[4] = xpoints;
      neighbors[5] = xpoints * ypoints;
      int* voxel_NearestNeighbor = new int[totalPoints];
      double* voxel_EuclideanDistance = new double[totalPoints];
      euclideanDistance = 0;
      for (int a = 0; a < (totalPoints); ++a)
      {
        voxel_NearestNeighbor[a] = m_NearestNeighbors[a * 3 + mapType];
        if(mapType == 0) { voxel_EuclideanDistance[a] = m_GBEuclideanDistances[a]; }
        else if(mapType == 1) { voxel_EuclideanDistance[a] = m_TJEuclideanDistances[a]; }
        else if(mapType == 2) { voxel_EuclideanDistance[a] = m_QPEuclideanDistances[a]; }
      }
      count = 1;
      int i;
      char mask[6] =
      { 0, 0, 0, 0, 0, 0 };
      while (count != 0)
      {
        count = 0;
        euclideanDistance++;

        for (int z = 0; z < zpoints; ++z)
        {
          mask[0] = mask[5] = 1;
          if(z == 0)
          {
            mask[0] = 0;
          }
          if(z == zpoints - 1)
          {
            mask[5] = 0;
          }

          for (int y = 0; y < ypoints; ++y)
          {
            mask[1] = mask[4] = 1;
            if(y == 0)
            {
              mask[1] = 0;
            }
            if(y == ypoints - 1)
            {
              mask[4] = 0;
            }

            for (int x = 0; x < xpoints; ++x)
            {
              mask[2] = mask[3] = 1;
              if(x == 0)
              {
                mask[2] = 0;
              }
              if(x == xpoints - 1)
              {
                mask[3] = 0;
              }

              i = (z * xpoints * ypoints) + (y * xpoints) + x;
              if(voxel_NearestNeighbor[i] == -1)
              {
                count++;
                for (int j = 0; j < 6; j++)
                {
                  neighpoint = i + neighbors[j];
                  if(mask[j] == 1)
                  {
                    if(voxel_EuclideanDistance[neighpoint] != -1.0)
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
          if(voxel_NearestNeighbor[j] != -1 && voxel_EuclideanDistance[j] == -1 && m_GrainIds[j] > 0)
          {
            voxel_EuclideanDistance[j] = euclideanDistance;
          }
        }
      }
      double x1, x2, y1, y2, z1, z2;
      double dist;
      for (int j = 0; j < (totalPoints); j++)
      {
        nearestneighbor = voxel_NearestNeighbor[j];
        x1 = resx * double(j % xpoints); // find_xcoord(j);
        y1 = resy * double((j / xpoints) % ypoints); // find_ycoord(j);
        z1 = resz * double(j / (xpoints * ypoints)); // find_zcoord(j);
        x2 = resx * double(nearestneighbor % xpoints); // find_xcoord(nearestneighbor);
        y2 = resy * double((nearestneighbor / xpoints) % ypoints); // find_ycoord(nearestneighbor);
        z2 = resz * double(nearestneighbor / (xpoints * ypoints)); // find_zcoord(nearestneighbor);
        dist = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
        dist = sqrt(dist);
        //        voxel_EuclideanDistance[j] = dist + (0.5 * resx);
      }
      for (int a = 0; a < (totalPoints); ++a)
      {
        m_NearestNeighbors[a * 3 + mapType] = voxel_NearestNeighbor[a];
        if(mapType == 0) { m_GBEuclideanDistances[a] = static_cast<float>(voxel_EuclideanDistance[a]); }
        else if(mapType == 1) { m_TJEuclideanDistances[a] = static_cast<float>(voxel_EuclideanDistance[a]); }
        else if(mapType == 2) { m_QPEuclideanDistances[a] = static_cast<float>(voxel_EuclideanDistance[a]); }
      }
      delete[] voxel_NearestNeighbor;
      delete[] voxel_EuclideanDistance;
    }

};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindEuclideanDistMap::FindEuclideanDistMap() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_GBEuclideanDistancesArrayName(DREAM3D::CellData::GBEuclideanDistances),
  m_TJEuclideanDistancesArrayName(DREAM3D::CellData::TJEuclideanDistances),
  m_QPEuclideanDistancesArrayName(DREAM3D::CellData::QPEuclideanDistances),
  m_NearestNeighborsArrayName(DREAM3D::CellData::NearestNeighbors),
  m_GrainIds(NULL),
  m_NearestNeighbors(NULL),
  m_GBEuclideanDistances(NULL),
  m_TJEuclideanDistances(NULL),
  m_QPEuclideanDistances(NULL)
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
void FindEuclideanDistMap::setupFilterParameters()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindEuclideanDistMap::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, NearestNeighbors, int32_t, Int32ArrayType, 0, voxels, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GBEuclideanDistances, float, FloatArrayType, -1, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, TJEuclideanDistances, float, FloatArrayType, -1, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, QPEuclideanDistances, float, FloatArrayType, -1, voxels, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::execute()
{
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(),  m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  find_euclideandistmap();
  notifyStatusMessage("Completed");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::find_euclideandistmap()
{
  VolumeDataContainer* m = getVolumeDataContainer();

  int64_t totalPoints = m->getTotalPoints();

  for (int i = 0; i < totalPoints * 3; i++)
  {
    m_NearestNeighbors[i] = -1;
    if(i < totalPoints)
    {
      m_GBEuclideanDistances[i] = -1;
      m_TJEuclideanDistances[i] = -1;
      m_QPEuclideanDistances[i] = -1;
    }
  }

  size_t column, row, plane;
  int add = 1;
  int good = 0;
  int grain, neighbor;
  std::vector<int> coordination;

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighbors[6];
  neighbors[0] = -dims[0] * dims[1];
  neighbors[1] = -dims[0];
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = dims[0];
  neighbors[5] = dims[0] * dims[1];

  size_t xPoints = m->getXPoints();
  size_t yPoints = m->getYPoints();
  size_t zPoints = m->getZPoints();


  for (int64_t a = 0; a < totalPoints; ++a)
  {
    grain = m_GrainIds[a];
    if(grain > 0)
    {
      column = a % xPoints;
      row = (a / xPoints) % yPoints;
      plane = a / (xPoints * yPoints);
      for (size_t k = 0; k < 6; k++)
      {
        good = 1;
        neighbor = static_cast<int>( a + neighbors[k] );
        if(k == 0 && plane == 0) { good = 0; }
        if(k == 5 && plane == (zPoints - 1)) { good = 0; }
        if(k == 1 && row == 0) { good = 0; }
        if(k == 4 && row == (yPoints - 1)) { good = 0; }
        if(k == 2 && column == 0) { good = 0; }
        if(k == 3 && column == (xPoints - 1)) { good = 0; }
        if(good == 1 && m_GrainIds[neighbor] != grain && m_GrainIds[neighbor] > 0)
        {
          add = 1;
          for(size_t i = 0; i < coordination.size(); i++)
          {
            if(m_GrainIds[neighbor] == coordination[i]) { add = 0; }
          }
          if(add == 1) { coordination.push_back(m_GrainIds[neighbor]); }
        }
      }
      if(coordination.size() == 0) { m_NearestNeighbors[a * 3 + 0] = -1, m_NearestNeighbors[a * 3 + 1] = -1, m_NearestNeighbors[a * 3 + 2] = -1; }
      if(coordination.size() >= 1) { m_GBEuclideanDistances[a] = 0, m_NearestNeighbors[a * 3 + 0] = coordination[0], m_NearestNeighbors[a * 3 + 1] = -1, m_NearestNeighbors[a * 3 + 2] = -1; }
      if(coordination.size() >= 2) { m_TJEuclideanDistances[a] = 0, m_NearestNeighbors[a * 3 + 0] = coordination[0], m_NearestNeighbors[a * 3 + 1] = coordination[0], m_NearestNeighbors[a * 3 + 2] = -1; }
      if(coordination.size() > 2) { m_QPEuclideanDistances[a] = 0, m_NearestNeighbors[a * 3 + 0] = coordination[0], m_NearestNeighbors[a * 3 + 1] = coordination[0], m_NearestNeighbors[a * 3 + 2] = coordination[0]; }
      coordination.resize(0);
    }
    else { m_NearestNeighbors[a * 3 + 0] = 0, m_NearestNeighbors[a * 3 + 1] = 0, m_NearestNeighbors[a * 3 + 2] = 0; }
  }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::task_group* g = new tbb::task_group;
    g->run(FindEuclideanMap(m, 0));
    g->run(FindEuclideanMap(m, 1));
    g->run(FindEuclideanMap(m, 2));
    g->wait();
    delete g;
  }
  else
#endif
  {
    for(int i = 0; i < 3; i++)
    {
      FindEuclideanMap f(m, i);
      f();
    }
  }
}
