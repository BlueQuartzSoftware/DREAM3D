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

#include "FindAxisODF.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/StatisticsFilters/FindShapes.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAxisODF::FindAxisODF() :
m_CreateNewStatsFile(true)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAxisODF::~FindAxisODF()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAxisODF::execute()
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

  H5StatsWriter::Pointer h5io = H5StatsWriter::New(getH5StatsFile(), m_CreateNewStatsFile);

  FindShapes::Pointer find_shapes = FindShapes::New();
  find_shapes->setDataContainer(getDataContainer());
  find_shapes->setObservers(this->getObservers());
  find_shapes->execute();
  setErrorCondition(find_shapes->getErrorCondition());
  if (getErrorCondition() != 0){
    setErrorMessage(find_shapes->getErrorMessage());
    return;
  }

  find_axisodf(h5io);
  notify("FindODF Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAxisODF::find_axisodf(H5StatsWriter::Pointer h5io)
{
  DataContainer* m = getDataContainer();
  float r1, r2, r3;
  int bin;
  float **axisodf;
  int *totalaxes;
  axisodf = new float *[m->crystruct.size()];
  totalaxes = new int [m->crystruct.size()];
  axisodf[0] = NULL;
  for(size_t i=1;i<m->crystruct.size();i++)
  {
	  totalaxes[i] = 0.0;
	  axisodf[i] = new float[36*36*36];
	  for(int j=0;j<(36*36*36);j++)
	  {
		axisodf[i][j] = 0.0;
	  }
  }
  size_t numgrains = m->m_Grains.size();
  for (size_t i = 1; i < numgrains; i++)
  {
    float ea1 = m->m_Grains[i]->euler1;
    float ea2 = m->m_Grains[i]->euler2;
	float ea3 = m->m_Grains[i]->euler3;
    if (m->m_Grains[i]->surfacefield == 0)
    {
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
	  m_OrientationOps[Ebsd::OrthoRhombic]->getFZRod(r1, r2, r3);
	  bin = m_OrientationOps[Ebsd::OrthoRhombic]->getOdfBin(r1, r2, r3);
      axisodf[m->m_Grains[i]->phase][bin] = axisodf[m->m_Grains[i]->phase][bin]++;
      totalaxes[m->m_Grains[i]->phase]++;
    }
  }
  int err;
  for(size_t i=1;i<m->crystruct.size();i++)
  {
	  err = h5io->writeAxisOrientationData(i, axisodf[i], totalaxes[i]);
	  delete[] axisodf[i];
  }
  delete[] axisodf;
  delete[] totalaxes;
}
