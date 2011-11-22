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

#include "AdjustVolume.h"
#include <map>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <cstddef>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>

using namespace std;

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "GrainGenerator/Algorithms/WriteGrainData.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

const static float m_pi = M_PI;

using namespace std;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteGrainData::WriteGrainData() :
    m_ErrorCondition(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteGrainData::~WriteGrainData()
{
}

void WriteGrainData::execute()
{

  DREAM3D_RANDOMNG_NEW()

  write_graindata(getGrainDataFile());

  // If there is an error set this to something negative and also set a message
  m_ErrorMessage = "PackGrainsGen2 Completed";
  m_ErrorCondition = 0;
}
void WriteGrainData::write_graindata(const std::string &filename)
{
  std::ofstream outFile;
  outFile.open(filename.c_str(), std::ios_base::binary);
  char space = DREAM3D::GrainData::Delimiter;
  outFile << m->m_Grains.size()-1 << std::endl;
  outFile << DREAM3D::GrainData::GrainID  << space << DREAM3D::GrainData::PhaseID << space
      << DREAM3D::GrainData::Phi1 << space << DREAM3D::GrainData::PHI<< space << DREAM3D::GrainData::Phi2 << space
      << DREAM3D::GrainData::EquivDiam << space
      << DREAM3D::GrainData::B_Over_A << space << DREAM3D::GrainData::C_Over_A << space << DREAM3D::GrainData::Omega3 << std::endl;

  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
	  outFile << i << space << m->m_Grains[i]->phase << space << m->m_Grains[i]->euler1 << space << m->m_Grains[i]->euler2 << space << m->m_Grains[i]->euler3 <<
		space << m->m_Grains[i]->equivdiameter << space << m->m_Grains[i]->radius2 << space << m->m_Grains[i]->radius3 <<
		space << m->m_Grains[i]->omega3 << std::endl;
  }
  outFile.close();
}







