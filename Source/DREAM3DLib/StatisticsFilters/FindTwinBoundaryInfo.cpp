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

#include "FindTwinBoundaryInfo.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaryInfo::FindTwinBoundaryInfo()  :
  AbstractFilter(),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_NeighborListArrayName(DREAM3D::FieldData::NeighborList),
  m_AvgQuats(NULL),
  m_FieldPhases(NULL),
  m_NeighborList(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationMath::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTwinBoundaryInfo::~FindTwinBoundaryInfo()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Twin Boundary Info File");
    option->setPropertyName("TwinBoundaryInfoFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
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
void FindTwinBoundaryInfo::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("TwinBoundaryInfoFile", getTwinBoundaryInfoFile() );
  writer->writeValue("AxisTolerance", getAxisTolerance() );
  writer->writeValue("AngleTolerance", getAngleTolerance() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -303, int32_t, Int32ArrayType, fields, 1)


  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  IDataArray::Pointer neighborListPtr = m->getFieldData(m_NeighborListArrayName);
  if (NULL == neighborListPtr.get())
  {
    ss.str("");
    ss << "NeighborLists are not available and are required for this filter to run. A filter that generates NeighborLists needs to be placed before this filter in the pipeline." << std::endl;
    setErrorCondition(-305);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(neighborListPtr.get());
  }

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTwinBoundaryInfo::execute()
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

  std::ofstream outFile;
  outFile.open(m_TwinBoundaryInfoFile.c_str(), std::ios_base::binary);

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;

  std::vector<std::vector<float> > misorientationlists;

  float n1 = 0.0f, n2 = 0.0f, n3= 0.0f;
  //float r1= 0.0f, r2 = 0.0f, r3 = 0.0f;
  // int mbin = 0;
  float w;
  float q1[5];
  float q2[5];
  float misq[5], sym_q1[5], sym_q2[5], s1misq[5], s2misq[5];

  float angtol = m_AngleTolerance;
  float axistol = static_cast<float>( m_AxisTolerance*M_PI/180.0f );

  size_t numgrains = m->getNumFieldTuples();
  unsigned int phase1, phase2;

  bool isTwin;
  float schmid1, schmid2, schmid3;
  int plane;
  float axisdiff111, angdiff60;

  float n[3], b[3];
  float g[3][3];
  float sampleLoading[3];
  float crystalLoading[3];

  float cosPhi, cosLambda;

  size_t nname;
  
  outFile << "Grain1	Grain2	IsTwin	Plane	Schmid1	Schmid2	Schmid3" << std::endl;
  
  for (size_t i = 1; i < numgrains; i++)
  {
    q1[0] = m_AvgQuats[5*i];
    q1[1] = m_AvgQuats[5*i+1];
    q1[2] = m_AvgQuats[5*i+2];
    q1[3] = m_AvgQuats[5*i+3];
    q1[4] = m_AvgQuats[5*i+4];
  	phase1 = m_CrystalStructures[m_FieldPhases[i]];
    OrientationMath::QuattoMat(q1, g);

    sampleLoading[0] = 0;
    sampleLoading[1] = 0;
    sampleLoading[2] = 1;

    MatrixMath::multiply3x3with3x1(g, sampleLoading, crystalLoading);

    OrientationMath::invertQuaternion(q1);
	  for (size_t j = 0; j < neighborlist[i].size(); j++)
    {
      nname = neighborlist[i][j];
	    if(nname > i)
	    {
	    w = 10000.0;
		  isTwin = false;
		  schmid1 = 0, schmid2 = 0, schmid3 = 0;
		  plane = 0;
		  q2[0] = m_AvgQuats[5*nname];
 		  q2[1] = m_AvgQuats[5*nname+1];
		  q2[2] = m_AvgQuats[5*nname+2];
		  q2[3] = m_AvgQuats[5*nname+3];
		  q2[4] = m_AvgQuats[5*nname+4];

		  phase2 = m_CrystalStructures[m_FieldPhases[nname]];
		  if (phase1 == phase2)
		  {
			OrientationMath::multiplyQuaternions(q1, q2, misq);
			int nsym = m_OrientationOps[m_CrystalStructures[m_FieldPhases[i]]]->getNumSymOps();
			for (int k=0; k< nsym;k++)
			{
			  //calculate the symmetric misorienation
			  m_OrientationOps[m_CrystalStructures[m_FieldPhases[i]]]->getQuatSymOp(k, sym_q1);
			  for (int l=0; l < nsym; l++)
			  {
			    m_OrientationOps[m_CrystalStructures[m_FieldPhases[i]]]->getQuatSymOp(l, sym_q2);
				  OrientationMath::invertQuaternion(sym_q2);
				  OrientationMath::multiplyQuaternions(misq, sym_q2, s2misq);
          OrientationMath::multiplyQuaternions(sym_q1, s2misq, s1misq);
          OrientationMath::QuattoAxisAngle(s1misq, w, n1, n2, n3);
				  w = w*180.0/m_pi;
	        axisdiff111 = acosf(fabs(n1)*0.57735f+fabs(n2)*0.57735f+fabs(n3)*0.57735f);
	        angdiff60 = fabs(w-60.0f);
	        if (axisdiff111 < axistol && angdiff60 < angtol)
				  {
					  isTwin = true;
					  n[0] = n1, n[1] = n2, n[2] = n3;
				  }
			  }
			}
      if (isTwin == true)
      {
				if(n[2] < 0) n[0] = -n[0], n[1] = -n[1], n[2] = -n[2];
				if(n[0] > 0 && n[1] > 0)
				{
					plane = 1;
					b[0] = 1, b[1] = -1, b[2] = 0;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid1 = cosPhi*cosLambda;
					b[0] = -1, b[1] = 0, b[2] = 1;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid2 = cosPhi*cosLambda;
					b[0] = 0, b[1] = -1, b[2] = 1;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid3 = cosPhi*cosLambda;
				}
				if(n[0] > 0 && n[1] < 0)
				{
					plane = 2;
					b[0] = 1, b[1] = 1, b[2] = 0;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid1 = cosPhi*cosLambda;
					b[0] = 0, b[1] = 1, b[2] = 1;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid2 = cosPhi*cosLambda;
					b[0] = -1, b[1] = 0, b[2] = 1;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid3 = cosPhi*cosLambda;
				}
				if(n[0] < 0 && n[1] > 0)
				{
					plane = 3;
					b[0] = 1, b[1] = 1, b[2] = 0;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid1 = cosPhi*cosLambda;
					b[0] = 1, b[1] = 0, b[2] = 1;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid2 = cosPhi*cosLambda;
					b[0] = 0, b[1] = -1, b[2] = 1;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid3 = cosPhi*cosLambda;
				}
				if(n[0] < 0 && n[1] < 0)
				{
					plane = 4;
					b[0] = 1, b[1] = 0, b[2] = 1;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid1 = cosPhi*cosLambda;
					b[0] = 0, b[1] = 1, b[2] = 1;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid2 = cosPhi*cosLambda;
					b[0] = 1, b[1] = -1, b[2] = 0;
					cosPhi = fabs(MatrixMath::dotProduct(crystalLoading, n));
					cosLambda = fabs(MatrixMath::dotProduct(crystalLoading, b));
					schmid3 = cosPhi*cosLambda;
				}
			}
		  }
		  outFile << i << "	" << nname << "	" << isTwin << "	" << plane << "	" << schmid1 << "	" << schmid2 << "	" << schmid3 << std::endl;
	  }
    }
  }
  outFile.close();

  notifyStatusMessage("FindTwinBoundaryInfo Completed");
}
