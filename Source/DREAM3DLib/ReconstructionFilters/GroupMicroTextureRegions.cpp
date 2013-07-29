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

#include "GroupMicroTextureRegions.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupMicroTextureRegions::GroupMicroTextureRegions() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_FieldParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CAxisTolerance(1.0f),
  m_GrainIds(NULL),
  m_CellParentIds(NULL),
  m_FieldParentIds(NULL),
  m_AvgQuats(NULL),
  m_Active(NULL),
  m_FieldPhases(NULL),
  m_NeighborList(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupMicroTextureRegions::~GroupMicroTextureRegions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("C-Axis Alignment Tolerance");
    option->setPropertyName("CAxisTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GroupMicroTextureRegions::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("CAxisTolerance", getCAxisTolerance() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellParentIds, ss, int32_t, Int32ArrayType, -1, voxels, 1)

  // Field Data
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -302, float, FloatArrayType, fields, 4)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -303, int32_t, Int32ArrayType, fields, 1)


  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldParentIds, ss, int32_t, Int32ArrayType, 0, fields, 1)
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
    ss << "NeighborLists Array Not Initialized correctly" << std::endl;
    setErrorCondition(-304);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
  }

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  //Convert user defined tolerance to radians.
  m_CAxisTolerance = m_CAxisTolerance * DREAM3D::Constants::k_Pi/180.0f;

  notifyStatusMessage("Grouping MicroTexture Regions");
  merge_micro_texture_regions();

  notifyStatusMessage("Characterizing MicroTexture Regions");
  characterize_micro_texture_regions();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("GroupMicroTextureRegions Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::merge_micro_texture_regions()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  VoxelDataContainer* m = getVoxelDataContainer();

  NeighborList<int>& neighborlist = *m_NeighborList;

  float angcur = 180.0f;
  std::vector<int> microtexturelist;
  float w;
  float g1[3][3];
  float g2[3][3];
  float g1t[3][3];
  float g2t[3][3];
  float c1[3];
  float c2[3];
  float caxis[3] = {0,0,1};
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  size_t numgrains = m->getNumFieldTuples();
  unsigned int phase1, phase2;
  int parentcount = 0;
  parentnumbers.resize(numgrains, -1);

  parentnumbers[0] = 0;
  for (size_t i = 1; i < numgrains; i++)
  {
    if (parentnumbers[i] == -1 && m_FieldPhases[i] > 0)
    {
      parentcount++;
      parentnumbers[i] = parentcount;
      m_Active[i] = true;
      microtexturelist.push_back(i);
      for (std::vector<int>::size_type j = 0; j < microtexturelist.size(); j++)
      {
        int firstgrain = microtexturelist[j];
        int size = int(neighborlist[firstgrain].size());
        QuaternionMathF::Copy(avgQuats[firstgrain], q1);
//        q1[0] = 1;
//        q1[1] = m_AvgQuats[5*firstgrain+1];
//        q1[2] = m_AvgQuats[5*firstgrain+2];
//        q1[3] = m_AvgQuats[5*firstgrain+3];
//        q1[4] = m_AvgQuats[5*firstgrain+4];
        phase1 = m_CrystalStructures[m_FieldPhases[firstgrain]];
        OrientationMath::QuattoMat(q1, g1);
        //transpose the g matrix so when caxis is multiplied by it
        //it will give the sample direction that the caxis is along
        MatrixMath::Transpose3x3(g1, g1t);
        MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
        //normalize so that the dot product can be taken below without
        //dividing by the magnitudes (they would be 1)
        MatrixMath::Normalize3x1(c1);


        for (int l = 0; l < size; l++)
        {
          angcur = 180.0f;
          size_t neigh = neighborlist[firstgrain][l];
          if (neigh != i && parentnumbers[neigh] == -1 && m_FieldPhases[neigh] > 0)
          {
            phase2 = m_CrystalStructures[m_FieldPhases[neigh]];
            if (phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High) )
            {
              QuaternionMathF::Copy(avgQuats[neigh], q2);
//              q2[0] = 1;
//              q2[1] = m_AvgQuats[5*neigh+1];
//              q2[2] = m_AvgQuats[5*neigh+2];
//              q2[3] = m_AvgQuats[5*neigh+3];
//              q2[4] = m_AvgQuats[5*neigh+4];
              OrientationMath::QuattoMat(q2, g2);
              //transpose the g matrix so when caxis is multiplied by it
              //it will give the sample direction that the caxis is along
              MatrixMath::Transpose3x3(g2, g2t);
              MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);
              //normalize so that the dot product can be taken below without
              //dividing by the magnitudes (they would be 1)
              MatrixMath::Normalize3x1(c2);

              w = ((c1[0]*c2[0])+(c1[1]*c2[1])+(c1[2]*c2[2]));
              w = acosf(w);
              if (w <= m_CAxisTolerance || (DREAM3D::Constants::k_Pi-w) <= m_CAxisTolerance)
              {
                parentnumbers[neigh] = parentcount;
                microtexturelist.push_back(neigh);
              }
            }
          }
        }
      }
    }
    microtexturelist.clear();
  }
  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int grainname = m_GrainIds[k];
    m_CellParentIds[k] = parentnumbers[grainname];
    m_FieldParentIds[grainname] = m_CellParentIds[k];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::characterize_micro_texture_regions()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  size_t numgrains = m->getNumFieldTuples();
  for (size_t i = 0; i < numgrains; i++)
  {

  }
}
