/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "LoadStatsData.h"
#include <map>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <cstddef>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>

using namespace std;

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/HDF5/H5StatsReader.h"

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

const static float m_pi = M_PI;

using namespace std;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadStatsData::LoadStatsData() :
    m_ErrorCondition(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadStatsData::~LoadStatsData()
{
}

void LoadStatsData::execute()
{

	int err = 0;
  DREAM3D_RANDOMNG_NEW()
  H5StatsReader::Pointer h5reader = H5StatsReader::New(m_H5StatsFile);
  // Open the HDF5 Stats file
  if (h5reader.get() == NULL)
  {
    return;
  }

  err = readReconStatsData(h5reader);
  err = readAxisOrientationData(h5reader);
  err = readODFData(h5reader);
  err = readMisorientationData(h5reader);
  initializeAttributes();

  // If there is an error set this to something negative and also set a message
  m_ErrorMessage = "PackGrainsGen2 Completed";
  m_ErrorCondition = 0;
}
#define GG_INIT_DOUBLE_ARRAY(array, value, size)\
    for(size_t n = 0; n < size; ++n) { array[n] = (value); }

void LoadStatsData::initializeAttributes()
{

  m->sizex = m->xpoints * m->resx;
  m->sizey = m->ypoints * m->resy;
  m->sizez = m->zpoints * m->resz;
  m->totalvol = m->sizex * m->sizey * m->sizez;
  m->totalpoints = m->xpoints * m->ypoints * m->zpoints;  
  const size_t startIndex = 0;
  const size_t endIndex = m->totalpoints;
  m->grain_indicies = m->m_GrainIndicies->WritePointer(startIndex, endIndex);
  m->ellipfuncs = m->m_Ellipfuncs->WritePointer(startIndex, endIndex);
  m->phases = m->m_Phases->WritePointer(startIndex, endIndex);
  m->euler1s = m->m_Euler1s->WritePointer(startIndex, endIndex);
  m->euler2s = m->m_Euler2s->WritePointer(startIndex, endIndex);
  m->euler3s = m->m_Euler3s->WritePointer(startIndex, endIndex);
  m->surfacevoxels = m->m_SurfaceVoxels->WritePointer(startIndex, endIndex);

	for(int i=0;i<m->totalpoints;i++)
	{
		m->grain_indicies[i] = 0;
		m->ellipfuncs[i] = -1;
		m->phases[i] = 0;
		m->euler1s[i] = -1;
		m->euler2s[i] = -1;
		m->euler3s[i] = -1;
		m->surfacevoxels[i] = 0;
	}
}
void LoadStatsData::initializeArrays(std::vector<Ebsd::CrystalStructure> structures)
{
  //------------------
  size_t nElements = 0;
  size_t size = structures.size();

  m->crystruct.resize(size+1);
  m->pptFractions.resize(size + 1);
  m->phaseType.resize(size+1);
  m->phasefraction.resize(size+1);

  // Initialize the first slot in these arrays since they should never be used
  m->crystruct[0] = Ebsd::UnknownCrystalStructure;
  m->phasefraction[0] = 0.0;
  m->phaseType[0] = DREAM3D::Reconstruction::UnknownPhaseType;
  m->pptFractions[0] = -1.0;

  m->mindiameter.resize(size+1);
  m->maxdiameter.resize(size+1);
  m->binstepsize.resize(size+1);
  m->numdiameterbins.resize(size+1);
  m->avgdiam.resize(size+1);
  m->sddiam.resize(size+1);
  m->bovera.resize(size+1);
  m->covera.resize(size+1);
  m->coverb.resize(size+1);
  m->omega3.resize(size+1);
  m->neighborparams.resize(size+1);

  m->actualodf.resize(size+1);
  m->simodf.resize(size+1);
  m->actualmdf.resize(size+1);
  m->simmdf.resize(size+1);
  m->axisodf.resize(size+1);
  for(size_t i= 1; i < size+1; ++i)
  {
    if(m->crystruct[i] == Ebsd::Hexagonal) nElements = 36*36*12;
    if(m->crystruct[i] == Ebsd::Cubic) nElements = 18*18*18;

    float initValue = 1.0/(float)(nElements);
    m->actualodf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(m->actualodf[i], initValue, nElements);

    m->simodf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(m->simodf[i], 0.0, nElements);
    m->actualmdf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(m->actualmdf[i], initValue, nElements);
    m->simmdf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(m->simmdf[i], 0.0, nElements);

    nElements = 36*36*36;
    initValue = (1.0/float(nElements));
    m->axisodf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(m->axisodf[i], initValue, nElements);
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
  var[index].resize(m->numdiameterbins[index]);\
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
  var[index].resize(m->numdiameterbins[index]);\
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

int LoadStatsData::readReconStatsData(H5StatsReader::Pointer h5io)
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
  int size = (int)(phases.size());

  int phase = -1;

  for (int i = 0; i < size; i++)
  {
      phase = phases[i];
    m->crystruct[phase] = structures[i];

    /* Read the PhaseFraction Value*/
      std::vector<float> pFraction;
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::PhaseFraction, pFraction);
    m->phasefraction[phase] = pFraction.front();

    std::vector<unsigned int> phasetypes;
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::PhaseType, phasetypes);
    m->phaseType[phase] = static_cast<DREAM3D::Reconstruction::PhaseType>(phasetypes[0]);

    // If the Phase Type is Precipitate then we need the pptFraction on Boundary
    if (m->phaseType[phase] == DREAM3D::Reconstruction::PrecipitatePhase)
    {
      float f = -1.0f;
      err = h5io->readScalarAttribute(phase, DREAM3D::HDF5::PhaseType, DREAM3D::HDF5::PrecipitateBoundaryFraction, f);
      if (err < 0) {
        f = -1.0f;
      }
      m->pptFractions[phase] = f;
    }
    if (m->phaseType[phase] != DREAM3D::Reconstruction::PrecipitatePhase) m->pptFractions[phase] = -1.0;

    /* Read the BinNumbers data set */
    std::vector<float> bins;
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::BinNumber, bins);
    CHECK_STATS_READ_ERROR(err, DREAM3D::HDF5::Statistics, DREAM3D::HDF5::BinNumber)
    m->numdiameterbins[phase] = bins.size();
    size_t nBins = bins.size();

    /* Read the Grain_Diameter_Info Data */
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::Grain_Diameter_Info, grainDiamInfo);
    CHECK_STATS_READ_ERROR(err,  DREAM3D::HDF5::Statistics, DREAM3D::HDF5::Grain_Diameter_Info)

    m->binstepsize[phase] = grainDiamInfo[0];
    m->maxdiameter[phase]  = grainDiamInfo[1];
    m->mindiameter[phase] = grainDiamInfo[2];

    /* Read the Grain_Size_Distribution Data */
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::Grain_Size_Distribution, double_data);
    CHECK_STATS_READ_ERROR(err,  DREAM3D::HDF5::Statistics, DREAM3D::HDF5::Grain_Size_Distribution)
    m->avgdiam[phase] = double_data[0];
    m->sddiam[phase] = double_data[1];

    DREAM3D::Reconstruction::DistributionType dt;
    std::string disType;

    /* Read the Shape Data */
    READ_2_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVBoverA_Distributions, m->bovera, DREAM3D::Reconstruction::Beta, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::BetaColumnCount);
    READ_2_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVCoverA_Distributions, m->covera, DREAM3D::Reconstruction::Beta, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::BetaColumnCount);
    READ_2_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVCoverB_Distributions, m->coverb, DREAM3D::Reconstruction::Beta, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::BetaColumnCount);

    /* Read the Omega3 Data */
    READ_2_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVOmega3_Distributions, m->omega3, DREAM3D::Reconstruction::Beta, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::BetaColumnCount);

    /* Read the Neighbor Data - This MUST be the last one because of how variables are assigned bvalues and used in the next section */
    READ_3_COLUMN_STATS_DATA(err, phase, phase, DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions, m->neighborparams, DREAM3D::Reconstruction::Power, DREAM3D::HDF5::Alpha, DREAM3D::HDF5::Beta, DREAM3D::HDF5::Exp_k, DREAM3D::HDF5::PowerLawColumnCount);
  }
  return err;
}

#define GGF_CHECK_READ_ERROR(func, name)\
    std::cout << "GrainGeneratorFunc::" << #func << " Error: There was an error trying to read the data set '"\
      << name << "' from the HDF5 file." << std::endl;\
      std::cout << "  File: " << __FILE__ << std::endl;\
      std::cout << "  Line: " << __LINE__ << std::endl;\

int LoadStatsData::readAxisOrientationData(H5StatsReader::Pointer h5io)
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
    m->axisodf[phase][k] = totaldensity;
    }
  }
  return err;
}

int LoadStatsData::readODFData(H5StatsReader::Pointer h5io)
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
    if(m->crystruct[phase] == Ebsd::Hexagonal) numbins = 36*36*12;
    if(m->crystruct[phase] == Ebsd::Cubic) numbins = 18*18*18;

    if (numbins != density.size() )
    {
    std::cout << "GrainGeneratorFunc::readODFData Error: Mismatch in number of elements in the 'ODF' "
       << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
       << " elements and we need " << numbins << " Elements. "<< std::endl;
    return -1;
    }
    for (size_t j=0;j<numbins;j++)
    {
    m->actualodf[phase][j] = density[j];
    }
  }
  return err;
}

int LoadStatsData::readMisorientationData(H5StatsReader::Pointer h5io)
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
    if(m->crystruct[phase] == Ebsd::Hexagonal) numbins = 36*36*12;
    if(m->crystruct[phase] == Ebsd::Cubic) numbins = 18*18*18;

    if (numbins != density.size() )
    {
    std::cout << "GrainGeneratorFunc::readMisorientationData Error: Mismatch in number of elements in the 'ODF' "
       << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size()
       << " elements and we need " << numbins << " Elements. "<< std::endl;
    return -1;
    }

    for (size_t k = 0; k < numbins; k++)
    {
    m->actualmdf[phase][k] = density[k];
    }
  }
  return err;
}



