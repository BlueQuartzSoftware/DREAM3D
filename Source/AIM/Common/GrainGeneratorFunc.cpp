////////////////////////////////////////////////////////////////////////////
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


const static double m_onepointthree = 1.33333333333;
const static double m_pi = 3.1415926535897;

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

#define DIMS "DIMENSIONS"
#define LOOKUP "LOOKUP_TABLE"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

using namespace std;

GrainGeneratorFunc::GrainGeneratorFunc() :
  grains(NULL),
  precipitates(NULL),
  gsizes(NULL),
  gremovals(NULL),
  actualodf(NULL),
  simodf(NULL),
  axisodf(NULL),
  gridfine(NULL),
  gridcourse(NULL),
  actualmdf(NULL),
  simmdf(NULL),
  actualmicrotex(NULL),
  simmicrotex(NULL)
{

}

GrainGeneratorFunc::~GrainGeneratorFunc()
{

  delete [] grains;
  delete [] precipitates;
  delete [] gsizes;
  delete [] gremovals;
  delete [] actualodf;
  delete [] simodf;
  delete [] axisodf;
  delete [] gridfine;
  delete [] gridcourse;
  delete [] actualmdf;
  delete [] simmdf;
  delete [] actualmicrotex;
  delete [] simmicrotex;

}

void GrainGeneratorFunc::initialize(int32 m_NumGrains, int32 m_ShapeClass,
              double m_XResolution, double m_YResolution, double m_ZResolution, int32 m_OverlapAllowed,
              int32 m_OverlapAssignment, int32 m_Precipitates, int32 m_CrystalStructure, double m_FractionPrecipitates)
{

  resx = m_XResolution;
  resy = m_YResolution;
  resz = m_ZResolution;

  resx1 = 2*m_XResolution;
  resy1 = 2*m_YResolution;
  resz1 = 2*m_ZResolution;

  resdiff = 2;
  fractionprecip = m_FractionPrecipitates;

  numgrains = m_NumGrains;
  shapeclass = m_ShapeClass;
  preciptype = m_Precipitates;
  overlapassignment = m_OverlapAssignment;
  overlapallowed = m_OverlapAllowed;
  crystruct = m_CrystalStructure;

  grains = new Grain[numgrains+1];
  gsizes = new int[numgrains+1];
  gremovals = new int[numgrains+1];
  actualodf = new Bin[18*18*18];
  simodf = new Bin[18*18*18];
  axisodf = new Bin[18*18*18];
  precipaxisodf = new Bin[18*18*18];
  actualmdf = new Bin[36];
  simmdf = new Bin[36];
  actualmicrotex = new Bin[10];
  simmicrotex = new Bin[10];
}

void GrainGeneratorFunc::initialize2()
{

  totalpoints = xpoints * ypoints * zpoints;
  totalpoints1 = (xpoints/2) * (ypoints/2) * (zpoints/2);

  gridfine = new Voxel[totalpoints];
  gridcourse = new Voxel[totalpoints1];
}

void  GrainGeneratorFunc::loadStatsData(string inname1)
{
    ifstream inputFile;
    inputFile.open(inname1.c_str());
	int diam=0;
	double param1=0;
	double param2=0;
	string word;
	inputFile >> word;
	while(!inputFile.eof())
	{
		inputFile >> word;
		if(word == "Grain_Diameter_Info")
		{
			inputFile >> numdiameters >> maxdiameter >> mindiameter;
		}
		if(word == "Precipitate_Diameter_Info")
		{
			inputFile >> numprecipdiameters >> maxprecipdiameter >> minprecipdiameter;
		}
		if(word == "Grain_Size_Distribution")
		{
			inputFile >> avgdiam >> sddiam;
		}
		if(word == "Precipitate_Size_Distribution")
		{
			inputFile >> avgprecipdiam >> sdprecipdiam;
		}
		if(word == "Grain_SizeVBoverA_Distributions")
		{
			bovera.resize(maxdiameter+1);
			for(int temp7 = 0; temp7 < maxdiameter+1; temp7++)
			{
				if(temp7 < mindiameter) bovera[temp7].resize(2,0);
				if(temp7 >= mindiameter)
				{
					inputFile >> diam >> param1 >> param2;
					bovera[diam].resize(2);
					bovera[diam][0]=param1;
					bovera[diam][1]=param2;
				}
			}	
		}
		if(word == "Grain_SizeVCoverA_Distributions")
		{
			covera.resize(maxdiameter+1);
			for(int temp7 = 0; temp7 < maxdiameter+1; temp7++)
			{
				if(temp7 < mindiameter) covera[temp7].resize(2,0);
				if(temp7 >= mindiameter)
				{
					inputFile >> diam >> param1 >> param2;
					covera[diam].resize(2);
					covera[diam][0]=param1;
					covera[diam][1]=param2;
				}
			}
		}
		if(word == "Grain_SizeVCoverB_Distributions")
		{
			coverb.resize(maxdiameter+1);
			for(int temp7 = 0; temp7 < maxdiameter+1; temp7++)
			{
				if(temp7 < mindiameter) coverb[temp7].resize(2,0);
				if(temp7 >= mindiameter)
				{
					inputFile >> diam >> param1 >> param2;
					coverb[diam].resize(2);
					coverb[diam][0]=param1;
					coverb[diam][1]=param2;
				}
			}
		}
		if(word == "Grain_SizeVNeighbors_Distributions")
		{
			svn.resize(maxdiameter+1);
			for(int temp7 = 0; temp7 < maxdiameter+1; temp7++)
			{
				if(temp7 < mindiameter) svn[temp7].resize(2,0);
				if(temp7 >= mindiameter)
				{
					inputFile >> diam >> param1 >> param2;
					svn[diam].resize(2);
					svn[diam][0]=param1;
					svn[diam][1]=param2;
				}
			}
		}
		if(word == "Grain_SizeVNeighborSize_Distributions")
		{
			svs.resize(maxdiameter+1);
			for(int temp7 = 0; temp7 < maxdiameter+1; temp7++)
			{
				if(temp7 < mindiameter) svs[temp7].resize(2,0);
				if(temp7 >= mindiameter)
				{
					inputFile >> diam >> param1 >> param2;
					svs[diam].resize(2);
					svs[diam][0]=param1;
					svs[diam][1]=param2;
				}
			}
		}
		if(word == "Grain_SizeVOmega3_Distributions")
		{
			svomega3.resize(maxdiameter+1);
			for(int temp7 = 0; temp7 < maxdiameter+1; temp7++)
			{
				if(temp7 < mindiameter) svomega3[temp7].resize(2,0);
				inputFile >> diam >> param1 >> param2;
				svomega3[diam].resize(2);
				svomega3[diam][0]=param1;
				svomega3[diam][1]=param2;
			}
		}
		if(word == "Precipitate_SizeVBoverA_Distributions")
		{
			precipbovera.resize(maxprecipdiameter+1);
			for(int temp7 = 0; temp7 < maxprecipdiameter+1; temp7++)
			{
				if(temp7 < minprecipdiameter) precipbovera[temp7].resize(2,0);
				if(temp7 >= minprecipdiameter)
				{
					inputFile >> diam >> param1 >> param2;
					precipbovera[diam].resize(2);
					precipbovera[diam][0]=param1;
					precipbovera[diam][1]=param2;
				}
			}	
		}
		if(word == "Precipitate_SizeVCoverA_Distributions")
		{
			precipcovera.resize(maxprecipdiameter+1);
			for(int temp7 = 0; temp7 < maxprecipdiameter+1; temp7++)
			{
				if(temp7 < minprecipdiameter) precipcovera[temp7].resize(2,0);
				if(temp7 >= minprecipdiameter)
				{
					inputFile >> diam >> param1 >> param2;
					precipcovera[diam].resize(2);
					precipcovera[diam][0]=param1;
					precipcovera[diam][1]=param2;
				}
			}
		}
		if(word == "Precipitate_SizeVCoverB_Distributions")
		{
			precipcoverb.resize(maxprecipdiameter+1);
			for(int temp7 = 0; temp7 < maxprecipdiameter+1; temp7++)
			{
				if(temp7 < minprecipdiameter) precipcoverb[temp7].resize(2,0);
				if(temp7 >= minprecipdiameter)
				{
					inputFile >> diam >> param1 >> param2;
					precipcoverb[diam].resize(2);
					precipcoverb[diam][0]=param1;
					precipcoverb[diam][1]=param2;
				}
			}
		}
		if(word == "Precipitate_SizeVOmega3_Distributions")
		{
			precipsvomega3.resize(maxprecipdiameter+1);
			for(int temp7 = 0; temp7 < maxprecipdiameter+1; temp7++)
			{
				if(temp7 < minprecipdiameter) precipsvomega3[temp7].resize(2,0);
				if(temp7 >= minprecipdiameter)
				{
					inputFile >> diam >> param1 >> param2;
					precipsvomega3[diam].resize(2);
					precipsvomega3[diam][0]=param1;
					precipsvomega3[diam][1]=param2;
				}
			}
		}
	}
    inputFile.close();
}


void  GrainGeneratorFunc::loadorientData(string inname6)
{
  ifstream inputFile;
  inputFile.open(inname6.c_str());
  double density;
  double totaldensity = 0;
  string word;
  inputFile >> word;
  while(!inputFile.eof())
  {
	inputFile >> word;
	if(word == "Grain_AxisODF")
	{
	  for (long k = 0; k < (18*18*18); k++)
	  {
		inputFile >> density;
		totaldensity = totaldensity+density;
		axisodf[k].density = totaldensity;
	  }
	}
	if(word == "Precipitate_AxisODF")
	{
	  for (long k = 0; k < (18*18*18); k++)
	  {
		inputFile >> density;
		totaldensity = totaldensity+density;
		precipaxisodf[k].density = totaldensity;
	  }
	}
  }
  inputFile.close();
}

void  GrainGeneratorFunc::loadeulerData(string inname7)
{
  ifstream inputFile;
  inputFile.open(inname7.c_str());
  double density;
  for(int i=0;i<(18*18*18);i++)
  {
	  inputFile >> density;
	  actualodf[i].density = density;
  }
  inputFile.close();
}

void  GrainGeneratorFunc::generate_grains(int numgrains)
{
  int good = 0;
  double r1 = 1;
  double u=0,u1=0,u2=0;
  double a1=0,a2=0,a3=0;
  double b1=0,b2=0,b3=0;
  double r2=0,r3=0;
  double diam = 0;
  double vol = 0;
  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
  totalvol = 0;
  for(int l=1;l<numgrains+1;l++)
  {
    int volgood = 0;
    while(volgood == 0)
	{
		volgood = 1;
	    u = rg.Random();
		diam = rg.RandNorm(avgdiam,sddiam);
		if((fabs(diam-avgdiam)/sddiam) > 2.0) volgood = 0;
		diam = exp(diam);
		vol = (4.0/3.0)*(m_pi)*((diam/2.0)*(diam/2.0)*(diam/2.0));
	}
    int diameter = int(diam);
    if(diameter >= maxdiameter) diameter = maxdiameter;
    if(diameter <= mindiameter) diameter = mindiameter;
    good = 0;
    while(good == 0)
    {
	  a1 = bovera[diameter][0];
	  b1 = bovera[diameter][1];
	  r2 = rg.RandBeta(a1,b1);
	  a2 = covera[diameter][0];
	  b2 = covera[diameter][1];
	  r3 = rg.RandBeta(a2,b2);
      double cob = r3/r2;
	  a3 = coverb[diameter][0];
	  b3 = coverb[diameter][1];
	  double prob = ((gamma((a3+b3))/(gamma(a3)*gamma(b3)))*(pow(cob,(a3-1)))*(pow((1-cob),(b3-1))));
	  double check = rg.Random();
      if(prob > check) good = 1;
      if(cob > 1) good = 0;
    }
	double random = rg.Random();
	int bin=0;
	for(int i=0;i<(18*18*18);i++)
	{
		double density = axisodf[i].density;
		if(random > density) bin = i;
		if(random < density) {break;}
	}
	double phi1 = bin%18;
	double PHI = (bin/18)%18;
	double phi2 = bin/(18*18);
	random = rg.Random();
	phi1 = ((phi1*5)+(random*5))*(m_pi/180.0);
	random = rg.Random();
	PHI = ((PHI*5)+(random*5))*(m_pi/180.0);
	random = rg.Random();
	phi2 = ((phi2*5)+(random*5))*(m_pi/180.0);
	double m = svomega3[diameter][0];
	double s = svomega3[diameter][1];
	double omega3 = rg.RandBeta(m,s);
    grains[l].volume = vol;
	grains[l].radius1 = r1;
	grains[l].radius2 = r2;
	grains[l].radius3 = r3;
    grains[l].axiseuler1 = phi1;
    grains[l].axiseuler2 = PHI;
    grains[l].axiseuler3 = phi2;
	grains[l].omega3 = omega3;
    totalvol = totalvol + vol;
  }
  double sizex = int(pow((totalvol*1),0.33333));
  double sizey = int(pow((totalvol*1),0.33333));
  double sizez = int(pow((totalvol*1),0.33333));
  xpoints = int((sizex/resx)+1);
  ypoints = int((sizey/resy)+1);
  zpoints = int((sizez/resz)+1);
  xpoints = int(xpoints/4)*4;
  ypoints = int(ypoints/4)*4;
  zpoints = int(zpoints/4)*4;
  xpoints1 = xpoints/int(resx1/resx);
  ypoints1 = ypoints/int(resx1/resx);
  zpoints1 = zpoints/int(resx1/resx);
  totalvol = (xpoints*resx)*(ypoints*resy)*(zpoints*resz);
  grainorder.resize(numgrains+1);
  takencheck.resize(numgrains+1);
  for(int i=1;i<numgrains+1;i++)
  {
    int maxgrain = 0;
    double maxvol = 0;
    for(int j=1;j<numgrains+1;j++)
    {
      double vol = grains[j].volume;
      if(vol > maxvol && takencheck[j] != 1)
      {
        maxvol = vol;
        maxgrain = j;
      }
    }
    takencheck[maxgrain] = 1;
	grainorder[i] = maxgrain;
  }
}

void  GrainGeneratorFunc::pack_grains(int numgrains)
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
  double Nvalue = 0;
  double Gvalue = 0;
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
  int uniquecursize = 0;
  int totalcursize = 0;
  nsdist.resize(numgrains+1);
  srand(static_cast<unsigned int>(time(NULL)));
  vector<long> availablelist;
  vector<long> tempavailablelist;
  vector<long> insidelist;
  availablelist.resize(xpoints1*ypoints1*zpoints1);
  tempavailablelist.resize(xpoints1*ypoints1*zpoints1);
  insidelist.resize(1000,0);
  for (long a = 0; a < (xpoints1*ypoints1*zpoints1); a++)
  {
	availablelist[a] = a;
  }
  for(int i=1;i<numgrains+1;i++)
  {
	nsdist[i].resize(maxdiameter+1);
    good = 0;
	int curgrain = grainorder[i];
    double volcur = grains[curgrain].volume;
	double bovera = grains[curgrain].radius2;
	double covera = grains[curgrain].radius3;
	double omega3 = grains[curgrain].omega3;
    double radcur1 = 1;
    if(shapeclass == 3)
    {
	  Gvalue = omega3;
      if(Gvalue >= 0 && Gvalue <= 1)
      {
        radcur1 = (volcur*6.0)/(6-(Gvalue*Gvalue*Gvalue));
      }
      if(Gvalue > 1 && Gvalue <= 2)
      {
        radcur1 = (volcur*6.0)/(3+(9*Gvalue)-(9*Gvalue*Gvalue)+(2*Gvalue*Gvalue*Gvalue));
      }
    }
    if(shapeclass == 2)
    {
	  Nvalue = omega3;
      double beta1 = (gamma((1.0/Nvalue))*gamma((1.0/Nvalue)))/gamma((2.0/Nvalue));
      double beta2 = (gamma((2.0/Nvalue))*gamma((1.0/Nvalue)))/gamma((3.0/Nvalue));
      radcur1 = (volcur*(3.0/2.0)*(1.0/bovera)*(1.0/covera)*((Nvalue*Nvalue)/4.0)*(1.0/beta1)*(1.0/beta2));
    }
    if(shapeclass == 1)
    {
      radcur1 = (volcur*(3.0/4.0)*(1.0/m_pi)*(1.0/bovera)*(1.0/covera));
    }
    radcur1 = pow(radcur1,0.333333333333);
    if(shapeclass == 3) radcur1 = radcur1/2.0;
    double radcur2 = (radcur1*bovera);
    double radcur3 = (radcur1*covera);
	double phi1 = grains[curgrain].axiseuler1;
	double PHI = grains[curgrain].axiseuler2;
	double phi2 = grains[curgrain].axiseuler3;
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
	tempavailablelist = availablelist;
    while(good == 0)
    {
        double random = rg.Random();
		pointsleft = tempavailablelist.size();
        int remainder = int(random*pointsleft);
	    if(remainder == pointsleft) remainder = pointsleft-1;
	    j = tempavailablelist[remainder];
	    tempavailablelist.erase(tempavailablelist.begin()+remainder);
		for(int c=0;c<i;c++)
		{
		  gremovals[c]=0;
		}
        good = 1;
        columncourse = (j%xpoints1);
        rowcourse = ((j/xpoints1)%ypoints1);
        planecourse = (j/(xpoints1*ypoints1));
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
        xmax = xpoints-1;
        ymin = 0;
        ymax = ypoints-1;
        zmin = 0;
        zmax = zpoints-1;
        if(column-((radcur1/resx)+1) > 0)
        {
          xmin = int(column-((radcur1/resx)+1));
        }
        if(column+((radcur1/resx)+1) < xpoints-1)
        {
          xmax = int(column+((radcur1/resx)+1));
        }
        if(row-((radcur1/resy)+1) > 0)
        {
          ymin = int(row-((radcur1/resy)+1));
        }
        if(row+((radcur1/resy)+1) < ypoints-1)
        {
          ymax = int(row+((radcur1/resy)+1));
        }
        if(plane-((radcur1/resz)+1) > 0)
        {
          zmin = int(plane-((radcur1/resz)+1));
        }
        if(plane+((radcur1/resz)+1) < zpoints-1)
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
                  if(((-axis1comp)+(-axis2comp)+(axis3comp)-((-0.5*Gvalue)+(-0.5*Gvalue)+2)) > 0) inside = -1;
                  if(((axis1comp)+(-axis2comp)+(axis3comp)-((2-(0.5*Gvalue))+(-0.5*Gvalue)+2)) > 0) inside = -1;
                  if(((axis1comp)+(axis2comp)+(axis3comp)-((2-(0.5*Gvalue))+(2-(0.5*Gvalue))+2)) > 0) inside = -1;
                  if(((-axis1comp)+(axis2comp)+(axis3comp)-((-0.5*Gvalue)+(2-(0.5*Gvalue))+2)) > 0) inside = -1;
                  if(((-axis1comp)+(-axis2comp)+(-axis3comp)-((-0.5*Gvalue)+(-0.5*Gvalue))) > 0) inside = -1;
                  if(((axis1comp)+(-axis2comp)+(-axis3comp)-((2-(0.5*Gvalue))+(-0.5*Gvalue))) > 0) inside = -1;
                  if(((axis1comp)+(axis2comp)+(-axis3comp)-((2-(0.5*Gvalue))+(2-(0.5*Gvalue)))) > 0) inside = -1;
                  if(((-axis1comp)+(axis2comp)+(-axis3comp)-((-0.5*Gvalue)+(2-(0.5*Gvalue)))) > 0) inside = -1;
                }
              }
              if(shapeclass == 2)
              {
			    axis1comp = fabs(axis1comp);
			    axis2comp = fabs(axis2comp);
			    axis3comp = fabs(axis3comp);
                axis1comp = pow(axis1comp,Nvalue);
                axis2comp = pow(axis2comp,Nvalue);
                axis3comp = pow(axis3comp,Nvalue);
                inside = 1-axis1comp-axis2comp-axis3comp;
              }
              if(shapeclass == 1)
              {
			    axis1comp = fabs(axis1comp);
			    axis2comp = fabs(axis2comp);
			    axis3comp = fabs(axis3comp);
                axis1comp = pow(axis1comp,2);
                axis2comp = pow(axis2comp,2);
                axis3comp = pow(axis3comp,2);
                inside = 1-axis1comp-axis2comp-axis3comp;
              }
              if(inside >= 0)
              {
                int currentpoint = (xpoints*ypoints*plane)+(xpoints*row)+column;
				insidelist[insidecount] = currentpoint;
				if (insidecount >= (0.9*insidelist.size())) insidelist.resize(insidecount + 1000);
                insidecount++;
				if(gridfine[currentpoint].grainname != 0 && overlapassignment == 1) badcount++;
                if(gridfine[currentpoint].grainname != 0 && overlapassignment == 2)
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
        for (int n = 1; n < i; n++)
        {
          double xold = grains[n].centroidx;
          double yold = grains[n].centroidy;
          double zold = grains[n].centroidz;
          double radold1 = grains[n].radius1;
          double radold2 = grains[n].radius2;
          double radold3 = grains[n].radius3;
          double xdist = fabs(xc-xold);
          double ydist = fabs(yc-yold);
          double zdist = fabs(zc-zold);
          double totdist = (xdist*xdist)+(ydist*ydist)+(zdist*zdist);
          totdist = pow(totdist,0.5);
          if((totdist < (0.7*(radcur1+radold1))) && (totdist < (0.7*(radcur1+radold1))) && (totdist < (0.7*(radcur1+radold1))))
          {
            int nnum = grains[n].tempneighnum;
            double lognnum = log10(double(nnum));
            if(nnum == 0) lognnum = 0;
            double nsizevol = 1.3333333333*(m_pi)*radold1*radold2*radold3;
            double nsize3 = ((nsizevol*(0.75))/(m_pi));
            double nsize = 2*(pow(nsize3,0.33333333333));
            double sizevol = 1.3333333333*(m_pi)*radcur1*radcur2*radcur3;
            double size3 = ((sizevol*(0.75))/(m_pi));
            double size = 2*(pow(size3,0.33333333333));
            int nbin = int(nsize);
            int bin = int(size);
            if(nbin >= maxdiameter) nbin = maxdiameter;
            if(nbin <= mindiameter) nbin = mindiameter;
            if(bin >= maxdiameter) bin = maxdiameter;
            if(bin <= mindiameter) bin = mindiameter;
            int was = int(log(double(nnum)));
            nnum = nnum + 1;
            int now = int(log(double(nnum)));
            double wasprob = (1.0/pow((2*m_pi*svn[nbin][1]*svn[nbin][1]),0.5))*exp(-pow((was-svn[nbin][0]),2)/(2*svn[nbin][1]*svn[nbin][1]));
            double nowprob = (1.0/pow((2*m_pi*svn[nbin][1]*svn[nbin][1]),0.5))*exp(-pow((now-svn[nbin][0]),2)/(2*svn[nbin][1]*svn[nbin][1]));
            double increase = (nowprob - wasprob);
            if(increase > 0) increase = 1;
            if(increase < 0) increase = -1;
            uberincrease = uberincrease + increase;
            nsdist[n][bin]++;
            double total = 0;
            for(int q = mindiameter; q < maxdiameter+1; q++)
            {
              total = total + nsdist[n][q];
            }
            double error = 0;
            for(int r = mindiameter; r < maxdiameter+1; r++)
            {
              double diam = log(double(r));
			  error = error + ((1.0/pow((2*m_pi*svs[nbin][1]*svs[nbin][1]),0.5))*exp(-pow((diam-svs[nbin][0]),2)/(2*svs[nbin][1]*svs[nbin][1])) - (nsdist[n][r]/total))*((1.0/pow((2*m_pi*svs[nbin][1]*svs[nbin][1]),0.5))*exp(-pow((diam-svs[nbin][0]),2)/(2*svs[nbin][1]*svs[nbin][1])) - (nsdist[n][r]/total));
            }
            double nserror = grains[n].nserror;
            nsdistchange = nsdistchange + (error-nserror);
            nsdist[n][bin] = nsdist[n][bin] - 1;
          }
        }
        double acceptable = 100*(badcount/insidecount);
        int toomuch = 0;
        if(overlapassignment == 2)
        {
          for(int d=0;d<i;d++)
          {
            double ratio = double(grains[d].currentsize-gremovals[d])/double(grains[d].initsize);
            if(ratio < (1-(overlapallowed/2.0))) toomuch = 1;
          }
        }
//        if(acceptable > overlapallowed || uberincrease < 0 || nsdistchange > 0 || toomuch == 1) good = 0;
        if(acceptable > overlapallowed) good = 0;
        if(good == 0)
        {
			insidelist.clear();
			insidelist.resize(1000);
		}
	}
	ncount = 0;
    for (int p = 1; p < i; p++)
    {
	  double xold = grains[p].centroidx;
	  double yold = grains[p].centroidy;
	  double zold = grains[p].centroidz;
      double radold1 = grains[p].radius1;
      double radold2 = grains[p].radius2;
      double radold3 = grains[p].radius3;
      double xdist = fabs(xc-xold);
      double ydist = fabs(yc-yold);
      double zdist = fabs(zc-zold);
      if((xdist < (0.7*(radcur1+radold1))) && (ydist < (0.7*(radcur1+radold1))) && (zdist < (0.7*(radcur1+radold1))))
      {
        ncount++;
        int nnum = grains[p].tempneighnum;
        nnum = nnum + 1;
        grains[p].tempneighnum = nnum;
        double volume = 1.3333333333*(m_pi)*radcur1*radcur2*radcur3;
        double volbin = volume*0.75/m_pi;
        int vbin = int(2*pow(volbin,0.3333333333));
        if(vbin >= maxdiameter) vbin = maxdiameter;
        if(vbin <= mindiameter) vbin = mindiameter;
        nsdist[p][vbin]++;
        double nvolume = 1.3333333333*(m_pi)*radold1*radold2*radold3;
        double nvolbin = nvolume*0.75/m_pi;
        int nvbin = int(2*pow(nvolbin,0.3333333333));
        if(nvbin >= maxdiameter) nvbin = maxdiameter;
        if(nvbin <= mindiameter) nvbin = mindiameter;
        double total = 0;
        for(int q = mindiameter; q < maxdiameter+1; q++)
        {
          total = total + nsdist[p][q];
        }
        double error = 0;
        for(int r = mindiameter; r < maxdiameter+1; r++)
        {
          double diam = log(double(r));
		  error = error + ((1.0/pow((2*m_pi*svs[nvbin][1]*svs[nvbin][1]),0.5))*exp(-pow((diam-svs[nvbin][0]),2)/(2*svs[nvbin][1]*svs[nvbin][1])) - (nsdist[p][r]/total))*((1.0/pow((2*m_pi*svs[nvbin][1]*svs[nvbin][1]),0.5))*exp(-pow((diam-svs[nvbin][0]),2)/(2*svs[nvbin][1]*svs[nvbin][1])) - (nsdist[p][r]/total));
        }
        grains[p].nserror = error;
      }
    }
    tempsurf = 0;
	for(int a=0;a<insidecount;a++)
    {
      int point = insidelist[a];
      int columncheck = point%xpoints;
      int rowcheck = (point/xpoints)%ypoints;
      int planecheck = point/(xpoints*ypoints);
	  int columnleft = columncheck%resdiff;
	  int rowleft = rowcheck%resdiff;
	  int planeleft = planecheck%resdiff;
      if(columncheck == 0 || rowcheck == 0 || planecheck == 0 || columncheck == (xpoints-1) || rowcheck == (ypoints-1) || planecheck == (zpoints-1)) tempsurf = 1;
	  if(gridfine[point].grainname == 0)
      {
          uniquecursize++;
          totalcursize++;
          gridfine[point].grainname = i;
		  if(columnleft == (resdiff/2) && rowleft == (resdiff/2) && planeleft == (resdiff/2))
		  {
			int columnc = columncheck/resdiff;
			int rowc = rowcheck/resdiff;
			int planec = planecheck/resdiff;
			int currentpointcourse = (xpoints1*ypoints1*planec)+(xpoints1*rowc)+columnc;
			gridcourse[currentpointcourse].grainname = i;
  		  }
      }
      if(gridfine[point].grainname != 0 && overlapassignment == 2)
      {
          int checkgrain = gridfine[point].grainname;
          if((gremovals[checkgrain]/grains[checkgrain].initsize) < (overlapallowed/5.0))
          {
            gridfine[point].grainname = i;
            totalcursize++;
		    if(columnleft == (resdiff/2) && rowleft == (resdiff/2) && planeleft == (resdiff/2))
		    {
				int columnc = columncheck/resdiff;
				int rowc = rowcheck/resdiff;
				int planec = planecheck/resdiff;
				int currentpointcourse = (xpoints1*ypoints1*planec)+(xpoints1*rowc)+columnc;
				gridcourse[currentpointcourse].grainname = i;
  		    }
          }
      }
    }
	int size = availablelist.size();
	for(int p=0;p<size;p++)
	{
		int point = availablelist[p];
		if(gridcourse[point].grainname != 0)
		{
			availablelist.erase(availablelist.begin()+p);
			p = p-1;
			size = availablelist.size();
		}
	}
    if(overlapassignment == 2)
    {
      for(int e=0;e<i;e++)
      {
        if((gremovals[e]/grains[e].initsize) < (overlapallowed/5.0))
        {
          int newsize = grains[e].currentsize-gremovals[e];
          grains[e].currentsize = newsize;
        }
      }
    }
    uniquecursize = 0;
    grains[curgrain].initsize = totalcursize;
    grains[curgrain].currentsize = totalcursize;
    grains[curgrain].centroidx = xc;
    grains[curgrain].centroidy = yc;
    grains[curgrain].centroidz = zc;
    grains[curgrain].radius1 = radcur1;
    grains[curgrain].radius2 = radcur2;
    grains[curgrain].radius3 = radcur3;
    grains[curgrain].temponsurf = tempsurf;
    grains[curgrain].tempneighnum = ncount;
    totalcursize = 0;
    count++;
  }
  for(int u=1;u<numgrains+1;u++)
  {
    gsizes[u] = 0;
  }
  for(int t=0;t<(xpoints*ypoints*zpoints);t++)
  {
    if(gridfine[t].grainname != -1)
    {
      int gname = gridfine[t].grainname;
      gsizes[gname]++;
    }
  }
  for(int v=1;v<numgrains+1;v++)
  {
    int cursize = gsizes[v];
    grains[v].currentsize = cursize;
  }
}

void  GrainGeneratorFunc::assign_eulers(int numgrains)
{
  int bin = 0;
  int size = 0;
  int picked = 0;
  int gnum = 0;
  double totaldensity = 0;
  double synea1=0,synea2=0,synea3=0;
  for(int i=1;i<numgrains+1;i++)
  {
	  double random = rg.Random();
	  int choose = 0;
	  totaldensity = 0;
	  for(int j=0;j<(18*18*18);j++)
	  {
		  double density = actualodf[j].density;
		  totaldensity = totaldensity + density;
		  if(random >= totaldensity) choose = j;
	  }
	  int phi1 = choose%18;
	  int PHI = (choose/18)%18;
	  int phi2 = choose/(18*18);
	  double random1 = rg.Random();
	  double random2 = rg.Random();
	  double random3 = rg.Random();
	  synea1 = (5*phi1)+(5*random1);
	  synea2 = (5*PHI)+(5*random2);
	  synea3 = (5*phi2)+(5*random3);
	  synea1 = synea1*(m_pi/180.0);
	  synea2 = synea2*(m_pi/180.0);
	  synea3 = synea3*(m_pi/180.0);
	  grains[i].euler1 = synea1;
	  grains[i].euler2 = synea2;
	  grains[i].euler3 = synea3;
	  simodf[choose].density = simodf[choose].density + (grains[i].volume/totalvol);
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
	  if(gridfine[i].grainname == 0)
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
          if(grain1 != 0)
          {
            neighs.push_back(grain1);
          }
        }
        if(column < xpoints-1)
        {
          int grain2 = gridfine[i+1].grainname;
          if(grain2 != 0)
          {
            neighs.push_back(grain2);
          }
        }
        if(row > 0)
        {
          int grain3 = gridfine[i-(xpoints)].grainname;
          if(grain3 != 0)
          {
            neighs.push_back(grain3);
          }
        }
        if(row < ypoints-1)
        {
          int grain4 = gridfine[i+(xpoints)].grainname;
          if(grain4 != 0)
          {
            neighs.push_back(grain4);
          }
        }
        if(plane > 0)
        {
          int grain5 = gridfine[i-(xpoints*ypoints)].grainname;
          if(grain5 != 0)
          {
            neighs.push_back(grain5);
          }
        }
        if(plane < zpoints-1)
        {
          int grain6 = gridfine[i+(xpoints*ypoints)].grainname;
          if(grain6 != 0)
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
          nowsize = grains[first].currentsize;
          nowsizegoal = grains[first].volume;
          nowtemponsurf = grains[first].temponsurf;
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
      int hasneigh = gridfine[j].hasneighbor;
	  if(gridfine[j].grainname == 0 && hasneigh == 1)
      {
        int neighbor = gridfine[j].neighbor;
        gridfine[j].grainname = neighbor;
      }
    }
  }
  for(int u=1;u<numgrains+1;u++)
  {
    gsizes[u] = 0;
  }
  for(int t=0;t<(xpoints*ypoints*zpoints);t++)
  {
    if(gridfine[t].grainname != -1)
    {
      int gname = gridfine[t].grainname;
      gsizes[gname]++;
    }
  }
  for(int v=1;v<numgrains+1;v++)
  {
    int cursize = gsizes[v];
    grains[v].currentsize = cursize;
  }
}

int GrainGeneratorFunc::create_precipitates()
{
  int good = 0;
  int count = 0;
  double r1 = 1;
  double u=0,u1=0,u2=0;
  double a1=0,a2=0,a3=0;
  double b1=0,b2=0,b3=0;
  double r2=0,r3=0;
  double diam = 0;
  double vol = 0;
  int size=0;
  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
  double totalprecipvol = 0;
  vector<double> vollist;
  vollist.resize(10);
  while(totalprecipvol < (totalvol*(fractionprecip/100.0)))
  {
    int volgood = 0;
    while(volgood == 0)
	{
		volgood = 1;
	    u = rg.Random();
		diam = rg.RandNorm(avgprecipdiam,sdprecipdiam);
		if((fabs(diam-avgprecipdiam)/sdprecipdiam) > 2.0) volgood = 0;
		diam = exp(diam);
		vol = (4.0/3.0)*(m_pi)*((diam/2.0)*(diam/2.0)*(diam/2.0));		
	}
	vollist[count] = vol;
	count++;
	size = vollist.size();
	if(size == count) vollist.resize(size+10);
    totalprecipvol = totalprecipvol + vol;
  }
  precipitates = new Grain[count];
  for(int a=1;a<count+1;a++)
  {
    vol = vollist[a];
	diam = (3.0/4.0)*(1.0/m_pi)*vol;
	diam = pow(diam,0.3333333);
	diam = 2.0*diam;
    int diameter = int(diam);
    if(diameter >= maxprecipdiameter) diameter = maxprecipdiameter;
    if(diameter <= minprecipdiameter) diameter = minprecipdiameter;
    good = 0;
    while(good == 0)
    {
	  a1 = precipbovera[diameter][0];
	  b1 = precipbovera[diameter][1];
	  r2 = rg.RandBeta(a1,b1);
	  a2 = precipcovera[diameter][0];
	  b2 = precipcovera[diameter][1];
	  r3 = rg.RandBeta(a2,b2);
      double cob = r3/r2;
	  a3 = precipcoverb[diameter][0];
	  b3 = precipcoverb[diameter][1];
	  double prob = ((gamma((a3+b3))/(gamma(a3)*gamma(b3)))*(pow(cob,(a3-1)))*(pow((1-cob),(b3-1))));
	  double check = rg.Random();
      if(prob > check) good = 1;
      if(cob > 1) good = 0;
    }
	double random = rg.Random();
	int bin=0;
	for(int i=0;i<(18*18*18);i++)
	{
		double density = precipaxisodf[i].density;
		if(random > density) bin = i;
		if(random < density) {break;}
	}
	double phi1 = bin%18;
	double PHI = (bin/18)%18;
	double phi2 = bin/(18*18);
	random = rg.Random();
	phi1 = ((phi1*5)+(random*5))*(m_pi/180.0);
	random = rg.Random();
	PHI = ((PHI*5)+(random*5))*(m_pi/180.0);
	random = rg.Random();
	phi2 = ((phi2*5)+(random*5))*(m_pi/180.0);
	double m = precipsvomega3[diameter][0];
	double s = precipsvomega3[diameter][1];
	double omega3 = rg.RandBeta(m,s);
    precipitates[a].volume = vol;
	precipitates[a].radius1 = r1;
	precipitates[a].radius2 = r2;
	precipitates[a].radius3 = r3;
    precipitates[a].axiseuler1 = phi1;
    precipitates[a].axiseuler2 = PHI;
    precipitates[a].axiseuler3 = phi2;
	precipitates[a].omega3 = omega3;
  }
  double sizex = int(pow((totalvol*1),0.33333));
  double sizey = int(pow((totalvol*1),0.33333));
  double sizez = int(pow((totalvol*1),0.33333));
  xpoints = int((sizex/resx)+1);
  ypoints = int((sizey/resy)+1);
  zpoints = int((sizez/resz)+1);
  xpoints = int(xpoints/4)*4;
  ypoints = int(ypoints/4)*4;
  zpoints = int(zpoints/4)*4;
  xpoints1 = xpoints/int(resx1/resx);
  ypoints1 = ypoints/int(resx1/resx);
  zpoints1 = zpoints/int(resx1/resx);
  precipitateorder.resize(count);
  takencheck.resize(count);
  for(int i=1;i<count+1;i++)
  {
    int maxprecipitate = 0;
    double maxvol = 0;
    for(int j=1;j<count+1;j++)
    {
      double vol = precipitates[j].volume;
      if(vol > maxvol && takencheck[j] != 1)
      {
        maxvol = vol;
        maxprecipitate = j;
      }
    }
    takencheck[maxprecipitate] = 1;
	precipitateorder[i] = maxprecipitate;
  }
  return count;
}
void GrainGeneratorFunc::insert_precipitates(int numprecipitates)
{
  int ncount = 0;
  int count = 0;
  int counter=0;
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
  double Nvalue = 0;
  double Gvalue = 0;
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
  int uniquecursize = 0;
  int totalcursize = 0;
  psizes = new int[numprecipitates];
  srand(static_cast<unsigned int>(time(NULL)));
  vector<long> availablelist;
  vector<long> tempavailablelist;
  vector<long> insidelist;
  availablelist.resize(xpoints*ypoints*zpoints);
  tempavailablelist.resize(xpoints*ypoints*zpoints);
  insidelist.resize(100);
  for (long a = 0; a < (xpoints*ypoints*zpoints); a++)
  {
	if(preciptype == 2)
	{
		if(gridfine[a].surfacevoxel == 1)
		{
			availablelist[counter] = a;
			counter++;
		}
	}
	if(preciptype == 3)
	{
		availablelist[counter] = a;
		counter++;
	}
  }
  availablelist.resize(counter);
  tempavailablelist.resize(counter);
  for (int i=1;i<numprecipitates+1; i++)
  {
    good = 0;
	int curprecip = precipitateorder[i];
    double volcur = precipitates[curprecip].volume;
	double bovera = precipitates[curprecip].radius2;
	double covera = precipitates[curprecip].radius3;
	double omega3 = precipitates[curprecip].omega3;
    double radcur1 = 1;
    if(shapeclass == 3)
    {
	  Gvalue = omega3;
      if(Gvalue >= 0 && Gvalue <= 1)
      {
        radcur1 = (volcur*6.0)/(6-(Gvalue*Gvalue*Gvalue));
      }
      if(Gvalue > 1 && Gvalue <= 2)
      {
        radcur1 = (volcur*6.0)/(3+(9*Gvalue)-(9*Gvalue*Gvalue)+(2*Gvalue*Gvalue*Gvalue));
      }
    }
    if(shapeclass == 2)
    {
	  Nvalue = omega3;
      double beta1 = (gamma((1.0/Nvalue))*gamma((1.0/Nvalue)))/gamma((2.0/Nvalue));
      double beta2 = (gamma((2.0/Nvalue))*gamma((1.0/Nvalue)))/gamma((3.0/Nvalue));
      radcur1 = (volcur*(3.0/2.0)*(1.0/bovera)*(1.0/covera)*((Nvalue*Nvalue)/4.0)*(1.0/beta1)*(1.0/beta2));
    }
    if(shapeclass == 1)
    {
      radcur1 = (volcur*(3.0/4.0)*(1.0/m_pi)*(1.0/bovera)*(1.0/covera));
    }
    radcur1 = pow(radcur1,0.333333333333);
    if(shapeclass == 3) radcur1 = radcur1/2.0;
    double radcur2 = (radcur1*bovera);
    double radcur3 = (radcur1*covera);
	double ga[3][3];
	double phi1 = precipitates[curprecip].axiseuler1;
	double PHI = precipitates[curprecip].axiseuler2;
	double phi2 = precipitates[curprecip].axiseuler3;
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
	tempavailablelist = availablelist;
    while(good == 0)
    {
        double random = rg.Random();
		pointsleft = tempavailablelist.size();
        int remainder = int(random*pointsleft);
	    if(remainder == pointsleft) remainder = pointsleft-1;
	    j = tempavailablelist[remainder];
	    tempavailablelist.erase(tempavailablelist.begin()+remainder);
		for(int c=0;c<i;c++)
		{
		  gremovals[c]=0;
		}
        good = 1;
        column = j%xpoints;
        row = (j/xpoints)%ypoints;
        plane = j/(xpoints*ypoints);
        xc = (column*resx)+(resx/2);
        yc = (row*resy)+(resy/2);
        zc = (plane*resz)+(resz/2);
        insidecount = 0;
        badcount = 0;
        xmin = 0;
        xmax = xpoints-1;
        ymin = 0;
        ymax = ypoints-1;
        zmin = 0;
        zmax = zpoints-1;
        if(column-((radcur1/resx)+1) > 0)
        {
          xmin = int(column-((radcur1/resx)+1));
        }
        if(column+((radcur1/resx)+1) < xpoints-1)
        {
          xmax = int(column+((radcur1/resx)+1));
        }
        if(row-((radcur1/resy)+1) > 0)
        {
          ymin = int(row-((radcur1/resy)+1));
        }
        if(row+((radcur1/resy)+1) < ypoints-1)
        {
          ymax = int(row+((radcur1/resy)+1));
        }
        if(plane-((radcur1/resz)+1) > 0)
        {
          zmin = int(plane-((radcur1/resz)+1));
        }
        if(plane+((radcur1/resz)+1) < zpoints-1)
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
                  if(((-axis1comp)+(-axis2comp)+(axis3comp)-((-0.5*Gvalue)+(-0.5*Gvalue)+2)) > 0) inside = -1;
                  if(((axis1comp)+(-axis2comp)+(axis3comp)-((2-(0.5*Gvalue))+(-0.5*Gvalue)+2)) > 0) inside = -1;
                  if(((axis1comp)+(axis2comp)+(axis3comp)-((2-(0.5*Gvalue))+(2-(0.5*Gvalue))+2)) > 0) inside = -1;
                  if(((-axis1comp)+(axis2comp)+(axis3comp)-((-0.5*Gvalue)+(2-(0.5*Gvalue))+2)) > 0) inside = -1;
                  if(((-axis1comp)+(-axis2comp)+(-axis3comp)-((-0.5*Gvalue)+(-0.5*Gvalue))) > 0) inside = -1;
                  if(((axis1comp)+(-axis2comp)+(-axis3comp)-((2-(0.5*Gvalue))+(-0.5*Gvalue))) > 0) inside = -1;
                  if(((axis1comp)+(axis2comp)+(-axis3comp)-((2-(0.5*Gvalue))+(2-(0.5*Gvalue)))) > 0) inside = -1;
                  if(((-axis1comp)+(axis2comp)+(-axis3comp)-((-0.5*Gvalue)+(2-(0.5*Gvalue)))) > 0) inside = -1;
                }
              }
              if(shapeclass == 2)
              {
			    axis1comp = fabs(axis1comp);
			    axis2comp = fabs(axis2comp);
			    axis3comp = fabs(axis3comp);
                axis1comp = pow(axis1comp,Nvalue);
                axis2comp = pow(axis2comp,Nvalue);
                axis3comp = pow(axis3comp,Nvalue);
                inside = 1-axis1comp-axis2comp-axis3comp;
              }
              if(shapeclass == 1)
              {
			    axis1comp = fabs(axis1comp);
			    axis2comp = fabs(axis2comp);
			    axis3comp = fabs(axis3comp);
                axis1comp = pow(axis1comp,2);
                axis2comp = pow(axis2comp,2);
                axis3comp = pow(axis3comp,2);
                inside = 1-axis1comp-axis2comp-axis3comp;
              }
              if(inside >= 0)
              {
                int currentpoint = (xpoints*ypoints*plane)+(xpoints*row)+column;
				insidelist[insidecount] = currentpoint;
				if(insidecount >= (0.9*insidelist.size())) insidelist.resize(insidecount+100);
                insidecount++;
				if(gridfine[currentpoint].precipitatename != -1) badcount++;
              }
            }
          }
        }
        double acceptable = 100*(badcount/insidecount);
        if(acceptable > overlapallowed) good = 0;
        if(good == 0)
        {
			insidelist.clear();
		}
	}
    tempsurf = 0;
    for(int a=0;a<insidecount;a++)
    {
      int point = insidelist[insidecount];
      int columncheck = point%xpoints;
      int rowcheck = (point/xpoints)%ypoints;
      int planecheck = point/(xpoints*ypoints);
      if(columncheck == 0 || rowcheck == 0 || planecheck == 0 || columncheck == (xpoints-1) || rowcheck == (ypoints-1) || planecheck == (zpoints-1)) tempsurf = 1;
	  if(gridfine[point].precipitatename == -1)
      {
          uniquecursize++;
          totalcursize++;
		  gridfine[point].precipitatename = i;
      }
    }
	int size = availablelist.size();
	for(int p=0;p<size;p++)
	{
		int point = availablelist[p];
		if(gridfine[point].precipitatename != -1)
		{
			availablelist.erase(availablelist.begin()+p);
			p = p-1;
			size = availablelist.size();
		}
	}
    uniquecursize = 0;
    precipitates[curprecip].initsize = totalcursize;
    precipitates[curprecip].currentsize = totalcursize;
    precipitates[curprecip].centroidx = xc;
    precipitates[curprecip].centroidy = yc;
    precipitates[curprecip].centroidz = zc;
    precipitates[curprecip].radius1 = radcur1;
    precipitates[curprecip].radius2 = radcur2;
    precipitates[curprecip].radius3 = radcur3;
    precipitates[curprecip].temponsurf = tempsurf;
    precipitates[curprecip].tempneighnum = ncount;
    totalcursize = 0;
    count++;
  }
  for(int u=1;u<numprecipitates+1;u++)
  {
    psizes[u] = 0;
  }
  for(int t=0;t<(xpoints*ypoints*zpoints);t++)
  {
	if(gridfine[t].precipitatename != -1)
    {
	  int gname = gridfine[t].precipitatename;
      psizes[gname]++;
	  gridfine[t].grainname = -1;
    }
  }
  for(int v=1;v<numprecipitates+1;v++)
  {
    int cursize = psizes[v];
    precipitates[v].currentsize = cursize;
  }
}
void GrainGeneratorFunc::read_structure(string inname8)
{
	const unsigned int size ( 1024 );
	char buf [ size ];
	std::ifstream in ( inname8.c_str() );
	std::string word;
	bool headerdone = false;
	while(headerdone == false)
	{
		in.getline( buf, size );
		std::string line = buf;
		in >> word;
		if (DIMS == word )
		{
		    in >> xpoints >> ypoints >> zpoints;
			totalpoints = xpoints * ypoints * zpoints;
			totalpoints1 = (xpoints/2) * (ypoints/2) * (zpoints/2);
			gridfine = new Voxel[totalpoints];
		}
		if(LOOKUP == word)
		{
			headerdone = true;
			in >> word;
		}
	}

	int gnum=0;
	for(int i=0;i<(xpoints*ypoints*zpoints);i++)
	{
		in >> gnum;
		gridfine[i].grainname = gnum;
	}
}
void  GrainGeneratorFunc::find_neighbors()
{
  double x = 0;
  double y = 0;
  double z = 0;
  int grain;
  int grain1;
  int grain2;
  int grain3;
  int grain4;
  int grain5;
  int grain6;
  int nnum;
  totalsurfacearea=0;
  int nListSize = 1000;
  std::vector<int> nlist(nListSize, -1);
  std::vector<double> nsalist(nListSize, -1);
  for(int i=1;i<numgrains+1;i++)
  {
    int numneighs = int(nlist.size());
	grains[i].numneighbors = 0;
    grains[i].neighborlist = new std::vector<int>(numneighs);
    grains[i].neighborlist->swap(nlist);
    grains[i].neighborsurfarealist = new std::vector<double>(numneighs);
    grains[i].neighborsurfarealist->swap(nsalist);
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    int onsurf = 0;
    int grain = gridfine[j].grainname;
	if(grain != 0)
	{
		x = j%xpoints;
		y = (j/xpoints)%ypoints;
		z = j/(xpoints*ypoints);
		if(x > 0)
		{
		  grain1 = gridfine[j-1].grainname;
		  if(grain1 != grain && grain1!= 0)
		  {
			  onsurf++;
			  nnum = grains[grain].numneighbors;
			  vector<int>* nlist = grains[grain].neighborlist;
			  if (nnum >= (0.9*nlist->size()))
			  {
				 nlist->resize(nnum + nListSize);
			  }
			  nlist->at(nnum) = grain1;
			  nnum++;
			  grains[grain].numneighbors = nnum;
		  }
		}
		if(x < xpoints-1)
		{
		  grain2 = gridfine[j+1].grainname;
		  if(grain2 != grain && grain2!= 0)
		  {
			  onsurf++;
			  nnum = grains[grain].numneighbors;
			  vector<int>* nlist = grains[grain].neighborlist;
			  if (nnum >= (0.9*nlist->size()))
			  {
				 nlist->resize(nnum + nListSize);
			  }
			  nlist->at(nnum) = grain2;
			  nnum++;
			  grains[grain].numneighbors = nnum;
		  }
		}
		if(y > 0)
		{
		  grain3 = gridfine[j-(xpoints)].grainname;
		  if(grain3 != grain && grain3!= 0)
		  {
			  onsurf++;
			  nnum = grains[grain].numneighbors;
			  vector<int>* nlist = grains[grain].neighborlist;
			  if (nnum >= (0.9*nlist->size()))
			  {
				 nlist->resize(nnum + nListSize);
			  }
			  nlist->at(nnum) = grain3;
			  nnum++;
			  grains[grain].numneighbors = nnum;
		  }
		}
		if(y < ypoints-1)
		{
		  grain4 = gridfine[j+(xpoints)].grainname;
		  if(grain4 != grain && grain4!= 0)
		  {
			  onsurf++;
			  nnum = grains[grain].numneighbors;
			  vector<int>* nlist = grains[grain].neighborlist;
			  if (nnum >= (0.9*nlist->size()))
			  {
				 nlist->resize(nnum + nListSize);
			  }
			  nlist->at(nnum) = grain4;
			  nnum++;
			  grains[grain].numneighbors = nnum;
		  }
		}
		if(z > 0)
		{
		  grain5 = gridfine[j-(xpoints*ypoints)].grainname;
		  if(grain5 != grain && grain5!= 0)
		  {
			  onsurf++;
			  nnum = grains[grain].numneighbors;
			  vector<int>* nlist = grains[grain].neighborlist;
			  if (nnum >= (0.9*nlist->size()))
			  {
				 nlist->resize(nnum + nListSize);
			  }
			  nlist->at(nnum) = grain5;
			  nnum++;
			  grains[grain].numneighbors = nnum;
		  }
		}
		if(z < zpoints-1)
		{
		  grain6 = gridfine[j+(xpoints*ypoints)].grainname;
		  if(grain6 != grain && grain6!= 0)
		  {
			  onsurf++;
			  nnum = grains[grain].numneighbors;
			  vector<int>* nlist = grains[grain].neighborlist;
			  if (nnum >= (0.9*nlist->size()))
			  {
				 nlist->resize(nnum + nListSize);
			  }
			  nlist->at(nnum) = grain6;
			  nnum++;
			  grains[grain].numneighbors = nnum;
		  }
		}
		gridfine[j].surfacevoxel = onsurf;
	}
  }
  vector<int> nlistcopy;
  for(int i=1;i<numgrains+1;i++)
  {
	vector<int>* nlist = grains[i].neighborlist;
	vector<double>* nsalist = grains[i].neighborsurfarealist;
	vector<int>::iterator newend;
	sort(nlist->begin(),nlist->end());
	nlistcopy.resize(nlist->size());
	copy(nlist->begin(),nlist->end(),nlistcopy.begin());
    newend = unique(nlist->begin(),nlist->end());
    nlist->erase(newend,nlist->end());
	nlist->erase(std::remove(nlist->begin(),nlist->end(),-1),nlist->end());
	nlist->erase(std::remove(nlist->begin(),nlist->end(),0),nlist->end());
	int numneighs = int(nlist->size());
	for(int j=0;j<numneighs;j++)
	{
		int neigh = nlist->at(j);
		int number = std::count(nlistcopy.begin(),nlistcopy.end(),neigh);
		double area = number*resx*resx;
		nsalist->at(j) = area;
		totalsurfacearea = totalsurfacearea + area;
	}
    grains[i].numneighbors = numneighs;
    grains[i].neighborlist = new std::vector<int>(numneighs);
    grains[i].neighborlist = nlist;
    grains[i].neighborsurfarealist = new std::vector<double>(numneighs);
    grains[i].neighborsurfarealist = nsalist;
  }
}
void  GrainGeneratorFunc::find_surfacevoxels()
{
  double x = 0;
  double y = 0;
  double z = 0;
  int grain1;
  int grain2;
  int grain3;
  int grain4;
  int grain5;
  int grain6;
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    int onsurf = 0;
    int grain = gridfine[j].grainname;
	x = j%xpoints;
	y = (j/xpoints)%ypoints;
	z = j/(xpoints*ypoints);
	if(x > 0)
	{
	  grain1 = gridfine[j-1].grainname;
	  if(grain1 != grain && grain1!= 0)
	  {
		onsurf = 1;
	  }
	}
	if(x < xpoints-1)
	{
	  grain2 = gridfine[j+1].grainname;
	  if(grain2 != grain && grain2!= 0)
	  {
		onsurf = 1;
	  }
	}
	if(y > 0)
	{
	  grain3 = gridfine[j-(xpoints)].grainname;
	  if(grain3 != grain && grain3!= 0)
	  {
		onsurf = 1;
	  }
	}
	if(y < ypoints-1)
	{
	  grain4 = gridfine[j+(xpoints)].grainname;
	  if(grain4 != grain && grain4!= 0)
	  {
		onsurf = 1;
	  }
	}
	if(z > 0)
	{
	  grain5 = gridfine[j-(xpoints*ypoints)].grainname;
	  if(grain5 != grain && grain5!= 0)
	  {
		onsurf = 1;
	  }
	}
	if(z < zpoints-1)
	{
	  grain6 = gridfine[j+(xpoints*ypoints)].grainname;
	  if(grain6 != grain && grain6!= 0)
	  {
		onsurf = 1;
	  }
	}
    gridfine[j].surfacevoxel = onsurf;
  }
}

void  GrainGeneratorFunc::loadMisoData(string inname10)
{
    ifstream inputFile;
    inputFile.open(inname10.c_str());
	int count = 0;
	double density = 0;
    for (int k = 0; k < 36; k++)
    {
		inputFile >> density;
		actualmdf[count].density = density;
        count++;
    }
    inputFile.close();
}

void  GrainGeneratorFunc::loadMicroData(string inname11)
{
    ifstream inputFile;
    inputFile.open(inname11.c_str());
  int count = 0;
  double density = 0;
    for (int k = 0; k < 10; k++)
    {
		inputFile >> density;
		actualmicrotex[count].density = density;
        count++;
    }
    inputFile.close();
}

void GrainGeneratorFunc::matchCrystallography(double quat_symmcubic[24][5],double quat_symmhex[12][5])
{
	double deltaerror = 1.0;
	int iterations = 0;
	double currentodferror = 0;
	double currentmdferror = 0;
	double n1,n2,n3;
	double totaldensity = 0;
	for(int i=0;i<(18*18*18);i++)
	{
		currentodferror = currentodferror + ((actualodf[i].density-simodf[i].density)*(actualodf[i].density-simodf[i].density));
	}
	for(int i=0;i<(10);i++)
	{
		currentmdferror = currentmdferror + ((actualmdf[i].density-simmdf[i].density)*(actualmdf[i].density-simmdf[i].density));
	}
	while(iterations < 10000)
	{
		iterations++;
		double random = rg.Random();
		if(random < 0.5)
		{
			int selectedgrain = int(rg.Random()*numgrains)+1;
			if(selectedgrain == numgrains+1) selectedgrain = numgrains;
			double curea1 = grains[selectedgrain].euler1;
			double curea2 = grains[selectedgrain].euler2;
			double curea3 = grains[selectedgrain].euler3;
			int cureuler1bin = int(curea1/(5.0*m_pi/180.0));
			int cureuler2bin = int(curea2/(5.0*m_pi/180.0));
			int cureuler3bin = int(curea3/(5.0*m_pi/180.0));
			int curodfbin = (cureuler3bin*18*18)+(cureuler2bin*18)+cureuler1bin;
			double random = rg.Random();
			int choose = 0;
			totaldensity = 0;
			for(int i=0;i<(18*18*18);i++)
			{
				double density = actualodf[i].density;
				totaldensity = totaldensity + density;
				if(random >= totaldensity) choose = i;
			}
			int phi1 = choose%18;
			int PHI = (choose/18)%18;
			int phi2 = choose/(18*18);
			double random1 = rg.Random();
			double random2 = rg.Random();
			double random3 = rg.Random();
			double chooseea1 = (5*phi1)+(5*random1);
			double chooseea2 = (5*PHI)+(5*random2);
			double chooseea3 = (5*phi2)+(5*random3);
			chooseea1 = chooseea1*(m_pi/180.0);
			chooseea2 = chooseea2*(m_pi/180.0);
			chooseea3 = chooseea3*(m_pi/180.0);
			double odfchange = ((actualodf[choose].density - simodf[choose].density)*(actualodf[choose].density - simodf[choose].density)) - ((actualodf[choose].density - (simodf[choose].density+(grains[selectedgrain].currentsize*resx*resy*resz/totalvol)))*(actualodf[choose].density - (simodf[choose].density+(grains[selectedgrain].currentsize*resx*resy*resz/totalvol))));
			odfchange = odfchange + (((actualodf[curodfbin].density - simodf[curodfbin].density)*(actualodf[curodfbin].density - simodf[curodfbin].density)) - ((actualodf[curodfbin].density - (simodf[curodfbin].density-(grains[selectedgrain].currentsize*resx*resy*resz/totalvol)))*(actualodf[curodfbin].density - (simodf[curodfbin].density-(grains[selectedgrain].currentsize*resx*resy*resz/totalvol)))));
			vector<int>* nlist = grains[selectedgrain].neighborlist;
			vector<double>* misolist = grains[selectedgrain].misorientationlist;
			vector<double>* neighborsurfarealist = grains[selectedgrain].neighborsurfarealist;
			double mdfchange = 0;
			for(int j=0;j<nlist->size();j++)
			{
				int neighbor = nlist->at(j);
				double curmiso = misolist->at(j);
				double neighsurfarea = neighborsurfarealist->at(j);
				int curmisobin = int(curmiso/(5.0*m_pi/180.0));
				double neighea1 = grains[neighbor].euler1;
				double neighea2 = grains[neighbor].euler2;
				double neighea3 = grains[neighbor].euler3;
				double newmiso = 0;
				if(crystruct == 1) newmiso = getmisoquathexagonal(quat_symmhex,chooseea1,chooseea2,chooseea3,neighea1,neighea2,neighea3,n1,n2,n3); 
				if(crystruct == 2) newmiso = getmisoquatcubic(chooseea1,chooseea2,chooseea3,neighea1,neighea2,neighea3,n1,n2,n3);
				int newmisobin = int(newmiso/5.0);
				mdfchange = mdfchange + (((actualmdf[curmisobin].density-simmdf[curmisobin].density)*(actualmdf[curmisobin].density-simmdf[curmisobin].density)) - ((actualmdf[curmisobin].density-(simmdf[curmisobin].density-(neighsurfarea/totalsurfacearea)))*(actualmdf[curmisobin].density-(simmdf[curmisobin].density-(neighsurfarea/totalsurfacearea)))));
				mdfchange = mdfchange + (((actualmdf[newmisobin].density-simmdf[newmisobin].density)*(actualmdf[newmisobin].density-simmdf[newmisobin].density)) - ((actualmdf[newmisobin].density-(simmdf[newmisobin].density+(neighsurfarea/totalsurfacearea)))*(actualmdf[newmisobin].density-(simmdf[newmisobin].density+(neighsurfarea/totalsurfacearea)))));
			}
			deltaerror = (odfchange/currentodferror) + (mdfchange/currentmdferror);
			if(deltaerror > 0)
			{
				simodf[choose].density = simodf[choose].density + (grains[selectedgrain].volume/totalvol);
				simodf[curodfbin].density = simodf[curodfbin].density - (grains[selectedgrain].volume/totalvol);
				for(int j=0;j<nlist->size();j++)
				{
					int neighbor = nlist->at(j);
					double curmiso = misolist->at(j);
					double neighsurfarea = neighborsurfarealist->at(j);
					int curmisobin = int(curmiso/(5.0*m_pi/180.0));
					double neighea1 = grains[neighbor].euler1;
					double neighea2 = grains[neighbor].euler2;
					double neighea3 = grains[neighbor].euler3;
					double newmiso = 0;
					if(crystruct == 1) newmiso = getmisoquathexagonal(quat_symmhex,chooseea1,chooseea2,chooseea3,neighea1,neighea2,neighea3,n1,n2,n3); 
					if(crystruct == 2) newmiso = getmisoquatcubic(chooseea1,chooseea2,chooseea3,neighea1,neighea2,neighea3,n1,n2,n3);
					int newmisobin = int(newmiso/(5.0*m_pi/180.0));
					misolist->at(j) = newmiso;
					simmdf[curmisobin].density = simmdf[curmisobin].density - (neighsurfarea/totalsurfacearea);
					simmdf[newmisobin].density = simmdf[newmisobin].density + (neighsurfarea/totalsurfacearea);
				}
				currentodferror = currentodferror - odfchange;
				currentmdferror = currentmdferror - mdfchange;
			}
		}
	}
}
void  GrainGeneratorFunc::measure_misorientations (double quat_symmcubic[24][5],double quat_symmhex[12][5])
{
  vector<double> misolist;
  double w;
  double n1;
  double n2;
  double n3;
  for (int i = 1; i < numgrains+1; i++)
  {
    vector<int>* nlist = grains[i].neighborlist;
	misolist.resize(nlist->size());
    double g1ea1 = grains[i].euler1;
    double g1ea2 = grains[i].euler2;
    double g1ea3 = grains[i].euler3;
    int size = 0;
    if (NULL != nlist) { size = nlist->size(); }
    for(int j=0;j<size;j++)
    {
      int nname = nlist->at(j);
      double g2ea1 = grains[nname].euler1;
      double g2ea2 = grains[nname].euler2;
      double g2ea3 = grains[nname].euler3;
      if(crystruct == 1) w = getmisoquathexagonal(quat_symmhex,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
      if(crystruct == 2) w = getmisoquatcubic(g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
      misolist[j] = w;
    }
	grains[i].misorientationlist = new std::vector<double>(misolist.size() );
	grains[i].misorientationlist->swap(misolist);
    misolist.clear();
  }
}

double GrainGeneratorFunc::getmisoquatcubic(double g1ea1,double g1ea2,double g1ea3,double g2ea1,double g2ea2,double g2ea3,double &n1,double &n2,double &n3)
{
  double wmin=9999999; //,na,nb,nc;
  double q1[4];
  double q2[4];
  double qc[4];
  double qco[4];
  q1[0]=sin((g1ea2/2.0))*cos(((g1ea1-g1ea3)/2.0));
  q1[1]=sin((g1ea2/2.0))*sin(((g1ea1-g1ea3)/2.0));
  q1[2]=cos((g1ea2/2.0))*sin(((g1ea1+g1ea3)/2.0));
  q1[3]=cos((g1ea2/2.0))*cos(((g1ea1+g1ea3)/2.0));
  q2[0]=sin((g2ea2/2.0))*cos(((g2ea1-g2ea3)/2.0));
  q2[1]=sin((g2ea2/2.0))*sin(((g2ea1-g2ea3)/2.0));
  q2[2]=cos((g2ea2/2.0))*sin(((g2ea1+g2ea3)/2.0));
  q2[3]=cos((g2ea2/2.0))*cos(((g2ea1+g2ea3)/2.0));
  q2[0]=q2[0];
  q2[1]=-q2[1];
  q2[2]=-q2[2];
  q2[3]=-q2[3];
  qc[0]=q1[0]*q2[0]-q1[1]*q2[1]-q1[2]*q2[2]-q1[3]*q2[3];
  qc[1]=q1[0]*q2[1]+q1[1]*q2[0]+q1[2]*q2[3]-q1[3]*q2[2];
  qc[2]=q1[0]*q2[2]-q1[1]*q2[3]+q1[2]*q2[0]+q1[3]*q2[1];
  qc[3]=q1[0]*q2[3]+q1[1]*q2[2]-q1[2]*q2[1]+q1[3]*q2[0];
  qc[0]=fabs(qc[0]);
  qc[1]=fabs(qc[1]);
  qc[2]=fabs(qc[2]);
  qc[3]=fabs(qc[3]);
  for(int i=0;i<4;i++)
  {
    qco[i]=100000;
    for(int j=0;j<4;j++)
    {
      if((qc[j] < qco[i] && i == 0) || (qc[j] < qco[i] && qc[j] > qco[i-1]))
      {
        qco[i] = qc[j];
      }
    }
  }
  wmin = qco[3];
  if(((qco[2]+qco[3])/(pow(2,0.5))) > wmin) wmin = ((qco[2]+qco[3])/(pow(2,0.5)));
  if(((qco[0]+qco[1]+qco[2]+qco[3])/2) > wmin) wmin = ((qco[0]+qco[1]+qco[2]+qco[3])/2);
  if(wmin < -1) wmin = -1;
  if(wmin > 1) wmin = 1;
  wmin = acos(wmin);
  wmin = (360.0/m_pi)*wmin;
  return wmin;
}

double GrainGeneratorFunc::getmisoquathexagonal(double quat_symmhex[12][5],double g1ea1,double g1ea2,double g1ea3,double g2ea1,double g2ea2,double g2ea3,double &n1,double &n2,double &n3)
{
  double wmin=9999999; //,na,nb,nc;
  double q1[4];
  double q2[4];
  double qr[4];
  double qc[4];
  double qco[4];
  q1[0]=sin((g1ea2/2.0))*cos(((g1ea1-g1ea3)/2.0));
  q1[1]=sin((g1ea2/2.0))*sin(((g1ea1-g1ea3)/2.0));
  q1[2]=cos((g1ea2/2.0))*sin(((g1ea1+g1ea3)/2.0));
  q1[3]=cos((g1ea2/2.0))*cos(((g1ea1+g1ea3)/2.0));
  q2[0]=sin((g2ea2/2.0))*cos(((g2ea1-g2ea3)/2.0));
  q2[1]=sin((g2ea2/2.0))*sin(((g2ea1-g2ea3)/2.0));
  q2[2]=cos((g2ea2/2.0))*sin(((g2ea1+g2ea3)/2.0));
  q2[3]=cos((g2ea2/2.0))*cos(((g2ea1+g2ea3)/2.0));
  q2[0]=q2[0];
  q2[1]=-q2[1];
  q2[2]=-q2[2];
  q2[3]=-q2[3];
  qr[0]=q1[0]*q2[3]-q1[3]*q2[0]+q1[1]*q2[2]-q1[2]*q2[1];
  qr[1]=q1[1]*q2[3]-q1[3]*q2[1]+q1[2]*q2[0]-q1[0]*q2[2];
  qr[2]=q1[2]*q2[3]-q1[3]*q2[2]+q1[0]*q2[1]-q1[1]*q2[0];
  qr[3]=q1[3]*q2[3]+q1[0]*q2[0]+q1[1]*q2[1]+q1[2]*q2[2];
  qr[0]=q1[0]*q2[3]-q1[3]*q2[0]+q1[1]*q2[2]-q1[2]*q2[1];
  qr[1]=q1[1]*q2[3]-q1[3]*q2[1]+q1[2]*q2[0]-q1[0]*q2[2];
  qr[2]=q1[2]*q2[3]-q1[3]*q2[2]+q1[0]*q2[1]-q1[1]*q2[0];
  qr[3]=q1[3]*q2[3]+q1[0]*q2[0]+q1[1]*q2[1]+q1[2]*q2[2];
  qc[0]=fabs(qc[0]);
  qc[1]=fabs(qc[1]);
  qc[2]=fabs(qc[2]);
  qc[3]=fabs(qc[3]);
  for(int i=0;i<4;i++)
  {
    qco[i]=100000;
    for(int j=0;j<4;j++)
    {
      if((qc[j] < qco[i] && i == 0) || (qc[j] < qco[i] && qc[j] > qco[i-1]))
      {
        qco[i] = qc[j];
      }
    }
  }
  wmin = qco[3];
  if(((qco[2]+qco[3])/(pow(2,0.5))) > wmin) wmin = ((qco[2]+qco[3])/(pow(2,0.5)));
  if(((qco[0]+qco[1]+qco[2]+qco[3])/2) > wmin) wmin = ((qco[0]+qco[1]+qco[2]+qco[3])/2);
  if(wmin < -1) wmin = -1;
  if(wmin > 1) wmin = 1;
  wmin = acos(wmin);
  wmin = (360.0/m_pi)*wmin;
  return wmin;
}

void  GrainGeneratorFunc::writeCube(string outname1, int numgrains, string outname2)
{
  ofstream outFile;
  outFile.open(outname1.c_str());
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
  for (int i = 1; i < numgrains+1; i++)
  {
	gsizes[i]=0;
  }
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int name = gridfine[i].grainname;
	gsizes[name]++;
	if(i%20 == 0 && i > 0) outFile << endl;
    outFile << "   ";
	if(name < 10000) outFile << " ";
	if(name < 1000) outFile << " ";
	if(name < 100) outFile << " ";
	if(name < 10) outFile << " ";
	outFile << name;
  }
  outFile << endl;
  outFile << "SCALARS PrecipitateID int  1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
	int name = gridfine[i].precipitatename;
	if(i%20 == 0 && i > 0) outFile << endl;
    outFile << "   ";
	if(name < 10000) outFile << " ";
	if(name < 1000) outFile << " ";
	if(name < 100) outFile << " ";
	if(name < 10) outFile << " ";
	outFile << name;
  }
  outFile << endl;
  outFile << "SCALARS PhaseID int  1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
	int name = gridfine[i].precipitatename;
	if(i%20 == 0 && i > 0) outFile << endl;
    if(name == -1) outFile << "       1";
    if(name != -1) outFile << "       2";
  }
  outFile.close();
  ofstream outFile2;
  outFile2.open(outname2.c_str());
  for(int i=1;i<numgrains+1;i++)
  {
	outFile2 << double(gsizes[i])/64.0 << endl;
  }
}

void GrainGeneratorFunc::write_eulerangles(string writename10)
{
  //std::cout << "GrainGeneratorFunc::write_volume1: '" << writename10 << "'" << std::endl;
  ofstream outFile;
  outFile.open(writename10.c_str());
  outFile << numgrains << endl;
  for (int i = 1; i < numgrains+1; i++)
  {
    double ea1 = grains[i].euler1;
    double ea2 = grains[i].euler2;
    double ea3 = grains[i].euler3;
    outFile << i << " " << ea1 << " " << ea2 << " " << ea3 << endl;
  }
  outFile.close();
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


double GrainGeneratorFunc::find_xcoord(long index)
{
	double x = resx*double(index%xpoints);
	return x;
}
double GrainGeneratorFunc::find_ycoord(long index)
{
	double y = resy*double((index/xpoints)%ypoints);
	return y;
}
double GrainGeneratorFunc::find_zcoord(long index)
{
	double z = resz*double(index/(xpoints*ypoints));
	return z;
}