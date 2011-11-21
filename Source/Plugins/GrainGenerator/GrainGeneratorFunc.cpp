/* ============================================================================
 * Copyright (c) 2009, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2009, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "GrainGeneratorFunc.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>

// EbsdLib Includes
#include "EbsdLib/EbsdConstants.h"

// DREAM3D Includes
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3DLib/ShapeOps/CylinderOps.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/ShapeOps/SuperEllipsoidOps.h"

#include "GrainGenerator/Algorithms/PackGrainsGen2.h"

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

//const static float m_onepointthree = 1.33333333333f;
const static float m_pi = M_PI;
//const static float m_one_over_pi = 1.0/m_pi;
const static float SinOfHalf = sinf(0.5f);
const static float CosOfHalf = cosf(0.5f);
//const static float SinOfZero = sinf(0.0f);
//const static float CosOfZero = cosf(0.0f);


#define DIMS "DIMENSIONS"
#define LOOKUP "LOOKUP_TABLE"

#if 0
// -i C:\Users\GroebeMA\Desktop\NewFolder --outputDir C:\Users\GroebeMA\Desktop\NewFolder -f Slice_ --angMaxSlice 400 -s 1 -e 30 -z 0.25 -t -g 10 -c 0.1 -o 5.0 -x 2
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorFunc::GrainGeneratorFunc()
{
  m_HexOps = HexagonalOps::New();
  m_OrientatioOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientatioOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientatioOps.push_back(m_OrthoOps.get());

  m_EllipsoidOps = DREAM3D::EllipsoidOps::New();
  m_ShapeOps[DREAM3D::SyntheticBuilder::EllipsoidShape] = m_EllipsoidOps.get();
  m_SuprtEllipsoidOps = DREAM3D::SuperEllipsoidOps::New();
  m_ShapeOps[DREAM3D::SyntheticBuilder::SuperEllipsoidShape] = m_SuprtEllipsoidOps.get();
  m_CubicOctohedronOps = DREAM3D::CubeOctohedronOps::New();
  m_ShapeOps[DREAM3D::SyntheticBuilder::CubeOctahedronShape] = m_CubicOctohedronOps.get();
  m_CylinderOps = DREAM3D::CylinderOps::New();
  m_ShapeOps[DREAM3D::SyntheticBuilder::CylinderShape] = m_CylinderOps.get();
  m_UnknownShapeOps = DREAM3D::ShapeOps::New();
  m_ShapeOps[DREAM3D::SyntheticBuilder::UnknownShapeType] = m_UnknownShapeOps.get();

//  voxels.reset(NULL);
  GGseed = MXA::getMilliSeconds();


// Just stuff to quiet the compiler
  float a = SinOfHalf;
  a = CosOfHalf;

  grain_indicies = NULL;
  ellipfuncs = NULL;
  phases = NULL;
  euler1s = NULL;
  euler2s = NULL;
  euler3s = NULL;
  surfacevoxels = NULL;
  totalsurfacearea = NULL;


  INIT_AIMARRAY(m_GrainIndicies,int);
  INIT_AIMARRAY(m_Ellipfuncs,float);
  INIT_AIMARRAY(m_Phases,int);
  INIT_AIMARRAY(m_Euler1s,float);
  INIT_AIMARRAY(m_Euler2s,float);
  INIT_AIMARRAY(m_Euler3s,float);
  INIT_AIMARRAY(m_SurfaceVoxels,char);
  INIT_AIMARRAY(m_TotalSurfaceArea,float);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorFunc::~GrainGeneratorFunc()
{

}


//FIXME: This should return an erorr condition
void GrainGeneratorFunc::write_graindata(const std::string &filename)
{
  std::ofstream outFile;
  outFile.open(filename.c_str(), std::ios_base::binary);
  char space = DREAM3D::GrainData::Delimiter;
  outFile << m_Grains.size()-1 << std::endl;
  outFile << DREAM3D::GrainData::GrainID  << space << DREAM3D::GrainData::PhaseID << space
      << DREAM3D::GrainData::Phi1 << space << DREAM3D::GrainData::PHI<< space << DREAM3D::GrainData::Phi2 << space
      << DREAM3D::GrainData::EquivDiam << space
      << DREAM3D::GrainData::B_Over_A << space << DREAM3D::GrainData::C_Over_A << space << DREAM3D::GrainData::Omega3 << std::endl;

  for (size_t i = 1; i < m_Grains.size(); i++)
  {
	  outFile << i << space << m_Grains[i]->phase << space << m_Grains[i]->euler1 << space << m_Grains[i]->euler2 << space << m_Grains[i]->euler3 <<
		space << m_Grains[i]->equivdiameter << space << m_Grains[i]->radius2 << space << m_Grains[i]->radius3 <<
		space << m_Grains[i]->omega3 << std::endl;
  }
  outFile.close();
}







