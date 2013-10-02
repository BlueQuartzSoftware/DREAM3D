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

#include "IdentifySample.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"



#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifySample::IdentifySample() :
  AbstractFilter(),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_AlreadyChecked(NULL),
  m_Neighbors(NULL),
  m_GoodVoxels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifySample::~IdentifySample()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::setupFilterParameters()
{

}
// -----------------------------------------------------------------------------
void IdentifySample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int IdentifySample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -301, bool, BoolArrayType, voxels, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::execute()
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
  DimType xp = dims[0];
  DimType yp = dims[1];
  DimType zp = dims[2];

  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  std::vector<int> currentvlist;
  std::vector<bool> checked;
  checked.resize(totalPoints,false);
  std::vector<bool> Sample;
  Sample.resize(totalPoints,false);
  std::vector<bool> notSample;
  notSample.resize(totalPoints,false);
  int biggestBlock = 0;
  size_t count;
  int good;
  int neighbor;
  DimType column, row, plane;
  int index;

  for (int i = 0; i < totalPoints; i++)
  {
    if(checked[i] == false && m_GoodVoxels[i] == false)
    {
      currentvlist.push_back(i);
      count = 0;
      while (count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = static_cast<int>( index + neighpoints[j] );
          if(j == 0 && plane == 0) good = 0;
          if(j == 5 && plane == (zp - 1)) good = 0;
          if(j == 1 && row == 0) good = 0;
          if(j == 4 && row == (yp - 1)) good = 0;
          if(j == 2 && column == 0) good = 0;
          if(j == 3 && column == (xp - 1)) good = 0;
          if(good == 1 && checked[neighbor] == false && m_GoodVoxels[neighbor] == false)
          {
            currentvlist.push_back(neighbor);
            checked[neighbor] = true;
          }
        }
        count++;
      }
      if(static_cast<int>(currentvlist.size()) >= biggestBlock)
      {
        biggestBlock = currentvlist.size();
        for(int j = 0; j < totalPoints; j++)
        {
          notSample[j] = false;
        }
        for(size_t j = 0; j < currentvlist.size(); j++)
        {
          notSample[currentvlist[j]] = true;
        }
      }
      currentvlist.clear();
    }
  }
  for (int i = 0; i < totalPoints; i++)
  {
    if (notSample[i] == false && m_GoodVoxels[i] == false) m_GoodVoxels[i] = true;
    else if (notSample[i] == true && m_GoodVoxels[i] == true) m_GoodVoxels[i] = false;
  }
  notSample.clear();
  checked.clear();

  biggestBlock = 0;
  checked.resize(totalPoints,false);
  for (int i = 0; i < totalPoints; i++)
  {
    if(checked[i] == false && m_GoodVoxels[i] == true)
    {
      currentvlist.push_back(i);
      count = 0;
      while (count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = static_cast<int>( index + neighpoints[j] );
          if(j == 0 && plane == 0) good = 0;
          if(j == 5 && plane == (zp - 1)) good = 0;
          if(j == 1 && row == 0) good = 0;
          if(j == 4 && row == (yp - 1)) good = 0;
          if(j == 2 && column == 0) good = 0;
          if(j == 3 && column == (xp - 1)) good = 0;
          if(good == 1 && checked[neighbor] == false && m_GoodVoxels[neighbor] == true)
          {
            currentvlist.push_back(neighbor);
            checked[neighbor] = true;
          }
        }
        count++;
      }
      if(static_cast<int>(currentvlist.size()) >= biggestBlock)
      {
        biggestBlock = currentvlist.size();
        for(int j = 0; j < totalPoints; j++)
        {
          Sample[j] = false;
        }
        for(size_t j = 0; j < currentvlist.size(); j++)
        {
          Sample[currentvlist[j]] = true;
        }
      }
      currentvlist.clear();
    }
  }
  for (int i = 0; i < totalPoints; i++)
  {
    if (Sample[i] == false && m_GoodVoxels[i] == true) m_GoodVoxels[i] = false;
    else if (Sample[i] == true && m_GoodVoxels[i] == false) m_GoodVoxels[i] = true;
  }
  Sample.clear();
  checked.clear();

  /*  std::vector<bool> change;
  change.resize(totalPoints,false);
  for (int i = 0; i < totalPoints; i++)
  {
    if(m_GoodVoxels[i] == false)
    {
        column = i % xp;
        row = (i / xp) % yp;
        plane = i / (xp * yp);
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = static_cast<int>( i + neighpoints[j] );
          if(j == 0 && plane == 0) good = 0;
          if(j == 5 && plane == (zp - 1)) good = 0;
          if(j == 1 && row == 0) good = 0;
          if(j == 4 && row == (yp - 1)) good = 0;
          if(j == 2 && column == 0) good = 0;
          if(j == 3 && column == (xp - 1)) good = 0;
          if(good == 1 && m_GoodVoxels[neighbor] == true)
          {
            change[i] = true;
          }
        }
  }
  }
  for(int j = 0; j < totalPoints; j++)
  {
  if(change[j] == true) m_GoodVoxels[j] = true;
  }
  change.clear();
  */
  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Identifying Sample Complete");
}
