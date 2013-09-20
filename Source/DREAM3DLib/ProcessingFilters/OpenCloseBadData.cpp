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

#include "OpenCloseBadData.h"


#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"



#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OpenCloseBadData::OpenCloseBadData() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_Direction(0),
m_NumIterations(1),
m_XDirOn(true),
m_YDirOn(true),
m_ZDirOn(true),
m_Neighbors(NULL),
m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OpenCloseBadData::~OpenCloseBadData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Operation");
    option->setPropertyName("Direction");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Dilate");
    choices.push_back("Erode");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Number of Iterations");
    option->setPropertyName("NumIterations");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Direction");
    option->setPropertyName("XDirOn");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Direction");
    option->setPropertyName("YDirOn");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Direction");
    option->setPropertyName("ZDirOn");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setDirection( reader->readValue("Direction", getDirection()) );
  setNumIterations( reader->readValue("NumIterations", getNumIterations()) );
  setXDirOn(reader->readValue("X Direction", getXDirOn()) );
  setYDirOn(reader->readValue("Y Direction", getYDirOn()) );
  setZDirOn(reader->readValue("Z Direction", getZDirOn()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int OpenCloseBadData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("Direction", getDirection() );
  writer->writeValue("NumIterations", getNumIterations() );
  writer->writeValue("X Direction", getXDirOn() );
  writer->writeValue("Y Direction", getYDirOn() );
  writer->writeValue("Z Direction", getZDirOn() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -301, int32_t, Int32ArrayType, voxels, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OpenCloseBadData::execute()
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
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
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
  size_t numgrains = m->getNumFieldTuples();

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);
  std::vector<int> currentvlist;

  size_t count = 0;
  int kstride, jstride;
  int grainname, grain;

  int current;
  int most;

  std::vector<int > n(numgrains + 1,0);
  for (int iteration = 0; iteration < m_NumIterations; iteration++)
  {
    for (int k = 0; k < dims[2]; k++)
    {
      kstride = static_cast<int>( dims[0]*dims[1]*k );
      for (int j = 0; j < dims[1]; j++)
      {
        jstride = static_cast<int>( dims[0]*j );
        for (int i = 0; i < dims[0]; i++)
        {
          count = kstride+jstride+i;
          
          grainname = m_GrainIds[count];
          if (grainname == 0)
          {
            current = 0;
            most = 0;
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = static_cast<int>( count + neighpoints[l] );
              if (l == 0 && (k == 0 || m_ZDirOn == false)) good = 0;
              else if (l == 5 && (k == (dims[2] - 1) || m_ZDirOn == false)) good = 0;
              else if (l == 1 && (j == 0 || m_YDirOn == false)) good = 0;
              else if (l == 4 && (j == (dims[1] - 1) || m_YDirOn == false)) good = 0;
              else if (l == 2 && (i == 0 || m_XDirOn == false)) good = 0;
              else if (l == 3 && (i == (dims[0] - 1) || m_XDirOn == false)) good = 0;
              if (good == 1)
              {
                grain = m_GrainIds[neighpoint];
                if (m_Direction == 0 && grain > 0)
                {
                  m_Neighbors[neighpoint] = count;
                }
                if ((grain > 0 && m_Direction == 1))
                {
                  n[grain]++;
                  current = n[grain];
                  if (current > most)
                  {
                    most = current;
                    m_Neighbors[count] = neighpoint;
                  }
                }
              }
            }
            if (m_Direction == 1)
            {
              for (int l = 0; l < 6; l++)
              {
                good = 1;
                neighpoint = static_cast<int>( count + neighpoints[l] );
                if (l == 0 && k == 0) good = 0;
                if (l == 5 && k == (dims[2] - 1)) good = 0;
                if (l == 1 && j == 0) good = 0;
                if (l == 4 && j == (dims[1] - 1)) good = 0;
                if (l == 2 && i == 0) good = 0;
                if (l == 3 && i == (dims[0] - 1)) good = 0;
                if (good == 1)
                {
                  grain = m_GrainIds[neighpoint];
                  n[grain] = 0;
                }
              }
            }
          }
        }
      }
    }
    QList<QString> voxelArrayNames = m->getCellArrayNameList();
    for (size_t j = 0; j < totalPoints; j++)
    {
      int grainname = m_GrainIds[j];
      int neighbor = m_Neighbors[j];
      if (neighbor >= 0)
      {
        if ( (grainname == 0 && m_GrainIds[neighbor] > 0 && m_Direction == 1)
          || (grainname > 0 && m_GrainIds[neighbor] == 0 && m_Direction == 0))
        {
          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            QString name = *iter;
            IDataArray::Pointer p = m->getCellData(*iter);
            p->CopyTuple(neighbor, j);
          }
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Complete");
}
