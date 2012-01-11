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
#include "DREAM3DLib/Common/DataContainerMacros.h"
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

MatchCrystallography::MatchCrystallography()  :
AbstractFilter(),
m_GrainIds(NULL),
m_EulerAnglesC(NULL),
m_SurfaceFields(NULL),
m_PhasesF(NULL),
m_NumCells(NULL),
m_EulerAnglesF(NULL),
m_AvgQuats(NULL),
m_NeighborList(NULL),
m_SharedSurfaceAreaList(NULL),
m_TotalSurfaceArea(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());
  setupFilterOptions();
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
void MatchCrystallography::setupFilterOptions()
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
  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  int err = 0;
  std::stringstream ss;

  DataContainer* m = getDataContainer();

  // Cell Data
  PF_CHECK_ARRAY_EXISTS( m, DREAM3D, VoxelData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels);
  PF_MAKE_SURE_ARRAY_EXISTS_SUFFIX( m, DREAM3D, VoxelData, EulerAngles, C, ss, FloatArrayType, voxels, 3);


  // Field Data
  PF_CHECK_ARRAY_EXISTS(m, DREAM3D, FieldData, SurfaceFields, ss, -303,  int8_t, Int8ArrayType, fields);
  PF_CHECK_ARRAY_EXISTS_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, -303,  int32_t, Int32ArrayType, fields);
  PF_CHECK_ARRAY_EXISTS(m, DREAM3D, FieldData, NumCells, ss, -303,  int32_t, Int32ArrayType, fields);
  PF_MAKE_SURE_ARRAY_EXISTS_SUFFIX(m, DREAM3D, FieldData, EulerAngles, F, ss, FloatArrayType, fields, 3);
  PF_MAKE_SURE_ARRAY_EXISTS(m, DREAM3D, FieldData, AvgQuats, ss, FloatArrayType, fields, 5);

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                                          (m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
	  ss << "NeighborLists Array Not Initialized At Beginning of MatchCrystallography Filter" << std::endl;
	  err = -300;
  }

  // And we do the same for the SharedSurfaceArea list
  m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>
                                 (m->getFieldData(DREAM3D::FieldData::SharedSurfaceAreaList).get());
  if(m_SharedSurfaceAreaList == NULL)
  {
    ss << "SurfaceAreaLists Array Not Initialized At Beginning of MatchCrystallography Filter" << std::endl;
    err = -300;
  }

  // Ensemble Data
  PF_CHECK_ARRAY_EXISTS(m, DREAM3D, EnsembleData, TotalSurfaceArea, ss, -303,  float, FloatArrayType, m->crystruct.size() );

  setErrorCondition(err);
  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::execute()
{
  int err = 0;
  setErrorCondition(err);

  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  DataContainer* m = getDataContainer();

  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  H5StatsReader::Pointer h5reader = H5StatsReader::New(m_H5StatsInputFile);
  readODFData(h5reader);
  if (getErrorCondition() < 0) {
    return;
  }
  readMisorientationData(h5reader);
  if (getErrorCondition() < 0) {
    return;
  }

  int64_t totalPoints = m->totalPoints();
  int totalFields = m->getTotalFields();
  dataCheck(false, totalPoints, totalFields, 1);

  assign_eulers();
  measure_misorientations();
  matchCrystallography();

  // If there is an error set this to something negative and also set a message
  notify("MatchCrystallography Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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

    float initValue = 1.0f/(float)(nElements);
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
  for(size_t i=1;i<m->getTotalFields();i++)
  {
    random = rg.genrand_res53();
    choose = 0;
    totaldensity = 0;
    phase = m_PhasesF[i];
  if(m->crystruct[phase] == Ebsd::Cubic) numbins = 5832;
  if(m->crystruct[phase] == Ebsd::Hexagonal) numbins = 15552;
    for (int j = 0; j < numbins; j++)
    {
      float density = actualodf[phase][j];
      totaldensity = totaldensity + density;
      if (random >= totaldensity) choose = j;
    }
    m_OrientationOps[m->crystruct[phase]]->determineEulerAngles(choose, synea1, synea2, synea3);
    m_EulerAnglesF[3*i] = synea1;
    m_EulerAnglesF[3*i+1] = synea2;
    m_EulerAnglesF[3*i+2] = synea3;
    OrientationMath::eulertoQuat(q, synea1, synea2, synea3);
    m_AvgQuats[5*i] = q[0];
    m_AvgQuats[5*i+1] = q[1];
    m_AvgQuats[5*i+2] = q[2];
    m_AvgQuats[5*i+3] = q[3];
    m_AvgQuats[5*i+4] = q[4];
    if (m_SurfaceFields[i] == false)
    {
      simodf[phase][choose] = simodf[phase][choose] + (float(m_NumCells[i]) * m->getXRes() * m->getYRes() * m->getZRes());
      unbiasedvol[phase] = unbiasedvol[phase] + (float(m_NumCells[i]) * m->getXRes() * m->getYRes() * m->getZRes());
    }
  }
  for(int i=0;i<numbins;i++)
  {
	  simodf[phase][i] = simodf[phase][i]/unbiasedvol[phase];
  }
}

void MatchCrystallography::MC_LoopBody1(int grain, int phase, int j, float neighsurfarea, Ebsd::CrystalStructure sym, float q1[5], float q2[5])
{
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float curmiso1, curmiso2, curmiso3;
  size_t curmisobin, newmisobin;

  curmiso1 = misorientationlists[grain][3*j];
  curmiso2 = misorientationlists[grain][3*j+1];
  curmiso3 = misorientationlists[grain][3*j+2];
  curmisobin = m_OrientationOps[sym]->getMisoBin( curmiso1, curmiso2, curmiso3);
  w = m_OrientationOps[sym]->getMisoQuat(q1,q2,n1,n2,n3);
  OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientationOps[sym]->getMisoBin(n1, n2, n3);
  mdfchange = mdfchange + (((actualmdf[phase][curmisobin]-simmdf[phase][curmisobin])*(actualmdf[phase][curmisobin]-simmdf[phase][curmisobin])) - ((actualmdf[phase][curmisobin]-(simmdf[phase][curmisobin]-(neighsurfarea/m_TotalSurfaceArea[phase])))*(actualmdf[phase][curmisobin]-(simmdf[phase][curmisobin]-(neighsurfarea/m_TotalSurfaceArea[phase])))));
  mdfchange = mdfchange + (((actualmdf[phase][newmisobin]-simmdf[phase][newmisobin])*(actualmdf[phase][newmisobin]-simmdf[phase][newmisobin])) - ((actualmdf[phase][newmisobin]-(simmdf[phase][newmisobin]+(neighsurfarea/m_TotalSurfaceArea[phase])))*(actualmdf[phase][newmisobin]-(simmdf[phase][newmisobin]+(neighsurfarea/m_TotalSurfaceArea[phase])))));
}

void MatchCrystallography::MC_LoopBody2(int grain, int phase, int j, float neighsurfarea, Ebsd::CrystalStructure sym, float q1[5], float q2[5])
{
  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float curmiso1, curmiso2, curmiso3;
  size_t curmisobin, newmisobin;
  float miso1 = 0.0f, miso2 = 0.0f, miso3 = 0.0f;

  curmiso1 = misorientationlists[grain][3 * j];
  curmiso2 = misorientationlists[grain][3 * j + 1];
  curmiso3 = misorientationlists[grain][3 * j + 2];
  curmisobin = m_OrientationOps[sym]->getMisoBin(curmiso1, curmiso2, curmiso3);
  w = m_OrientationOps[sym]->getMisoQuat(q1,q2,n1,n2,n3);
  OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
  newmisobin = m_OrientationOps[sym]->getMisoBin(n1, n2, n3);
  misorientationlists[grain][3 * j] = miso1;
  misorientationlists[grain][3 * j + 1] = miso2;
  misorientationlists[grain][3 * j + 2] = miso3;
  simmdf[phase][curmisobin] = simmdf[phase][curmisobin] - (neighsurfarea / m_TotalSurfaceArea[phase]);
  simmdf[phase][newmisobin] = simmdf[phase][newmisobin] + (neighsurfarea / m_TotalSurfaceArea[phase]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::matchCrystallography()
{
  DataContainer* m = getDataContainer();
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;

  int64_t totalPoints = m->totalPoints();
  m_GrainIds = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == m_GrainIds) { return; }
  m_EulerAnglesC = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::EulerAngles, totalPoints*3, this);
  if (NULL == m_EulerAnglesC) { return; }

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  float volResConst = xRes * yRes * zRes;
  DREAM3D_RANDOMNG_NEW()
  int numbins = 0;
  int iterations = 0, badtrycount = 0;
  float random = 0;
  int good = 0;
  float q1[5], q2[5];
  float ea1 = 0, ea2 = 0, ea3 = 0;
  float r1 = 0, r2 = 0, r3 = 0;
  float g1ea1 = 0, g1ea2 = 0, g1ea3 = 0, g2ea1 = 0, g2ea2 = 0, g2ea3 = 0;
  int g1odfbin = 0, g2odfbin = 0;
  float totaldensity = 0, deltaerror = 0;
  float currentodferror = 0, currentmdferror = 0;
  size_t selectedgrain1 = 0, selectedgrain2 = 0;
  size_t xtalSize = m->crystruct.size();
  for(size_t iter=1;iter<xtalSize;++iter)
  {
    if(m->crystruct[iter] == Ebsd::Cubic) numbins = 18 * 18 * 18;
    if(m->crystruct[iter] == Ebsd::Hexagonal) numbins = 36 * 36 * 12;
    while (badtrycount < 10000 && iterations < 1000000)
    {
      currentodferror = 0;
      currentmdferror = 0;
      for (int i = 0; i < numbins; i++)
      {
        currentodferror = currentodferror + ((actualodf[iter][i] - simodf[iter][i]) * (actualodf[iter][i] - simodf[iter][i]));
      }
      for (int i = 0; i < (numbins); i++)
      {
        currentmdferror = currentmdferror + ((actualmdf[iter][i] - simmdf[iter][i]) * (actualmdf[iter][i] - simmdf[iter][i]));
      }
      iterations++;
      badtrycount++;
      random = rg.genrand_res53();

      if(random < 0.5) // SwapOutOrientation
      {
		  good = 0;
		  while (good == 0)
		  {
			good = 1;
			selectedgrain1 = int(rg.genrand_res53() * m->getTotalFields());
			if (selectedgrain1 == 0) selectedgrain1 = 1;
			if (selectedgrain1 == m->getTotalFields()) selectedgrain1 = m->getTotalFields() - 1;
			if (m_SurfaceFields[selectedgrain1] == true) good = 0;
		  }

		  ea1 = m_EulerAnglesF[3*selectedgrain1];
		  ea2 = m_EulerAnglesF[3*selectedgrain1+1];
		  ea3 = m_EulerAnglesF[3*selectedgrain1+2];
		  OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
		  int phase = m_PhasesF[selectedgrain1];
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

		  odfchange = ((actualodf[phase][choose] - simodf[phase][choose]) * (actualodf[phase][choose] - simodf[phase][choose])) - ((actualodf[phase][choose] - (simodf[phase][choose]
			  + (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]))) * (actualodf[phase][choose] - (simodf[phase][choose]
			  + (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]))));
		  odfchange = odfchange + (((actualodf[phase][g1odfbin] - simodf[phase][g1odfbin]) * (actualodf[phase][g1odfbin] - simodf[phase][g1odfbin])) - ((actualodf[phase][g1odfbin] - (simodf[phase][g1odfbin]
			  - (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]))) * (actualodf[phase][g1odfbin] - (simodf[phase][g1odfbin]
			  - (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase])))));

		  mdfchange = 0;
		  size_t size = 0;
		  if (neighborlist[selectedgrain1].size() != 0) size = neighborlist[selectedgrain1].size();
		  for (size_t j = 0; j < size; j++)
		  {
			  int neighbor = neighborlist[selectedgrain1][j];
			  ea1 = m_EulerAnglesF[3*neighbor];
			  ea2 = m_EulerAnglesF[3*neighbor+1];
			  ea3 = m_EulerAnglesF[3*neighbor+2];
		      OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
			  float neighsurfarea = neighborsurfacearealist[selectedgrain1][j];
			  MC_LoopBody1(selectedgrain1, phase, j, neighsurfarea, m->crystruct[phase], q1, q2);
		  }

		  deltaerror = (odfchange/currentodferror) + (mdfchange/currentmdferror);
		  if (deltaerror > 0)
		  {
			badtrycount = 0;
			m_EulerAnglesF[3*selectedgrain1] = g1ea1;
			m_EulerAnglesF[3*selectedgrain1+1] = g1ea2;
			m_EulerAnglesF[3*selectedgrain1+2] = g1ea3;
			m_AvgQuats[5*selectedgrain1+1] = q1[1];
			m_AvgQuats[5*selectedgrain1+2] = q1[2];
			m_AvgQuats[5*selectedgrain1+3] = q1[3];
			m_AvgQuats[5*selectedgrain1+4] = q1[4];
			simodf[phase][choose] = simodf[phase][choose] + (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]);
			simodf[phase][g1odfbin] = simodf[phase][g1odfbin] - (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]);
			size_t size = 0;
			if (neighborlist[selectedgrain1].size() != 0) size = neighborlist[selectedgrain1].size();
			for (size_t j = 0; j < size; j++)
			{
			  int neighbor = neighborlist[selectedgrain1][j];
			  ea1 = m_EulerAnglesF[3*neighbor];
			  ea2 = m_EulerAnglesF[3*neighbor+1];
			  ea3 = m_EulerAnglesF[3*neighbor+2];
		      OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
			  float neighsurfarea = neighborsurfacearealist[selectedgrain1][j];
			  MC_LoopBody2(selectedgrain1, phase, j, neighsurfarea, m->crystruct[phase], q1, q2);
			}
		  }
      }
      else if(random > 0.5) // SwitchOrientation
      {
		  good = 0;
		  while (good == 0)
		  {
			good = 1;
			selectedgrain1 = static_cast<size_t>(rg.genrand_res53() * m->getTotalFields());
			if (selectedgrain1 == 0) selectedgrain1 = 1;
			if (selectedgrain1 == m->getTotalFields()) selectedgrain1 = m->getTotalFields() - 1;
			selectedgrain2 = static_cast<size_t>(rg.genrand_res53() * m->getTotalFields());
			if (selectedgrain2 == 0) selectedgrain2 = 1;
			if (selectedgrain2 == m->getTotalFields()) selectedgrain2 = m->getTotalFields() - 1;
			if (m_SurfaceFields[selectedgrain1] == true || m_SurfaceFields[selectedgrain2] == true) good = 0;
		  }
		  g1ea1 = m_EulerAnglesF[3*selectedgrain1];
		  g1ea2 = m_EulerAnglesF[3*selectedgrain1+1];
		  g1ea3 = m_EulerAnglesF[3*selectedgrain1+2];
		  g2ea1 = m_EulerAnglesF[3*selectedgrain2];
		  g2ea2 = m_EulerAnglesF[3*selectedgrain2+1];
		  g2ea3 = m_EulerAnglesF[3*selectedgrain2+2];
		  q1[1] = m_AvgQuats[5*selectedgrain1+1];
		  q1[2] = m_AvgQuats[5*selectedgrain1+2];
		  q1[3] = m_AvgQuats[5*selectedgrain1+3];
		  q1[4] = m_AvgQuats[5*selectedgrain1+4];
		  int phase = m_PhasesF[selectedgrain1];
		  OrientationMath::eulertoRod(r1, r2, r3, g1ea1, g1ea2, g1ea3);
		  g1odfbin = m_OrientationOps[m->crystruct[phase]]->getOdfBin(r1, r2, r3);
		  q1[1] = m_AvgQuats[5*selectedgrain2+1];
		  q1[2] = m_AvgQuats[5*selectedgrain2+2];
		  q1[3] = m_AvgQuats[5*selectedgrain2+3];
		  q1[4] = m_AvgQuats[5*selectedgrain2+4];
		  OrientationMath::eulertoRod(r1, r2, r3, g2ea1, g2ea2, g2ea3);
		  g2odfbin = m_OrientationOps[m->crystruct[phase]]->getOdfBin(r1, r2, r3);

		  odfchange = ((actualodf[phase][g1odfbin]-simodf[phase][g1odfbin]) * (actualodf[phase][g1odfbin]-simodf[phase][g1odfbin])) - ((actualodf[phase][g1odfbin]
			 -(simodf[phase][g1odfbin] - (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]) + (float(m_NumCells[selectedgrain2]) * m->getXRes()
				  * m->getYRes() * m->getZRes() / unbiasedvol[phase]))) * (actualodf[phase][g1odfbin]-(simodf[phase][g1odfbin] - (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase])
			  + (float(m_NumCells[selectedgrain2]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]))));
		  odfchange = odfchange + (((actualodf[phase][g2odfbin]-simodf[phase][g2odfbin]) * (actualodf[phase][g2odfbin]-simodf[phase][g2odfbin])) - ((actualodf[phase][g2odfbin]
			 -(simodf[phase][g2odfbin] - (float(m_NumCells[selectedgrain2]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]) + (float(m_NumCells[selectedgrain1]) * m->getXRes()
				  * m->getYRes() *m->getZRes() / unbiasedvol[phase]))) * (actualodf[phase][g2odfbin]-(simodf[phase][g2odfbin] - (float(m_NumCells[selectedgrain2]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase])
			  + (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase])))));


		  mdfchange = 0;
		  OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
		  size_t size = 0;
		  if (neighborlist[selectedgrain1].size() != 0) size = neighborlist[selectedgrain1].size();
		  for (size_t j = 0; j < size; j++)
		  {
			int neighbor = neighborlist[selectedgrain1][j];
			ea1 = m_EulerAnglesF[3*neighbor];
			ea2 = m_EulerAnglesF[3*neighbor+1];
			ea3 = m_EulerAnglesF[3*neighbor+2];
		    OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
			float neighsurfarea = neighborsurfacearealist[selectedgrain1][j];
			if (neighbor != selectedgrain2)
			{
			  MC_LoopBody1(selectedgrain1, phase, j, neighsurfarea, m->crystruct[phase], q1, q2);
			}
		  }

		  OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);
		  size = 0;
		  if (neighborlist[selectedgrain2].size() != 0) size = neighborlist[selectedgrain2].size();
		  for (size_t j = 0; j < size; j++)
		  {
			size_t neighbor = neighborlist[selectedgrain2][j];
			ea1 = m_EulerAnglesF[3*neighbor];
			ea2 = m_EulerAnglesF[3*neighbor+1];
			ea3 = m_EulerAnglesF[3*neighbor+2];
		    OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
			float neighsurfarea = neighborsurfacearealist[selectedgrain2][j];
			if (neighbor != selectedgrain1)
			{
			  MC_LoopBody1(selectedgrain2, phase, j, neighsurfarea, m->crystruct[phase], q1, q2);
			}
		  }

		  deltaerror = (odfchange/currentodferror) + (mdfchange/currentmdferror);
		  if (deltaerror > 0)
		  {
			badtrycount = 0;
			m_EulerAnglesF[3*selectedgrain1] = g2ea1;
			m_EulerAnglesF[3*selectedgrain1+1] = g2ea2;
			m_EulerAnglesF[3*selectedgrain1+2] = g2ea3;
			m_EulerAnglesF[3*selectedgrain2] = g1ea1;
			m_EulerAnglesF[3*selectedgrain2+1] = g1ea2;
			m_EulerAnglesF[3*selectedgrain2+2] = g1ea3;
			simodf[phase][g1odfbin] = simodf[phase][g1odfbin] + (float(m_NumCells[selectedgrain2]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase])
				- (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]);
			simodf[phase][g2odfbin] = simodf[phase][g2odfbin] + (float(m_NumCells[selectedgrain1]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase])
				- (float(m_NumCells[selectedgrain2]) * m->getXRes() * m->getYRes() * m->getZRes() / unbiasedvol[phase]);

		    OrientationMath::eulertoQuat(q1, g2ea1, g2ea2, g2ea3);
			m_AvgQuats[5*selectedgrain1+1] = q1[1];
			m_AvgQuats[5*selectedgrain1+2] = q1[2];
			m_AvgQuats[5*selectedgrain1+3] = q1[3];
			m_AvgQuats[5*selectedgrain1+4] = q1[4];
			size = 0;
			if (neighborlist[selectedgrain1].size() != 0) size = neighborlist[selectedgrain1].size();
			for (size_t j = 0; j < size; j++)
			{
			  size_t neighbor = neighborlist[selectedgrain1][j];
			  ea1 = m_EulerAnglesF[3*neighbor];
			  ea2 = m_EulerAnglesF[3*neighbor+1];
			  ea3 = m_EulerAnglesF[3*neighbor+2];
		      OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
			  float neighsurfarea = neighborsurfacearealist[selectedgrain1][j];
			  if (neighbor != selectedgrain2)
			  {
				MC_LoopBody2(selectedgrain1, phase, j, neighsurfarea, m->crystruct[phase], q1, q2);
			  }
			}

		    OrientationMath::eulertoQuat(q1, g1ea1, g1ea2, g1ea3);
			m_AvgQuats[5*selectedgrain2+1] = q1[1];
			m_AvgQuats[5*selectedgrain2+2] = q1[2];
			m_AvgQuats[5*selectedgrain2+3] = q1[3];
			m_AvgQuats[5*selectedgrain2+4] = q1[4];
			size = 0;
			if (neighborlist[selectedgrain2].size() != 0) size = neighborlist[selectedgrain2].size();
			for (size_t j = 0; j < size; j++)
			{
			  size_t neighbor = neighborlist[selectedgrain2][j];
			  ea1 = m_EulerAnglesF[3*neighbor];
			  ea2 = m_EulerAnglesF[3*neighbor+1];
			  ea3 = m_EulerAnglesF[3*neighbor+2];
		      OrientationMath::eulertoQuat(q2, ea1, ea2, ea3);
			  float neighsurfarea = neighborsurfacearealist[selectedgrain2][j];
			  if (neighbor != selectedgrain1)
			  {
				MC_LoopBody2(selectedgrain2, phase, j, neighsurfarea, m->crystruct[phase], q1, q2);
			  }
			}
		  }
      }
    }
  }
  for(int i = 0; i < totalPoints; i++)
  {
	m_EulerAnglesC[3*i] = m_EulerAnglesF[3*m_GrainIds[i]];
    m_EulerAnglesC[3*i + 1] = m_EulerAnglesF[3*m_GrainIds[i]+1];
    m_EulerAnglesC[3*i + 2] = m_EulerAnglesF[3*m_GrainIds[i]+2];
  }
}

void  MatchCrystallography::measure_misorientations ()
{
  DataContainer* m = getDataContainer();
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;
  NeighborList<float>& neighborsurfacearealist = *m_SharedSurfaceAreaList;


  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  float q1[5];
  float q2[5];
  Ebsd::CrystalStructure phase1, phase2;
  int mbin;

  misorientationlists.resize(m->getTotalFields());
  for (size_t i = 1; i < m->getTotalFields(); i++)
  {
    if(misorientationlists[i].size() != 0)
    {
      misorientationlists[i].clear();
    }
    if(neighborlist[i].size() != 0)
    {
      misorientationlists[i].resize(neighborlist[i].size() * 3, 0.0);
    }

    q1[1] = m_AvgQuats[5*i+1];
    q1[2] = m_AvgQuats[5*i+2];
    q1[3] = m_AvgQuats[5*i+3];
    q1[4] = m_AvgQuats[5*i+4];
    phase1 = m->crystruct[m_PhasesF[i]];
    size_t size = 0;
    if(neighborlist[i].size() != 0 && neighborsurfacearealist[i].size() != 0 && neighborsurfacearealist[i].size() == neighborlist[i].size())
    {
      size = neighborlist[i].size();
    }

    for (size_t j = 0; j < size; j++)
    {
      w = 10000.0;
      int nname = neighborlist[i][j];
      float neighsurfarea = neighborsurfacearealist[i][j];
      q2[1] = m_AvgQuats[5*nname+1];
      q2[2] = m_AvgQuats[5*nname+2];
      q2[3] = m_AvgQuats[5*nname+3];
      q2[4] = m_AvgQuats[5*nname+4];
      phase2 = m->crystruct[m_PhasesF[nname]];
      if(phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
      OrientationMath::axisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
      if(phase1 == phase2)
      {
        misorientationlists[i][3 * j] = r1;
        misorientationlists[i][3 * j + 1] = r2;
        misorientationlists[i][3 * j + 2] = r3;
      }
      if(phase1 != phase2)
      {
        misorientationlists[i][3 * j] = -100;
        misorientationlists[i][3 * j + 1] = -100;
        misorientationlists[i][3 * j + 2] = -100;
      }
      if(phase1 == phase2)
      {
        mbin = m_OrientationOps[phase1]->getMisoBin(misorientationlists[i][3 * j], misorientationlists[i][3 * j + 1], misorientationlists[i][3 * j + 2]);
      }

      if(m_SurfaceFields[i] == false && (nname > i || m_SurfaceFields[nname] == true) && phase1 == phase2)
      {
        simmdf[m_PhasesF[i]][mbin] = simmdf[m_PhasesF[i]][mbin] + (neighsurfarea / m_TotalSurfaceArea[m_PhasesF[i]]);
      }
    }
  }
}
