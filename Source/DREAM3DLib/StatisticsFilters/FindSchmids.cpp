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

#include "FindSchmids.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::FindSchmids()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::~FindSchmids()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::execute()
{
  setErrorCondition(0);

  find_schmids();
  notify("FindSchmids Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::find_schmids()
{
  DataContainer* m = getDataContainer();
  int ss = 0;
  float q1[5];
  float schmid = 0;
  float loadx, loady, loadz;
  float theta1, theta2, theta3, theta4;
  float lambda1, lambda2, lambda3, lambda4, lambda5, lambda6;
  float schmid1, schmid2, schmid3, schmid4, schmid5, schmid6, schmid7, schmid8, schmid9, schmid10, schmid11, schmid12;
  size_t numgrains = m->m_Grains.size();
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m->m_Grains[i]->active == true)
    {
      q1[1] = m->m_Grains[i]->avg_quat[1]/m->m_Grains[i]->avg_quat[0];
      q1[2] = m->m_Grains[i]->avg_quat[2]/m->m_Grains[i]->avg_quat[0];
      q1[3] = m->m_Grains[i]->avg_quat[3]/m->m_Grains[i]->avg_quat[0];
      q1[4] = m->m_Grains[i]->avg_quat[4]/m->m_Grains[i]->avg_quat[0];
	  if(m->m_Grains[i]->avg_quat[0] == 0) q1[1] = 0, q1[2] = 0, q1[3] = 0, q1[4] = 1;
      loadx = (2 * q1[1] * q1[3] + 2 * q1[2] * q1[4]) * 1;
      loady = (2 * q1[2] * q1[3] - 2 * q1[1] * q1[4]) * 1;
      loadz = (1 - 2 * q1[1] * q1[1] - 2 * q1[2] * q1[2]) * 1;
      float mag = loadx * loadx + loady * loady + loadz * loadz;
      mag = sqrt(mag);
      theta1 = (loadx + loady + loadz) / (mag * 1.732f);
      theta1 = fabs(theta1);
      theta2 = (loadx + loady - loadz) / (mag * 1.732f);
      theta2 = fabs(theta2);
      theta3 = (loadx - loady + loadz) / (mag * 1.732f);
      theta3 = fabs(theta3);
      theta4 = (-loadx + loady + loadz) / (mag * 1.732f);
      theta4 = fabs(theta4);
      lambda1 = (loadx + loady) / (mag * 1.414f);
      lambda1 = fabs(lambda1);
      lambda2 = (loadx + loadz) / (mag * 1.414f);
      lambda2 = fabs(lambda2);
      lambda3 = (loadx - loady) / (mag * 1.414f);
      lambda3 = fabs(lambda3);
      lambda4 = (loadx - loadz) / (mag * 1.414f);
      lambda4 = fabs(lambda4);
      lambda5 = (loady + loadz) / (mag * 1.414f);
      lambda5 = fabs(lambda5);
      lambda6 = (loady - loadz) / (mag * 1.414f);
      lambda6 = fabs(lambda6);
      schmid1 = theta1 * lambda6;
      schmid2 = theta1 * lambda4;
      schmid3 = theta1 * lambda3;
      schmid4 = theta2 * lambda3;
      schmid5 = theta2 * lambda2;
      schmid6 = theta2 * lambda5;
      schmid7 = theta3 * lambda1;
      schmid8 = theta3 * lambda5;
      schmid9 = theta3 * lambda4;
      schmid10 = theta4 * lambda1;
      schmid11 = theta4 * lambda2;
      schmid12 = theta4 * lambda6;
      schmid = schmid1, ss = 0;
      if (schmid2 > schmid) schmid = schmid2, ss = 1;
      if (schmid3 > schmid) schmid = schmid3, ss = 2;
      if (schmid4 > schmid) schmid = schmid4, ss = 3;
      if (schmid5 > schmid) schmid = schmid5, ss = 4;
      if (schmid6 > schmid) schmid = schmid6, ss = 5;
      if (schmid7 > schmid) schmid = schmid7, ss = 6;
      if (schmid8 > schmid) schmid = schmid8, ss = 7;
      if (schmid9 > schmid) schmid = schmid9, ss = 8;
      if (schmid10 > schmid) schmid = schmid10, ss = 9;
      if (schmid11 > schmid) schmid = schmid11, ss = 10;
      if (schmid12 > schmid) schmid = schmid12, ss = 11;
      m->m_Grains[i]->schmidfactor = schmid;
	  m->m_Grains[i]->slipsystem = ss;
    }
  }
}

