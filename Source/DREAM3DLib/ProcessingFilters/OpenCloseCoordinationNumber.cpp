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

#include "OpenCloseCoordinationNumber.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"



#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OpenCloseCoordinationNumber::OpenCloseCoordinationNumber() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_Loop(false),
m_CoordinationNumber(6),
m_Neighbors(NULL),
m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OpenCloseCoordinationNumber::~OpenCloseCoordinationNumber()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseCoordinationNumber::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Coordination Number to Consider");
    option->setPropertyName("CoordinationNumber");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Loop Until Gone");
    option->setPropertyName("Loop");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseCoordinationNumber::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setCoordinationNumber( reader->readValue("CoordinationNumber", getCoordinationNumber()) );
  setLoop( reader->readValue("Loop", false) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int OpenCloseCoordinationNumber::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("CoordinationNumber", getCoordinationNumber() );
  writer->writeValue("Loop", getLoop() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseCoordinationNumber::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -301, int32_t, Int32ArrayType, voxels, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseCoordinationNumber::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseCoordinationNumber::execute()
{
  setErrorCondition(0);
 // int err = 0;
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }


  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0 && getErrorCondition() != -305)
  {
    return;
  }
  setErrorCondition(0);

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->GetPointer(0);
  neighborsPtr->initializeWithValues(-1);

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

//  size_t count = 1;
  int good = 1;
//  int neighbor;
//  int index = 0;
//  float x, y, z;
//  DimType row, plane;
  int neighpoint;
  size_t numgrains = m->getNumCellFieldTuples();
  if (numgrains == 0)
  {
    setErrorCondition(-90001);
    notifyErrorMessage("No grains have been defined in the Field map. A filter needs to be executed before this filter that defines the number of grains.", getErrorCondition());
    notifyStatusMessage("Completed with Errors");
    return;
  }

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);
 // QVector<int> currentvlist;

  size_t point = 0;
  int kstride, jstride;
  int grainname, grain;
  int coordination = 0;
  int current = 0;
  int most = 0;

  std::list<std::string> voxelArrayNames = m->getCellArrayNameList();

  QVector<int > n(numgrains + 1,0);

  QVector<int > coordinationNumber(totalPoints,0);
  bool keepgoing = true;
  int counter = 1;
  while (counter > 0 && keepgoing == true)
  {
    counter = 0;
    if(m_Loop == false) keepgoing = false;

    for (int k = 0; k < dims[2]; k++)
    {
      kstride = static_cast<int>( dims[0]*dims[1]*k );
      for (int j = 0; j < dims[1]; j++)
      {
        jstride = static_cast<int>( dims[0]*j );
        for (int i = 0; i < dims[0]; i++)
        {
          point = kstride+jstride+i;
          grainname = m_GrainIds[point];
      coordination = 0;
      current = 0;
      most = 0;
      for (int l = 0; l < 6; l++)
          {
            good = 1;
            neighpoint = static_cast<int>( point + neighpoints[l] );
            if (l == 0 && k == 0) good = 0;
            if (l == 5 && k == (dims[2] - 1)) good = 0;
            if (l == 1 && j == 0) good = 0;
            if (l == 4 && j == (dims[1] - 1)) good = 0;
            if (l == 2 && i == 0) good = 0;
            if (l == 3 && i == (dims[0] - 1)) good = 0;
            if (good == 1)
            {
              grain = m_GrainIds[neighpoint];
              if((grainname > 0 && grain == 0) || (grainname == 0 && grain > 0))
              {
                coordination = coordination + 1;
        n[grain]++;
        current = n[grain];
        if (current > most)
        {
          most = current;
          m_Neighbors[point] = neighpoint;
        }
              }
            }
          }
          coordinationNumber[point] = coordination;
        int neighbor = m_Neighbors[point];
        if (coordinationNumber[point] >= m_CoordinationNumber && coordinationNumber[point] > 0)
        {
            for(std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
            {
              QString name = *iter;
              IDataArray::Pointer p = m->getCellData(*iter);
              p->CopyTuple(neighbor, point);
            }
      }
      for (int l = 0; l < 6; l++)
      {
            good = 1;
            neighpoint = static_cast<int>( point + neighpoints[l] );
            if (l == 0 && k == 0) good = 0;
            if (l == 5 && k == (dims[2] - 1)) good = 0;
            if (l == 1 && j == 0) good = 0;
            if (l == 4 && j == (dims[1] - 1)) good = 0;
            if (l == 2 && i == 0) good = 0;
            if (l == 3 && i == (dims[0] - 1)) good = 0;
            if (good == 1)
        {
        grain = m_GrainIds[neighpoint];
        if(grain > 0) n[grain] = 0;
        }
      }
    }
      }
    }
    for (int k = 0; k < dims[2]; k++)
    {
      kstride = static_cast<int>( dims[0]*dims[1]*k );
      for (int j = 0; j < dims[1]; j++)
      {
        jstride = static_cast<int>( dims[0]*j );
        for (int i = 0; i < dims[0]; i++)
        {
          point = kstride+jstride+i;
          if(coordinationNumber[point] >= m_CoordinationNumber)
          {
            counter++;
          }
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Complete");
}
