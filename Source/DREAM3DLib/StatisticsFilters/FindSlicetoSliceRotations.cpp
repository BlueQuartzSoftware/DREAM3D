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

#include "FindSlicetoSliceRotations.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlicetoSliceRotations::FindSlicetoSliceRotations() :
AbstractFilter(),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
m_QuatsArrayName(DREAM3D::CellData::Quats),
m_CellPhases(NULL),
m_GoodVoxels(NULL),
m_Quats(NULL),
m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationMath::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlicetoSliceRotations::~FindSlicetoSliceRotations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Slice to Slice Rotations File");
    option->setPropertyName("SlicetoSliceRotationsFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSlicetoSliceRotations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SlicetoSliceRotationsFile", getSlicetoSliceRotationsFile() );
  writer->closeFilterGroup();
  return index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -300, int32_t, Int32ArrayType,  voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -300, bool, BoolArrayType,  voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -303, float, FloatArrayType, voxels, 5)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::preflight()
{
  dataCheck(true, 1,1,1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::execute()
{
  setErrorCondition(0);

  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  //int64_t totalPoints = m->getTotalPoints();

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  float q1[5];
  float q2[5];
//  int numVoxel; // number of voxels in the grain...
//  int numchecks; // number of voxels in the grain...
  bool good = false;

  float w, n1, n2, n3;
//  unsigned int phase1 = Ebsd::CrystalStructure::UnknownCrystalStructure;
//  unsigned int phase2 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType xPoints = static_cast<DimType>(udims[0]);
  DimType yPoints = static_cast<DimType>(udims[1]);
  DimType zPoints = static_cast<DimType>(udims[2]);

  size_t point;
  size_t inNeighbor = 0;
  size_t outNeighbor = 0;

  DimType neighpoints[6];
  neighpoints[0] = -(xPoints * yPoints);
  neighpoints[1] = -xPoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = (xPoints * yPoints);

  float inPlaneAngle, outPlaneAngle;
  float inPlaneAxisX, inPlaneAxisY, inPlaneAxisZ;
  float outPlaneAxisX, outPlaneAxisY, outPlaneAxisZ;
  int inPlaneCount, outPlaneCount;

  std::ofstream outFile;
  outFile.open(m_SlicetoSliceRotationsFile.c_str(), std::ios_base::binary);

  for (DimType plane = 0; plane < zPoints; plane++)
  {
    inPlaneAngle = 0;
    outPlaneAngle = 0;
    inPlaneAxisX = 0;
    outPlaneAxisX = 0;
    inPlaneAxisY = 0;
    outPlaneAxisY = 0;
    inPlaneAxisZ = 0;
    outPlaneAxisZ = 0;
    inPlaneCount = 0;
    outPlaneCount = 0;
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType col = 0; col < xPoints; col++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        q1[1] = m_Quats[point*5 + 1];
        q1[2] = m_Quats[point*5 + 2];
        q1[3] = m_Quats[point*5 + 3];
        q1[4] = m_Quats[point*5 + 4];
        if (plane < zPoints-1)
        {
            outNeighbor = point + neighpoints[5];
            if (m_CellPhases[point] == m_CellPhases[outNeighbor] && m_CellPhases[point] > 0 && m_GoodVoxels[point] == true && m_GoodVoxels[outNeighbor] == true)
            {
                q2[1] = m_Quats[outNeighbor*5 + 1];
                q2[2] = m_Quats[outNeighbor*5 + 2];
                q2[3] = m_Quats[outNeighbor*5 + 3];
                q2[4] = m_Quats[outNeighbor*5 + 4];
                w = m_OrientationOps[m_CrystalStructures[m_CellPhases[point]]]->getMisoQuat( q1, q2, n1, n2, n3);
                if(w < 5.0f*m_pi/180.0f)
                {
                    OrientationMath::changeAxisReferenceFrame(q1, n1, n2, n3);
                    outPlaneAngle = outPlaneAngle + w;
                    outPlaneAxisX = outPlaneAxisX + n1;
                    outPlaneAxisY = outPlaneAxisY + n2;
                    outPlaneAxisZ = outPlaneAxisZ + n3;
                    outPlaneCount++;
                }
            }
        }
        for(size_t i = 1; i < 5; i++)
        {
          good = true;
          inNeighbor = point + neighpoints[i];
          if(i == 1 && row == 0) good = false;
          if(i == 4 && row == (yPoints - 1)) good = false;
          if(i == 2 && col == 0) good = false;
          if(i == 3 && col == (xPoints - 1)) good = false;
          if(good == true && m_CellPhases[point] == m_CellPhases[inNeighbor] && m_CellPhases[point] > 0 && m_GoodVoxels[point] == true && m_GoodVoxels[inNeighbor] == true)
          {
             q2[1] = m_Quats[inNeighbor*5 + 1];
             q2[2] = m_Quats[inNeighbor*5 + 2];
             q2[3] = m_Quats[inNeighbor*5 + 3];
             q2[4] = m_Quats[inNeighbor*5 + 4];
             w = m_OrientationOps[m_CrystalStructures[m_CellPhases[point]]]->getMisoQuat( q1, q2, n1, n2, n3);
             if(w < 5.0f*m_pi/180.0f)
             {
                OrientationMath::changeAxisReferenceFrame(q1, n1, n2, n3);
                inPlaneAngle = inPlaneAngle + w;
                inPlaneAxisX = inPlaneAxisX + n1;
                inPlaneAxisY = inPlaneAxisY + n2;
                inPlaneAxisZ = inPlaneAxisZ + n3;
                inPlaneCount++;
             }
          }
        }
      }
    }
    if(inPlaneCount > 0)
    {
        inPlaneAngle = inPlaneAngle/inPlaneCount;
        inPlaneAxisX = inPlaneAxisX/inPlaneCount;
        inPlaneAxisY = inPlaneAxisY/inPlaneCount;
        inPlaneAxisZ = inPlaneAxisZ/inPlaneCount;
    }
    if(outPlaneCount > 0)
    {
        outPlaneAngle = outPlaneAngle/outPlaneCount;
        outPlaneAxisX = outPlaneAxisX/outPlaneCount;
        outPlaneAxisY = outPlaneAxisY/outPlaneCount;
        outPlaneAxisZ = outPlaneAxisZ/outPlaneCount;
    }
    outFile << plane << "	" << inPlaneCount << "	" << inPlaneAngle << "	" << inPlaneAxisX << "	" << inPlaneAxisY << "	" << inPlaneAxisZ << "	" << outPlaneCount << "	" << outPlaneAngle << "	" << outPlaneAxisX << "	" << outPlaneAxisY << "	" << outPlaneAxisZ << std::endl;
  }



 notifyStatusMessage("FindSlicetoSliceRotations Completed");
}
