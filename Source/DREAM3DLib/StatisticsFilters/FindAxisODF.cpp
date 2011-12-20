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
FindAxisODF::FindAxisODF()
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));

  grainmoments = NULL;
  INIT_DataArray(m_GrainMoments,float);}

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
  setErrorCondition(0);

  H5StatsWriter::Pointer h5io = H5StatsWriter::New(getH5StatsFile());

  FindShapes::Pointer find_shapes = FindShapes::New();
  find_shapes->setDataContainer(getDataContainer());
  find_shapes->setObservers(this->getObservers());
  find_shapes->execute();
  setErrorCondition(find_shapes->getErrorCondition());
  if (getErrorCondition() != 0){
    setErrorMessage(find_shapes->getErrorMessage());
    return;
  }
  int numgrains = m->m_Grains.size();
  grainmoments = m_GrainMoments->WritePointer(0, numgrains * 6);
  for(int i =0; i < numgrains; i++)
  {
	  grainmoments[i*6+0] = find_shapes->grainmoments[i*6+0];
	  grainmoments[i*6+1] = find_shapes->grainmoments[i*6+1];
	  grainmoments[i*6+2] = find_shapes->grainmoments[i*6+2];
	  grainmoments[i*6+3] = find_shapes->grainmoments[i*6+3];
	  grainmoments[i*6+4] = find_shapes->grainmoments[i*6+4];
	  grainmoments[i*6+5] = find_shapes->grainmoments[i*6+5];
  }
  if(m->zpoints > 1) find_axisodf(h5io);
  if(m->zpoints == 1) find_axisodf2D(h5io);
  notify("FindODF Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAxisODF::find_axisodf(H5StatsWriter::Pointer h5io)
{
  DataContainer* m = getDataContainer();
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
    float Ixx = grainmoments[i*6+0];
    float Iyy = grainmoments[i*6+1];
    float Izz = grainmoments[i*6+2];
    float Ixy = grainmoments[i*6+3];
    float Iyz = grainmoments[i*6+4];
    float Ixz = grainmoments[i*6+5];
    float radius1 = m->m_Grains[i]->radius1;
    float radius2 = m->m_Grains[i]->radius2;
    float radius3 = m->m_Grains[i]->radius3;
    float e[3][1];
    float uber[3][3];
    float bmat[3][1];
    float vect[3][3];
    e[0][0] = radius1;
    e[1][0] = radius2;
    e[2][0] = radius3;
    bmat[0][0] = 0.0000001;
    bmat[1][0] = 0.0000001;
    bmat[2][0] = 0.0000001;
    for (int j = 0; j < 3; j++)
    {
      uber[0][0] = Ixx - e[j][0];
      uber[0][1] = Ixy;
      uber[0][2] = Ixz;
      uber[1][0] = Ixy;
      uber[1][1] = Iyy - e[j][0];
      uber[1][2] = Iyz;
      uber[2][0] = Ixz;
      uber[2][1] = Iyz;
      uber[2][2] = Izz - e[j][0];
      float **uberelim;
      float **uberbelim;
      uberelim = new float *[3];
      uberbelim = new float *[3];
      for (int d = 0; d < 3; d++)
      {
        uberelim[d] = new float[3];
        uberbelim[d] = new float[1];
      }
      int elimcount = 0;
      int elimcount1 = 0;
      float q = 0;
      float sum = 0;
      float c = 0;
      for (int a = 0; a < 3; a++)
      {
        elimcount1 = 0;
        for (int b = 0; b < 3; b++)
        {
          uberelim[elimcount][elimcount1] = uber[a][b];
          elimcount1++;
        }
        uberbelim[elimcount][0] = bmat[a][0];
        elimcount++;
      }
      for (int k = 0; k < elimcount - 1; k++)
      {
        for (int l = k + 1; l < elimcount; l++)
        {
          c = uberelim[l][k] / uberelim[k][k];
          for (int r = k + 1; r < elimcount; r++)
          {
            uberelim[l][r] = uberelim[l][r] - c * uberelim[k][r];
          }
          uberbelim[l][0] = uberbelim[l][0] - c * uberbelim[k][0];
        }
      }
      uberbelim[elimcount - 1][0] = uberbelim[elimcount - 1][0] / uberelim[elimcount - 1][elimcount - 1];
      for (int l = 1; l < elimcount; l++)
      {
        int r = (elimcount - 1) - l;
        sum = 0;
        for (int n = r + 1; n < elimcount; n++)
        {
          sum = sum + (uberelim[r][n] * uberbelim[n][0]);
        }
        uberbelim[r][0] = (uberbelim[r][0] - sum) / uberelim[r][r];
      }
      for (int p = 0; p < elimcount; p++)
      {
        q = uberbelim[p][0];
        vect[j][p] = q;
      }
    }
    float n1x = vect[0][0];
    float n1y = vect[0][1];
    float n1z = vect[0][2];
    float n2x = vect[1][0];
    float n2y = vect[1][1];
    float n2z = vect[1][2];
    float n3x = vect[2][0];
    float n3y = vect[2][1];
    float n3z = vect[2][2];
    float norm1 = sqrt(((n1x * n1x) + (n1y * n1y) + (n1z * n1z)));
    float norm2 = sqrt(((n2x * n2x) + (n2y * n2y) + (n2z * n2z)));
    float norm3 = sqrt(((n3x * n3x) + (n3y * n3y) + (n3z * n3z)));
    if (m->m_Grains[i]->surfacefield == 0)
    {
      n1x = n1x / norm1;
      n1y = n1y / norm1;
      n1z = n1z / norm1;
      n2x = n2x / norm2;
      n2y = n2y / norm2;
      n2z = n2z / norm2;
      n3x = n3x / norm3;
      n3y = n3y / norm3;
      n3z = n3z / norm3;
      for (int k = 0; k < 4; k++)
      {
        float o[3][3];
        float ga[3][3];
        float m1[3][3];
        if (k == 0)
        {
          o[0][0] = 1.0;
          o[0][1] = 0.0;
          o[0][2] = 0.0;
          o[1][0] = 0.0;
          o[1][1] = 1.0;
          o[1][2] = 0.0;
          o[2][0] = 0.0;
          o[2][1] = 0.0;
          o[2][2] = 1.0;
        }
        else if (k == 1)
        {
          o[0][0] = -1.0;
          o[0][1] = 0.0;
          o[0][2] = 0.0;
          o[1][0] = 0.0;
          o[1][1] = 1.0;
          o[1][2] = 0.0;
          o[2][0] = 0.0;
          o[2][1] = 0.0;
          o[2][2] = -1.0;
        }
        else if (k == 2)
        {
          o[0][0] = 1.0;
          o[0][1] = 0.0;
          o[0][2] = 0.0;
          o[1][0] = 0.0;
          o[1][1] = -1.0;
          o[1][2] = 0.0;
          o[2][0] = 0.0;
          o[2][1] = 0.0;
          o[2][2] = -1.0;
        }
        else if (k == 3)
        {
          o[0][0] = -1.0;
          o[0][1] = 0.0;
          o[0][2] = 0.0;
          o[1][0] = 0.0;
          o[1][1] = -1.0;
          o[1][2] = 0.0;
          o[2][0] = 0.0;
          o[2][1] = 0.0;
          o[2][2] = 1.0;
        }
        ga[0][0] = n1x;
        ga[0][1] = n2x;
        ga[0][2] = n3x;
        ga[1][0] = n1y;
        ga[1][1] = n2y;
        ga[1][2] = n3y;
        ga[2][0] = n1z;
        ga[2][1] = n2z;
        ga[2][2] = n3z;
        m1[0][0] = o[0][0] * ga[0][0] + o[0][1] * ga[1][0] + o[0][2] * ga[2][0];
        m1[0][1] = o[0][0] * ga[0][1] + o[0][1] * ga[1][1] + o[0][2] * ga[2][1];
        m1[0][2] = o[0][0] * ga[0][2] + o[0][1] * ga[1][2] + o[0][2] * ga[2][2];
        m1[1][0] = o[1][0] * ga[0][0] + o[1][1] * ga[1][0] + o[1][2] * ga[2][0];
        m1[1][1] = o[1][0] * ga[0][1] + o[1][1] * ga[1][1] + o[1][2] * ga[2][1];
        m1[1][2] = o[1][0] * ga[0][2] + o[1][1] * ga[1][2] + o[1][2] * ga[2][2];
        m1[2][0] = o[2][0] * ga[0][0] + o[2][1] * ga[1][0] + o[2][2] * ga[2][0];
        m1[2][1] = o[2][0] * ga[0][1] + o[2][1] * ga[1][1] + o[2][2] * ga[2][1];
        m1[2][2] = o[2][0] * ga[0][2] + o[2][1] * ga[1][2] + o[2][2] * ga[2][2];
        float ea2 = acos(m1[2][2]);
        float cosine3 = (m1[1][2] / sinf(ea2));
        float sine3 = (m1[0][2] / sinf(ea2));
        float cosine1 = (-m1[2][1] / sinf(ea2));
        float sine1 = (m1[2][0] / sinf(ea2));
        float ea3 = acos(cosine3);
        float ea1 = acos(cosine1);
        if (sine3 < 0) ea3 = (2 * m_pi) - ea3;
        if (sine1 < 0) ea1 = (2 * m_pi) - ea1;
        int ea1bin = int(ea1 / (m_pi / 36));
        int ea2bin = int(ea2 / (m_pi / 36));
        int ea3bin = int(ea3 / (m_pi / 36));
        int bin = 0;
        if (ea1 >= 0.0 && ea2 >= 0.0 && ea3 >= 0.0 && ea1 < (m_pi) && ea2 < (m_pi) && ea3 < (m_pi))
        {
          m->m_Grains[i]->axiseuler1 = ea1;
          m->m_Grains[i]->axiseuler2 = ea2;
          m->m_Grains[i]->axiseuler3 = ea3;
          bin = (ea3bin * 36 * 36) + (ea2bin * 36) + (ea1bin);
          axisodf[m->m_Grains[i]->phase][bin]++;
          totalaxes[m->m_Grains[i]->phase]++;
        }
      }
    }
  }
  int err;
  for(size_t i=1;i<m->crystruct.size();i++)
  {
	  err = h5io->writeAxisOrientationData(i, axisodf[i], totalaxes[i]);
	  delete[] axisodf[i];
  }
  delete[] axisodf;
}

void FindAxisODF::find_axisodf2D(H5StatsWriter::Pointer h5io)
{
  DataContainer* m = getDataContainer();
  float **axisodf;
  int *totalaxes;
  axisodf = new float *[m->crystruct.size()];
  totalaxes = new int [m->crystruct.size()];
  axisodf[0] = NULL;
  for(size_t i=1;i<m->crystruct.size();i++)
  {
	  totalaxes[i] = 0.0;
	  axisodf[i] = new float[18 * 18 * 18];
	  for(int j=0;j<(18*18*18);j++)
	  {
		axisodf[i][j] = 0.0;
	  }
  }
  size_t numgrains = m->m_Grains.size();

  for (size_t i = 1; i < numgrains; i++)
  {
    float Ixx = grainmoments[i*6+0];
    float Iyy = grainmoments[i*6+1];
    float Ixy = grainmoments[i*6+2];
    float I1 = (Ixx + Iyy) / 2.0 + sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    float I2 = (Ixx + Iyy) / 2.0 - sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    float n1x = (Ixx - I1) / Ixy;
    float n1y = 1;
    float n2x = (Ixx - I2) / Ixy;
    float n2y = 1;
    float norm1 = sqrt((n1x * n1x + n1y * n1y));
    float norm2 = sqrt((n2x * n2x + n2y * n2y));
    n1x = n1x / norm1;
    n1y = n1y / norm1;
    n2x = n2x / norm2;
    n2y = n2y / norm2;
    float cosine1 = n1x;
    float ea1 = acos(cosine1);
    if (ea1 > m_pi) ea1 = ea1 - m_pi;
    int ea1bin = int(ea1 / (m_pi / 18));
    int bin = 0;
    if (ea1 >= 0.0 && ea1 < (m_pi))
    {
      m->m_Grains[i]->axiseuler1 = ea1;
      bin = ea1bin;
      axisodf[m->m_Grains[i]->phase][bin]++;
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
}

