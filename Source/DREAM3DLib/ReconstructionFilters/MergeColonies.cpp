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

#include "MergeColonies.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"


#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = static_cast<float>(M_PI);

  static const float unit110 = 1.0/sqrtf(2.0);
  static const float unit111 = 1.0/sqrtf(3.0);
  static const float unit112_1 = 1.0/sqrtf(6.0);
  static const float unit112_2 = 2.0/sqrtf(6.0);

  float crystalDirections[12][3][3] = {{{unit111, unit112_1, unit110},
                    {-unit111, -unit112_1, unit110},
                    {unit111, -unit112_2, 0}},

                    {{-unit111, unit112_1, unit110},
                    {unit111, -unit112_1, unit110},
                    {unit111, unit112_2, 0}},

                    {{unit111, -unit112_1, unit110},
                    {unit111, -unit112_1, -unit110},
                    {unit111, unit112_2, 0}},

                    {{unit111, unit112_1, unit110},
                    {unit111, unit112_1, -unit110},
                    {-unit111, unit112_2, 0}},

                    {{unit111, unit112_1, unit110},
                    {unit111, -unit112_2, 0},
                    {unit111, unit112_1, -unit110}},

                    {{unit111, -unit112_1, unit110},
                    {-unit111, -unit112_2, 0},
                    {unit111, -unit112_1, -unit110}},

                    {{unit111, -unit112_1, unit110},
                    {unit111, unit112_2, 0},
                    {-unit111, unit112_1, unit110}},

                    {{-unit111, -unit112_1, unit110},
                    {unit111, -unit112_2, 0},
                    {unit111, unit112_1, unit110}},

                    {{unit111, -unit112_2,0},
                    {unit111, unit112_1, unit110},
                    {-unit111, -unit112_1, unit110}},

                    {{unit111, unit112_2, 0},
                    {-unit111, unit112_1, unit110},
                    {unit111, -unit112_1, unit110}},

                    {{unit111, unit112_2, 0},
                    {unit111, -unit112_1, unit110},
                    {unit111, -unit112_1, -unit110}},

                    {{-unit111, unit112_2, 0},
                    {unit111, unit112_1, unit110},
                    {unit111, unit112_1, -unit110}}};


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeColonies::MergeColonies() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_ParentIdsArrayName(DREAM3D::CellData::ParentIds),
m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_ActiveArrayName(DREAM3D::FieldData::Active),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_AxisTolerance(1.0f),
m_AngleTolerance(1.0f),
m_RandomizeParentIds(true),
m_GrainIds(NULL),
m_ParentIds(NULL),
m_AvgQuats(NULL),
m_Active(NULL),
m_FieldPhases(NULL),
m_NeighborList(NULL),
m_CrystalStructures(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeColonies::~MergeColonies()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Axis Tolerance");
    option->setPropertyName("AxisTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Angle Tolerance");
    option->setPropertyName("AngleTolerance");
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
void MergeColonies::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("AxisTolerance", getAxisTolerance() );
  writer->writeValue("AngleTolerance", getAngleTolerance() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  int err = 0;

  // Cell Data
  GET_PREREQ_DATA( m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ParentIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)

  // Field Data
  TEST_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, err, -303, float, FloatArrayType, fields, 5)
  if(err == -303)
  {
    setErrorCondition(0);
    FindAvgOrientations::Pointer find_avgorients = FindAvgOrientations::New();
    find_avgorients->setObservers(this->getObservers());
    find_avgorients->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) find_avgorients->preflight();
    if(preflight == false) find_avgorients->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)

  TEST_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, err, -303,  int32_t, Int32ArrayType, fields, 1)
  if(err == -303)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) find_grainphases->preflight();
    if(preflight == false) find_grainphases->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -303, int32_t, Int32ArrayType, fields, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
    setErrorCondition(0);
    FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
    find_neighbors->setObservers(this->getObservers());
    find_neighbors->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) find_neighbors->preflight();
    if(preflight == false) find_neighbors->execute();
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
    if(m_NeighborList == NULL)
    {
      ss << "NeighborLists Array Not Initialized correctly" << std::endl;
      setErrorCondition(-304);
      addErrorMessage(getHumanLabel(), ss.str(), -1);
    }
  }

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::execute()
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

 notifyStatusMessage("Merging Colonies");
  merge_colonies();

 notifyStatusMessage("Characterizing Colonies");
  characterize_colonies();

  m_RandomizeParentIds = false;
  if (true == m_RandomizeParentIds)
  {
    int64_t totalPoints = m->getTotalPoints();

    // Generate all the numbers up front
    const int rangeMin = 1;
    const int rangeMax = numParents - 1;
    typedef boost::uniform_int<int> NumberDistribution;
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::variate_generator<RandomNumberGenerator&,
                                     NumberDistribution> Generator;

    NumberDistribution distribution(rangeMin, rangeMax);
    RandomNumberGenerator generator;
    Generator numberGenerator(generator, distribution);
    generator.seed(static_cast<boost::uint32_t>( MXA::getMilliSeconds() )); // seed with the current time

    DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(numParents, "New ParentIds");
    int32_t* pid = rndNumbers->GetPointer(0);
    pid[0] = 0;
    std::set<int32_t> parentIdSet;
    parentIdSet.insert(0);
    for(int i = 1; i < numParents; ++i)
    {
      pid[i] = i; //numberGenerator();
      parentIdSet.insert(pid[i]);
    }

    int r;
    size_t temp;
    //--- Shuffle elements by randomly exchanging each with one other.
    for (int i=1; i< numParents; i++) {
      r = numberGenerator(); // Random remaining position.
      if (r >= numParents) {
        continue;
      }
      temp = pid[i];
      pid[i] = pid[r];
      pid[r] = temp;
    }

    // Now adjust all the Grain Id values for each Voxel
    for(int64_t i = 0; i < totalPoints; ++i)
    {
       m_ParentIds[i] = pid[ m_ParentIds[i] ];
    }
  }

 notifyStatusMessage("Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::merge_colonies()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  VoxelDataContainer* m = getVoxelDataContainer();

  NeighborList<int>& neighborlist = *m_NeighborList;

  //Converting the user defined tolerance to radians.
  m_AxisTolerance = m_AxisTolerance*m_pi/180.0f;

  std::vector<int> colonylist;
  float w;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  float r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
  float q1[5];
  float q2[5];
  size_t numgrains = m->getNumFieldTuples();
  unsigned int phase1, phase2;
  int parentcount = 0;
  parentnumbers.resize(numgrains, -1);

  for (size_t i = 1; i < numgrains; i++)
  {
    if (parentnumbers[i] == -1 && m_FieldPhases[i] > 0)
    {
      parentcount++;
      parentnumbers[i] = parentcount;
      m_Active[i] = true;
      colonylist.push_back(i);
      for (std::vector<int>::size_type j = 0; j < colonylist.size(); j++)
      {
        int firstgrain = colonylist[j];
        int size = int(neighborlist[firstgrain].size());
        for (int l = 0; l < size; l++)
        {
          int colony = 0;
          size_t neigh = neighborlist[firstgrain][l];
          if (neigh != i && parentnumbers[neigh] == -1 && m_FieldPhases[neigh] > 0)
          {
            w = 10000.0f;
            q1[0] = 1;
            q1[1] = m_AvgQuats[5*firstgrain+1];
            q1[2] = m_AvgQuats[5*firstgrain+2];
            q1[3] = m_AvgQuats[5*firstgrain+3];
            q1[4] = m_AvgQuats[5*firstgrain+4];
            phase1 = m_CrystalStructures[m_FieldPhases[firstgrain]];
            q2[0] = 1;
            q2[1] = m_AvgQuats[5*neigh+1];
            q2[2] = m_AvgQuats[5*neigh+2];
            q2[3] = m_AvgQuats[5*neigh+3];
            q2[4] = m_AvgQuats[5*neigh+4];
            phase2 = m_CrystalStructures[m_FieldPhases[neigh]];
            if (phase1 == phase2 && phase1 == Ebsd::CrystalStructure::Hexagonal)
            {
                w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
                OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
                m_OrientationOps[phase1]->getMDFFZRod(r1, r2, r3);
                OrientationMath::RodtoAxisAngle(r1, r2, r3, w, n1, n2, n3);
                w = w * (180.0f/m_pi);
//				float vecttol = 0.01f;
//	            if (fabs(fabs(r1) - 0.0000f) < vecttol && fabs(fabs(r2) - 0.0000f) < vecttol && fabs(fabs(r3) - 0.0922f) < vecttol) colony = 1;
//	            if (fabs(fabs(r1) - 0.9957f) < vecttol && fabs(fabs(r2) - 0.0917f) < vecttol && fabs(fabs(r3) - 0.0000f) < vecttol) colony = 1;
//				if (fabs(fabs(r1) - 0.5773f) < vecttol && fabs(fabs(r2) - 0.0000f) < vecttol && fabs(fabs(r3) - 0.0000f) < vecttol) colony = 1;
//				if (fabs(fabs(r1) - 0.5773f) < vecttol && fabs(fabs(r2) - 0.0530f) < vecttol && fabs(fabs(r3) - 0.0922f) < vecttol) colony = 1;
//				if (fabs(fabs(r1) - 0.5870f) < vecttol && fabs(fabs(r2) - 0.0000f) < vecttol && fabs(fabs(r3) - 0.1858f) < vecttol) colony = 1;
                float angdiff1 = fabs(w-10.53f);
                float axisdiff1 = acosf(fabs(n1)*0.0000f+fabs(n2)*0.0000f+fabs(n3)*1.0000f);
                if(angdiff1 < m_AngleTolerance && axisdiff1 < m_AxisTolerance) colony = 1;
                float angdiff2 = fabs(w-90.00f);
                float axisdiff2 = acosf(fabs(n1)*0.9957f+fabs(n2)*0.0917f+fabs(n3)*0.0000f);
                if(angdiff2 < m_AngleTolerance && axisdiff2 < m_AxisTolerance) colony = 1;
                float angdiff3 = fabs(w-60.00f);
                float axisdiff3 = acosf(fabs(n1)*1.0000f+fabs(n2)*0.0000f+fabs(n3)*0.0000f);
                if(angdiff3 < m_AngleTolerance && axisdiff3 < m_AxisTolerance) colony = 1;
                float angdiff4 = fabs(w-60.83f);
                float axisdiff4 = acosf(fabs(n1)*0.9834f+fabs(n2)*0.0917f+fabs(n3)*0.1570f);
                if(angdiff4 < m_AngleTolerance && axisdiff4 < m_AxisTolerance) colony = 1;
                float angdiff5 = fabs(w-63.26f);
                float axisdiff5 = acosf(fabs(n1)*0.9534f+fabs(n2)*0.0000f+fabs(n3)*0.3017f);
                if(angdiff5 < m_AngleTolerance && axisdiff5 < m_AxisTolerance) colony = 1;
                if (colony == 1)
                {
                  parentnumbers[neigh] = parentcount;
                  colonylist.push_back(neigh);
                }
            }
            else if ((phase1 == Ebsd::CrystalStructure::Hexagonal && phase2 == Ebsd::CrystalStructure::Cubic))
            {
                colony = check_for_burgers(q2, q1);
                if (colony == 1)
                {
//                  parentnumbers[neigh] = parentcount;
//                  colonylist.push_back(neigh);
                }
            }
            else if ((phase1 == Ebsd::CrystalStructure::Cubic && phase2 == Ebsd::CrystalStructure::Hexagonal))
            {
                colony = check_for_burgers(q1, q2);
                if (colony == 1)
                {
//                  parentnumbers[neigh] = parentcount;
//                  colonylist.push_back(neigh);
                }
            }
          }
        }
      }
    }
    colonylist.clear();
  }
  size_t totalPoints = static_cast<size_t>(m->getTotalPoints());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int grainname = m_GrainIds[k];
  if(grainname > 0) m_ParentIds[k] = parentnumbers[grainname];
  else m_ParentIds[k] = 0;
  }
  numParents = parentcount+1;
}

void MergeColonies::characterize_colonies()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  size_t numgrains = m->getNumFieldTuples();
  for (size_t i = 0; i < numgrains; i++)
  {

  }
}

int MergeColonies::check_for_burgers(float betaQuat[5], float alphaQuat[5])
{

/*  float ea1 = 0.0*m_pi/180.0f;
  float ea2 = 0.0*m_pi/180.0f;
  float ea3 = 0.0*m_pi/180.0f;
  OrientationMath::eulertoQuat(betaQuat, ea1, ea2, ea3);

  ea1 = 0.0*m_pi/180.0f;
  ea2 = 45.0*m_pi/180.0f;
  ea3 = 54.73*m_pi/180.0f;
  OrientationMath::eulertoQuat(alphaQuat, ea1, ea2, ea3);*/

  float w = 0.0;
  float radToDeg = 180.0f/m_pi;

  float gBeta[3][3];
  float gBetaT[3][3];
  OrientationMath::QuattoMat(betaQuat, gBeta);
  //transpose gBeta so the sample direction is the output when
  //gBeta is multiplied by the crystal directions below
  MatrixMath::transpose3x3(gBeta, gBetaT);

  float gAlpha[3][3];
  float gAlphaT[3][3];
  OrientationMath::QuattoMat(alphaQuat, gAlpha);
  //transpose gBeta so the sample direction is the output when
  //gBeta is multiplied by the crystal directions below
  MatrixMath::transpose3x3(gAlpha, gAlphaT);

  float mat[3][3];
  for(int i=0;i<12;i++)
  {
    MatrixMath::multiply3x3with3x3(gBetaT, crystalDirections[i], mat);
    w = OrientationMath::matrixMisorientation(mat, gAlphaT);
    if((w*radToDeg) < m_AngleTolerance) return 1;
    else if((180.0f-(w*radToDeg)) < m_AngleTolerance) return 1;
  }

  return 0;
}
