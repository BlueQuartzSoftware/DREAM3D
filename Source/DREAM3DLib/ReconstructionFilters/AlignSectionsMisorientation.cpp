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

#include "AlignSectionsMisorientation.h"

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

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

using namespace std;

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsMisorientation::AlignSectionsMisorientation() :
AlignSections(),
m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_QuatsArrayName(DREAM3D::CellData::Quats),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_MisorientationTolerance(5.0f),
m_Quats(NULL),
m_CellPhases(NULL),
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

  setupFilterOptions();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsMisorientation::~AlignSectionsMisorientation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::setupFilterOptions()
{
  // Run the superclass first.
  //AlignSections::setupFilterOptions();
  // Now append our options
  std::vector<FilterOption::Pointer> options = getFilterOptions();
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
void AlignSectionsMisorientation::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  AlignSections::writeFilterOptions(writer);
  writer->writeValue("MisorientationTolerance", getMisorientationTolerance() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();


  if(true == getWriteAlignmentShifts() && getAlignmentShiftFileName().empty() == true)
  {
    ss << getNameOfClass() << ": The Alignment Shift file name must be set before executing this filter.";
    setErrorCondition(-1);
  }

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
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302,  int32_t, Int32ArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -303, bool, BoolArrayType, voxels, 1);

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1);

  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::execute()
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

  AlignSections::execute();

  // If there is an error set this to something negative and also set a message
  notify("Aligning Sections Complete", 0, Observable::UpdateProgressMessage);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::find_shifts(std::vector<int> &xshifts, std::vector<int> &yshifts)
{
  DataContainer* m = getDataContainer();
  //int64_t totalPoints = m->totalPoints();

  ofstream outFile;
  if (getWriteAlignmentShifts() == true) {
    outFile.open(getAlignmentShiftFileName().c_str());
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

  float disorientation = 0;
  float mindisorientation = 100000000;
  int newxshift = 0;
  int newyshift = 0;
  int oldxshift = 0;
  int oldyshift = 0;
  float count = 0;
  int slice = 0;
//  int xspot, yspot;
  float w;
  float n1, n2, n3;
  float q1[5];
  float q2[5];
  int refposition = 0;
  int curposition = 0;

  unsigned int phase1, phase2;

  std::vector<std::vector<float> >  misorients;
  misorients.resize(dims[0]);
  for (DimType a = 0; a < dims[0]; a++)
  {
	  misorients[a].resize(dims[1], 0.0);
  }
  for (DimType iter = 1; iter < dims[2]; iter++)
  {
    std::stringstream ss;
    ss << "Aligning Sections - Determining Shifts - " << ((float)iter/dims[2])*100 << " Percent Complete";
  //  notify(ss.str(), 0, Observable::UpdateProgressMessage);
    mindisorientation = 100000000;
    slice = (dims[2] - 1) - iter;
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
                  if(m_GoodVoxels[refposition] == true && m_GoodVoxels[curposition] == true)
                  {
                      w = 10000.0;
	                  count++;
                      if(m_CellPhases[refposition] > 0 && m_CellPhases[curposition] > 0)
                      {
                        q1[1] = m_Quats[refposition * 5 + 1];
                        q1[2] = m_Quats[refposition * 5 + 2];
                        q1[3] = m_Quats[refposition * 5 + 3];
                        q1[4] = m_Quats[refposition * 5 + 4];
                        phase1 = m_CrystalStructures[m_CellPhases[refposition]];
                        q2[1] = m_Quats[curposition * 5 + 1];
                        q2[2] = m_Quats[curposition * 5 + 2];
                        q2[3] = m_Quats[curposition * 5 + 3];
                        q2[4] = m_Quats[curposition * 5 + 4];
                        phase2 = m_CrystalStructures[m_CellPhases[curposition]];
                        if(phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
                      }
                      if(w > m_MisorientationTolerance) disorientation++;
                  }
                  if(m_GoodVoxels[refposition] == true && m_GoodVoxels[curposition] == false) disorientation++;
                  if(m_GoodVoxels[refposition] == false && m_GoodVoxels[curposition] == true) disorientation++;
                }
                else
                {

                }
              }
            }
            disorientation = disorientation/count;
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
    xshifts[iter] = xshifts[iter-1] + newxshift;
    yshifts[iter] = yshifts[iter-1] + newyshift;
    if (getWriteAlignmentShifts() == true) {
      outFile << slice << "	" << slice+1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << endl;
    }
  }
  if (getWriteAlignmentShifts() == true) {
  outFile.close();
  }
}
