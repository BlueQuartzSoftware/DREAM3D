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

#include "FindSlipTransmissionMetrics.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"


const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlipTransmissionMetrics::FindSlipTransmissionMetrics() :
AbstractFilter(),
m_F1ArrayName(DREAM3D::CellData::F1),
m_F1sptArrayName(DREAM3D::CellData::F1spt),
m_F7ArrayName(DREAM3D::CellData::F7),
m_mPrimeArrayName(DREAM3D::CellData::mPrime),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GBEuclideanDistancesArrayName(DREAM3D::CellData::GBEuclideanDistances),
m_TJEuclideanDistancesArrayName(DREAM3D::CellData::TJEuclideanDistances),
m_QPEuclideanDistancesArrayName(DREAM3D::CellData::QPEuclideanDistances),
m_NearestNeighborsArrayName(DREAM3D::CellData::NearestNeighbors),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
m_GrainIds(NULL),
m_FieldPhases(NULL),
m_NearestNeighbors(NULL),
m_AvgQuats(NULL),
m_F1(NULL),
m_F1spt(NULL),
m_F7(NULL),
m_mPrime(NULL),
m_GBEuclideanDistances(NULL),
m_TJEuclideanDistances(NULL),
m_QPEuclideanDistances(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlipTransmissionMetrics::~FindSlipTransmissionMetrics()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setupFilterParameters()
{

}
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::writeFilterParameters(AbstractFilterParametersWriter* writer)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  int err = 0;

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, NearestNeighbors, ss, -300, int32_t, Int32ArrayType, voxels, 3)
  GET_PREREQ_DATA(m, DREAM3D, CellData, GBEuclideanDistances, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, TJEuclideanDistances, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, QPEuclideanDistances, ss, -300, float, FloatArrayType, voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)
  TEST_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, err, -302, int32_t, Int32ArrayType, fields, 1)
  if(err == -302)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) find_grainphases->preflight();
    if(preflight == false) find_grainphases->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType, fields, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, F1, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, F1spt, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, F7, ss, float, FloatArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, mPrime, ss, float, FloatArrayType, 0, voxels, 1)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::preflight()
{
  dataCheck(true, 1,1 ,1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  typedef DataArray<unsigned int> XTalType;
  XTalType* crystruct
      = XTalType::SafeObjectDownCast<IDataArray*, XTalType*>(m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructures).get());

//  size_t numXTals = crystruct->GetNumberOfTuples();


  float mprime, F1, F1spt, F7;
  int gname, gname2;
  float q1[5], q2[5];

//  int mprimebin, F1bin, F1sptbin, F7bin, disbin;
 // int actualpoints = 0;

  float LD[3];
  LD[0] = 0;
  LD[1] = 0;
  LD[2] = 1;
  for (int i = 0; i < totalPoints; i++)
  {
    gname = m_GrainIds[i];
    if(gname > 0)
    {
      gname2 = m_NearestNeighbors[i * 3 + 0];
      for (int j = 0; j < 5; j++)
      {
        q1[j] = m_AvgQuats[5 * gname + j];
        q2[j] = m_AvgQuats[5 * gname2 + j];
      }
      if(crystruct->GetValue(m_FieldPhases[gname]) == crystruct->GetValue(m_FieldPhases[gname2]) && m_FieldPhases[gname] > 0)
      {
    m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname])]->getmPrime(q1, q2, LD, mprime);
    m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname])]->getF1(q1, q2, LD, true, F1);
    m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname])]->getF1spt(q1, q2, LD, true, F1spt);
    m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname])]->getF7(q1, q2, LD, true, F7);
      }
      else
      {
    mprime = 0;
    F1 = 0;
    F1spt = 0;
    F7 = 0;
      }
    m_mPrime[i] = mprime;
    m_F1[i] = F1;
    m_F1spt[i] = F1spt;
    m_F7[i] = F7;
    }
  }

 notifyStatusMessage("Completed");
}
