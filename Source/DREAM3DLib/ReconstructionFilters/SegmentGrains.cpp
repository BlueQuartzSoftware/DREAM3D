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

#include "SegmentGrains.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SegmentGrains::SegmentGrains() :
AbstractFilter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SegmentGrains::~SegmentGrains()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentGrains::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SegmentGrains::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentGrains::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentGrains::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentGrains::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  std::stringstream ss;
  if(NULL == m)
  {
    setErrorCondition(-1);
    ss << " DataContainer was NULL";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return;
  }
  // int64_t totalPoints = m->getTotalPoints();

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };

  size_t gnum = 1;
  int seed = 0;
  int neighbor;
  bool good = 0;
  DimType col, row, plane;
  size_t size = 0;
  size_t initialVoxelsListSize = 1000;
  std::vector<int> voxelslist(initialVoxelsListSize, -1);
  DimType neighpoints[6];
  neighpoints[0] = -(dims[0] * dims[1]);
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = (dims[0] * dims[1]);

  // Burn volume with tight orientation tolerance to simulate simultaneous growth/aglomeration
  while (seed >= 0)
  {
    seed = getSeed(gnum);
    if(seed >= 0)
    {
      size = 0;
      voxelslist[size] = seed;
      size++;
      for (size_t j = 0; j < size; ++j)
      {
        size_t currentpoint = voxelslist[j];
        col = currentpoint % dims[0];
        row = (currentpoint / dims[0]) % dims[1];
        plane = currentpoint / (dims[0] * dims[1]);
        for (int i = 0; i < 6; i++)
        {
          good = true;
          neighbor = currentpoint + neighpoints[i];

          if(i == 0 && plane == 0) good = false;
          if(i == 5 && plane == (dims[2] - 1)) good = false;
          if(i == 1 && row == 0) good = false;
          if(i == 4 && row == (dims[1] - 1)) good = false;
          if(i == 2 && col == 0) good = false;
          if(i == 3 && col == (dims[0] - 1)) good = false;
          if(good == true)
          {
            if(determineGrouping(currentpoint, neighbor, gnum) == true)
            {
              voxelslist[size] = neighbor;
              size++;
              if(size >= voxelslist.size()) voxelslist.resize(size + size, -1);
            }
          }
        }
      }
      voxelslist.clear();
      voxelslist.resize(initialVoxelsListSize, -1);
      gnum++;
      ss.str("");
      ss << "Total Grains: " << gnum;
      if(gnum%100 == 0) notifyStatusMessage(ss.str());
    }
  }

  // If there is an error set this to something negative and also set a message
 notifyStatusMessage("Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SegmentGrains::getSeed(size_t gnum)
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SegmentGrains::determineGrouping(int referencepoint, int neighborpoint, size_t gnum)
{
  return false;
}
