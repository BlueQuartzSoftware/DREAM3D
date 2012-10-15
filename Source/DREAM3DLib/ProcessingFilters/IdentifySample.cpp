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
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

const static float m_pi = static_cast<float>(M_PI);

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
void IdentifySample::writeFilterParameters(AbstractFilterParametersWriter* writer)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

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
  VoxelDataContainer* m = getVoxelDataContainer();
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

  BoolArrayType::Pointer alreadCheckedPtr = BoolArrayType::CreateArray(totalPoints, "AlreadyChecked");
  m_AlreadyChecked = alreadCheckedPtr->GetPointer(0);
  alreadCheckedPtr->initializeWithZeros();

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

  std::vector<int > neighs;
  std::vector<int > remove;
//  size_t count = 1;
//  int good = 1;
//  int neighbor;
//  int index = 0;
//  float x, y, z;
//  DimType column, row, plane;
//  int neighpoint;
//  size_t numgrains = m->getNumFieldTuples();

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);
  std::vector<int> currentvlist;

/*  std::vector<int > n(numgrains + 1);
  for (size_t iteration = 0; iteration < m_NumIterations; iteration++)
  {
    for (int i = 0; i < totalPoints; i++)
    {
	  std::stringstream ss;
//	  ss << "Cleaning Up Grains - Removing Bad Points - Cycle " << count << " - " << ((float)i/totalPoints)*100 << "Percent Complete";
//	  notifyStatusMessage(ss.str());
      int grainname = m_GrainIds[i];
      if ((grainname == 0 && m_Direction == 1) || (grainname > 0 && m_Direction == 0))
      {
        for (size_t c = 1; c < numgrains; c++)
        {
          n[c] = 0;
        }
        x = static_cast<float>(i % dims[0]);
        y = static_cast<float>((i / dims[0]) % dims[1]);
        z = static_cast<float>(i / (dims[0] * dims[1]));
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = i + neighpoints[j];
          if (j == 0 && z == 0) good = 0;
          if (j == 5 && z == (dims[2] - 1)) good = 0;
          if (j == 1 && y == 0) good = 0;
          if (j == 4 && y == (dims[1] - 1)) good = 0;
          if (j == 2 && x == 0) good = 0;
          if (j == 3 && x == (dims[0] - 1)) good = 0;
          if (good == 1)
          {
            int grain = m_GrainIds[neighpoint];
			if ((grain > 0 && m_Direction == 1) || (grain == 0 && m_Direction == 0))
            {
              neighs.push_back(grain);
            }
          }
        }
        int current = 0;
        int most = 0;
        int curgrain = 0;
        int size = int(neighs.size());
        for (int k = 0; k < size; k++)
        {
          int neighbor = neighs[k];
          n[neighbor]++;
          current = n[neighbor];
          if (current > most)
          {
            most = current;
            curgrain = neighbor;
          }
        }
        if (size > 0)
        {
          m_Neighbors[i] = curgrain;
          neighs.clear();
        }
      }
    }
    for (int j = 0; j < totalPoints; j++)
    {
      int grainname = m_GrainIds[j];
      int neighbor = m_Neighbors[j];
//	  if ((grain > 0 && m_Direction == 1) || (grain == 0 && m_Direction == 0))
      if ((grainname == 0 && neighbor > 0 && m_Direction == 1) || (grainname > 0 && neighbor == 0 && m_Direction == 0))
      {
        m_GrainIds[j] = neighbor;
		    m_CellPhases[j] = m_FieldPhases[neighbor];
      }
    }
//    std::stringstream ss;
//     ss << "Assigning Bad Voxels count = " << count;
//    notify(ss.str().c_str(), 0, Observable::UpdateProgressMessage);
  }
*/
  // If there is an error set this to something negative and also set a message
 notifyStatusMessage("Identifying Sample Complete");
}
