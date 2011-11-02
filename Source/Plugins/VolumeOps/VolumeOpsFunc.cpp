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
#include "VolumeOpsFunc.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeOpsFunc::VolumeOpsFunc()
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));

  grain_indicies = NULL;
  phases = NULL;
  euler1s = NULL;
  euler2s = NULL;
  euler3s = NULL;
  grain_indicies_old = NULL;
  phases_old = NULL;
  euler1s_old = NULL;
  euler2s_old = NULL;
  euler3s_old = NULL;

  INIT_AIMARRAY(m_GrainIndicies,int);
  INIT_AIMARRAY(m_Phases,int);
  INIT_AIMARRAY(m_Euler1s,float);
  INIT_AIMARRAY(m_Euler2s,float);
  INIT_AIMARRAY(m_Euler3s,float);
  INIT_AIMARRAY(m_GrainIndicies_old,int);
  INIT_AIMARRAY(m_Phases_old,int);
  INIT_AIMARRAY(m_Euler1s_old,float);
  INIT_AIMARRAY(m_Euler2s_old,float);
  INIT_AIMARRAY(m_Euler3s_old,float);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeOpsFunc::~VolumeOpsFunc()
{

}
void VolumeOpsFunc::initializeAttributes()
{
  grain_indicies = m_GrainIndicies->WritePointer(0, totalpoints);
  phases = m_Phases->WritePointer(0, totalpoints);
  euler1s = m_Euler1s->WritePointer(0, totalpoints);
  euler2s = m_Euler2s->WritePointer(0, totalpoints);
  euler3s = m_Euler3s->WritePointer(0, totalpoints);
  for (int i = 0; i < totalpoints; ++i)
  {
    euler1s[i] = -1.0f;
    euler2s[i] = -1.0f;
    euler3s[i] = -1.0f;
  }
  grain_indicies_old = m_GrainIndicies_old->WritePointer(0, totalpoints_old);
  phases_old = m_Phases_old->WritePointer(0, totalpoints_old);
  euler1s_old = m_Euler1s_old->WritePointer(0, totalpoints_old);
  euler2s_old = m_Euler2s_old->WritePointer(0, totalpoints_old);
  euler3s_old = m_Euler3s_old->WritePointer(0, totalpoints_old);
  for (int i = 0; i < totalpoints_old; ++i)
  {
    euler1s_old[i] = -1.0f;
    euler2s_old[i] = -1.0f;
    euler3s_old[i] = -1.0f;
  }
}

void VolumeOpsFunc::manipulate_volume()
{
	int counter = 0;
	float x, y, z;
	int col, row, plane;
	int index;
	int index_old;
	for (int i = 0; i < xpoints; i++)
	{
		for (int j = 0; j < ypoints; j++)
		{
			for (int k = 0; k < xpoints; k++)
			{
				x = (k * resx) + xstart;
				y = (j * resy) + ystart;
				z = (i * resz) + zstart;
				col = int(x / resx_old);
				row = int(y / resy_old);
				plane = int(z / resz_old);
				index_old = (plane * xpoints_old * ypoints_old) + (row * xpoints_old) + col;
				index = (i * xpoints * ypoints) + (j * xpoints) + k;
				grain_indicies[index] = grain_indicies_old[index_old];
				phases[index] = phases_old[index_old];
				euler1s[index] = euler1s_old[index_old];
				euler2s[index] = euler2s_old[index_old];
				euler3s[index] = euler3s_old[index_old];
			}
		}
	}
}
