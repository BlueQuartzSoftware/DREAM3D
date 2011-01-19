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

#include <stdio.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

#include "MXA/Utilities/MXADir.h"
#include "AIM/ANG/AngReader.h"
#include "AIM/Common/Constants.h"
#include "AIM/Common/OIMColoring.hpp"
#include "AIM/Common/Quaternions.h"
#include "AIM/Common/MisorientationCalculations.h"
#include "AIM/Common/HDF5/AIM_H5VtkDataWriter.h"

#if 0
// -i C:\Users\GroebeMA\Desktop\NewFolder --outputDir C:\Users\GroebeMA\Desktop\NewFolder -f Slice_ --angMaxSlice 400 -s 1 -e 30 -z 0.25 -t -g 10 -c 0.1 -o 5.0 -x 2
#endif


const static double m_pi = 3.1415926535897;
const static double m_OnePointThree = 1.33333333333;

using namespace std;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionFunc::ReconstructionFunc() :
  voxels(NULL),
  m_Grains(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionFunc::~ReconstructionFunc()
{

  delete [] voxels;
  m_Grains.clear();
  delete [] graincenters;
  delete [] grainmoments;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionFunc::initialize(int nX, int nY, int nZ,
                                    double xRes, double yRes, double zRes,
                                    bool v_mergetwinsoption, bool v_mergecoloniesoption,
                                    int v_minallowedgrainsize, double v_minseedconfidence,
                                    double v_downsamplefactor, double v_minseedimagequality,
                                    double v_misorientationtolerance,
                                    AIM::Reconstruction::CrystalStructure v_crystruct,
                                    int v_alignmeth, bool v_alreadyformed)
{

  mergetwinsoption = (v_mergetwinsoption == true) ? 1 : 0;
  mergecoloniesoption = (v_mergecoloniesoption == true) ? 1 : 0;
  minallowedgrainsize = v_minallowedgrainsize;
  minseedconfidence = v_minseedconfidence;
  downsamplefactor = v_downsamplefactor;
  minseedimagequality = v_minseedimagequality;
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

  sizex = (xpoints - 1 ) * resx;
  sizey = (ypoints - 1 ) * resy;
  sizez = (zpoints - 1 ) * resz;

  totalpoints = xpoints * ypoints * zpoints;

  m_Grains.resize(100, Grain());
  voxels = new Voxel[totalpoints];
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
	neighbors[0] = -(xpoints*ypoints);
	neighbors[1] = -xpoints;
	neighbors[2] = -1;
	neighbors[3] = 1;
	neighbors[4] = xpoints;
	neighbors[5] = (xpoints*ypoints);
	while(count != 0)
	{
	    count = 0;
	    for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
	    {
		  if(voxels[i].grainname == -1 && voxels[i].confidence < minseedconfidence)
	      {
		    x = i%xpoints;
			y = (i/xpoints)%ypoints;
		    z = i/(xpoints*ypoints);
			bestneighborconfidence = minseedconfidence;
			for(int j=0;j<6;j++)
			{
				good = 1;
				neighpoint = i+neighbors[j];
			    if(j == 0 && z == 0) good = 0;
			    if(j == 5 && z == (zpoints-1)) good = 0;
			    if(j == 1 && y == 0) good = 0;
			    if(j == 4 && y == (ypoints-1)) good = 0;
			    if(j == 2 && x == 0) good = 0;
			    if(j == 3 && x == (xpoints-1)) good = 0;
				if(good == 1)
		        {
					confidence = voxels[neighpoint].confidence;
					if(confidence > bestneighborconfidence)
					{
						count++;
						bestneighborconfidence = confidence;
						bestneighbor = neighpoint;
					}
				}
		     }
	         if(bestneighborconfidence > minseedconfidence)
	         {
	           voxels[i].neighbor = bestneighbor;
	         }
	      }
	    }
	    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
	    {
			bestneighbor = voxels[j].neighbor;
			if(bestneighbor >= 0 && voxels[j].confidence < minseedconfidence)
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
  std::vector<int> voxelslist(initialVoxelsListSize, -1);
  if(alignmeth == 3)
  {
	  for(int iter=0;iter<(xpoints*ypoints*zpoints);iter++)
	  {
		voxels[iter].grainname = 0;
	  }
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    if (voxels[j].imagequality > minseedimagequality )
	{
	    count = 0;
	    goodcount = 0;
		col = j%xpoints;
		row = (j/xpoints)%ypoints;
		plane = j/(xpoints*ypoints);
		for(int i = 0; i < 6; i++)
		{
			good = 1;
			neighbor = j+neighbors[i];
			if (i == 0 && plane == 0) good = 0;
			if (i == 5 && plane == (zpoints - 1)) good = 0;
			if (i == 1 && row == 0) good = 0;
			if (i == 4 && row == (ypoints - 1)) good = 0;
			if (i == 2 && col == 0) good = 0;
			if (i == 3 && col == (xpoints - 1)) good = 0;
			if(good == 1)
			{
				count++;
				if (voxels[neighbor].imagequality > minseedimagequality) goodcount++;
			}
		}
    }
	if(goodcount == count && goodcount > 0)
	{
      voxels[j].grainname = -1;
      voxelslist[size] = j;
      size++;
      if (size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
	}
  }
  for(size_t j = 0; j < size; j++)
  {
	currentpoint = voxelslist[j];
	col = currentpoint%xpoints;
	row = (currentpoint/xpoints)%ypoints;
	plane = currentpoint/(xpoints*ypoints);
    q1[0] = 0;
    q1[1] = voxels[currentpoint].quat[1];
    q1[2] = voxels[currentpoint].quat[2];
    q1[3] = voxels[currentpoint].quat[3];
    q1[4] = voxels[currentpoint].quat[4];
    for(int i = 1; i < 6; i++)
    {
	  good = 1;
	  neighbor = currentpoint+neighbors[i];
      if (i == 0 && plane == 0) good = 0;
      if (i == 5 && plane == (zpoints - 1)) good = 0;
      if (i == 1 && row == 0) good = 0;
      if (i == 4 && row == (ypoints - 1)) good = 0;
      if (i == 2 && col == 0) good = 0;
      if (i == 3 && col == (xpoints - 1)) good = 0;
	  if(good == 1 && voxels[neighbor].grainname == 0)
      {
         q2[0] = 0;
         q2[1] = voxels[neighbor].quat[1];
         q2[2] = voxels[neighbor].quat[2];
         q2[3] = voxels[neighbor].quat[3];
         q2[4] = voxels[neighbor].quat[4];

	   if(crystruct == AIM::Reconstruction::Hexagonal){
	     w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
	   }
	   if(crystruct == AIM::Reconstruction::Cubic) {
	     w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
	   }
         if (w < misorientationtolerance)
         {
             voxels[neighbor].grainname = -2;
//             voxelslist[size] = neighbor;
//             size++;
//             if (size >= voxelslist.size()) voxelslist.resize(size + initialVoxelsListSize, -1);
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
  int xspot,yspot;
  double w;
  double n1,n2,n3;
  double q1[5];
  double q2[5];
  double refci,curci,refiq,curiq;
  double refxcentroid, refycentroid;
  double curxcentroid, curycentroid;
  int refgnum, curgnum;
  int refposition=0;
  int curposition=0;
//  int curposition1=0;
//  int curposition2=0;
//  int alternatetempposition;
//  int halfshift=0;
  int position;
  int tempposition;


  shifts = new int *[zpoints];
  for(int a=0;a<zpoints;a++)
  {
	shifts[a] = new int [2];
	for(int b=0;b<2;b++)
	{
		shifts[a][b] = 0;
	}
  }
  if(alignmeth == 1)
  {
    refxcentroid = 0;
    refycentroid = 0;
	count = 0;
	slice = (zpoints-1);
	for(int l=0;l<ypoints;l++)
	{
		for(int m=0;m<xpoints;m++)
		{
			refposition = ((slice)*xpoints*ypoints)+(l*xpoints)+m;
			refxcentroid = refxcentroid + (((resx*m)+(resx/2.0))*voxels[refposition].grainname);
			refycentroid = refycentroid + (((resy*l)+(resy/2.0))*voxels[refposition].grainname);
			count = count + voxels[refposition].grainname;
		}
	}
	refxcentroid = refxcentroid/double(count);
	refycentroid = refycentroid/double(count);
  }
  for(int iter=1;iter<zpoints;iter++)
  {
	  outFile << endl;
	  outFile << endl;
	  mindisorientation = 100000000;
	  slice = (zpoints-1)-iter;
	  if(alignmeth == 3)
	  {
		  graincount1 = graincounts[slice];
		  graincount2 = graincounts[slice+1];
		  mutualinfo12 = new double *[graincount1];
		  mutualinfo1 = new double [graincount1];
		  mutualinfo2 = new double [graincount2];
		  for(int a=0;a<graincount1;a++)
		  {
			mutualinfo1[a] = 0;
			mutualinfo12[a] = new double [graincount2];
			for(int b=0;b<graincount2;b++)
			{
				mutualinfo12[a][b] = 0;
				mutualinfo2[b] = 0;
			}
		  }
	  }
	  tempxshift = 0;
	  tempyshift = 0;
	  if(alignmeth == 1)
	  {
		curxcentroid = 0;
		curycentroid = 0;
		count = 0;
		slice = (zpoints-1)-iter;
		for(int l=0;l<ypoints;l++)
		{
			for(int m=0;m<xpoints;m++)
			{
				curposition = ((slice)*xpoints*ypoints)+(l*xpoints)+m;
				curxcentroid = curxcentroid + (((resx*m)+(resx/2.0))*voxels[curposition].grainname);
				curycentroid = curycentroid + (((resy*l)+(resy/2.0))*voxels[curposition].grainname);
				count = count + voxels[curposition].grainname;
			}
		}
		curxcentroid = curxcentroid/double(count);
		curycentroid = curycentroid/double(count);
	  }
	  if(alignmeth >= 2)
	  {
		  for(int a=0;a<2;a++)
		  {
			  if(a == 0) step = 3, nsteps = 3;
			  if(a == 1) step = 1, nsteps = 3;
			  for(int j=-nsteps;j<(nsteps+1);j++)
			  {
				outFile << endl;
				for(int k=-nsteps;k<(nsteps+1);k++)
				{
					disorientation = 0;
					count = 0;
					for(int l=0;l<ypoints;l++)
					{
						for(int m=0;m<xpoints;m++)
						{
							count++;
							if((l+(j*step)+tempyshift) >= 0 && (l+(j*step)+tempyshift) < ypoints && (m+(k*step)+tempxshift) >= 0 && (m+(k*step)+tempxshift) < xpoints)
							{
								refposition = ((slice+1)*xpoints*ypoints)+(l*xpoints)+m;
								curposition = (slice*xpoints*ypoints)+((l+(j*step)+tempyshift)*xpoints)+(m+(k*step)+tempxshift);
								refci = voxels[refposition].confidence;
								curci = voxels[curposition].confidence;
								refiq = voxels[refposition].imagequality;
								curiq = voxels[curposition].imagequality;
								refgnum = voxels[refposition].grainname;
								curgnum = voxels[curposition].grainname;
								if(alignmeth == 3)
								{
									if(curgnum >= 0 && refgnum >= 0)
									{
										mutualinfo12[curgnum][refgnum]++;
										mutualinfo1[curgnum]++;
										mutualinfo2[refgnum]++;
									}
								}
								if(alignmeth == 2)
								{
									if(refiq > minseedimagequality && curiq > minseedimagequality)
									{
										q1[1] = voxels[refposition].quat[1];
										q1[2] = voxels[refposition].quat[2];
										q1[3] = voxels[refposition].quat[3];
										q1[4] = voxels[refposition].quat[4];
										q2[1] = voxels[curposition].quat[1];
										q2[2] = voxels[curposition].quat[2];
										q2[3] = voxels[curposition].quat[3];
										q2[4] = voxels[curposition].quat[4];
								     if(crystruct == AIM::Reconstruction::Hexagonal){
								       w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
								     }
								     if(crystruct == AIM::Reconstruction::Cubic) {
								       w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
								     }
								     if(w > misorientationtolerance) disorientation++;
									}
									if(refiq < minseedimagequality && curiq > minseedimagequality) disorientation++;
									if(refiq > minseedimagequality && curiq < minseedimagequality) disorientation++;
								}
							}
							else
							{
								if(alignmeth == 3)
								{
									mutualinfo12[0][0]++;
									mutualinfo1[0]++;
									mutualinfo2[0]++;
								}
								if(alignmeth == 2) disorientation = disorientation;
							}
						}
					}
					if(alignmeth == 3)
					{
						double ha = 0;
						double hb = 0;
						double hab = 0;
						for(int b=0;b<graincount1;b++)
						{
							mutualinfo1[b] = mutualinfo1[b]/double(count);
							if(mutualinfo1[b] != 0) ha = ha + mutualinfo1[b]*log(mutualinfo1[b]);
						}
						for(int c=0;c<graincount2;c++)
						{
							mutualinfo2[c] = mutualinfo2[c]/double(count);
							if(mutualinfo2[c] != 0) hb = hb + mutualinfo2[c]*log(mutualinfo2[c]);
						}
						for(int b=0;b<graincount1;b++)
						{
							for(int c=0;c<graincount2;c++)
							{
								mutualinfo12[b][c] = mutualinfo12[b][c]/double(count);
								if(mutualinfo12[b][c] != 0) hab = hab + mutualinfo12[b][c]*log(mutualinfo12[b][c]);
								double value = 0;
								if(mutualinfo1[b] > 0 && mutualinfo2[c] > 0) value = (mutualinfo12[b][c]/(mutualinfo1[b]*mutualinfo2[c]));
								if(value != 0) disorientation = disorientation + (mutualinfo12[b][c]*log(value));
							}
						}
						for(int b=0;b<graincount1;b++)
						{
							for(int c=0;c<graincount2;c++)
							{
								mutualinfo12[b][c]=0;
								mutualinfo1[b]=0;
								mutualinfo2[c]=0;
							}
						}
				//		double disorientation2 = ha + hb - hab;
						disorientation = 1.0/disorientation;
					}
					outFile << disorientation << "	";
					if(disorientation < mindisorientation)
					{
						xshift = (k*step)+tempxshift;
						yshift = (j*step)+tempyshift;
						mindisorientation = disorientation;
					}
				}
			 }
			 tempxshift = xshift;
			 tempyshift = yshift;
		  }
		  shifts[iter][0] = shifts[iter-1][0] + xshift;
		  shifts[iter][1] = shifts[iter-1][1] + yshift;
	  }
	  if(alignmeth == 1)
	  {
		xshift = int(((curxcentroid-refxcentroid)/resx)+0.5);
		yshift = int(((curycentroid-refycentroid)/resy)+0.5);
	    shifts[iter][0] = xshift;
	    shifts[iter][1] = yshift;
	  }
  }
  for(int iter=1;iter<zpoints;iter++)
  {
	  slice = (zpoints-1)-iter;
	  for(int l=0;l<ypoints;l++)
	  {
		for(int m=0;m<xpoints;m++)
		{
			if(shifts[iter][1] >= 0) yspot = l;
			if(shifts[iter][0] >= 0) xspot = m;
			if(shifts[iter][1] < 0) yspot = ypoints-1-l;
			if(shifts[iter][0] < 0) xspot = xpoints-1-m;
			position = (slice*xpoints*ypoints)+(yspot*xpoints)+xspot;
			tempposition = (slice*xpoints*ypoints)+((yspot+shifts[iter][1])*xpoints)+(xspot+shifts[iter][0]);
			if((yspot+shifts[iter][1]) >= 0 && (yspot+shifts[iter][1]) <= ypoints-1 && (xspot+shifts[iter][0]) >= 0 && (xspot+shifts[iter][0]) <= xpoints-1)
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
			if((yspot+shifts[iter][1]) < 0 || (yspot+shifts[iter][1]) > ypoints-1 || (xspot+shifts[iter][0]) < 0 || (xspot+shifts[iter][0]) > xpoints-1)
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
void  ReconstructionFunc::form_grains_sections()
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
  std::vector<int> voxelslist(initialVoxelsListSize, -1);
  int neighbors[8];
  neighbors[0] = -xpoints-1;
  neighbors[1] = -xpoints;
  neighbors[2] = -xpoints+1;
  neighbors[3] = -1;
  neighbors[4] = 1;
  neighbors[5] = xpoints-1;
  neighbors[6] = xpoints;
  neighbors[7] = xpoints+1;
  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
  for(int slice=0;slice<zpoints;slice++)
  {
	  graincount = 1;
	  noseeds = 0;
	  while(noseeds == 0)
	  {
		seed = -1;
		randx = int(double(rg.Random())*double(xpoints));
		randy = int(double(rg.Random())*double(ypoints));
		for(int j = 0; j < ypoints; ++j)
		{
			for(int i = 0; i < xpoints; ++i)
			{
			  x = randx+i;
			  y = randy+j;
			  z = slice;
			  if(x > xpoints-1) x = x-xpoints;
			  if(y > ypoints-1) y = y-ypoints;
			  point = (z*xpoints*ypoints)+(y*xpoints)+x;
			  double confidence = voxels[point].confidence;
			  double imagequality = voxels[point].imagequality;
			//  int grainname = voxels[point].grainname;
			  if(confidence > minseedconfidence && imagequality > minseedimagequality && voxels[point].grainname == -1)
			  {
				seed = point;
			  }
			  if(seed > -1) {break;}
			}
			if(seed > -1) {break;}
		}
		if(seed == -1) { noseeds = 1; }
		if(seed >= 0)
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
		  for(size_t j = 0; j < size; ++j)
		  {
			int currentpoint = voxelslist[j];
			col = currentpoint%xpoints;
			row = (currentpoint/xpoints)%ypoints;
			q1[0] = 0;
			q1[1] = voxels[currentpoint].quat[1];
			q1[2] = voxels[currentpoint].quat[2];
			q1[3] = voxels[currentpoint].quat[3];
			q1[4] = voxels[currentpoint].quat[4];
			for(int i = 0; i < 8; i++)
			{
			  good = 1;
			  neighbor = currentpoint+neighbors[i];
			  if((i == 0 || i == 1 || i == 2) && row == 0) good = 0;
			  if((i == 5 || i == 6 || i == 7) && row == (ypoints-1)) good = 0;
			  if((i == 0 || i == 3 || i == 5) && col == 0) good = 0;
			  if((i == 2 || i == 4 || i == 7) && col == (xpoints-1)) good = 0;
			  if(good == 1 && voxels[neighbor].grainname <= 0)
			  {
				q2[0] = 0;
				q2[1] = voxels[neighbor].quat[1];
				q2[2] = voxels[neighbor].quat[2];
				q2[3] = voxels[neighbor].quat[3];
				q2[4] = voxels[neighbor].quat[4];
		     if(crystruct == AIM::Reconstruction::Hexagonal){
		       w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
		     }
		     if(crystruct == AIM::Reconstruction::Cubic) {
		       w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
		     }

		     if(crystruct == AIM::Reconstruction::Hexagonal){
		       w2 = MisorientationCalculations::getMisoQuatHexagonal(qs,q2,n1,n2,n3);
		     }
		     if(crystruct == AIM::Reconstruction::Cubic) {
		       w2 = MisorientationCalculations::getMisoQuatCubic(qs,q2,n1,n2,n3);
		     }

		     if(w < misorientationtolerance)
				{
				  voxels[neighbor].grainname = graincount;
				  voxelslist[size] = neighbor;
				  size++;
				  if (size >= voxelslist.size()) voxelslist.resize(size+initialVoxelsListSize,-1);
				}
			  }
			}
		  }
		  voxelslist.erase(std::remove(voxelslist.begin(),voxelslist.end(),-1),voxelslist.end());
		  graincount++;
		  voxelslist.clear();
		  voxelslist.resize(initialVoxelsListSize,-1);
		}
	  }
	  graincounts[slice] = graincount;
  }
}

int  ReconstructionFunc::form_grains()
{
  int point = 0;
//  int totalsize = 0;
//  int toosmallsize = 0;
  int noseeds = 0;
  //  int checked = 1;
  size_t graincount = 1;
  // int currentpoint;
  int neighbor;
  double q1[5];
  double q2[5];
  double w;
  //double w2;
  double n1;
  double n2;
  double n3;
  int seed = -1;
  int randx = 0;
  int randy = 0;
  int randz = 0;
  int good = 0;
  int x, y, z;
  int col, row, plane;
  size_t size = 0;
  size_t initialVoxelsListSize = 1000;
  std::vector<int > voxelslist(initialVoxelsListSize, -1);
  int neighbors[6];
  neighbors[0] = -(xpoints * ypoints);
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = (xpoints * ypoints);
  rg.RandomInit((static_cast<unsigned int > (time(NULL))));
  while (noseeds == 0)
  {
    seed = -1;
    randx = int(double(rg.Random()) * double(xpoints));
    randy = int(double(rg.Random()) * double(ypoints));
    randz = int(double(rg.Random()) * double(zpoints));
    for (int k = 0; k < zpoints; ++k)
    {
      for (int j = 0; j < ypoints; ++j)
      {
        for (int i = 0; i < xpoints; ++i)
        {
          x = randx + i;
          y = randy + j;
          z = randz + k;
          if (x > xpoints - 1) x = x - xpoints;
          if (y > ypoints - 1) y = y - ypoints;
          if (z > zpoints - 1) z = z - zpoints;
          point = (z * xpoints * ypoints) + (y * xpoints) + x;
          if (voxels[point].confidence > minseedconfidence && voxels[point].imagequality > minseedimagequality && voxels[point].grainname == -1)
          {
            seed = point;
          }
          if (seed > -1) break;
        }
        if (seed > -1) break;
      }
      if (seed > -1) break;
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
      for (size_t j = 0; j < size; ++j)
      {
        int currentpoint = voxelslist[j];
        col = currentpoint % xpoints;
        row = (currentpoint / xpoints) % ypoints;
        plane = currentpoint / (xpoints * ypoints);
        q1[0] = 0;
        q1[1] = voxels[currentpoint].quat[1];
        q1[2] = voxels[currentpoint].quat[2];
        q1[3] = voxels[currentpoint].quat[3];
        q1[4] = voxels[currentpoint].quat[4];
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
          if (good == 1 && voxels[neighbor].grainname == -1)
          {
            q2[0] = 0;
            q2[1] = voxels[neighbor].quat[1];
            q2[2] = voxels[neighbor].quat[2];
            q2[3] = voxels[neighbor].quat[3];
            q2[4] = voxels[neighbor].quat[4];

            if(crystruct == AIM::Reconstruction::Hexagonal){
              w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
            }
            if(crystruct == AIM::Reconstruction::Cubic) {
              w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
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
      size = voxelslist.size();
      if (size >= minallowedgrainsize)
      {
        graincount++;
        voxelslist.clear();
        voxelslist.resize(initialVoxelsListSize, -1);
      }
      if (size < minallowedgrainsize)
      {
        for (size_t b = 0; b < size; b++)
        {
          int index = voxelslist[b];
          voxels[index].grainname = -2;
        }
        voxelslist.clear();
        voxelslist.resize(initialVoxelsListSize, -1);
      }
    }
  }
  m_Grains.resize(graincount);
  return graincount;
}

void  ReconstructionFunc::assign_badpoints()
{
  vector<int> neighs;
  vector<int> remove;
//  int checked = 1;
  int count = 1;
//  int size = 0;
  int good = 1;
  double x, y, z;
//  int point;
  int neighpoint;
//  int col, row, plane;
  int neighbors[6];
  std::vector<int> n(numgrains+1);
//  vector<int>* vlist;
  neighbors[0] = -xpoints*ypoints;
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = xpoints*ypoints;
  while(count != 0)
  {
    count = 0;
    for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
    {
      int grainname = voxels[i].grainname;
      if(grainname <= -1)
      {
		count++;
		voxels[i].unassigned = 1;
        for(int c = 1; c < numgrains; c++)
        {
          n[c] = 0;
        }
	    x = i%xpoints;
		y = (i/xpoints)%ypoints;
	    z = i/(xpoints*ypoints);
		for(int j=0;j<6;j++)
		{
			good = 1;
			neighpoint = i+neighbors[j];
		    if(j == 0 && z == 0) good = 0;
		    if(j == 5 && z == (zpoints-1)) good = 0;
		    if(j == 1 && y == 0) good = 0;
		    if(j == 4 && y == (ypoints-1)) good = 0;
		    if(j == 2 && x == 0) good = 0;
		    if(j == 3 && x == (xpoints-1)) good = 0;
			if(good == 1)
	        {
				int grain = voxels[neighpoint].grainname;
				if(grain >= 0)
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
          voxels[i].neighbor = curgrain;
          neighs.clear();
        }
      }
    }
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int grainname = voxels[j].grainname;
      int neighbor = voxels[j].neighbor;
      if(grainname <= -1 && neighbor >= 0)
      {
        voxels[j].grainname = neighbor;
      }
    }
  }
}
void  ReconstructionFunc::merge_containedgrains()
{
  int containedmerged = 1;
  vector<int> nlistgood;
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    int numneighbors = m_Grains[grainname].numneighbors;
    if(numneighbors == 1)
    {
      vector<int>* neighborlist = m_Grains[grainname].neighborlist;
      int neighbor = neighborlist->at(0);
      voxels[i].grainname = neighbor;
	  voxels[i].unassigned = 1;
      m_Grains[grainname].gotcontainedmerged = containedmerged;
    }
  }
  for(int j = 1; j < numgrains; j++)
  {
    if(m_Grains[j].gotcontainedmerged != 1)
    {
      vector<int>* neighborlist = m_Grains[j].neighborlist;
      int size = 0;
      if (NULL != neighborlist ) { size = neighborlist->size(); }
      for(int k=0;k<size;k++)
      {
        int firstneighbor = neighborlist->at(k);
        int gotcontainedmerged = m_Grains[firstneighbor].gotcontainedmerged;
        if(gotcontainedmerged != 1) nlistgood.push_back(firstneighbor);
      }
      int neighcount = int(nlistgood.size());
      m_Grains[j].numneighbors = neighcount;
      if (m_Grains[j].neighborlist == NULL) { m_Grains[j].neighborlist = new std::vector<int>(nlistgood.size() ); }
      m_Grains[j].neighborlist->swap(nlistgood);
      nlistgood.clear();
    }
  }
}

int  ReconstructionFunc::renumber_grains()
{
  int graincount = 1;
  for (int i = 1; i < numgrains; i++)
  {
    int gotcontainedmerged = m_Grains[i].gotcontainedmerged;
    if (gotcontainedmerged != 1)
    {
      m_Grains[i].newgrainname = graincount;
      int onedge = m_Grains[i].surfacegrain;
      m_Grains[graincount].surfacegrain = onedge;
      graincount++;
    }
  }
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
    int grainname = voxels[j].grainname;
    if (grainname >= 1)
    {
      int newgrainname = m_Grains[grainname].newgrainname;
      voxels[j].grainname = newgrainname;
    }
  }
  return graincount;
}

int ReconstructionFunc::define_subgrains()
{
  size_t initialVoxelsListSize = 1000;
  std::vector<int > voxelslist;
  voxelslist.resize(initialVoxelsListSize, -1);
  vector<int >* vlist;
  size_t size = 0;
  int neighbors[6];
  neighbors[0] = -(xpoints * ypoints);
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = (xpoints * ypoints);
  int good = 0, keepgoing = 1;
  int index, neighbor, nucleus, name;
  int col, row, plane;
  int newnumgrains = numgrains;
  double minw = 10000.0;
  double q1[5], q2[5];
  double w, n1, n2, n3;
  for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
  {
	  voxels[j].alreadychecked = 0;
  }
  for (int i = 1; i < numgrains; i++)
  {
    if (NULL == m_Grains[i].voxellist)
    {
      m_Grains[i].voxellist = new std::vector<int>;
    }
    vlist = m_Grains[i].voxellist;
    nucleus = m_Grains[i].nucleus;
    size = 0;
    voxelslist[size] = nucleus;
    voxels[nucleus].grainname = i;
    voxels[nucleus].alreadychecked = 1;
    size++;
    for (size_t j = 0; j < size; j++)
    {
      index = voxelslist[j];
      col = index % xpoints;
      row = (index / xpoints) % ypoints;
      plane = index / (xpoints * ypoints);
      for (int k = 0; k < 6; k++)
      {
        good = 1;
        neighbor = index + neighbors[k];
        if (k == 0 && plane == 0) good = 0;
        if (k == 5 && plane == (zpoints - 1)) good = 0;
        if (k == 1 && row == 0) good = 0;
        if (k == 4 && row == (ypoints - 1)) good = 0;
        if (k == 2 && col == 0) good = 0;
        if (k == 3 && col == (xpoints - 1)) good = 0;
        if (good == 1 && voxels[neighbor].alreadychecked == 0 && voxels[neighbor].grainname == i)
        {
          if (voxels[neighbor].kernelmisorientation <= (misorientationtolerance/2.0))
          {
            voxels[neighbor].grainname = i;
            voxelslist[size] = neighbor;
            voxels[neighbor].alreadychecked = 1;
            size++;
            if (size >= voxelslist.size())
            {
              voxelslist.resize(size + initialVoxelsListSize, -1);
            }
          }
        }
      }
    }
    voxelslist.clear();
    voxelslist.resize(initialVoxelsListSize, -1);
    keepgoing = 1;
    while (keepgoing > 0)
    {
      keepgoing = 0;
      for (size_t j = 0; j < vlist->size(); j++)
      {
        index = vlist->at(j);
        if (voxels[index].kernelmisorientation <= (misorientationtolerance/2.0) && voxels[index].alreadychecked == 0)
        {
          keepgoing++;
          size = 0;
          voxelslist[size] = index;
          voxels[index].grainname = newnumgrains;
          voxels[index].alreadychecked = 1;
          size++;
          for (size_t k = 0; k < size; k++)
          {
            index = voxelslist[k];
            col = index % xpoints;
            row = (index / xpoints) % ypoints;
            plane = index / (xpoints * ypoints);
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighbor = index + neighbors[l];
              if (l == 0 && plane == 0) good = 0;
              if (l == 5 && plane == (zpoints - 1)) good = 0;
              if (l == 1 && row == 0) good = 0;
              if (l == 4 && row == (ypoints - 1)) good = 0;
              if (l == 2 && col == 0) good = 0;
              if (l == 3 && col == (xpoints - 1)) good = 0;
              if (good == 1 && voxels[neighbor].alreadychecked == 0 && voxels[neighbor].grainname == i)
              {
                if (voxels[neighbor].kernelmisorientation <= (misorientationtolerance/2.0))
                {
                  voxelslist[size] = neighbor;
                  voxels[neighbor].grainname = newnumgrains;
                  voxels[neighbor].alreadychecked = 1;
                  size++;
                  if (size >= voxelslist.size())
                  {
                    voxelslist.resize(size + initialVoxelsListSize, -1);
                  }
                }
              }
            }
          }
          voxelslist.clear();
          voxelslist.resize(initialVoxelsListSize, -1);
          newnumgrains++;
        }
      }
    }
  }
  keepgoing = 1;
  while (keepgoing > 0)
  {
    keepgoing = 0;
    for (int j = 0; j < (xpoints * ypoints * zpoints); j++)
    {
      minw = 100000.0;
      if (voxels[j].grainname == 0) voxels[j].alreadychecked = 1;
      if (voxels[j].alreadychecked == 0 && voxels[j].grainname >= 0)
      {
        col = j % xpoints;
        row = (j / xpoints) % ypoints;
        plane = j / (xpoints * ypoints);
        q1[0] = 0;
        q1[1] = voxels[j].quat[1];
        q1[2] = voxels[j].quat[2];
        q1[3] = voxels[j].quat[3];
        q1[4] = voxels[j].quat[4];
        for (int k = 0; k < 6; k++)
        {
          good = 1;
          neighbor = j + neighbors[k];
          if (k == 0 && plane == 0) good = 0;
          if (k == 5 && plane == (zpoints - 1)) good = 0;
          if (k == 1 && row == 0) good = 0;
          if (k == 4 && row == (ypoints - 1)) good = 0;
          if (k == 2 && col == 0) good = 0;
          if (k == 3 && col == (xpoints - 1)) good = 0;
          if (good == 1 && voxels[neighbor].alreadychecked == 1 && voxels[neighbor].grainname >= 0)
          {
            q2[0] = 0;
            q2[1] = voxels[neighbor].quat[1];
            q2[2] = voxels[neighbor].quat[2];
            q2[3] = voxels[neighbor].quat[3];
            q2[4] = voxels[neighbor].quat[4];
            if(crystruct == AIM::Reconstruction::Hexagonal){
              w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
            }
            if(crystruct == AIM::Reconstruction::Cubic) {
              w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
            }
            if (w < minw) name = voxels[neighbor].grainname, minw = w;
          }
          if (minw < misorientationtolerance)
          {
            voxels[j].grainname = name;
            voxels[j].alreadychecked = 1;
            keepgoing++;
          }
        }
      }
    }
  }
  m_Grains.clear();
  m_Grains.resize(newnumgrains);
  return newnumgrains;
}
int ReconstructionFunc::reburn_grains()
{
    size_t initialVoxelsListSize = 1000;
    std::vector<int> voxelslist;
	voxelslist.resize(initialVoxelsListSize, -1);
	size_t size = 0;
	int graincount = 1;
	int neighbors[26];
	int good = 0;
	//int currentpoint = 0;
	int neighbor = 0;
	int col, row, plane;
//	int newvoxelcount=0;
	int mincol, maxcol, minrow, maxrow, minplane, maxplane;
	int gnum;
//	int point;
	mincol = xpoints;
	minrow = ypoints;
	minplane = zpoints;
	maxcol = 0;
	maxrow = 0;
	maxplane = 0;
/*	for(int k = 0; k < zpoints; ++k)
	{
		for(int j = 0; j < ypoints; ++j)
		{
			for(int i = 0; i < xpoints; ++i)
			{
				point = (k*xpoints*ypoints)+(j*xpoints)+i;
				if(voxels[point].grainname > 0)
				{
					col = i;
					row = j;
					plane = k;
					if(col < mincol) mincol = col;
					if(col > maxcol) maxcol = col;
					if(row < minrow) minrow = row;
					if(row > maxrow) maxrow = row;
					if(plane < minplane) minplane = plane;
					if(plane > maxplane) maxplane = plane;
				}
			}
		 }
	}
	newvoxelcount = 0;
	int newxpoints = (maxcol-mincol)+1;
	int newypoints = (maxrow-minrow)+1;
	int newzpoints = (maxplane-minplane)+1;
	sizex = (maxcol-mincol)*resx;
	sizey = (maxrow-minrow)*resy;
	sizez = (maxplane-minplane)*resz;
	if(newxpoints != xpoints || newypoints != ypoints || newzpoints != zpoints)
	{
		voxelstemp = new Voxel[(newxpoints*newypoints*newzpoints)];
		for(int k = minplane; k < maxplane+1; ++k)
		{
			for(int j = minrow; j < maxrow+1; ++j)
			{
				for(int i = mincol; i < maxcol+1; ++i)
				{
					point = (k*xpoints*ypoints)+(j*xpoints)+i;
					if(voxels[point].grainname == 0)
					{
//						voxels[point].grainname = -1;
						voxels[point].neighbor = -1;
						voxels[point].unassigned = 1;
					}
					voxelstemp[newvoxelcount] = voxels[point];
					newvoxelcount++;
				}
			}
		}
		delete [] voxels;
		xpoints = newxpoints;
		ypoints = newypoints;
		zpoints = newzpoints;
		totalpoints = (xpoints*ypoints*zpoints);
		voxels = new Voxel[totalpoints];
		for(int i=0;i<totalpoints;i++)
		{
			voxels[i] = voxelstemp[i];
		}
		delete [] voxelstemp;
	}
	assign_badpoints();
*/	neighbors[0] = -(xpoints*ypoints)-xpoints-1;
	neighbors[1] = -(xpoints*ypoints)-xpoints;
	neighbors[2] = -(xpoints*ypoints)-xpoints+1;
	neighbors[3] = -(xpoints*ypoints)-1;
	neighbors[4] = -(xpoints*ypoints);
	neighbors[5] = -(xpoints*ypoints)+1;
	neighbors[6] = -(xpoints*ypoints)+xpoints-1;
	neighbors[7] = -(xpoints*ypoints)+xpoints;
	neighbors[8] = -(xpoints*ypoints)+xpoints+1;
	neighbors[9] = -xpoints-1;
	neighbors[10] = -xpoints;
	neighbors[11] = -xpoints+1;
	neighbors[12] = -1;
	neighbors[13] = 1;
	neighbors[14] = xpoints-1;
	neighbors[15] = xpoints;
	neighbors[16] = xpoints+1;
	neighbors[17] = (xpoints*ypoints)-xpoints-1;
	neighbors[18] = (xpoints*ypoints)-xpoints;
	neighbors[19] = (xpoints*ypoints)-xpoints+1;
	neighbors[20] = (xpoints*ypoints)-1;
	neighbors[21] = (xpoints*ypoints);
	neighbors[22] = (xpoints*ypoints)+1;
	neighbors[23] = (xpoints*ypoints)+xpoints-1;
	neighbors[24] = (xpoints*ypoints)+xpoints;
	neighbors[25] = (xpoints*ypoints)+xpoints+1;
	for(int i=0;i<(xpoints*ypoints*zpoints);i++)
	{
		voxels[i].alreadychecked = 0;
		gnum = voxels[i].grainname;
		if(gnum > 0 && m_Grains[gnum].kernelmisorientation > voxels[i].kernelmisorientation)
		{
			m_Grains[gnum].active = 1;
			m_Grains[gnum].nucleus = i;
			m_Grains[gnum].kernelmisorientation = voxels[i].kernelmisorientation;
		}
	}
	for(int i=1;i<numgrains;i++)
	{
		size = 0;
		int nucleus = m_Grains[i].nucleus;
		voxelslist[size] = nucleus;
		voxels[nucleus].alreadychecked = 1;
		voxels[nucleus].grainname = graincount;
		size++;
		for(size_t j=0;j<size;j++)
		{
			int currentpoint = voxelslist[j];
			col = currentpoint%xpoints;
			row = (currentpoint/xpoints)%ypoints;
			plane = currentpoint/(xpoints*ypoints);
			for(int k=0;k<26;k++)
			{
			   good = 1;
			   neighbor = currentpoint+neighbors[k];
			   if(k < 9 && plane == 0) good = 0;
			   if(k > 16 && plane == (zpoints-1)) good = 0;
			   if((k == 0 || k == 1 || k == 2 || k == 9 || k == 10 || k == 11 || k == 17 || k == 18 || k == 19) && row == 0) good = 0;
			   if((k == 6 || k == 7 || k == 8 || k == 14 || k == 15 || k == 16 || k == 23 || k == 24 || k == 25) && row == (ypoints-1)) good = 0;
			   if((k == 0 || k == 3 || k == 6 || k == 9 || k == 12 || k == 14 || k == 17 || k == 20 || k == 23) && col == 0) good = 0;
			   if((k == 2 || k == 5 || k == 8 || k == 11 || k == 13 || k == 16 || k == 19 || k == 22 || k == 25) && col == (xpoints-1)) good = 0;
			   if(good == 1 && voxels[neighbor].alreadychecked == 0)
			   {
					int grainname = voxels[neighbor].grainname;
					if(grainname == i)
					{
						voxelslist[size] = neighbor;
						voxels[neighbor].alreadychecked = 1;
						voxels[neighbor].grainname = graincount;
						size++;
						if (size >= voxelslist.size())
						{
							voxelslist.resize(size+initialVoxelsListSize,-1);
						}
					}
			   }
			}
		}
		voxelslist.erase(std::remove(voxelslist.begin(),voxelslist.end(),-1),voxelslist.end());
		if(m_Grains[graincount].voxellist == NULL)
		{
			m_Grains[graincount].voxellist = new std::vector<int>(voxelslist.size());
		}
		m_Grains[graincount].nucleus = m_Grains[i].nucleus;
		m_Grains[graincount].numvoxels = voxelslist.size();
		m_Grains[graincount].voxellist->swap(voxelslist);
		voxelslist.clear();
		voxelslist.resize(initialVoxelsListSize,-1);
		graincount++;
	}
	return graincount;
}
void  ReconstructionFunc::find_kernels()
{
  double q1[5];
  double q2[5];
  int numVoxel;                 // number of voxels in the grain...
//  int index;
//  int gnum;
  int good = 0;
  int neighbor;
  int col, row, plane;
  double w, totalmisorientation;
  double n1,n2,n3;
  m_Grains[0].averagemisorientation = 0.0;
  int steps = 1;
  for(int i=0;i<(xpoints*ypoints*zpoints);i++)
  {
	  if(voxels[i].grainname > 0 && voxels[i].unassigned != 1)
	  {
		totalmisorientation = 0.0;
		numVoxel = 0;
	    q1[1] = voxels[i].quat[1];
	    q1[2] = voxels[i].quat[2];
	    q1[3] = voxels[i].quat[3];
	    q1[4] = voxels[i].quat[4];
		col = i%xpoints;
		row = (i/xpoints)%ypoints;
		plane = i/(xpoints*ypoints);
		for(int j=-steps;j<steps+1;j++)
	    {
		    for(int k=-steps;k<steps+1;k++)
			{
			    for(int l=-steps;l<steps+1;l++)
			    {
				  good = 1;
				  neighbor = i+(j*xpoints*ypoints)+(k*xpoints)+(l);
			      if(plane+j < 0) good = 0;
			      if(plane+j > zpoints-1) good = 0;
			      if(row+k < 0) good = 0;
			      if(row+k > ypoints-1) good = 0;
			      if(col+l < 0) good = 0;
			      if(col+l > xpoints-1) good = 0;
				  if(good == 1 && voxels[i].grainname == voxels[neighbor].grainname && voxels[neighbor].unassigned != 1)
				  {
					  numVoxel++;
				      q2[1] = voxels[neighbor].quat[1];
					  q2[2] = voxels[neighbor].quat[2];
					  q2[3] = voxels[neighbor].quat[3];
					  q2[4] = voxels[neighbor].quat[4];
		         if(crystruct == AIM::Reconstruction::Hexagonal){
		           w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
		         }
		         if(crystruct == AIM::Reconstruction::Cubic) {
		           w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
		         }
					  totalmisorientation = totalmisorientation + w;
				  }
				}
			}
		}
		voxels[i].kernelmisorientation = totalmisorientation/(float)numVoxel;
	  }
	  if(voxels[i].grainname == 0 || voxels[i].unassigned == 1)
	  {
		  voxels[i].kernelmisorientation = 0;
	  }
  }
}
void  ReconstructionFunc::homogenize_grains()
{
  int i, j, ii, jj, kk, p, pp;
//  int sign1, sign2, sign3, sign4; // If the value is 1, it's positive; -1, negative; 0, 0.000...
  float qT[5]; // Temporary quaternions...
  float qN[5]; // Member quaternions inside a grain...
  float qC[5]; // Center quaternions inside that grain...
  float qSum[5]; // Sum of quaternions inside a cloud...
  double q1[5];
  double q2[5];
  int numsymm=0;
  int numVoxel;                 // number of voxels in the grain...
  int nucleus; //, nucl_site, nucl_spin;
//  int index;
//  int nid, nspin;
//  int atSurf;
//  int coin;
  int size_threshold;
  double maxdisorientation = 0;
  size_threshold = 100;          // Gonna pick inside pixel as a nucleus...
  float q4Temp;
  float max, min;
  int maxid, minid, sign;
  float dist2; // square distance between quaternions...
  double diff, sum, tmp, wmin;
//  double g1ea1good, g1ea2good, g1ea3good, g2ea1good, g2ea2good, g2ea3good;
//  double q1tot=0, q2tot=0, q3tot=0, q4tot=0;
  double n1,n2,n3;
  m_Grains[0].euler1 = 12.566;
  m_Grains[0].euler2 = 12.566;
  m_Grains[0].euler3 = 12.566;
  m_Grains[0].averagemisorientation = 0.0;
  if(crystruct == AIM::Reconstruction::Hexagonal)
  {
	numsymm = 12;
	quat_symm = new double *[numsymm];
	for(i=0;i<numsymm;i++)
	{
		quat_symm[i] = new double [5];
		for(j=0;j<5;j++)
		{
			quat_symm[i][j] = AIM::Quaternions::quat_symmhex[i][j];
		}
	}
  }
  if(crystruct == AIM::Reconstruction::Cubic)
  {
	numsymm = 24;
	quat_symm = new double *[numsymm];
	for(i=0;i<numsymm;i++)
	{
		quat_symm[i] = new double [5];
		for(j=0;j<5;j++)
		{
			quat_symm[i][j] = AIM::Quaternions::quat_symmcubic[i][j];
		}
	}
  }
  for(int i = 1; i < numgrains; i++)
  {
	numVoxel = 0;
    qSum[0] = 0.0;
    qSum[1] = 0.0;
    qSum[2] = 0.0;
    qSum[3] = 0.0;
    qSum[4] = 0.0;
	nucleus = m_Grains[i].nucleus;
    qC[1] = voxels[nucleus].quat[1];
    qC[2] = voxels[nucleus].quat[2];
    qC[3] = voxels[nucleus].quat[3];
    qC[4] = voxels[nucleus].quat[4];
    // Selecting quaternions with largest absolute valued (4th component)...
    max = 0.0;
    maxid = -1;
    for(ii=0; ii<numsymm; ii++)
	{
      qT[4] = quat_symm[ii][4]*qC[4] - quat_symm[ii][1]*qC[1] - quat_symm[ii][2]*qC[2] - quat_symm[ii][3]*qC[3];
      q4Temp = (float)fabs((double)qT[4]);
      if(q4Temp>max)
	  {
		  max = q4Temp;
		  maxid = ii;
      }
	  else
	  {
		max = max;
		maxid = maxid;
      }
    }
    qT[1] = quat_symm[maxid][1]*qC[4] + quat_symm[maxid][4]*qC[1] - quat_symm[maxid][2]*qC[3] + quat_symm[maxid][3]*qC[2];
    qT[2] = quat_symm[maxid][2]*qC[4] + quat_symm[maxid][4]*qC[2] - quat_symm[maxid][3]*qC[1] + quat_symm[maxid][1]*qC[3];
    qT[3] = quat_symm[maxid][3]*qC[4] + quat_symm[maxid][4]*qC[3] - quat_symm[maxid][1]*qC[2] + quat_symm[maxid][2]*qC[1];
    qT[4] = quat_symm[maxid][4]*qC[4] - quat_symm[maxid][1]*qC[1] - quat_symm[maxid][2]*qC[2] - quat_symm[maxid][3]*qC[3];
    if(qT[4]<0.0)
	{         // Change the sign so that 4th component is positive...
      qT[1] = (-1.0)*qT[1];
      qT[2] = (-1.0)*qT[2];
      qT[3] = (-1.0)*qT[3];
      qT[4] = (-1.0)*qT[4];
    }
    // Now begins to calculate the avg. orientation...
    if (NULL == m_Grains[i].voxellist)
    {
      m_Grains[i].voxellist = new std::vector<int>;
    }
	vector<int>* voxellist = m_Grains[i].voxellist;
	int size = voxellist->size();
	for(jj=0; jj<size; jj++)
	{
		int index = voxellist->at(jj);
		if(voxels[index].unassigned >= 0)
		{
		  numVoxel++;
		  for(pp=1; pp<5; pp++)
		  {
			qC[pp] = voxels[index].quat[pp];
		  }
		  min = 10.0;
		  minid = -1;
		  sign = 1;
		  for(kk=0; kk<numsymm; kk++)
		  {
			 qN[1] = quat_symm[kk][1]*qC[4] + quat_symm[kk][4]*qC[1] - quat_symm[kk][2]*qC[3] + quat_symm[kk][3]*qC[2];
			 qN[2] = quat_symm[kk][2]*qC[4] + quat_symm[kk][4]*qC[2] - quat_symm[kk][3]*qC[1] + quat_symm[kk][1]*qC[3];
			 qN[3] = quat_symm[kk][3]*qC[4] + quat_symm[kk][4]*qC[3] - quat_symm[kk][1]*qC[2] + quat_symm[kk][2]*qC[1];
			 qN[4] = quat_symm[kk][4]*qC[4] - quat_symm[kk][1]*qC[1] - quat_symm[kk][2]*qC[2] - quat_symm[kk][3]*qC[3];
			 dist2 = 2.0 * ( 1 -( qT[4]*qN[4] + qT[1]*qN[1] + qT[2]*qN[2] + qT[3]*qN[3] ) );
			 if(dist2<0.0)
			 {
			   dist2 = 0.0;
			 }
			 if(dist2<min)
			 {
			   min = dist2;
			   minid = kk;
			   sign = 1;
			 }
			 else
			 {
			   min = min;
			   minid = minid;
			   sign = sign;
			 }
			 dist2 = 2.0 * ( 1 + ( qT[4]*qN[4] + qT[1]*qN[1] + qT[2]*qN[2] + qT[3]*qN[3] ) );
			 if(dist2<0.0)
			 {
			   dist2 = 0.0;
			 }
			 if(dist2<min)
			 {
			   min = dist2;
			   minid = kk;
			   sign = -1;
			 }
			 else
			 {
			   min = min;
			   minid = minid;
			   sign = sign;
			 }
		   }
		   qN[1] = quat_symm[minid][1]*qC[4] + quat_symm[minid][4]*qC[1] - quat_symm[minid][2]*qC[3] + quat_symm[minid][3]*qC[2];
		   qN[2] = quat_symm[minid][2]*qC[4] + quat_symm[minid][4]*qC[2] - quat_symm[minid][3]*qC[1] + quat_symm[minid][1]*qC[3];
		   qN[3] = quat_symm[minid][3]*qC[4] + quat_symm[minid][4]*qC[3] - quat_symm[minid][1]*qC[2] + quat_symm[minid][2]*qC[1];
		   qN[4] = quat_symm[minid][4]*qC[4] - quat_symm[minid][1]*qC[1] - quat_symm[minid][2]*qC[2] - quat_symm[minid][3]*qC[3];
		   if(sign==-1)
		   {
			 qN[1] = (-1.0)*qN[1];
			 qN[2] = (-1.0)*qN[2];
			 qN[3] = (-1.0)*qN[3];
			 qN[4] = (-1.0)*qN[4];
		   }
		   for(p=0; p<5; p++)
		   {
		     voxels[index].quat[p] = qN[p];
			 qSum[p] = qSum[p] + qN[p];
		   }
		}
    }
	for(int s=1; s<5; s++)
	{
		m_Grains[i].avg_quat[s] = qSum[s]/(float)numVoxel;
    }
	double q1tot = m_Grains[i].avg_quat[1];
	double q2tot = m_Grains[i].avg_quat[2];
	double q3tot = m_Grains[i].avg_quat[3];
	double q4tot = m_Grains[i].avg_quat[4];
	double normalizer = (q1tot*q1tot)+(q2tot*q2tot)+(q3tot*q3tot)+(q4tot*q4tot);
	normalizer = pow(normalizer,0.5);
	q1tot = q1tot/normalizer;
	q2tot = q2tot/normalizer;
	q3tot = q3tot/normalizer;
	q4tot = q4tot/normalizer;
	m_Grains[i].avg_quat[1] = q1tot;
	m_Grains[i].avg_quat[2] = q2tot;
	m_Grains[i].avg_quat[3] = q3tot;
	m_Grains[i].avg_quat[4] = q4tot;
	diff=atan2(q2tot,q1tot);
	sum=atan2(q3tot,q4tot);
	double ea1good=(diff+sum);
	double ea3good=(sum-diff);
	tmp=(q3tot*q3tot)+(q4tot*q4tot);
	tmp = pow(tmp,0.5);
	if(tmp > 1.0) tmp=1.0;
	double ea2good=2*acos(tmp);
    m_Grains[i].euler1 = ea1good;
    m_Grains[i].euler2 = ea2good;
    m_Grains[i].euler3 = ea3good;
    double avgmiso = 0;
	double averageiq;
    double avgmisoorig = 0;
    double totalcount = 0;
    q1[1] = voxels[nucleus].quat[1];
    q1[2] = voxels[nucleus].quat[2];
    q1[3] = voxels[nucleus].quat[3];
    q1[4] = voxels[nucleus].quat[4];
	for(jj=0; jj<size; jj++)
	{
		int index = voxellist->at(jj);
		if(voxels[index].unassigned == 0)
		{
		   avgmisoorig = avgmisoorig+voxels[index].misorientation;
		   averageiq = averageiq+voxels[index].imagequality;
//		   double angcur = 1000000;
		   q2[1] = voxels[index].quat[1];
		   q2[2] = voxels[index].quat[2];
		   q2[3] = voxels[index].quat[3];
		   q2[4] = voxels[index].quat[4];
       if(crystruct == AIM::Reconstruction::Hexagonal){
         wmin = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
       }
       if(crystruct == AIM::Reconstruction::Cubic) {
         wmin = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
       }
	       voxels[index].misorientation = wmin;
		   avgmiso = avgmiso + wmin;
		   totalcount++;
		   if(wmin > maxdisorientation) maxdisorientation = wmin;
        }
		if(voxels[index].unassigned == 1)
		{
			voxels[index].misorientation = 0;
		}
    }
    avgmiso = avgmiso/totalcount;
    averageiq = averageiq/totalcount;
    avgmisoorig = avgmisoorig/totalcount;
	m_Grains[i].averagemisorientation = avgmiso;
	m_Grains[i].averageimagequality = averageiq;
  }

}

int  ReconstructionFunc::load_data(string readname)
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
  for(int i = 0; i < 1; i++)
  {
	inputFile >> xpoints >> ypoints >> zpoints;
	inputFile >> resx >> resy >> resz;
  }
  delete [] voxels;
  voxels = new Voxel[(xpoints*ypoints*zpoints)];
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    inputFile >> ea1 >> ea2 >> ea3 >> weight >> strain >> stress >> x >> y >> z >> gnum >> phase;
	ea1 = ea1*3.1415926535897/180.0;
	ea2 = ea2*3.1415926535897/180.0;
	ea3 = ea3*3.1415926535897/180.0;
    voxels[i].grainname = gnum;
	voxels[i].euler1 = ea1;
	voxels[i].euler2 = ea2;
	voxels[i].euler3 = ea3;
    s=sin(0.5*ea2);
    c=cos(0.5*ea2);
    s1=sin(0.5*(ea1-ea3));
	c1=cos(0.5*(ea1-ea3));
	s2=sin(0.5*(ea1+ea3));
    c2=cos(0.5*(ea1+ea3));
	q1 = s*c1;
	q2 = s*s1;
	q3 = c*s2;
	q4 = c*c2;
	voxels[i].quat[0] = 0;
	voxels[i].quat[1] = q1;
	voxels[i].quat[2] = q2;
	voxels[i].quat[3] = q3;
	voxels[i].quat[4] = q4;
	if(gnum > numgrains)
	{
		numgrains = gnum;
		m_Grains.resize(numgrains+1);
	}
 	vnum = m_Grains[gnum].numvoxels;
  if (m_Grains[gnum].voxellist == NULL)
  {
    m_Grains[gnum].voxellist = new std::vector<int>(vListSize,-1);
  }
	if(vnum >= m_Grains[gnum].voxellist->size())
	{
		m_Grains[gnum].voxellist->resize(m_Grains[gnum].voxellist->size()+vListSize,-1);
	}
	m_Grains[gnum].voxellist->at(vnum) = i;
	vnum++;
	m_Grains[gnum].numvoxels = vnum;
  }
  numgrains = numgrains+1;
  for(int i=1;i<numgrains;i++)
  {
	if(m_Grains[i].numvoxels > 0)
	{
		m_Grains[i].voxellist->erase(std::remove(m_Grains[i].voxellist->begin(),m_Grains[i].voxellist->end(),-1),m_Grains[i].voxellist->end());
		int numvoxels = int(m_Grains[i].voxellist->size());
		m_Grains[i].numvoxels = numvoxels;
		m_Grains[i].nucleus = m_Grains[i].voxellist->at(0);
		m_Grains[i].active = active;
	}
  }
  inputFile.close();
return numgrains;
}
void  ReconstructionFunc::merge_twins ()
{
  int twinmerged = 1;
  double angcur = 180;
  vector<int > twinlist;
  double w;
  double n1, n2, n3;
  double q1[5];
  double q2[5];
  for (int i = 1; i < numgrains; i++)
  {
    if (m_Grains[i].twinnewnumber == -1)
    {
      twinlist.push_back(i);
      int tsize = int(twinlist.size());
      for (int m = 0; m < tsize; m++)
      {
        tsize = int(twinlist.size());
        int firstgrain = twinlist[m];
        vector<int >* nlist = m_Grains[firstgrain].neighborlist;
        int size = int(nlist->size());
        for (int l = 0; l < size; l++)
        {
          angcur = 180;
          int twin = 0;
          int neigh = nlist->at(l);
          if (neigh != i && m_Grains[neigh].twinnewnumber == -1)
          {
            q1[1] = m_Grains[firstgrain].avg_quat[1];
            q1[2] = m_Grains[firstgrain].avg_quat[2];
            q1[3] = m_Grains[firstgrain].avg_quat[3];
            q1[4] = m_Grains[firstgrain].avg_quat[4];
            q2[1] = m_Grains[neigh].avg_quat[1];
            q2[2] = m_Grains[neigh].avg_quat[2];
            q2[3] = m_Grains[neigh].avg_quat[3];
            q2[4] = m_Grains[neigh].avg_quat[4];
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
              m_Grains[neigh].gottwinmerged = twinmerged;
              m_Grains[neigh].twinnewnumber = i;
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
    int gottwinmerged = m_Grains[grainname].gottwinmerged;
    if (gottwinmerged == 1)
    {
      int twinnewnumber = m_Grains[grainname].twinnewnumber;
      voxels[k].grainname = twinnewnumber;
    }
  }
}

void  ReconstructionFunc::merge_colonies ()
{
  int colonymerged = 1;
  double angcur = 180;
  vector<int > colonylist;
  double w;
  double n1, n2, n3;
  double q1[5];
  double q2[5];
  for (int i = 1; i < numgrains; i++)
  {
    if (m_Grains[i].colonynewnumber != -1)
    {
      colonylist.push_back(i);
      int csize = int(colonylist.size());
      for (int m = 0; m < csize; m++)
      {
        csize = int(colonylist.size());
        int firstgrain = colonylist[m];
        vector<int >* nlist = m_Grains[firstgrain].neighborlist;
        int size = int(nlist->size());
        for (int l = 0; l < size; l++)
        {
          angcur = 180;
          int colony = 0;
          int neigh = nlist->at(l);
          if (neigh != i && m_Grains[neigh].colonynewnumber != -1)
          {
            q1[1] = m_Grains[firstgrain].avg_quat[1];
            q1[2] = m_Grains[firstgrain].avg_quat[2];
            q1[3] = m_Grains[firstgrain].avg_quat[3];
            q1[4] = m_Grains[firstgrain].avg_quat[4];
            q2[1] = m_Grains[neigh].avg_quat[1];
            q2[2] = m_Grains[neigh].avg_quat[2];
            q2[3] = m_Grains[neigh].avg_quat[3];
            q2[4] = m_Grains[neigh].avg_quat[4];
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
              m_Grains[neigh].gotcolonymerged = colonymerged;
              m_Grains[neigh].colonynewnumber = i;
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
    int gotcolonymerged = m_Grains[grainname].gotcolonymerged;
    if (gotcolonymerged == 1)
    {
      int colonynewnumber = m_Grains[grainname].colonynewnumber;
      voxels[k].grainname = colonynewnumber;
    }
  }
}

void  ReconstructionFunc::characterize_twins()
{
  for(int i=0;i<numgrains;i++)
  {

  }
}

void  ReconstructionFunc::characterize_colonies()
{
  for(int i=0;i<numgrains;i++)
  {

  }
}

int  ReconstructionFunc::renumber_grains3()
{
  int graincount = 1;
  std::vector<int> newnames(numgrains);
  for(int i = 1; i < numgrains; i++)
  {
    int gottwinmerged = m_Grains[i].gottwinmerged;
    if(gottwinmerged != 1)
    {
	  newnames[i] = graincount;
      double ea1good = m_Grains[i].euler1;
      double ea2good = m_Grains[i].euler2;
      double ea3good = m_Grains[i].euler3;
      int size = m_Grains[i].numvoxels;
      int numneighbors = m_Grains[i].numneighbors;
      vector<int>* nlist = m_Grains[i].neighborlist;
      m_Grains[graincount].numvoxels = size;
      m_Grains[graincount].numneighbors = numneighbors;
      if (m_Grains[graincount].neighborlist == NULL)
      {
        m_Grains[graincount].neighborlist = new std::vector<int>(numneighbors);
      }
      if (NULL != nlist)
	  {
        m_Grains[graincount].neighborlist->swap(*nlist);
      }
      m_Grains[graincount].euler1 = ea1good;
      m_Grains[graincount].euler2 = ea2good;
      m_Grains[graincount].euler3 = ea3good;
      graincount++;
    }
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    int grainname = voxels[j].grainname;
    if(grainname >= 1)
	{
		int newgrainname = newnames[grainname];
		voxels[j].grainname = newgrainname;
	}
  }
  return graincount;
}
void  ReconstructionFunc::find_neighbors()
{
  int neighbors[6];
  neighbors[0] = -(xpoints*ypoints);
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = (xpoints*ypoints);
  double column, row, plane;
  double x, y, z;
  double xn, yn, zn;
  double xdist, ydist, zdist;
  int grain;
  int nnum;
  int onsurf = 0;
  double dist, dist2, diam, diam2;
  int good = 0;
  int neighbor = 0;
  totalsurfacearea=0;
  int surfacegrain = 1;
  int nListSize = 1000;
  std::vector<int> nlist(nListSize, -1);
  std::vector<double> nsalist(nListSize, -1);
  for(int i=0;i<numgrains;i++)
  {
    int numneighs = int(nlist.size());
	m_Grains[i].numneighbors = 0;
    m_Grains[i].neighborlist = new std::vector<int>(numneighs);
    m_Grains[i].neighborlist->swap(nlist);
    m_Grains[i].neighborsurfarealist = new std::vector<double>(numneighs);
    m_Grains[i].neighborsurfarealist->swap(nsalist);
	m_Grains[i].neighbordistfunc.resize(3,0);
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    onsurf = 0;
    grain = voxels[j].grainname;
	if(grain > 0)
	{
		column = j%xpoints;
		row = (j/xpoints)%ypoints;
		plane = j/(xpoints*ypoints);
		if((column == 0 || column == (xpoints-1) || row == 0 || row == (ypoints-1) || plane == 0 || plane == (zpoints-1)) && zpoints != 1) m_Grains[grain].surfacegrain = surfacegrain;
		if((column == 0 || column == (xpoints-1) || row == 0 || row == (ypoints-1)) && zpoints == 1) m_Grains[grain].surfacegrain = surfacegrain;
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
		  if(good == 1 && voxels[neighbor].grainname != grain && voxels[neighbor].grainname >= 0)
          {
			  onsurf++;
			  nnum = m_Grains[grain].numneighbors;
			  vector<int>* nlist = m_Grains[grain].neighborlist;
			  if (nnum >= (0.9*nlist->size()))
			  {
				 nlist->resize(nnum + nListSize);
			  }
			  nlist->at(nnum) = voxels[neighbor].grainname;
			  nnum++;
			  m_Grains[grain].numneighbors = nnum;
			  voxels[j].nearestneighbor = voxels[neighbor].grainname;
		  }
		}
	}
	voxels[j].surfacevoxel = onsurf;
	if(onsurf > 0) voxels[j].nearestneighbordistance = 0, voxels[j].neighbor = j;
	if(onsurf == 0) voxels[j].nearestneighbordistance = -1, voxels[j].nearestneighbor = -1;
  }
  vector<int>* nlistcopy;
  for(int i=1;i<numgrains;i++)
  {
	vector<int>* nlist = m_Grains[i].neighborlist;
	vector<double>* nsalist = m_Grains[i].neighborsurfarealist;
	vector<int>::iterator newend;
	sort(nlist->begin(),nlist->end());
	nlistcopy = nlist;
    newend = unique(nlist->begin(),nlist->end());
    nlist->erase(newend,nlist->end());
	nlist->erase(std::remove(nlist->begin(),nlist->end(),-1),nlist->end());
//	nlist->erase(std::remove(nlist->begin(),nlist->end(),0),nlist->end());
	int numneighs = int(nlist->size());
	for(int j=0;j<numneighs;j++)
	{
		int neigh = nlist->at(j);
		int number = std::count(nlistcopy->begin(),nlistcopy->end(),neigh);
		double area = number*resx*resx;
		nsalist->at(j) = area;
		if(m_Grains[i].surfacegrain == 0 && (neigh > i || m_Grains[neigh].surfacegrain == 1))
		{
			totalsurfacearea = totalsurfacearea + area;
		}
	}
    m_Grains[i].numneighbors = numneighs;
    m_Grains[i].neighborlist = new std::vector<int>(numneighs);
    m_Grains[i].neighborlist = nlist;
    m_Grains[i].neighborsurfarealist = new std::vector<double>(numneighs);
    m_Grains[i].neighborsurfarealist = nsalist;
  }
  if(m_Grains[1].equivdiameter != 0)
  {
	  for(int i=1;i<numgrains;i++)
	  {
		x = m_Grains[i].centroidx;
		y = m_Grains[i].centroidy;
		z = m_Grains[i].centroidz;
		diam = m_Grains[i].equivdiameter;
		for(int j=i;j<numgrains;j++)
		{
			xn = m_Grains[j].centroidx;
			yn = m_Grains[j].centroidy;
			zn = m_Grains[j].centroidz;
			diam2 = m_Grains[j].equivdiameter;
			xdist = fabs(x-xn);
			ydist = fabs(y-yn);
			zdist = fabs(z-zn);
			dist = (xdist*xdist)+(ydist*ydist)+(zdist*zdist);
			dist = pow(dist,0.5);
			dist2 = dist;
			dist = int(dist/diam);
			dist2 = int(dist2/diam2);
			if(dist < 3)
			{
				m_Grains[i].neighbordistfunc[dist]++;
			}
			if(dist2 < 3)
			{
				m_Grains[j].neighbordistfunc[dist2]++;
			}
		}
	  }
  }
}
void  ReconstructionFunc::find_centroids()
{
//  int count = 0;
  int onedge = 0;
  maxdiameter=0;
  mindiameter=100000;
  double x, y, z;
  double radcubed;
  double diameter;
  graincenters = new double *[numgrains];
  for(int i = 0; i < numgrains; i++)
  {
    graincenters[i] = new double [5];
	for(int j=0;j<5;j++)
	{
		graincenters[i][j]=0;
	}
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
	    onedge = 0;
        int gnum = voxels[j].grainname;
        graincenters[gnum][0]++;
        x = find_xcoord(j);
        y = find_ycoord(j);
        z = find_zcoord(j);
	    int col = j%xpoints;
	    int row = (j/xpoints)%ypoints;
		int plane = j/(xpoints*ypoints);
        if(col <= 0) onedge = 1;
        if(col >= xpoints-1) onedge = 1;
        if(row <= 0) onedge = 1;
        if(row >= ypoints-1) onedge = 1;
		if(plane <= 0) onedge = 1;
		if(plane >= zpoints-1) onedge = 1;
        graincenters[gnum][1] = graincenters[gnum][1] + x;
        graincenters[gnum][2] = graincenters[gnum][2] + y;
        graincenters[gnum][3] = graincenters[gnum][3] + z;
		if(onedge == 1) graincenters[gnum][4] = 1;
  }
  for(int i=1;i<numgrains;i++)
  {
    graincenters[i][1] = graincenters[i][1]/graincenters[i][0];
    graincenters[i][2] = graincenters[i][2]/graincenters[i][0];
    graincenters[i][3] = graincenters[i][3]/graincenters[i][0];
    m_Grains[i].centroidx = graincenters[i][1];
    m_Grains[i].centroidy = graincenters[i][2];
    m_Grains[i].centroidz = graincenters[i][3];
    m_Grains[i].numvoxels = graincenters[i][0];
	m_Grains[i].volume = (graincenters[i][0]*resx*resy*resz);
    m_Grains[i].surfacegrain = graincenters[i][4];
	radcubed = (0.75*m_Grains[i].volume)/m_pi;
	diameter = (2*pow(radcubed,0.3333333333));
	m_Grains[i].equivdiameter = diameter;
	if(int(diameter) > maxdiameter)
	{
		maxdiameter = int(diameter);
	}
	if(diameter < mindiameter) mindiameter = diameter;
  }
}
void  ReconstructionFunc::find_centroids2D()
{
//  int count = 0;
  int onedge = 0;
  maxdiameter=0;
  mindiameter=100000;
  double x, y;
  double radsquared;
  double diameter;
  graincenters = new double *[numgrains];
  for(int i = 0; i < numgrains; i++)
  {
    graincenters[i] = new double [5];
	for(int j=0;j<4;j++)
	{
		graincenters[i][j]=0;
	}
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
	    onedge = 0;
        int gnum = voxels[j].grainname;
        graincenters[gnum][0]++;
        x = find_xcoord(j);
        y = find_ycoord(j);
	    int col = j%xpoints;
	    int row = (j/xpoints)%ypoints;
        if(col <= 0) onedge = 1;
        if(col >= xpoints-1) onedge = 1;
        if(row <= 0) onedge = 1;
        if(row >= ypoints-1) onedge = 1;
        graincenters[gnum][1] = graincenters[gnum][1] + x;
        graincenters[gnum][2] = graincenters[gnum][2] + y;
		if(onedge == 1) graincenters[gnum][3] = 1;
  }
  for(int i=1;i<numgrains;i++)
  {
    graincenters[i][1] = graincenters[i][1]/graincenters[i][0];
    graincenters[i][2] = graincenters[i][2]/graincenters[i][0];
    m_Grains[i].centroidx = graincenters[i][1];
    m_Grains[i].centroidy = graincenters[i][2];
    m_Grains[i].numvoxels = graincenters[i][0];
	m_Grains[i].volume = (graincenters[i][0]*resx*resy);
    m_Grains[i].surfacegrain = graincenters[i][3];
	radsquared = m_Grains[i].volume/m_pi;
	diameter = (2*pow(radsquared,0.5));
	m_Grains[i].equivdiameter = diameter;
	if(int(diameter) > maxdiameter)
	{
		maxdiameter = int(diameter);
	}
	if(diameter < mindiameter) mindiameter = diameter;
  }
}
void  ReconstructionFunc::find_euclidean_map()
{
  int nearestneighbordistance = 0;
//  int checked = 1;
  int count = 1;
  int good = 1;
  double x, y, z;
  int neighpoint;
  int neighbors[6];
  neighbors[0] = -xpoints*ypoints;
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = xpoints*ypoints;
  for(int a=0;a<(xpoints*ypoints*zpoints);a++)
  {
	  if(voxels[a].surfacevoxel == 0)
	  {
		voxels[a].neighbor = -1;
		voxels[a].nearestneighbor = -1;
		voxels[a].nearestneighbordistance = -1;
	  }
  }
  while(count != 0)
  {
    count = 0;
	nearestneighbordistance++;
	for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
	{
	  int nearestneighbor = voxels[i].nearestneighbor;
      if(nearestneighbor == -1)
      {
	    x = i%xpoints;
		y = (i/xpoints)%ypoints;
	    z = i/(xpoints*ypoints);
		for(int j=0;j<6;j++)
		{
			good = 1;
			neighpoint = i+neighbors[j];
		    if(j == 0 && z == 0) good = 0;
		    if(j == 5 && z == (zpoints-1)) good = 0;
		    if(j == 1 && y == 0) good = 0;
		    if(j == 4 && y == (ypoints-1)) good = 0;
		    if(j == 2 && x == 0) good = 0;
		    if(j == 3 && x == (xpoints-1)) good = 0;
			if(good == 1)
	        {
		        count++;
			    int nearestneighbor = voxels[neighpoint].nearestneighbor;
			    int nearestneighbordistance = voxels[neighpoint].nearestneighbordistance;
			    if(nearestneighbordistance != -1)
				{
					voxels[i].nearestneighbor = nearestneighbor;
					voxels[i].neighbor = voxels[neighpoint].neighbor;
				}
			}
		}
	  }
	}
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      if(voxels[j].neighbor != -1)
      {
		  voxels[j].nearestneighbordistance = nearestneighbordistance;
      }
    }
  }
  double x1,x2,y1,y2,z1,z2;
  double dist;
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
	  int nearestneighbor = voxels[j].neighbor;
	  x1 = find_xcoord(j);
	  y1 = find_ycoord(j);
	  z1 = find_zcoord(j);
	  x2 = find_xcoord(nearestneighbor);
	  y2 = find_ycoord(nearestneighbor);
	  z2 = find_zcoord(nearestneighbor);
	  dist = ((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2))+((z1-z2)*(z1-z2));
	  dist = pow(dist,0.5);
	  if(dist > 30)
	  {
	//	int stop = 0;
	  }
	  voxels[j].nearestneighbordistance = dist;
  }
}
void  ReconstructionFunc::find_moments ()
{
//  int count = 0;
  double u200=0;
  double u020=0;
  double u002=0;
  double u110=0;
  double u011=0;
  double u101=0;
  grainmoments = new double *[numgrains];
  for(int i = 0; i < numgrains; i++)
  {
	grainmoments[i] = new double [6];
	for(int j=0;j<6;j++)
	{
		grainmoments[i][j] = 0;
	}
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
	  u200=0;
	  u020=0;
	  u002=0;
	  u110=0;
	  u011=0;
	  u101=0;
      int gnum = voxels[j].grainname;
      double x = find_xcoord(j);
      double y = find_ycoord(j);
      double z = find_zcoord(j);
      double x1 = x+(resx/2);
      double x2 = x-(resx/2);
      double y1 = y+(resy/2);
      double y2 = y-(resy/2);
      double z1 = z+(resz/2);
      double z2 = z-(resz/2);
      double xdist1 = (x1-graincenters[gnum][1]);
      double ydist1 = (y1-graincenters[gnum][2]);
      double zdist1 = (z1-graincenters[gnum][3]);
      double xdist2 = (x1-graincenters[gnum][1]);
      double ydist2 = (y1-graincenters[gnum][2]);
      double zdist2 = (z2-graincenters[gnum][3]);
      double xdist3 = (x1-graincenters[gnum][1]);
      double ydist3 = (y2-graincenters[gnum][2]);
      double zdist3 = (z1-graincenters[gnum][3]);
      double xdist4 = (x1-graincenters[gnum][1]);
      double ydist4 = (y2-graincenters[gnum][2]);
      double zdist4 = (z2-graincenters[gnum][3]);
      double xdist5 = (x2-graincenters[gnum][1]);
      double ydist5 = (y1-graincenters[gnum][2]);
      double zdist5 = (z1-graincenters[gnum][3]);
      double xdist6 = (x2-graincenters[gnum][1]);
      double ydist6 = (y1-graincenters[gnum][2]);
      double zdist6 = (z2-graincenters[gnum][3]);
      double xdist7 = (x2-graincenters[gnum][1]);
      double ydist7 = (y2-graincenters[gnum][2]);
      double zdist7 = (z1-graincenters[gnum][3]);
      double xdist8 = (x2-graincenters[gnum][1]);
      double ydist8 = (y2-graincenters[gnum][2]);
      double zdist8 = (z2-graincenters[gnum][3]);
      u200 = u200 + ((ydist1)*(ydist1))+((zdist1)*(zdist1)) + ((ydist2)*(ydist2))+((zdist2)*(zdist2)) + ((ydist3)*(ydist3))+((zdist3)*(zdist3)) + ((ydist4)*(ydist4))+((zdist4)*(zdist4)) + ((ydist5)*(ydist5))+((zdist5)*(zdist5)) + ((ydist6)*(ydist6))+((zdist6)*(zdist6)) + ((ydist7)*(ydist7))+((zdist7)*(zdist7)) + ((ydist8)*(ydist8))+((zdist8)*(zdist8));
      u020 = u020 + ((xdist1)*(xdist1))+((zdist1)*(zdist1)) + ((xdist2)*(xdist2))+((zdist2)*(zdist2)) + ((xdist3)*(xdist3))+((zdist3)*(zdist3)) + ((xdist4)*(xdist4))+((zdist4)*(zdist4)) + ((xdist5)*(xdist5))+((zdist5)*(zdist5)) + ((xdist6)*(xdist6))+((zdist6)*(zdist6)) + ((xdist7)*(xdist7))+((zdist7)*(zdist7)) + ((xdist8)*(xdist8))+((zdist8)*(zdist8));
      u002 = u002 + ((xdist1)*(xdist1))+((ydist1)*(ydist1)) + ((xdist2)*(xdist2))+((ydist2)*(ydist2)) + ((xdist3)*(xdist3))+((ydist3)*(ydist3)) + ((xdist4)*(xdist4))+((ydist4)*(ydist4)) + ((xdist5)*(xdist5))+((ydist5)*(ydist5)) + ((xdist6)*(xdist6))+((ydist6)*(ydist6)) + ((xdist7)*(xdist7))+((ydist7)*(ydist7)) + ((xdist8)*(xdist8))+((ydist8)*(ydist8));
      u110 = u110 + ((xdist1)*(ydist1)) + ((xdist2)*(ydist2)) + ((xdist3)*(ydist3)) + ((xdist4)*(ydist4)) + ((xdist5)*(ydist5)) + ((xdist6)*(ydist6)) + ((xdist7)*(ydist7)) + ((xdist8)*(ydist8));
      u011 = u011 + ((ydist1)*(zdist1)) + ((ydist2)*(zdist2)) + ((ydist3)*(zdist3)) + ((ydist4)*(zdist4)) + ((ydist5)*(zdist5)) + ((ydist6)*(zdist6)) + ((ydist7)*(zdist7)) + ((ydist8)*(zdist8));
      u101 = u101 + ((xdist1)*(zdist1)) + ((xdist2)*(zdist2)) + ((xdist3)*(zdist3)) + ((xdist4)*(zdist4)) + ((xdist5)*(zdist5)) + ((xdist6)*(zdist6)) + ((xdist7)*(zdist7)) + ((xdist8)*(zdist8));
	  grainmoments[gnum][0] = grainmoments[gnum][0] + u200;
	  grainmoments[gnum][1] = grainmoments[gnum][1] + u020;
	  grainmoments[gnum][2] = grainmoments[gnum][2] + u002;
	  grainmoments[gnum][3] = grainmoments[gnum][3] + u110;
	  grainmoments[gnum][4] = grainmoments[gnum][4] + u011;
	  grainmoments[gnum][5] = grainmoments[gnum][5] + u101;
  }
  for(int i=1;i<numgrains;i++)
  {
	grainmoments[i][0] = grainmoments[i][0]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][1] = grainmoments[i][1]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][2] = grainmoments[i][2]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][3] = grainmoments[i][3]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][4] = grainmoments[i][4]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][5] = grainmoments[i][5]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	double o3 = (grainmoments[i][0]*grainmoments[i][1]*grainmoments[i][2])+(2.0*grainmoments[i][3]*grainmoments[i][5]*grainmoments[i][4])-(grainmoments[i][0]*grainmoments[i][4]*grainmoments[i][4])-(grainmoments[i][1]*grainmoments[i][5]*grainmoments[i][5])-(grainmoments[i][2]*grainmoments[i][3]*grainmoments[i][3]);
	double vol5 = m_Grains[i].volume;
	vol5 = pow(vol5,5);
	double omega3 = vol5/o3;
	m_Grains[i].Ixx = grainmoments[i][0];
	m_Grains[i].Iyy = grainmoments[i][1];
	m_Grains[i].Izz = grainmoments[i][2];
	m_Grains[i].Ixy = -grainmoments[i][3];
	m_Grains[i].Iyz = -grainmoments[i][4];
	m_Grains[i].Ixz = -grainmoments[i][5];
	m_Grains[i].omega3 = omega3;
  }
}
void  ReconstructionFunc::find_axes ()
{
  for (int i = 1; i < numgrains; i++)
  {
    double Ixx = m_Grains[i].Ixx;
    double Iyy = m_Grains[i].Iyy;
    double Izz = m_Grains[i].Izz;
    double Ixy = m_Grains[i].Ixy;
    double Iyz = m_Grains[i].Iyz;
    double Ixz = m_Grains[i].Ixz;
    double a = 1;
    double b = -Ixx-Iyy-Izz;
    double c = ((Ixx*Izz)+(Ixx*Iyy)+(Iyy*Izz)-(Ixz*Ixz)-(Ixy*Ixy)-(Iyz*Iyz));
    double d = ((Ixz*Iyy*Ixz)+(Ixy*Izz*Ixy)+(Iyz*Ixx*Iyz)-(Ixx*Iyy*Izz)-(Ixy*Iyz*Ixz)-(Ixy*Iyz*Ixz));
    double f = ((3*c/a)-((b/a)*(b/a)))/3;
    double g = ((2*(b/a)*(b/a)*(b/a))-(9*b*c/(a*a))+(27*(d/a)))/27;
    double h = (g*g/4)+(f*f*f/27);
    double rsquare = (g*g/4)-h;
    double r = pow(rsquare,0.5);
    double theta = 0;
    if(r == 0)
    {
      theta = 0;
    }
    if(r != 0)
    {
      theta = acos(-g/(2*r));
    }
    double r1 = 2*pow(r,0.33333333333)*cos(theta/3)-(b/(3*a));
    double r2 = -pow(r,0.33333333333)*(cos(theta/3)-(1.7320508*sin(theta/3)))-(b/(3*a));
    double r3 = -pow(r,0.33333333333)*(cos(theta/3)+(1.7320508*sin(theta/3)))-(b/(3*a));
	m_Grains[i].radius1 = r1;
	m_Grains[i].radius2 = r2;
	m_Grains[i].radius3 = r3;
  }
}
void  ReconstructionFunc::find_vectors (H5ReconStatsWriter::Pointer h5io)
{
  totalaxes = 0.0;
  double *axisodf;
  axisodf = new double [18*18*18];
  for(int i = 1; i < numgrains; i++)
  {
 //   int size = grains[i].numvoxels;
    double Ixx = m_Grains[i].Ixx;
    double Iyy = m_Grains[i].Iyy;
    double Izz = m_Grains[i].Izz;
    double Ixy = m_Grains[i].Ixy;
    double Iyz = m_Grains[i].Iyz;
    double Ixz = m_Grains[i].Ixz;
	double radius1 = m_Grains[i].radius1;
	double radius2 = m_Grains[i].radius2;
	double radius3 = m_Grains[i].radius3;
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
    for(int j = 0; j < 3; j++)
    {
        uber[0][0] = Ixx-e[j][0];
        uber[0][1] = Ixy;
        uber[0][2] = Ixz;
        uber[1][0] = Ixy;
        uber[1][1] = Iyy-e[j][0];
        uber[1][2] = Iyz;
        uber[2][0] = Ixz;
        uber[2][1] = Iyz;
        uber[2][2] = Izz-e[j][0];
        double **uberelim;
        double **uberbelim;
        uberelim = new double *[3];
        uberbelim = new double *[3];
        for(int d = 0; d < 3; d++)
        {
          uberelim[d] = new double [3];
          uberbelim[d] = new double [1];
        }
        int elimcount = 0;
        int elimcount1 = 0;
        double q = 0;
        double sum = 0;
        double c = 0;
        for(int a = 0; a < 3; a++)
        {
          elimcount1 = 0;
          for(int b = 0; b < 3; b++)
          {
            uberelim[elimcount][elimcount1] = uber[a][b];
            elimcount1++;
          }
          uberbelim[elimcount][0] = bmat[a][0];
          elimcount++;
        }
        for(int k = 0; k < elimcount-1; k++)
        {
          for(int l = k+1; l < elimcount; l++)
          {
            c = uberelim[l][k]/uberelim[k][k];
            for(int m = k+1; m < elimcount; m++)
            {
              uberelim[l][m] = uberelim[l][m] - c*uberelim[k][m];
            }
            uberbelim[l][0] = uberbelim[l][0] - c*uberbelim[k][0];
          }
        }
        uberbelim[elimcount-1][0] = uberbelim[elimcount-1][0]/uberelim[elimcount-1][elimcount-1];
        for(int l = 1; l < elimcount; l++)
        {
          int m = (elimcount-1)-l;
          sum = 0;
          for(int n = m+1; n < elimcount; n++)
          {
            sum = sum + (uberelim[m][n]*uberbelim[n][0]);
          }
          uberbelim[m][0] = (uberbelim[m][0]-sum)/uberelim[m][m];
        }
        for(int p = 0; p < elimcount; p++)
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
	double norm1 = pow(((n1x*n1x)+(n1y*n1y)+(n1z*n1z)),0.5);
	double norm2 = pow(((n2x*n2x)+(n2y*n2y)+(n2z*n2z)),0.5);
	double norm3 = pow(((n3x*n3x)+(n3y*n3y)+(n3z*n3z)),0.5);
	if(m_Grains[i].surfacegrain == 0)
	{
		n1x = n1x/norm1;
		n1y = n1y/norm1;
		n1z = n1z/norm1;
		n2x = n2x/norm2;
		n2y = n2y/norm2;
		n2z = n2z/norm2;
		n3x = n3x/norm3;
		n3y = n3y/norm3;
		n3z = n3z/norm3;
		for(int k = 0; k < 4; k++)
		{
			double o[3][3];
			double ga[3][3];
			double m1[3][3];
			if (k == 0)
			{
			  o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
			  o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
			  o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
			}
			else if (k == 1)
			{
			  o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
			  o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
			  o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
			}
			else if (k == 2)
			{
			  o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
			  o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
			  o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
			}
			else if (k == 3)
			{
			  o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
			  o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
			  o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
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
			m1[0][0] = o[0][0]*ga[0][0] + o[0][1]*ga[1][0] + o[0][2]*ga[2][0];
			m1[0][1] = o[0][0]*ga[0][1] + o[0][1]*ga[1][1] + o[0][2]*ga[2][1];
			m1[0][2] = o[0][0]*ga[0][2] + o[0][1]*ga[1][2] + o[0][2]*ga[2][2];
			m1[1][0] = o[1][0]*ga[0][0] + o[1][1]*ga[1][0] + o[1][2]*ga[2][0];
			m1[1][1] = o[1][0]*ga[0][1] + o[1][1]*ga[1][1] + o[1][2]*ga[2][1];
			m1[1][2] = o[1][0]*ga[0][2] + o[1][1]*ga[1][2] + o[1][2]*ga[2][2];
			m1[2][0] = o[2][0]*ga[0][0] + o[2][1]*ga[1][0] + o[2][2]*ga[2][0];
			m1[2][1] = o[2][0]*ga[0][1] + o[2][1]*ga[1][1] + o[2][2]*ga[2][1];
			m1[2][2] = o[2][0]*ga[0][2] + o[2][1]*ga[1][2] + o[2][2]*ga[2][2];
			double ea2 = acos(m1[2][2]);
			double cosine3 = (m1[1][2]/sin(ea2));
			double sine3 = (m1[0][2]/sin(ea2));
			double cosine1 = (-m1[2][1]/sin(ea2));
			double sine1 = (m1[2][0]/sin(ea2));
			double ea3 = acos(cosine3);
			double ea1 = acos(cosine1);
			if(sine3 < 0) ea3 = (2*m_pi)-ea3;
			if(sine1 < 0) ea1 = (2*m_pi)-ea1;
			int ea1bin = int(ea1/(m_pi/18));
			int ea2bin = int(ea2/(m_pi/18));
			int ea3bin = int(ea3/(m_pi/18));
			int bin=0;
			if(ea1 >= 0.0 && ea2 >= 0.0 && ea3 >= 0.0 && ea1 <= (m_pi) && ea2 <= (m_pi) && ea3 <= (m_pi))
			{
				m_Grains[i].axiseuler1 = ea1;
				m_Grains[i].axiseuler2 = ea2;
				m_Grains[i].axiseuler3 = ea3;
				bin = (ea3bin*18*18)+(ea2bin*18)+(ea1bin);
				axisodf[bin] = axisodf[bin] + 1.0;
				totalaxes = totalaxes+1.0;
			}
		}
    }
  }
  int err;
  err = h5io->writeAxisOrientationData(axisodf, totalaxes);
  delete [] axisodf;
}
void  ReconstructionFunc::find_moments2D()
{
//  int count = 0;
  double u200=0;
  double u020=0;
  double u110=0;
  grainmoments = new double *[numgrains];
  for(int i = 0; i < numgrains; i++)
  {
	grainmoments[i] = new double [3];
	for(int j=0;j<3;j++)
	{
		grainmoments[i][j] = 0;
	}
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
	  u200=0;
	  u020=0;
	  u110=0;
      int gnum = voxels[j].grainname;
      double x = find_xcoord(j);
      double y = find_ycoord(j);
      double x1 = x+(resx/2);
      double x2 = x-(resx/2);
      double y1 = y+(resy/2);
      double y2 = y-(resy/2);
      double xdist1 = (x1-graincenters[gnum][1]);
      double ydist1 = (y1-graincenters[gnum][2]);
      double xdist2 = (x1-graincenters[gnum][1]);
      double ydist2 = (y2-graincenters[gnum][2]);
      double xdist3 = (x2-graincenters[gnum][1]);
      double ydist3 = (y1-graincenters[gnum][2]);
      double xdist4 = (x2-graincenters[gnum][1]);
      double ydist4 = (y2-graincenters[gnum][2]);
      u200 = u200 + ((ydist1)*(ydist1)) + ((ydist2)*(ydist2)) + ((ydist3)*(ydist3)) + ((ydist4)*(ydist4));
      u020 = u020 + ((xdist1)*(xdist1)) + ((xdist2)*(xdist2)) + ((xdist3)*(xdist3)) + ((xdist4)*(xdist4));
      u110 = u110 + ((xdist1)*(ydist1)) + ((xdist2)*(ydist2)) + ((xdist3)*(ydist3)) + ((xdist4)*(ydist4));
	  grainmoments[gnum][0] = grainmoments[gnum][0] + u200;
	  grainmoments[gnum][1] = grainmoments[gnum][1] + u020;
	  grainmoments[gnum][2] = grainmoments[gnum][2] + u110;
  }
  for(int i=1;i<numgrains;i++)
  {
	grainmoments[i][0] = grainmoments[i][0]*(resx/2.0)*(resy/2.0);
	grainmoments[i][1] = grainmoments[i][1]*(resx/2.0)*(resy/2.0);
	grainmoments[i][2] = grainmoments[i][2]*(resx/2.0)*(resy/2.0);
//	double o3 = (grainmoments[i][0]*grainmoments[i][1]*grainmoments[i][2])+(2.0*grainmoments[i][3]*grainmoments[i][5]*grainmoments[i][4])-(grainmoments[i][0]*grainmoments[i][4]*grainmoments[i][4])-(grainmoments[i][1]*grainmoments[i][5]*grainmoments[i][5])-(grainmoments[i][2]*grainmoments[i][3]*grainmoments[i][3]);
//	double vol5 = m_Grains[i].volume;
//	vol5 = pow(vol5,5);
//	double omega3 = vol5/o3;
	m_Grains[i].Ixx = grainmoments[i][0];
	m_Grains[i].Iyy = grainmoments[i][1];
	m_Grains[i].Ixy = -grainmoments[i][2];
//	m_Grains[i].omega3 = omega3;
  }
}
void  ReconstructionFunc::find_axes2D()
{
  for (int i = 1; i < numgrains; i++)
  {
    double Ixx = m_Grains[i].Ixx;
    double Iyy = m_Grains[i].Iyy;
    double Ixy = m_Grains[i].Ixy;
    double r1 = (Ixx+Iyy)/2.0 + sqrt(((Ixx+Iyy)*(Ixx+Iyy))/4.0+(Ixy*Ixy-Ixx*Iyy));
    double r2 = (Ixx+Iyy)/2.0 - sqrt(((Ixx+Iyy)*(Ixx+Iyy))/4.0+(Ixy*Ixy-Ixx*Iyy));
	double preterm = 4/3.1415926535897;
	preterm = pow(preterm,0.25);
	double postterm1 = r1*r1*r1/r2;
	double postterm2 = r2*r2*r2/r1;
	postterm1 = pow(postterm1,0.125);
	postterm2 = pow(postterm2,0.125);
	r1 = preterm*postterm1;
	r2 = preterm*postterm2;
	m_Grains[i].radius1 = r1;
	m_Grains[i].radius2 = r2;
  }
}
void  ReconstructionFunc::find_vectors2D(H5ReconStatsWriter::Pointer h5io)
{
  totalaxes = 0.0;
  double *axisodf;
  axisodf = new double [18*18*18];
  for(int i = 1; i < numgrains; i++)
  {
 //   int size = grains[i].numvoxels;
    double Ixx = m_Grains[i].Ixx;
    double Iyy = m_Grains[i].Iyy;
    double Ixy = m_Grains[i].Ixy;
    double I1 = (Ixx+Iyy)/2.0 + sqrt(((Ixx+Iyy)*(Ixx+Iyy))/4.0+(Ixy*Ixy-Ixx*Iyy));
    double I2 = (Ixx+Iyy)/2.0 - sqrt(((Ixx+Iyy)*(Ixx+Iyy))/4.0+(Ixy*Ixy-Ixx*Iyy));
	double n1x = (Ixx-I1)/Ixy;
	double n1y = 1;
	double n2x = (Ixx-I2)/Ixy;
	double n2y = 1;
	double norm1 = pow((n1x*n1x+n1y*n1y),0.5);
	double norm2 = pow((n2x*n2x+n2y*n2y),0.5);
	n1x = n1x/norm1;
	n1y = n1y/norm1;
	n2x = n2x/norm2;
	n2y = n2y/norm2;
	double cosine1 = n1x;
	double ea1 = acos(cosine1);
	if(ea1 > m_pi) ea1 = ea1-m_pi;
	int ea1bin = int(ea1/(m_pi/18));
	int bin=0;
	if(ea1 >= 0.0 && ea1 < (m_pi))
	{
		m_Grains[i].axiseuler1 = ea1;
		bin = ea1bin;
		axisodf[bin] = axisodf[bin] + 1.0;
		totalaxes = totalaxes+1.0;
	}
  }
  int err;
  err = h5io->writeAxisOrientationData(axisodf, totalaxes);
  delete [] axisodf;
}
void  ReconstructionFunc::find_eulerodf (H5ReconStatsWriter::Pointer h5io)
{
	totalvol = 0;
	double *eulerodf;
	if(crystruct == AIM::Reconstruction::Hexagonal)
	{
		eulerodf = new double [36*36*12];
		for(int i=0;i<36*36*12;i++)
		{
			eulerodf[i] = 0.0;
		}
	}
	if(crystruct == AIM::Reconstruction::Cubic)
	{
		eulerodf = new double [18*18*18];
		for(int i=0;i<18*18*18;i++)
		{
			eulerodf[i] = 0.0;
		}
	}
	double a = sqrt(3.0)/2.0;
	double Oc[3][3];
	double Os[3][3];
	double ga[3][3];
	double m1[3][3];
	double m2[3][3];
	for(int i=1;i<numgrains;i++)
	{
		if(m_Grains[i].surfacegrain == 0)
		{
			double vol = m_Grains[i].volume;
			double ea1good = m_Grains[i].euler1;
			double ea2good = m_Grains[i].euler2;
			double ea3good = m_Grains[i].euler3;
			if(crystruct == AIM::Reconstruction::Hexagonal)
			{
				for(int k = 0; k < 12; k++)
				{
					if (k == 0)
					{
						Oc[0][0] = 1.0; Oc[0][1] = 0.0; Oc[0][2] = 0.0;
						Oc[1][0] = 0.0; Oc[1][1] = 1.0; Oc[1][2] = 0.0;
						Oc[2][0] = 0.0; Oc[2][1] = 0.0; Oc[2][2] = 1.0;
					}
					else if (k == 1)
					{
						Oc[0][0] = -0.5; Oc[0][1] = a; Oc[0][2] =  0.0;
						Oc[1][0] = -a; Oc[1][1] = -0.5; Oc[1][2] = 0.0;
						Oc[2][0] = 0.0; Oc[2][1] = 0.0; Oc[2][2] = 1.0;
					}
					else if (k == 2)
					{
						Oc[0][0] = -0.5; Oc[0][1] =  -a; Oc[0][2] =  0.0;
						Oc[1][0] = a; Oc[1][1] = -0.5; Oc[1][2] =  0.0;
						Oc[2][0] = 0.0; Oc[2][1] =  0.0; Oc[2][2] = 1.0;
					}
					else if (k == 3)
					{
						Oc[0][0] = 0.5; Oc[0][1] =  a; Oc[0][2] = 0.0;
						Oc[1][0] = -a; Oc[1][1] =  0.5; Oc[1][2] = 0.0;
						Oc[2][0] = 0.0; Oc[2][1] = 0.0; Oc[2][2] = 1.0;
					}
					else if (k == 4)
					{
						Oc[0][0] = -1.0; Oc[0][1] = 0.0; Oc[0][2] = 0.0;
						Oc[1][0] = 0.0; Oc[1][1] = -1.0; Oc[1][2] =  0.0;
						Oc[2][0] = 0.0; Oc[2][1] = 0.0; Oc[2][2] =  1.0;
					}
					else if (k == 5)
					{
						Oc[0][0] =  0.5; Oc[0][1] = -a; Oc[0][2] = 0.0;
						Oc[1][0] =  a; Oc[1][1] = 0.5; Oc[1][2] = 0.0;
						Oc[2][0] = 0.0; Oc[2][1] = 0.0; Oc[2][2] = 1.0;
					}
					else if (k == 6)
					{
						Oc[0][0] = -0.5; Oc[0][1] = -a; Oc[0][2] =  0.0;
						Oc[1][0] =  -a; Oc[1][1] = 0.5; Oc[1][2] =  0.0;
						Oc[2][0] =  0.0; Oc[2][1] = 0.0; Oc[2][2] = -1.0;
					}
					else if (k == 7)
					{
						Oc[0][0] = 1.0; Oc[0][1] =  0.0; Oc[0][2] = 0.0;
						Oc[1][0] =  0.0; Oc[1][1] = -1.0; Oc[1][2] = 0.0;
						Oc[2][0] =  0.0; Oc[2][1] =  0.0; Oc[2][2] = -1.0;
					}
					else if (k == 8)
					{
						Oc[0][0] =  -0.5; Oc[0][1] = a; Oc[0][2] = 0.0;
						Oc[1][0] = a; Oc[1][1] = 0.5; Oc[1][2] = 0.0;
						Oc[2][0] =  0.0; Oc[2][1] = 0.0; Oc[2][2] = -1.0;
					}
					else if (k == 9)
					{
						Oc[0][0] = 0.5; Oc[0][1] = a; Oc[0][2] = 0.0;
						Oc[1][0] = a; Oc[1][1] =  -0.5; Oc[1][2] = 0.0;
						Oc[2][0] = 0.0; Oc[2][1] =  0.0; Oc[2][2] = -1.0;
					}
					else if (k == 10)
					{
						Oc[0][0] =  -1.0; Oc[0][1] = 0.0; Oc[0][2] = 0.0;
						Oc[1][0] =  0.0; Oc[1][1] =  1.0; Oc[1][2] = 0.0;
						Oc[2][0] = 0.0; Oc[2][1] =  0.0; Oc[2][2] = -1.0;
					}
					else
					{
						Oc[0][0] =  0.5; Oc[0][1] =  -a; Oc[0][2] = 0.0;
						Oc[1][0] = -a; Oc[1][1] =  -0.5; Oc[1][2] = 0.0;
						Oc[2][0] =  0.0; Oc[2][1] = 0.0; Oc[2][2] = -1.0;
					}
					ga[0][0] = cos(ea1good)*cos(ea3good)-sin(ea1good)*sin(ea3good)*cos(ea2good);
					ga[0][1] = sin(ea1good)*cos(ea3good)+cos(ea1good)*sin(ea3good)*cos(ea2good);
					ga[0][2] = sin(ea3good)*sin(ea2good);
					ga[1][0] = -cos(ea1good)*sin(ea3good)-sin(ea1good)*cos(ea3good)*cos(ea2good);
					ga[1][1] = -sin(ea1good)*sin(ea3good)+cos(ea1good)*cos(ea3good)*cos(ea2good);
					ga[1][2] =  cos(ea3good)*sin(ea2good);
					ga[2][0] =  sin(ea1good)*sin(ea2good);
					ga[2][1] = -cos(ea1good)*sin(ea2good);
					ga[2][2] =  cos(ea2good);
					m1[0][0] = Oc[0][0]*ga[0][0] + Oc[0][1]*ga[1][0] + Oc[0][2]*ga[2][0];
					m1[0][1] = Oc[0][0]*ga[0][1] + Oc[0][1]*ga[1][1] + Oc[0][2]*ga[2][1];
					m1[0][2] = Oc[0][0]*ga[0][2] + Oc[0][1]*ga[1][2] + Oc[0][2]*ga[2][2];
					m1[1][0] = Oc[1][0]*ga[0][0] + Oc[1][1]*ga[1][0] + Oc[1][2]*ga[2][0];
					m1[1][1] = Oc[1][0]*ga[0][1] + Oc[1][1]*ga[1][1] + Oc[1][2]*ga[2][1];
					m1[1][2] = Oc[1][0]*ga[0][2] + Oc[1][1]*ga[1][2] + Oc[1][2]*ga[2][2];
					m1[2][0] = Oc[2][0]*ga[0][0] + Oc[2][1]*ga[1][0] + Oc[2][2]*ga[2][0];
					m1[2][1] = Oc[2][0]*ga[0][1] + Oc[2][1]*ga[1][1] + Oc[2][2]*ga[2][1];
					m1[2][2] = Oc[2][0]*ga[0][2] + Oc[2][1]*ga[1][2] + Oc[2][2]*ga[2][2];
					for(int l=0;l<4;l++)
					{
						if (l == 0)
						{
						  Os[0][0] = 1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
						  Os[1][0] = 0.0; Os[1][1] = 1.0; Os[1][2] = 0.0;
						  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = 1.0;
						}
						else if (l == 1)
						{
						  Os[0][0] = -1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
						  Os[1][0] = 0.0; Os[1][1] = 1.0; Os[1][2] = 0.0;
						  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = -1.0;
						}
						else if (l == 2)
						{
						  Os[0][0] = 1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
						  Os[1][0] = 0.0; Os[1][1] = -1.0; Os[1][2] = 0.0;
						  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = -1.0;
						}
						else if (l == 3)
						{
						  Os[0][0] = -1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
						  Os[1][0] = 0.0; Os[1][1] = -1.0; Os[1][2] = 0.0;
						  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = 1.0;
						}
						m2[0][0] = Os[0][0]*m1[0][0] + Os[1][0]*m1[0][1] + Os[2][0]*m1[0][2];
						m2[0][1] = Os[0][1]*m1[0][0] + Os[1][1]*m1[0][1] + Os[2][1]*m1[0][2];
						m2[0][2] = Os[0][2]*m1[0][0] + Os[1][2]*m1[0][1] + Os[2][2]*m1[0][2];
						m2[1][0] = Os[0][0]*m1[1][0] + Os[1][0]*m1[1][1] + Os[2][0]*m1[1][2];
						m2[1][1] = Os[0][1]*m1[1][0] + Os[1][1]*m1[1][1] + Os[2][1]*m1[1][2];
						m2[1][2] = Os[0][2]*m1[1][0] + Os[1][2]*m1[1][1] + Os[2][2]*m1[1][2];
						m2[2][0] = Os[0][0]*m1[2][0] + Os[1][0]*m1[2][1] + Os[2][0]*m1[2][2];
						m2[2][1] = Os[0][1]*m1[2][0] + Os[1][1]*m1[2][1] + Os[2][1]*m1[2][2];
						m2[2][2] = Os[0][2]*m1[2][0] + Os[1][2]*m1[2][1] + Os[2][2]*m1[2][2];
						double ea2 = acos(m2[2][2]);
						double cosine3 = (m2[1][2]/sin(ea2));
						double sine3 = (m2[0][2]/sin(ea2));
						double cosine1 = (-m2[2][1]/sin(ea2));
						double sine1 = (m2[2][0]/sin(ea2));
						double ea3 = acos(cosine3);
						double ea1 = acos(cosine1);
						if(sine3 < 0) ea3 = (2*m_pi)-ea3;
						if(sine1 < 0) ea1 = (2*m_pi)-ea1;
						int ea1bin = int(ea1/(m_pi/36));
						int ea2bin = int(ea2/(m_pi/36));
						int ea3bin = int(ea3/(m_pi/36));
						int bin=0;
						if(ea1 >= 0.0 && ea2 >= 0.0 && ea3 >= 0.0 && ea1 <= (m_pi) && ea2 <= (m_pi) && ea3 <= (m_pi/3.0))
						{
						  bin = (ea3bin*36*36)+(ea2bin*36)+(ea1bin);
						  eulerodf[bin] = eulerodf[bin] + vol;
						  totalvol = totalvol + vol;
						}
					}
				}
			}
			if(crystruct == AIM::Reconstruction::Cubic)
			{
				for(int k = 0; k < 24; k++)
				{
					if (k == 0)
					{
					  Oc[0][0] = 1.0; Oc[0][1] = 0.0; Oc[0][2] = 0.0;
					  Oc[1][0] = 0.0; Oc[1][1] = 1.0; Oc[1][2] = 0.0;
					  Oc[2][0] = 0.0; Oc[2][1] = 0.0; Oc[2][2] = 1.0;
					}
					else if (k == 1)
					{
					  Oc[0][0] = 1.0; Oc[0][1] = 0.0; Oc[0][2] =  0.0;
					  Oc[1][0] = 0.0; Oc[1][1] = 0.0; Oc[1][2] = -1.0;
					  Oc[2][0] = 0.0; Oc[2][1] = 1.0; Oc[2][2] =  0.0;
					}
					else if (k == 2)
					{
					  Oc[0][0] = 1.0; Oc[0][1] =  0.0; Oc[0][2] =  0.0;
					  Oc[1][0] = 0.0; Oc[1][1] = -1.0; Oc[1][2] =  0.0;
					  Oc[2][0] = 0.0; Oc[2][1] =  0.0; Oc[2][2] = -1.0;
					}
					else if (k == 3)
					{
					  Oc[0][0] = 1.0; Oc[0][1] =  0.0; Oc[0][2] = 0.0;
					  Oc[1][0] = 0.0; Oc[1][1] =  0.0; Oc[1][2] = 1.0;
					  Oc[2][0] = 0.0; Oc[2][1] = -1.0; Oc[2][2] = 0.0;
					}
					else if (k == 4)
					{
					  Oc[0][0] = 0.0; Oc[0][1] = 0.0; Oc[0][2] = -1.0;
					  Oc[1][0] = 0.0; Oc[1][1] = 1.0; Oc[1][2] =  0.0;
					  Oc[2][0] = 1.0; Oc[2][1] = 0.0; Oc[2][2] =  0.0;
					}
					else if (k == 5)
					{
					  Oc[0][0] =  0.0; Oc[0][1] = 0.0; Oc[0][2] = 1.0;
					  Oc[1][0] =  0.0; Oc[1][1] = 1.0; Oc[1][2] = 0.0;
					  Oc[2][0] = -1.0; Oc[2][1] = 0.0; Oc[2][2] = 0.0;
					}
					else if (k == 6)
					{
					  Oc[0][0] = -1.0; Oc[0][1] = 0.0; Oc[0][2] =  0.0;
					  Oc[1][0] =  0.0; Oc[1][1] = 1.0; Oc[1][2] =  0.0;
					  Oc[2][0] =  0.0; Oc[2][1] = 0.0; Oc[2][2] = -1.0;
					}
					else if (k == 7)
					{
					  Oc[0][0] = -1.0; Oc[0][1] =  0.0; Oc[0][2] = 0.0;
					  Oc[1][0] =  0.0; Oc[1][1] = -1.0; Oc[1][2] = 0.0;
					  Oc[2][0] =  0.0; Oc[2][1] =  0.0; Oc[2][2] = 1.0;
					}
					else if (k == 8)
					{
					  Oc[0][0] =  0.0; Oc[0][1] = 1.0; Oc[0][2] = 0.0;
					  Oc[1][0] = -1.0; Oc[1][1] = 0.0; Oc[1][2] = 0.0;
					  Oc[2][0] =  0.0; Oc[2][1] = 0.0; Oc[2][2] = 1.0;
					}
					else if (k == 9)
					{
					  Oc[0][0] = 0.0; Oc[0][1] = -1.0; Oc[0][2] = 0.0;
					  Oc[1][0] = 1.0; Oc[1][1] =  0.0; Oc[1][2] = 0.0;
					  Oc[2][0] = 0.0; Oc[2][1] =  0.0; Oc[2][2] = 1.0;
					}
					else if (k == 10)
					{
					  Oc[0][0] =  0.0; Oc[0][1] = -1.0; Oc[0][2] = 0.0;
					  Oc[1][0] =  0.0; Oc[1][1] =  0.0; Oc[1][2] = 1.0;
					  Oc[2][0] = -1.0; Oc[2][1] =  0.0; Oc[2][2] = 0.0;
					}
					else if (k == 11)
					{
					  Oc[0][0] =  0.0; Oc[0][1] =  0.0; Oc[0][2] = 1.0;
					  Oc[1][0] = -1.0; Oc[1][1] =  0.0; Oc[1][2] = 0.0;
					  Oc[2][0] =  0.0; Oc[2][1] = -1.0; Oc[2][2] = 0.0;
					}
					else if (k == 12)
					{
					  Oc[0][0] = 0.0; Oc[0][1] = -1.0; Oc[0][2] =  0.0;
					  Oc[1][0] = 0.0; Oc[1][1] =  0.0; Oc[1][2] = -1.0;
					  Oc[2][0] = 1.0; Oc[2][1] =  0.0; Oc[2][2] =  0.0;
					}
					else if (k == 13)
					{
					  Oc[0][0] = 0.0; Oc[0][1] =  0.0; Oc[0][2] = -1.0;
					  Oc[1][0] = 1.0; Oc[1][1] =  0.0; Oc[1][2] =  0.0;
					  Oc[2][0] = 0.0; Oc[2][1] = -1.0; Oc[2][2] =  0.0;
					}
					else if (k == 14)
					{
					  Oc[0][0] =  0.0; Oc[0][1] = 1.0; Oc[0][2] =  0.0;
					  Oc[1][0] =  0.0; Oc[1][1] = 0.0; Oc[1][2] = -1.0;
					  Oc[2][0] = -1.0; Oc[2][1] = 0.0; Oc[2][2] =  0.0;
					}
					else if (k == 15)
					{
					  Oc[0][0] =  0.0; Oc[0][1] = 0.0; Oc[0][2] = -1.0;
					  Oc[1][0] = -1.0; Oc[1][1] = 0.0; Oc[1][2] =  0.0;
					  Oc[2][0] =  0.0; Oc[2][1] = 1.0; Oc[2][2] =  0.0;
					}
					else if (k == 16)
					{
					  Oc[0][0] = 0.0; Oc[0][1] = 1.0; Oc[0][2] = 0.0;
					  Oc[1][0] = 0.0; Oc[1][1] = 0.0; Oc[1][2] = 1.0;
					  Oc[2][0] = 1.0; Oc[2][1] = 0.0; Oc[2][2] = 0.0;
					}
					else if (k == 17)
					{
					  Oc[0][0] = 0.0; Oc[0][1] = 0.0; Oc[0][2] = 1.0;
					  Oc[1][0] = 1.0; Oc[1][1] = 0.0; Oc[1][2] = 0.0;
					  Oc[2][0] = 0.0; Oc[2][1] = 1.0; Oc[2][2] = 0.0;
					}
					else if (k == 18)
					{
					  Oc[0][0] = 0.0; Oc[0][1] = 1.0; Oc[0][2] =  0.0;
					  Oc[1][0] = 1.0; Oc[1][1] = 0.0; Oc[1][2] =  0.0;
					  Oc[2][0] = 0.0; Oc[2][1] = 0.0; Oc[2][2] = -1.0;
					}
					else if (k == 19)
					{
					  Oc[0][0] = -1.0; Oc[0][1] = 0.0; Oc[0][2] = 0.0;
					  Oc[1][0] =  0.0; Oc[1][1] = 0.0; Oc[1][2] = 1.0;
					  Oc[2][0] =  0.0; Oc[2][1] = 1.0; Oc[2][2] = 0.0;
					}
					else if (k == 20)
					{
					  Oc[0][0] = 0.0; Oc[0][1] =  0.0; Oc[0][2] = 1.0;
					  Oc[1][0] = 0.0; Oc[1][1] = -1.0; Oc[1][2] = 0.0;
					  Oc[2][0] = 1.0; Oc[2][1] =  0.0; Oc[2][2] = 0.0;
					}
					else if (k == 21)
					{
					  Oc[0][0] = -1.0; Oc[0][1] =  0.0; Oc[0][2] =  0.0;
					  Oc[1][0] =  0.0; Oc[1][1] =  0.0; Oc[1][2] = -1.0;
					  Oc[2][0] =  0.0; Oc[2][1] = -1.0; Oc[2][2] =  0.0;
					}
					else if (k == 22)
					{
					  Oc[0][0] =  0.0; Oc[0][1] =  0.0; Oc[0][2] = -1.0;
					  Oc[1][0] =  0.0; Oc[1][1] = -1.0; Oc[1][2] =  0.0;
					  Oc[2][0] = -1.0; Oc[2][1] =  0.0; Oc[2][2] =  0.0;
					}
					else if (k == 23)
					{
					  Oc[0][0] =  0.0; Oc[0][1] = -1.0; Oc[0][2] =  0.0;
					  Oc[1][0] = -1.0; Oc[1][1] =  0.0; Oc[1][2] =  0.0;
					  Oc[2][0] =  0.0; Oc[2][1] =  0.0; Oc[2][2] = -1.0;
					}
					ga[0][0] = cos(ea1good)*cos(ea3good)-sin(ea1good)*sin(ea3good)*cos(ea2good);
					ga[0][1] = sin(ea1good)*cos(ea3good)+cos(ea1good)*sin(ea3good)*cos(ea2good);
					ga[0][2] = sin(ea3good)*sin(ea2good);
					ga[1][0] = -cos(ea1good)*sin(ea3good)-sin(ea1good)*cos(ea3good)*cos(ea2good);
					ga[1][1] = -sin(ea1good)*sin(ea3good)+cos(ea1good)*cos(ea3good)*cos(ea2good);
					ga[1][2] =  cos(ea3good)*sin(ea2good);
					ga[2][0] =  sin(ea1good)*sin(ea2good);
					ga[2][1] = -cos(ea1good)*sin(ea2good);
					ga[2][2] =  cos(ea2good);
					m1[0][0] = Oc[0][0]*ga[0][0] + Oc[0][1]*ga[1][0] + Oc[0][2]*ga[2][0];
					m1[0][1] = Oc[0][0]*ga[0][1] + Oc[0][1]*ga[1][1] + Oc[0][2]*ga[2][1];
					m1[0][2] = Oc[0][0]*ga[0][2] + Oc[0][1]*ga[1][2] + Oc[0][2]*ga[2][2];
					m1[1][0] = Oc[1][0]*ga[0][0] + Oc[1][1]*ga[1][0] + Oc[1][2]*ga[2][0];
					m1[1][1] = Oc[1][0]*ga[0][1] + Oc[1][1]*ga[1][1] + Oc[1][2]*ga[2][1];
					m1[1][2] = Oc[1][0]*ga[0][2] + Oc[1][1]*ga[1][2] + Oc[1][2]*ga[2][2];
					m1[2][0] = Oc[2][0]*ga[0][0] + Oc[2][1]*ga[1][0] + Oc[2][2]*ga[2][0];
					m1[2][1] = Oc[2][0]*ga[0][1] + Oc[2][1]*ga[1][1] + Oc[2][2]*ga[2][1];
					m1[2][2] = Oc[2][0]*ga[0][2] + Oc[2][1]*ga[1][2] + Oc[2][2]*ga[2][2];
					for(int l=0;l<4;l++)
					{
						if (l == 0)
						{
						  Os[0][0] = 1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
						  Os[1][0] = 0.0; Os[1][1] = 1.0; Os[1][2] = 0.0;
						  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = 1.0;
						}
						else if (l == 1)
						{
						  Os[0][0] = -1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
						  Os[1][0] = 0.0; Os[1][1] = 1.0; Os[1][2] = 0.0;
						  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = -1.0;
						}
						else if (l == 2)
						{
						  Os[0][0] = 1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
						  Os[1][0] = 0.0; Os[1][1] = -1.0; Os[1][2] = 0.0;
						  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = -1.0;
						}
						else if (l == 3)
						{
						  Os[0][0] = -1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
						  Os[1][0] = 0.0; Os[1][1] = -1.0; Os[1][2] = 0.0;
						  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = 1.0;
						}
						m2[0][0] = Os[0][0]*m1[0][0] + Os[1][0]*m1[0][1] + Os[2][0]*m1[0][2];
						m2[0][1] = Os[0][1]*m1[0][0] + Os[1][1]*m1[0][1] + Os[2][1]*m1[0][2];
						m2[0][2] = Os[0][2]*m1[0][0] + Os[1][2]*m1[0][1] + Os[2][2]*m1[0][2];
						m2[1][0] = Os[0][0]*m1[1][0] + Os[1][0]*m1[1][1] + Os[2][0]*m1[1][2];
						m2[1][1] = Os[0][1]*m1[1][0] + Os[1][1]*m1[1][1] + Os[2][1]*m1[1][2];
						m2[1][2] = Os[0][2]*m1[1][0] + Os[1][2]*m1[1][1] + Os[2][2]*m1[1][2];
						m2[2][0] = Os[0][0]*m1[2][0] + Os[1][0]*m1[2][1] + Os[2][0]*m1[2][2];
						m2[2][1] = Os[0][1]*m1[2][0] + Os[1][1]*m1[2][1] + Os[2][1]*m1[2][2];
						m2[2][2] = Os[0][2]*m1[2][0] + Os[1][2]*m1[2][1] + Os[2][2]*m1[2][2];
						double ea2 = acos(m2[2][2]);
						double cosine3 = (m2[1][2]/sin(ea2));
						double sine3 = (m2[0][2]/sin(ea2));
						double cosine1 = (-m2[2][1]/sin(ea2));
						double sine1 = (m2[2][0]/sin(ea2));
						double ea3 = acos(cosine3);
						double ea1 = acos(cosine1);
						if(sine3 < 0) ea3 = (2*m_pi)-ea3;
						if(sine1 < 0) ea1 = (2*m_pi)-ea1;
						int ea1bin = int(ea1/(m_pi/36));
						int ea2bin = int(ea2/(m_pi/36));
						int ea3bin = int(ea3/(m_pi/36));
						int bin=0;
						if(ea1 >= 0.0 && ea2 >= 0.0 && ea3 >= 0.0 && ea1 <= (m_pi/2.0) && ea2 <= (m_pi/2.0) && ea3 <= (m_pi/2.0))
						{
						  bin = (ea3bin*18*18)+(ea2bin*18)+(ea1bin);
						  eulerodf[bin] = eulerodf[bin] + vol;
						  totalvol = totalvol + vol;
						}
					}
				}
			}
		}
	}
	int err;
	err = h5io->writeODFData(crystruct, eulerodf, totalvol);
	double delta = m_pi / 18;
	double texindex = 0;
	double texstrength = 0;
	int bin = 0;
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
	delete [] eulerodf;
}
void  ReconstructionFunc::measure_misorientations (H5ReconStatsWriter::Pointer h5io)
{
  size_t initialsize = 10;
  vector<double > misolist(initialsize, -1);
  double degtorad = m_pi/180.0;
  double dim = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));
  double n1, n2, n3;
  int miso1bin, miso2bin, miso3bin;
  double w;
  double q1[5];
  double q2[5];
  double axis[3];
  double denom = 0;
  vector<int >* nlist;
  for (int i = 1; i < numgrains; i++)
  {
    nlist = m_Grains[i].neighborlist;
    q1[1] = m_Grains[i].avg_quat[1];
    q1[2] = m_Grains[i].avg_quat[2];
    q1[3] = m_Grains[i].avg_quat[3];
    q1[4] = m_Grains[i].avg_quat[4];
    int size = 0;
    if (NULL != nlist)
    {
      size = nlist->size();
    }
    misolist.resize(size*3, -1);
    for (int j = 0; j < size; j++)
    {
      int nname = nlist->at(j);
	  if(nname > 0)
	  {
	      q2[1] = m_Grains[nname].avg_quat[1];
	      q2[2] = m_Grains[nname].avg_quat[2];
	      q2[3] = m_Grains[nname].avg_quat[3];
	      q2[4] = m_Grains[nname].avg_quat[4];
	      if (crystruct == AIM::Reconstruction::Hexagonal)
	      {
	        w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
	      }
	      if (crystruct == AIM::Reconstruction::Cubic)
	      {
	        w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
	      }
		  w = w*degtorad;
		  denom = (n1*n1)+(n2*n2)+(n3*n3);
		  denom = pow(denom,0.5);
		  n1 = n1/denom;
		  n2 = n2/denom;
		  n3 = n3/denom;
	      if ( n1 >= n2 && n1 >= n3)
	      {
	        axis[0] = n1;
	        if (n2 > n3) { axis[1] = n2; axis[2] = n3; }
	        else { axis[1] = n3; axis[2] = n2; }
	      }
	      else if ( n2 >= n1 && n2 >= n3)
	      {
	        axis[0] = n2;
	        if (n1 > n3) { axis[1] = n1; axis[2] = n3; }
	        else { axis[1] = n3; axis[2] = n1; }
	      }
	      else if ( n1 >= n2 )
	      {
	        axis[1] = n1; axis[2] = n2; axis[0] = n3;
	      }
	      else { axis[2] = n1; axis[1] = n2; axis[0] = n3;}
		  if(w == 0) axis[0] = 0.0, axis[1] = 0.0, axis[2] = 0.0;
		  misolist[3*j] = axis[0]*pow(((3.0/4.0)*(w-sin(w))),(1.0/3.0));
	      misolist[3*j+1] = axis[1]*pow(((3.0/4.0)*(w-sin(w))),(1.0/3.0));
	      misolist[3*j+2] = axis[2]*pow(((3.0/4.0)*(w-sin(w))),(1.0/3.0));
	    }
	}
    m_Grains[i].misorientationlist = new std::vector<double >(misolist.size());
    m_Grains[i].misorientationlist->swap(misolist);
    misolist.clear();
  }
  double actualgrains = 0;
  double misocount = 0;
  double *misobin;
  misobin = new double [18*18*18];
  double microbin[10];
  for (int e = 0; e < 18*18*18; e++)
  {
    misobin[e] = 0;
    if (e < 10) microbin[e] = 0;
  }
  int neigh, size;
  double microcount = 0;
  double nsa, miso1, miso2, miso3;
  vector<double >* mlist;
  vector<double >* neighborsurfarealist;
  for (int l = 1; l < numgrains; l++)
  {
    if (m_Grains[l].surfacegrain == 0)
    {
      microcount = 0;
      nlist = m_Grains[l].neighborlist;
      mlist = m_Grains[l].misorientationlist;
      neighborsurfarealist = m_Grains[l].neighborsurfarealist;
      size = int(nlist->size());
      for (int k = 0; k < size; k++)
      {
        neigh = nlist->at(k);
        miso1 = mlist->at(3*k);
        miso2 = mlist->at(3*k+1);
        miso3 = mlist->at(3*k+2);
        nsa = neighborsurfarealist->at(k);
		miso1bin = int(miso1*18.0/dim);
		miso2bin = int(miso2*18.0/dim);
		miso3bin = int(miso3*18.0/dim);
        if (miso1 < 15) microcount++;
        if (neigh > l || m_Grains[neigh].surfacegrain == 1)
        {
          misobin[(18*18*miso3bin)+(18*miso2bin)+miso1bin] = misobin[(18*18*miso3bin)+(18*miso2bin)+miso1bin] + (nsa / totalsurfacearea);
          misocount++;
        }
      }
      if (size != 0)
      {
        microcount = microcount / size;
      }
      else
      {
        microcount = 0;
      }
      int microcur = int(microcount / 0.1);
      if (microcur == 10) microcur = 9;
      microbin[microcur]++;
    }
  }
  h5io->writeMisorientationBinsData(misobin, 18*18*18);
  h5io->writeMicroTextureData(microbin, 10, actualgrains);
 // delete [] misobin;
}


void  ReconstructionFunc::find_colors()
{
  double red, green, blue;
  unsigned char rgb[3] = {0, 0, 0};
  double RefDirection[3] = {0.0, 0.0, 1.0};
  for (int i = 1; i < numgrains; i++)
  {
    double g1ea1 = m_Grains[i].euler1;
    double g1ea2 = m_Grains[i].euler2;
    double g1ea3 = m_Grains[i].euler3;
    double q1[4];

    // Create a Unit Quaterion based on the Euler Angles
    q1[0] = sin((g1ea2 / 2.0)) * cos(((g1ea1 - g1ea3) / 2.0));
    q1[1] = sin((g1ea2 / 2.0)) * sin(((g1ea1 - g1ea3) / 2.0));
    q1[2] = cos((g1ea2 / 2.0)) * sin(((g1ea1 + g1ea3) / 2.0));
    q1[3] = cos((g1ea2 / 2.0)) * cos(((g1ea1 + g1ea3) / 2.0));
    if (crystruct == AIM::Reconstruction::Cubic)
    {
      OIMColoring::GenerateIPFColor(g1ea1, g1ea2, g1ea3, RefDirection[0], RefDirection[1], RefDirection[2], rgb);

      m_Grains[i].red = static_cast<double>(rgb[0]/255.0);
      m_Grains[i].green = static_cast<double>(rgb[1]/255.0);;
      m_Grains[i].blue = static_cast<double>(rgb[2]/255.0);;
    }

    if (crystruct == AIM::Reconstruction::Hexagonal)
    {
      red = 1.0 / 3.0;
      green = 1.0 / 3.0;
      blue = 1.0 / 3.0;
      OIMColoring::CalculateHexIPFColor(q1, rgb);
      m_Grains[i].red = red/255.0;
      m_Grains[i].green = green/255.0;
      m_Grains[i].blue = blue/255.0;
    }
  }
}
void  ReconstructionFunc::find_convexities()
{
//  double convexity = 1;
  for(int i = 1; i < numgrains; i++)
  {
    int size = m_Grains[i].numvoxels;
    if(size > 1)
    {
      double insidecount = 0;
      int vol = m_Grains[i].numvoxels;
      double voxvol = vol*resx*resy*resz;
      double xc = m_Grains[i].centroidx;
      double yc = m_Grains[i].centroidy;
      double zc = m_Grains[i].centroidz;
	  double phi1 = m_Grains[i].axiseuler1;
	  double PHI = m_Grains[i].axiseuler2;
	  double phi2 = m_Grains[i].axiseuler3;
	  double ga[3][3];
	  ga[0][0] = cos(phi1)*cos(phi2)-sin(phi1)*sin(phi2)*cos(PHI);
	  ga[0][1] = sin(phi1)*cos(phi2)+cos(phi1)*sin(phi2)*cos(PHI);
	  ga[0][2] = sin(phi2)*sin(PHI);
	  ga[1][0] = -cos(phi1)*sin(phi2)-sin(phi1)*cos(phi2)*cos(PHI);
	  ga[1][1] = -sin(phi1)*sin(phi2)+cos(phi1)*cos(phi2)*cos(PHI);
	  ga[1][2] =  cos(phi2)*sin(PHI);
	  ga[2][0] =  sin(phi1)*sin(PHI);
	  ga[2][1] = -cos(phi1)*sin(PHI);
	  ga[2][2] =  cos(PHI);
      double rad1x = ga[0][0];
      double rad1y = ga[1][0];
      double rad1z = ga[2][0];
      double rad2x = ga[0][1];
      double rad2y = ga[1][1];
      double rad2z = ga[2][1];
      double rad3x = ga[0][2];
      double rad3y = ga[1][2];
      double rad3z = ga[2][2];
      double mag1 = (rad1x*rad1x)+(rad1y*rad1y)+(rad1z*rad1z);
      mag1 = pow(mag1,0.5);
      double mag2 = (rad2x*rad2x)+(rad2y*rad2y)+(rad2z*rad2z);
      mag2 = pow(mag2,0.5);
      double mag3 = (rad3x*rad3x)+(rad3y*rad3y)+(rad3z*rad3z);
      mag3 = pow(mag3,0.5);
      rad1x = rad1x/mag1;
      rad1y = rad1y/mag1;
      rad1z = rad1z/mag1;
      rad2x = rad2x/mag2;
      rad2y = rad2y/mag2;
      rad2z = rad2z/mag2;
      rad3x = rad3x/mag3;
      rad3y = rad3y/mag3;
      rad3z = rad3z/mag3;
      double I1 = m_Grains[i].radius1;
      double I2 = m_Grains[i].radius2;
      double I3 = m_Grains[i].radius3;
      I1 = (15*I1)/(4*m_pi);
      I2 = (15*I2)/(4*m_pi);
      I3 = (15*I3)/(4*m_pi);
      double A = (I1+I2-I3)/2;
      double B = (I1+I3-I2)/2;
      double C = (I2+I3-I1)/2;
      double a = (A*A*A*A)/(B*C);
      a = pow(a,0.1);
      double b = B/A;
      b = pow(b,0.5)*a;
      double c = A/(a*a*a*b);
      double bovera = b/a;
      double covera = c/a;
      double rad_1 = voxvol/(m_OnePointThree * m_pi * bovera * covera );
      //double rad_1 = voxvol/(m_OnePointThree*m_pi*bovera*covera);
      rad_1 = pow(rad_1,0.3333333);
      double rad_2 = rad_1*bovera;
      double rad_3 = rad_1*covera;
      if (NULL == m_Grains[i].voxellist)
      {
        m_Grains[i].voxellist = new std::vector<int>;
      }
	  vector<int>* voxellist = m_Grains[i].voxellist;
	  int size = voxellist->size();
	  for(int j=0;j<size;j++)
	  {
		  int point = voxellist->at(j);
          double x = find_xcoord(point);
	      double y = find_ycoord(point);
	      double z = find_zcoord(point);
          double axis[3][3];
          double diff[3][1];
          double axiselim[3][3];
          double diffelim[3][1];
          double constmat[3][1];
          axis[0][0] = rad1x;
          axis[0][1] = rad2x;
          axis[0][2] = rad3x;
          axis[1][0] = rad1y;
          axis[1][1] = rad2y;
          axis[1][2] = rad3y;
          axis[2][0] = rad1z;
          axis[2][1] = rad2z;
          axis[2][2] = rad3z;
          diff[0][0] = x-xc;
          diff[1][0] = y-yc;
          diff[2][0] = z-zc;
          int elimcount = 0;
          int elimcount1 = 0;
          double q = 0;
          double sum = 0;
          double c = 0;
          for(int a = 0; a < 3; a++)
          {
            elimcount1 = 0;
            for(int b = 0; b < 3; b++)
            {
              axiselim[elimcount][elimcount1] = axis[a][b];
              if(axiselim[elimcount][elimcount1] == 0)
              {
                axiselim[elimcount][elimcount1] = 0.000001;
              }
              elimcount1++;
            }
            diffelim[elimcount][0] = diff[a][0];
            elimcount++;
          }
          for(int k = 0; k < elimcount-1; k++)
          {
            for(int l = k+1; l < elimcount; l++)
            {
              c = axiselim[l][k]/axiselim[k][k];
              for(int m = k+1; m < elimcount; m++)
              {
                axiselim[l][m] = axiselim[l][m] - c*axiselim[k][m];
              }
              diffelim[l][0] = diffelim[l][0] - c*diffelim[k][0];
            }
          }
          diffelim[elimcount-1][0] = diffelim[elimcount-1][0]/axiselim[elimcount-1][elimcount-1];
          for(int l = 1; l < elimcount; l++)
          {
            int m = (elimcount-1)-l;
            sum = 0;
            for(int n = m+1; n < elimcount; n++)
            {
              sum = sum + (axiselim[m][n]*diffelim[n][0]);
            }
            diffelim[m][0] = (diffelim[m][0]-sum)/axiselim[m][m];
          }
          for(int p = 0; p < elimcount; p++)
          {
            q = diffelim[p][0];
            constmat[p][0] = q;
          }
          double inside = 1-(((constmat[0][0])/rad_3)*((constmat[0][0])/rad_3))-(((constmat[1][0])/rad_2)*((constmat[1][0])/rad_2))-(((constmat[2][0])/rad_1)*((constmat[2][0])/rad_1));
          if(inside >= 0)
          {
            insidecount++;
          }
      }
      double convex = insidecount/size;
      m_Grains[i].convexity = convex;
    }
    if(size == 1)
    {
      double convex = 1;
      m_Grains[i].convexity = convex;
    }
  }
}
void ReconstructionFunc::find_schmids()
{
  double schmid=0;
  double loadx,loady,loadz;
  double theta1,theta2,theta3,theta4;
  double lambda1,lambda2,lambda3,lambda4,lambda5,lambda6;
  double schmid1,schmid2,schmid3,schmid4,schmid5,schmid6,schmid7,schmid8,schmid9,schmid10,schmid11,schmid12;
  for(int i=1;i<numgrains;i++)
  {
	    double g1ea1 = m_Grains[i].euler1;
	    double g1ea2 = m_Grains[i].euler2;
	    double g1ea3 = m_Grains[i].euler3;
	    double q1[4];
	 //   double p[3];
		q1[0]=sin((g1ea2/2.0))*cos(((g1ea1-g1ea3)/2.0));
		q1[1]=sin((g1ea2/2.0))*sin(((g1ea1-g1ea3)/2.0));
		q1[2]=cos((g1ea2/2.0))*sin(((g1ea1+g1ea3)/2.0));
		q1[3]=cos((g1ea2/2.0))*cos(((g1ea1+g1ea3)/2.0));
	    loadx = (2*q1[0]*q1[2]+2*q1[1]*q1[3])*1;
		loady = (2*q1[1]*q1[2]+2*q1[0]*q1[3])*1;
		loadz = (1-2*q1[0]*q1[0]-2*q1[1]*q1[1])*1;
		double mag = loadx*loadx+loady*loady+loadz*loadz;
		mag = pow(mag,0.5);
		theta1 = (loadx+loady+loadz)/(mag*1.732);
		theta1 = fabs(theta1);
		theta2 = (loadx+loady-loadz)/(mag*1.732);
		theta2 = fabs(theta2);
		theta3 = (loadx-loady+loadz)/(mag*1.732);
		theta3 = fabs(theta3);
		theta4 = (-loadx+loady+loadz)/(mag*1.732);
		theta4 = fabs(theta4);
		lambda1 = (loadx+loady)/(mag*1.414);
		lambda1 = fabs(lambda1);
		lambda2 = (loadx+loadz)/(mag*1.414);
		lambda2 = fabs(lambda2);
		lambda3 = (loadx-loady)/(mag*1.414);
		lambda3 = fabs(lambda3);
		lambda4 = (loadx-loadz)/(mag*1.414);
		lambda4 = fabs(lambda4);
		lambda5 = (loady+loadz)/(mag*1.414);
		lambda5 = fabs(lambda5);
		lambda6 = (loady-loadz)/(mag*1.414);
		lambda6 = fabs(lambda6);
		schmid1 = theta1*lambda6;
		schmid2 = theta1*lambda4;
		schmid3 = theta1*lambda3;
		schmid4 = theta2*lambda3;
		schmid5 = theta2*lambda2;
		schmid6 = theta2*lambda5;
		schmid7 = theta3*lambda1;
		schmid8 = theta3*lambda5;
		schmid9 = theta3*lambda4;
		schmid10 = theta4*lambda1;
		schmid11 = theta4*lambda2;
		schmid12 = theta4*lambda6;
		schmid = schmid1;
		if(schmid2 > schmid) schmid = schmid2;
		if(schmid3 > schmid) schmid = schmid3;
		if(schmid4 > schmid) schmid = schmid4;
		if(schmid5 > schmid) schmid = schmid5;
		if(schmid6 > schmid) schmid = schmid6;
		if(schmid7 > schmid) schmid = schmid7;
		if(schmid8 > schmid) schmid = schmid8;
		if(schmid9 > schmid) schmid = schmid9;
		if(schmid10 > schmid) schmid = schmid10;
		if(schmid11 > schmid) schmid = schmid11;
		if(schmid12 > schmid) schmid = schmid12;
		m_Grains[i].schmidfactor = schmid;
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
  vector<int > neighdistfunc;
  neighborhood.resize((maxdiameter-mindiameter)+1);
  neighborhoodfit.resize((maxdiameter-mindiameter)+1);
  svbovera.resize((maxdiameter-mindiameter)+1);
  svcovera.resize((maxdiameter-mindiameter)+1);
  svcoverb.resize((maxdiameter-mindiameter)+1);
  svschmid.resize((maxdiameter-mindiameter)+1);
  svomega3.resize((maxdiameter-mindiameter)+1);
  for (int temp = 0; temp < ((maxdiameter-mindiameter)+1); temp++)
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
    int onedge = m_Grains[i].surfacegrain;
    if (onedge == 0)
    {
      actualgrains++;
      int vol = m_Grains[i].numvoxels;
      double voxvol = vol * resx * resy * resz;
      double logvol = log(voxvol);
      double diam = m_Grains[i].equivdiameter;
      double logdiam = log(diam);
      double I1 = m_Grains[i].radius1;
      double I2 = m_Grains[i].radius2;
      double I3 = m_Grains[i].radius3;
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
      double schmid = m_Grains[i].schmidfactor;
      double omega3 = m_Grains[i].omega3;
      neighdistfunc = m_Grains[i].neighbordistfunc;
      avgvol = avgvol + voxvol;
      avglnvol = avglnvol + logvol;
      avgbovera = avgbovera + bovera;
      avgcovera = avgcovera + covera;
      avgcoverb = avgcoverb + coverb;
      avgdiam = avgdiam + diam;
      avglogdiam = avglogdiam + logdiam;
      avgschmid = avgschmid + schmid;
      avgomega3 = avgomega3 + omega3;
      int diamint = int(diam)-mindiameter;
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
      int size = 0;
      size = neighdistfunc.size();
      neighborhood[diamint][0]++;
      for (int k = 0; k < size; k++)
      {
        int nnum = neighdistfunc[k];
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
  for (int temp3 = 0; temp3 < ((maxdiameter-mindiameter)+1); temp3++)
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
      neighborhoodfit[temp3][3] = log((neighborhood[temp3][5]-neighborhoodfit[temp3][2])/neighborhoodfit[temp3][1])/log(2.0);
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
    int onedge = m_Grains[j].surfacegrain;
    if (onedge == 0)
    {
      int vol = m_Grains[j].numvoxels;
      double voxvol = vol * resx * resy * resz;
      double logvol = log(voxvol);
      double rad_3 = 0.75 * (1 / m_pi) * voxvol;
      double diam = 2 * pow(rad_3, 0.333333333);
      double logdiam = log(diam);
      double I1 = m_Grains[j].radius1;
      double I2 = m_Grains[j].radius2;
      double I3 = m_Grains[j].radius3;
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
      double schmid = m_Grains[j].schmidfactor;
      double omega3 = m_Grains[j].omega3;
      neighdistfunc = m_Grains[j].neighbordistfunc;
      sdvol = sdvol + ((voxvol - avgvol) * (voxvol - avgvol));
      sdlnvol = sdlnvol + ((logvol - avglnvol) * (logvol - avglnvol));
      sdbovera = sdbovera + ((bovera - avgbovera) * (bovera - avgbovera));
      sdcovera = sdcovera + ((covera - avgcovera) * (covera - avgcovera));
      sdcoverb = sdcoverb + ((coverb - avgcoverb) * (coverb - avgcoverb));
      sddiam = sddiam + ((diam - avgdiam) * (diam - avgdiam));
      sdlogdiam = sdlogdiam + ((logdiam - avglogdiam) * (logdiam - avglogdiam));
      sdschmid = sdschmid + ((schmid - avgschmid) * (schmid - avgschmid));
      sdomega3 = sdomega3 + ((omega3 - avgomega3) * (omega3 - avgomega3));
      int diamint = int(diam)-mindiameter;
      svbovera[diamint][2] = svbovera[diamint][2] + ((bovera - svbovera[diamint][1]) * (bovera - svbovera[diamint][1]));
      svcovera[diamint][2] = svcovera[diamint][2] + ((covera - svcovera[diamint][1]) * (covera - svcovera[diamint][1]));
      svcoverb[diamint][2] = svcoverb[diamint][2] + ((coverb - svcoverb[diamint][1]) * (coverb - svcoverb[diamint][1]));
      svschmid[diamint][2] = svschmid[diamint][2] + ((schmid - svschmid[diamint][1]) * (schmid - svschmid[diamint][1]));
      svomega3[diamint][2] = svomega3[diamint][2] + ((omega3 - svomega3[diamint][1]) * (omega3 - svomega3[diamint][1]));
      int size = 0;
      size = neighdistfunc.size();
      for (int k = 0; k < size; k++)
      {
        int nnum = neighdistfunc[k];
        neighborhood[diamint][((2 * k) + 2)] = neighborhood[diamint][((2 * k) + 2)] + ((neighborhood[diamint][((2 * k) + 1)] - nnum)
            * (neighborhood[diamint][((2 * k) + 1)] - nnum));
      }
    }
  }
  for (int temp4 = 0; temp4 < ((maxdiameter-mindiameter)+1); temp4++)
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
  for (int temp5 = 0; temp5 < ((maxdiameter-mindiameter)+1); temp5++)
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

  retErr = h5io->writeVolumeStats(maxdiameter, mindiameter, 1.0, avglogdiam, sdlogdiam,
                                  svbovera, svcovera, svcoverb, neighborhoodfit, svomega3);

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
  vector<int> neighdistfunc;
  neighborhood.resize((maxdiameter-mindiameter)+1);
  neighborhoodfit.resize((maxdiameter-mindiameter)+1);
  svbovera.resize((maxdiameter-mindiameter)+1);
  svcovera.resize((maxdiameter-mindiameter)+1);
  svcoverb.resize((maxdiameter-mindiameter)+1);
  svschmid.resize((maxdiameter-mindiameter)+1);
  svomega3.resize((maxdiameter-mindiameter)+1);
  for(int temp = 0; temp < ((maxdiameter-mindiameter)+1); temp++)
  {
	neighborhood[temp].resize(7,0);
	neighborhoodfit[temp].resize(4,0);
    svbovera[temp].resize(5,0);
    svcovera[temp].resize(5,0);
    svcoverb[temp].resize(5,0);
    svschmid[temp].resize(5,0);
    svomega3[temp].resize(5,0);
  }
  for(int i = 1; i < numgrains; i++)
  {
    int onedge = m_Grains[i].surfacegrain;
    if(onedge == 0)
    {
      actualgrains++;
      int vol = m_Grains[i].numvoxels;
      double voxvol = vol*resx*resy;
      double logvol = log(voxvol);
	  double diam = m_Grains[i].equivdiameter;
      double logdiam = log(diam);
      double rad1 = m_Grains[i].radius1;
      double rad2 = m_Grains[i].radius2;
	  double bovera = rad2/rad1;
      double schmid = m_Grains[i].schmidfactor;
//	  double omega3 = m_Grains[i].omega3;
      neighdistfunc = m_Grains[i].neighbordistfunc;
      avgvol = avgvol+voxvol;
      avglnvol = avglnvol+logvol;
      avgbovera = avgbovera+bovera;
      avgdiam = avgdiam + diam;
      avglogdiam = avglogdiam+logdiam;
      avgschmid = avgschmid+schmid;
//      avgomega3 = avgomega3+omega3;
      int diamint = int(diam)-mindiameter;
      neighborhood[diamint][0]++;
      svbovera[diamint][0]++;
      svschmid[diamint][0]++;
      svomega3[diamint][0]++;
      svbovera[diamint][1] = svbovera[diamint][1] + bovera;
      svschmid[diamint][1] = svschmid[diamint][1] + schmid;
//      svomega3[diamint][1] = svomega3[diamint][1] + omega3;
      int size = 0;
      size = neighdistfunc.size();
	  neighborhood[diamint][0]++;
      for(int k=0;k<size;k++)
      {
        int nnum = neighdistfunc[k];
		neighborhood[diamint][((2*k)+1)] = neighborhood[diamint][((2*k)+1)] + nnum;
	  }
      if(voxvol > maxvol) maxvol = voxvol;
      if(bovera > maxbovera) maxbovera = bovera;
      if(diam > maxdiam) maxdiam = diam;
      if(logdiam > maxlogdiam) maxlogdiam = logdiam;
      if(schmid > maxschmid) maxschmid = schmid;
//      if(omega3 > maxomega3) maxomega3 = omega3;
    }
  }
  for(int temp3 = 0; temp3 < ((maxdiameter-mindiameter)+1); temp3++)
  {
    if(svbovera[temp3][0] != 0)
    {
      neighborhood[temp3][1] = neighborhood[temp3][1]/neighborhood[temp3][0];
      neighborhood[temp3][3] = neighborhood[temp3][3]/neighborhood[temp3][0];
      neighborhood[temp3][5] = neighborhood[temp3][5]/neighborhood[temp3][0];
      svbovera[temp3][1] = svbovera[temp3][1]/svbovera[temp3][0];
      svschmid[temp3][1] = svschmid[temp3][1]/svschmid[temp3][0];
//      svomega3[temp3][1] = svomega3[temp3][1]/svomega3[temp3][0];
      neighborhoodfit[temp3][0] = neighborhood[temp3][0];
      neighborhoodfit[temp3][1] = neighborhood[temp3][3] - neighborhood[temp3][1];
      neighborhoodfit[temp3][2] = neighborhood[temp3][1];
      neighborhoodfit[temp3][3] = log((neighborhood[temp3][5]-neighborhoodfit[temp3][2])/neighborhoodfit[temp3][1])/log(2.0);
    }
  }
  avgvol = avgvol/actualgrains;
  avglnvol = avglnvol/actualgrains;
  avgbovera = avgbovera/actualgrains;
  avgdiam = avgdiam/actualgrains;
  avglogdiam = avglogdiam/actualgrains;
  avgdiam2 = avgdiam2/neighcount;
  avgschmid = avgschmid/actualgrains;
//  avgomega3 = avgomega3/actualgrains;
  maxvol = maxvol/avgvol;
  double sdvol = 0;
  double sdlnvol = 0;
  double sdbovera = 0;
  double sddiam = 0;
  double sdlogdiam = 0;
  double sddiam2 = 0;
  double sdschmid = 0;
//  double sdomega3 = 0;
  for(int j = 1; j < numgrains; j++)
  {
    int onedge = m_Grains[j].surfacegrain;
    if(onedge == 0)
    {
      int vol = m_Grains[j].numvoxels;
      double voxvol = vol*resx*resy*resz;
      double logvol = log(voxvol);
	  double diam = m_Grains[j].equivdiameter;
      double logdiam = log(diam);
      double rad1 = m_Grains[j].radius1;
      double rad2 = m_Grains[j].radius2;
	  double bovera = rad2/rad1;
      double schmid = m_Grains[j].schmidfactor;
//	  double omega3 = m_Grains[j].omega3;
      neighdistfunc = m_Grains[j].neighbordistfunc;
      sdvol = sdvol + ((voxvol-avgvol)*(voxvol-avgvol));
      sdlnvol = sdlnvol + ((logvol-avglnvol)*(logvol-avglnvol));
      sdbovera = sdbovera + ((bovera-avgbovera)*(bovera-avgbovera));
      sddiam = sddiam + ((diam-avgdiam)*(diam-avgdiam));
      sdlogdiam = sdlogdiam + ((logdiam-avglogdiam)*(logdiam-avglogdiam));
      sdschmid = sdschmid + ((schmid-avgschmid)*(schmid-avgschmid));
//      sdomega3 = sdomega3 + ((omega3-avgomega3)*(omega3-avgomega3));
      int diamint = int(diam)-mindiameter;
      svbovera[diamint][2] = svbovera[diamint][2] + ((bovera-svbovera[diamint][1])*(bovera-svbovera[diamint][1]));
      svschmid[diamint][2] = svschmid[diamint][2] + ((schmid-svschmid[diamint][1])*(schmid-svschmid[diamint][1]));
//      svomega3[diamint][2] = svomega3[diamint][2] + ((omega3-svomega3[diamint][1])*(omega3-svomega3[diamint][1]));
      int size = 0;
      size = neighdistfunc.size();
      for(int k=0;k<size;k++)
      {
        int nnum = neighdistfunc[k];
		neighborhood[diamint][((2*k)+2)] = neighborhood[diamint][((2*k)+2)] + ((neighborhood[diamint][((2*k)+1)]-nnum)*(neighborhood[diamint][((2*k)+1)]-nnum));
	  }
    }
  }
  for(int temp4 = 0; temp4 < ((maxdiameter-mindiameter)+1); temp4++)
  {
    if(svbovera[temp4][0] != 0)
    {
      neighborhood[temp4][2] = neighborhood[temp4][2]/neighborhood[temp4][0];
      neighborhood[temp4][4] = neighborhood[temp4][4]/neighborhood[temp4][0];
      neighborhood[temp4][6] = neighborhood[temp4][6]/neighborhood[temp4][0];
      svbovera[temp4][2] = svbovera[temp4][2]/svbovera[temp4][0];
      svschmid[temp4][2] = svschmid[temp4][2]/svschmid[temp4][0];
//      svomega3[temp4][2] = svomega3[temp4][2]/svomega3[temp4][0];
      svbovera[temp4][3] = svbovera[temp4][1]*(((svbovera[temp4][1]*(1-svbovera[temp4][1]))/svbovera[temp4][2])-1);
      svbovera[temp4][4] = (1-svbovera[temp4][1])*(((svbovera[temp4][1]*(1-svbovera[temp4][1]))/svbovera[temp4][2])-1);
//      svomega3[temp4][3] = svomega3[temp4][1]*(((svomega3[temp4][1]*(1-svomega3[temp4][1]))/svomega3[temp4][2])-1);
//      svomega3[temp4][4] = (1-svomega3[temp4][1])*(((svomega3[temp4][1]*(1-svomega3[temp4][1]))/svomega3[temp4][2])-1);
      neighborhood[temp4][2] = pow(neighborhood[temp4][2],0.5);
      neighborhood[temp4][4] = pow(neighborhood[temp4][4],0.5);
      neighborhood[temp4][6] = pow(neighborhood[temp4][6],0.5);
      svbovera[temp4][2] = pow(svbovera[temp4][2],0.5);
      svschmid[temp4][2] = pow(svschmid[temp4][2],0.5);
//      svomega3[temp4][2] = pow(svomega3[temp4][2],0.5);
    }
  }
  sdvol = sdvol/actualgrains;
  sdlnvol = sdlnvol/actualgrains;
  sdbovera = sdbovera/actualgrains;
  sddiam = sddiam/actualgrains;
  sdlogdiam = sdlogdiam/actualgrains;
  sddiam2 = sddiam2/neighcount;
  sdschmid = sdschmid/actualgrains;
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
  sdvol = pow(sdvol,0.5);
  sdlnvol = pow(sdlnvol,0.5);
  sdbovera = pow(sdbovera,0.5);
  sddiam = pow(sddiam,0.5);
  sdlogdiam = pow(sdlogdiam,0.5);
  sddiam2 = pow(sddiam2,0.5);
  sdschmid = pow(sdschmid,0.5);
//  sdomega3 = pow(sdomega3,0.5);
  double svboveracr = 0;
  double svschmidcr = 0;
//  double svomega3cr = 0;
  for(int temp5 = 0; temp5 < ((maxdiameter-mindiameter)+1); temp5++)
  {
    svboveracr = svboveracr + (svbovera[temp5][0]*((svbovera[temp5][1]-avgbovera)*(svbovera[temp5][1]-avgbovera)));
    svschmidcr = svschmidcr + (svschmid[temp5][0]*((svschmid[temp5][1]-avgschmid)*(svschmid[temp5][1]-avgschmid)));
//    svomega3cr = svomega3cr + (svomega3[temp5][0]*((svomega3[temp5][1]-avgomega3)*(svomega3[temp5][1]-avgomega3)));
  }
  svboveracr = svboveracr/(actualgrains*boveravar);
  svschmidcr = svschmidcr/(actualgrains*schmidvar);
//  svomega3cr = svomega3cr/(actualgrains*omega3var);


  retErr = h5io->writeVolumeStats(maxdiameter, mindiameter, 1.0, avglogdiam, sdlogdiam,
                                  svbovera, svcovera, svcoverb, neighborhoodfit, svomega3);


  return retErr;
}




#define WRITE_VTK_GRAIN_HEADER()\
  fprintf(f, "# vtk DataFile Version 2.0\n");\
  fprintf(f, "data set from AIMReconstruction\n");\
  fprintf(f, "ASCII\n");\
  fprintf(f, "DATASET STRUCTURED_POINTS\n");\
  fprintf(f, "DIMENSIONS %d %d %d\n", xpoints, ypoints, zpoints);\
  fprintf(f, "ORIGIN 0.0 0.0 0.0\n");\
  fprintf(f, "SPACING %f %f %f\n", resx, resy, resz);\
  fprintf(f, "POINT_DATA %d\n\n", xpoints*ypoints*zpoints );\



#define WRITE_VTK_GRAIN_IDS()\
  fprintf(f, "SCALARS GrainID int  1\n");\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (size_t i = 0; i < total; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n"); }\
    fprintf(f, "%d ", voxels[i].grainname);\
  }\
  fprintf(f, "\n");\


#define WRITE_VTK_SCALARS_FROM_VOXEL(name, type, var)\
  fprintf(f, "SCALARS %s %s\n", #name, #type);\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (size_t i = 0; i < total; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, "%f ", voxels[i].var);\
  }\


#define WRITE_VTK_GRAIN_WITH_VOXEL_SCALAR_VALUE(name, var)\
int ReconstructionFunc::write##name##VizFile(const std::string &file)\
{\
  FILE* f = NULL;\
  f = fopen(file.c_str(), "w");\
  if (NULL == f) {return 1;}\
  WRITE_VTK_GRAIN_HEADER()\
  size_t total = xpoints*ypoints*zpoints;\
  WRITE_VTK_GRAIN_IDS()\
  WRITE_VTK_SCALARS_FROM_VOXEL(name, float, var)\
  fclose(f);\
  return 0;\
}


#define WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE(name, var)\
int ReconstructionFunc::write##name##VizFile(const std::string &file)\
{\
  FILE* f = NULL;\
  f = fopen(file.c_str(), "w");\
  if (NULL == f) {return 1;}\
  WRITE_VTK_GRAIN_HEADER()\
  size_t total = xpoints*ypoints*zpoints;\
  WRITE_VTK_GRAIN_IDS()\
  fprintf(f, "SCALARS SchmidFactor float\n");\
  fprintf(f, "LOOKUP_TABLE default\n");\
  for (size_t i = 0; i < total; i++) {\
    if(i%20 == 0 && i > 0) { fprintf(f, "\n");}\
    fprintf(f, "%f ", m_Grains[voxels[i].grainname].schmidfactor);\
  }\
  fclose(f);\
  return 0;\
}

WRITE_VTK_GRAIN_WITH_VOXEL_SCALAR_VALUE(Disorientation, kernelmisorientation);
WRITE_VTK_GRAIN_WITH_VOXEL_SCALAR_VALUE(ImageQuality, imagequality);
WRITE_VTK_GRAIN_WITH_GRAIN_SCALAR_VALUE(SchmidFactor, schmidfactor);

int ReconstructionFunc::writeVisualizationFile(const std::string &file)
{
  FILE* f = NULL;
  f = fopen(file.c_str(), "w");
  if (NULL == f) {return 1;}
  WRITE_VTK_GRAIN_HEADER()
  size_t total = xpoints*ypoints*zpoints;
  WRITE_VTK_GRAIN_IDS()

  WRITE_VTK_SCALARS_FROM_VOXEL(SurfaceVoxel, float, nearestneighbordistance)

  if (mergetwinsoption == 1)
  {
    fprintf(f, "SCALARS WasTwin int 1");
    fprintf(f, "LOOKUP_TABLE default\n");
    size_t grainname = 0;
    for (size_t i = 0; i < total; i++) {
      if(i%20 == 0 && i > 0) { fprintf(f, "\n");}
      grainname = voxels[i].grainname;
      fprintf(f, "%d ", m_Grains[grainname].gottwinmerged);
    }
  }

  fclose(f);
  return 0;
}

int ReconstructionFunc::writeIPFVizFile(const std::string &file)
{
  FILE* f = NULL;
  f = fopen(file.c_str(), "w");
  if (NULL == f) {return 1;}
  WRITE_VTK_GRAIN_HEADER()
  size_t total = xpoints*ypoints*zpoints;
  WRITE_VTK_GRAIN_IDS()

  fprintf(f, "COLOR_SCALARS colors 3\n");
  double red,green,blue;
  double q1[4];
  unsigned char rgb[3] = {0, 0, 0};
  double RefDirection[3] = {0.0, 0.0, 1.0};
  for (size_t i = 0; i < total; i++)
  {
    if(crystruct == AIM::Reconstruction::Cubic)
    {
      OIMColoring::GenerateIPFColor(voxels[i].euler1, voxels[i].euler2, voxels[i].euler3, RefDirection[0], RefDirection[1], RefDirection[2], rgb);
      red = static_cast<double>(double(rgb[0])/255.0);
      green = static_cast<double>(double(rgb[1])/255.0);
      blue = static_cast<double>(double(rgb[2])/255.0);
    }
    if(crystruct == AIM::Reconstruction::Hexagonal)
    {
      q1[0]=voxels[i].quat[1];
      q1[1]=voxels[i].quat[2];
      q1[2]=voxels[i].quat[3];
      q1[3]=voxels[i].quat[4];
      OIMColoring::CalculateHexIPFColor(q1, rgb);
      red = static_cast<double>(double(rgb[0])/255.0);
      green = static_cast<double>(double(rgb[1])/255.0);
      blue = static_cast<double>(double(rgb[2])/255.0);
    }
    fprintf(f, "%f %f %f\n",red, green, blue);
  }

  fclose(f);
  return 0;
}
int  ReconstructionFunc::writeDownSampledVizFile(const std::string &file )
{
  FILE* f = NULL;
  f = fopen(file.c_str(), "w");
  if (NULL == f) {return 1;}
  int counter = 0;
  double x, y, z;
  double dsresx, dsresy, dsresz;
  int col, row, plane;
  int index;
  int dsxpoints = int(sizex / (resx * downsamplefactor));
  int dsypoints = int(sizey / (resy * downsamplefactor));
  int dszpoints = int(sizez / (resz * downsamplefactor));
  dsresx = resx * downsamplefactor;
  dsresy = resy * downsamplefactor;
  dsresz = resz * downsamplefactor;
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "Down Sampled from AIMReconstruction\n");
  fprintf(f, "ASCII\n");
  fprintf(f, "DATASET STRUCTURED_POINTS\n");
  fprintf(f, "DIMENSIONS %d %d %d\n", dsxpoints, dsypoints, dszpoints);
  fprintf(f, "ORIGIN 0.0 0.0 0.0\n");
  fprintf(f, "SPACING %f %f %f\n", dsresx, dsresy, dsresz);
  fprintf(f, "POINT_DATA %d\n\n", dsxpoints*dsypoints*dszpoints );
  fprintf(f, "SCALARS GrainID int  1\n");
  fprintf(f, "LOOKUP_TABLE default\n");
  for (int i = 0; i < dszpoints; i++)
  {
    for (int j = 0; j < dsypoints; j++)
    {
      for (int k = 0; k < dsxpoints; k++)
      {
        x = (k * dsresx) + (dsresx / 2.0);
        y = (j * dsresy) + (dsresy / 2.0);
        z = (i * dsresz) + (dsresz / 2.0);
        col = int(x / resx);
        row = int(y / resy);
        plane = int(z / resz);
        index = (plane * xpoints * ypoints) + (row * xpoints) + col;
        if(counter%20 == 0 && counter > 0) { fprintf(f, "\n"); }
        fprintf(f,"%d ", voxels[index].grainname);
        counter++;
      }
    }
  }

  fclose(f);
  return 0;
}

void ReconstructionFunc::write_graindata(const std::string &graindataFile)
{
  vector<int>* nlist;
  ofstream outFile;
  outFile.open(graindataFile.c_str());
  outFile << numgrains << endl;
  outFile << "Grain ID	Euler1	Euler2	Euler3	Equiv. Diameter	Grain Avg. Disorientation	Surface Grain	No. Neighbors" << endl;
  for(int i = 1; i < numgrains; i++)
  {
	double diameter = m_Grains[i].equivdiameter;
	int onsurface = m_Grains[i].surfacegrain;
	double avgmiso = m_Grains[i].averagemisorientation;
	nlist = m_Grains[i].neighborlist;
	int nucleus = m_Grains[i].nucleus;
	outFile << i << "	" << voxels[nucleus].euler1 <<  "	" << voxels[nucleus].euler2 <<  "	" << voxels[nucleus].euler3 << "	" << diameter << "	" << avgmiso << "	" << onsurface << "	" << nlist->size() << "	";
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
  for (int i = 1; i < numgrains; i++)
  {
    ss.str("");
    ss << outputdir << MXADir::Separator << "Grain_" << std::setw(5) << i << ".vtk";

    outFile.open(ss.str().c_str(), std::ios::trunc);
    outFile << "# vtk DataFile Version 2.0" << endl;
    outFile << "data set from FFT2dx_GB" << endl;
    outFile << "ASCII" << endl;
    outFile << "DATASET UNSTRUCTURED_GRID" << endl;
    if (NULL == m_Grains[i].voxellist)
    {
      m_Grains[i].voxellist = new std::vector<int>;
    }
    vlist = m_Grains[i].voxellist;
    pcount = 0;
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
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
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
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
          outFile << (col * resx) << "	" << (row * resy) << "	" << (plane * resz) << endl;
        }
      }
    }
    outFile << endl;
    outFile << "CELLS " << vlist->size() << " " << vlist->size() * 9 << endl;
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      ocol = vid % xpoints;
      orow = (vid / xpoints) % ypoints;
      oplane = vid / (xpoints * ypoints);
      outFile << "8	";
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
        outFile << plist[pid] << "	";
      }
      outFile << endl;
    }
    outFile << endl;
    outFile << "CELL_TYPES " << vlist->size() << endl;
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
    {
      outFile << "11" << endl;
    }
    outFile << endl;
    outFile << "CELL_DATA " << vlist->size() << endl;
    outFile << "SCALARS GrainID int" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].grainname << endl;
    }
    outFile << endl;
    outFile << "SCALARS KernelAvgDisorientation float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].kernelmisorientation << endl;
    }
    outFile << endl;
    outFile << "SCALARS GrainAvgDisorientation float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].misorientation << endl;
    }
    outFile << endl;
    outFile << "SCALARS ImageQuality float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].imagequality << endl;
    }
    outFile << endl;
    outFile << "SCALARS SchmidFactor float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
    {
      outFile << m_Grains[i].schmidfactor << endl;
    }
    outFile.close();
    plist.clear();
    plist.resize(((xpoints + 1) * (ypoints + 1) * (zpoints + 1)), 0);
  }
}

double ReconstructionFunc::gamma(double x)
{
    int i,k,m;
    double ga,gr,r,z;


    static double g[] = {
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
        0.14e-14};

    if (x > 171.0) return 1e308;    // This value is an overflow flag.
    if (x == (int)x) {
        if (x > 0.0) {
            ga = 1.0;               // use factorial
            for (i=2;i<x;i++) {
               ga *= i;
            }
         }
         else
            ga = 1e308;
     }
     else {
        if (fabs(x) > 1.0) {
            z = fabs(x);
            m = (int)z;
            r = 1.0;
            for (k=1;k<=m;k++) {
                r *= (z-k);
            }
            z -= m;
        }
        else
            z = x;
        gr = g[24];
        for (k=23;k>=0;k--) {
            gr = gr*z+g[k];
        }
        ga = 1.0/(gr*z);
        if (fabs(x) > 1.0) {
            ga *= r;
            if (x < 0.0) {
                ga = -1 * m_pi/(x*ga*sin(m_pi*x));
            }
        }
    }
    return ga;
}


double ReconstructionFunc::find_xcoord(long index)
{
	double x = resx*double(index%xpoints);
	return x;
}
double ReconstructionFunc::find_ycoord(long index)
{
	double y = resy*double((index/xpoints)%ypoints);
	return y;
}
double ReconstructionFunc::find_zcoord(long index)
{
	double z = resz*double(index/(xpoints*ypoints));
	return z;
}


int ReconstructionFunc::writeHDF5GrainsFile(const std::string &hdfFile )
{
  int err = -1;
  AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();
  h5writer->setFileName(hdfFile);
  err = h5writer->openFile(false); // Open a new file over writing any other file

  std::stringstream ss;
  std::string hdfPath;
  std::vector<std::string > hdfPaths;
  // std::cout << "Writing out " << numgrains << " to an HDF5 Grain File..." << std::endl;
  for (int i = 1; i < numgrains; i++)
  {
    //   std::cout << " Grain: " << i << " Gathering Data" << std::endl;
    vector<int >* vlist = m_Grains[i].voxellist;
    int vid = vlist->at(0);
    ss.str("");
    ss << "/" << i;
    hdfPath = ss.str();
    hdfPaths.push_back(hdfPath);

    vector<int > plist(((xpoints + 1) * (ypoints + 1) * (zpoints + 1)), 0);
    int pcount = 0;
    double q1[5];
    unsigned char rgb[3] =
    { 0, 0, 0 };
    double RefDirection[3] =
    { 0.0, 0.0, 1.0 };
    int ocol, orow, oplane;
    int col, row, plane;
    int pid;
    int err = 0;
    // outFile << "POINTS " << pcount << " float" << endl;
    std::vector<float > points;
    std::vector<int32_t > cells;
    std::vector<int32_t > cell_types(vlist->size(), VTK_CELLTYPE_VOXEL);

    std::vector<float > kernelAvgDisorientation(vlist->size());
    std::vector<float > grainAvgDisorientation(vlist->size());
    std::vector<float > imageQuality(vlist->size());
    std::vector<unsigned char > ipfColor(vlist->size() * 3);
    std::vector<float > schmidFactor(1);

    std::vector<int32_t > grainName(1);

    pcount = 0;
    plist.clear();
    plist.resize(((xpoints + 1) * (ypoints + 1) * (zpoints + 1)), 0);
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      ocol = vid % xpoints;
      orow = (vid / xpoints) % ypoints;
      oplane = vid / (xpoints * ypoints);
      cells.push_back(8);
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
          //     outFile << (col * resx) << "  " << (row * resy) << "  " << (plane * resz) << endl;
          points.push_back((col * resx));
          points.push_back((row * resy));
          points.push_back((plane * resz));
        }
        // Add onto our cells vector
        cells.push_back(plist[pid]);
      }
      // Append a grainId to the grainIds vector
      kernelAvgDisorientation[j] = voxels[vid].kernelmisorientation;
      grainAvgDisorientation[j] = voxels[vid].misorientation;
      imageQuality[j] = voxels[vid].imagequality;
      if (crystruct == AIM::Reconstruction::Cubic)
      {
        OIMColoring::GenerateIPFColor(voxels[vid].euler1, voxels[vid].euler2, voxels[vid].euler3, RefDirection[0], RefDirection[1], RefDirection[2], rgb);
      }
      if (crystruct == AIM::Reconstruction::Hexagonal)
      {
        q1[0] = voxels[i].quat[1];
        q1[1] = voxels[i].quat[2];
        q1[2] = voxels[i].quat[3];
        q1[3] = voxels[i].quat[4];
        OIMColoring::CalculateHexIPFColor(q1, rgb);
      }
      ipfColor[j * 3] = rgb[0];
      ipfColor[j * 3 + 1] = rgb[1];
      ipfColor[j * 3 + 2] = rgb[2];
      grainName[0] = voxels[vid].grainname;
    }
    //   std::cout << " Grain: " << i << " Writing HDF5 File" << std::endl;
    err = h5writer->writeUnstructuredGrid(hdfPath, points, cells, cell_types);
    points.resize(0);
    cells.resize(0);
    cell_types.resize(0);

    //Write the Field Data
    err = h5writer->writeFieldData<int > (hdfPath, grainName, AIM::Representation::Grain_ID.c_str(), 1);

    schmidFactor[0] = m_Grains[i].schmidfactor;
    err = h5writer->writeFieldData<float > (hdfPath, schmidFactor, AIM::Representation::SchmidFactor.c_str(), 1);

    // Write the Neighbor list
    err = h5writer->writeFieldData<int > (hdfPath, *(m_Grains[i].neighborlist), AIM::Representation::Neighbor_Grain_ID_List.c_str(), 1);

    // Write CELL_DATA
    err = h5writer->writeCellData<float > (hdfPath, kernelAvgDisorientation, AIM::Representation::KernelAvgDisorientation.c_str(), 1);
    err = h5writer->writeCellData<float > (hdfPath, grainAvgDisorientation, AIM::Representation::GrainAvgDisorientation.c_str(), 1);
    err = h5writer->writeCellData<float > (hdfPath, imageQuality, AIM::Representation::ImageQuality.c_str(), 1);
    err = h5writer->writeCellData<unsigned char > (hdfPath, ipfColor, AIM::Representation::IPFColor.c_str(), 3);

  }

  err = h5writer->writeObjectIndex(hdfPaths);
  err = h5writer->closeFile();
  return err;
}

