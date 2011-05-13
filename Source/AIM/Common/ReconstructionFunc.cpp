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

#include "ReconstructionFunc.h"

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
#include "AIM/Common/AIMMath.h"
#include "AIM/Common/Constants.h"
#include "AIM/Common/OIMColoring.hpp"
#include "AIM/Common/ReconstructionVTKWriter.h"
#include "AIM/Common/HDF5/AIM_H5VtkDataWriter.h"
#include "AIM/Parallel/Algo.hpp"


const static float m_pi = M_PI;
const static float m_OnePointThree = 1.33333333333;
const static float threesixty_over_pi = 360.0 / m_pi;
const static float m_pi_over_180 = M_PI / 180.0;
const static float sqrt_two = powf(2.0, 0.5);
const static float acos_neg_one = acos(-1.0);
const static float acos_pos_one = acos(1.0);
const static float sin_wmin_neg_1_over_2 = sin(acos_neg_one / 2.0);
const static float sin_wmin_pos_1_over_2 = sin(acos_pos_one / 2.0);

#define DIMS "DIMENSIONS"
#define LOOKUP "LOOKUP_TABLE"

#if 0
// -i C:\Users\GroebeMA\Desktop\NewFolder --outputDir C:\Users\GroebeMA\Desktop\NewFolder -f Slice_ --angMaxSlice 400 -s 1 -e 30 -z 0.25 -t -g 10 -c 0.1 -o 5.0 -x 2
#endif

using namespace std;

ReconstructionFunc::ReconstructionFunc()
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());
}

ReconstructionFunc::~ReconstructionFunc()
{
  // std::cout << "~ReconstructionFunc" << std::endl;
  m_Grains.clear();
  graincenters.clear();
  grainmoments.clear();
  voxels.reset(NULL);
}

void ReconstructionFunc::initialize(int nX, int nY, int nZ, float xRes, float yRes, float zRes, bool v_mergetwinsoption,
									bool v_mergecoloniesoption, int v_minallowedgrainsize, float v_minseedconfidence,
									float v_downsamplefactor, float v_minseedimagequality, float v_misorientationtolerance,
									float v_sizebinstepsize, vector<AIM::Reconstruction::CrystalStructure> v_crystruct,
                                    int v_alignmeth, bool v_alreadyformed)
{

  mergetwinsoption = (v_mergetwinsoption == true) ? 1 : 0;
  mergecoloniesoption = (v_mergecoloniesoption == true) ? 1 : 0;
  minallowedgrainsize = v_minallowedgrainsize;
  minseedconfidence = v_minseedconfidence;
  downsamplefactor = v_downsamplefactor;
  minseedimagequality = v_minseedimagequality;
  sizebinstepsize = v_sizebinstepsize;
  misorientationtolerance = v_misorientationtolerance;
  crystruct = v_crystruct;
  alignmeth = v_alignmeth;
  alreadyformed = (v_alreadyformed == true) ? 1 : 0;

  maxdiameter.resize(crystruct.size());
  mindiameter.resize(crystruct.size());
  totalvol.resize(crystruct.size());
  totalsurfacearea.resize(crystruct.size());
  totalaxes.resize(crystruct.size());
  phasefraction.resize(crystruct.size());
  totalpoints = -1;
  xpoints = nX;
  ypoints = nY;
  zpoints = nZ;
  resx = xRes;
  resy = yRes;
  resz = zRes;

  sizex = (xpoints - 1) * resx;
  sizey = (ypoints - 1) * resy;
  sizez = (zpoints - 1) * resz;

  totalpoints = xpoints * ypoints * zpoints;

  int numgrains = 100;
  size_t oldSize = 0;
  m_Grains.resize(numgrains + 1);
  for(size_t g = oldSize; g < m_Grains.size(); ++g)
  {
    m_Grains[g] = Grain::New();
  }

  voxels.reset(new ReconstructionVoxel[totalpoints]);
}

void ReconstructionFunc::initializeQuats()
{
  float qr[5];
  AIM::Reconstruction::CrystalStructure xtal = AIM::Reconstruction::UnknownCrystalStructure;
  int phase = -1;
  for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
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
  }
}
void ReconstructionFunc::cleanup_data()
{
  float bestneighborconfidence;
  int bestneighbor;
  float confidence;
  int x, y, z;
  int neighpoint;
  int good = 0;
  int count = 1;
  int neighbors[6];
  neighbors[0] = -(xpoints * ypoints);
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = (xpoints * ypoints);
  while (count != 0)
  {
    count = 0;
    for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
    {
      if (voxels[i].grainname == -1 && voxels[i].confidence < minseedconfidence)
      {
        x = i % xpoints;
        y = (i / xpoints) % ypoints;
        z = i / (xpoints * ypoints);
        bestneighborconfidence = minseedconfidence;
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = i + neighbors[j];
          if (j == 0 && z == 0) good = 0;
          if (j == 5 && z == (zpoints - 1)) good = 0;
          if (j == 1 && y == 0) good = 0;
          if (j == 4 && y == (ypoints - 1)) good = 0;
          if (j == 2 && x == 0) good = 0;
          if (j == 3 && x == (xpoints - 1)) good = 0;
          if (good == 1)
          {
            confidence = voxels[neighpoint].confidence;
            if (confidence > bestneighborconfidence)
            {
              count++;
              bestneighborconfidence = confidence;
              bestneighbor = neighpoint;
            }
          }
        }
        if (bestneighborconfidence > minseedconfidence)
        {
          voxels[i].neighbor = bestneighbor;
        }
      }
    }
    for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
    {
      bestneighbor = voxels[j].neighbor;
      if (bestneighbor >= 0 && voxels[j].confidence < minseedconfidence)
      {
        voxels[j].euler1 = voxels[bestneighbor].euler1;
        voxels[j].euler2 = voxels[bestneighbor].euler2;
        voxels[j].euler3 = voxels[bestneighbor].euler3;
        voxels[j].confidence = voxels[bestneighbor].confidence;
        voxels[j].imagequality = voxels[bestneighbor].imagequality;
        voxels[j].quat[0] = voxels[bestneighbor].quat[0];
        voxels[j].quat[1] = voxels[bestneighbor].quat[1];
        voxels[j].quat[2] = voxels[bestneighbor].quat[2];
        voxels[j].quat[3] = voxels[bestneighbor].quat[3];
        voxels[j].quat[4] = voxels[bestneighbor].quat[4];
        voxels[j].neighbor = -1;
      }
    }
  }
/*  float q1[5];
  float q2[5];
  float qtot[5];
  int numVoxel;
  int col, row, plane;
  int steps = 1;
  int jStride;
  int kStride;
  int neighbor, good;
  float w, n1, n2, n3;
  for (int i = 0; i < totalpoints; i++)
  {
	if (voxels[i].grainname != 0)
    {
	  for(int j=0;j<5;j++)
	  {
		qtot[j] = 0.0;
	  }
      numVoxel = 0;
      q1[0] = 1.0;
      q1[1] = voxels[i].quat[1];
      q1[2] = voxels[i].quat[2];
      q1[3] = voxels[i].quat[3];
      q1[4] = voxels[i].quat[4];
	  for(int m=0;m<5;m++)
	  {
		  qtot[m] = qtot[m] + q1[m];
	  }
	  numVoxel++;
      col = i % xpoints;
      row = (i / xpoints) % ypoints;
      plane = i / (xpoints * ypoints);
      for (int j = -steps; j < steps + 1; j++)
      {
        jStride = j * xpoints * ypoints;
        for (int k = -steps; k < steps + 1; k++)
        {
          kStride = k * xpoints;
          for (int l = -steps; l < steps + 1; l++)
          {
            good = 1;
            neighbor = i + (jStride) + (kStride) + (l);
            if (plane + j < 0) good = 0;
            if (plane + j > zpoints - 1) good = 0;
            if (row + k < 0) good = 0;
            if (row + k > ypoints - 1) good = 0;
            if (col + l < 0) good = 0;
            if (col + l > xpoints - 1) good = 0;
            if (good == 1)
            {
              q2[0] = 1.0;
              q2[1] = voxels[neighbor].quat[1];
              q2[2] = voxels[neighbor].quat[2];
              q2[3] = voxels[neighbor].quat[3];
              q2[4] = voxels[neighbor].quat[4];
              if (crystruct == AIM::Reconstruction::Hexagonal) w = OrientationMath::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
              if (crystruct == AIM::Reconstruction::Cubic) w = OrientationMath::getMisoQuatCubic(q1, q2, n1, n2, n3);
              if(w < 5)
			  {
				  if (crystruct == AIM::Reconstruction::Cubic) OrientationMath::getNearestQuatCubic(q1, q2);
				  for(int m=0;m<5;m++)
				  {
					  qtot[m] = qtot[m] + q2[m];
				  }
				  numVoxel++;
			  }
			}
		  }
		}
	  }
	  for(int m=0;m<5;m++)
	  {
		  qtot[m] = qtot[m]/numVoxel;
		  voxels[i].quat[m] = qtot[m];
	  }
	}
  }*/
}
void ReconstructionFunc::find_border()
{
  int neighbors[6];
  neighbors[0] = -(xpoints * ypoints);
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = (xpoints * ypoints);
  float w, n1, n2, n3;
  float q1[5];
  float q2[5];
  //size_t size = 0;
  int index;
  int good = 0;
  size_t count = 0;
  int currentpoint = 0;
  int neighbor = 0;
  int col, row, plane;
  AIM::Reconstruction::CrystalStructure phase1, phase2;
  int initialVoxelsListSize = 10000;
  std::vector<int> voxelslist(initialVoxelsListSize, -1);
  size_t totalPoints = xpoints * ypoints * zpoints;

  AIMArray<int>::Pointer gNamesPtr = AIMArray<int>::CreateArray(totalPoints);
  int *gnames = gNamesPtr->getPointer(0);
  AIMArray<int>::Pointer checkedPtr = AIMArray<int>::CreateArray(totalPoints);
  int *checked = checkedPtr->getPointer(0);

  for (int iter = 0; iter < (xpoints * ypoints * zpoints); iter++)
  {
    gnames[iter] = -1;
    checked[iter] = 0;
  }
  index = 0;
  while (voxels[index].imagequality > minseedimagequality && voxels[index].confidence > minseedconfidence)
  {
    index++;
  }
  voxelslist[count] = index;
  checked[index] = 1;
  count++;
  for (size_t i = 0; i < count; i++)
  {
    index = voxelslist[i];
    col = index % xpoints;
    row = (index / xpoints) % ypoints;
    plane = index / (xpoints * ypoints);
    for (int j = 0; j < 6; j++)
    {
      good = 1;
      neighbor = index + neighbors[j];
      if (j == 0 && plane == 0) good = 0;
      if (j == 5 && plane == (zpoints - 1)) good = 0;
      if (j == 1 && row == 0) good = 0;
      if (j == 4 && row == (ypoints - 1)) good = 0;
      if (j == 2 && col == 0) good = 0;
      if (j == 3 && col == (xpoints - 1)) good = 0;
      if (good == 1 && checked[neighbor] == 0)
      {
        if (voxels[neighbor].imagequality < minseedimagequality || voxels[neighbor].confidence < minseedconfidence)
        {
          gnames[neighbor] = 0;
          checked[neighbor] = 1;
          voxelslist[count] = neighbor;
          count++;
          if (count >= voxelslist.size()) voxelslist.resize(count + initialVoxelsListSize, -1);
        }
      }
    }
  }
  voxelslist.clear();
  voxelslist.resize(initialVoxelsListSize, -1);
  for (int iter = 0; iter < (xpoints * ypoints * zpoints); iter++)
  {
    checked[iter] = 0;
  }
  index = 0;
  while (gnames[index] != -1)
  {
    index++;
  }
  count = 0;
  voxelslist[count] = index;
  checked[index] = 1;
  count++;
  for (size_t j = 0; j < count; j++)
  {
    currentpoint = voxelslist[j];
    col = currentpoint % xpoints;
    row = (currentpoint / xpoints) % ypoints;
    plane = currentpoint / (xpoints * ypoints);
    q1[0] = 0;
    q1[1] = voxels[currentpoint].quat[1];
    q1[2] = voxels[currentpoint].quat[2];
    q1[3] = voxels[currentpoint].quat[3];
    q1[4] = voxels[currentpoint].quat[4];
    phase1 = crystruct[voxels[currentpoint].phase];
    for (int i = 1; i < 6; i++)
    {
      good = 1;
      neighbor = currentpoint + neighbors[i];
      if (i == 0 && plane == 0) good = 0;
      if (i == 5 && plane == (zpoints - 1)) good = 0;
      if (i == 1 && row == 0) good = 0;
      if (i == 4 && row == (ypoints - 1)) good = 0;
      if (i == 2 && col == 0) good = 0;
      if (i == 3 && col == (xpoints - 1)) good = 0;
      if (good == 1 && gnames[neighbor] == -1 && checked[neighbor] == 0)
      {
        voxelslist[count] = neighbor;
        checked[neighbor] = 1;
        count++;
        if (count >= voxelslist.size()) voxelslist.resize(count + initialVoxelsListSize, -1);
      }
      if (good == 1 && gnames[neighbor] == 0)
      {
        w = 10000.0;
        q2[0] = 0;
        q2[1] = voxels[neighbor].quat[1];
        q2[2] = voxels[neighbor].quat[2];
        q2[3] = voxels[neighbor].quat[3];
        q2[4] = voxels[neighbor].quat[4];
        phase2 = crystruct[voxels[neighbor].phase];
        if (phase1 == phase2) {
          w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
        }
        if (w < misorientationtolerance)
        {
          gnames[neighbor] = -1;
          checked[neighbor] = 1;
          voxelslist[count] = neighbor;
          count++;
          if (count >= voxelslist.size()) voxelslist.resize(count + initialVoxelsListSize, -1);
        }
      }
    }
  }
  voxelslist.clear();
  for (int iter = 0; iter < (xpoints * ypoints * zpoints); iter++)
  {
    voxels[iter].grainname = gnames[iter];
  }

}

void ReconstructionFunc::align_sections(const std::string &filename)
{

  ofstream outFile;
  outFile.open(filename.c_str());
  float disorientation = 0;
  float mindisorientation = 100000000;
  float **mutualinfo12;
  float *mutualinfo1;
  float *mutualinfo2;
  int graincount1, graincount2;
  int xshift = 0;
  int yshift = 0;
  int tempxshift = 0;
  int tempyshift = 0;
  int count = 0;
  int step = 0;
  int slice = 0;
  int nsteps = 0;
  int xspot, yspot;
  float w;
  float n1, n2, n3;
  float q1[5];
  float q2[5];
  float refci, curci, refiq, curiq;
  float refxcentroid, refycentroid;
  float curxcentroid, curycentroid;
  int refgnum, curgnum;
  int refposition = 0;
  int curposition = 0;
  int position;
  int tempposition;
  AIM::Reconstruction::CrystalStructure phase1, phase2;

  int** shifts = new int *[zpoints];
  for (int a = 0; a < zpoints; a++)
  {
    shifts[a] = new int[2];
    for (int b = 0; b < 2; b++)
    {
      shifts[a][b] = 0;
    }
  }
  if (alignmeth == AIM::Reconstruction::OuterBoundary)
  {
    refxcentroid = 0;
    refycentroid = 0;
    count = 0;
    slice = (zpoints - 1);
    for (int l = 0; l < ypoints; l++)
    {
      for (int m = 0; m < xpoints; m++)
      {
        refposition = ((slice) * xpoints * ypoints) + (l * xpoints) + m;
        refxcentroid = refxcentroid + (((resx * m) + (resx / 2.0)) * (float(voxels[refposition].grainname)));
        refycentroid = refycentroid + (((resy * l) + (resy / 2.0)) * (float(voxels[refposition].grainname)));
        count = count + voxels[refposition].grainname;
      }
    }
    refxcentroid = refxcentroid / float(count);
    refycentroid = refycentroid / float(count);
  }
  for (int iter = 1; iter < zpoints; iter++)
  {
    outFile << endl;
    outFile << endl;
    mindisorientation = 100000000;
    slice = (zpoints - 1) - iter;
    if (alignmeth == AIM::Reconstruction::MutualInformation)
    {
      graincount1 = graincounts[slice];
      graincount2 = graincounts[slice + 1];
      mutualinfo12 = new float *[graincount1];
      mutualinfo1 = new float[graincount1];
      mutualinfo2 = new float[graincount2];
      for (int a = 0; a < graincount1; a++)
      {
        mutualinfo1[a] = 0;
        mutualinfo12[a] = new float[graincount2];
        for (int b = 0; b < graincount2; b++)
        {
          mutualinfo12[a][b] = 0;
          mutualinfo2[b] = 0;
        }
      }
    }
    tempxshift = 0;
    tempyshift = 0;
    if (alignmeth == AIM::Reconstruction::OuterBoundary)
    {
      curxcentroid = 0;
      curycentroid = 0;
      count = 0;
      slice = (zpoints - 1) - iter;
      for (int l = 0; l < ypoints; l++)
      {
        for (int m = 0; m < xpoints; m++)
        {
          curposition = ((slice) * xpoints * ypoints) + (l * xpoints) + m;
          curxcentroid = curxcentroid + (((resx * m) + (resx / 2.0)) * (float(voxels[curposition].grainname)));
          curycentroid = curycentroid + (((resy * l) + (resy / 2.0)) * (float(voxels[curposition].grainname)));
          count = count + voxels[curposition].grainname;
        }
      }
      curxcentroid = curxcentroid / float(count);
      curycentroid = curycentroid / float(count);
    }
    if (alignmeth >= 1)
    {
      for (int a = 0; a < 2; a++)
      {
        if (a == 0) step = 3, nsteps = 3;
        if (a == 1) step = 1, nsteps = 3;
        for (int j = -nsteps; j < (nsteps + 1); j++)
        {
          outFile << endl;
          for (int k = -nsteps; k < (nsteps + 1); k++)
          {
            disorientation = 0;
            count = 0;
            for (int l = 0; l < ypoints; l++)
            {
              for (int m = 0; m < xpoints; m++)
              {
                count++;
                if ((l + (j * step) + tempyshift) >= 0 && (l + (j * step) + tempyshift) < ypoints && (m + (k * step) + tempxshift) >= 0 && (m + (k * step)
                    + tempxshift) < xpoints)
                {
                  refposition = ((slice + 1) * xpoints * ypoints) + (l * xpoints) + m;
                  curposition = (slice * xpoints * ypoints) + ((l + (j * step) + tempyshift) * xpoints) + (m + (k * step) + tempxshift);
                  refci = voxels[refposition].confidence;
                  curci = voxels[curposition].confidence;
                  refiq = voxels[refposition].imagequality;
                  curiq = voxels[curposition].imagequality;
                  refgnum = voxels[refposition].grainname;
                  curgnum = voxels[curposition].grainname;
                  if (alignmeth == AIM::Reconstruction::MutualInformation)
                  {
                    if (curgnum >= 0 && refgnum >= 0)
                    {
                      mutualinfo12[curgnum][refgnum]++;
                      mutualinfo1[curgnum]++;
                      mutualinfo2[refgnum]++;
                    }
                  }
                  if (alignmeth == AIM::Reconstruction::Misorientation)
                  {
                    if (refiq > minseedimagequality && curiq > minseedimagequality)
                    {
                      w = 10000.0;
                      q1[1] = voxels[refposition].quat[1];
                      q1[2] = voxels[refposition].quat[2];
                      q1[3] = voxels[refposition].quat[3];
                      q1[4] = voxels[refposition].quat[4];
                      phase1 = crystruct[voxels[refposition].phase];
                      q2[1] = voxels[curposition].quat[1];
                      q2[2] = voxels[curposition].quat[2];
                      q2[3] = voxels[curposition].quat[3];
                      q2[4] = voxels[curposition].quat[4];
                      phase2 = crystruct[voxels[curposition].phase];
                      if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
                      if (w > misorientationtolerance) disorientation++;
                    }
                    if (refiq < minseedimagequality && curiq > minseedimagequality) disorientation++;
                    if (refiq > minseedimagequality && curiq < minseedimagequality) disorientation++;
                  }
                }
                else
                {
                  if (alignmeth == AIM::Reconstruction::MutualInformation)
                  {
                    mutualinfo12[0][0]++;
                    mutualinfo1[0]++;
                    mutualinfo2[0]++;
                  }
                  //if (alignmeth == AIM::Reconstruction::Misorientation) disorientation = disorientation;
                }
              }
            }
            if (alignmeth == AIM::Reconstruction::MutualInformation)
            {
              float ha = 0;
              float hb = 0;
              float hab = 0;
              for (int b = 0; b < graincount1; b++)
              {
                mutualinfo1[b] = mutualinfo1[b] / float(count);
                if (mutualinfo1[b] != 0) ha = ha + mutualinfo1[b] * log(mutualinfo1[b]);
              }
              for (int c = 0; c < graincount2; c++)
              {
                mutualinfo2[c] = mutualinfo2[c] / float(count);
                if (mutualinfo2[c] != 0) hb = hb + mutualinfo2[c] * log(mutualinfo2[c]);
              }
              for (int b = 0; b < graincount1; b++)
              {
                for (int c = 0; c < graincount2; c++)
                {
                  mutualinfo12[b][c] = mutualinfo12[b][c] / float(count);
                  if (mutualinfo12[b][c] != 0) hab = hab + mutualinfo12[b][c] * log(mutualinfo12[b][c]);
                  float value = 0;
                  if (mutualinfo1[b] > 0 && mutualinfo2[c] > 0) value = (mutualinfo12[b][c] / (mutualinfo1[b] * mutualinfo2[c]));
                  if (value != 0) disorientation = disorientation + (mutualinfo12[b][c] * log(value));
                }
              }
              for (int b = 0; b < graincount1; b++)
              {
                for (int c = 0; c < graincount2; c++)
                {
                  mutualinfo12[b][c] = 0;
                  mutualinfo1[b] = 0;
                  mutualinfo2[c] = 0;
                }
              }
              //    float disorientation2 = ha + hb - hab;
              disorientation = 1.0 / disorientation;
            }
            outFile << disorientation << "  ";
            if (disorientation < mindisorientation)
            {
              xshift = (k * step) + tempxshift;
              yshift = (j * step) + tempyshift;
              mindisorientation = disorientation;
            }
          }
        }
        tempxshift = xshift;
        tempyshift = yshift;
      }
      shifts[iter][0] = shifts[iter - 1][0] + xshift;
      shifts[iter][1] = shifts[iter - 1][1] + yshift;
    }
    if (alignmeth == AIM::Reconstruction::OuterBoundary)
    {
      xshift = int(((curxcentroid - refxcentroid) / resx) + 0.5);
      yshift = int(((curycentroid - refycentroid) / resy) + 0.5);
      shifts[iter][0] = xshift;
      shifts[iter][1] = yshift;
    }
  }
  for (int iter = 1; iter < zpoints; iter++)
  {
    slice = (zpoints - 1) - iter;
    for (int l = 0; l < ypoints; l++)
    {
      for (int m = 0; m < xpoints; m++)
      {
        if (shifts[iter][1] >= 0) yspot = l;
        if (shifts[iter][0] >= 0) xspot = m;
        if (shifts[iter][1] < 0) yspot = ypoints - 1 - l;
        if (shifts[iter][0] < 0) xspot = xpoints - 1 - m;
        position = (slice * xpoints * ypoints) + (yspot * xpoints) + xspot;
        tempposition = (slice * xpoints * ypoints) + ((yspot + shifts[iter][1]) * xpoints) + (xspot + shifts[iter][0]);
        if ((yspot + shifts[iter][1]) >= 0 && (yspot + shifts[iter][1]) <= ypoints - 1 && (xspot + shifts[iter][0]) >= 0 && (xspot + shifts[iter][0])
            <= xpoints - 1)
        {
          voxels[position].euler1 = voxels[tempposition].euler1;
          voxels[position].euler2 = voxels[tempposition].euler2;
          voxels[position].euler3 = voxels[tempposition].euler3;
          voxels[position].quat[0] = voxels[tempposition].quat[0];
          voxels[position].quat[1] = voxels[tempposition].quat[1];
          voxels[position].quat[2] = voxels[tempposition].quat[2];
          voxels[position].quat[3] = voxels[tempposition].quat[3];
          voxels[position].quat[4] = voxels[tempposition].quat[4];
          voxels[position].confidence = voxels[tempposition].confidence;
          voxels[position].imagequality = voxels[tempposition].imagequality;
          voxels[position].grainname = voxels[tempposition].grainname;
        }
        if ((yspot + shifts[iter][1]) < 0 || (yspot + shifts[iter][1]) > ypoints - 1 || (xspot + shifts[iter][0]) < 0 || (xspot + shifts[iter][0]) > xpoints
            - 1)
        {
          voxels[position].euler1 = 0.0;
          voxels[position].euler2 = 0.0;
          voxels[position].euler3 = 0.0;
          voxels[position].quat[0] = 0.0;
          voxels[position].quat[1] = 0.0;
          voxels[position].quat[2] = 0.0;
          voxels[position].quat[3] = 0.0;
          voxels[position].quat[4] = 1.0;
          voxels[position].confidence = 0.0;
          voxels[position].imagequality = 0.0;
          voxels[position].grainname = 0;
        }
      }
    }
  }

  // Clean up the memory
  for (int a = 0; a < zpoints; a++)
  {
    delete [] shifts[a];
  }
  delete [] shifts;
  outFile.close();
}
void ReconstructionFunc::form_grains_sections()
{
  int point = 0;
  int noseeds = 0;
  int graincount = 1;
  int neighbor;
  // int currentpoint;
  float q1[5];
  float q2[5];
  float qs[5];
  float w;
  float n1;
  float n2;
  float n3;
  int seed = -1;
  int randx = 0;
  int randy = 0;
  int good = 0;
  int x, y, z;
  int col, row;
  size_t size = 0;
  size_t initialVoxelsListSize = 1000;

  graincounts.reset(new int[zpoints]);

  std::vector<int > voxelslist(initialVoxelsListSize, -1);
  int neighbors[8];
  neighbors[0] = -xpoints - 1;
  neighbors[1] = -xpoints;
  neighbors[2] = -xpoints + 1;
  neighbors[3] = -1;
  neighbors[4] = 1;
  neighbors[5] = xpoints - 1;
  neighbors[6] = xpoints;
  neighbors[7] = xpoints + 1;
  rg.RandomInit((static_cast<unsigned int > (time(NULL))));
  AIM::Reconstruction::CrystalStructure phase1, phase2;
  for (int slice = 0; slice < zpoints; slice++)
  {
    graincount = 1;
    noseeds = 0;
    while (noseeds == 0)
    {
      seed = -1;
      randx = int(float(rg.Random()) * float(xpoints));
      randy = int(float(rg.Random()) * float(ypoints));
      for (int j = 0; j < ypoints; ++j)
      {
        for (int i = 0; i < xpoints; ++i)
        {
          x = randx + i;
          y = randy + j;
          z = slice;
          if (x > xpoints - 1) x = x - xpoints;
          if (y > ypoints - 1) y = y - ypoints;
          point = (z * xpoints * ypoints) + (y * xpoints) + x;
          float confidence = voxels[point].confidence;
          float imagequality = voxels[point].imagequality;
          //  int grainname = voxels[point].grainname;
          if (confidence > minseedconfidence && imagequality > minseedimagequality && voxels[point].grainname == -1)
          {
            seed = point;
          }
          if (seed > -1)
          {
            break;
          }
        }
        if (seed > -1)
        {
          break;
        }
      }
      if (seed == -1)
      {
        noseeds = 1;
      }
      if (seed >= 0)
      {
        size = 0;
        voxels[seed].grainname = graincount;
        voxelslist[size] = seed;
        size++;
        qs[0] = 0;
        qs[1] = voxels[seed].quat[1];
        qs[2] = voxels[seed].quat[2];
        qs[3] = voxels[seed].quat[3];
        qs[4] = voxels[seed].quat[4];
        for (size_t j = 0; j < size; ++j)
        {
          int currentpoint = voxelslist[j];
          col = currentpoint % xpoints;
          row = (currentpoint / xpoints) % ypoints;
          q1[0] = 0;
          q1[1] = voxels[currentpoint].quat[1];
          q1[2] = voxels[currentpoint].quat[2];
          q1[3] = voxels[currentpoint].quat[3];
          q1[4] = voxels[currentpoint].quat[4];
		  phase1 = crystruct[voxels[currentpoint].phase];
          for (int i = 0; i < 8; i++)
          {
            good = 1;
            neighbor = currentpoint + neighbors[i];
            if ((i == 0 || i == 1 || i == 2) && row == 0) good = 0;
            if ((i == 5 || i == 6 || i == 7) && row == (ypoints - 1)) good = 0;
            if ((i == 0 || i == 3 || i == 5) && col == 0) good = 0;
            if ((i == 2 || i == 4 || i == 7) && col == (xpoints - 1)) good = 0;
            if (good == 1 && voxels[neighbor].grainname <= 0)
            {
			  w = 10000.0;
              q2[0] = 0;
              q2[1] = voxels[neighbor].quat[1];
              q2[2] = voxels[neighbor].quat[2];
              q2[3] = voxels[neighbor].quat[3];
              q2[4] = voxels[neighbor].quat[4];
			  phase2 = crystruct[voxels[neighbor].phase];
			  if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
              if (w < misorientationtolerance)
              {
                voxels[neighbor].grainname = graincount;
                voxelslist[size] = neighbor;
                size++;
                if (size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
              }
            }
          }
        }
        voxelslist.erase(std::remove(voxelslist.begin(), voxelslist.end(), -1), voxelslist.end());
        graincount++;
        voxelslist.clear();
        voxelslist.resize(initialVoxelsListSize, -1);
      }
    }
    graincounts[slice] = graincount;
  }
}

void ReconstructionFunc::form_grains()
{
  int noseeds = 0;
  size_t graincount = 1;
  int neighbor;
  float q1[5];
  float q2[5];
  float qa[5];
  float qb[5];
  float w;
  float n1, n2, n3;
  int seed = -1;
//  int point = 0;
  int randpoint = 0;
  int good = 0;
  int col, row, plane;
  size_t size = 0;
  size_t initialVoxelsListSize = 1000;
  size_t initialMergeListSize = 10;
  int vid;
  std::vector<int > voxelslist(initialVoxelsListSize, -1);
  std::vector<int >* vlist;
  std::vector<int > mergelist(initialMergeListSize, -1);
  int neighbors[6];
  neighbors[0] = -(xpoints * ypoints);
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = (xpoints * ypoints);
  rg.RandomInit((static_cast<unsigned int > (time(NULL))));
  AIM::Reconstruction::CrystalStructure phase1, phase2;

  // Precalculate some constants
  int totalPMinus1 = totalpoints - 1;


  // Copy all the grain names into a densly packed array
  IntArray gnames(new int[totalpoints]);
//  IntArrayType::Pointer grainNames = IntArrayType::CreateArray(totalpoints);
//  int* gnames = grainNames->getPointer(0);
  FloatArray iqs(new float[totalpoints]);
//  DoubleArrayType::Pointer m_IQs = DoubleArrayType::CreateArray(totalpoints);
//  float* iqs = m_IQs->getPointer(0);
  for (int i = 0; i < totalpoints; ++i)
  {
    gnames[i] = voxels[i].grainname;
    iqs[i] = voxels[i].imagequality;
  }

  // Create initial set of grain average quaternions

  FloatArrayType::Pointer m_grainQuats = FloatArrayType::CreateArray(1000*5);
  m_grainQuats->initializeWithZeros();
  float* grainquats = m_grainQuats->getPointer(0);

  IntArrayType::Pointer m_grainPhases = IntArrayType::CreateArray(1000*5);
  int* gphases = m_grainPhases->getPointer(0);

  for(int i=0;i<1000*5;i++)
  {
	  grainquats[i] = 0.0;
	  gphases[i] = 0;
  }

  // Burn volume with tight orientation tolerance to simulate simultaneous growth/aglomeration
  while (noseeds == 0)
  {
    seed = -1;
    int counter = 0;
    randpoint = int(float(rg.Random()) * float(totalPMinus1));
    while (seed == -1 && counter < totalpoints)
    {
      if (randpoint > totalPMinus1) randpoint = randpoint - totalpoints;
      if (gnames[randpoint] == -1 && iqs[randpoint] > minseedimagequality) seed = randpoint;
      randpoint++;
      counter++;
    }
    if (seed == -1)
    {
      noseeds = 1;
    }
    if (seed >= 0)
    {
      size = 0;
      gnames[seed] = graincount;
      voxelslist[size] = seed;
      size++;
      gphases[graincount] = voxels[seed].phase;
      for (int k = 0; k < 5; k++)
      {
        grainquats[graincount * 5 + k] = grainquats[graincount * 5 + k] + voxels[seed].quat[k];
      }
      for (size_t j = 0; j < size; ++j)
      {
        int currentpoint = voxelslist[j];
        col = currentpoint % xpoints;
        row = (currentpoint / xpoints) % ypoints;
        plane = currentpoint / (xpoints * ypoints);
        //        q1[0] = 1;
        //        q1[1] = voxels[currentpoint].quat[1];
        //        q1[2] = voxels[currentpoint].quat[2];
        //        q1[3] = voxels[currentpoint].quat[3];
        //        q1[4] = voxels[currentpoint].quat[4];
        phase1 = crystruct[voxels[currentpoint].phase];
        for (int i = 0; i < 6; i++)
        {
          q1[0] = 1;
          q1[1] = grainquats[graincount * 5 + 1] / grainquats[graincount * 5];
          q1[2] = grainquats[graincount * 5 + 2] / grainquats[graincount * 5];
          q1[3] = grainquats[graincount * 5 + 3] / grainquats[graincount * 5];
          q1[4] = grainquats[graincount * 5 + 4] / grainquats[graincount * 5];
          good = 1;
          neighbor = currentpoint + neighbors[i];
          if (i == 0 && plane == 0) good = 0;
          if (i == 5 && plane == (zpoints - 1)) good = 0;
          if (i == 1 && row == 0) good = 0;
          if (i == 4 && row == (ypoints - 1)) good = 0;
          if (i == 2 && col == 0) good = 0;
          if (i == 3 && col == (xpoints - 1)) good = 0;
          if (good == 1 && gnames[neighbor] == -1)
          {
            w = 10000.0;
            q2[0] = 1;
            q2[1] = voxels[neighbor].quat[1];
            q2[2] = voxels[neighbor].quat[2];
            q2[3] = voxels[neighbor].quat[3];
            q2[4] = voxels[neighbor].quat[4];
            phase2 = crystruct[voxels[neighbor].phase];
            if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
            if (w < 5.0)
            {
              gnames[neighbor] = graincount;
              for (int k = 0; k < 5; k++)
              {
                qa[k] = voxels[seed].quat[k];
                qb[k] = voxels[neighbor].quat[k];
              }
              if (phase1 == phase2) m_OrientationOps[phase1]->getNearestQuat(qa, qb);
              for (int k = 0; k < 5; k++)
              {
                voxels[neighbor].quat[k] = qb[k];
                grainquats[graincount * 5 + k] = grainquats[graincount * 5 + k] + voxels[neighbor].quat[k];
              }
              voxelslist[size] = neighbor;
              size++;
              if (size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
            }
          }
        }
      }
      voxelslist.erase(std::remove(voxelslist.begin(), voxelslist.end(), -1), voxelslist.end());
      if (m_Grains[graincount]->voxellist != NULL)
      {
        delete m_Grains[graincount]->voxellist;
      }
      m_Grains[graincount]->voxellist = new std::vector<int>(voxelslist.size());
      m_Grains[graincount]->voxellist->swap(voxelslist);
      m_Grains[graincount]->active = 1;
      m_Grains[graincount]->phase = voxels[seed].phase;
      graincount++;
      if (graincount >= m_Grains.size())
      {
        size_t oldSize = m_Grains.size();
        m_Grains.resize(m_Grains.size() + 100);
        for (size_t g = oldSize; g < m_Grains.size(); ++g)
        {
          m_Grains[g] = Grain::New();
        }
      }
      if (graincount >= m_grainQuats->getNumberOfElements() / 5)
      {
        // This will allocate a new array and copy all the old values to the new array
        m_grainQuats->resize((graincount + 100) * 5);
        grainquats = m_grainQuats->getPointer(0);
        m_grainPhases->resize((graincount + 100));
        gphases = m_grainPhases->getPointer(0);
        for (size_t i = graincount; i < graincount + 100; i++)
        {
          gphases[i] = 0;
          for (size_t j = 0; j < 5; j++)
          {
            grainquats[i * 5 + j] = 0.0;
          }
        }
      }
      voxelslist.clear();
      voxelslist.resize(initialVoxelsListSize, -1);
    }
  }


  IntArrayType::Pointer mergedgrainNames = IntArrayType::CreateArray(graincount);
  int* mergedgnames = mergedgrainNames->getPointer(0);
  IntArrayType::Pointer newgrainNames = IntArrayType::CreateArray(graincount);
  int* newgnames = newgrainNames->getPointer(0);

  for(size_t i=0;i<graincount;i++)
  {
   mergedgnames[i] = i;
   newgnames[i] = i;
  }
  for(size_t i=1;i<graincount;i++)
  {
    if (m_Grains[i]->active == 1)
    {
      size = 0;
      mergelist[size] = i;
      size++;
      for (size_t j = 0; j < size; j++)
      {
        vlist = m_Grains[mergelist[j]]->voxellist;
        size_t vlistSize = vlist->size();
        for (size_t k = 0; k < vlistSize; k++)
        {
          vid = vlist->at(k);
          col = vid % xpoints;
          row = (vid / xpoints) % ypoints;
          plane = vid / (xpoints * ypoints);
          for (int l = 0; l < 6; l++)
          {
            good = 1;
            neighbor = vid + neighbors[l];
            if (l == 0 && plane == 0) good = 0;
            if (l == 5 && plane == (zpoints - 1)) good = 0;
            if (l == 1 && row == 0) good = 0;
            if (l == 4 && row == (ypoints - 1)) good = 0;
            if (l == 2 && col == 0) good = 0;
            if (l == 3 && col == (xpoints - 1)) good = 0;
            if (good == 1 && gnames[neighbor] != i && gnames[neighbor] > 0)
            {
              if (m_Grains[gnames[neighbor]]->active == 1)
              {
                w = 10000.0;
                q1[0] = grainquats[i * 5];
                q1[1] = grainquats[i * 5 + 1] / grainquats[i * 5];
                q1[2] = grainquats[i * 5 + 2] / grainquats[i * 5];
                q1[3] = grainquats[i * 5 + 3] / grainquats[i * 5];
                q1[4] = grainquats[i * 5 + 4] / grainquats[i * 5];
                phase1 = crystruct[gphases[i]];
                q2[0] = grainquats[gnames[neighbor] * 5];
                q2[1] = grainquats[gnames[neighbor] * 5 + 1] / grainquats[gnames[neighbor] * 5];
                q2[2] = grainquats[gnames[neighbor] * 5 + 2] / grainquats[gnames[neighbor] * 5];
                q2[3] = grainquats[gnames[neighbor] * 5 + 3] / grainquats[gnames[neighbor] * 5];
                q2[4] = grainquats[gnames[neighbor] * 5 + 4] / grainquats[gnames[neighbor] * 5];
                phase2 = crystruct[gphases[gnames[neighbor]]];
                if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
                if (w < misorientationtolerance)
                {
                  mergelist[size] = gnames[neighbor];
                  size++;
                  m_Grains[gnames[neighbor]]->active = 0;
                  mergedgnames[gnames[neighbor]] = i;
                  if (phase1 == phase2) m_OrientationOps[phase1]->getNearestQuat(qa, qb);
                  for (int m = 0; m < 5; m++)
                  {
                    q2[m] = q2[m] * q2[0];
                    grainquats[i * 5 + m] = grainquats[i * 5 + m] + q2[m];
                  }
                  if (size >= mergelist.size()) mergelist.resize(size + initialMergeListSize, -1);
                }
              }
            }
          }
        }
      }
      mergelist.clear();
      mergelist.resize(initialMergeListSize, -1);
    }
  }

  int goodgraincount = 1;
  for(size_t i = 0; i < graincount; ++i)
  {
    if(m_Grains[i]->active == 1)
    {
    newgnames[i] = goodgraincount;
    m_Grains[goodgraincount]->active = 1;
    goodgraincount++;
    }
  }
  // Copy the grain names back into the Voxel objects
  int mergedname;
  int newname;
  for (int i = 0; i < totalpoints; ++i)
  {
    if(gnames[i] >= 0)
	{
	    mergedname = mergedgnames[gnames[i]];
		while(mergedgnames[mergedname] != mergedname)
		{
		  mergedname = mergedgnames[mergedname];
		}
		newname = newgnames[mergedname];
	    voxels[i].grainname = newname;
	}
  }
  size_t oldSize = m_Grains.size();
  m_Grains.resize(goodgraincount);
  for(size_t g = oldSize; g < m_Grains.size(); ++g)
  {
    m_Grains[g] = Grain::New();
  }

  goodgraincount = remove_smallgrains(goodgraincount);
  oldSize = m_Grains.size();
  m_Grains.resize(goodgraincount);
  for(size_t g = oldSize; g < m_Grains.size(); ++g)
  {
    m_Grains[g] = Grain::New();
  }
  m_grainQuats = AIMArray<float >::NullPointer(); // Clean up the array to release some memory

  assign_badpoints();
}

void ReconstructionFunc::assign_badpoints()
{
  vector<int > neighs;
  vector<int > remove;
  int count = 1;
  int good = 1;
  float x, y, z;
  int neighpoint;
  int neighbors[6];
  size_t numgrains = m_Grains.size();
  std::vector<int > n(numgrains + 1);
  neighbors[0] = -xpoints * ypoints;
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = xpoints * ypoints;


  IntArray gnames (new int[totalpoints]);
  IntArray gneighs (new int[totalpoints]);

  for (int i = 0; i < totalpoints; ++i)
  {
    gnames[i] = voxels[i].grainname;
    gneighs[i] = voxels[i].grainname;
  }
  while (count != 0)
  {
    count = 0;
    for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
    {
      int grainname = gnames[i];
      if (grainname == 0) voxels[i].unassigned = 1;
      if (grainname <= -1)
      {
        count++;
        voxels[i].unassigned = 1;
        for (size_t c = 1; c < numgrains; c++)
        {
          n[c] = 0;
        }
        x = i % xpoints;
        y = (i / xpoints) % ypoints;
        z = i / (xpoints * ypoints);
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = i + neighbors[j];
          if (j == 0 && z == 0) good = 0;
          if (j == 5 && z == (zpoints - 1)) good = 0;
          if (j == 1 && y == 0) good = 0;
          if (j == 4 && y == (ypoints - 1)) good = 0;
          if (j == 2 && x == 0) good = 0;
          if (j == 3 && x == (xpoints - 1)) good = 0;
          if (good == 1)
          {
            int grain = gnames[neighpoint];
            if (grain >= 0)
            {
              neighs.push_back(grain);
            }
          }
        }
        int current = 0;
        int most = 0;
        int curgrain = 0;
        int size = int(neighs.size());
        for (int k = 0; k < size; k++)
        {
          int neighbor = neighs[k];
          n[neighbor]++;
          current = n[neighbor];
          if (current > most)
          {
            most = current;
            curgrain = neighbor;
          }
        }
        if (size > 0)
        {
          gneighs[i] = curgrain;
          neighs.clear();
        }
      }
    }
    for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
    {
      int grainname = gnames[j];
      int neighbor = gneighs[j];
      if (grainname <= -1 && neighbor >= 0)
      {
        gnames[j] = neighbor;
        m_Grains[neighbor]->numvoxels++;
      }
    }
  }
  for (int i = 0; i < totalpoints; ++i)
  {
    voxels[i].grainname = gnames[i];
    voxels[i].neighbor = gneighs[i];
  }
}


void ReconstructionFunc::merge_containedgrains()
{
  for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    if (m_Grains[grainname]->numneighbors == 1)
    {
      m_Grains[grainname]->gotcontainedmerged = 1;
      voxels[i].grainname = m_Grains[grainname]->neighborlist->at(0);
      m_Grains[m_Grains[grainname]->neighborlist->at(0)]->numvoxels++;
      voxels[i].unassigned = 1;
    }
  }

}

void ReconstructionFunc::reorder_grains(const std::string &reconVisFile)
{
  size_t initialVoxelsListSize = 1000;
  size_t size = 0;
  int neighbors[26];
  int good = 0;
  int neighbor = 0;
  int col, row, plane;
  int gnum;
  float q1[5];
  float q2[5];
  float q[5];
  float ea1, ea2, ea3;
  size_t currentgrain = 1;
  AIM::Reconstruction::CrystalStructure phase;

  neighbors[0] = -(xpoints * ypoints) - xpoints - 1;
  neighbors[1] = -(xpoints * ypoints) - xpoints;
  neighbors[2] = -(xpoints * ypoints) - xpoints + 1;
  neighbors[3] = -(xpoints * ypoints) - 1;
  neighbors[4] = -(xpoints * ypoints);
  neighbors[5] = -(xpoints * ypoints) + 1;
  neighbors[6] = -(xpoints * ypoints) + xpoints - 1;
  neighbors[7] = -(xpoints * ypoints) + xpoints;
  neighbors[8] = -(xpoints * ypoints) + xpoints + 1;
  neighbors[9] = -xpoints - 1;
  neighbors[10] = -xpoints;
  neighbors[11] = -xpoints + 1;
  neighbors[12] = -1;
  neighbors[13] = 1;
  neighbors[14] = xpoints - 1;
  neighbors[15] = xpoints;
  neighbors[16] = xpoints + 1;
  neighbors[17] = (xpoints * ypoints) - xpoints - 1;
  neighbors[18] = (xpoints * ypoints) - xpoints;
  neighbors[19] = (xpoints * ypoints) - xpoints + 1;
  neighbors[20] = (xpoints * ypoints) - 1;
  neighbors[21] = (xpoints * ypoints);
  neighbors[22] = (xpoints * ypoints) + 1;
  neighbors[23] = (xpoints * ypoints) + xpoints - 1;
  neighbors[24] = (xpoints * ypoints) + xpoints;
  neighbors[25] = (xpoints * ypoints) + xpoints + 1;
  size_t numgrains = m_Grains.size();

  size_t maxGrain = 0;
  // Reset all the Grain nucleus values to -1;
  for (size_t i = 1; i < numgrains; i++)
  {
    m_Grains[i]->nucleus = -1;
	m_Grains[i]->voxellist->resize(1,0);
	m_Grains[i]->gotcontainedmerged = 0;
  }

  // Reset the "already checked" to 0 for all voxels
  for (int i = 0; i < totalpoints; i++)
  {
    voxels[i].alreadychecked = 0;
    if(voxels[i].unassigned == 0)
    {
      gnum = voxels[i].grainname;
      m_Grains[gnum]->nucleus = i;
    }
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    if(m_Grains[i]->nucleus != -1)
    {
      size = 0;
      int nucleus = m_Grains[i]->nucleus;
	  phase = crystruct[voxels[nucleus].phase];
      if(m_Grains[currentgrain]->voxellist == NULL)
      {
        if (NULL != m_Grains[currentgrain]->voxellist)
        {
          delete m_Grains[currentgrain]->voxellist;
          m_Grains[currentgrain]->voxellist = NULL;
        }
        m_Grains[currentgrain]->voxellist = new std::vector<int>(initialVoxelsListSize,-1);
      }
      else
      {
        m_Grains[currentgrain]->voxellist->resize(initialVoxelsListSize,-1);
      }
      m_Grains[currentgrain]->voxellist->at(size) = nucleus;
      voxels[nucleus].alreadychecked = 1;
      voxels[nucleus].grainname = currentgrain;
      if (currentgrain > maxGrain) maxGrain = currentgrain;
      size++;
      for (size_t k = 0; k < 5; k++)
      {
        m_Grains[currentgrain]->avg_quat[k] = 0.0;
      }
      for (size_t j = 0; j < size; j++)
      {
        int currentpoint = m_Grains[currentgrain]->voxellist->at(j);
        col = currentpoint % xpoints;
        row = (currentpoint / xpoints) % ypoints;
        plane = currentpoint / (xpoints * ypoints);
        if(voxels[currentpoint].unassigned == 0)
        {
          for (int k = 0; k < 5; k++)
          {
            q1[k] = voxels[nucleus].quat[k];
            q2[k] = voxels[currentpoint].quat[k];
          }
          m_OrientationOps[phase]->getNearestQuat(q1,q2);
          for (int k = 0; k < 5; k++)
          {
            voxels[currentpoint].quat[k] = q2[k];
            m_Grains[currentgrain]->avg_quat[k] = m_Grains[currentgrain]->avg_quat[k] + voxels[currentpoint].quat[k];
          }
        }
        for (int k = 0; k < 26; k++)
        {
          good = 1;
          neighbor = currentpoint + neighbors[k];
          if (k < 9 && plane == 0) good = 0;
          if (k > 16 && plane == (zpoints - 1)) good = 0;
          if ((k == 0 || k == 1 || k == 2 || k == 9 || k == 10 || k == 11 || k == 17 || k == 18 || k == 19) && row == 0) good = 0;
          if ((k == 6 || k == 7 || k == 8 || k == 14 || k == 15 || k == 16 || k == 23 || k == 24 || k == 25) && row == (ypoints - 1)) good = 0;
          if ((k == 0 || k == 3 || k == 6 || k == 9 || k == 12 || k == 14 || k == 17 || k == 20 || k == 23) && col == 0) good = 0;
          if ((k == 2 || k == 5 || k == 8 || k == 11 || k == 13 || k == 16 || k == 19 || k == 22 || k == 25) && col == (xpoints - 1)) good = 0;
          if (good == 1 && voxels[neighbor].alreadychecked == 0)
          {
            size_t grainname = voxels[neighbor].grainname;
            if (grainname == i)
            {
              m_Grains[currentgrain]->voxellist->at(size) = neighbor;
              voxels[neighbor].alreadychecked = 1;
              voxels[neighbor].grainname = currentgrain;
              if (currentgrain > maxGrain) maxGrain = currentgrain;
              size++;
              if (size >= m_Grains[currentgrain]->voxellist->size())
              {
                m_Grains[currentgrain]->voxellist->resize(size + initialVoxelsListSize, -1);
              }
            }
          }
        }
      }
      m_Grains[currentgrain]->voxellist->erase(std::remove(m_Grains[currentgrain]->voxellist->begin(), m_Grains[currentgrain]->voxellist->end(), -1), m_Grains[currentgrain]->voxellist->end());
      m_Grains[currentgrain]->active = 1;
      m_Grains[currentgrain]->nucleus = nucleus;
	  m_Grains[currentgrain]->phase = voxels[nucleus].phase;
      q[1] = m_Grains[currentgrain]->avg_quat[1]/m_Grains[currentgrain]->avg_quat[0];
      q[2] = m_Grains[currentgrain]->avg_quat[2]/m_Grains[currentgrain]->avg_quat[0];
      q[3] = m_Grains[currentgrain]->avg_quat[3]/m_Grains[currentgrain]->avg_quat[0];
      q[4] = m_Grains[currentgrain]->avg_quat[4]/m_Grains[currentgrain]->avg_quat[0];
	  OrientationMath::QuattoEuler(q, ea1, ea2, ea3);
	  m_Grains[currentgrain]->euler1 = ea1;
      m_Grains[currentgrain]->euler2 = ea2;
      m_Grains[currentgrain]->euler3 = ea3;
      currentgrain++;
    }
  }

  for (int i = 0; i < totalpoints; i++)
  {
	if(voxels[i].grainname >= currentgrain)
    {
      voxels[i].grainname = -2;
	  voxels[i].unassigned = 1;
    }
  }
  assign_badpoints();

  numgrains = currentgrain;
  // Resize the m_Grains vector by the appropriate amount
  size_t oldSize = m_Grains.size();
  m_Grains.resize(currentgrain);
  for(size_t g = oldSize; g < m_Grains.size(); ++g)
  {
    m_Grains[g] = Grain::New();
  }

  find_neighbors();
}

void ReconstructionFunc::fillin_sample()
{
  int col, row, plane;
  int point;
  int mincol, maxcol, minrow, maxrow, minplane, maxplane;
  mincol = xpoints;
  minrow = ypoints;
  minplane = zpoints;
  maxcol = 0;
  maxrow = 0;
  maxplane = 0;
  boost::shared_array<ReconstructionVoxel> voxelstemp;

  for (int k = 0; k < zpoints; ++k)
  {
    for (int j = 0; j < ypoints; ++j)
    {
      for (int i = 0; i < xpoints; ++i)
      {
        point = (k * xpoints * ypoints) + (j * xpoints) + i;
        if (voxels[point].grainname > 0)
        {
          col = i;
          row = j;
          plane = k;
          if (col < mincol) mincol = col;
          if (col > maxcol) maxcol = col;
          if (row < minrow) minrow = row;
          if (row > maxrow) maxrow = row;
          if (plane < minplane) minplane = plane;
          if (plane > maxplane) maxplane = plane;
        }
      }
    }
  }
  int newvoxelcount = 0;
  int newxpoints = (maxcol - mincol) + 1;
  int newypoints = (maxrow - minrow) + 1;
  int newzpoints = (maxplane - minplane) + 1;
  sizex = (maxcol - mincol) * resx;
  sizey = (maxrow - minrow) * resy;
  sizez = (maxplane - minplane) * resz;
  if (newxpoints != xpoints || newypoints != ypoints || newzpoints != zpoints)
  {
    voxelstemp.reset (new ReconstructionVoxel[(newxpoints * newypoints * newzpoints)] );
    for (int k = minplane; k < maxplane + 1; ++k)
    {
      for (int j = minrow; j < maxrow + 1; ++j)
      {
        for (int i = mincol; i < maxcol + 1; ++i)
        {
          point = (k * xpoints * ypoints) + (j * xpoints) + i;
          if (voxels[point].grainname == 0)
          {
            voxels[point].grainname = -1;
            voxels[point].neighbor = -1;
            voxels[point].unassigned = 1;
          }
          voxelstemp[newvoxelcount].deepCopy( &(voxels[point]));
          newvoxelcount++;
        }
      }
    }
    voxels.reset(NULL); // Same effect as delete [] voxels
    xpoints = newxpoints;
    ypoints = newypoints;
    zpoints = newzpoints;
    totalpoints = (xpoints * ypoints * zpoints);
    voxels.reset(new ReconstructionVoxel[totalpoints]);
    for (int i = 0; i < totalpoints; i++)
    {
      voxels[i].deepCopy(&(voxelstemp[i]));
    }

  }
  assign_badpoints();
}
int ReconstructionFunc::remove_smallgrains(size_t numgrains)
{
  size_t initialVoxelsListSize = 1000;
  std::vector<int > voxelslist;
  voxelslist.resize(initialVoxelsListSize,-1);
  size_t size = 0;
  int neighbors[26];
  int good = 0;
  int neighbor = 0;
  int col, row, plane;
  int gnum;
  int currentgrain = 1;
  neighbors[0] = -(xpoints * ypoints) - xpoints - 1;
  neighbors[1] = -(xpoints * ypoints) - xpoints;
  neighbors[2] = -(xpoints * ypoints) - xpoints + 1;
  neighbors[3] = -(xpoints * ypoints) - 1;
  neighbors[4] = -(xpoints * ypoints);
  neighbors[5] = -(xpoints * ypoints) + 1;
  neighbors[6] = -(xpoints * ypoints) + xpoints - 1;
  neighbors[7] = -(xpoints * ypoints) + xpoints;
  neighbors[8] = -(xpoints * ypoints) + xpoints + 1;
  neighbors[9] = -xpoints - 1;
  neighbors[10] = -xpoints;
  neighbors[11] = -xpoints + 1;
  neighbors[12] = -1;
  neighbors[13] = 1;
  neighbors[14] = xpoints - 1;
  neighbors[15] = xpoints;
  neighbors[16] = xpoints + 1;
  neighbors[17] = (xpoints * ypoints) - xpoints - 1;
  neighbors[18] = (xpoints * ypoints) - xpoints;
  neighbors[19] = (xpoints * ypoints) - xpoints + 1;
  neighbors[20] = (xpoints * ypoints) - 1;
  neighbors[21] = (xpoints * ypoints);
  neighbors[22] = (xpoints * ypoints) + 1;
  neighbors[23] = (xpoints * ypoints) + xpoints - 1;
  neighbors[24] = (xpoints * ypoints) + xpoints;
  neighbors[25] = (xpoints * ypoints) + xpoints + 1;
  for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
  {
    voxels[i].alreadychecked = 0;
	gnum = voxels[i].grainname;
	if(gnum >= 0) m_Grains[gnum]->nucleus = i;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
      size = 0;
      int nucleus = m_Grains[i]->nucleus;
      voxelslist[size] = nucleus;
      voxels[nucleus].alreadychecked = 1;
      voxels[nucleus].grainname = currentgrain;
      size++;
      for (size_t j = 0; j < size; j++)
      {
        int currentpoint = voxelslist[j];
        col = currentpoint % xpoints;
        row = (currentpoint / xpoints) % ypoints;
        plane = currentpoint / (xpoints * ypoints);
        for (size_t k = 0; k < 26; k++)
        {
          good = 1;
          neighbor = currentpoint + neighbors[k];
          if (k < 9 && plane == 0) good = 0;
          if (k > 16 && plane == (zpoints - 1)) good = 0;
          if ((k == 0 || k == 1 || k == 2 || k == 9 || k == 10 || k == 11 || k == 17 || k == 18 || k == 19) && row == 0) good = 0;
          if ((k == 6 || k == 7 || k == 8 || k == 14 || k == 15 || k == 16 || k == 23 || k == 24 || k == 25) && row == (ypoints - 1)) good = 0;
          if ((k == 0 || k == 3 || k == 6 || k == 9 || k == 12 || k == 14 || k == 17 || k == 20 || k == 23) && col == 0) good = 0;
          if ((k == 2 || k == 5 || k == 8 || k == 11 || k == 13 || k == 16 || k == 19 || k == 22 || k == 25) && col == (xpoints - 1)) good = 0;
          if (good == 1 && voxels[neighbor].alreadychecked == 0)
          {
            int grainname = voxels[neighbor].grainname;
            if (grainname == i)
            {
              voxelslist[size] = neighbor;
              voxels[neighbor].alreadychecked = 1;
              voxels[neighbor].grainname = currentgrain;
              size++;
              if (size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
            }
          }
        }
      }
      voxelslist.erase(std::remove(voxelslist.begin(), voxelslist.end(), -1), voxelslist.end());
    if(voxelslist.size() >= static_cast<size_t>(minallowedgrainsize) )
    {
      m_Grains[currentgrain]->active = 1;
      m_Grains[currentgrain]->numvoxels = voxelslist.size();
      currentgrain++;
      voxelslist.clear();
      voxelslist.resize(initialVoxelsListSize,-1);
    }
    if(voxelslist.size() < static_cast<size_t>(minallowedgrainsize) )
    {
        for (size_t b = 0; b < voxelslist.size(); b++)
        {
          int index = voxelslist[b];
          voxels[index].grainname = -2;
        }
        voxelslist.resize(initialVoxelsListSize, -1);
    }
  }
  return currentgrain;
}

void ReconstructionFunc::find_grain_and_kernel_misorientations()
{
  IntArray gnames(new int[totalpoints]);
  IntArray unassigned(new int[totalpoints]);
  FloatArray gam(new float[totalpoints]);

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
    gnames[i] = voxels[i].grainname;
    unassigned[i] = voxels[i].unassigned;
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
                    w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
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
          if (unassigned[point] == 0)
          {
            voxels[point].grainmisorientation = w;
            gam[point] = w;
            avgmiso[gnames[point]][0]++;
            avgmiso[gnames[point]][1] = avgmiso[gnames[point]][1] + w;
          }
          if (unassigned[point] != 0) voxels[point].grainmisorientation = 0;
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
  for (size_t i = 0; i < grainsSize; i++)
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

int ReconstructionFunc::load_data(const std::string &readname)
{
#if 1
  return m_Grains.size();
#else
  int bensdata = 1;
  int yoonsdata = 0;
  float **mat;
  float **rod;
  size_t numgrains = m_Grains.size();
  if (yoonsdata == 1)
  {
    const unsigned int size(1024);
    char buf[size];
    std::ifstream in(readname.c_str());
    std::string word;
    bool headerdone = false;
    while (headerdone == false)
    {
      in.getline(buf, size);
      std::string line = buf;
      in >> word;
      if (DIMS == word)
      {
        in >> xpoints >> ypoints >> zpoints;
        totalpoints = xpoints * ypoints * zpoints;
        delete[] voxels;
        voxels = new Voxel[totalpoints];
        mat = new float *[totalpoints];
        totalvol = float(totalpoints) * resx * resy * resz;
      }
      if (LOOKUP == word)
      {
        headerdone = true;
        in >> word;
      }
    }
    int gnum = 0;
    int onedge = 0;
    int col, row, plane;
    float value;
    for (int i = 0; i < totalpoints; i++)
    {
      mat[i] = new float[9];
      onedge = 0;
      in >> gnum;
      col = i % xpoints;
      row = (i / xpoints) % ypoints;
      plane = i / (xpoints * ypoints);
      if (col == 0 || col == (xpoints - 1) || row == 0 || row == (ypoints - 1) || plane == 0 || plane == (zpoints - 1)) onedge = 1;
      voxels[i].grainname = gnum;
      if (gnum >= numgrains)
      {
        numgrains = gnum + 1;
        size_t oldSize = m_Grains.size();
        m_Grains.resize(numgrains);
        for(size_t g = oldSize; g < m_Grains.size(); ++g)
        {
          m_Grains[g] = Grain::New();
        }
      }
      m_Grains[gnum]->surfacegrain = onedge;
    }
    for (int iter = 0; iter < 9; iter++)
    {
      headerdone = false;
      while (headerdone == false)
      {
        in.getline(buf, size);
        std::string line = buf;
        in >> word;
        if (LOOKUP == word)
        {
          headerdone = true;
          in >> word;
        }
      }
      for (int i = 0; i < totalpoints; i++)
      {
        onedge = 0;
        in >> value;
        mat[i][iter] = value;
      }
    }
    float s, c, s1, c1, s2, c2;
    float ea1, ea2, ea3;
    float cosine1, cosine3, sine1, sine3;
    float denom;
    for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
    {
      denom = mat[i][0] * mat[i][0] + mat[i][1] * mat[i][1] + mat[i][2] * mat[i][2];
      denom = powf(denom, 0.5);
      mat[i][0] = mat[i][0] / denom;
      mat[i][3] = mat[i][3] / denom;
      mat[i][6] = mat[i][6] / denom;
      denom = mat[i][3] * mat[i][3] + mat[i][4] * mat[i][4] + mat[i][5] * mat[i][5];
      denom = powf(denom, 0.5);
      mat[i][1] = mat[i][1] / denom;
      mat[i][4] = mat[i][4] / denom;
      mat[i][7] = mat[i][7] / denom;
      denom = mat[i][6] * mat[i][6] + mat[i][7] * mat[i][7] + mat[i][8] * mat[i][8];
      denom = powf(denom, 0.5);
      mat[i][2] = mat[i][2] / denom;
      mat[i][5] = mat[i][5] / denom;
      mat[i][8] = mat[i][8] / denom;
      if (mat[i][8] > 1) mat[i][8] = 1;
      if (mat[i][8] < -1) mat[i][8] = -1;
      ea2 = acos(mat[i][8]);
      cosine3 = (mat[i][5] / sin(ea2));
      sine3 = (mat[i][2] / sin(ea2));
      cosine1 = (-mat[i][7] / sin(ea2));
      sine1 = (mat[i][6] / sin(ea2));
      if (cosine3 > 1) cosine3 = 1;
      if (cosine3 < -1) cosine3 = -1;
      ea3 = acos(cosine3);
      if (cosine1 > 1) cosine1 = 1;
      if (cosine1 < -1) cosine1 = -1;
      ea1 = acos(cosine1);
      if (sine3 < 0) ea3 = (2 * 3.1415926535897) - ea3;
      if (sine1 < 0) ea1 = (2 * 3.1415926535897) - ea1;
      voxels[i].euler1 = ea1;
      voxels[i].euler2 = ea2;
      voxels[i].euler3 = ea3;
      s = sin(0.5 * ea2);
      c = cos(0.5 * ea2);
      s1 = sin(0.5 * (ea1 - ea3));
      c1 = cos(0.5 * (ea1 - ea3));
      s2 = sin(0.5 * (ea1 + ea3));
      c2 = cos(0.5 * (ea1 + ea3));
	  voxels[i].quat[0] = 1.0;
      voxels[i].quat[1] = s * c1;
      voxels[i].quat[2] = s * s1;
      voxels[i].quat[3] = c * s2;
      voxels[i].quat[4] = c * c2;
      delete[] mat[i];
    }
    delete[] mat;
  }
  if (bensdata == 1)
  {
    const unsigned int size(1024);
    char buf[size];
    std::ifstream in(readname.c_str());
    std::string word;
    bool headerdone = false;
    while (headerdone == false)
    {
      in.getline(buf, size);
      std::string line = buf;
      in >> word;
      if (DIMS == word)
      {
        in >> xpoints >> ypoints >> zpoints;
        totalpoints = xpoints * ypoints * zpoints;
        delete[] voxels;
        voxels = new Voxel[totalpoints];
        rod = new float *[totalpoints];
        totalvol = float(totalpoints) * resx * resy * resz;
      }
      if (LOOKUP == word)
      {
        headerdone = true;
        in >> word;
      }
    }
    int gnum = 0;
    int onedge = 0;
    int col, row, plane;
    float value;
    for (int i = 0; i < totalpoints; i++)
    {
      rod[i] = new float[3];
      onedge = 0;
      in >> gnum;
      col = i % xpoints;
      row = (i / xpoints) % ypoints;
      plane = i / (xpoints * ypoints);
      if (col == 0 || col == (xpoints - 1) || row == 0 || row == (ypoints - 1) || plane == 0 || plane == (zpoints - 1)) onedge = 1;
      voxels[i].grainname = gnum;
      if (gnum >= numgrains)
      {
        numgrains = gnum + 1;
        size_t oldSize = m_Grains.size();
        m_Grains.resize(numgrains);
        for(size_t g = oldSize; g < m_Grains.size(); ++g)
        {
          m_Grains[g] = Grain::New();
        }
      }
      m_Grains[gnum]->surfacegrain = onedge;
    }
    for (int iter = 0; iter < 3; iter++)
    {
      headerdone = false;
      while (headerdone == false)
      {
        in.getline(buf, size);
        std::string line = buf;
        in >> word;
        if (LOOKUP == word)
        {
          headerdone = true;
          in >> word;
        }
      }
      for (int i = 0; i < totalpoints; i++)
      {
        onedge = 0;
        in >> value;
        rod[i][iter] = value;
      }
    }
    float denom, mag;
	float q1[5];
    for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
    {
      denom = powf((rod[i][0] * rod[i][0] + rod[i][1] * rod[i][1] + rod[i][2] * rod[i][2]),0.5);
      rod[i][0] = rod[i][0] / denom;
      rod[i][1] = rod[i][1] / denom;
      rod[i][2] = rod[i][2] / denom;
      mag = powf((rod[i][0] * rod[i][0] + rod[i][1] * rod[i][1] + rod[i][2] * rod[i][2]),0.5);
	  q1[0] = 1.0;
	  q1[1] = rod[i][0];
	  q1[2] = rod[i][1];
	  q1[3] = rod[i][2];
	  q1[4] = cos(asin(mag));
      OrientationMath::getFZQuatCubic(q1);
	  voxels[i].quat[0] = q1[0];
      voxels[i].quat[1] = q1[1];
      voxels[i].quat[2] = q1[2];
      voxels[i].quat[3] = q1[3];
      voxels[i].quat[4] = q1[4];
      delete[] rod[i];
    }
    delete[] rod;
  }
  return int(numgrains);
#endif
}

void ReconstructionFunc::merge_twins()
{
  int twinmerged = 1;
  float angcur = 180;
  vector<int > twinlist;
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float q1[5];
  float q2[5];
  size_t numgrains = m_Grains.size();
  AIM::Reconstruction::CrystalStructure phase1, phase2;

  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_Grains[i]->twinnewnumber == -1)
    {
      twinlist.push_back(i);
      int tsize = int(twinlist.size());
      for (int m = 0; m < tsize; m++)
      {
        tsize = int(twinlist.size());
        int firstgrain = twinlist[m];
        IntVectorType nlist = m_Grains[firstgrain]->neighborlist;
        int size = int(nlist->size());
        for (int l = 0; l < size; l++)
        {
          angcur = 180;
          int twin = 0;
          size_t neigh = nlist->at(l);
          if (neigh != i && m_Grains[neigh]->twinnewnumber == -1)
          {
            w = 10000.0;
            q1[1] = m_Grains[firstgrain]->avg_quat[1];
            q1[2] = m_Grains[firstgrain]->avg_quat[2];
            q1[3] = m_Grains[firstgrain]->avg_quat[3];
            q1[4] = m_Grains[firstgrain]->avg_quat[4];
            phase1 = crystruct[m_Grains[firstgrain]->phase];
            q2[1] = m_Grains[neigh]->avg_quat[1];
            q2[2] = m_Grains[neigh]->avg_quat[2];
            q2[3] = m_Grains[neigh]->avg_quat[3];
            q2[4] = m_Grains[neigh]->avg_quat[4];
            phase2 = crystruct[m_Grains[neigh]->phase];
            if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
			OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
            float vecttol = 0.03;
            float rodvectdiff11 = fabs(fabs(r1) - (1.0 / 3.0));
            float rodvectdiff12 = fabs(fabs(r2) - (1.0 / 3.0));
            float rodvectdiff13 = fabs(fabs(r3) - (1.0 / 3.0));
            float rodvectdiff21 = fabs(fabs(r1) - 0.2);
            float rodvectdiff22 = fabs(fabs(r2) - 0.2);
            float rodvectdiff23 = fabs(fabs(r3) - 0.2);
            float rodvectdiff31 = fabs(fabs(r1) - 0.25);
            float rodvectdiff32 = fabs(fabs(r2) - 0.25);
            float rodvectdiff33 = fabs(fabs(r3) - 0.25);
            if (rodvectdiff11 < vecttol && rodvectdiff12 < vecttol && rodvectdiff13 < vecttol) twin = 1;
            if (rodvectdiff11 < vecttol && fabs(r2) < vecttol && fabs(r3) < vecttol) twin = 1;
            if (rodvectdiff12 < vecttol && fabs(r1) < vecttol && fabs(r3) < vecttol) twin = 1;
            if (rodvectdiff13 < vecttol && fabs(r1) < vecttol && fabs(r2) < vecttol) twin = 1;
            if (rodvectdiff11 < vecttol && rodvectdiff12 < vecttol && fabs(r3) < vecttol) twin = 1;
            if (rodvectdiff11 < vecttol && rodvectdiff13 < vecttol && fabs(r2) < vecttol) twin = 1;
            if (rodvectdiff12 < vecttol && rodvectdiff13 < vecttol && fabs(r1) < vecttol) twin = 1;
            if (rodvectdiff21 < vecttol && rodvectdiff22 < vecttol && rodvectdiff23 < vecttol) twin = 1;
            if (rodvectdiff31 < vecttol && rodvectdiff32 < vecttol && fabs(r3) < vecttol) twin = 1;
            if (rodvectdiff31 < vecttol && rodvectdiff33 < vecttol && fabs(r2) < vecttol) twin = 1;
            if (rodvectdiff32 < vecttol && rodvectdiff33 < vecttol && fabs(r1) < vecttol) twin = 1;
            if (w < angcur)
            {
              angcur = w;
            }
            if (twin == 1)
            {
              m_Grains[neigh]->gottwinmerged = twinmerged;
              m_Grains[neigh]->twinnewnumber = i;
              twinlist.push_back(neigh);
            }
          }
        }
      }
    }
    twinlist.clear();
  }
  for (int k = 0; k < (xpoints * ypoints * zpoints); k++)
  {
    int grainname = voxels[k].grainname;
    int gottwinmerged = m_Grains[grainname]->gottwinmerged;
    if (gottwinmerged == 1)
    {
      int twinnewnumber = m_Grains[grainname]->twinnewnumber;
      voxels[k].grainname = twinnewnumber;
    }
  }
}

void ReconstructionFunc::merge_colonies()
{
  int colonymerged = 1;
  float angcur = 180;
  vector<int > colonylist;
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float q1[5];
  float q2[5];
  size_t numgrains = m_Grains.size();
  AIM::Reconstruction::CrystalStructure phase1, phase2;

  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_Grains[i]->colonynewnumber != -1)
    {
      colonylist.push_back(i);
      int csize = int(colonylist.size());
      for (int m = 0; m < csize; m++)
      {
        csize = int(colonylist.size());
        int firstgrain = colonylist[m];
        IntVectorType nlist = m_Grains[firstgrain]->neighborlist;
        int size = int(nlist->size());
        for (int l = 0; l < size; l++)
        {
          angcur = 180;
          int colony = 0;
          size_t neigh = nlist->at(l);
          if (neigh != i && m_Grains[neigh]->colonynewnumber != -1)
          {
		    w = 10000.0;
            q1[1] = m_Grains[firstgrain]->avg_quat[1];
            q1[2] = m_Grains[firstgrain]->avg_quat[2];
            q1[3] = m_Grains[firstgrain]->avg_quat[3];
            q1[4] = m_Grains[firstgrain]->avg_quat[4];
			phase1 = crystruct[m_Grains[firstgrain]->phase];
            q2[1] = m_Grains[neigh]->avg_quat[1];
            q2[2] = m_Grains[neigh]->avg_quat[2];
            q2[3] = m_Grains[neigh]->avg_quat[3];
            q2[4] = m_Grains[neigh]->avg_quat[4];
			phase2 = crystruct[m_Grains[neigh]->phase];
			if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
			OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
			float vecttol = 0.03;
            if (fabs(r1) < vecttol && fabs(r2) < vecttol && fabs(fabs(r3) - 0.0919) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.289) < vecttol && fabs(fabs(r2) - 0.5) < vecttol && fabs(r3) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.57735) < vecttol && fabs(r2) < vecttol && fabs(r3) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.33) < vecttol && fabs(fabs(r2) - 0.473) < vecttol && fabs(fabs(r3) - 0.093) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.577) < vecttol && fabs(fabs(r2) - 0.053) < vecttol && fabs(fabs(r3) - 0.093) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.293) < vecttol && fabs(fabs(r2) - 0.508) < vecttol && fabs(fabs(r3) - 0.188) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.5866) < vecttol && fabs(r2) < vecttol && fabs(fabs(r3) - 0.188) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.5769) < vecttol && fabs(fabs(r2) - 0.8168) < vecttol && fabs(r3) < vecttol) colony = 1;
            if (fabs(fabs(r1) - 0.9958) < vecttol && fabs(fabs(r2) - 0.0912) < vecttol && fabs(r3) < vecttol) colony = 1;
            if (w < angcur)
            {
              angcur = w;
            }
            if (colony == 1)
            {
              m_Grains[neigh]->gotcolonymerged = colonymerged;
              m_Grains[neigh]->colonynewnumber = i;
              colonylist.push_back(neigh);
            }
          }
        }
      }
    }
    colonylist.clear();
  }
  for (int k = 0; k < (xpoints * ypoints * zpoints); k++)
  {
    int grainname = voxels[k].grainname;
    int gotcolonymerged = m_Grains[grainname]->gotcolonymerged;
    if (gotcolonymerged == 1)
    {
      int colonynewnumber = m_Grains[grainname]->colonynewnumber;
      voxels[k].grainname = colonynewnumber;
    }
  }
}

void ReconstructionFunc::characterize_twins()
{
  size_t numgrains = m_Grains.size();
  for (size_t i = 0; i < numgrains; i++)
  {

  }
}

void ReconstructionFunc::characterize_colonies()
{
  size_t numgrains = m_Grains.size();
  for (size_t i = 0; i < numgrains; i++)
  {

  }
}

void ReconstructionFunc::renumber_grains3()
{
  size_t numgrains = m_Grains.size();
  int graincount = 1;
  std::vector<int > newnames(numgrains);
  for (size_t i = 1; i < numgrains; i++)
  {
    int gottwinmerged = m_Grains[i]->gottwinmerged;
    if (gottwinmerged != 1)
    {
      newnames[i] = graincount;
      float ea1good = m_Grains[i]->euler1;
      float ea2good = m_Grains[i]->euler2;
      float ea3good = m_Grains[i]->euler3;
      int size = m_Grains[i]->numvoxels;
      int numneighbors = m_Grains[i]->numneighbors;
      IntVectorType nlist = m_Grains[i]->neighborlist;
      m_Grains[graincount]->numvoxels = size;
      m_Grains[graincount]->numneighbors = numneighbors;
      if (m_Grains[graincount]->neighborlist.get() == NULL)
      {
        m_Grains[graincount]->neighborlist = IntVectorType(new std::vector<int>(numneighbors));
      }
      if (NULL != nlist)
      {
        m_Grains[graincount]->neighborlist->swap(*nlist);
      }
      m_Grains[graincount]->euler1 = ea1good;
      m_Grains[graincount]->euler2 = ea2good;
      m_Grains[graincount]->euler3 = ea3good;
      graincount++;
    }
  }
#if 0
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalpoints ), ParallelRenumberGrains( this) );
#else
 for (int j = 0; j < totalpoints; j++)
  {
    int grainname = voxels[j].grainname;
    if (grainname >= 1)
    {
      int newgrainname = newnames[grainname];
      voxels[j].grainname = newgrainname;
    }
  }
#endif
}

void ReconstructionFunc::find_neighbors()
{
  int neighbors[6];
  neighbors[0] = -(xpoints * ypoints);
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = (xpoints * ypoints);
  float column, row, plane;
  int grain;
  size_t nnum;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;
  for(size_t i=0;i<crystruct.size();i++)
  {
	  totalsurfacearea[i] = 0;
  }
  int surfacegrain = 1;
  int nListSize = 100;
  // Copy all the grain names into a densely packed array
  int* gnames = new int[totalpoints];
  size_t numgrains = m_Grains.size();
  std::set<int> leftOutGrains;

  for (int i = 0; i < totalpoints; ++i)
  {
    gnames[i] = voxels[i].grainname;
    voxels[i].neighborlist = IntVectorType(new std::vector<int>(6, -1) );
  }

  for (size_t i = 0; i < numgrains; i++)
  {
    m_Grains[i]->numneighbors = 0;
    m_Grains[i]->neighborlist->assign(nListSize, -1);
    m_Grains[i]->neighborsurfarealist->assign(nListSize, -1.0);
    for(int j=0;j<3;j++)
    {
      m_Grains[i]->neighbordistfunc[j] = 0;
    }
  }
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
    onsurf = 0;
    grain = gnames[j];
    if (grain > 0)
    {
      column = j % xpoints;
      row = (j / xpoints) % ypoints;
      plane = j / (xpoints * ypoints);
      if ((column == 0 || column == (xpoints - 1) || row == 0 || row == (ypoints - 1) || plane == 0 || plane == (zpoints - 1)) && zpoints != 1) m_Grains[grain]->surfacegrain = surfacegrain;
      if ((column == 0 || column == (xpoints - 1) || row == 0 || row == (ypoints - 1)) && zpoints == 1) m_Grains[grain]->surfacegrain = surfacegrain;
      for (int k = 0; k < 6; k++)
      {
        good = 1;
        neighbor = j + neighbors[k];
        if (k == 0 && plane == 0) good = 0;
        if (k == 5 && plane == (zpoints - 1)) good = 0;
        if (k == 1 && row == 0) good = 0;
        if (k == 4 && row == (ypoints - 1)) good = 0;
        if (k == 2 && column == 0) good = 0;
        if (k == 3 && column == (xpoints - 1)) good = 0;
        if (good == 1 && gnames[neighbor] != grain && gnames[neighbor] > 0)
        {
          voxels[j].neighborlist->at(onsurf) = gnames[neighbor];
          nnum = m_Grains[grain]->numneighbors;
          if (nnum >= (m_Grains[grain]->neighborlist->size()))
          {
            m_Grains[grain]->neighborlist->resize(nnum + nListSize);
            m_Grains[grain]->neighborsurfarealist->resize(nnum + nListSize);
          }
          m_Grains[grain]->neighborlist->at(nnum) = gnames[neighbor];
          nnum++;
          m_Grains[grain]->numneighbors = nnum;
          onsurf++;
        }
      }
    }
    if (onsurf > 0)
    {
      vector<int >::iterator newend;
      sort(voxels[j].neighborlist->begin(), voxels[j].neighborlist->end());
      newend = unique(voxels[j].neighborlist->begin(), voxels[j].neighborlist->end());
      voxels[j].neighborlist->erase(newend, voxels[j].neighborlist->end());
      voxels[j].neighborlist->erase(std::remove(voxels[j].neighborlist->begin(), voxels[j].neighborlist->end(), -1), voxels[j].neighborlist->end());
      voxels[j].surfacevoxel = onsurf;
    }
    if (voxels[j].neighborlist->size() >= 3) voxels[j].nearestneighbordistance[0] = 0, voxels[j].nearestneighbordistance[1] = 0, voxels[j].nearestneighbordistance[2]
        = 0, voxels[j].nearestneighbor[0] = j, voxels[j].nearestneighbor[1] = j, voxels[j].nearestneighbor[2] = j;
    if (voxels[j].neighborlist->size() == 2) voxels[j].nearestneighbordistance[0] = 0, voxels[j].nearestneighbordistance[1] = 0, voxels[j].nearestneighbordistance[2]
        = -1, voxels[j].nearestneighbor[0] = j, voxels[j].nearestneighbor[1] = j, voxels[j].nearestneighbor[2] = -1;
    if (voxels[j].neighborlist->size() == 1) voxels[j].nearestneighbordistance[0] = 0, voxels[j].nearestneighbordistance[1] = -1, voxels[j].nearestneighbordistance[2]
        = -1, voxels[j].nearestneighbor[0] = j, voxels[j].nearestneighbor[1] = -1, voxels[j].nearestneighbor[2] = -1;
    if (onsurf == 0) voxels[j].nearestneighbordistance[0] = -1, voxels[j].nearestneighbordistance[1] = -1, voxels[j].nearestneighbordistance[2] = -1, voxels[j].nearestneighbor[0]
        = -1, voxels[j].nearestneighbor[1] = -1, voxels[j].nearestneighbor[2] = -1;
  }
  delete[] gnames;
  vector<int> nlistcopy;
  for (size_t i = 1; i < numgrains; i++)
  {
    vector<int >::iterator newend;
    sort(m_Grains[i]->neighborlist->begin(), m_Grains[i]->neighborlist->end());
    nlistcopy.assign(m_Grains[i]->neighborlist->begin(), m_Grains[i]->neighborlist->end() );
    newend = unique(m_Grains[i]->neighborlist->begin(), m_Grains[i]->neighborlist->end());
    m_Grains[i]->neighborlist->erase(newend, m_Grains[i]->neighborlist->end());
    m_Grains[i]->neighborlist->erase(std::remove(m_Grains[i]->neighborlist->begin(), m_Grains[i]->neighborlist->end(), -1), m_Grains[i]->neighborlist->end());
    //  nlist->erase(std::remove(nlist->begin(),nlist->end(),0),nlist->end());
    int numneighs = int(m_Grains[i]->neighborlist->size());
    for (int j = 0; j < numneighs; j++)
    {
      size_t neigh = m_Grains[i]->neighborlist->at(j);
      int number = std::count(nlistcopy.begin(), nlistcopy.end(), neigh);
      float area = number * resx * resx;
      m_Grains[i]->neighborsurfarealist->at(j) = area;
      if (m_Grains[i]->surfacegrain == 0 && (neigh > i || m_Grains[neigh]->surfacegrain == 1))
      {
        if(m_Grains[i]->phase == m_Grains[neigh]->phase) totalsurfacearea[m_Grains[i]->phase] = totalsurfacearea[m_Grains[i]->phase] + area;
      }
    }
    m_Grains[i]->numneighbors = numneighs;
  }

  merge_containedgrains();

}

void ReconstructionFunc::define_neighborhood()
{
  float x, y, z;
  float xn, yn, zn;
  float xdist, ydist, zdist;
  float dist, dist2, diam, diam2;
  int dist_int, dist2_int;
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
            dist = powf(dist, 0.5);
            dist2 = dist;
            dist_int = int(dist / (diam/2.0));
            dist2_int = int(dist2 / (diam2/2.0));
            if (dist_int < 3)
            {
				for(int iter=dist_int;iter<3;iter++)
				{
	              grain.neighbordistfunc[dist_int]++;
				}
            }
            if (dist2_int < 3)
            {
				for(int iter=dist2_int;iter<3;iter++)
				{
	              grain_j.neighbordistfunc[dist2_int]++;
				}
            }
          }
        }
      }
  }
}

void ReconstructionFunc::find_centroids()
{
  //  int count = 0;
  int onedge = 0;
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
    onedge = 0;
    int gnum = voxels[j].grainname;
    graincenters[gnum][0]++;
    col = j % xpoints;
    row = (j / xpoints) % ypoints;
    plane = j / (xpoints * ypoints);
	x = float(col)*resx;
	y = float(row)*resy;
	z = float(plane)*resz;
    if (col <= 0) onedge = 1;
    if (col >= xpoints - 1) onedge = 1;
    if (row <= 0) onedge = 1;
    if (row >= ypoints - 1) onedge = 1;
    if (plane <= 0) onedge = 1;
    if (plane >= zpoints - 1) onedge = 1;
    graincenters[gnum][1] = graincenters[gnum][1] + x;
    graincenters[gnum][2] = graincenters[gnum][2] + y;
    graincenters[gnum][3] = graincenters[gnum][3] + z;
    if (onedge == 1) graincenters[gnum][4] = 1;
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
    m_Grains[i]->surfacegrain = graincenters[i][4];
    radcubed = m_Grains[i]->volume/vol_term;
    diameter = 2.0*powf(radcubed, 0.3333333333);
    m_Grains[i]->equivdiameter = diameter;
    if(int(diameter) > maxdiameter[m_Grains[i]->phase]) maxdiameter[m_Grains[i]->phase] = int(diameter);
    if(int(diameter) < mindiameter[m_Grains[i]->phase]) mindiameter[m_Grains[i]->phase] = int(diameter);
	if(m_Grains[i]->surfacegrain == 0)
	{
		totalvol[m_Grains[i]->phase] = totalvol[m_Grains[i]->phase] + m_Grains[i]->volume;
		insidegraincount++;
	}
  }
  unbiasedvol = 0;
  for(size_t i=1;i<crystruct.size();i++)
  {
    unbiasedvol = unbiasedvol + totalvol[i];
  }
  for(size_t i=1;i<crystruct.size();i++)
  {
    phasefraction[i] = totalvol[i]/unbiasedvol;
  }
}
void ReconstructionFunc::find_centroids2D()
{
  //  int count = 0;
  int onedge = 0;
  int insidegraincount = 0;
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
    onedge = 0;
    int gnum = voxels[j].grainname;
    graincenters[gnum][0]++;
    col = j % xpoints;
    row = (j / xpoints) % ypoints;
	x = float(col)*resx;
	y = float(row)*resy;
    if (col <= 0) onedge = 1;
    if (col >= xpoints - 1) onedge = 1;
    if (row <= 0) onedge = 1;
    if (row >= ypoints - 1) onedge = 1;
    graincenters[gnum][1] = graincenters[gnum][1] + x;
    graincenters[gnum][2] = graincenters[gnum][2] + y;
    if (onedge == 1) graincenters[gnum][3] = 1;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    graincenters[i][1] = graincenters[i][1] / graincenters[i][0];
    graincenters[i][2] = graincenters[i][2] / graincenters[i][0];
    m_Grains[i]->centroidx = graincenters[i][1];
    m_Grains[i]->centroidy = graincenters[i][2];
    m_Grains[i]->numvoxels = graincenters[i][0];
    m_Grains[i]->volume = (graincenters[i][0] * resx * resy);
    m_Grains[i]->surfacegrain = graincenters[i][3];
    radsquared = m_Grains[i]->volume / m_pi;
    diameter = (2 * powf(radsquared, 0.5));
    m_Grains[i]->equivdiameter = diameter;
    if (int(diameter) > maxdiameter[m_Grains[i]->phase]) maxdiameter[m_Grains[i]->phase] = int(diameter);
    if (int(diameter) < mindiameter[m_Grains[i]->phase]) mindiameter[m_Grains[i]->phase] = int(diameter);
	if(m_Grains[i]->surfacegrain == 0)
	{
		totalvol[m_Grains[i]->phase] = totalvol[m_Grains[i]->phase] + m_Grains[i]->volume;
		insidegraincount++;
	}
  }
  unbiasedvol = 0;
  for(size_t i=1;i<crystruct.size();i++)
  {
	unbiasedvol = unbiasedvol + totalvol[i];
  }
  for(size_t i=1;i<crystruct.size();i++)
  {
	phasefraction[i] = totalvol[i]/unbiasedvol;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionFunc::find_euclidean_map()
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
float ReconstructionFunc::find_xcoord(size_t index)
{
  float x = resx * float(index % xpoints);
  return x;
}
float ReconstructionFunc::find_ycoord(size_t index)
{
  float y = resy * float((index / xpoints) % ypoints);
  return y;
}
float ReconstructionFunc::find_zcoord(size_t index)
{
  float z = resz * float(index / (xpoints * ypoints));
  return z;
}

void ReconstructionFunc::find_moments()
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
    int gnum = voxels[j].grainname;
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
void ReconstructionFunc::find_axes()
{

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
    float r = powf(rsquare, 0.5);
    float theta = 0;
    if (r == 0)
    {
      theta = 0;
    }
    if (r != 0)
    {
      theta = acos(-g / (2 * r));
    }
    float r1 = 2 * powf(r, 0.33333333333) * cos(theta / 3) - (b / (3 * a));
    float r2 = -powf(r, 0.33333333333) * (cos(theta / 3) - (1.7320508 * sin(theta / 3))) - (b / (3 * a));
    float r3 = -powf(r, 0.33333333333) * (cos(theta / 3) + (1.7320508 * sin(theta / 3))) - (b / (3 * a));
    m_Grains[i]->radius1 = r1;
    m_Grains[i]->radius2 = r2;
    m_Grains[i]->radius3 = r3;
  }
}
void ReconstructionFunc::find_vectors(H5ReconStatsWriter::Pointer h5io)
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
    float norm1 = powf(((n1x * n1x) + (n1y * n1y) + (n1z * n1z)), 0.5);
    float norm2 = powf(((n2x * n2x) + (n2y * n2y) + (n2z * n2z)), 0.5);
    float norm3 = powf(((n3x * n3x) + (n3y * n3y) + (n3z * n3z)), 0.5);
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
        float cosine3 = (m1[1][2] / sin(ea2));
        float sine3 = (m1[0][2] / sin(ea2));
        float cosine1 = (-m1[2][1] / sin(ea2));
        float sine1 = (m1[2][0] / sin(ea2));
        float ea3 = acos(cosine3);
        float ea1 = acos(cosine1);
        if (sine3 < 0) ea3 = (2 * m_pi) - ea3;
        if (sine1 < 0) ea1 = (2 * m_pi) - ea1;
        int ea1bin = int(ea1 / (m_pi / 18));
        int ea2bin = int(ea2 / (m_pi / 18));
        int ea3bin = int(ea3 / (m_pi / 18));
        int bin = 0;
        if (ea1 >= 0.0 && ea2 >= 0.0 && ea3 >= 0.0 && ea1 <= (m_pi) && ea2 <= (m_pi) && ea3 <= (m_pi))
        {
          m_Grains[i]->axiseuler1 = ea1;
          m_Grains[i]->axiseuler2 = ea2;
          m_Grains[i]->axiseuler3 = ea3;
          bin = (ea3bin * 18 * 18) + (ea2bin * 18) + (ea1bin);
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
void ReconstructionFunc::find_moments2D()
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
    int gnum = voxels[j].grainname;
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
void ReconstructionFunc::find_axes2D()
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
void ReconstructionFunc::find_vectors2D(H5ReconStatsWriter::Pointer h5io)
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
    float norm1 = powf((n1x * n1x + n1y * n1y), 0.5);
    float norm2 = powf((n2x * n2x + n2y * n2y), 0.5);
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
void ReconstructionFunc::find_eulerodf(H5ReconStatsWriter::Pointer h5io)
{
  size_t bin;
  size_t numgrains = m_Grains.size();
  AIM::Reconstruction::CrystalStructure phase;
  float **eulerodf;

  eulerodf = new float *[crystruct.size()];
  for(size_t i=1;i<crystruct.size();i++)
  {
	  if (crystruct[i] == AIM::Reconstruction::Hexagonal)
	  {
	    eulerodf[i] = new float[36 * 36 * 12];
	    for (int j = 0; j < 36 * 36 * 12; j++)
	    {
	      eulerodf[i][j] = 0.0;
	    }
	  }
	  else if (crystruct[i] == AIM::Reconstruction::Cubic)
	  {
		  eulerodf[i] = new float[18 * 18 * 18];
		  for (int j = 0; j < 18 * 18 * 18; j++)
		  {
			eulerodf[i][j] = 0.0;
		  }
	  }
  }
  float r1, r2, r3;
  float ea1, ea2, ea3;
  for (size_t i = 1; i < numgrains; i++)
  {
    if (m_Grains[i]->surfacegrain == 0 && m_Grains[i]->active == 1)
    {
      float vol = m_Grains[i]->volume;
	  ea1 = m_Grains[i]->euler1;
	  ea2 = m_Grains[i]->euler2;
	  ea3 = m_Grains[i]->euler3;
      phase = crystruct[m_Grains[i]->phase];
	  OrientationMath::eulertoRod(r1,r2,r3,ea1,ea2,ea3);
      bin = m_OrientationOps[phase]->getOdfBin(r1,r2,r3);
      eulerodf[m_Grains[i]->phase][bin] = eulerodf[m_Grains[i]->phase][bin] + (vol/totalvol[m_Grains[i]->phase]);
    }
  }
  int err;
  for(size_t i=1;i<crystruct.size();i++)
  {
	  err = h5io->writeODFData(i, crystruct[i], eulerodf[i]);
	  delete[] eulerodf[i];
  }
  delete[] eulerodf;
}


void ReconstructionFunc::measure_misorientations(H5ReconStatsWriter::Pointer h5io)
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

  misobin = new float *[crystruct.size()];
  for(size_t i=1;i<crystruct.size();i++)
  {
	  if (crystruct[i] == AIM::Reconstruction::Hexagonal)
	  {
	    misobin[i] = new float[36 * 36 * 12];
	    for (int j = 0; j < 36 * 36 * 12; j++)
	    {
	      misobin[i][j] = 0.0;
	    }
	  }
	  if (crystruct[i] == AIM::Reconstruction::Cubic)
	  {
		misobin[i] = new float[18 * 18 * 18];
		for (int j = 0; j < 18 * 18 * 18; j++)
		{
		  misobin[i][j] = 0.0;
		}
	  }
  }
  float microbin[10];
  for(size_t e = 0; e < 10; ++e)
  {
    microbin[e] = 0.0;
  }
  size_t nname;
  float microcount = 0.0;
  float nsa;

  for (size_t i = 1; i < numgrains; i++)
  {
    microcount = 0;
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
      if (w < 0.261799) microcount++;
      if ((nname > i || m_Grains[nname]->surfacegrain == 1) && phase1 == phase2)
      {
        nsa = m_Grains[i]->neighborsurfarealist->at(j);
        misobin[m_Grains[i]->phase][mbin] = misobin[m_Grains[i]->phase][mbin] + (nsa / totalsurfacearea[m_Grains[i]->phase]);
      }
    }
    if (m_Grains[i]->neighborlist->size() > 0)
    {
      int micbin = int((float(microcount) / float(m_Grains[i]->neighborlist->size())) / 0.1);
      microbin[micbin]++;
    }
  }
  for(size_t i=1;i<crystruct.size();i++)
  {
	  h5io->writeMisorientationBinsData(i, crystruct[i], misobin[i]);
	  h5io->writeMicroTextureData(i, microbin, 10, numgrains);
	  delete[] misobin[i];
  }
  delete[] misobin;
}

void ReconstructionFunc::find_colors()
{
  size_t numgrains = m_Grains.size();
  //  float red, green, blue;
  unsigned char rgb[3] =
  { 0, 0, 0 };
  unsigned char hkl[3] =
  { 0, 0, 0 };
  float RefDirection[3] =
  { 0.0, 1.0, 0.0 };
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
void ReconstructionFunc::find_schmids()
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
      mag = powf(mag, 0.5);
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

int ReconstructionFunc::volume_stats(H5ReconStatsWriter::Pointer h5io)
{
  int retErr = 0;
  float actualgrains = 0;
  float avglogdiam = 0;
//  float neighcount = 0;
  size_t numgrains = m_Grains.size();
  vector<vector<float> > neighborhood;
  vector<vector<float> > neighborhoodfit;
  vector<vector<float> > svbovera;
  vector<vector<float> > svcovera;
  vector<vector<float> > svcoverb;
  vector<vector<float> > svschmid;
  vector<vector<float> > svomega3;
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
		  I1 = (15 * I1) / (4 * m_pi);
		  I2 = (15 * I2) / (4 * m_pi);
		  I3 = (15 * I3) / (4 * m_pi);
		  float A = (I1 + I2 - I3) / 2;
		  float B = (I1 + I3 - I2) / 2;
		  float C = (I2 + I3 - I1) / 2;
		  float a = (A * A * A * A) / (B * C);
		  a = powf(a, 0.1);
		  float b = B / A;
		  b = powf(b, 0.5) * a;
		  float c = A / (a * a * a * b);
		  float bovera = b / a;
		  float covera = c / a;
		  float coverb = c / b;
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
		  I1 = (15 * I1) / (4 * m_pi);
		  I2 = (15 * I2) / (4 * m_pi);
		  I3 = (15 * I3) / (4 * m_pi);
		  float A = (I1 + I2 - I3) / 2;
		  float B = (I1 + I3 - I2) / 2;
		  float C = (I2 + I3 - I1) / 2;
		  float a = (A * A * A * A) / (B * C);
		  a = powf(a, 0.1);
		  float b = B / A;
		  b = powf(b, 0.5) * a;
		  float c = A / (a * a * a * b);
		  float bovera = b / a;
		  float covera = c / a;
		  float coverb = c / b;
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
		  neighborhood[temp4][2] = powf(neighborhood[temp4][2], 0.5);
		  neighborhood[temp4][4] = powf(neighborhood[temp4][4], 0.5);
		  neighborhood[temp4][6] = powf(neighborhood[temp4][6], 0.5);
		  svbovera[temp4][2] = powf(svbovera[temp4][2], 0.5);
		  svcovera[temp4][2] = powf(svcovera[temp4][2], 0.5);
		  svcoverb[temp4][2] = powf(svcoverb[temp4][2], 0.5);
		  svschmid[temp4][2] = powf(svschmid[temp4][2], 0.5);
		  svomega3[temp4][2] = powf(svomega3[temp4][2], 0.5);
		}
	  }
	  sdlogdiam = sdlogdiam / actualgrains;
	  sdlogdiam = powf(sdlogdiam, 0.5);
	  retErr = h5io->writeVolumeStats(iter, crystruct[iter], phasefraction[iter], maxdiameter[iter], mindiameter[iter], 1.0, avglogdiam, sdlogdiam, svbovera, svcovera, svcoverb, neighborhoodfit, svomega3);
  }

  return retErr;
}

int ReconstructionFunc::volume_stats2D(H5ReconStatsWriter::Pointer h5io)
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
        neighborhood[temp4][2] = powf(neighborhood[temp4][2], 0.5);
        neighborhood[temp4][4] = powf(neighborhood[temp4][4], 0.5);
        neighborhood[temp4][6] = powf(neighborhood[temp4][6], 0.5);
        svbovera[temp4][2] = powf(svbovera[temp4][2], 0.5);
        svschmid[temp4][2] = powf(svschmid[temp4][2], 0.5);
      }
    }
    sdlogdiam = sdlogdiam / actualgrains;
    sdlogdiam = powf(sdlogdiam, 0.5);

    retErr
        = h5io->writeVolumeStats2D(iter, crystruct[iter], phasefraction[iter], maxdiameter[iter], mindiameter[iter], 1.0, avglogdiam, sdlogdiam, svbovera, neighborhoodfit);
  }
  return retErr;
}

void ReconstructionFunc::deformation_stats(const std::string &filename, const std::string &filename2)
{
  ofstream outFile;
  outFile.open(filename.c_str());
  float w, n1, n2, n3;
  int distance;
  float km, iq, gbdist, tjdist, qpdist, sf, sf2, sfmm, gam, lmg, ssap;
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
  float kmviq[10][2];
  float gamviq[10][2];
  float lmgviq[10][2];
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
  int sfbin, ssapbin, sfmmbin, disbin, iqbin;
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
			  kmviq[i][j] = 0;
			  gamviq[i][j] = 0;
			  lmgviq[i][j] = 0;
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
    if (voxels[i].unassigned == 0)
    {
      km = voxels[i].kernelmisorientation;
      gam = voxels[i].grainmisorientation;
      lmg = voxels[i].misorientationgradient;
      iq = voxels[i].imagequality;
      gbdist = voxels[i].nearestneighbordistance[0];
      tjdist = voxels[i].nearestneighbordistance[1];
      qpdist = voxels[i].nearestneighbordistance[2];
      gname = voxels[i].grainname;
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
      if (crystruct[m_Grains[gname]->phase] == crystruct[m_Grains[gname2]->phase]) {
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
	  iqbin = int((iq) / 250.0);
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
      if (iqbin < 0) iqbin = 0;
      if (iqbin > 9) iqbin = 9;
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
      kmviq[iqbin][0]++;
      kmviq[iqbin][1] = kmviq[iqbin][1] + km;
      gamviq[iqbin][0]++;
      gamviq[iqbin][1] = gamviq[iqbin][1] + gam;
      lmgviq[iqbin][0]++;
      lmgviq[iqbin][1] = lmgviq[iqbin][1] + lmg;
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
  }
  outFile << "Kernel Misorientation Data" << endl;
  outFile << "GB		TJ		QP		IQ		SF		SFMM		SSAP		DIS" << endl;
  for (int i = 0; i < 10; i++)
  {
    if (kmvgb[i][0] > 0) kmvgb[i][1] = kmvgb[i][1] / kmvgb[i][0];
    if (kmvtj[i][0] > 0) kmvtj[i][1] = kmvtj[i][1] / kmvtj[i][0];
    if (kmvqp[i][0] > 0) kmvqp[i][1] = kmvqp[i][1] / kmvqp[i][0];
    if (kmviq[i][0] > 0) kmviq[i][1] = kmviq[i][1] / kmviq[i][0];
    if (kmvsf[i][0] > 0) kmvsf[i][1] = kmvsf[i][1] / kmvsf[i][0];
    if (kmvsfmm[i][0] > 0) kmvsfmm[i][1] = kmvsfmm[i][1] / kmvsfmm[i][0];
    if (kmvssap[i][0] > 0) kmvssap[i][1] = kmvssap[i][1] / kmvssap[i][0];
    if (kmvdis[i][0] > 0) kmvdis[i][1] = kmvdis[i][1] / kmvdis[i][0];
    outFile << kmvgb[i][0] << "	" << kmvgb[i][1] << "	"
		<< kmvtj[i][0] << "	" << kmvtj[i][1] << "	"
		<< kmvqp[i][0] << "	" << kmvqp[i][1] << "	"
		<< kmviq[i][0] << "	" << kmviq[i][1] << "	"
		<< kmvsf[i][0] << "	" << kmvsf[i][1] << "	"
		<< kmvsfmm[i][0] << "	" << kmvsfmm[i][1] << "	"
		<< kmvssap[i][0] << "	" << kmvssap[i][1] << "	"
		<< kmvdis[i][0] << "	" << kmvdis[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Grain Average Misorientation Data" << endl;
  outFile << "GB		TJ		QP		IQ		SF		SFMM		SSAP		DIS" << endl;
  for (int i = 0; i < 10; i++)
  {
    if (gamvgb[i][0] > 0) gamvgb[i][1] = gamvgb[i][1] / gamvgb[i][0];
    if (gamvtj[i][0] > 0) gamvtj[i][1] = gamvtj[i][1] / gamvtj[i][0];
    if (gamvqp[i][0] > 0) gamvqp[i][1] = gamvqp[i][1] / gamvqp[i][0];
    if (gamviq[i][0] > 0) gamviq[i][1] = gamviq[i][1] / gamviq[i][0];
    if (gamvsf[i][0] > 0) gamvsf[i][1] = gamvsf[i][1] / gamvsf[i][0];
    if (gamvsfmm[i][0] > 0) gamvsfmm[i][1] = gamvsfmm[i][1] / gamvsfmm[i][0];
    if (gamvssap[i][0] > 0) gamvssap[i][1] = gamvssap[i][1] / gamvssap[i][0];
    if (gamvdis[i][0] > 0) gamvdis[i][1] = gamvdis[i][1] / gamvdis[i][0];
    outFile << gamvgb[i][0] << "	" << gamvgb[i][1] << "	"
		<< gamvtj[i][0] << "	" << gamvtj[i][1] << "	"
		<< gamvqp[i][0] << "	" << gamvqp[i][1] << "	"
		<< gamviq[i][0] << "	" << gamviq[i][1] << "	"
		<< gamvsf[i][0] << "	" << gamvsf[i][1] << "	"
		<< gamvsfmm[i][0] << "	" << gamvsfmm[i][1] << "	"
		<< gamvssap[i][0] << "	" << gamvssap[i][1] << "	"
		<< gamvdis[i][0] << "	" << gamvdis[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Misorientation Gradient Data" << endl;
  outFile << "GB		TJ		QP		IQ		SF		SFMM		SSAP		DIS" << endl;
  for (int i = 0; i < 10; i++)
  {
    if (lmgvgb[i][0] > 0) lmgvgb[i][1] = lmgvgb[i][1] / lmgvgb[i][0];
    if (lmgvtj[i][0] > 0) lmgvtj[i][1] = lmgvtj[i][1] / lmgvtj[i][0];
    if (lmgvqp[i][0] > 0) lmgvqp[i][1] = lmgvqp[i][1] / lmgvqp[i][0];
    if (lmgviq[i][0] > 0) lmgviq[i][1] = lmgviq[i][1] / lmgviq[i][0];
    if (lmgvsf[i][0] > 0) lmgvsf[i][1] = lmgvsf[i][1] / lmgvsf[i][0];
    if (lmgvsfmm[i][0] > 0) lmgvsfmm[i][1] = lmgvsfmm[i][1] / lmgvsfmm[i][0];
    if (lmgvssap[i][0] > 0) lmgvssap[i][1] = lmgvssap[i][1] / lmgvssap[i][0];
    if (lmgvdis[i][0] > 0) lmgvdis[i][1] = lmgvdis[i][1] / lmgvdis[i][0];
    outFile << lmgvgb[i][0] << "	" << lmgvgb[i][1] << "	"
		<< lmgvtj[i][0] << "	" << lmgvtj[i][1] << "	"
		<< lmgvqp[i][0] << "	" << lmgvqp[i][1] << "	"
		<< lmgviq[i][0] << "	" << lmgviq[i][1] << "	"
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



void ReconstructionFunc::write_graindata(const std::string &graindataFile)
{
  IntVectorType nlist;
  ofstream outFile;
  size_t numgrains = m_Grains.size();
  outFile.open(graindataFile.c_str());
  outFile << numgrains << endl;
  outFile << "Grain ID  Euler1  Euler2  Euler3  Equiv. Diameter Grain Avg. Disorientation Surface Grain Schmid Factor No. Neighbors Omega3" << endl;
  for (size_t i = 1; i < numgrains; i++)
  {
    float diameter = m_Grains[i]->equivdiameter;
    int onsurface = m_Grains[i]->surfacegrain;
    float avgmiso = m_Grains[i]->averagemisorientation;
    float schmid = m_Grains[i]->schmidfactor;
    nlist = m_Grains[i]->neighborlist;
    int nucleus = m_Grains[i]->nucleus;
	float omega3 = m_Grains[i]->omega3;
    outFile << i << " " << voxels[nucleus].euler1 << "  " << voxels[nucleus].euler2 << "  " << voxels[nucleus].euler3 << "  " << diameter << "  " << avgmiso << " "
        << onsurface << " " << schmid << "  " << nlist->size() << " " << omega3 << endl;
  }
  outFile.close();

}


float ReconstructionFunc::gamma(float x)
{
  int i, k, m;
  float ga, gr, r, z;

  static float g[] =
  {
  1.0,
  0.5772156649015329,
  -0.6558780715202538,
  -0.420026350340952e-1,
  0.1665386113822915,
  -0.421977345555443e-1,
  -0.9621971527877e-2,
  0.7218943246663e-2,
  -0.11651675918591e-2,
  -0.2152416741149e-3,
  0.1280502823882e-3,
  -0.201348547807e-4,
  -0.12504934821e-5,
  0.1133027232e-5,
  -0.2056338417e-6,
  0.6116095e-8,
  0.50020075e-8,
  -0.11812746e-8,
  0.1043427e-9,
  0.77823e-11,
  -0.36968e-11,
  0.51e-12,
  -0.206e-13,
  -0.54e-14,
  0.14e-14 };

  if (x > 171.0) return 1e308; // This value is an overflow flag.
  if (x == (int)x)
  {
    if (x > 0.0)
    {
      ga = 1.0; // use factorial
      for (i = 2; i < x; i++)
      {
        ga *= i;
      }
    }
    else ga = 1e308;
  }
  else
  {
    if (fabs(x) > 1.0)
    {
      z = fabs(x);
      m = (int)z;
      r = 1.0;
      for (k = 1; k <= m; k++)
      {
        r *= (z - k);
      }
      z -= m;
    }
    else z = x;
    gr = g[24];
    for (k = 23; k >= 0; k--)
    {
      gr = gr * z + g[k];
    }
    ga = 1.0 / (gr * z);
    if (fabs(x) > 1.0)
    {
      ga *= r;
      if (x < 0.0)
      {
        ga = -1 * m_pi / (x * ga * sin(m_pi * x));
      }
    }
  }
  return ga;
}
