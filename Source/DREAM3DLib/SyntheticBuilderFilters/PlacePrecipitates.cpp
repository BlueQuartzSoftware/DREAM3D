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

#include "PlacePrecipitates.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3DLib/ShapeOps/CylinderOps.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/ShapeOps/SuperEllipsoidOps.h"

#include "DREAM3DLib/SyntheticBuilderFilters/PackGrainsGen2.h"
#include "DREAM3DLib/GenericFilters/FindNeighbors.h"





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PlacePrecipitates::PlacePrecipitates() :
grain_indicies(NULL),
phases(NULL),
surfacevoxels(NULL)
{


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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PlacePrecipitates::~PlacePrecipitates()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PlacePrecipitates::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  DataContainer* m = getDataContainer();

  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  GET_NAMED_ARRAY_SIZE_CHK(m, Voxel, DREAM3D::VoxelData::GrainIds, Int32ArrayType, int32_t, (m->totalpoints), gi);
  GET_NAMED_ARRAY_SIZE_CHK(m, Voxel, DREAM3D::VoxelData::Phases, Int32ArrayType, int32_t, (m->totalpoints), ph);
  GET_NAMED_ARRAY_SIZE_CHK(m, Voxel, DREAM3D::VoxelData::SurfaceVoxels, Int8ArrayType, int8_t, (m->totalpoints), surf);


  grain_indicies = gi;
  phases = ph;
  surfacevoxels = surf;


  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  find_neighbors->setDataContainer(getDataContainer());
  find_neighbors->setObservers(this->getObservers());
  find_neighbors->execute();
  err = find_neighbors->getErrorCondition();

  sizex = m->xpoints * m->resx;
  sizey = m->ypoints * m->resy;
  sizez = m->zpoints * m->resz;

  totalvol = sizex * sizey * sizez;

  place_precipitates();
  fillin_precipitates();

  // If there is an error set this to something negative and also set a message
  notify("PlacePrecipitates Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PlacePrecipitates::insert_precipitate(size_t gnum, float coatingthickness)
{
  DREAM3D_RANDOMNG_NEW()
  DataContainer* m = getDataContainer();
  float dist;
  float inside = -1;
  float coatinginside = -1;
  int index;
  int column, row, plane;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float xp, yp, zp;
  float x, y, z;
  float insidecount = 0;
  float coatinginsidecount = 0;
  float volcur = m->m_Grains[gnum]->volume;
  float bovera = m->m_Grains[gnum]->radius2;
  float covera = m->m_Grains[gnum]->radius3;
  float omega3 = m->m_Grains[gnum]->omega3;
  float radcur1, radcur2, radcur3;
  float coatingradcur1, coatingradcur2, coatingradcur3;
  currentprecipvoxellist.resize(1000);
  currentcoatingvoxellist.resize(1000);

  DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[m->m_Grains[gnum]->phase];
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

  radcur2 = (radcur1*bovera);
  radcur3 = (radcur1*covera);
  coatingradcur1 = radcur1+coatingthickness;
  coatingradcur2 = radcur2+coatingthickness;
  coatingradcur3 = radcur3+coatingthickness;
  float phi1 = m->m_Grains[gnum]->axiseuler1;
  float PHI = m->m_Grains[gnum]->axiseuler2;
  float phi2 = m->m_Grains[gnum]->axiseuler3;
  float ga[3][3];
  ga[0][0] = cosf(phi1)*cosf(phi2)-sinf(phi1)*sinf(phi2)*cosf(PHI);
  ga[0][1] = sinf(phi1)*cosf(phi2)+cosf(phi1)*sinf(phi2)*cosf(PHI);
  ga[0][2] = sinf(phi2)*sinf(PHI);
  ga[1][0] = -cosf(phi1)*sinf(phi2)-sinf(phi1)*cosf(phi2)*cosf(PHI);
  ga[1][1] = -sinf(phi1)*sinf(phi2)+cosf(phi1)*cosf(phi2)*cosf(PHI);
  ga[1][2] =  cosf(phi2)*sinf(PHI);
  ga[2][0] =  sinf(phi1)*sinf(PHI);
  ga[2][1] = -cosf(phi1)*sinf(PHI);
  ga[2][2] =  cosf(PHI);
  xc = m->m_Grains[gnum]->centroidx;
  yc = m->m_Grains[gnum]->centroidy;
  zc = m->m_Grains[gnum]->centroidz;
  column = (xc-(m->resx/2))/m->resx;
  row = (yc-(m->resy/2))/m->resy;
  plane = (zc-(m->resz/2))/m->resz;
  xmin = int(column-((radcur1/m->resx)+1));
  xmax = int(column+((radcur1/m->resx)+1));
  ymin = int(row-((radcur1/m->resy)+1));
  ymax = int(row+((radcur1/m->resy)+1));
  zmin = int(plane-((radcur1/m->resz)+1));
  zmax = int(plane+((radcur1/m->resz)+1));
  if(m_periodic_boundaries == true)
  {
    if(xmin < -m->xpoints) xmin = -m->xpoints;
    if(xmax > 2*m->xpoints-1) xmax = (2*m->xpoints-1);
    if(ymin < -m->ypoints) ymin = -m->ypoints;
    if(ymax > 2*m->ypoints-1) ymax = (2*m->ypoints-1);
    if(zmin < -m->zpoints) zmin = -m->zpoints;
    if(zmax > 2*m->zpoints-1) zmax = (2*m->zpoints-1);
  }
  if(m_periodic_boundaries == false)
  {
    if(xmin < 0) xmin = 0;
    if(xmax > m->xpoints-1) xmax = m->xpoints-1;
    if(ymin < 0) ymin = 0;
    if(ymax > m->ypoints-1) ymax = m->ypoints-1;
    if(zmin < 0) zmin = 0;
    if(zmax > m->zpoints-1) zmax = m->zpoints-1;
  }
  for(int iter1 = xmin; iter1 < xmax+1; iter1++)
  {
    for(int iter2 = ymin; iter2 < ymax+1; iter2++)
    {
		for(int iter3 = zmin; iter3 < zmax+1; iter3++)
		{
		  column = iter1;
		  row = iter2;
		  plane = iter3;
		  if(iter1 < 0) column = iter1+m->xpoints;
		  if(iter1 > m->xpoints-1) column = iter1-m->xpoints;
		  if(iter2 < 0) row = iter2+m->ypoints;
		  if(iter2 > m->ypoints-1) row = iter2-m->ypoints;
		  if(iter3 < 0) plane = iter3+m->zpoints;
		  if(iter3 > m->zpoints-1) plane = iter3-m->zpoints;
		  index = (plane*m->xpoints*m->ypoints)+(row*m->xpoints)+column;
		  inside = -1;
		  coatinginside = -1;
		  x = float(column)*m->resx;
		  y = float(row)*m->resy;
		  z = float(plane)*m->resz;
		  if(iter1 < 0) x = x-sizex;
		  if(iter1 > m->xpoints-1) x = x+sizex;
		  if(iter2 < 0) y = y-sizey;
		  if(iter2 > m->ypoints-1) y = y+sizey;
		  if(iter3 < 0) z = z-sizez;
		  if(iter3 > m->zpoints-1) z = z+sizez;
		  dist = ((x-xc)*(x-xc))+((y-yc)*(y-yc))+((z-zc)*(z-zc));
		  dist = sqrt(dist);
		  if(dist < coatingradcur1)
		  {
			x = x-xc;
			y = y-yc;
			z = z-zc;
			xp = (x*ga[0][0])+(y*ga[0][1])+(z*ga[0][2]);
			yp = (x*ga[1][0])+(y*ga[1][1])+(z*ga[1][2]);
			zp = (x*ga[2][0])+(y*ga[2][1])+(z*ga[2][2]);
			float axis1comp = xp/radcur1;
			float axis2comp = yp/radcur2;
			float axis3comp = zp/radcur3;
			float coatingaxis1comp = xp/coatingradcur1;
			float coatingaxis2comp = yp/coatingradcur2;
			float coatingaxis3comp = zp/coatingradcur3;
		    inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
		    coatinginside = m_ShapeOps[shapeclass]->inside(coatingaxis1comp, coatingaxis2comp, coatingaxis3comp);
			if(inside >= 0)
			{
				int currentpoint = index;
				currentprecipvoxellist[insidecount] = currentpoint;
				insidecount++;
				if (insidecount >= (currentprecipvoxellist.size()))
				{
				  currentprecipvoxellist.resize(insidecount + 1000,-1);
				}
			}
			if(inside < 0 && coatinginside >= 0)
			{
				int currentpoint = index;
				currentcoatingvoxellist[coatinginsidecount] = currentpoint;
				coatinginsidecount++;
				if (coatinginsidecount >= (currentcoatingvoxellist.size()))
				{
				  currentcoatingvoxellist.resize(coatinginsidecount + 1000,-1);
				}
			}
		  }
		}
    }
  }
}




void  PlacePrecipitates::fillin_precipitates()
{
  DataContainer* m = getDataContainer();
  std::vector<int> neighs;
  std::vector<int> remove;
  std::vector<int> gsizes;
  std::vector<int> neighbors;
  int count = 1;
  int good = 1;
  float x, y, z;
  gsizes.resize(m->m_Grains.size(),0);
  neighbors.resize(m->totalpoints,0);
  int neighpoint;
  int neighpoints[6];
  std::vector<int> n(m->m_Grains.size());
  neighpoints[0] = -m->xpoints*m->ypoints;
  neighpoints[1] = -m->xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = m->xpoints;
  neighpoints[5] = m->xpoints*m->ypoints;
  while(count != 0)
  {
    count = 0;
    for(int i = 0; i < (m->xpoints*m->ypoints*m->zpoints); i++)
    {
      int grainname = grain_indicies[i];
      if(grainname <= 0)
      {
      count++;
        for(size_t c = 1; c < m->m_Grains.size(); c++)
        {
          n[c] = 0;
        }
      x = i%m->xpoints;
    y = (i/m->xpoints)%m->ypoints;
    z = i/(m->xpoints*m->ypoints);
      for(int j=0;j<6;j++)
      {
        good = 1;
        neighpoint = i+neighpoints[j];
          if(j == 0 && z == 0) good = 0;
          if(j == 5 && z == (m->zpoints-1)) good = 0;
          if(j == 1 && y == 0) good = 0;
          if(j == 4 && y == (m->ypoints-1)) good = 0;
          if(j == 2 && x == 0) good = 0;
          if(j == 3 && x == (m->xpoints-1)) good = 0;
      if(good == 1)
          {
          int grain = grain_indicies[neighpoint];
          if(grain > 0 && grain >= numprimarygrains)
          {
            neighs.push_back(grain);
          }
        }
        }
        int current = 0;
        int most = 0;
        int curgrain = 0;
        int size = int(neighs.size());
        for(int k=0;k<size;k++)
        {
          int neighbor = neighs[k];
          n[neighbor]++;
          current = n[neighbor];
          if(current > most)
          {
            most = current;
            curgrain = neighbor;
          }
        }
        if(size > 0)
        {
          neighbors[i] = curgrain;
          neighs.clear();
        }
      }
    }
    for(int j = 0; j < (m->xpoints*m->ypoints*m->zpoints); j++)
    {
      int grainname = grain_indicies[j];
      int neighbor = neighbors[j];
      if(grainname <= 0 && neighbor > 0 && neighbor >= numprimarygrains)
      {
        grain_indicies[j] = neighbor;
		phases[j] = m->m_Grains[neighbor]->phase;
      }
    }
  }
  gsizes.resize(m->m_Grains.size(),0);
  for (int i = 0; i < (m->xpoints*m->ypoints*m->zpoints); i++)
  {
    int name = grain_indicies[i];
    gsizes[name]++;
  }
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    m->m_Grains[i]->numvoxels = gsizes[i];
  }
  gsizes.clear();
}

void  PlacePrecipitates::place_precipitates()
{
  DREAM3D_RANDOMNG_NEW()
  DataContainer* m = getDataContainer();
  totalprecipvol = 0;
  int precipvoxelcounter = 0;
  float thickness = 0.5;
  size_t currentnumgrains = m->m_Grains.size();
  numprimarygrains = m->m_Grains.size();
 // size_t index;
  int phase;
  float precipboundaryfraction = 0.0;
  float random;
  int random2;
  float xc, yc, zc;
  double totalprecipitatefractions = 0.0;
  for (size_t i = 1; i < m->phaseType.size();++i)
  {
    if(m->phaseType[i] == DREAM3D::Reconstruction::PrecipitatePhase)
    {
	    precipitatephases.push_back(i);
	    precipitatephasefractions.push_back(m->phasefraction[i]);
	    totalprecipitatefractions = totalprecipitatefractions + m->phasefraction[i];
    }
  }
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    precipitatephasefractions[i] = precipitatephasefractions[i]/totalprecipitatefractions;
    if(i > 0) precipitatephasefractions[i] = precipitatephasefractions[i] + precipitatephasefractions[i-1];
  }
  PackGrainsGen2::Pointer packGrains = PackGrainsGen2::New();
  packGrains->setDataContainer(getDataContainer());
  H5StatsReader::Pointer h5reader = H5StatsReader::New(m_H5StatsFile);
  int err = packGrains->readReconStatsData(h5reader);
  err = packGrains->readAxisOrientationData(h5reader);
  while(totalprecipvol < totalvol*totalprecipitatefractions)
  {
    Seed++;
    random = rg.genrand_res53();
    for (size_t j = 0; j < precipitatephases.size();++j)
    {
      if (random < precipitatephasefractions[j])
      {
        phase = precipitatephases[j];
        break;
      }
    }

    m->m_Grains.resize(currentnumgrains+1);
    m->m_Grains[currentnumgrains] = Field::New();
    packGrains->generate_grain(currentnumgrains, phase, Seed);
    precipboundaryfraction = m->pptFractions[phase];
    random = rg.genrand_res53();
    if(random <= precipboundaryfraction)
    {
		random2 = int(rg.genrand_res53()*double(m->totalpoints-1));
		while(surfacevoxels[random2] == 0 || grain_indicies[random2] > numprimarygrains)
		{
		  random2++;
		  if(random2 >= m->totalpoints) random2 = random2-m->totalpoints;
		}
	}
    else if(random > precipboundaryfraction)
    {
		random2 = rg.genrand_res53()*(m->totalpoints-1);
		while(surfacevoxels[random2] != 0 || grain_indicies[random2] > numprimarygrains)
		{
		  random2++;
		  if(random2 >= m->totalpoints) random2 = random2-m->totalpoints;
		}
    }
    xc = find_xcoord(random2);
    yc = find_ycoord(random2);
    zc = find_zcoord(random2);
    m->m_Grains[currentnumgrains]->centroidx = xc;
    m->m_Grains[currentnumgrains]->centroidy = yc;
    m->m_Grains[currentnumgrains]->centroidz = zc;
    insert_precipitate(currentnumgrains, thickness);

    m->m_Grains[currentnumgrains]->active = 1;
    precipvoxelcounter = 0;
    for(size_t j = 0; j < currentprecipvoxellist.size(); j++)
    {

		if(grain_indicies[currentprecipvoxellist[j]] > 0 && grain_indicies[currentprecipvoxellist[j]] < numprimarygrains)
		{
		  precipvoxelcounter++;
		}
	}
    if(precipvoxelcounter == currentprecipvoxellist.size())
    {
		precipvoxelcounter = 0;
		for(size_t j = 0; j < currentprecipvoxellist.size(); j++)
		{
		    grain_indicies[currentprecipvoxellist[j]] = currentnumgrains;
		    phases[currentprecipvoxellist[j]] = m->m_Grains[currentnumgrains]->phase;
		    precipvoxelcounter++;
		}
		for(size_t j = 0; j < currentcoatingvoxellist.size(); j++)
		{
			if(grain_indicies[currentcoatingvoxellist[j]] < numprimarygrains)
			{
			    phases[currentprecipvoxellist[j]] = 3;
			}
		}
		totalprecipvol = totalprecipvol + (precipvoxelcounter*m->resx*m->resy*m->resz);
		currentnumgrains++;
	}
  }
}
float PlacePrecipitates::find_xcoord(long long int index)
{
  DataContainer* m = getDataContainer();
  float x = m->resx*float(index%m->xpoints);
  return x;
}
float PlacePrecipitates::find_ycoord(long long int index)
{
  DataContainer* m = getDataContainer();
  float y = m->resy*float((index/m->xpoints)%m->ypoints);
  return y;
}
float PlacePrecipitates::find_zcoord(long long int index)
{
  DataContainer* m = getDataContainer();
  float z = m->resz*float(index/(m->xpoints*m->ypoints));
  return z;
}
