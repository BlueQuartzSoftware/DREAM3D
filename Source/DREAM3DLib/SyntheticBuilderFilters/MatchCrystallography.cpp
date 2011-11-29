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

#include "MatchCrystallography.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/OrientationMath.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

#define GG_INIT_DOUBLE_ARRAY(array, value, size)\
    for(size_t n = 0; n < size; ++n) { array[n] = (value); }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchCrystallography::MatchCrystallography()
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());

  totalsurfacearea = NULL;
  INIT_AIMARRAY(m_TotalSurfaceArea, float);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchCrystallography::~MatchCrystallography()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::execute()
{
  int err = 0;
  setErrorCondition(err);

  DataContainer* m = getDataContainer();
  H5StatsReader::Pointer h5reader = H5StatsReader::New(m_H5StatsFile);
  readODFData(h5reader);
  readMisorientationData(h5reader);

  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  find_neighbors->setDataContainer(getDataContainer());
  find_neighbors->setObservers(this->getObservers());
  find_neighbors->execute();
  setErrorCondition(find_neighbors->getErrorCondition());
  if (getErrorCondition() != 0){
    setErrorMessage(find_neighbors->getErrorMessage());
    return;
  }
  size_t xtalCount = m->crystruct.size();
  totalsurfacearea = m_TotalSurfaceArea->WritePointer(0, xtalCount);
  for(size_t i=0;i<xtalCount;i++)
  {
	  totalsurfacearea[i] = find_neighbors->totalsurfacearea[i];
  }

  assign_eulers();
  measure_misorientations();
  matchCrystallography();

  // If there is an error set this to something negative and also set a message
  notify("MatchCrystallography Completed", 0, Observable::UpdateProgressMessage);
}



void MatchCrystallography::initializeArrays(std::vector<Ebsd::CrystalStructure> structures)
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

  actualodf.resize(size+1);
  simodf.resize(size+1);
  actualmdf.resize(size+1);
  simmdf.resize(size+1);
  for(size_t i= 1; i < size+1; ++i)
  {
    if(m->crystruct[i] == Ebsd::Hexagonal) nElements = 36*36*12;
    if(m->crystruct[i] == Ebsd::Cubic) nElements = 18*18*18;

    float initValue = 1.0/(float)(nElements);
    actualodf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(actualodf[i], initValue, nElements);

    simodf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(simodf[i], 0.0, nElements);
    actualmdf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(actualmdf[i], initValue, nElements);
    simmdf[i] = SharedFloatArray(new float [nElements]);
    GG_INIT_DOUBLE_ARRAY(simmdf[i], 0.0, nElements);

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatchCrystallography::readODFData(H5StatsReader::Pointer h5io)
{
  DataContainer* m = getDataContainer();
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
  // Now that we have that information - initialize the arrays
  initializeArrays(structures);

  int phase = -1;
  size_t size = phases.size();
  for(size_t i = 0; i< size ;i++)
  {
      phase = phases[i];
    err = h5io->readStatsDataset(phase, DREAM3D::HDF5::ODF, density);
    if (err < 0)
    {
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
		actualodf[phase][j] = density[j];
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatchCrystallography::readMisorientationData(H5StatsReader::Pointer h5io)
{
  DataContainer* m = getDataContainer();
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
		actualmdf[phase][k] = density[k];
    }
  }
  return err;
}




void MatchCrystallography::assign_eulers()
{
  DREAM3D_RANDOMNG_NEW()
    DataContainer* m = getDataContainer();
 // int gnum = 0;
  int numbins = 0;
  float totaldensity = 0;
  float synea1=0,synea2=0,synea3=0;
  float q[5];
  float random;
  int choose, phase;

  size_t xtalCount = m->crystruct.size();
  unbiasedvol.resize(xtalCount);
  for(size_t i=1;i<xtalCount;++i)
  {
   unbiasedvol[i] = 0;
  }
  for(size_t i=1;i<m->m_Grains.size();i++)
  {
    random = rg.genrand_res53();
    choose = 0;
    totaldensity = 0;
    phase = m->m_Grains[i]->phase;
  if(m->crystruct[phase] == Ebsd::Cubic) numbins = 5832;
  if(m->crystruct[phase] == Ebsd::Hexagonal) numbins = 15552;
    for (int j = 0; j < numbins; j++)
    {
      float density = actualodf[phase][j];
      totaldensity = totaldensity + density;
      if (random >= totaldensity) choose = j;
    }
    m_OrientationOps[m->crystruct[phase]]->determineEulerAngles(choose, synea1, synea2, synea3);
    m->m_Grains[i]->euler1 = synea1;
    m->m_Grains[i]->euler2 = synea2;
    m->m_Grains[i]->euler3 = synea3;
    OrientationMath::eulertoQuat(q, synea1, synea2, synea3);
    m->m_Grains[i]->avg_quat[0] = q[0];
    m->m_Grains[i]->avg_quat[1] = q[1];
    m->m_Grains[i]->avg_quat[2] = q[2];
    m->m_Grains[i]->avg_quat[3] = q[3];
    m->m_Grains[i]->avg_quat[4] = q[4];
    if (m->m_Grains[i]->surfacegrain == 0)
    {
      simodf[phase][choose] = simodf[phase][choose] + (float(m->m_Grains[i]->numvoxels) * m->resx * m->resy * m->resz);
      unbiasedvol[phase] = unbiasedvol[phase] + (float(m->m_Grains[i]->numvoxels) * m->resx * m->resy * m->resz);
    }
  }
  for(int i=0;i<numbins;i++)
  {
	  simodf[phase][i] = simodf[phase][i]/unbiasedvol[phase];
  }
}

void MatchCrystallography::MC_LoopBody1(int phase, size_t neighbor, int j,std::vector<float>* misolist,std::vector<float>* neighborsurfacealist, float &mdfchange)
{
  DataContainer* m = getDataContainer();
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
  curmisobin = m_OrientationOps[m->crystruct[phase]]->getMisoBin( curmiso1, curmiso2, curmiso3);
  q2[1] = m->m_Grains[neighbor]->avg_quat[1];
  q2[2] = m->m_Grains[neighbor]->avg_quat[2];
  q2[3] = m->m_Grains[neighbor]->avg_quat[3];
  q2[4] = m->m_Grains[neighbor]->avg_quat[4];
  w = m_OrientationOps[m->crystruct[phase]]->getMisoQuat(q1,q2,n1,n2,n3);
  OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientationOps[m->crystruct[phase]]->getMisoBin(n1, n2, n3);
  mdfchange = mdfchange + (((actualmdf[phase][curmisobin]-simmdf[phase][curmisobin])*(actualmdf[phase][curmisobin]-simmdf[phase][curmisobin])) - ((actualmdf[phase][curmisobin]-(simmdf[phase][curmisobin]-(neighsurfarea/totalsurfacearea[phase])))*(actualmdf[phase][curmisobin]-(simmdf[phase][curmisobin]-(neighsurfarea/totalsurfacearea[phase])))));
  mdfchange = mdfchange + (((actualmdf[phase][newmisobin]-simmdf[phase][newmisobin])*(actualmdf[phase][newmisobin]-simmdf[phase][newmisobin])) - ((actualmdf[phase][newmisobin]-(simmdf[phase][newmisobin]+(neighsurfarea/totalsurfacearea[phase])))*(actualmdf[phase][newmisobin]-(simmdf[phase][newmisobin]+(neighsurfarea/totalsurfacearea[phase])))));
}

void MatchCrystallography::MC_LoopBody2(int phase, size_t neighbor, int j,std::vector<float>* misolist,std::vector<float>* neighborsurfacealist)
{
  DataContainer* m = getDataContainer();
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
  curmisobin = m_OrientationOps[m->crystruct[phase]]->getMisoBin(curmiso1, curmiso2, curmiso3);
  q2[1] = m->m_Grains[neighbor]->avg_quat[1];
  q2[2] = m->m_Grains[neighbor]->avg_quat[2];
  q2[3] = m->m_Grains[neighbor]->avg_quat[3];
  q2[4] = m->m_Grains[neighbor]->avg_quat[4];
  w = m_OrientationOps[m->crystruct[phase]]->getMisoQuat(q1,q2,n1,n2,n3);
  OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientationOps[m->crystruct[phase]]->getMisoBin(n1, n2, n3);
  misolist->at(3 * j) = miso1;
  misolist->at(3 * j + 1) = miso2;
  misolist->at(3 * j + 2) = miso3;
  simmdf[phase][curmisobin] = simmdf[phase][curmisobin] - (neighsurfarea / totalsurfacearea[phase]);
  simmdf[phase][newmisobin] = simmdf[phase][newmisobin] + (neighsurfarea / totalsurfacearea[phase]);
}


void MatchCrystallography::swapOutOrientation( int &badtrycount, int &numbins, float currentodferror, float currentmdferror)
{
  DREAM3D_RANDOMNG_NEW()
    DataContainer* m = getDataContainer();
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
    selectedgrain1 = int(rg.genrand_res53() * m->m_Grains.size());
    if (selectedgrain1 == 0) selectedgrain1 = 1;
    if (selectedgrain1 == m->m_Grains.size()) selectedgrain1 = m->m_Grains.size() - 1;
    if (m->m_Grains[selectedgrain1]->surfacegrain > 0) good = 0;
  }

  ea1 = m->m_Grains[selectedgrain1]->euler1;
  ea2 = m->m_Grains[selectedgrain1]->euler2;
  ea3 = m->m_Grains[selectedgrain1]->euler3;
  OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
  int phase = m->m_Grains[selectedgrain1]->phase;
  g1odfbin = m_OrientationOps[m->crystruct[phase]]->getOdfBin(r1, r2, r3);
  random = rg.genrand_res53();
  int choose = 0;
  totaldensity = 0;
  for (int i = 0; i < numbins; i++)
  {
    float density = actualodf[phase][i];
    totaldensity = totaldensity + density;
    if (random >= totaldensity) choose = i;
  }

  m_OrientationOps[m->crystruct[phase]]->determineEulerAngles(choose, g1ea1, g1ea2, g1ea3);
  OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);

  float odfchange = ((actualodf[phase][choose] - simodf[phase][choose]) * (actualodf[phase][choose] - simodf[phase][choose])) - ((actualodf[phase][choose] - (simodf[phase][choose]
      + (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]))) * (actualodf[phase][choose] - (simodf[phase][choose]
      + (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]))));
  odfchange = odfchange + (((actualodf[phase][g1odfbin] - simodf[phase][g1odfbin]) * (actualodf[phase][g1odfbin] - simodf[phase][g1odfbin])) - ((actualodf[phase][g1odfbin] - (simodf[phase][g1odfbin]
      - (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]))) * (actualodf[phase][g1odfbin] - (simodf[phase][g1odfbin]
      - (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase])))));

  float mdfchange = 0;
  nlist = m->m_Grains[selectedgrain1]->neighborlist;
  misolist = m->m_Grains[selectedgrain1]->misorientationlist;
  neighborsurfacealist = m->m_Grains[selectedgrain1]->neighborsurfacealist;
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
    m->m_Grains[selectedgrain1]->euler1 = g1ea1;
    m->m_Grains[selectedgrain1]->euler2 = g1ea2;
    m->m_Grains[selectedgrain1]->euler3 = g1ea3;
    m->m_Grains[selectedgrain1]->avg_quat[1] = q1[1];
    m->m_Grains[selectedgrain1]->avg_quat[2] = q1[2];
    m->m_Grains[selectedgrain1]->avg_quat[3] = q1[3];
    m->m_Grains[selectedgrain1]->avg_quat[4] = q1[4];
    simodf[phase][choose] = simodf[phase][choose] + (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]);
    simodf[phase][g1odfbin] = simodf[phase][g1odfbin] - (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]);
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

void MatchCrystallography::switchOrientations( int &badtrycount, int &numbins, float currentodferror, float currentmdferror)
{
  DREAM3D_RANDOMNG_NEW()
    DataContainer* m = getDataContainer();
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
    selectedgrain1 = static_cast<size_t>(rg.genrand_res53() * m->m_Grains.size());
    if (selectedgrain1 == 0) selectedgrain1 = 1;
    if (selectedgrain1 == m->m_Grains.size()) selectedgrain1 = m->m_Grains.size() - 1;
    selectedgrain2 = static_cast<size_t>(rg.genrand_res53() * m->m_Grains.size());
    if (selectedgrain2 == 0) selectedgrain2 = 1;
    if (selectedgrain2 == m->m_Grains.size()) selectedgrain2 = m->m_Grains.size() - 1;
    if (m->m_Grains[selectedgrain1]->surfacegrain > 0 || m->m_Grains[selectedgrain2]->surfacegrain > 0) good = 0;
  }
  g1ea1 = m->m_Grains[selectedgrain1]->euler1;
  g1ea2 = m->m_Grains[selectedgrain1]->euler2;
  g1ea3 = m->m_Grains[selectedgrain1]->euler3;
  g2ea1 = m->m_Grains[selectedgrain2]->euler1;
  g2ea2 = m->m_Grains[selectedgrain2]->euler2;
  g2ea3 = m->m_Grains[selectedgrain2]->euler3;
  q1[1] = m->m_Grains[selectedgrain1]->avg_quat[1];
  q1[2] = m->m_Grains[selectedgrain1]->avg_quat[2];
  q1[3] = m->m_Grains[selectedgrain1]->avg_quat[3];
  q1[4] = m->m_Grains[selectedgrain1]->avg_quat[4];
  int phase = m->m_Grains[selectedgrain1]->phase;
  OrientationMath::eulertoRod(r1, r2, r3, g1ea1, g1ea2, g1ea3);
  g1odfbin = m_OrientationOps[m->crystruct[phase]]->getOdfBin(r1, r2, r3);
  q1[1] = m->m_Grains[selectedgrain2]->avg_quat[1];
  q1[2] = m->m_Grains[selectedgrain2]->avg_quat[2];
  q1[3] = m->m_Grains[selectedgrain2]->avg_quat[3];
  q1[4] = m->m_Grains[selectedgrain2]->avg_quat[4];
  OrientationMath::eulertoRod(r1, r2, r3, g2ea1, g2ea2, g2ea3);
  g2odfbin = m_OrientationOps[m->crystruct[phase]]->getOdfBin(r1, r2, r3);

  float odfchange = ((actualodf[phase][g1odfbin]-simodf[phase][g1odfbin]) * (actualodf[phase][g1odfbin]-simodf[phase][g1odfbin])) - ((actualodf[phase][g1odfbin]
     -(simodf[phase][g1odfbin] - (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]) + (float(m->m_Grains[selectedgrain2]->numvoxels) * m->resx
          * m->resy * m->resz / unbiasedvol[phase]))) * (actualodf[phase][g1odfbin]-(simodf[phase][g1odfbin] - (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase])
      + (float(m->m_Grains[selectedgrain2]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]))));
  odfchange = odfchange + (((actualodf[phase][g2odfbin]-simodf[phase][g2odfbin]) * (actualodf[phase][g2odfbin]-simodf[phase][g2odfbin])) - ((actualodf[phase][g2odfbin]
     -(simodf[phase][g2odfbin] - (float(m->m_Grains[selectedgrain2]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]) + (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx
          * m->resy *m-> resz / unbiasedvol[phase]))) * (actualodf[phase][g2odfbin]-(simodf[phase][g2odfbin] - (float(m->m_Grains[selectedgrain2]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase])
      + (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase])))));


  float mdfchange = 0;
  OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
  nlist = m->m_Grains[selectedgrain1]->neighborlist;
  misolist = m->m_Grains[selectedgrain1]->misorientationlist;
  neighborsurfacealist = m->m_Grains[selectedgrain1]->neighborsurfacealist;
  size_t size = 0;
  if (NULL != nlist)
  {
    size = nlist->size();
  }
  for (size_t j = 0; j < size; j++)
  {
    size_t neighbor = nlist->at(j);
    if (neighbor != selectedgrain2)
    {
      MC_LoopBody1(phase, neighbor, j, misolist, neighborsurfacealist, mdfchange);
    }
  }

  OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);
  nlist = m->m_Grains[selectedgrain2]->neighborlist;
  misolist = m->m_Grains[selectedgrain2]->misorientationlist;
  neighborsurfacealist = m->m_Grains[selectedgrain2]->neighborsurfacealist;
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
      MC_LoopBody1(phase, neighbor, j, misolist, neighborsurfacealist, mdfchange);
    }
  }

  deltaerror = (odfchange/currentodferror) + (mdfchange/currentmdferror);
  if (deltaerror > 0)
  {
    badtrycount = 0;
    m->m_Grains[selectedgrain1]->euler1 = g2ea1;
    m->m_Grains[selectedgrain1]->euler2 = g2ea2;
    m->m_Grains[selectedgrain1]->euler3 = g2ea3;
    m->m_Grains[selectedgrain2]->euler1 = g1ea1;
    m->m_Grains[selectedgrain2]->euler2 = g1ea2;
    m->m_Grains[selectedgrain2]->euler3 = g1ea3;
    simodf[phase][g1odfbin] = simodf[phase][g1odfbin] + (float(m->m_Grains[selectedgrain2]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase])
        - (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]);
    simodf[phase][g2odfbin] = simodf[phase][g2odfbin] + (float(m->m_Grains[selectedgrain1]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase])
        - (float(m->m_Grains[selectedgrain2]->numvoxels) * m->resx * m->resy * m->resz / unbiasedvol[phase]);

  OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
  nlist = m->m_Grains[selectedgrain1]->neighborlist;
    misolist = m->m_Grains[selectedgrain1]->misorientationlist;
    neighborsurfacealist = m->m_Grains[selectedgrain1]->neighborsurfacealist;
    m->m_Grains[selectedgrain1]->avg_quat[1] = q1[1];
    m->m_Grains[selectedgrain1]->avg_quat[2] = q1[2];
    m->m_Grains[selectedgrain1]->avg_quat[3] = q1[3];
    m->m_Grains[selectedgrain1]->avg_quat[4] = q1[4];
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
    nlist = m->m_Grains[selectedgrain2]->neighborlist;
    misolist = m->m_Grains[selectedgrain2]->misorientationlist;
    neighborsurfacealist = m->m_Grains[selectedgrain2]->neighborsurfacealist;
    m->m_Grains[selectedgrain2]->avg_quat[1] = q1[1];
    m->m_Grains[selectedgrain2]->avg_quat[2] = q1[2];
    m->m_Grains[selectedgrain2]->avg_quat[3] = q1[3];
    m->m_Grains[selectedgrain2]->avg_quat[4] = q1[4];
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

void MatchCrystallography::matchCrystallography()
{
  DREAM3D_RANDOMNG_NEW()
    DataContainer* m = getDataContainer();
  int numbins = 0;

  int iterations = 0;
  int badtrycount = 0;
  float random;
  float currentodferror = 0;
  float currentmdferror = 0;
  size_t xtalSize = m->crystruct.size();
  for(size_t iter=1;iter<xtalSize;++iter)
  {
    if(m->crystruct[iter] == Ebsd::Cubic) numbins = 18*18*18;
    if(m->crystruct[iter] == Ebsd::Hexagonal) numbins = 36*36*12;
    while(badtrycount < 10000 && iterations < 1000000)
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
  for(int i = 0; i < m->totalpoints; i++)
  {
    m->euler1s[i] = m->m_Grains[m->grain_indicies[i]]->euler1;
    m->euler2s[i] = m->m_Grains[m->grain_indicies[i]]->euler2;
    m->euler3s[i] = m->m_Grains[m->grain_indicies[i]]->euler3;
  }
}
void  MatchCrystallography::measure_misorientations ()
{
  DataContainer* m = getDataContainer();
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float q1[5];
  float q2[5];
  Ebsd::CrystalStructure phase1, phase2;
  int mbin;


  std::vector<int>* nlist;
  std::vector<float>* neighsurfarealist;

  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    nlist = m->m_Grains[i]->neighborlist;
    neighsurfarealist = m->m_Grains[i]->neighborsurfacealist;
    if (NULL != m->m_Grains[i]->misorientationlist)
    {
      delete m->m_Grains[i]->misorientationlist;
      m->m_Grains[i]->misorientationlist = NULL;
    }
    if (NULL != nlist)
    {
      m->m_Grains[i]->misorientationlist = new std::vector<float>(nlist->size() * 3, 0.0);
    }

    q1[1] = m->m_Grains[i]->avg_quat[1];
    q1[2] = m->m_Grains[i]->avg_quat[2];
    q1[3] = m->m_Grains[i]->avg_quat[3];
    q1[4] = m->m_Grains[i]->avg_quat[4];
    phase1 = m->crystruct[m->m_Grains[i]->phase];
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
      q2[1] = m->m_Grains[nname]->avg_quat[1];
      q2[2] = m->m_Grains[nname]->avg_quat[2];
      q2[3] = m->m_Grains[nname]->avg_quat[3];
      q2[4] = m->m_Grains[nname]->avg_quat[4];
      phase2 = m->crystruct[m->m_Grains[nname]->phase];
      if (phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
      OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
      if (phase1 == phase2)
      {
        m->m_Grains[i]->misorientationlist->at(3 * j) = r1;
        m->m_Grains[i]->misorientationlist->at(3 * j + 1) = r2;
        m->m_Grains[i]->misorientationlist->at(3 * j + 2) = r3;
      }
      if (phase1 != phase2)
      {
        m->m_Grains[i]->misorientationlist->at(3 * j) = -100;
        m->m_Grains[i]->misorientationlist->at(3 * j + 1) = -100;
        m->m_Grains[i]->misorientationlist->at(3 * j + 2) = -100;
      }
      if (phase1 == phase2)
      {
        mbin =
            m_OrientationOps[phase1]->getMisoBin(m->m_Grains[i]->misorientationlist->at(3 * j),
                                                m->m_Grains[i]->misorientationlist->at(3 * j + 1),
                                                m->m_Grains[i]->misorientationlist->at(3 * j + 2));
      }

      if (m->m_Grains[i]->surfacegrain == 0 && (nname > i || m->m_Grains[nname]->surfacegrain == 1) && phase1 == phase2)
      {
        simmdf[m->m_Grains[i]->phase][mbin] = simmdf[m->m_Grains[i]->phase][mbin] + (neighsurfarea / totalsurfacearea[m->m_Grains[i]->phase]);
      }
    }
  }
}
