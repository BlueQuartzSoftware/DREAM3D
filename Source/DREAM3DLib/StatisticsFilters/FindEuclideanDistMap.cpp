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
    int32_t* m_FeatureIds;
    int32_t* m_NearestNeighbors;
    float* m_GBEuclideanDistances;
    float* m_TJEuclideanDistances;
    float* m_QPEuclideanDistances;
    int mapType;

  public:
    /**
     * @brief
     * @param datacontainer
     */
    FindEuclideanMap(VolumeDataContainer* datacontainer, int32_t* fIds, int32_t* nearNeighs, float* gbDists, float* tjDists, float* qpDists, int type) :
      m(datacontainer),
      m_FeatureIds(fIds),
      m_NearestNeighbors(nearNeighs),
      m_GBEuclideanDistances(gbDists),
      m_TJEuclideanDistances(tjDists),
      m_QPEuclideanDistances(qpDists),
      mapType(type)
    {
    }
    virtual ~FindEuclideanMap()
    {
    }

    void operator()() const
    {
      // qDebug() << "  FindEuclideanMap: Loop = " << loop << "\n";
      int64_t totalPoints = m->getTotalPoints();

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
          if(voxel_NearestNeighbor[j] != -1 && voxel_EuclideanDistance[j] == -1 && m_FeatureIds[j] > 0)
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
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_GBEuclideanDistancesArrayName(DREAM3D::CellData::GBEuclideanDistances),
  m_TJEuclideanDistancesArrayName(DREAM3D::CellData::TJEuclideanDistances),
  m_QPEuclideanDistancesArrayName(DREAM3D::CellData::QPEuclideanDistances),
  m_NearestNeighborsArrayName(DREAM3D::CellData::NearestNeighbors),
  m_FeatureIds(NULL),
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
void FindEuclideanDistMap::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FindEuclideanDistMap>(getDataContainerName(), false, this);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = m->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -300, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_GBEuclideanDistancesPtr = m->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_CellAttributeMatrixName,  m_GBEuclideanDistancesArrayName, -1, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GBEuclideanDistancesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GBEuclideanDistances = m_GBEuclideanDistancesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_TJEuclideanDistancesPtr = m->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_CellAttributeMatrixName,  m_TJEuclideanDistancesArrayName, -1, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_TJEuclideanDistancesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_TJEuclideanDistances = m_TJEuclideanDistancesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_QPEuclideanDistancesPtr = m->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_CellAttributeMatrixName,  m_QPEuclideanDistancesArrayName, -1, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QPEuclideanDistancesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_QPEuclideanDistances = m_QPEuclideanDistancesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_NearestNeighborsPtr = m->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_CellAttributeMatrixName,  m_NearestNeighborsArrayName, 0, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NearestNeighborsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NearestNeighbors = m_NearestNeighborsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

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
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(getDataContainerName(), false, this);
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, 0, 0);
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

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
  int feature, neighbor;
  QVector<int> coordination;

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
    feature = m_FeatureIds[a];
    if(feature > 0)
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
        if(good == 1 && m_FeatureIds[neighbor] != feature && m_FeatureIds[neighbor] > 0)
        {
          add = 1;
          for(size_t i = 0; i < coordination.size(); i++)
          {
            if(m_FeatureIds[neighbor] == coordination[i]) { add = 0; }
          }
          if(add == 1) { coordination.push_back(m_FeatureIds[neighbor]); }
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
    g->run(FindEuclideanMap(m, m_FeatureIds, m_NearestNeighbors, m_GBEuclideanDistances, m_TJEuclideanDistances, m_QPEuclideanDistances, 0));
    g->run(FindEuclideanMap(m, m_FeatureIds, m_NearestNeighbors, m_GBEuclideanDistances, m_TJEuclideanDistances, m_QPEuclideanDistances, 1));
    g->run(FindEuclideanMap(m, m_FeatureIds, m_NearestNeighbors, m_GBEuclideanDistances, m_TJEuclideanDistances, m_QPEuclideanDistances, 2));
    g->wait();
    delete g;
  }
  else
#endif
  {
    for(int i = 0; i < 3; i++)
    {
      FindEuclideanMap f(m, m_FeatureIds, m_NearestNeighbors, m_GBEuclideanDistances, m_TJEuclideanDistances, m_QPEuclideanDistances, i);
      f();
    }
  }
}
