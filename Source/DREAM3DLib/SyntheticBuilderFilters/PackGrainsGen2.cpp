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
#include "DREAM3DLib/Common/DataContainerMacros.h"
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



const static float m_pi = static_cast<float>(M_PI);


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
PackGrainsGen2::PackGrainsGen2() :
AbstractFilter(),
m_H5StatsInputFile(""),
m_ErrorOutputFile(""),
m_VtkOutputFile(""),
m_MaxIterations(1),
m_PeriodicBoundaries(false),
m_NeighborhoodErrorWeight(1.0f),
m_GrainIds(NULL),
m_PhasesC(NULL),
m_EulerAngles(NULL),
m_SurfaceVoxels(NULL),
m_Active(NULL),
m_PhasesF(NULL),
m_Neighborhoods(NULL),
m_Centroids(NULL),
m_Volumes(NULL),
m_AxisLengths(NULL),
m_AxisEulerAngles(NULL),
m_Omega3s(NULL),
m_EquivalentDiameters(NULL)
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
  setupFilterOptions();
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
void PackGrainsGen2::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Input Statistics File");
    option->setPropertyName("H5StatsInputFile");
    option->setWidgetType(FilterOption::InputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Max Iterations");
    option->setPropertyName("MaxIterations");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Neighborhood Error Weight");
    option->setPropertyName("NeighborhoodErrorWeight");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Periodic Boundary");
    option->setPropertyName("PeriodicBoundaries");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }

  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackGrainsGen2::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  int err = 0;
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  TEST_MACRO(DREAM3D, VoxelData, GrainIds);



  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, VoxelData, GrainIds, ss, Int32ArrayType, voxels, 1);
  PF_MAKE_SURE_ARRAY_EXISTS_SUFFIX(m, DREAM3D, VoxelData, Phases, C, ss, Int32ArrayType, voxels, 1);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, VoxelData, EulerAngles, ss, FloatArrayType, voxels, 3);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, VoxelData, SurfaceVoxels, ss, Int8ArrayType, voxels, 1);

  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, FieldData, Active, ss, BoolArrayType, fields, 1);
  PF_MAKE_SURE_ARRAY_EXISTS_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, Int32ArrayType, fields, 1);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, FieldData, Neighborhoods, ss, Int32ArrayType, fields, 3);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, FieldData, Centroids, ss, FloatArrayType, fields, 3);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, FieldData, Volumes, ss, FloatArrayType, fields, 1);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, FieldData, AxisLengths, ss, FloatArrayType, fields, 3);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, FieldData, AxisEulerAngles, ss, FloatArrayType, fields, 3);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, FieldData, Omega3s, ss, FloatArrayType, fields, 1);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, FieldData, EquivalentDiameters, ss, FloatArrayType, fields, 1);


  setErrorCondition(err);
  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackGrainsGen2::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackGrainsGen2::execute()
{
  DataContainer* m = getDataContainer();
  bool writeErrorFile = true;
  std::ofstream outFile;
  if(m_ErrorOutputFile.empty() == false)
  {
    outFile.open(m_ErrorOutputFile.c_str(), std::ios_base::binary);
    writeErrorFile = true;
  }

  int err = 0;
  setErrorCondition(err);
  unsigned long long int Seed = MXA::getMilliSeconds();
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed)

  H5StatsReader::Pointer h5reader = H5StatsReader::New(m_H5StatsInputFile);
  // Open the HDF5 Stats file
  if(h5reader.get() == NULL)
  {
    setErrorCondition(-1);
    setErrorMessage("Error Loading H5 Stats File");
    return;
  }

  err = readReconStatsData(h5reader);
  if(err < 0)
  {
    setErrorCondition(-1);
    setErrorMessage("Error Reading the Stats Data from the HDF5 File");
    notify(getErrorMessage().c_str(), 0, Observable::UpdateErrorMessage);
    return;
  }

  err = readAxisOrientationData(h5reader);
  if(err < 0)
  {
    setErrorCondition(-1);
    setErrorMessage("Error Reading the Axis Orientation Data from the HDF5 File");
    notify(getErrorMessage().c_str(), 0, Observable::UpdateErrorMessage);
    return;
  }

  int64_t totalPoints = m->totalPoints();
  int totalFields = m->getTotalFields();
  dataCheck(false, totalPoints, totalFields, m->crystruct.size());
  initializeAttributes();

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]),
      static_cast<DimType>(udims[1]),
      static_cast<DimType>(udims[2]), };

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();


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
  Field field;
  int gid = 0;
  float currentvol = 0.0;
  m->resizeFieldDataArrays(1);
//FIXME: Initialize the Grain with some sort of default data
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

    generate_grain(phase, Seed, &field);
    currentsizedisterror = check_sizedisterror(gid, -1000);
    change = (currentsizedisterror) - (oldsizedisterror);
    if(change > 0 || currentsizedisterror > (1.0 - (iter * 0.001)))
    {
	   gid++;
       m->resizeFieldDataArrays(gid + 1);
       m_Active[gid] = 1;
	   transfer_attributes(gid, &field);
       oldsizedisterror = currentsizedisterror;
       currentvol = currentvol + m_Volumes[gid];
       //FIXME: Initialize the Grain with some sort of default data
       iter = 0;
    }
  }
  if(m_PeriodicBoundaries == false)
  {
    iter = 0;
    int xgrains, ygrains, zgrains;

    xgrains = int(powf((m->getTotalFields()*(sizex/sizey)*(sizex/sizez)),(1.0f/3.0f))+1);
    ygrains = int(xgrains*(sizey/sizex)+1);
    zgrains = int(xgrains*(sizez/sizex)+1);
    factor = 0.25f * (1.0f - (float((xgrains-2)*(ygrains-2)*(zgrains-2))/float(xgrains*ygrains*zgrains)));
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

      generate_grain(phase, Seed, &field);
      currentsizedisterror = check_sizedisterror(gid, -1000);
      change = (currentsizedisterror) - (oldsizedisterror);
      if(change > 0 || currentsizedisterror > (1.0 - (iter * 0.001f)))
      {
	    gid++;
        m->resizeFieldDataArrays(gid + 1);
        m_Active[gid] = 1;
	    transfer_attributes(gid, &field);
        oldsizedisterror = currentsizedisterror;
        currentvol = currentvol + m_Volumes[gid];
        gid++;
        m->resizeFieldDataArrays(gid + 1);
        //FIXME: Initialize the new grain with default data
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
      neighbordist[i][j][0] = neighborparams[phase][j][0] * powf(0.5f, neighborparams[phase][j][2]) + neighborparams[phase][j][1];
      neighbordist[i][j][1] = neighborparams[phase][j][0] * powf(1.5f, neighborparams[phase][j][2]) + neighborparams[phase][j][1];
      neighbordist[i][j][2] = neighborparams[phase][j][0] * powf(2.5f, neighborparams[phase][j][2]) + neighborparams[phase][j][1];
    }
  }
  //  for each grain : select centroid, determine voxels in grain, monitor filling error and decide of the 10 placements which
  // is the most beneficial, then the grain is added and its neighbors are determined

  int numgrains = m->getTotalFields();

  columnlist.resize(numgrains);
  rowlist.resize(numgrains);
  planelist.resize(numgrains);
  packqualities.resize(numgrains);
  fillingerror = 1;
  for (int i = 1; i < numgrains; i++)
  {
    xc = sizex / 2.0f;
    yc = sizey / 2.0f;
    zc = sizez / 2.0f;
    m_Centroids[3*i] = xc;
    m_Centroids[3*i+1] = yc;
    m_Centroids[3*i+2] = zc;
    insert_grain(i);
    fillingerror = check_fillingerror(i, -1000);
    for (int iter = 0; iter < 10; iter++)
    {
      xc = rg.genrand_res53() * (dims[0] * m->getXRes());
      yc = rg.genrand_res53() * (dims[1] * m->getYRes());
      zc = rg.genrand_res53() * (dims[2] * m->getZRes());
      oldxc = m_Centroids[3*i];
      oldyc = m_Centroids[3*i+1];
      oldzc = m_Centroids[3*i+2];
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, i);
      move_grain(i, xc, yc, zc);
      fillingerror = check_fillingerror(i, -1000);
      if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, i);
        move_grain(i, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(i, -1000);
      }
    }
  }

  // determine initial filling and neighbor distribution errors
  oldneighborhooderror = check_neighborhooderror(-1000, -1000);
  // begin swaping/moving/adding/removing grains to try to improve packing
  int totalAdjustments = static_cast<int>(100 * numgrains);
  for (int iteration = 0; iteration < totalAdjustments; ++iteration)
  {
//    change1 = 0;
//    change2 = 0;
    int option = iteration % 2;

    if(writeErrorFile == true && iteration % 25 == 0)
    {
      outFile << iteration << " " << fillingerror << "  " << oldsizedisterror << "  " << oldneighborhooderror << "  " << numgrains << " " << acceptedmoves
          << std::endl;
    }

    // JUMP - this option moves one grain to a random spot in the volume
    if(option == 0)
    {
      randomgrain = int(rg.genrand_res53() * numgrains);
      if(randomgrain == 0) randomgrain = 1;
      if(randomgrain == numgrains) randomgrain = numgrains - 1;
      Seed++;

      xc = rg.genrand_res53() * (dims[0] * m->getXRes());
      yc = rg.genrand_res53() * (dims[1] * m->getYRes());
      zc = rg.genrand_res53() * (dims[2] * m->getZRes());
      oldxc = m_Centroids[3*randomgrain];
      oldyc = m_Centroids[3*randomgrain+1];
      oldzc = m_Centroids[3*randomgrain+2];
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, randomgrain);
      move_grain(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(randomgrain, -1000);
//      currentneighborhooderror = check_neighborhooderror(-1000, random);
//      change2 = (currentneighborhooderror * currentneighborhooderror) - (oldneighborhooderror * oldneighborhooderror);
      if(fillingerror <= oldfillingerror)
      {
//        oldneighborhooderror = currentneighborhooderror;
        acceptedmoves++;
      }
      else if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, randomgrain);
        move_grain(randomgrain, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(randomgrain, -1000);
      }
    }
    // NUDGE - this option moves one grain to a spot close to its current centroid
    if(option == 1)
    {
      randomgrain = int(rg.genrand_res53() * numgrains);
      if(randomgrain == 0) randomgrain = 1;
      if(randomgrain == numgrains) randomgrain = numgrains - 1;
      Seed++;
      oldxc = m_Centroids[3*randomgrain];
      oldyc = m_Centroids[3*randomgrain+1];
      oldzc = m_Centroids[3*randomgrain+2];
      xc = oldxc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * packingresx));
      yc = oldyc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * packingresy));
      zc = oldzc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * packingresz));
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, randomgrain);
      move_grain(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(randomgrain, -1000);
//      currentneighborhooderror = check_neighborhooderror(-1000, random);
//      change2 = (currentneighborhooderror * currentneighborhooderror) - (oldneighborhooderror * oldneighborhooderror);
      if(fillingerror <= oldfillingerror)
      {
//        oldneighborhooderror = currentneighborhooderror;
        acceptedmoves++;
      }
      else if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, randomgrain);
        move_grain(randomgrain, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(randomgrain, -1000);
      }
    }
  }

  if(m_VtkOutputFile.empty() == false)
  {
    err = writeVtkFile();
    if(err < 0)
    {
      return;
    }
  }

  assign_voxels();
  assign_gaps();
  cleanup_grains();

  // If there is an error set this to something negative and also set a message
  notify("PackGrainsGen2 Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PackGrainsGen2::writeVtkFile()
{
  //  ofstream outFile;
  std::ofstream outFile;
  outFile.open(m_VtkOutputFile.c_str(), std::ios_base::binary);
  if(outFile.is_open() == false)
  {
    setErrorMessage("Could not open Vtk File for writing from PackGrains");
    setErrorCondition(-55);
    return -1;
  }
  outFile << "# vtk DataFile Version 2.0" << std::endl;
  outFile << "data set from FFT2dx_GB" << std::endl;
  outFile << "ASCII" << std::endl;
  outFile << "DATASET STRUCTURED_POINTS" << std::endl;
  outFile << "DIMENSIONS " << packingxpoints << " " << packingypoints << " " << packingzpoints << std::endl;
  outFile << "ORIGIN 0.0 0.0 0.0" << std::endl;
  outFile << "SPACING " << packingresx << " " << packingresy << " " << packingresz << std::endl;
  outFile << "POINT_DATA " << packingxpoints * packingypoints * packingzpoints << std::endl;
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
        if(i % 20 == 0 && i > 0) outFile << std::endl;
        outFile << "     ";
        if(name < 100) outFile << " ";
        if(name < 10) outFile << " ";
        outFile << name;
      }
    }
  }
  outFile.close();
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackGrainsGen2::initialize_packinggrid()
{
  DataContainer* m = getDataContainer();

  packingresx = m->getXRes() * 2.0f;
  packingresy = m->getYRes() * 2.0f;
  packingresz = m->getZRes() * 2.0f;
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


void PackGrainsGen2::generate_grain(int phase, int Seed, Field* field)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed)
  DataContainer* m = getDataContainer();
//  int good = 0;
  float r1 = 1;
  float a1 = 0, a3 = 0;
  float b1 = 0, b3 = 0;
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
   */
  a3 = coverb[phase][diameter][0];
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

  field->m_Volumes = vol;
  field->m_EquivalentDiameters = diam;
  field->m_AxisLengths[0] = r1;
  field->m_AxisLengths[1] = r2;
  field->m_AxisLengths[2] = r3;
  field->m_AxisEulerAngles[0] = phi1;
  field->m_AxisEulerAngles[1] = PHI;
  field->m_AxisEulerAngles[2] = phi2;
  field->m_Omega3s = omega3f;
  field->m_PhasesF = phase;
  field->m_Neighborhoods[0] = 0;
  field->m_Neighborhoods[1] = 0;
  field->m_Neighborhoods[2] = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackGrainsGen2::transfer_attributes(int gnum, Field* field)
{
  m_Volumes[gnum] = field->m_Volumes;
  m_EquivalentDiameters[gnum] = field->m_EquivalentDiameters;
  m_AxisLengths[3*gnum+0] = field->m_AxisLengths[0];
  m_AxisLengths[3*gnum+1] = field->m_AxisLengths[1];
  m_AxisLengths[3*gnum+2] = field->m_AxisLengths[2];
  m_AxisEulerAngles[3*gnum+0] = field->m_AxisEulerAngles[0];
  m_AxisEulerAngles[3*gnum+1] = field->m_AxisEulerAngles[1];
  m_AxisEulerAngles[3*gnum+2] = field->m_AxisEulerAngles[2];
  m_Omega3s[gnum] = field->m_Omega3s;
  m_PhasesF[gnum] = field->m_PhasesF;
  m_Neighborhoods[3*gnum+0] = field->m_Neighborhoods[0];
  m_Neighborhoods[3*gnum+1] = field->m_Neighborhoods[1];
  m_Neighborhoods[3*gnum+2] = field->m_Neighborhoods[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackGrainsGen2::initializeAttributes()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[0])
  };

  sizex = dims[0] * m->getXRes();
  sizey = dims[1] * m->getYRes();
  sizez = dims[2] * m->getZRes();
  totalvol = sizex*sizey*sizez;

	for(int i=0;i<totalPoints;i++)
	{
		m_GrainIds[i] = 0;
		m_PhasesC[i] = 0;
		m_EulerAngles[3*i] = -1.0f;
		m_EulerAngles[3*i + 1] = -1.0f;
		m_EulerAngles[3*i + 2] = -1.0f;
		m_SurfaceVoxels[i] = 0;
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
  if(err < 0)
  {
    return err;
  }
  int phase = -1;
  size_t count = phases.size();
  for (size_t i = 0; i < count; i++)
  {
    totaldensity = 0.0;
      phase = phases[i];
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::AxisOrientation, density);
    if(err < 0)
    {
      GGF_CHECK_READ_ERROR(readAxisOrientationData, DREAM3D::HDF5::AxisOrientation)
      //FIXME: This should probably return an ERROR because nothing was read
      return 10;
    }
    size = 36 * 36 * 36;
    if(size != density.size())
    {
      std::cout << "GrainGeneratorFunc::readAxisOrientationData Error: Mismatch in number of elements in the 'AxisOrientation' "
          << " Arrays. The Array stored in the Reconstruction HDF5 file has " << density.size() << " elements and we need " << size << " Elements. "
          << std::endl;
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
  float oxc = m_Centroids[3*gnum];
  float oyc = m_Centroids[3*gnum+1];
  float ozc = m_Centroids[3*gnum+2];
  occolumn = (oxc - (packingresx / 2)) / packingresx;
  ocrow = (oyc - (packingresy / 2)) / packingresy;
  ocplane = (ozc - (packingresz / 2)) / packingresz;
  nccolumn = (xc - (packingresx / 2)) / packingresx;
  ncrow = (yc - (packingresy / 2)) / packingresy;
  ncplane = (zc - (packingresz / 2)) / packingresz;
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m_Centroids[3*gnum] = xc;
  m_Centroids[3*gnum+1] = yc;
  m_Centroids[3*gnum+2] = zc;
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
  x = m_Centroids[3*gnum];
  y = m_Centroids[3*gnum+1];
  z = m_Centroids[3*gnum+2];
  dia = m_EquivalentDiameters[gnum];
  for (size_t n = 1; n < m->getTotalFields(); n++)
  {
    xn = m_Centroids[3*n];
    yn = m_Centroids[3*n+1];
    zn = m_Centroids[3*n+2];
    dia2 = m_EquivalentDiameters[n];
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
        if(add > 0) m_Neighborhoods[3*n + iter]++;
        if(add < 0) m_Neighborhoods[3*n + iter] = m_Neighborhoods[3*n + iter] - 1;
      }
    }
    if(totdist < (3 * (dia2 / 2.0)))
    {
      DoverR = int(totdist / (dia2 / 2.0));
      for (int iter = DoverR; iter < 3; iter++)
      {
        if(add > 0) m_Neighborhoods[3*gnum + iter]++;
        if(add < 0) m_Neighborhoods[3*gnum + iter] = m_Neighborhoods[3*gnum + iter] - 1;
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
    if(gadd > 0 && m_PhasesF[gadd] == phase)
    {
      determine_neighbors(gadd, 1);
    }
    if(gremove > 0 && m_PhasesF[gremove] == phase)
    {
      determine_neighbors(gremove, -1);
    }
    for (size_t i = 1; i < m->getTotalFields(); i++)
    {
      nnum = 0;
      index = i;
      if(index != gremove && m_PhasesF[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        if(dia > maxdiameter[phase]) dia = maxdiameter[phase];
        if(dia < mindiameter[phase]) dia = mindiameter[phase];
        dia = int((dia - mindiameter[phase]) / binstepsize[phase]);
        for (int j = 0; j < 3; j++)
        {
          nnum = m_Neighborhoods[3*index + j];
          if(nnum > 0)
          {
            simneighbordist[iter][dia][j] = simneighbordist[iter][dia][j] + nnum;
          }
        }
        count[dia]++;
      }
    }
    if(gadd > 0 && m_PhasesF[gadd] == phase)
    {
      dia = m_EquivalentDiameters[index];
      if(dia > maxdiameter[phase]) dia = maxdiameter[phase];
      if(dia < mindiameter[phase]) dia = mindiameter[phase];
      dia = int((dia - mindiameter[phase]) / binstepsize[phase]);
      for (int j = 0; j < 3; j++)
      {
        nnum = m_Neighborhoods[3*index + j];
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
    if(gadd > 0 && m_PhasesF[gadd] == phase)
    {
      determine_neighbors(gadd, -1);
    }
    if(gremove > 0 && m_PhasesF[gremove] == phase)
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
    for (size_t b = 1; b < m->getTotalFields(); b++)
    {
      index = b;
      if(index != gremove && m_PhasesF[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        dia = (dia - (mindiameter[phase] / 2.0)) / grainsizediststep[iter];
        if(dia < 0) dia = 0;
        if(dia > grainsizedist[iter].size() - 1) dia = grainsizedist[iter].size() - 1;
        simgrainsizedist[iter][int(dia)]++;
        count++;
      }
    }
    if(gadd > 0 && m_PhasesF[gadd] == phase)
    {
      dia = m_EquivalentDiameters[gadd];
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
      if(m_PeriodicBoundaries == true)
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
      if(m_PeriodicBoundaries == false)
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
      if(m_PeriodicBoundaries == true)
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
      if(m_PeriodicBoundaries == false)
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
  float volcur = m_Volumes[gnum];
  float bovera = m_AxisLengths[3*gnum+1];
  float covera = m_AxisLengths[3*gnum+2];
  float omega3 = m_Omega3s[gnum];
  float radcur1 = 1;
  DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[m_PhasesF[gnum]];

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
  float phi1 = m_AxisEulerAngles[3*gnum];
  float PHI = m_AxisEulerAngles[3*gnum+1];
  float phi2 = m_AxisEulerAngles[3*gnum+2];
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
  xc = m_Centroids[3*gnum];
  yc = m_Centroids[3*gnum+1];
  zc = m_Centroids[3*gnum+2];
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
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighpoints[6];
  neighpoints[0] = -dims[0]*dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0]*dims[1];

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  int oldname;
  size_t column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;
  DimType xmin, xmax, ymin, ymax, zmin, zmax;
  int64_t totpoints = m->totalPoints();
  gsizes.resize(m->getTotalFields());

  for (size_t i = 1; i < m->getTotalFields(); i++)
  {
    gsizes[i] = 0;
  }
  for (size_t i = 1; i < m->getTotalFields(); i++)
  {
    float volcur = m_Volumes[i];
    float bovera = m_AxisLengths[3*i+1];
    float covera = m_AxisLengths[3*i+2];
    float omega3 = m_Omega3s[i];
	xc = m_Centroids[3*i];
	yc = m_Centroids[3*i+1];
	zc = m_Centroids[3*i+2];
    float radcur1 = 0.0f;
    //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
    DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[m_PhasesF[i]];

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
	float phi1 = m_AxisEulerAngles[3*i];
	float PHI = m_AxisEulerAngles[3*i+1];
	float phi2 = m_AxisEulerAngles[3*i+2];
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
    column = (xc - (xRes / 2.0f)) / xRes;
    row = (yc - (yRes / 2.0f)) / yRes;
    plane = (zc - (zRes / 2.0f)) / zRes;
    xmin = int(column - ((radcur1 / xRes) + 1));
    xmax = int(column + ((radcur1 / xRes) + 1));
    ymin = int(row - ((radcur1 / yRes) + 1));
    ymax = int(row + ((radcur1 / yRes) + 1));
    zmin = int(plane - ((radcur1 / zRes) + 1));
    zmax = int(plane + ((radcur1 / zRes) + 1));
    if (m_PeriodicBoundaries == true)
    {
      if (xmin < -dims[0]) xmin = -dims[0];
      if (xmax > 2 * dims[0] - 1) xmax = (2 * dims[0] - 1);
      if (ymin < -dims[1]) ymin = -dims[1];
      if (ymax > 2 * dims[1] - 1) ymax = (2 * dims[1] - 1);
      if (zmin < -dims[2]) zmin = -dims[2];
      if (zmax > 2 * dims[2] - 1) zmax = (2 * dims[2] - 1);
    }
    if (m_PeriodicBoundaries == false)
    {
      if (xmin < 0) xmin = 0;
      if (xmax > dims[0] - 1) xmax = dims[0] - 1;
      if (ymin < 0) ymin = 0;
      if (ymax > dims[1] - 1) ymax = dims[1] - 1;
      if (zmin < 0) zmin = 0;
      if (zmax > dims[2] - 1) zmax = dims[2] - 1;
    }
    for (DimType iter1 = xmin; iter1 < xmax + 1; iter1++)
    {
      for (DimType iter2 = ymin; iter2 < ymax + 1; iter2++)
      {
        for (DimType iter3 = zmin; iter3 < zmax + 1; iter3++)
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
          x = float(column) * xRes;
          y = float(row) * yRes;
          z = float(plane) * zRes;
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

              if (m_GrainIds[currentpoint] > 0)
              {
                oldname = m_GrainIds[currentpoint];
                gsizes[oldname] = gsizes[oldname] - 1;
                m_GrainIds[currentpoint] = -1;
              }
              if (m_GrainIds[currentpoint] == 0)
              {
                m_GrainIds[currentpoint] = i;
                gsizes[i]++;
              }
            }
          }
        }
      }
    }
  }
  newnames.resize(m->getTotalFields());
  int goodcount = 1;
  for (size_t i = 1; i < m->getTotalFields(); i++)
  {
    newnames[i] = 0;
    if (gsizes[i] > 0)
    {
//      m->m_Grains[goodcount] = m->m_Grains[i];
      newnames[i] = goodcount;
      goodcount++;
    }
  }
  for (int i = 0; i < totpoints; i++)
  {
    if (m_GrainIds[i] > 0)
    {
	  m_GrainIds[i] = newnames[m_GrainIds[i]];
    }
  }
  m->resizeFieldDataArrays(goodcount);
}

void PackGrainsGen2::assign_gaps()
{
  DataContainer* m = getDataContainer();
  int64_t totpoints = m->totalPoints();
  int index;
  int timestep = 100;
  int unassignedcount = 1;
  int column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };


  DimType xmin, xmax, ymin, ymax, zmin, zmax;

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

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
	  for (size_t i = 1; i < m->getTotalFields(); i++)
	  {
		float volcur = m_Volumes[i];
		float bovera = m_AxisLengths[3*i+1];
		float covera = m_AxisLengths[3*i+2];
		float omega3 = m_Omega3s[i];
		xc = m_Centroids[3*i];
		yc = m_Centroids[3*i+1];
		zc = m_Centroids[3*i+2];
		float radcur1 = 0.0f;
		//Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
		DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[m_PhasesF[i]];

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
		float phi1 = m_AxisEulerAngles[3*i];
		float PHI = m_AxisEulerAngles[3*i+1];
		float phi2 = m_AxisEulerAngles[3*i+2];
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
		column = (xc - (xRes / 2)) / xRes;
		row = (yc - (yRes / 2)) / yRes;
		plane = (zc - (zRes / 2)) / zRes;
		xmin = int(column - ((radcur1 / xRes) + 1));
		xmax = int(column + ((radcur1 / xRes) + 1));
		ymin = int(row - ((radcur1 / yRes) + 1));
		ymax = int(row + ((radcur1 / yRes) + 1));
		zmin = int(plane - ((radcur1 / zRes) + 1));
		zmax = int(plane + ((radcur1 / zRes) + 1));
		if (m_PeriodicBoundaries == true)
		{
		  if (xmin < -dims[0]) xmin = -dims[0];
		  if (xmax > 2 * dims[0] - 1) xmax = (2 *dims[0] - 1);
		  if (ymin < -dims[1]) ymin = -dims[1];
		  if (ymax > 2 * dims[1] - 1) ymax = (2 * dims[1] - 1);
		  if (zmin < -dims[2]) zmin = -dims[2];
		  if (zmax > 2 * dims[2] - 1) zmax = (2 * dims[2] - 1);
		}
		if (m_PeriodicBoundaries == false)
		{
		  if (xmin < 0) xmin = 0;
		  if (xmax > dims[0] - 1) xmax = dims[0] - 1;
		  if (ymin < 0) ymin = 0;
		  if (ymax > dims[1] - 1) ymax = dims[1] - 1;
		  if (zmin < 0) zmin = 0;
		  if (zmax > dims[2] - 1) zmax = dims[2] - 1;
		}
		for (DimType iter1 = xmin; iter1 < xmax + 1; iter1++)
		{
		  for (DimType iter2 = ymin; iter2 < ymax + 1; iter2++)
		  {
			for (DimType iter3 = zmin; iter3 < zmax + 1; iter3++)
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
			  if(m_GrainIds[index] <= 0)
			  {
				  inside = -1;
				  x = float(column) * xRes;
				  y = float(row) * yRes;
				  z = float(plane) * zRes;
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
	    if (ellipfuncs[i] >= 0) m_GrainIds[i] = newowners[i];
		if (m_GrainIds[i] <= 0) unassignedcount++;
		newowners[i] = -1;
		ellipfuncs[i] = -1.0;
	  }
  }
  for (int i = 0; i < totpoints; i++)
  {
	  if(m_GrainIds[i] > 0) m_PhasesC[i] = m_PhasesF[m_GrainIds[i]];
  }
  delete [] ellipfuncs;
  delete [] newowners;
}
void PackGrainsGen2::cleanup_grains()
{
  DataContainer* m = getDataContainer();
  int64_t totpoints = m->totalPoints();
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighpoints[6];
  DimType xp = dims[0];
  DimType yp = dims[1];
  DimType zp = dims[2];



  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  std::vector<std::vector<int> > vlists;
  vlists.resize(m->getTotalFields());
  std::vector<int> currentvlist;
  std::vector<bool> checked;
  checked.resize(totpoints,false);
  size_t count;
  int touchessurface = 0;
  int good;
  DimType neighbor;
  DimType column, row, plane;
  int index;
  float minsize = 0;
  gsizes.resize(m->getTotalFields());
  for (size_t i = 1; i < m->getTotalFields(); i++)
  {
    gsizes[i] = 0;
  }

  float resConst = m->getXRes() * m->getYRes() * m->getZRes();
  for (int i = 0; i < totpoints; i++)
  {
    touchessurface = 0;
    if(checked[i] == false && m_GrainIds[i] > 0)
    {
      minsize = mindiameter[m_PhasesC[i]] * mindiameter[m_PhasesC[i]] * mindiameter[m_PhasesC[i]] * M_PI / 6.0;
      minsize = int(minsize / (resConst));
      currentvlist.push_back(i);
      count = 0;
      while (count < currentvlist.size())
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
          if(m_PeriodicBoundaries == false)
          {
            if(j == 0 && plane == 0) good = 0;
            if(j == 5 && plane == (zp - 1)) good = 0;
            if(j == 1 && row == 0) good = 0;
            if(j == 4 && row == (yp - 1)) good = 0;
            if(j == 2 && column == 0) good = 0;
            if(j == 3 && column == (xp - 1)) good = 0;
            if(good == 1 && m_GrainIds[neighbor] == m_GrainIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
          else if(m_PeriodicBoundaries == true)
          {
            if(j == 0 && plane == 0) neighbor = neighbor + (xp * yp * zp);
            if(j == 5 && plane == (zp - 1)) neighbor = neighbor - (xp * yp * zp);
            if(j == 1 && row == 0) neighbor = neighbor + (xp * yp);
            if(j == 4 && row == (yp - 1)) neighbor = neighbor - (xp * yp);
            if(j == 2 && column == 0) neighbor = neighbor + (xp);
            if(j == 3 && column == (xp - 1)) neighbor = neighbor - (xp);
            if(m_GrainIds[neighbor] == m_GrainIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
        }
        count++;
      }
      size_t size = vlists[m_GrainIds[i]].size();
      if(size > 0)
      {
        if(size < currentvlist.size())
        {
          for (size_t k = 0; k < vlists[m_GrainIds[i]].size(); k++)
          {
            m_GrainIds[vlists[m_GrainIds[i]][k]] = -1;
          }
          vlists[m_GrainIds[i]].resize(currentvlist.size());
          vlists[m_GrainIds[i]].swap(currentvlist);
        }
        else if(size >= currentvlist.size())
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_GrainIds[currentvlist[k]] = -1;
          }
        }
      }
      else if(size == 0)
      {
        if(currentvlist.size() >= minsize || touchessurface == 1)
        {
          vlists[m_GrainIds[i]].resize(currentvlist.size());
          vlists[m_GrainIds[i]].swap(currentvlist);
        }
        if(currentvlist.size() < minsize && touchessurface == 0)
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_GrainIds[currentvlist[k]] = -1;
          }
        }
      }
      currentvlist.clear();
    }
  }
  for (int i = 0; i < totpoints; i++)
  {
	if(m_GrainIds[i] > 0) gsizes[m_GrainIds[i]]++;
  }
  newnames.resize(m->getTotalFields());
  int goodcount = 1;
  for (size_t i = 1; i < m->getTotalFields(); i++)
  {
     newnames[i] = 0;
     if(gsizes[i] > 0)
     {
//        m->m_Grains[goodcount] = m->m_Grains[i];
        newnames[i] = goodcount;
        goodcount++;
     }
  }
  for (int i = 0; i < totpoints; i++)
  {
	if (m_GrainIds[i] > 0)
	{
	  m_GrainIds[i] = newnames[m_GrainIds[i]];
	}
  }
  for (int i = 0; i < totpoints; i++)
  {
	  if(m_GrainIds[i] > 0) { m_PhasesC[i] = m_PhasesF[m_GrainIds[i]]; }
  }
  m->resizeFieldDataArrays(goodcount);
  assign_gaps();
}
