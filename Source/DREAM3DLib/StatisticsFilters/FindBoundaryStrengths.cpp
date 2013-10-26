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

#include "FindBoundaryStrengths.h"

// DREAM3D includes
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"






// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryStrengths::FindBoundaryStrengths() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_SurfaceDataContainerName(DREAM3D::HDF5::SurfaceDataContainerName),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshF1sArrayName(DREAM3D::FaceData::SurfaceMeshF1s),
  m_SurfaceMeshF1sptsArrayName(DREAM3D::FaceData::SurfaceMeshF1spts),
  m_SurfaceMeshF7sArrayName(DREAM3D::FaceData::SurfaceMeshF7s),
  m_SurfaceMeshmPrimesArrayName(DREAM3D::FaceData::SurfaceMeshmPrimes),
  m_FieldPhases(NULL),
  m_AvgQuats(NULL),
  m_CrystalStructures(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshF1s(NULL),
  m_SurfaceMeshF1spts(NULL),
  m_SurfaceMeshF7s(NULL),
  m_SurfaceMeshmPrimes(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  m_Loading.x = 1.0f;
  m_Loading.y = 1.0f;
  m_Loading.z = 1.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryStrengths::~FindBoundaryStrengths()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Loading");
    option->setPropertyName("Loading");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindBoundaryStrengths::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setLoading( reader->readFloatVec3("Loading", getLoading() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindBoundaryStrengths::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("Loading", getLoading() );
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
    setErrorCondition(-384);
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -385);
    setErrorCondition(-385);
  }
  else
  {
    QVector<int> dims(1, 2);
    GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceLabels, -386, int32_t, Int32ArrayType, fields, dims)
    CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshF1s, float, FloatArrayType, 0, fields, dims)
    CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshF1spts, float, FloatArrayType, 0, fields, dims)
    CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshF7s, float, FloatArrayType, 0, fields, dims)
    CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshmPrimes, float, FloatArrayType, 0, fields, dims)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::dataCheckVoxel(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 4);
  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, AvgQuats, -301, float, FloatArrayType, fields, dims)

  dims[0] = 1;
  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldPhases, -302, int32_t, Int32ArrayType, fields, dims)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, CrystalStructures, -305, unsigned int, XTalStructArrayType, ensembles, dims)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  dataCheckVoxel(true, 1, 1 , 1);

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", -383);
    setErrorCondition(-383);
  }

  dataCheckSurfaceMesh(true, 1, 1 , 1);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::execute()
{

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheckSurfaceMesh(false, 0, sm->getNumFaceTuples(), 0);
  dataCheckVoxel(false, m->getTotalPoints(), m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }


  // float w, n1, n2, n3;
// float sf1, sf2;
  float mPrime_1, mPrime_2, F1_1, F1_2, F1spt_1, F1spt_2, F7_1,  F7_2;
  int gname1, gname2;
  // int ss1, ss2;
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float LD[3];

  LD[0] = m_Loading.x;
  LD[1] = m_Loading.y;
  LD[2] = m_Loading.z;
  MatrixMath::Normalize3x1(LD);

  int nTriangles = sm->getNumFaceTuples();
  for (int i = 0; i < nTriangles; i++)
  {
    gname1 = m_SurfaceMeshFaceLabels[i * 2];
    gname2 = m_SurfaceMeshFaceLabels[i * 2 + 1];
    if(gname1 > 0 && gname2 > 0)
    {

      QuaternionMathF::Copy(avgQuats[gname1], q1);
      QuaternionMathF::Copy(avgQuats[gname2], q2);
//      for (int j = 0; j < 5; j++)
//      {
//        q1[j] = m_AvgQuats[5 * gname1 + j];
//        q2[j] = m_AvgQuats[5 * gname2 + j];
//      }
      if(m_CrystalStructures[m_FieldPhases[gname1]] == m_CrystalStructures[m_FieldPhases[gname2]]
          && m_FieldPhases[gname1] > 0)
      {
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname1]]]->getmPrime(q1, q2, LD, mPrime_1);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname1]]]->getmPrime(q2, q1, LD, mPrime_2);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname1]]]->getF1(q1, q2, LD, true, F1_1);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname1]]]->getF1(q2, q1, LD, true, F1_2);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname1]]]->getF1spt(q1, q2, LD, true, F1spt_1);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname1]]]->getF1spt(q2, q1, LD, true, F1spt_2);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname1]]]->getF7(q1, q2, LD, true, F7_1);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname1]]]->getF7(q2, q1, LD, true, F7_2);
      }
      else
      {
        mPrime_1 = 0;
        F1_1 = 0;
        F1spt_1 = 0;
        F7_1 = 0;
        mPrime_2 = 0;
        F1_2 = 0;
        F1spt_2 = 0;
        F7_2 = 0;
      }
    }
    else
    {
      mPrime_1 = 0;
      F1_1 = 0;
      F1spt_1 = 0;
      F7_1 = 0;
      mPrime_2 = 0;
      F1_2 = 0;
      F1spt_2 = 0;
      F7_2 = 0;
    }
    m_SurfaceMeshmPrimes[2 * i] = mPrime_1;
    m_SurfaceMeshmPrimes[2 * i + 1] = mPrime_2;
    m_SurfaceMeshF1s[2 * i] = F1_1;
    m_SurfaceMeshF1s[2 * i + 1] = F1_2;
    m_SurfaceMeshF1spts[2 * i] = F1spt_1;
    m_SurfaceMeshF1spts[2 * i + 1] = F1spt_2;
    m_SurfaceMeshF7s[2 * i] = F7_1;
    m_SurfaceMeshF7s[2 * i + 1] = F7_2;
  }


  notifyStatusMessage("Completed");
}
