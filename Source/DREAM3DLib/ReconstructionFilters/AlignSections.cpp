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

#include "AlignSections.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/DataArray.hpp"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/PrivateFilters/FindCellQuats.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

using namespace std;

const static float m_pi = M_PI;

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSections::AlignSections() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_AlignmentMethod(DREAM3D::AlignmentMethod::UnknownAlignmentMethod),
m_MisorientationTolerance(5.0f),
m_GrainIds(NULL),
m_Quats(NULL),
m_PhasesC(NULL),
m_GoodVoxels(NULL),
m_CrystalStructures(NULL)
{
  Seed = MXA::getMilliSeconds();

  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());


  graincounts = NULL;
  INIT_DataArray(m_GrainCounts, int);
  setupFilterOptions();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSections::~AlignSections()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    ChoiceFilterOption::Pointer option = ChoiceFilterOption::New();
    option->setHumanLabel("Alignment Method");
    option->setPropertyName("AlignmentMethod");
    option->setWidgetType(FilterOption::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Outer Boundary");
    choices.push_back("Misorientation");
    choices.push_back("Mutual Information");
    option->setChoices(choices);
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Misorientation Tolerance");
    option->setPropertyName("MisorientationTolerance");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }
  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

//  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, -1, voxels, 1);
  CREATE_NON_PREREQ_DATA_TEST(m, m_GrainIdsArrayName, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1);

  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -301, float, FloatArrayType, voxels, 5);
  if(getErrorCondition() == -301)
  {
	setErrorCondition(0);
	FindCellQuats::Pointer find_cellquats = FindCellQuats::New();
	find_cellquats->setObservers(this->getObservers());
	find_cellquats->setDataContainer(getDataContainer());
	if(preflight == true) find_cellquats->preflight();
	if(preflight == false) find_cellquats->execute();
	GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -301, float, FloatArrayType, voxels, 5);
  }
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, CellData, Phases, C, ss, -302,  int32_t, Int32ArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -303, bool, BoolArrayType, voxels, 1);

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1);

  if (m_AlignmentMethod >= DREAM3D::AlignmentMethod::Count)
  {
    ss << "Filter " << getNameOfClass() << " requires the Alignment Method to be set correctly and it is not." << std::endl;
    ss << "The allowable values fall between 0 and " << DREAM3D::AlignmentMethod::Count-1 << " (inclusive). The current value is "
        << m_AlignmentMethod << std::endl;
     setErrorCondition(-366);
  }
  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::execute()
{
  setErrorCondition(0);
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }


  if(m_AlignmentMethod == DREAM3D::AlignmentMethod::MutualInformation)
  {
    form_grains_sections();
  }

  align_sections();
  if (getErrorCondition() < 0 )
  {
    return;
  }

  // If there is an error set this to something negative and also set a message
  notify("Aligning Sections Complete", 0, Observable::UpdateProgressMessage);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::align_sections()
{
  DataContainer* m = getDataContainer();
  //int64_t totalPoints = m->totalPoints();

  ofstream outFile;
  string filename = "aligntest.txt";
  outFile.open(filename.c_str());

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

  float disorientation = 0;
  float mindisorientation = 100000000;
  float **mutualinfo12 = NULL;
  float *mutualinfo1 = NULL;
  float *mutualinfo2 = NULL;
  int graincount1, graincount2;
  int newxshift = 0;
  int newyshift = 0;
  int oldxshift = 0;
  int oldyshift = 0;
  float count = 0;
  int slice = 0;
  int xspot, yspot;
  float w;
  float n1, n2, n3;
  float q1[5];
  float q2[5];
  int refgnum, curgnum;
  int refposition = 0;
  int curposition = 0;
  DimType newPosition;
  DimType currentPosition;
  unsigned int phase1, phase2;

  int** shifts = AlignSections::Allocate2DArray<int>(dims[2], 2);
  for (DimType a = 0; a < dims[2]; a++)
  {
    for (int b = 0; b < 2; b++)
    {
      shifts[a][b] = 0;
    }
  }

  float** misorients = AlignSections::Allocate2DArray<float>(dims[0], dims[1]);
 // int** misorients = new int *[dims[0]];
  for (DimType a = 0; a < dims[0]; a++)
  {
//    misorients[a] = new int[dims[1]];
    for (DimType b = 0; b < dims[1]; b++)
    {
      misorients[a][b] = 0;
    }
  }
  for (DimType iter = 1; iter < dims[2]; iter++)
  {
    std::stringstream ss;
    ss << "Aligning Sections - Determining Shifts - " << ((float)iter/dims[2])*100 << " Percent Complete";
  //  notify(ss.str(), 0, Observable::UpdateProgressMessage);
    mindisorientation = 100000000;
    slice = (dims[2] - 1) - iter;
    if(m_AlignmentMethod == DREAM3D::AlignmentMethod::MutualInformation)
    {
      graincount1 = graincounts[slice];
      graincount2 = graincounts[slice + 1];
      mutualinfo12 = AlignSections::Allocate2DArray<float>(graincount1, graincount2);
//      mutualinfo12 = new float *[graincount1];
      mutualinfo1 = new float[graincount1];
      mutualinfo2 = new float[graincount2];

      for (int a = 0; a < graincount1; a++)
      {
        mutualinfo1[a] = 0.0f;
      //  mutualinfo12[a] = new float[graincount2];
        for (int b = 0; b < graincount2; b++)
        {
          mutualinfo12[a][b] = 0.0f;
          mutualinfo2[b] = 0.0f;
        }
      }
    }
    oldxshift = -1;
    oldyshift = -1;
    newxshift = 0;
    newyshift = 0;
    for (DimType a = 0; a < dims[0]; a++)
    {
      for (DimType b = 0; b < dims[1]; b++)
      {
        misorients[a][b] = 0;
      }
    }
    while (newxshift != oldxshift || newyshift != oldyshift)
    {
      oldxshift = newxshift;
      oldyshift = newyshift;
      for (int j = -3; j < 4; j++)
      {
        for (int k = -3; k < 4; k++)
        {
          disorientation = 0;
          count = 0;
          if(misorients[k + oldxshift + size_t(dims[0] / 2)][j + oldyshift + (size_t)(dims[1] / 2)] == 0 && abs(k + oldxshift) < (dims[0] / 2)
              && (j + oldyshift) < (dims[1] / 2))
          {
            for (DimType l = 0; l < dims[1]; l = l + 4)
            {
              for (DimType n = 0; n < dims[0]; n = n + 4)
              {
                if((l + j + oldyshift) >= 0 && (l + j + oldyshift) < dims[1] && (n + k + oldxshift) >= 0 && (n + k + oldxshift) < dims[0])
                {
                  refposition = ((slice + 1) * dims[0] * dims[1]) + (l * dims[0]) + n;
                  curposition = (slice * dims[0] * dims[1]) + ((l + j + oldyshift) * dims[0]) + (n + k + oldxshift);
                  if(m_AlignmentMethod == DREAM3D::AlignmentMethod::MutualInformation)
                  {
                    refgnum = m_GrainIds[refposition];
                    curgnum = m_GrainIds[curposition];
                    if(curgnum >= 0 && refgnum >= 0)
                    {
                      mutualinfo12[curgnum][refgnum]++;
                      mutualinfo1[curgnum]++;
                      mutualinfo2[refgnum]++;
	                  count++;
                    }
                  }
                  else if(m_AlignmentMethod == DREAM3D::AlignmentMethod::Misorientation)
                  {
                    if(m_GoodVoxels[refposition] == true && m_GoodVoxels[curposition] == true)
                    {
                      w = 10000.0;
	                  count++;
                      if(m_PhasesC[refposition] > 0 && m_PhasesC[curposition] > 0)
                      {
                        q1[1] = m_Quats[refposition * 5 + 1];
                        q1[2] = m_Quats[refposition * 5 + 2];
                        q1[3] = m_Quats[refposition * 5 + 3];
                        q1[4] = m_Quats[refposition * 5 + 4];
                        phase1 = m_CrystalStructures[m_PhasesC[refposition]];
                        q2[1] = m_Quats[curposition * 5 + 1];
                        q2[2] = m_Quats[curposition * 5 + 2];
                        q2[3] = m_Quats[curposition * 5 + 3];
                        q2[4] = m_Quats[curposition * 5 + 4];
                        phase2 = m_CrystalStructures[m_PhasesC[curposition]];
                        if(phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
                      }
                      if(w > m_MisorientationTolerance) disorientation++;
                    }
                    if(m_GoodVoxels[refposition] == true && m_GoodVoxels[curposition] == false) disorientation++;
                    if(m_GoodVoxels[refposition] == false && m_GoodVoxels[curposition] == true) disorientation++;
                  }
                  else if(m_AlignmentMethod == DREAM3D::AlignmentMethod::OuterBoundary)
                  {
                    if(m_GoodVoxels[refposition] != m_GoodVoxels[curposition]) disorientation++;
	                count++;
                  }
                }
                else
                {
                  if(m_AlignmentMethod == DREAM3D::AlignmentMethod::MutualInformation)
                  {
                    mutualinfo12[0][0]++;
                    mutualinfo1[0]++;
                    mutualinfo2[0]++;
                  }
                }
              }
            }
            if(m_AlignmentMethod == DREAM3D::AlignmentMethod::MutualInformation)
            {
              float ha = 0;
              float hb = 0;
              float hab = 0;
              for (int b = 0; b < graincount1; b++)
              {
                mutualinfo1[b] = mutualinfo1[b] / float(count);
                if(mutualinfo1[b] != 0) ha = ha + mutualinfo1[b] * log(mutualinfo1[b]);
              }
              for (int c = 0; c < graincount2; c++)
              {
                mutualinfo2[c] = mutualinfo2[c] / float(count);
                if(mutualinfo2[c] != 0) hb = hb + mutualinfo2[c] * log(mutualinfo2[c]);
              }
              for (int b = 0; b < graincount1; b++)
              {
                for (int c = 0; c < graincount2; c++)
                {
                  mutualinfo12[b][c] = mutualinfo12[b][c] / float(count);
                  if(mutualinfo12[b][c] != 0) hab = hab + mutualinfo12[b][c] * log(mutualinfo12[b][c]);
                  float value = 0;
                  if(mutualinfo1[b] > 0 && mutualinfo2[c] > 0) value = (mutualinfo12[b][c] / (mutualinfo1[b] * mutualinfo2[c]));
                  if(value != 0) disorientation = disorientation + (mutualinfo12[b][c] * log(value));
                }
              }
              for (int b = 0; b < graincount1; b++)
              {
                for (int c = 0; c < graincount2; c++)
                {
                  mutualinfo12[b][c] = 0;
                  mutualinfo1[b] = 0;
                  mutualinfo2[c] = 0;
                }
              }
              disorientation = 1.0 / disorientation;
            }
            if(m_AlignmentMethod == DREAM3D::AlignmentMethod::OuterBoundary || m_AlignmentMethod == DREAM3D::AlignmentMethod::Misorientation) disorientation = disorientation/count;
            misorients[k + oldxshift + int(dims[0] / 2)][j + oldyshift + int(dims[1] / 2)] = disorientation;
            if(disorientation < mindisorientation)
            {
              newxshift = k + oldxshift;
              newyshift = j + oldyshift;
              mindisorientation = disorientation;
            }
          }
        }
      }
    }
    shifts[iter][0] = shifts[iter-1][0] + newxshift;
    shifts[iter][1] = shifts[iter-1][1] + newyshift;
	outFile << slice << "	" << slice+1 << "	" << newxshift << "	" << newyshift << "	" << shifts[iter][0] << "	" << shifts[iter][1] << endl;
    if(m_AlignmentMethod == DREAM3D::AlignmentMethod::MutualInformation)
    {
      AlignSections::Deallocate2DArray<float>(graincount1, graincount2, mutualinfo12);
      delete [] mutualinfo1;
      delete [] mutualinfo2;
      mutualinfo1 = NULL;
      mutualinfo2 = NULL;
      mutualinfo12 = NULL;
    }
  }
  std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
  for (DimType iter = 1; iter < dims[2]; iter++)
  {
    std::stringstream ss;
    ss << "Aligning Sections - Transferring Cell Data - " << ((float)iter/dims[2])*100 << " Percent Complete";
   // notify(ss.str(), 0, Observable::UpdateProgressMessage);
    slice = (dims[2] - 1) - iter;
    for (DimType l = 0; l < dims[1]; l++)
    {
      for (DimType n = 0; n < dims[0]; n++)
      {
        if(shifts[iter][1] >= 0) yspot = l;
        if(shifts[iter][0] >= 0) xspot = n;
        if(shifts[iter][1] < 0) yspot = dims[1] - 1 - l;
        if(shifts[iter][0] < 0) xspot = dims[0] - 1 - n;
        newPosition = (slice * dims[0] * dims[1]) + (yspot * dims[0]) + xspot;
        currentPosition = (slice * dims[0] * dims[1]) + ((yspot + shifts[iter][1]) * dims[0]) + (xspot + shifts[iter][0]);
        if((yspot + shifts[iter][1]) >= 0 && (yspot + shifts[iter][1]) <= dims[1] - 1 && (xspot + shifts[iter][0]) >= 0
            && (xspot + shifts[iter][0]) <= dims[0] - 1)
        {
          for(std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            std::string name = *iter;
            IDataArray::Pointer p = m->getCellData(*iter);
        	  p->CopyTuple(currentPosition, newPosition);
          }
        }
        if((yspot + shifts[iter][1]) < 0 || (yspot + shifts[iter][1]) > dims[1] - 1 || (xspot + shifts[iter][0]) < 0
            || (xspot + shifts[iter][0]) > dims[0] - 1)
        {
          m_Quats[newPosition * 5 + 0] = 0.0;
          m_Quats[newPosition * 5 + 1] = 0.0;
          m_Quats[newPosition * 5 + 2] = 0.0;
          m_Quats[newPosition * 5 + 3] = 0.0;
          m_Quats[newPosition * 5 + 4] = 1.0;
          m_GoodVoxels[newPosition] = false;
          m_PhasesC[newPosition] = 0;
        }
      }
    }
  }


#if 1
  Deallocate2DArray(dims[2], 2, shifts);
  Deallocate2DArray(dims[0], dims[1], misorients);

#else
  // Clean up the memory
  for (int a = 0; a < dims[2]; a++)
  {
    delete[] shifts[a];
  }
  for (int a = 0; a < dims[0]; a++)
  {
    delete[] misorients[a];
  }
  delete[] shifts;
  delete[] misorients;
#endif
  outFile.close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::form_grains_sections()
{
  DREAM3D_RANDOMNG_NEW()
  DataContainer* m = getDataContainer();

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

 // int64_t totalPoints = m->totalPoints();


  int point = 0;
  int seed = 0;
  int noseeds = 0;
  int graincount = 1;
  int neighbor;
  // int currentpoint;
  float q1[5];
  float q2[5];
//  float qs[5];
  float w;
  float n1;
  float n2;
  float n3;
  DimType randx = 0;
  DimType randy = 0;
  DimType good = 0;
  DimType x, y, z;
  DimType col, row;
  size_t size = 0;
  size_t initialVoxelsListSize = 1000;

  graincounts = m_GrainCounts->WritePointer(0, dims[2]);

  std::vector<int> voxelslist(initialVoxelsListSize, -1);
  DimType neighpoints[8];
  neighpoints[0] = -dims[0] - 1;
  neighpoints[1] = -dims[0];
  neighpoints[2] = -dims[0] + 1;
  neighpoints[3] = -1;
  neighpoints[4] = 1;
  neighpoints[5] = dims[0] - 1;
  neighpoints[6] = dims[0];
  neighpoints[7] = dims[0] + 1;
  unsigned int phase1, phase2;
  for (DimType slice = 0; slice < dims[2]; slice++)
  {
    std::stringstream ss;
    ss << "Aligning Sections - Identifying Grains on Sections - " << ((float)slice/dims[2])*100 << " Percent Complete";
 //   notify(ss.str(), 0, Observable::UpdateProgressMessage);
    graincount = 1;
    noseeds = 0;
    while (noseeds == 0)
    {
      seed = -1;
      randx = int(float(rg.genrand_res53()) * float(dims[0]));
      randy = int(float(rg.genrand_res53()) * float(dims[1]));
      for (DimType j = 0; j < dims[1]; ++j)
      {
        for (DimType i = 0; i < dims[0]; ++i)
        {
          x = randx + i;
          y = randy + j;
          z = slice;
          if(x > dims[0] - 1) x = x - dims[0];
          if(y > dims[1] - 1) y = y - dims[1];
          point = (z * dims[0] * dims[1]) + (y * dims[0]) + x;
          if(m_GoodVoxels[point] == true && m_GrainIds[point] == -1 && m_PhasesC[point] > 0)
          {
            seed = point;
          }
          if(seed > -1) break;
        }
        if(seed > -1) break;
      }
      if(seed == -1) noseeds = 1;
      if(seed >= 0)
      {
        size = 0;
        m_GrainIds[seed] = graincount;
        voxelslist[size] = seed;
        size++;
//        qs[0] = 0;
//        qs[1] = m_Quats[seed * 5 + 1];
//        qs[2] = m_Quats[seed * 5 + 2];
//        qs[3] = m_Quats[seed * 5 + 3];
//        qs[4] = m_Quats[seed * 5 + 4];
        for (size_t j = 0; j < size; ++j)
        {
          int currentpoint = voxelslist[j];
          col = currentpoint % dims[0];
          row = (currentpoint / dims[0]) % dims[1];
          q1[0] = 0;
          q1[1] = m_Quats[currentpoint * 5 + 1];
          q1[2] = m_Quats[currentpoint * 5 + 2];
          q1[3] = m_Quats[currentpoint * 5 + 3];
          q1[4] = m_Quats[currentpoint * 5 + 4];
          phase1 = m_CrystalStructures[m_PhasesC[currentpoint]];
          for (int i = 0; i < 8; i++)
          {
            good = 1;
            neighbor = currentpoint + neighpoints[i];
            if((i == 0 || i == 1 || i == 2) && row == 0) good = 0;
            if((i == 5 || i == 6 || i == 7) && row == (dims[1] - 1)) good = 0;
            if((i == 0 || i == 3 || i == 5) && col == 0) good = 0;
            if((i == 2 || i == 4 || i == 7) && col == (dims[0] - 1)) good = 0;
            if(good == 1 && m_GrainIds[neighbor] <= 0 && m_PhasesC[neighbor] > 0)
            {
              w = 10000.0;
              q2[0] = 0;
              q2[1] = m_Quats[neighbor * 5 + 1];
              q2[2] = m_Quats[neighbor * 5 + 2];
              q2[3] = m_Quats[neighbor * 5 + 3];
              q2[4] = m_Quats[neighbor * 5 + 4];
              phase2 = m_CrystalStructures[m_PhasesC[neighbor]];
              if(phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
              if(w < m_MisorientationTolerance)
              {
                m_GrainIds[neighbor] = graincount;
                voxelslist[size] = neighbor;
                size++;
                if(size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
              }
            }
          }
        }
        voxelslist.erase(std::remove(voxelslist.begin(), voxelslist.end(), -1), voxelslist.end());
        graincount++;
        voxelslist.clear();
        voxelslist.resize(initialVoxelsListSize, -1);
      }
    }
    graincounts[slice] = graincount;
  }
}
