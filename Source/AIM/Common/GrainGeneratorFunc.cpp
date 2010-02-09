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


const static double m_pi = 3.1415926535897;
const static double m_OnePointThree = 1.33333333333;

#include <sstream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

using namespace std;

GrainGeneratorFunc::GrainGeneratorFunc() :
  grains(NULL),
  packedgrain(NULL),
  gsizes(NULL),
  gremovals(NULL),
  eulerodf(NULL),
  axisodf(NULL),
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

  delete [] grains;
  delete [] packedgrain;
  delete [] gsizes;
  delete [] gremovals;
  delete [] eulerodf;
  delete [] axisodf;
  delete [] gridfine;
  delete [] gridcourse;
  delete [] actualmisobin;
  delete [] simmisobin;
  delete [] actualmicrobin;
  delete [] simmicrobin;

}

void GrainGeneratorFunc::initialize(int32 m_NumGrains, int32 m_ShapeClass,
              double m_XResolution, double m_YResolution, double m_ZResolution, int32 m_OverlapAllowed,
              int32 m_OverlapAssignment, int32 m_CrystalStructure)
{

  resx = m_XResolution;
  resy = m_YResolution;
  resz = m_ZResolution;

  resx1 = 2*m_XResolution;
  resy1 = 2*m_YResolution;
  resz1 = 2*m_ZResolution;

  numgrains = m_NumGrains;
  shapeclass = m_ShapeClass;
  overlapassignment = m_OverlapAssignment;
  overlapallowed = m_OverlapAllowed;
  crystruct = m_CrystalStructure;

  grains = new Grain[numgrains];
  packedgrain = new Grain[numgrains];
  gsizes = new int[numgrains];
  gremovals = new int[numgrains];
  eulerodf = new Orient[18*18*18];
  axisodf = new Orient[18*18*18];
  actualmisobin = new Bin[10];
  simmisobin = new Bin[10];
  actualmicrobin = new Bin[10];
  simmicrobin = new Bin[10];
}

void GrainGeneratorFunc::initialize2(int32 xpoints, int32 ypoints, int32 zpoints)
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
	inputFile >> numdiameters >> maxdiameter >> mindiameter;
	inputFile >> avgdiam >> sddiam;
	bovera.resize(maxdiameter+1);
	covera.resize(maxdiameter+1);
	coverb.resize(maxdiameter+1);
	svn.resize(maxdiameter+1);
	svs.resize(maxdiameter+1);
	svshape.resize(maxdiameter+1);
	for(int iter=0;iter<mindiameter;iter++)
	{
		bovera[iter].resize(2);
		covera[iter].resize(2);
		coverb[iter].resize(2);
		svn[iter].resize(2);
		svs[iter].resize(2);
		svshape[iter].resize(2);
	}
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> diam >> param1 >> param2;
		bovera[diam].resize(2);
		bovera[diam][0]=param1;
		bovera[diam][1]=param2;
	}	
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> diam >> param1 >> param2;
		covera[diam].resize(2);
		covera[diam][0]=param1;
		covera[diam][1]=param2;
	}
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> diam >> param1 >> param2;
		coverb[diam].resize(2);
		coverb[diam][0]=param1;
		coverb[diam][1]=param2;
	}
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> diam >> param1 >> param2;
		svn[diam].resize(2);
		svn[diam][0]=param1;
		svn[diam][1]=param2;
	}
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> diam >> param1 >> param2;
		svs[diam].resize(2);
		svs[diam][0]=param1;
		svs[diam][1]=param2;
	}
	for(int temp7 = 0; temp7 < numdiameters; temp7++)
	{
		inputFile >> diam >> param1 >> param2;
		svshape[diam].resize(2);
		svshape[diam][0]=param1;
		svshape[diam][1]=param2;
	}
    inputFile.close();
}


void  GrainGeneratorFunc::loadorientData(string inname6)
{
  ifstream inputFile;
  inputFile.open(inname6.c_str());
  int count = 0;
  double density;
  inputFile >> numorients;
  for (long k = 0; k < (18*18*18); k++)
  {
    inputFile >> density;
	axisodf[k].density = density;
    count++;
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
	  eulerodf[i].density = density;
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
  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
  totalvol = 0;
  for(int l = 0; l < numgrains; l++)
  {
    u = rg.Random();
	double diam = rg.RandNorm(avgdiam,sddiam);
	diam = exp(diam);
    double vol = (4.0/3.0)*(m_pi)*((diam/2.0)*(diam/2.0)*(diam/2.0));
    int diameter = int(diam);
    if(diameter >= maxdiameter) diameter = maxdiameter;
    if(diameter <= mindiameter) diameter = mindiameter;
    good = 0;
    while(good == 0)
    {
	  a1 = bovera[diameter][0];
	  b1 = bovera[diameter][1];
	  u1 = rg.Random();
	  u2 = rg.Random();
	  r2 = rg.RandBeta(a1,b1);
	  a2 = covera[diameter][0];
	  b2 = covera[diameter][1];
	  u1 = rg.Random();
	  u2 = rg.Random();
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
    double r1x = ga[0][0];
    double r1y = ga[1][0];
    double r1z = ga[2][0];
    double r2x = ga[0][1];
    double r2y = ga[1][1];
    double r2z = ga[2][1];
    double r3x = ga[0][2];
    double r3y = ga[1][2];
    double r3z = ga[2][2];
    u = rg.Random();
	double m = svshape[diameter][0];
	double s = svshape[diameter][1];
	double N = rg.RandNorm(m,s);
	N = exp(N);
    int gnum = l;
    grains[l].grainname = gnum;
    grains[l].volume = vol;
	grains[l].radius1 = r1;
	grains[l].radius2 = r2;
	grains[l].radius3 = r3;
    grains[l].axis1x = r1x;
    grains[l].axis1y = r1y;
    grains[l].axis1z = r1z;
    grains[l].axis2x = r2x;
    grains[l].axis2y = r2y;
    grains[l].axis2z = r2z;
    grains[l].axis3x = r3x;
    grains[l].axis3y = r3y;
    grains[l].axis3z = r3z;
    grains[l].Nvalue = N;
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
  xpoints1 = xpoints/4;
  ypoints1 = ypoints/4;
  zpoints1 = zpoints/4;
  grainorder.resize(numgrains);
  takencheck.resize(numgrains);
  for(int i=0;i<numgrains;i++)
  {
    int maxgrain = 0;
    double maxvol = 0;
    for(int j=0;j<numgrains;j++)
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

void  GrainGeneratorFunc::assign_eulers(int numgrains)
{
  int bin = 0;
  int picked = 0;
  double density = 0;
  double selectedvol = 0;
  double totalselectedvol = 0;
  vector<int> grainlist;
  grainlist.resize(numgrains);
  for(int i=0;i<numgrains;i++)
  {
	grainlist[i] = i;
  }
  double synea1=0,synea2=0,synea3=0;
  for(int j = 0; j < 18; j++)
  {
    for(int k = 0; k < 18; k++)
    {
      for(int l = 0; l < 18; l++)
      {
	    bin = (j*18*18)+(k*18)+(l);
		double density = eulerodf[bin].density*totalvol;
        selectedvol = 0;
        while(selectedvol < density)
        {
          double random = rg.Random();
          double random1 = random;
          random = rg.Random();
          double random2 = random;
          random = rg.Random();
          double random3 = random;
          synea1 = (5*j)+(5*random1);
          synea2 = (5*k)+(5*random2);
          synea3 = (5*l)+(5*random3);
		  synea1 = synea1*(m_pi/180.0);
		  synea2 = synea2*(m_pi/180.0);
		  synea3 = synea3*(m_pi/180.0);
		  random = rg.Random();
		  picked = random*grainlist.size();
		  if(picked == grainlist.size()) picked = grainlist.size()-1;
		  int gnum = grainlist[picked];
          selectedvol = selectedvol + grains[gnum].volume;
          totalselectedvol = totalselectedvol + grains[gnum].volume;
		  grainlist.erase(grainlist.begin()+picked);
          grains[gnum].euler1 = synea1;
          grains[gnum].euler2 = synea2;
          grains[gnum].euler3 = synea3;
		  if(totalselectedvol == totalvol) {break;}
        }
        if(totalselectedvol == totalvol) {break;}
      }
      if(totalselectedvol == totalvol) {break;}
    }
    if(totalselectedvol == totalvol) {break;}
  }
  double lastcur = 1;
  while(totalselectedvol < totalvol)
  {
    double cur = 0;
    for(int m = 0; m < 18; m++)
    {
      for(int n = 0; n < 18; n++)
      {
        for(int p = 0; p < 18; p++)
        {
		  double density = eulerodf[bin].density;
          if(density > cur && density < lastcur)
          {
            cur = density;
            double random = rg.Random();
            double random4 = random;
            random = rg.Random();
            double random5 = random;
            random = rg.Random();
            double random6 = random;
	        synea1 = (5*m)+(5*random4);
		    synea2 = (5*n)+(5*random5);
			synea3 = (5*p)+(5*random6);
			synea1 = synea1*(m_pi/180.0);
			synea2 = synea2*(m_pi/180.0);
			synea3 = synea3*(m_pi/180.0);
          }
        }
      }
    }
    lastcur = cur;
	double random = rg.Random();
	picked = random*grainlist.size();
	if(picked == grainlist.size()) picked = grainlist.size()-1;
	int gnum = grainlist[picked];
	selectedvol = selectedvol + grains[gnum].volume;
	totalselectedvol = totalselectedvol + grains[gnum].volume;
	grainlist.erase(grainlist.begin()+picked);
	grains[gnum].euler1 = synea1;
	grains[gnum].euler2 = synea2;
	grains[gnum].euler3 = synea3;
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
  nsdist.resize(numgrains);
  vector<long> alreadytriedlist;
  srand(static_cast<unsigned int>(time(NULL)));
  long nottakencount = (xpoints*ypoints*zpoints);
  long nottakencountcourse = (xpoints1*ypoints1*zpoints1);
  vector<long> availablelist;
  vector<long> tempavailablelist;
  availablelist.resize(nottakencountcourse);
  tempavailablelist.resize(nottakencountcourse);
  for (long a = 0; a < nottakencountcourse; a++)
  {
    gridcourse[a].available = init;
    gridcourse[a].available90 = init;
	availablelist[a] = a;
  }
  tempavailablelist = availablelist;
  for (long b = 0; b < nottakencount; b++)
  {
    gridfine[b].available = init;
    gridfine[b].available90 = init;
  }
  for (int i = 0; i < numgrains; i++)
  {
	nsdist[i].resize(maxdiameter+1);
    good = 0;
    triedcount = 0;
	int curgrain = grainorder[i];
    int size = int(alreadytriedlist.size());
    for(int k=0;k<size;k++)
    {
      int firstgridpoint = alreadytriedlist[k];
      gridcourse[firstgridpoint].available = init;
    }
    double volcur = grains[curgrain].volume;
	double bovera = grains[curgrain].radius2;
	double covera = grains[curgrain].radius3;
    double Nvalue = grains[curgrain].Nvalue;
    double beta1 = (gamma((1.0/Nvalue))*gamma((1.0/Nvalue)))/gamma((2.0/Nvalue));
    double beta2 = (gamma((2.0/Nvalue))*gamma((1.0/Nvalue)))/gamma((3.0/Nvalue));
    double radcur1 = 1;
    if(shapeclass == 3)
    {
      if(Nvalue >= 0 && Nvalue <= 1)
      {
        radcur1 = (volcur*6.0)/(6-(Nvalue*Nvalue*Nvalue));
      }
      if(Nvalue > 1 && Nvalue <= 2)
      {
        radcur1 = (volcur*6.0)/(3+(9*Nvalue)-(9*Nvalue*Nvalue)+(2*Nvalue*Nvalue*Nvalue));
      }
    }
    if(shapeclass == 2)
    {
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
    double rad1x = grains[curgrain].axis1x;
    double rad1y = grains[curgrain].axis1y;
    double rad1z = grains[curgrain].axis1z;
    double rad2x = grains[curgrain].axis2x;
    double rad2y = grains[curgrain].axis2y;
    double rad2z = grains[curgrain].axis2z;
    double rad3x = grains[curgrain].axis3x;
    double rad3y = grains[curgrain].axis3y;
    double rad3z = grains[curgrain].axis3z;
	tempavailablelist = availablelist;
    while(good == 0)
    {
      double random = rg.Random();
      pointsleft = nottakencountcourse-triedcount;
      int remainder = int(random*pointsleft);
	  if(remainder == pointsleft) remainder = pointsleft-1;
	  j = tempavailablelist[remainder];
/*      int selector = -1;
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
*/	  int alreadypicked = 0;
      int tried = gridcourse[j].available;
      gridcourse[j].available = alreadypicked;
	  tempavailablelist.erase(tempavailablelist.begin()+remainder);
      int noway = gridcourse[j].available90;
      if(noway == 1 && tried == 1)
      {
        for(int c=0;c<i;c++)
        {
          gremovals[c]=0;
        }
        alreadytriedlist.push_back(j);
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
                int resdiff = int(double(resx1)/double(resx)+0.00000000001);
                int columnleft = column%resdiff;
                int rowleft = row%resdiff;
                int planeleft = plane%resdiff;
                if(columnleft == (resdiff/2) && rowleft == (resdiff/2) && planeleft == (resdiff/2))
                {
                  int columnc = column/resdiff;
                  int rowc = row/resdiff;
                  int planec = plane/resdiff;
                  int currentpointcourse = (xpoints1*ypoints1*planec)+(xpoints1*rowc)+columnc;
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
          double xold = packedgrain[n].centroidx;
          double yold = packedgrain[n].centroidy;
          double zold = packedgrain[n].centroidz;
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
            int was = log(double(nnum));
            nnum = nnum + 1;
            int now = log(double(nnum));
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
            double nserror = packedgrain[n].nserror;
            nsdistchange = nsdistchange + (error-nserror);
            nsdist[n][bin] = nsdist[n][bin] - 1;
          }
        }
//        double randomsize = rg.Random();
//        double randomsize1 = randomsize;
        double acceptable = 100*(badcount/insidecount);
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
//        if(acceptable > overlapallowed || uberincrease < 0 || nsdistchange > 0 || toomuch == 1) good = 0;
        if(acceptable > overlapallowed) good = 0;
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
              int point = (column+(row*xpoints1)+(plane*xpoints1*ypoints1));
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
	  double xold = packedgrain[p].centroidx;
	  double yold = packedgrain[p].centroidy;
	  double zold = packedgrain[p].centroidz;
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
        double volume = 1.3333333333*(m_pi)*radcur1*radcur2*radcur3;
        double volbin = volume*0.75/m_pi;
        int vbin = int(2*pow(volbin,0.3333333333));
        if(vbin >= maxdiameter) vbin = maxdiameter;
        if(vbin <= mindiameter) vbin = mindiameter;
        nsdist[p][vbin]++;
        double nvolume = 1.3333333333*(m_pi)*radold1*radold2*radold3;
        double nvolbin = nvolume*0.75/m_pi;
        int nvbin = int(2*pow(nvolbin,0.3333333333));
        if(nvbin >= maxdiameter) vbin = maxdiameter;
        if(nvbin <= mindiameter) vbin = mindiameter;
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
            }
            if(current == 0 && overlapassignment == 2)
            {
              int checkgrain = gridfine[point].grainname;
              if((gremovals[checkgrain]/packedgrain[checkgrain].initsize) < (overlapallowed/5.0))
              {
                gridfine[point].grainname = i;
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
          int point = (column+(row*xpoints1)+(plane*xpoints1*ypoints1));
          int inside = gridcourse[point].inside;
          if(inside == 1)
          {
            int currentcourse = gridcourse[point].available90;
            if(currentcourse == 1)
            {
              uniquecurcoursesize++;
            }
            gridcourse[point].available90 = taken90;
			availablelist.erase(remove_if(availablelist.begin(),availablelist.end(),bind2nd(equal_to<int>(),point)),availablelist.end());
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
    double ea1 = grains[i].euler1;
    double ea2 = grains[i].euler2;
    double ea3 = grains[i].euler3;
    packedgrain[i].initsize = totalcursize;
    packedgrain[i].currentsize = totalcursize;
    packedgrain[i].grainname = i;
    packedgrain[i].centroidx = xc;
    packedgrain[i].centroidy = yc;
    packedgrain[i].centroidz = zc;
    packedgrain[i].radius1 = radcur1;
    packedgrain[i].radius2 = radcur2;
    packedgrain[i].radius3 = radcur3;
    packedgrain[i].temponsurf = tempsurf;
    packedgrain[i].euler1 = ea1;
    packedgrain[i].euler2 = ea2;
    packedgrain[i].euler3 = ea3;
    packedgrain[i].tempneighnum = ncount;
    totalcursize = 0;
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
        if(column < xpoints-1)
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
        if(row < ypoints-1)
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
        if(plane < zpoints-1)
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

void  GrainGeneratorFunc::find_neighbors()
{
  double x = 0;
  double y = 0;
  double z = 0;
  size_t nListSize = 1000;
  vector<int> nlist(nListSize, -1);


  for(int i = 0; i < numgrains; i++)
  {
    //std::cout << "find_neighbors: " << i << " of " << numgrains << " grains." << std::endl;
      size_t nListIndex = 0;
      for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
      {
//        int onsurf = 0;
        int gnum = gridfine[j].grainname;
        if(gnum == i)
        {
		    x = j%xpoints;
		    y = (j/xpoints)%ypoints;
		    z = j/(xpoints*ypoints);
//          int first = voxels[j].grainname;
          if(x > 0)
          {
              int grain1 = gridfine[j-1].grainname;
              if(grain1 != i)
            {
              nlist[nListIndex] = (grain1);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(x < xpoints-1)
          {
            int grain2 = gridfine[j+1].grainname;
            if(grain2 != i)
            {
              nlist[nListIndex] = (grain2);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(y > 0)
          {
            int grain3 = gridfine[j-(xpoints)].grainname;
            if(grain3 != i)
            {
              nlist[nListIndex] = (grain3);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(y < ypoints-1)
          {
            int grain4 = gridfine[j+(xpoints)].grainname;
            if(grain4 != i)
            {
              nlist[nListIndex] = (grain4);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(z > 0)
          {
            int grain5 = gridfine[j-(xpoints*ypoints)].grainname;
            if(grain5 != i)
            {
              nlist[nListIndex] = (grain5);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(z < zpoints-1)
          {
            int grain6 = gridfine[j+(xpoints*ypoints)].grainname;
            if(grain6 != i)
            {
              nlist[nListIndex] = (grain6);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
        }
	  }
      if (nListIndex > 0)
      {
        vector<int>::iterator newend;
        sort(nlist.begin(),nlist.end());
        newend = unique(nlist.begin(),nlist.end());
        nlist.erase(newend,nlist.end());
        int numneighs = int(nlist.size());
        packedgrain[i].numneighbors = numneighs;
        packedgrain[i].neighborlist = new std::vector<int>(numneighs);
        packedgrain[i].neighborlist->swap(nlist);
//        grains[i].neighborlist = nlist;
      }
      nlist.clear();
      nlist.resize(nListSize);
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

void  GrainGeneratorFunc::measure_misorientations ()
{
  size_t initialsize=10;
  vector<double> misolist(initialsize,-1);
  double n1;
  double n2;
  double n3;
  for (int i = 0; i < numgrains; i++)
  {
    vector<int>* nlist = packedgrain[i].neighborlist;
    double g1ea1 = packedgrain[i].avgeuler1;
    double g1ea2 = packedgrain[i].avgeuler2;
    double g1ea3 = packedgrain[i].avgeuler3;
    int size = 0;
    if (NULL != nlist) { size = nlist->size(); }
    for(int j=0;j<size;j++)
    {
      int nname = nlist->at(j);
      double g2ea1 = packedgrain[nname].avgeuler1;
      double g2ea2 = packedgrain[nname].avgeuler2;
      double g2ea3 = packedgrain[nname].avgeuler3;
      double w = getmisoquat(crystruct,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
      misolist[j] = w;
    }
	packedgrain[i].misorientationlist = new std::vector<double>(misolist.size() );
	packedgrain[i].misorientationlist->swap(misolist);
    misolist.clear();
  }
}

double GrainGeneratorFunc::getmisoquat(double crystruct,double g1ea1,double g1ea2,double g1ea3,double g2ea1,double g2ea2,double g2ea3,double &n1,double &n2,double &n3)
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
    vector<double>* misolist = packedgrain[i].misorientationlist;
    int size = int(misolist->size());
    for(int k=0;k<size;k++)
    {
      double misofirst = misolist->at(k);
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
    vector<double>* misolist = packedgrain[i].misorientationlist;
    int size = int(misolist->size());
    for(int k=0;k<size;k++)
    {
      double misofirst = misolist->at(k);
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
    vector<double>* misolist = packedgrain[i].misorientationlist;
    int size = int(misolist->size());
    for(int k=0;k<size;k++)
    {
      double misofirst = misolist->at(k);
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
    vector<double>* misolist = packedgrain[i].misorientationlist;
    vector<int>* nlist = packedgrain[i].neighborlist;
    int size = 0;
    if (NULL != nlist) { size = nlist->size(); }
    for(int j=0;j<size;j++)
    {
      int nname = nlist->at(j);
      double neighfrac = packedgrain[nname].lowanglefraction;
      if(neighfrac > 0.24) neighfrac = neighfrac*4;
      if(neighfrac > 0.49) neighfrac = neighfrac;
      if(neighfrac > 0.74) neighfrac = neighfrac*2;
      temprank = neighfrac;
      double misofirst = misolist->at(j);
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
    vector<double>* misolist = packedgrain[i].misorientationlist;
    vector<int>* nlist = packedgrain[i].neighborlist;
    int size = 0;
    if (NULL != nlist) { size = nlist->size(); }
    for(int j=0;j<size;j++)
    {
      int nname = nlist->at(j);
      double neighmiso = misolist->at(j);
      double neighfrac = packedgrain[nname].lowanglefraction;
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

void GrainGeneratorFunc::write_eulerangles(string writename10)
{
  //std::cout << "GrainGeneratorFunc::write_volume1: '" << writename10 << "'" << std::endl;
  ofstream outFile;
  outFile.open(writename10.c_str());
  outFile << numgrains << endl;
  for (int i = 0; i < numgrains; i++)
  {
    double ea1 = grains[i].euler1;
    double ea2 = grains[i].euler2;
    double ea3 = grains[i].euler3;
    outFile << i << " " << ea1 << " " << ea2 << " " << ea3 << endl;
  }
  outFile.close();
}

void GrainGeneratorFunc::create_dxfile(string dxfile)
{
  ofstream outFile;
  outFile.open(dxfile.c_str());
  outFile << "object 1 class gridpositions counts " << xpoints+1 << " " << ypoints+1 << " " << zpoints+1 << endl;
  outFile << "origin	0	0	0" << endl;
  outFile << "delta	1	0	0" << endl;
  outFile << "delta	0	1	0" << endl;
  outFile << "delta	0	0	0" << endl;
  outFile << endl;
  outFile << "object 2 class gridconnections counts " <<  xpoints+1 << " " << ypoints+1 << " " << zpoints+1 << endl;
  outFile << endl;
  outFile << "object 3 class array type int rank 0 items " << totalpoints << " data follows" << endl;
  for(int i=0; i<totalpoints; i++)
  {
    outFile << gridfine[i].grainname+1 << " ";
    if((i!=0)&&(i%20==0))
	{
      outFile << endl;
    }
  }
  outFile << endl;
  outFile << "attribute 'dep' string 'connections" << endl;
  outFile << endl;
  outFile << "object '3d Micro' class field" << endl;
  outFile << "component  'positions'    value 1" << endl;
  outFile << "component  'connections'  value 2" << endl;
  outFile << "component  'data'         value 3" << endl;
  outFile << endl;
  outFile << "end" << endl;
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

