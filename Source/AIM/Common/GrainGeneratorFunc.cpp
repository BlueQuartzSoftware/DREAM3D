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

#include "GrainGeneratorFunc.h"

#if 0
// -i C:\Users\GroebeMA\Desktop\NewFolder --outputDir C:\Users\GroebeMA\Desktop\NewFolder -f Slice_ --angMaxSlice 400 -s 1 -e 30 -z 0.25 -t -g 10 -c 0.1 -o 5.0 -x 2
#endif

#ifndef M_PI
#define M_PI 3.1415926535897;
#endif

const static double m_pi = 3.1415926535897;
const static double m_OnePointThree = 1.33333333333;

#include <sstream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class BoundaryCenter {
public:
  BoundaryCenter() :
      grain1(0),
      grain2(0),
      x(0.0),
      y(0.0),
      z(0.0)
  {}


  virtual ~BoundaryCenter() {}
  int grain1;
  int grain2;
  double x;
  double y;
  double z;


};

using namespace std;

GrainGeneratorFunc::GrainGeneratorFunc() :
  voxels(NULL),
  tempgrain(NULL),
  grains(NULL),
  goodgrain(NULL),
  packedgrain(NULL),
  gsizes(NULL),
  gremovals(NULL),
  seNbin(NULL),
  eulerbin(NULL),
  orient(NULL),
  gridfine(NULL),
  gridcourse(NULL),
  actualmisobin(NULL),
  simmisobin(NULL),
  actualmicrobin(NULL),
  simmicrobin(NULL)
{

}

GrainGeneratorFunc::~GrainGeneratorFunc()
{

  delete [] voxels;
  delete [] tempgrain;
  delete [] grains;
  delete [] goodgrain;
  delete [] packedgrain;
  delete [] gsizes;
  delete [] gremovals;
  delete [] seNbin;
  delete [] eulerbin;
  delete [] orient;
  delete [] gridfine;
  delete [] gridcourse;
  delete [] actualmisobin;
  delete [] simmisobin;
  delete [] actualmicrobin;
  delete [] simmicrobin;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorFunc::initialize(double stepX, double stepY, double stepZ,
                  int32 angNumCols, int32 angNumRows, int32 angSlices,
                  bool v_mergetwinsoption,
                  int32 v_minallowedgrainsize,
                  double v_minseedconfidence,
                  double v_misorientationtolerance,
                  int32 v_crystruct,
                  bool v_alreadyformed)
{

  resx = stepX;
  resy = stepY;
  resz = stepZ;

  mergetwinsoption = (v_mergetwinsoption == true) ? 1 : 0;
  minallowedgrainsize = v_minallowedgrainsize;
  minseedconfidence = v_minseedconfidence;
  misorientationtolerance = v_misorientationtolerance;
  crystruct = v_crystruct;
  alreadyformed = (v_alreadyformed == true) ? 1 : 0;


  xpoints = angNumCols;
  ypoints = angNumRows;
  zpoints = angSlices;

  sizex = (xpoints - 1 ) * resx;
  sizey = (ypoints - 1 ) * resy;
  sizez = (zpoints - 1 ) * resz;

  totalpoints = xpoints * ypoints * zpoints;


#if 0
  std::cout << "X Size: " << sizex << std::endl;
  std::cout << "Y Size: " << sizey << std::endl;
  std::cout << "Z Size: " << sizez << std::endl;
  std::cout << "X Res: " << resx << std::endl;
  std::cout << "Y Res: " << resy << std::endl;
  std::cout << "Z Res: " << resz << std::endl;
  std::cout << "X Points: " << xpoints << std::endl;
  std::cout << "Y Points: " << ypoints << std::endl;
  std::cout << "Z Points: " << zpoints << std::endl;
  std::cout << "Total Points: " << totalpoints << std::endl;
#endif



  voxels = new Voxel[totalpoints];
  tempgrain = new Grain[1];
  grains = new Grain[totalpoints/20];
  goodgrain = new Grain[totalpoints/20];
  packedgrain = new Grain[1];
  gsizes = new int[totalpoints/20];
  gremovals = new int[totalpoints/20];
  seNbin = new Bin[1000];
  eulerbin = new Bin[1000];
  orient = new Orient[5000];
#if 0
  gridfine = new Voxel[totalpoints];
  gridcourse = new Voxel[totalpoints];
  actualmisobin = new Bin[totalpoints];
  simmisobin = new Bin[totalpoints];
  actualmicrobin = new Bin[totalpoints];
  simmicrobin = new Bin[totalpoints];
#endif
  bcentSize = 0;
}

void GrainGeneratorFunc::write_volume(string writename10)
{
  //std::cout << "GrainGeneratorFunc::write_volume1: '" << writename10 << "'" << std::endl;
  ofstream outFile;
  outFile.open(writename10.c_str());
  for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    double ea1 = voxels[i].euler1;
    double ea2 = voxels[i].euler2;
    double ea3 = voxels[i].euler3;
    double ci = voxels[i].confidence;
    outFile << grainname << " " << ea1 << " " << ea2 << " " << ea3 << " " << ci << endl;
  }
  outFile.close();
}

void  GrainGeneratorFunc::loadStatsData(string inname1)
{
    ifstream inputFile;
    inputFile.open(inname1.c_str());
	int numdiameters=0;
	double avgdiam=0;
	double sddiam=0;
	inputFile >> numdiameters;
	inputFile >> avgdiam >> sddiam;
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> temp7 >> svbovera[temp7][3] >> svbovera[temp7][4];
	}	
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> temp7 >> svcovera[temp7][3] >> svcovera[temp7][4];
	}
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> temp7 >> svcoverb[temp7][1] >> svcoverb[temp7][2];
	}
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> temp7 >> svn[temp7][1] >> svn[temp7][2];
	}
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> temp7 >> svs[temp7][1] >> svs[temp7][2];
	}
    inputFile.close();
}

void  GrainGeneratorFunc::loadNData(string inname5, int numshapebins)
{
    ifstream inputFile;
    inputFile.open(inname5.c_str());
  int count = 0;
  double Nprob;
  double N;
  double junk;
  int bnum=0;
  for (long j = 0; j < numsizebins; j++)
  {
    for (long k = 0; k < numseNbins; k++)
    {
      inputFile >> junk >> Nprob >> N;
      bnum = (j*numseNbins)+k;
      seNbin[bnum].Nprobability = Nprob;
      seNbin[bnum].Nvalue = N;
          count++;
    }
  }
   inputFile.close();
}

void  GrainGeneratorFunc::loadorientData(string inname6, int numorients)
{
    ifstream inputFile;
    inputFile.open(inname6.c_str());
  int count = 0;
  double r1x;
  double r1y;
  double r1z;
  double r2x;
  double r2y;
  double r2z;
  double r3x;
  double r3y;
  double r3z;
  inputFile >> numorients;
  for (long k = 0; k < numorients; k++)
    {
    inputFile >> r1x >> r1y >> r1z >> r2x >> r2y >> r2z >> r3x >> r3y >> r3z;
    double denom1 = pow((r1x*r1x+r1y*r1y+r1z*r1z),0.5);
    double denom2 = pow((r2x*r2x+r2y*r2y+r2z*r2z),0.5);
    double denom3 = pow((r3x*r3x+r3y*r3y+r3z*r3z),0.5);
    r1x = r1x/denom1;
    r1y = r1y/denom1;
    r1z = r1z/denom1;
    r2x = r2x/denom2;
    r2y = r2y/denom2;
    r2z = r2z/denom2;
    r3x = r3x/denom3;
    r3y = r3y/denom3;
    r3z = r3z/denom3;
    orient[k].rad1x = r1x;
    orient[k].rad1y = r1y;
    orient[k].rad1z = r1z;
    orient[k].rad2x = r2x;
    orient[k].rad2y = r2y;
    orient[k].rad2z = r2z;
    orient[k].rad3x = r3x;
    orient[k].rad3y = r3y;
    orient[k].rad3z = r3z;
        count++;
    }
    inputFile.close();
}

void  GrainGeneratorFunc::loadeulerData(string inname7, int numeulers)
{
    ifstream inputFile;
    inputFile.open(inname7.c_str());
//  int count = 0;
  double g1ea1;
  double g1ea2;
  double g1ea3;
  double ea1;
  double ea2;
  double ea3;
  int totalcount = 0;
//  int bnum=0;
  inputFile >> numeulers;
  for (long iter = 0; iter < numeulers; iter++)
    {
    inputFile >> g1ea1 >> g1ea2 >> g1ea3;
    for(int k = 0; k < 24; k++)
    {
      double o[3][3];
      double ga[3][3];
      double m1[3][3];
      double mo[3][3];
      if (k == 0)
      {
        o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
        o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
        o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
      }
      else if (k == 1)
      {
        o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] =  0.0;
        o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = -1.0;
        o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] =  0.0;
      }
      else if (k == 2)
      {
        o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] =  0.0;
        o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] =  0.0;
        o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = -1.0;
      }
      else if (k == 3)
      {
        o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] = 0.0;
        o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = 1.0;
        o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] = 0.0;
      }
      else if (k == 4)
      {
        o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = -1.0;
        o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] =  0.0;
        o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] =  0.0;
      }
      else if (k == 5)
      {
        o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = 1.0;
        o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] = 0.0;
        o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] = 0.0;
      }
      else if (k == 6)
      {
        o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] =  0.0;
        o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] =  0.0;
        o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = -1.0;
      }
      else if (k == 7)
      {
        o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] = 0.0;
        o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] = 0.0;
        o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = 1.0;
      }
      else if (k == 8)
      {
        o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] = 0.0;
        o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] = 0.0;
        o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = 1.0;
      }
      else if (k == 9)
      {
        o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] = 0.0;
        o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] = 0.0;
        o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = 1.0;
      }
      else if (k == 10)
      {
        o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] = 0.0;
        o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = 1.0;
        o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] = 0.0;
      }
      else if (k == 11)
      {
        o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = 1.0;
        o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] = 0.0;
        o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] = 0.0;
      }
      else if (k == 12)
      {
        o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] =  0.0;
        o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = -1.0;
        o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] =  0.0;
      }
      else if (k == 13)
      {
        o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = -1.0;
        o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] =  0.0;
        o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] =  0.0;
      }
      else if (k == 14)
      {
        o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] =  0.0;
        o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = -1.0;
        o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] =  0.0;
      }
      else if (k == 15)
      {
        o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = -1.0;
        o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] =  0.0;
        o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] =  0.0;
      }
      else if (k == 16)
      {
        o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] = 0.0;
        o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = 1.0;
        o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] = 0.0;
      }
      else if (k == 17)
      {
        o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = 1.0;
        o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] = 0.0;
        o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] = 0.0;
      }
      else if (k == 18)
      {
        o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] =  0.0;
        o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] =  0.0;
        o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
      }
      else if (k == 19)
      {
        o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
        o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = 1.0;
        o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] = 0.0;
      }
      else if (k == 20)
      {
        o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = 1.0;
        o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
        o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] = 0.0;
      }
      else if (k == 21)
      {
        o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] =  0.0;
        o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = -1.0;
        o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] =  0.0;
      }
      else if (k == 22)
      {
        o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = -1.0;
        o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] =  0.0;
        o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] =  0.0;
      }
      else if (k == 23)
      {
        o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] =  0.0;
        o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] =  0.0;
        o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = -1.0;
      }
      for(int p = 0; p < 4; p++)
      {
        double o2[3][3];
        if (p == 0)
        {
          o2[0][0] = 1.0; o2[0][1] = 0.0; o2[0][2] = 0.0;
          o2[1][0] = 0.0; o2[1][1] = 1.0; o2[1][2] = 0.0;
          o2[2][0] = 0.0; o2[2][1] = 0.0; o2[2][2] = 1.0;
        }
        else if (p == 1)
        {
          o2[0][0] = 1.0; o2[0][1] = 0.0; o2[0][2] =  0.0;
          o2[1][0] = 0.0; o2[1][1] = -1.0; o2[1][2] = 0.0;
          o2[2][0] = 0.0; o2[2][1] = 0.0; o2[2][2] =  -1.0;
        }
        else if (p == 2)
        {
          o2[0][0] = -1.0; o2[0][1] =  0.0; o2[0][2] =  0.0;
          o2[1][0] = 0.0; o2[1][1] = 1.0; o2[1][2] =  0.0;
          o2[2][0] = 0.0; o2[2][1] =  0.0; o2[2][2] = -1.0;
        }
        else if (p == 3)
        {
          o2[0][0] = -1.0; o2[0][1] =  0.0; o2[0][2] = 0.0;
          o2[1][0] = 0.0; o2[1][1] = -1.0; o2[1][2] = 0.0;
          o2[2][0] = 0.0; o2[2][1] = 0.0; o2[2][2] = 1.0;
        }
        ga[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
        ga[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
        ga[0][2] = sin(g1ea3)*sin(g1ea2);
        ga[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
        ga[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
        ga[1][2] =  cos(g1ea3)*sin(g1ea2);
        ga[2][0] =  sin(g1ea1)*sin(g1ea2);
        ga[2][1] = -cos(g1ea1)*sin(g1ea2);
        ga[2][2] =  cos(g1ea2);

        mo[0][0] = o[0][0]*ga[0][0] + o[0][1]*ga[1][0] + o[0][2]*ga[2][0];
        mo[0][1] = o[0][0]*ga[0][1] + o[0][1]*ga[1][1] + o[0][2]*ga[2][1];
        mo[0][2] = o[0][0]*ga[0][2] + o[0][1]*ga[1][2] + o[0][2]*ga[2][2];
        mo[1][0] = o[1][0]*ga[0][0] + o[1][1]*ga[1][0] + o[1][2]*ga[2][0];
        mo[1][1] = o[1][0]*ga[0][1] + o[1][1]*ga[1][1] + o[1][2]*ga[2][1];
        mo[1][2] = o[1][0]*ga[0][2] + o[1][1]*ga[1][2] + o[1][2]*ga[2][2];
        mo[2][0] = o[2][0]*ga[0][0] + o[2][1]*ga[1][0] + o[2][2]*ga[2][0];
        mo[2][1] = o[2][0]*ga[0][1] + o[2][1]*ga[1][1] + o[2][2]*ga[2][1];
        mo[2][2] = o[2][0]*ga[0][2] + o[2][1]*ga[1][2] + o[2][2]*ga[2][2];

        m1[0][0] = mo[0][0]*o2[0][0] + mo[0][1]*o2[1][0] + mo[0][2]*o2[2][0];
        m1[0][1] = mo[0][0]*o2[0][1] + mo[0][1]*o2[1][1] + mo[0][2]*o2[2][1];
        m1[0][2] = mo[0][0]*o2[0][2] + mo[0][1]*o2[1][2] + mo[0][2]*o2[2][2];
        m1[1][0] = mo[1][0]*o2[0][0] + mo[1][1]*o2[1][0] + mo[1][2]*o2[2][0];
        m1[1][1] = mo[1][0]*o2[0][1] + mo[1][1]*o2[1][1] + mo[1][2]*o2[2][1];
        m1[1][2] = mo[1][0]*o2[0][2] + mo[1][1]*o2[1][2] + mo[1][2]*o2[2][2];
        m1[2][0] = mo[2][0]*o2[0][0] + mo[2][1]*o2[1][0] + mo[2][2]*o2[2][0];
        m1[2][1] = mo[2][0]*o2[0][1] + mo[2][1]*o2[1][1] + mo[2][2]*o2[2][1];
        m1[2][2] = mo[2][0]*o2[0][2] + mo[2][1]*o2[1][2] + mo[2][2]*o2[2][2];

        ea2 = acos(m1[2][2]);
        double cosine3 = (m1[1][2]/sin(ea2));
        double sine3 = (m1[0][2]/sin(ea2));
        double cosine1 = (-m1[2][1]/sin(ea2));
        double sine1 = (m1[2][0]/sin(ea2));
        ea3 = acos(cosine3);
        ea1 = acos(cosine1);
        if(sine3 < 0) ea3 = (2*3.1415926535897)-ea3;
        if(sine1 < 0) ea1 = (2*3.1415926535897)-ea1;
        int ea1bin = int(ea1/(3.1415926535897/36));
        int ea2bin = int(ea2/(3.1415926535897/36));
        int ea3bin = int(ea3/(3.1415926535897/36));
        if(ea1 >= 0 && ea2 >= 0 && ea3 >= 0 && ea1 <= (3.1415926535897/2) && ea2 <= (3.1415926535897/2) && ea3 <= (3.1415926535897/2))
        {
//          int curcount = eulercount[ea1bin][ea2bin][ea3bin];
          int bnum = (ea1bin*18*18)+(ea2bin*18)+ea3bin;
          eulerbin[bnum].euler1 = ea1;
          eulerbin[bnum].euler2 = ea2;
          eulerbin[bnum].euler3 = ea3;
          eulercount[ea1bin][ea2bin][ea3bin]++;
          totalcount++;
        }
      }
    }
    }
    inputFile.close();
}

void  GrainGeneratorFunc::generate_grains(int numgrains)
{
  int good = 1;
  double totvol = 0;
  int curbin1 = 0;
  int curbin2 = 0;
  int curbin3 = 0;
//  int curbin4 = 0;
  int curbin5 = 0;
  double r1 = 1;
  int bnum=0;
  int takencheck[1000];
  srand(static_cast<unsigned int>(time(NULL)));
  for(int l = 0; l < numgrains; l++)
  {
    curbin1 = 0;
    good = 1;
    double random = rg.Random();
    double random1 = random;
    for(int i = 0; i < numdiambins; i++)
    {
      double dprob = diambin[i].dprobability;
      if(dprob < random1) curbin1 = i;
    }
    double diam = diambin[curbin1].diameter;
    double vol = (4.0/3.0)*(3.1415926535897)*((diam/2.0)*(diam/2.0)*(diam/2.0));
    int diameter = int(diam);
    if(diameter >= numsizebins) diameter = numsizebins-1;
    int good = 0;
    while(good == 0)
    {
      random = rg.Random();
      double random2 = random;
      random = rg.Random();
      double random3 = random;
      for(int j = 0; j < numshapebins; j++)
      {
        bnum = (diameter*numshapebins)+j;
        double sprob1 = boverabin[bnum].sprobability;
        double sprob2 = coverabin[bnum].sprobability;
        if(sprob1 < random2) curbin2 = j;
        if(sprob2 < random3) curbin3 = j;
      }
      random = rg.Random();
      double random4 = 0.01*random;
      bnum = (diameter*numshapebins)+curbin2;
      double tempr2 = boverabin[bnum].rad;
      bnum = (diameter*numshapebins)+curbin3;
      double tempr3 = coverabin[bnum].rad;
      double coverb = tempr3/tempr2;
      double sprob3 = 0;
      for(int k = 0; k < numshapebins; k++)
      {
        bnum = (diameter*numshapebins)+k;
        double checkcoverb = coverbbin[bnum].rad;
        if(checkcoverb < coverb)
        {
          sprob3 = coverbbin[bnum].sprobability;
        }
      }
      if(sprob3 > random4) good = 1;
      if(coverb > 1) good = 0;
    }
    bnum = (diameter*numshapebins)+curbin2;
    double r2 = boverabin[bnum].rad;
    bnum = (diameter*numshapebins)+curbin3;
    double r3 = coverabin[bnum].rad;
    random = rg.Random();
    double random5 = random;
    int onum = int(random5*numorients);
    double r1x = orient[onum].rad1x;
    double r1y = orient[onum].rad1y;
    double r1z = orient[onum].rad1z;
    double r2x = orient[onum].rad2x;
    double r2y = orient[onum].rad2y;
    double r2z = orient[onum].rad2z;
    double r3x = orient[onum].rad3x;
    double r3y = orient[onum].rad3y;
    double r3z = orient[onum].rad3z;
    random = rg.Random();
    double random6 = random;
    for(int p = 0; p < numseNbins; p++)
    {
      bnum = (diameter*numseNbins)+p;
      double Nprob = seNbin[bnum].Nprobability;
      if(Nprob < random6) curbin5 = p;
    }
    bnum = (diameter*numshapebins)+curbin5;
    double N = seNbin[bnum].Nvalue;
    int gnum = l;
    tempgrain[l].grainname = gnum;
    tempgrain[l].volume = vol;
    tempgrain[l].axis1 = r1;
    tempgrain[l].axis2 = r2;
    tempgrain[l].axis3 = r3;
    tempgrain[l].axis1x = r1x;
    tempgrain[l].axis1y = r1y;
    tempgrain[l].axis1z = r1z;
    tempgrain[l].axis2x = r2x;
    tempgrain[l].axis2y = r2y;
    tempgrain[l].axis2z = r2z;
    tempgrain[l].axis3x = r3x;
    tempgrain[l].axis3y = r3y;
    tempgrain[l].axis3z = r3z;
    tempgrain[l].Nvalue = N;
    totvol = totvol + vol;
  }
  sizex = int(pow((totvol*0.9),0.33333));
  sizey = int(pow((totvol*0.9),0.33333));
  sizez = int(pow((totvol*0.9),0.33333));
  for(int i=0;i<numgrains;i++)
  {
    int maxgrain = 0;
    double maxvol = 0;
    for(int j=0;j<numgrains;j++)
    {
      double vol = tempgrain[j].volume;
      if(vol > maxvol && takencheck[j] != 1)
      {
        maxvol = vol;
        maxgrain = j;
      }
    }
    takencheck[maxgrain] = 1;
    double vol = tempgrain[maxgrain].volume;
    double r1 = tempgrain[maxgrain].axis1;
    double r2 = tempgrain[maxgrain].axis2;
    double r3 = tempgrain[maxgrain].axis3;
    double r1x = tempgrain[maxgrain].axis1x;
    double r1y = tempgrain[maxgrain].axis1y;
    double r1z = tempgrain[maxgrain].axis1z;
    double r2x = tempgrain[maxgrain].axis2x;
    double r2y = tempgrain[maxgrain].axis2y;
    double r2z = tempgrain[maxgrain].axis2z;
    double r3x = tempgrain[maxgrain].axis3x;
    double r3y = tempgrain[maxgrain].axis3y;
    double r3z = tempgrain[maxgrain].axis3z;
    double N = tempgrain[maxgrain].Nvalue;
    grains[i].grainname = i;
    grains[i].volume = vol;
    grains[i].axis1 = r1;
    grains[i].axis2 = r2;
    grains[i].axis3 = r3;
    grains[i].axis1x = r1x;
    grains[i].axis1y = r1y;
    grains[i].axis1z = r1z;
    grains[i].axis2x = r2x;
    grains[i].axis2y = r2y;
    grains[i].axis2z = r2z;
    grains[i].axis3x = r3x;
    grains[i].axis3y = r3y;
    grains[i].axis3z = r3z;
    grains[i].Nvalue = N;
  }
}

void  GrainGeneratorFunc::assign_eulers(int numgrains)
{
  int count = 0;
//  double grainssleft = numgrains-count;
  double density = 0;
  double synea1=0,synea2=0,synea3=0;
  for(int j = 0; j < 18; j++)
  {
    for(int k = 0; k < 18; k++)
    {
      for(int l = 0; l < 18; l++)
      {
        double num = (numgrains)*(double(eulercount[j][k][l]))/numeulers;
        density = density + (eulercount[j][k][l]*eulercount[j][k][l]);
//        double adjust = double(numgrains)/double(numeulers);
        num = num;
        int numint = int(num);
        for(int m = 0; m < numint; m++)
        {
          double random = rg.Random();
          double random1 = random;
          random = rg.Random();
          double random2 = random;
          random = rg.Random();
          double random3 = random;
          synea1 = ((3.1415926535897/36)*j)+((3.1415926535897/36)*random1);
          synea2 = ((3.1415926535897/36)*k)+((3.1415926535897/36)*random2);
          synea3 = ((3.1415926535897/36)*l)+((3.1415926535897/36)*random3);
          grains[count].euler1 = synea1;
          grains[count].euler2 = synea2;
          grains[count].euler3 = synea3;
          count++;
          if(count == numgrains) {break;}
        }
        if(count == numgrains) {break;}
      }
      if(count == numgrains) {break;}
    }
    if(count == numgrains) {break;}
  }
  double lastcur = 1;
  while(count < numgrains)
  {
    double cur = 0;
    for(int m = 0; m < 18; m++)
    {
      for(int n = 0; n < 18; n++)
      {
        for(int p = 0; p < 18; p++)
        {
          double num = eulercount[m][n][p]/numeulers;
          if(num > cur && num < lastcur)
          {
            cur = num;
            double random = rg.Random();
            double random4 = random;
            random = rg.Random();
            double random5 = random;
            random = rg.Random();
            double random6 = random;
            synea1 = ((3.1415926535897/36)*m)+((3.1415926535897/36)*random4);
            synea2 = ((3.1415926535897/36)*n)+((3.1415926535897/36)*random5);
            synea3 = ((3.1415926535897/36)*p)+((3.1415926535897/36)*random6);
          }
        }
      }
    }
    lastcur = cur;
    synea1 = rg.Random()*2*3.141592;
    synea2 = rg.Random()*1*3.141592;
    synea3 = rg.Random()*2*3.141592;
    grains[count].euler1 = synea1;
    grains[count].euler2 = synea2;
    grains[count].euler3 = synea3;
    count++;
  }
}

void  GrainGeneratorFunc::make_points(int numgrains)
{
  int ncount = 0;
  int count = 0;
  int good = 0;
  int column = 0;
  int row = 0;
  int plane = 0;
  int columncourse = 0;
  int rowcourse = 0;
  int planecourse = 0;
  int tempsurf = 0;
  int xmin = 0;
  int xmax = 0;
  int ymin = 0;
  int ymax = 0;
  int zmin = 0;
  int zmax = 0;
  long j = 0;
  double xc = 0;
  double yc = 0;
  double zc = 0;
  double x = 0;
  double y = 0;
  double z = 0;
  int init = 1;
  int in = 1;
  int out = 0;
  int taken90 = 0;
  long pointsleft = 0;
  double insidecount = 0;
  double badcount = 0;
  long triedcount = 0;
  int uniquecursize = 0;
  int totalcursize = 0;
  int uniquecurcoursesize = 0;
  vector<long> alreadytriedlist;
  srand(static_cast<unsigned int>(time(NULL)));
  int xpoints = int(sizex/resx);
  int ypoints = int(sizey/resy);
  int zpoints = int(sizez/resz);
  int xpointscourse = int(sizex/resx1);
  int ypointscourse = int(sizey/resy1);
  int zpointscourse = int(sizez/resz1);
  long nottakencount = (xpoints*ypoints*zpoints);
  long nottakencountcourse = (xpointscourse*ypointscourse*zpointscourse);
  for (long a = 0; a < nottakencountcourse; a++)
  {
    gridcourse[a].available = init;
    gridcourse[a].available90 = init;
  }
  for (long b = 0; b < nottakencount; b++)
  {
    gridfine[b].available = init;
    gridfine[b].available90 = init;
  }
  for (int i = 0; i < numgrains; i++)
  {
    good = 0;
    triedcount = 0;
    int size = int(alreadytriedlist.size());
    for(int k=0;k<size;k++)
    {
      int firstgridpoint = alreadytriedlist[k];
      gridcourse[firstgridpoint].available = init;
    }
    double volcur = grains[i].volume;
//    double aovera = grains[i].axis1;
    double bovera = grains[i].axis2;
    double covera = grains[i].axis3;
//    double coverb = covera/bovera;
    double Nvalue = grains[i].Nvalue;
    double beta1 = (gamma((1.0/Nvalue))*gamma((1/Nvalue)))/gamma((2/Nvalue));
    double beta2 = (gamma((2.0/Nvalue))*gamma((1/Nvalue)))/gamma((3/Nvalue));
    double radcur1cube = 1;
    if(shapeclass == 3)
    {
      if(Nvalue >= 0 && Nvalue <= 1)
      {
        radcur1cube = (volcur*6.0)/(6-(Nvalue*Nvalue*Nvalue));
      }
      if(Nvalue > 1 && Nvalue <= 2)
      {
        radcur1cube = (volcur*6.0)/(3+(9*Nvalue)-(9*Nvalue*Nvalue)+(2*Nvalue*Nvalue*Nvalue));
      }
    }
    if(shapeclass == 2)
    {
      radcur1cube = (volcur*(3.0/2.0)*(1.0/bovera)*(1.0/covera)*((Nvalue*Nvalue)/4.0)*(1.0/beta1)*(1.0/beta2));
    }
    if(shapeclass == 1)
    {
      radcur1cube = (volcur*(3.0/4.0)*(1.0/3.1415926535897)*(1.0/bovera)*(1.0/covera));
    }
    double rad = pow(radcur1cube,0.333333333333);
    if(shapeclass == 3) rad = rad/2.0;
    double radcur1 = rad;
    double radcur2 = (rad*bovera);
    double radcur3 = (rad*covera);
    double rad1x = grains[i].axis1x;
    double rad1y = grains[i].axis1y;
    double rad1z = grains[i].axis1z;
    double rad2x = grains[i].axis2x;
    double rad2y = grains[i].axis2y;
    double rad2z = grains[i].axis2z;
    double rad3x = grains[i].axis3x;
    double rad3y = grains[i].axis3y;
    double rad3z = grains[i].axis3z;
    while(good == 0)
    {
      double random = rg.Random();
      double random1 = random;
      pointsleft = nottakencountcourse-triedcount;
      int remainder = int(random1*pointsleft-1);
      int selector = -1;
      int tempcounter = 0;
      while(selector != remainder)
      {
        int picked = gridcourse[tempcounter].available;
        int noway2 = gridcourse[tempcounter].available90;
        if(picked == 1 && noway2 == 1)
        {
          selector++;
          j = tempcounter;
        }
        tempcounter++;
      }
      int alreadypicked = 0;
      int tried = gridcourse[j].available;
      gridcourse[j].available = alreadypicked;
      int noway = gridcourse[j].available90;
      if(noway == 1 && tried == 1)
      {
        for(int c=0;c<i;c++)
        {
          gremovals[c]=0;
        }
        alreadytriedlist.push_back(j);
        good = 1;
        columncourse = (j%xpointscourse);
        rowcourse = ((j/xpointscourse)%ypointscourse);
        planecourse = (j/(xpointscourse*ypointscourse));
        xc = (columncourse*resx1)+(resx1/2);
        yc = (rowcourse*resy1)+(resy1/2);
        zc = (planecourse*resz1)+(resz1/2);
        column = int((xc-(resx1/2))/resx);
        row = int((yc-(resy1/2))/resy);
        plane = int((zc-(resz1/2))/resz);
        xc = (column*resx)+(resx/2);
        yc = (row*resy)+(resy/2);
        zc = (plane*resz)+(resz/2);
        insidecount = 0;
        badcount = 0;
        xmin = 0;
        xmax = int((sizex/resx)-1);
        ymin = 0;
        ymax = int((sizey/resy)-1);
        zmin = 0;
        zmax = int((sizez/resz)-1);
        if(column-((radcur1/resx)+1) > 0)
        {
          xmin = int(column-((radcur1/resx)+1));
        }
        if(column+((radcur1/resx)+1) < (sizex/resx)-1)
        {
          xmax = int(column+((radcur1/resx)+1));
        }
        if(row-((radcur1/resy)+1) > 0)
        {
          ymin = int(row-((radcur1/resy)+1));
        }
        if(row+((radcur1/resy)+1) < (sizey/resy)-1)
        {
          ymax = int(row+((radcur1/resy)+1));
        }
        if(plane-((radcur1/resz)+1) > 0)
        {
          zmin = int(plane-((radcur1/resz)+1));
        }
        if(plane+((radcur1/resz)+1) < (sizez/resz)-1)
        {
          zmax = int(plane+((radcur1/resz)+1));
        }
        for(int iter1 = xmin; iter1 < xmax+1; iter1++)
        {
          for(int iter2 = ymin; iter2 < ymax+1; iter2++)
          {
            for(int iter3 = zmin; iter3 < zmax+1; iter3++)
            {
              double inside = -1;
              column = iter1;
              row = iter2;
              plane = iter3;
              x = (column*resx)+(resx/2);
              y = (row*resy)+(resy/2);
              z = (plane*resz)+(resz/2);
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
                    axiselim[elimcount][elimcount1] = 0.0001;
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
              double axis1comp = constmat[0][0]/radcur1;
              double axis2comp = constmat[1][0]/radcur2;
              double axis3comp = constmat[2][0]/radcur3;
              if(shapeclass == 3)
              {
                if(fabs(axis1comp) <= 1 && fabs(axis2comp) <= 1 && fabs(axis3comp) <= 1)
                {
                  inside = 1;
                  axis1comp = axis1comp+1;
                  axis2comp = axis2comp+1;
                  axis3comp = axis3comp+1;
                  if(((-axis1comp)+(-axis2comp)+(axis3comp)-((-0.5*Nvalue)+(-0.5*Nvalue)+2)) > 0) inside = -1;
                  if(((axis1comp)+(-axis2comp)+(axis3comp)-((2-(0.5*Nvalue))+(-0.5*Nvalue)+2)) > 0) inside = -1;
                  if(((axis1comp)+(axis2comp)+(axis3comp)-((2-(0.5*Nvalue))+(2-(0.5*Nvalue))+2)) > 0) inside = -1;
                  if(((-axis1comp)+(axis2comp)+(axis3comp)-((-0.5*Nvalue)+(2-(0.5*Nvalue))+2)) > 0) inside = -1;
                  if(((-axis1comp)+(-axis2comp)+(-axis3comp)-((-0.5*Nvalue)+(-0.5*Nvalue))) > 0) inside = -1;
                  if(((axis1comp)+(-axis2comp)+(-axis3comp)-((2-(0.5*Nvalue))+(-0.5*Nvalue))) > 0) inside = -1;
                  if(((axis1comp)+(axis2comp)+(-axis3comp)-((2-(0.5*Nvalue))+(2-(0.5*Nvalue)))) > 0) inside = -1;
                  if(((-axis1comp)+(axis2comp)+(-axis3comp)-((-0.5*Nvalue)+(2-(0.5*Nvalue)))) > 0) inside = -1;
                }
              }
              if(shapeclass == 2)
              {
                axis1comp = pow(axis1comp,Nvalue);
                axis2comp = pow(axis2comp,Nvalue);
                axis3comp = pow(axis3comp,Nvalue);
                inside = 1-axis1comp-axis2comp-axis3comp;
              }
              if(shapeclass == 1)
              {
                axis1comp = pow(axis1comp,2);
                axis2comp = pow(axis2comp,2);
                axis3comp = pow(axis3comp,2);
                inside = 1-axis1comp-axis2comp-axis3comp;
              }
              if(inside >= 0)
              {
                int currentpoint = (xpoints*ypoints*plane)+(xpoints*row)+column;
                int resdiff = int(double(resx1)/double(resx)+0.00000000001);
                int columnleft = column%resdiff;
                int rowleft = row%resdiff;
                int planeleft = plane%resdiff;
                if(columnleft == (resdiff/2) && rowleft == (resdiff/2) && planeleft == (resdiff/2))
                {
                  int columnc = column/resdiff;
                  int rowc = row/resdiff;
                  int planec = plane/resdiff;
                  int currentpointcourse = (xpointscourse*ypointscourse*planec)+(xpointscourse*rowc)+columnc;
                  gridcourse[currentpointcourse].inside = in;
                }
                gridfine[currentpoint].inside = in;
                insidecount++;
                int used = gridfine[currentpoint].available90;
                if(used == 0 && overlapassignment == 1) badcount++;
                if(used == 0 && overlapassignment == 2)
                {
                  int usedgrain = gridfine[currentpoint].grainname;
                  if((i%2) == 0)
                  {
                    gremovals[usedgrain]++;
                  }
                  if((i%2) == 1)
                  {
                    badcount++;
                  }
                }
              }
            }
          }
        }
        double uberincrease = 0;
        double nsdistchange = 0;
        for (int n = 0; n < i; n++)
        {
          double xold = packedgrain[n].x;
          double yold = packedgrain[n].y;
          double zold = packedgrain[n].z;
          double radold1 = packedgrain[n].radius1;
          double radold2 = packedgrain[n].radius2;
          double radold3 = packedgrain[n].radius3;
          double xdist = fabs(xc-xold);
          double ydist = fabs(yc-yold);
          double zdist = fabs(zc-zold);
          double totdist = (xdist*xdist)+(ydist*ydist)+(zdist*zdist);
          totdist = pow(totdist,0.5);
          if((totdist < (0.7*(radcur1+radold1))) && (totdist < (0.7*(radcur1+radold1))) && (totdist < (0.7*(radcur1+radold1))))
          {
            int nnum = packedgrain[n].tempneighnum;
            double lognnum = log10(double(nnum));
            if(nnum == 0) lognnum = 0;
            double nsizevol = 1.3333333333*(3.1415926535897)*radold1*radold2*radold3;
            double nsize3 = ((nsizevol*(0.75))/(3.1415926535897));
            double nsize = 2*(pow(nsize3,0.33333333333));
            double sizevol = 1.3333333333*(3.1415926535897)*radcur1*radcur2*radcur3;
            double size3 = ((sizevol*(0.75))/(3.1415926535897));
            double size = 2*(pow(size3,0.33333333333));
            int nbin = int(nsize);
            int bin = int(size);
            if(nbin >= numsizebins) nbin = numsizebins-1;
            if(bin >= numsizebins) bin = numsizebins-1;
            int was = nnum;
            nnum = nnum + 1;
            int now = nnum;
            double wasprob = svn[nbin][was];
            double nowprob = svn[nbin][now];
            double increase = (nowprob - wasprob);
            if(increase > 0) increase = 1;
            if(increase < 0) increase = -1;
            uberincrease = uberincrease + increase;
            nsdist[n][bin]++;
            double total = 0;
            for(int q = 0; q < numsizebins; q++)
            {
              total = total + nsdist[n][q];
            }
            double error = 0;
            for(int r = 0; r < numsizebins; r++)
            {
              error = error + (svs[nbin][r] - (nsdist[n][r]/total))*(svs[nbin][r] - (nsdist[n][r]/total));
            }
            double nserror = packedgrain[n].nserror;
            nsdistchange = nsdistchange + (error-nserror);
            nsdist[n][bin] = nsdist[n][bin] - 1;
          }
        }
//        double randomsize = rg.Random();
//        double randomsize1 = randomsize;
        double acceptable = badcount/insidecount;
        if(insidecount == 0)
        {
//          int stop = 0;
        }
        int toomuch = 0;
        if(overlapassignment == 2)
        {
          for(int d=0;d<i;d++)
          {
            double ratio = double(packedgrain[d].currentsize-gremovals[d])/double(packedgrain[d].initsize);
            if(ratio < (1-(overlapallowed/2.0))) toomuch = 1;
          }
        }
        if(acceptable > overlapallowed || uberincrease < 0 || nsdistchange > 0 || toomuch == 1) good = 0;
        triedcount++;
      }
      if(good == 0)
      {
        for(int temp1 = xmin; temp1 < xmax+1; temp1++)
        {
          for(int temp2 = ymin; temp2 < ymax+1; temp2++)
          {
            for(int temp3 = zmin; temp3 < zmax+1; temp3++)
            {
              int point = (temp1+(temp2*xpoints)+(temp3*xpoints*ypoints));
              int inside = gridfine[point].inside;
              if(inside == 1)
              {
                gridfine[point].inside = out;
              }
            }
          }
        }
        for(int temp4 = xmin; temp4 < xmax+1; temp4++)
        {
          for(int temp5 = ymin; temp5 < ymax+1; temp5++)
          {
            for(int temp6 = zmin; temp6 < zmax+1; temp6++)
            {
              int column = temp4;
              int row = temp5;
              int plane = temp6;
              int resdiff = int(double(resx1)/double(resx)+0.00000000001);
              column = column/resdiff;
              row = row/resdiff;
              plane = plane/resdiff;
              int point = (column+(row*xpointscourse)+(plane*xpointscourse*ypointscourse));
              int inside = gridcourse[point].inside;
              if(inside == 1)
              {
                gridcourse[point].inside = out;
              }
            }
          }
        }
      }
    }
    ncount = 0;
    for (int p = 0; p < i; p++)
    {
      double xold = packedgrain[p].x;
      double yold = packedgrain[p].y;
      double zold = packedgrain[p].z;
      double radold1 = packedgrain[p].radius1;
      double radold2 = packedgrain[p].radius2;
      double radold3 = packedgrain[p].radius3;
      double xdist = fabs(xc-xold);
      double ydist = fabs(yc-yold);
      double zdist = fabs(zc-zold);
      if((xdist < (0.7*(radcur1+radold1))) && (ydist < (0.7*(radcur1+radold1))) && (zdist < (0.7*(radcur1+radold1))))
      {
        ncount++;
        int nnum = packedgrain[p].tempneighnum;
        nnum = nnum + 1;
        packedgrain[p].tempneighnum = nnum;
        double volume = 1.3333333333*(3.1415926535897)*radcur1*radcur2*radcur3;
        double volbin = volume*0.75/3.1415926535897;
        int vbin = int(2*pow(volbin,0.3333333333));
        if(vbin >= numsizebins) vbin = numsizebins-1;
        nsdist[p][vbin]++;
        double nvolume = 1.3333333333*(3.1415926535897)*radold1*radold2*radold3;
        double nvolbin = nvolume*0.75/3.1415926535897;
        int nvbin = int(2*pow(nvolbin,0.3333333333));
        if(nvbin >= numsizebins) nvbin = numsizebins-1;
        double total = 0;
        for(int q = 0; q < numsizebins; q++)
        {
          total = total + nsdist[p][q];
        }
        double error = 0;
        for(int r = 0; r < numsizebins; r++)
        {
          error = error + (svs[nvbin][r] - (nsdist[p][r]/total))*(svs[nvbin][r] - (nsdist[p][r]/total));
        }
        packedgrain[p].nserror = error;
      }
    }
    tempsurf = 0;
    for(int temp7 = xmin; temp7 < xmax+1; temp7++)
    {
      for(int temp8 = ymin; temp8 < ymax+1; temp8++)
      {
        for(int temp9 = zmin; temp9 < zmax+1; temp9++)
        {
          int point = (temp7+(temp8*xpoints)+(temp9*xpoints*ypoints));
          int inside = gridfine[point].inside;
          if(inside == 1)
          {
            int columncheck = point%xpoints;
            int rowcheck = (point/xpoints)%ypoints;
            int planecheck = point/(xpoints*ypoints);
            if(columncheck == 0 || rowcheck == 0 || planecheck == 0 || columncheck == (xpoints-1) || rowcheck == (ypoints-1) || planecheck == (zpoints-1)) tempsurf = 1;
            int current = gridfine[point].available90;
            if(current == 1)
            {
              uniquecursize++;
              totalcursize++;
              gridfine[point].grainname = i;
              voxelsvector[i].push_back(point);
            }
            if(current == 0 && overlapassignment == 2)
            {
              int checkgrain = gridfine[point].grainname;
              if((gremovals[checkgrain]/packedgrain[checkgrain].initsize) < (overlapallowed/5.0))
              {
                gridfine[point].grainname = i;
                voxelsvector[i].push_back(point);
                totalcursize++;
              }
            }
            gridfine[point].inside = out;
            gridfine[point].available90 = taken90;
          }
        }
      }
    }
    for(int temp10 = xmin; temp10 < xmax+1; temp10++)
    {
      for(int temp11 = ymin; temp11 < ymax+1; temp11++)
      {
        for(int temp12 = zmin; temp12 < zmax+1; temp12++)
        {
          int column = temp10;
          int row = temp11;
          int plane = temp12;
          int resdiff = int(double(resx1)/double(resx)+0.00000000001);
          column = column/resdiff;
          row = row/resdiff;
          plane = plane/resdiff;
          int point = (column+(row*xpointscourse)+(plane*xpointscourse*ypointscourse));
          int inside = gridcourse[point].inside;
          if(inside == 1)
          {
            int currentcourse = gridcourse[point].available90;
            if(currentcourse == 1)
            {
              uniquecurcoursesize++;
            }
            gridcourse[point].available90 = taken90;
          }
        }
      }
    }
    if(overlapassignment == 2)
    {
      for(int e=0;e<i;e++)
      {
        if((gremovals[e]/packedgrain[e].initsize) < (overlapallowed/5.0))
        {
          int newsize = packedgrain[e].currentsize-gremovals[e];
          packedgrain[e].currentsize = newsize;
        }
      }
    }
    nottakencountcourse = nottakencountcourse - uniquecurcoursesize;
    nottakencount = nottakencount - uniquecursize;
    uniquecursize = 0;
    uniquecurcoursesize = 0;
    int pointcolumn = j%xpointscourse;
    int pointrow = (j/xpointscourse)%ypointscourse;
    int pointplane = j/(xpointscourse*ypointscourse);
    double pointx = (pointcolumn*resx1)+(resx1/2);
    double pointy = (pointrow*resy1)+(resy1/2);
    double pointz = (pointplane*resz1)+(resz1/2);
    double ea1 = grains[i].euler1;
    double ea2 = grains[i].euler2;
    double ea3 = grains[i].euler3;
    packedgrain[i].initsize = totalcursize;
    packedgrain[i].currentsize = totalcursize;
    packedgrain[i].grainname = i;
    packedgrain[i].xc = pointx;
    packedgrain[i].yc = pointy;
    packedgrain[i].zc = pointz;
    packedgrain[i].axis1 = radcur1;
    packedgrain[i].axis2 = radcur2;
    packedgrain[i].axis3 = radcur3;
    packedgrain[i].temponsurf = tempsurf;
    packedgrain[i].euler1 = ea1;
    packedgrain[i].euler2 = ea2;
    packedgrain[i].euler3 = ea3;
    packedgrain[i].tempneighnum = ncount;
    totalcursize = 0;
    cout << i << "-" << pointx << "-" << pointy << "-" << pointz << "-" << radcur1 << "-" << radcur2 << "-" << radcur3 << endl;
    count++;
  }
  for(int u=0;u<numgrains;u++)
  {
    gsizes[u] = 0;
  }
  for(int t=0;t<(xpoints*ypoints*zpoints);t++)
  {
    int available = gridfine[t].available90;
    if(available == 0)
    {
      int gname = gridfine[t].grainname;
      gsizes[gname]++;
    }
  }
  for(int v=0;v<numgrains;v++)
  {
    int cursize = gsizes[v];
    packedgrain[v].currentsize = cursize;
  }
}

void  GrainGeneratorFunc::fill_gaps(int numgrains)
{
  int count = 1;
//  int dup = 0;
  int fixed = 0;
  vector<int> neighs;
  vector<int> remove;
  int column = 0;
  int row = 0;
  int plane = 0;
  int *n;
  n = new int[numgrains];
  while(count != 0)
  {
    count = 0;
    for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
    {
      int taken = gridfine[i].available90;
      if(taken == 1)
      {
        for(int c = 0; c < numgrains; c++)
        {
          n[c] = 0;
        }
        column = i%xpoints;
        row = (i/xpoints)%ypoints;
        plane = i/(xpoints*ypoints);
        count++;
        if(column > 0)
        {
          int grain1 = gridfine[i-1].grainname;
          int taken1 = gridfine[i-1].available90;
          if(taken1 == 0)
          {
            neighs.push_back(grain1);
          }
        }
        if(column < (sizex/resx)-1)
        {
          int grain2 = gridfine[i+1].grainname;
          int taken2 = gridfine[i+1].available90;
          if(taken2 == 0)
          {
            neighs.push_back(grain2);
          }
        }
        if(row > 0)
        {
          int grain3 = gridfine[i-(xpoints)].grainname;
          int taken3 = gridfine[i-(xpoints)].available90;
          if(taken3 == 0)
          {
            neighs.push_back(grain3);
          }
        }
        if(row < (sizey/resy)-1)
        {
          int grain4 = gridfine[i+(xpoints)].grainname;
          int taken4 = gridfine[i+(xpoints)].available90;
          if(taken4 == 0)
          {
            neighs.push_back(grain4);
          }
        }
        if(plane > 0)
        {
          int grain5 = gridfine[i-(xpoints*ypoints)].grainname;
          int taken5 = gridfine[i-(xpoints*ypoints)].available90;
          if(taken5 == 0)
          {
            neighs.push_back(grain5);
          }
        }
        if(plane < (sizez/resz)-1)
        {
          int grain6 = gridfine[i+(xpoints*ypoints)].grainname;
          int taken6 = gridfine[i+(xpoints*ypoints)].available90;
          if(taken6 == 0)
          {
            neighs.push_back(grain6);
          }
        }
        int now = 0;
        int then = 0;
        double thendiff = 0;
        double nowdiff = 0;
        double nowsizegoal = 0;
        double nowsize = 0;
        int nowtemponsurf = 0;
        int curgrain = 0;
        int size = int(neighs.size());
        for(int k=0;k<size;k++)
        {
          int first = neighs[k];
          n[first]++;
          now = n[first];
          nowsize = packedgrain[first].currentsize;
          nowsizegoal = grains[first].volume;
          nowtemponsurf = packedgrain[first].temponsurf;
          nowdiff = nowsizegoal-nowsize;
          if(now > then)
//          if(now > then || (now == then && nowdiff > thendiff))
          {
            then = now;
            thendiff = nowdiff;
            curgrain = first;
          }
        }
        if(size > 0)
        {
          int bordered = 1;
          gridfine[i].hasneighbor = bordered;
          gridfine[i].neighbor = curgrain;
          neighs.clear();
        }
      }
    }
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int taken = gridfine[j].available90;
      int hasneigh = gridfine[j].hasneighbor;
      if(taken == 1 && hasneigh == 1)
      {
        int neighbor = gridfine[j].neighbor;
        gridfine[j].grainname = neighbor;
        gridfine[j].available90 = fixed;
      }
    }
  }
}


void  GrainGeneratorFunc::loadMisoData(string inname10)
{
    ifstream inputFile;
    inputFile.open(inname10.c_str());
  int count = 0;
  double height = 0;
    for (int k = 0; k < nummisobins; k++)
    {
    inputFile >> height;
    actualmisobin[count].height = height;
        count++;
    }
    inputFile.close();
}

void  GrainGeneratorFunc::loadMicroData(string inname11)
{
    ifstream inputFile;
    inputFile.open(inname11.c_str());
  int count = 0;
  double height = 0;
    for (int k = 0; k < nummicrobins; k++)
    {
    inputFile >> height;
    actualmicrobin[count].height = height;
        count++;
    }
    inputFile.close();
}

void  GrainGeneratorFunc::rank_misobins(int numgrains)
{
  int count = 0;
  int check = 0;
  for(int h = 0; h < nummisobins; h++)
  {
    int tempheight = 0;
    simmisobin[h].height = tempheight;
  }
  for(int i = 0; i < numgrains; i++)
  {
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(misolist.size());
    for(int k=0;k<size;k++)
    {
      double misofirst = misolist[k];
      int misocur = int(misofirst/(100/nummisobins));
      double height = simmisobin[misocur].height;
      height = height + 1;
      simmisobin[misocur].height = height;
      count++;
    }
  }
  for(int j = 0; j < nummisobins; j ++)
  {
    double actualheight = actualmisobin[j].height;
    actualheight = actualheight*count;
    double simheight = simmisobin[j].height;
    double diff = fabs(simheight-actualheight);
    simmisobin[j].difference = diff;
    double temp = 0;
    simmisobin[j].binrank = temp;
  }
//  int difflast = count;
  int curbin = 0;
  while(check < nummisobins)
  {
    int diffcur = 0;
    for(int k = 0; k < nummisobins; k++)
    {
      double diff = simmisobin[k].difference;
      double ranked = simmisobin[k].binrank;
      if(diff > diffcur && ranked == 0)
      {
        diffcur = int(diff);
        curbin = k;
      }
    }
    double rank = nummisobins-check;
    double actual = actualmisobin[curbin].height;
    actual = actual*count;
    double sim = simmisobin[curbin].height;
    if((sim-actual) < 0)
    {
      rank = -rank;
    }
    simmisobin[curbin].binrank = rank;
    check++;
  }
}

void  GrainGeneratorFunc::count_misorientations(int numgrains)
{
  int count = 0;
  double lowangle = 0;
  for(int i = 0; i < numgrains; i++)
  {
    lowangle = 0;
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(misolist.size());
    for(int k=0;k<size;k++)
    {
      double misofirst = misolist[k];
      if(misofirst < 15) lowangle++;
    }
    double fraction = lowangle/size;
    packedgrain[i].lowanglefraction = fraction;
    count++;
  }
}

int  GrainGeneratorFunc::rank_microbins(int numgrains)
{
  int count = 0;
  int check = 0;
  for(int h = 0; h < nummicrobins; h++)
  {
    int tempheight = 0;
    simmicrobin[h].height = tempheight;
  }
  for(int i = 0; i < numgrains; i++)
  {
    double microtexture = packedgrain[i].lowanglefraction;
    int microcur = int(microtexture*(nummicrobins));
    double height = simmicrobin[microcur].height;
    height = height + 1;
    simmicrobin[microcur].height = height;
  }
  for(int j = 0; j < nummicrobins; j ++)
  {
    double actualheight = actualmicrobin[j].height;
    actualheight = actualheight*numgrains;
    double simheight = simmicrobin[j].height;
    double diff = fabs(simheight-actualheight);
    simmicrobin[j].difference = diff;
    double temp = 0;
    simmicrobin[j].binrank = temp;
  }
  int curbin = 0;
  while(check < nummicrobins)
  {
    int diffcur = 0;
    for(int k = 0; k < nummicrobins; k++)
    {
      double diff = simmicrobin[k].difference;
      double ranked = simmicrobin[k].binrank;
      if(diff > diffcur && ranked == 0)
      {
        diffcur = int(diff);
        curbin = k;
      }
    }
    double rank = nummicrobins-check;
    double actual = actualmicrobin[curbin].height;
    actual = actual*numgrains;
    double sim = simmicrobin[curbin].height;
    if((sim-actual) < 0)
    {
      rank = -rank;
    }
    simmicrobin[curbin].binrank = rank;
    check++;
  }
  double heightcheck = simmicrobin[nummicrobins-1].height;
  double heightcheck2temp = actualmicrobin[nummicrobins-1].height;
  double heightcheck2 = heightcheck2temp*numgrains;
  double percenterror = (heightcheck2-heightcheck)/heightcheck2;
  if(percenterror < 0.15)
  {
    count = 1;
  }
return count;
}

void  GrainGeneratorFunc::rank_grains1(int numgrains)
{
  int count = 0;
  for(int i = 0; i < numgrains; i++)
  {
    double rank = 0;
    double temprank = 0;
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(misolist.size());
    for(int k=0;k<size;k++)
    {
      double misofirst = misolist[k];
      int misocur = int(misofirst/(100/nummisobins));
      temprank = simmisobin[misocur].binrank;
      rank = rank + temprank;
      if(misofirst == 0)
      {
        rank = rank + 100;
      }
    }
//    double frozen = packedgrain[i].frozen;
    packedgrain[i].grainrank = rank;
    count++;
  }
}

void  GrainGeneratorFunc::identify_grains1(int numgrains,int nummisomoves)
{
  int check = 0;
  int worst = 0;
  int clear = 0;
  double bad = 1;
  int count = 0;
  for(int h = 0; h < numgrains; h++)
  {
    packedgrain[h].picked = clear;
  }
  while(check < nummisomoves)
  {
    double rankcur = -10000000;
    count = 0;
    for(int i = 0; i < numgrains; i++)
    {
      double rank = packedgrain[i].grainrank;
      double picked = packedgrain[i].picked;
      double froze = packedgrain[i].frozen;
      if(rank > rankcur && picked == 0 && froze == 0)
      {
        rankcur = rank;
        worst = i;
        count++;
      }
    }
    packedgrain[worst].picked = bad;
    check++;
  }
}

void  GrainGeneratorFunc::move_grains1(int numgrains)
{
  int check = 0;
  vector<int> pickedlist;
  vector<int> temppickedlist;
  for(int i = 0; i < numgrains; i++)
  {
    double ea1temp = packedgrain[i].euler1;
    double ea2temp = packedgrain[i].euler2;
    double ea3temp = packedgrain[i].euler3;
    packedgrain[i].oeuler1 = ea1temp;
    packedgrain[i].oeuler2 = ea2temp;
    packedgrain[i].oeuler3 = ea3temp;
    double picked = packedgrain[i].picked;
    if(picked == 1)
    {
      pickedlist.push_back(i);
      check++;
    }
  }
  for(int j = 0; j < numgrains; j++)
  {
    double picked = packedgrain[j].picked;
    if(picked == 1)
    {
      int size = int(pickedlist.size());
      double random = rg.Random();
      double random1 = random;
      int remainder = int(random1*size);
      if(size <= remainder)
      {
        remainder = remainder - 1;
      }
      int swap = pickedlist[remainder];
      int sizecheck = int(pickedlist.size());
      if(swap == j && sizecheck > 1)
      {
        swap = pickedlist[(remainder+1)];
        pickedlist.erase(pickedlist.begin()+remainder+1);
      }
      if(swap != j) pickedlist.erase(pickedlist.begin()+remainder);
//      int grainname = packedgrain[j].grainname;
      double ea1 = packedgrain[swap].oeuler1;
      double ea2 = packedgrain[swap].oeuler2;
      double ea3 = packedgrain[swap].oeuler3;
//      int nnum = packedgrain[j].neighnum;
      packedgrain[j].euler1 = ea1;
      packedgrain[j].euler2 = ea2;
      packedgrain[j].euler3 = ea3;
    }
  }
}

void  GrainGeneratorFunc::rank_grains2(int numgrains)
{
  int count = 0;
  for(int i = 0; i < numgrains; i++)
  {
    double microrank = 0;
    double temprank = 0;
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(neighborvector[i].size());
    for(int j = 0; j < size; j++)
    {
      int neighfirst = neighborvector[i][j];
      double neighfrac = packedgrain[neighfirst].lowanglefraction;
      if(neighfrac > 0.24) neighfrac = neighfrac*4;
      if(neighfrac > 0.49) neighfrac = neighfrac;
      if(neighfrac > 0.74) neighfrac = neighfrac*2;
      temprank = neighfrac;
      double misofirst = misolist[j];
      if(misofirst < 15)
      {
        temprank = 0;
        if(neighfrac > 4)
        {
          microrank = 0;
          {break;}
        }
      }
      double ownfrac = packedgrain[i].lowanglefraction;
      if(ownfrac < 0.4) temprank = temprank*2;
      microrank = microrank + temprank;
    }
    packedgrain[i].grainrank = microrank;
    count++;
  }
}

void  GrainGeneratorFunc::identify_grains2(int numgrains,int nummicromoves)
{
  int check = 0;
  int check2 = 0;
  int worst = 0;
  int clear = 0;
  double bad = 1;
  double none = 2;
  nummicromoves = 150;
  for(int h = 0; h < numgrains; h++)
  {
    packedgrain[h].picked = clear;
  }
  while(check < nummicromoves)
  {
    double rankcur = 0;
    for(int i = 0; i < numgrains; i++)
    {
      double rank = packedgrain[i].grainmicrorank;
      double picked = packedgrain[i].picked;
      if(rank > rankcur && picked == 0)
      {
        rankcur = rank;
        worst = i;
      }
    }
    packedgrain[worst].picked = bad;
    check++;
  }
  while(check2 < nummicromoves)
  {
    double rankcur2 = 0;
    for(int j = 0; j < numgrains; j++)
    {
      double rank2 = packedgrain[j].grainmicrorank;
      double picked2 = packedgrain[j].picked;
      if(rank2 > rankcur2 && picked2 == 0)
      {
        rankcur2 = rank2;
        worst = j;
      }
    }
    packedgrain[worst].picked = none;
    check2++;
  }
}

void  GrainGeneratorFunc::move_grains2(int numgrains)
{
  int check = 0;
  vector<int> pickedlist;
  vector<int> nonelist;
  vector<int> tempnonelist;
  for(int i = 0; i < numgrains; i++)
  {
    double ea1temp = packedgrain[i].euler1;
    double ea2temp = packedgrain[i].euler2;
    double ea3temp = packedgrain[i].euler3;
    packedgrain[i].oeuler1 = ea1temp;
    packedgrain[i].oeuler2 = ea2temp;
    packedgrain[i].oeuler3 = ea3temp;
    double picked = packedgrain[i].picked;
    if(picked == 2)
    {
      nonelist.push_back(i);
      check++;
    }
  }
  for(int j = 0; j < numgrains; j++)
  {
    double picked = packedgrain[j].picked;
    if(picked == 1)
    {
      int size = int(nonelist.size());
      double random = rg.Random();
      double random1 = random;
      int remainder = int(random1*size);
      if(size <= remainder)
      {
        remainder = remainder - 1;
      }
      int swap = nonelist[remainder];
      nonelist.erase(nonelist.begin()+remainder);
//      int grainname = packedgrain[j].grainname;
      double ea1 = packedgrain[swap].oeuler1;
      double ea2 = packedgrain[swap].oeuler2;
      double ea3 = packedgrain[swap].oeuler3;
//      int nnum = packedgrain[j].neighnum;
      packedgrain[j].euler1 = ea1;
      packedgrain[j].euler2 = ea2;
      packedgrain[j].euler3 = ea3;
//      int sgrainname = packedgrain[swap].grainname;
      double sea1 = packedgrain[j].oeuler1;
      double sea2 = packedgrain[j].oeuler2;
      double sea3 = packedgrain[j].oeuler3;
//      int snnum = packedgrain[swap].neighnum;
      packedgrain[swap].euler1 = sea1;
      packedgrain[swap].euler2 = sea2;
      packedgrain[swap].euler3 = sea3;
    }
  }
}

void  GrainGeneratorFunc::freeze_grains(int numgrains)
{
//  int count = 0;
  double froze = 0;
  for(int i = 0; i < numgrains; i++)
  {
    froze = 0;
    double fraction = packedgrain[i].lowanglefraction;
    if(fraction > 0.85)
    {
      froze = 1;
    }
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(neighborvector[i].size());
    for(int j = 0; j < size; j++)
    {
      int neighfirst = neighborvector[i][j];
      double neighmiso = misolist[j];
      double neighfrac = packedgrain[neighfirst].lowanglefraction;
      if(neighfrac > 0.85 && neighmiso < 15)
      {
        froze = 1;
      }
    }
    packedgrain[i].frozen = froze;
  }
}


void  GrainGeneratorFunc::writeCube(string outname1, int numgrains)
{
    ofstream outFile;
    outFile.open(outname1.c_str());
  int xpoints = int(sizex/resx);
  int ypoints = int(sizey/resy);
  int zpoints = int(sizez/resz);
  outFile << "# vtk DataFile Version 2.0" << endl;
  outFile << "data set from FFT2dx_GB" << endl;
  outFile << "ASCII" << endl;
  outFile << "DATASET STRUCTURED_POINTS" << endl;
  outFile << "DIMENSIONS " << xpoints << " " << ypoints << " " << zpoints << endl;
  outFile << "ORIGIN 0.0 0.0 0.0" << endl;
  outFile << "SPACING " << resx << " " << resy << " " << resz << endl;
  outFile << "POINT_DATA " << xpoints*ypoints*zpoints << endl;
  outFile << endl;
  outFile << endl;
  outFile << "SCALARS GrainID int  1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    if(i%20 == 0 && i > 0) outFile << endl;
    int name = gridfine[i].grainname;
//    int column = (i%xpoints);
//    int row = ((i/xpoints)%ypoints);
//    int plane = (i/(xpoints*ypoints));
//    double xc = (column*resx)+(resx/2);
//    double yc = (row*resy)+(resy/2);
//    double zc = (plane*resz)+(resz/2);
//    int gnum = gridfine[i].grainname;
//    double r = packedgrain[gnum].red;
//    double g = packedgrain[gnum].green;
//    double b = packedgrain[gnum].blue;
    outFile << "   ";
    if(name < 10000) outFile << " ";
    if(name < 1000) outFile << " ";
    if(name < 100) outFile << " ";
    if(name < 10) outFile << " ";
    outFile << name;
  }
  outFile.close();
}


void  GrainGeneratorFunc::write_grains(string outname4, int numgrains)
{
    ofstream outFile;
    outFile.open(outname4.c_str());
  outFile << numgrains << endl;
  for(int i=0;i<numgrains;i++)
  {
    int name = packedgrain[i].grainname;
    double xc = packedgrain[i].xc;
    double yc = packedgrain[i].yc;
    double zc = packedgrain[i].zc;
    double ea1 = packedgrain[i].euler1;
    double ea2 = packedgrain[i].euler2;
    double ea3 = packedgrain[i].euler3;
    int nnum = packedgrain[i].neighnum;
    outFile << name << "  " << xc << "  " << yc << "  " << zc << "  " << ea1 << " " << ea2 << " " << ea3 << " " << nnum;
    int size = int(neighborvector[i].size());
    for(int j = 0; j < size; j++)
    {
      int neigh = neighborvector[i][j];
      outFile << "  " << neigh;
    }
    outFile << endl;
  }
}
double GrainGeneratorFunc::gamma(double x)
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

