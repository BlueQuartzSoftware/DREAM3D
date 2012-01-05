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

#include <limits>
#include "LoadSlices.h"


#include "EbsdLib/H5EbsdVolumeInfo.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/AngDirectoryPatterns.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/TSL/AngPhase.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = M_PI;



#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadSlices::LoadSlices()
{
  Seed = MXA::getMilliSeconds();

  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadSlices::~LoadSlices()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadSlices::execute()
{

  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
	int err = 0;
  setErrorCondition(err);
  std::string manufacturer;
  // Get the Size and Resolution of the Volume
  {
    H5EbsdVolumeInfo::Pointer volumeInfoReader = H5EbsdVolumeInfo::New();
    volumeInfoReader->setFileName(m_H5AngFile);
    err = volumeInfoReader->readVolumeInfo();
    setErrorCondition(err);
    int64_t dims[3];
    float res[3];
    volumeInfoReader->getDimsAndResolution(dims[0], dims[1], dims[2], res[0], res[1], res[2]);
    /* Sanity check what we are trying to load to make sure it can fit in our address space.
     * Note that this does not guarantee the user has enough left, just that the
     * size of the volume can fit in the address space of the program
     */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
    if (dims[0] * dims[1] * dims[2] > max )
    {
      err = -1;
      std::stringstream s;
      s << "The total number of elements '" << (dims[0] * dims[1] * dims[2])
                  << "' is greater than this program can hold. Try the 64 bit version.";
      setErrorCondition(err);
      setErrorMessage(s.str());
      return;
    }

    if (dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      std::stringstream s;
      s << "One of the dimensions is greater than the max index for this sysem. Try the 64 bit version.";
      s << " dim[0]="<< dims[0] << "  dim[1]="<<dims[1] << "  dim[2]=" << dims[2];
      setErrorCondition(err);
      setErrorMessage(s.str());
      return;
    }
    /* ************ End Sanity Check *************************** */
    size_t dcDims[3] = {dims[0], dims[1], dims[2]};
    m->setDimensions(dcDims);
    m->setResolution(res);
    //Now Calculate our "subvolume" of slices, ie, those start and end values that the user selected from the GUI
    // The GUI code has already added 1 to the end index so nothing special needs to be done
    // for this calculation
    dcDims[2] = m_ZEndIndex - m_ZStartIndex + 1;
    m->setDimensions(dcDims);
    manufacturer = volumeInfoReader->getManufacturer();
    volumeInfoReader = H5EbsdVolumeInfo::NullPointer();
  }
  H5EbsdVolumeReader::Pointer ebsdReader;
  std::vector<float> precipFractions;
  std::vector<Ebsd::CrystalStructure> crystalStructures;
  if (manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
  {
    ebsdReader = H5AngVolumeReader::New();
    if (NULL == ebsdReader)
    {
      setErrorCondition(-1);
      setErrorMessage("Could not Create H5AngVolumeReader object.");
      return;
    }
    H5AngVolumeReader* angReader = dynamic_cast<H5AngVolumeReader*>(ebsdReader.get());
    err = loadInfo<H5AngVolumeReader, AngPhase>(angReader, precipFractions, crystalStructures );
  }
  else if (manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
    ebsdReader = H5CtfVolumeReader::New();
    if (NULL == ebsdReader)
    {
      setErrorCondition(-1);
      setErrorMessage("Could not Create H5CtfVolumeReader object.");
      return;
    }
    H5CtfVolumeReader* ctfReader = dynamic_cast<H5CtfVolumeReader*>(ebsdReader.get());
    err = loadInfo<H5CtfVolumeReader, CtfPhase>(ctfReader, precipFractions, crystalStructures );
  }
  else
  {
    setErrorCondition(-1);
    std::string msg("Could not determine or match a supported manufacturer from the data file.");
    msg = msg.append("Supported manufacturer codes are: ").append(Ebsd::Ctf::Manufacturer);
    msg = msg.append(" and ").append(Ebsd::Ang::Manufacturer);
    setErrorMessage(msg);
    return;
  }

  initialize(m->getXPoints(), m->getYPoints(), m->getZPoints(),
                m->getXRes(), m->getYRes(), m->getZRes(),
                crystalStructures, m_PhaseTypes, precipFractions);
  if (getErrorCondition() < 0)
  {
    return;
  }
  int64_t totalPoints = m->totalPoints();
    int32_t* phases = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, this);
  if (NULL == phases) { return; }
    float* euler1s = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler1, totalPoints, this);
  if (NULL == euler1s) { return; }
  float* euler2s = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler2, totalPoints, this);
  if (NULL == euler2s) { return; }
  float* euler3s = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler3, totalPoints, this);
  if (NULL == euler3s) { return; }
  bool* goodVoxels = m->getVoxelDataSizeCheck<bool, BoolArrayType, AbstractFilter>(DREAM3D::VoxelData::GoodVoxels, totalPoints, this);
  if (NULL == goodVoxels) { return; }


  // During the loading of the EBSD data the Quality Metric Filters will be run
  // and fill in the ReconstrucionFunc->goodVoxels array.
  ebsdReader->setSliceStart(m_ZStartIndex);
  ebsdReader->setSliceEnd(m_ZEndIndex);
  err = ebsdReader->loadData(euler1s, euler2s, euler3s, phases, goodVoxels, m->getXPoints(), m->getYPoints(), m->getZPoints(), m_RefFrameZDir, m_QualityMetricFilters);
  setErrorCondition(err);
  if (err < 0)
  {
    setErrorMessage("Error Loading Data from Ebsd Data file.");
    return;
  }
  float radianconversion = M_PI/180.0;
  if (manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
	  for(size_t i = 0; i < (m->getXPoints()*m->getYPoints()*m->getZPoints()); i++)
	  {
		  euler1s[i] = euler1s[i] * radianconversion;
		  euler2s[i] = euler2s[i] * radianconversion;
		  euler3s[i] = euler3s[i] * radianconversion;
	  }
  }

  initializeQuats();

  threshold_points();

  // If there is an error set this to something negative and also set a message
  notify("LoadSlices Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadSlices::initialize(size_t nX, size_t nY, size_t nZ, float xRes, float yRes, float zRes,
                                    std::vector<Ebsd::CrystalStructure> crystalStructures,
									std::vector<DREAM3D::Reconstruction::PhaseType> phaseTypes,
                                    std::vector<float> precipFractions)
{
  notify("Initializing Variables", 0, Observable::UpdateProgressValueAndMessage);
  DataContainer* m = getDataContainer();
  m->crystruct = crystalStructures;
  m->phaseType = m_PhaseTypes;
  m->pptFractions = precipFractions;

  m->setDimensions(nX, nY, nZ);
  m->setResolution(xRes, yRes, zRes);

  int64_t totalPoints = m->totalPoints();
  int numgrains = 100;
  size_t oldSize = 0;
  m->m_Grains.resize(numgrains + 1);
  for(size_t g = oldSize; g < m->m_Grains.size(); ++g)
  {
    m->m_Grains[g] = Field::New();
  }

#if 1
  /* ********** This is the proposed way to add arrays to the DataContainer */

  // First check to see if the array already exists in the Data Container. If
  // it does NOT exist then we would get a NULL pointer back.
  IDataArray::Pointer smart_pointer = m->getVoxelData("Grain Indices");
  if (smart_pointer.get() == NULL) {
    IDataArray::Pointer grainIndices = Int32ArrayType::CreateArray(totalPoints);
    m->addVoxelData("Grain Indices", grainIndices);
  }

  // Now later on we can get at that array by casting the IDataArray to the proper type
  Int32ArrayType* grainIndices =
      Int32ArrayType::SafeObjectDownCast<IDataArray*, Int32ArrayType* >(m->getVoxelData("Grain Indices").get());
  // If the casting fails, because we got the wrong type then the resulting pointer will be null
  if (NULL == grainIndices)
  {
    std::cout << "Something went wrong pulling the Grain Indices array from the Data Container. Was it created?" << std::endl;
  }

  // We can even get the raw pointer to the data if we wanted to;
  int32_t* grain_indices = grainIndices->GetPointer(0);
  for(int i =0; i < totalPoints; ++i)
  {
    grain_indices[i] = i; //Note that there is NO bounds checking. This is the raw pointer
    // but will point back to the array that is store in the DataContainer
  }
  /* ************************** End of Example *****************************/
#endif

  int32_t* grain_indicies = m->createVoxelData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, 1, this);
  if (grain_indicies == NULL) { return; }
  int32_t* phases = m->createVoxelData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, 1, this);
  if (phases == NULL) { return; }
  float* euler1s = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler1, totalPoints, 1, this);
  if (NULL == euler1s) {return;}
  float* euler2s = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler2, totalPoints, 1, this);
  if (NULL == euler2s) {return;}
  float* euler3s = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler3, totalPoints, 1, this);
  if (NULL == euler3s) {return;}
  int8_t* surfacevoxels = m->createVoxelData<int8_t, Int8ArrayType, AbstractFilter>(DREAM3D::VoxelData::SurfaceVoxels, totalPoints, 1, this);
  if (NULL == surfacevoxels) {return;}
  int32_t* neighbors = m->createVoxelData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Neighbors, totalPoints, 1, this);
  if (neighbors == NULL) { return; }
  float* quats = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Quats, totalPoints*5, 1, this);
  if (NULL == quats) {return;}
  bool* alreadychecked = m->createVoxelData<bool, BoolArrayType, AbstractFilter>(DREAM3D::VoxelData::AlreadyChecked, totalPoints, 1, this);
  if (NULL == alreadychecked) {return;}
  bool* goodVoxels = m->createVoxelData<bool, BoolArrayType, AbstractFilter>(DREAM3D::VoxelData::GoodVoxels, totalPoints, 1, this);
  if (NULL == goodVoxels) {return;}

  for(int i = 0;i < totalPoints;i++)
  {
    grain_indicies[i] = -1;
    phases[i] = 1;
    euler1s[i] = -1;
    euler2s[i] = -1;
    euler3s[i] = -1;
    neighbors[i] = -1;
    surfacevoxels[i] = 0;
    alreadychecked[i] = false;
    goodVoxels[i] = false; // All Voxels are "Bad"
  }

}

void LoadSlices::initializeQuats()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

    int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == grain_indicies) { return; }
    int32_t* phases = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, this);
  if (NULL == phases) { return; }
    float* euler1s = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler1, totalPoints, this);
  if (NULL == euler1s) { return; }
  float* euler2s = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler2, totalPoints, this);
  if (NULL == euler2s) { return; }
  float* euler3s = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Euler3, totalPoints, this);
  if (NULL == euler3s) { return; }
  float* quats = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Quats, (totalPoints*5), this);
  if (NULL == quats) { return; }

  float qr[5];
  Ebsd::CrystalStructure xtal = Ebsd::UnknownCrystalStructure;
  int phase = -1;
  for (int i = 0; i < totalPoints; i++)
  {
    OrientationMath::eulertoQuat(qr, euler1s[i], euler2s[i], euler3s[i]);
    phase = phases[i];
    xtal = m->crystruct[phase];
    if (xtal == Ebsd::UnknownCrystalStructure)
    {
      qr[1] = 0.0;
      qr[2] = 0.0;
      qr[3] = 0.0;
      qr[4] = 1.0;
    }
    else
    {
      m_OrientationOps[xtal]->getFZQuat(qr);
    }

    quats[i*5 + 0] = 1.0f;
    quats[i*5 + 1] = qr[1];
    quats[i*5 + 2] = qr[2];
    quats[i*5 + 3] = qr[3];
    quats[i*5 + 4] = qr[4];
  }
}
void LoadSlices::threshold_points()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();
    int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if (NULL == grain_indicies) { return; }
    int32_t* phases = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, this);
  if (NULL == phases) { return; }
  bool* goodVoxels = m->getVoxelDataSizeCheck<bool, BoolArrayType, AbstractFilter>(DREAM3D::VoxelData::GoodVoxels, totalPoints, this);
  if (NULL == goodVoxels) { return; }
    bool* alreadychecked = m->getVoxelDataSizeCheck<bool, BoolArrayType, AbstractFilter>(DREAM3D::VoxelData::AlreadyChecked, totalPoints, this);
  if (NULL == alreadychecked) { return; }
  float* quats = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Quats, (totalPoints*5), this);
  if (NULL == quats) { return; }
  int32_t* neighbors = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Neighbors, totalPoints, this);
  if (NULL == neighbors) { return; }


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
    static_cast<DimType>(udims[0]),
  };

  int neighpoints[6];
  neighpoints[0] = -dims[0]*dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0]*dims[1];

  float w, n1, n2, n3;
  float q1[5];
  float q2[5];
//  int index;
  int good = 0;
  size_t count = 0;
  int currentpoint = 0;
  int neighbor = 0;
  size_t col, row, plane;

//  int noborder = 0;
  Ebsd::CrystalStructure phase1, phase2;
  int initialVoxelsListSize = 10000;
  std::vector<int> voxelslist(initialVoxelsListSize, -1);

  for (int iter = 0; iter < totalPoints; iter++)
  {
    alreadychecked[iter] = false;
    if(goodVoxels[iter] == 0) grain_indicies[iter] = 0;
    if(goodVoxels[iter] == 1 && phases[iter] > 0)
    {
      grain_indicies[iter] = -1;
      voxelslist[count] = iter;
      alreadychecked[iter] = true;
      count++;
      if(count >= voxelslist.size()) voxelslist.resize(count + initialVoxelsListSize, -1);
    }
  }
  for (size_t j = 0; j < count; j++)
  {
	currentpoint = voxelslist[j];
	col = currentpoint % m->getXPoints();
	row = (currentpoint / m->getXPoints()) % m->getYPoints();
	plane = currentpoint / (m->getXPoints() * m->getYPoints());
	q1[0] = 0;
	q1[1] = quats[currentpoint*5 + 1];
	q1[2] = quats[currentpoint*5 + 2];
	q1[3] = quats[currentpoint*5 + 3];
	q1[4] = quats[currentpoint*5 + 4];
	phase1 = m->crystruct[phases[currentpoint]];
	for (size_t i = 0; i < 6; i++)
	{
	  good = 1;
	  neighbor = currentpoint + neighbors[i];
	  if (i == 0 && plane == 0) good = 0;
	  if (i == 5 && plane == (dims[2] - 1)) good = 0;
	  if (i == 1 && row == 0) good = 0;
	  if (i == 4 && row == (dims[1] - 1)) good = 0;
	  if (i == 2 && col == 0) good = 0;
	  if (i == 3 && col == (dims[0] - 1)) good = 0;
	  if (good == 1 && grain_indicies[neighbor] == 0 && phases[neighbor] > 0)
	  {
		w = 10000.0;
		q2[0] = 0;
		q2[1] = quats[neighbor*5 + 1];
		q2[2] = quats[neighbor*5 + 2];
		q2[3] = quats[neighbor*5 + 3];
		q2[4] = quats[neighbor*5 + 4];
		phase2 = m->crystruct[phases[neighbor]];
		if (phase1 == phase2)
		{
		  w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
		}
		if (w < m_MisoTolerance)
		{
		  grain_indicies[neighbor] = -1;
		  alreadychecked[neighbor] = true;
		  voxelslist[count] = neighbor;
		  count++;
		  if (count >= voxelslist.size()) voxelslist.resize(count + initialVoxelsListSize, -1);
		}
	  }
	}
  }
  voxelslist.clear();
}

