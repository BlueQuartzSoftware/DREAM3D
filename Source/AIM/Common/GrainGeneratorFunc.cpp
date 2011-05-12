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

// C Includes
#include <string.h>

// C++ Includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cmath>
#include <limits>

// AIM Includes
#include "AIM/Common/AIMMath.h"


const static double m_onepointthree = 1.33333333333;
const static double m_pi = M_PI;
const static double SinOfHalf = sin(0.5);
const static double CosOfHalf = cos(0.5);
const static double SinOfZero = sin(0.0);
const static double CosOfZero = cos(0.0);

#define DIMS "DIMENSIONS"
#define LOOKUP "LOOKUP_TABLE"

#if 0
// -i C:\Users\GroebeMA\Desktop\NewFolder --outputDir C:\Users\GroebeMA\Desktop\NewFolder -f Slice_ --angMaxSlice 400 -s 1 -e 30 -z 0.25 -t -g 10 -c 0.1 -o 5.0 -x 2
#endif



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

using namespace std;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorFunc::GrainGeneratorFunc() :
actualodf(NULL),
simodf(NULL),
axisodf(NULL),
voxels(NULL),
actualmdf(NULL),
simmdf(NULL),
actualmicrotex(NULL),
simmicrotex(NULL),
m_Grains(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientatioOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientatioOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientatioOps.push_back(m_OrthoOps.get());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorFunc::~GrainGeneratorFunc()
{

  m_Grains.clear();
  precipitates.clear();
}

#define GG_INIT_DOUBLE_ARRAY(array, value, size)\
    for(size_t n = 0; n < size; ++n) { array[n] = (value); }


void GrainGeneratorFunc::initialize(int32_t m_NumGrains, int32_t m_ShapeClass,
                                    double m_XResolution, double m_YResolution, double m_ZResolution,
        double m_fillingerrorweight, double m_neighborhooderrorweight, double m_sizedisterrorweight,
        int32_t m_Precipitates, double m_FractionPrecipitates,std::vector<AIM::Reconstruction::CrystalStructure> m_CrystalStructure)
{
  numgrains = m_NumGrains;
  shapeclass = m_ShapeClass;
  resx = m_XResolution;
  resy = m_YResolution;
  resz = m_ZResolution;
  fillingerrorweight = m_fillingerrorweight;
  neighborhooderrorweight = m_neighborhooderrorweight;
  sizedisterrorweight = m_sizedisterrorweight;
  crystruct = m_CrystalStructure;

  initializeArrays();
}

void GrainGeneratorFunc::initializeArrays()
{
  //------------------
  resdiff = 1;
  numextragrains = (25*numgrains);
  m_Grains.resize(numextragrains+1);
  for(size_t g = 0; g < m_Grains.size(); ++g)
  {
    m_Grains[g] = Grain::New();
  }
  size_t nElements = 0;
  size_t xtalSize = crystruct.size();
  //actualodf = new double *[xtalSize];
  actualodf.resize(xtalSize);

  simodf.resize(xtalSize);
  actualmdf.resize(xtalSize);
  simmdf.resize(xtalSize);
  axisodf.resize(xtalSize);
  actualmicrotex.resize(xtalSize);
  simmicrotex.resize(xtalSize);
  for(size_t i= 0; i < xtalSize; ++i)
  {
    if(crystruct[i] == AIM::Reconstruction::Hexagonal) nElements = 36*36*12;
    if(crystruct[i] == AIM::Reconstruction::Cubic) nElements = 18*18*18;

    double initValue = 1.0/(double)(nElements);
    actualodf[i] = DoubleArray(new double [nElements]);
    GG_INIT_DOUBLE_ARRAY(actualodf[i], initValue, nElements);

    simodf[i] = DoubleArray(new double [nElements]);
    GG_INIT_DOUBLE_ARRAY(simodf[i], 0.0, nElements);
    actualmdf[i] = DoubleArray(new double [nElements]);
    GG_INIT_DOUBLE_ARRAY(actualmdf[i], initValue, nElements);
    simmdf[i] = DoubleArray(new double [nElements]);
    GG_INIT_DOUBLE_ARRAY(simmdf[i], 0.0, nElements);

    nElements = 36*36*36;
    initValue = (1.0/double(nElements));
    axisodf[i] = DoubleArray(new double [nElements]);
    GG_INIT_DOUBLE_ARRAY(axisodf[i], initValue, nElements);
    nElements = 10;
    initValue = (1.0/double(nElements));
    actualmicrotex[i] = DoubleArray(new double [nElements]);
    GG_INIT_DOUBLE_ARRAY(actualmicrotex[i], initValue, nElements);
    simmicrotex[i] = DoubleArray(new double [nElements]);
    GG_INIT_DOUBLE_ARRAY(simmicrotex[i], 0.0, nElements);
  }
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

#define CHECK_STATS_READ_ERROR(err, group, dataset)\
if (err < 0) {\
  std::cout << "GrainGeneratorFunc::readReconStatsData Error: Could not read '" << group << "' data set '" << dataset << "'" << std::endl;\
  std::cout << "  File: " << __FILE__ << std::endl;\
  std::cout << "  Line: " << __LINE__ << std::endl;\
return err; }

#define READ_2_COLUMN_STATS_DATA(err, index, phase, group, var, distribution, Col0Hdr, Col1Hdr, ColCount)\
{\
  disType = h5io->getDistributionType(phase, group, dt);\
  var[index].resize(numdiameterbins[index]);\
  std::vector<double> col0;\
  std::vector<double> col1;\
  switch(dt)\
  {\
    case distribution:\
      path = group + ("/") + Col0Hdr;\
      err = h5io->readStatsDataset(phase, path, col0);\
      CHECK_STATS_READ_ERROR(err, group, Col0Hdr)\
      path = group + ("/") + Col1Hdr;\
      err = h5io->readStatsDataset(phase, path, col1);\
      CHECK_STATS_READ_ERROR(err, group, Col1Hdr)\
      for (size_t temp7 = 0; temp7 < nBins; temp7++)\
      {\
        var[index][temp7].resize(ColCount);\
        var[index][temp7][0] = col0[temp7];\
        var[index][temp7][1] = col1[temp7];\
      }\
      break;\
    default:\
      std::cout << "Error Reading " << group <<\
                " the distribution must be of type '" << distribution << "' but is of type '"\
                << disType << "'" << std::endl;\
      return -1;\
      break;\
  }\
}

#define READ_3_COLUMN_STATS_DATA(err, index, phase, group, var, distribution, Col0Hdr, Col1Hdr, Col2Hdr, ColCount)\
{\
  disType = h5io->getDistributionType(phase, group, dt);\
  var[index].resize(numdiameterbins[index]);\
  std::vector<double> col0;\
  std::vector<double> col1;\
  std::vector<double> col2;\
  switch(dt)\
  {\
    case distribution:\
      path = group + ("/") + Col0Hdr;\
      err = h5io->readStatsDataset(phase, path, col0);\
      CHECK_STATS_READ_ERROR(err, group, Col0Hdr)\
      path = group + ("/") + Col1Hdr;\
      err = h5io->readStatsDataset(phase, path, col1);\
      CHECK_STATS_READ_ERROR(err, group, Col1Hdr)\
      path = group + ("/") + Col2Hdr;\
      err = h5io->readStatsDataset(phase, path, col2);\
      CHECK_STATS_READ_ERROR(err, group, Col2Hdr)\
      for (size_t temp7 = 0; temp7 < nBins; temp7++)\
      {\
        var[index][temp7].resize(ColCount);\
        var[index][temp7][0] = col0[temp7];\
        var[index][temp7][1] = col1[temp7];\
        var[index][temp7][2] = col2[temp7];\
      }\
      break;\
  default:\
    std::cout << "Error Reading " << group <<\
              " the distribution must be of type '" << distribution << "' but is of type '"\
              << disType << "'" << std::endl;\
    return -1;\
    break;\
  }\
}

int GrainGeneratorFunc::readReconStatsData(H5ReconStatsReader::Pointer h5io)
{
  int err = -1;
  std::vector<double> grainDiamInfo;
  std::vector<double> double_data;
  std::string path;

  // Read the Phase and Crystal Structure information from the Stats File
  std::vector<int> phases;
  err = h5io->getPhaseAndCrystalStructures(phases, crystruct);
  if (err < 0)
  {
    return err;
  }

  // Now that we have that information - finish initializing the arrays
  initializeArrays();

  // Initialize some more arrays
  size_t size = crystruct.size();
  phasefraction.resize(size);
  mindiameter.resize(size);
  maxdiameter.resize(size);
  binstepsize.resize(size);
  numdiameterbins.resize(size);
  avgdiam.resize(size);
  sddiam.resize(size);
  grainsizediststep.resize(size);
  grainsizedist.resize(size);
  simgrainsizedist.resize(size);
  bovera.resize(size);
  covera.resize(size);
  coverb.resize(size);
  omega3.resize(size);
  neighborhood.resize(size);
  neighbordist.resize(size);

  int phase = -1;
  for (size_t i = 0; i < size; i++)
  {
    phase = phases[i];
	  /* Read the PhaseFraction Value*/
    std::vector<double> pFraction;
	  err = h5io->readStatsDataset(phase, AIM::HDF5::PhaseFraction, pFraction);
	  phasefraction[i] = pFraction.front();

	  /* Read the BinNumbers data set */
	  std::vector<double> bins;
	  err = h5io->readStatsDataset(phase, AIM::HDF5::BinNumber, bins);
	  CHECK_STATS_READ_ERROR(err, AIM::HDF5::Reconstruction, AIM::HDF5::BinNumber)
	  numdiameterbins[i] = bins.size();
	  size_t nBins = bins.size();

	  /* Read the Grain_Diameter_Info Data */
	  err = h5io->readStatsDataset(phase, AIM::HDF5::Grain_Diameter_Info, grainDiamInfo);
	  CHECK_STATS_READ_ERROR(err,  AIM::HDF5::Reconstruction, AIM::HDF5::Grain_Diameter_Info)

	  binstepsize[i] = grainDiamInfo[0];
	  maxdiameter[i]  = grainDiamInfo[1];
	  mindiameter[i] = grainDiamInfo[2];

	  /* Read the Grain_Size_Distribution Data */
	  err = h5io->readStatsDataset(phase, AIM::HDF5::Grain_Size_Distribution, double_data);
	  CHECK_STATS_READ_ERROR(err,  AIM::HDF5::Reconstruction, AIM::HDF5::Grain_Size_Distribution)
	  avgdiam[i] = double_data[0];
	  sddiam[i] = double_data[1];
	  grainsizediststep[i] = ((2*maxdiameter[i])-(mindiameter[i]/2.0))/20.0;
	  grainsizedist[i].resize(40);
	  simgrainsizedist[i].resize(40);
	  double root2pi = pow((2.0 * 3.1415926535897), 0.5);
	  double input = 0;
	  for (int j=0;j<40;j++)
	  {
		input = ((double(j)*grainsizediststep[i])+(grainsizediststep[i]/2.0))+(mindiameter[i]/2.0);
		grainsizedist[i][j] = (grainsizediststep[i]/(input*double_data[1]*root2pi))*exp(-((log(double(input))-double_data[0])*(log(double(input))-double_data[0]))/(2*double_data[1]*double_data[1]));
	  }

	  AIM::Reconstruction::DistributionType dt;
	  std::string disType;

	  /* Read the Shape Data */
	  READ_2_COLUMN_STATS_DATA(err, i, phase, AIM::HDF5::Grain_SizeVBoverA_Distributions, bovera, AIM::Reconstruction::Beta, AIM::HDF5::Alpha, AIM::HDF5::Beta, AIM::HDF5::BetaColumnCount);
	  READ_2_COLUMN_STATS_DATA(err, i, phase, AIM::HDF5::Grain_SizeVCoverA_Distributions, covera, AIM::Reconstruction::Beta, AIM::HDF5::Alpha, AIM::HDF5::Beta, AIM::HDF5::BetaColumnCount);
	  READ_2_COLUMN_STATS_DATA(err, i, phase, AIM::HDF5::Grain_SizeVCoverB_Distributions, coverb, AIM::Reconstruction::Beta, AIM::HDF5::Alpha, AIM::HDF5::Beta, AIM::HDF5::BetaColumnCount);

	  /* Read the Omega3 Data */
	  READ_2_COLUMN_STATS_DATA(err, i, phase, AIM::HDF5::Grain_SizeVOmega3_Distributions, omega3, AIM::Reconstruction::Beta, AIM::HDF5::Alpha, AIM::HDF5::Beta, AIM::HDF5::BetaColumnCount);

	  /* Read the Neighbor Data - This MUST be the last one because of how variables are assigned bvalues and used in the next section */
	  READ_3_COLUMN_STATS_DATA(err, i, phase, AIM::HDF5::Grain_SizeVNeighbors_Distributions, neighborhood, AIM::Reconstruction::Power, AIM::HDF5::Alpha, AIM::HDF5::Beta, AIM::HDF5::Exp_k, AIM::HDF5::PowerLawColumnCount);

	  /* Convert the data into the various "shell" data which is the data that is actually needed.
	   * The conversion is done "in place" by extracting out the alpha, beta, and K values then
	   * using those values to calculate the 3 values that get put back into the row.
	   */
	  disType = h5io->getDistributionType(phase, AIM::HDF5::Grain_SizeVNeighbors_Distributions, dt);
	  if(dt == AIM::Reconstruction::Power)
	  {
      double a, b, k;
      for (size_t temp7 = 0; temp7 < nBins; temp7++)
      {
        a = neighborhood[i][temp7][0];
        b = neighborhood[i][temp7][1];
        k = neighborhood[i][temp7][2];
        neighborhood[i][temp7][0] = a*pow(0,k)+b;
        neighborhood[i][temp7][1] = a*pow(1,k)+b;
        neighborhood[i][temp7][2] = a*pow(2,k)+b;
      }
	  }

	  neighbordist[i].resize(nBins);
	  for (size_t j=0;j<nBins;j++)
	  {
	    neighbordist[i][j].resize(3, 0.0);
	  }
  }

  return err;
}


#define GGF_CHECK_READ_ERROR(func, name)\
    std::cout << "GrainGeneratorFunc::" << #func << " Error: There was an error trying to read the data set '"\
      << name << "' from the HDF5 file." << std::endl;\
      std::cout << "  File: " << __FILE__ << std::endl;\
      std::cout << "  Line: " << __LINE__ << std::endl;\
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int  GrainGeneratorFunc::readAxisOrientationData(H5ReconStatsReader::Pointer h5io)
{
  std::vector<double> density;
  int err = 0;
  double totaldensity = 0;
  size_t size = 0;
  // Read the Phase and Crystal Structure information from the Stats File
  std::vector<int> phases;
  err = h5io->getPhaseAndCrystalStructures(phases, crystruct);
  if (err < 0)
  {
    return err;
  }
  int phase = -1;
  size_t count = crystruct.size();
  for(size_t i = 0; i< count ;i++)
  {
    phase = phases[i];
	  err = h5io->readStatsDataset(phase, AIM::HDF5::AxisOrientation, density);
	  if (err < 0)
	  {
		GGF_CHECK_READ_ERROR(readAxisOrientationData, AIM::HDF5::AxisOrientation)
		//FIXME: This should probably return an ERROR because nothing was read
		return 10;
	  }
	  size = 36 * 36 * 36;
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
		axisodf[i][k] = totaldensity;
	  }
  }
  return err;
}

int GrainGeneratorFunc::readODFData(H5ReconStatsReader::Pointer h5io)
{
  std::vector<double> density;
  int err = 0;
  // Read the Phase and Crystal Structure information from the Stats File
  std::vector<int> phases;
  err = h5io->getPhaseAndCrystalStructures(phases, crystruct);
  if (err < 0)
  {
    return err;
  }
  int phase = -1;
  size_t size = crystruct.size();
  for(size_t i = 0; i< size ;i++)
  {
    phase = phases[i];
	  err = h5io->readStatsDataset(phase, AIM::HDF5::ODF, density);
	  if (err < 0)
	  {
		GGF_CHECK_READ_ERROR(readODFData, AIM::HDF5::ODF)
		//FIXME: This should probably return an ERROR because nothing was read
		return 10;
	  }
	  size_t numbins = 0;
	  if(crystruct[i] == AIM::Reconstruction::Hexagonal) numbins = 36*36*12;
	  if(crystruct[i] == AIM::Reconstruction::Cubic) numbins = 18*18*18;

	  if (numbins != density.size() )
	  {
		std::cout << "GrainGeneratorFunc::readODFData Error: Mismatch in number of elements in the 'ODF' "
			 << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
			 << " elements and we need " << numbins << " Elements. "<< std::endl;
		return -1;
	  }
	  for (size_t j=0;j<numbins;j++)
	  {
		actualodf[i][j] = density[j];
	  }
  }
  return err;
}

int GrainGeneratorFunc::readMisorientationData(H5ReconStatsReader::Pointer h5io)
{
  std::vector<double> density;
  int err = 0;
  // Read the Phase and Crystal Structure information from the Stats File
  std::vector<int> phases;
  err = h5io->getPhaseAndCrystalStructures(phases, crystruct);
  if (err < 0)
  {
    return err;
  }
  int phase = -1;
  size_t size = crystruct.size();
  for(size_t i = 0; i< size ;i++)
  {
    phase = phases[i];
	  err = h5io->readStatsDataset(phase, AIM::HDF5::MisorientationBins, density);
	  if (err < 0)
	  {
		GGF_CHECK_READ_ERROR(readMisorientationData, AIM::HDF5::MisorientationBins)
	   //FIXME: This should probably return an ERROR because nothing was read
		return 10;
	  }
	  size_t numbins = 0;
	  if(crystruct[i] == AIM::Reconstruction::Hexagonal) numbins = 36*36*12;
	  if(crystruct[i] == AIM::Reconstruction::Cubic) numbins = 18*18*18;

	  if (numbins != density.size() )
	  {
		std::cout << "GrainGeneratorFunc::readMisorientationData Error: Mismatch in number of elements in the 'ODF' "
			 << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
			 << " elements and we need " << numbins << " Elements. "<< std::endl;
		return -1;
	  }

	  for (size_t k = 0; k < numbins; k++)
	  {
		actualmdf[i][k] = density[k];
	  }
	}
  return err;
}

int GrainGeneratorFunc::readMicroTextureData(H5ReconStatsReader::Pointer h5io)
{
  std::vector<double> density;
  int err = 0;
  // Read the Phase and Crystal Structure information from the Stats File
  std::vector<int> phases;
  err = h5io->getPhaseAndCrystalStructures(phases, crystruct);
  if (err < 0)
  {
    return err;
  }
  int phase = -1;
  size_t size = crystruct.size();
  for(size_t i = 0; i< size ;i++)
  {
    phase = phases[i];
	  err = h5io->readStatsDataset(phase, AIM::HDF5::MicroTextureBins, density);
	  if (err < 0)
	  {
		GGF_CHECK_READ_ERROR(readStatsDataset, AIM::HDF5::MicroTextureBins)
		//FIXME: This should probably return an ERROR because nothing was read
		return 10;
	  }
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
		actualmicrotex[i][k] = density[k];
	  }
  }
  return err;
}

void  GrainGeneratorFunc::generate_grain(int gnum, int phase)
{
  int good = 0;
  double r1 = 1;
  double u=0 ;
  double a1=0,a2=0,a3=0;
  double b1=0,b2=0,b3=0;
  double r2=0,r3=0;
  double diam = 0;
  double vol = 0;
  int volgood = 0;
  while(volgood == 0)
  {
	  volgood = 1;
	  u = rg.Random();
	  diam = rg.RandNorm(avgdiam[phase],sddiam[phase]);
	  diam = exp(diam);
	  if(diam >= maxdiameter[phase]) volgood = 0;
	  if(diam < mindiameter[phase]) volgood = 0;
	  vol = (4.0/3.0)*(m_pi)*((diam/2.0)*(diam/2.0)*(diam/2.0));
  }
  int diameter = int((diam-mindiameter[phase])/binstepsize[phase]);
  good = 0;
  while(good == 0)
  {
    a1 = bovera[phase][diameter][0];
    b1 = bovera[phase][diameter][1];
    if(a1 == 0)
    {
      a1 = bovera[phase][diameter-1][0];
      b1 = bovera[phase][diameter-1][1];
    }
    r2 = rg.RandBeta(a1,b1);
    a2 = covera[phase][diameter][0];
    b2 = covera[phase][diameter][1];
    if(a2 == 0)
    {
      a2 = covera[phase][diameter-1][0];
      b2 = covera[phase][diameter-1][1];
    }
    r3 = rg.RandBeta(a2,b2);
    double cob = r3/r2;
    a3 = coverb[phase][diameter][0];
    b3 = coverb[phase][diameter][1];
    if(a3 == 0)
    {
      a3 = coverb[phase][diameter-1][0];
      b3 = coverb[phase][diameter-1][1];
    }
    double prob = ((gamma((a3+b3))/(gamma(a3)*gamma(b3)))*(pow(cob,(a3-1)))*(pow((1-cob),(b3-1))));
    double check = rg.Random();
    if(prob > check) good = 1;
    if(cob > 1) good = 0;
  }
  double random = rg.Random();
  int bin=0;
  double totaldensity = 0;
  for(int i=0;i<(36*36*36);i++)
  {
	totaldensity = totaldensity + axisodf[phase][i];
    if(random > totaldensity) bin = i;
    if(random < totaldensity) {break;}
  }
  double phi1 = bin%36;
  double PHI = (bin/36)%36;
  double phi2 = bin/(36*36);
  random = rg.Random();
  phi1 = ((phi1*5)+(random*5))*(m_pi/180.0);
  random = rg.Random();
  PHI = ((PHI*5)+(random*5))*(m_pi/180.0);
  random = rg.Random();
  phi2 = ((phi2*5)+(random*5))*(m_pi/180.0);
  double m = omega3[phase][diameter][0];
  double s = omega3[phase][diameter][1];
  double omega3 = rg.RandBeta(m,s);
  m_Grains[gnum]->volume = vol;
  m_Grains[gnum]->equivdiameter = diam;
  m_Grains[gnum]->radius1 = r1;
  m_Grains[gnum]->radius2 = r2;
  m_Grains[gnum]->radius3 = r3;
  m_Grains[gnum]->axiseuler1 = phi1;
  m_Grains[gnum]->axiseuler2 = PHI;
  m_Grains[gnum]->axiseuler3 = phi2;
  m_Grains[gnum]->omega3 = omega3;
  m_Grains[gnum]->phase = phase;
  m_Grains[gnum]->neighbordistfunc[0] = 0;
  m_Grains[gnum]->neighbordistfunc[1] = 0;
  m_Grains[gnum]->neighbordistfunc[2] = 0;
}

void  GrainGeneratorFunc::insert_grain(size_t gnum)
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
  double ellipfunc = 0;
  double insidecount = 0;
  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
  std::vector<int> insidelist(1000,-1);
  std::vector<double> ellipfunclist(1000,-1);
  double volcur = m_Grains[gnum]->volume;
  double bovera = m_Grains[gnum]->radius2;
  double covera = m_Grains[gnum]->radius3;
  double omega3 = m_Grains[gnum]->omega3;
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
  double phi1 = m_Grains[gnum]->axiseuler1;
  double PHI = m_Grains[gnum]->axiseuler2;
  double phi2 = m_Grains[gnum]->axiseuler3;
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
  xc = m_Grains[gnum]->centroidx;
  yc = m_Grains[gnum]->centroidy;
  zc = m_Grains[gnum]->centroidz;
  column = (xc-(resx/2))/resx;
  row = (yc-(resy/2))/resy;
  plane = (zc-(resz/2))/resz;
  xmin = int(column-((radcur1/resx)+1));
  xmax = int(column+((radcur1/resx)+1));
  ymin = int(row-((radcur1/resy)+1));
  ymax = int(row+((radcur1/resy)+1));
  zmin = int(plane-((radcur1/resz)+1));
  zmax = int(plane+((radcur1/resz)+1));
  if(xmin < -0.5*xpoints) xmin = -0.5*xpoints;
  if(xmax > (1.5*xpoints-1)) xmax = (1.5*xpoints-1);
  if(ymin < -0.5*ypoints) ymin = -0.5*ypoints;
  if(ymax > (1.5*ypoints-1)) ymax = (1.5*ypoints-1);
  if(zmin < -0.5*zpoints) zmin = -0.5*zpoints;
  if(zmax > (1.5*zpoints-1)) zmax = (1.5*zpoints-1);
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
		  x = double(column)*resx;
		  y = double(row)*resy;
		  z = double(plane)*resz;
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
//				if(index >= totalpoints)
//				{
//				  int stop = 0;
//				}
				ellipfunc = (-0.1/((pow((axis1comp+axis2comp+axis3comp),1)*(1.0-(1.0/(0.90*0.90))))))*(1.0-(((axis1comp+axis2comp+axis3comp)*(axis1comp+axis2comp+axis3comp))/(0.90*0.90)));
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
  m_Grains[gnum]->voxellist = new std::vector<int>(insidecount);
  m_Grains[gnum]->ellipfunclist = new std::vector<double>(insidecount);
  m_Grains[gnum]->voxellist->swap(insidelist);
  m_Grains[gnum]->ellipfunclist->swap(ellipfunclist);
  m_Grains[gnum]->neighbordistfunclist.resize(3);
  insidelist.clear();
}

void  GrainGeneratorFunc::remove_grain(size_t gnum)
{
  int index;
  double ellipfunc;
//  int neigh;
  for(size_t i=0;i<m_Grains[gnum]->voxellist->size();i++)
  {
  index = m_Grains[gnum]->voxellist->at(i);
  ellipfunc = m_Grains[gnum]->ellipfunclist->at(i);
  voxels[index].grainlist->erase(std::remove(voxels[index].grainlist->begin(),voxels[index].grainlist->end(),gnum),voxels[index].grainlist->end());
  voxels[index].ellipfunclist->erase(std::remove(voxels[index].ellipfunclist->begin(),voxels[index].ellipfunclist->end(),ellipfunc),voxels[index].ellipfunclist->end());
  }
  for(int i=0;i<3;i++)
  {
    for(size_t j=0;j<m_Grains[gnum]->neighbordistfunclist[i].size();j++)
    {
    index = m_Grains[gnum]->neighbordistfunclist[i][j];
    m_Grains[index]->neighbordistfunc[i] = m_Grains[index]->neighbordistfunc[i]-1;
    }
  }
}

void  GrainGeneratorFunc::add_grain(size_t gnum)
{
  int index;
  double ellipfunc;
//  int neigh;
  for(size_t i=0;i<m_Grains[gnum]->voxellist->size();i++)
  {
	  index = m_Grains[gnum]->voxellist->at(i);
	  ellipfunc = m_Grains[gnum]->ellipfunclist->at(i);
	  voxels[index].grainlist->resize(voxels[index].grainlist->size()+1);
	  voxels[index].ellipfunclist->resize(voxels[index].ellipfunclist->size()+1);
	  voxels[index].grainlist->at(voxels[index].grainlist->size()-1) = gnum;
	  voxels[index].ellipfunclist->at(voxels[index].ellipfunclist->size()-1) = ellipfunc;
  }
  for(int i=0;i<3;i++)
  {
    for(size_t j=0;j<m_Grains[gnum]->neighbordistfunclist[i].size();j++)
    {
	    index = m_Grains[gnum]->neighbordistfunclist[i][j];
	    m_Grains[index]->neighbordistfunc[i]++;
    }
  }
}

void GrainGeneratorFunc::determine_neighbors()
{
  double x, y, z;
//  double rad1, rad2;

  double xn, yn, zn;
  double dia, dia2;
  int DoverR;
  double xdist, ydist, zdist, totdist;
  int nnum = 0;
  for(int gnum=1;gnum<(numextragrains+1);gnum++)
  {
    nnum = 0;
    x = m_Grains[gnum]->centroidx;
    y = m_Grains[gnum]->centroidy;
    z = m_Grains[gnum]->centroidz;
    dia = m_Grains[gnum]->equivdiameter;
    for(int n=gnum;n<(numextragrains+1);n++)
    {
      xn = m_Grains[n]->centroidx;
      yn = m_Grains[n]->centroidy;
      zn = m_Grains[n]->centroidz;
      dia2 = m_Grains[n]->equivdiameter;
      xdist = fabs(x-xn);
      ydist = fabs(y-yn);
      zdist = fabs(z-zn);
      totdist = (xdist*xdist)+(ydist*ydist)+(zdist*zdist);
      totdist = pow(totdist,0.5);
      if(totdist < (3*(dia/2.0)))
      {
        DoverR = int(totdist/(dia/2.0));
		for(int iter=DoverR;iter<3;iter++)
		{
	        m_Grains[n]->neighbordistfunclist[iter].push_back(gnum);
		}
      }
      if(totdist < (3*(dia2/2.0)))
      {
        DoverR = int(totdist/(dia2/2.0));
		for(int iter=DoverR;iter<3;iter++)
		{
	        m_Grains[gnum]->neighbordistfunclist[iter].push_back(n);
		}
      }
    }
  }
}

double GrainGeneratorFunc::check_neighborhooderror(int gadd, int gremove)
{
  double neighborerror = 0;
  double dia;
  int nnum;
  int index;
  int count = 0;
  int phase;
  size_t xtalSize = crystruct.size();
  for(std::vector<AIM::Reconstruction::CrystalStructure>::size_type iter = 0; iter < xtalSize;++iter)
  {
	  phase = iter+1;
	  for(int i=0;i<numdiameterbins[phase];i++)
	  {
		neighbordist[phase][i][0] = 0;
		neighbordist[phase][i][1] = 0;
		neighbordist[phase][i][2] = 0;
	  }
	  if(gadd > 0 && m_Grains[gadd]->phase == phase)
	  {
		for(int i=0;i<3;i++)
		{
		  for(size_t j=0;j<m_Grains[gadd]->neighbordistfunclist[i].size();j++)
		  {
			  index = m_Grains[gadd]->neighbordistfunclist[i][j];
			  m_Grains[index]->neighbordistfunc[i]++;
		  }
		}
	  }
	  if(gremove > 0 && m_Grains[gremove]->phase == phase)
	  {
		for(int i=0;i<3;i++)
		{
		  for(size_t j=0;j<m_Grains[gremove]->neighbordistfunclist[i].size();j++)
		  {
			  index = m_Grains[gremove]->neighbordistfunclist[i][j];
			  m_Grains[index]->neighbordistfunc[i] = m_Grains[index]->neighbordistfunc[i]-1;
		  }
		}
	  }
	  for(size_t i=1;i<activegrainlist.size();i++)
	  {
		nnum=0;
		index = activegrainlist[i];
		if(index != gremove && m_Grains[index]->phase == phase)
		{
		  for(int j=0;j<3;j++)
		  {
			nnum = m_Grains[index]->neighbordistfunc[j];
			dia = m_Grains[index]->equivdiameter;
			if(dia > maxdiameter[phase]) dia = maxdiameter[phase];
			if(dia < mindiameter[phase]) dia = mindiameter[phase];
			dia = int((dia-mindiameter[phase])/binstepsize[phase]);
			if(nnum > 0)
			{
			  neighbordist[phase][dia][j] = neighbordist[phase][dia][j]+nnum;
			}
		  }
		  count++;
		}
	  }
	  if(gadd > 0 && m_Grains[gadd]->phase == phase)
	  {
		for(int j=0;j<3;j++)
		{
		  nnum = m_Grains[index]->neighbordistfunc[j];
		  dia = m_Grains[index]->equivdiameter;
		  if(dia > maxdiameter[phase]) dia = maxdiameter[phase];
		  if(dia < mindiameter[phase]) dia = mindiameter[phase];
		  dia = int((dia-mindiameter[phase])/binstepsize[phase]);
		  if(nnum > 0)
		  {
			neighbordist[phase][dia][j] = neighbordist[phase][dia][j]+nnum;
		  }
		}
		count++;
	  }
	  for(int i=0;i<numdiameterbins[phase];i++)
	  {
		for(int j=0;j<3;j++)
		{
		  neighborerror = neighborerror + ((neighborhood[phase][i][j]-(neighbordist[phase][i][j]/double(count)))*(neighborhood[phase][i][j]-(neighbordist[phase][i][j]/double(count))));
		}
	  }
	  if(gadd > 0 && m_Grains[gadd]->phase == phase)
	  {
		for(int i=0;i<3;i++)
		{
		  for(size_t j=0;j<m_Grains[gadd]->neighbordistfunclist[i].size();j++)
		  {
			  index = m_Grains[gadd]->neighbordistfunclist[i][j];
			  m_Grains[index]->neighbordistfunc[i] = m_Grains[index]->neighbordistfunc[i]-1;
		  }
		}
	  }
	  if(gremove > 0 && m_Grains[gadd]->phase == phase)
	  {
		for(int i=0;i<3;i++)
		{
		  for(size_t j=0;j<m_Grains[gremove]->neighbordistfunclist[i].size();j++)
		  {
			  index = m_Grains[gremove]->neighbordistfunclist[i][j];
			  m_Grains[index]->neighbordistfunc[i]++;
		  }
		}
	  }
  }
  return neighborerror;
}

double GrainGeneratorFunc::costcheck_remove(size_t gnum)
{
  int index;
  double removecost = 0;
  for(size_t i=0;i<m_Grains[gnum]->voxellist->size();i++)
  {
  index = m_Grains[gnum]->voxellist->at(i);
  if(voxels[index].grainlist->size() == 1) removecost = removecost+1.0;
  if(voxels[index].grainlist->size() > 1)
  {
    if(voxels[index].grainlist->size() == 2) removecost = removecost - voxels[index].ellipfunclist->at(0);
    removecost = removecost - m_Grains[gnum]->ellipfunclist->at(i);
  }
  }
  return removecost;
}

double GrainGeneratorFunc::costcheck_add(size_t gnum)
{
  int index;
  double addcost = 0;
  for(size_t i=0;i<m_Grains[gnum]->voxellist->size();i++)
  {
  index = m_Grains[gnum]->voxellist->at(i);
  if(voxels[index].grainlist->size() == 0) addcost = addcost-1.0;
  if(voxels[index].grainlist->size() >= 1)
  {
    if(voxels[index].grainlist->size() == 1) addcost = addcost + voxels[index].ellipfunclist->at(0);
    addcost = addcost + m_Grains[gnum]->ellipfunclist->at(i);
  }
  }
  return addcost;
}

double GrainGeneratorFunc::check_sizedisterror(int gadd, int gremove)
{
  double dia;
  double sizedisterror = 0;
  int index;
  int count = 0;
  int phase;
  size_t xtalSize = crystruct.size();
  for(std::vector<AIM::Reconstruction::CrystalStructure>::size_type iter = 0; iter < xtalSize; ++iter)
  {
	  phase = iter+1;
	  for(int i=0;i<40;i++)
	  {
		simgrainsizedist[phase][i] = 0.0;
	  }
	  for(size_t b=1;b<activegrainlist.size();b++)
	  {
		index = activegrainlist[b];
		if(index != gremove && m_Grains[index]->surfacegrain == 0 && m_Grains[index]->phase == phase)
		{
		  dia = m_Grains[index]->equivdiameter;
		  dia = (dia-(mindiameter[phase]/2.0))/grainsizediststep[phase];
		  if(dia < 0) dia = 0;
		  if(dia > 39) dia = 39;
		  simgrainsizedist[phase][int(dia)]++;
		  count++;
		}
	  }
	  if(gadd > 0 && m_Grains[gadd]->surfacegrain == 0 && m_Grains[gadd]->phase == phase)
	  {
		dia = m_Grains[gadd]->equivdiameter;
		dia = (dia-(mindiameter[phase]/2.0))/grainsizediststep[phase];
		if(dia < 0) dia = 0;
		if(dia > 39) dia = 39;
		simgrainsizedist[phase][int(dia)]++;
		count++;
	  }
	  for(int i=0;i<40;i++)
	  {
		simgrainsizedist[phase][i] = simgrainsizedist[phase][i]/double(count);
	  }
	  for(int i=0;i<40;i++)
	  {
		  sizedisterror = sizedisterror + ((simgrainsizedist[phase][i]-grainsizedist[phase][i])*(simgrainsizedist[phase][i]-grainsizedist[phase][i]));
	  }
  }
  return sizedisterror;
}

int  GrainGeneratorFunc::pack_grains(const std::string &filename, int numgrains)
{
  totalvol = 0;
  double change1, change2, change3;
  std::ofstream outFile;
  outFile.open(filename.c_str());
  size_t index;
  int phase;
  double random;
  double xc, yc, zc;
  currentfillingerror = 0, oldfillingerror = 0;
  currentneighborhooderror = 0, oldneighborhooderror = 0;
  currentsizedisterror = 0, oldsizedisterror = 0;
  double addcost, removecost;
  int acceptedmoves = 0;
  double acceptableerror = 0.0;
  rg.RandomInit((static_cast<unsigned int> (time(NULL))));
  activegrainlist.resize(numgrains + 1);
  for (std::vector<AIM::Reconstruction::CrystalStructure>::size_type i = 1; i < crystruct.size();++i)
  {
    phasefraction[i] = phasefraction[i] + phasefraction[i - 1];
  }
  for (int i = 1; i < (numextragrains + 1); i++)
  {
    random = rg.Random();
    for (std::vector<AIM::Reconstruction::CrystalStructure>::size_type j = 0; j < crystruct.size();++j)
    {
      if (random < phasefraction[j])
      {
        phase = j;
        {
          break;
        }
      }
    }
    generate_grain(i, phase);
    totalvol = totalvol + m_Grains[i]->volume;
  }
  totalvol = totalvol / (double(numextragrains) / double(numgrains));
  initialize2();
  for (int i = 1; i < (numextragrains + 1); i++)
  {
    xc = rg.Random() * (xpoints * resx);
    yc = rg.Random() * (ypoints * resy);
    zc = rg.Random() * (zpoints * resz);
    m_Grains[i]->centroidx = xc;
    m_Grains[i]->centroidy = yc;
    m_Grains[i]->centroidz = zc;
  }
  for (int i = 1; i < (numextragrains + 1); i++)
  {
    insert_grain(i);
  }
  if (neighborhooderrorweight > 0) determine_neighbors();
  oldfillingerror = totalpoints;
  for (int i = 1; i < numgrains + 1; i++)
  {
    int random = int(rg.Random() * (numextragrains));
    if (random == 0) random = 1;
    if (random > (numextragrains)) random = (numextragrains);
    while (m_Grains[random]->active == 1)
    {
      random++;
      if (random > (numextragrains)) random = random - (numextragrains);
      if (random == 0) random = 1;
    }
    m_Grains[random]->active = 1;
    addcost = costcheck_add(random);
    add_grain(random);
    activegrainlist[i] = random;
    oldfillingerror = oldfillingerror + addcost;
  }
  oldsizedisterror = check_sizedisterror(-1000, -1000);
  oldneighborhooderror = check_neighborhooderror(-1000, -1000);
  for (int iteration = 0; iteration < (250000); iteration++)
  {
    change1 = 0;
    change2 = 0;
    change3 = 0;
    //    acceptableerror = 0.005*exp(-7*(iteration/double(250000)));
    acceptableerror = 0;
    int option = iteration % 4;
    if (iteration % 100 == 0) outFile << oldfillingerror << " " << oldsizedisterror << "  " << oldneighborhooderror << "  " << acceptedmoves << std::endl;
    if (option == 0)
    {
      int random = int(rg.Random() * (numextragrains));
      if (random == 0) random = 1;
      if (random > (numextragrains)) random = (numextragrains);
      while (m_Grains[random]->active == 1)
      {
        random++;
        if (random > (numextragrains)) random = random - (numextragrains);
        if (random == 0) random = 1;
      }
      addcost = costcheck_add(random);
      if (fillingerrorweight > 0) currentfillingerror = oldfillingerror + addcost;
      if (sizedisterrorweight > 0) currentsizedisterror = check_sizedisterror(random, -1000);
      if (neighborhooderrorweight > 0) currentneighborhooderror = check_neighborhooderror(random, -1000);
      if (fillingerrorweight > 0) change1 = (currentfillingerror - oldfillingerror) / oldfillingerror;
      if (fillingerrorweight > 0 && oldfillingerror < 0) change1 = -change1;
      if (sizedisterrorweight > 0) change2 = (currentsizedisterror - oldsizedisterror) / oldsizedisterror;
      if (neighborhooderrorweight > 0) change3 = (currentneighborhooderror - oldneighborhooderror) / oldneighborhooderror;
      if (fillingerrorweight * change1 + sizedisterrorweight * change2 + neighborhooderrorweight * change3 <= acceptableerror)
      {
        m_Grains[random]->active = 1;
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
      if (fillingerrorweight > 0) currentfillingerror = oldfillingerror + removecost;
      if (sizedisterrorweight > 0) currentsizedisterror = check_sizedisterror(-1000, random);
      if (neighborhooderrorweight > 0) currentneighborhooderror = check_neighborhooderror(-1000, random);
      if (fillingerrorweight > 0) change1 = (currentfillingerror - oldfillingerror) / oldfillingerror;
      if (fillingerrorweight > 0 && oldfillingerror < 0) change1 = -change1;
      if (sizedisterrorweight > 0) change2 = (currentsizedisterror - oldsizedisterror) / oldsizedisterror;
      if (neighborhooderrorweight > 0) change3 = (currentneighborhooderror - oldneighborhooderror) / oldneighborhooderror;
      if (fillingerrorweight * change1 + sizedisterrorweight * change2 + neighborhooderrorweight * change3 <= acceptableerror)
      {
        m_Grains[random]->active = 0;
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
      while (m_Grains[random]->active == 1)
      {
        random++;
        if (random > (numextragrains)) random = random - (numextragrains);
        if (random == 0) random = 1;
      }
      addcost = costcheck_add(random);
      removecost = costcheck_remove(random1);
      if (fillingerrorweight > 0) currentfillingerror = oldfillingerror + addcost + removecost;
      if (sizedisterrorweight > 0) currentsizedisterror = check_sizedisterror(random, random1);
      if (neighborhooderrorweight > 0) currentneighborhooderror = check_neighborhooderror(random, random1);
      if (fillingerrorweight > 0) change1 = (currentfillingerror - oldfillingerror) / oldfillingerror;
      if (fillingerrorweight > 0 && oldfillingerror < 0) change1 = -change1;
      if (sizedisterrorweight > 0) change2 = (currentsizedisterror - oldsizedisterror) / oldsizedisterror;
      if (neighborhooderrorweight > 0) change3 = (currentneighborhooderror - oldneighborhooderror) / oldneighborhooderror;
      if (fillingerrorweight * change1 + sizedisterrorweight * change2 + neighborhooderrorweight * change3 <= acceptableerror)
      {
        m_Grains[random]->active = 1;
        m_Grains[random1]->active = 0;
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
      //    double mindist = 100000.0;
      size_t random = random1;
      for (size_t i = 0; i < m_Grains[random1]->neighbordistfunclist[1].size(); i++)
      {
        index = m_Grains[random1]->neighbordistfunclist[1][i];
        if (m_Grains[index]->active == 0 && index != random1)
        {
          random = index;
          i = m_Grains[random1]->neighbordistfunclist[1].size();
        }
      }
      if (random != random1)
      {
        addcost = costcheck_add(random);
        removecost = costcheck_remove(random1);
        if (fillingerrorweight > 0) currentfillingerror = oldfillingerror + addcost + removecost;
        if (sizedisterrorweight > 0) currentsizedisterror = check_sizedisterror(random, random1);
        if (neighborhooderrorweight > 0) currentneighborhooderror = check_neighborhooderror(random, random1);
        if (fillingerrorweight > 0) change1 = (currentfillingerror - oldfillingerror) / oldfillingerror;
        if (fillingerrorweight > 0 && oldfillingerror < 0) change1 = -change1;
        if (sizedisterrorweight > 0) change2 = (currentsizedisterror - oldsizedisterror) / oldsizedisterror;
        if (neighborhooderrorweight > 0) change3 = (currentneighborhooderror - oldneighborhooderror) / oldneighborhooderror;
        if (fillingerrorweight * change1 + sizedisterrorweight * change2 + neighborhooderrorweight * change3 <= acceptableerror)
        {
          m_Grains[random]->active = 1;
          m_Grains[random1]->active = 0;
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
    m_Grains[i + 1] = m_Grains[activegrainlist[i]];
  }
  m_Grains.resize(activegrainlist.size() + 1);
  return (m_Grains.size());
}

int GrainGeneratorFunc::assign_voxels(int numgrains)
{
  int index;
  int *gsizes;
  gsizes = new int[numgrains];
  int oldname;
 // int size;
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
  int *gnames;
  gnames = new int[totalpoints];
  int *unassigned;
  unassigned = new int[totalpoints];
  for(int i=0;i<totalpoints;i++)
  {
	  gnames[i] = voxels[i].grainname;
	  unassigned[i] = voxels[i].unassigned;
  }
  for(int i=1;i<numgrains;i++)
  {
	  gsizes[i] = 0;
  }
  for(int i=1;i<numgrains;i++)
  {
    double volcur = m_Grains[i]->volume;
    double bovera = m_Grains[i]->radius2;
    double covera = m_Grains[i]->radius3;
    double omega3 = m_Grains[i]->omega3;
    xc = m_Grains[i]->centroidx;
    yc = m_Grains[i]->centroidy;
    zc = m_Grains[i]->centroidz;
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
    double phi1 = m_Grains[i]->axiseuler1;
    double PHI = m_Grains[i]->axiseuler2;
    double phi2 = m_Grains[i]->axiseuler3;
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
	if(xmin < -0.5*xpoints) xmin = -0.5*xpoints;
	if(xmax > (1.5*xpoints-1)) xmax = (1.5*xpoints-1);
	if(ymin < -0.5*ypoints) ymin = -0.5*ypoints;
	if(ymax > (1.5*ypoints-1)) ymax = (1.5*ypoints-1);
	if(zmin < -0.5*zpoints) zmin = -0.5*zpoints;
	if(zmax > (1.5*zpoints-1)) zmax = (1.5*zpoints-1);
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
			x = double(column)*resx;
			y = double(row)*resy;
			z = double(plane)*resz;
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
				  if(gnames[currentpoint] > 0)
				  {
					oldname = gnames[currentpoint];
					gsizes[oldname] = gsizes[oldname]-1;
					gnames[currentpoint] = -1;
					unassigned[currentpoint] = 1;
				  }
				  if(gnames[currentpoint] == 0 && unassigned[currentpoint] == 0)
				  {
					gnames[currentpoint] = i;
					gsizes[i]++;
				  }
			  }
			}
		  }
	  }
	}
    m_Grains[i]->centroidx = xc;
    m_Grains[i]->centroidy = yc;
    m_Grains[i]->centroidz = zc;
    m_Grains[i]->numvoxels = gsizes[i];
  }
  int *newnames;
  newnames = new int[numgrains];
  int goodcount = 1;
  for(int i=1;i<numgrains;i++)
  {
    newnames[i] = 0;
    if(gsizes[i] > 0)
    {
	  m_Grains[goodcount] = m_Grains[i];
	  newnames[i] = goodcount;
      goodcount++;
    }
  }
  for(int i=0;i<totalpoints;i++)
  {
	  if(gnames[i] > 0)
	  {
		  voxels[i].grainname = newnames[gnames[i]];
		  voxels[i].unassigned = unassigned[i];
	  }
	  if(gnames[i] <= 0)
	  {
		  voxels[i].grainname = gnames[i];
		  voxels[i].unassigned = unassigned[i];
	  }
  }
  return goodcount;
}
void  GrainGeneratorFunc::assign_eulers(int numgrains)
{
  int gnum = 0;
  int numbins = 0;
  double totaldensity = 0;
  double synea1=0,synea2=0,synea3=0;
  double q[5];
  double random;
  int choose, phase;

  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
  for(int i=1;i<numgrains;i++)
  {
    random = rg.Random();
    choose = 0;
    totaldensity = 0;
    phase = m_Grains[i]->phase;
    for (int j = 0; j < numbins; j++)
    {
      double density = actualodf[phase][j];
      totaldensity = totaldensity + density;
      if (random >= totaldensity) choose = j;
    }
    m_OrientatioOps[crystruct[phase]]->determineEulerAngles(choose, synea1, synea2, synea3);
    m_Grains[i]->euler1 = synea1;
    m_Grains[i]->euler2 = synea2;
    m_Grains[i]->euler3 = synea3;
    OrientationMath::eulertoQuat(q, synea1, synea2, synea3);
    m_Grains[i]->avg_quat[0] = q[0];
    m_Grains[i]->avg_quat[1] = q[1];
    m_Grains[i]->avg_quat[2] = q[2];
    m_Grains[i]->avg_quat[3] = q[3];
    m_Grains[i]->avg_quat[4] = q[4];
    if (m_Grains[gnum]->surfacegrain == 0)
    {
      simodf[phase][choose] = simodf[phase][choose] + (double(m_Grains[i]->numvoxels) * resx * resy * resz);
      unbiasedvol[phase] = unbiasedvol[phase] + (double(m_Grains[i]->numvoxels) * resx * resy * resz);
    }
  }
  for(int i=0;i<numbins;i++)
  {
	simodf[phase][i] = simodf[phase][i]/unbiasedvol[phase];
  }
}

void  GrainGeneratorFunc::fill_gaps(int numgrains)
{
 std::vector<int> neighs;
 std::vector<int> remove;
 std::vector<int> gsizes;
  int count = 1;
  int good = 1;
  double x, y, z;
  gsizes.resize(numgrains,0);
  int neighpoint;
  int neighbors[6];
  std::vector<int> n(numgrains);
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
      if(grainname <= 0)
      {
	    count++;
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
  gsizes.resize(numgrains,0);
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int name = voxels[i].grainname;
    gsizes[name]++;
  }
  for (int i = 1; i < numgrains; i++)
  {
	  m_Grains[i]->numvoxels = gsizes[i];
	  m_Grains[i]->equivdiameter = 2.0*pow((gsizes[i]*resx*resy*resz*(3.0/4.0)*(1/m_pi)),(1.0/3.0));
  }
  gsizes.clear();
}

int GrainGeneratorFunc::adjust_boundaries(int numgrains)
{
	int neighbors[6];
	neighbors[0] = -xpoints*ypoints;
	neighbors[1] = -xpoints;
	neighbors[2] = -1;
	neighbors[3] = 1;
	neighbors[4] = xpoints;
	neighbors[5] = xpoints*ypoints;
	int iterations = 0;
	int selectedgrain = 0;
	int good = 0;
	int growth = 1;
	int nucleus;
	int bad = 0;
	double random, oldsizedisterror, currentsizedisterror, diam;
	int x, y, z;
	int neighpoint, index;
	size_t count, affectedcount;
	int vListSize = 1000;
	int *gsizes;
	double voxtovol = resx*resy*resz*(3.0/4.0)*(1.0/m_pi);
	gsizes = new int[numgrains];
	activegrainlist.resize(numgrains);
	std::vector<int> voxellist(vListSize,-1);
	std::vector<int> affectedvoxellist(vListSize,-1);
	for(int i=1;i<numgrains;i++)
	{
		activegrainlist[i] = i;
		gsizes[i] = 0;
	}
	int *gnames;
	gnames = new int[totalpoints];
	int *reassigned;
	reassigned = new int[totalpoints];
	for(int i=0;i<totalpoints;i++)
	{
	  gnames[i] = voxels[i].grainname;
	  reassigned[i] = 0;
	  gsizes[gnames[i]]++;
	}
	oldsizedisterror = check_sizedisterror(-1000,-1000);
	while(iterations < 10000)
	{
		iterations++;
		good = 0;
		while (good == 0)
		{
			good = 1;
			selectedgrain = int(rg.Random() * numgrains);
			if (selectedgrain >= numgrains) selectedgrain = numgrains-1;
			if (selectedgrain == 0) selectedgrain = 1;
			if (m_Grains[selectedgrain]->surfacegrain > 0) good = 0;
		}
		growth = 1;
		random = rg.Random();
		if(random < 0.5) growth = -1;
		nucleus = 0;
		count = 0;
		affectedcount = 0;
		while(gnames[nucleus] != selectedgrain)
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
			  neighpoint = index+neighbors[j];
			  if(j == 0 && z == 0) good = 0;
			  if(j == 5 && z == (zpoints-1)) good = 0;
			  if(j == 1 && y == 0) good = 0;
			  if(j == 4 && y == (ypoints-1)) good = 0;
			  if(j == 2 && x == 0) good = 0;
			  if(j == 3 && x == (xpoints-1)) good = 0;
			  if(good == 1 && gnames[neighpoint] == selectedgrain && reassigned[neighpoint] == 0)
			  {
				voxellist[count] = neighpoint;
				reassigned[neighpoint] = -1;
				count++;
				if(count >= voxellist.size()) voxellist.resize(voxellist.size()+vListSize,-1);
			  }
			  if(good == 1 && gnames[neighpoint] != selectedgrain && gnames[index] == selectedgrain)
			  {
				if(growth == 1 && reassigned[neighpoint] <= 0)
				{
					reassigned[neighpoint] = gnames[neighpoint];
					gnames[neighpoint] = gnames[index];
					affectedvoxellist[affectedcount] = neighpoint;
					affectedcount++;
					if(affectedcount >= affectedvoxellist.size()) affectedvoxellist.resize(affectedvoxellist.size()+vListSize,-1);
				}
				if(growth == -1 && reassigned[neighpoint] <= 0)
				{
					reassigned[index] = gnames[index];
					gnames[index] = gnames[neighpoint];
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
				gsizes[gnames[index]]++;
				gsizes[reassigned[index]] = gsizes[reassigned[index]]-1;
			}
		}
		for(std::vector<int>::size_type i=1;i<activegrainlist.size();i++)
		{
			index = activegrainlist[i];
			diam = 2.0*pow((gsizes[index]*voxtovol),(1.0/3.0));
			m_Grains[index]->equivdiameter = diam;
		}
		currentsizedisterror = check_sizedisterror(-1000,-1000);
		if(currentsizedisterror <= oldsizedisterror)
		{
			oldsizedisterror = currentsizedisterror;
			for(int i=1;i<numgrains;i++)
			{
				if(gsizes[i] == 0) activegrainlist.erase(std::remove(activegrainlist.begin(), activegrainlist.end(), i), activegrainlist.end());
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
					gsizes[gnames[index]] = gsizes[gnames[index]]-1;
					gnames[index] = reassigned[index];
					gsizes[gnames[index]]++;
				}
			}
			for(std::vector<int>::size_type i=1;i<activegrainlist.size();i++)
			{
				index = activegrainlist[i];
				diam = 2.0*pow((gsizes[index]*voxtovol),(1.0/3.0));
				m_Grains[index]->equivdiameter = diam;
			}
		}
		for(int i=0;i<totalpoints;i++)
		{
			reassigned[i] = 0;
		}
	}
	int *newnames;
	newnames = new int[m_Grains.size()];
	for (size_t i=1;i<activegrainlist.size();i++)
	{
		m_Grains[i] = m_Grains[activegrainlist[i]];
		newnames[activegrainlist[i]] = i;
	}
	m_Grains.resize(activegrainlist.size());
	for(int i=0;i<totalpoints;i++)
	{
	  voxels[i].grainname = newnames[gnames[i]];
	}
	return (m_Grains.size());
}
void GrainGeneratorFunc::read_structure(const std::string &filename)
{
  const unsigned int size ( 1024 );
  char buf [ size ];
  std::ifstream in ( filename.c_str() );
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
    m_Grains[gnum]->surfacegrain = onedge;
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
  double column, row, plane;
  double x, y, z;
  double xn, yn, zn;
  double xdist, ydist, zdist;
  int grain;
  size_t nnum;
  int onsurf = 0;
  double dist, dist2, diam, diam2;
  int dist_int, dist2_int;
  int good = 0;
  int neighbor = 0;
  size_t xtalCount = crystruct.size();
  for(size_t i=0;i<xtalCount;++i)
  {
	totalsurfacearea[i] = 0;
  }
  int surfacegrain = 1;
  int nListSize = 100;
  for(int i=0;i<numgrains;i++)
  {
    m_Grains[i]->numneighbors = 0;
    m_Grains[i]->neighborlist->assign(nListSize, -1);
    m_Grains[i]->neighborsurfarealist->assign(nListSize, -1.0);
    for(int j=0;j<3;j++)
    {
      m_Grains[i]->neighbordistfunc[j] = 0;
    }
  }
  int *gnames;
  gnames = new int[totalpoints];
  for(int i=0;i<totalpoints;i++)
  {
	  gnames[i] = voxels[i].grainname;
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
		if((column == 0 || column == (xpoints-1) || row == 0 || row == (ypoints-1) || plane == 0 || plane == (zpoints-1)) && zpoints != 1) m_Grains[grain]->surfacegrain = surfacegrain;
		if((column == 0 || column == (xpoints-1) || row == 0 || row == (ypoints-1)) && zpoints == 1) m_Grains[grain]->surfacegrain = surfacegrain;
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
	        IntVectorType nlist = m_Grains[grain]->neighborlist;
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
 std::vector<int> nlistcopy;
  for(int i=1;i<numgrains;i++)
  {
    int phase = m_Grains[i]->phase;
    IntVectorType nlist = m_Grains[i]->neighborlist;
    DoubleVectorType nsalist = m_Grains[i]->neighborsurfarealist;
   std::vector<int>::iterator newend;
    sort(nlist->begin(), nlist->end());
    // Make a copy of the contents of the neighborlist vector
    nlistcopy.assign(nlist->begin(), nlist->end());
    newend = unique(nlist->begin(), nlist->end());
    nlist->erase(newend, nlist->end());
    nlist->erase(std::remove(nlist->begin(), nlist->end(), -1), nlist->end());
    nlist->erase(std::remove(nlist->begin(), nlist->end(), 0), nlist->end());
    int numneighs = int(nlist->size());
	nsalist->resize(numneighs,0);
    for (int j = 0; j < numneighs; j++)
    {
      int neigh = nlist->at(j);
      int number = std::count(nlistcopy.begin(), nlistcopy.end(), neigh);
      double area = number * resx * resx;
      nsalist->at(j) = area;
      if (m_Grains[i]->surfacegrain == 0 && (neigh > i || m_Grains[neigh]->surfacegrain == 1))
      {
        totalsurfacearea[phase] = totalsurfacearea[phase] + area;
      }
    }
    m_Grains[i]->numneighbors = numneighs;
  }
  for(int i=1;i<numgrains;i++)
  {
	  x = m_Grains[i]->centroidx;
	  y = m_Grains[i]->centroidy;
	  z = m_Grains[i]->centroidz;
	  diam = m_Grains[i]->equivdiameter;
	  for(int j=i;j<numgrains;j++)
	  {
		xn = m_Grains[j]->centroidx;
		yn = m_Grains[j]->centroidy;
		zn = m_Grains[j]->centroidz;
		diam2 = m_Grains[j]->equivdiameter;
		xdist = fabs(x-xn);
		ydist = fabs(y-yn);
		zdist = fabs(z-zn);
		dist = (xdist*xdist)+(ydist*ydist)+(zdist*zdist);
		dist = pow(dist,0.5);
		dist2 = dist;
		dist_int = int(dist/diam);
		dist2_int = int(dist2/diam2);
		if(dist < 3)
		{
		  m_Grains[i]->neighbordistfunc[dist_int]++;
		}
		if(dist2 < 3)
		{
		  m_Grains[j]->neighbordistfunc[dist2_int]++;
		}
	  }
  }
}


void GrainGeneratorFunc::MC_LoopBody1(int phase, size_t neighbor, int j,std::vector<double>* misolist,std::vector<double>* neighborsurfarealist, double &mdfchange)
{
  double w;
  double n1, n2, n3;
  double r1, r2, r3;

  int curmiso1 = std::numeric_limits<int >::max();
  int curmiso2 = std::numeric_limits<int >::max();
  int curmiso3 = std::numeric_limits<int >::max();

  int neighsurfarea = std::numeric_limits<int >::max();
  int curmisobin = std::numeric_limits<int >::max();
  int newmisobin = std::numeric_limits<int >::max();

  double q1[5], q2[5];

  curmiso1 = misolist->at(3*j);
  curmiso2 = misolist->at(3*j+1);
  curmiso3 = misolist->at(3*j+2);
  neighsurfarea = neighborsurfarealist->at(j);
  curmisobin = m_OrientatioOps[crystruct[phase]]->getMisoBin( curmiso1, curmiso2, curmiso3);
  q2[1] = m_Grains[neighbor]->avg_quat[1];
  q2[2] = m_Grains[neighbor]->avg_quat[2];
  q2[3] = m_Grains[neighbor]->avg_quat[3];
  q2[4] = m_Grains[neighbor]->avg_quat[4];
  w = m_OrientatioOps[crystruct[phase]]->getMisoQuat(q1,q2,n1,n2,n3);
  OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientatioOps[crystruct[phase]]->getMisoBin(n1, n2, n3);
  mdfchange = mdfchange + (((actualmdf[phase][curmisobin]-simmdf[phase][curmisobin])*(actualmdf[phase][curmisobin]-simmdf[phase][curmisobin])) - ((actualmdf[phase][curmisobin]-(simmdf[phase][curmisobin]-(neighsurfarea/totalsurfacearea[phase])))*(actualmdf[phase][curmisobin]-(simmdf[phase][curmisobin]-(neighsurfarea/totalsurfacearea[phase])))));
  mdfchange = mdfchange + (((actualmdf[phase][newmisobin]-simmdf[phase][newmisobin])*(actualmdf[phase][newmisobin]-simmdf[phase][newmisobin])) - ((actualmdf[phase][newmisobin]-(simmdf[phase][newmisobin]+(neighsurfarea/totalsurfacearea[phase])))*(actualmdf[phase][newmisobin]-(simmdf[phase][newmisobin]+(neighsurfarea/totalsurfacearea[phase])))));
}

void GrainGeneratorFunc::MC_LoopBody2(int phase, size_t neighbor, int j,std::vector<double>* misolist,std::vector<double>* neighborsurfarealist)
{
  double w;
  double n1, n2, n3;
  double r1, r2, r3;

  int curmiso1 = std::numeric_limits<int >::max();
  int curmiso2 = std::numeric_limits<int >::max();
  int curmiso3 = std::numeric_limits<int >::max();

  int neighsurfarea = std::numeric_limits<int >::max();
  int curmisobin = std::numeric_limits<int >::max();
  int newmisobin = std::numeric_limits<int >::max();

  double q1[5], q2[5];
  double miso1 = std::numeric_limits<double >::max();
  double miso2 = std::numeric_limits<double >::max();
  double miso3 = std::numeric_limits<double >::max();

  curmiso1 = misolist->at(3 * j);
  curmiso2 = misolist->at(3 * j + 1);
  curmiso3 = misolist->at(3 * j + 2);
  neighsurfarea = neighborsurfarealist->at(j);
  curmisobin = m_OrientatioOps[crystruct[phase]]->getMisoBin(curmiso1, curmiso2, curmiso3);
  q2[1] = m_Grains[neighbor]->avg_quat[1];
  q2[2] = m_Grains[neighbor]->avg_quat[2];
  q2[3] = m_Grains[neighbor]->avg_quat[3];
  q2[4] = m_Grains[neighbor]->avg_quat[4];
  w = m_OrientatioOps[crystruct[phase]]->getMisoQuat(q1,q2,n1,n2,n3);
  OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientatioOps[crystruct[phase]]->getMisoBin(n1, n2, n3);
  misolist->at(3 * j) = miso1;
  misolist->at(3 * j + 1) = miso2;
  misolist->at(3 * j + 2) = miso3;
  simmdf[phase][curmisobin] = simmdf[phase][curmisobin] - (neighsurfarea / totalsurfacearea[phase]);
  simmdf[phase][newmisobin] = simmdf[phase][newmisobin] + (neighsurfarea / totalsurfacearea[phase]);
}

void GrainGeneratorFunc::swapOutOrientation( int &badtrycount, int &numbins, double currentodferror, double currentmdferror)
{
  double random;
  int good;
  double deltaerror = 1.0;
  int selectedgrain1;
  double q1[5];
  double ea1, ea2, ea3;
  double r1, r2, r3;

  int g1odfbin = std::numeric_limits<int >::max();

  double g1ea1 = std::numeric_limits<double >::max();
  double g1ea2 = std::numeric_limits<double >::max();
  double g1ea3 = std::numeric_limits<double >::max();

  IntVectorType nlist;
  std::vector<double>* misolist;
  DoubleVectorType neighborsurfarealist;

  double totaldensity = 0;

  good = 0;
  while (good == 0)
  {
    good = 1;
    selectedgrain1 = int(rg.Random() * numgrains);
    if (selectedgrain1 == 0) selectedgrain1 = 1;
    if (selectedgrain1 == numgrains) selectedgrain1 = numgrains - 1;
    if (m_Grains[selectedgrain1]->surfacegrain > 0) good = 0;
  }

  ea1 = m_Grains[selectedgrain1]->euler1;
  ea2 = m_Grains[selectedgrain1]->euler2;
  ea3 = m_Grains[selectedgrain1]->euler3;
  OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
  int phase = m_Grains[selectedgrain1]->phase;
  g1odfbin = m_OrientatioOps[crystruct[phase]]->getOdfBin(r1, r2, r3);
  random = rg.Random();
  int choose = 0;
  totaldensity = 0;
  for (int i = 0; i < numbins; i++)
  {
    double density = actualodf[phase][i];
    totaldensity = totaldensity + density;
    if (random >= totaldensity) choose = i;
  }

  m_OrientatioOps[crystruct[phase]]->determineEulerAngles(choose, g1ea1, g1ea2, g1ea3);
  OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);

  double odfchange = ((actualodf[phase][choose] - simodf[phase][choose]) * (actualodf[phase][choose] - simodf[phase][choose])) - ((actualodf[phase][choose] - (simodf[phase][choose]
      + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]))) * (actualodf[phase][choose] - (simodf[phase][choose]
      + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]))));
  odfchange = odfchange + (((actualodf[phase][g1odfbin] - simodf[phase][g1odfbin]) * (actualodf[phase][g1odfbin] - simodf[phase][g1odfbin])) - ((actualodf[phase][g1odfbin] - (simodf[phase][g1odfbin]
      - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]))) * (actualodf[phase][g1odfbin] - (simodf[phase][g1odfbin]
      - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase])))));

  double mdfchange = 0;
  nlist = m_Grains[selectedgrain1]->neighborlist;
  misolist = m_Grains[selectedgrain1]->misorientationlist;
  neighborsurfarealist = m_Grains[selectedgrain1]->neighborsurfarealist;
  for (size_t j = 0; j < nlist->size(); j++)
  {
    int neighbor = nlist->at(j);
    MC_LoopBody1(phase, neighbor, j, misolist, neighborsurfarealist.get(), mdfchange);
  }

  deltaerror = (odfchange/currentodferror) + (mdfchange/currentmdferror);
  if (deltaerror > 0)
  {
    badtrycount = 0;
    m_Grains[selectedgrain1]->euler1 = g1ea1;
    m_Grains[selectedgrain1]->euler2 = g1ea2;
    m_Grains[selectedgrain1]->euler3 = g1ea3;
    m_Grains[selectedgrain1]->avg_quat[1] = q1[1];
    m_Grains[selectedgrain1]->avg_quat[2] = q1[2];
    m_Grains[selectedgrain1]->avg_quat[3] = q1[3];
    m_Grains[selectedgrain1]->avg_quat[4] = q1[4];
    simodf[phase][choose] = simodf[phase][choose] + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]);
    simodf[phase][g1odfbin] = simodf[phase][g1odfbin] - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]);
    for (size_t j = 0; j < nlist->size(); j++)
    {
      int neighbor = nlist->at(j);
      MC_LoopBody2(phase, neighbor, j, misolist, neighborsurfarealist.get());
    }
  }

}

void GrainGeneratorFunc::switchOrientations( int &badtrycount, int &numbins, double currentodferror, double currentmdferror)
{
  int good = 0;
  double deltaerror;
  int selectedgrain1;
  int selectedgrain2;
  double q1[5];
  double r1, r2, r3;

  int g1odfbin = std::numeric_limits<int >::max();
  int g2odfbin = std::numeric_limits<int >::max();

  double g1ea1 = std::numeric_limits<double >::max();
  double g1ea2 = std::numeric_limits<double >::max();
  double g1ea3 = std::numeric_limits<double >::max();
  double g2ea1 = std::numeric_limits<double >::max();
  double g2ea2 = std::numeric_limits<double >::max();
  double g2ea3 = std::numeric_limits<double >::max();

  IntVectorType nlist;
 std::vector<double>* misolist;
  DoubleVectorType neighborsurfarealist;

  good = 0;
  while (good == 0)
  {
    good = 1;
    selectedgrain1 = int(rg.Random() * numgrains);
    if (selectedgrain1 == 0) selectedgrain1 = 1;
    if (selectedgrain1 == numgrains) selectedgrain1 = numgrains - 1;
    selectedgrain2 = int(rg.Random() * numgrains);
    if (selectedgrain2 == 0) selectedgrain2 = 1;
    if (selectedgrain2 == numgrains) selectedgrain2 = numgrains - 1;
    if (m_Grains[selectedgrain1]->surfacegrain > 0 || m_Grains[selectedgrain2]->surfacegrain > 0) good = 0;
  }
  g1ea1 = m_Grains[selectedgrain1]->euler1;
  g1ea2 = m_Grains[selectedgrain1]->euler2;
  g1ea3 = m_Grains[selectedgrain1]->euler3;
  g2ea1 = m_Grains[selectedgrain2]->euler1;
  g2ea2 = m_Grains[selectedgrain2]->euler2;
  g2ea3 = m_Grains[selectedgrain2]->euler3;
  q1[1] = m_Grains[selectedgrain1]->avg_quat[1];
  q1[2] = m_Grains[selectedgrain1]->avg_quat[2];
  q1[3] = m_Grains[selectedgrain1]->avg_quat[3];
  q1[4] = m_Grains[selectedgrain1]->avg_quat[4];
  int phase = m_Grains[selectedgrain1]->phase;
  OrientationMath::eulertoRod(r1, r2, r3, g1ea1, g1ea2, g1ea3);
  g1odfbin = m_OrientatioOps[crystruct[phase]]->getOdfBin(r1, r2, r3);
  q1[1] = m_Grains[selectedgrain2]->avg_quat[1];
  q1[2] = m_Grains[selectedgrain2]->avg_quat[2];
  q1[3] = m_Grains[selectedgrain2]->avg_quat[3];
  q1[4] = m_Grains[selectedgrain2]->avg_quat[4];
  OrientationMath::eulertoRod(r1, r2, r3, g2ea1, g2ea2, g2ea3);
  g2odfbin = m_OrientatioOps[crystruct[phase]]->getOdfBin(r1, r2, r3);

  double odfchange = ((actualodf[phase][g1odfbin]-simodf[phase][g1odfbin]) * (actualodf[phase][g1odfbin]-simodf[phase][g1odfbin])) - ((actualodf[phase][g1odfbin]
     -(simodf[phase][g1odfbin] - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]) + (double(m_Grains[selectedgrain2]->numvoxels) * resx
          * resy * resz / unbiasedvol[phase]))) * (actualodf[phase][g1odfbin]-(simodf[phase][g1odfbin] - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase])
      + (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase]))));
  odfchange = odfchange + (((actualodf[phase][g2odfbin]-simodf[phase][g2odfbin]) * (actualodf[phase][g2odfbin]-simodf[phase][g2odfbin])) - ((actualodf[phase][g2odfbin]
     -(simodf[phase][g2odfbin] - (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase]) + (double(m_Grains[selectedgrain1]->numvoxels) * resx
          * resy * resz / unbiasedvol[phase]))) * (actualodf[phase][g2odfbin]-(simodf[phase][g2odfbin] - (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase])
      + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase])))));


  double mdfchange = 0;
  OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
  nlist = m_Grains[selectedgrain1]->neighborlist;
  misolist = m_Grains[selectedgrain1]->misorientationlist;
  neighborsurfarealist = m_Grains[selectedgrain1]->neighborsurfarealist;
  for (size_t j = 0; j < nlist->size(); j++)
  {
    int neighbor = nlist->at(j);
    if (neighbor != selectedgrain2)
    {
      MC_LoopBody1(phase, neighbor, j, misolist, neighborsurfarealist.get(), mdfchange);
    }
  }

  OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);
  nlist = m_Grains[selectedgrain2]->neighborlist;
  misolist = m_Grains[selectedgrain2]->misorientationlist;
  neighborsurfarealist = m_Grains[selectedgrain2]->neighborsurfarealist;
  for (size_t j = 0; j < nlist->size(); j++)
  {
    int neighbor = nlist->at(j);
    if (neighbor != selectedgrain1)
    {
      MC_LoopBody1(phase, neighbor, j, misolist, neighborsurfarealist.get(), mdfchange);
    }
  }

  deltaerror = (odfchange/currentodferror) + (mdfchange/currentmdferror);
  if (deltaerror > 0)
  {
    badtrycount = 0;
    m_Grains[selectedgrain1]->euler1 = g2ea1;
    m_Grains[selectedgrain1]->euler2 = g2ea2;
    m_Grains[selectedgrain1]->euler3 = g2ea3;
    m_Grains[selectedgrain2]->euler1 = g1ea1;
    m_Grains[selectedgrain2]->euler2 = g1ea2;
    m_Grains[selectedgrain2]->euler3 = g1ea3;
    simodf[phase][g1odfbin] = simodf[phase][g1odfbin] + (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase])
        - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]);
    simodf[phase][g2odfbin] = simodf[phase][g2odfbin] + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase])
        - (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase]);

	OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
	nlist = m_Grains[selectedgrain1]->neighborlist;
    misolist = m_Grains[selectedgrain1]->misorientationlist;
    neighborsurfarealist = m_Grains[selectedgrain1]->neighborsurfarealist;
    m_Grains[selectedgrain1]->avg_quat[1] = q1[1];
    m_Grains[selectedgrain1]->avg_quat[2] = q1[2];
    m_Grains[selectedgrain1]->avg_quat[3] = q1[3];
    m_Grains[selectedgrain1]->avg_quat[4] = q1[4];
    for (size_t j = 0; j < nlist->size(); j++)
    {
      int neighbor = nlist->at(j);
      if (neighbor != selectedgrain2)
      {
        MC_LoopBody2(phase, neighbor, j, misolist, neighborsurfarealist.get());
      }
    }

	OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);
    nlist = m_Grains[selectedgrain2]->neighborlist;
    misolist = m_Grains[selectedgrain2]->misorientationlist;
    neighborsurfarealist = m_Grains[selectedgrain2]->neighborsurfarealist;
    m_Grains[selectedgrain2]->avg_quat[1] = q1[1];
    m_Grains[selectedgrain2]->avg_quat[2] = q1[2];
    m_Grains[selectedgrain2]->avg_quat[3] = q1[3];
    m_Grains[selectedgrain2]->avg_quat[4] = q1[4];
    for (size_t j = 0; j < nlist->size(); j++)
    {
      int neighbor = nlist->at(j);
      if (neighbor != selectedgrain1)
      {
        MC_LoopBody2(phase, neighbor, j, misolist, neighborsurfarealist.get());
      }
    }
  }
}

void GrainGeneratorFunc::matchCrystallography(const std::string &ErrorFile, H5ReconStatsWriter::Pointer h5io)
{
  ofstream outFile;
  outFile.open(ErrorFile.c_str());
  int numbins = 0;

  int iterations = 0;
  int badtrycount = 0;
  double random;
  double currentodferror = 0;
  double currentmdferror = 0;
  size_t xtalSize = crystruct.size();
  for(size_t iter=0;iter<xtalSize;++iter)
  {
	  if(crystruct[iter] == AIM::Reconstruction::Cubic) numbins = 18*18*18;
	  if(crystruct[iter] == AIM::Reconstruction::Hexagonal) numbins = 36*36*12;
	  rg.RandomInit((static_cast<unsigned int>(time(NULL))));
	  while(badtrycount < 5000 && iterations < 100000)
	  {
		currentodferror = 0;
		currentmdferror = 0;
		for(int i=0;i<numbins;i++)
		{
		  currentodferror = currentodferror + ((actualodf[iter][i]-simodf[iter][i])*(actualodf[iter][i]-simodf[iter][i]));
		}
		for(int i=0;i<(numbins);i++)
		{
		  currentmdferror = currentmdferror + ((actualmdf[iter][i]-simmdf[iter][i])*(actualmdf[iter][i]-simmdf[iter][i]));
		}
		if(iterations%100 == 0) outFile << iterations << "  " << currentodferror << " " << currentmdferror << endl;
		iterations++;
		badtrycount++;
		random = rg.Random();

		if(random < 0.5)  // SwapOutOrientation
		{
		  swapOutOrientation(badtrycount, numbins, currentodferror, currentmdferror);
		}
		else if(random > 0.5) // SwitchOrientation
		{
		  switchOrientations(badtrycount, numbins, currentodferror, currentmdferror);
		}
	  }
	  outFile.close();
	  int err;
	  err = h5io->writeODFData(iter, crystruct[iter], simodf[iter].get());
	  err = h5io->writeMisorientationBinsData(iter, crystruct[iter], simmdf[iter].get());
  }
}
void  GrainGeneratorFunc::measure_misorientations ()
{
 std::vector<double > misolist;
  double w;
  double n1, n2, n3;
  double r1, r2, r3;
  double q1[5];
  double q2[5];
  double denom;
  AIM::Reconstruction::CrystalStructure phase1, phase2;
  double degtorad = m_pi/180.0;
  int mbin;

  IntVectorType nlist ;
  DoubleVectorType neighsurfarealist;
  for (int i = 1; i < numgrains; i++)
  {
    nlist = m_Grains[i]->neighborlist;
    neighsurfarealist = m_Grains[i]->neighborsurfarealist;
    misolist.resize(nlist->size()*3);
    q1[1] = m_Grains[i]->avg_quat[1];
    q1[2] = m_Grains[i]->avg_quat[2];
    q1[3] = m_Grains[i]->avg_quat[3];
    q1[4] = m_Grains[i]->avg_quat[4];
	phase1 = crystruct[m_Grains[i]->phase];
    size_t size = 0;
    if (NULL != nlist)
    {
      size = nlist->size();
    }
    for (size_t j = 0; j < size; j++)
    {
	  w = 10000.0;
      int nname = nlist->at(j);
      double neighsurfarea = neighsurfarealist->at(j);
      q2[1] = m_Grains[nname]->avg_quat[1];
      q2[2] = m_Grains[nname]->avg_quat[2];
      q2[3] = m_Grains[nname]->avg_quat[3];
      q2[4] = m_Grains[nname]->avg_quat[4];
	  phase2 = crystruct[m_Grains[nname]->phase];
      if(phase1 == phase2) w = m_OrientatioOps[phase1]->getMisoQuat(q1,q2,n1,n2,n3);
	  OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
      if(phase1 == phase2)
	  {
		  m_Grains[i]->misorientationlist->at(3 * j) = r1;
	      m_Grains[i]->misorientationlist->at(3 * j + 1) = r2;
	      m_Grains[i]->misorientationlist->at(3 * j + 2) = r3;
	  }
	  if(phase1 != phase2)
	  {
		  m_Grains[i]->misorientationlist->at(3 * j) = -100;
	      m_Grains[i]->misorientationlist->at(3 * j + 1) = -100;
	      m_Grains[i]->misorientationlist->at(3 * j + 2) = -100;
	  }
      if (phase1 == phase2) mbin = m_OrientatioOps[phase1]->getMisoBin(m_Grains[i]->misorientationlist->at(3*j), m_Grains[i]->misorientationlist->at(3*j+1), m_Grains[i]->misorientationlist->at(3*j+2));
      if (m_Grains[i]->surfacegrain == 0 && (nname > i || m_Grains[nname]->surfacegrain == 1) && phase1 == phase2)
      {
        simmdf[m_Grains[i]->phase][mbin] = simmdf[m_Grains[i]->phase][mbin] + (neighsurfarea / totalsurfacearea[m_Grains[i]->phase]);
      }
    }
    m_Grains[i]->misorientationlist = new std::vector<double >(misolist.size());
    m_Grains[i]->misorientationlist->swap(misolist);
    misolist.clear();
  }
}

void  GrainGeneratorFunc::find_centroids()
{
//  int count = 0;
  int onedge = 0;
  size_t xtalSize = crystruct.size();
  for(size_t i=0;i<xtalSize;++i)
  {
	  maxdiameter[i]=0;
	  mindiameter[i]=100000;
  }
  double x, y, z;
  double radcubed, diameter, packquality;
  int col, row, plane;
  graincenters.resize(numgrains);
  for(int i = 0; i < numgrains; i++)
  {
    graincenters[i] = DoubleArray(new double [5]);
    for(int j=0;j<5;j++)
    {
      graincenters[i][j]=0;
    }
  }
  int totalPoints = (xpoints*ypoints*zpoints);
  for(int j = 0; j < totalPoints; ++j)
  {
      onedge = 0;
      int gnum = voxels[j].grainname;
      graincenters[gnum][0]++;
      x = find_xcoord(j);
      y = find_ycoord(j);
      z = find_zcoord(j);
      col = j%xpoints;
      row = (j/xpoints)%ypoints;
      plane = j/(xpoints*ypoints);
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
  int phase;
  for(int i=1;i<numgrains;i++)
  {
    graincenters[i][1] = graincenters[i][1]/graincenters[i][0];
    graincenters[i][2] = graincenters[i][2]/graincenters[i][0];
    graincenters[i][3] = graincenters[i][3]/graincenters[i][0];
    m_Grains[i]->centroidx = graincenters[i][1];
    m_Grains[i]->centroidy = graincenters[i][2];
    m_Grains[i]->centroidz = graincenters[i][3];
    m_Grains[i]->numvoxels = graincenters[i][0];
    m_Grains[i]->volume = (graincenters[i][0]*resx*resy*resz);
    m_Grains[i]->surfacegrain = graincenters[i][4];
    radcubed = (0.75*m_Grains[i]->volume)/m_pi;
    diameter = (2*pow(radcubed,0.3333333333));
    packquality = (diameter-m_Grains[i]->equivdiameter)/m_Grains[i]->equivdiameter;
    m_Grains[i]->equivdiameter = diameter;
    m_Grains[i]->packquality = packquality;
	  phase = m_Grains[i]->phase;
    if((diameter) > maxdiameter[phase]) maxdiameter[phase] = (diameter);
    if(diameter < mindiameter[phase]) mindiameter[phase] = diameter;
  }
}
void  GrainGeneratorFunc::find_moments ()
{
//  int count = 0;
  double u200=0;
  double u020=0;
  double u002=0;
  double u110=0;
  double u011=0;
  double u101=0;
  grainmoments.resize(numgrains);
  for(int i = 0; i < numgrains; i++)
  {
  grainmoments[i] = DoubleArray(new double [6]);
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
  double vol5 = m_Grains[i]->volume;
  vol5 = pow(vol5,5);
  double omega3 = vol5/o3;
  m_Grains[i]->Ixx = grainmoments[i][0];
  m_Grains[i]->Iyy = grainmoments[i][1];
  m_Grains[i]->Izz = grainmoments[i][2];
  m_Grains[i]->Ixy = -grainmoments[i][3];
  m_Grains[i]->Iyz = -grainmoments[i][4];
  m_Grains[i]->Ixz = -grainmoments[i][5];
  m_Grains[i]->omega3 = omega3;
  }
}
void  GrainGeneratorFunc::find_axes ()
{
  for (int i = 1; i < numgrains; i++)
  {
    double Ixx = m_Grains[i]->Ixx;
    double Iyy = m_Grains[i]->Iyy;
    double Izz = m_Grains[i]->Izz;
    double Ixy = m_Grains[i]->Ixy;
    double Iyz = m_Grains[i]->Iyz;
    double Ixz = m_Grains[i]->Ixz;
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
  m_Grains[i]->radius1 = r1;
  m_Grains[i]->radius2 = r2;
  m_Grains[i]->radius3 = r3;
  }
}
void  GrainGeneratorFunc::find_vectors (H5ReconStatsWriter::Pointer h5io)
{
  size_t xtalSize = crystruct.size();
	totalaxes.resize(xtalSize);
  double **axisodf;
  axisodf = new double *[xtalSize];
  for(size_t i=0;i<xtalSize;++i)
  {
	  totalaxes[i] = 0.0;
	  //FIXME: Leaking memory because this was already allocated in the "initializeArrays" method
	  axisodf[i] = new double [18*18*18];
  }
  for(int i = 1; i < numgrains; i++)
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
  if(m_Grains[i]->surfacegrain == 0)
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
        m_Grains[i]->axiseuler1 = ea1;
        m_Grains[i]->axiseuler2 = ea2;
        m_Grains[i]->axiseuler3 = ea3;
		int phase = m_Grains[i]->phase;
        bin = (ea3bin*18*18)+(ea2bin*18)+(ea1bin);
        axisodf[phase][bin] = axisodf[phase][bin] + 1.0;
        totalaxes[phase] = totalaxes[phase]+1.0;
      }
    }
    }
  }
  xtalSize = crystruct.size();
  for(size_t i=0;i<xtalSize;++i)
  {
	  int err;
	  err = h5io->writeAxisOrientationData(i, axisodf[i], totalaxes[i]);
	  delete [] axisodf[i];
  }
  delete [] axisodf;
}
int GrainGeneratorFunc::volume_stats(H5ReconStatsWriter::Pointer h5io)
{
  int retErr = 0;
  double actualgrains = 0;
  double avglogdiam = 0;
  int numbins;
  size_t xtalSize = crystruct.size();
  for(size_t iter=0;iter< xtalSize; ++iter)
  {
	  numbins = ((maxdiameter[iter]-mindiameter[iter])/binstepsize[iter])+1;
	  neighborhood[iter].resize(numbins);
	  neighborhoodfit[iter].resize(numbins);
	  svbovera[iter].resize(numbins);
	  svcovera[iter].resize(numbins);
	  svcoverb[iter].resize(numbins);
	  svschmid[iter].resize(numbins);
	  svomega3[iter].resize(numbins);
	  for (int temp = 0; temp < numbins; temp++)
	  {
		neighborhood[iter][temp].resize(7, 0);
		neighborhoodfit[iter][temp].resize(4, 0);
		svbovera[iter][temp].resize(5, 0);
		svcovera[iter][temp].resize(5, 0);
		svcoverb[iter][temp].resize(5, 0);
		svschmid[iter][temp].resize(5, 0);
		svomega3[iter][temp].resize(5, 0);
	  }
	  for (int i = 1; i < numgrains; i++)
    {
      int onedge = m_Grains[i]->surfacegrain;
      if (onedge == 0 && m_Grains[i]->phase == static_cast<int>(iter) )
      {
        actualgrains++;
     //   int vol = m_Grains[i]->numvoxels;
      //  double voxvol = vol * resx * resy * resz;
        //  double logvol = log(voxvol);
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
        avglogdiam = avglogdiam + logdiam;
        int diamint = int((diam - mindiameter[iter]) / binstepsize[iter]);
        neighborhood[iter][diamint][0]++;
        svbovera[iter][diamint][0]++;
        svcovera[iter][diamint][0]++;
        svcoverb[iter][diamint][0]++;
        svschmid[iter][diamint][0]++;
        svomega3[iter][diamint][0]++;
        svbovera[iter][diamint][1] = svbovera[iter][diamint][1] + bovera;
        svcovera[iter][diamint][1] = svcovera[iter][diamint][1] + covera;
        svcoverb[iter][diamint][1] = svcoverb[iter][diamint][1] + coverb;
        svschmid[iter][diamint][1] = svschmid[iter][diamint][1] + schmid;
        svomega3[iter][diamint][1] = svomega3[iter][diamint][1] + omega3;
        neighborhood[iter][diamint][0]++;
        for (int k = 0; k < 3; k++)
        {
          int nnum = m_Grains[i]->neighbordistfunc[k];
          neighborhood[iter][diamint][((2 * k) + 1)] = neighborhood[iter][diamint][((2 * k) + 1)] + nnum;
        }
      }
    }
	  for (int temp3 = 0; temp3 < numbins; temp3++)
	  {
      if (svbovera[iter][temp3][0] > 1)
      {
        neighborhood[iter][temp3][1] = neighborhood[iter][temp3][1] / neighborhood[iter][temp3][0];
        neighborhood[iter][temp3][3] = neighborhood[iter][temp3][3] / neighborhood[iter][temp3][0];
        neighborhood[iter][temp3][5] = neighborhood[iter][temp3][5] / neighborhood[iter][temp3][0];
        svbovera[iter][temp3][1] = svbovera[iter][temp3][1] / svbovera[iter][temp3][0];
        svcovera[iter][temp3][1] = svcovera[iter][temp3][1] / svcovera[iter][temp3][0];
        svcoverb[iter][temp3][1] = svcoverb[iter][temp3][1] / svcoverb[iter][temp3][0];
        svschmid[iter][temp3][1] = svschmid[iter][temp3][1] / svschmid[iter][temp3][0];
        svomega3[iter][temp3][1] = svomega3[iter][temp3][1] / svomega3[iter][temp3][0];
        neighborhoodfit[iter][temp3][0] = neighborhood[iter][temp3][0];
        neighborhoodfit[iter][temp3][1] = neighborhood[iter][temp3][3] - neighborhood[iter][temp3][1];
        neighborhoodfit[iter][temp3][2] = neighborhood[iter][temp3][1];
        neighborhoodfit[iter][temp3][3] = log((neighborhood[iter][temp3][5]-neighborhoodfit[iter][temp3][2])/neighborhoodfit[iter][temp3][1])/log(2.0);
      }
	  }
	  avglogdiam = avglogdiam / actualgrains;
	  double sdlogdiam = 0;
	  for (int j = 1; j < numgrains; j++)
    {
      int onedge = m_Grains[j]->surfacegrain;
      if (onedge == 0 && m_Grains[j]->phase == static_cast<int>(iter) )
      {
        int vol = m_Grains[j]->numvoxels;
        double voxvol = vol * resx * resy * resz;
        //  double logvol = log(voxvol);
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
        sdlogdiam = sdlogdiam + ((logdiam - avglogdiam) * (logdiam - avglogdiam));
        int diamint = int((diam - mindiameter[iter]) / binstepsize[iter]);
        svbovera[iter][diamint][2] = svbovera[iter][diamint][2] + ((bovera - svbovera[iter][diamint][1]) * (bovera - svbovera[iter][diamint][1]));
        svcovera[iter][diamint][2] = svcovera[iter][diamint][2] + ((covera - svcovera[iter][diamint][1]) * (covera - svcovera[iter][diamint][1]));
        svcoverb[iter][diamint][2] = svcoverb[iter][diamint][2] + ((coverb - svcoverb[iter][diamint][1]) * (coverb - svcoverb[iter][diamint][1]));
        svschmid[iter][diamint][2] = svschmid[iter][diamint][2] + ((schmid - svschmid[iter][diamint][1]) * (schmid - svschmid[iter][diamint][1]));
        svomega3[iter][diamint][2] = svomega3[iter][diamint][2] + ((omega3 - svomega3[iter][diamint][1]) * (omega3 - svomega3[iter][diamint][1]));
        for (int k = 0; k < 3; k++)
        {
          int nnum = m_Grains[j]->neighbordistfunc[k];
          neighborhood[iter][diamint][((2 * k) + 2)] = neighborhood[iter][diamint][((2 * k) + 2)] + ((neighborhood[iter][diamint][((2 * k) + 1)] - nnum)
              * (neighborhood[iter][diamint][((2 * k) + 1)] - nnum));
        }
      }
    }
	  for (int temp4 = 0; temp4 < numbins; temp4++)
	  {
		if (svbovera[iter][temp4][0] > 1)
		{
		  neighborhood[iter][temp4][2] = neighborhood[iter][temp4][2] / neighborhood[iter][temp4][0];
		  neighborhood[iter][temp4][4] = neighborhood[iter][temp4][4] / neighborhood[iter][temp4][0];
		  neighborhood[iter][temp4][6] = neighborhood[iter][temp4][6] / neighborhood[iter][temp4][0];
		  svbovera[iter][temp4][2] = svbovera[iter][temp4][2] / svbovera[iter][temp4][0];
		  svcovera[iter][temp4][2] = svcovera[iter][temp4][2] / svcovera[iter][temp4][0];
		  svcoverb[iter][temp4][2] = svcoverb[iter][temp4][2] / svcoverb[iter][temp4][0];
		  svschmid[iter][temp4][2] = svschmid[iter][temp4][2] / svschmid[iter][temp4][0];
		  svomega3[iter][temp4][2] = svomega3[iter][temp4][2] / svomega3[iter][temp4][0];
		  svbovera[iter][temp4][3] = svbovera[iter][temp4][1] * (((svbovera[iter][temp4][1] * (1 - svbovera[iter][temp4][1])) / svbovera[iter][temp4][2]) - 1);
		  svbovera[iter][temp4][4] = (1 - svbovera[iter][temp4][1]) * (((svbovera[iter][temp4][1] * (1 - svbovera[iter][temp4][1])) / svbovera[iter][temp4][2]) - 1);
		  svcovera[iter][temp4][3] = svcovera[iter][temp4][1] * (((svcovera[iter][temp4][1] * (1 - svcovera[iter][temp4][1])) / svcovera[iter][temp4][2]) - 1);
		  svcovera[iter][temp4][4] = (1 - svcovera[iter][temp4][1]) * (((svcovera[iter][temp4][1] * (1 - svcovera[iter][temp4][1])) / svcovera[iter][temp4][2]) - 1);
		  svcoverb[iter][temp4][3] = svcoverb[iter][temp4][1] * (((svcoverb[iter][temp4][1] * (1 - svcoverb[iter][temp4][1])) / svcoverb[iter][temp4][2]) - 1);
		  svcoverb[iter][temp4][4] = (1 - svcoverb[iter][temp4][1]) * (((svcoverb[iter][temp4][1] * (1 - svcoverb[iter][temp4][1])) / svcoverb[iter][temp4][2]) - 1);
		  svomega3[iter][temp4][3] = svomega3[iter][temp4][1] * (((svomega3[iter][temp4][1] * (1 - svomega3[iter][temp4][1])) / svomega3[iter][temp4][2]) - 1);
		  svomega3[iter][temp4][4] = (1 - svomega3[iter][temp4][1]) * (((svomega3[iter][temp4][1] * (1 - svomega3[iter][temp4][1])) / svomega3[iter][temp4][2]) - 1);
		  neighborhood[iter][temp4][2] = pow(neighborhood[iter][temp4][2], 0.5);
		  neighborhood[iter][temp4][4] = pow(neighborhood[iter][temp4][4], 0.5);
		  neighborhood[iter][temp4][6] = pow(neighborhood[iter][temp4][6], 0.5);
		  svbovera[iter][temp4][2] = pow(svbovera[iter][temp4][2], 0.5);
		  svcovera[iter][temp4][2] = pow(svcovera[iter][temp4][2], 0.5);
		  svcoverb[iter][temp4][2] = pow(svcoverb[iter][temp4][2], 0.5);
		  svschmid[iter][temp4][2] = pow(svschmid[iter][temp4][2], 0.5);
		  svomega3[iter][temp4][2] = pow(svomega3[iter][temp4][2], 0.5);
		}
	  }
	  sdlogdiam = sdlogdiam / actualgrains;
	  sdlogdiam = pow(sdlogdiam, 0.5);

	  retErr = h5io->writeVolumeStats(iter, crystruct[iter], phasefraction[iter], maxdiameter[iter], mindiameter[iter], binstepsize[iter], avglogdiam, sdlogdiam,
									  svbovera[iter], svcovera[iter], svcoverb[iter], neighborhoodfit[iter], svomega3[iter]);
  }
  return retErr;
}

#if 0
void  GrainGeneratorFunc::writeCube(const std::string &filename, int numgrains)
{
  ofstream outFile;
  outFile.open(filename.c_str());
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
//  int name = voxels[i].grainname;
  if(i%20 == 0 && i > 0) outFile << endl;
//    if(name <= numgrains) outFile << "       1";
//    if(name > numgrains) outFile << "       2";
  outFile << voxels[i].unassigned << "  ";
  }
  outFile.close();
}
#endif

void GrainGeneratorFunc::write_eulerangles(const std::string &filename)
{
  //std::cout << "GrainGeneratorFunc::write_volume1: '" << writename10 << "'" << std::endl;
  ofstream outFile;
  outFile.open(filename.c_str());
  for (int i = 1; i < numgrains; i++)
  {
    double ea1 = m_Grains[i]->euler1;
    double ea2 = m_Grains[i]->euler2;
    double ea3 = m_Grains[i]->euler3;
    outFile << i << " " << ea1 << " " << ea2 << " " << ea3 << endl;
  }
  outFile.close();
}

void GrainGeneratorFunc::write_graindata(const std::string &filename)
{
  double ea1, ea2, ea3;
  double packquality, equivdiam;
  int onsurface, numneighbors;
  ofstream outFile;
  outFile.open(filename.c_str());
  outFile << numgrains << endl;
  for(int i = 1; i < numgrains; i++)
  {
  equivdiam = m_Grains[i]->equivdiameter;
  packquality = m_Grains[i]->packquality;
  onsurface = m_Grains[i]->surfacegrain;
  ea1 = m_Grains[i]->euler1;
  ea2 = m_Grains[i]->euler2;
  ea3 = m_Grains[i]->euler3;
  numneighbors = m_Grains[i]->numneighbors;
  outFile << i << " " << equivdiam << " " << packquality << " " << numneighbors << "  " << onsurface << " " << ea1 << " " << ea2 << " " << ea3 << endl;
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

