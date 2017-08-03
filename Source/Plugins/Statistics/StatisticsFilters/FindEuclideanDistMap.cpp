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

#include "FindEuclideanDistMap.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/atomic.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

/**
 * @brief The ComputeDistanceMapImpl class implements a threaded algorithm that computes the  distance map
 * for each point in the supplied volume
 */
template <typename T>
class ComputeDistanceMapImpl
{
    DataContainer::Pointer m_DataContainer;
    int32_t* m_FeatureIds;
    int32_t* m_NearestNeighbors;
    bool m_CalcManhattanDist;
    T* m_GBManhattanDistances;
    T* m_TJManhattanDistances;
    T* m_QPManhattanDistances;
    FindEuclideanDistMap::MapType m_MapType;

  public:
    ComputeDistanceMapImpl(DataContainer::Pointer datacontainer, int32_t* fIds, int32_t* nearNeighs, bool calcManhattanDist, T* gbDists, T* tjDists, T* qpDists, FindEuclideanDistMap::MapType mapType)
      : m_DataContainer(datacontainer)
      , m_FeatureIds(fIds)
      , m_NearestNeighbors(nearNeighs)
      , m_CalcManhattanDist(calcManhattanDist)
      , m_GBManhattanDistances(gbDists)
      , m_TJManhattanDistances(tjDists)
      , m_QPManhattanDistances(qpDists)
      , m_MapType(mapType)
    {
    }

    virtual ~ComputeDistanceMapImpl()
    {
    }

    void operator()() const
    {
      ImageGeom::Pointer imageGeom = m_DataContainer->getGeometryAs<ImageGeom>();
      size_t totalPoints = imageGeom->getNumberOfElements();
      double Distance = 0.0;
      size_t count = 1;
      size_t changed = 1;
      size_t neighpoint = 0;
      int64_t nearestneighbor;
      int64_t neighbors[6] = {0, 0, 0, 0, 0, 0};
      int64_t xpoints = static_cast<int64_t>(imageGeom->getXPoints());
      int64_t ypoints = static_cast<int64_t>(imageGeom->getYPoints());
      int64_t zpoints = static_cast<int64_t>(imageGeom->getZPoints());
      double resx = static_cast<double>(imageGeom->getXRes());
      double resy = static_cast<double>(imageGeom->getYRes());
      double resz = static_cast<double>(imageGeom->getZRes());

      neighbors[0] = -xpoints * ypoints;
      neighbors[1] = -xpoints;
      neighbors[2] = -1;
      neighbors[3] = 1;
      neighbors[4] = xpoints;
      neighbors[5] = xpoints * ypoints;

      // Use a std::vector to get an auto cleaned up array thus not needing the 'delete' keyword later on.
      std::vector<int32_t> voxNN(totalPoints, 0);
      int32_t* voxel_NearestNeighbor = &(voxNN.front());
      std::vector<double> voxEDist(totalPoints, 0.0);
      double* voxel_Distance = &(voxEDist.front());

      Distance = 0;
      for(size_t a = 0; a < totalPoints; ++a)
      {
        if(m_NearestNeighbors[a * 3 + static_cast<uint32_t>(m_MapType) ] >= 0)
        {
          voxel_NearestNeighbor[a] = static_cast<int32_t>(a);
        } // if voxel is boundary voxel, then want to use itself as nearest boundary voxel
        else
        {
          voxel_NearestNeighbor[a] = -1;
        }
        if(m_MapType == FindEuclideanDistMap::MapType::FeatureBoundary)
        {
          voxel_Distance[a] = static_cast<double>(m_GBManhattanDistances[a]);
        }
        else if(m_MapType == FindEuclideanDistMap::MapType::TripleJunction)
        {
          voxel_Distance[a] = static_cast<double>(m_TJManhattanDistances[a]);
        }
        else if(m_MapType == FindEuclideanDistMap::MapType::QuadPoint)
        {
          voxel_Distance[a] = static_cast<double>(m_QPManhattanDistances[a]);
        }
      }

      // ------------- Calculate the Manhattan Distance ----------------
      count = 1;
      changed = 1;
      int64_t i = 0;
      int64_t zBlock = xpoints * ypoints;
      int64_t zStride = 0, yStride = 0;
      char mask[6] = {0, 0, 0, 0, 0, 0};
      while(count > 0 && changed > 0)
      {
        count = 0;
        changed = 0;
        Distance++;

        for(int64_t z = 0; z < zpoints; ++z)
        {
          zStride = z * zBlock;
          mask[0] = mask[5] = 1;
          if(z == 0)
          {
            mask[0] = 0;
          }
          if(z == zpoints - 1)
          {
            mask[5] = 0;
          }

          for(int64_t y = 0; y < ypoints; ++y)
          {
            yStride = y * xpoints;
            mask[1] = mask[4] = 1;
            if(y == 0)
            {
              mask[1] = 0;
            }
            if(y == ypoints - 1)
            {
              mask[4] = 0;
            }

            for(int64_t x = 0; x < xpoints; ++x)
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

              i = zStride + yStride + x;
              if(voxel_NearestNeighbor[i] == -1 && m_FeatureIds[i] > 0)
              {
                count++;
                for(int32_t j = 0; j < 6; j++)
                {
                  neighpoint = i + neighbors[j];
                  if(mask[j] == 1)
                  {
                    if(voxel_Distance[neighpoint] != -1.0)
                    {
                      voxel_NearestNeighbor[i] = voxel_NearestNeighbor[neighpoint];
                    }
                  }
                }
              }
            }
          }
        }
        for(size_t j = 0; j < totalPoints; ++j)
        {
          if(voxel_NearestNeighbor[j] != -1 && voxel_Distance[j] == -1.0 && m_FeatureIds[j] > 0)
          {
            changed++;
            voxel_Distance[j] = Distance;
          }
        }
      }

      // ------------- Calculate the Euclidian Distance ----------------

      if(m_CalcManhattanDist == false)
      {
        double x1 = 0.0, x2 = 0.0, y1 = 0.0, y2 = 0.0, z1 = 0.0, z2 = 0.0;
        double dist = 0.0;
        double oneOverzBlock = 1.0 / double(zBlock);
        double oneOverxpoints = 1.0 / double(xpoints);
        for(int64_t m = 0; m < zpoints; m++)
        {
          zStride = m * zBlock;
          for(int64_t n = 0; n < ypoints; n++)
          {
            yStride = n * xpoints;
            for(int64_t p = 0; p < xpoints; p++)
            {
              x1 = double(p) * resx;
              y1 = double(n) * resy;
              z1 = double(m) * resz;
              nearestneighbor = voxel_NearestNeighbor[zStride + yStride + p];
              if(nearestneighbor >= 0)
              {
                x2 = resx * double(nearestneighbor % xpoints);                           // find_xcoord(nearestneighbor);
                y2 = resy * double(int64_t(nearestneighbor * oneOverxpoints) % ypoints); // find_ycoord(nearestneighbor);
                z2 = resz * floor(nearestneighbor * oneOverzBlock);                      // find_zcoord(nearestneighbor);
                dist = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
                dist = sqrt(dist);
                voxel_Distance[zStride + yStride + p] = dist;
              }
            }
          }
        }
      }
      for(size_t a = 0; a < totalPoints; ++a)
      {
        m_NearestNeighbors[a * 3 + static_cast<uint32_t>(m_MapType)] = voxel_NearestNeighbor[a];
        if(m_MapType == FindEuclideanDistMap::MapType::FeatureBoundary)
        {
          m_GBManhattanDistances[a] = static_cast<T>(voxel_Distance[a]);
        }
        else if(m_MapType == FindEuclideanDistMap::MapType::TripleJunction)
        {
          m_TJManhattanDistances[a] = static_cast<T>(voxel_Distance[a]);
        }
        else if(m_MapType == FindEuclideanDistMap::MapType::QuadPoint)
        {
          m_QPManhattanDistances[a] = static_cast<T>(voxel_Distance[a]);
        }
      }
    }
};

// Include the MOC generated file for this class
#include "moc_FindEuclideanDistMap.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindEuclideanDistMap::FindEuclideanDistMap()
  : AbstractFilter()
  , m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
  , m_GBDistancesArrayName(SIMPL::CellData::GBManhattanDistances)
  , m_TJDistancesArrayName(SIMPL::CellData::TJManhattanDistances)
  , m_QPDistancesArrayName(SIMPL::CellData::QPManhattanDistances)
  , m_NearestNeighborsArrayName(SIMPL::CellData::NearestNeighbors)
  , m_DoBoundaries(true)
  , m_DoTripleLines(false)
  , m_DoQuadPoints(false)
  , m_SaveNearestNeighbors(false)
  , m_CalcManhattanDist(true)
  , m_FeatureIds(nullptr)
  , m_NearestNeighbors(nullptr)
  , m_GBEuclideanDistances(nullptr)
  , m_TJEuclideanDistances(nullptr)
  , m_QPEuclideanDistances(nullptr)
  , m_GBManhattanDistances(nullptr)
  , m_TJManhattanDistances(nullptr)
  , m_QPManhattanDistances(nullptr)
{
  setupFilterParameters();
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
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_BOOL_FP("Calculate Manhattan Distance", CalcManhattanDist, FilterParameter::Parameter, FindEuclideanDistMap));
  QStringList linkedProps("GBDistancesArrayName");

  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Calculate Distance to Boundaries", DoBoundaries, FilterParameter::Parameter, FindEuclideanDistMap, linkedProps));
  linkedProps.clear();
  linkedProps << "TJDistancesArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Calculate Distance to Triple Lines", DoTripleLines, FilterParameter::Parameter, FindEuclideanDistMap, linkedProps));
  linkedProps.clear();
  linkedProps << "QPDistancesArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Calculate Distance to Quadruple Points", DoQuadPoints, FilterParameter::Parameter, FindEuclideanDistMap, linkedProps));
  linkedProps.clear();
  linkedProps << "NearestNeighborsArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Store the Nearest Boundary Cells", SaveNearestNeighbors, FilterParameter::Parameter, FindEuclideanDistMap, linkedProps));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindEuclideanDistMap, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Boundary Distances", GBDistancesArrayName, FilterParameter::CreatedArray, FindEuclideanDistMap));
  parameters.push_back(SIMPL_NEW_STRING_FP("Triple Line Distances", TJDistancesArrayName, FilterParameter::CreatedArray, FindEuclideanDistMap));
  parameters.push_back(SIMPL_NEW_STRING_FP("Quadruple Point Distances", QPDistancesArrayName, FilterParameter::CreatedArray, FindEuclideanDistMap));
  parameters.push_back(SIMPL_NEW_STRING_FP("Nearest Neighbors", NearestNeighborsArrayName, FilterParameter::CreatedArray, FindEuclideanDistMap));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNearestNeighborsArrayName(reader->readString("NearestNeighborsArrayName", getNearestNeighborsArrayName()));
  setQPDistancesArrayName(reader->readString("QPEuclideanDistancesArrayName", getQPDistancesArrayName()));
  setTJDistancesArrayName(reader->readString("TJEuclideanDistancesArrayName", getTJDistancesArrayName()));
  setGBDistancesArrayName(reader->readString("GBEuclideanDistancesArrayName", getGBDistancesArrayName()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setDoBoundaries(reader->readValue("DoBoundaries", getDoBoundaries()));
  setDoTripleLines(reader->readValue("DoTripleLines", getDoTripleLines()));
  setDoQuadPoints(reader->readValue("DoQuadPoints", getDoQuadPoints()));
  setSaveNearestNeighbors(reader->readValue("SaveNearestNeighbors", getSaveNearestNeighbors()));
  setCalcManhattanDist(reader->readValue("CalcOnlyManhattanDist", getCalcManhattanDist()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  if(m_DoBoundaries == true)
  {
    tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getGBDistancesArrayName());
    if (m_CalcManhattanDist == true)
    {
      m_GBManhattanDistancesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, cDims);
      if(nullptr != m_GBManhattanDistancesPtr.lock().get())
      {
        m_GBManhattanDistances = m_GBManhattanDistancesPtr.lock()->getPointer(0);
      }
    }
    else
    {
      m_GBEuclideanDistancesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, -1, cDims);
      if(nullptr != m_GBEuclideanDistancesPtr.lock().get())
      {
        m_GBEuclideanDistances = m_GBEuclideanDistancesPtr.lock()->getPointer(0);
      }
    }
  }

  if(m_DoTripleLines == true)
  {
    tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getTJDistancesArrayName());
    if (m_CalcManhattanDist == true)
    {
      m_TJManhattanDistancesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, cDims);
      if(nullptr != m_TJManhattanDistancesPtr.lock().get())
      {
        m_TJManhattanDistances = m_TJManhattanDistancesPtr.lock()->getPointer(0);
      }
    }
    else
    {
      m_TJEuclideanDistancesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, -1, cDims);
      if(nullptr != m_TJEuclideanDistancesPtr.lock().get())
      {
        m_TJEuclideanDistances = m_TJEuclideanDistancesPtr.lock()->getPointer(0);
      }
    }
  }

  if(m_DoQuadPoints == true)
  {
    tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getQPDistancesArrayName());
    if (m_CalcManhattanDist == true)
    {
      m_QPManhattanDistancesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, cDims);
      if(nullptr != m_QPManhattanDistancesPtr.lock().get())
      {
        m_QPManhattanDistances = m_QPManhattanDistancesPtr.lock()->getPointer(0);
      }
    }
    else
    {
      m_QPEuclideanDistancesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, -1, cDims);
      if(nullptr != m_QPEuclideanDistancesPtr.lock().get())
      {
        m_QPEuclideanDistances = m_QPEuclideanDistancesPtr.lock()->getPointer(0);
      }
    }
  }

  cDims[0] = 3;
  tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getNearestNeighborsArrayName());
  m_NearestNeighborsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
        this, tempPath, 0, cDims);                    /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NearestNeighborsPtr.lock().get())
  {
    m_NearestNeighbors = m_NearestNeighborsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    emit preflightExecuted();
    setInPreflight(false);
    return;
  }
  if(m_SaveNearestNeighbors == false)
  {
    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());
    AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(getFeatureIdsArrayPath().getAttributeMatrixName());
    attrMat->removeAttributeArray(getNearestNeighborsArrayName());
  }
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::findDistanceMap()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  for(size_t i = 0; i < totalPoints; i++)
  {
    if(m_DoBoundaries == true)
    {
      if (m_CalcManhattanDist == true)
      {
        m_GBManhattanDistances[i] = -1;
      }
      else
      {
        m_GBEuclideanDistances[i] = -1;
      }
    }
    if(m_DoTripleLines == true)
    {
      if (m_CalcManhattanDist == true)
      {
        m_TJManhattanDistances[i] = -1;
      }
      else
      {
        m_TJEuclideanDistances[i] = -1;
      }
    }
    if(m_DoQuadPoints == true)
    {
      if (m_CalcManhattanDist == true)
      {
        m_QPManhattanDistances[i] = -1;
      }
      else
      {
        m_QPEuclideanDistances[i] = -1;
      }
    }
  }

  int64_t column = 0, row = 0, plane = 0;
  bool good = false, add = true;
  int32_t feature = 0;
  std::vector<int32_t> coordination;

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] = {
    static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  int64_t neighbor = 0;
  int64_t neighbors[6] = {0, 0, 0, 0, 0, 0};
  neighbors[0] = -dims[0] * dims[1];
  neighbors[1] = -dims[0];
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = dims[0];
  neighbors[5] = dims[0] * dims[1];

  size_t xPoints = (m->getGeometryAs<ImageGeom>()->getXPoints());
  size_t yPoints = (m->getGeometryAs<ImageGeom>()->getYPoints());
  size_t zPoints = (m->getGeometryAs<ImageGeom>()->getZPoints());

  for(size_t a = 0; a < totalPoints; ++a)
  {
    feature = m_FeatureIds[a];
    if(feature > 0)
    {
      column = static_cast<int64_t>(a % xPoints);
      row = static_cast<int64_t>((a / xPoints) % yPoints);
      plane = static_cast<int64_t>(a / (xPoints * yPoints));
      for(int32_t k = 0; k < 6; k++)
      {
        good = true;
        neighbor = static_cast<int64_t>(a + neighbors[k]);
        if(k == 0 && plane == 0)
        {
          good = false;
        }
        if(k == 5 && plane == static_cast<int64_t>(zPoints - 1))
        {
          good = false;
        }
        if(k == 1 && row == 0)
        {
          good = false;
        }
        if(k == 4 && row == static_cast<int64_t>(yPoints - 1))
        {
          good = false;
        }
        if(k == 2 && column == 0)
        {
          good = false;
        }
        if(k == 3 && column == static_cast<int64_t>(xPoints - 1))
        {
          good = false;
        }
        if(good == true && m_FeatureIds[neighbor] != feature && m_FeatureIds[neighbor] >= 0)
        {
          add = true;
          for(size_t i = 0; i < coordination.size(); i++)
          {
            if(m_FeatureIds[neighbor] == coordination[i])
            {
              add = false;
            }
          }
          if(add == true)
          {
            coordination.push_back(m_FeatureIds[neighbor]);
          }
        }
      }
      if(coordination.size() == 0)
      {
        m_NearestNeighbors[a * 3 + 0] = -1;
        m_NearestNeighbors[a * 3 + 1] = -1;
        m_NearestNeighbors[a * 3 + 2] = -1;
      }
      if(coordination.size() >= 1 && m_DoBoundaries == true)
      {
        if (m_CalcManhattanDist == true)
        {
          m_GBManhattanDistances[a] = 0;
        }
        else
        {
          m_GBEuclideanDistances[a] = 0.0f;
        }

        m_NearestNeighbors[a * 3 + 0] = coordination[0];
        m_NearestNeighbors[a * 3 + 1] = -1;
        m_NearestNeighbors[a * 3 + 2] = -1;
      }
      if(coordination.size() >= 2 && m_DoTripleLines == true)
      {
        if (m_CalcManhattanDist == true)
        {
          m_TJManhattanDistances[a] = 0;
        }
        else
        {
          m_TJEuclideanDistances[a] = 0.0f;
        }

        m_NearestNeighbors[a * 3 + 0] = coordination[0];
        m_NearestNeighbors[a * 3 + 1] = coordination[0];
        m_NearestNeighbors[a * 3 + 2] = -1;
      }
      if(coordination.size() > 2 && m_DoQuadPoints == true)
      {
        if (m_CalcManhattanDist == true)
        {
          m_QPManhattanDistances[a] = 0;
        }
        else
        {
          m_QPEuclideanDistances[a] = 0.0f;
        }

        m_NearestNeighbors[a * 3 + 0] = coordination[0];
        m_NearestNeighbors[a * 3 + 1] = coordination[0];
        m_NearestNeighbors[a * 3 + 2] = coordination[0];
      }
      coordination.resize(0);
    }
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if(doParallel == true)
  {
    tbb::task_group* g = new tbb::task_group;
    if(m_DoBoundaries == true)
    {
      if (m_CalcManhattanDist == true)
      {
        g->run(ComputeDistanceMapImpl<int32_t>(m, m_FeatureIds, m_NearestNeighbors, m_CalcManhattanDist, m_GBManhattanDistances, m_TJManhattanDistances, m_QPManhattanDistances, MapType::FeatureBoundary));
      }
      else
      {
        g->run(ComputeDistanceMapImpl<float>(m, m_FeatureIds, m_NearestNeighbors, m_CalcManhattanDist, m_GBEuclideanDistances, m_TJEuclideanDistances, m_QPEuclideanDistances, MapType::FeatureBoundary));
      }
    }
    if(m_DoTripleLines == true)
    {
      if (m_CalcManhattanDist == true)
      {
        g->run(ComputeDistanceMapImpl<int32_t>(m, m_FeatureIds, m_NearestNeighbors, m_CalcManhattanDist, m_GBManhattanDistances, m_TJManhattanDistances, m_QPManhattanDistances, MapType::TripleJunction));
      }
      else
      {
        g->run(ComputeDistanceMapImpl<float>(m, m_FeatureIds, m_NearestNeighbors, m_CalcManhattanDist, m_GBEuclideanDistances, m_TJEuclideanDistances, m_QPEuclideanDistances, MapType::TripleJunction));
      }
    }
    if(m_DoQuadPoints == true)
    {
      if (m_CalcManhattanDist == true)
      {
        g->run(ComputeDistanceMapImpl<int32_t>(m, m_FeatureIds, m_NearestNeighbors, m_CalcManhattanDist, m_GBManhattanDistances, m_TJManhattanDistances, m_QPManhattanDistances, MapType::QuadPoint));
      }
      else
      {
        g->run(ComputeDistanceMapImpl<float>(m, m_FeatureIds, m_NearestNeighbors, m_CalcManhattanDist, m_GBEuclideanDistances, m_TJEuclideanDistances, m_QPEuclideanDistances, MapType::QuadPoint));
      }
    }
    g->wait();
    delete g;
  }
  else
#endif
  {
    for(int32_t i = 0; i < 3; i++)
    {
      FindEuclideanDistMap::MapType mapType = FindEuclideanDistMap::MapType::FeatureBoundary;
      if(i == 1)
      {
        mapType = FindEuclideanDistMap::MapType::TripleJunction;
      }
      else if(i == 2)
      {
        mapType = FindEuclideanDistMap::MapType::QuadPoint;
      }

      if((i == 0 && m_DoBoundaries == true) || (i == 1 && m_DoTripleLines == true) || (i == 2 && m_DoQuadPoints == true))
      {
        if (m_CalcManhattanDist == true)
        {
          ComputeDistanceMapImpl<int32_t> f(m, m_FeatureIds, m_NearestNeighbors, m_CalcManhattanDist, m_GBManhattanDistances, m_TJManhattanDistances, m_QPManhattanDistances, mapType);
          f();
        }
        else
        {
          ComputeDistanceMapImpl<float> f(m, m_FeatureIds, m_NearestNeighbors, m_CalcManhattanDist, m_GBEuclideanDistances, m_TJEuclideanDistances, m_QPEuclideanDistances, mapType);
          f();
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEuclideanDistMap::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  findDistanceMap();

  if(m_SaveNearestNeighbors == false)
  {
    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());
    AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(getFeatureIdsArrayPath().getAttributeMatrixName());
    attrMat->removeAttributeArray(getNearestNeighborsArrayName());
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindEuclideanDistMap::newFilterInstance(bool copyFilterParameters)
{
  FindEuclideanDistMap::Pointer filter = FindEuclideanDistMap::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindEuclideanDistMap::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindEuclideanDistMap::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindEuclideanDistMap::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindEuclideanDistMap::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindEuclideanDistMap::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindEuclideanDistMap::getHumanLabel()
{
  return "Find Euclidean Distance Map";
}
