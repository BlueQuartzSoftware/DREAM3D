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
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

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
  m_MTRdensityArrayName(DREAM3D::CellData::MTRdensity),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_VolumesArrayName(DREAM3D::FieldData::Volumes),
  m_ContiguousNeighborListArrayName(DREAM3D::FieldData::NeighborList),
  m_NonContiguousNeighborListArrayName(DREAM3D::FieldData::NeighborhoodList),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_FieldParentIdsArrayName(DREAM3D::FieldData::ParentIds),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CAxisTolerance(1.0f),
  m_UseNonContiguousNeighbors(false),
  m_GrainIds(NULL),
  m_CellParentIds(NULL),
  m_FieldParentIds(NULL),
  m_MTRdensity(NULL),
  m_AvgQuats(NULL),
  m_Active(NULL),
  m_FieldPhases(NULL),
  m_Volumes(NULL),
  m_ContiguousNeighborList(NULL),
  m_NonContiguousNeighborList(NULL),
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
  FilterParameterVector parameters;
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

  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Use Non-Contiguous Neighbors");
    option->setPropertyName("UseNonContiguousNeighbors");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    option->setUnits("");
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
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setCAxisTolerance( reader->readValue("CAxisTolerance", getCAxisTolerance()) );
  setUseNonContiguousNeighbors( reader->readValue("UseNonContiguousNeighbors", getUseNonContiguousNeighbors()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GroupMicroTextureRegions::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("CAxisTolerance", getCAxisTolerance() );
  writer->writeValue("UseNonContiguousNeighbors", getUseNonContiguousNeighbors() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, -301, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellParentIds, int32_t, Int32ArrayType, -1, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, MTRdensity, float, FloatArrayType, 0, voxels, 1)

  // Field Data
  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, AvgQuats, -302, float, FloatArrayType, fields, 4)
  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldPhases, -303, int32_t, Int32ArrayType, fields, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, Active, bool, BoolArrayType, true, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldParentIds, int32_t, Int32ArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, Volumes, float, FloatArrayType, 0, fields, 1)

  if(m_UseNonContiguousNeighbors == false)
  {
    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    m_ContiguousNeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getCellFieldData(DREAM3D::FieldData::NeighborList).get());
    if(m_ContiguousNeighborList == NULL)
    {
      QString ss = QObject::tr("NeighborLists Array Not Initialized correctly");
      setErrorCondition(-304);
      addErrorMessage(getHumanLabel(), ss, -1);
    }
  }
  else
  {
    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    m_ContiguousNeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getCellFieldData(DREAM3D::FieldData::NeighborList).get());
    m_NonContiguousNeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getCellFieldData(DREAM3D::FieldData::NeighborhoodList).get());
    if(m_ContiguousNeighborList == NULL || m_NonContiguousNeighborList == NULL)
    {
      QString ss = QObject::tr("NeighborhoodLists Array Not Initialized correctly");
      setErrorCondition(-305);
      addErrorMessage(getHumanLabel(), ss, -1);
    }
  }
  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, CrystalStructures, -305, unsigned int, XTalStructArrayType, ensembles, 1)
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  // Convert user defined tolerance to radians.
  m_CAxisTolerance = m_CAxisTolerance * DREAM3D::Constants::k_Pi / 180.0f;

  notifyStatusMessage("Grouping MicroTexture Regions");
  merge_micro_texture_regions();

  notifyStatusMessage("Characterizing MicroTexture Regions");
  // characterize_micro_texture_regions();

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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  NeighborList<int>& neighborlist = *m_ContiguousNeighborList;
  NeighborList<int>& neighborhoodlist = *m_NonContiguousNeighborList;

  float angcur = 180.0f;
  std::vector<int> microtexturelist;
  std::vector<int> totalCheckList;
  float microtexturevolume = 0.0f;
  float totalCheckVolume = 0.0f;
  float w;
  float g1[3][3];
  float g2[3][3];
  float g1t[3][3];
  float g2t[3][3];
  float c1[3];
  float c2[3];
  float caxis[3] = {0, 0, 1};
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  size_t numgrains = m->getNumCellFieldTuples();
  unsigned int phase1, phase2;
  int parentcount = 0;
  parentnumbers.resize(numgrains, -1);
  beenChecked.resize(numgrains, false);
  intensities.resize(numgrains, 0.0);
  int size1 = 0, size2 = 0, size = 0;
  parentnumbers[0] = 0;
  for (size_t i = 1; i < numgrains; i++)
  {
    if (parentnumbers[i] == -1 && m_FieldPhases[i] > 0)
    {
      parentcount++;
      parentnumbers[i] = parentcount;
      if (i % 1000 == 0)
      {
        QString ss = QObject::tr("Working On Grain %1 of %2").arg(i).arg(numgrains);
        notifyStatusMessage(ss);
      }
      m_Active[i] = true;
      microtexturelist.push_back(i);
      totalCheckList.push_back(i);
      microtexturevolume = m_Volumes[i];
      totalCheckVolume = m_Volumes[i];
      for (std::vector<int>::size_type j = 0; j < microtexturelist.size(); j++)
      {
        int firstgrain = microtexturelist[j];
        size1 = int(neighborlist[firstgrain].size());
        if (m_UseNonContiguousNeighbors == true) { size2 = int(neighborhoodlist[firstgrain].size()); }
        QuaternionMathF::Copy(avgQuats[firstgrain], q1);
        phase1 = m_CrystalStructures[m_FieldPhases[firstgrain]];
        OrientationMath::QuattoMat(q1, g1);
        //transpose the g matrix so when caxis is multiplied by it
        //it will give the sample direction that the caxis is along
        MatrixMath::Transpose3x3(g1, g1t);
        MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
        //normalize so that the dot product can be taken below without
        //dividing by the magnitudes (they would be 1)
        MatrixMath::Normalize3x1(c1);

        size_t neigh;
        for (int k = 0; k < 2; k++)
        {
          if (k == 0) { size = size1; }
          else if (k == 1) { size = size2; }
          for (int l = 0; l < size; l++)
          {
            angcur = 180.0f;
            if (k == 0) { neigh = neighborlist[firstgrain][l]; }
            else if (k == 1) { neigh = neighborhoodlist[firstgrain][l]; }
            if (beenChecked[neigh] == false)
            {
              totalCheckList.push_back(neigh);
              totalCheckVolume += m_Volumes[neigh];
            }
            beenChecked[neigh] = true;
            if (neigh != i && parentnumbers[neigh] == -1 && m_FieldPhases[neigh] > 0)
            {
              phase2 = m_CrystalStructures[m_FieldPhases[neigh]];
              if (phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High) )
              {
                QuaternionMathF::Copy(avgQuats[neigh], q2);
                OrientationMath::QuattoMat(q2, g2);
                //transpose the g matrix so when caxis is multiplied by it
                //it will give the sample direction that the caxis is along
                MatrixMath::Transpose3x3(g2, g2t);
                MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);
                //normalize so that the dot product can be taken below without
                //dividing by the magnitudes (they would be 1)
                MatrixMath::Normalize3x1(c2);

                w = MatrixMath::CosThetaBetweenVectors(c1, c2);
                DREAM3DMath::boundF(w, -1, 1);
                w = acosf(w);
                if (w <= m_CAxisTolerance || (DREAM3D::Constants::k_Pi - w) <= m_CAxisTolerance)
                {
                  parentnumbers[neigh] = parentcount;
                  microtexturelist.push_back(neigh);
                  microtexturevolume += m_Volumes[neigh];
                }
              }
            }
          }
        }
      }
      float fraction = microtexturevolume / totalCheckVolume;
      intensities[parentcount] = fraction;
      int checkedSize = totalCheckList.size();
      for (size_t j = 0 ; j < checkedSize ; j++)
      {
        beenChecked[totalCheckList[j]] = false;
      }
    }
    microtexturelist.clear();
    totalCheckList.clear();
    microtexturevolume = 0.0f;
    totalCheckVolume = 0.0f;
  }
  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());

  for (size_t k = 0; k < totalPoints; k++)
  {
    int grainname = m_GrainIds[k];
    m_CellParentIds[k] = parentnumbers[grainname];
    m_FieldParentIds[grainname] = m_CellParentIds[k];
    m_MTRdensity[k] = intensities[parentnumbers[grainname]];
  }

}
