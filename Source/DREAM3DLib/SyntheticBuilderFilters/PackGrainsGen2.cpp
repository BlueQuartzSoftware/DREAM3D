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

#include "PackGrainsGen2.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3DLib/ShapeOps/CylinderOps.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/ShapeOps/SuperEllipsoidOps.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = M_PI;


#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();
#define GG_INIT_DOUBLE_ARRAY(array, value, size)\
    for(size_t n = 0; n < size; ++n) { array[n] = (value); }
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
#define GGF_CHECK_READ_ERROR(func, name)\
    std::cout << "GrainGeneratorFunc::" << #func << " Error: There was an error trying to read the data set '"\
      << name << "' from the HDF5 file." << std::endl;\
      std::cout << "  File: " << __FILE__ << std::endl;\
      std::cout << "  Line: " << __LINE__ << std::endl;\

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PackGrainsGen2::PackGrainsGen2()
{
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

  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());

  Seed = MXA::getMilliSeconds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PackGrainsGen2::~PackGrainsGen2()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackGrainsGen2::execute()
{
  DataContainer* m = getDataContainer();
#if ERROR_TXT_OUT
  std::ofstream outFile;
  std::string filename = "error.txt";
  outFile.open(filename.c_str(), std::ios_base::binary);
#endif
  int err = 0;
  setErrorCondition(err);
  unsigned long long int Seed = MXA::getMilliSeconds();
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed)

  H5StatsReader::Pointer h5reader = H5StatsReader::New(m_H5StatsFile);
  // Open the HDF5 Stats file
  if (h5reader.get() == NULL)
  {
    setErrorCondition(-1);
    setErrorMessage("Error Loading H5 Stats File");
    return;
  }

  err = readReconStatsData(h5reader);
  if (err < 0)
  {
    setErrorCondition(-1);
    setErrorMessage("Error Reading the Stats Data from the HDF5 File");
    notify( getErrorMessage().c_str(), 0, Observable::UpdateErrorMessage);
    return;
  }

  err = readAxisOrientationData(h5reader);
  if (err < 0)
  {
    setErrorCondition(-1);
    setErrorMessage("Error Reading the Axis Orientation Data from the HDF5 File");
    notify( getErrorMessage().c_str(), 0, Observable::UpdateErrorMessage);
    return;
  }

  initializeAttributes();

 // float change1, change2;
  float change;
  int phase;
  size_t randomgrain;
  float random;
//  int newgrain;
 // float check;
  float xc, yc, zc;
  float oldxc, oldyc, oldzc;
  oldfillingerror = 0;
  currentneighborhooderror = 0, oldneighborhooderror = 0;
  currentsizedisterror = 0, oldsizedisterror = 0;
  int acceptedmoves = 0;
  double totalprimaryfractions = 0.0;
  // find which phases are primary phases
  for (size_t i = 1; i < m->phaseType.size(); ++i)
  {
    if(m->phaseType[i] == DREAM3D::Reconstruction::PrimaryPhase)
    {
      primaryphases.push_back(i);
      primaryphasefractions.push_back(m->phasefraction[i]);
      totalprimaryfractions = totalprimaryfractions + m->phasefraction[i];
    }
  }
  // scale the primary phase fractions to total to 1
  for (size_t i = 0; i < primaryphasefractions.size(); i++)
  {
    primaryphasefractions[i] = primaryphasefractions[i] / totalprimaryfractions;
    if(i > 0) primaryphasefractions[i] = primaryphasefractions[i] + primaryphasefractions[i - 1];
  }
  // this initializes the arrays to hold the details of the locations of all of the grains during packing
  initialize_packinggrid();
  // initialize the sim and goal size distributions for the primary phases
  grainsizedist.resize(primaryphases.size());
  simgrainsizedist.resize(primaryphases.size());
  grainsizediststep.resize(primaryphases.size());
  for (size_t i = 0; i < primaryphases.size(); i++)
  {
    phase = primaryphases[i];
    grainsizedist[i].resize(40);
    simgrainsizedist[i].resize(40);
    grainsizediststep[i] = ((2 * maxdiameter[phase]) - (mindiameter[phase] / 2.0)) / grainsizedist[i].size();
    float input = 0;
    float previoustotal = 0;
    for (size_t j = 0; j < grainsizedist[i].size(); j++)
    {
      input = (float(j + 1) * grainsizediststep[i]) + (mindiameter[phase] / 2.0);
      if(logf(input) <= avgdiam[phase]) grainsizedist[i][j] = 0.5
          - 0.5 * (DREAM3DMath::erf((avgdiam[phase] - logf(float(input))) / (sqrtf(2 * sddiam[phase] * sddiam[phase])))) - previoustotal;
      if(logf(input) > avgdiam[phase]) grainsizedist[i][j] = 0.5
          + 0.5 * (DREAM3DMath::erf((logf(float(input)) - avgdiam[phase]) / (sqrtf(2 * sddiam[phase] * sddiam[phase])))) - previoustotal;
      previoustotal = previoustotal + grainsizedist[i][j];
    }
  }
  // generate the grains and monitor the size distribution error while doing so. After grains are generated, no new grains can enter or leave the structure.
  int gid = 1;
  float currentvol = 0.0;
  m->m_Grains.resize(2);
  m->m_Grains[1] = Field::New();
  float factor = 1.0;
  float iter = 0;
  while (currentvol < (factor * totalvol))
  {
    iter++;
    Seed++;
    random = rg.genrand_res53();
    for (size_t j = 0; j < primaryphases.size(); ++j)
    {
      if(random < primaryphasefractions[j])
      {
        phase = primaryphases[j];
        break;
      }
    }
    generate_grain(gid, phase, Seed);
    currentsizedisterror = check_sizedisterror(gid, -1000);
    change = (currentsizedisterror) - (oldsizedisterror);
    if(change > 0 || currentsizedisterror > (1.0-(iter*0.001)))
    {
       m->m_Grains[gid]->active = 1;
       oldsizedisterror = currentsizedisterror;
       currentvol = currentvol + m->m_Grains[gid]->volume;
       gid++;
       m->m_Grains.resize(gid + 1);
       m->m_Grains[gid] = Field::New();
       iter = 0;
    }
  }
  if(m_periodic_boundaries == false)
  {
    iter = 0;
    int xgrains, ygrains, zgrains;
    xgrains = int(powf((m->m_Grains.size()*(sizex/sizey)*(sizex/sizez)),(1.0/3.0))+1);
    ygrains = int(xgrains*(sizey/sizex)+1);
    zgrains = int(xgrains*(sizez/sizex)+1);
    factor = 0.25 * (1.0 - (float((xgrains-2)*(ygrains-2)*(zgrains-2))/float(xgrains*ygrains*zgrains)));
    while (currentvol < ((1+factor) * totalvol))
    {
      iter++;
	  Seed++;
      random = rg.genrand_res53();
      for (size_t j = 0; j < primaryphases.size(); ++j)
      {
		if(random < primaryphasefractions[j])
		{
			phase = primaryphases[j];
			break;
		}
      }
      generate_grain(gid, phase, Seed);
      currentsizedisterror = check_sizedisterror(gid, -1000);
      change = (currentsizedisterror) - (oldsizedisterror);
      if(change > 0 || currentsizedisterror > (1.0-(iter*0.001)))
      {
		m->m_Grains[gid]->active = 1;
		oldsizedisterror = currentsizedisterror;
		currentvol = currentvol + m->m_Grains[gid]->volume;
		gid++;
		m->m_Grains.resize(gid + 1);
		m->m_Grains[gid] = Field::New();
		iter = 0;
	  }
    }
  }
  // initialize the sim and goal neighbor distribution for the primary phases
  neighbordist.resize(primaryphases.size());
  simneighbordist.resize(primaryphases.size());
  for (size_t i = 0; i < primaryphases.size(); i++)
  {
    phase = primaryphases[i];
    neighbordist[i].resize(numdiameterbins[phase]);
    simneighbordist[i].resize(numdiameterbins[phase]);
    for (size_t j = 0; j < neighbordist[i].size(); j++)
    {
      neighbordist[i][j].resize(3);
      simneighbordist[i][j].resize(3);
      neighbordist[i][j][0] = neighborparams[phase][j][0] * powf(0.5, neighborparams[phase][j][2]) + neighborparams[phase][j][1];
      neighbordist[i][j][1] = neighborparams[phase][j][0] * powf(1.5, neighborparams[phase][j][2]) + neighborparams[phase][j][1];
      neighbordist[i][j][2] = neighborparams[phase][j][0] * powf(2.5, neighborparams[phase][j][2]) + neighborparams[phase][j][1];
    }
  }
  //  for each grain : select centroid, determine voxels in grain, monitor filling error and decide of the 10 placements which
  // is the most beneficial, then the grain is added and its neighbors are determined
  int numgrains = m->m_Grains.size();
  columnlist.resize(numgrains);
  rowlist.resize(numgrains);
  planelist.resize(numgrains);
  packqualities.resize(numgrains);
  fillingerror = 1;
  for (size_t i = 1; i < numgrains; i++)
  {
    xc = sizex / 2.0;
    yc = sizey / 2.0;
    zc = sizez / 2.0;
    m->m_Grains[i]->centroidx = xc;
    m->m_Grains[i]->centroidy = yc;
    m->m_Grains[i]->centroidz = zc;
    insert_grain(i);
    fillingerror = check_fillingerror(i,-1000);
    for (int iter = 0; iter < 10; iter++)
    {
      xc = rg.genrand_res53() * (m->getXPoints() * m->getXRes());
      yc = rg.genrand_res53() * (m->getYPoints() * m->getYRes());
      zc = rg.genrand_res53() * (m->getZPoints() * m->getZRes());
      oldxc = m->m_Grains[i]->centroidx;
      oldyc = m->m_Grains[i]->centroidy;
      oldzc = m->m_Grains[i]->centroidz;
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000,i);
      move_grain(i, xc, yc, zc);
      fillingerror = check_fillingerror(i, -1000);
      if(fillingerror > oldfillingerror)
      {
		fillingerror = check_fillingerror(-1000,i);
		move_grain(i, oldxc, oldyc, oldzc);
		fillingerror = check_fillingerror(i,-1000);
      }
    }
  }

  // determine initial filling and neighbor distribution errors
  oldneighborhooderror = check_neighborhooderror(-1000, -1000);
  // begin swaping/moving/adding/removing grains to try to improve packing
  for (int iteration = 0; iteration < (100*numgrains); iteration++)
  {
//    change1 = 0;
//    change2 = 0;
    int option = iteration % 2;
#if ERROR_TXT_OUT
    if(iteration%25 == 0)
    {
      outFile << iteration << " " << fillingerror << "  " << oldsizedisterror << "  " << oldneighborhooderror << "  " << numgrains << " " << acceptedmoves << std::endl;
    }
#endif
    // JUMP - this option moves one grain to a random spot in the volume
    if(option == 0)
    {
      randomgrain = int(rg.genrand_res53() * numgrains);
      if(randomgrain == 0) randomgrain = 1;
      if(randomgrain == numgrains) randomgrain = numgrains - 1;
      Seed++;
      xc = rg.genrand_res53() * (m->getXPoints() * m->getXRes());
      yc = rg.genrand_res53() * (m->getYPoints() * m->getYRes());
      zc = rg.genrand_res53() * (m->getZPoints() * m->getZRes());
      oldxc = m->m_Grains[randomgrain]->centroidx;
      oldyc = m->m_Grains[randomgrain]->centroidy;
      oldzc = m->m_Grains[randomgrain]->centroidz;
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000,randomgrain);
      move_grain(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(randomgrain,-1000);
//      currentneighborhooderror = check_neighborhooderror(-1000, random);
//      change2 = (currentneighborhooderror * currentneighborhooderror) - (oldneighborhooderror * oldneighborhooderror);
      if(fillingerror <= oldfillingerror)
      {
//        oldneighborhooderror = currentneighborhooderror;
        acceptedmoves++;
      }
      else if(fillingerror > oldfillingerror)
      {
		fillingerror = check_fillingerror(-1000,randomgrain);
		move_grain(randomgrain, oldxc, oldyc, oldzc);
		fillingerror = check_fillingerror(randomgrain,-1000);
      }
    }
    // NUDGE - this option moves one grain to a spot close to its current centroid
    if(option == 1)
    {
      randomgrain = int(rg.genrand_res53() * numgrains);
      if(randomgrain == 0) randomgrain = 1;
      if(randomgrain == numgrains) randomgrain = numgrains - 1;
      Seed++;
      oldxc = m->m_Grains[randomgrain]->centroidx;
      oldyc = m->m_Grains[randomgrain]->centroidy;
      oldzc = m->m_Grains[randomgrain]->centroidz;
      xc = oldxc + ((2.0 * (rg.genrand_res53() - 0.5)) * (2.0 * packingresx));
      yc = oldyc + ((2.0 * (rg.genrand_res53() - 0.5)) * (2.0 * packingresy));
      zc = oldzc + ((2.0 * (rg.genrand_res53() - 0.5)) * (2.0 * packingresz));
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000,randomgrain);
      move_grain(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(randomgrain,-1000);
//      currentneighborhooderror = check_neighborhooderror(-1000, random);
//      change2 = (currentneighborhooderror * currentneighborhooderror) - (oldneighborhooderror * oldneighborhooderror);
      if(fillingerror <= oldfillingerror)
      {
//        oldneighborhooderror = currentneighborhooderror;
        acceptedmoves++;
      }
      else if(fillingerror > oldfillingerror)
      {
		fillingerror = check_fillingerror(-1000,randomgrain);
		move_grain(randomgrain, oldxc, oldyc, oldzc);
		fillingerror = check_fillingerror(randomgrain,-1000);
      }
    }
  }
#if ERROR_TXT_OUT
  outFile.close();
#endif

#if ERROR_TXT_OUT1

//  ofstream outFile;
  filename = "test.vtk";
  outFile.open(filename.c_str(), std::ios_base::binary);
  outFile << "# vtk DataFile Version 2.0" << std::endl;
  outFile << "data set from FFT2dx_GB" << std::endl;
  outFile << "ASCII" << std::endl;
  outFile << "DATASET STRUCTURED_POINTS" << std::endl;
  outFile << "DIMENSIONS " << packingxpoints << " " << packingypoints << " " << packingzpoints << std::endl;
  outFile << "ORIGIN 0.0 0.0 0.0" << std::endl;
  outFile << "SPACING " << packingresx << " " << packingresy << " " << packingresz << std::endl;
  outFile << "POINT_DATA " << packingxpoints*packingypoints*packingzpoints << std::endl;
  outFile << std::endl;
  outFile << std::endl;
  outFile << "SCALARS GrainID int  1" << std::endl;
  outFile << "LOOKUP_TABLE default" << std::endl;
  for (int i = 0; i < (packingzpoints); i++)
  {
    for (int j = 0; j < (packingypoints); j++)
    {
      for (int k = 0; k < (packingxpoints); k++)
      {
        int name = grainowners[k][j][i];
        if(i%20 == 0 && i > 0) outFile << std::endl;
        outFile << "     ";
        if(name < 100) outFile << " ";
        if(name < 10) outFile << " ";
        outFile << name;
      }
    }
  }
  outFile.close();
#endif

  assign_voxels();
  assign_gaps();
  cleanup_grains();

  // If there is an error set this to something negative and also set a message
  notify("PackGrainsGen2 Completed", 0, Observable::UpdateProgressMessage);
}

void PackGrainsGen2::initialize_packinggrid()
{
  DataContainer* m = getDataContainer();
  packingresx = m->getXRes() * 2.0;
  packingresy = m->getYRes() * 2.0;
  packingresz = m->getZRes() * 2.0;
  packingxpoints = int(sizex / packingresx);
  packingypoints = int(sizey / packingresy);
  packingzpoints = int(sizez / packingresz);
  packingtotalpoints = packingxpoints * packingypoints * packingzpoints;
  grainowners.resize(packingxpoints);
  for (int i = 0; i < packingxpoints; i++)
  {
    grainowners[i].resize(packingypoints);
    for (int j = 0; j < packingypoints; j++)
    {
      grainowners[i][j].resize(packingzpoints, 0);
    }
  }
}

void PackGrainsGen2::generate_grain(int gnum, int phase, int Seed)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed)
    DataContainer* m = getDataContainer();
//  int good = 0;
  float r1 = 1;
  float a1 = 0,  a3 = 0;
  float b1 = 0,  b3 = 0;
  float r2 = 0, r3 = 0;
  float diam = 0;
  float vol = 0;
  int volgood = 0;
  float phi1, PHI, phi2;
  while (volgood == 0)
  {
    volgood = 1;
    diam = rg.genrand_norm(avgdiam[phase], sddiam[phase]);
    diam = exp(diam);
    if(diam >= maxdiameter[phase]) volgood = 0;
    if(diam < mindiameter[phase]) volgood = 0;
    vol = (4.0 / 3.0) * (m_pi) * ((diam / 2.0) * (diam / 2.0) * (diam / 2.0));
  }
  int diameter = int((diam - mindiameter[phase]) / binstepsize[phase]);
  a1 = bovera[phase][diameter][0];
  b1 = bovera[phase][diameter][1];
  if(a1 == 0)
  {
      a1 = bovera[phase][diameter - 1][0];
      b1 = bovera[phase][diameter - 1][1];
  }
  r2 = rg.genrand_beta(a1, b1);
/*  a2 = m->covera[phase][diameter][0];
  b2 = m->covera[phase][diameter][1];
  if(a2 == 0)
  {
      a2 = m->covera[phase][diameter - 1][0];
      b2 = m->covera[phase][diameter - 1][1];
  }
  r3 = rg.genrand_beta(a2, b2);
  float cob = r3 / r2;
*/  a3 = coverb[phase][diameter][0];
  b3 = coverb[phase][diameter][1];
  if(a3 == 0)
  {
      a3 = coverb[phase][diameter - 1][0];
      b3 = coverb[phase][diameter - 1][1];
  }
  r3 = rg.genrand_beta(a3, b3) * r2;
  float random = rg.genrand_res53();
  int bin = 0;
  while (random > axisodf[phase][bin])
  {
	bin++;
  }
  m_OrientationOps[Ebsd::OrthoRhombic]->determineEulerAngles(bin, phi1, PHI, phi2);
  float mf = omega3[phase][diameter][0];
  float s = omega3[phase][diameter][1];
  float omega3f = rg.genrand_beta(mf, s);
  DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[phase];
  if(shapeclass == DREAM3D::SyntheticBuilder::EllipsoidShape) omega3f = 1;
  m->m_Grains[gnum]->volume = vol;
  m->m_Grains[gnum]->equivdiameter = diam;
  m->m_Grains[gnum]->radius1 = r1;
  m->m_Grains[gnum]->radius2 = r2;
  m->m_Grains[gnum]->radius3 = r3;
  m->m_Grains[gnum]->axiseuler1 = phi1;
  m->m_Grains[gnum]->axiseuler2 = PHI;
  m->m_Grains[gnum]->axiseuler3 = phi2;
  m->m_Grains[gnum]->omega3 = omega3f;
  m->m_Grains[gnum]->phase = phase;
  m->m_Grains[gnum]->neighbordistfunc[0] = 0;
  m->m_Grains[gnum]->neighbordistfunc[1] = 0;
  m->m_Grains[gnum]->neighbordistfunc[2] = 0;
}

void PackGrainsGen2::initializeAttributes()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

  sizex = m->getXPoints() * m->getXRes();
  sizey = m->getYPoints() * m->getYRes();
  sizez = m->getZPoints() * m->getZRes();
  totalvol = sizex*sizey*sizez;


  grain_indicies = m->createVoxelData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, 1, this);
  if (grain_indicies == NULL) { return; }
  phases = m->createVoxelData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, 1, this);
  if (phases == NULL) { return; }
  euler1s = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler1, totalPoints, 1, this);
  if (NULL == euler1s) {return;}
  euler2s = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler2, totalPoints, 1, this);
  if (NULL == euler2s) {return;}
  euler3s = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler3, totalPoints, 1, this);
  if (NULL == euler3s) {return;}
  surfacevoxels = m->createVoxelData<int8_t, Int8ArrayType, AbstractFilter>(DREAM3D::VoxelData::SurfaceVoxels, totalPoints, 1, this);
  if (NULL == surfacevoxels) {return;}

	for(int i=0;i<totalPoints;i++)
	{
		grain_indicies[i] = 0;
		phases[i] = 0;
		euler1s[i] = -1.0f;
		euler2s[i] = -1.0f;
		euler3s[i] = -1.0f;
		surfacevoxels[i] = 0;
	}
}


void PackGrainsGen2::initializeArrays(std::vector<Ebsd::CrystalStructure> structures)
{
  DataContainer* m = getDataContainer();
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
  axisodf.resize(size+1);

  for(size_t i= 1; i < size+1; ++i)
  {
    nElements = 36*36*36;
    float initValue = (1.0/float(nElements));
    axisodf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(axisodf[i], initValue, nElements);
  }
}





int PackGrainsGen2::readReconStatsData(H5StatsReader::Pointer h5io)
{
  DataContainer* m = getDataContainer();
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

int PackGrainsGen2::readAxisOrientationData(H5StatsReader::Pointer h5io)
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

void PackGrainsGen2::move_grain(size_t gnum, float xc, float yc, float zc)
{
  DataContainer* m = getDataContainer();
 // int column, row, plane;
  int occolumn, ocrow, ocplane;
  int nccolumn, ncrow, ncplane;
  int shiftcolumn, shiftrow, shiftplane;
  float oxc = m->m_Grains[gnum]->centroidx;
  float oyc = m->m_Grains[gnum]->centroidy;
  float ozc = m->m_Grains[gnum]->centroidz;
  occolumn = (oxc - (packingresx / 2)) / packingresx;
  ocrow = (oyc - (packingresy / 2)) / packingresy;
  ocplane = (ozc - (packingresz / 2)) / packingresz;
  nccolumn = (xc - (packingresx / 2)) / packingresx;
  ncrow = (yc - (packingresy / 2)) / packingresy;
  ncplane = (zc - (packingresz / 2)) / packingresz;
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m->m_Grains[gnum]->centroidx = xc;
  m->m_Grains[gnum]->centroidy = yc;
  m->m_Grains[gnum]->centroidz = zc;
  size_t size = columnlist[gnum].size();

  for (size_t i = 0; i < size; i++)
  {
    columnlist[gnum][i] = columnlist[gnum][i] + shiftcolumn;
    rowlist[gnum][i] = rowlist[gnum][i] + shiftrow;
    planelist[gnum][i] = planelist[gnum][i] + shiftplane;
  }
}
void PackGrainsGen2::determine_neighbors(size_t gnum, int add)
{
  DataContainer* m = getDataContainer();
  float x, y, z;
  float xn, yn, zn;
  float dia, dia2;
  int DoverR;
  float xdist, ydist, zdist, totdist;
//  int nnum = 0;
//  nnum = 0;
  x = m->m_Grains[gnum]->centroidx;
  y = m->m_Grains[gnum]->centroidy;
  z = m->m_Grains[gnum]->centroidz;
  dia = m->m_Grains[gnum]->equivdiameter;
  for (size_t n = 1; n < m->m_Grains.size(); n++)
  {
    xn = m->m_Grains[n]->centroidx;
    yn = m->m_Grains[n]->centroidy;
    zn = m->m_Grains[n]->centroidz;
    dia2 = m->m_Grains[n]->equivdiameter;
    xdist = fabs(x - xn);
    ydist = fabs(y - yn);
    zdist = fabs(z - zn);
    totdist = (xdist * xdist) + (ydist * ydist) + (zdist * zdist);
    totdist = sqrt(totdist);
    if(totdist < (3 * (dia / 2.0)))
    {
      DoverR = int(totdist / (dia / 2.0));
      for (int iter = DoverR; iter < 3; iter++)
      {
        if(add > 0) m->m_Grains[n]->neighbordistfunc[iter]++;
        if(add < 0) m->m_Grains[n]->neighbordistfunc[iter] = m->m_Grains[n]->neighbordistfunc[iter] - 1;
      }
    }
    if(totdist < (3 * (dia2 / 2.0)))
    {
      DoverR = int(totdist / (dia2 / 2.0));
      for (int iter = DoverR; iter < 3; iter++)
      {
        if(add > 0) m->m_Grains[gnum]->neighbordistfunc[iter]++;
        if(add < 0) m->m_Grains[gnum]->neighbordistfunc[iter] = m->m_Grains[gnum]->neighbordistfunc[iter] - 1;
      }
    }
  }
}

float PackGrainsGen2::check_neighborhooderror(int gadd, int gremove)
{
  DataContainer* m = getDataContainer();
  float neighborerror;
  float bhattdist;
  float dia;
  int nnum;
  int index;
  std::vector<int> count;
  int phase;
  for (size_t iter = 0; iter < neighbordist.size(); ++iter)
  {
    phase = primaryphases[iter];
    count.resize(simneighbordist[iter].size(), 0);
    for (size_t i = 0; i < simneighbordist[iter].size(); i++)
    {
      simneighbordist[iter][i][0] = 0;
      simneighbordist[iter][i][1] = 0;
      simneighbordist[iter][i][2] = 0;
    }
    if(gadd > 0 && m->m_Grains[gadd]->phase == phase)
    {
      determine_neighbors(gadd, 1);
    }
    if(gremove > 0 && m->m_Grains[gremove]->phase == phase)
    {
      determine_neighbors(gremove, -1);
    }
    for (size_t i = 1; i < m->m_Grains.size(); i++)
    {
      nnum = 0;
      index = i;
      if(index != gremove && m->m_Grains[index]->phase == phase)
      {
        dia = m->m_Grains[index]->equivdiameter;
        if(dia > maxdiameter[phase]) dia = maxdiameter[phase];
        if(dia < mindiameter[phase]) dia = mindiameter[phase];
        dia = int((dia - mindiameter[phase]) / binstepsize[phase]);
        for (int j = 0; j < 3; j++)
        {
          nnum = m->m_Grains[index]->neighbordistfunc[j];
          if(nnum > 0)
          {
            simneighbordist[iter][dia][j] = simneighbordist[iter][dia][j] + nnum;
          }
        }
        count[dia]++;
      }
    }
    if(gadd > 0 && m->m_Grains[gadd]->phase == phase)
    {
      dia = m->m_Grains[index]->equivdiameter;
      if(dia > maxdiameter[phase]) dia = maxdiameter[phase];
      if(dia < mindiameter[phase]) dia = mindiameter[phase];
      dia = int((dia - mindiameter[phase]) / binstepsize[phase]);
      for (int j = 0; j < 3; j++)
      {
        nnum = m->m_Grains[index]->neighbordistfunc[j];
        if(nnum > 0)
        {
          simneighbordist[iter][dia][j] = simneighbordist[iter][dia][j] + nnum;
        }
      }
      count[dia]++;
    }
    for (size_t i = 0; i < simneighbordist[iter].size(); i++)
    {
      for (size_t j = 0; j < 3; j++)
      {
        simneighbordist[iter][i][j] = simneighbordist[iter][i][j] / double(count[i]);
        if(count[i] == 0) simneighbordist[iter][i][j] = 0.0;
      }
    }
    if(gadd > 0 && m->m_Grains[gadd]->phase == phase)
    {
      determine_neighbors(gadd, -1);
    }
    if(gremove > 0 && m->m_Grains[gremove]->phase == phase)
    {
      determine_neighbors(gremove, 1);
    }
  }
  compare_3Ddistributions(simneighbordist, neighbordist, bhattdist);
  neighborerror = bhattdist;
  return neighborerror;
}

void PackGrainsGen2::compare_1Ddistributions(std::vector<float> array1, std::vector<float> array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    bhattdist = bhattdist + sqrt((array1[i]*array2[i]));
  }
}
void PackGrainsGen2::compare_2Ddistributions(std::vector<std::vector<float> > array1, std::vector<std::vector<float> > array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      bhattdist = bhattdist + sqrt((array1[i][j] * array2[i][j]));
    }
  }
}

void PackGrainsGen2::compare_3Ddistributions(std::vector<std::vector<std::vector<float> > > array1, std::vector<std::vector<std::vector<float> > > array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      for (size_t k = 0; k < array1[i][j].size(); k++)
      {
        bhattdist = bhattdist + sqrt((array1[i][j][k]*array2[i][j][k]));
      }
    }
  }
}

float PackGrainsGen2::check_sizedisterror(int gadd, int gremove)
{
  DataContainer* m = getDataContainer();
  float dia;
  float sizedisterror = 0;
  float bhattdist;
  int index;
  int count = 0;
  int phase;
  for (size_t iter = 0; iter < grainsizedist.size(); ++iter)
  {
    phase = primaryphases[iter];
    count = 0;
    for (size_t i = 0; i < grainsizedist[iter].size(); i++)
    {
      simgrainsizedist[iter][i] = 0.0;
    }
    for (size_t b = 1; b < m->m_Grains.size(); b++)
    {
      index = b;
      if(index != gremove && m->m_Grains[index]->phase == phase)
      {
        dia = m->m_Grains[index]->equivdiameter;
        dia = (dia - (mindiameter[phase] / 2.0)) / grainsizediststep[iter];
        if(dia < 0) dia = 0;
        if(dia > grainsizedist[iter].size() - 1) dia = grainsizedist[iter].size() - 1;
        simgrainsizedist[iter][int(dia)]++;
        count++;
      }
    }
    if(gadd > 0 && m->m_Grains[gadd]->phase == phase)
    {
      dia = m->m_Grains[gadd]->equivdiameter;
      dia = (dia - (mindiameter[phase] / 2.0)) / grainsizediststep[iter];
      if(dia < 0) dia = 0;
      if(dia > grainsizedist[iter].size() - 1) dia = grainsizedist[iter].size() - 1;
      simgrainsizedist[iter][int(dia)]++;
      count++;
    }
    for (size_t i = 0; i < grainsizedist[iter].size(); i++)
    {
      simgrainsizedist[iter][i] = simgrainsizedist[iter][i] / float(count);
      if(count == 0) simgrainsizedist[iter][i] = 0.0;
    }
  }
  compare_2Ddistributions(simgrainsizedist, grainsizedist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

float PackGrainsGen2::check_fillingerror(int gadd, int gremove)
{
  DataContainer* m = getDataContainer();
  fillingerror = fillingerror * float(packingtotalpoints);
  int col, row, plane;
  if(gadd > 0)
  {
    size_t size = columnlist[gadd].size();

	float packquality = 0;
    for (size_t i = 0; i < size; i++)
    {
      col = columnlist[gadd][i];
      row = rowlist[gadd][i];
      plane = planelist[gadd][i];
      if(m_periodic_boundaries == true)
      {
        if(col < 0) col = col + packingxpoints;
        if(col > packingxpoints - 1) col = col - packingxpoints;
        if(row < 0) row = row + packingypoints;
        if(row > packingypoints - 1) row = row - packingypoints;
        if(plane < 0) plane = plane + packingzpoints;
        if(plane > packingzpoints - 1) plane = plane - packingzpoints;
        fillingerror = fillingerror + (2 * grainowners[col][row][plane] - 1);
		grainowners[col][row][plane]++;
		packquality = packquality + ((grainowners[col][row][plane]-1)*(grainowners[col][row][plane]-1));
      }
      if(m_periodic_boundaries == false)
      {
        if(col >= 0 && col <= packingxpoints - 1 && row >= 0 && row <= packingypoints - 1 && plane >= 0 && plane <= packingzpoints - 1)
        {
          fillingerror = fillingerror + (2 * grainowners[col][row][plane] - 1);
		  grainowners[col][row][plane]++;
		  packquality = packquality + ((grainowners[col][row][plane]-1)*(grainowners[col][row][plane]-1));
        }
      }
    }
	packqualities[gadd] = packquality/float(size);
  }
  if(gremove > 0)
  {
    size_t size = columnlist[gremove].size();
    for (size_t i = 0; i < size; i++)
    {
      col = columnlist[gremove][i];
      row = rowlist[gremove][i];
      plane = planelist[gremove][i];
      if(m_periodic_boundaries == true)
      {
        if(col < 0) col = col + packingxpoints;
        if(col > packingxpoints - 1) col = col - packingxpoints;
        if(row < 0) row = row + packingypoints;
        if(row > packingypoints - 1) row = row - packingypoints;
        if(plane < 0) plane = plane + packingzpoints;
        if(plane > packingzpoints - 1) plane = plane - packingzpoints;
        fillingerror = fillingerror + (-2 * grainowners[col][row][plane] + 3);
		grainowners[col][row][plane] = grainowners[col][row][plane] - 1;
      }
      if(m_periodic_boundaries == false)
      {
        if(col >= 0 && col <= packingxpoints - 1 && row >= 0 && row <= packingypoints - 1 && plane >= 0 && plane <= packingzpoints - 1)
        {
          fillingerror = fillingerror + (-2 * grainowners[col][row][plane] + 3);
		  grainowners[col][row][plane] = grainowners[col][row][plane] - 1;
        }
      }
    }
  }
  fillingerror = fillingerror / float(packingtotalpoints);
  return fillingerror;
}

void PackGrainsGen2::insert_grain(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW()
    DataContainer* m = getDataContainer();
//  float dist;
  float inside = -1;
  int column, row, plane;
  int centercolumn, centerrow, centerplane;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float xp, yp, zp;
  float x, y, z;
  float volcur = m->m_Grains[gnum]->volume;
  float bovera = m->m_Grains[gnum]->radius2;
  float covera = m->m_Grains[gnum]->radius3;
  float omega3 = m->m_Grains[gnum]->omega3;
  float radcur1 = 1;
  DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[m->m_Grains[gnum]->phase];

  // init any values for each of the Shape Ops
  for (std::map<DREAM3D::SyntheticBuilder::ShapeType, DREAM3D::ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops)
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
  float phi1 = m->m_Grains[gnum]->axiseuler1;
  float PHI = m->m_Grains[gnum]->axiseuler2;
  float phi2 = m->m_Grains[gnum]->axiseuler3;
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
  xc = m->m_Grains[gnum]->centroidx;
  yc = m->m_Grains[gnum]->centroidy;
  zc = m->m_Grains[gnum]->centroidz;
  centercolumn = (xc - (packingresx / 2)) / packingresx;
  centerrow = (yc - (packingresy / 2)) / packingresy;
  centerplane = (zc - (packingresz / 2)) / packingresz;
  xmin = int(centercolumn - ((radcur1 / packingresx) + 1));
  xmax = int(centercolumn + ((radcur1 / packingresx) + 1));
  ymin = int(centerrow - ((radcur1 / packingresy) + 1));
  ymax = int(centerrow + ((radcur1 / packingresy) + 1));
  zmin = int(centerplane - ((radcur1 / packingresz) + 1));
  zmax = int(centerplane + ((radcur1 / packingresz) + 1));
  if(xmin < -packingxpoints) xmin = -packingxpoints;
  if(xmax > 2 * packingxpoints - 1) xmax = (2 * packingxpoints - 1);
  if(ymin < -packingypoints) ymin = -packingypoints;
  if(ymax > 2 * packingypoints - 1) ymax = (2 * packingypoints - 1);
  if(zmin < -packingzpoints) zmin = -packingzpoints;
  if(zmax > 2 * packingzpoints - 1) zmax = (2 * packingzpoints - 1);
  for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        x = float(column) * packingresx;
        y = float(row) * packingresy;
        z = float(plane) * packingresz;
        inside = -1;
		x = x - xc;
		y = y - yc;
		z = z - zc;
		xp = (x*ga[0][0])+(y*ga[0][1])+(z*ga[0][2]);
		yp = (x*ga[1][0])+(y*ga[1][1])+(z*ga[1][2]);
		zp = (x*ga[2][0])+(y*ga[2][1])+(z*ga[2][2]);
		float axis1comp = xp / radcur1;
		float axis2comp = yp / radcur2;
		float axis3comp = zp / radcur3;
		inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
		if(inside >= 0)
		{
			columnlist[gnum].push_back(column);
			rowlist[gnum].push_back(row);
			planelist[gnum].push_back(plane);
		}
      }
    }
  }
}

void PackGrainsGen2::assign_voxels()
{
  DataContainer* m = getDataContainer();
  int index;
  int neighpoints[6];
  neighpoints[0] = -(m->getXPoints() * m->getYPoints());
  neighpoints[1] = -m->getXPoints();
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = m->getXPoints();
  neighpoints[5] = (m->getXPoints() * m->getYPoints());
  int oldname;
  size_t column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;
  size_t xmin, xmax, ymin, ymax, zmin, zmax;
  int64_t totpoints = m->totalPoints();
  size_t dims[3] = {0,0,0};
  m->getDimensions(dims);


  gsizes.resize(m->m_Grains.size());

  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    gsizes[i] = 0;
  }
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    float volcur = m->m_Grains[i]->volume;
    float bovera = m->m_Grains[i]->radius2;
    float covera = m->m_Grains[i]->radius3;
    float omega3 = m->m_Grains[i]->omega3;
    xc = m->m_Grains[i]->centroidx;
    yc = m->m_Grains[i]->centroidy;
    zc = m->m_Grains[i]->centroidz;
    float radcur1 = 0.0f;
    //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
    DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[m->m_Grains[i]->phase];

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
    float phi1 = m->m_Grains[i]->axiseuler1;
    float PHI = m->m_Grains[i]->axiseuler2;
    float phi2 = m->m_Grains[i]->axiseuler3;
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
    column = (xc - (m->getXRes() / 2)) / m->getXRes();
    row = (yc - (m->getYRes() / 2)) / m->getYRes();
    plane = (zc - (m->getZRes() / 2)) / m->getZRes();
    xmin = int(column - ((radcur1 / m->getXRes()) + 1));
    xmax = int(column + ((radcur1 / m->getXRes()) + 1));
    ymin = int(row - ((radcur1 / m->getYRes()) + 1));
    ymax = int(row + ((radcur1 / m->getYRes()) + 1));
    zmin = int(plane - ((radcur1 / m->getZRes()) + 1));
    zmax = int(plane + ((radcur1 / m->getZRes()) + 1));
    if (m_periodic_boundaries == true)
    {
      if (xmin < -dims[0]) xmin = -dims[0];
      if (xmax > 2 * dims[0] - 1) xmax = (2 * dims[0] - 1);
      if (ymin < -dims[1]) ymin = -dims[1];
      if (ymax > 2 * dims[1] - 1) ymax = (2 * dims[1] - 1);
      if (zmin < -dims[2]) zmin = -dims[2];
      if (zmax > 2 * dims[2] - 1) zmax = (2 * dims[2] - 1);
    }
    if (m_periodic_boundaries == false)
    {
      if (xmin < 0) xmin = 0;
      if (xmax > dims[0] - 1) xmax = dims[0] - 1;
      if (ymin < 0) ymin = 0;
      if (ymax > dims[1] - 1) ymax = dims[1] - 1;
      if (zmin < 0) zmin = 0;
      if (zmax > dims[2] - 1) zmax = dims[2] - 1;
    }
    for (size_t iter1 = xmin; iter1 < xmax + 1; iter1++)
    {
      for (size_t iter2 = ymin; iter2 < ymax + 1; iter2++)
      {
        for (size_t iter3 = zmin; iter3 < zmax + 1; iter3++)
        {
          column = iter1;
          row = iter2;
          plane = iter3;
          if (iter1 < 0) column = iter1 + dims[0];
          if (iter1 > dims[0] - 1) column = iter1 - dims[0];
          if (iter2 < 0) row = iter2 + dims[1];
          if (iter2 > dims[1] - 1) row = iter2 - dims[1];
          if (iter3 < 0) plane = iter3 + dims[2];
          if (iter3 > dims[2] - 1) plane = iter3 - dims[2];
          index = (plane * dims[0] * dims[1]) + (row * dims[0]) + column;
          inside = -1;
          x = float(column) * m->getXRes();
          y = float(row) * m->getYRes();
          z = float(plane) * m->getZRes();
          if (iter1 < 0) x = x - sizex;
          if (iter1 > dims[0] - 1) x = x + sizex;
          if (iter2 < 0) y = y - sizey;
          if (iter2 > dims[1] - 1) y = y + sizey;
          if (iter3 < 0) z = z - sizez;
          if (iter3 > dims[2] - 1) z = z + sizez;
          dist = ((x - xc) * (x - xc)) + ((y - yc) * (y - yc)) + ((z - zc) * (z - zc));
          dist = sqrtf(dist);
          if (dist < radcur1)
          {
            x = x - xc;
            y = y - yc;
            z = z - zc;
            xp = (x * ga[0][0]) + (y * ga[0][1]) + (z * ga[0][2]);
            yp = (x * ga[1][0]) + (y * ga[1][1]) + (z * ga[1][2]);
            zp = (x * ga[2][0]) + (y * ga[2][1]) + (z * ga[2][2]);
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
  newnames.resize(m->m_Grains.size());
  int goodcount = 1;
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    newnames[i] = 0;
    if (gsizes[i] > 0)
    {
      m->m_Grains[goodcount] = m->m_Grains[i];
      newnames[i] = goodcount;
      goodcount++;
    }
  }
  for (int i = 0; i < totpoints; i++)
  {
    if (grain_indicies[i] > 0)
    {
	  grain_indicies[i] = newnames[grain_indicies[i]];
    }
  }
  m->m_Grains.resize(goodcount);
}

void PackGrainsGen2::assign_gaps()
{
  DataContainer* m = getDataContainer();
  int totpoints = m->totalPoints();
  int index;
  int timestep = 100;
  int unassignedcount = 1;
  int column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;
  size_t xmin, xmax, ymin, ymax, zmin, zmax;

  int *newowners;
  newowners = new int [totpoints];
  float *ellipfuncs;
  ellipfuncs = new float [totpoints];
  for(int i = 0; i < totpoints; i++)
  {
	  newowners[i] = -1;
	  ellipfuncs[i] = -1.0;
  }

  while (unassignedcount != 0)
  {
	  unassignedcount = 0;
	  timestep = timestep + 50;
	  for (size_t i = 1; i < m->m_Grains.size(); i++)
	  {
		float volcur = m->m_Grains[i]->volume;
		float bovera = m->m_Grains[i]->radius2;
		float covera = m->m_Grains[i]->radius3;
		float omega3 = m->m_Grains[i]->omega3;
		xc = m->m_Grains[i]->centroidx;
		yc = m->m_Grains[i]->centroidy;
		zc = m->m_Grains[i]->centroidz;
		float radcur1 = 0.0f;
		//Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
		DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[m->m_Grains[i]->phase];

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
		radcur1 = (float(timestep)/100.0)*radcur1;
		radcur2 = (float(timestep)/100.0)*radcur2;
		radcur3 = (float(timestep)/100.0)*radcur3;
		float phi1 = m->m_Grains[i]->axiseuler1;
		float PHI = m->m_Grains[i]->axiseuler2;
		float phi2 = m->m_Grains[i]->axiseuler3;
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
		column = (xc - (m->getXRes() / 2)) / m->getXRes();
		row = (yc - (m->getYRes() / 2)) / m->getYRes();
		plane = (zc - (m->getZRes() / 2)) / m->getZRes();
		xmin = int(column - ((radcur1 / m->getXRes()) + 1));
		xmax = int(column + ((radcur1 / m->getXRes()) + 1));
		ymin = int(row - ((radcur1 / m->getYRes()) + 1));
		ymax = int(row + ((radcur1 / m->getYRes()) + 1));
		zmin = int(plane - ((radcur1 / m->getZRes()) + 1));
		zmax = int(plane + ((radcur1 / m->getZRes()) + 1));
		if (m_periodic_boundaries == true)
		{
		  if (xmin < -m->getXPoints()) xmin = -m->getXPoints();
		  if (xmax > 2 * m->getXPoints() - 1) xmax = (2 *m->getXPoints() - 1);
		  if (ymin < -m->getYPoints()) ymin = -m->getYPoints();
		  if (ymax > 2 * m->getYPoints() - 1) ymax = (2 * m->getYPoints() - 1);
		  if (zmin < -m->getZPoints()) zmin = -m->getZPoints();
		  if (zmax > 2 * m->getZPoints() - 1) zmax = (2 * m->getZPoints() - 1);
		}
		if (m_periodic_boundaries == false)
		{
		  if (xmin < 0) xmin = 0;
		  if (xmax > m->getXPoints() - 1) xmax = m->getXPoints() - 1;
		  if (ymin < 0) ymin = 0;
		  if (ymax > m->getYPoints() - 1) ymax = m->getYPoints() - 1;
		  if (zmin < 0) zmin = 0;
		  if (zmax > m->getZPoints() - 1) zmax = m->getZPoints() - 1;
		}
		for (size_t iter1 = xmin; iter1 < xmax + 1; iter1++)
		{
		  for (size_t iter2 = ymin; iter2 < ymax + 1; iter2++)
		  {
			for (size_t iter3 = zmin; iter3 < zmax + 1; iter3++)
			{
			  column = iter1;
			  row = iter2;
			  plane = iter3;
			  if (iter1 < 0) column = iter1 + m->getXPoints();
			  if (iter1 > m->getXPoints() - 1) column = iter1 - m->getXPoints();
			  if (iter2 < 0) row = iter2 + m->getYPoints();
			  if (iter2 > m->getYPoints() - 1) row = iter2 - m->getYPoints();
			  if (iter3 < 0) plane = iter3 + m->getZPoints();
			  if (iter3 > m->getZPoints() - 1) plane = iter3 - m->getZPoints();
			  index = (plane * m->getXPoints() * m->getYPoints()) + (row * m->getXPoints()) + column;
			  if(grain_indicies[index] <= 0)
			  {
				  inside = -1;
				  x = float(column) * m->getXRes();
				  y = float(row) * m->getYRes();
				  z = float(plane) * m->getZRes();
				  if (iter1 < 0) x = x - sizex;
				  if (iter1 > m->getXPoints() - 1) x = x + sizex;
				  if (iter2 < 0) y = y - sizey;
				  if (iter2 > m->getYPoints() - 1) y = y + sizey;
				  if (iter3 < 0) z = z - sizez;
				  if (iter3 > m->getZPoints() - 1) z = z + sizez;
				  dist = ((x - xc) * (x - xc)) + ((y - yc) * (y - yc)) + ((z - zc) * (z - zc));
				  dist = sqrtf(dist);
				  if (dist < radcur1)
				  {
					x = x - xc;
					y = y - yc;
					z = z - zc;
					xp = (x * ga[0][0]) + (y * ga[0][1]) + (z * ga[0][2]);
				    yp = (x * ga[1][0]) + (y * ga[1][1]) + (z * ga[1][2]);
			        zp = (x * ga[2][0]) + (y * ga[2][1]) + (z * ga[2][2]);
					float axis1comp = xp / radcur1;
					float axis2comp = yp / radcur2;
					float axis3comp = zp / radcur3;
					inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
					if (inside >= 0 && inside > ellipfuncs[index])
					{
						newowners[index] = i;
						ellipfuncs[index] = inside;
					}
				  }
			  }
			}
		  }
		}
	  }
	  for (int i = 0; i < totpoints; i++)
	  {
	    if (ellipfuncs[i] >= 0) grain_indicies[i] = newowners[i];
		if (grain_indicies[i] <= 0) unassignedcount++;
		newowners[i] = -1;
		ellipfuncs[i] = -1.0;
	  }
  }
  for (int i = 0; i < totpoints; i++)
  {
	  if(grain_indicies[i] > 0) phases[i] = m->m_Grains[grain_indicies[i]]->phase;
  }
  delete [] ellipfuncs;
  delete [] newowners;
}
void PackGrainsGen2::cleanup_grains()
{
  DataContainer* m = getDataContainer();
  int totpoints = m->totalPoints();
  int neighpoints[6];
  int xp = m->getXPoints();
  int yp = m->getYPoints();
  int zp = m->getZPoints();
  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  std::vector<std::vector<int> > vlists;
  vlists.resize(m->m_Grains.size());
  std::vector<int> currentvlist;
  std::vector<bool> checked;
  checked.resize(totpoints,false);
  size_t count;
  int touchessurface = 0;
  int good;
  int neighbor;
  int column, row, plane;
  int index;
  float minsize = 0;
  gsizes.resize(m->m_Grains.size());
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
	gsizes[i] = 0;
  }


  for (int i = 0; i < totpoints; i++)
  {
    touchessurface = 0;
	if(checked[i] == false && grain_indicies[i] > 0)
	{
		minsize = mindiameter[phases[i]]*mindiameter[phases[i]]*mindiameter[phases[i]]*M_PI/6.0;
		minsize = int(minsize/(m->getXRes()*m->getYRes()*m->getZRes()));
		currentvlist.push_back(i);
		count = 0;
		while(count < currentvlist.size())
		{
			index = currentvlist[count];
			column = index % xp;
			row = (index / xp) % yp;
			plane = index / (xp * yp);
			if(column == 0 || column == xp || row == 0 || row == yp || plane == 0 || plane == zp) touchessurface = 1;
			for (int j = 0; j < 6; j++)
			{
				good = 1;
				neighbor = index + neighpoints[j];
				if (m_periodic_boundaries == false)
				{
					if (j == 0 && plane == 0) good = 0;
					if (j == 5 && plane == (zp - 1)) good = 0;
					if (j == 1 && row == 0) good = 0;
					if (j == 4 && row == (yp - 1)) good = 0;
					if (j == 2 && column == 0) good = 0;
					if (j == 3 && column == (xp - 1)) good = 0;
					if (good == 1 && grain_indicies[neighbor] == grain_indicies[index] && checked[neighbor] == false)
					{
						currentvlist.push_back(neighbor);
						checked[neighbor] = true;
					}
				}
				else if (m_periodic_boundaries == true)
				{
					if (j == 0 && plane == 0) neighbor = neighbor + (xp*yp*zp);
					if (j == 5 && plane == (zp - 1)) neighbor = neighbor - (xp*yp*zp);
					if (j == 1 && row == 0) neighbor = neighbor + (xp*yp);
					if (j == 4 && row == (yp - 1)) neighbor = neighbor - (xp*yp);
					if (j == 2 && column == 0) neighbor = neighbor + (xp);
					if (j == 3 && column == (xp - 1)) neighbor = neighbor - (xp);
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
			if(currentvlist.size() >= minsize || touchessurface == 1)
			{
				vlists[grain_indicies[i]].resize(currentvlist.size());
				vlists[grain_indicies[i]].swap(currentvlist);
			}
			if(currentvlist.size() < minsize && touchessurface == 0)
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
  for (int i = 0; i < totpoints; i++)
  {
	if(grain_indicies[i] > 0) gsizes[grain_indicies[i]]++;
  }
  newnames.resize(m->m_Grains.size());
  int goodcount = 1;
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
     newnames[i] = 0;
     if(gsizes[i] > 0)
     {
        m->m_Grains[goodcount] = m->m_Grains[i];
        newnames[i] = goodcount;
        goodcount++;
     }
  }
  for (int i = 0; i < totpoints; i++)
  {
	if (grain_indicies[i] > 0)
	{
	  grain_indicies[i] = newnames[grain_indicies[i]];
	}
  }
  for (int i = 0; i < totpoints; i++)
  {

	  if(grain_indicies[i] > 0) { phases[i] = m->m_Grains[grain_indicies[i]]->phase; }
  }
  m->m_Grains.resize(goodcount);
  assign_gaps();
}
