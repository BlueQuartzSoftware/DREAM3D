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

#include "FindNeighbors.h"

#include <sstream>

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/NeighborList.hpp"
#include "DREAM3DLib/Common/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::FindNeighbors() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_SurfaceVoxelsArrayName(DREAM3D::CellData::SurfaceVoxels),
m_NumNeighborsArrayName(DREAM3D::FieldData::NumNeighbors),
m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
m_SharedSurfaceAreaListArrayName(DREAM3D::FieldData::SharedSurfaceAreaList),
m_NeighborListArrayName(DREAM3D::FieldData::NeighborList),
m_GrainIds(NULL),
m_SurfaceVoxels(NULL),
m_SurfaceFields(NULL),
m_NumNeighbors(NULL),
m_NeighborList(NULL),
m_SharedSurfaceAreaList(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighbors::~FindNeighbors()
{
}
// -----------------------------------------------------------------------------
void FindNeighbors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindNeighbors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, int8_t, Int8ArrayType, 0, voxels, 1)

  // Field Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFieldTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                                          (m->getCellFieldData(m_NeighborListArrayName).get());
  if(m_NeighborList == NULL)
  {
    NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();
    neighborlistPtr->SetName(m_NeighborListArrayName);
    neighborlistPtr->Resize(fields);
    neighborlistPtr->setNumNeighborsArrayName(m_NumNeighborsArrayName);
    m->addCellFieldData(m_NeighborListArrayName, neighborlistPtr);
    if (neighborlistPtr.get() == NULL) {
      ss << "NeighborLists Array Not Initialized at Beginning of FindNeighbors Filter" << std::endl;
      setErrorCondition(-308);
    }
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                                          (m->getCellFieldData(m_NeighborListArrayName).get());

    CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();
    e->setFilterName(this->getNameOfClass());
    e->setFilterHumanLabel(this->getHumanLabel());
    e->setFilterGroup(this->getGroupName());
    e->setFilterSubGroup(this->getSubGroupName());
    e->setArrayDefaultName(m_NeighborListArrayName);
    e->setArrayGroup("Field");
    e->setArrayNumComponents(0);
    e->setArrayType("NeighborList");
    addCreatedArrayHelpIndexEntry(e);
  }

  // And we do the same for the SharedSurfaceArea list
  m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>
                                 (m->getCellFieldData(m_SharedSurfaceAreaListArrayName).get());
  if(m_SharedSurfaceAreaList == NULL)
  {
    NeighborList<float>::Pointer sharedSurfaceAreaListPtr = NeighborList<float>::New();
    sharedSurfaceAreaListPtr->SetName(m_SharedSurfaceAreaListArrayName);
    sharedSurfaceAreaListPtr->Resize(fields);
    sharedSurfaceAreaListPtr->setNumNeighborsArrayName(m_NumNeighborsArrayName);
    m->addCellFieldData(m_SharedSurfaceAreaListArrayName, sharedSurfaceAreaListPtr);
    if (sharedSurfaceAreaListPtr.get() == NULL)
    {
      ss.str("");
      ss << "SurfaceAreaLists Array Not Initialized correctly at Beginning of FindNeighbors Filter" << std::endl;
      setErrorCondition(-308);
      addErrorMessage(getHumanLabel(), ss.str(), -308);
    }
    m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>
                                 (m->getCellFieldData(m_SharedSurfaceAreaListArrayName).get());
    CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();
    e->setFilterName(this->getNameOfClass());
    e->setFilterHumanLabel(this->getHumanLabel());
    e->setFilterGroup(this->getGroupName());
    e->setFilterSubGroup(this->getSubGroupName());
    e->setArrayDefaultName(m_SharedSurfaceAreaListArrayName);
    e->setArrayGroup("Field");
    e->setArrayNumComponents(0);
    e->setArrayType("SurfaceAreaLists");
    addCreatedArrayHelpIndexEntry(e);
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, SurfaceFields, bool, BoolArrayType, false, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, NumNeighbors, int32_t, Int32ArrayType, 0, fields, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighbors::execute()
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }


  int64_t totalPoints = m->getTotalPoints();
  int totalFields = int(m->getNumCellFieldTuples());
  dataCheck(false, totalPoints, totalFields, m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

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

  DimType neighpoints[6];
  neighpoints[0] = -dims[0]*dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0]*dims[1];

  float column, row, plane;
  int grain;
  size_t nnum;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;

  //size_t xtalCount = m->getCellEnsembleData(DREAM3D::EnsembleData::CrystalStructures)->GetNumberOfTuples();

    std::vector<std::vector<int> > neighborlist;
    std::vector<std::vector<float> > neighborsurfacearealist;

  int nListSize = 100;
  neighborlist.resize(totalFields);
  neighborsurfacearealist.resize(totalFields);
  for (int i = 1; i < totalFields; i++)
  {
    std::stringstream ss;
    ss << "Finding Neighbors - Initializing Neighbor Lists - " << (static_cast<float>(i)/totalFields)*100 << " Percent Complete";
 //   notifyStatusMessage(ss.str());
    m_NumNeighbors[i] = 0;
    neighborlist[i].resize(nListSize);
    neighborsurfacearealist[i].resize(nListSize, -1.0);
  m_SurfaceFields[i] = false;
  }

  totalPoints = m->getTotalPoints();

  for (int64_t j = 0; j < totalPoints; j++)
  {
    std::stringstream ss;
    ss << "Finding Neighbors - Determining Neighbor Lists - " << (static_cast<float>(j)/totalPoints)*100 << " Percent Complete";
 //   notifyStatusMessage(ss.str());
    onsurf = 0;
    grain = m_GrainIds[j];
    if(grain > 0)
    {
      column = static_cast<float>( j % m->getXPoints() );
      row = static_cast<float>( (j / m->getXPoints()) % m->getYPoints() );
      plane = static_cast<float>( j / (m->getXPoints() * m->getYPoints()) );
      if((column == 0 || column == (m->getXPoints() - 1) || row == 0 || row == (m->getYPoints() - 1) || plane == 0 || plane == (m->getZPoints() - 1)) && m->getZPoints() != 1)
      {
        m_SurfaceFields[grain] = true;
      }
      if((column == 0 || column == (m->getXPoints() - 1) || row == 0 || row == (m->getYPoints() - 1)) && m->getZPoints() == 1)
      {
        m_SurfaceFields[grain] = true;
      }
      for (int k = 0; k < 6; k++)
      {
        good = 1;
        neighbor = static_cast<int>( j + neighpoints[k] );
        if(k == 0 && plane == 0) good = 0;
        if(k == 5 && plane == (m->getZPoints() - 1)) good = 0;
        if(k == 1 && row == 0) good = 0;
        if(k == 4 && row == (m->getYPoints() - 1)) good = 0;
        if(k == 2 && column == 0) good = 0;
        if(k == 3 && column == (m->getXPoints() - 1)) good = 0;
        if(good == 1 && m_GrainIds[neighbor] != grain && m_GrainIds[neighbor] > 0)
        {
          onsurf++;
          nnum = m_NumNeighbors[grain];
          neighborlist[grain].push_back(m_GrainIds[neighbor]);
          nnum++;
          m_NumNeighbors[grain] = static_cast<int32_t>(nnum);
        }
      }
    }
    m_SurfaceVoxels[j] = onsurf;
  }

  // We do this to create new set of NeighborList objects
  dataCheck(false, totalPoints, totalFields, m->getNumCellEnsembleTuples());

  for (size_t i = 1; i < m->getNumCellFieldTuples(); i++)
  {
    std::stringstream ss;
    ss << "Finding Neighbors - Calculating Surface Areas - " << ((float)i/totalFields)*100 << " Percent Complete";
  //  notifyStatusMessage(ss.str());

    std::map<int, int> neighToCount;
    int numneighs = static_cast<int>( neighborlist[i].size() );

    // this increments the voxel counts for each grain
    for (int j = 0; j < numneighs; j++)
    {
      neighToCount[neighborlist[i][j]]++;
    }

    neighToCount.erase(0);
    neighToCount.erase(-1);
    //Resize the grains neighbor list to zero
    neighborlist[i].resize(0);
    neighborsurfacearealist[i].resize(0);

    for (std::map<int, int>::iterator iter = neighToCount.begin(); iter != neighToCount.end(); ++iter)
    {
      int neigh = iter->first; // get the neighbor grain
      int number = iter->second; // get the number of voxels
      float area = number * m->getXRes() * m->getYRes();

      // Push the neighbor grain id back onto the list so we stay synced up
      neighborlist[i].push_back(neigh);
      neighborsurfacearealist[i].push_back(area);
    }
    m_NumNeighbors[i] = int32_t( neighborlist[i].size() );

    // Set the vector for each list into the NeighborList Object
    NeighborList<int>::SharedVectorType sharedNeiLst(new std::vector<int>);
    sharedNeiLst->assign(neighborlist[i].begin(), neighborlist[i].end());
    m_NeighborList->setList(static_cast<int>(i), sharedNeiLst);

    NeighborList<float>::SharedVectorType sharedSAL(new std::vector<float>);
    sharedSAL->assign(neighborsurfacearealist[i].begin(), neighborsurfacearealist[i].end());
    m_SharedSurfaceAreaList->setList(static_cast<int>(i), sharedSAL);
  }

 notifyStatusMessage("Finding Neighbors Complete");
}

