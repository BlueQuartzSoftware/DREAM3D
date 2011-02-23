///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include "ReconstructionFunc.h"

// C Includes
#include <stdio.h>

// C++ Includes
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <set>

//-- MXA Includes
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"

//-- AIM Includes
#include "AIM/Common/AIMMath.h"
#include "AIM/Common/Constants.h"
#include "AIM/Common/OIMColoring.hpp"
#include "AIM/Common/Quaternions.h"
#include "AIM/Common/MisorientationCalculations.h"
#include "AIM/Common/HDF5/AIM_H5VtkDataWriter.h"
#include "AIM/ANG/AngReader.h"

#if AIM_USE_PARALLEL_ALGORITHMS
#include "AIM/Common/Parallel/Algo.hpp"
#endif

const static double m_pi = M_PI;
const static double m_OnePointThree = 1.33333333333;
const static double threesixty_over_pi = 360.0 / m_pi;
const static double m_pi_over_180 = M_PI / 180.0;
const static double sqrt_two = pow(2.0, 0.5);
const static double acos_neg_one = acos(-1.0);
const static double acos_pos_one = acos(1.0);
const static double sin_wmin_neg_1_over_2 = sin(acos_neg_one / 2.0);
const static double sin_wmin_pos_1_over_2 = sin(acos_pos_one / 2.0);

#define DIMS "DIMENSIONS"
#define LOOKUP "LOOKUP_TABLE"

#if 0
// -i C:\Users\GroebeMA\Desktop\NewFolder --outputDir C:\Users\GroebeMA\Desktop\NewFolder -f Slice_ --angMaxSlice 400 -s 1 -e 30 -z 0.25 -t -g 10 -c 0.1 -o 5.0 -x 2
#endif

using namespace std;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionFunc::ReconstructionFunc() :
  voxels(NULL), m_Grains(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionFunc::~ReconstructionFunc()
{
  delete[] voxels;
  m_Grains.clear();
  graincenters.clear();
  grainmoments.clear();
  m_grainQuats = DoubleArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionFunc::initialize(int nX,
                                    int nY,
                                    int nZ,
                                    double xRes,
                                    double yRes,
                                    double zRes,
                                    bool v_mergetwinsoption,
                                    bool v_mergecoloniesoption,
                                    int v_minallowedgrainsize,
                                    double v_minseedconfidence,
                                    double v_downsamplefactor,
                                    double v_minseedimagequality,
                                    double v_misorientationtolerance,
                                    double v_sizebinstepsize,
                                    AIM::Reconstruction::CrystalStructure v_crystruct,
                                    int v_alignmeth,
                                    bool v_alreadyformed)
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
//  m_Grains.resize(101);
  size_t oldSize = 0;
  m_Grains.resize(numgrains + 1);
  for(size_t g = oldSize; g < m_Grains.size(); ++g)
  {
    m_Grains[g] = Grain::New();
  }

  voxels = new Voxel[totalpoints];

  m_grainQuats = DoubleArrayType::NullPointer();
}

void ReconstructionFunc::cleanup_data()
{
  double bestneighborconfidence;
  int bestneighbor;
  double confidence;
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
  double w, n1, n2, n3;
  double q1[5];
  double q2[5];
  //  int seed = 0;
  size_t size = 0;
  int good = 0;
  int count = 0, goodcount = 0;
  int currentpoint = 0;
  int neighbor = 0;
  //  int keepgoing = 1;
  int col, row, plane;
  int initialVoxelsListSize = 10000;
  std::vector<int > voxelslist(initialVoxelsListSize, -1);
  if (alignmeth == 3)
  {
    for (int iter = 0; iter < (xpoints * ypoints * zpoints); iter++)
    {
      voxels[iter].grainname = 0;
    }
  }
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
    if (voxels[j].imagequality > minseedimagequality)
    {
      count = 0;
      goodcount = 0;
      col = j % xpoints;
      row = (j / xpoints) % ypoints;
      plane = j / (xpoints * ypoints);
      for (int i = 0; i < 6; i++)
      {
        good = 1;
        neighbor = j + neighbors[i];
        if (i == 0 && plane == 0) good = 0;
        if (i == 5 && plane == (zpoints - 1)) good = 0;
        if (i == 1 && row == 0) good = 0;
        if (i == 4 && row == (ypoints - 1)) good = 0;
        if (i == 2 && col == 0) good = 0;
        if (i == 3 && col == (xpoints - 1)) good = 0;
        if (good == 1)
        {
          count++;
          if (voxels[neighbor].imagequality > minseedimagequality) goodcount++;
        }
      }
    }
    if (goodcount == count && goodcount > 0)
    {
      voxels[j].grainname = -1;
      voxelslist[size] = j;
      size++;
      if (size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
    }
  }
  for (size_t j = 0; j < size; j++)
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
      if (good == 1 && voxels[neighbor].grainname == 0)
      {
        q2[0] = 0;
        q2[1] = voxels[neighbor].quat[1];
        q2[2] = voxels[neighbor].quat[2];
        q2[3] = voxels[neighbor].quat[3];
        q2[4] = voxels[neighbor].quat[4];

        if (crystruct == AIM::Reconstruction::Hexagonal)
        {
          w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
        }
        if (crystruct == AIM::Reconstruction::Cubic)
        {
          w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
        }
        if (w < misorientationtolerance)
        {
          voxels[neighbor].grainname = -1;
        }
      }
    }
  }
  voxelslist.clear();
}

void ReconstructionFunc::align_sections(const std::string &filename)
{

  ofstream outFile;
  outFile.open(filename.c_str());
  double disorientation = 0;
  double mindisorientation = 100000000;
  double **mutualinfo12;
  double *mutualinfo1;
  double *mutualinfo2;
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
  double w;
  double n1, n2, n3;
  double q1[5];
  double q2[5];
  double refci, curci, refiq, curiq;
  double refxcentroid, refycentroid;
  double curxcentroid, curycentroid;
  int refgnum, curgnum;
  int refposition = 0;
  int curposition = 0;
  //  int curposition1=0;
  //  int curposition2=0;
  //  int alternatetempposition;
  //  int halfshift=0;
  int position;
  int tempposition;

  shifts = new int *[zpoints];
  for (int a = 0; a < zpoints; a++)
  {
    shifts[a] = new int[2];
    for (int b = 0; b < 2; b++)
    {
      shifts[a][b] = 0;
    }
  }
  if (alignmeth == 0)
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
        refxcentroid = refxcentroid + (((resx * m) + (resx / 2.0)) * (double(voxels[refposition].grainname)));
        refycentroid = refycentroid + (((resy * l) + (resy / 2.0)) * (double(voxels[refposition].grainname)));
        count = count + voxels[refposition].grainname;
      }
    }
    refxcentroid = refxcentroid / double(count);
    refycentroid = refycentroid / double(count);
  }
  for (int iter = 1; iter < zpoints; iter++)
  {
    outFile << endl;
    outFile << endl;
    mindisorientation = 100000000;
    slice = (zpoints - 1) - iter;
    if (alignmeth == 2)
    {
      graincount1 = graincounts[slice];
      graincount2 = graincounts[slice + 1];
      mutualinfo12 = new double *[graincount1];
      mutualinfo1 = new double[graincount1];
      mutualinfo2 = new double[graincount2];
      for (int a = 0; a < graincount1; a++)
      {
        mutualinfo1[a] = 0;
        mutualinfo12[a] = new double[graincount2];
        for (int b = 0; b < graincount2; b++)
        {
          mutualinfo12[a][b] = 0;
          mutualinfo2[b] = 0;
        }
      }
    }
    tempxshift = 0;
    tempyshift = 0;
    if (alignmeth == 0)
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
          curxcentroid = curxcentroid + (((resx * m) + (resx / 2.0)) * (double(voxels[curposition].grainname)));
          curycentroid = curycentroid + (((resy * l) + (resy / 2.0)) * (double(voxels[curposition].grainname)));
          count = count + voxels[curposition].grainname;
        }
      }
      curxcentroid = curxcentroid / double(count);
      curycentroid = curycentroid / double(count);
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
                  if (alignmeth == 2)
                  {
                    if (curgnum >= 0 && refgnum >= 0)
                    {
                      mutualinfo12[curgnum][refgnum]++;
                      mutualinfo1[curgnum]++;
                      mutualinfo2[refgnum]++;
                    }
                  }
                  if (alignmeth == 1)
                  {
                    if (refiq > minseedimagequality && curiq > minseedimagequality)
                    {
                      q1[1] = voxels[refposition].quat[1];
                      q1[2] = voxels[refposition].quat[2];
                      q1[3] = voxels[refposition].quat[3];
                      q1[4] = voxels[refposition].quat[4];
                      q2[1] = voxels[curposition].quat[1];
                      q2[2] = voxels[curposition].quat[2];
                      q2[3] = voxels[curposition].quat[3];
                      q2[4] = voxels[curposition].quat[4];
                      if (crystruct == AIM::Reconstruction::Hexagonal)
                      {
                        w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
                      }
                      if (crystruct == AIM::Reconstruction::Cubic)
                      {
                        w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
                      }
                      if (w > misorientationtolerance) disorientation++;
                    }
                    if (refiq < minseedimagequality && curiq > minseedimagequality) disorientation++;
                    if (refiq > minseedimagequality && curiq < minseedimagequality) disorientation++;
                  }
                }
                else
                {
                  if (alignmeth == 2)
                  {
                    mutualinfo12[0][0]++;
                    mutualinfo1[0]++;
                    mutualinfo2[0]++;
                  }
                  if (alignmeth == 1) disorientation = disorientation;
                }
              }
            }
            if (alignmeth == 2)
            {
              double ha = 0;
              double hb = 0;
              double hab = 0;
              for (int b = 0; b < graincount1; b++)
              {
                mutualinfo1[b] = mutualinfo1[b] / double(count);
                if (mutualinfo1[b] != 0) ha = ha + mutualinfo1[b] * log(mutualinfo1[b]);
              }
              for (int c = 0; c < graincount2; c++)
              {
                mutualinfo2[c] = mutualinfo2[c] / double(count);
                if (mutualinfo2[c] != 0) hb = hb + mutualinfo2[c] * log(mutualinfo2[c]);
              }
              for (int b = 0; b < graincount1; b++)
              {
                for (int c = 0; c < graincount2; c++)
                {
                  mutualinfo12[b][c] = mutualinfo12[b][c] / double(count);
                  if (mutualinfo12[b][c] != 0) hab = hab + mutualinfo12[b][c] * log(mutualinfo12[b][c]);
                  double value = 0;
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
              //    double disorientation2 = ha + hb - hab;
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
    if (alignmeth == 0)
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
  outFile.close();
}
void ReconstructionFunc::form_grains_sections()
{
  int point = 0;
  int noseeds = 0;
  int graincount = 1;
  graincounts = new int[zpoints];
  int neighbor;
  // int currentpoint;
  double q1[5];
  double q2[5];
  double qs[5];
  double w, w2;
  double n1;
  double n2;
  double n3;
  int seed = -1;
  int randx = 0;
  int randy = 0;
  int good = 0;
  int x, y, z;
  int col, row;
  size_t size = 0;
  size_t initialVoxelsListSize = 1000;
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
  for (int slice = 0; slice < zpoints; slice++)
  {
    graincount = 1;
    noseeds = 0;
    while (noseeds == 0)
    {
      seed = -1;
      randx = int(double(rg.Random()) * double(xpoints));
      randy = int(double(rg.Random()) * double(ypoints));
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
          double confidence = voxels[point].confidence;
          double imagequality = voxels[point].imagequality;
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
              q2[0] = 0;
              q2[1] = voxels[neighbor].quat[1];
              q2[2] = voxels[neighbor].quat[2];
              q2[3] = voxels[neighbor].quat[3];
              q2[4] = voxels[neighbor].quat[4];
              if (crystruct == AIM::Reconstruction::Hexagonal)
              {
                w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
              }
              if (crystruct == AIM::Reconstruction::Cubic)
              {
                w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
              }

              if (crystruct == AIM::Reconstruction::Hexagonal)
              {
                w2 = MisorientationCalculations::getMisoQuatHexagonal(qs, q2, n1, n2, n3);
              }
              if (crystruct == AIM::Reconstruction::Cubic)
              {
                w2 = MisorientationCalculations::getMisoQuatCubic(qs, q2, n1, n2, n3);
              }

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
  double q1[5];
  double q2[5];
  double qa[5];
  double qb[5];
  double w;
  double n1, n2, n3;
  int seed = -1;
  int point = 0;
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

  // Precalculate some constants
  int totalPMinus1 = totalpoints - 1;

  // Initialize set of grains
  m_Grains.resize(100);
  for(size_t g = 0; g < 100; ++g)
  {
    m_Grains[g] = Grain::New();
  }

  // Copy all the grain names into a densly packed array
  IntArrayType::Pointer grainNames = IntArrayType::CreateArray(totalpoints);
  int* gnames = grainNames->getPointer(0);
  for (int i = 0; i < totalpoints; ++i)
  {
    gnames[i] = voxels[i].grainname;
  }

  // Create initial set of grain average quaternions
  DoubleArrayType::Pointer m_grainQuats = DoubleArrayType::CreateArray(1000*5);
  m_grainQuats->initializeWithZeros();
  double* grainquats = m_grainQuats->getPointer(0);
  for(int i=0;i<1000*5;i++)
  {
  grainquats[i] = 0.0;
  }

  // Burn volume with tight orientation tolerance to simulate simultaneous growth/aglomeration
  while (noseeds == 0)
  {
    seed = -1;
    randpoint = int(double(rg.Random()) * double(totalPMinus1));
    for (int k = 0; k < totalpoints; ++k)
    {
     point = randpoint+k;
       if (point > totalPMinus1) point = point - totalpoints;
       if (gnames[point] == -1) seed = point;
       if (seed > -1) break;
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
    for (int k = 0; k < 5; k++)
    {
    grainquats[graincount*5 + k] = grainquats[graincount*5 + k] + voxels[seed].quat[k];
    }
      for (size_t j = 0; j < size; ++j)
      {
        int currentpoint = voxelslist[j];
        col = currentpoint % xpoints;
        row = (currentpoint / xpoints) % ypoints;
        plane = currentpoint / (xpoints * ypoints);
        q1[0] = 1;
        q1[1] = voxels[currentpoint].quat[1];
        q1[2] = voxels[currentpoint].quat[2];
        q1[3] = voxels[currentpoint].quat[3];
        q1[4] = voxels[currentpoint].quat[4];
//        q1[0] = 1;
//        q1[1] = grainquats[graincount*5 + 1] / grainquats[graincount*5];
//        q1[2] = grainquats[graincount*5 + 2] / grainquats[graincount*5];
//        q1[3] = grainquats[graincount*5 + 3] / grainquats[graincount*5];
//        q1[4] = grainquats[graincount*5 + 4] / grainquats[graincount*5];

        for (int i = 0; i < 6; i++)
        {
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
            q2[0] = 1;
            q2[1] = voxels[neighbor].quat[1];
            q2[2] = voxels[neighbor].quat[2];
            q2[3] = voxels[neighbor].quat[3];
            q2[4] = voxels[neighbor].quat[4];
            if (crystruct == AIM::Reconstruction::Hexagonal)
            {
              w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
            }
            if (crystruct == AIM::Reconstruction::Cubic)
            {
              w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
            }
            if (w < 1.0)
            {
              gnames[neighbor] = graincount;
			for (int k = 0; k < 5; k++)
			{
				qa[k] = voxels[seed].quat[k];
				qb[k] = voxels[neighbor].quat[k];
			}
			if (crystruct == AIM::Reconstruction::Cubic) MisorientationCalculations::getNearestQuatCubic(qa,qb);
			if (crystruct == AIM::Reconstruction::Hexagonal) MisorientationCalculations::getNearestQuatHexagonal(qa,qb);
			for (int k = 0; k < 5; k++)
			{
				voxels[neighbor].quat[k] = qb[k];
				grainquats[graincount*5 + k] = grainquats[graincount*5 + k] + voxels[neighbor].quat[k];
			}
              voxelslist[size] = neighbor;
              size++;
              if (size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
            }
          }
        }
      }
      voxelslist.erase(std::remove(voxelslist.begin(), voxelslist.end(), -1), voxelslist.end());
	  if(m_Grains[graincount]->voxellist != NULL) { delete m_Grains[graincount]->voxellist; }
	  m_Grains[graincount]->voxellist = new std::vector<int>(voxelslist.size());
	  m_Grains[graincount]->voxellist->swap(voxelslist);
	  m_Grains[graincount]->active = 1;
      graincount++;
	  if (graincount >= m_Grains.size())
	  {
	      size_t oldSize = m_Grains.size();
	      m_Grains.resize(m_Grains.size() + 100);
	      for(size_t g = oldSize; g < m_Grains.size(); ++g)
	      {
	        m_Grains[g] = Grain::New();
	      }
	  }
      if (graincount >= m_grainQuats->getNumberOfElements()/5)
      {
        // This will allocate a new array and copy all the old values to the new array
        m_grainQuats->resize( (graincount + 100) * 5);
        grainquats = m_grainQuats->getPointer(0);
        for (size_t i = graincount; i < graincount + 100; i++)
        {
      for(size_t j=0;j<5;j++)
      {
        grainquats[i*5+j] = 0.0;
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

  for(int i=0;i<graincount;i++)
  {
   mergedgnames[i] = i;
   newgnames[i] = i;
  }
  for(int i=1;i<graincount;i++)
  {
	  if(m_Grains[i]->active == 1)
	  {
		  size = 0;
		  mergelist[size] = i;
		  size++;
		  for(int j=0;j<size;j++)
		  {
			  vlist = m_Grains[mergelist[j]]->voxellist;
			  for(int k=0;k<vlist->size();k++)
			  {
				vid = vlist->at(k);
		        col = vid % xpoints;
		        row = (vid / xpoints) % ypoints;
			    plane = vid / (xpoints * ypoints);
		        for (int l=0;l<6;l++)
		        {
		          good = 1;
		          neighbor = vid + neighbors[l];
		          if (l == 0 && plane == 0) good = 0;
		          if (l == 5 && plane == (zpoints - 1)) good = 0;
		          if (l == 1 && row == 0) good = 0;
		          if (l == 4 && row == (ypoints - 1)) good = 0;
		          if (l == 2 && col == 0) good = 0;
		          if (l == 3 && col == (xpoints - 1)) good = 0;
				  if (good == 1 && gnames[neighbor] != i && m_Grains[gnames[neighbor]]->active == 1)
				  {
					q1[0] = grainquats[i*5];
					q1[1] = grainquats[i*5 + 1] / grainquats[i*5];
					q1[2] = grainquats[i*5 + 2] / grainquats[i*5];
					q1[3] = grainquats[i*5 + 3] / grainquats[i*5];
					q1[4] = grainquats[i*5 + 4] / grainquats[i*5];
					q2[0] = grainquats[gnames[neighbor]*5];
					q2[1] = grainquats[gnames[neighbor]*5 + 1] / grainquats[gnames[neighbor]*5];
					q2[2] = grainquats[gnames[neighbor]*5 + 2] / grainquats[gnames[neighbor]*5];
					q2[3] = grainquats[gnames[neighbor]*5 + 3] / grainquats[gnames[neighbor]*5];
					q2[4] = grainquats[gnames[neighbor]*5 + 4] / grainquats[gnames[neighbor]*5];
					if (crystruct == AIM::Reconstruction::Hexagonal)
					{
					  w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
					}
					if (crystruct == AIM::Reconstruction::Cubic)
					{
					  w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
					}
					if (w < misorientationtolerance)
					{
						mergelist[size] = gnames[neighbor];
						size++;
						m_Grains[gnames[neighbor]]->active = 0;
						mergedgnames[gnames[neighbor]] = i;
					    if (crystruct == AIM::Reconstruction::Cubic) MisorientationCalculations::getNearestQuatCubic(q1,q2);
					    if (crystruct == AIM::Reconstruction::Hexagonal) MisorientationCalculations::getNearestQuatHexagonal(q1,q2);
						for(int m=0;m<5;m++)
						{
							q2[m] = q2[m]*q2[0];
							grainquats[i*5 + m] = grainquats[i*5 + m] + q2[m];
						}
		                if (size >= mergelist.size()) mergelist.resize(size + initialMergeListSize, -1);
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
  for(int i = 0; i < graincount; ++i)
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
    mergedname = mergedgnames[gnames[i]];
	while(mergedgnames[mergedname] != mergedname)
	{
	  mergedname = mergedgnames[mergedname];
	}
	newname = newgnames[mergedname];
    voxels[i].grainname = newname;
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
  m_grainQuats = AIMArray<double >::NullPointer(); // Clean up the array to release some memory

}

void ReconstructionFunc::assign_badpoints()
{
  vector<int > neighs;
  vector<int > remove;
  int count = 1;
  int good = 1;
  double x, y, z;
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
  int* gnames = new int[totalpoints];
  int* gneighs = new int[totalpoints];
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
        for (int c = 1; c < numgrains; c++)
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
  delete[] gnames;
  delete[] gneighs;
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

void ReconstructionFunc::reorder_grains()
{
  size_t initialVoxelsListSize = 1000;
  size_t size = 0;
  int neighbors[26];
  int good = 0;
  int neighbor = 0;
  int col, row, plane;
  int gnum;
  double q1[5];
  double q2[5];
  double q1avg, q2avg, q3avg, q4avg;
  double diff, sum, tmp;
  double ea1good, ea2good, ea3good;
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
  size_t numgrains = m_Grains.size();
  std::cout << "numgrains: " << numgrains << std::endl;

  size_t maxGrain = 0;
  // Reset all the Grain nucleus values to -1;
  for (int i = 1; i < numgrains; i++)
  {
    m_Grains[i]->nucleus = -1;

#if 0
    std::vector<int>* vlist = m_Grains[i]->voxellist;
    int unassignedCount = 0;
    int assignedCount = 0;
    for (size_t v = 0; v < vlist->size(); ++v)
    {
       int idx = vlist->at(v);
       if (voxels[idx].unassigned == 1) unassignedCount++;
       else assignedCount++;
    }
    std::cout << "Grain[" << i << "] Unassigned Count:" << unassignedCount << "   Assigned Count:" << assignedCount << std::endl;
#endif
  }

  // Reset the "already checked" to 0 for all voxels
  int gg = 0;
  for (int i = 0; i < totalpoints; i++)
  {
    voxels[i].alreadychecked = 0;
    if(voxels[i].unassigned == 0)
    {
      gnum = voxels[i].grainname;
      m_Grains[gnum]->nucleus = i;
    }
  }
  for (int i = 1; i < numgrains; i++)
  {
    if(m_Grains[i]->nucleus != -1)
    {
      size = 0;
      int nucleus = m_Grains[i]->nucleus;
      if(m_Grains[currentgrain]->voxellist == NULL) 
      { 
        m_Grains[currentgrain]->voxellist = new std::vector<int>(initialVoxelsListSize,-1);
      }
      else 
      {
        m_Grains[currentgrain]->voxellist->resize(initialVoxelsListSize,-1);
      }
      m_Grains[currentgrain]->voxellist->at(size) = nucleus;
      voxels[nucleus].alreadychecked = 1;
      if (currentgrain>= m_Grains.size() ) 
      {
        std::cout << "==> currentgrain: " << currentgrain << " m_Grains.size(): " << m_Grains.size() << std::endl;
      }
      voxels[nucleus].grainname = currentgrain;
      if (currentgrain > maxGrain) maxGrain = currentgrain;
      size++;
      for (int k = 0; k < 5; k++)
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
          if (crystruct == AIM::Reconstruction::Cubic) MisorientationCalculations::getNearestQuatCubic(q1,q2);
          if (crystruct == AIM::Reconstruction::Hexagonal) MisorientationCalculations::getNearestQuatHexagonal(q1,q2);
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
            int grainname = voxels[neighbor].grainname;
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
      q1avg = m_Grains[currentgrain]->avg_quat[1]/m_Grains[currentgrain]->avg_quat[0];
      q2avg = m_Grains[currentgrain]->avg_quat[2]/m_Grains[currentgrain]->avg_quat[0];
      q3avg = m_Grains[currentgrain]->avg_quat[3]/m_Grains[currentgrain]->avg_quat[0];
      q4avg = m_Grains[currentgrain]->avg_quat[4]/m_Grains[currentgrain]->avg_quat[0];
      diff=atan2(q2avg,q1avg);
      sum=atan2(q3avg,q4avg);
      ea1good=(diff+sum);
      ea3good=(sum-diff);
      tmp=(q3avg*q3avg)+(q4avg*q4avg);
      tmp = pow(tmp,0.5);
      if(tmp > 1.0) tmp=1.0;
      ea2good=2*acos(tmp);
      m_Grains[currentgrain]->euler1 = ea1good;
      m_Grains[currentgrain]->euler2 = ea2good;
      m_Grains[currentgrain]->euler3 = ea3good;
   //   std::cout << "--> currentgrain: " << currentgrain << std::endl;
      currentgrain++;
    }
    else 
    {
      std::cout << "Grain " << i << " had no nucleus." << std::endl;
    }
  }

#if 1
  std::cout << "m_Grains.size(): " << m_Grains.size() << std::endl;
  std::cout << "currentgrain: " << currentgrain << std::endl;
  std::cout << "maxGrain: " << maxGrain << std::endl;


 // size_t m_grains_size = m_Grains.size();
  gg = 0;
  std::set<int> badGrains;
  for (size_t i = 0; i < totalpoints; ++i)
  {
    if (voxels[i].grainname >= currentgrain && gg != voxels[i].grainname)
    {
      gg = voxels[i].grainname;
      std::cout << "voxels[" << i << "].grainname=" 
        << voxels[i].grainname << "   m_Grains.size()=" 
        <<  currentgrain << "  unassigned: " << voxels[i].unassigned <<  std::endl;
      badGrains.insert(voxels[i].grainname);
    }
  }

  for(std::set<int>::iterator iter = badGrains.begin(); iter != badGrains.end(); ++iter)
  {
    std::cout << "Grain " << *iter << std::endl;
    std::vector<int>* vlist = m_Grains[*iter]->voxellist;
    int unassignedCount = 0;
    int assignedCount = 0;
    std::cout << "   VoxelCount: " << vlist->size() << std::endl;
    for (size_t v = 0; v < vlist->size(); ++v)
    {
      int idx = vlist->at(v);
      if (voxels[idx].unassigned == 1) unassignedCount++;
      else assignedCount++;
    }
    std::cout << "  Unassigned Count:" << unassignedCount << "\n   Assigned Count:" << assignedCount << std::endl;
    std::cout << "  GotContainedMerged: " << m_Grains[*iter]->gotcontainedmerged << std::endl;
  }

#endif

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
    voxelstemp = new Voxel[(newxpoints * newypoints * newzpoints)];
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
          voxelstemp[newvoxelcount] = voxels[point];
          newvoxelcount++;
        }
      }
    }
    delete[] voxels;
    xpoints = newxpoints;
    ypoints = newypoints;
    zpoints = newzpoints;
    totalpoints = (xpoints * ypoints * zpoints);
    voxels = new Voxel[totalpoints];
    for (int i = 0; i < totalpoints; i++)
    {
      voxels[i] = voxelstemp[i];
    }
    delete[] voxelstemp;
  }
  assign_badpoints();
}
int ReconstructionFunc::remove_smallgrains(int numgrains)
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
  m_Grains[gnum]->nucleus = i;
  }
  for (int i = 1; i < numgrains; i++)
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
    if(voxelslist.size() >= minallowedgrainsize)
    {
      m_Grains[currentgrain]->active = 1;
      m_Grains[currentgrain]->numvoxels = voxelslist.size();
      currentgrain++;
      voxelslist.clear();
      voxelslist.resize(initialVoxelsListSize,-1);
    }
    if(voxelslist.size() < minallowedgrainsize)
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
  int* gnames = new int[totalpoints];
  int* unassigned = new int[totalpoints];
  for (int i = 0; i < totalpoints; ++i)
  {
    gnames[i] = voxels[i].grainname;
    unassigned[i] = voxels[i].unassigned;
  }

#if AIM_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalpoints ), ParallelFindKernels( this, gnames, unassigned) );

#else
  double q1[5];
  double q2[5];
  int numVoxel; // number of voxels in the grain...
  int good = 0;
  int neighbor;
  int col, row, plane;
  double w, totalmisorientation;
  double n1, n2, n3;

  int steps = 2;
  int jStride;
  int kStride;

  for (int i = 0; i < totalpoints; i++)
  {
    if (gnames[i] > 0 && unassigned[i] == 0)
    {
      totalmisorientation = 0.0;
      numVoxel = 0;
      q1[1] = voxels[i].quat[1];
      q1[2] = voxels[i].quat[2];
      q1[3] = voxels[i].quat[3];
      q1[4] = voxels[i].quat[4];
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
            if (good == 1 && gnames[i] == gnames[neighbor] && unassigned[neighbor] != 1)
            {
              numVoxel++;
              q2[1] = voxels[neighbor].quat[1];
              q2[2] = voxels[neighbor].quat[2];
              q2[3] = voxels[neighbor].quat[3];
              q2[4] = voxels[neighbor].quat[4];
              if (crystruct == AIM::Reconstruction::Hexagonal)
              {
                w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
              }
              if (crystruct == AIM::Reconstruction::Cubic)
              {
                w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
              }
              totalmisorientation = totalmisorientation + w;
            }
          }
        }
      }
      voxels[i].kernelmisorientation = totalmisorientation / (float)numVoxel;
    q2[0] = 1;
    q2[1] = m_Grains[gnames[i]]->avg_quat[1]/m_Grains[gnames[i]]->avg_quat[0];
    q2[2] = m_Grains[gnames[i]]->avg_quat[2]/m_Grains[gnames[i]]->avg_quat[0];
    q2[3] = m_Grains[gnames[i]]->avg_quat[3]/m_Grains[gnames[i]]->avg_quat[0];
    q2[4] = m_Grains[gnames[i]]->avg_quat[4]/m_Grains[gnames[i]]->avg_quat[0];
      if (crystruct == AIM::Reconstruction::Hexagonal)
      {
         w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
      }
      if (crystruct == AIM::Reconstruction::Cubic)
      {
         w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
      }
      voxels[i].grainmisorientation = w;
    }
    if (gnames[i] == 0 || unassigned[i] != 0)
    {
      voxels[i].kernelmisorientation = 0;
      voxels[i].grainmisorientation = 0;
    }
  }
#endif
  delete[] gnames;
  delete[] unassigned;
}

int ReconstructionFunc::load_data(const std::string &readname)
{
  int bensdata = 0;
  int yoonsdata = 1;
  double **mat;
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
        mat = new double *[totalpoints];
        totalvol = double(totalpoints) * resx * resy * resz;
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
    double value;
    for (int i = 0; i < totalpoints; i++)
    {
      mat[i] = new double[9];
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
    double s, c, s1, c1, s2, c2;
    double ea1, ea2, ea3;
    double cosine1, cosine3, sine1, sine3;
    double denom;
    for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
    {
      denom = mat[i][0] * mat[i][0] + mat[i][1] * mat[i][1] + mat[i][2] * mat[i][2];
      denom = pow(denom, 0.5);
      mat[i][0] = mat[i][0] / denom;
      mat[i][3] = mat[i][3] / denom;
      mat[i][6] = mat[i][6] / denom;
      denom = mat[i][3] * mat[i][3] + mat[i][4] * mat[i][4] + mat[i][5] * mat[i][5];
      denom = pow(denom, 0.5);
      mat[i][1] = mat[i][1] / denom;
      mat[i][4] = mat[i][4] / denom;
      mat[i][7] = mat[i][7] / denom;
      denom = mat[i][6] * mat[i][6] + mat[i][7] * mat[i][7] + mat[i][8] * mat[i][8];
      denom = pow(denom, 0.5);
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
    ifstream inputFile;
    inputFile.open(readname.c_str());
    double s, c, s1, c1, s2, c2;
    double q1, q2, q3, q4;
    int gnum, phase;
    double ea1, ea2, ea3;
    int x, y, z;
    double stress, strain, weight;
    string dummy;
    numgrains = 0;
    int active = 1;
    size_t vnum;
    int vListSize = 1000;
    for (int i = 0; i < 1; i++)
    {
      inputFile >> xpoints >> ypoints >> zpoints;
      inputFile >> resx >> resy >> resz;
    }
    totalpoints = (xpoints * ypoints * zpoints);
    delete[] voxels;
    voxels = new Voxel[totalpoints];
    for (int i = 0; i < totalpoints; i++)
    {
      inputFile >> ea1 >> ea2 >> ea3 >> weight >> strain >> stress >> x >> y >> z >> gnum >> phase;
      ea1 = ea1 * m_pi_over_180;
      ea2 = ea2 * m_pi_over_180;
      ea3 = ea3 * m_pi_over_180;
      voxels[i].grainname = gnum;
      voxels[i].euler1 = ea1;
      voxels[i].euler2 = ea2;
      voxels[i].euler3 = ea3;
      s = sin(0.5 * ea2);
      c = cos(0.5 * ea2);
      s1 = sin(0.5 * (ea1 - ea3));
      c1 = cos(0.5 * (ea1 - ea3));
      s2 = sin(0.5 * (ea1 + ea3));
      c2 = cos(0.5 * (ea1 + ea3));
      q1 = s * c1;
      q2 = s * s1;
      q3 = c * s2;
      q4 = c * c2;
      voxels[i].quat[0] = 0;
      voxels[i].quat[1] = q1;
      voxels[i].quat[2] = q2;
      voxels[i].quat[3] = q3;
      voxels[i].quat[4] = q4;
      if (gnum > numgrains)
      {
        numgrains = gnum;
        size_t oldSize = m_Grains.size();
        m_Grains.resize(numgrains + 1);
        for(size_t g = oldSize; g < m_Grains.size(); ++g)
        {
          m_Grains[g] = Grain::New();
        }
      }
      vnum = m_Grains[gnum]->numvoxels;
      if (m_Grains[gnum]->voxellist == NULL)
      {
        m_Grains[gnum]->voxellist = new std::vector<int >(vListSize, -1);
      }
      if (vnum >= m_Grains[gnum]->voxellist->size())
      {
        m_Grains[gnum]->voxellist->resize(m_Grains[gnum]->voxellist->size() + vListSize, -1);
      }
      m_Grains[gnum]->voxellist->at(vnum) = i;
      vnum++;
      m_Grains[gnum]->numvoxels = vnum;
    }
    numgrains = numgrains + 1;
    for (int i = 1; i < numgrains; i++)
    {
      if (m_Grains[i]->numvoxels > 0)
      {
        m_Grains[i]->voxellist->erase(std::remove(m_Grains[i]->voxellist->begin(), m_Grains[i]->voxellist->end(), -1), m_Grains[i]->voxellist->end());
        int numvoxels = int(m_Grains[i]->voxellist->size());
        m_Grains[i]->numvoxels = numvoxels;
        m_Grains[i]->nucleus = m_Grains[i]->voxellist->at(0);
        m_Grains[i]->active = active;
      }
    }
    inputFile.close();
  }
  return numgrains;
}
void ReconstructionFunc::merge_twins()
{
  int twinmerged = 1;
  double angcur = 180;
  vector<int > twinlist;
  double w;
  double n1, n2, n3;
  double q1[5];
  double q2[5];
  size_t numgrains = m_Grains.size();

  for (int i = 1; i < numgrains; i++)
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
          int neigh = nlist->at(l);
          if (neigh != i && m_Grains[neigh]->twinnewnumber == -1)
          {
            q1[1] = m_Grains[firstgrain]->avg_quat[1];
            q1[2] = m_Grains[firstgrain]->avg_quat[2];
            q1[3] = m_Grains[firstgrain]->avg_quat[3];
            q1[4] = m_Grains[firstgrain]->avg_quat[4];
            q2[1] = m_Grains[neigh]->avg_quat[1];
            q2[2] = m_Grains[neigh]->avg_quat[2];
            q2[3] = m_Grains[neigh]->avg_quat[3];
            q2[4] = m_Grains[neigh]->avg_quat[4];
            double s = sin(0.5 * 1.86141);
            double c = cos(0.5 * 1.86141);
            double s1 = sin(0.5 * (2.39796 - 1.23277));
            double c1 = cos(0.5 * (2.39796 - 1.23277));
            double s2 = sin(0.5 * (2.39796 + 1.23277));
            double c2 = cos(0.5 * (2.39796 + 1.23277));
            q1[1] = s * c1;
            q1[2] = s * s1;
            q1[3] = c * s2;
            q1[4] = c * c2;
            s = sin(0.5 * 0.78007);
            c = cos(0.5 * 0.78007);
            s1 = sin(0.5 * (1.76807 - 1.4174));
            c1 = cos(0.5 * (1.76807 - 1.4174));
            s2 = sin(0.5 * (1.76807 + 1.4174));
            c2 = cos(0.5 * (1.76807 + 1.4174));
            q2[1] = s * c1;
            q2[2] = s * s1;
            q2[3] = c * s2;
            q2[4] = c * c2;
            if (crystruct == AIM::Reconstruction::Hexagonal)
            {
              w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
            }
            if (crystruct == AIM::Reconstruction::Cubic)
            {
              w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
            }

            w = w * (m_pi / 180.0);
            double tanhalfang = tan(w / 2.0);
            double rodvect1 = tanhalfang * n1;
            double rodvect2 = tanhalfang * n2;
            double rodvect3 = tanhalfang * n3;
            double vecttol = 0.03;
            double rodvectdiff11 = fabs(fabs(rodvect1) - (1.0 / 3.0));
            double rodvectdiff12 = fabs(fabs(rodvect2) - (1.0 / 3.0));
            double rodvectdiff13 = fabs(fabs(rodvect3) - (1.0 / 3.0));
            double rodvectdiff21 = fabs(fabs(rodvect1) - 0.2);
            double rodvectdiff22 = fabs(fabs(rodvect2) - 0.2);
            double rodvectdiff23 = fabs(fabs(rodvect3) - 0.2);
            double rodvectdiff31 = fabs(fabs(rodvect1) - 0.25);
            double rodvectdiff32 = fabs(fabs(rodvect2) - 0.25);
            double rodvectdiff33 = fabs(fabs(rodvect3) - 0.25);
            if (rodvectdiff11 < vecttol && rodvectdiff12 < vecttol && rodvectdiff13 < vecttol) twin = 1;
            if (rodvectdiff11 < vecttol && fabs(rodvect2) < vecttol && fabs(rodvect3) < vecttol) twin = 1;
            if (rodvectdiff12 < vecttol && fabs(rodvect1) < vecttol && fabs(rodvect3) < vecttol) twin = 1;
            if (rodvectdiff13 < vecttol && fabs(rodvect1) < vecttol && fabs(rodvect2) < vecttol) twin = 1;
            if (rodvectdiff11 < vecttol && rodvectdiff12 < vecttol && fabs(rodvect3) < vecttol) twin = 1;
            if (rodvectdiff11 < vecttol && rodvectdiff13 < vecttol && fabs(rodvect2) < vecttol) twin = 1;
            if (rodvectdiff12 < vecttol && rodvectdiff13 < vecttol && fabs(rodvect1) < vecttol) twin = 1;
            if (rodvectdiff21 < vecttol && rodvectdiff22 < vecttol && rodvectdiff23 < vecttol) twin = 1;
            if (rodvectdiff31 < vecttol && rodvectdiff32 < vecttol && fabs(rodvect3) < vecttol) twin = 1;
            if (rodvectdiff31 < vecttol && rodvectdiff33 < vecttol && fabs(rodvect2) < vecttol) twin = 1;
            if (rodvectdiff32 < vecttol && rodvectdiff33 < vecttol && fabs(rodvect1) < vecttol) twin = 1;
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
  double angcur = 180;
  vector<int > colonylist;
  double w;
  double n1, n2, n3;
  double q1[5];
  double q2[5];
  size_t numgrains = m_Grains.size();
  for (int i = 1; i < numgrains; i++)
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
          int neigh = nlist->at(l);
          if (neigh != i && m_Grains[neigh]->colonynewnumber != -1)
          {
            q1[1] = m_Grains[firstgrain]->avg_quat[1];
            q1[2] = m_Grains[firstgrain]->avg_quat[2];
            q1[3] = m_Grains[firstgrain]->avg_quat[3];
            q1[4] = m_Grains[firstgrain]->avg_quat[4];
            q2[1] = m_Grains[neigh]->avg_quat[1];
            q2[2] = m_Grains[neigh]->avg_quat[2];
            q2[3] = m_Grains[neigh]->avg_quat[3];
            q2[4] = m_Grains[neigh]->avg_quat[4];
            if (crystruct == AIM::Reconstruction::Hexagonal)
            {
              w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
            }
            if (crystruct == AIM::Reconstruction::Cubic)
            {
              w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
            }
            double tanhalfang = tan((w * m_pi / 180.0) / 2.0);
            double rodvect1 = tanhalfang * n1;
            double rodvect2 = tanhalfang * n2;
            double rodvect3 = tanhalfang * n3;
            double vecttol = 0.03;
            if (fabs(rodvect1) < vecttol && fabs(rodvect2) < vecttol && fabs(fabs(rodvect3) - 0.0919) < vecttol) colony = 1;
            if (fabs(fabs(rodvect1) - 0.289) < vecttol && fabs(fabs(rodvect2) - 0.5) < vecttol && fabs(rodvect3) < vecttol) colony = 1;
            if (fabs(fabs(rodvect1) - 0.57735) < vecttol && fabs(rodvect2) < vecttol && fabs(rodvect3) < vecttol) colony = 1;
            if (fabs(fabs(rodvect1) - 0.33) < vecttol && fabs(fabs(rodvect2) - 0.473) < vecttol && fabs(fabs(rodvect3) - 0.093) < vecttol) colony = 1;
            if (fabs(fabs(rodvect1) - 0.577) < vecttol && fabs(fabs(rodvect2) - 0.053) < vecttol && fabs(fabs(rodvect3) - 0.093) < vecttol) colony = 1;
            if (fabs(fabs(rodvect1) - 0.293) < vecttol && fabs(fabs(rodvect2) - 0.508) < vecttol && fabs(fabs(rodvect3) - 0.188) < vecttol) colony = 1;
            if (fabs(fabs(rodvect1) - 0.5866) < vecttol && fabs(rodvect2) < vecttol && fabs(fabs(rodvect3) - 0.188) < vecttol) colony = 1;
            if (fabs(fabs(rodvect1) - 0.5769) < vecttol && fabs(fabs(rodvect2) - 0.8168) < vecttol && fabs(rodvect3) < vecttol) colony = 1;
            if (fabs(fabs(rodvect1) - 0.9958) < vecttol && fabs(fabs(rodvect2) - 0.0912) < vecttol && fabs(rodvect3) < vecttol) colony = 1;
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
  for (int i = 0; i < numgrains; i++)
  {

  }
}

void ReconstructionFunc::characterize_colonies()
{
  size_t numgrains = m_Grains.size();
  for (int i = 0; i < numgrains; i++)
  {

  }
}

void ReconstructionFunc::renumber_grains3()
{
  size_t numgrains = m_Grains.size();
  int graincount = 1;
  std::vector<int > newnames(numgrains);
  for (int i = 1; i < numgrains; i++)
  {
    int gottwinmerged = m_Grains[i]->gottwinmerged;
    if (gottwinmerged != 1)
    {
      newnames[i] = graincount;
      double ea1good = m_Grains[i]->euler1;
      double ea2good = m_Grains[i]->euler2;
      double ea3good = m_Grains[i]->euler3;
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
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
    int grainname = voxels[j].grainname;
    if (grainname >= 1)
    {
      int newgrainname = newnames[grainname];
      voxels[j].grainname = newgrainname;
    }
  }
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
  double column, row, plane;
  int grain;
  int nnum;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;
  totalsurfacearea = 0;
  int surfacegrain = 1;
  int nListSize = 100;
  // Copy all the grain names into a densly packed array
  int* gnames = new int[totalpoints];
  size_t numgrains = m_Grains.size();
  int gnum = 0;
  std::set<int> leftOutGrains;
  for (int i = 0; i < totalpoints; ++i)
  {
    gnames[i] = voxels[i].grainname;
	if (voxels[i].neighborlist != NULL) { delete voxels[i].neighborlist; }
	voxels[i].neighborlist = new std::vector<int>(6, -1);
#if 0
    if (gnum >= numgrains) {
      gnames[i] = 0;
      leftOutGrains.insert(gnum);
    }
    else
#endif
  }

  for(std::set<int>::iterator iter = leftOutGrains.begin(); iter != leftOutGrains.end(); ++iter)
  {
    std::cout << "Left Out Grain: " << *iter << std::endl;
  }


  for (int i = 0; i < numgrains; i++)
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
    if (voxels[j].neighborlist->size() >= 3) voxels[j].nearestneighbordistance[0] = 0.5 * resx, voxels[j].nearestneighbordistance[1] = 0.5 * resx, voxels[j].nearestneighbordistance[2]
        = 0.5 * resx, voxels[j].nearestneighbor[0] = j, voxels[j].nearestneighbor[1] = j, voxels[j].nearestneighbor[2] = j;
    if (voxels[j].neighborlist->size() == 2) voxels[j].nearestneighbordistance[0] = 0.5 * resx, voxels[j].nearestneighbordistance[1] = 0.5 * resx, voxels[j].nearestneighbordistance[2]
        = -1, voxels[j].nearestneighbor[0] = j, voxels[j].nearestneighbor[1] = j, voxels[j].nearestneighbor[2] = -1;
    if (voxels[j].neighborlist->size() == 1) voxels[j].nearestneighbordistance[0] = 0.5 * resx, voxels[j].nearestneighbordistance[1] = -1, voxels[j].nearestneighbordistance[2]
        = -1, voxels[j].nearestneighbor[0] = j, voxels[j].nearestneighbor[1] = -1, voxels[j].nearestneighbor[2] = -1;
    if (onsurf == 0) voxels[j].nearestneighbordistance[0] = -1, voxels[j].nearestneighbordistance[1] = -1, voxels[j].nearestneighbordistance[2] = -1, voxels[j].nearestneighbor[0]
        = -1, voxels[j].nearestneighbor[1] = -1, voxels[j].nearestneighbor[2] = -1;
  }
  delete[] gnames;
  vector<int> nlistcopy;
  for (int i = 1; i < numgrains; i++)
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
      int neigh = m_Grains[i]->neighborlist->at(j);
      int number = std::count(nlistcopy.begin(), nlistcopy.end(), neigh);
      double area = number * resx * resx;
      m_Grains[i]->neighborsurfarealist->at(j) = area;
      if (m_Grains[i]->surfacegrain == 0 && (neigh > i || m_Grains[neigh]->surfacegrain == 1))
      {
        totalsurfacearea = totalsurfacearea + area;
      }
    }
    m_Grains[i]->numneighbors = numneighs;
  }
}

void ReconstructionFunc::define_neighborhood()
{
  double x, y, z;
  double xn, yn, zn;
  double xdist, ydist, zdist;
  double dist, dist2, diam, diam2;
  int dist_int, dist2_int;
  size_t numgrains = m_Grains.size();

  for (int i = 1; i < numgrains; i++)
  {
      Grain& grain = *(m_Grains[i].get());
      if (grain.active == 1)
      {
        x = grain.centroidx;
        y = grain.centroidy;
        z = grain.centroidz;
        diam = grain.equivdiameter;
        for (int j = i; j < numgrains; j++)
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
            dist = pow(dist, 0.5);
            dist2 = dist;
            dist_int = int(dist / diam);
            dist2_int = int(dist2 / diam2);
            if (dist_int < 3)
            {
              grain.neighbordistfunc[dist_int]++;
            }
            if (dist2_int < 3)
            {
              grain_j.neighbordistfunc[dist2_int]++;
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
  maxdiameter = 0;
  mindiameter = 100000;
  double x, y, z;
  double radcubed;
  double diameter;
  size_t numgrains = m_Grains.size();
  graincenters.resize(numgrains);
  for (int i = 0; i < numgrains; i++)
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
    x = find_xcoord(j);
    y = find_ycoord(j);
    z = find_zcoord(j);
    int col = j % xpoints;
    int row = (j / xpoints) % ypoints;
    int plane = j / (xpoints * ypoints);
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
  for (int i = 1; i < numgrains; i++)
  {
    graincenters[i][1] = graincenters[i][1] / graincenters[i][0];
    graincenters[i][2] = graincenters[i][2] / graincenters[i][0];
    graincenters[i][3] = graincenters[i][3] / graincenters[i][0];
    m_Grains[i]->centroidx = graincenters[i][1];
    m_Grains[i]->centroidy = graincenters[i][2];
    m_Grains[i]->centroidz = graincenters[i][3];
    m_Grains[i]->numvoxels = graincenters[i][0];
    m_Grains[i]->volume = (graincenters[i][0] * resx * resy * resz);
    m_Grains[i]->surfacegrain = graincenters[i][4];
    radcubed = (0.75 * m_Grains[i]->volume) / m_pi;
    diameter = (2 * pow(radcubed, 0.3333333333));
    m_Grains[i]->equivdiameter = diameter;
    if (int(diameter) > maxdiameter)
    {
      maxdiameter = int(diameter);
    }
    if (diameter < mindiameter) mindiameter = diameter;
  }
}
void ReconstructionFunc::find_centroids2D()
{
  //  int count = 0;
  int onedge = 0;
  maxdiameter = 0;
  mindiameter = 100000;
  double x, y;
  double radsquared;
  double diameter;
  size_t numgrains = m_Grains.size();
  graincenters.resize(numgrains);
  for (int i = 0; i < numgrains; i++)
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
    x = find_xcoord(j);
    y = find_ycoord(j);
    int col = j % xpoints;
    int row = (j / xpoints) % ypoints;
    if (col <= 0) onedge = 1;
    if (col >= xpoints - 1) onedge = 1;
    if (row <= 0) onedge = 1;
    if (row >= ypoints - 1) onedge = 1;
    graincenters[gnum][1] = graincenters[gnum][1] + x;
    graincenters[gnum][2] = graincenters[gnum][2] + y;
    if (onedge == 1) graincenters[gnum][3] = 1;
  }
  for (int i = 1; i < numgrains; i++)
  {
    graincenters[i][1] = graincenters[i][1] / graincenters[i][0];
    graincenters[i][2] = graincenters[i][2] / graincenters[i][0];
    m_Grains[i]->centroidx = graincenters[i][1];
    m_Grains[i]->centroidy = graincenters[i][2];
    m_Grains[i]->numvoxels = graincenters[i][0];
    m_Grains[i]->volume = (graincenters[i][0] * resx * resy);
    m_Grains[i]->surfacegrain = graincenters[i][3];
    radsquared = m_Grains[i]->volume / m_pi;
    diameter = (2 * pow(radsquared, 0.5));
    m_Grains[i]->equivdiameter = diameter;
    if (int(diameter) > maxdiameter)
    {
      maxdiameter = int(diameter);
    }
    if (diameter < mindiameter) mindiameter = diameter;
  }
}

void ReconstructionFunc::find_euclidean_map()
{
  int nearestneighbordistance = 0;
  int count = 1;
  int good = 1;
  double x, y, z;
  int neighpoint;
  int nearestneighbor;
  int neighbors[6];
  neighbors[0] = -xpoints * ypoints;
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = xpoints * ypoints;
  int* voxel_NearestNeighbor = new int[totalpoints];
  double* voxel_NearestNeighborDistance = new double[totalpoints];

  for (int loop = 0; loop < 3; loop++)
  {
    nearestneighbordistance = 0;
    for (int a = 0; a < (totalpoints); ++a)
    {
      voxel_NearestNeighbor[a] = voxels[a].nearestneighbor[loop];
      voxel_NearestNeighborDistance[a] = voxels[a].nearestneighbordistance[loop];
    }
    count = 1;
    while (count != 0)
    {
      count = 0;
      nearestneighbordistance++;
      for (int i = 0; i < (totalpoints); ++i)
      {
        if (voxel_NearestNeighbor[i] == -1)
        {
          x = i % xpoints;
          y = (i / xpoints) % ypoints;
          z = i / (xpoints * ypoints);
          for (int j = 0; j < 6; j++)
          {
            good = 1;
            neighpoint = i + neighbors[j];
            if (j == 0 && z == 0) good = 0;
            else if (j == 5 && z == (zpoints - 1)) good = 0;
            else if (j == 1 && y == 0) good = 0;
            else if (j == 4 && y == (ypoints - 1)) good = 0;
            else if (j == 2 && x == 0) good = 0;
            else if (j == 3 && x == (xpoints - 1)) good = 0;
            if (good == 1)
            {
              count++;
              if (voxel_NearestNeighborDistance[neighpoint] != -1)
              {
                voxel_NearestNeighbor[i] = voxel_NearestNeighbor[neighpoint];
              }
            }
          }
        }
      }
      for (int j = 0; j < (totalpoints); ++j)
      {
        if (voxel_NearestNeighbor[j] != -1 && voxel_NearestNeighborDistance[j] == -1)
        {
          voxel_NearestNeighborDistance[j] = nearestneighbordistance;
        }
      }
    }
    double x1, x2, y1, y2, z1, z2;
    double dist;
    for (int j = 0; j < (totalpoints); j++)
    {
      nearestneighbor = voxel_NearestNeighbor[j];
      x1 = resx * double(j % xpoints); // find_xcoord(j);
      y1 = resy * double((j / xpoints) % ypoints);// find_ycoord(j);
      z1 = resz * double(j / (xpoints * ypoints)); // find_zcoord(j);
      x2 = resx * double(nearestneighbor % xpoints); // find_xcoord(nearestneighbor);
      y2 = resy * double((nearestneighbor / xpoints) % ypoints); // find_ycoord(nearestneighbor);
      z2 = resz * double(nearestneighbor / (xpoints * ypoints)); // find_zcoord(nearestneighbor);
      dist = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
      dist = pow(dist, 0.5);
      voxel_NearestNeighborDistance[j] = dist + (0.5 * resx);
    }
    for (int a = 0; a < (totalpoints); ++a)
    {
      voxels[a].nearestneighbor[loop] = voxel_NearestNeighbor[a];
      voxels[a].nearestneighbordistance[loop] = voxel_NearestNeighborDistance[a];
    }
  }
  delete[] voxel_NearestNeighbor;
  delete[] voxel_NearestNeighborDistance;
}
double ReconstructionFunc::find_xcoord(size_t index)
{
  double x = resx * double(index % xpoints);
  return x;
}
double ReconstructionFunc::find_ycoord(size_t index)
{
  double y = resy * double((index / xpoints) % ypoints);
  return y;
}
double ReconstructionFunc::find_zcoord(size_t index)
{
  double z = resz * double(index / (xpoints * ypoints));
  return z;
}

void ReconstructionFunc::find_moments()
{
  //  int count = 0;
  double u200 = 0;
  double u020 = 0;
  double u002 = 0;
  double u110 = 0;
  double u011 = 0;
  double u101 = 0;
  size_t numgrains = m_Grains.size();
  grainmoments.resize(numgrains);
  for (int i = 0; i < numgrains; i++)
  {
    grainmoments[i].resize(6);
    for (int j = 0; j < 6; j++)
    {
      grainmoments[i][j] = 0;
    }
  }
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
    u200 = 0;
    u020 = 0;
    u002 = 0;
    u110 = 0;
    u011 = 0;
    u101 = 0;
    int gnum = voxels[j].grainname;
    double x = find_xcoord(j);
    double y = find_ycoord(j);
    double z = find_zcoord(j);
    double x1 = x + (resx / 2);
    double x2 = x - (resx / 2);
    double y1 = y + (resy / 2);
    double y2 = y - (resy / 2);
    double z1 = z + (resz / 2);
    double z2 = z - (resz / 2);
    double xdist1 = (x1 - graincenters[gnum][1]);
    double ydist1 = (y1 - graincenters[gnum][2]);
    double zdist1 = (z1 - graincenters[gnum][3]);
    double xdist2 = (x1 - graincenters[gnum][1]);
    double ydist2 = (y1 - graincenters[gnum][2]);
    double zdist2 = (z2 - graincenters[gnum][3]);
    double xdist3 = (x1 - graincenters[gnum][1]);
    double ydist3 = (y2 - graincenters[gnum][2]);
    double zdist3 = (z1 - graincenters[gnum][3]);
    double xdist4 = (x1 - graincenters[gnum][1]);
    double ydist4 = (y2 - graincenters[gnum][2]);
    double zdist4 = (z2 - graincenters[gnum][3]);
    double xdist5 = (x2 - graincenters[gnum][1]);
    double ydist5 = (y1 - graincenters[gnum][2]);
    double zdist5 = (z1 - graincenters[gnum][3]);
    double xdist6 = (x2 - graincenters[gnum][1]);
    double ydist6 = (y1 - graincenters[gnum][2]);
    double zdist6 = (z2 - graincenters[gnum][3]);
    double xdist7 = (x2 - graincenters[gnum][1]);
    double ydist7 = (y2 - graincenters[gnum][2]);
    double zdist7 = (z1 - graincenters[gnum][3]);
    double xdist8 = (x2 - graincenters[gnum][1]);
    double ydist8 = (y2 - graincenters[gnum][2]);
    double zdist8 = (z2 - graincenters[gnum][3]);
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
  for (int i = 1; i < numgrains; i++)
  {
    grainmoments[i][0] = grainmoments[i][0] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][1] = grainmoments[i][1] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][2] = grainmoments[i][2] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][3] = grainmoments[i][3] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][4] = grainmoments[i][4] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    grainmoments[i][5] = grainmoments[i][5] * (resx / 2.0) * (resy / 2.0) * (resz / 2.0);
    double o3 = (grainmoments[i][0] * grainmoments[i][1] * grainmoments[i][2]) + (2.0 * grainmoments[i][3] * grainmoments[i][5] * grainmoments[i][4])
        - (grainmoments[i][0] * grainmoments[i][4] * grainmoments[i][4]) - (grainmoments[i][1] * grainmoments[i][5] * grainmoments[i][5]) - (grainmoments[i][2]
        * grainmoments[i][3] * grainmoments[i][3]);
    double vol5 = m_Grains[i]->volume;
    vol5 = pow(vol5, 5);
    double omega3 = vol5 / o3;
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
for (int i = 1; i < numgrains; i++)
  {
    double Ixx = m_Grains[i]->Ixx;
    double Iyy = m_Grains[i]->Iyy;
    double Izz = m_Grains[i]->Izz;
    double Ixy = m_Grains[i]->Ixy;
    double Iyz = m_Grains[i]->Iyz;
    double Ixz = m_Grains[i]->Ixz;
    double a = 1;
    double b = -Ixx - Iyy - Izz;
    double c = ((Ixx * Izz) + (Ixx * Iyy) + (Iyy * Izz) - (Ixz * Ixz) - (Ixy * Ixy) - (Iyz * Iyz));
    double d = ((Ixz * Iyy * Ixz) + (Ixy * Izz * Ixy) + (Iyz * Ixx * Iyz) - (Ixx * Iyy * Izz) - (Ixy * Iyz * Ixz) - (Ixy * Iyz * Ixz));
    double f = ((3 * c / a) - ((b / a) * (b / a))) / 3;
    double g = ((2 * (b / a) * (b / a) * (b / a)) - (9 * b * c / (a * a)) + (27 * (d / a))) / 27;
    double h = (g * g / 4) + (f * f * f / 27);
    double rsquare = (g * g / 4) - h;
    double r = pow(rsquare, 0.5);
    double theta = 0;
    if (r == 0)
    {
      theta = 0;
    }
    if (r != 0)
    {
      theta = acos(-g / (2 * r));
    }
    double r1 = 2 * pow(r, 0.33333333333) * cos(theta / 3) - (b / (3 * a));
    double r2 = -pow(r, 0.33333333333) * (cos(theta / 3) - (1.7320508 * sin(theta / 3))) - (b / (3 * a));
    double r3 = -pow(r, 0.33333333333) * (cos(theta / 3) + (1.7320508 * sin(theta / 3))) - (b / (3 * a));
    m_Grains[i]->radius1 = r1;
    m_Grains[i]->radius2 = r2;
    m_Grains[i]->radius3 = r3;
  }
}
void ReconstructionFunc::find_vectors(H5ReconStatsWriter::Pointer h5io)
{
  totalaxes = 0.0;
  double *axisodf;
  axisodf = new double[18 * 18 * 18];
  size_t numgrains = m_Grains.size();
  for (int i = 1; i < numgrains; i++)
  {
    //   int size = grains[i].numvoxels;
    double Ixx = m_Grains[i]->Ixx;
    double Iyy = m_Grains[i]->Iyy;
    double Izz = m_Grains[i]->Izz;
    double Ixy = m_Grains[i]->Ixy;
    double Iyz = m_Grains[i]->Iyz;
    double Ixz = m_Grains[i]->Ixz;
    double radius1 = m_Grains[i]->radius1;
    double radius2 = m_Grains[i]->radius2;
    double radius3 = m_Grains[i]->radius3;
    double m[3][3];
    double e[3][1];
    double uber[3][3];
    double bmat[3][1];
    double vect[3][3];
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
      double **uberelim;
      double **uberbelim;
      uberelim = new double *[3];
      uberbelim = new double *[3];
      for (int d = 0; d < 3; d++)
      {
        uberelim[d] = new double[3];
        uberbelim[d] = new double[1];
      }
      int elimcount = 0;
      int elimcount1 = 0;
      double q = 0;
      double sum = 0;
      double c = 0;
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
    double n1x = vect[0][0];
    double n1y = vect[0][1];
    double n1z = vect[0][2];
    double n2x = vect[1][0];
    double n2y = vect[1][1];
    double n2z = vect[1][2];
    double n3x = vect[2][0];
    double n3y = vect[2][1];
    double n3z = vect[2][2];
    double norm1 = pow(((n1x * n1x) + (n1y * n1y) + (n1z * n1z)), 0.5);
    double norm2 = pow(((n2x * n2x) + (n2y * n2y) + (n2z * n2z)), 0.5);
    double norm3 = pow(((n3x * n3x) + (n3y * n3y) + (n3z * n3z)), 0.5);
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
        double o[3][3];
        double ga[3][3];
        double m1[3][3];
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
        double ea2 = acos(m1[2][2]);
        double cosine3 = (m1[1][2] / sin(ea2));
        double sine3 = (m1[0][2] / sin(ea2));
        double cosine1 = (-m1[2][1] / sin(ea2));
        double sine1 = (m1[2][0] / sin(ea2));
        double ea3 = acos(cosine3);
        double ea1 = acos(cosine1);
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
          axisodf[bin] = axisodf[bin] + 1.0;
          totalaxes = totalaxes + 1.0;
        }
      }
    }
  }
  int err;
  err = h5io->writeAxisOrientationData(axisodf, totalaxes);
  delete[] axisodf;
}
void ReconstructionFunc::find_moments2D()
{
  //  int count = 0;
  double u200 = 0;
  double u020 = 0;
  double u110 = 0;
  size_t numgrains = m_Grains.size();
  grainmoments.resize(numgrains);
  for (int i = 0; i < numgrains; i++)
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
    double x = find_xcoord(j);
    double y = find_ycoord(j);
    double x1 = x + (resx / 2);
    double x2 = x - (resx / 2);
    double y1 = y + (resy / 2);
    double y2 = y - (resy / 2);
    double xdist1 = (x1 - graincenters[gnum][1]);
    double ydist1 = (y1 - graincenters[gnum][2]);
    double xdist2 = (x1 - graincenters[gnum][1]);
    double ydist2 = (y2 - graincenters[gnum][2]);
    double xdist3 = (x2 - graincenters[gnum][1]);
    double ydist3 = (y1 - graincenters[gnum][2]);
    double xdist4 = (x2 - graincenters[gnum][1]);
    double ydist4 = (y2 - graincenters[gnum][2]);
    u200 = u200 + ((ydist1) * (ydist1)) + ((ydist2) * (ydist2)) + ((ydist3) * (ydist3)) + ((ydist4) * (ydist4));
    u020 = u020 + ((xdist1) * (xdist1)) + ((xdist2) * (xdist2)) + ((xdist3) * (xdist3)) + ((xdist4) * (xdist4));
    u110 = u110 + ((xdist1) * (ydist1)) + ((xdist2) * (ydist2)) + ((xdist3) * (ydist3)) + ((xdist4) * (ydist4));
    grainmoments[gnum][0] = grainmoments[gnum][0] + u200;
    grainmoments[gnum][1] = grainmoments[gnum][1] + u020;
    grainmoments[gnum][2] = grainmoments[gnum][2] + u110;
  }
  for (int i = 1; i < numgrains; i++)
  {
    grainmoments[i][0] = grainmoments[i][0] * (resx / 2.0) * (resy / 2.0);
    grainmoments[i][1] = grainmoments[i][1] * (resx / 2.0) * (resy / 2.0);
    grainmoments[i][2] = grainmoments[i][2] * (resx / 2.0) * (resy / 2.0);
    //  double o3 = (grainmoments[i][0]*grainmoments[i][1]*grainmoments[i][2])+(2.0*grainmoments[i][3]*grainmoments[i][5]*grainmoments[i][4])-(grainmoments[i][0]*grainmoments[i][4]*grainmoments[i][4])-(grainmoments[i][1]*grainmoments[i][5]*grainmoments[i][5])-(grainmoments[i][2]*grainmoments[i][3]*grainmoments[i][3]);
    //  double vol5 = m_Grains[i]->volume;
    //  vol5 = pow(vol5,5);
    //  double omega3 = vol5/o3;
    m_Grains[i]->Ixx = grainmoments[i][0];
    m_Grains[i]->Iyy = grainmoments[i][1];
    m_Grains[i]->Ixy = -grainmoments[i][2];
    //  m_Grains[i]->omega3 = omega3;
  }
}
void ReconstructionFunc::find_axes2D()
{
  size_t numgrains = m_Grains.size();
  for (int i = 1; i < numgrains; i++)
  {
    double Ixx = m_Grains[i]->Ixx;
    double Iyy = m_Grains[i]->Iyy;
    double Ixy = m_Grains[i]->Ixy;
    double r1 = (Ixx + Iyy) / 2.0 + sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    double r2 = (Ixx + Iyy) / 2.0 - sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    double preterm = 4 / 3.1415926535897;
    preterm = pow(preterm, 0.25);
    double postterm1 = r1 * r1 * r1 / r2;
    double postterm2 = r2 * r2 * r2 / r1;
    postterm1 = pow(postterm1, 0.125);
    postterm2 = pow(postterm2, 0.125);
    r1 = preterm * postterm1;
    r2 = preterm * postterm2;
    m_Grains[i]->radius1 = r1;
    m_Grains[i]->radius2 = r2;
  }
}
void ReconstructionFunc::find_vectors2D(H5ReconStatsWriter::Pointer h5io)
{
  totalaxes = 0.0;
  double *axisodf;
  size_t numgrains = m_Grains.size();

  axisodf = new double[18 * 18 * 18];
  for (int i = 1; i < numgrains; i++)
  {
    //   int size = grains[i].numvoxels;
    double Ixx = m_Grains[i]->Ixx;
    double Iyy = m_Grains[i]->Iyy;
    double Ixy = m_Grains[i]->Ixy;
    double I1 = (Ixx + Iyy) / 2.0 + sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    double I2 = (Ixx + Iyy) / 2.0 - sqrt(((Ixx + Iyy) * (Ixx + Iyy)) / 4.0 + (Ixy * Ixy - Ixx * Iyy));
    double n1x = (Ixx - I1) / Ixy;
    double n1y = 1;
    double n2x = (Ixx - I2) / Ixy;
    double n2y = 1;
    double norm1 = pow((n1x * n1x + n1y * n1y), 0.5);
    double norm2 = pow((n2x * n2x + n2y * n2y), 0.5);
    n1x = n1x / norm1;
    n1y = n1y / norm1;
    n2x = n2x / norm2;
    n2y = n2y / norm2;
    double cosine1 = n1x;
    double ea1 = acos(cosine1);
    if (ea1 > m_pi) ea1 = ea1 - m_pi;
    int ea1bin = int(ea1 / (m_pi / 18));
    int bin = 0;
    if (ea1 >= 0.0 && ea1 < (m_pi))
    {
      m_Grains[i]->axiseuler1 = ea1;
      bin = ea1bin;
      axisodf[bin] = axisodf[bin] + 1.0;
      totalaxes = totalaxes + 1.0;
    }
  }
  int err;
  err = h5io->writeAxisOrientationData(axisodf, totalaxes);
  delete[] axisodf;
}
void ReconstructionFunc::find_eulerodf(H5ReconStatsWriter::Pointer h5io)
{
  totalvol = 0;
  //double w, denom, n1, n2, n3;
  size_t bin;
  double dim1 = 0.0;
  double dim2 = 0.0;
  double dim3 = 0.0;
  int numbins = 0;
  size_t numgrains = m_Grains.size();

  MisorientationCalculations::initializeDims(crystruct, dim1, dim2, dim3, numbins);

  //double degtorad = m_pi / 180.0;
  double q1[5];
  double qref[5];
  double *eulerodf;
  qref[1] = sin(0.5 * 0.0) * cos(0.5 * (0.0 - 0.0));
  qref[2] = sin(0.5 * 0.0) * sin(0.5 * (0.0 - 0.0));
  qref[3] = cos(0.5 * 0.0) * sin(0.5 * (0.0 + 0.0));
  qref[4] = cos(0.5 * 0.0) * cos(0.5 * (0.0 + 0.0));
  if (crystruct == AIM::Reconstruction::Hexagonal)
  {
    eulerodf = new double[36 * 36 * 12];
    for (int i = 0; i < 36 * 36 * 12; i++)
    {
      eulerodf[i] = 0.0;
    }
  }
  if (crystruct == AIM::Reconstruction::Cubic)
  {
    eulerodf = new double[18 * 18 * 18];
    for (int i = 0; i < 18 * 18 * 18; i++)
    {
      eulerodf[i] = 0.0;
    }
  }
  for (int i = 1; i < numgrains; i++)
  {
    if (m_Grains[i]->surfacegrain == 0 && m_Grains[i]->active == 1)
    {
      double vol = m_Grains[i]->volume;
      q1[1] = m_Grains[i]->avg_quat[1];
      q1[2] = m_Grains[i]->avg_quat[2];
      q1[3] = m_Grains[i]->avg_quat[3];
      q1[4] = m_Grains[i]->avg_quat[4];
      if (crystruct == AIM::Reconstruction::Hexagonal)
      {
        bin = MisorientationCalculations::calculateHexOdfBin(q1, qref, dim1, dim2, dim3);

//        w = MisorientationCalculations::getMisoQuatHexagonal(q1, qref, n1, n2, n3);
//        w = w * degtorad;
//        denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
//        denom = pow(denom, 0.5);
//        n1 = n1 / denom;
//        n2 = n2 / denom;
//        n3 = n3 / denom;
//        n1 = n1 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
//        n2 = n2 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
//        n3 = n3 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
//        ea1bin = size_t(n1 * 36.0 / dim1);
//        ea2bin = size_t(n2 * 36.0 / dim2);
//        ea3bin = size_t(n3 * 12.0 / dim3);
//        if (ea1bin >= 36) ea1bin = 35;
//        if (ea2bin >= 36) ea2bin = 35;
//        if (ea3bin >= 12) ea3bin = 11;
//        bin = (ea3bin * 36 * 36) + (ea2bin * 36) + (ea1bin);
        eulerodf[bin] = eulerodf[bin] + vol;
        totalvol = totalvol + vol;
      }
      else if (crystruct == AIM::Reconstruction::Cubic)
      {
        bin = MisorientationCalculations::calculateCubicOdfBin(q1, qref, dim1, dim2, dim3);
//        w = MisorientationCalculations::getMisoQuatCubic(q1, qref, n1, n2, n3);
//        w = w * degtorad;
//        denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
//        denom = pow(denom, 0.5);
//        n1 = n1 / denom;
//        n2 = n2 / denom;
//        n3 = n3 / denom;
//        n1 = n1 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
//        n2 = n2 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
//        n3 = n3 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
//        ea1bin = size_t(n1 * 18.0 / dim1);
//        ea2bin = size_t(n2 * 18.0 / dim2);
//        ea3bin = size_t(n3 * 18.0 / dim3);
//        if (ea1bin >= 18) ea1bin = 17;
//        if (ea2bin >= 18) ea2bin = 17;
//        if (ea3bin >= 18) ea3bin = 17;
//        bin = (ea3bin * 18 * 18) + (ea2bin * 18) + (ea1bin);
        eulerodf[bin] = eulerodf[bin] + vol;
        totalvol = totalvol + vol;
      }
    }
  }
  int err;
  err = h5io->writeODFData(crystruct, eulerodf, totalvol);
  /*  double delta = m_pi / 18;
   double texindex = 0;
   double texstrength = 0;
   for (int iter51 = 0; iter51 < 18; iter51++)
   {
   for (int iter52 = 0; iter52 < 18; iter52++)
   {
   for (int iter53 = 0; iter53 < 18; iter53++)
   {
   double f = 0;
   bin = (iter51 * 18 * 18) + (iter52 * 18) + (iter53);
   if (iter52 == 0)
   {
   f = (m_pi * m_pi / 4) * (double(eulerodf[bin] / totalvol) / ((delta) * (delta) * cos(double((iter52 * delta) + (delta / 2)))));
   }
   if (iter52 == 18)
   {
   f = (m_pi * m_pi / 4) * (double(eulerodf[bin] / totalvol) / ((delta) * (delta) * cos(double((iter52 * delta) - (delta / 2)))));
   }
   if (iter52 != 0 && iter52 != 18)
   {
   f = (m_pi * m_pi / 4) * (double(eulerodf[bin] / totalvol) / ((delta) * (delta) * (cos(double((iter52 * delta) - (delta / 2)))
   - cos(double((iter52 * delta) + (delta / 2))))));
   }
   texindex = texindex + (f * f);
   }
   }
   }
   texindex = texindex / (18 * 18 * 18);
   texstrength = pow(texindex, 0.5);
   */
  delete[] eulerodf;
}


void ReconstructionFunc::measure_misorientations(H5ReconStatsWriter::Pointer h5io)
{
  size_t initialsize = 10;
  double degtorad = m_pi / 180.0;
  double n1, n2, n3;
  int mbin;
  double w;
  double q1[5];
  double q2[5];
  double denom = 0;
  int nummisobins = 0;
  size_t numgrains = m_Grains.size();
  if (crystruct == AIM::Reconstruction::Cubic) nummisobins = 18 * 18 * 18;
  if (crystruct == AIM::Reconstruction::Hexagonal) nummisobins = 36 * 36 * 12;
  DoubleArrayType::Pointer misobinPtr = DoubleArrayType::CreateArray(nummisobins);
  misobinPtr->initializeWithZeros();
  double* misobin = misobinPtr->getPointer(0);
  double microbin[10];
  for(size_t e = 0; e < 10; ++e)
  {
    microbin[e] = 0.0;
  }
  for (int e = 0; e < nummisobins; e++)
  {
    misobin[e] = 0.0;
  }
  int nname;
  double microcount = 0.0;
  double nsa;

  for (int i = 1; i < numgrains; i++)
  {
    microcount = 0;
    microcount = 0.0;
  q1[1] = m_Grains[i]->avg_quat[1]/m_Grains[i]->avg_quat[0];
    q1[2] = m_Grains[i]->avg_quat[2]/m_Grains[i]->avg_quat[0];
    q1[3] = m_Grains[i]->avg_quat[3]/m_Grains[i]->avg_quat[0];
    q1[4] = m_Grains[i]->avg_quat[4]/m_Grains[i]->avg_quat[0];
  //  int size = 0;
  m_Grains[i]->misorientationlist = new std::vector<double >(m_Grains[i]->neighborlist->size()*3, -1.0);
  for (int j = 0; j < m_Grains[i]->neighborlist->size(); j++)
    {
      nname = m_Grains[i]->neighborlist->at(j);
      q2[1] = m_Grains[nname]->avg_quat[1]/m_Grains[nname]->avg_quat[0];
      q2[2] = m_Grains[nname]->avg_quat[2]/m_Grains[nname]->avg_quat[0];
      q2[3] = m_Grains[nname]->avg_quat[3]/m_Grains[nname]->avg_quat[0];
      q2[4] = m_Grains[nname]->avg_quat[4]/m_Grains[nname]->avg_quat[0];
      if (crystruct == AIM::Reconstruction::Hexagonal)
      {
          w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
      }
      else if (crystruct == AIM::Reconstruction::Cubic)
      {
          w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
      }
      w = w * degtorad;
      denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
      denom = pow(denom, 0.5);
      n1 = n1 / denom;
      n2 = n2 / denom;
      n3 = n3 / denom;
    m_Grains[i]->misorientationlist->at(3 * j) = n1 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
      m_Grains[i]->misorientationlist->at(3 * j + 1) = n2 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
      m_Grains[i]->misorientationlist->at(3 * j + 2) = n3 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
      if (crystruct == AIM::Reconstruction::Cubic)
      {
          mbin = MisorientationCalculations::getMisoBinCubic(m_Grains[i]->misorientationlist->at(3*j), m_Grains[i]->misorientationlist->at(3*j+1), m_Grains[i]->misorientationlist->at(3*j+2));
      }
      else if (crystruct == AIM::Reconstruction::Hexagonal)
      {
          mbin = MisorientationCalculations::getMisoBinHexagonal(m_Grains[i]->misorientationlist->at(3*j), m_Grains[i]->misorientationlist->at(3*j+1), m_Grains[i]->misorientationlist->at(3*j+2));
      }
      if (w < 0.261799)
      {
          microcount++;
      }
      if (nname > i || m_Grains[nname]->surfacegrain == 1)
      {
      nsa = m_Grains[i]->neighborsurfarealist->at(j);
          misobin[mbin] = misobin[mbin] + (nsa / totalsurfacearea);
      }
    }
  if (m_Grains[i]->neighborlist->size() > 0)
  {
      int micbin = int((double(microcount) / double(m_Grains[i]->neighborlist->size())) / 0.1);
      microbin[micbin]++;
    }
  }
  h5io->writeMisorientationBinsData(misobin, nummisobins);
  h5io->writeMicroTextureData(microbin, 10, numgrains);
}

void ReconstructionFunc::find_colors()
{
  size_t numgrains = m_Grains.size();
  //  double red, green, blue;
  unsigned char rgb[3] =
  { 0, 0, 0 };
  double RefDirection[3] =
  { 0.0, 0.0, 1.0 };
  for (int i = 1; i < numgrains; i++)
  {
    if (m_Grains[i]->active == 1)
    {
      double g1ea1 = m_Grains[i]->euler1;
      double g1ea2 = m_Grains[i]->euler2;
      double g1ea3 = m_Grains[i]->euler3;
      double q1[4];

      // Create a Unit Quaterion based on the Euler Angles
      q1[0] = sin((g1ea2 / 2.0)) * cos(((g1ea1 - g1ea3) / 2.0));
      q1[1] = sin((g1ea2 / 2.0)) * sin(((g1ea1 - g1ea3) / 2.0));
      q1[2] = cos((g1ea2 / 2.0)) * sin(((g1ea1 + g1ea3) / 2.0));
      q1[3] = cos((g1ea2 / 2.0)) * cos(((g1ea1 + g1ea3) / 2.0));
      if (crystruct == AIM::Reconstruction::Cubic)
      {
        OIMColoring::GenerateIPFColor(g1ea1, g1ea2, g1ea3, RefDirection[0], RefDirection[1], RefDirection[2], rgb);
        m_Grains[i]->red = static_cast<double > (rgb[0] / 255.0);
        m_Grains[i]->green = static_cast<double > (rgb[1] / 255.0);
        ;
        m_Grains[i]->blue = static_cast<double > (rgb[2] / 255.0);
        ;
      }
      if (crystruct == AIM::Reconstruction::Hexagonal)
      {
        OIMColoring::CalculateHexIPFColor(q1, rgb);
        m_Grains[i]->red = rgb[0] / 255.0;
        m_Grains[i]->green = rgb[1] / 255.0;
        m_Grains[i]->blue = rgb[2] / 255.0;
      }
    }
  }
}
void ReconstructionFunc::find_schmids()
{
  double schmid = 0;
  double loadx, loady, loadz;
  double theta1, theta2, theta3, theta4;
  double lambda1, lambda2, lambda3, lambda4, lambda5, lambda6;
  double schmid1, schmid2, schmid3, schmid4, schmid5, schmid6, schmid7, schmid8, schmid9, schmid10, schmid11, schmid12;
  size_t numgrains = m_Grains.size();
  for (int i = 1; i < numgrains; i++)
  {
    if (m_Grains[i]->active == 1)
    {
      double g1ea1 = m_Grains[i]->euler1;
      double g1ea2 = m_Grains[i]->euler2;
      double g1ea3 = m_Grains[i]->euler3;
      double q1[4];
//      q1[0] = sin((g1ea2 / 2.0)) * cos(((g1ea1 - g1ea3) / 2.0));
//      q1[1] = sin((g1ea2 / 2.0)) * sin(((g1ea1 - g1ea3) / 2.0));
//      q1[2] = cos((g1ea2 / 2.0)) * sin(((g1ea1 + g1ea3) / 2.0));
//      q1[3] = cos((g1ea2 / 2.0)) * cos(((g1ea1 + g1ea3) / 2.0));
    q1[0] = m_Grains[i]->avg_quat[1]/m_Grains[i]->avg_quat[0];
    q1[1] = m_Grains[i]->avg_quat[2]/m_Grains[i]->avg_quat[0];
    q1[2] = m_Grains[i]->avg_quat[3]/m_Grains[i]->avg_quat[0];
    q1[3] = m_Grains[i]->avg_quat[4]/m_Grains[i]->avg_quat[0];
      loadx = (2 * q1[0] * q1[2] + 2 * q1[1] * q1[3]) * 1;
      loady = (2 * q1[1] * q1[2] + 2 * q1[0] * q1[3]) * 1;
      loadz = (1 - 2 * q1[0] * q1[0] - 2 * q1[1] * q1[1]) * 1;
      double mag = loadx * loadx + loady * loady + loadz * loadz;
      mag = pow(mag, 0.5);
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
      schmid = schmid1;
      if (schmid2 > schmid) schmid = schmid2;
      if (schmid3 > schmid) schmid = schmid3;
      if (schmid4 > schmid) schmid = schmid4;
      if (schmid5 > schmid) schmid = schmid5;
      if (schmid6 > schmid) schmid = schmid6;
      if (schmid7 > schmid) schmid = schmid7;
      if (schmid8 > schmid) schmid = schmid8;
      if (schmid9 > schmid) schmid = schmid9;
      if (schmid10 > schmid) schmid = schmid10;
      if (schmid11 > schmid) schmid = schmid11;
      if (schmid12 > schmid) schmid = schmid12;
      m_Grains[i]->schmidfactor = schmid;
    }
  }
}

int ReconstructionFunc::volume_stats(H5ReconStatsWriter::Pointer h5io)
{
  int retErr = 0;
  double actualgrains = 0;
  double avgvol = 0;
  double avglnvol = 0;
  double avgbovera = 0;
  double avgcovera = 0;
  double avgcoverb = 0;
  double avgdiam = 0;
  double avglogdiam = 0;
  double avgdiam2 = 0;
  double avgschmid = 0;
  double avgomega3 = 0;
  double neighcount = 0;
  double maxvol = 0;
  double maxdiam = 0;
  double maxlogdiam = 0;
  double maxbovera = 0;
  double maxcovera = 0;
  double maxcoverb = 0;
  double maxschmid = 0;
  double maxomega3 = 0;
  size_t numgrains = m_Grains.size();
  int numbins = int((maxdiameter - mindiameter) / sizebinstepsize) + 1;
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
  for (int i = 1; i < numgrains; i++)
  {
    int onedge = m_Grains[i]->surfacegrain;
    if (onedge == 0 && m_Grains[i]->active == 1)
    {
      actualgrains++;
      int vol = m_Grains[i]->numvoxels;
      double voxvol = vol * resx * resy * resz;
      double logvol = log(voxvol);
      double diam = m_Grains[i]->equivdiameter;
      double logdiam = log(diam);
      double I1 = m_Grains[i]->radius1;
      double I2 = m_Grains[i]->radius2;
      double I3 = m_Grains[i]->radius3;
      I1 = (15 * I1) / (4 * m_pi);
      I2 = (15 * I2) / (4 * m_pi);
      I3 = (15 * I3) / (4 * m_pi);
      double A = (I1 + I2 - I3) / 2;
      double B = (I1 + I3 - I2) / 2;
      double C = (I2 + I3 - I1) / 2;
      double a = (A * A * A * A) / (B * C);
      a = pow(a, 0.1);
      double b = B / A;
      b = pow(b, 0.5) * a;
      double c = A / (a * a * a * b);
      double bovera = b / a;
      double covera = c / a;
      double coverb = c / b;
      double schmid = m_Grains[i]->schmidfactor;
      double omega3 = m_Grains[i]->omega3;
      avgvol = avgvol + voxvol;
      avglnvol = avglnvol + logvol;
      avgbovera = avgbovera + bovera;
      avgcovera = avgcovera + covera;
      avgcoverb = avgcoverb + coverb;
      avgdiam = avgdiam + diam;
      avglogdiam = avglogdiam + logdiam;
      avgschmid = avgschmid + schmid;
      avgomega3 = avgomega3 + omega3;
      int diamint = int((diam - mindiameter) / sizebinstepsize);
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
      if (voxvol > maxvol) maxvol = voxvol;
      if (bovera > maxbovera) maxbovera = bovera;
      if (covera > maxcovera) maxcovera = covera;
      if (coverb > maxcoverb) maxcoverb = coverb;
      if (diam > maxdiam) maxdiam = diam;
      if (logdiam > maxlogdiam) maxlogdiam = logdiam;
      if (schmid > maxschmid) maxschmid = schmid;
      if (omega3 > maxomega3) maxomega3 = omega3;
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
  avgvol = avgvol / actualgrains;
  avglnvol = avglnvol / actualgrains;
  avgbovera = avgbovera / actualgrains;
  avgcovera = avgcovera / actualgrains;
  avgcoverb = avgcoverb / actualgrains;
  avgdiam = avgdiam / actualgrains;
  avglogdiam = avglogdiam / actualgrains;
  avgdiam2 = avgdiam2 / neighcount;
  avgschmid = avgschmid / actualgrains;
  avgomega3 = avgomega3 / actualgrains;
  maxvol = maxvol / avgvol;
  double sdvol = 0;
  double sdlnvol = 0;
  double sdbovera = 0;
  double sdcovera = 0;
  double sdcoverb = 0;
  double sddiam = 0;
  double sdlogdiam = 0;
  double sddiam2 = 0;
  double sdschmid = 0;
  double sdomega3 = 0;
  for (int j = 1; j < numgrains; j++)
  {
    int onedge = m_Grains[j]->surfacegrain;
    if (onedge == 0 && m_Grains[j]->active == 1)
    {
      int vol = m_Grains[j]->numvoxels;
      double voxvol = vol * resx * resy * resz;
      double logvol = log(voxvol);
      double rad_3 = 0.75 * (1 / m_pi) * voxvol;
      double diam = 2 * pow(rad_3, 0.333333333);
      double logdiam = log(diam);
      double I1 = m_Grains[j]->radius1;
      double I2 = m_Grains[j]->radius2;
      double I3 = m_Grains[j]->radius3;
      I1 = (15 * I1) / (4 * m_pi);
      I2 = (15 * I2) / (4 * m_pi);
      I3 = (15 * I3) / (4 * m_pi);
      double A = (I1 + I2 - I3) / 2;
      double B = (I1 + I3 - I2) / 2;
      double C = (I2 + I3 - I1) / 2;
      double a = (A * A * A * A) / (B * C);
      a = pow(a, 0.1);
      double b = B / A;
      b = pow(b, 0.5) * a;
      double c = A / (a * a * a * b);
      double bovera = b / a;
      double covera = c / a;
      double coverb = c / b;
      double schmid = m_Grains[j]->schmidfactor;
      double omega3 = m_Grains[j]->omega3;
      sdvol = sdvol + ((voxvol - avgvol) * (voxvol - avgvol));
      sdlnvol = sdlnvol + ((logvol - avglnvol) * (logvol - avglnvol));
      sdbovera = sdbovera + ((bovera - avgbovera) * (bovera - avgbovera));
      sdcovera = sdcovera + ((covera - avgcovera) * (covera - avgcovera));
      sdcoverb = sdcoverb + ((coverb - avgcoverb) * (coverb - avgcoverb));
      sddiam = sddiam + ((diam - avgdiam) * (diam - avgdiam));
      sdlogdiam = sdlogdiam + ((logdiam - avglogdiam) * (logdiam - avglogdiam));
      sdschmid = sdschmid + ((schmid - avgschmid) * (schmid - avgschmid));
      sdomega3 = sdomega3 + ((omega3 - avgomega3) * (omega3 - avgomega3));
      int diamint = int((diam - mindiameter) / sizebinstepsize);
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
      neighborhood[temp4][2] = pow(neighborhood[temp4][2], 0.5);
      neighborhood[temp4][4] = pow(neighborhood[temp4][4], 0.5);
      neighborhood[temp4][6] = pow(neighborhood[temp4][6], 0.5);
      svbovera[temp4][2] = pow(svbovera[temp4][2], 0.5);
      svcovera[temp4][2] = pow(svcovera[temp4][2], 0.5);
      svcoverb[temp4][2] = pow(svcoverb[temp4][2], 0.5);
      svschmid[temp4][2] = pow(svschmid[temp4][2], 0.5);
      svomega3[temp4][2] = pow(svomega3[temp4][2], 0.5);
    }
  }
  sdvol = sdvol / actualgrains;
  sdlnvol = sdlnvol / actualgrains;
  sdbovera = sdbovera / actualgrains;
  sdcovera = sdcovera / actualgrains;
  sdcoverb = sdcoverb / actualgrains;
  sddiam = sddiam / actualgrains;
  sdlogdiam = sdlogdiam / actualgrains;
  sddiam2 = sddiam2 / neighcount;
  sdschmid = sdschmid / actualgrains;
  sdomega3 = sdomega3 / actualgrains;
  //  double volvar = sdvol;
  //  double vollnvar = sdlnvol;
  double boveravar = sdbovera;
  double coveravar = sdcovera;
  double coverbvar = sdcoverb;
  //  double diamvar = sddiam;
  //  double logdiamvar = sdlogdiam;
  //  double diamvar2 = sddiam2;
  double schmidvar = sdschmid;
  double omega3var = sdomega3;
  //  double pbovera = avgbovera*(((avgbovera*(1-avgbovera))/boveravar)-1);
  //  double qbovera = (1-avgbovera)*(((avgbovera*(1-avgbovera))/boveravar)-1);
  //  double pcovera = avgcovera*(((avgcovera*(1-avgcovera))/coveravar)-1);
  //  double qcovera = (1-avgcovera)*(((avgcovera*(1-avgcovera))/coveravar)-1);
  //  double pcoverb = avgcoverb*(((avgcoverb*(1-avgcoverb))/coverbvar)-1);
  //  double qcoverb = (1-avgcoverb)*(((avgcoverb*(1-avgcoverb))/coverbvar)-1);
  sdvol = pow(sdvol, 0.5);
  sdlnvol = pow(sdlnvol, 0.5);
  sdbovera = pow(sdbovera, 0.5);
  sdcovera = pow(sdcovera, 0.5);
  sdcoverb = pow(sdcoverb, 0.5);
  sddiam = pow(sddiam, 0.5);
  sdlogdiam = pow(sdlogdiam, 0.5);
  sddiam2 = pow(sddiam2, 0.5);
  sdschmid = pow(sdschmid, 0.5);
  sdomega3 = pow(sdomega3, 0.5);
  double svboveracr = 0;
  double svcoveracr = 0;
  double svcoverbcr = 0;
  double svschmidcr = 0;
  double svomega3cr = 0;
  for (int temp5 = 0; temp5 < numbins; temp5++)
  {
    svboveracr = svboveracr + (svbovera[temp5][0] * ((svbovera[temp5][1] - avgbovera) * (svbovera[temp5][1] - avgbovera)));
    svcoveracr = svcoveracr + (svcovera[temp5][0] * ((svcovera[temp5][1] - avgcovera) * (svcovera[temp5][1] - avgcovera)));
    svcoverbcr = svcoverbcr + (svcoverb[temp5][0] * ((svcoverb[temp5][1] - avgcoverb) * (svcoverb[temp5][1] - avgcoverb)));
    svschmidcr = svschmidcr + (svschmid[temp5][0] * ((svschmid[temp5][1] - avgschmid) * (svschmid[temp5][1] - avgschmid)));
    svomega3cr = svomega3cr + (svomega3[temp5][0] * ((svomega3[temp5][1] - avgomega3) * (svomega3[temp5][1] - avgomega3)));
  }
  svboveracr = svboveracr / (actualgrains * boveravar);
  svcoveracr = svcoveracr / (actualgrains * coveravar);
  svcoverbcr = svcoverbcr / (actualgrains * coverbvar);
  svschmidcr = svschmidcr / (actualgrains * schmidvar);
  svomega3cr = svomega3cr / (actualgrains * omega3var);

  retErr = h5io->writeVolumeStats(maxdiameter, mindiameter, 1.0, avglogdiam, sdlogdiam, svbovera, svcovera, svcoverb, neighborhoodfit, svomega3);

  return retErr;
}

int ReconstructionFunc::volume_stats2D(H5ReconStatsWriter::Pointer h5io)
{
  int retErr = 0;
  double actualgrains = 0;
  double avgvol = 0;
  double avglnvol = 0;
  double avgbovera = 0;
  //  double avgcovera = 0;
  //  double avgcoverb = 0;
  double avgdiam = 0;
  double avglogdiam = 0;
  double avgdiam2 = 0;
  double avgschmid = 0;
  //  double avgomega3 = 0;
  double neighcount = 0;
  double maxvol = 0;
  double maxdiam = 0;
  double maxlogdiam = 0;
  double maxbovera = 0;
  //  double maxcovera = 0;
  //  double maxcoverb = 0;
  double maxschmid = 0;
  //  double maxomega3 = 0;
  int neighdistfunc[3];
  int numbins = int((maxdiameter - mindiameter) / sizebinstepsize) + 1;
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
    svcovera[temp].resize(5, 0);
    svcoverb[temp].resize(5, 0);
    svschmid[temp].resize(5, 0);
    svomega3[temp].resize(5, 0);
  }
  for (int i = 1; i < numgrains; i++)
  {
    int onedge = m_Grains[i]->surfacegrain;
    if (onedge == 0 && m_Grains[i]->active == 1)
    {
      actualgrains++;
      int vol = m_Grains[i]->numvoxels;
      double voxvol = vol * resx * resy;
      double logvol = log(voxvol);
      double diam = m_Grains[i]->equivdiameter;
      double logdiam = log(diam);
      double rad1 = m_Grains[i]->radius1;
      double rad2 = m_Grains[i]->radius2;
      double bovera = rad2 / rad1;
      double schmid = m_Grains[i]->schmidfactor;
      //    double omega3 = m_Grains[i]->omega3;
      avgvol = avgvol + voxvol;
      avglnvol = avglnvol + logvol;
      avgbovera = avgbovera + bovera;
      avgdiam = avgdiam + diam;
      avglogdiam = avglogdiam + logdiam;
      avgschmid = avgschmid + schmid;
      //      avgomega3 = avgomega3+omega3;
      int diamint = int((diam - mindiameter) / sizebinstepsize);
      neighborhood[diamint][0]++;
      svbovera[diamint][0]++;
      svschmid[diamint][0]++;
      svomega3[diamint][0]++;
      svbovera[diamint][1] = svbovera[diamint][1] + bovera;
      svschmid[diamint][1] = svschmid[diamint][1] + schmid;
      //      svomega3[diamint][1] = svomega3[diamint][1] + omega3;
      neighborhood[diamint][0]++;
      for (int k = 0; k < 3; k++)
      {
        int nnum = neighdistfunc[k];
        neighborhood[diamint][((2 * k) + 1)] = neighborhood[diamint][((2 * k) + 1)] + nnum;
      }
      if (voxvol > maxvol) maxvol = voxvol;
      if (bovera > maxbovera) maxbovera = bovera;
      if (diam > maxdiam) maxdiam = diam;
      if (logdiam > maxlogdiam) maxlogdiam = logdiam;
      if (schmid > maxschmid) maxschmid = schmid;
      //      if(omega3 > maxomega3) maxomega3 = omega3;
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
      //      svomega3[temp3][1] = svomega3[temp3][1]/svomega3[temp3][0];
      neighborhoodfit[temp3][0] = neighborhood[temp3][0];
      neighborhoodfit[temp3][1] = neighborhood[temp3][3] - neighborhood[temp3][1];
      neighborhoodfit[temp3][2] = neighborhood[temp3][1];
      neighborhoodfit[temp3][3] = log((neighborhood[temp3][5] - neighborhoodfit[temp3][2]) / neighborhoodfit[temp3][1]) / log(2.0);
    }
  }
  avgvol = avgvol / actualgrains;
  avglnvol = avglnvol / actualgrains;
  avgbovera = avgbovera / actualgrains;
  avgdiam = avgdiam / actualgrains;
  avglogdiam = avglogdiam / actualgrains;
  avgdiam2 = avgdiam2 / neighcount;
  avgschmid = avgschmid / actualgrains;
  //  avgomega3 = avgomega3/actualgrains;
  maxvol = maxvol / avgvol;
  double sdvol = 0;
  double sdlnvol = 0;
  double sdbovera = 0;
  double sddiam = 0;
  double sdlogdiam = 0;
  double sddiam2 = 0;
  double sdschmid = 0;
  //  double sdomega3 = 0;
  for (int j = 1; j < numgrains; j++)
  {
    int onedge = m_Grains[j]->surfacegrain;
    if (onedge == 0 && m_Grains[j]->active == 1)
    {
      int vol = m_Grains[j]->numvoxels;
      double voxvol = vol * resx * resy * resz;
      double logvol = log(voxvol);
      double diam = m_Grains[j]->equivdiameter;
      double logdiam = log(diam);
      double rad1 = m_Grains[j]->radius1;
      double rad2 = m_Grains[j]->radius2;
      double bovera = rad2 / rad1;
      double schmid = m_Grains[j]->schmidfactor;
      //    double omega3 = m_Grains[j]->omega3;
      sdvol = sdvol + ((voxvol - avgvol) * (voxvol - avgvol));
      sdlnvol = sdlnvol + ((logvol - avglnvol) * (logvol - avglnvol));
      sdbovera = sdbovera + ((bovera - avgbovera) * (bovera - avgbovera));
      sddiam = sddiam + ((diam - avgdiam) * (diam - avgdiam));
      sdlogdiam = sdlogdiam + ((logdiam - avglogdiam) * (logdiam - avglogdiam));
      sdschmid = sdschmid + ((schmid - avgschmid) * (schmid - avgschmid));
      //      sdomega3 = sdomega3 + ((omega3-avgomega3)*(omega3-avgomega3));
      int diamint = int((diam - mindiameter) / sizebinstepsize);
      svbovera[diamint][2] = svbovera[diamint][2] + ((bovera - svbovera[diamint][1]) * (bovera - svbovera[diamint][1]));
      svschmid[diamint][2] = svschmid[diamint][2] + ((schmid - svschmid[diamint][1]) * (schmid - svschmid[diamint][1]));
      //      svomega3[diamint][2] = svomega3[diamint][2] + ((omega3-svomega3[diamint][1])*(omega3-svomega3[diamint][1]));
      for (int k = 0; k < 3; k++)
      {
        int nnum = neighdistfunc[k];
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
      //      svomega3[temp4][2] = svomega3[temp4][2]/svomega3[temp4][0];
      svbovera[temp4][3] = svbovera[temp4][1] * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
      svbovera[temp4][4] = (1 - svbovera[temp4][1]) * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
      //      svomega3[temp4][3] = svomega3[temp4][1]*(((svomega3[temp4][1]*(1-svomega3[temp4][1]))/svomega3[temp4][2])-1);
      //      svomega3[temp4][4] = (1-svomega3[temp4][1])*(((svomega3[temp4][1]*(1-svomega3[temp4][1]))/svomega3[temp4][2])-1);
      neighborhood[temp4][2] = pow(neighborhood[temp4][2], 0.5);
      neighborhood[temp4][4] = pow(neighborhood[temp4][4], 0.5);
      neighborhood[temp4][6] = pow(neighborhood[temp4][6], 0.5);
      svbovera[temp4][2] = pow(svbovera[temp4][2], 0.5);
      svschmid[temp4][2] = pow(svschmid[temp4][2], 0.5);
      //      svomega3[temp4][2] = pow(svomega3[temp4][2],0.5);
    }
  }
  sdvol = sdvol / actualgrains;
  sdlnvol = sdlnvol / actualgrains;
  sdbovera = sdbovera / actualgrains;
  sddiam = sddiam / actualgrains;
  sdlogdiam = sdlogdiam / actualgrains;
  sddiam2 = sddiam2 / neighcount;
  sdschmid = sdschmid / actualgrains;
  //  sdomega3 = sdomega3/actualgrains;
  //  double volvar = sdvol;
  //  double vollnvar = sdlnvol;
  double boveravar = sdbovera;
  //  double diamvar = sddiam;
  //  double logdiamvar = sdlogdiam;
  //  double diamvar2 = sddiam2;
  double schmidvar = sdschmid;
  //  double omega3var = sdomega3;
  //  double pbovera = avgbovera*(((avgbovera*(1-avgbovera))/boveravar)-1);
  //  double qbovera = (1-avgbovera)*(((avgbovera*(1-avgbovera))/boveravar)-1);
  sdvol = pow(sdvol, 0.5);
  sdlnvol = pow(sdlnvol, 0.5);
  sdbovera = pow(sdbovera, 0.5);
  sddiam = pow(sddiam, 0.5);
  sdlogdiam = pow(sdlogdiam, 0.5);
  sddiam2 = pow(sddiam2, 0.5);
  sdschmid = pow(sdschmid, 0.5);
  //  sdomega3 = pow(sdomega3,0.5);
  double svboveracr = 0;
  double svschmidcr = 0;
  //  double svomega3cr = 0;
  for (int temp5 = 0; temp5 < numbins; temp5++)
  {
    svboveracr = svboveracr + (svbovera[temp5][0] * ((svbovera[temp5][1] - avgbovera) * (svbovera[temp5][1] - avgbovera)));
    svschmidcr = svschmidcr + (svschmid[temp5][0] * ((svschmid[temp5][1] - avgschmid) * (svschmid[temp5][1] - avgschmid)));
    //    svomega3cr = svomega3cr + (svomega3[temp5][0]*((svomega3[temp5][1]-avgomega3)*(svomega3[temp5][1]-avgomega3)));
  }
  svboveracr = svboveracr / (actualgrains * boveravar);
  svschmidcr = svschmidcr / (actualgrains * schmidvar);
  //  svomega3cr = svomega3cr/(actualgrains*omega3var);


  retErr = h5io->writeVolumeStats(maxdiameter, mindiameter, 1.0, avglogdiam, sdlogdiam, svbovera, svcovera, svcoverb, neighborhoodfit, svomega3);

  return retErr;
}

void ReconstructionFunc::deformation_stats(const std::string &filename)
{
  ofstream outFile;
  outFile.open(filename.c_str());
  double avgkm = 0;
  double avggam = 0;
  double avgiq = 0;
  double avggbdist = 0;
  double avgtjdist = 0;
  double avgqpdist = 0;
  double avgsf = 0;
  double avgsfmm = 0;
  double km, iq, gbdist, tjdist, qpdist, sf, sf2, sfmm, gam;
  int nearestneighbor, gname, gname2;
  double gbvkm[20][2];
  double gbvgam[20][2];
  double tjvkm[20][2];
  double tjvgam[20][2];
  double qpvkm[20][2];
  double qpvgam[20][2];
  double iqvkm[20][2];
  double iqvgam[20][2];
  double sfvkm[20][2];
  double sfvgam[20][2];
  double sfmmvkm[20][2];
  double sfmmvgam[20][2];
  double kmvgb[20][2];
  double gamvgb[20][2];
  double kmvtj[20][2];
  double gamvtj[20][2];
  double kmvqp[20][2];
  double gamvqp[20][2];
  double kmvsf[20][2];
  double gamvsf[20][2];
  int kmbin, gambin;
  int gbdistbin, tjdistbin, qpdistbin, sfbin;
  int actualpoints = 0;
  for (int i = 0; i < 20; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      gbvkm[i][j] = 0;
      gbvgam[i][j] = 0;
      tjvkm[i][j] = 0;
      tjvgam[i][j] = 0;
      qpvkm[i][j] = 0;
      qpvgam[i][j] = 0;
      iqvkm[i][j] = 0;
      iqvgam[i][j] = 0;
      sfvkm[i][j] = 0;
      sfvgam[i][j] = 0;
      sfmmvkm[i][j] = 0;
      sfmmvgam[i][j] = 0;
	  kmvgb[i][j] = 0;
	  gamvgb[i][j] = 0;
	  kmvtj[i][j] = 0;
	  gamvtj[i][j] = 0;
	  kmvqp[i][j] = 0;
	  gamvqp[i][j] = 0;
	  kmvsf[i][j] = 0;
	  gamvsf[i][j] = 0;
    }
  }
  for (int i = 0; i < totalpoints; i++)
  {
    if (voxels[i].unassigned != 1)
    {
      km = voxels[i].kernelmisorientation;
      gam = voxels[i].grainmisorientation;
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
      avgkm = avgkm + km;
      avggam = avggam + gam;
      avgiq = avgiq + iq;
      avggbdist = avggbdist + gbdist;
      avgtjdist = avgtjdist + tjdist;
      avgqpdist = avgqpdist + qpdist;
      avgsf = avgsf + sf;
      avgsfmm = avgsfmm + sfmm;
      actualpoints++;
    }
  }
  avgkm = avgkm / double(actualpoints);
  avggam = avggam / double(actualpoints);
  avgiq = avgiq / double(actualpoints);
  avggbdist = avggbdist / double(actualpoints);
  avgtjdist = avgtjdist / double(actualpoints);
  avgqpdist = avgqpdist / double(actualpoints);
  avgsf = avgsf / double(actualpoints);
  avgsfmm = avgsfmm / double(actualpoints);
  for (int i = 0; i < totalpoints; i++)
  {
    if (voxels[i].unassigned != 1)
    {
      km = voxels[i].kernelmisorientation;
      gam = voxels[i].grainmisorientation;
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
      kmbin = int(((km / avgkm)) / 0.2);
      gambin = int(((gam / avggam)) / 0.2);
	  gbdistbin = int(((gbdist / avggbdist)) / 0.2);
	  tjdistbin = int(((tjdist / avgtjdist)) / 0.2);
	  qpdistbin = int(((qpdist / avgqpdist)) / 0.2);
	  sfbin = int((sf-0.25) / 0.05);
      if (kmbin < 0) kmbin = 0;
      if (kmbin > 19) kmbin = 19;
      if (gambin < 0) gambin = 0;
      if (gambin > 19) gambin = 19;
      if (gbdistbin < 0) gbdistbin = 0;
      if (gbdistbin > 19) gbdistbin = 19;
      if (tjdistbin < 0) tjdistbin = 0;
      if (tjdistbin > 19) tjdistbin = 19;
      if (qpdistbin < 0) qpdistbin = 0;
      if (qpdistbin > 19) qpdistbin = 19;
      if (sfbin < 0) sfbin = 0;
      if (sfbin > 19) sfbin = 19;
      gbvkm[kmbin][0]++;
      gbvkm[kmbin][1] = gbvkm[kmbin][1] + gbdist;
      gbvgam[gambin][0]++;
      gbvgam[gambin][1] = gbvgam[gambin][1] + gbdist;
      tjvkm[kmbin][0]++;
      tjvkm[kmbin][1] = tjvkm[kmbin][1] + tjdist;
      tjvgam[gambin][0]++;
      tjvgam[gambin][1] = tjvgam[gambin][1] + tjdist;
      qpvkm[kmbin][0]++;
      qpvkm[kmbin][1] = qpvkm[kmbin][1] + qpdist;
      qpvgam[gambin][0]++;
      qpvgam[gambin][1] = qpvgam[gambin][1] + qpdist;
      iqvkm[kmbin][0]++;
      iqvkm[kmbin][1] = iqvkm[kmbin][1] + iq;
      iqvgam[gambin][0]++;
      iqvgam[gambin][1] = iqvgam[gambin][1] + iq;
      sfvkm[kmbin][0]++;
      sfvkm[kmbin][1] = sfvkm[kmbin][1] + sf;
      sfvgam[gambin][0]++;
      sfvgam[gambin][1] = sfvgam[gambin][1] + sf;
	  if(gbdist/avggbdist < 1)
	  {
	      sfmmvkm[kmbin][0]++;	
	      sfmmvkm[kmbin][1] = sfmmvkm[kmbin][1] + sfmm;
	      sfmmvgam[gambin][0]++;
	      sfmmvgam[gambin][1] = sfmmvgam[gambin][1] + sfmm;
	  }
      kmvgb[gbdistbin][0]++;
      kmvgb[gbdistbin][1] = kmvgb[gbdistbin][1] + km;
      gamvgb[gbdistbin][0]++;
      gamvgb[gbdistbin][1] = gamvgb[gbdistbin][1] + gam;
      kmvtj[tjdistbin][0]++;
      kmvtj[tjdistbin][1] = kmvtj[tjdistbin][1] + km;
      gamvtj[tjdistbin][0]++;
      gamvtj[tjdistbin][1] = gamvtj[tjdistbin][1] + gam;
      kmvqp[qpdistbin][0]++;
      kmvqp[qpdistbin][1] = kmvqp[qpdistbin][1] + km;
      gamvqp[qpdistbin][0]++;
      gamvqp[qpdistbin][1] = gamvqp[qpdistbin][1] + gam;
      kmvsf[sfbin][0]++;
      kmvsf[sfbin][1] = kmvsf[sfbin][1] + km;
      gamvsf[sfbin][0]++;
      gamvsf[sfbin][1] = gamvsf[sfbin][1] + gam;
    }
  }
  outFile << "Kernel Misorientation Data" << endl;
  outFile << "GB    TJ    QP    IQ    SF    SF" << endl;
  for (int i = 0; i < 20; i++)
  {
    if (gbvkm[i][0] > 0) gbvkm[i][1] = gbvkm[i][1] / gbvkm[i][0];
    if (tjvkm[i][0] > 0) tjvkm[i][1] = tjvkm[i][1] / tjvkm[i][0];
    if (qpvkm[i][0] > 0) qpvkm[i][1] = qpvkm[i][1] / qpvkm[i][0];
    if (iqvkm[i][0] > 0) iqvkm[i][1] = iqvkm[i][1] / iqvkm[i][0];
    if (sfvkm[i][0] > 0) sfvkm[i][1] = sfvkm[i][1] / sfvkm[i][0];
    if (sfmmvkm[i][0] > 0) sfmmvkm[i][1] = sfmmvkm[i][1] / sfmmvkm[i][0];
    outFile << gbvkm[i][0] << "	" << gbvkm[i][1] / avggbdist << "	" 
		<< tjvkm[i][0] << "	" << tjvkm[i][1] / avgtjdist << "	" 
		<< qpvkm[i][0] << "	" << qpvkm[i][1] / avgqpdist << "	" 
		<< iqvkm[i][0] << "	" << iqvkm[i][1] / avgiq << "	" 
		<< sfvkm[i][0] << "	" << sfvkm[i][1] << "	" 
		<< sfmmvkm[i][0] << "	" << sfmmvkm[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Grain Average Misorientation Data" << endl;
  outFile << "GB    TJ    QP    IQ    SF    SF" << endl;
  for (int i = 0; i < 20; i++)
  {
    if (gbvgam[i][0] > 0) gbvgam[i][1] = gbvgam[i][1] / gbvgam[i][0];
    if (tjvgam[i][0] > 0) tjvgam[i][1] = tjvgam[i][1] / tjvgam[i][0];
    if (qpvgam[i][0] > 0) qpvgam[i][1] = qpvgam[i][1] / qpvgam[i][0];
    if (iqvgam[i][0] > 0) iqvgam[i][1] = iqvgam[i][1] / iqvgam[i][0];
    if (sfvgam[i][0] > 0) sfvgam[i][1] = sfvgam[i][1] / sfvgam[i][0];
    if (sfmmvgam[i][0] > 0) sfmmvgam[i][1] = sfmmvgam[i][1] / sfmmvgam[i][0];
    outFile << gbvgam[i][0] << "	" << gbvgam[i][1] / avggbdist << "	" 
		<< tjvgam[i][0] << "	" << tjvgam[i][1] / avgtjdist << "	" 
		<< qpvgam[i][0] << "	" << qpvgam[i][1] / avgqpdist << "	" 
		<< iqvgam[i][0] << "	" << iqvgam[i][1] / avgiq << "	" 
		<< sfvgam[i][0] << "	" << sfvgam[i][1] << "	"
        << sfmmvgam[i][0] << "	" << sfmmvgam[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Distance Data" << endl;
  outFile << "GB KAM		TJ KAM		QP KAM		GB GAM		TJ GAM		QP GAM" << endl;
  for (int i = 0; i < 20; i++)
  {
    if (kmvgb[i][0] > 0) kmvgb[i][1] = kmvgb[i][1] / kmvgb[i][0];
    if (gamvgb[i][0] > 0) gamvgb[i][1] = gamvgb[i][1] / gamvgb[i][0];
    if (kmvtj[i][0] > 0) kmvtj[i][1] = kmvtj[i][1] / kmvtj[i][0];
    if (gamvtj[i][0] > 0) gamvtj[i][1] = gamvtj[i][1] / gamvtj[i][0];
    if (kmvqp[i][0] > 0) kmvqp[i][1] = kmvqp[i][1] / kmvqp[i][0];
    if (gamvqp[i][0] > 0) gamvqp[i][1] = gamvqp[i][1] / gamvqp[i][0];
    outFile << kmvgb[i][0] << "	" << kmvgb[i][1] / avgkm << "	"
		<< kmvtj[i][0] << "	" << kmvtj[i][1] / avgkm << "	"
		<< kmvqp[i][0] << "	" << kmvqp[i][1] / avgkm << "	"
		<< gamvgb[i][0] << "	" << gamvgb[i][1] / avggam << "	" 
		<< gamvtj[i][0] << "	" << gamvtj[i][1] / avggam << "	"
        << gamvqp[i][0] << "	" << gamvqp[i][1] / avggam << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Schmid Factor Data" << endl;
  outFile << "SF KAM		SF GAM" << endl;
  for (int i = 0; i < 20; i++)
  {
    if (kmvsf[i][0] > 0) kmvsf[i][1] = kmvsf[i][1] / kmvsf[i][0];
    if (gamvsf[i][0] > 0) gamvsf[i][1] = gamvsf[i][1] / gamvsf[i][0];
    outFile << kmvsf[i][0] << "	" << kmvsf[i][1] / avgkm << "	"
        << gamvsf[i][0] << "	" << gamvsf[i][1] / avggam << endl;
  }
  outFile.close();
}



void ReconstructionFunc::write_graindata(const std::string &graindataFile)
{
  IntVectorType nlist;
  ofstream outFile;
  size_t numgrains = m_Grains.size();
  outFile.open(graindataFile.c_str());
  outFile << numgrains << endl;
  outFile << "Grain ID  Euler1  Euler2  Euler3  Equiv. Diameter Grain Avg. Disorientation Surface Grain Schmid Factor No. Neighbors" << endl;
  for (int i = 1; i < numgrains; i++)
  {
    double diameter = m_Grains[i]->equivdiameter;
    int onsurface = m_Grains[i]->surfacegrain;
    double avgmiso = m_Grains[i]->averagemisorientation;
    double schmid = m_Grains[i]->schmidfactor;
    nlist = m_Grains[i]->neighborlist;
    int nucleus = m_Grains[i]->nucleus;
    outFile << i << " " << voxels[nucleus].euler1 << "  " << voxels[nucleus].euler2 << "  " << voxels[nucleus].euler3 << "  " << diameter << "  " << avgmiso << " "
        << onsurface << " " << schmid << "  " << nlist->size() << " ";
    outFile << endl;
  }
  outFile.close();

}

void ReconstructionFunc::write_grains(const std::string &outputdir)
{
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');

  std::ofstream outFile;

  vector<int >* vlist = NULL;
  vector<int > plist(((xpoints + 1) * (ypoints + 1) * (zpoints + 1)), 0);
  int pcount = 0;
  int ocol, orow, oplane;
  int col, row, plane;
  int vid, pid;
  size_t numgrains = m_Grains.size();
  for (int i = 1; i < numgrains; i++)
  {
    ss.str("");
    ss << outputdir << MXADir::Separator << "Grain_" << std::setw(5) << i << ".vtk";

    outFile.open(ss.str().c_str(), std::ios::trunc);
    outFile << "# vtk DataFile Version 2.0" << endl;
    outFile << "data set from FFT2dx_GB" << endl;
    outFile << "ASCII" << endl;
    outFile << "DATASET UNSTRUCTURED_GRID" << endl;
    if (NULL == m_Grains[i]->voxellist)
    {
      m_Grains[i]->voxellist = new std::vector<int >;
    }
    vlist = m_Grains[i]->voxellist;
    pcount = 0;
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      ocol = vid % xpoints;
      orow = (vid / xpoints) % ypoints;
      oplane = vid / (xpoints * ypoints);
      for (int k = 0; k < 8; k++)
      {
        if (k == 0) col = ocol, row = orow, plane = oplane;
        if (k == 1) col = ocol + 1, row = orow, plane = oplane;
        if (k == 2) col = ocol, row = orow + 1, plane = oplane;
        if (k == 3) col = ocol + 1, row = orow + 1, plane = oplane;
        if (k == 4) col = ocol, row = orow, plane = oplane + 1;
        if (k == 5) col = ocol + 1, row = orow, plane = oplane + 1;
        if (k == 6) col = ocol, row = orow + 1, plane = oplane + 1;
        if (k == 7) col = ocol + 1, row = orow + 1, plane = oplane + 1;
        pid = (plane * (xpoints + 1) * (ypoints + 1)) + (row * (xpoints + 1)) + col;
        if (plist[pid] == 0)
        {
          plist[pid] = pcount;
          pcount++;
        }
      }
    }
    outFile << "POINTS " << pcount << " float" << endl;
    pcount = 0;
    plist.clear();
    plist.resize(((xpoints + 1) * (ypoints + 1) * (zpoints + 1)), 0);
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      ocol = vid % xpoints;
      orow = (vid / xpoints) % ypoints;
      oplane = vid / (xpoints * ypoints);
      for (int k = 0; k < 8; k++)
      {
        if (k == 0) col = ocol, row = orow, plane = oplane;
        if (k == 1) col = ocol + 1, row = orow, plane = oplane;
        if (k == 2) col = ocol, row = orow + 1, plane = oplane;
        if (k == 3) col = ocol + 1, row = orow + 1, plane = oplane;
        if (k == 4) col = ocol, row = orow, plane = oplane + 1;
        if (k == 5) col = ocol + 1, row = orow, plane = oplane + 1;
        if (k == 6) col = ocol, row = orow + 1, plane = oplane + 1;
        if (k == 7) col = ocol + 1, row = orow + 1, plane = oplane + 1;
        pid = (plane * (xpoints + 1) * (ypoints + 1)) + (row * (xpoints + 1)) + col;
        if (plist[pid] == 0)
        {
          plist[pid] = pcount;
          pcount++;
          outFile << (col * resx) << "  " << (row * resy) << "  " << (plane * resz) << endl;
        }
      }
    }
    outFile << endl;
    outFile << "CELLS " << vlist->size() << " " << vlist->size() * 9 << endl;
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      ocol = vid % xpoints;
      orow = (vid / xpoints) % ypoints;
      oplane = vid / (xpoints * ypoints);
      outFile << "8 ";
      for (int k = 0; k < 8; k++)
      {
        if (k == 0) col = ocol, row = orow, plane = oplane;
        if (k == 1) col = ocol + 1, row = orow, plane = oplane;
        if (k == 2) col = ocol, row = orow + 1, plane = oplane;
        if (k == 3) col = ocol + 1, row = orow + 1, plane = oplane;
        if (k == 4) col = ocol, row = orow, plane = oplane + 1;
        if (k == 5) col = ocol + 1, row = orow, plane = oplane + 1;
        if (k == 6) col = ocol, row = orow + 1, plane = oplane + 1;
        if (k == 7) col = ocol + 1, row = orow + 1, plane = oplane + 1;
        pid = (plane * (xpoints + 1) * (ypoints + 1)) + (row * (xpoints + 1)) + col;
        outFile << plist[pid] << "  ";
      }
      outFile << endl;
    }
    outFile << endl;
    outFile << "CELL_TYPES " << vlist->size() << endl;
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      outFile << "11" << endl;
    }
    outFile << endl;
    outFile << "CELL_DATA " << vlist->size() << endl;
    outFile << "SCALARS GrainID int" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].grainname << endl;
    }
    outFile << endl;
    outFile << "SCALARS KernelAvgDisorientation float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].kernelmisorientation << endl;
    }
    outFile << endl;
    outFile << "SCALARS GrainAvgDisorientation float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].grainmisorientation << endl;
    }
    outFile << endl;
    outFile << "SCALARS ImageQuality float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].imagequality << endl;
    }
    outFile << endl;
    outFile << "SCALARS SchmidFactor float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      outFile << m_Grains[i]->schmidfactor << endl;
    }
    outFile.close();
    plist.clear();
    plist.resize(((xpoints + 1) * (ypoints + 1) * (zpoints + 1)), 0);
  }
}

double ReconstructionFunc::gamma(double x)
{
  int i, k, m;
  double ga, gr, r, z;

  static double g[] =
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
