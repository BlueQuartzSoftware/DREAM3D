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
m_CreateNewStatsFile(true)
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
void FindODF::preflight()
{
  int err = 0;
  std::stringstream ss;
  DataContainer::Pointer m = DataContainer::New();
  IDataArray::Pointer d = m->getVoxelData(DREAM3D::VoxelData::Phases);
  if(d.get() == NULL)
  {
	  ss << "Phases (Cells) Array Not Initialized At Beginning of FindODF Filter" << std::endl;
	  err = -300;
  }
  d = m->getFieldData(DREAM3D::FieldData::Phases);
  if(d.get() == NULL)
  {
	  ss << "Phases (Field) Array Not Initialized At Beginning of FindODF Filter" << std::endl;
	  err = -300;
  }
  d = m->getFieldData(DREAM3D::FieldData::EulerAngles);
  if(d.get() == NULL)
  {
	  ss << "EulerAngles (Fields) Array Not Initialized At Beginning of FindODF Filter" << std::endl;
	  err = -300;
  }
  d = m->getFieldData(DREAM3D::FieldData::SurfaceFields);
  if(d.get() == NULL)
  {
	  ss << "SurfaceFields Array Not Initialized At Beginning of FindODF Filter" << std::endl;
	  err = -300;
  }
  d = m->getFieldData(DREAM3D::FieldData::Volumes);
  if(d.get() == NULL)
  {
	  ss << "Volumes Array Not Initialized At Beginning of FindODF Filter" << std::endl;
	  err = -300;
  }
  d = m->getFieldData(DREAM3D::FieldData::Active);
  if(d.get() == NULL)
  {
	  ss << "Active Array Not Initialized At Beginning of FindODF Filter" << std::endl;
	  err = -300;
  }

  setErrorCondition(err);
  setErrorMessage(ss.str());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindODF::execute()
{
  setErrorCondition(0);

  H5StatsWriter::Pointer h5io = H5StatsWriter::New(getH5StatsFile(), m_CreateNewStatsFile);

  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int64_t totalPoints = m->totalPoints();
    int32_t* phases = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, this);
  if (NULL == phases) { return; }
  size_t bin;
  size_t numgrains = m->m_Grains.size();
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
	  totalvol[phases[i]]++;
  }
  for (size_t i = 1; i < m->crystruct.size(); i++)
  {
	  totalvol[i] = totalvol[i]*float(m->getXRes()*m->getYRes()*m->getZRes());
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m->m_Grains[i]->surfacefield == false && m->m_Grains[i]->active == true)
    {
      float vol = m->m_Grains[i]->volume;
      ea1 = m->m_Grains[i]->euler1;
      ea2 = m->m_Grains[i]->euler2;
      ea3 = m->m_Grains[i]->euler3;
      phase = m->crystruct[m->m_Grains[i]->phase];
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
      bin = m_OrientationOps[phase]->getOdfBin(r1, r2, r3);
      eulerodf[m->m_Grains[i]->phase][bin] = eulerodf[m->m_Grains[i]->phase][bin] + (vol / totalvol[m->m_Grains[i]->phase]);
    }
  }
  int err;
  for(size_t i=1;i<m->crystruct.size();i++)
  {
	  err = h5io->writeODFData(i, &dims, eulerodf[i]);
	  delete[] eulerodf[i];
  }
  delete[] eulerodf;

  notify("FindODF Completed", 0, Observable::UpdateProgressMessage);
}
