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

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cmath>

#include "AIM/Common/MisorientationCalculations.h"


const static double m_onepointthree = 1.33333333333;
const static double m_pi = 3.1415926535897;

#define DIMS "DIMENSIONS"
#define LOOKUP "LOOKUP_TABLE"

#if 0
// -i C:\Users\GroebeMA\Desktop\NewFolder --outputDir C:\Users\GroebeMA\Desktop\NewFolder -f Slice_ --angMaxSlice 400 -s 1 -e 30 -z 0.25 -t -g 10 -c 0.1 -o 5.0 -x 2
#endif



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

using namespace std;

GrainGeneratorFunc::GrainGeneratorFunc() :
actualodf(NULL),
simodf(NULL),
axisodf(NULL),
precipaxisodf(NULL),
voxels(NULL),
actualmdf(NULL),
simmdf(NULL),
actualmicrotex(NULL),
simmicrotex(NULL),

precipitates(NULL),
psizes(NULL)
{

}

GrainGeneratorFunc::~GrainGeneratorFunc()
{

  grains.clear();
  delete [] precipitates;
  delete [] actualodf;
  delete [] simodf;
  delete [] axisodf;
  delete [] voxels;
  delete [] actualmdf;
  delete [] simmdf;
  delete [] actualmicrotex;
  delete [] simmicrotex;

}

void GrainGeneratorFunc::initialize(int32_t m_NumGrains, int32_t m_ShapeClass, double m_XResolution, double m_YResolution, double m_ZResolution, 
			  double m_fillingerrorweight, double m_neighborhooderrorweight, double m_sizedisterrorweight, int32_t m_Precipitates,
              AIM::Reconstruction::CrystalStructure m_CrystalStructure, double m_FractionPrecipitates)
{

  resx = m_XResolution;
  resy = m_YResolution;
  resz = m_ZResolution;

  resdiff = 1;
  fractionprecip = m_FractionPrecipitates;

  numgrains = m_NumGrains;
  numextragrains = (25*m_NumGrains);
  shapeclass = m_ShapeClass;
  preciptype = m_Precipitates;
  crystruct = m_CrystalStructure;

  fillingerrorweight = m_fillingerrorweight;
  neighborhooderrorweight = m_neighborhooderrorweight;
  sizedisterrorweight = m_sizedisterrorweight;

  grains.resize((numextragrains+1), Grain());
  if(crystruct == 1)
  {
	  actualodf = new Bin[36*36*12];
	  simodf = new Bin[36*36*12];
  }
  if(crystruct == 2)
  {
	  actualodf = new Bin[18*18*18];
	  simodf = new Bin[18*18*18];
  }
  axisodf = new Bin[18*18*18];
  precipaxisodf = new Bin[18*18*18];
  actualmdf = new Bin[36];
  simmdf = new Bin[36];
  actualmicrotex = new Bin[10];
  simmicrotex = new Bin[10];
}
void GrainGeneratorFunc::initialize2()
{
  resx = resx*4.0;
  resy = resy*4.0;
  resz = resz*4.0;
  sizex = (pow(totalvol,0.33333));
  sizey = (pow(totalvol,0.33333));
  sizez = (pow(totalvol,0.33333));
  xpoints = int((sizex/resx)+1);
  ypoints = int((sizey/resy)+1);
  zpoints = int((sizez/resz)+1);
  sizex = xpoints*resx;
  sizey = ypoints*resy;
  sizez = zpoints*resz;
  totalvol = ((xpoints-1)*resx)*((ypoints-1)*resy)*((zpoints-1)*resz);
  totalpoints = xpoints * ypoints * zpoints;

  voxels = new Voxel[totalpoints];
}

#define CHECK_STATS_READ_ERROR(err, name)\
if (err < 0) {\
  std::cout << "GrainGeneratorFunc::readReconStatsData Error: Could not read the " << name << " Group Data" << std::endl;\
  return err; }


#define READ_STATS_DATA_DISTRIBUTION(err, group, var)\
    path = group + ("/") + AIM::HDF5::BinNumber;\
    err = h5io->readStatsDataset(path, binNumbers);\
    CHECK_STATS_READ_ERROR(err, path)\
    path = group + ("/") + AIM::HDF5::Average;\
    err = h5io->readStatsDataset(path, averages);\
    CHECK_STATS_READ_ERROR(err, path)\
    path = group + ("/") + AIM::HDF5::StandardDeviation;\
    err = h5io->readStatsDataset(path, stdDevs);\
    CHECK_STATS_READ_ERROR(err, path)\
    var.resize(maxdiameter + 1);\
    for (int temp7 = 0; temp7 < maxdiameter + 1; temp7++)\
    {\
      if (temp7 < mindiameter) var[temp7].resize(2, 0);\
      if (temp7 >= mindiameter)\
      {\
        var[binNumbers[temp7]].resize(3);\
        var[binNumbers[temp7]][0] = averages[temp7];\
        var[binNumbers[temp7]][1] = stdDevs[temp7];\
      }\
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrainGeneratorFunc::readReconStatsData(H5ReconStatsReader::Pointer h5io)
{
  int err = -1;
  std::vector<int> int_data;
  std::vector<double> double_data;
  std::string path;
  /* Read the Grain_Diameter_Info Data */
  err = h5io->readStatsDataset(AIM::HDF5::Grain_Diameter_Info, int_data);
  CHECK_STATS_READ_ERROR(err, AIM::HDF5::Grain_Diameter_Info)
  numdiameters = int_data[0];
  maxdiameter  = int_data[1];
  mindiameter = int_data[2];

  /* Read the Grain_Size_Distribution Data */
  err = h5io->readStatsDataset(AIM::HDF5::Grain_Size_Distribution, double_data);
  CHECK_STATS_READ_ERROR(err, AIM::HDF5::Grain_Size_Distribution)
  avgdiam = double_data[0];
  sddiam = double_data[1];
  grainsizedist.resize(maxdiameter+1);
  simgrainsizedist.resize(maxdiameter+1);
  double root2pi = pow((2.0 * 3.1415926535897), 0.5);
  for(int i=0;i<maxdiameter+1;i++)
  {
	if(i < mindiameter) grainsizedist[i] = 0;
	if(i >= mindiameter)
	{
		grainsizedist[i] = (1.0 / (double(i+0.5) * double_data[1] * root2pi)) * exp(-((log(double(i+0.5)) - double_data[0]) * (log(double(i+0.5)) - double_data[0])) / (2 * double_data[1 * double_data[1]]));
	}
  }


  /* Read the Grain_SizeVBoverA_Distributions Data */
  std::vector<int> binNumbers;
  std::vector<double> averages;
  std::vector<double>  stdDevs;

  READ_STATS_DATA_DISTRIBUTION(err, AIM::HDF5::Grain_SizeVBoverA_Distributions, bovera)
  READ_STATS_DATA_DISTRIBUTION(err, AIM::HDF5::Grain_SizeVCoverA_Distributions, covera)
  READ_STATS_DATA_DISTRIBUTION(err, AIM::HDF5::Grain_SizeVCoverB_Distributions, coverb)
  READ_STATS_DATA_DISTRIBUTION(err, AIM::HDF5::Grain_SizeVOmega3_Distributions, svomega3)
  std::vector<double> a;
  std::vector<double> b;
  std::vector<double> k;


  path = AIM::HDF5::Grain_SizeVNeighbors_Distributions + ("/") + AIM::HDF5::BinNumber;
  err = h5io->readStatsDataset(path, binNumbers);
  CHECK_STATS_READ_ERROR(err, path)

  path = AIM::HDF5::Grain_SizeVNeighbors_Distributions + ("/") + AIM::HDF5::alpha;
  err = h5io->readStatsDataset(path, a);
  CHECK_STATS_READ_ERROR(err, path)
  path = AIM::HDF5::Grain_SizeVNeighbors_Distributions + ("/") + AIM::HDF5::beta;
  err = h5io->readStatsDataset(path, b);
  CHECK_STATS_READ_ERROR(err, path)
  path = AIM::HDF5::Grain_SizeVNeighbors_Distributions + ("/") + AIM::HDF5::Exp_k;
  err = h5io->readStatsDataset(path, k);
  CHECK_STATS_READ_ERROR(err, path)

  neighborhood.resize(maxdiameter + 1);
  for (int temp7 = 0; temp7 < maxdiameter + 1; temp7++)
  {
    if (temp7 < mindiameter) neighborhood[temp7].resize(3, 0);
    if (temp7 >= mindiameter)
    {
      neighborhood[binNumbers[temp7]].resize(3);
      neighborhood[binNumbers[temp7]][0] = a[temp7]*pow(1,k[temp7])+b[temp7];
      neighborhood[binNumbers[temp7]][1] = a[temp7]*pow(2,k[temp7])+b[temp7];
      neighborhood[binNumbers[temp7]][2] = a[temp7]*pow(3,k[temp7])+b[temp7];
    }
  }
  neighbordist.resize(maxdiameter + 1);
  for (int i = 0; i < maxdiameter + 1; i++)
  {
    neighbordist[i].resize(3, 0.0);
  }

  return err;
}

int  GrainGeneratorFunc::readAxisOrientationData(H5ReconStatsReader::Pointer h5io)
{
  std::vector<double> density;
  int err = 0;
  double totaldensity = 0;
//FIXME: Where is the reading of the Precipiate Data?
  err = h5io->readStatsDataset(AIM::HDF5::AxisOrientation, density);
//FIXME: This seems to be forcing FCC. What if the reconstruction was Hexagonal?
  size_t size = 18 * 18 * 18;

  if (size != density.size() )
  {
    std::cout << "GrainGeneratorFunc::readAxisOrientationData Error: Mismatch in number of elements in the 'AxisOrientation' "
         << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
         << " elements and we need " << size << " Elements. "<< std::endl;
    return -1;
  }
  for (size_t k = 0; k < size; k++)
  {
    totaldensity = totaldensity + density[k];
    axisodf[k].density = totaldensity;
  }
  return err;
}


int GrainGeneratorFunc::readODFData(H5ReconStatsReader::Pointer h5io)
{
  std::vector<double> density;
  int err = 0;
  err = h5io->readStatsDataset(AIM::HDF5::ODF, density);
  size_t numbins = 0;
  if(crystruct == AIM::Reconstruction::Hexagonal) numbins = 36*36*12;
  if(crystruct == AIM::Reconstruction::Cubic) numbins = 18*18*18;

  if (numbins != density.size() )
  {
    std::cout << "GrainGeneratorFunc::readODFData Error: Mismatch in number of elements in the 'ODF' "
         << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
         << " elements and we need " << numbins << " Elements. "<< std::endl;
    return -1;
  }
  for (size_t i = 0; i < numbins; i++)
  {
    actualodf[i].density = density[i];
  }
  return err;
}

int GrainGeneratorFunc::readMisorientationData(H5ReconStatsReader::Pointer h5io)
{
  std::vector<double> density;
  int err = 0;
  err = h5io->readStatsDataset(AIM::HDF5::MisorientationBins, density);
  size_t numbins = 36;

  if (numbins != density.size() )
  {
    std::cout << "GrainGeneratorFunc::readMisorientationData Error: Mismatch in number of elements in the 'ODF' "
         << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
         << " elements and we need " << numbins << " Elements. "<< std::endl;
    return -1;
  }

  for (size_t k = 0; k < numbins; k++)
  {
    actualmdf[k].density = density[k];
  }
  return err;
}


int GrainGeneratorFunc::readMicroTextureData(H5ReconStatsReader::Pointer h5io)
{
  std::vector<double> density;
  int err = 0;
  err = h5io->readStatsDataset(AIM::HDF5::MicroTextureBins, density);
  size_t numbins = 10;

  if (numbins != density.size() )
  {
    std::cout << "GrainGeneratorFunc::readMicroTextureData Error: Mismatch in number of elements in the 'ODF' "
         << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
         << " elements and we need " << numbins << " Elements. "<< std::endl;
    return -1;
  }

  for (size_t k = 0; k < numbins; k++)
  {
    actualmdf[k].density = density[k];
  }
  return err;
}

void  GrainGeneratorFunc::generate_grain(int gnum)
{
  int good = 0;
  double r1 = 1;
  double u=0 ;
  double a1=0,a2=0,a3=0;
  double b1=0,b2=0,b3=0;
  double r2=0,r3=0;
  double diam = 0;
  double vol = 0;
//  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
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
  grains[gnum].volume = vol;
  grains[gnum].equivdiameter = diam;
  grains[gnum].radius1 = r1;
  grains[gnum].radius2 = r2;
  grains[gnum].radius3 = r3;
  grains[gnum].axiseuler1 = phi1;
  grains[gnum].axiseuler2 = PHI;
  grains[gnum].axiseuler3 = phi2;
  grains[gnum].omega3 = omega3;
}

void  GrainGeneratorFunc::insert_grain(int gnum)
{
  double dist;
  double Nvalue = 0;
  double Gvalue = 0;
  double inside = -1;
  int index;
  int column, row, plane;
  double xmin, xmax, ymin, ymax, zmin, zmax;
  double xc, yc, zc;
  double xp, yp, zp;
  double x, y, z;
//  double x1, y1, z1;
//  double x2, y2, z2;
  double ellipfunc = 0;
  double insidecount = 0;
  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
  std::vector<int> insidelist(1000,-1);
  std::vector<double> ellipfunclist(1000,-1);
  double volcur = grains[gnum].volume;
  double bovera = grains[gnum].radius2;
  double covera = grains[gnum].radius3;
  double omega3 = grains[gnum].omega3;
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
  double phi1 = grains[gnum].axiseuler1;
  double PHI = grains[gnum].axiseuler2;
  double phi2 = grains[gnum].axiseuler3;
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
  xc = grains[gnum].centroidx;
  yc = grains[gnum].centroidy;
  zc = grains[gnum].centroidz;
  column = (xc-(resx/2))/resx;
  row = (yc-(resy/2))/resy;
  plane = (zc-(resz/2))/resz;
  xmin = int(column-((radcur1/resx)+1));
  xmax = int(column+((radcur1/resx)+1));
  ymin = int(row-((radcur1/resy)+1));
  ymax = int(row+((radcur1/resy)+1));
  zmin = int(plane-((radcur1/resz)+1));
  zmax = int(plane+((radcur1/resz)+1));
  for(int iter1 = xmin; iter1 < xmax+1; iter1++)
  {
	  for(int iter2 = ymin; iter2 < ymax+1; iter2++)
	  {
		for(int iter3 = zmin; iter3 < zmax+1; iter3++)
		{
		  column = iter1;
		  row = iter2;
		  plane = iter3;
		  if(iter1 < 0) column = iter1+xpoints;
		  if(iter1 > xpoints-1) column = iter1-xpoints;
		  if(iter2 < 0) row = iter2+ypoints;
		  if(iter2 > ypoints-1) row = iter2-ypoints;
		  if(iter3 < 0) plane = iter3+zpoints;
		  if(iter3 > zpoints-1) plane = iter3-zpoints;
		  index = (plane*xpoints*ypoints)+(row*xpoints)+column;
		  inside = -1;
		  x = find_xcoord(index);
		  y = find_ycoord(index);
		  z = find_zcoord(index);
		  if(iter1 < 0) x = x-sizex;
		  if(iter1 > xpoints-1) x = x+sizex;
		  if(iter2 < 0) y = y-sizey;
		  if(iter2 > ypoints-1) y = y+sizey;
		  if(iter3 < 0) z = z-sizez;
		  if(iter3 > zpoints-1) z = z+sizez;
		  dist = ((x-xc)*(x-xc))+((y-yc)*(y-yc))+((z-zc)*(z-zc));
		  dist = pow(dist,0.5);
		  if(dist < radcur1)
		  {
			  x = x-xc;
			  y = y-yc;
			  z = z-zc;
			  xp = (x*ga[0][0])+(y*ga[1][0])+(z*ga[2][0]);
			  yp = (x*ga[0][1])+(y*ga[1][1])+(z*ga[2][1]);
			  zp = (x*ga[0][2])+(y*ga[1][2])+(z*ga[2][2]);
			  double axis1comp = xp/radcur1;
			  double axis2comp = yp/radcur2;
			  double axis3comp = zp/radcur3;
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
				int currentpoint = index;
				ellipfunc = (-0.1/((pow((axis1comp+axis2comp+axis3comp),1)*(1.0-(1.0/(0.80*0.80))))))*(1.0-(((axis1comp+axis2comp+axis3comp)*(axis1comp+axis2comp+axis3comp))/(0.80*0.80)));
				insidelist[insidecount] = currentpoint;
				ellipfunclist[insidecount] = ellipfunc;
				insidecount++;
				if (insidecount >= (insidelist.size()))
				{
					insidelist.resize(insidecount + 1000,-1);
					ellipfunclist.resize(insidecount + 1000,-1);
				}
			  }
		  }
		}
	  }
  }
  insidelist.erase(std::remove(insidelist.begin(),insidelist.end(),-1),insidelist.end());
  ellipfunclist.erase(std::remove(ellipfunclist.begin(),ellipfunclist.end(),-1),ellipfunclist.end());
  grains[gnum].voxellist = new std::vector<int>(insidecount);
  grains[gnum].ellipfunclist = new std::vector<double>(insidecount);
  grains[gnum].voxellist->swap(insidelist);
  grains[gnum].ellipfunclist->swap(ellipfunclist);
  grains[gnum].neighbordistfunc.resize(3,0);
  grains[gnum].neighbordistfunclist.resize(3);
  insidelist.clear();
}

void  GrainGeneratorFunc::remove_grain(int gnum)
{
  int index;
  double ellipfunc;
//  int neigh;
  for(size_t i=0;i<grains[gnum].voxellist->size();i++)
  {
	index = grains[gnum].voxellist->at(i);
	ellipfunc = grains[gnum].ellipfunclist->at(i);
	voxels[index].grainlist->erase(std::remove(voxels[index].grainlist->begin(),voxels[index].grainlist->end(),gnum),voxels[index].grainlist->end());
	voxels[index].ellipfunclist->erase(std::remove(voxels[index].ellipfunclist->begin(),voxels[index].ellipfunclist->end(),ellipfunc),voxels[index].ellipfunclist->end());
  }
  for(int i=0;i<3;i++)
  {
	  for(size_t j=0;j<grains[gnum].neighbordistfunclist[i].size();j++)
	  {
		index = grains[gnum].neighbordistfunclist[i][j];
		grains[index].neighbordistfunc[i] = grains[index].neighbordistfunc[i]-1;
	  }
  }
}
void  GrainGeneratorFunc::add_grain(int gnum)
{
  int index;
  double ellipfunc;
  int neigh;
  for(size_t i=0;i<grains[gnum].voxellist->size();i++)
  {
	index = grains[gnum].voxellist->at(i);
	ellipfunc = grains[gnum].ellipfunclist->at(i);
	voxels[index].grainlist->resize(voxels[index].grainlist->size()+1);
	voxels[index].ellipfunclist->resize(voxels[index].ellipfunclist->size()+1);
	voxels[index].grainlist->at(voxels[index].grainlist->size()-1) = gnum;
	voxels[index].ellipfunclist->at(voxels[index].ellipfunclist->size()-1) = ellipfunc;
  }
  for(int i=0;i<3;i++)
  {
	  for(size_t j=0;j<grains[gnum].neighbordistfunclist[i].size();j++)
	  {
		index = grains[gnum].neighbordistfunclist[i][j];
		grains[index].neighbordistfunc[i]++;
	  }
  }
}
void GrainGeneratorFunc::determine_neighbors()
{
  double x, y, z;
  double rad1, rad2;

  double xn, yn, zn;
  double dia, dia2;
  int DoverR;
  double xdist, ydist, zdist, totdist;
  int nnum = 0;
  for(int gnum=1;gnum<(numextragrains+1);gnum++)
  {
	    nnum = 0;
		x = grains[gnum].centroidx;
		y = grains[gnum].centroidy;
		z = grains[gnum].centroidz;
		dia = grains[gnum].equivdiameter;
		for(int n=gnum;n<(numextragrains+1);n++)
		{
			xn = grains[n].centroidx;
			yn = grains[n].centroidy;
			zn = grains[n].centroidz;
			dia2 = grains[n].equivdiameter;
			xdist = fabs(x-xn);
			ydist = fabs(y-yn);
			zdist = fabs(z-zn);
			totdist = (xdist*xdist)+(ydist*ydist)+(zdist*zdist);
			totdist = pow(totdist,0.5);
			if(totdist < (3*dia))
			{
				DoverR = int(totdist/dia);
				grains[n].neighbordistfunclist[int(DoverR)].resize(grains[n].neighbordistfunclist[int(DoverR)].size()+1);
				grains[n].neighbordistfunclist[int(DoverR)][(grains[n].neighbordistfunclist[int(DoverR)].size()-1)] = gnum;
			}
			if(totdist < (3*dia2))
			{
				DoverR = int(totdist/dia2);
				grains[gnum].neighbordistfunclist[int(DoverR)].resize(grains[gnum].neighbordistfunclist[int(DoverR)].size()+1);
				grains[gnum].neighbordistfunclist[int(DoverR)][(grains[gnum].neighbordistfunclist[int(DoverR)].size()-1)] = n;
			}
		}
  }
}
double GrainGeneratorFunc::check_neighborhooderror(int gadd, int gremove)
{
	double p, df;
	double uvar;
	double tvalue;
	double neighborerror = 0;
	double dia;
	int nnum;
	int index;
	for(int i=0;i<maxdiameter+1;i++)
	{
		neighbordist[i][0] = 0;
		neighbordist[i][1] = 0;
		neighbordist[i][2] = 0;
	}
	if(gadd > 0)
	{
	  for(int i=0;i<3;i++)
	  {
		  for(size_t j=0;j<grains[gadd].neighbordistfunclist[i].size();j++)
		  {
			index = grains[gadd].neighbordistfunclist[i][j];
			grains[index].neighbordistfunc[i]++;
		  }
	  }
	}
	if(gremove > 0)
	{
	  for(int i=0;i<3;i++)
	  {
		  for(size_t j=0;j<grains[gremove].neighbordistfunclist[i].size();j++)
		  {
			index = grains[gremove].neighbordistfunclist[i][j];
			grains[index].neighbordistfunc[i] = grains[index].neighbordistfunc[i]-1;
		  }
	  }
	}
	for(size_t i=1;i<activegrainlist.size();i++)
	{
		nnum=0;
		index = activegrainlist[i];
		if(index != gremove)
		{
			for(int j=0;j<3;j++)
			{
				nnum = grains[index].neighbordistfunc[j];
				dia = grains[index].equivdiameter;
				if(dia > maxdiameter) dia = maxdiameter;
				if(dia < mindiameter) dia = mindiameter;
				if(nnum > 0)
				{
					neighbordist[dia][j] = neighbordist[dia][j]+nnum;
				}
			}
		}
	}
	if(gadd > 0)
	{
		for(int j=0;j<3;j++)
		{
			nnum = grains[index].neighbordistfunc[j];
			dia = grains[index].equivdiameter;
			if(dia > maxdiameter) dia = maxdiameter;
			if(dia < mindiameter) dia = mindiameter;
			if(nnum > 0)
			{
				neighbordist[dia][j] = neighbordist[dia][j]+nnum;
			}
		}
	}
	for(int i=0;i<maxdiameter+1;i++)
	{
		for(int j=0;j<3;j++)
		{
			neighborerror = neighborerror + ((neighborhood[i][j]-neighbordist[i][j])*(neighborhood[i][j]-neighbordist[i][j]));
		}
	}
	if(gadd > 0)
	{
	  for(int i=0;i<4;i++)
	  {
		  for(size_t j=0;j<grains[gadd].neighbordistfunclist[i].size();j++)
		  {
			index = grains[gadd].neighbordistfunclist[i][j];
			grains[index].neighbordistfunc[i] = grains[index].neighbordistfunc[i]-1;
		  }
	  }
	}
	if(gremove > 0)
	{
	  for(int i=0;i<4;i++)
	  {
		  for(size_t j=0;j<grains[gremove].neighbordistfunclist[i].size();j++)
		  {
			index = grains[gremove].neighbordistfunclist[i][j];
			grains[index].neighbordistfunc[i]++;
		  }
	  }
	}
	return neighborerror;
}
double GrainGeneratorFunc::costcheck_remove(int gnum)
{
  int index;
  double removecost = 0;
  for(size_t i=0;i<grains[gnum].voxellist->size();i++)
  {
	index = grains[gnum].voxellist->at(i);
	if(voxels[index].grainlist->size() == 1) removecost = removecost+1.0;
	if(voxels[index].grainlist->size() > 1)
	{
		if(voxels[index].grainlist->size() == 2) removecost = removecost - voxels[index].ellipfunclist->at(0);
		removecost = removecost - grains[gnum].ellipfunclist->at(i);
	}
  }
  return removecost;
}
double GrainGeneratorFunc::costcheck_add(int gnum)
{
  int index;
  double addcost = 0;
  for(size_t i=0;i<grains[gnum].voxellist->size();i++)
  {
	index = grains[gnum].voxellist->at(i);
	if(voxels[index].grainlist->size() == 0) addcost = addcost-1.0;
	if(voxels[index].grainlist->size() >= 1)
	{
		if(voxels[index].grainlist->size() == 1) addcost = addcost + voxels[index].ellipfunclist->at(0);
		addcost = addcost + grains[gnum].ellipfunclist->at(i);
	}
  }
  return addcost;
}
double GrainGeneratorFunc::check_sizedisterror(int gadd, int gremove)
{
	double p, df;
	double avgdia=0, stddia=0;
	double uvar;
	double dia;
	double tvalue;
	double sizedisterror = 0;
	int index;
	int count = 0;
	for(int i=0;i<maxdiameter+1;i++)
	{
		simgrainsizedist[i] = 0.0;
	}
	for(size_t b=1;b<activegrainlist.size();b++)
	{
		index = activegrainlist[b];
		if(index != gremove)
		{
			dia = grains[index].equivdiameter;
			simgrainsizedist[int(dia)]++;
			count++;
		}
	}
	if(gadd > 0)
	{
		dia = grains[gadd].equivdiameter;
		simgrainsizedist[int(dia)]++;
		count++;
	}
	for(int i=0;i<maxdiameter+1;i++)
	{
		simgrainsizedist[i] = simgrainsizedist[i]/double(count);
	}
	for(int i=0;i<maxdiameter+1;i++)
	{
		sizedisterror = sizedisterror + ((simgrainsizedist[i]-grainsizedist[i])*(simgrainsizedist[i]-grainsizedist[i]));
	}
	return sizedisterror;
}
int  GrainGeneratorFunc::pack_grains(int numgrains)
{
  totalvol = 0;
  double change1, change2, change3;
  ofstream outFile;
  string filename = "test.txt";
  outFile.open(filename.c_str());
  size_t index;
  double xc, yc, zc;
  currentfillingerror = 0, oldfillingerror = 0;
  currentneighborhooderror = 0, oldneighborhooderror = 0;
  currentsizedisterror = 0, oldsizedisterror = 0;
  double addcost, removecost;
  int acceptedmoves = 0;
  rg.RandomInit((static_cast<unsigned int > (time(NULL))));
  // std::vector<int>* nlist;
  activegrainlist.resize(numgrains + 1);
  for (int i = 1; i < (numextragrains + 1); i++)
  {
    generate_grain(i);
    totalvol = totalvol + grains[i].volume;
  }
  totalvol = totalvol / (double(numextragrains) / double(numgrains));
  initialize2();
  for (int i = 1; i < (numextragrains + 1); i++)
  {
    xc = rg.Random() * (xpoints * resx);
    yc = rg.Random() * (ypoints * resy);
    zc = rg.Random() * (zpoints * resz);
    grains[i].centroidx = xc;
    grains[i].centroidy = yc;
    grains[i].centroidz = zc;
  }
  for (int i = 1; i < (numextragrains + 1); i++)
  {
    insert_grain(i);
  }
  if(neighborhooderrorweight > 0) determine_neighbors();
  oldfillingerror = numextragrains;
  for (int i = 1; i < numgrains + 1; i++)
  {
    int random = int(rg.Random() * (numextragrains));
    if (random == 0) random = 1;
    if (random > (numextragrains)) random = (numextragrains);
    while (grains[random].active == 1)
    {
      random++;
      if (random > (numextragrains)) random = random - (numextragrains);
      if (random == 0) random = 1;
    }
    grains[random].active = 1;
    addcost = costcheck_add(random);
    add_grain(random);
    activegrainlist[i] = random;
    oldfillingerror = oldfillingerror + addcost;
  }
  for(int i=0;i<maxdiameter+1;i++)
  {
	  oldsizedisterror = oldsizedisterror + (grainsizedist[i]*grainsizedist[i]);
  }
  oldneighborhooderror = (maxdiameter + 1) * 4;
  for (int iteration = 0; iteration < (250000); iteration++)
  {
	change1 = 0;
	change2 = 0;
	change3 = 0;
    int option = iteration % 4;
	if(iteration%50 == 0) outFile << oldfillingerror << "	" << oldsizedisterror << "	" << oldneighborhooderror << "	" << acceptedmoves << endl;
    if (option == 0)
    {
      int random = int(rg.Random() * (numextragrains));
      if (random == 0) random = 1;
      if (random > (numextragrains)) random = (numextragrains);
      while (grains[random].active == 1)
      {
        random++;
        if (random > (numextragrains)) random = random - (numextragrains);
        if (random == 0) random = 1;
      }
      addcost = costcheck_add(random);
      if(fillingerrorweight > 0) currentfillingerror = oldfillingerror + addcost;
      if(sizedisterrorweight > 0) currentsizedisterror = check_sizedisterror(random,-1000);
      if(neighborhooderrorweight > 0) currentneighborhooderror = check_neighborhooderror(random,-1000);
      if(fillingerrorweight > 0) change1 = (currentfillingerror - oldfillingerror) / oldfillingerror;
      if(fillingerrorweight > 0 && oldfillingerror < 0) change1 = -change1;
      if(sizedisterrorweight > 0) change2 = (currentsizedisterror - oldsizedisterror) / oldsizedisterror;
      if(neighborhooderrorweight > 0) change3 = (currentneighborhooderror - oldneighborhooderror) / oldneighborhooderror;
      if (fillingerrorweight*change1 + sizedisterrorweight*change2 + neighborhooderrorweight*change3 <= 0.0)
      {
        grains[random].active = 1;
        add_grain(random);
        activegrainlist.resize(activegrainlist.size() + 1);
        activegrainlist[activegrainlist.size() - 1] = random;
        oldfillingerror = currentfillingerror;
        oldneighborhooderror = currentneighborhooderror;
        oldsizedisterror = currentsizedisterror;
        acceptedmoves++;
      }
    }
    if (option == 1)
    {
      size_t random = int(rg.Random() * activegrainlist.size());
      if (random == 0) random = 1;
      if (random == activegrainlist.size()) random = activegrainlist.size() - 1;
      random = activegrainlist[random];
      removecost = costcheck_remove(random);
      if(fillingerrorweight > 0) currentfillingerror = oldfillingerror + removecost;
      if(sizedisterrorweight > 0) currentsizedisterror = check_sizedisterror(-1000, random);
      if(neighborhooderrorweight > 0) currentneighborhooderror = check_neighborhooderror(-1000,random);
      if(fillingerrorweight > 0) change1 = (currentfillingerror - oldfillingerror) / oldfillingerror;
      if(fillingerrorweight > 0 && oldfillingerror < 0) change1 = -change1;
      if(sizedisterrorweight > 0) change2 = (currentsizedisterror - oldsizedisterror) / oldsizedisterror;
      if(neighborhooderrorweight > 0) change3 = (currentneighborhooderror - oldneighborhooderror) / oldneighborhooderror;
      if (fillingerrorweight*change1 + sizedisterrorweight*change2 + neighborhooderrorweight*change3 <= 0.0)
      {
        grains[random].active = 0;
        remove_grain(random);
        activegrainlist.erase(std::remove(activegrainlist.begin(), activegrainlist.end(), random), activegrainlist.end());
        oldfillingerror = currentfillingerror;
        oldneighborhooderror = currentneighborhooderror;
        oldsizedisterror = currentsizedisterror;
        acceptedmoves++;
      }
    }
    if (option == 2)
    {
      size_t random1 = int(rg.Random() * activegrainlist.size());
      if (random1 == 0) random1 = 1;
      if (random1 == activegrainlist.size()) random1 = activegrainlist.size() - 1;
      random1 = activegrainlist[random1];
      int random = int(rg.Random() * (numextragrains));
      if (random == 0) random = 1;
      if (random > (numextragrains)) random = (numextragrains);
      while (grains[random].active == 1)
      {
        random++;
        if (random > (numextragrains)) random = random - (numextragrains);
        if (random == 0) random = 1;
      }
      addcost = costcheck_add(random);
      removecost = costcheck_remove(random1);
      if(fillingerrorweight > 0) currentfillingerror = oldfillingerror + addcost + removecost;
      if(sizedisterrorweight > 0) currentsizedisterror = check_sizedisterror(random, random1);
      if(neighborhooderrorweight > 0) currentneighborhooderror = check_neighborhooderror(random,random1);
      if(fillingerrorweight > 0) change1 = (currentfillingerror - oldfillingerror) / oldfillingerror;
      if(fillingerrorweight > 0 && oldfillingerror < 0) change1 = -change1;
      if(sizedisterrorweight > 0) change2 = (currentsizedisterror - oldsizedisterror) / oldsizedisterror;
      if(neighborhooderrorweight > 0) change3 = (currentneighborhooderror - oldneighborhooderror) / oldneighborhooderror;
      if (fillingerrorweight*change1 + sizedisterrorweight*change2 + neighborhooderrorweight*change3 <= 0.0)
      {
        grains[random].active = 1;
        grains[random1].active = 0;
        add_grain(random);
        remove_grain(random1);
        activegrainlist.erase(std::remove(activegrainlist.begin(), activegrainlist.end(), random1), activegrainlist.end());
        activegrainlist.resize(activegrainlist.size() + 1);
        activegrainlist[activegrainlist.size() - 1] = random;
        oldfillingerror = currentfillingerror;
        oldneighborhooderror = currentneighborhooderror;
        oldsizedisterror = currentsizedisterror;
        acceptedmoves++;
      }
    }
    if (option == 3)
    {
      size_t random1 = int(rg.Random() * activegrainlist.size());
      if (random1 == 0) random1 = 1;
      if (random1 == activegrainlist.size()) random1 = activegrainlist.size() - 1;
      random1 = activegrainlist[random1];
      //		double mindist = 100000.0;
      size_t random = random1;
      for (size_t i = 0; i < grains[random1].neighbordistfunclist[1].size(); i++)
      {
        index = grains[random1].neighbordistfunclist[1][i];
        if (grains[index].active == 0 && index != random1)
        {
          random = index;
          i = grains[random1].neighbordistfunclist[1].size();
        }
      }
      if (random != random1)
      {
        addcost = costcheck_add(random);
        removecost = costcheck_remove(random1);
        if(fillingerrorweight > 0) currentfillingerror = oldfillingerror + addcost + removecost;
        if(sizedisterrorweight > 0) currentsizedisterror = check_sizedisterror(random, random1);
        if(neighborhooderrorweight > 0) currentneighborhooderror = check_neighborhooderror(random,random1);
        if(fillingerrorweight > 0) change1 = (currentfillingerror - oldfillingerror) / oldfillingerror;
        if(fillingerrorweight > 0 && oldfillingerror < 0) change1 = -change1;
        if(sizedisterrorweight > 0) change2 = (currentsizedisterror - oldsizedisterror) / oldsizedisterror;
        if(neighborhooderrorweight > 0) change3 = (currentneighborhooderror - oldneighborhooderror) / oldneighborhooderror;
        if (fillingerrorweight*change1 + sizedisterrorweight*change2 + neighborhooderrorweight*change3 <= 0.0)
        {
          grains[random].active = 1;
          grains[random1].active = 0;
          add_grain(random);
          remove_grain(random1);
          activegrainlist.erase(std::remove(activegrainlist.begin(), activegrainlist.end(), random1), activegrainlist.end());
          activegrainlist.resize(activegrainlist.size() + 1);
          activegrainlist[activegrainlist.size() - 1] = random;
          oldfillingerror = currentfillingerror;
          oldneighborhooderror = currentneighborhooderror;
          oldsizedisterror = currentsizedisterror;
          acceptedmoves++;
        }
      }
    }
  }
  sort(activegrainlist.begin(), activegrainlist.end());
  activegrainlist.erase(std::remove(activegrainlist.begin(), activegrainlist.end(), 0), activegrainlist.end());
  for (size_t i = 0; i < activegrainlist.size(); i++)
  {
    grains[i + 1] = grains[activegrainlist[i]];
  }
  grains.resize(activegrainlist.size() + 1);
  return (activegrainlist.size());
}

int GrainGeneratorFunc::assign_voxels(int numgrains)
{
  int count = 0;
  int index;
  int column, row, plane;
  double inside;
  double Nvalue = 0;
  double Gvalue = 0;
  double xc, yc, zc;
  double xp, yp, zp;
  double dist;
  double x, y, z;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  resx = resx/4.0;
  resy = resy/4.0;
  resz = resz/4.0;
  xpoints = int((sizex/resx)+1);
  ypoints = int((sizey/resy)+1);
  zpoints = int((sizez/resz)+1);
  totalpoints = xpoints * ypoints * zpoints;
  delete [] voxels;
  voxels = new Voxel[totalpoints];
  for(int i=1;i<numgrains+1;i++)
  {
	  count++;
	  double volcur = grains[i].volume;
	  double bovera = grains[i].radius2;
	  double covera = grains[i].radius3;
	  double omega3 = grains[i].omega3;
	  xc = grains[i].centroidx;
	  yc = grains[i].centroidy;
	  zc = grains[i].centroidz;
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
	  double phi1 = grains[i].axiseuler1;
	  double PHI = grains[i].axiseuler2;
	  double phi2 = grains[i].axiseuler3;
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
	  column = (xc-(resx/2))/resx;
	  row = (yc-(resy/2))/resy;
	  plane = (zc-(resz/2))/resz;
	  xmin = int(column-((radcur1/resx)+1));
	  xmax = int(column+((radcur1/resx)+1));
	  ymin = int(row-((radcur1/resy)+1));
	  ymax = int(row+((radcur1/resy)+1));
	  zmin = int(plane-((radcur1/resz)+1));
	  zmax = int(plane+((radcur1/resz)+1));
	  for(int iter1 = xmin; iter1 < xmax+1; iter1++)
	  {
		  for(int iter2 = ymin; iter2 < ymax+1; iter2++)
		  {
			for(int iter3 = zmin; iter3 < zmax+1; iter3++)
			{
			  column = iter1;
			  row = iter2;
			  plane = iter3;
			  if(iter1 < 0) column = iter1+xpoints;
			  if(iter1 > xpoints-1) column = iter1-xpoints;
			  if(iter2 < 0) row = iter2+ypoints;
			  if(iter2 > ypoints-1) row = iter2-ypoints;
			  if(iter3 < 0) plane = iter3+zpoints;
			  if(iter3 > zpoints-1) plane = iter3-zpoints;
			  index = (plane*xpoints*ypoints)+(row*xpoints)+column;
			  inside = -1;
			  x = find_xcoord(index);
			  y = find_ycoord(index);
			  z = find_zcoord(index);
			  if(iter1 < 0) x = x-sizex;
			  if(iter1 > xpoints-1) x = x+sizex;
			  if(iter2 < 0) y = y-sizey;
			  if(iter2 > ypoints-1) y = y+sizey;
			  if(iter3 < 0) z = z-sizez;
			  if(iter3 > zpoints-1) z = z+sizez;
			  dist = ((x-xc)*(x-xc))+((y-yc)*(y-yc))+((z-zc)*(z-zc));
			  dist = pow(dist,0.5);
			  if(dist < radcur1)
			  {
				  x = x-xc;
				  y = y-yc;
				  z = z-zc;
				  xp = (x*ga[0][0])+(y*ga[1][0])+(z*ga[2][0]);
				  yp = (x*ga[0][1])+(y*ga[1][1])+(z*ga[2][1]);
				  zp = (x*ga[0][2])+(y*ga[1][2])+(z*ga[2][2]);
				  double axis1comp = xp/radcur1;
				  double axis2comp = yp/radcur2;
				  double axis3comp = zp/radcur3;
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
					int currentpoint = index;
					if(voxels[currentpoint].grainname > 0)
					{
						voxels[currentpoint].grainname = -1;
						voxels[currentpoint].unassigned = 1;
					}
					if(voxels[currentpoint].grainname == 0 && voxels[currentpoint].unassigned == 0)
					{
						voxels[currentpoint].grainname = i;
					}
				  }
			  }
			}
		 }
	  }
	  grains[i].centroidx = xc;
	  grains[i].centroidy = yc;
	  grains[i].centroidz = zc;
  }
  return(count);
}
void  GrainGeneratorFunc::assign_eulers(int numgrains)
{

  int gnum = 0;
  int phi1, PHI, phi2;
  int numbins = 0;
  if(crystruct == 1) numbins = 36*36*12;
  if(crystruct == 2) numbins = 18*18*18;
  double totaldensity = 0;
  double synea1=0,synea2=0,synea3=0;
  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
  for(int i=1;i<numgrains+1;i++)
  {
	  double random = rg.Random();
	  int choose = 0;
	  totaldensity = 0;
	  for(int j=0;j<numbins;j++)
	  {
		  double density = actualodf[j].density;
		  totaldensity = totaldensity + density;
		  if(random >= totaldensity) choose = j;
	  }
	  if(crystruct == 1)
	  {
		  phi1 = choose%36;
		  PHI = (choose/36)%36;
		  phi2 = choose/(36*36);
	  }
	  if(crystruct == 2)
	  {
		  phi1 = choose%18;
		  PHI = (choose/18)%18;
		  phi2 = choose/(18*18);
	  }
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
      double s=sin(0.5*synea2);
	  double c=cos(0.5*synea2);
	  double s1=sin(0.5*(synea1-synea3));
	  double c1=cos(0.5*(synea1-synea3));
	  double s2=sin(0.5*(synea1+synea3));
	  double c2=cos(0.5*(synea1+synea3));
	  grains[i].avg_quat[1] = s*c1;
	  grains[i].avg_quat[2] = s*s1;
	  grains[i].avg_quat[3] = c*s2;
	  grains[i].avg_quat[4] = c*c2;
	  if(grains[gnum].surfacegrain == 0)
	  {
		  simodf[choose].density = simodf[choose].density + (double(grains[i].numvoxels)*resx*resy*resz/totalvol);
	  }
  }
}

void  GrainGeneratorFunc::fill_gaps(int numgrains)
{
  vector<int> neighs;
  vector<int> remove;
  vector<int> gsizes;
  vector<int>* voxellist;
  int count = 1;
  int good = 1;
  double x, y, z;
  gsizes.resize(numgrains,0);
  int neighpoint;
  int neighbors[6];
  std::vector<int> n(numgrains+1);
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
      if(grainname < 0)
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
				if(grain > 0)
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
      if(grainname <= 0 && neighbor > 0)
      {
        voxels[j].grainname = neighbor;
      }
    }
  }
  gsizes.resize(numgrains+1,0);
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int name = voxels[i].grainname;
	gsizes[name]++;
  }
  count = 1;
  int testcount = 0;
  int size = 0;
  int current = 0;
  int most = 0;
  int curgrain = 0;
  int checkcount = 0;
  while(count != 0)
  {
    count = 0;
    for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
    {
      int grainname = voxels[i].grainname;
      if(grainname == 0)
      {
	    testcount++;
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
				if(grain > 0)
				{
					neighs.push_back(grain);
				}
			}
        }
        size = int(neighs.size());
		current = 0;
		most = 0;
		curgrain = -1;
        for(int k=0;k<size;k++)
        {
          int neighbor = neighs[k];
          n[neighbor]++;
          current = n[neighbor];
		  if(current > most && (gsizes[neighbor]*resx*resy*resz) < (1.3333*m_pi*pow((grains[neighbor].equivdiameter/2.0),3)))
          {
			most = current;
	        curgrain = neighbor;
          }
        }
        if(size > 0 && curgrain > 0)
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
      if(grainname == 0 && neighbor > 0)
      {
		count++;
        voxels[j].grainname = neighbor;
		gsizes[neighbor]++;
      }
    }
  }
  count = 1;
  while(count != 0)
  {
    count = 0;
    for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
    {
      int grainname = voxels[i].grainname;
      if(grainname == 0)
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
				if(grain > 0)
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
      if(grainname <= 0 && neighbor > 0)
      {
        voxels[j].grainname = neighbor;
		gsizes[neighbor]++;
      }
    }
  }
  for (int i = 1; i < numgrains+1; i++)
  {
	  grains[i].numvoxels = gsizes[i];
  }
  gsizes.clear();
}

int GrainGeneratorFunc::create_precipitates()
{
  int good = 0;
  int count = 0;
  double r1 = 1;
  double u=0;
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
  int count = 0;
  int counter=0;
  int good = 0;
  int column = 0;
  int row = 0;
  int plane = 0;
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
		if(voxels[a].surfacevoxel == 1)
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
				if(voxels[currentpoint].grainname > numgrains) badcount++;
              }
            }
          }
        }
        double acceptable = 100*(badcount/insidecount);
        if(acceptable > 0.25) good = 0;
        if(good == 0)
        {
			insidelist.clear();
		}
	}
    tempsurf = 0;
    for(int a=0;a<insidecount;a++)
    {
      int point = insidelist[insidecount];
//      int columncheck = point % xpoints;
//      int rowcheck = (point / xpoints) % ypoints;
//      int planecheck = point / (xpoints * ypoints);
      if (voxels[point].grainname <= numgrains)
      {
        uniquecursize++;
        totalcursize++;
        voxels[point].grainname = i + numgrains;
      }
    }
	int size = availablelist.size();
	for(int p=0;p<size;p++)
	{
		int point = availablelist[p];
		if(voxels[point].grainname > numgrains)
		{
			availablelist.erase(availablelist.begin()+p);
			p = p-1;
			size = availablelist.size();
		}
	}
    uniquecursize = 0;
    precipitates[curprecip].numvoxels = totalcursize;
    precipitates[curprecip].centroidx = xc;
    precipitates[curprecip].centroidy = yc;
    precipitates[curprecip].centroidz = zc;
    precipitates[curprecip].radius1 = radcur1;
    precipitates[curprecip].radius2 = radcur2;
    precipitates[curprecip].radius3 = radcur3;
    totalcursize = 0;
    count++;
  }
  for(int u=1;u<numprecipitates+1;u++)
  {
    psizes[u] = 0;
  }
  for(int t=0;t<(xpoints*ypoints*zpoints);t++)
  {
	if(voxels[t].grainname > numgrains)
    {
	  int gname = voxels[t].grainname;
	  gname = gname-numgrains;
      psizes[gname]++;
    }
  }
  for(int v=1;v<numprecipitates+1;v++)
  {
    int cursize = psizes[v];
	precipitates[v].numvoxels = cursize;
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
			voxels = new Voxel[totalpoints];
			totalvol = double(totalpoints)*resx*resy*resz;
		}
		if(LOOKUP == word)
		{
			headerdone = true;
			in >> word;
		}
	}

	int gnum=0;
	int onedge = 0;
	int col, row, plane;
	for(int i=0;i<(xpoints*ypoints*zpoints);i++)
	{
		onedge = 0;
		in >> gnum;
		col = i%xpoints;
		row = (i/xpoints)%ypoints;
		plane = i/(xpoints*ypoints);
		if(col == 0 || col == (xpoints-1) || row == 0 || row == (ypoints-1) || plane == 0 || plane == (zpoints-1)) onedge = 1;
		voxels[i].grainname = gnum;
		grains[gnum].surfacegrain = onedge;
	}
}
void  GrainGeneratorFunc::find_neighbors()
{
  int neighbors[6];
  neighbors[0] = -(xpoints*ypoints);
  neighbors[1] = -xpoints;
  neighbors[2] = -1;
  neighbors[3] = 1;
  neighbors[4] = xpoints;
  neighbors[5] = (xpoints*ypoints);
  double column = 0;
  double row = 0;
  double plane = 0;
  int grain;
  int nnum;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;
  totalsurfacearea=0;
  int surfacegrain = 1;
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
    onsurf = 0;
    grain = voxels[j].grainname;
	if(grain > 0)
	{
		column = j%xpoints;
		row = (j/xpoints)%ypoints;
		plane = j/(xpoints*ypoints);
		if(column == 0 || column == (xpoints-1) || row == 0 || row == (ypoints-1) || plane == 0 || plane == (zpoints-1)) grains[grain].surfacegrain = surfacegrain;
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
			  nnum = grains[grain].numneighbors;
			  vector<int>* nlist = grains[grain].neighborlist;
			  if (nnum >= (0.9*nlist->size()))
			  {
				 nlist->resize(nnum + nListSize);
			  }
			  nlist->at(nnum) = voxels[neighbor].grainname;
			  nnum++;
			  grains[grain].numneighbors = nnum;
			  voxels[j].nearestneighbor = voxels[neighbor].grainname;
		  }
		}
	}
	voxels[j].surfacevoxel = onsurf;
	if(onsurf > 0) voxels[j].nearestneighbordistance = 0, voxels[j].neighbor = j;
	if(onsurf == 0) voxels[j].nearestneighbordistance = -1, voxels[j].nearestneighbor = -1;
  }
  vector<int>* nlistcopy;
  for(int i=1;i<numgrains+1;i++)
  {
	vector<int>* nlist = grains[i].neighborlist;
	vector<double>* nsalist = grains[i].neighborsurfarealist;
	vector<int>::iterator newend;
	sort(nlist->begin(),nlist->end());
	nlistcopy = nlist;
    newend = unique(nlist->begin(),nlist->end());
    nlist->erase(newend,nlist->end());
	nlist->erase(std::remove(nlist->begin(),nlist->end(),-1),nlist->end());
	nlist->erase(std::remove(nlist->begin(),nlist->end(),0),nlist->end());
	int numneighs = int(nlist->size());
	for(int j=0;j<numneighs;j++)
	{
		int neigh = nlist->at(j);
		int number = std::count(nlistcopy->begin(),nlistcopy->end(),neigh);
		double area = number*resx*resx;
		nsalist->at(j) = area;
		if(grains[i].surfacegrain == 0 && (neigh > i || grains[neigh].surfacegrain == 1))
		{
			totalsurfacearea = totalsurfacearea + area;
		}
	}
    grains[i].numneighbors = numneighs;
    grains[i].neighborlist = new std::vector<int>(numneighs);
    grains[i].neighborlist = nlist;
    grains[i].neighborsurfarealist = new std::vector<double>(numneighs);
    grains[i].neighborsurfarealist = nsalist;
  }
}


void GrainGeneratorFunc::matchCrystallography(const std::string &ErrorFile)
{
	ofstream outFile;
    outFile.open(ErrorFile.c_str());
	double deltaerror = 1.0;
	int iterations = 0;
	double currentodferror = 0;
	double currentmdferror = 0;
	double n1,n2,n3;
	double q1[5];
	double q2[5];
	int good = 0;
	int selectedgrain = 0;
	int selectedgrain1 = 0;
	int selectedgrain2 = 0;
	double totaldensity = 0;
	int badtrycount = 0;
	double s,c,s1,c1,s2,c2;
	int phi1, PHI, phi2;
	int curodfbin;
	int g1odfbin, g2odfbin;
	int numbins = 0;
	if(crystruct == 1) numbins = 36*36*12;
	if(crystruct == 2) numbins = 18*18*18;
    rg.RandomInit((static_cast<unsigned int>(time(NULL))));
	while(badtrycount < 5000 && iterations < 100000)
	{
		currentodferror = 0;
		currentmdferror = 0;
		for(int i=0;i<numbins;i++)
		{
			currentodferror = currentodferror + ((actualodf[i].density-simodf[i].density)*(actualodf[i].density-simodf[i].density));
		}
		for(int i=0;i<(36);i++)
		{
			currentmdferror = currentmdferror + ((actualmdf[i].density-simmdf[i].density)*(actualmdf[i].density-simmdf[i].density));
		}
		outFile << iterations << "	" << currentodferror << "	" << currentmdferror << endl;
		iterations++;
		badtrycount++;
		double random = rg.Random();
		if(random < 0.5)
		{
			good = 0;
			while(good == 0)
			{
				selectedgrain = int(rg.Random()*numgrains)+1;
				if(selectedgrain == 0) selectedgrain = 1;
				if(selectedgrain == numgrains+1) selectedgrain = numgrains;
				if(grains[selectedgrain].surfacegrain == 0) good = 1;
			}
			double curea1 = grains[selectedgrain].euler1;
			double curea2 = grains[selectedgrain].euler2;
			double curea3 = grains[selectedgrain].euler3;
			int cureuler1bin = int(curea1/(5.0*m_pi/180.0));
			int cureuler2bin = int(curea2/(5.0*m_pi/180.0));
			int cureuler3bin = int(curea3/(5.0*m_pi/180.0));
			if(crystruct == 1) curodfbin = (cureuler3bin*36*36)+(cureuler2bin*36)+cureuler1bin;
			if(crystruct == 2) curodfbin = (cureuler3bin*18*18)+(cureuler2bin*18)+cureuler1bin;
			double random = rg.Random();
			int choose = 0;
			totaldensity = 0;
			for(int i=0;i<numbins;i++)
			{
				double density = actualodf[i].density;
				totaldensity = totaldensity + density;
				if(random >= totaldensity) choose = i;
			}
		    if(crystruct == 1)
		    {
				  phi1 = choose%36;
				  PHI = (choose/36)%36;
				  phi2 = choose/(36*36);
		    }
		    if(crystruct == 2)
		    {
				  phi1 = choose%18;
				  PHI = (choose/18)%18;
				  phi2 = choose/(18*18);
		    }
			double random1 = rg.Random();
			double random2 = rg.Random();
			double random3 = rg.Random();
			double chooseea1 = (5*phi1)+(5*random1);
			double chooseea2 = (5*PHI)+(5*random2);
			double chooseea3 = (5*phi2)+(5*random3);
			chooseea1 = chooseea1*(m_pi/180.0);
			chooseea2 = chooseea2*(m_pi/180.0);
			chooseea3 = chooseea3*(m_pi/180.0);
	        s=sin(0.5*chooseea2);
		    c=cos(0.5*chooseea2);
		    s1=sin(0.5*(chooseea1-chooseea3));
			c1=cos(0.5*(chooseea1-chooseea3));
			s2=sin(0.5*(chooseea1+chooseea3));
		    c2=cos(0.5*(chooseea1+chooseea3));
			q1[1] = s*c1;
			q1[2] = s*s1;
			q1[3] = c*s2;
			q1[4] = c*c2;
			double odfchange = ((actualodf[choose].density - simodf[choose].density)*(actualodf[choose].density - simodf[choose].density)) - ((actualodf[choose].density - (simodf[choose].density+(double(grains[selectedgrain].numvoxels)*resx*resy*resz/totalvol)))*(actualodf[choose].density - (simodf[choose].density+(double(grains[selectedgrain].numvoxels)*resx*resy*resz/totalvol))));
			odfchange = odfchange + (((actualodf[curodfbin].density - simodf[curodfbin].density)*(actualodf[curodfbin].density - simodf[curodfbin].density)) - ((actualodf[curodfbin].density - (simodf[curodfbin].density-(double(grains[selectedgrain].numvoxels)*resx*resy*resz/totalvol)))*(actualodf[curodfbin].density - (simodf[curodfbin].density-(double(grains[selectedgrain].numvoxels)*resx*resy*resz/totalvol)))));
			vector<int>* nlist = grains[selectedgrain].neighborlist;
			vector<double>* misolist = grains[selectedgrain].misorientationlist;
			vector<double>* neighborsurfarealist = grains[selectedgrain].neighborsurfarealist;
			double mdfchange = 0;
			for(size_t j=0;j<nlist->size();j++)
			{
				int neighbor = nlist->at(j);
				double curmiso = misolist->at(j);
				double neighsurfarea = neighborsurfarealist->at(j);
				int curmisobin = int(curmiso/5.0);
				q2[1] = grains[neighbor].avg_quat[1];
				q2[2] = grains[neighbor].avg_quat[2];
				q2[3] = grains[neighbor].avg_quat[3];
				q2[4] = grains[neighbor].avg_quat[4];
				double newmiso = 0;
	      if(crystruct == AIM::Reconstruction::Hexagonal){
	        newmiso = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
	      }
	      if(crystruct == AIM::Reconstruction::Cubic) {
	        newmiso = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
	      }

				int newmisobin = int(newmiso/5.0);
				mdfchange = mdfchange + (((actualmdf[curmisobin].density-simmdf[curmisobin].density)*(actualmdf[curmisobin].density-simmdf[curmisobin].density)) - ((actualmdf[curmisobin].density-(simmdf[curmisobin].density-(neighsurfarea/totalsurfacearea)))*(actualmdf[curmisobin].density-(simmdf[curmisobin].density-(neighsurfarea/totalsurfacearea)))));
				mdfchange = mdfchange + (((actualmdf[newmisobin].density-simmdf[newmisobin].density)*(actualmdf[newmisobin].density-simmdf[newmisobin].density)) - ((actualmdf[newmisobin].density-(simmdf[newmisobin].density+(neighsurfarea/totalsurfacearea)))*(actualmdf[newmisobin].density-(simmdf[newmisobin].density+(neighsurfarea/totalsurfacearea)))));
			}
			deltaerror = 4.0*odfchange + 0.25*mdfchange;
			if(deltaerror > 0)
			{
				badtrycount = 0;
				grains[selectedgrain].euler1 = chooseea1;
				grains[selectedgrain].euler2 = chooseea2;
				grains[selectedgrain].euler3 = chooseea3;
				simodf[choose].density = simodf[choose].density + (double(grains[selectedgrain].numvoxels)*resx*resy*resz/totalvol);
				simodf[curodfbin].density = simodf[curodfbin].density - (double(grains[selectedgrain].numvoxels)*resx*resy*resz/totalvol);
				for(size_t j=0;j<nlist->size();j++)
				{
					int neighbor = nlist->at(j);
					double curmiso = misolist->at(j);
					double neighsurfarea = neighborsurfarealist->at(j);
					int curmisobin = int(curmiso/5.0);
					q2[1] = grains[neighbor].avg_quat[1];
					q2[2] = grains[neighbor].avg_quat[2];
					q2[3] = grains[neighbor].avg_quat[3];
					q2[4] = grains[neighbor].avg_quat[4];
					double newmiso = 0;
		      if(crystruct == AIM::Reconstruction::Hexagonal){
		        newmiso = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
		      }
		      if(crystruct == AIM::Reconstruction::Cubic) {
		        newmiso = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
		      }

					int newmisobin = int(newmiso/5.0);
					misolist->at(j) = newmiso;
					simmdf[curmisobin].density = simmdf[curmisobin].density - (neighsurfarea/totalsurfacearea);
					simmdf[newmisobin].density = simmdf[newmisobin].density + (neighsurfarea/totalsurfacearea);
				}
//				currentodferror = currentodferror - odfchange;
//				currentmdferror = currentmdferror - mdfchange;
			}
		}
		if(random > 0.5)
		{
			good = 0;
			while(good == 0)
			{
				selectedgrain1 = int(rg.Random()*numgrains)+1;
				if(selectedgrain1 == 0) selectedgrain1 = 1;
				if(selectedgrain1 == numgrains+1) selectedgrain1 = numgrains;
				selectedgrain2 = int(rg.Random()*numgrains)+1;
				if(selectedgrain2 == 0) selectedgrain2 = 1;
				if(selectedgrain2 == numgrains+1) selectedgrain2 = numgrains;
				if(grains[selectedgrain1].surfacegrain == 0 && grains[selectedgrain2].surfacegrain == 0) good = 1;
			}
			double g1ea1 = grains[selectedgrain1].euler1;
			double g1ea2 = grains[selectedgrain1].euler2;
			double g1ea3 = grains[selectedgrain1].euler3;
			double g2ea1 = grains[selectedgrain2].euler1;
			double g2ea2 = grains[selectedgrain2].euler2;
			double g2ea3 = grains[selectedgrain2].euler3;
			int g1euler1bin = int(g1ea1/(5.0*m_pi/180.0));
			int g1euler2bin = int(g1ea2/(5.0*m_pi/180.0));
			int g1euler3bin = int(g1ea3/(5.0*m_pi/180.0));
			int g2euler1bin = int(g2ea1/(5.0*m_pi/180.0));
			int g2euler2bin = int(g2ea2/(5.0*m_pi/180.0));
			int g2euler3bin = int(g2ea3/(5.0*m_pi/180.0));
			if(crystruct == 1) g1odfbin = (g1euler3bin*36*36)+(g1euler2bin*36)+g1euler1bin, g2odfbin = (g2euler3bin*36*36)+(g2euler2bin*36)+g2euler1bin;
			if(crystruct == 2) g1odfbin = (g1euler3bin*18*18)+(g1euler2bin*18)+g1euler1bin, g2odfbin = (g2euler3bin*18*18)+(g2euler2bin*18)+g2euler1bin;
			double random = rg.Random();
			double odfchange = ((actualodf[g1odfbin].density - simodf[g1odfbin].density)*(actualodf[g1odfbin].density - simodf[g1odfbin].density)) - ((actualodf[g1odfbin].density - (simodf[g1odfbin].density-(double(grains[selectedgrain1].numvoxels)*resx*resy*resz/totalvol)+(double(grains[selectedgrain2].numvoxels)*resx*resy*resz/totalvol)))*(actualodf[g1odfbin].density - (simodf[g1odfbin].density-(double(grains[selectedgrain1].numvoxels)*resx*resy*resz/totalvol)+(double(grains[selectedgrain2].numvoxels)*resx*resy*resz/totalvol))));
			odfchange = odfchange + (((actualodf[g2odfbin].density - simodf[g2odfbin].density)*(actualodf[g2odfbin].density - simodf[g2odfbin].density)) - ((actualodf[g2odfbin].density - (simodf[g2odfbin].density-(double(grains[selectedgrain2].numvoxels)*resx*resy*resz/totalvol)+(double(grains[selectedgrain1].numvoxels)*resx*resy*resz/totalvol)))*(actualodf[g2odfbin].density - (simodf[g2odfbin].density-(double(grains[selectedgrain2].numvoxels)*resx*resy*resz/totalvol)+(double(grains[selectedgrain1].numvoxels)*resx*resy*resz/totalvol)))));
			vector<int>* nlist = grains[selectedgrain1].neighborlist;
			vector<double>* misolist = grains[selectedgrain1].misorientationlist;
			vector<double>* neighborsurfarealist = grains[selectedgrain1].neighborsurfarealist;
			double mdfchange = 0;
	        s=sin(0.5*g2ea2);
		    c=cos(0.5*g2ea2);
		    s1=sin(0.5*(g2ea1-g2ea3));
			c1=cos(0.5*(g2ea1-g2ea3));
			s2=sin(0.5*(g2ea1+g2ea3));
		    c2=cos(0.5*(g2ea1+g2ea3));
			q1[1] = s*c1;
			q1[2] = s*s1;
			q1[3] = c*s2;
			q1[4] = c*c2;
			for(size_t j=0;j<nlist->size();j++)
			{
				int neighbor = nlist->at(j);
				double curmiso = misolist->at(j);
				double neighsurfarea = neighborsurfarealist->at(j);
				int curmisobin = int(curmiso/5.0);
				q2[1] = grains[neighbor].avg_quat[1];
				q2[2] = grains[neighbor].avg_quat[2];
				q2[3] = grains[neighbor].avg_quat[3];
				q2[4] = grains[neighbor].avg_quat[4];
				double newmiso = 0;
	      if(crystruct == AIM::Reconstruction::Hexagonal){
	        newmiso = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
	      }
	      if(crystruct == AIM::Reconstruction::Cubic) {
	        newmiso = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
	      }

				int newmisobin = int(newmiso/5.0);
				mdfchange = mdfchange + (((actualmdf[curmisobin].density-simmdf[curmisobin].density)*(actualmdf[curmisobin].density-simmdf[curmisobin].density)) - ((actualmdf[curmisobin].density-(simmdf[curmisobin].density-(neighsurfarea/totalsurfacearea)))*(actualmdf[curmisobin].density-(simmdf[curmisobin].density-(neighsurfarea/totalsurfacearea)))));
				mdfchange = mdfchange + (((actualmdf[newmisobin].density-simmdf[newmisobin].density)*(actualmdf[newmisobin].density-simmdf[newmisobin].density)) - ((actualmdf[newmisobin].density-(simmdf[newmisobin].density+(neighsurfarea/totalsurfacearea)))*(actualmdf[newmisobin].density-(simmdf[newmisobin].density+(neighsurfarea/totalsurfacearea)))));
			}
			nlist = grains[selectedgrain2].neighborlist;
			misolist = grains[selectedgrain2].misorientationlist;
			neighborsurfarealist = grains[selectedgrain2].neighborsurfarealist;
	        s=sin(0.5*g1ea2);
		    c=cos(0.5*g1ea2);
		    s1=sin(0.5*(g1ea1-g1ea3));
			c1=cos(0.5*(g1ea1-g1ea3));
			s2=sin(0.5*(g1ea1+g1ea3));
		    c2=cos(0.5*(g1ea1+g1ea3));
			q1[1] = s*c1;
			q1[2] = s*s1;
			q1[3] = c*s2;
			q1[4] = c*c2;
			for(size_t j=0;j<nlist->size();j++)
			{
				int neighbor = nlist->at(j);
				double curmiso = misolist->at(j);
				double neighsurfarea = neighborsurfarealist->at(j);
				int curmisobin = int(curmiso/5.0);
				q2[1] = grains[neighbor].avg_quat[1];
				q2[2] = grains[neighbor].avg_quat[2];
				q2[3] = grains[neighbor].avg_quat[3];
				q2[4] = grains[neighbor].avg_quat[4];
				double newmiso = 0;
	      if(crystruct == AIM::Reconstruction::Hexagonal){
	        newmiso = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
	      }
	      if(crystruct == AIM::Reconstruction::Cubic) {
	        newmiso = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
	      }
				int newmisobin = int(newmiso/5.0);
				mdfchange = mdfchange + (((actualmdf[curmisobin].density-simmdf[curmisobin].density)*(actualmdf[curmisobin].density-simmdf[curmisobin].density)) - ((actualmdf[curmisobin].density-(simmdf[curmisobin].density-(neighsurfarea/totalsurfacearea)))*(actualmdf[curmisobin].density-(simmdf[curmisobin].density-(neighsurfarea/totalsurfacearea)))));
				mdfchange = mdfchange + (((actualmdf[newmisobin].density-simmdf[newmisobin].density)*(actualmdf[newmisobin].density-simmdf[newmisobin].density)) - ((actualmdf[newmisobin].density-(simmdf[newmisobin].density+(neighsurfarea/totalsurfacearea)))*(actualmdf[newmisobin].density-(simmdf[newmisobin].density+(neighsurfarea/totalsurfacearea)))));
			}
			deltaerror = 4.0*odfchange + 0.25*mdfchange;
			if(deltaerror > 0)
			{
				badtrycount = 0;
				grains[selectedgrain1].euler1 = g2ea1;
				grains[selectedgrain1].euler2 = g2ea2;
				grains[selectedgrain1].euler3 = g2ea3;
				grains[selectedgrain2].euler1 = g1ea1;
				grains[selectedgrain2].euler2 = g1ea2;
				grains[selectedgrain2].euler3 = g1ea3;
				simodf[g1odfbin].density = simodf[g1odfbin].density + (double(grains[selectedgrain2].numvoxels)*resx*resy*resz/totalvol) - (double(grains[selectedgrain1].numvoxels)*resx*resy*resz/totalvol);
				simodf[g2odfbin].density = simodf[g2odfbin].density + (double(grains[selectedgrain1].numvoxels)*resx*resy*resz/totalvol) - (double(grains[selectedgrain2].numvoxels)*resx*resy*resz/totalvol);
				nlist = grains[selectedgrain1].neighborlist;
				misolist = grains[selectedgrain1].misorientationlist;
				neighborsurfarealist = grains[selectedgrain1].neighborsurfarealist;
		        s=sin(0.5*g2ea2);
			    c=cos(0.5*g2ea2);
			    s1=sin(0.5*(g2ea1-g2ea3));
				c1=cos(0.5*(g2ea1-g2ea3));
				s2=sin(0.5*(g2ea1+g2ea3));
			    c2=cos(0.5*(g2ea1+g2ea3));
				q1[1] = s*c1;
				q1[2] = s*s1;
				q1[3] = c*s2;
				q1[4] = c*c2;
				for(size_t j=0;j<nlist->size();j++)
				{
					int neighbor = nlist->at(j);
					double curmiso = misolist->at(j);
					double neighsurfarea = neighborsurfarealist->at(j);
					int curmisobin = int(curmiso/5.0);
					q2[1] = grains[neighbor].avg_quat[1];
					q2[2] = grains[neighbor].avg_quat[2];
					q2[3] = grains[neighbor].avg_quat[3];
					q2[4] = grains[neighbor].avg_quat[4];
					double newmiso = 0;
		      if(crystruct == AIM::Reconstruction::Hexagonal){
		        newmiso = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
		      }
		      if(crystruct == AIM::Reconstruction::Cubic) {
		        newmiso = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
		      }

					int newmisobin = int(newmiso/5.0);
					misolist->at(j) = newmiso;
					simmdf[curmisobin].density = simmdf[curmisobin].density - (neighsurfarea/totalsurfacearea);
					simmdf[newmisobin].density = simmdf[newmisobin].density + (neighsurfarea/totalsurfacearea);
				}
				nlist = grains[selectedgrain2].neighborlist;
				misolist = grains[selectedgrain2].misorientationlist;
				neighborsurfarealist = grains[selectedgrain2].neighborsurfarealist;
		        s=sin(0.5*g1ea2);
			    c=cos(0.5*g1ea2);
			    s1=sin(0.5*(g1ea1-g1ea3));
				c1=cos(0.5*(g1ea1-g1ea3));
				s2=sin(0.5*(g1ea1+g1ea3));
			    c2=cos(0.5*(g1ea1+g1ea3));
				q1[1] = s*c1;
				q1[2] = s*s1;
				q1[3] = c*s2;
				q1[4] = c*c2;
				for(size_t j=0;j<nlist->size();j++)
				{
					int neighbor = nlist->at(j);
					double curmiso = misolist->at(j);
					double neighsurfarea = neighborsurfarealist->at(j);
					int curmisobin = int(curmiso/5.0);
					q2[1] = grains[neighbor].avg_quat[1];
					q2[2] = grains[neighbor].avg_quat[2];
					q2[3] = grains[neighbor].avg_quat[3];
					q2[4] = grains[neighbor].avg_quat[4];
					double newmiso = 0;
		      if(crystruct == AIM::Reconstruction::Hexagonal){
		        newmiso = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
		      }
		      if(crystruct == AIM::Reconstruction::Cubic) {
		        newmiso = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
		      }
					int newmisobin = int(newmiso/5.0);
					misolist->at(j) = newmiso;
					simmdf[curmisobin].density = simmdf[curmisobin].density - (neighsurfarea/totalsurfacearea);
					simmdf[newmisobin].density = simmdf[newmisobin].density + (neighsurfarea/totalsurfacearea);
				}
//				currentodferror = currentodferror - odfchange;
//				currentmdferror = currentmdferror - mdfchange;
			}
		}
	}
	outFile.close();
}
void  GrainGeneratorFunc::measure_misorientations ()
{
  vector<double > misolist;
  double w;
  double n1;
  double n2;
  double n3;
  double q1[5];
  double q2[5];
  for (int i = 1; i < numgrains + 1; i++)
  {
    vector<int >* nlist = grains[i].neighborlist;
    vector<double >* neighsurfarealist = grains[i].neighborsurfarealist;
    misolist.resize(nlist->size());
    q1[1] = grains[i].avg_quat[1];
    q1[2] = grains[i].avg_quat[2];
    q1[3] = grains[i].avg_quat[3];
    q1[4] = grains[i].avg_quat[4];
    int size = 0;
    if (NULL != nlist)
    {
      size = nlist->size();
    }
    for (int j = 0; j < size; j++)
    {
      int nname = nlist->at(j);
      double neighsurfarea = neighsurfarealist->at(j);
      q2[1] = grains[nname].avg_quat[1];
      q2[2] = grains[nname].avg_quat[2];
      q2[3] = grains[nname].avg_quat[3];
      q2[4] = grains[nname].avg_quat[4];
      if(crystruct == AIM::Reconstruction::Hexagonal){
        w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
      }
      if(crystruct == AIM::Reconstruction::Cubic) {
        w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
      }
      misolist[j] = w;
      int misobin = int(w / 5.0);
      if (grains[i].surfacegrain == 0 && (nname > i || grains[nname].surfacegrain == 1))
      {
        simmdf[misobin].density = simmdf[misobin].density + (neighsurfarea / totalsurfacearea);
      }
    }
    grains[i].misorientationlist = new std::vector<double >(misolist.size());
    grains[i].misorientationlist->swap(misolist);
    misolist.clear();
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
    int name = voxels[i].grainname;
	if(i%20 == 0 && i > 0) outFile << endl;
    outFile << "   ";
	if(name < 10000) outFile << " ";
	if(name < 1000) outFile << " ";
	if(name < 100) outFile << " ";
	if(name < 10) outFile << " ";
	outFile << name;
  }
  outFile << endl;
  outFile << "SCALARS SurfaceVoxel int  1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
	int name = voxels[i].surfacevoxel;
	if(i%20 == 0 && i > 0) outFile << endl;
    outFile << "   ";
	if(name < 10000) outFile << " ";
	if(name < 1000) outFile << " ";
	if(name < 100) outFile << " ";
	if(name < 10) outFile << " ";
	outFile << name;
  }
  outFile << endl;
  outFile << "SCALARS PhaseID int 1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
//	int name = voxels[i].grainname;
	if(i%20 == 0 && i > 0) outFile << endl;
//    if(name <= numgrains) outFile << "       1";
//    if(name > numgrains) outFile << "       2";
	outFile << voxels[i].unassigned << "	";
  }
  outFile.close();
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

void GrainGeneratorFunc::write_graindata(string gdata, string MoDFFile)
{
  double misobin[36];
  double microbin[10];
  for(int e = 0; e < 36; e++)
  {
    misobin[e] = 0;
	if(e < 10) microbin[e] = 0;
  }
  ofstream outFile;
  outFile.open(gdata.c_str());
  outFile << numgrains << endl;
  for(int i = 1; i < numgrains; i++)
  {
	double volume = grains[i].numvoxels*resx*resy*resz;
	double equivdiam = grains[i].equivdiameter;
	double diam = 2*pow(((0.75)*(1.0/m_pi)*volume),0.3333333333);
	int onsurface = grains[i].surfacegrain;
	double ea1 = grains[i].euler1;
	double ea2 = grains[i].euler2;
	double ea3 = grains[i].euler3;
	int numneighbors = grains[i].numneighbors;
	outFile << i << "	" << diam << "	" << equivdiam << "	" << numneighbors << "	" << onsurface << "	" << ea1 << "	" << ea2 << "	" << ea3 << endl;
  }
  outFile.close();
  outFile.open(MoDFFile.c_str());
  for(int l = 1; l < numgrains; l++)
  {
	if(grains[l].surfacegrain == 0)
    {
      double microcount = 0;
	  vector<int>* nlist = grains[l].neighborlist;
      vector<double>* misolist = grains[l].misorientationlist;
	  vector<double>* neighborsurfarealist = grains[l].neighborsurfarealist;
      int size = int(misolist->size());
      for(int k=0;k<size;k++)
      {
        int neigh = nlist->at(k);
        double firstmiso = misolist->at(k);
        double firstnsa = neighborsurfarealist->at(k);
        int misocur = int(firstmiso/5.0);
        if(misocur == 36) misocur = 35;
		if(neigh > l || grains[neigh].surfacegrain == 1)
		{
	        misobin[misocur] = misobin[misocur] + (firstnsa/totalsurfacearea);
		}
        if(firstmiso < 15) microcount++;
      }
      if (size != 0 )
	  {
        microcount = microcount/size;
      }
      else
      {
        microcount = 0;
      }
      int microcur = int(microcount/0.1);
      if(microcur == 10) microcur = 9;
      microbin[microcur]++;
    }
  }
  for(int i = 0; i < 36; i++)
  {
    outFile << misobin[i] << endl;
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


double GrainGeneratorFunc::gammastirf(double x)
{
    double result;
    double y;
    double w;
    double v;
    double stir;

    w = 1/x;
    stir = 7.87311395793093628397E-4;
    stir = -2.29549961613378126380E-4+w*stir;
    stir = -2.68132617805781232825E-3+w*stir;
    stir = 3.47222221605458667310E-3+w*stir;
    stir = 8.33333333333482257126E-2+w*stir;
    w = 1+w*stir;
    y = exp(x);
    if(x > 143.01608)
    {
        v = pow(x, 0.5*x-0.25);
        y = v*(v/y);
    }
    else
    {
        y = pow(x, x-0.5)/y;
    }
    result = 2.50662827463100050242*y*w;
    return result;
}
double GrainGeneratorFunc::lngamma(double x, double& sgngam)
{
    double result;
    double a;
    double b;
    double c;
    double p;
    double q;
    double u;
    double w;
    double z;
    int i;
    double logpi;
    double ls2pi;
    double tmp;

    sgngam = 1;
    logpi = 1.14472988584940017414;
    ls2pi = 0.91893853320467274178;
    if(x < -34.0)
    {
        q = -x;
        w = lngamma(q, tmp);
        p = int(floor(q));
        i = int(floor(p+0.5));
        if( i%2==0 )
        {
            sgngam = -1;
        }
        else
        {
            sgngam = 1;
        }
        z = q-p;
        if(z > 0.5)
        {
            p = p+1;
            z = p-q;
        }
        z = q*sin(m_pi*z);
        result = logpi-log(z)-w;
        return result;
    }
    if(x < 13)
    {
        z = 1;
        p = 0;
        u = x;
        while(u > 3)
        {
            p = p-1;
            u = x+p;
            z = z*u;
        }
        while(u < 2)
        {
            z = z/u;
            p = p+1;
            u = x+p;
        }
        if(z <0)
        {
            sgngam = -1;
            z = -z;
        }
        else
        {
            sgngam = 1;
        }
        if(u == 2)
        {
            result = log(z);
            return result;
        }
        p = p-2;
        x = x+p;
        b = -1378.25152569120859100;
        b = -38801.6315134637840924+x*b;
        b = -331612.992738871184744+x*b;
        b = -1162370.97492762307383+x*b;
        b = -1721737.00820839662146+x*b;
        b = -853555.664245765465627+x*b;
        c = 1;
        c = -351.815701436523470549+x*c;
        c = -17064.2106651881159223+x*c;
        c = -220528.590553854454839+x*c;
        c = -1139334.44367982507207+x*c;
        c = -2532523.07177582951285+x*c;
        c = -2018891.41433532773231+x*c;
        p = x*b/c;
        result = log(z)+p;
        return result;
    }
    q = (x-0.5)*log(x)-x+ls2pi;
    if(x >= 100000000)
    {
        result = q;
        return result;
    }
    p = 1/(x*x);
    if(x >= 1000.0)
    {
        q = q+((7.9365079365079365079365*0.0001*p-2.7777777777777777777778*0.001)*p+0.0833333333333333333333)/x;
    }
    else
    {
        a = 8.11614167470508450300*0.0001;
        a = -5.95061904284301438324*0.0001+p*a;
        a = 7.93650340457716943945*0.0001+p*a;
        a = -2.77777777730099687205*0.001+p*a;
        a = 8.33333333333331927722*0.01+p*a;
        q = q+a/x;
    }
    result = q;
    return result;
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
double GrainGeneratorFunc::erf(double x)
{
    double result;
    double xsq;
    double s;
    double p;
    double q;


    s = 1;
	if(x < 0) s = -1;
    x = fabs(x);
    if(x < 0.5)
    {
        xsq = x*x;
        p = 0.007547728033418631287834;
        p = 0.288805137207594084924010+xsq*p;
        p = 14.3383842191748205576712+xsq*p;
        p = 38.0140318123903008244444+xsq*p;
        p = 3017.82788536507577809226+xsq*p;
        p = 7404.07142710151470082064+xsq*p;
        p = 80437.3630960840172832162+xsq*p;
        q = 0.0;
        q = 1.00000000000000000000000+xsq*q;
        q = 38.0190713951939403753468+xsq*q;
        q = 658.070155459240506326937+xsq*q;
        q = 6379.60017324428279487120+xsq*q;
        q = 34216.5257924628539769006+xsq*q;
        q = 80437.3630960840172826266+xsq*q;
        result = s*1.1283791670955125738961589031*x*p/q;
        return result;
    }
    if(x >= 10)
    {
        result = s;
        return result;
    }
    result = s*(1-erfc(x));
    return result;
}
double GrainGeneratorFunc::erfc(double x)
{
    double result;
    double p;
    double q;

    if(x < 0)
    {
        result = 2-erfc(-x);
        return result;
    }
    if(x < 0.5)
    {
        result = 1.0-erf(x);
        return result;
    }
    if(x >= 10)
    {
        result = 0;
        return result;
    }
    p = 0.0;
    p = 0.5641877825507397413087057563+x*p;
    p = 9.675807882987265400604202961+x*p;
    p = 77.08161730368428609781633646+x*p;
    p = 368.5196154710010637133875746+x*p;
    p = 1143.262070703886173606073338+x*p;
    p = 2320.439590251635247384768711+x*p;
    p = 2898.0293292167655611275846+x*p;
    p = 1826.3348842295112592168999+x*p;
    q = 1.0;
    q = 17.14980943627607849376131193+x*q;
    q = 137.1255960500622202878443578+x*q;
    q = 661.7361207107653469211984771+x*q;
    q = 2094.384367789539593790281779+x*q;
    q = 4429.612803883682726711528526+x*q;
    q = 6089.5424232724435504633068+x*q;
    q = 4958.82756472114071495438422+x*q;
    q = 1826.3348842295112595576438+x*q;
    result = exp(-(x*x))*p/q;
    return result;
}
double GrainGeneratorFunc::incompletebeta(double a, double b, double x)
{
	machineepsilon = 5E-16;
	maxrealnumber = 1E300;
	minrealnumber = 1E-300;
    double result;
    double t;
    double xc;
    double w;
    double y;
    int flag;
    double sg;
    double big;
    double biginv;
    double maxgam;
    double minlog;
    double maxlog;

    big = 4.503599627370496e15;
    biginv = 2.22044604925031308085e-16;
    maxgam = 171.624376956302725;
    minlog = log(minrealnumber);
    maxlog = log(maxrealnumber);
    if(x == 0)
    {
        result = 0;
        return result;
    }
    if(x == 1)
    {
        result = 1;
        return result;
    }
    flag = 0;
    if((b*x) <= 1.0 && x <= 0.95)
    {
        result = incompletebetaps(a, b, x, maxgam);
        return result;
    }
    w = 1.0-x;
    if(x > (a/(a+b)))
    {
        flag = 1;
        t = a;
        a = b;
        b = t;
        xc = x;
        x = w;
    }
    else
    {
        xc = w;
    }
    if(flag == 1 && (b*x) <= 1.0 && x <=0.95)
    {
        t = incompletebetaps(a, b, x, maxgam);
        if(t <= machineepsilon)
        {
            result = 1.0-machineepsilon;
        }
        else
        {
            result = 1.0-t;
        }
        return result;
    }
    y = x*(a+b-2.0)-(a-1.0);
    if(y < 0.0)
    {
        w = incompletebetafe(a, b, x, big, biginv);
    }
    else
    {
        w = incompletebetafe2(a, b, x, big, biginv)/xc;
    }
    y = a*log(x);
    t = b*log(xc);
    if((a+b) < maxgam && fabs(y) < maxlog && fabs(t) < maxlog)
    {
        t = pow(xc, b);
        t = t*pow(x, a);
        t = t/a;
        t = t*w;
        t = t*(gamma(a+b)/(gamma(a)*gamma(b)));
        if( flag==1 )
        {
            if(t <= machineepsilon)
            {
                result = 1.0-machineepsilon;
            }
            else
            {
                result = 1.0-t;
            }
        }
        else
        {
            result = t;
        }
        return result;
    }
    y = y+t+lngamma(a+b, sg)-lngamma(a, sg)-lngamma(b, sg);
    y = y+log(w/a);
    if(y < minlog)
    {
        t = 0.0;
    }
    else
    {
        t = exp(y);
    }
    if(flag == 1)
    {
        if(t <= machineepsilon)
        {
            t = 1.0-machineepsilon;
        }
        else
        {
            t = 1.0-t;
        }
    }
    result = t;
    return result;
}
double GrainGeneratorFunc::incompletebetafe(double a, double b, double x, double big, double biginv)
{
    double result;
    double xk;
    double pk;
    double pkm1;
    double pkm2;
    double qk;
    double qkm1;
    double qkm2;
    double k1;
    double k2;
    double k3;
    double k4;
    double k5;
    double k6;
    double k7;
    double k8;
    double r;
    double t;
    double ans;
    double thresh;
    int n;

    k1 = a;
    k2 = a+b;
    k3 = a;
    k4 = a+1.0;
    k5 = 1.0;
    k6 = b-1.0;
    k7 = k4;
    k8 = a+2.0;
    pkm2 = 0.0;
    qkm2 = 1.0;
    pkm1 = 1.0;
    qkm1 = 1.0;
    ans = 1.0;
    r = 1.0;
    n = 0;
    thresh = 3.0*machineepsilon;
    do
    {
        xk = -x*k1*k2/(k3*k4);
        pk = pkm1+pkm2*xk;
        qk = qkm1+qkm2*xk;
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        xk = x*k5*k6/(k7*k8);
        pk = pkm1+pkm2*xk;
        qk = qkm1+qkm2*xk;
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        if(qk != 0)
        {
            r = pk/qk;
        }
        if(r != 0)
        {
            t = fabs((ans-r)/r);
            ans = r;
        }
        else
        {
            t = 1.0;
        }
        if(t < thresh)
        {
            break;
        }
        k1 = k1+1.0;
        k2 = k2+1.0;
        k3 = k3+2.0;
        k4 = k4+2.0;
        k5 = k5+1.0;
        k6 = k6-1.0;
        k7 = k7+2.0;
        k8 = k8+2.0;
        if((fabs(qk)+fabs(pk)) > big)
        {
            pkm2 = pkm2*biginv;
            pkm1 = pkm1*biginv;
            qkm2 = qkm2*biginv;
            qkm1 = qkm1*biginv;
        }
        if(fabs(qk) < biginv || fabs(pk) < biginv)
        {
            pkm2 = pkm2*big;
            pkm1 = pkm1*big;
            qkm2 = qkm2*big;
            qkm1 = qkm1*big;
        }
        n = n+1;
    }
    while(n!=300);
    result = ans;
    return result;
}
double GrainGeneratorFunc::incompletebetafe2(double a, double b, double x, double big, double biginv)
{
    double result;
    double xk;
    double pk;
    double pkm1;
    double pkm2;
    double qk;
    double qkm1;
    double qkm2;
    double k1;
    double k2;
    double k3;
    double k4;
    double k5;
    double k6;
    double k7;
    double k8;
    double r;
    double t;
    double ans;
    double z;
    double thresh;
    int n;

    k1 = a;
    k2 = b-1.0;
    k3 = a;
    k4 = a+1.0;
    k5 = 1.0;
    k6 = a+b;
    k7 = a+1.0;
    k8 = a+2.0;
    pkm2 = 0.0;
    qkm2 = 1.0;
    pkm1 = 1.0;
    qkm1 = 1.0;
    z = x/(1.0-x);
    ans = 1.0;
    r = 1.0;
    n = 0;
    thresh = 3.0*machineepsilon;
    do
    {
        xk = -z*k1*k2/(k3*k4);
        pk = pkm1+pkm2*xk;
        qk = qkm1+qkm2*xk;
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        xk = z*k5*k6/(k7*k8);
        pk = pkm1+pkm2*xk;
        qk = qkm1+qkm2*xk;
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        if(qk != 0)
        {
            r = pk/qk;
        }
        if(r != 0)
        {
            t = fabs((ans-r)/r);
            ans = r;
        }
        else
        {
            t = 1.0;
        }
        if(t < thresh)
        {
            break;
        }
        k1 = k1+1.0;
        k2 = k2-1.0;
        k3 = k3+2.0;
        k4 = k4+2.0;
        k5 = k5+1.0;
        k6 = k6+1.0;
        k7 = k7+2.0;
        k8 = k8+2.0;
        if((fabs(qk)+fabs(pk)) > big)
        {
            pkm2 = pkm2*biginv;
            pkm1 = pkm1*biginv;
            qkm2 = qkm2*biginv;
            qkm1 = qkm1*biginv;
        }
        if(fabs(qk) < biginv || fabs(pk) < biginv)
        {
            pkm2 = pkm2*big;
            pkm1 = pkm1*big;
            qkm2 = qkm2*big;
            qkm1 = qkm1*big;
        }
        n = n+1;
    }
    while(n!=300);
    result = ans;
    return result;
}
double GrainGeneratorFunc::incompletebetaps(double a, double b, double x, double maxgam)
{
	double result;
    double s;
    double t;
    double u;
    double v;
    double n;
    double t1;
    double z;
    double ai;
    double sg;

    ai = 1.0/a;
    u = (1.0-b)*x;
    v = u/(a+1.0);
    t1 = v;
    t = u;
    n = 2.0;
    s = 0.0;
    z = machineepsilon*ai;
    while(fabs(v) > z)
    {
        u = (n-b)*x/n;
        t = t*u;
        v = t/(a+n);
        s = s+v;
        n = n+1.0;
    }
    s = s+t1;
    s = s+ai;
    u = a*log(x);
    if((a+b) < maxgam && fabs(u) < log(maxrealnumber))
    {
        t = gamma(a+b)/(gamma(a)*gamma(b));
        s = s*t*pow(x, a);
    }
    else
    {
        t = lngamma(a+b, sg)-lngamma(a, sg)-lngamma(b, sg)+u+log(s);
        if(t < log(minrealnumber))
        {
            s = 0.0;
        }
        else
        {
            s = exp(t);
        }
    }
    result = s;
    return result;
}

