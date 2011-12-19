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

#include "FindMDF.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMDF::FindMDF()
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));

  totalsurfacearea = NULL;
  INIT_DataArray(m_TotalSurfaceArea, float);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMDF::~FindMDF()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMDF::execute()
{
  DataContainer* m = getDataContainer();
  setErrorCondition(0);

  H5StatsWriter::Pointer h5io = H5StatsWriter::New(getH5StatsFile());

  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  find_neighbors->setDataContainer(getDataContainer());
  find_neighbors->setObservers(this->getObservers());
  find_neighbors->execute();
  setErrorCondition(find_neighbors->getErrorCondition());
  if (getErrorCondition() != 0){
    setErrorMessage(find_neighbors->getErrorMessage());
    return;
  }
  size_t xtalCount = m->crystruct.size();
  totalsurfacearea = m_TotalSurfaceArea->WritePointer(0, xtalCount);
  for(size_t i=0;i<xtalCount;i++)
  {
	  totalsurfacearea[i] = find_neighbors->totalsurfacearea[i];
  }
  int numgrains = m->m_Grains.size();
  neighborsurfacearealist.resize(numgrains);
  for(size_t i=0;i<numgrains;i++)
  {
	  neighborsurfacearealist[i] = find_neighbors->neighborsurfacearealist[i];
  }

  find_mdf(h5io);
  notify("FindMDF Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMDF::find_mdf(H5StatsWriter::Pointer h5io)
{
  DataContainer* m = getDataContainer();
  float n1, n2, n3;
  float r1, r2, r3;
  int mbin;
  float w;
  float q1[5];
  float q2[5];
  size_t numgrains = m->m_Grains.size();
  Ebsd::CrystalStructure phase1, phase2;
  float **misobin;
  int numbins = 0;

  misobin = new float *[m->crystruct.size()];
  for(size_t i=1;i<m->crystruct.size();i++)
  {
    if (m->crystruct[i] == Ebsd::Hexagonal)
    {
      numbins = 36 * 36 * 12;
      misobin[i] = new float[numbins];
    }
    else if (m->crystruct[i] == Ebsd::Cubic)
    {
      numbins = 18 * 18 * 18;
      misobin[i] = new float[numbins];
    }
    // Now initialize all bins to 0.0
    for (int j = 0; j < numbins; j++)
    {
      misobin[i][j] = 0.0;
    }
  }
  size_t nname;
  float nsa;
  misorientationlists.resize(numgrains);
  for (size_t i = 1; i < numgrains; i++)
  {
	  if(m->m_Grains[i]->active == true)
	  {
		q1[0] = m->m_Grains[i]->avg_quat[0] / m->m_Grains[i]->avg_quat[0];
		q1[1] = m->m_Grains[i]->avg_quat[1] / m->m_Grains[i]->avg_quat[0];
		q1[2] = m->m_Grains[i]->avg_quat[2] / m->m_Grains[i]->avg_quat[0];
		q1[3] = m->m_Grains[i]->avg_quat[3] / m->m_Grains[i]->avg_quat[0];
		q1[4] = m->m_Grains[i]->avg_quat[4] / m->m_Grains[i]->avg_quat[0];
		phase1 = m->crystruct[m->m_Grains[i]->phase];
		misorientationlists[i].resize(m->m_Grains[i]->neighborlist->size() * 3, -1.0);
		for (size_t j = 0; j < m->m_Grains[i]->neighborlist->size(); j++)
		{
		  w = 10000.0;
		  nname = m->m_Grains[i]->neighborlist->at(j);
		  q2[0] = m->m_Grains[nname]->avg_quat[0] / m->m_Grains[nname]->avg_quat[0];
		  q2[1] = m->m_Grains[nname]->avg_quat[1] / m->m_Grains[nname]->avg_quat[0];
		  q2[2] = m->m_Grains[nname]->avg_quat[2] / m->m_Grains[nname]->avg_quat[0];
		  q2[3] = m->m_Grains[nname]->avg_quat[3] / m->m_Grains[nname]->avg_quat[0];
		  q2[4] = m->m_Grains[nname]->avg_quat[4] / m->m_Grains[nname]->avg_quat[0];
		  phase2 = m->crystruct[m->m_Grains[nname]->phase];
		  if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
		  if (phase1 == phase2)
		  {
			OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
			misorientationlists[i][3 * j] = r1;
			misorientationlists[i][3 * j + 1] = r2;
			misorientationlists[i][3 * j + 2] = r3;
		  }
		  if (phase1 != phase2)
		  {
			misorientationlists[i][3 * j] = -100;
			misorientationlists[i][3 * j + 1] = -100;
			misorientationlists[i][3 * j + 2] = -100;
		  }
		  if (phase1 == phase2) mbin = m_OrientationOps[phase1]->getMisoBin(
																   misorientationlists[i][3*j],
																   misorientationlists[i][3 * j + 1],
																   misorientationlists[i][3 * j + 2]);
		  if ((nname > i || m->m_Grains[nname]->surfacefield == 1) && phase1 == phase2)
		  {
			nsa = neighborsurfacearealist[i][j];
			misobin[m->m_Grains[i]->phase][mbin] = misobin[m->m_Grains[i]->phase][mbin] + (nsa / totalsurfacearea[m->m_Grains[i]->phase]);
		  }
		}
	  }
  }
  unsigned long long int dims = static_cast<unsigned long long int>(numbins);
  for(size_t i=1;i<m->crystruct.size();i++)
  {
	  h5io->writeMisorientationBinsData(i, &dims, misobin[i]);
	  delete[] misobin[i];
  }
  delete[] misobin;
}

