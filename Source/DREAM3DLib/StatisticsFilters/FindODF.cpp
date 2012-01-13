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

#include "FindODF.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindODF::FindODF()  :
AbstractFilter(),
m_CreateNewStatsFile(true),
m_Volumes(NULL),
m_EulerAngles(NULL),
m_PhasesC(NULL),
m_PhasesF(NULL),
m_Active(NULL),
m_SurfaceFields(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
  setupFilterOptions();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindODF::~FindODF()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindODF::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Output Statistics File");
    option->setPropertyName("H5StatsFile");
    option->setWidgetType(FilterOption::OutputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindODF::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, VoxelData, Phases, C, ss, -300, int32_t, Int32ArrayType,  voxels, 1);

  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, -303,  int32_t, Int32ArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, VoxelData, EulerAngles, ss, -304, float, FloatArrayType, voxels, 3);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, -303, bool, BoolArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, -309, float, FloatArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, -304, bool, BoolArrayType, fields, 1);

  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindODF::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindODF::execute()
{
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->totalPoints();
  dataCheck(false, m->totalPoints(), m->getTotalFields(), m->crystruct.size());

  size_t bin;
  size_t numgrains = m->getTotalFields();
  int phase;
  float *totalvol;
  //  Ebsd::CrystalStructure xtal;
  float **eulerodf;

  totalvol = new float [m->crystruct.size()];
  eulerodf = new float *[m->crystruct.size()];
  unsigned long long dims = 0;
  for(unsigned long long i=1;i<m->crystruct.size();i++)
  {
	  totalvol[i] = 0;
	  if (m->crystruct[i] == Ebsd::Hexagonal)
	  {
	    dims = 36 * 36 * 12;
	    eulerodf[i] = new float[dims];
	    for (unsigned long long j = 0; j < dims; j++)
	    {
	      eulerodf[i][j] = 0.0;
	    }
	  }
	  else if (m->crystruct[i] == Ebsd::Cubic)
	  {
	    dims = 18 * 18 * 18;
		  eulerodf[i] = new float[dims];
		  for (unsigned long long j = 0; j < dims; j++)
		  {
			eulerodf[i][j] = 0.0;
		  }
	  }
  }
  float ea1, ea2, ea3;
  float r1, r2, r3;
  for (int i = 0; i < totalPoints; i++)
  {
	  totalvol[m_PhasesC[i]]++;
  }
  for (size_t i = 1; i < m->crystruct.size(); i++)
  {
	  totalvol[i] = totalvol[i]*float(m->getXRes()*m->getYRes()*m->getZRes());
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_SurfaceFields[i] == false && m_Active[i] == true)
    {
      float vol = m_Volumes[i];
      ea1 = m_EulerAngles[3*i];
      ea2 = m_EulerAngles[3*i+1];
      ea3 = m_EulerAngles[3*i+2];
      phase = m->crystruct[m_PhasesF[i]];
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
      bin = m_OrientationOps[phase]->getOdfBin(r1, r2, r3);
      eulerodf[m_PhasesF[i]][bin] = eulerodf[m_PhasesF[i]][bin] + (vol / totalvol[m_PhasesF[i]]);
    }
  }
  int err;

  H5StatsWriter::Pointer h5io = H5StatsWriter::New(getH5StatsFile(), m_CreateNewStatsFile);

  for(size_t i=1;i<m->crystruct.size();i++)
  {
	  err = h5io->writeODFData(i, &dims, eulerodf[i]);
	  delete[] eulerodf[i];
  }
  delete[] eulerodf;

  notify("FindODF Completed", 0, Observable::UpdateProgressMessage);
}
