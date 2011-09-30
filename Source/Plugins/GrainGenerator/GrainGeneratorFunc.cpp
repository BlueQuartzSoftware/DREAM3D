/* ============================================================================
 * Copyright (c) 2009, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2009, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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


#include "EbsdLib/EbsdConstants.h"

// DREAM3D Includes
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "GrainGenerator/Algorithms/PackGrainsGen2.h"



#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();


//const static float m_onepointthree = 1.33333333333f;
const static float m_pi = M_PI;
//const static float m_one_over_pi = 1.0/m_pi;
const static float SinOfHalf = sinf(0.5f);
const static float CosOfHalf = cosf(0.5f);
//const static float SinOfZero = sinf(0.0f);
//const static float CosOfZero = cosf(0.0f);


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
GrainGeneratorFunc::GrainGeneratorFunc()
{
  m_HexOps = HexagonalOps::New();
  m_OrientatioOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientatioOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientatioOps.push_back(m_OrthoOps.get());

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

//  voxels.reset(NULL);
  GGseed = MXA::getMilliSeconds();


// Just stuff to quiet the compiler
  float a = SinOfHalf;
  a = CosOfHalf;

  grain_indicies = NULL;
  ellipfuncs = NULL;
  phases = NULL;
  euler1s = NULL;
  euler2s = NULL;
  euler3s = NULL;
  surfacevoxels = NULL;
  totalsurfacearea = NULL;


INIT_AIMARRAY(m_GrainIndicies,int);
INIT_AIMARRAY(m_Ellipfuncs,float);
INIT_AIMARRAY(m_Phases,int);
INIT_AIMARRAY(m_Euler1s,float);
INIT_AIMARRAY(m_Euler2s,float);
INIT_AIMARRAY(m_Euler3s,float);
INIT_AIMARRAY(m_SurfaceVoxels,char);
INIT_AIMARRAY(m_TotalSurfaceArea,float);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorFunc::~GrainGeneratorFunc()
{

}

#define GG_INIT_DOUBLE_ARRAY(array, value, size)\
    for(size_t n = 0; n < size; ++n) { array[n] = (value); }

void GrainGeneratorFunc::initializeAttributes()
{

  const size_t startIndex = 0;
  const size_t endIndex = totalpoints;
  grain_indicies = m_GrainIndicies->WritePointer(startIndex, endIndex);
  ellipfuncs = m_Ellipfuncs->WritePointer(startIndex, endIndex);
  phases = m_Phases->WritePointer(startIndex, endIndex);
  euler1s = m_Euler1s->WritePointer(startIndex, endIndex);
  euler2s = m_Euler2s->WritePointer(startIndex, endIndex);
  euler3s = m_Euler3s->WritePointer(startIndex, endIndex);
  surfacevoxels = m_SurfaceVoxels->WritePointer(startIndex, endIndex);

	for(int i=0;i<totalpoints;i++)
	{
		grain_indicies[i] = 0;
		ellipfuncs[i] = 0;
		phases[i] = 0;
		euler1s[i] = -1;
		euler2s[i] = -1;
		euler3s[i] = -1;
		surfacevoxels[i] = 0;
	}
}
void GrainGeneratorFunc::initializeArrays(std::vector<Ebsd::CrystalStructure> structures)
{
  //------------------
  size_t nElements = 0;
  size_t size = structures.size();

  crystruct.resize(size+1);
  pptFractions.resize(size + 1);
  phaseType.resize(size+1);
  phasefraction.resize(size+1);

  // Initialize the first slot in these arrays since they should never be used
  crystruct[0] = Ebsd::UnknownCrystalStructure;
  phasefraction[0] = 0.0;
  phaseType[0] = DREAM3D::Reconstruction::UnknownPhaseType;
  pptFractions[0] = -1.0;

  mindiameter.resize(size+1);
  maxdiameter.resize(size+1);
  binstepsize.resize(size+1);
  numdiameterbins.resize(size+1);
  avgdiam.resize(size+1);
  sddiam.resize(size+1);
  bovera.resize(size+1);
  covera.resize(size+1);
  coverb.resize(size+1);
  omega3.resize(size+1);
  neighborparams.resize(size+1);

  actualodf.resize(size+1);
  simodf.resize(size+1);
  actualmdf.resize(size+1);
  simmdf.resize(size+1);
  axisodf.resize(size+1);
  for(size_t i= 1; i < size+1; ++i)
  {
    if(crystruct[i] == Ebsd::Hexagonal) nElements = 36*36*12;
    if(crystruct[i] == Ebsd::Cubic) nElements = 18*18*18;

    float initValue = 1.0/(float)(nElements);
    actualodf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(actualodf[i], initValue, nElements);

    simodf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(simodf[i], 0.0, nElements);
    actualmdf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(actualmdf[i], initValue, nElements);
    simmdf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(simmdf[i], 0.0, nElements);

    nElements = 36*36*36;
    initValue = (1.0/float(nElements));
    axisodf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(axisodf[i], initValue, nElements);
  }
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
  std::vector<float> col0;\
  std::vector<float> col1;\
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
  std::vector<float> col0;\
  std::vector<float> col1;\
  std::vector<float> col2;\
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

int GrainGeneratorFunc::readReconStatsData(H5StatsReader::Pointer h5io)
{
  int err = -1;
  std::vector<float> grainDiamInfo;
  std::vector<float> double_data;
  std::string path;

  // Read the Phase and Crystal Structure information from the Stats File
  std::vector<int> phases;
  std::vector<Ebsd::CrystalStructure> structures;
  err = h5io->getPhaseAndCrystalStructures(phases, structures);
  if (err < 0)
  {
    return err;
  }

  // Now that we have that information - initialize the arrays
  initializeArrays(structures);
  int size = phases.size();

  int phase = -1;

  for (int i = 0; i < size; i++)
  {
      phase = phases[i];
    crystruct[phase] = structures[i];

    /* Read the PhaseFraction Value*/
      std::vector<float> pFraction;
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::PhaseFraction, pFraction);
    phasefraction[phase] = pFraction.front();

    std::vector<unsigned int> phasetypes;
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::PhaseType, phasetypes);
    phaseType[phase] = static_cast<DREAM3D::Reconstruction::PhaseType>(phasetypes[0]);

    // If the Phase Type is Precipitate then we need the pptFraction on Boundary
    if (phaseType[phase] == DREAM3D::Reconstruction::PrecipitatePhase)
    {
      float f = -1.0f;
      err = h5io->readScalarAttribute(phase, DREAM3D::HDF5::PhaseType, DREAM3D::HDF5::PrecipitateBoundaryFraction, f);
      if (err < 0) {
        f = -1.0f;
      }
      pptFractions[phase] = f;
    }
    if (phaseType[phase] != DREAM3D::Reconstruction::PrecipitatePhase) pptFractions[phase] = -1.0;

    /* Read the BinNumbers data set */
    std::vector<float> bins;
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::BinNumber, bins);
    CHECK_STATS_READ_ERROR(err, DREAM3D::HDF5::Statistics, DREAM3D::HDF5::BinNumber)
    numdiameterbins[phase] = bins.size();
    size_t nBins = bins.size();

    /* Read the Grain_Diameter_Info Data */
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::Grain_Diameter_Info, grainDiamInfo);
    CHECK_STATS_READ_ERROR(err,  DREAM3D::HDF5::Statistics, DREAM3D::HDF5::Grain_Diameter_Info)

    binstepsize[phase] = grainDiamInfo[0];
    maxdiameter[phase]  = grainDiamInfo[1];
    mindiameter[phase] = grainDiamInfo[2];

    /* Read the Grain_Size_Distribution Data */
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::Grain_Size_Distribution, double_data);
    CHECK_STATS_READ_ERROR(err,  DREAM3D::HDF5::Statistics, DREAM3D::HDF5::Grain_Size_Distribution)
    avgdiam[phase] = double_data[0];
    sddiam[phase] = double_data[1];

    DREAM3D::Reconstruction::DistributionType dt;
    std::string disType;

    /* Read the Shape Data */
    READ_2_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVBoverA_Distributions, bovera, DREAM3D::Reconstruction::Beta, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::BetaColumnCount);
    READ_2_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVCoverA_Distributions, covera, DREAM3D::Reconstruction::Beta, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::BetaColumnCount);
    READ_2_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVCoverB_Distributions, coverb, DREAM3D::Reconstruction::Beta, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::BetaColumnCount);

    /* Read the Omega3 Data */
    READ_2_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVOmega3_Distributions, omega3, DREAM3D::Reconstruction::Beta, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::BetaColumnCount);

    /* Read the Neighbor Data - This MUST be the last one because of how variables are assigned bvalues and used in the next section */
    READ_3_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions, neighborparams, DREAM3D::Reconstruction::Power, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::Exp_k, DREAM3D::HDF5::PowerLawColumnCount);
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
int  GrainGeneratorFunc::readAxisOrientationData(H5StatsReader::Pointer h5io)
{
  std::vector<float> density;
  int err = 0;
  float totaldensity = 0.0;
  size_t size = 0;
  // Read the Phase and Crystal Structure information from the Stats File
  std::vector<int> phases;
  std::vector<Ebsd::CrystalStructure> structures;
  err = h5io->getPhaseAndCrystalStructures(phases, structures);
  if (err < 0)
  {
    return err;
  }
  int phase = -1;
  size_t count = phases.size();
  for(size_t i = 0; i< count ;i++)
  {
    totaldensity = 0.0;
      phase = phases[i];
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::AxisOrientation, density);
    if (err < 0)
    {
    GGF_CHECK_READ_ERROR(readAxisOrientationData, DREAM3D::HDF5::AxisOrientation)
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
    axisodf[phase][k] = totaldensity;
    }
  }
  return err;
}

int GrainGeneratorFunc::readODFData(H5StatsReader::Pointer h5io)
{
  std::vector<float> density;
  int err = 0;
  // Read the Phase and Crystal Structure information from the Stats File
  std::vector<int> phases;
  std::vector<Ebsd::CrystalStructure> structures;
  err = h5io->getPhaseAndCrystalStructures(phases, structures);
  if (err < 0)
  {
    return err;
  }
  int phase = -1;
  size_t size = phases.size();
  for(size_t i = 0; i< size ;i++)
  {
      phase = phases[i];
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::ODF, density);
    if (err < 0)
    {
    GGF_CHECK_READ_ERROR(readODFData, DREAM3D::HDF5::ODF)
    //FIXME: This should probably return an ERROR because nothing was read
    return 10;
    }
    size_t numbins = 0;
    if(crystruct[phase] == Ebsd::Hexagonal) numbins = 36*36*12;
    if(crystruct[phase] == Ebsd::Cubic) numbins = 18*18*18;

    if (numbins != density.size() )
    {
    std::cout << "GrainGeneratorFunc::readODFData Error: Mismatch in number of elements in the 'ODF' "
       << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
       << " elements and we need " << numbins << " Elements. "<< std::endl;
    return -1;
    }
    for (size_t j=0;j<numbins;j++)
    {
    actualodf[phase][j] = density[j];
    }
  }
  return err;
}

int GrainGeneratorFunc::readMisorientationData(H5StatsReader::Pointer h5io)
{
  std::vector<float> density;
  int err = 0;
  // Read the Phase and Crystal Structure information from the Stats File
  std::vector<int> phases;
  std::vector<Ebsd::CrystalStructure> structures;
  err = h5io->getPhaseAndCrystalStructures(phases, structures);
  if (err < 0)
  {
    return err;
  }
  int phase = -1;
  size_t size = phases.size();
  for(size_t i = 0; i< size ;i++)
  {
      phase = phases[i];
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::MisorientationBins, density);
    if (err < 0)
    {
    GGF_CHECK_READ_ERROR(readMisorientationData, DREAM3D::HDF5::MisorientationBins)
     //FIXME: This should probably return an ERROR because nothing was read
    return 10;
    }
    size_t numbins = 0;
    if(crystruct[phase] == Ebsd::Hexagonal) numbins = 36*36*12;
    if(crystruct[phase] == Ebsd::Cubic) numbins = 18*18*18;

    if (numbins != density.size() )
    {
    std::cout << "GrainGeneratorFunc::readMisorientationData Error: Mismatch in number of elements in the 'ODF' "
       << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
       << " elements and we need " << numbins << " Elements. "<< std::endl;
    return -1;
    }

    for (size_t k = 0; k < numbins; k++)
    {
    actualmdf[phase][k] = density[k];
    }
  }
  return err;
}



void GrainGeneratorFunc::insert_precipitate(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW()

  float dist;
  float inside = -1;
  int index;
  int column, row, plane;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float xp, yp, zp;
  float x, y, z;
  float insidecount = 0;
  std::vector<int> insidelist(1000,-1);
  float volcur = m_Grains[gnum]->volume;
  float bovera = m_Grains[gnum]->radius2;
  float covera = m_Grains[gnum]->radius3;
  float omega3 = m_Grains[gnum]->omega3;
  float radcur1 = 1;
  DREAM3D::SyntheticBuilder::ShapeType shapeclass = shapeTypes[m_Grains[gnum]->phase];

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

  float radcur2 = (radcur1*bovera);
  float radcur3 = (radcur1*covera);
  float phi1 = m_Grains[gnum]->axiseuler1;
  float PHI = m_Grains[gnum]->axiseuler2;
  float phi2 = m_Grains[gnum]->axiseuler3;
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
  if(periodic_boundaries == true)
  {
    if(xmin < -xpoints) xmin = -xpoints;
    if(xmax > 2*xpoints-1) xmax = (2*xpoints-1);
    if(ymin < -ypoints) ymin = -ypoints;
    if(ymax > 2*ypoints-1) ymax = (2*ypoints-1);
    if(zmin < -zpoints) zmin = -zpoints;
    if(zmax > 2*zpoints-1) zmax = (2*zpoints-1);
  }
  if(periodic_boundaries == false)
  {
    if(xmin < 0) xmin = 0;
    if(xmax > xpoints-1) xmax = xpoints-1;
    if(ymin < 0) ymin = 0;
    if(ymax > ypoints-1) ymax = ypoints-1;
    if(zmin < 0) zmin = 0;
    if(zmax > zpoints-1) zmax = zpoints-1;
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
		  if(iter1 < 0) column = iter1+xpoints;
		  if(iter1 > xpoints-1) column = iter1-xpoints;
		  if(iter2 < 0) row = iter2+ypoints;
		  if(iter2 > ypoints-1) row = iter2-ypoints;
		  if(iter3 < 0) plane = iter3+zpoints;
		  if(iter3 > zpoints-1) plane = iter3-zpoints;
		  index = (plane*xpoints*ypoints)+(row*xpoints)+column;
		  inside = -1;
		  x = float(column)*resx;
		  y = float(row)*resy;
		  z = float(plane)*resz;
		  if(iter1 < 0) x = x-sizex;
		  if(iter1 > xpoints-1) x = x+sizex;
		  if(iter2 < 0) y = y-sizey;
		  if(iter2 > ypoints-1) y = y+sizey;
		  if(iter3 < 0) z = z-sizez;
		  if(iter3 > zpoints-1) z = z+sizez;
		  dist = ((x-xc)*(x-xc))+((y-yc)*(y-yc))+((z-zc)*(z-zc));
		  dist = sqrt(dist);
		  if(dist < radcur1)
		  {
			x = x-xc;
			y = y-yc;
			z = z-zc;
			xp = (x*ga[0][0])+(y*ga[1][0])+(z*ga[2][0]);
			yp = (x*ga[0][1])+(y*ga[1][1])+(z*ga[2][1]);
			zp = (x*ga[0][2])+(y*ga[1][2])+(z*ga[2][2]);
			float axis1comp = xp/radcur1;
			float axis2comp = yp/radcur2;
			float axis3comp = zp/radcur3;
		    inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
			if(inside >= 0)
			{
				int currentpoint = index;
				insidelist[insidecount] = currentpoint;
				insidecount++;
				if (insidecount >= (insidelist.size()))
				{
				  insidelist.resize(insidecount + 1000,-1);
				}
			}
		  }
		}
    }
  }
  insidelist.resize(insidecount);
  // Initialize a new Voxel List if necessary
  if (m_Grains[gnum]->voxellist == NULL )
  {
    m_Grains[gnum]->voxellist = new std::vector<int>(0);
  }
  m_Grains[gnum]->voxellist->assign(insidelist.begin(), insidelist.end());
}




void GrainGeneratorFunc::assign_voxels()
{
  int index;
  int neighpoints[6];
  neighpoints[0] = -(xpoints * ypoints);
  neighpoints[1] = -xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xpoints;
  neighpoints[5] = (xpoints * ypoints);
  int oldname;
  int column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;
  int xmin, xmax, ymin, ymax, zmin, zmax;

  initializeAttributes();
  gsizes.resize(m_Grains.size());

  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    gsizes[i] = 0;
  }
  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    float volcur = m_Grains[i]->volume;
    float bovera = m_Grains[i]->radius2;
    float covera = m_Grains[i]->radius3;
    float omega3 = m_Grains[i]->omega3;
    xc = m_Grains[i]->centroidx;
    yc = m_Grains[i]->centroidy;
    zc = m_Grains[i]->centroidz;
    float radcur1 = 0.0f;
    //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
    DREAM3D::SyntheticBuilder::ShapeType shapeclass = shapeTypes[m_Grains[i]->phase];

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

    float radcur2 = (radcur1 * bovera);
    float radcur3 = (radcur1 * covera);
    float phi1 = m_Grains[i]->axiseuler1;
    float PHI = m_Grains[i]->axiseuler2;
    float phi2 = m_Grains[i]->axiseuler3;
    float ga[3][3];
    ga[0][0] = cosf(phi1) * cosf(phi2) - sinf(phi1) * sinf(phi2) * cosf(PHI);
    ga[0][1] = sinf(phi1) * cosf(phi2) + cosf(phi1) * sinf(phi2) * cosf(PHI);
    ga[0][2] = sinf(phi2) * sinf(PHI);
    ga[1][0] = -cosf(phi1) * sinf(phi2) - sinf(phi1) * cosf(phi2) * cosf(PHI);
    ga[1][1] = -sinf(phi1) * sinf(phi2) + cosf(phi1) * cosf(phi2) * cosf(PHI);
    ga[1][2] = cosf(phi2) * sinf(PHI);
    ga[2][0] = sinf(phi1) * sinf(PHI);
    ga[2][1] = -cosf(phi1) * sinf(PHI);
    ga[2][2] = cosf(PHI);
    column = (xc - (resx / 2)) / resx;
    row = (yc - (resy / 2)) / resy;
    plane = (zc - (resz / 2)) / resz;
    xmin = int(column - ((radcur1 / resx) + 1));
    xmax = int(column + ((radcur1 / resx) + 1));
    ymin = int(row - ((radcur1 / resy) + 1));
    ymax = int(row + ((radcur1 / resy) + 1));
    zmin = int(plane - ((radcur1 / resz) + 1));
    zmax = int(plane + ((radcur1 / resz) + 1));
    if (periodic_boundaries == true)
    {
      if (xmin < -xpoints) xmin = -xpoints;
      if (xmax > 2 * xpoints - 1) xmax = (2 * xpoints - 1);
      if (ymin < -ypoints) ymin = -ypoints;
      if (ymax > 2 * ypoints - 1) ymax = (2 * ypoints - 1);
      if (zmin < -zpoints) zmin = -zpoints;
      if (zmax > 2 * zpoints - 1) zmax = (2 * zpoints - 1);
    }
    if (periodic_boundaries == false)
    {
      if (xmin < 0) xmin = 0;
      if (xmax > xpoints - 1) xmax = xpoints - 1;
      if (ymin < 0) ymin = 0;
      if (ymax > ypoints - 1) ymax = ypoints - 1;
      if (zmin < 0) zmin = 0;
      if (zmax > zpoints - 1) zmax = zpoints - 1;
    }
    for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
    {
      for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
      {
        for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
        {
          column = iter1;
          row = iter2;
          plane = iter3;
          if (iter1 < 0) column = iter1 + xpoints;
          if (iter1 > xpoints - 1) column = iter1 - xpoints;
          if (iter2 < 0) row = iter2 + ypoints;
          if (iter2 > ypoints - 1) row = iter2 - ypoints;
          if (iter3 < 0) plane = iter3 + zpoints;
          if (iter3 > zpoints - 1) plane = iter3 - zpoints;
          index = (plane * xpoints * ypoints) + (row * xpoints) + column;
          inside = -1;
          x = float(column) * resx;
          y = float(row) * resy;
          z = float(plane) * resz;
          if (iter1 < 0) x = x - sizex;
          if (iter1 > xpoints - 1) x = x + sizex;
          if (iter2 < 0) y = y - sizey;
          if (iter2 > ypoints - 1) y = y + sizey;
          if (iter3 < 0) z = z - sizez;
          if (iter3 > zpoints - 1) z = z + sizez;
          dist = ((x - xc) * (x - xc)) + ((y - yc) * (y - yc)) + ((z - zc) * (z - zc));
          dist = sqrtf(dist);
          if (dist < radcur1)
          {
            x = x - xc;
            y = y - yc;
            z = z - zc;
            xp = (x * ga[0][0]) + (y * ga[1][0]) + (z * ga[2][0]);
            yp = (x * ga[0][1]) + (y * ga[1][1]) + (z * ga[2][1]);
            zp = (x * ga[0][2]) + (y * ga[1][2]) + (z * ga[2][2]);
            float axis1comp = xp / radcur1;
            float axis2comp = yp / radcur2;
            float axis3comp = zp / radcur3;
            inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
            if (inside >= 0)
            {
              int currentpoint = index;
              if (grain_indicies[currentpoint] > 0)
              {
                oldname = grain_indicies[currentpoint];
                gsizes[oldname] = gsizes[oldname] - 1;
                grain_indicies[currentpoint] = -1;
              }
              if (grain_indicies[currentpoint] == 0)
              {
                grain_indicies[currentpoint] = i;
                gsizes[i]++;
              }
            }
          }
        }
      }
    }
  }
  newnames.resize(m_Grains.size());
  int goodcount = 1;
  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    newnames[i] = 0;
    if (gsizes[i] > 0)
    {
      m_Grains[goodcount] = m_Grains[i];
      newnames[i] = goodcount;
      goodcount++;
    }
  }
  for (int i = 0; i < totalpoints; i++)
  {
    if (grain_indicies[i] > 0)
    {
	  grain_indicies[i] = newnames[grain_indicies[i]];
    }
  }
  m_Grains.resize(goodcount);
}
void GrainGeneratorFunc::assign_gaps()
{
  int index;
  int timestep = 100;
  int unassignedcount = 1;
  int column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;
  int xmin, xmax, ymin, ymax, zmin, zmax;

  while (unassignedcount != 0)
  {
	  unassignedcount = 0;
	  timestep = timestep + 10;
	  for (size_t i = 1; i < m_Grains.size(); i++)
	  {
		float volcur = m_Grains[i]->volume;
		float bovera = m_Grains[i]->radius2;
		float covera = m_Grains[i]->radius3;
		float omega3 = m_Grains[i]->omega3;
		xc = m_Grains[i]->centroidx;
		yc = m_Grains[i]->centroidy;
		zc = m_Grains[i]->centroidz;
		float radcur1 = 0.0f;
		//Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
		DREAM3D::SyntheticBuilder::ShapeType shapeclass = shapeTypes[m_Grains[i]->phase];

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

		float radcur2 = (radcur1 * bovera);
		float radcur3 = (radcur1 * covera);
		radcur1 = (double(timestep)/100.0)*radcur1;
		radcur2 = (double(timestep)/100.0)*radcur2;
		radcur3 = (double(timestep)/100.0)*radcur3;
		float phi1 = m_Grains[i]->axiseuler1;
		float PHI = m_Grains[i]->axiseuler2;
		float phi2 = m_Grains[i]->axiseuler3;
		float ga[3][3];
		ga[0][0] = cosf(phi1) * cosf(phi2) - sinf(phi1) * sinf(phi2) * cosf(PHI);
		ga[0][1] = sinf(phi1) * cosf(phi2) + cosf(phi1) * sinf(phi2) * cosf(PHI);
		ga[0][2] = sinf(phi2) * sinf(PHI);
		ga[1][0] = -cosf(phi1) * sinf(phi2) - sinf(phi1) * cosf(phi2) * cosf(PHI);
		ga[1][1] = -sinf(phi1) * sinf(phi2) + cosf(phi1) * cosf(phi2) * cosf(PHI);
		ga[1][2] = cosf(phi2) * sinf(PHI);
		ga[2][0] = sinf(phi1) * sinf(PHI);
		ga[2][1] = -cosf(phi1) * sinf(PHI);
		ga[2][2] = cosf(PHI);
		column = (xc - (resx / 2)) / resx;
		row = (yc - (resy / 2)) / resy;
		plane = (zc - (resz / 2)) / resz;
		xmin = int(column - ((radcur1 / resx) + 1));
		xmax = int(column + ((radcur1 / resx) + 1));
		ymin = int(row - ((radcur1 / resy) + 1));
		ymax = int(row + ((radcur1 / resy) + 1));
		zmin = int(plane - ((radcur1 / resz) + 1));
		zmax = int(plane + ((radcur1 / resz) + 1));
		if (periodic_boundaries == true)
		{
		  if (xmin < -xpoints) xmin = -xpoints;
		  if (xmax > 2 * xpoints - 1) xmax = (2 * xpoints - 1);
		  if (ymin < -ypoints) ymin = -ypoints;
		  if (ymax > 2 * ypoints - 1) ymax = (2 * ypoints - 1);
		  if (zmin < -zpoints) zmin = -zpoints;
		  if (zmax > 2 * zpoints - 1) zmax = (2 * zpoints - 1);
		}
		if (periodic_boundaries == false)
		{
		  if (xmin < 0) xmin = 0;
		  if (xmax > xpoints - 1) xmax = xpoints - 1;
		  if (ymin < 0) ymin = 0;
		  if (ymax > ypoints - 1) ymax = ypoints - 1;
		  if (zmin < 0) zmin = 0;
		  if (zmax > zpoints - 1) zmax = zpoints - 1;
		}
		for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
		{
		  for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
		  {
			for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
			{
			  column = iter1;
			  row = iter2;
			  plane = iter3;
			  if (iter1 < 0) column = iter1 + xpoints;
			  if (iter1 > xpoints - 1) column = iter1 - xpoints;
			  if (iter2 < 0) row = iter2 + ypoints;
			  if (iter2 > ypoints - 1) row = iter2 - ypoints;
			  if (iter3 < 0) plane = iter3 + zpoints;
			  if (iter3 > zpoints - 1) plane = iter3 - zpoints;
			  index = (plane * xpoints * ypoints) + (row * xpoints) + column;
			  if(grain_indicies[index] <= 0)
			  {
				  inside = -1;
				  x = float(column) * resx;
				  y = float(row) * resy;
				  z = float(plane) * resz;
				  if (iter1 < 0) x = x - sizex;
				  if (iter1 > xpoints - 1) x = x + sizex;
				  if (iter2 < 0) y = y - sizey;
				  if (iter2 > ypoints - 1) y = y + sizey;
				  if (iter3 < 0) z = z - sizez;
				  if (iter3 > zpoints - 1) z = z + sizez;
				  dist = ((x - xc) * (x - xc)) + ((y - yc) * (y - yc)) + ((z - zc) * (z - zc));
				  dist = sqrtf(dist);
				  if (dist < radcur1)
				  {
					x = x - xc;
					y = y - yc;
					z = z - zc;
					xp = (x * ga[0][0]) + (y * ga[1][0]) + (z * ga[2][0]);
					yp = (x * ga[0][1]) + (y * ga[1][1]) + (z * ga[2][1]);
					zp = (x * ga[0][2]) + (y * ga[1][2]) + (z * ga[2][2]);
					float axis1comp = xp / radcur1;
					float axis2comp = yp / radcur2;
					float axis3comp = zp / radcur3;
					inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
					if (inside >= 0 && inside > ellipfuncs[index])
					{
						grain_indicies[index] = i;
						ellipfuncs[index] = inside;
					}
				  }
			  }
			}
		  }
		}
	  }
	  for (int i = 0; i < totalpoints; i++)
	  {
		if (grain_indicies[i] <= 0) unassignedcount++;
	  }
  }
  for (int i = 0; i < totalpoints; i++)
  {
	  if(grain_indicies[i] > 0) phases[i] = m_Grains[grain_indicies[i]]->phase;
  }
}
void GrainGeneratorFunc::cleanup_grains()
{
	  int neighpoints[6];
	  neighpoints[0] = -(xpoints * ypoints);
	  neighpoints[1] = -xpoints;
	  neighpoints[2] = -1;
	  neighpoints[3] = 1;
	  neighpoints[4] = xpoints;
	  neighpoints[5] = (xpoints * ypoints);
	  vector<vector<int> > vlists;
	  vlists.resize(m_Grains.size());
	  vector<int> currentvlist;
	  vector<bool> checked;
	  checked.resize(totalpoints,false);
	  size_t count;
	  int good;
	  int neighbor;
	  int column, row, plane;
	  int index;
	  float minsize = 0;
	  gsizes.resize(m_Grains.size());
	  for (int i = 1; i < m_Grains.size(); i++)
	  {
		gsizes[i] = 0;
	  }
	  for (int i = 0; i < totalpoints; i++)
	  {
		if(checked[i] == false && grain_indicies[i] > 0)
		{
			minsize = mindiameter[phases[i]]*mindiameter[phases[i]]*mindiameter[phases[i]]*M_PI/6.0;
			minsize = int(minsize/(resx*resy*resz));
			currentvlist.push_back(i);
			count = 0;
			while(count < currentvlist.size())
			{
				index = currentvlist[count];
				column = index % xpoints;
				row = (index / xpoints) % ypoints;
				plane = index / (xpoints * ypoints);
				for (int j = 0; j < 6; j++)
				{
					good = 1;
					neighbor = index + neighpoints[j];
					if (periodic_boundaries == false)
					{
						if (j == 0 && plane == 0) good = 0;
						if (j == 5 && plane == (zpoints - 1)) good = 0;
						if (j == 1 && row == 0) good = 0;
						if (j == 4 && row == (ypoints - 1)) good = 0;
						if (j == 2 && column == 0) good = 0;
						if (j == 3 && column == (xpoints - 1)) good = 0;
						if (good == 1 && grain_indicies[neighbor] == grain_indicies[index] && checked[neighbor] == false)
						{
							currentvlist.push_back(neighbor);
							checked[neighbor] = true;
						}
					}
					else if (periodic_boundaries == true)
					{
						if (j == 0 && plane == 0) neighbor = neighbor + (xpoints*ypoints*zpoints);
						if (j == 5 && plane == (zpoints - 1)) neighbor = neighbor - (xpoints*ypoints*zpoints);
						if (j == 1 && row == 0) neighbor = neighbor + (xpoints*ypoints);
						if (j == 4 && row == (ypoints - 1)) neighbor = neighbor - (xpoints*ypoints);
						if (j == 2 && column == 0) neighbor = neighbor + (xpoints);
						if (j == 3 && column == (xpoints - 1)) neighbor = neighbor - (xpoints);
						if (grain_indicies[neighbor] == grain_indicies[index] && checked[neighbor] == false)
						{
							currentvlist.push_back(neighbor);
							checked[neighbor] = true;
						}
					}
				}
				count++;
			}
			size_t size = vlists[grain_indicies[i]].size();
			if(size > 0)
			{
				if(size < currentvlist.size())
				{
					for (size_t k = 0; k < vlists[grain_indicies[i]].size(); k++)
					{
						grain_indicies[vlists[grain_indicies[i]][k]] = -1;
					}
					vlists[grain_indicies[i]].resize(currentvlist.size());
					vlists[grain_indicies[i]].swap(currentvlist);
				}
				else if(size >= currentvlist.size())
				{
					for (size_t k = 0; k < currentvlist.size(); k++)
					{
						grain_indicies[currentvlist[k]] = -1;
					}
				}
			}
			else if(size == 0)
			{
				if(currentvlist.size() >= minsize)
				{
					vlists[grain_indicies[i]].resize(currentvlist.size());
					vlists[grain_indicies[i]].swap(currentvlist);
				}
				if(currentvlist.size() < minsize)
				{
					for (size_t k = 0; k < currentvlist.size(); k++)
					{
						grain_indicies[currentvlist[k]] = -1;
					}
				}
			}
			currentvlist.clear();
		}
	  }
	  for (int i = 0; i < totalpoints; i++)
	  {
		if(grain_indicies[i] > 0) gsizes[grain_indicies[i]]++;
	  }
	  newnames.resize(m_Grains.size());
	  int goodcount = 1;
	  for (int i = 1; i < m_Grains.size(); i++)
	  {
		newnames[i] = 0;
		if (gsizes[i] > 0)
		{
		  m_Grains[goodcount] = m_Grains[i];
		  newnames[i] = goodcount;
		  goodcount++;
		}
	  }
	  for (int i = 0; i < totalpoints; i++)
	  {
		if (grain_indicies[i] > 0)
		{
		  grain_indicies[i] = newnames[grain_indicies[i]];
		}
	  }
	  m_Grains.resize(goodcount);
	  assign_gaps();
}
void GrainGeneratorFunc::assign_eulers()
{
  DREAM3D_RANDOMNG_NEW()

 // int gnum = 0;
  int numbins = 0;
  float totaldensity = 0;
  float synea1=0,synea2=0,synea3=0;
  float q[5];
  float random;
  int choose, phase;

  size_t xtalCount = crystruct.size();
  unbiasedvol.resize(xtalCount);
  for(size_t i=1;i<xtalCount;++i)
  {
   unbiasedvol[i] = 0;
  }
  for(size_t i=1;i<m_Grains.size();i++)
  {
    random = rg.genrand_res53();
    choose = 0;
    totaldensity = 0;
    phase = m_Grains[i]->phase;
  if(crystruct[phase] == Ebsd::Cubic) numbins = 5832;
  if(crystruct[phase] == Ebsd::Hexagonal) numbins = 15552;
    for (int j = 0; j < numbins; j++)
    {
      float density = actualodf[phase][j];
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
    if (m_Grains[i]->surfacegrain == 0)
    {
      simodf[phase][choose] = simodf[phase][choose] + (float(m_Grains[i]->numvoxels) * resx * resy * resz);
      unbiasedvol[phase] = unbiasedvol[phase] + (float(m_Grains[i]->numvoxels) * resx * resy * resz);
    }
  }
  for(int i=0;i<numbins;i++)
  {
	  simodf[phase][i] = simodf[phase][i]/unbiasedvol[phase];
  }
}

void  GrainGeneratorFunc::fillin_precipitates()
{
  std::vector<int> neighs;
  std::vector<int> remove;
  std::vector<int> gsizes;
  std::vector<int> neighbors;
  int count = 1;
  int good = 1;
  float x, y, z;
  gsizes.resize(m_Grains.size(),0);
  neighbors.resize(totalpoints,0);
  int neighpoint;
  int neighpoints[6];
  std::vector<int> n(m_Grains.size());
  neighpoints[0] = -xpoints*ypoints;
  neighpoints[1] = -xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xpoints;
  neighpoints[5] = xpoints*ypoints;
  while(count != 0)
  {
    count = 0;
    for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
    {
      int grainname = grain_indicies[i];
      if(grainname <= 0)
      {
      count++;
        for(size_t c = 1; c < m_Grains.size(); c++)
        {
          n[c] = 0;
        }
      x = i%xpoints;
    y = (i/xpoints)%ypoints;
    z = i/(xpoints*ypoints);
      for(int j=0;j<6;j++)
      {
        good = 1;
        neighpoint = i+neighpoints[j];
          if(j == 0 && z == 0) good = 0;
          if(j == 5 && z == (zpoints-1)) good = 0;
          if(j == 1 && y == 0) good = 0;
          if(j == 4 && y == (ypoints-1)) good = 0;
          if(j == 2 && x == 0) good = 0;
          if(j == 3 && x == (xpoints-1)) good = 0;
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
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int grainname = grain_indicies[j];
      int neighbor = neighbors[j];
      if(grainname <= 0 && neighbor > 0 && neighbor >= numprimarygrains)
      {
        grain_indicies[j] = neighbor;
		phases[j] = m_Grains[neighbor]->phase;
      }
    }
  }
  gsizes.resize(m_Grains.size(),0);
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int name = grain_indicies[i];
    gsizes[name]++;
  }
  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    m_Grains[i]->numvoxels = gsizes[i];
  }
  gsizes.clear();
}

void  GrainGeneratorFunc::place_precipitates()
{
  DREAM3D_RANDOMNG_NEW()

  totalprecipvol = 0;
  int precipvoxelcounter = 0;
  size_t currentnumgrains = m_Grains.size();
  numprimarygrains = m_Grains.size();
 // size_t index;
  int phase;
  float precipboundaryfraction = 0.0;
  float random;
  int random2;
  float xc, yc, zc;
  double totalprecipitatefractions = 0.0;
  for (size_t i = 1; i < phaseType.size();++i)
  {
    if(phaseType[i] == DREAM3D::Reconstruction::PrecipitatePhase)
    {
	    precipitatephases.push_back(i);
	    precipitatephasefractions.push_back(phasefraction[i]);
	    totalprecipitatefractions = totalprecipitatefractions + phasefraction[i];
    }
  }
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    precipitatephasefractions[i] = precipitatephasefractions[i]/totalprecipitatefractions;
    if(i > 0) precipitatephasefractions[i] = precipitatephasefractions[i] + precipitatephasefractions[i-1];
    //if(i == 0) precipitatephasefractions[i] = precipitatephasefractions[i];
  }
  PackGrainsGen2::Pointer packGrains = PackGrainsGen2::New();
  packGrains->setGrainGenFunc(this);
  while(totalprecipvol < totalvol*totalprecipitatefractions)
  {
    GGseed++;
    random = rg.genrand_res53();
    for (size_t j = 0; j < precipitatephases.size();++j)
    {
      if (random < precipitatephasefractions[j])
      {
        phase = precipitatephases[j];
        break;
      }
    }
  m_Grains.resize(currentnumgrains+1);
  m_Grains[currentnumgrains] = Grain::New();
  packGrains->generate_grain(currentnumgrains, phase);
  precipboundaryfraction = pptFractions[phase];
  random = rg.genrand_res53();
  if(random <= precipboundaryfraction)
  {
    random2 = int(rg.genrand_res53()*double(totalpoints-1));
    while(surfacevoxels[random2] == 0 || grain_indicies[random2] > numprimarygrains)
    {
      random2++;
      if(random2 >= totalpoints) random2 = random2-totalpoints;
    }
  }
  else if(random > precipboundaryfraction)
  {
    random2 = rg.genrand_res53()*(totalpoints-1);
    while(surfacevoxels[random2] != 0 || grain_indicies[random2] > numprimarygrains)
    {
      random2++;
      if(random2 >= totalpoints) random2 = random2-totalpoints;
    }
  }
    xc = find_xcoord(random2);
    yc = find_ycoord(random2);
    zc = find_zcoord(random2);
    m_Grains[currentnumgrains]->centroidx = xc;
    m_Grains[currentnumgrains]->centroidy = yc;
    m_Grains[currentnumgrains]->centroidz = zc;
    insert_precipitate(currentnumgrains);
  m_Grains[currentnumgrains]->active = 1;
  precipvoxelcounter = 0;
  for(size_t j = 0; j < m_Grains[currentnumgrains]->voxellist->size(); j++)
  {
    if(grain_indicies[m_Grains[currentnumgrains]->voxellist->at(j)] > 0 && grain_indicies[m_Grains[currentnumgrains]->voxellist->at(j)] < numprimarygrains)
    {
      precipvoxelcounter++;
    }
  }
  if(double(precipvoxelcounter)/double(m_Grains[currentnumgrains]->voxellist->size()) > 0.75)
  {
    precipvoxelcounter = 0;
    for(size_t j = 0; j < m_Grains[currentnumgrains]->voxellist->size(); j++)
    {
      if(grain_indicies[m_Grains[currentnumgrains]->voxellist->at(j)] < 0 || grain_indicies[m_Grains[currentnumgrains]->voxellist->at(j)] >= numprimarygrains)
      {
        grain_indicies[m_Grains[currentnumgrains]->voxellist->at(j)] = -1;
        phases[m_Grains[currentnumgrains]->voxellist->at(j)] = 0;
      }
      if(grain_indicies[m_Grains[currentnumgrains]->voxellist->at(j)] > 0 && grain_indicies[m_Grains[currentnumgrains]->voxellist->at(j)] < numprimarygrains)
      {
        grain_indicies[m_Grains[currentnumgrains]->voxellist->at(j)] = currentnumgrains;
        phases[m_Grains[currentnumgrains]->voxellist->at(j)] = m_Grains[currentnumgrains]->phase;
        precipvoxelcounter++;
      }
    }
      totalprecipvol = totalprecipvol + (precipvoxelcounter*resx*resy*resz);
    currentnumgrains++;
  }
  }
}


void GrainGeneratorFunc::adjust_boundaries()
{
  DREAM3D_RANDOMNG_NEW()

  int neighpoints[6];
  neighpoints[0] = -xpoints*ypoints;
  neighpoints[1] = -xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xpoints;
  neighpoints[5] = xpoints*ypoints;
  int iterations = 0;
  size_t selectedgrain = 0;
  int good = 0;
  int growth = 1;
  int nucleus;
  int bad = 0;
  float random, oldsizedisterror, currentsizedisterror, diam;
  int x, y, z;
  int neighpoint, index;
  size_t count, affectedcount;
  int vListSize = 1000;

  float voxtovol = resx*resy*resz*(3.0/4.0)*(1.0/m_pi);

  NEW_SHARED_ARRAY(gsizes, int, m_Grains.size())

  std::vector<int> voxellist(vListSize,-1);
  std::vector<int> affectedvoxellist(vListSize,-1);
  for(size_t i=1;i<m_Grains.size();i++)
  {
    gsizes[i] = 0;
  }
  NEW_SHARED_ARRAY(reassigned, int, totalpoints)

  for(int i=0;i<totalpoints;i++)
  {
    reassigned[i] = 0;
    gsizes[grain_indicies[i]]++;
  }
  PackGrainsGen2::Pointer packGrains = PackGrainsGen2::New();
  packGrains->setGrainGenFunc(this);
  oldsizedisterror = packGrains->check_sizedisterror(-1000,-1000);
  while(iterations < 10000)
  {
    iterations++;
    good = 0;
    while (good == 0)
    {
      good = 1;
      selectedgrain = int(rg.genrand_res53() * m_Grains.size());
      if (selectedgrain >= m_Grains.size()) selectedgrain = m_Grains.size()-1;
      if (selectedgrain == 0) selectedgrain = 1;
      if (m_Grains[selectedgrain]->surfacegrain > 0) good = 0;
    }
    growth = 1;
    random = rg.genrand_res53();
    if(random < 0.5) growth = -1;
    nucleus = 0;
    count = 0;
    affectedcount = 0;
    while(grain_indicies[nucleus] != selectedgrain)
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
        neighpoint = index+neighpoints[j];
        if(j == 0 && z == 0) good = 0;
        if(j == 5 && z == (zpoints-1)) good = 0;
        if(j == 1 && y == 0) good = 0;
        if(j == 4 && y == (ypoints-1)) good = 0;
        if(j == 2 && x == 0) good = 0;
        if(j == 3 && x == (xpoints-1)) good = 0;
        if(good == 1 && grain_indicies[neighpoint] == selectedgrain && reassigned[neighpoint] == 0)
        {
	        voxellist[count] = neighpoint;
	        reassigned[neighpoint] = -1;
	        count++;
	        if(count >= voxellist.size()) voxellist.resize(voxellist.size()+vListSize,-1);
        }
        if(good == 1 && grain_indicies[neighpoint] != selectedgrain && grain_indicies[index] == selectedgrain)
        {
	        if(growth == 1 && reassigned[neighpoint] <= 0)
	        {
	          reassigned[neighpoint] = grain_indicies[neighpoint];
	          grain_indicies[neighpoint] = grain_indicies[index];
	          affectedvoxellist[affectedcount] = neighpoint;
	          affectedcount++;
	          if(affectedcount >= affectedvoxellist.size()) affectedvoxellist.resize(affectedvoxellist.size()+vListSize,-1);
	        }
	        if(growth == -1 && reassigned[neighpoint] <= 0)
	        {
	          reassigned[index] = grain_indicies[index];
	          grain_indicies[index] = grain_indicies[neighpoint];
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
        gsizes[grain_indicies[index]]++;
        gsizes[reassigned[index]] = gsizes[reassigned[index]]-1;
      }
    }
    for(size_t i=1;i<m_Grains.size();i++)
    {
      index = i;
      diam = 2.0*powf((gsizes[index]*voxtovol),(1.0/3.0));
      m_Grains[index]->equivdiameter = diam;
    }
    PackGrainsGen2::Pointer packGrains = PackGrainsGen2::New();
    packGrains->setGrainGenFunc(this);
    currentsizedisterror = packGrains->check_sizedisterror(-1000,-1000);

    if(currentsizedisterror <= oldsizedisterror)
    {
      oldsizedisterror = currentsizedisterror;
      for(size_t i=1;i<m_Grains.size();i++)
      {
        if(gsizes[i] == 0) m_Grains.erase(m_Grains.begin() + i);
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
          gsizes[grain_indicies[index]] = gsizes[grain_indicies[index]]-1;
          grain_indicies[index] = reassigned[index];
          gsizes[grain_indicies[index]]++;
        }
      }
      for(size_t i=1;i<m_Grains.size();i++)
      {
        index = i;
        diam = 2.0*powf((gsizes[index]*voxtovol),(1.0/3.0));
        m_Grains[index]->equivdiameter = diam;
      }
    }
    for(int i=0;i<totalpoints;i++)
    {
      reassigned[i] = 0;
    }
  }
  NEW_SHARED_ARRAY(newnames, int, m_Grains.size())

  for (size_t i=1;i<m_Grains.size();i++)
  {
    newnames[i] = i;
  }
  for(int i=0;i<totalpoints;i++)
  {
    grain_indicies[i] = newnames[grain_indicies[i]];
  }
}



void  GrainGeneratorFunc::find_neighbors()
{
  int neighpoints[6];
  neighpoints[0] = -(xpoints * ypoints);
  neighpoints[1] = -xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xpoints;
  neighpoints[5] = (xpoints * ypoints);
  float column, row, plane;
  int grain;
  size_t nnum;
  int onsurf = 0;
  int good = 0;
  int neighbor = 0;
  size_t xtalCount = crystruct.size();
  totalsurfacearea = m_TotalSurfaceArea->WritePointer(0, xtalCount);
  for (size_t i = 1; i < xtalCount; ++i)
  {
    totalsurfacearea[i] = 0.0f;
  }
  int nListSize = 100;
  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    m_Grains[i]->numneighbors = 0;
    if (m_Grains[i]->neighborlist == NULL)
    {
      m_Grains[i]->neighborlist = new std::vector<int>(0);
    }
    m_Grains[i]->neighborlist->resize(nListSize, -1);
    if (m_Grains[i]->neighborsurfacealist == NULL)
    {
      m_Grains[i]->neighborsurfacealist = new std::vector<float>(0);
    }
    m_Grains[i]->neighborsurfacealist->resize(nListSize, -1.0);
    for (int j = 0; j < 3; j++)
    {
      m_Grains[i]->neighbordistfunc[j] = 0;
    }
  }

  for (int j = 0; j < totalpoints; j++)
  {
    onsurf = 0;
    grain = grain_indicies[j];
    if (grain > 0)
    {
      column = j % xpoints;
      row = (j / xpoints) % ypoints;
      plane = j / (xpoints * ypoints);
      if ((column == 0 || column == (xpoints - 1) || row == 0 || row == (ypoints - 1) || plane == 0 || plane == (zpoints - 1)) && zpoints != 1)
      {
        m_Grains[grain]->surfacegrain = true;
      }
      if ((column == 0 || column == (xpoints - 1) || row == 0 || row == (ypoints - 1)) && zpoints == 1)
      {
        m_Grains[grain]->surfacegrain = true;
      }
      for (int k = 0; k < 6; k++)
      {
        good = 1;
        neighbor = j + neighpoints[k];
        if (k == 0 && plane == 0) good = 0;
        if (k == 5 && plane == (zpoints - 1)) good = 0;
        if (k == 1 && row == 0) good = 0;
        if (k == 4 && row == (ypoints - 1)) good = 0;
        if (k == 2 && column == 0) good = 0;
        if (k == 3 && column == (xpoints - 1)) good = 0;
        if (good == 1 && grain_indicies[neighbor] != grain && grain_indicies[neighbor] > 0)
        {
          onsurf++;
          nnum = m_Grains[grain]->numneighbors;
          std::vector<int>* nlist = m_Grains[grain]->neighborlist;
          if (nnum >= (nlist->size()))
          {
            nlist->resize(nnum + nListSize);
          }
          nlist->at(nnum) = grain_indicies[neighbor];
          nnum++;
          m_Grains[grain]->numneighbors = nnum;
        }
      }
    }
    surfacevoxels[j] = onsurf;
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


void GrainGeneratorFunc::MC_LoopBody1(int phase, size_t neighbor, int j,std::vector<float>* misolist,std::vector<float>* neighborsurfacealist, float &mdfchange)
{
  float w;
  float n1, n2, n3;
  float r1, r2, r3;

  int curmiso1 = std::numeric_limits<int >::max();
  int curmiso2 = std::numeric_limits<int >::max();
  int curmiso3 = std::numeric_limits<int >::max();

  int neighsurfarea = std::numeric_limits<int >::max();
  int curmisobin = std::numeric_limits<int >::max();
  int newmisobin = std::numeric_limits<int >::max();

  float q1[5], q2[5];

  curmiso1 = misolist->at(3*j);
  curmiso2 = misolist->at(3*j+1);
  curmiso3 = misolist->at(3*j+2);
  neighsurfarea = neighborsurfacealist->at(j);
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

void GrainGeneratorFunc::MC_LoopBody2(int phase, size_t neighbor, int j,std::vector<float>* misolist,std::vector<float>* neighborsurfacealist)
{
  float w;
  float n1, n2, n3;
  float r1, r2, r3;

  int curmiso1 = std::numeric_limits<int >::max();
  int curmiso2 = std::numeric_limits<int >::max();
  int curmiso3 = std::numeric_limits<int >::max();

  int neighsurfarea = std::numeric_limits<int >::max();
  int curmisobin = std::numeric_limits<int >::max();
  int newmisobin = std::numeric_limits<int >::max();

  float q1[5], q2[5];
  float miso1 = std::numeric_limits<float >::max();
  float miso2 = std::numeric_limits<float >::max();
  float miso3 = std::numeric_limits<float >::max();

  curmiso1 = misolist->at(3 * j);
  curmiso2 = misolist->at(3 * j + 1);
  curmiso3 = misolist->at(3 * j + 2);
  neighsurfarea = neighborsurfacealist->at(j);
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


void GrainGeneratorFunc::swapOutOrientation( int &badtrycount, int &numbins, float currentodferror, float currentmdferror)
{
  DREAM3D_RANDOMNG_NEW()

  float random;
  int good;
  float deltaerror = 1.0;
  size_t selectedgrain1;
  float q1[5];
  float ea1, ea2, ea3;
  float r1, r2, r3;

  int g1odfbin = std::numeric_limits<int >::max();

  float g1ea1 = std::numeric_limits<float >::max();
  float g1ea2 = std::numeric_limits<float >::max();
  float g1ea3 = std::numeric_limits<float >::max();

  std::vector<int>* nlist;
  std::vector<float>* misolist;
  std::vector<float>* neighborsurfacealist;

  float totaldensity = 0;

  good = 0;
  while (good == 0)
  {
    good = 1;
    selectedgrain1 = int(rg.genrand_res53() * m_Grains.size());
    if (selectedgrain1 == 0) selectedgrain1 = 1;
    if (selectedgrain1 == m_Grains.size()) selectedgrain1 = m_Grains.size() - 1;
    if (m_Grains[selectedgrain1]->surfacegrain > 0) good = 0;
  }

  ea1 = m_Grains[selectedgrain1]->euler1;
  ea2 = m_Grains[selectedgrain1]->euler2;
  ea3 = m_Grains[selectedgrain1]->euler3;
  OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
  int phase = m_Grains[selectedgrain1]->phase;
  g1odfbin = m_OrientatioOps[crystruct[phase]]->getOdfBin(r1, r2, r3);
  random = rg.genrand_res53();
  int choose = 0;
  totaldensity = 0;
  for (int i = 0; i < numbins; i++)
  {
    float density = actualodf[phase][i];
    totaldensity = totaldensity + density;
    if (random >= totaldensity) choose = i;
  }

  m_OrientatioOps[crystruct[phase]]->determineEulerAngles(choose, g1ea1, g1ea2, g1ea3);
  OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);

  float odfchange = ((actualodf[phase][choose] - simodf[phase][choose]) * (actualodf[phase][choose] - simodf[phase][choose])) - ((actualodf[phase][choose] - (simodf[phase][choose]
      + (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]))) * (actualodf[phase][choose] - (simodf[phase][choose]
      + (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]))));
  odfchange = odfchange + (((actualodf[phase][g1odfbin] - simodf[phase][g1odfbin]) * (actualodf[phase][g1odfbin] - simodf[phase][g1odfbin])) - ((actualodf[phase][g1odfbin] - (simodf[phase][g1odfbin]
      - (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]))) * (actualodf[phase][g1odfbin] - (simodf[phase][g1odfbin]
      - (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase])))));

  float mdfchange = 0;
  nlist = m_Grains[selectedgrain1]->neighborlist;
  misolist = m_Grains[selectedgrain1]->misorientationlist;
  neighborsurfacealist = m_Grains[selectedgrain1]->neighborsurfacealist;
  size_t size = 0;
  if (NULL != nlist)
  {
    size = nlist->size();
  }
  for (size_t j = 0; j < size; j++)
  {
    int neighbor = nlist->at(j);
    MC_LoopBody1(phase, neighbor, j, misolist, neighborsurfacealist, mdfchange);
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
    simodf[phase][choose] = simodf[phase][choose] + (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]);
    simodf[phase][g1odfbin] = simodf[phase][g1odfbin] - (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]);
    size_t size = 0;
    if (NULL != nlist)
    {
      size = nlist->size();
    }
	for (size_t j = 0; j < size; j++)
    {
      int neighbor = nlist->at(j);
      MC_LoopBody2(phase, neighbor, j, misolist, neighborsurfacealist);
    }
  }

}

void GrainGeneratorFunc::switchOrientations( int &badtrycount, int &numbins, float currentodferror, float currentmdferror)
{
  DREAM3D_RANDOMNG_NEW()

  int good = 0;
  float deltaerror;
  size_t selectedgrain1;
  size_t selectedgrain2;
  float q1[5];
  float r1, r2, r3;

  int g1odfbin = std::numeric_limits<int >::max();
  int g2odfbin = std::numeric_limits<int >::max();

  float g1ea1 = std::numeric_limits<float >::max();
  float g1ea2 = std::numeric_limits<float >::max();
  float g1ea3 = std::numeric_limits<float >::max();
  float g2ea1 = std::numeric_limits<float >::max();
  float g2ea2 = std::numeric_limits<float >::max();
  float g2ea3 = std::numeric_limits<float >::max();

  std::vector<int>* nlist;
 std::vector<float>* misolist;
  std::vector<float>* neighborsurfacealist;

  good = 0;
  while (good == 0)
  {
    good = 1;
    selectedgrain1 = int(rg.genrand_res53() * m_Grains.size());
    if (selectedgrain1 == 0) selectedgrain1 = 1;
    if (selectedgrain1 == m_Grains.size()) selectedgrain1 = m_Grains.size() - 1;
    selectedgrain2 = int(rg.genrand_res53() * m_Grains.size());
    if (selectedgrain2 == 0) selectedgrain2 = 1;
    if (selectedgrain2 == m_Grains.size()) selectedgrain2 = m_Grains.size() - 1;
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

  float odfchange = ((actualodf[phase][g1odfbin]-simodf[phase][g1odfbin]) * (actualodf[phase][g1odfbin]-simodf[phase][g1odfbin])) - ((actualodf[phase][g1odfbin]
     -(simodf[phase][g1odfbin] - (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]) + (float(m_Grains[selectedgrain2]->numvoxels) * resx
          * resy * resz / unbiasedvol[phase]))) * (actualodf[phase][g1odfbin]-(simodf[phase][g1odfbin] - (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase])
      + (float(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase]))));
  odfchange = odfchange + (((actualodf[phase][g2odfbin]-simodf[phase][g2odfbin]) * (actualodf[phase][g2odfbin]-simodf[phase][g2odfbin])) - ((actualodf[phase][g2odfbin]
     -(simodf[phase][g2odfbin] - (float(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase]) + (float(m_Grains[selectedgrain1]->numvoxels) * resx
          * resy * resz / unbiasedvol[phase]))) * (actualodf[phase][g2odfbin]-(simodf[phase][g2odfbin] - (float(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase])
      + (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase])))));


  float mdfchange = 0;
  OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
  nlist = m_Grains[selectedgrain1]->neighborlist;
  misolist = m_Grains[selectedgrain1]->misorientationlist;
  neighborsurfacealist = m_Grains[selectedgrain1]->neighborsurfacealist;
  size_t size = 0;
  if (NULL != nlist)
  {
    size = nlist->size();
  }
  for (size_t j = 0; j < size; j++)
  {
    int neighbor = nlist->at(j);
    if (neighbor != selectedgrain2)
    {
      MC_LoopBody1(phase, neighbor, j, misolist, neighborsurfacealist, mdfchange);
    }
  }

  OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);
  nlist = m_Grains[selectedgrain2]->neighborlist;
  misolist = m_Grains[selectedgrain2]->misorientationlist;
  neighborsurfacealist = m_Grains[selectedgrain2]->neighborsurfacealist;
  size = 0;
  if (NULL != nlist)
  {
    size = nlist->size();
  }
  for (size_t j = 0; j < size; j++)
  {
    int neighbor = nlist->at(j);
    if (neighbor != selectedgrain1)
    {
      MC_LoopBody1(phase, neighbor, j, misolist, neighborsurfacealist, mdfchange);
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
    simodf[phase][g1odfbin] = simodf[phase][g1odfbin] + (float(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase])
        - (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase]);
    simodf[phase][g2odfbin] = simodf[phase][g2odfbin] + (float(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / unbiasedvol[phase])
        - (float(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / unbiasedvol[phase]);

  OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
  nlist = m_Grains[selectedgrain1]->neighborlist;
    misolist = m_Grains[selectedgrain1]->misorientationlist;
    neighborsurfacealist = m_Grains[selectedgrain1]->neighborsurfacealist;
    m_Grains[selectedgrain1]->avg_quat[1] = q1[1];
    m_Grains[selectedgrain1]->avg_quat[2] = q1[2];
    m_Grains[selectedgrain1]->avg_quat[3] = q1[3];
    m_Grains[selectedgrain1]->avg_quat[4] = q1[4];
    size = 0;
    if (NULL != nlist)
    {
      size = nlist->size();
    }
	for (size_t j = 0; j < size; j++)
    {
      size_t neighbor = nlist->at(j);
      if (neighbor != selectedgrain2)
      {
        MC_LoopBody2(phase, neighbor, j, misolist, neighborsurfacealist);
      }
    }

  OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);
    nlist = m_Grains[selectedgrain2]->neighborlist;
    misolist = m_Grains[selectedgrain2]->misorientationlist;
    neighborsurfacealist = m_Grains[selectedgrain2]->neighborsurfacealist;
    m_Grains[selectedgrain2]->avg_quat[1] = q1[1];
    m_Grains[selectedgrain2]->avg_quat[2] = q1[2];
    m_Grains[selectedgrain2]->avg_quat[3] = q1[3];
    m_Grains[selectedgrain2]->avg_quat[4] = q1[4];
    size = 0;
    if (NULL != nlist)
    {
      size = nlist->size();
    }
	for (size_t j = 0; j < size; j++)
    {
      size_t neighbor = nlist->at(j);
      if (neighbor != selectedgrain1)
      {
        MC_LoopBody2(phase, neighbor, j, misolist, neighborsurfacealist);
      }
    }
  }
}

void GrainGeneratorFunc::matchCrystallography()
{
  DREAM3D_RANDOMNG_NEW()

  int numbins = 0;

  int iterations = 0;
  int badtrycount = 0;
  float random;
  float currentodferror = 0;
  float currentmdferror = 0;
  size_t xtalSize = crystruct.size();
  for(size_t iter=1;iter<xtalSize;++iter)
  {
    if(crystruct[iter] == Ebsd::Cubic) numbins = 18*18*18;
    if(crystruct[iter] == Ebsd::Hexagonal) numbins = 36*36*12;
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
    iterations++;
    badtrycount++;
    random = rg.genrand_res53();

    if(random < 0.5)  // SwapOutOrientation
    {
      swapOutOrientation(badtrycount, numbins, currentodferror, currentmdferror);
    }
    else if(random > 0.5) // SwitchOrientation
    {
      switchOrientations(badtrycount, numbins, currentodferror, currentmdferror);
    }
    }
  }
 // float q[5];
  for(int i = 0; i < totalpoints; i++)
  {
    euler1s[i] = m_Grains[grain_indicies[i]]->euler1;
    euler2s[i] = m_Grains[grain_indicies[i]]->euler2;
    euler3s[i] = m_Grains[grain_indicies[i]]->euler3;
  }
}
void  GrainGeneratorFunc::measure_misorientations ()
{
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float q1[5];
  float q2[5];
  Ebsd::CrystalStructure phase1, phase2;
  int mbin;


  std::vector<int>* nlist;
  std::vector<float>* neighsurfarealist;

  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    nlist = m_Grains[i]->neighborlist;
    neighsurfarealist = m_Grains[i]->neighborsurfacealist;
    if (NULL != m_Grains[i]->misorientationlist)
    {
      delete m_Grains[i]->misorientationlist;
      m_Grains[i]->misorientationlist = NULL;
    }
    if (NULL != nlist)
    {
      m_Grains[i]->misorientationlist = new std::vector<float>(nlist->size() * 3, 0.0);
    }

    q1[1] = m_Grains[i]->avg_quat[1];
    q1[2] = m_Grains[i]->avg_quat[2];
    q1[3] = m_Grains[i]->avg_quat[3];
    q1[4] = m_Grains[i]->avg_quat[4];
    phase1 = crystruct[m_Grains[i]->phase];
    size_t size = 0;
    if (NULL != nlist && neighsurfarealist != NULL && neighsurfarealist->size() == nlist->size() )
    {
      size = nlist->size();
    }


    for (size_t j = 0; j < size; j++)
    {
      w = 10000.0;
      int nname = nlist->at(j);
      float neighsurfarea = neighsurfarealist->at(j);
      q2[1] = m_Grains[nname]->avg_quat[1];
      q2[2] = m_Grains[nname]->avg_quat[2];
      q2[3] = m_Grains[nname]->avg_quat[3];
      q2[4] = m_Grains[nname]->avg_quat[4];
      phase2 = crystruct[m_Grains[nname]->phase];
      if (phase1 == phase2) w = m_OrientatioOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
      OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
      if (phase1 == phase2)
      {
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
      if (phase1 == phase2)
      {
        mbin =
            m_OrientatioOps[phase1]->getMisoBin(m_Grains[i]->misorientationlist->at(3 * j),
                                                m_Grains[i]->misorientationlist->at(3 * j + 1),
                                                m_Grains[i]->misorientationlist->at(3 * j + 2));
      }

      if (m_Grains[i]->surfacegrain == 0 && (nname > i || m_Grains[nname]->surfacegrain == 1) && phase1 == phase2)
      {
        simmdf[m_Grains[i]->phase][mbin] = simmdf[m_Grains[i]->phase][mbin] + (neighsurfarea / totalsurfacearea[m_Grains[i]->phase]);
      }
    }
  }
}
//FIXME: This should return an erorr condition
void GrainGeneratorFunc::write_eulerangles(const std::string &filename)
{
  ofstream outFile;
  outFile.open(filename.c_str());
  outFile << m_Grains.size() << std::endl;
  char space = DREAM3D::GrainData::Delimiter;
  outFile << DREAM3D::GrainData::GrainID  << space
          << DREAM3D::GrainData::Phi1 << space << DREAM3D::GrainData::PHI<< space << DREAM3D::GrainData::Phi2 << std::endl;
  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    outFile << i << space << m_Grains[i]->euler1 << space << m_Grains[i]->euler2 << space << m_Grains[i]->euler3 << endl;
  }
  outFile.close();
}

//FIXME: This should return an erorr condition
void GrainGeneratorFunc::write_graindata(const std::string &filename)
{
  ofstream outFile;
  outFile.open(filename.c_str());
  char space = DREAM3D::GrainData::Delimiter;
  outFile << m_Grains.size() << std::endl;
  outFile << DREAM3D::GrainData::GrainID  << space << DREAM3D::GrainData::PhaseID << space
      << DREAM3D::GrainData::Phi1 << space << DREAM3D::GrainData::PHI<< space << DREAM3D::GrainData::Phi2 << space
      << DREAM3D::GrainData::EquivDiam << space
      << DREAM3D::GrainData::B_Over_A << space << DREAM3D::GrainData::C_Over_A << space << DREAM3D::GrainData::Omega3 << std::endl;

  for (size_t i = 1; i < m_Grains.size(); i++)
  {
	  outFile << i << space << m_Grains[i]->phase << space << m_Grains[i]->euler1 << space << m_Grains[i]->euler2 << space << m_Grains[i]->euler3 <<
		space << m_Grains[i]->equivdiameter << space << m_Grains[i]->radius2 << space << m_Grains[i]->radius3 <<
		space << m_Grains[i]->omega3 << endl;
  }
  outFile.close();
}


float GrainGeneratorFunc::find_xcoord(long long int index)
{
  float x = resx*float(index%xpoints);
  return x;
}
float GrainGeneratorFunc::find_ycoord(long long int index)
{
  float y = resy*float((index/xpoints)%ypoints);
  return y;
}
float GrainGeneratorFunc::find_zcoord(long long int index)
{
  float z = resz*float(index/(xpoints*ypoints));
  return z;
}



//int GrainGeneratorFunc::writePhFormat(const std::string &file)
//{
//  int err = 0;
//
//  FILE* f = fopen(file.c_str(), "wb");
//
//  fprintf(f, "     %d     %d     %d     Written by DREAM.3D\n", xpoints, ypoints, zpoints);
//  fprintf(f, "'testXX'               0.00 10.792  0.0       %d\n", (int)(m_Grains.size()) );
//  fprintf(f, "1.000 1.000 1.000       %d\n", (int)(m_Grains.size()));
//  for(int i = 0; i < totalpoints; ++i)
//  {
//    fprintf(f, "    %d\n",  voxels[i].grain_index );
//  }
//
//  fclose(f);
//
//  return err;
//}


