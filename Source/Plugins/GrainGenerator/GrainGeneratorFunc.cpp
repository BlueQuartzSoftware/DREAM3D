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

// AIM Includes
#include "DREAM3D/Common/AIMMath.h"
#include "DREAM3D/Common/Constants.h"

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

#define ERROR_TXT_OUT 0
#define ERROR_TXT_OUT1 0

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
  unassigned = NULL;
  ellipfuncs = NULL;
  phases = NULL;
  euler1s = NULL;
  euler2s = NULL;
  euler3s = NULL;
  surfacevoxels = NULL;
  quats = NULL;
  totalsurfacearea = NULL;

  m_GrainIndicies = AIMArray<int>::CreateArray(0);
  m_Unassigned = AIMArray<bool>::CreateArray(0);
  m_Ellipfuncs = AIMArray<float>::CreateArray(0);
  m_Phases = AIMArray<int>::CreateArray(0);
  m_Euler1s = AIMArray<float>::CreateArray(0);
  m_Euler2s = AIMArray<float>::CreateArray(0);
  m_Euler3s = AIMArray<float>::CreateArray(0);
  m_SurfaceVoxels = AIMArray<float>::CreateArray(0);
  m_Quats = AIMArray<float>::CreateArray(0);
  m_TotalSurfaceArea = AIMArray<float>::CreateArray(0);

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
  grain_indicies = m_GrainIndicies->WritePointer(0, totalpoints);
  unassigned = m_Unassigned->WritePointer(false, totalpoints);
  ellipfuncs = m_Ellipfuncs->WritePointer(0, totalpoints);
  phases = m_Phases->WritePointer(0, totalpoints);
  euler1s = m_Euler1s->WritePointer(0, totalpoints);
  euler2s = m_Euler2s->WritePointer(0, totalpoints);
  euler3s = m_Euler3s->WritePointer(0, totalpoints);
  surfacevoxels = m_SurfaceVoxels->WritePointer(0, totalpoints);
  quats = m_Quats->WritePointer(0, totalpoints*5);
  m_Quats->SetNumberOfComponents(5);
	for(int i=0;i<totalpoints;i++)
	{
		grain_indicies[i] = 0;
		unassigned[i] = false;
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


void GrainGeneratorFunc::initialize_packinggrid()
{
  sizex = xpoints*resx;
  sizey = ypoints*resy;
  sizez = zpoints*resz;
  totalvol = sizex*sizey*sizez;
  totalpoints = xpoints * ypoints * zpoints;

  packingresx = resx*2.0;
  packingresy = resy*2.0;
  packingresz = resz*2.0;
  packingxpoints = int(sizex/packingresx);
  packingypoints = int(sizey/packingresy);
  packingzpoints = int(sizez/packingresz);
  packingtotalpoints = packingxpoints*packingypoints*packingzpoints;
  grainowners.resize(packingxpoints);
  for(int i=0;i<packingxpoints;i++)
  {
    grainowners[i].resize(packingypoints);
    for(int j=0;j<packingypoints;j++)
    {
      grainowners[i][j].resize(packingzpoints,0);
    }
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

void GrainGeneratorFunc::generate_grain(int gnum, int phase)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(GGseed)

  int good = 0;
  float r1 = 1;
  float a1=0,a2=0,a3=0;
  float b1=0,b2=0,b3=0;
  float r2=0,r3=0;
  float diam = 0;
  float vol = 0;
  int volgood = 0;
  float phi1, PHI, phi2;
  while(volgood == 0)
  {
    volgood = 1;
    diam = rg.genrand_norm(avgdiam[phase],sddiam[phase]);
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
    r2 = rg.genrand_beta(a1,b1);
    a2 = covera[phase][diameter][0];
    b2 = covera[phase][diameter][1];
    if(a2 == 0)
    {
      a2 = covera[phase][diameter-1][0];
      b2 = covera[phase][diameter-1][1];
    }
    r3 = rg.genrand_beta(a2,b2);
    float cob = r3/r2;
    a3 = coverb[phase][diameter][0];
    b3 = coverb[phase][diameter][1];
    if(a3 == 0)
    {
      a3 = coverb[phase][diameter-1][0];
      b3 = coverb[phase][diameter-1][1];
    }
    float prob = ((gamma((a3+b3))/(gamma(a3)*gamma(b3)))*(powf(cob,(a3-1)))*(powf((1-cob),(b3-1))));
    float check = rg.genrand_res53();
    if(prob > check) good = 1;
    if(cob > 1) good = 0;
  }
  float random = rg.genrand_res53();
  int bin=0;
  for(int i=0;i<(36*36*36);i++)
  {
    if(random > axisodf[phase][i]) bin = i;
    if(random < axisodf[phase][i]) {break;}
  }
  m_OrientatioOps[Ebsd::OrthoRhombic]->determineEulerAngles(bin, phi1, PHI, phi2);
  float m = omega3[phase][diameter][0];
  float s = omega3[phase][diameter][1];
  float omega3 = rg.genrand_beta(m,s);
  DREAM3D::SyntheticBuilder::ShapeType shapeclass = shapeTypes[phase];
  if(shapeclass == DREAM3D::SyntheticBuilder::EllipsoidShape) omega3 = 1;
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

void GrainGeneratorFunc::insert_grain(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW()

  float dist;
  float inside = -1;
  int column, row, plane;
  int centercolumn, centerrow, centerplane;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float xp, yp, zp;
  float x, y, z;
  float volcur = m_Grains[gnum]->volume;
  float bovera = m_Grains[gnum]->radius2;
  float covera = m_Grains[gnum]->radius3;
  float omega3 = m_Grains[gnum]->omega3;
  float radcur1 = 1;
  if (NULL == m_Grains[gnum]->columnlist) {
  m_Grains[gnum]->columnlist = new std::vector<int>(0);}
  if (NULL == m_Grains[gnum]->rowlist) {
  m_Grains[gnum]->rowlist = new std::vector<int>(0);}
  if (NULL == m_Grains[gnum]->planelist) {
  m_Grains[gnum]->planelist = new std::vector<int>(0);}


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
  centercolumn = (xc-(packingresx/2))/packingresx;
  centerrow = (yc-(packingresy/2))/packingresy;
  centerplane = (zc-(packingresz/2))/packingresz;
  xmin = int(centercolumn-((radcur1/packingresx)+1));
  xmax = int(centercolumn+((radcur1/packingresx)+1));
  ymin = int(centerrow-((radcur1/packingresy)+1));
  ymax = int(centerrow+((radcur1/packingresy)+1));
  zmin = int(centerplane-((radcur1/packingresz)+1));
  zmax = int(centerplane+((radcur1/packingresz)+1));
  if(xmin < -packingxpoints) xmin = -packingxpoints;
  if(xmax > 2*packingxpoints-1) xmax = (2*packingxpoints-1);
  if(ymin < -packingypoints) ymin = -packingypoints;
  if(ymax > 2*packingypoints-1) ymax = (2*packingypoints-1);
  if(zmin < -packingzpoints) zmin = -packingzpoints;
  if(zmax > 2*packingzpoints-1) zmax = (2*packingzpoints-1);
  for(int iter1 = xmin; iter1 < xmax+1; iter1++)
  {
    for(int iter2 = ymin; iter2 < ymax+1; iter2++)
    {
		for(int iter3 = zmin; iter3 < zmax+1; iter3++)
		{
		  column = iter1;
		  row = iter2;
		  plane = iter3;
		  x = float(column)*packingresx;
		  y = float(row)*packingresy;
		  z = float(plane)*packingresz;
		  inside = -1;
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
				m_Grains[gnum]->columnlist->push_back(column);
				m_Grains[gnum]->rowlist->push_back(row);
				m_Grains[gnum]->planelist->push_back(plane);
			}
		  }
		}
    }
  }
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


void GrainGeneratorFunc::move_grain(size_t gnum, float xc, float yc, float zc)
{
  std::vector<int> voxellist(1000,-1);
  int column, row, plane;
  int occolumn, ocrow, ocplane;
  int nccolumn, ncrow, ncplane;
  int shiftcolumn, shiftrow, shiftplane;
  float oxc = m_Grains[gnum]->centroidx;
  float oyc = m_Grains[gnum]->centroidy;
  float ozc = m_Grains[gnum]->centroidz;
  occolumn = (oxc-(packingresx/2))/packingresx;
  ocrow = (oyc-(packingresy/2))/packingresy;
  ocplane = (ozc-(packingresz/2))/packingresz;
  nccolumn = (xc-(packingresx/2))/packingresx;
  ncrow = (yc-(packingresy/2))/packingresy;
  ncplane = (zc-(packingresz/2))/packingresz;
  shiftcolumn = nccolumn-occolumn;
  shiftrow = ncrow-ocrow;
  shiftplane = ncplane-ocplane;
  m_Grains[gnum]->centroidx = xc;
  m_Grains[gnum]->centroidy = yc;
  m_Grains[gnum]->centroidz = zc;
  for(size_t i=0;i<m_Grains[gnum]->columnlist->size();i++)
  {
    column = m_Grains[gnum]->columnlist->at(i);
    row = m_Grains[gnum]->rowlist->at(i);
    plane = m_Grains[gnum]->planelist->at(i);
    column = column+shiftcolumn;
    row = row+shiftrow;
    plane = plane+shiftplane;
    m_Grains[gnum]->columnlist->at(i) = column;
    m_Grains[gnum]->rowlist->at(i) = row;
    m_Grains[gnum]->planelist->at(i) = plane;
  }
}
void GrainGeneratorFunc::remove_grain(size_t gnum)
{
  int index;
  int col, row, plane;
  for(size_t i=0;i<m_Grains[gnum]->columnlist->size();i++)
  {
    index = -1;
    if(periodic_boundaries == true)
    {
	  col = m_Grains[gnum]->columnlist->at(i);
	  row = m_Grains[gnum]->rowlist->at(i);
	  plane = m_Grains[gnum]->planelist->at(i);
	  if(col < 0) col = m_Grains[gnum]->columnlist->at(i) + packingxpoints;
	  if(col > packingxpoints-1) col = m_Grains[gnum]->columnlist->at(i) - packingxpoints;
	  if(row < 0) row = m_Grains[gnum]->rowlist->at(i) + packingypoints;
	  if(row > packingypoints-1) row = m_Grains[gnum]->rowlist->at(i) - packingypoints;
	  if(plane < 0) plane = m_Grains[gnum]->planelist->at(i) + packingzpoints;
	  if(plane > packingzpoints-1) plane = m_Grains[gnum]->planelist->at(i) - packingzpoints;
	  grainowners[col][row][plane] = grainowners[col][row][plane] - 1;
	}
    if(periodic_boundaries == false)
    {
      if(m_Grains[gnum]->columnlist->at(i) >= 0 && m_Grains[gnum]->columnlist->at(i) <= packingxpoints-1 && m_Grains[gnum]->rowlist->at(i) >= 0 && m_Grains[gnum]->rowlist->at(i) <= packingypoints-1 && m_Grains[gnum]->planelist->at(i) >= 0 && m_Grains[gnum]->planelist->at(i) <= packingzpoints-1)
      {
        grainowners[m_Grains[gnum]->columnlist->at(i)][m_Grains[gnum]->rowlist->at(i)][m_Grains[gnum]->planelist->at(i)] = grainowners[m_Grains[gnum]->columnlist->at(i)][m_Grains[gnum]->rowlist->at(i)][m_Grains[gnum]->planelist->at(i)] - 1;
      }
    }
  }
  determine_neighbors(gnum, -1);
}

void GrainGeneratorFunc::add_grain(size_t gnum)
{
  int index;
  int col, row, plane;
  for(size_t i=0;i<m_Grains[gnum]->columnlist->size();i++)
  {
    index = -1;
    if(periodic_boundaries == true)
    {
	  col = m_Grains[gnum]->columnlist->at(i);
	  row = m_Grains[gnum]->rowlist->at(i);
	  plane = m_Grains[gnum]->planelist->at(i);
	  if(col < 0) col = m_Grains[gnum]->columnlist->at(i) + packingxpoints;
	  if(col > packingxpoints-1) col = m_Grains[gnum]->columnlist->at(i) - packingxpoints;
	  if(row < 0) row = m_Grains[gnum]->rowlist->at(i) + packingypoints;
	  if(row > packingypoints-1) row = m_Grains[gnum]->rowlist->at(i) - packingypoints;
	  if(plane < 0) plane = m_Grains[gnum]->planelist->at(i) + packingzpoints;
	  if(plane > packingzpoints-1) plane = m_Grains[gnum]->planelist->at(i) - packingzpoints;
      grainowners[col][row][plane]++;
    }
    if(periodic_boundaries == false)
    {
      if(m_Grains[gnum]->columnlist->at(i) >= 0 && m_Grains[gnum]->columnlist->at(i) <= packingxpoints-1 && m_Grains[gnum]->rowlist->at(i) >= 0 && m_Grains[gnum]->rowlist->at(i) <= packingypoints-1 && m_Grains[gnum]->planelist->at(i) >= 0 && m_Grains[gnum]->planelist->at(i) <= packingzpoints-1)
      {
        grainowners[m_Grains[gnum]->columnlist->at(i)][m_Grains[gnum]->rowlist->at(i)][m_Grains[gnum]->planelist->at(i)]++;
      }
    }
  }
  determine_neighbors(gnum, 1);
}

void GrainGeneratorFunc::determine_neighbors(size_t gnum, int add)
{
  float x, y, z;
  float xn, yn, zn;
  float dia, dia2;
  int DoverR;
  float xdist, ydist, zdist, totdist;
  int nnum = 0;
  nnum = 0;
  x = m_Grains[gnum]->centroidx;
  y = m_Grains[gnum]->centroidy;
  z = m_Grains[gnum]->centroidz;
  dia = m_Grains[gnum]->equivdiameter;
  for(size_t n=1;n<m_Grains.size();n++)
  {
      xn = m_Grains[n]->centroidx;
      yn = m_Grains[n]->centroidy;
      zn = m_Grains[n]->centroidz;
      dia2 = m_Grains[n]->equivdiameter;
      xdist = fabs(x-xn);
      ydist = fabs(y-yn);
      zdist = fabs(z-zn);
      totdist = (xdist*xdist)+(ydist*ydist)+(zdist*zdist);
      totdist = sqrt(totdist);
      if(totdist < (3*(dia/2.0)))
      {
        DoverR = int(totdist/(dia/2.0));
		for(int iter=DoverR;iter<3;iter++)
		{
          if(add > 0) m_Grains[n]->neighbordistfunc[iter]++;
          if(add < 0) m_Grains[n]->neighbordistfunc[iter] = m_Grains[n]->neighbordistfunc[iter]-1;
		}
      }
      if(totdist < (3*(dia2/2.0)))
      {
        DoverR = int(totdist/(dia2/2.0));
		for(int iter=DoverR;iter<3;iter++)
		{
          if(add > 0) m_Grains[gnum]->neighbordistfunc[iter]++;
          if(add < 0) m_Grains[gnum]->neighbordistfunc[iter] = m_Grains[gnum]->neighbordistfunc[iter]-1;
		}
      }
  }
}

float GrainGeneratorFunc::check_neighborhooderror(int gadd, int gremove)
{
  float neighborerror;
  float sqrerror;
  float dia;
  int nnum;
  int index;
  vector<int> count;
  int phase;
  for (size_t iter = 0; iter < neighbordist.size(); ++iter)
  {
    phase = primaryphases[iter];
    count.resize(simneighbordist[iter].size(),0);
    for(size_t i=0;i<simneighbordist[iter].size();i++)
    {
    simneighbordist[iter][i][0] = 0;
    simneighbordist[iter][i][1] = 0;
    simneighbordist[iter][i][2] = 0;
    }
    if(gadd > 0 && m_Grains[gadd]->phase == phase)
    {
    determine_neighbors(gadd, 1);
    }
    if(gremove > 0 && m_Grains[gremove]->phase == phase)
    {
    determine_neighbors(gremove, -1);
    }
    for(size_t i=1;i<m_Grains.size();i++)
    {
    nnum=0;
    index = i;
    if(index != gremove && m_Grains[index]->phase == phase)
    {
      dia = m_Grains[index]->equivdiameter;
      if(dia > maxdiameter[phase]) dia = maxdiameter[phase];
      if(dia < mindiameter[phase]) dia = mindiameter[phase];
      dia = int((dia-mindiameter[phase])/binstepsize[phase]);
      for(int j=0;j<3;j++)
      {
      nnum = m_Grains[index]->neighbordistfunc[j];
      if(nnum > 0)
      {
        simneighbordist[iter][dia][j] = simneighbordist[iter][dia][j]+nnum;
      }
      }
      count[dia]++;
    }
    }
    if(gadd > 0 && m_Grains[gadd]->phase == phase)
    {
      dia = m_Grains[index]->equivdiameter;
      if(dia > maxdiameter[phase]) dia = maxdiameter[phase];
      if(dia < mindiameter[phase]) dia = mindiameter[phase];
      dia = int((dia-mindiameter[phase])/binstepsize[phase]);
    for(int j=0;j<3;j++)
    {
      nnum = m_Grains[index]->neighbordistfunc[j];
      if(nnum > 0)
      {
      simneighbordist[iter][dia][j] = simneighbordist[iter][dia][j]+nnum;
      }
    }
    count[dia]++;
    }
    for(size_t i=0;i<simneighbordist[iter].size();i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
          simneighbordist[iter][i][j] = simneighbordist[iter][i][j] / double(count[i]);
          if (count[i] == 0) simneighbordist[iter][i][j] = 0.0;
        }
    }
    if(gadd > 0 && m_Grains[gadd]->phase == phase)
    {
    determine_neighbors(gadd, -1);
    }
    if(gremove > 0 && m_Grains[gremove]->phase == phase)
    {
    determine_neighbors(gremove, 1);
    }
  }
  compare_3Ddistributions(simneighbordist, neighbordist, sqrerror);
  neighborerror = sqrerror;
  return neighborerror;
}

void GrainGeneratorFunc::compare_1Ddistributions(std::vector<float> array1, std::vector<float> array2, float &sqrerror)
{
  sqrerror = 0;
  float scalingmag = 0;
  for(size_t i=0;i<array1.size();i++)
  {
    sqrerror = sqrerror + ((array1[i]-array2[i])*(array1[i]-array2[i]));
    scalingmag = scalingmag + array2[i]*array2[i];
  }
//  sqrerror;
  sqrerror = sqrerror/scalingmag;
}
void GrainGeneratorFunc::compare_2Ddistributions(std::vector<std::vector<float> > array1, std::vector<std::vector<float> > array2, float &sqrerror)
{
  sqrerror = 0;
  float scalingmag = 0;
  for(size_t i=0;i<array1.size();i++)
  {
    for(size_t j=0;j<array1[i].size();j++)
    {
      sqrerror = sqrerror + ((array1[i][j]-array2[i][j])*(array1[i][j]-array2[i][j]));
      scalingmag = scalingmag + array2[i][j]*array2[i][j];
    }
  }
  sqrerror = sqrerror/scalingmag;
}
void GrainGeneratorFunc::compare_3Ddistributions(std::vector<std::vector<std::vector<float> > > array1, std::vector<std::vector<std::vector<float> > > array2, float &sqrerror)
{
  sqrerror = 0;
  float scalingmag = 0;
  for(size_t i=0;i<array1.size();i++)
  {
    for(size_t j=0;j<array1[i].size();j++)
    {
      for(size_t k=0;k<array1[i][j].size();k++)
      {
        sqrerror = sqrerror + ((array1[i][j][k]-array2[i][j][k])*(array1[i][j][k]-array2[i][j][k]));
        scalingmag = scalingmag + array2[i][j][k]*array2[i][j][k];
      }
    }
  }
  sqrerror = sqrerror/scalingmag;
}
float GrainGeneratorFunc::check_sizedisterror(int gadd, int gremove)
{
  float dia;
  float sizedisterror = 0;
  float sqrerror;
  int index;
  int count = 0;
  int phase;
  for(size_t iter = 0; iter < grainsizedist.size(); ++iter)
  {
    phase = primaryphases[iter];
    count = 0;
    for(size_t i=0;i<grainsizedist[iter].size();i++)
    {
		simgrainsizedist[iter][i] = 0.0;
    }
    for(size_t b=1;b<m_Grains.size();b++)
    {
		index = b;
		if(index != gremove && m_Grains[index]->surfacegrain == 0 && m_Grains[index]->phase == phase)
		{
		  dia = m_Grains[index]->equivdiameter;
		  dia = (dia-(mindiameter[phase]/2.0))/grainsizediststep[iter];
	      if(dia < 0) dia = 0;
	      if(dia > grainsizedist[iter].size()-1) dia = grainsizedist[iter].size()-1;
	      simgrainsizedist[iter][int(dia)]++;
	      count++;
	    }
    }
    if(gadd > 0 && m_Grains[gadd]->surfacegrain == 0 && m_Grains[gadd]->phase == phase)
    {
	    dia = m_Grains[gadd]->equivdiameter;
	    dia = (dia-(mindiameter[phase]/2.0))/grainsizediststep[iter];
	    if(dia < 0) dia = 0;
	    if(dia > grainsizedist[iter].size()-1) dia = grainsizedist[iter].size()-1;
	    simgrainsizedist[iter][int(dia)]++;
	    count++;
    }
    for(size_t i=0;i<grainsizedist[iter].size();i++)
    {
	    simgrainsizedist[iter][i] = simgrainsizedist[iter][i]/float(count);
	    if(count == 0) simgrainsizedist[iter][i] = 0.0;
    }
  }
  compare_2Ddistributions(simgrainsizedist, grainsizedist, sqrerror);
  sizedisterror = sqrerror;
  return sizedisterror;
}

float GrainGeneratorFunc::check_fillingerror(int gadd, int gremove)
{
  float fillingerror = oldfillingerror*float(packingtotalpoints);
  int col, row, plane;
  if(gadd > 0)
  {
    for(size_t i=0;i<m_Grains[gadd]->columnlist->size();i++)
    {
		col = m_Grains[gadd]->columnlist->at(i);
		row = m_Grains[gadd]->rowlist->at(i);
		plane = m_Grains[gadd]->planelist->at(i);
		if(periodic_boundaries == true)
		{
		  if(col < 0) col = col + packingxpoints;
		  if(col > packingxpoints-1) col = col - packingxpoints;
		  if(row < 0) row = row + packingypoints;
		  if(row > packingypoints-1) row = row - packingypoints;
		  if(plane < 0) plane = plane + packingzpoints;
		  if(plane > packingzpoints-1) plane = plane - packingzpoints;
		  fillingerror = fillingerror + (2*grainowners[col][row][plane]-1);
		}
		if(periodic_boundaries == false)
		{
		  if(col >= 0 && col <= packingxpoints-1 && row >= 0 && row <= packingypoints-1 && plane >= 0 && plane <= packingzpoints-1)
		  {
			  fillingerror = fillingerror + (2*grainowners[col][row][plane]-1);
		  }
		}
    }
  }
  if(gremove > 0)
  {
    for(size_t i=0;i<m_Grains[gremove]->columnlist->size();i++)
    {
		col = m_Grains[gremove]->columnlist->at(i);
		row = m_Grains[gremove]->rowlist->at(i);
		plane = m_Grains[gremove]->planelist->at(i);
		if(periodic_boundaries == true)
		{
		  if(col < 0) col = col + packingxpoints;
		  if(col > packingxpoints-1) col = col - packingxpoints;
		  if(row < 0) row = row + packingypoints;
		  if(row > packingypoints-1) row = row - packingypoints;
		  if(plane < 0) plane = plane + packingzpoints;
		  if(plane > packingzpoints-1) plane = plane - packingzpoints;
		  fillingerror = fillingerror + (-2*grainowners[col][row][plane]+3);
		}
		if(periodic_boundaries == false)
		{
		  if(col >= 0 && col <= packingxpoints-1 && row >= 0 && row <= packingypoints-1 && plane >= 0 && plane <= packingzpoints-1)
		  {
			  fillingerror = fillingerror + (-2*grainowners[col][row][plane]+3);
		  }
		}
    }
  }
  fillingerror = fillingerror/float(packingtotalpoints);
  return fillingerror;
}

void GrainGeneratorFunc::pack_grains()
{
  DREAM3D_RANDOMNG_NEW()

  totalvol = 0;
  float change1, change2, change3;
  int phase;
  float random;
  int newgrain;
  float xc, yc, zc;
  float bestxc, bestyc, bestzc, bestcurrentfillingerror;
  currentfillingerror = 0, oldfillingerror = 0;
  currentneighborhooderror = 0, oldneighborhooderror = 0;
  currentsizedisterror = 0, oldsizedisterror = 0;
  int acceptedmoves = 0;
  double totalprimaryfractions = 0.0;
  // find which phases are primary phases
  for (size_t i = 1; i < phaseType.size();++i)
  {
    if(phaseType[i] == DREAM3D::Reconstruction::PrimaryPhase)
    {
      primaryphases.push_back(i);
      primaryphasefractions.push_back(phasefraction[i]);
      totalprimaryfractions = totalprimaryfractions + phasefraction[i];
    }
  }
  // scale the primary phase fractions to total to 1
  for (size_t i = 0; i < primaryphasefractions.size(); i++)
  {
    primaryphasefractions[i] = primaryphasefractions[i]/totalprimaryfractions;
    if(i > 0) primaryphasefractions[i] = primaryphasefractions[i] + primaryphasefractions[i-1];
  }
  // this initializes the arrays to hold the details of the locations of all of the grains during packing
  initialize_packinggrid();
  // generate the grains
  int gid = 1;
  float currentvol = 0.0;
  m_Grains.resize(2);
  m_Grains[1] = Grain::New();
  float factor = 1.0;
  if(periodic_boundaries == false) factor = 1.1;
  while (currentvol < (factor*totalvol))
  {
    GGseed++;
    random = rg.genrand_res53();
    for (size_t j = 0; j < primaryphases.size(); ++j)
    {
      if(random < primaryphasefractions[j])
      {
        phase = primaryphases[j];
        break;
      }
    }
    generate_grain(gid, phase);
    currentvol = currentvol + m_Grains[gid]->volume;
    gid++;
    m_Grains.resize(gid + 1);
    m_Grains[gid] = Grain::New();
  }
  // initialize the sim and goal size distributions for the primary phases
  grainsizedist.resize(primaryphases.size());
  simgrainsizedist.resize(primaryphases.size());
  grainsizediststep.resize(primaryphases.size());
  for(size_t i = 0; i < primaryphases.size(); i++)
  {
    phase = primaryphases[i];
    grainsizedist[i].resize(40);
    simgrainsizedist[i].resize(40);
    grainsizediststep[i] = ((2*maxdiameter[phase])-(mindiameter[phase]/2.0))/grainsizedist[i].size();
  //  float root2pi = sqrt((2.0 * 3.1415926535897));
    float input = 0;
	float previoustotal = 0;
    for (size_t j=0;j<grainsizedist[i].size();j++)
    {
	    input = (float(j+1)*grainsizediststep[i])+(mindiameter[phase]/2.0);
		if(logf(input) <= avgdiam[phase]) grainsizedist[i][j] = 0.5 - 0.5*(erf((avgdiam[phase]-logf(float(input)))/(sqrtf(2*sddiam[phase]*sddiam[phase])))) - previoustotal;
		if(logf(input) > avgdiam[phase]) grainsizedist[i][j] = 0.5 + 0.5*(erf((logf(float(input))-avgdiam[phase])/(sqrtf(2*sddiam[phase]*sddiam[phase])))) - previoustotal;
		previoustotal = previoustotal + grainsizedist[i][j];
    }
  }
  // initialize the sim and goal neighbor distribution for the primary phases
  neighbordist.resize(primaryphases.size());
  simneighbordist.resize(primaryphases.size());
  for(size_t i = 0; i < primaryphases.size(); i++)
  {
    phase = primaryphases[i];
    neighbordist[i].resize(numdiameterbins[phase]);
    simneighbordist[i].resize(numdiameterbins[phase]);
    for (size_t j=0;j<neighbordist[i].size();j++)
    {
	    neighbordist[i][j].resize(3);
	    simneighbordist[i][j].resize(3);
	    neighbordist[i][j][0] = neighborparams[phase][j][0]*powf(0.5,neighborparams[phase][j][2])+neighborparams[phase][j][1];
	    neighbordist[i][j][1] = neighborparams[phase][j][0]*powf(1.5,neighborparams[phase][j][2])+neighborparams[phase][j][1];
	    neighbordist[i][j][2] = neighborparams[phase][j][0]*powf(2.5,neighborparams[phase][j][2])+neighborparams[phase][j][1];
    }
  }
  //  for each grain : select centroid, determine voxels in grain, monitor filling error and decide of the 50 placements which
  // is the most beneficial, then the grain is added and its neighbors are determined
  oldfillingerror = 1;
  for (size_t i = 1; i < m_Grains.size(); i++)
  {
    bestcurrentfillingerror = 100000000000000.0;
    m_Grains[i]->active = 1;
	xc = sizex/2.0;
	yc = sizey/2.0;
	zc = sizez/2.0;
	m_Grains[i]->centroidx = xc;
	m_Grains[i]->centroidy = yc;
	m_Grains[i]->centroidz = zc;
	insert_grain(i);
	for(int iter=0;iter<10;iter++)
	{
		xc = rg.genrand_res53() * (xpoints * resx);
		yc = rg.genrand_res53() * (ypoints * resy);
		zc = rg.genrand_res53() * (zpoints * resz);
		move_grain(i, xc, yc, zc);
		currentfillingerror = check_fillingerror(i, -1000);
		if(currentfillingerror < bestcurrentfillingerror)
		{
		  bestcurrentfillingerror = currentfillingerror;
		  bestxc = xc;
		  bestyc = yc;
		  bestzc = zc;
		}
	}
	move_grain(i, bestxc, bestyc, bestzc);
    add_grain(i);
	oldfillingerror = bestcurrentfillingerror;
  }
  // determine initial filling, size distribution and neighbor distribution errors
  oldsizedisterror = check_sizedisterror(-1000, -1000);
  oldneighborhooderror = check_neighborhooderror(-1000, -1000);
  oldfillingerror = check_fillingerror(-1000, -1000);
#if ERROR_TXT_OUT
  ofstream outFile;
  string filename = "error.txt";
  outFile.open(filename.c_str());
#endif
  // begin swaping/moving/adding/removing grains to try to improve packing
  for (int iteration = 0; iteration < (250000); iteration++)
  {
	change1 = 0;
    change2 = 0;
    change3 = 0;
    int option = iteration % 4;
#if ERROR_TXT_OUT
	if(iteration%25 == 0) {
	  outFile << oldfillingerror << "	" << oldsizedisterror << "	" << oldneighborhooderror << "	" << m_Grains.size() << endl;
	}
#endif
  // this option adds a grain
    if (option == 0)
    {
      newgrain = m_Grains.size();
      m_Grains.resize(newgrain+1);
      m_Grains[newgrain] = Grain::New();
      random = rg.genrand_res53();
      for (size_t j = 0; j < primaryphases.size();++j)
      {
        if (random < primaryphasefractions[j])
        {
          phase = primaryphases[j];
          break;
        }
      }
      generate_grain(newgrain, phase);
      GGseed++;
      xc = rg.genrand_res53() * (xpoints * resx);
      yc = rg.genrand_res53() * (ypoints * resy);
      zc = rg.genrand_res53() * (zpoints * resz);
      m_Grains[newgrain]->centroidx = xc;
      m_Grains[newgrain]->centroidy = yc;
      m_Grains[newgrain]->centroidz = zc;
      insert_grain(newgrain);
      currentfillingerror = check_fillingerror(newgrain, -1000);
      currentsizedisterror = check_sizedisterror(newgrain, -1000);
      currentneighborhooderror = check_neighborhooderror(newgrain, -1000);
      change1 = (currentfillingerror*currentfillingerror) - (oldfillingerror*oldfillingerror);
      change2 = (currentsizedisterror*currentsizedisterror) - (oldsizedisterror*oldsizedisterror);
      change3 = (currentneighborhooderror*currentneighborhooderror) - (oldneighborhooderror*oldneighborhooderror);
      if (change1 + change2 + change3 < 0)
//      if (fillingerrorweight * change1 + sizedisterrorweight * change2 + neighborhooderrorweight * change3 < 0)
//      if (fillingerrorweight * change1 < 0)
      {
        m_Grains[newgrain]->active = 1;
        add_grain(newgrain);
        oldfillingerror = currentfillingerror;
        oldneighborhooderror = currentneighborhooderror;
        oldsizedisterror = currentsizedisterror;
        acceptedmoves++;
      }
      else
      {
        m_Grains.resize(newgrain);
      }
    }
  // this option removes a grain
    if (option == 1)
    {
      size_t random = int(rg.genrand_res53() * m_Grains.size());
      if (random == 0) random = 1;
	  if (random == m_Grains.size()) random = m_Grains.size()-1;
      currentfillingerror = check_fillingerror(-1000, random);
      currentsizedisterror = check_sizedisterror(-1000, random);
      currentneighborhooderror = check_neighborhooderror(-1000, random);
      change1 = (currentfillingerror - oldfillingerror)/oldfillingerror;
      change1 = (currentfillingerror*currentfillingerror) - (oldfillingerror*oldfillingerror);
      change2 = (currentsizedisterror*currentsizedisterror) - (oldsizedisterror*oldsizedisterror);
      change3 = (currentneighborhooderror*currentneighborhooderror) - (oldneighborhooderror*oldneighborhooderror);
      if (change1 + change2 + change3 < 0)
//      if (fillingerrorweight * change1 + sizedisterrorweight * change2 + neighborhooderrorweight * change3 < 0)
//      if (fillingerrorweight * change1 < 0)
      {
        remove_grain(random);
		m_Grains.erase(m_Grains.begin()+random);
        oldfillingerror = currentfillingerror;
        oldneighborhooderror = currentneighborhooderror;
        oldsizedisterror = currentsizedisterror;
        acceptedmoves++;
      }
    }
  // this option removes one grain and adds another grain
    if (option == 2)
    {
      size_t random1 = int(rg.genrand_res53() * m_Grains.size());
      if (random1 == 0) random1 = 1;
	  if (random1 == m_Grains.size()) random1 = m_Grains.size()-1;
      newgrain = m_Grains.size();
      m_Grains.resize(newgrain+1);
      m_Grains[newgrain] = Grain::New();
      random = rg.genrand_res53();
      for (size_t j = 0; j < primaryphases.size();++j)
      {
        if (random < primaryphasefractions[j])
        {
          phase = primaryphases[j];
          break;
        }
      }
      generate_grain(newgrain, phase);
      GGseed++;
      xc = rg.genrand_res53() * (xpoints * resx);
      yc = rg.genrand_res53() * (ypoints * resy);
      zc = rg.genrand_res53() * (zpoints * resz);
      m_Grains[newgrain]->centroidx = xc;
      m_Grains[newgrain]->centroidy = yc;
      m_Grains[newgrain]->centroidz = zc;
      insert_grain(newgrain);
      currentfillingerror = check_fillingerror(newgrain, random1);
      currentsizedisterror = check_sizedisterror(newgrain, random1);
      currentneighborhooderror = check_neighborhooderror(newgrain, random1);
      change1 = (currentfillingerror*currentfillingerror) - (oldfillingerror*oldfillingerror);
      change2 = (currentsizedisterror*currentsizedisterror) - (oldsizedisterror*oldsizedisterror);
      change3 = (currentneighborhooderror*currentneighborhooderror) - (oldneighborhooderror*oldneighborhooderror);
      if (change1 + change2 + change3 < 0)
//      if (fillingerrorweight * change1 + sizedisterrorweight * change2 + neighborhooderrorweight * change3 < 0)
//      if (fillingerrorweight * change1 < 0)
      {
        m_Grains[newgrain]->active = 1;
        add_grain(newgrain);
        remove_grain(random1);
		m_Grains.erase(m_Grains.begin()+random1);
        oldfillingerror = currentfillingerror;
        oldneighborhooderror = currentneighborhooderror;
        oldsizedisterror = currentsizedisterror;
        acceptedmoves++;
      }
      else
      {
        m_Grains.resize(newgrain);
      }
    }
  // this option removes a grain and replaces it with another grain at the same centroid
    if (option == 3)
    {
      size_t random1 = int(rg.genrand_res53() * m_Grains.size());
      if (random1 == 0) random1 = 1;
	  if (random1 == m_Grains.size()) random1 = m_Grains.size()-1;
      newgrain = m_Grains.size();
      m_Grains.resize(newgrain+1);
      m_Grains[newgrain] = Grain::New();
      random = rg.genrand_res53();
      for (size_t j = 0; j < primaryphases.size();++j)
      {
        if (random < primaryphasefractions[j])
        {
          phase = primaryphases[j];
          break;
        }
      }
      generate_grain(newgrain, phase);
      GGseed++;
      xc = m_Grains[random1]->centroidx;
      yc = m_Grains[random1]->centroidy;
      zc = m_Grains[random1]->centroidz;
      m_Grains[newgrain]->centroidx = xc;
      m_Grains[newgrain]->centroidy = yc;
      m_Grains[newgrain]->centroidz = zc;
      insert_grain(newgrain);
      currentfillingerror = check_fillingerror(newgrain, random1);
      currentsizedisterror = check_sizedisterror(newgrain, random1);
      currentneighborhooderror = check_neighborhooderror(newgrain, random1);
      change1 = (currentfillingerror*currentfillingerror) - (oldfillingerror*oldfillingerror);
      change2 = (currentsizedisterror*currentsizedisterror) - (oldsizedisterror*oldsizedisterror);
      change3 = (currentneighborhooderror*currentneighborhooderror) - (oldneighborhooderror*oldneighborhooderror);
      if (change1 + change2 + change3 < 0)
//      if (fillingerrorweight * change1 + sizedisterrorweight * change2 + neighborhooderrorweight * change3 < 0)
//      if (fillingerrorweight * change1 < 0)
      {
        m_Grains[newgrain]->active = 1;
        add_grain(newgrain);
        remove_grain(random1);
		m_Grains.erase(m_Grains.begin()+random1);
        oldfillingerror = currentfillingerror;
        oldneighborhooderror = currentneighborhooderror;
        oldsizedisterror = currentsizedisterror;
        acceptedmoves++;
      }
      else
      {
        m_Grains.resize(newgrain);
      }
    }
  }
#if ERROR_TXT_OUT1
  outFile.close();


//  ofstream outFile;
  filename = "test.vtk";
  outFile.open(filename.c_str());
  outFile << "# vtk DataFile Version 2.0" << endl;
  outFile << "data set from FFT2dx_GB" << endl;
  outFile << "ASCII" << endl;
  outFile << "DATASET STRUCTURED_POINTS" << endl;
  outFile << "DIMENSIONS " << packingxpoints << " " << packingypoints << " " << packingzpoints << endl;
  outFile << "ORIGIN 0.0 0.0 0.0" << endl;
  outFile << "SPACING " << packingresx << " " << packingresy << " " << packingresz << endl;
  outFile << "POINT_DATA " << packingxpoints*packingypoints*packingzpoints << endl;
  outFile << endl;
  outFile << endl;
  outFile << "SCALARS GrainID int  1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (packingzpoints); i++)
  {
    for (int j = 0; j < (packingypoints); j++)
    {
      for (int k = 0; k < (packingxpoints); k++)
      {
        int name = grainowners[k][j][i];
        if(i%20 == 0 && i > 0) outFile << endl;
        outFile << "     ";
        if(name < 100) outFile << " ";
        if(name < 10) outFile << " ";
        outFile << name;
      }
    }
  }
  outFile.close();
#endif
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

  for (int i = 0; i < totalpoints; i++)
  {
    unassigned[i] = false;
  }
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
                unassigned[currentpoint] = true;
              }
              if (grain_indicies[currentpoint] == 0 && unassigned[currentpoint] == false)
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
    unassigned[i] = false;
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

  for (int i = 0; i < totalpoints; i++)
  {
	if (grain_indicies[i] <= 0) unassigned[i] = true;
  }
  while (unassignedcount != 0)
  {
	  unassignedcount = 0;
	  timestep = timestep + 5;
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
			  if(unassigned[index] == true)
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
		if (grain_indicies[i] > 0) unassigned[i] = false;
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
	  size_t count;
	  int good;
	  int neighbor;
	  int column, row, plane;
	  int index;
	  gsizes.resize(m_Grains.size());
	  for (int i = 1; i < m_Grains.size(); i++)
	  {
		gsizes[i] = 0;
	  }
	  for (int i = 0; i < totalpoints; i++)
	  {
		unassigned[i] = false;
	  }
	  for (int i = 0; i < totalpoints; i++)
	  {
		if(unassigned[i] == false && grain_indicies[i] > 0)
		{
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
						if (good == 1 && grain_indicies[neighbor] == grain_indicies[index] && unassigned[neighbor] == false)
						{
							currentvlist.push_back(neighbor);
							unassigned[neighbor] = true;
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
						if (grain_indicies[neighbor] == grain_indicies[index] && unassigned[neighbor] == false)
						{
							currentvlist.push_back(neighbor);
							unassigned[neighbor] = true;
						}
					}
				}
				count++;
			}
			if(vlists[grain_indicies[currentvlist[0]]].size() > 0)
			{
				if(vlists[grain_indicies[currentvlist[0]]].size() < currentvlist.size())
				{
					for (size_t k = 0; k < vlists[grain_indicies[currentvlist[0]]].size(); k++)
					{
						grain_indicies[vlists[grain_indicies[currentvlist[0]]][k]] = -1;
					}
					vlists[grain_indicies[currentvlist[0]]].resize(currentvlist.size());
					vlists[grain_indicies[currentvlist[0]]].swap(currentvlist);
				}
				else if(vlists[grain_indicies[currentvlist[0]]].size() >= currentvlist.size())
				{
					for (size_t k = 0; k < currentvlist.size(); k++)
					{
						grain_indicies[currentvlist[k]] = -1;
					}
				}
			}
			else if(vlists[grain_indicies[currentvlist[0]]].size() == 0)
			{
				vlists[grain_indicies[currentvlist[0]]].resize(currentvlist.size());
				vlists[grain_indicies[currentvlist[0]]].swap(currentvlist);
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
		unassigned[i] = true;
		if (grain_indicies[i] > 0)
		{
		  grain_indicies[i] = newnames[grain_indicies[i]];
		  unassigned[i] = false;
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
    generate_grain(currentnumgrains, phase);
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
  oldsizedisterror = check_sizedisterror(-1000,-1000);
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
    currentsizedisterror = check_sizedisterror(-1000,-1000);
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
  int surfacegrain = 1;
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
        m_Grains[grain]->surfacegrain = surfacegrain;
      }
      if ((column == 0 || column == (xpoints - 1) || row == 0 || row == (ypoints - 1)) && zpoints == 1)
      {
        m_Grains[grain]->surfacegrain = surfacegrain;
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
  float q[5];
  for(int i = 0; i < totalpoints; i++)
  {
    euler1s[i] = m_Grains[grain_indicies[i]]->euler1;
    euler2s[i] = m_Grains[grain_indicies[i]]->euler2;
    euler3s[i] = m_Grains[grain_indicies[i]]->euler3;
    OrientationMath::eulertoQuat(q, euler1s[i], euler2s[i], euler3s[i]);
    quats[i*5 + 0] = 1.0;
    quats[i*5 + 1] = q[1];
    quats[i*5 + 2] = q[2];
    quats[i*5 + 3] = q[3];
    quats[i*5 + 4] = q[4];
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


float GrainGeneratorFunc::gamma(float x)
{
    int i,k,m;
    float ga,gr,r,z;


    static float g[] = {
        1.0f,
        0.5772156649015329f,
       -0.6558780715202538f,
       -0.420026350340952e-1f,
        0.1665386113822915f,
       -0.421977345555443e-1f,
       -0.9621971527877e-2f,
        0.7218943246663e-2f,
       -0.11651675918591e-2f,
       -0.2152416741149e-3f,
        0.1280502823882e-3f,
       -0.201348547807e-4f,
       -0.12504934821e-5f,
        0.1133027232e-5f,
       -0.2056338417e-6f,
        0.6116095e-8f,
        0.50020075e-8f,
       -0.11812746e-8f,
        0.1043427e-9f,
        0.77823e-11f,
       -0.36968e-11f,
        0.51e-12f,
       -0.206e-13f,
       -0.54e-14f,
        0.14e-14f};

    if (x > 171.0f) return 1e308;    // This value is an overflow flag.
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
                ga = -1 * m_pi/(x*ga*sinf(m_pi*x));
            }
        }
    }
    return ga;
}


float GrainGeneratorFunc::gammastirf(float x)
{
    float result;
    float y;
    float w;
    float v;
    float stir;

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
        v = powf(x, 0.5*x-0.25);
        y = v*(v/y);
    }
    else
    {
        y = powf(x, x-0.5)/y;
    }
    result = 2.50662827463100050242*y*w;
    return result;
}
float GrainGeneratorFunc::lngamma(float x, float& sgngam)
{
    float result;
    float a;
    float b;
    float c;
    float p;
    float q;
    float u;
    float w;
    float z;
    int i;
    float logpi;
    float ls2pi;
    float tmp;

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
        z = q*sinf(m_pi*z);
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
float GrainGeneratorFunc::erf(float x)
{
    float result;
    float xsq;
    float s;
    float p;
    float q;


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
float GrainGeneratorFunc::erfc(float x)
{
    float result;
    float p;
    float q;

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
float GrainGeneratorFunc::incompletebeta(float a, float b, float x)
{
  machineepsilon = 5E-16;
  maxrealnumber = 1E300;
  minrealnumber = 1E-300;
    float result;
    float t;
    float xc;
    float w;
    float y;
    int flag;
    float sg;
    float big;
    float biginv;
    float maxgam;
    float minlog;
    float maxlog;

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
        t = powf(xc, b);
        t = t*powf(x, a);
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
float GrainGeneratorFunc::incompletebetafe(float a, float b, float x, float big, float biginv)
{
    float result;
    float xk;
    float pk;
    float pkm1;
    float pkm2;
    float qk;
    float qkm1;
    float qkm2;
    float k1;
    float k2;
    float k3;
    float k4;
    float k5;
    float k6;
    float k7;
    float k8;
    float r;
    float t;
    float ans;
    float thresh;
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
float GrainGeneratorFunc::incompletebetafe2(float a, float b, float x, float big, float biginv)
{
    float result;
    float xk;
    float pk;
    float pkm1;
    float pkm2;
    float qk;
    float qkm1;
    float qkm2;
    float k1;
    float k2;
    float k3;
    float k4;
    float k5;
    float k6;
    float k7;
    float k8;
    float r;
    float t;
    float ans;
    float z;
    float thresh;
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
float GrainGeneratorFunc::incompletebetaps(float a, float b, float x, float maxgam)
{
  float result;
    float s;
    float t;
    float u;
    float v;
    float n;
    float t1;
    float z;
    float ai;
    float sg;

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
        s = s*t*powf(x, a);
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


