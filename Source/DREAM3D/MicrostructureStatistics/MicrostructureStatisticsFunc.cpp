/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MicrostructureStatisticsFunc.h"

// C Includes
#include <stdio.h>

// C++ Includes
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <set>

// TSL Ang Library Includes
#include "ANG/AngReader.h"

//-- MXA Includes
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"

//-- AIM Includes
#include "DREAM3D/Common/AIMMath.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/OIMColoring.hpp"
#include "DREAM3D/HDF5/AIM_H5VtkDataWriter.h"
#include "DREAM3D/Parallel/Algo.hpp"

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

const static float m_pi = M_PI;
const static float m_OnePointThree = 1.33333333333;
const static float threesixty_over_pi = 360.0 / m_pi;
const static float m_pi_over_180 = M_PI / 180.0;
const static float sqrt_two = powf(2.0, 0.5);
const static float acos_neg_one = acos(-1.0);
const static float acos_pos_one = acos(1.0);
const static float sin_wmin_neg_1_over_2 = sinf(acos_neg_one / 2.0);
const static float sin_wmin_pos_1_over_2 = sinf(acos_pos_one / 2.0);

#define DIMS "DIMENSIONS"
#define LOOKUP "LOOKUP_TABLE"

using namespace std;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatisticsFunc::MicrostructureStatisticsFunc()
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
MicrostructureStatisticsFunc::~MicrostructureStatisticsFunc()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsFunc::initializeGrains()
{
  /*
   * This assumes a dense packed grain list which probably isn't that safe based
   * on some of the data that is floating around. For our data though this is a
   * reasonable assumption.
   */
  // Put at least 1 Grain in the Vector
  m_Grains.resize(1);
  size_t curGrainSize = 1;
  int grainIndex = 0;
  Grain::Pointer grain;
  for(int i = 0; i < totalpoints; ++i)
  {
    grainIndex = voxels[i].grain_index;
    curGrainSize = m_Grains.size();
    if (grainIndex > m_Grains.size()-1)
    {
      // Resize the Grain Vector to be as large as this index. The other Grain
      // objects will be copied to the resized Vector. This probably isn't really
      // efficient at all.
      m_Grains.resize(grainIndex + 1);
    }
    curGrainSize = m_Grains.size();
    grain = m_Grains[grainIndex];
    if (NULL == grain.get())
    {
      m_Grains[grainIndex] = Grain::New();
      grain = m_Grains[grainIndex];
      // Assign a new voxel list pointer to it
      grain->voxellist = new std::vector<int>(0);
      grain->phase = voxels[i].phase;
    }
    grain->voxellist->push_back(i);
    grain->numvoxels = grain->voxellist->size();
	grain->active = 1;
  }

#if 0
  // Loop over the Grains and initialize them as necessary
  size_t gSize = m_Grains.size();
  for (int g = 0; g < gSize; ++g)
  {

  }
#endif
}


#define GG_INIT_DOUBLE_ARRAY(array, value, size)\
    for(size_t n = 0; n < size; ++n) { array[n] = (value); }

void MicrostructureStatisticsFunc::initializeArrays()
{

  size_t size = crystruct.size();

  pptFractions.resize(size);
  phaseType.resize(size);
  phasefraction.resize(size);

  // Initialize the first slot in these arrays since they should never be used
  phasefraction[0] = 0.0;
  phaseType[0] = AIM::Reconstruction::UnknownPhaseType;
  pptFractions[0] = -1.0;

  mindiameter.resize(size);
  maxdiameter.resize(size);
  totalvol.resize(size);
  totalaxes.resize(size);
}



void MicrostructureStatisticsFunc::find_neighbors()
{
  int neighbors[6];
  neighbors[0] = -(xpoints*ypoints);
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = (xpoints*ypoints);
  float column, row, plane;
  int grain;
  size_t nnum;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;
  size_t xtalCount = crystruct.size();
  totalsurfacearea.resize(xtalCount);
  for(size_t i=1;i<xtalCount;++i)
  {
	totalsurfacearea[i] = 0;
  }
  int surfacegrain = 1;
  int nListSize = 100;
  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    m_Grains[i]->numneighbors = 0;
    if (m_Grains[i]->neighborlist == NULL)
    {
      m_Grains[i]->neighborlist = new std::vector<int>(0);
    }
    m_Grains[i]->neighborlist->assign(nListSize, -1);
    if (m_Grains[i]->neighborsurfacealist == NULL)
    {
      m_Grains[i]->neighborsurfacealist = new std::vector<float>(0);
    }
    m_Grains[i]->neighborsurfacealist->assign(nListSize, -1.0);
    for (int j = 0; j < 3; j++)
    {
      m_Grains[i]->neighbordistfunc[j] = 0;
    }
  }

  NEW_SHARED_ARRAY(gnames, int, totalpoints)
  for (int i = 0; i < totalpoints; i++)
  {
    gnames[i] = voxels[i].grain_index;
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    onsurf = 0;
    grain = gnames[j];
	if(grain > 0)
	{
		column = j%xpoints;
		row = (j/xpoints)%ypoints;
		plane = j/(xpoints*ypoints);
        for(int k=0;k<6;k++)
        {
	      good = 1;
	      neighbor = j+neighbors[k];
          if(k == 0 && plane == 0) good = 0;
          if(k == 5 && plane == (zpoints-1)) good = 0;
          if(k == 1 && row == 0) good = 0;
          if(k == 4 && row == (ypoints-1)) good = 0;
          if(k == 2 && column == 0) good = 0;
          if(k == 3 && column == (xpoints-1)) good = 0;
	      if(good == 1 && gnames[neighbor] != grain && gnames[neighbor] > 0)
          {
	        onsurf++;
	        nnum = m_Grains[grain]->numneighbors;
	        std::vector<int>* nlist = m_Grains[grain]->neighborlist;
	        if (nnum >= (nlist->size()))
	        {
	         nlist->resize(nnum + nListSize);
	        }
	        nlist->at(nnum) = gnames[neighbor];
	        nnum++;
	        m_Grains[grain]->numneighbors = nnum;
	      }
	    }
	}
	voxels[j].surfacevoxel = onsurf;
  }
  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    int phase = m_Grains[i]->phase;
    std::vector<int>* nlist = m_Grains[i]->neighborlist;

    std::map<int, int> neighToCount;
    int numneighs = int(nlist->size());

    // this increments the voxel counts for each grain
    for (int j = 0; j < numneighs; j++)
    {
      neighToCount[nlist->at(j)]++;
    }

    neighToCount.erase(0);
    neighToCount.erase(-1);
    //Resize the grains neighbor list to zero
    m_Grains[i]->neighborlist->resize(0);
    m_Grains[i]->neighborsurfacealist->resize(0);

    for (std::map<int, int>::iterator iter = neighToCount.begin(); iter != neighToCount.end(); ++iter )
    {
      int neigh = iter->first; // get the neighbor grain
      int number = iter->second; // get the number of voxels
      float area = number * resx * resy;
      if (m_Grains[i]->surfacegrain == 0 && (neigh > i || m_Grains[neigh]->surfacegrain == 1))
      {
        totalsurfacearea[phase] = totalsurfacearea[phase] + area;
      }

      // Push the neighbor grain id back onto the list so we stay synced up
      m_Grains[i]->neighborlist->push_back(neigh);
      m_Grains[i]->neighborsurfacealist->push_back(area);
    }
    m_Grains[i]->numneighbors = numneighs;
  }
}

void MicrostructureStatisticsFunc::define_neighborhood()
{
  float x, y, z;
  float xn, yn, zn;
  float xdist, ydist, zdist;
  float dist, dist2, diam, diam2;
  unsigned int dist_int, dist2_int;
  size_t numgrains = m_Grains.size();

  for (size_t i = 1; i < numgrains; i++)
  {
    Grain& grain = *(m_Grains[i].get());
    if (grain.active == 1)
    {
      x = grain.centroidx;
      y = grain.centroidy;
      z = grain.centroidz;
      diam = grain.equivdiameter;
      for (size_t j = i; j < numgrains; j++)
      {
        Grain& grain_j = *(m_Grains[j].get());
        if (grain_j.active == 1)
        {
          xn = grain_j.centroidx;
          yn = grain_j.centroidy;
          zn = grain_j.centroidz;
          diam2 = grain_j.equivdiameter;
          xdist = fabs(x - xn);
          ydist = fabs(y - yn);
          zdist = fabs(z - zn);
          dist = (xdist * xdist) + (ydist * ydist) + (zdist * zdist);
          dist = sqrt(dist);
          dist2 = dist;
          dist_int = int(dist / (diam / 2.0));
          dist2_int = int(dist2 / (diam2 / 2.0));
          if (dist_int < 3)
          {
            for (int iter = dist_int; iter < 3; iter++)
            {
              grain.neighbordistfunc[dist_int]++;
            }
          }
          if (dist2_int < 3)
          {
            for (int iter = dist2_int; iter < 3; iter++)
            {
              grain_j.neighbordistfunc[dist2_int]++;
            }
          }
        }
      }
    }
  }
}

void MicrostructureStatisticsFunc::find_surfacegrains()
{
  int onedge = 0;
  int col, row, plane;
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
    onedge = 0;
    int gnum = voxels[j].grain_index;
    col = j % xpoints;
    row = (j / xpoints) % ypoints;
    plane = j / (xpoints * ypoints);
    if (col <= 0) onedge = 1;
    if (col >= xpoints - 1) onedge = 1;
    if (row <= 0) onedge = 1;
    if (row >= ypoints - 1) onedge = 1;
    if (plane <= 0) onedge = 1;
    if (plane >= zpoints - 1) onedge = 1;
    if(onedge == 1) m_Grains[gnum]->surfacegrain = 1;
  }
}
void MicrostructureStatisticsFunc::find_surfacegrains2D()
{
  int onedge = 0;
  int col, row;
  for (int j = 0; j < (xpoints * ypoints); j++)
  {
    onedge = 0;
    int gnum = voxels[j].grain_index;
    col = j % xpoints;
    row = (j / xpoints) % ypoints;
    if (col <= 0) onedge = 1;
    if (col >= xpoints - 1) onedge = 1;
    if (row <= 0) onedge = 1;
    if (row >= ypoints - 1) onedge = 1;
    if(onedge == 1) m_Grains[gnum]->surfacegrain = 1;
  }
}
void MicrostructureStatisticsFunc::find_centroids()
{
  //  int count = 0;
  int insidegraincount = 0;
  for(size_t i=0;i<crystruct.size();i++)
  {
	  maxdiameter[i] = 0;
	  mindiameter[i] = 100000;
	  phasefraction[i] = 0;
  }
  float x, y, z;
  int col, row, plane;
  float radcubed;
  float diameter;
  size_t numgrains = m_Grains.size();
  graincenters.resize(numgrains);
  for (size_t i = 0; i < numgrains; i++)
  {
    graincenters[i].resize(5);
    for (int j = 0; j < 5; j++)
    {
      graincenters[i][j] = 0;
    }
  }
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
    int gnum = voxels[j].grain_index;
    graincenters[gnum][0]++;
    col = j % xpoints;
    row = (j / xpoints) % ypoints;
    plane = j / (xpoints * ypoints);
	x = float(col)*resx;
	y = float(row)*resy;
	z = float(plane)*resz;
    graincenters[gnum][1] = graincenters[gnum][1] + x;
    graincenters[gnum][2] = graincenters[gnum][2] + y;
    graincenters[gnum][3] = graincenters[gnum][3] + z;
  }
  float res_scalar = resx * resy * resz;
  float vol_term = (4.0/3.0)*m_pi;
  for (size_t i = 1; i < numgrains; i++)
  {
    graincenters[i][1] = graincenters[i][1] / graincenters[i][0];
    graincenters[i][2] = graincenters[i][2] / graincenters[i][0];
    graincenters[i][3] = graincenters[i][3] / graincenters[i][0];
    m_Grains[i]->centroidx = graincenters[i][1];
    m_Grains[i]->centroidy = graincenters[i][2];
    m_Grains[i]->centroidz = graincenters[i][3];
    m_Grains[i]->numvoxels = graincenters[i][0];
    m_Grains[i]->volume = (graincenters[i][0] * res_scalar);
    radcubed = m_Grains[i]->volume/vol_term;
    diameter = 2.0*powf(radcubed, 0.3333333333);
    m_Grains[i]->equivdiameter = diameter;
    if(int(diameter) > maxdiameter[m_Grains[i]->phase]) maxdiameter[m_Grains[i]->phase] = int(diameter);
    if(int(diameter) < mindiameter[m_Grains[i]->phase]) mindiameter[m_Grains[i]->phase] = int(diameter);
	totalvol[m_Grains[i]->phase] = totalvol[m_Grains[i]->phase] + m_Grains[i]->volume;
  }
  for(size_t i=1;i<crystruct.size();i++)
  {
    phasefraction[i] = totalvol[i]/(sizex*sizey*sizez);
  }
}
void MicrostructureStatisticsFunc::find_centroids2D()
{
  //  int count = 0;
  for(size_t i=0;i<crystruct.size();i++)
  {
	  maxdiameter[i] = 0;
	  mindiameter[i] = 100000;
	  phasefraction[i] = 0;
  }
  float x, y;
  int col, row;
  float radsquared;
  float diameter;
  size_t numgrains = m_Grains.size();
  graincenters.resize(numgrains);
  for (size_t i = 0; i < numgrains; i++)
  {
    graincenters[i].resize(5);
    for (int j = 0; j < 5; j++)
    {
      graincenters[i][j] = 0;
    }
  }
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
    int gnum = voxels[j].grain_index;
    graincenters[gnum][0]++;
    col = j % xpoints;
    row = (j / xpoints) % ypoints;
	x = float(col)*resx;
	y = float(row)*resy;
    graincenters[gnum][1] = graincenters[gnum][1] + x;
    graincenters[gnum][2] = graincenters[gnum][2] + y;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    graincenters[i][1] = graincenters[i][1] / graincenters[i][0];
    graincenters[i][2] = graincenters[i][2] / graincenters[i][0];
    m_Grains[i]->centroidx = graincenters[i][1];
    m_Grains[i]->centroidy = graincenters[i][2];
    m_Grains[i]->numvoxels = graincenters[i][0];
    m_Grains[i]->volume = (graincenters[i][0] * resx * resy);
    radsquared = m_Grains[i]->volume / m_pi;
    diameter = (2 * sqrt(radsquared));
    m_Grains[i]->equivdiameter = diameter;
    if (int(diameter) > maxdiameter[m_Grains[i]->phase]) maxdiameter[m_Grains[i]->phase] = int(diameter);
    if (int(diameter) < mindiameter[m_Grains[i]->phase]) mindiameter[m_Grains[i]->phase] = int(diameter);
	totalvol[m_Grains[i]->phase] = totalvol[m_Grains[i]->phase] + m_Grains[i]->volume;
  }
  for(size_t i=1;i<crystruct.size();i++)
  {
    phasefraction[i] = totalvol[i]/(sizex*sizey);
  }
}

void MicrostructureStatisticsFunc::find_euclidean_map()
{

#if AIM_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  tbb::task_group* g = new tbb::task_group;
  g->run(FindEuclideanMap(this, 0));
  g->run(FindEuclideanMap(this, 1));
  g->run(FindEuclideanMap(this, 2));
  g->wait();
  delete g;
  #else
  for (int loop = 0; loop < 3; loop++)
  {
    FindEuclideanMap f(this, loop);
    f();
  }
#endif
}
void MicrostructureStatisticsFunc::find_moments()
{
  //  int count = 0;
  float u200 = 0;
  float u020 = 0;
  float u002 = 0;
  float u110 = 0;
  float u011 = 0;
  float u101 = 0;
  size_t numgrains = m_Grains.size();
  grainmoments.resize(numgrains);
  for (size_t i = 0; i < numgrains; i++)
  {
    grainmoments[i].resize(6);
    for (int j = 0; j < 6; j++)
    {
      grainmoments[i][j] = 0;
    }
  }
  size_t totalPoints = xpoints * ypoints * zpoints;
  for (size_t j = 0; j < totalPoints; j++)
  {
    u200 = 0;
    u020 = 0;
    u002 = 0;
    u110 = 0;
    u011 = 0;
    u101 = 0;
    int gnum = voxels[j].grain_index;
    float x = find_xcoord(j);
    float y = find_ycoord(j);
    float z = find_zcoord(j);
    float x1 = x + (resx / 4);
    float x2 = x - (resx / 4);
    float y1 = y + (resy / 4);
    float y2 = y - (resy / 4);
    float z1 = z + (resz / 4);
    float z2 = z - (resz / 4);
    float xdist1 = (x1 - graincenters[gnum][1]);
    float ydist1 = (y1 - graincenters[gnum][2]);
    float zdist1 = (z1 - graincenters[gnum][3]);
    float xdist2 = (x1 - graincenters[gnum][1]);
    float ydist2 = (y1 - graincenters[gnum][2]);
    float zdist2 = (z2 - graincenters[gnum][3]);
    float xdist3 = (x1 - graincenters[gnum][1]);
    float ydist3 = (y2 - graincenters[gnum][2]);
    float zdist3 = (z1 - graincenters[gnum][3]);
    float xdist4 = (x1 - graincenters[gnum][1]);
    float ydist4 = (y2 - graincenters[gnum][2]);
    float zdist4 = (z2 - graincenters[gnum][3]);
    float xdist5 = (x2 - graincenters[gnum][1]);
    float ydist5 = (y1 - graincenters[gnum][2]);
    float zdist5 = (z1 - graincenters[gnum][3]);
    float xdist6 = (x2 - graincenters[gnum][1]);
    float ydist6 = (y1 - graincenters[gnum][2]);
    float zdist6 = (z2 - graincenters[gnum][3]);
    float xdist7 = (x2 - graincenters[gnum][1]);
    float ydist7 = (y2 - graincenters[gnum][2]);
    float zdist7 = (z1 - graincenters[gnum][3]);
    float xdist8 = (x2 - graincenters[gnum][1]);
    float ydist8 = (y2 - graincenters[gnum][2]);
    float zdist8 = (z2 - graincenters[gnum][3]);
    u200 = u200 + ((ydist1) * (ydist1)) + ((zdist1) * (zdist1)) + ((ydist2) * (ydist2)) + ((zdist2) * (zdist2)) + ((ydist3) * (ydist3)) + ((zdist3) * (zdist3))
        + ((ydist4) * (ydist4)) + ((zdist4) * (zdist4)) + ((ydist5) * (ydist5)) + ((zdist5) * (zdist5)) + ((ydist6) * (ydist6)) + ((zdist6) * (zdist6))
        + ((ydist7) * (ydist7)) + ((zdist7) * (zdist7)) + ((ydist8) * (ydist8)) + ((zdist8) * (zdist8));
    u020 = u020 + ((xdist1) * (xdist1)) + ((zdist1) * (zdist1)) + ((xdist2) * (xdist2)) + ((zdist2) * (zdist2)) + ((xdist3) * (xdist3)) + ((zdist3) * (zdist3))
        + ((xdist4) * (xdist4)) + ((zdist4) * (zdist4)) + ((xdist5) * (xdist5)) + ((zdist5) * (zdist5)) + ((xdist6) * (xdist6)) + ((zdist6) * (zdist6))
        + ((xdist7) * (xdist7)) + ((zdist7) * (zdist7)) + ((xdist8) * (xdist8)) + ((zdist8) * (zdist8));
    u002 = u002 + ((xdist1) * (xdist1)) + ((ydist1) * (ydist1)) + ((xdist2) * (xdist2)) + ((ydist2) * (ydist2)) + ((xdist3) * (xdist3)) + ((ydist3) * (ydist3))
        + ((xdist4) * (xdist4)) + ((ydist4) * (ydist4)) + ((xdist5) * (xdist5)) + ((ydist5) * (ydist5)) + ((xdist6) * (xdist6)) + ((ydist6) * (ydist6))
        + ((xdist7) * (xdist7)) + ((ydist7) * (ydist7)) + ((xdist8) * (xdist8)) + ((ydist8) * (ydist8));
    u110 = u110 + ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4)) + ((xdist5) * (ydist5)) + ((xdist6) * (ydist6))
        + ((xdist7) * (ydist7)) + ((xdist8) * (ydist8));
    u011 = u011 + ((ydist1) * (zdist1)) + ((ydist2) * (zdist2)) + ((ydist3) * (zdist3)) + ((ydist4) * (zdist4)) + ((ydist5) * (zdist5)) + ((ydist6) * (zdist6))
        + ((ydist7) * (zdist7)) + ((ydist8) * (zdist8));
    u101 = u101 + ((xdist1) * (zdist1)) + ((xdist2) * (zdist2)) + ((xdist3) * (zdist3)) + ((xdist4) * (zdist4)) + ((xdist5) * (zdist5)) + ((xdist6) * (zdist6))
        + ((xdist7) * (zdist7)) + ((xdist8) * (zdist8));
    grainmoments[gnum][0] = grainmoments[gnum][0] + u200;
    grainmoments[gnum][1] = grainmoments[gnum][1] + u020;
    grainmoments[gnum][2] = grainmoments[gnum][2] + u002;
    grainmoments[gnum][3] = grainmoments[gnum][3] + u110;
    grainmoments[gnum][4] = grainmoments[gnum][4] + u011;
    grainmoments[gnum][5] = grainmoments[gnum][5] + u101;
  }
  float sphere = (2000.0*m_pi*m_pi)/9.0;
  for (size_t i = 1; i < numgrains; i++)
  {
    grainmoments[i][0] = grainmoments[i][0] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][1] = grainmoments[i][1] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][2] = grainmoments[i][2] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][3] = grainmoments[i][3] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][4] = grainmoments[i][4] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][5] = grainmoments[i][5] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    u200 = (grainmoments[i][1] + grainmoments[i][2] - grainmoments[i][0]) / 2;
    u020 = (grainmoments[i][0] + grainmoments[i][2] - grainmoments[i][1]) / 2;
    u002 = (grainmoments[i][0] + grainmoments[i][1] - grainmoments[i][2]) / 2;
    u110 = grainmoments[i][3];
    u011 = grainmoments[i][4];
    u101 = grainmoments[i][5];
    //    float o3 = (grainmoments[i][0] * grainmoments[i][1] * grainmoments[i][2]) + (2.0 * grainmoments[i][3] * grainmoments[i][5] * grainmoments[i][4])
    //        - (grainmoments[i][0] * grainmoments[i][4] * grainmoments[i][4]) - (grainmoments[i][1] * grainmoments[i][5] * grainmoments[i][5]) - (grainmoments[i][2]
    //        * grainmoments[i][3] * grainmoments[i][3]);
    float o3 = (u200 * u020 * u002) + (2.0 * u110 * u101 * u011) - (u200 * u011 * u011) - (u020 * u101 * u101) - (u002 * u110 * u110);
    float vol5 = m_Grains[i]->volume;
    vol5 = powf(vol5, 5);
    float omega3 = vol5 / o3;
    omega3 = omega3 / sphere;
    if (omega3 > 1) omega3 = 1;
    m_Grains[i]->Ixx = grainmoments[i][0];
    m_Grains[i]->Iyy = grainmoments[i][1];
    m_Grains[i]->Izz = grainmoments[i][2];
    m_Grains[i]->Ixy = -grainmoments[i][3];
    m_Grains[i]->Iyz = -grainmoments[i][4];
    m_Grains[i]->Ixz = -grainmoments[i][5];
    m_Grains[i]->omega3 = omega3;
  }
}
void MicrostructureStatisticsFunc::find_axes()
{
  float I1, I2, I3;
  size_t numgrains = m_Grains.size();
  for (size_t i = 1; i < numgrains; i++)
  {
    float Ixx = m_Grains[i]->Ixx;
    float Iyy = m_Grains[i]->Iyy;
    float Izz = m_Grains[i]->Izz;
    float Ixy = m_Grains[i]->Ixy;
    float Iyz = m_Grains[i]->Iyz;
    float Ixz = m_Grains[i]->Ixz;
    float a = 1;
    float b = -Ixx - Iyy - Izz;
    float c = ((Ixx * Izz) + (Ixx * Iyy) + (Iyy * Izz) - (Ixz * Ixz) - (Ixy * Ixy) - (Iyz * Iyz));
    float d = ((Ixz * Iyy * Ixz) + (Ixy * Izz * Ixy) + (Iyz * Ixx * Iyz) - (Ixx * Iyy * Izz) - (Ixy * Iyz * Ixz) - (Ixy * Iyz * Ixz));
    float f = ((3 * c / a) - ((b / a) * (b / a))) / 3;
    float g = ((2 * (b / a) * (b / a) * (b / a)) - (9 * b * c / (a * a)) + (27 * (d / a))) / 27;
    float h = (g * g / 4) + (f * f * f / 27);
    float rsquare = (g * g / 4) - h;
    float r = sqrt(rsquare);
    float theta = 0;
    if (r == 0)
    {
      theta = 0;
    }
    if (r != 0)
    {
      theta = acos(-g / (2 * r));
    }
    float r1 = 2 * powf(r, 0.33333333333) * cosf(theta / 3) - (b / (3 * a));
    float r2 = -powf(r, 0.33333333333) * (cosf(theta / 3) - (1.7320508 * sinf(theta / 3))) - (b / (3 * a));
    float r3 = -powf(r, 0.33333333333) * (cosf(theta / 3) + (1.7320508 * sinf(theta / 3))) - (b / (3 * a));
    m_Grains[i]->radius1 = r1;
    m_Grains[i]->radius2 = r2;
    m_Grains[i]->radius3 = r3;
    I1 = (15 * r1) / (4 * m_pi);
    I2 = (15 * r2) / (4 * m_pi);
    I3 = (15 * r3) / (4 * m_pi);
    float A = (I1 + I2 - I3) / 2;
    float B = (I1 + I3 - I2) / 2;
    float C = (I2 + I3 - I1) / 2;
    a = (A * A * A * A) / (B * C);
    a = powf(a, 0.1);
    b = B / A;
    b = sqrt(b) * a;
    c = A / (a * a * a * b);
    float bovera = b / a;
    float covera = c / a;
    float coverb = c / b;
    m_Grains[i]->aspectratio1 = bovera;
    m_Grains[i]->aspectratio2 = covera;
  }
}
void MicrostructureStatisticsFunc::find_vectors(H5ReconStatsWriter::Pointer h5io)
{
  float **axisodf;
  axisodf = new float *[crystruct.size()];
  axisodf[0] = NULL;
  for(size_t i=1;i<crystruct.size();i++)
  {
	  totalaxes[i] = 0.0;
	  axisodf[i] = new float[36*36*36];
  }
  size_t numgrains = m_Grains.size();
  for (size_t i = 1; i < numgrains; i++)
  {
    //   int size = grains[i].numvoxels;
    float Ixx = m_Grains[i]->Ixx;
    float Iyy = m_Grains[i]->Iyy;
    float Izz = m_Grains[i]->Izz;
    float Ixy = m_Grains[i]->Ixy;
    float Iyz = m_Grains[i]->Iyz;
    float Ixz = m_Grains[i]->Ixz;
    float radius1 = m_Grains[i]->radius1;
    float radius2 = m_Grains[i]->radius2;
    float radius3 = m_Grains[i]->radius3;
    float m[3][3];
    float e[3][1];
    float uber[3][3];
    float bmat[3][1];
    float vect[3][3];
    m[0][0] = Ixx;
    m[0][1] = Ixy;
    m[0][2] = Ixz;
    m[1][0] = Ixy;
    m[1][1] = Iyy;
    m[1][2] = Iyz;
    m[2][0] = Ixz;
    m[2][1] = Iyz;
    m[2][2] = Izz;
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
          for (int m = k + 1; m < elimcount; m++)
          {
            uberelim[l][m] = uberelim[l][m] - c * uberelim[k][m];
          }
          uberbelim[l][0] = uberbelim[l][0] - c * uberbelim[k][0];
        }
      }
      uberbelim[elimcount - 1][0] = uberbelim[elimcount - 1][0] / uberelim[elimcount - 1][elimcount - 1];
      for (int l = 1; l < elimcount; l++)
      {
        int m = (elimcount - 1) - l;
        sum = 0;
        for (int n = m + 1; n < elimcount; n++)
        {
          sum = sum + (uberelim[m][n] * uberbelim[n][0]);
        }
        uberbelim[m][0] = (uberbelim[m][0] - sum) / uberelim[m][m];
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
    if (m_Grains[i]->surfacegrain == 0)
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
          m_Grains[i]->axiseuler1 = ea1;
          m_Grains[i]->axiseuler2 = ea2;
          m_Grains[i]->axiseuler3 = ea3;
          bin = (ea3bin * 36 * 36) + (ea2bin * 36) + (ea1bin);
          axisodf[m_Grains[i]->phase][bin]++;
          totalaxes[m_Grains[i]->phase]++;
        }
      }
    }
  }
  int err;
  for(size_t i=1;i<crystruct.size();i++)
  {
	  err = h5io->writeAxisOrientationData(i, axisodf[i], totalaxes[i]);
	  delete[] axisodf[i];
  }
  delete[] axisodf;
}

void MicrostructureStatisticsFunc::find_grain_and_kernel_misorientations()
{

  std::vector<int> gNamesVec(totalpoints);
  std::vector<int> unAssignedVec(totalpoints);
  std::vector<float> gamVec(totalpoints);
  int* gnames = &(gNamesVec.front());
  int* unassigned = &(unAssignedVec.front());
  float* gam = &(gamVec.front());

//  IntArray gnames(new int[totalpoints]);
//  IntArray unassigned(new int[totalpoints]);
//  FloatArray gam(new float[totalpoints]);

  float** avgmiso = new float *[m_Grains.size()];
  for (size_t i = 0; i < m_Grains.size(); i++)
  {
    avgmiso[i] = new float[2];
    for (int j = 0; j < 2; j++)
    {
      avgmiso[i][j] = 0.0;
    }
  }
  for (int i = 0; i < totalpoints; ++i)
  {
    gnames[i] = voxels[i].grain_index;
    gam[i] = 0.0;
  }

  float q1[5];
  float q2[5];
  int numVoxel; // number of voxels in the grain...
  int numchecks; // number of voxels in the grain...
  int good = 0;
  int neighbor;
  int point;
  float w, totalmisorientation;
  float n1, n2, n3;
  AIM::Reconstruction::CrystalStructure phase1 = AIM::Reconstruction::UnknownCrystalStructure;
  AIM::Reconstruction::CrystalStructure phase2 = AIM::Reconstruction::UnknownCrystalStructure;

  int steps = 1;
  int jStride;
  int kStride;
  CubicOps c;

  for (int col = 0; col < xpoints; col++)
  {
    for (int row = 0; row < ypoints; row++)
    {
      for (int plane = 0; plane < zpoints; plane++)
      {
        point = (plane * xpoints * ypoints) + (row * xpoints) + col;
        if (gnames[point] > 0)
        {
          totalmisorientation = 0.0;
          numVoxel = 0;
          q1[1] = voxels[point].quat[1];
          q1[2] = voxels[point].quat[2];
          q1[3] = voxels[point].quat[3];
          q1[4] = voxels[point].quat[4];
          phase1 = crystruct[voxels[point].phase];
          for (int j = -steps; j < steps + 1; j++)
          {
            jStride = j * xpoints * ypoints;
            for (int k = -steps; k < steps + 1; k++)
            {
              kStride = k * xpoints;
              for (int l = -steps; l < steps + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if (plane + j < 0) good = 0;
                else if (plane + j > zpoints - 1) good = 0;
                else if (row + k < 0) good = 0;
                else if (row + k > ypoints - 1) good = 0;
                else if (col + l < 0) good = 0;
                else if (col + l > xpoints - 1) good = 0;
                if (good == 1)
                {
                  w = 10000.0;
                  q2[1] = voxels[neighbor].quat[1];
                  q2[2] = voxels[neighbor].quat[2];
                  q2[3] = voxels[neighbor].quat[3];
                  q2[4] = voxels[neighbor].quat[4];
                  phase2 = crystruct[voxels[neighbor].phase];
                  if (phase1 == phase2) {
                    //w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
                    switch(phase1)
                    {
                      case 0: w = m_HexOps->getMisoQuat( q1, q2, n1, n2, n3); break;
                      case 1: w = m_CubicOps->getMisoQuat( q1, q2, n1, n2, n3); break;
                      case 2: w = m_OrthoOps->getMisoQuat( q1, q2, n1, n2, n3); break;
                      default:
                        assert(false);
                    }
                  }
                  if (w < 5.0)
                  {
                    totalmisorientation = totalmisorientation + w;
                    numVoxel++;
                  }
                }
              }
            }
          }
          voxels[point].kernelmisorientation = totalmisorientation / (float)numVoxel;
          if (numVoxel == 0) voxels[point].kernelmisorientation = 0;
          q2[0] = 1;
          q2[1] = m_Grains[gnames[point]]->avg_quat[1] / m_Grains[gnames[point]]->avg_quat[0];
          q2[2] = m_Grains[gnames[point]]->avg_quat[2] / m_Grains[gnames[point]]->avg_quat[0];
          q2[3] = m_Grains[gnames[point]]->avg_quat[3] / m_Grains[gnames[point]]->avg_quat[0];
          q2[4] = m_Grains[gnames[point]]->avg_quat[4] / m_Grains[gnames[point]]->avg_quat[0];
          w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
          voxels[point].grainmisorientation = w;
          gam[point] = w;
          avgmiso[gnames[point]][0]++;
          avgmiso[gnames[point]][1] = avgmiso[gnames[point]][1] + w;
        }
        if (gnames[point] == 0)
        {
          voxels[point].kernelmisorientation = 0;
          voxels[point].grainmisorientation = 0;
          gam[point] = 0;
        }
      }
    }
  }

  size_t grainsSize = m_Grains.size();
  for (size_t i = 1; i < grainsSize; i++)
  {
    m_Grains[i]->averagemisorientation = avgmiso[i][1] / avgmiso[i][0];
  }

  steps = 1;
  for (int col = 0; col < xpoints; col++)
  {
    for (int row = 0; row < ypoints; row++)
    {
      for (int plane = 0; plane < zpoints; plane++)
      {
        point = (plane * xpoints * ypoints) + (row * xpoints) + col;
        if (gnames[point] > 0 && unassigned[point] == 0)
        {
          totalmisorientation = 0.0;
          numchecks = 0;
          for (int j = -steps; j < steps + 1; j++)
          {
            jStride = j * xpoints * ypoints;
            for (int k = -steps; k < steps + 1; k++)
            {
              kStride = k * xpoints;
              for (int l = -steps; l < steps + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if (plane + j < 0) good = 0;
                if (plane + j > zpoints - 1) good = 0;
                if (row + k < 0) good = 0;
                if (row + k > ypoints - 1) good = 0;
                if (col + l < 0) good = 0;
                if (col + l > xpoints - 1) good = 0;
                if (good == 1 && gnames[point] == gnames[neighbor] && unassigned[neighbor] != 1)
                {
                  numchecks++;
                  totalmisorientation = totalmisorientation + fabs(gam[point] - gam[neighbor]);
                }
              }
            }
          }
          voxels[point].misorientationgradient = totalmisorientation / (float)numchecks;
        }
        if (gnames[point] == 0 || unassigned[point] != 0)
        {
          voxels[point].misorientationgradient = 0;
        }
      }
    }
  }

  // Clean up all the heap allocated memory
  for (size_t i = 0; i < m_Grains.size(); i++)
  {
    delete[] avgmiso[i];
  }
  delete avgmiso;
}

void MicrostructureStatisticsFunc::find_moments2D()
{
  //  int count = 0;
  float u200 = 0;
  float u020 = 0;
  float u110 = 0;
  size_t numgrains = m_Grains.size();
  grainmoments.resize(numgrains);
  for (size_t i = 0; i < numgrains; i++)
  {
    grainmoments[i].resize(3);
    for (int j = 0; j < 3; j++)
    {
      grainmoments[i][j] = 0;
    }
  }
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
    u200 = 0;
    u020 = 0;
    u110 = 0;
    int gnum = voxels[j].grain_index;
    float x = find_xcoord(j);
    float y = find_ycoord(j);
    float x1 = x + (resx / 2);
    float x2 = x - (resx / 2);
    float y1 = y + (resy / 2);
    float y2 = y - (resy / 2);
    float xdist1 = (x1 - graincenters[gnum][1]);
    float ydist1 = (y1 - graincenters[gnum][2]);
    float xdist2 = (x1 - graincenters[gnum][1]);
    float ydist2 = (y2 - graincenters[gnum][2]);
    float xdist3 = (x2 - graincenters[gnum][1]);
    float ydist3 = (y1 - graincenters[gnum][2]);
    float xdist4 = (x2 - graincenters[gnum][1]);
    float ydist4 = (y2 - graincenters[gnum][2]);
    u200 = u200 + ((ydist1) * (ydist1)) + ((ydist2) * (ydist2)) + ((ydist3) * (ydist3)) + ((ydist4) * (ydist4));
    u020 = u020 + ((xdist1) * (xdist1)) + ((xdist2) * (xdist2)) + ((xdist3) * (xdist3)) + ((xdist4) * (xdist4));
    u110 = u110 + ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4));
    grainmoments[gnum][0] = grainmoments[gnum][0] + u200;
    grainmoments[gnum][1] = grainmoments[gnum][1] + u020;
    grainmoments[gnum][2] = grainmoments[gnum][2] + u110;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    grainmoments[i][0] = grainmoments[i][0] * (resx / 2.0) * (resy / 2.0);
    grainmoments[i][1] = grainmoments[i][1] * (resx / 2.0) * (resy / 2.0);
    grainmoments[i][2] = grainmoments[i][2] * (resx / 2.0) * (resy / 2.0);
    //  float o3 = (grainmoments[i][0]*grainmoments[i][1]*grainmoments[i][2])+(2.0*grainmoments[i][3]*grainmoments[i][5]*grainmoments[i][4])-(grainmoments[i][0]*grainmoments[i][4]*grainmoments[i][4])-(grainmoments[i][1]*grainmoments[i][5]*grainmoments[i][5])-(grainmoments[i][2]*grainmoments[i][3]*grainmoments[i][3]);
    //  float vol5 = m_Grains[i]->volume;
    //  vol5 = powf(vol5,5);
    //  float omega3 = vol5/o3;
    m_Grains[i]->Ixx = grainmoments[i][0];
    m_Grains[i]->Iyy = grainmoments[i][1];
    m_Grains[i]->Ixy = -grainmoments[i][2];
    //  m_Grains[i]->omega3 = omega3;
  }
}
void MicrostructureStatisticsFunc::find_axes2D()
{
  size_t numgrains = m_Grains.size();
  for (size_t i = 1; i < numgrains; i++)
  {
    float Ixx = m_Grains[i]->Ixx;
    float Iyy = m_Grains[i]->Iyy;
    float Ixy = m_Grains[i]->Ixy;
    float r1 = (Ixx + Iyy) / 2.0 + sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    float r2 = (Ixx + Iyy) / 2.0 - sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    float preterm = 4 / 3.1415926535897;
    preterm = powf(preterm, 0.25);
    float postterm1 = r1 * r1 * r1 / r2;
    float postterm2 = r2 * r2 * r2 / r1;
    postterm1 = powf(postterm1, 0.125);
    postterm2 = powf(postterm2, 0.125);
    r1 = preterm * postterm1;
    r2 = preterm * postterm2;
    m_Grains[i]->radius1 = r1;
    m_Grains[i]->radius2 = r2;
  }
}
void MicrostructureStatisticsFunc::find_vectors2D(H5ReconStatsWriter::Pointer h5io)
{
  float **axisodf;
  axisodf = new float *[crystruct.size()];
  for(size_t i=1;i<crystruct.size();i++)
  {
	  totalaxes[i] = 0.0;
	  axisodf[i] = new float[18 * 18 * 18];
  }
  size_t numgrains = m_Grains.size();

  for (size_t i = 1; i < numgrains; i++)
  {
    //   int size = grains[i].numvoxels;
    float Ixx = m_Grains[i]->Ixx;
    float Iyy = m_Grains[i]->Iyy;
    float Ixy = m_Grains[i]->Ixy;
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
      m_Grains[i]->axiseuler1 = ea1;
      bin = ea1bin;
      axisodf[m_Grains[i]->phase][bin]++;
      totalaxes[m_Grains[i]->phase]++;
    }
  }
  int err;

  for(size_t i=1;i<crystruct.size();i++)
  {
	  err = h5io->writeAxisOrientationData(i, axisodf[i], totalaxes[i]);
	  delete[] axisodf[i];
  }
  delete[] axisodf;
}
void MicrostructureStatisticsFunc::find_grainorientations()
{
  size_t numgrains = m_Grains.size();
  int phase;
  AIM::Reconstruction::CrystalStructure xtal;
  for (size_t i = 1; i < numgrains; i++)
  {
	  m_Grains[i]->avg_quat[0] = 0.0;
	  m_Grains[i]->avg_quat[1] = 0.0;
	  m_Grains[i]->avg_quat[2] = 0.0;
	  m_Grains[i]->avg_quat[3] = 0.0;
	  m_Grains[i]->avg_quat[4] = 0.0;
  }
  float qr[5];
  for(size_t i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    OrientationMath::eulertoQuat(qr, voxels[i].euler1, voxels[i].euler2, voxels[i].euler3);
    phase = voxels[i].phase;
    xtal = crystruct[phase];
    m_OrientationOps[xtal]->getFZQuat(qr);
    voxels[i].quat[0] = 1.0;
    voxels[i].quat[1] = qr[1];
    voxels[i].quat[2] = qr[2];
    voxels[i].quat[3] = qr[3];
    voxels[i].quat[4] = qr[4];
	m_OrientationOps[xtal]->getNearestQuat(m_Grains[voxels[i].grain_index]->avg_quat, voxels[i].quat);
    for (int k = 0; k < 5; k++)
    {
	  m_Grains[voxels[i].grain_index]->avg_quat[k] = m_Grains[voxels[i].grain_index]->avg_quat[k] + voxels[i].quat[k];
    }
  }
  float q[5];
  float ea1, ea2, ea3;
  for (size_t i = 1; i < numgrains; i++)
  {
      q[1] = m_Grains[i]->avg_quat[1]/m_Grains[i]->avg_quat[0];
      q[2] = m_Grains[i]->avg_quat[2]/m_Grains[i]->avg_quat[0];
      q[3] = m_Grains[i]->avg_quat[3]/m_Grains[i]->avg_quat[0];
      q[4] = m_Grains[i]->avg_quat[4]/m_Grains[i]->avg_quat[0];
	  OrientationMath::QuattoEuler(q, ea1, ea2, ea3);
	  m_Grains[i]->euler1 = ea1;
      m_Grains[i]->euler2 = ea2;
      m_Grains[i]->euler3 = ea3;
  }
}

void MicrostructureStatisticsFunc::find_eulerodf(H5ReconStatsWriter::Pointer h5io)
{
  size_t bin;
  size_t numgrains = m_Grains.size();
  int phase;
  AIM::Reconstruction::CrystalStructure xtal;
  float **eulerodf;

  eulerodf = new float *[crystruct.size()];
  unsigned long long dims = 0;
  for(unsigned long long i=1;i<crystruct.size();i++)
  {
	  if (crystruct[i] == AIM::Reconstruction::Hexagonal)
	  {
	    dims = 36 * 36 * 12;
	    eulerodf[i] = new float[dims];
	    for (unsigned long long j = 0; j < dims; j++)
	    {
	      eulerodf[i][j] = 0.0;
	    }
	  }
	  else if (crystruct[i] == AIM::Reconstruction::Cubic)
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
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_Grains[i]->surfacegrain == 0 && m_Grains[i]->active == 1)
    {
      float vol = m_Grains[i]->volume;
      ea1 = m_Grains[i]->euler1;
      ea2 = m_Grains[i]->euler2;
      ea3 = m_Grains[i]->euler3;
      phase = crystruct[m_Grains[i]->phase];
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
      bin = m_OrientationOps[phase]->getOdfBin(r1, r2, r3);
      eulerodf[m_Grains[i]->phase][bin] = eulerodf[m_Grains[i]->phase][bin] + (vol / totalvol[m_Grains[i]->phase]);
    }
  }
  int err;
  for(size_t i=1;i<crystruct.size();i++)
  {
	  err = h5io->writeODFData(i, &dims, eulerodf[i]);
	  delete[] eulerodf[i];
  }
  delete[] eulerodf;
}


void MicrostructureStatisticsFunc::measure_misorientations(H5ReconStatsWriter::Pointer h5io)
{
  float n1, n2, n3;
  float r1, r2, r3;
  int mbin;
  float w;
  float q1[5];
  float q2[5];
  size_t numgrains = m_Grains.size();
  AIM::Reconstruction::CrystalStructure phase1, phase2;
  float **misobin;
  int numbins = 0;

  misobin = new float *[crystruct.size()];
  for(size_t i=1;i<crystruct.size();i++)
  {
    if (crystruct[i] == AIM::Reconstruction::Hexagonal)
    {
      numbins = 36 * 36 * 12;
      misobin[i] = new float[numbins];
    }
    else if (crystruct[i] == AIM::Reconstruction::Cubic)
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

  for (size_t i = 1; i < numgrains; i++)
  {
    q1[0] = m_Grains[i]->avg_quat[0] / m_Grains[i]->avg_quat[0];
    q1[1] = m_Grains[i]->avg_quat[1] / m_Grains[i]->avg_quat[0];
    q1[2] = m_Grains[i]->avg_quat[2] / m_Grains[i]->avg_quat[0];
    q1[3] = m_Grains[i]->avg_quat[3] / m_Grains[i]->avg_quat[0];
    q1[4] = m_Grains[i]->avg_quat[4] / m_Grains[i]->avg_quat[0];
    phase1 = crystruct[m_Grains[i]->phase];
    m_Grains[i]->misorientationlist = new std::vector<float>(m_Grains[i]->neighborlist->size() * 3, -1.0);
    for (size_t j = 0; j < m_Grains[i]->neighborlist->size(); j++)
    {
      w = 10000.0;
      nname = m_Grains[i]->neighborlist->at(j);
      q2[0] = m_Grains[nname]->avg_quat[0] / m_Grains[nname]->avg_quat[0];
      q2[1] = m_Grains[nname]->avg_quat[1] / m_Grains[nname]->avg_quat[0];
      q2[2] = m_Grains[nname]->avg_quat[2] / m_Grains[nname]->avg_quat[0];
      q2[3] = m_Grains[nname]->avg_quat[3] / m_Grains[nname]->avg_quat[0];
      q2[4] = m_Grains[nname]->avg_quat[4] / m_Grains[nname]->avg_quat[0];
      phase2 = crystruct[m_Grains[nname]->phase];
      if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
      if (phase1 == phase2)
      {
        OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
        m_Grains[i]->misorientationlist->at(3 * j) = r1;
        m_Grains[i]->misorientationlist->at(3 * j + 1) = r2;
        m_Grains[i]->misorientationlist->at(3 * j + 2) = r3;
      }
      if (phase1 != phase2)
      {
        m_Grains[i]->misorientationlist->at(3 * j) = -100;
        m_Grains[i]->misorientationlist->at(3 * j + 1) = -100;
        m_Grains[i]->misorientationlist->at(3 * j + 2) = -100;
      }
      if (phase1 == phase2) mbin = m_OrientationOps[phase1]->getMisoBin(
                                                               m_Grains[i]->misorientationlist->at(3*j),
                                                               m_Grains[i]->misorientationlist->at(3 * j + 1),
                                                               m_Grains[i]->misorientationlist->at(3 * j + 2));
      if ((nname > i || m_Grains[nname]->surfacegrain == 1) && phase1 == phase2)
      {
        nsa = m_Grains[i]->neighborsurfacealist->at(j);
        misobin[m_Grains[i]->phase][mbin] = misobin[m_Grains[i]->phase][mbin] + (nsa / totalsurfacearea[m_Grains[i]->phase]);
      }
    }
  }
  unsigned long long int dims = static_cast<unsigned long long int>(numbins);
  for(size_t i=1;i<crystruct.size();i++)
  {
	  h5io->writeMisorientationBinsData(i, &dims, misobin[i]);
	  delete[] misobin[i];
  }
  delete[] misobin;
}

void MicrostructureStatisticsFunc::find_colors()
{
  size_t numgrains = m_Grains.size();
  //  float red, green, blue;
  unsigned char rgb[3] = { 0, 0, 0 };
  unsigned char hkl[3] = { 0, 0, 0 };
  float RefDirection[3] = { 0.0, 1.0, 0.0 };
  float q1[5];
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_Grains[i]->active == 1)
    {
      float g1ea1 = m_Grains[i]->euler1;
      float g1ea2 = m_Grains[i]->euler2;
      float g1ea3 = m_Grains[i]->euler3;
      q1[0] = m_Grains[i]->avg_quat[0] / m_Grains[i]->avg_quat[0];
      q1[1] = m_Grains[i]->avg_quat[1] / m_Grains[i]->avg_quat[0];
      q1[2] = m_Grains[i]->avg_quat[2] / m_Grains[i]->avg_quat[0];
      q1[3] = m_Grains[i]->avg_quat[3] / m_Grains[i]->avg_quat[0];
      q1[4] = m_Grains[i]->avg_quat[4] / m_Grains[i]->avg_quat[0];
      if (crystruct[m_Grains[i]->phase] == AIM::Reconstruction::Cubic)
      {
        OIMColoring::GenerateIPFColor(g1ea1, g1ea2, g1ea3, RefDirection[0], RefDirection[1], RefDirection[2], rgb, hkl);
        m_Grains[i]->red = static_cast<float> (rgb[0] / 255.0);
        m_Grains[i]->green = static_cast<float> (rgb[1] / 255.0);
        m_Grains[i]->blue = static_cast<float> (rgb[2] / 255.0);
        m_Grains[i]->IPF[0] = static_cast<float> (hkl[0] / 100.0);
        m_Grains[i]->IPF[1] = static_cast<float> (hkl[1] / 100.0);
        m_Grains[i]->IPF[2] = static_cast<float> (hkl[2] / 100.0);
      }
      if (crystruct[m_Grains[i]->phase] == AIM::Reconstruction::Hexagonal)
      {
        OIMColoring::CalculateHexIPFColor(q1, RefDirection[0], RefDirection[1], RefDirection[2], rgb);
        m_Grains[i]->red = rgb[0] / 255.0;
        m_Grains[i]->green = rgb[1] / 255.0;
        m_Grains[i]->blue = rgb[2] / 255.0;
      }
    }
  }
}
void MicrostructureStatisticsFunc::find_schmids()
{
  int ss = 0;
  float q1[5];
  float schmid = 0;
  float loadx, loady, loadz;
  float theta1, theta2, theta3, theta4;
  float lambda1, lambda2, lambda3, lambda4, lambda5, lambda6;
  float schmid1, schmid2, schmid3, schmid4, schmid5, schmid6, schmid7, schmid8, schmid9, schmid10, schmid11, schmid12;
  size_t numgrains = m_Grains.size();
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_Grains[i]->active == 1)
    {
      q1[1] = m_Grains[i]->avg_quat[1]/m_Grains[i]->avg_quat[0];
      q1[2] = m_Grains[i]->avg_quat[2]/m_Grains[i]->avg_quat[0];
      q1[3] = m_Grains[i]->avg_quat[3]/m_Grains[i]->avg_quat[0];
      q1[4] = m_Grains[i]->avg_quat[4]/m_Grains[i]->avg_quat[0];
      loadx = (2 * q1[1] * q1[3] + 2 * q1[2] * q1[4]) * 1;
      loady = (2 * q1[2] * q1[3] - 2 * q1[1] * q1[4]) * 1;
      loadz = (1 - 2 * q1[1] * q1[1] - 2 * q1[2] * q1[2]) * 1;
      float mag = loadx * loadx + loady * loady + loadz * loadz;
      mag = sqrt(mag);
      theta1 = (loadx + loady + loadz) / (mag * 1.732);
      theta1 = fabs(theta1);
      theta2 = (loadx + loady - loadz) / (mag * 1.732);
      theta2 = fabs(theta2);
      theta3 = (loadx - loady + loadz) / (mag * 1.732);
      theta3 = fabs(theta3);
      theta4 = (-loadx + loady + loadz) / (mag * 1.732);
      theta4 = fabs(theta4);
      lambda1 = (loadx + loady) / (mag * 1.414);
      lambda1 = fabs(lambda1);
      lambda2 = (loadx + loadz) / (mag * 1.414);
      lambda2 = fabs(lambda2);
      lambda3 = (loadx - loady) / (mag * 1.414);
      lambda3 = fabs(lambda3);
      lambda4 = (loadx - loadz) / (mag * 1.414);
      lambda4 = fabs(lambda4);
      lambda5 = (loady + loadz) / (mag * 1.414);
      lambda5 = fabs(lambda5);
      lambda6 = (loady - loadz) / (mag * 1.414);
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
      m_Grains[i]->schmidfactor = schmid;
	  m_Grains[i]->slipsystem = ss;
    }
  }
}

int MicrostructureStatisticsFunc::volume_stats(H5ReconStatsWriter::Pointer h5io, float sizebinstepsize)
{
  int retErr = 0;
  float actualgrains = 0;
  float avglogdiam = 0;
//  float neighcount = 0;
  size_t numgrains = m_Grains.size();
  std::vector<std::vector<float> > neighborhood;
  std::vector<std::vector<float> > neighborhoodfit;
  std::vector<std::vector<float> > svbovera;
  std::vector<std::vector<float> > svcovera;
  std::vector<std::vector<float> > svcoverb;
  std::vector<std::vector<float> > svschmid;
  std::vector<std::vector<float> > svomega3;
  for(size_t iter=1;iter<crystruct.size();iter++)
  {
	  int numbins = int((maxdiameter[iter] - mindiameter[iter]) / sizebinstepsize) + 1;
	  neighborhood.resize(numbins);
	  neighborhoodfit.resize(numbins);
	  svbovera.resize(numbins);
	  svcovera.resize(numbins);
	  svcoverb.resize(numbins);
	  svschmid.resize(numbins);
	  svomega3.resize(numbins);
	  for (int temp = 0; temp < numbins; temp++)
	  {
		neighborhood[temp].resize(7, 0);
		neighborhoodfit[temp].resize(4, 0);
		svbovera[temp].resize(5, 0);
		svcovera[temp].resize(5, 0);
		svcoverb[temp].resize(5, 0);
		svschmid[temp].resize(5, 0);
		svomega3[temp].resize(5, 0);
	  }
	  for (size_t i = 1; i < numgrains; i++)
	  {
		int onedge = m_Grains[i]->surfacegrain;
		if (onedge == 0 && m_Grains[i]->active == 1 && m_Grains[i]->phase == static_cast<int>(iter) )
		{
		  actualgrains++;
		  float diam = m_Grains[i]->equivdiameter;
		  float logdiam = log(diam);
		  float I1 = m_Grains[i]->radius1;
		  float I2 = m_Grains[i]->radius2;
		  float I3 = m_Grains[i]->radius3;
		  float bovera = m_Grains[i]->aspectratio1;
		  float covera = m_Grains[i]->aspectratio2;
		  float coverb = covera/bovera;
		  float schmid = m_Grains[i]->schmidfactor;
		  float omega3 = m_Grains[i]->omega3;
		  avglogdiam = avglogdiam + logdiam;
		  int diamint = int((diam - mindiameter[iter]) / sizebinstepsize);
		  neighborhood[diamint][0]++;
		  svbovera[diamint][0]++;
		  svcovera[diamint][0]++;
		  svcoverb[diamint][0]++;
		  svschmid[diamint][0]++;
		  svomega3[diamint][0]++;
		  svbovera[diamint][1] = svbovera[diamint][1] + bovera;
		  svcovera[diamint][1] = svcovera[diamint][1] + covera;
		  svcoverb[diamint][1] = svcoverb[diamint][1] + coverb;
		  svschmid[diamint][1] = svschmid[diamint][1] + schmid;
		  svomega3[diamint][1] = svomega3[diamint][1] + omega3;
		  neighborhood[diamint][0]++;
		  for (int k = 0; k < 3; k++)
		  {
			int nnum = m_Grains[i]->neighbordistfunc[k];
			neighborhood[diamint][((2 * k) + 1)] = neighborhood[diamint][((2 * k) + 1)] + nnum;
		  }
		}
	  }
	  for (int temp3 = 0; temp3 < numbins; temp3++)
	  {
		if (svbovera[temp3][0] > 1)
		{
		  neighborhood[temp3][1] = neighborhood[temp3][1] / neighborhood[temp3][0];
		  neighborhood[temp3][3] = neighborhood[temp3][3] / neighborhood[temp3][0];
		  neighborhood[temp3][5] = neighborhood[temp3][5] / neighborhood[temp3][0];
		  svbovera[temp3][1] = svbovera[temp3][1] / svbovera[temp3][0];
		  svcovera[temp3][1] = svcovera[temp3][1] / svcovera[temp3][0];
		  svcoverb[temp3][1] = svcoverb[temp3][1] / svcoverb[temp3][0];
		  svschmid[temp3][1] = svschmid[temp3][1] / svschmid[temp3][0];
		  svomega3[temp3][1] = svomega3[temp3][1] / svomega3[temp3][0];
		  neighborhoodfit[temp3][0] = neighborhood[temp3][0];
		  neighborhoodfit[temp3][1] = neighborhood[temp3][3] - neighborhood[temp3][1];
		  neighborhoodfit[temp3][2] = neighborhood[temp3][1];
		  neighborhoodfit[temp3][3] = log((neighborhood[temp3][5] - neighborhoodfit[temp3][2]) / neighborhoodfit[temp3][1]) / log(2.0);
		}
	  }
	  avglogdiam = avglogdiam / actualgrains;
	  float sdlogdiam = 0;
	  for (size_t j = 1; j < numgrains; j++)
	  {
		int onedge = m_Grains[j]->surfacegrain;
		if (onedge == 0 && m_Grains[j]->active == 1 && m_Grains[j]->phase == static_cast<int>(iter) )
		{
		  float diam = m_Grains[j]->equivdiameter;
		  float logdiam = log(diam);
		  float I1 = m_Grains[j]->radius1;
		  float I2 = m_Grains[j]->radius2;
		  float I3 = m_Grains[j]->radius3;
		  float bovera = m_Grains[j]->aspectratio1;
		  float covera = m_Grains[j]->aspectratio2;
		  float coverb = covera/bovera;
		  float schmid = m_Grains[j]->schmidfactor;
		  float omega3 = m_Grains[j]->omega3;
		  sdlogdiam = sdlogdiam + ((logdiam - avglogdiam) * (logdiam - avglogdiam));
		  int diamint = int((diam - mindiameter[iter]) / sizebinstepsize);
		  svbovera[diamint][2] = svbovera[diamint][2] + ((bovera - svbovera[diamint][1]) * (bovera - svbovera[diamint][1]));
		  svcovera[diamint][2] = svcovera[diamint][2] + ((covera - svcovera[diamint][1]) * (covera - svcovera[diamint][1]));
		  svcoverb[diamint][2] = svcoverb[diamint][2] + ((coverb - svcoverb[diamint][1]) * (coverb - svcoverb[diamint][1]));
		  svschmid[diamint][2] = svschmid[diamint][2] + ((schmid - svschmid[diamint][1]) * (schmid - svschmid[diamint][1]));
		  svomega3[diamint][2] = svomega3[diamint][2] + ((omega3 - svomega3[diamint][1]) * (omega3 - svomega3[diamint][1]));
		  for (int k = 0; k < 3; k++)
		  {
			int nnum = m_Grains[j]->neighbordistfunc[k];
			neighborhood[diamint][((2 * k) + 2)] = neighborhood[diamint][((2 * k) + 2)] + ((neighborhood[diamint][((2 * k) + 1)] - nnum)
				* (neighborhood[diamint][((2 * k) + 1)] - nnum));
		  }
		}
	  }
	  for (int temp4 = 0; temp4 < numbins; temp4++)
	  {
		if (svbovera[temp4][0] > 1)
		{
		  neighborhood[temp4][2] = neighborhood[temp4][2] / neighborhood[temp4][0];
		  neighborhood[temp4][4] = neighborhood[temp4][4] / neighborhood[temp4][0];
		  neighborhood[temp4][6] = neighborhood[temp4][6] / neighborhood[temp4][0];
		  svbovera[temp4][2] = svbovera[temp4][2] / svbovera[temp4][0];
		  svcovera[temp4][2] = svcovera[temp4][2] / svcovera[temp4][0];
		  svcoverb[temp4][2] = svcoverb[temp4][2] / svcoverb[temp4][0];
		  svschmid[temp4][2] = svschmid[temp4][2] / svschmid[temp4][0];
		  svomega3[temp4][2] = svomega3[temp4][2] / svomega3[temp4][0];
		  svbovera[temp4][3] = svbovera[temp4][1] * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
		  svbovera[temp4][4] = (1 - svbovera[temp4][1]) * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
		  svcovera[temp4][3] = svcovera[temp4][1] * (((svcovera[temp4][1] * (1 - svcovera[temp4][1])) / svcovera[temp4][2]) - 1);
		  svcovera[temp4][4] = (1 - svcovera[temp4][1]) * (((svcovera[temp4][1] * (1 - svcovera[temp4][1])) / svcovera[temp4][2]) - 1);
		  svcoverb[temp4][3] = svcoverb[temp4][1] * (((svcoverb[temp4][1] * (1 - svcoverb[temp4][1])) / svcoverb[temp4][2]) - 1);
		  svcoverb[temp4][4] = (1 - svcoverb[temp4][1]) * (((svcoverb[temp4][1] * (1 - svcoverb[temp4][1])) / svcoverb[temp4][2]) - 1);
		  svomega3[temp4][3] = svomega3[temp4][1] * (((svomega3[temp4][1] * (1 - svomega3[temp4][1])) / svomega3[temp4][2]) - 1);
		  svomega3[temp4][4] = (1 - svomega3[temp4][1]) * (((svomega3[temp4][1] * (1 - svomega3[temp4][1])) / svomega3[temp4][2]) - 1);
		  neighborhood[temp4][2] = sqrt(neighborhood[temp4][2]);
		  neighborhood[temp4][4] = sqrt(neighborhood[temp4][4]);
		  neighborhood[temp4][6] = sqrt(neighborhood[temp4][6]);
		  svbovera[temp4][2] = sqrt(svbovera[temp4][2]);
		  svcovera[temp4][2] = sqrt(svcovera[temp4][2]);
		  svcoverb[temp4][2] = sqrt(svcoverb[temp4][2]);
		  svschmid[temp4][2] = sqrt(svschmid[temp4][2]);
		  svomega3[temp4][2] = sqrt(svomega3[temp4][2]);
		}
	  }
	  sdlogdiam = sdlogdiam / actualgrains;
	  sdlogdiam = sqrt(sdlogdiam);
	  retErr = h5io->writeVolumeStats(iter, crystruct[iter], phaseType[iter], phasefraction[iter], pptFractions[iter],
	                                  maxdiameter[iter], mindiameter[iter], 1.0, avglogdiam, sdlogdiam, svbovera, svcovera, svcoverb, neighborhoodfit, svomega3);
  }

  return retErr;
}

int MicrostructureStatisticsFunc::volume_stats2D(H5ReconStatsWriter::Pointer h5io, float sizebinstepsize)
{
  int retErr = 0;
  float actualgrains = 0;
  // float avgdiam = 0;
  float avglogdiam = 0;
  //  int neighdistfunc[3];
  vector<vector<float> > neighborhood;
  vector<vector<float> > neighborhoodfit;
  vector<vector<float> > svbovera;
  vector<vector<float> > svcovera;
  vector<vector<float> > svcoverb;
  vector<vector<float> > svschmid;
  vector<vector<float> > svomega3;
  for (size_t iter = 1; iter < crystruct.size(); iter++)
  {
    int numbins = int((maxdiameter[iter] - mindiameter[iter]) / sizebinstepsize) + 1;
    size_t numgrains = m_Grains.size();

    neighborhood.resize(numbins);
    neighborhoodfit.resize(numbins);
    svbovera.resize(numbins);
    svcovera.resize(numbins);
    svcoverb.resize(numbins);
    svschmid.resize(numbins);
    svomega3.resize(numbins);
    for (int temp = 0; temp < numbins; temp++)
    {
      neighborhood[temp].resize(7, 0);
      neighborhoodfit[temp].resize(4, 0);
      svbovera[temp].resize(5, 0);
      svschmid[temp].resize(5, 0);
    }
    for (size_t i = 1; i < numgrains; i++)
    {
      int onedge = m_Grains[i]->surfacegrain;
      if (onedge == 0 && m_Grains[i]->active == 1 && m_Grains[i]->phase == static_cast<int>(iter) )
      {
        actualgrains++;
        float diam = m_Grains[i]->equivdiameter;
        float logdiam = log(diam);
        float rad1 = m_Grains[i]->radius1;
        float rad2 = m_Grains[i]->radius2;
        float bovera = rad2 / rad1;
        float schmid = m_Grains[i]->schmidfactor;
        avglogdiam = avglogdiam + logdiam;
        int diamint = int((diam - mindiameter[iter]) / sizebinstepsize);
        neighborhood[diamint][0]++;
        svbovera[diamint][0]++;
        svschmid[diamint][0]++;
        svbovera[diamint][1] = svbovera[diamint][1] + bovera;
        svschmid[diamint][1] = svschmid[diamint][1] + schmid;
        neighborhood[diamint][0]++;
        for (int k = 0; k < 3; k++)
        {
          int nnum = m_Grains[i]->neighbordistfunc[k];
          neighborhood[diamint][((2 * k) + 1)] = neighborhood[diamint][((2 * k) + 1)] + nnum;
        }
      }
    }
    for (int temp3 = 0; temp3 < numbins; temp3++)
    {
      if (svbovera[temp3][0] != 0)
      {
        neighborhood[temp3][1] = neighborhood[temp3][1] / neighborhood[temp3][0];
        neighborhood[temp3][3] = neighborhood[temp3][3] / neighborhood[temp3][0];
        neighborhood[temp3][5] = neighborhood[temp3][5] / neighborhood[temp3][0];
        svbovera[temp3][1] = svbovera[temp3][1] / svbovera[temp3][0];
        svschmid[temp3][1] = svschmid[temp3][1] / svschmid[temp3][0];
        neighborhoodfit[temp3][0] = neighborhood[temp3][0];
        neighborhoodfit[temp3][1] = neighborhood[temp3][3] - neighborhood[temp3][1];
        neighborhoodfit[temp3][2] = neighborhood[temp3][1];
        neighborhoodfit[temp3][3] = log((neighborhood[temp3][5] - neighborhoodfit[temp3][2]) / neighborhoodfit[temp3][1]) / log(2.0);
      }
    }
    avglogdiam = avglogdiam / actualgrains;
    float sdlogdiam = 0;
    for (size_t j = 1; j < numgrains; j++)
    {
      int onedge = m_Grains[j]->surfacegrain;
      if (onedge == 0 && m_Grains[j]->active == 1 && m_Grains[j]->phase == static_cast<int>(iter) )
      {
        float diam = m_Grains[j]->equivdiameter;
        float logdiam = log(diam);
        float rad1 = m_Grains[j]->radius1;
        float rad2 = m_Grains[j]->radius2;
        float bovera = rad2 / rad1;
        float schmid = m_Grains[j]->schmidfactor;
        sdlogdiam = sdlogdiam + ((logdiam - avglogdiam) * (logdiam - avglogdiam));
        int diamint = int((diam - mindiameter[iter]) / sizebinstepsize);
        svbovera[diamint][2] = svbovera[diamint][2] + ((bovera - svbovera[diamint][1]) * (bovera - svbovera[diamint][1]));
        svschmid[diamint][2] = svschmid[diamint][2] + ((schmid - svschmid[diamint][1]) * (schmid - svschmid[diamint][1]));
        for (int k = 0; k < 3; k++)
        {
          int nnum = m_Grains[j]->neighbordistfunc[k];
          neighborhood[diamint][((2 * k) + 2)] = neighborhood[diamint][((2 * k) + 2)] + ((neighborhood[diamint][((2 * k) + 1)] - nnum)
              * (neighborhood[diamint][((2 * k) + 1)] - nnum));
        }
      }
    }
    for (int temp4 = 0; temp4 < numbins; temp4++)
    {
      if (svbovera[temp4][0] != 0)
      {
        neighborhood[temp4][2] = neighborhood[temp4][2] / neighborhood[temp4][0];
        neighborhood[temp4][4] = neighborhood[temp4][4] / neighborhood[temp4][0];
        neighborhood[temp4][6] = neighborhood[temp4][6] / neighborhood[temp4][0];
        svbovera[temp4][2] = svbovera[temp4][2] / svbovera[temp4][0];
        svschmid[temp4][2] = svschmid[temp4][2] / svschmid[temp4][0];
        svbovera[temp4][3] = svbovera[temp4][1] * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
        svbovera[temp4][4] = (1 - svbovera[temp4][1]) * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
        neighborhood[temp4][2] = sqrt(neighborhood[temp4][2]);
        neighborhood[temp4][4] = sqrt(neighborhood[temp4][4]);
        neighborhood[temp4][6] = sqrt(neighborhood[temp4][6]);
        svbovera[temp4][2] = sqrt(svbovera[temp4][2]);
        svschmid[temp4][2] = sqrt(svschmid[temp4][2]);
      }
    }
    sdlogdiam = sdlogdiam / actualgrains;
    sdlogdiam = sqrt(sdlogdiam);

    retErr
        = h5io->writeVolumeStats2D(iter, crystruct[iter], phaseType[iter], phasefraction[iter], pptFractions[iter],
                                   maxdiameter[iter], mindiameter[iter], 1.0, avglogdiam, sdlogdiam, svbovera, neighborhoodfit);
  }
  return retErr;
}

void MicrostructureStatisticsFunc::deformation_stats(const std::string &filename, const std::string &filename2)
{
  ofstream outFile;
  outFile.open(filename.c_str());
  float w, n1, n2, n3;
  int distance;
  float km, gbdist, tjdist, qpdist, sf, sf2, sfmm, gam, lmg, ssap;
  int nearestneighbor, gname, gname2, ss1, ss2;
  float q1[5], q2[5];
  int kmdist[25];
  int gamdist[25];
  int lmgdist[25];
  float kmvgb[10][2];
  float gamvgb[10][2];
  float lmgvgb[10][2];
  float kmvtj[10][2];
  float gamvtj[10][2];
  float lmgvtj[10][2];
  float kmvqp[10][2];
  float gamvqp[10][2];
  float lmgvqp[10][2];
  float kmvsf[10][2];
  float gamvsf[10][2];
  float lmgvsf[10][2];
  float kmvsfmm[10][2];
  float gamvsfmm[10][2];
  float lmgvsfmm[10][2];
  float kmvssap[10][2];
  float gamvssap[10][2];
  float lmgvssap[10][2];
  float kmvdis[10][2];
  float gamvdis[10][2];
  float lmgvdis[10][2];
  float kmvsfdistthresh[10][10][2];
  float gamvsfdistthresh[10][10][2];
  float lmgvsfdistthresh[10][10][2];
  float kmvsfmmdistthresh[10][10][2];
  float gamvsfmmdistthresh[10][10][2];
  float lmgvsfmmdistthresh[10][10][2];
  float kmvssapdistthresh[10][10][2];
  float gamvssapdistthresh[10][10][2];
  float lmgvssapdistthresh[10][10][2];
  float kmvdisdistthresh[10][10][2];
  float gamvdisdistthresh[10][10][2];
  float lmgvdisdistthresh[10][10][2];
  float kmvsfmmssapthresh[10][10][2];
  float gamvsfmmssapthresh[10][10][2];
  float lmgvsfmmssapthresh[10][10][2];
  int kmbin, gambin, lmgbin;
  int gbbin, tjbin, qpbin;
  int sfbin, ssapbin, sfmmbin, disbin;
 // int actualpoints = 0;
  for (int h = 0; h < 25; h++)
  {
	kmdist[h] = 0;
	gamdist[h] = 0;
	lmgdist[h] = 0;
  }
  for (int h = 0; h < 10; h++)
  {
	  for (int i = 0; i < 10; i++)
	  {
	    for (int j = 0; j < 2; j++)
	    {
		  kmvsfmmssapthresh[h][i][j] = 0;
		  gamvsfmmssapthresh[h][i][j] = 0;
		  lmgvsfmmssapthresh[h][i][j] = 0;
		  kmvsfdistthresh[h][i][j] = 0;
		  gamvsfdistthresh[h][i][j] = 0;
		  lmgvsfdistthresh[h][i][j] = 0;
		  kmvssapdistthresh[h][i][j] = 0;
		  gamvssapdistthresh[h][i][j] = 0;
		  lmgvssapdistthresh[h][i][j] = 0;
		  kmvssapdistthresh[h][i][j] = 0;
		  gamvssapdistthresh[h][i][j] = 0;
		  lmgvssapdistthresh[h][i][j] = 0;
		  kmvsfmmdistthresh[h][i][j] = 0;
		  gamvsfmmdistthresh[h][i][j] = 0;
		  lmgvsfmmdistthresh[h][i][j] = 0;
		  kmvdisdistthresh[h][i][j] = 0;
		  gamvdisdistthresh[h][i][j] = 0;
		  lmgvdisdistthresh[h][i][j] = 0;
		  if(h == 0)
		  {
			  kmvgb[i][j] = 0;
			  gamvgb[i][j] = 0;
			  lmgvgb[i][j] = 0;
			  kmvtj[i][j] = 0;
			  gamvtj[i][j] = 0;
			  lmgvtj[i][j] = 0;
			  kmvqp[i][j] = 0;
			  gamvqp[i][j] = 0;
			  lmgvqp[i][j] = 0;
			  kmvsf[i][j] = 0;
			  gamvsf[i][j] = 0;
			  lmgvsf[i][j] = 0;
			  kmvsfmm[i][j] = 0;
			  gamvsfmm[i][j] = 0;
			  lmgvsfmm[i][j] = 0;
			  kmvssap[i][j] = 0;
			  gamvssap[i][j] = 0;
			  lmgvssap[i][j] = 0;
			  kmvdis[i][j] = 0;
			  gamvdis[i][j] = 0;
			  lmgvdis[i][j] = 0;
		  }
		}
    }
  }

  for (int i = 0; i < totalpoints; i++)
  {
      km = voxels[i].kernelmisorientation;
      gam = voxels[i].grainmisorientation;
      lmg = voxels[i].misorientationgradient;
      gbdist = voxels[i].nearestneighbordistance[0];
      tjdist = voxels[i].nearestneighbordistance[1];
      qpdist = voxels[i].nearestneighbordistance[2];
      gname = voxels[i].grain_index;
      nearestneighbor = voxels[i].nearestneighbor[0];
      gname2 = voxels[nearestneighbor].neighborlist->at(0);
      sf = m_Grains[gname]->schmidfactor;
      sf2 = m_Grains[gname2]->schmidfactor;
      sfmm = sf / sf2;
	  ss1 = m_Grains[gname]->slipsystem;
	  ss2 = m_Grains[gname2]->slipsystem;
	  for(int j=0;j<5;j++)
	  {
		q1[j] = m_Grains[gname]->avg_quat[j]/m_Grains[gname]->avg_quat[0];
		q2[j] = m_Grains[gname2]->avg_quat[j]/m_Grains[gname2]->avg_quat[0];
	  }
	  OrientationMath::getSlipMisalignment(ss1, q1, q2, ssap);
      if (crystruct[m_Grains[gname]->phase] == crystruct[m_Grains[gname2]->phase])
	  {
        w = m_OrientationOps[crystruct[m_Grains[gname]->phase]]->getMisoQuat(q1, q2, n1, n2, n3);
      }
      kmbin = int(km/0.2);
      gambin = int(gam/0.8);
      lmgbin = int(lmg/0.1);
      gbbin = int(gbdist);
      tjbin = int(tjdist);
      qpbin = int(qpdist);
	  sfbin = int((sf-0.25) / 0.025);
	  if(sfmm >= 1) sfmmbin = int((sfmm-1.0)/0.2)+5;
	  if(sfmm < 1) sfmmbin = 4-int(((1.0/sfmm)-1.0)/0.2);
	  ssapbin = int((ssap-0.4) / 0.06);
	  disbin = int((w) / 10.0);
      if (kmbin < 0) kmbin = 0;
      if (kmbin > 24) kmbin = 24;
      if (gambin < 0) gambin = 0;
      if (gambin > 24) gambin = 24;
      if (lmgbin < 0) lmgbin = 0;
      if (lmgbin > 24) lmgbin = 24;
      if (gbbin < 0) gbbin = 0;
      if (gbbin > 9) gbbin = 9;
      if (tjbin < 0) tjbin = 0;
      if (tjbin > 9) tjbin = 9;
      if (qpbin < 0) qpbin = 0;
      if (qpbin > 9) qpbin = 9;
      if (sfbin < 0) sfbin = 0;
      if (sfbin > 9) sfbin = 9;
      if (sfmmbin < 0) sfmmbin = 0;
      if (sfmmbin > 9) sfmmbin = 9;
      if (ssapbin < 0) ssapbin = 0;
      if (ssapbin > 9) ssapbin = 9;
      if (disbin < 0) disbin = 0;
      if (disbin > 9) disbin = 9;
	  kmdist[kmbin]++;
	  gamdist[gambin]++;
	  lmgdist[lmgbin]++;
      kmvgb[gbbin][0]++;
      kmvgb[gbbin][1] = kmvgb[gbbin][1] + km;
      gamvgb[gbbin][0]++;
      gamvgb[gbbin][1] = gamvgb[gbbin][1] + gam;
      lmgvgb[gbbin][0]++;
      lmgvgb[gbbin][1] = lmgvgb[gbbin][1] + lmg;
      kmvtj[tjbin][0]++;
      kmvtj[tjbin][1] = kmvtj[tjbin][1] + km;
      gamvtj[tjbin][0]++;
      gamvtj[tjbin][1] = gamvtj[tjbin][1] + gam;
      lmgvtj[tjbin][0]++;
      lmgvtj[tjbin][1] = lmgvtj[tjbin][1] + lmg;
      kmvqp[qpbin][0]++;
      kmvqp[qpbin][1] = kmvqp[qpbin][1] + km;
      gamvqp[qpbin][0]++;
      gamvqp[qpbin][1] = gamvqp[qpbin][1] + gam;
      lmgvqp[qpbin][0]++;
      lmgvqp[qpbin][1] = lmgvqp[qpbin][1] + lmg;
	  distance = int(voxels[i].nearestneighbordistance[0]);
	  if(distance > 9) distance = 9;
	  if(distance <= 5)
	  {
		  kmvsf[sfbin][0]++;
		  kmvsf[sfbin][1] = kmvsf[sfbin][1] + km;
		  gamvsf[sfbin][0]++;
		  gamvsf[sfbin][1] = gamvsf[sfbin][1] + gam;
		  lmgvsf[sfbin][0]++;
		  lmgvsf[sfbin][1] = lmgvsf[sfbin][1] + lmg;
		  kmvsfmm[sfmmbin][0]++;
		  kmvsfmm[sfmmbin][1] = kmvsfmm[sfmmbin][1] + km;
		  gamvsfmm[sfmmbin][0]++;
		  gamvsfmm[sfmmbin][1] = gamvsfmm[sfmmbin][1] + gam;
		  lmgvsfmm[sfmmbin][0]++;
		  lmgvsfmm[sfmmbin][1] = lmgvsfmm[sfmmbin][1] + lmg;
		  kmvssap[ssapbin][0]++;
		  kmvssap[ssapbin][1] = kmvssap[ssapbin][1] + km;
		  gamvssap[ssapbin][0]++;
		  gamvssap[ssapbin][1] = gamvssap[ssapbin][1] + gam;
		  lmgvssap[ssapbin][0]++;
		  lmgvssap[ssapbin][1] = lmgvssap[ssapbin][1] + lmg;
		  kmvdis[disbin][0]++;
		  kmvdis[disbin][1] = kmvdis[disbin][1] + km;
		  gamvdis[disbin][0]++;
		  gamvdis[disbin][1] = gamvdis[disbin][1] + gam;
		  lmgvdis[disbin][0]++;
		  lmgvdis[disbin][1] = lmgvdis[disbin][1] + lmg;
	  }
	  kmvsfdistthresh[distance][sfbin][0]++;
      kmvsfdistthresh[distance][sfbin][1] = kmvsfdistthresh[distance][sfbin][1] + km;
      gamvsfdistthresh[distance][sfbin][0]++;
      gamvsfdistthresh[distance][sfbin][1] = gamvsfdistthresh[distance][sfbin][1] + gam;
      lmgvsfdistthresh[distance][sfbin][0]++;
      lmgvsfdistthresh[distance][sfbin][1] = lmgvsfdistthresh[distance][sfbin][1] + lmg;
      kmvsfmmdistthresh[distance][sfmmbin][0]++;
      kmvsfmmdistthresh[distance][sfmmbin][1] = kmvsfmmdistthresh[distance][sfmmbin][1] + km;
      gamvsfmmdistthresh[distance][sfmmbin][0]++;
      gamvsfmmdistthresh[distance][sfmmbin][1] = gamvsfmmdistthresh[distance][sfmmbin][1] + gam;
      lmgvsfmmdistthresh[distance][sfmmbin][0]++;
      lmgvsfmmdistthresh[distance][sfmmbin][1] = lmgvsfmmdistthresh[distance][sfmmbin][1] + lmg;
      kmvssapdistthresh[distance][ssapbin][0]++;
      kmvssapdistthresh[distance][ssapbin][1] = kmvssapdistthresh[distance][ssapbin][1] + km;
      gamvssapdistthresh[distance][ssapbin][0]++;
      gamvssapdistthresh[distance][ssapbin][1] = gamvssapdistthresh[distance][ssapbin][1] + gam;
      lmgvssapdistthresh[distance][ssapbin][0]++;
      lmgvssapdistthresh[distance][ssapbin][1] = lmgvssapdistthresh[distance][ssapbin][1] + lmg;
      kmvdisdistthresh[distance][disbin][0]++;
      kmvdisdistthresh[distance][disbin][1] = kmvdisdistthresh[distance][disbin][1] + km;
      gamvdisdistthresh[distance][disbin][0]++;
      gamvdisdistthresh[distance][disbin][1] = gamvdisdistthresh[distance][disbin][1] + gam;
      lmgvdisdistthresh[distance][disbin][0]++;
      lmgvdisdistthresh[distance][disbin][1] = lmgvdisdistthresh[distance][disbin][1] + lmg;
	  kmvsfmmssapthresh[sfmmbin][ssapbin][0]++;
	  kmvsfmmssapthresh[sfmmbin][ssapbin][1] = kmvsfmmssapthresh[sfmmbin][ssapbin][1] + km;
	  gamvsfmmssapthresh[sfmmbin][ssapbin][0]++;
	  gamvsfmmssapthresh[sfmmbin][ssapbin][1] = gamvsfmmssapthresh[sfmmbin][ssapbin][1] + gam;
	  lmgvsfmmssapthresh[sfmmbin][ssapbin][0]++;
	  lmgvsfmmssapthresh[sfmmbin][ssapbin][1] = lmgvsfmmssapthresh[sfmmbin][ssapbin][1] + lmg;
  }
  outFile << "Kernel Misorientation Data" << endl;
  outFile << "GB		TJ		QP		SF		SFMM		SSAP		DIS" << endl;
  for (int i = 0; i < 10; i++)
  {
    if (kmvgb[i][0] > 0) kmvgb[i][1] = kmvgb[i][1] / kmvgb[i][0];
    if (kmvtj[i][0] > 0) kmvtj[i][1] = kmvtj[i][1] / kmvtj[i][0];
    if (kmvqp[i][0] > 0) kmvqp[i][1] = kmvqp[i][1] / kmvqp[i][0];
    if (kmvsf[i][0] > 0) kmvsf[i][1] = kmvsf[i][1] / kmvsf[i][0];
    if (kmvsfmm[i][0] > 0) kmvsfmm[i][1] = kmvsfmm[i][1] / kmvsfmm[i][0];
    if (kmvssap[i][0] > 0) kmvssap[i][1] = kmvssap[i][1] / kmvssap[i][0];
    if (kmvdis[i][0] > 0) kmvdis[i][1] = kmvdis[i][1] / kmvdis[i][0];
    outFile << kmvgb[i][0] << "	" << kmvgb[i][1] << "	"
		<< kmvtj[i][0] << "	" << kmvtj[i][1] << "	"
		<< kmvqp[i][0] << "	" << kmvqp[i][1] << "	"
		<< kmvsf[i][0] << "	" << kmvsf[i][1] << "	"
		<< kmvsfmm[i][0] << "	" << kmvsfmm[i][1] << "	"
		<< kmvssap[i][0] << "	" << kmvssap[i][1] << "	"
		<< kmvdis[i][0] << "	" << kmvdis[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Grain Average Misorientation Data" << endl;
  outFile << "GB		TJ		QP		SF		SFMM		SSAP		DIS" << endl;
  for (int i = 0; i < 10; i++)
  {
    if (gamvgb[i][0] > 0) gamvgb[i][1] = gamvgb[i][1] / gamvgb[i][0];
    if (gamvtj[i][0] > 0) gamvtj[i][1] = gamvtj[i][1] / gamvtj[i][0];
    if (gamvqp[i][0] > 0) gamvqp[i][1] = gamvqp[i][1] / gamvqp[i][0];
    if (gamvsf[i][0] > 0) gamvsf[i][1] = gamvsf[i][1] / gamvsf[i][0];
    if (gamvsfmm[i][0] > 0) gamvsfmm[i][1] = gamvsfmm[i][1] / gamvsfmm[i][0];
    if (gamvssap[i][0] > 0) gamvssap[i][1] = gamvssap[i][1] / gamvssap[i][0];
    if (gamvdis[i][0] > 0) gamvdis[i][1] = gamvdis[i][1] / gamvdis[i][0];
    outFile << gamvgb[i][0] << "	" << gamvgb[i][1] << "	"
		<< gamvtj[i][0] << "	" << gamvtj[i][1] << "	"
		<< gamvqp[i][0] << "	" << gamvqp[i][1] << "	"
		<< gamvsf[i][0] << "	" << gamvsf[i][1] << "	"
		<< gamvsfmm[i][0] << "	" << gamvsfmm[i][1] << "	"
		<< gamvssap[i][0] << "	" << gamvssap[i][1] << "	"
		<< gamvdis[i][0] << "	" << gamvdis[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Misorientation Gradient Data" << endl;
  outFile << "GB		TJ		QP		SF		SFMM		SSAP		DIS" << endl;
  for (int i = 0; i < 10; i++)
  {
    if (lmgvgb[i][0] > 0) lmgvgb[i][1] = lmgvgb[i][1] / lmgvgb[i][0];
    if (lmgvtj[i][0] > 0) lmgvtj[i][1] = lmgvtj[i][1] / lmgvtj[i][0];
    if (lmgvqp[i][0] > 0) lmgvqp[i][1] = lmgvqp[i][1] / lmgvqp[i][0];
    if (lmgvsf[i][0] > 0) lmgvsf[i][1] = lmgvsf[i][1] / lmgvsf[i][0];
    if (lmgvsfmm[i][0] > 0) lmgvsfmm[i][1] = lmgvsfmm[i][1] / lmgvsfmm[i][0];
    if (lmgvssap[i][0] > 0) lmgvssap[i][1] = lmgvssap[i][1] / lmgvssap[i][0];
    if (lmgvdis[i][0] > 0) lmgvdis[i][1] = lmgvdis[i][1] / lmgvdis[i][0];
    outFile << lmgvgb[i][0] << "	" << lmgvgb[i][1] << "	"
		<< lmgvtj[i][0] << "	" << lmgvtj[i][1] << "	"
		<< lmgvqp[i][0] << "	" << lmgvqp[i][1] << "	"
		<< lmgvsf[i][0] << "	" << lmgvsf[i][1] << "	"
		<< lmgvsfmm[i][0] << "	" << lmgvsfmm[i][1] << "	"
		<< lmgvssap[i][0] << "	" << lmgvssap[i][1] << "	"
		<< lmgvdis[i][0] << "	" << lmgvdis[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Thresheld Data" << endl;
  outFile << "SF KAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvsfdistthresh[j][i][0] > 0) kmvsfdistthresh[j][i][1] = kmvsfdistthresh[j][i][1] / kmvsfdistthresh[j][i][0];
	    outFile << kmvsfdistthresh[j][i][0] << "	" << kmvsfdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM KAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvsfmmdistthresh[j][i][0] > 0) kmvsfmmdistthresh[j][i][1] = kmvsfmmdistthresh[j][i][1] / kmvsfmmdistthresh[j][i][0];
	    outFile << kmvsfmmdistthresh[j][i][0] << "	" << kmvsfmmdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SSAP KAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvssapdistthresh[j][i][0] > 0) kmvssapdistthresh[j][i][1] = kmvssapdistthresh[j][i][1] / kmvssapdistthresh[j][i][0];
	    outFile << kmvssapdistthresh[j][i][0] << "	" << kmvssapdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "DIS KAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvdisdistthresh[j][i][0] > 0) kmvdisdistthresh[j][i][1] = kmvdisdistthresh[j][i][1] / kmvdisdistthresh[j][i][0];
	    outFile << kmvdisdistthresh[j][i][0] << "	" << kmvdisdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SF GAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvsfdistthresh[j][i][0] > 0) gamvsfdistthresh[j][i][1] = gamvsfdistthresh[j][i][1] / gamvsfdistthresh[j][i][0];
	    outFile << gamvsfdistthresh[j][i][0] << "	" << gamvsfdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM GAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvsfmmdistthresh[j][i][0] > 0) gamvsfmmdistthresh[j][i][1] = gamvsfmmdistthresh[j][i][1] / gamvsfmmdistthresh[j][i][0];
	    outFile << gamvsfmmdistthresh[j][i][0] << "	" << gamvsfmmdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SSAP GAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvssapdistthresh[j][i][0] > 0) gamvssapdistthresh[j][i][1] = gamvssapdistthresh[j][i][1] / gamvssapdistthresh[j][i][0];
	    outFile << gamvssapdistthresh[j][i][0] << "	" << gamvssapdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "DIS GAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvdisdistthresh[j][i][0] > 0) gamvdisdistthresh[j][i][1] = gamvdisdistthresh[j][i][1] / gamvdisdistthresh[j][i][0];
	    outFile << gamvdisdistthresh[j][i][0] << "	" << gamvdisdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SF LMG" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvsfdistthresh[j][i][0] > 0) lmgvsfdistthresh[j][i][1] = lmgvsfdistthresh[j][i][1] / lmgvsfdistthresh[j][i][0];
	    outFile << lmgvsfdistthresh[j][i][0] << "	" << lmgvsfdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM LMG" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvsfmmdistthresh[j][i][0] > 0) lmgvsfmmdistthresh[j][i][1] = lmgvsfmmdistthresh[j][i][1] / lmgvsfmmdistthresh[j][i][0];
	    outFile << lmgvsfmmdistthresh[j][i][0] << "	" << lmgvsfmmdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SSAP LMG" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvssapdistthresh[j][i][0] > 0) lmgvssapdistthresh[j][i][1] = lmgvssapdistthresh[j][i][1] / lmgvssapdistthresh[j][i][0];
	    outFile << lmgvssapdistthresh[j][i][0] << "	" << lmgvssapdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "DIS LMG" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvdisdistthresh[j][i][0] > 0) lmgvdisdistthresh[j][i][1] = lmgvdisdistthresh[j][i][1] / lmgvdisdistthresh[j][i][0];
	    outFile << lmgvdisdistthresh[j][i][0] << "	" << lmgvdisdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM SSAP KAM" << endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvsfmmssapthresh[i][j][0] > 0) kmvsfmmssapthresh[i][j][1] = kmvsfmmssapthresh[i][j][1] / kmvsfmmssapthresh[i][j][0];
	    outFile << kmvsfmmssapthresh[i][j][0] << "	" << kmvsfmmssapthresh[i][j][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM SSAP GAM" << endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvsfmmssapthresh[i][j][0] > 0) gamvsfmmssapthresh[i][j][1] = gamvsfmmssapthresh[i][j][1] / gamvsfmmssapthresh[i][j][0];
	    outFile << gamvsfmmssapthresh[i][j][0] << "	" << gamvsfmmssapthresh[i][j][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM SSAP LMG" << endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvsfmmssapthresh[i][j][0] > 0) lmgvsfmmssapthresh[i][j][1] = lmgvsfmmssapthresh[i][j][1] / lmgvsfmmssapthresh[i][j][0];
	    outFile << lmgvsfmmssapthresh[i][j][0] << "	" << lmgvsfmmssapthresh[i][j][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "KAM DIST		GAM DIST		LMG DIST" << endl;
  for (int i = 0; i < 25; i++)
  {
	    outFile << float(i)*0.2+0.1 << "	" << kmdist[i] << "	" << float(i)*0.8+0.4 << "	" << gamdist[i] << "	" << float(i)*0.1+0.05 << "	" << lmgdist[i] << endl;
  }
  outFile.close();
  FILE* vtkFile = NULL;
  vtkFile = fopen(filename2.c_str(), "wb");
  if (NULL == vtkFile)
  {
	std::cout << "Error Creating VTK Visualization File '" << filename << "'" << std::endl;
	return;
  }
  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile,  "data set from FFT2dx_GB\n");
  fprintf(vtkFile,  "ASCII\n");
  fprintf(vtkFile,  "DATASET UNSTRUCTURED_GRID\n");
  fprintf(vtkFile,  "POINTS %ld float\n", m_Grains.size()-1);

  std::vector<Grain::Pointer>::size_type size = m_Grains.size();

  for(size_t i=1;i<size;i++)
  {
		float x = m_Grains[i]->IPF[0] - (m_Grains[i]->IPF[0] * (m_Grains[i]->IPF[2] / (m_Grains[i]->IPF[2] + 1)));;
		float y = m_Grains[i]->IPF[1] - (m_Grains[i]->IPF[1] * (m_Grains[i]->IPF[2] / (m_Grains[i]->IPF[2] + 1)));;
		float z = 0.0;
		fprintf(vtkFile, "%f %f %f\n", x, y, z);
  }

  fprintf(vtkFile, "CELLS %ld %ld\n", m_Grains.size()-1, ((m_Grains.size()-1)*2));
//  Store the Grain Ids so we don't have to re-read the triangles file again
  for(size_t i=1;i<size;i++)
  {
	   fprintf(vtkFile, "1 %ld\n", (i-1));
  }

  // Write the CELL_TYPES into the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_TYPES %ld\n", m_Grains.size()-1);
  for(size_t i=1;i<size;i++)
  {
	fprintf(vtkFile, "1\n");
  }


  // Write the GrainId Data to teh file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %ld\n", m_Grains.size()-1);
  fprintf(vtkFile, "SCALARS Misorientation float\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for (size_t i = 1; i < size; i++)
  {
	  float miso = m_Grains[i]->averagemisorientation;
	  fprintf(vtkFile, "%f\n", miso);
  }
  fclose(vtkFile);
}



void MicrostructureStatisticsFunc::write_graindata(const std::string &graindataFile, bool writesizes, bool writeshapes, bool writeneighbors, bool writeavgorientations)
{
  IntVectorType nlist;
  ofstream outFile;
  size_t numgrains = m_Grains.size();
  outFile.open(graindataFile.c_str());
  outFile << numgrains << endl;
  outFile << "Grain ID	Surface Grain";
  if(writeavgorientations == true) outFile << "	Phi1	PHI	Phi2";
  if(writesizes == true) outFile <<  "	Equiv_Diameter";
  if(writeshapes == true) outFile << "	b/a	c/a	Omega3";
  if(writeneighbors == true) outFile << "	No_Neighbors";
  outFile << endl;
  for (size_t i = 1; i < numgrains; i++)
  {
    outFile << i << "	" << m_Grains[i]->surfacegrain;
	if(writeavgorientations == true) outFile << "	" << m_Grains[i]->euler1 << "	" << m_Grains[i]->euler2 << "	" << m_Grains[i]->euler3;
	if(writesizes == true) outFile <<  "	" << m_Grains[i]->equivdiameter;
	if(writeshapes == true) outFile << "	" << m_Grains[i]->aspectratio1 << "	" << m_Grains[i]->aspectratio2 << "	" << m_Grains[i]->omega3;
	if(writeneighbors == true) outFile << "	" << m_Grains[i]->neighborlist->size();
	outFile << endl;
  }
  outFile.close();
}


float MicrostructureStatisticsFunc::find_xcoord(size_t index)
{
  float x = resx * float(index % xpoints);
  return x;
}
float MicrostructureStatisticsFunc::find_ycoord(size_t index)
{
  float y = resy * float((index / xpoints) % ypoints);
  return y;
}
float MicrostructureStatisticsFunc::find_zcoord(size_t index)
{
  float z = resz * float(index / (xpoints * ypoints));
  return z;
}
