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

void GrainGeneratorFunc::assign_gaps()
{
  int index;
  int timestep = 100;
  int unassignedcount = 1;
  int column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;
  int xmin, xmax, ymin, ymax, zmin, zmax;

  while (unassignedcount != 0)
  {
	  unassignedcount = 0;
	  timestep = timestep + 5;
	  for (size_t i = 1; i < m_Grains.size(); i++)
	  {
		float volcur = m_Grains[i]->volume;
		float bovera = m_Grains[i]->radius2;
		float covera = m_Grains[i]->radius3;
		float omega3 = m_Grains[i]->omega3;
		xc = m_Grains[i]->centroidx;
		yc = m_Grains[i]->centroidy;
		zc = m_Grains[i]->centroidz;
		float radcur1 = 0.0f;
		//Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
		DREAM3D::SyntheticBuilder::ShapeType shapeclass = shapeTypes[m_Grains[i]->phase];

		// init any values for each of the Shape Ops
		for (std::map<DREAM3D::SyntheticBuilder::ShapeType, DREAM3D::ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops )
		{
		  (*ops).second->init();
		}
		// Create our Argument Map
		std::map<DREAM3D::ShapeOps::ArgName, float> shapeArgMap;
		shapeArgMap[DREAM3D::ShapeOps::Omega3] = omega3;
		shapeArgMap[DREAM3D::ShapeOps::VolCur] = volcur;
		shapeArgMap[DREAM3D::ShapeOps::B_OverA] = bovera;
		shapeArgMap[DREAM3D::ShapeOps::C_OverA] = covera;

		radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

		float radcur2 = (radcur1 * bovera);
		float radcur3 = (radcur1 * covera);
		radcur1 = (float(timestep)/100.0)*radcur1;
		radcur2 = (float(timestep)/100.0)*radcur2;
		radcur3 = (float(timestep)/100.0)*radcur3;
		float phi1 = m_Grains[i]->axiseuler1;
		float PHI = m_Grains[i]->axiseuler2;
		float phi2 = m_Grains[i]->axiseuler3;
		float ga[3][3];
		ga[0][0] = cosf(phi1) * cosf(phi2) - sinf(phi1) * sinf(phi2) * cosf(PHI);
		ga[0][1] = sinf(phi1) * cosf(phi2) + cosf(phi1) * sinf(phi2) * cosf(PHI);
		ga[0][2] = sinf(phi2) * sinf(PHI);
		ga[1][0] = -cosf(phi1) * sinf(phi2) - sinf(phi1) * cosf(phi2) * cosf(PHI);
		ga[1][1] = -sinf(phi1) * sinf(phi2) + cosf(phi1) * cosf(phi2) * cosf(PHI);
		ga[1][2] = cosf(phi2) * sinf(PHI);
		ga[2][0] = sinf(phi1) * sinf(PHI);
		ga[2][1] = -cosf(phi1) * sinf(PHI);
		ga[2][2] = cosf(PHI);
		column = (xc - (resx / 2)) / resx;
		row = (yc - (resy / 2)) / resy;
		plane = (zc - (resz / 2)) / resz;
		xmin = int(column - ((radcur1 / resx) + 1));
		xmax = int(column + ((radcur1 / resx) + 1));
		ymin = int(row - ((radcur1 / resy) + 1));
		ymax = int(row + ((radcur1 / resy) + 1));
		zmin = int(plane - ((radcur1 / resz) + 1));
		zmax = int(plane + ((radcur1 / resz) + 1));
		if (periodic_boundaries == true)
		{
		  if (xmin < -xpoints) xmin = -xpoints;
		  if (xmax > 2 * xpoints - 1) xmax = (2 * xpoints - 1);
		  if (ymin < -ypoints) ymin = -ypoints;
		  if (ymax > 2 * ypoints - 1) ymax = (2 * ypoints - 1);
		  if (zmin < -zpoints) zmin = -zpoints;
		  if (zmax > 2 * zpoints - 1) zmax = (2 * zpoints - 1);
		}
		if (periodic_boundaries == false)
		{
		  if (xmin < 0) xmin = 0;
		  if (xmax > xpoints - 1) xmax = xpoints - 1;
		  if (ymin < 0) ymin = 0;
		  if (ymax > ypoints - 1) ymax = ypoints - 1;
		  if (zmin < 0) zmin = 0;
		  if (zmax > zpoints - 1) zmax = zpoints - 1;
		}
		for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
		{
		  for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
		  {
			for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
			{
			  column = iter1;
			  row = iter2;
			  plane = iter3;
			  if (iter1 < 0) column = iter1 + xpoints;
			  if (iter1 > xpoints - 1) column = iter1 - xpoints;
			  if (iter2 < 0) row = iter2 + ypoints;
			  if (iter2 > ypoints - 1) row = iter2 - ypoints;
			  if (iter3 < 0) plane = iter3 + zpoints;
			  if (iter3 > zpoints - 1) plane = iter3 - zpoints;
			  index = (plane * xpoints * ypoints) + (row * xpoints) + column;
			  if(grain_indicies[index] <= 0)
			  {
				  inside = -1;
				  x = float(column) * resx;
				  y = float(row) * resy;
				  z = float(plane) * resz;
				  if (iter1 < 0) x = x - sizex;
				  if (iter1 > xpoints - 1) x = x + sizex;
				  if (iter2 < 0) y = y - sizey;
				  if (iter2 > ypoints - 1) y = y + sizey;
				  if (iter3 < 0) z = z - sizez;
				  if (iter3 > zpoints - 1) z = z + sizez;
				  dist = ((x - xc) * (x - xc)) + ((y - yc) * (y - yc)) + ((z - zc) * (z - zc));
				  dist = sqrtf(dist);
				  if (dist < radcur1)
				  {
					x = x - xc;
					y = y - yc;
					z = z - zc;
					xp = (x * ga[0][0]) + (y * ga[1][0]) + (z * ga[2][0]);
					yp = (x * ga[0][1]) + (y * ga[1][1]) + (z * ga[2][1]);
					zp = (x * ga[0][2]) + (y * ga[1][2]) + (z * ga[2][2]);
					float axis1comp = xp / radcur1;
					float axis2comp = yp / radcur2;
					float axis3comp = zp / radcur3;
					inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
					if (inside >= 0 && inside > ellipfuncs[index])
					{
						grain_indicies[index] = i;
						ellipfuncs[index] = inside;
					}
				  }
			  }
			}
		  }
		}
	  }
	  for (int i = 0; i < totalpoints; i++)
	  {
		if (grain_indicies[i] <= 0) unassignedcount++;
	  }
  }
  for (int i = 0; i < totalpoints; i++)
  {
	  if(grain_indicies[i] > 0) phases[i] = m_Grains[grain_indicies[i]]->phase;
  }
}
void GrainGeneratorFunc::cleanup_grains()
{
	  int neighpoints[6];
	  neighpoints[0] = -(xpoints * ypoints);
	  neighpoints[1] = -xpoints;
	  neighpoints[2] = -1;
	  neighpoints[3] = 1;
	  neighpoints[4] = xpoints;
	  neighpoints[5] = (xpoints * ypoints);
	  std::vector<std::vector<int> > vlists;
	  vlists.resize(m_Grains.size());
	  std::vector<int> currentvlist;
	  std::vector<bool> checked;
	  checked.resize(totalpoints,false);
	  size_t count;
	  int good;
	  int neighbor;
	  int column, row, plane;
	  int index;
	  float minsize = 0;
	  gsizes.resize(m_Grains.size());
	  for (size_t i = 1; i < m_Grains.size(); i++)
	  {
		gsizes[i] = 0;
	  }
	  for (int i = 0; i < totalpoints; i++)
	  {
		if(checked[i] == false && grain_indicies[i] > 0)
		{
			minsize = mindiameter[phases[i]]*mindiameter[phases[i]]*mindiameter[phases[i]]*M_PI/6.0;
			minsize = int(minsize/(resx*resy*resz));
			currentvlist.push_back(i);
			count = 0;
			while(count < currentvlist.size())
			{
				index = currentvlist[count];
				column = index % xpoints;
				row = (index / xpoints) % ypoints;
				plane = index / (xpoints * ypoints);
				for (int j = 0; j < 6; j++)
				{
					good = 1;
					neighbor = index + neighpoints[j];
					if (periodic_boundaries == false)
					{
						if (j == 0 && plane == 0) good = 0;
						if (j == 5 && plane == (zpoints - 1)) good = 0;
						if (j == 1 && row == 0) good = 0;
						if (j == 4 && row == (ypoints - 1)) good = 0;
						if (j == 2 && column == 0) good = 0;
						if (j == 3 && column == (xpoints - 1)) good = 0;
						if (good == 1 && grain_indicies[neighbor] == grain_indicies[index] && checked[neighbor] == false)
						{
							currentvlist.push_back(neighbor);
							checked[neighbor] = true;
						}
					}
					else if (periodic_boundaries == true)
					{
						if (j == 0 && plane == 0) neighbor = neighbor + (xpoints*ypoints*zpoints);
						if (j == 5 && plane == (zpoints - 1)) neighbor = neighbor - (xpoints*ypoints*zpoints);
						if (j == 1 && row == 0) neighbor = neighbor + (xpoints*ypoints);
						if (j == 4 && row == (ypoints - 1)) neighbor = neighbor - (xpoints*ypoints);
						if (j == 2 && column == 0) neighbor = neighbor + (xpoints);
						if (j == 3 && column == (xpoints - 1)) neighbor = neighbor - (xpoints);
						if (grain_indicies[neighbor] == grain_indicies[index] && checked[neighbor] == false)
						{
							currentvlist.push_back(neighbor);
							checked[neighbor] = true;
						}
					}
				}
				count++;
			}
			size_t size = vlists[grain_indicies[i]].size();
			if(size > 0)
			{
				if(size < currentvlist.size())
				{
					for (size_t k = 0; k < vlists[grain_indicies[i]].size(); k++)
					{
						grain_indicies[vlists[grain_indicies[i]][k]] = -1;
					}
					vlists[grain_indicies[i]].resize(currentvlist.size());
					vlists[grain_indicies[i]].swap(currentvlist);
				}
				else if(size >= currentvlist.size())
				{
					for (size_t k = 0; k < currentvlist.size(); k++)
					{
						grain_indicies[currentvlist[k]] = -1;
					}
				}
			}
			else if(size == 0)
			{
				if(currentvlist.size() >= minsize)
				{
					vlists[grain_indicies[i]].resize(currentvlist.size());
					vlists[grain_indicies[i]].swap(currentvlist);
				}
				if(currentvlist.size() < minsize)
				{
					for (size_t k = 0; k < currentvlist.size(); k++)
					{
						grain_indicies[currentvlist[k]] = -1;
					}
				}
			}
			currentvlist.clear();
		}
	  }
	  for (int i = 0; i < totalpoints; i++)
	  {
		if(grain_indicies[i] > 0) gsizes[grain_indicies[i]]++;
	  }
	  newnames.resize(m_Grains.size());
	  int goodcount = 1;
	  for (size_t i = 1; i < m_Grains.size(); i++)
    {
      newnames[i] = 0;
      if(gsizes[i] > 0)
      {
        m_Grains[goodcount] = m_Grains[i];
        newnames[i] = goodcount;
        goodcount++;
      }
    }
	  for (int i = 0; i < totalpoints; i++)
	  {
		if (grain_indicies[i] > 0)
		{
		  grain_indicies[i] = newnames[grain_indicies[i]];
		}
	  }
	  m_Grains.resize(goodcount);
	  assign_gaps();
}
void GrainGeneratorFunc::adjust_boundaries()
{
  DREAM3D_RANDOMNG_NEW()

  int neighpoints[6];
  neighpoints[0] = -xpoints*ypoints;
  neighpoints[1] = -xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xpoints;
  neighpoints[5] = xpoints*ypoints;
  int iterations = 0;
  size_t selectedgrain = 0;
  int good = 0;
  int growth = 1;
  int nucleus;
  int bad = 0;
  float random, oldsizedisterror, currentsizedisterror, diam;
  int x, y, z;
  int neighpoint, index;
  size_t count, affectedcount;
  int vListSize = 1000;

  float voxtovol = resx*resy*resz*(3.0/4.0)*(1.0/m_pi);

  NEW_SHARED_ARRAY(gsizes, int, m_Grains.size())

  std::vector<int> voxellist(vListSize,-1);
  std::vector<int> affectedvoxellist(vListSize,-1);
  for(size_t i=1;i<m_Grains.size();i++)
  {
    gsizes[i] = 0;
  }
  NEW_SHARED_ARRAY(reassigned, int, totalpoints)

  for(int i=0;i<totalpoints;i++)
  {
    reassigned[i] = 0;
    gsizes[grain_indicies[i]]++;
  }
  PackGrainsGen2::Pointer packGrains = PackGrainsGen2::New();
  packGrains->setGrainGenFunc(this);
  oldsizedisterror = packGrains->check_sizedisterror(-1000,-1000);
  while(iterations < 10000)
  {
    iterations++;
    good = 0;
    while (good == 0)
    {
      good = 1;
      selectedgrain = int(rg.genrand_res53() * m_Grains.size());
      if (selectedgrain >= m_Grains.size()) selectedgrain = m_Grains.size()-1;
      if (selectedgrain == 0) selectedgrain = 1;
      if (m_Grains[selectedgrain]->surfacegrain > 0) good = 0;
    }
    growth = 1;
    random = rg.genrand_res53();
    if(random < 0.5) growth = -1;
    nucleus = 0;
    count = 0;
    affectedcount = 0;
    while(grain_indicies[nucleus] != selectedgrain)
    {
      nucleus++;
      if(nucleus >= totalpoints) selectedgrain++, nucleus = 0;
    }
    voxellist[count] = nucleus;
    count++;
    for(size_t i=0;i<count;++i)
    {
      index = voxellist[i];
      x = index%xpoints;
      y = (index/xpoints)%ypoints;
      z = index/(xpoints*ypoints);
      for(int j=0;j<6;j++)
      {
        good = 1;
        neighpoint = index+neighpoints[j];
        if(j == 0 && z == 0) good = 0;
        if(j == 5 && z == (zpoints-1)) good = 0;
        if(j == 1 && y == 0) good = 0;
        if(j == 4 && y == (ypoints-1)) good = 0;
        if(j == 2 && x == 0) good = 0;
        if(j == 3 && x == (xpoints-1)) good = 0;
        if(good == 1 && grain_indicies[neighpoint] == selectedgrain && reassigned[neighpoint] == 0)
        {
	        voxellist[count] = neighpoint;
	        reassigned[neighpoint] = -1;
	        count++;
	        if(count >= voxellist.size()) voxellist.resize(voxellist.size()+vListSize,-1);
        }
        if(good == 1 && grain_indicies[neighpoint] != selectedgrain && grain_indicies[index] == selectedgrain)
        {
	        if(growth == 1 && reassigned[neighpoint] <= 0)
	        {
	          reassigned[neighpoint] = grain_indicies[neighpoint];
	          grain_indicies[neighpoint] = grain_indicies[index];
	          affectedvoxellist[affectedcount] = neighpoint;
	          affectedcount++;
	          if(affectedcount >= affectedvoxellist.size()) affectedvoxellist.resize(affectedvoxellist.size()+vListSize,-1);
	        }
	        if(growth == -1 && reassigned[neighpoint] <= 0)
	        {
	          reassigned[index] = grain_indicies[index];
	          grain_indicies[index] = grain_indicies[neighpoint];
	          affectedvoxellist[affectedcount] = index;
	          affectedcount++;
	          if(affectedcount >= affectedvoxellist.size()) affectedvoxellist.resize(affectedvoxellist.size()+vListSize,-1);
	        }
        }
      }
    }
    for(size_t i=0;i<affectedcount;i++)
    {
      index = affectedvoxellist[i];
      if(reassigned[index] > 0)
      {
        gsizes[grain_indicies[index]]++;
        gsizes[reassigned[index]] = gsizes[reassigned[index]]-1;
      }
    }
    for(size_t i=1;i<m_Grains.size();i++)
    {
      index = i;
      diam = 2.0*powf((gsizes[index]*voxtovol),(1.0/3.0));
      m_Grains[index]->equivdiameter = diam;
    }
    PackGrainsGen2::Pointer packGrains = PackGrainsGen2::New();
    packGrains->setGrainGenFunc(this);
    currentsizedisterror = packGrains->check_sizedisterror(-1000,-1000);

    if(currentsizedisterror <= oldsizedisterror)
    {
      oldsizedisterror = currentsizedisterror;
      for(size_t i=1;i<m_Grains.size();i++)
      {
        if(gsizes[i] == 0) m_Grains.erase(m_Grains.begin() + i);
      }
    }
    if(currentsizedisterror > oldsizedisterror)
    {
      bad++;
      for(size_t i=0;i<affectedcount;i++)
      {
        index = affectedvoxellist[i];
        if(reassigned[index] > 0)
        {
          gsizes[grain_indicies[index]] = gsizes[grain_indicies[index]]-1;
          grain_indicies[index] = reassigned[index];
          gsizes[grain_indicies[index]]++;
        }
      }
      for(size_t i=1;i<m_Grains.size();i++)
      {
        index = i;
        diam = 2.0*powf((gsizes[index]*voxtovol),(1.0/3.0));
        m_Grains[index]->equivdiameter = diam;
      }
    }
    for(int i=0;i<totalpoints;i++)
    {
      reassigned[i] = 0;
    }
  }
  NEW_SHARED_ARRAY(newnames, int, m_Grains.size())

  for (size_t i=1;i<m_Grains.size();i++)
  {
    newnames[i] = i;
  }
  for(int i=0;i<totalpoints;i++)
  {
    grain_indicies[i] = newnames[grain_indicies[i]];
  }
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


float GrainGeneratorFunc::find_xcoord(long long int index)
{
  float x = resx*float(index%xpoints);
  return x;
}
float GrainGeneratorFunc::find_ycoord(long long int index)
{
  float y = resy*float((index/xpoints)%ypoints);
  return y;
}
float GrainGeneratorFunc::find_zcoord(long long int index)
{
  float z = resz*float(index/(xpoints*ypoints));
  return z;
}





