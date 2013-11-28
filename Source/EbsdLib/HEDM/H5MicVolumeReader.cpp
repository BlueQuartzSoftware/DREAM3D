/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "H5MicVolumeReader.h"

#include <cmath>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/Utilities/StringUtils.h"
#include "EbsdLib/HEDM/H5MicReader.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5MicVolumeReader::H5MicVolumeReader() :
    H5EbsdVolumeReader()
{
  // Init all the arrays to NULL
  m_Euler1 = NULL;
  m_Euler2 = NULL;
  m_Euler3 = NULL;
  m_Conf = NULL;
  m_Phase = NULL;
  m_X = NULL;
  m_Y = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5MicVolumeReader::~H5MicVolumeReader()
{
  deletePointers();
}


#define H5MICREADER_ALLOCATE_ARRAY(name, type)\
  if (readAllArrays == true || arrayNames.find(Ebsd::Mic::name) != arrayNames.end()) {\
  type* _##name = allocateArray<type>(numElements);\
  if (NULL != _##name) {\
  ::memset(_##name, 0, numBytes);\
  }\
  set##name##Pointer(_##name);\
 }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5MicVolumeReader::initPointers(size_t numElements)
{
  setNumberOfElements(numElements);
  size_t numBytes = numElements * sizeof(float);
  bool readAllArrays = getReadAllArrays();
  std::set<std::string> arrayNames = getArraysToRead();


  H5MICREADER_ALLOCATE_ARRAY(Euler1, float)
  H5MICREADER_ALLOCATE_ARRAY(Euler2, float)
  H5MICREADER_ALLOCATE_ARRAY(Euler3, float)
  H5MICREADER_ALLOCATE_ARRAY(Confidence, float)
  H5MICREADER_ALLOCATE_ARRAY(Phase, int)
  H5MICREADER_ALLOCATE_ARRAY(X, float)
  H5MICREADER_ALLOCATE_ARRAY(Y, float)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5MicVolumeReader::deletePointers()
{
  this->deallocateArrayData<float > (m_Euler1);
  this->deallocateArrayData<float > (m_Euler2);
  this->deallocateArrayData<float > (m_Euler3);
  this->deallocateArrayData<float > (m_Conf);
  this->deallocateArrayData<int > (m_Phase);
  this->deallocateArrayData<float > (m_X);
  this->deallocateArrayData<float > (m_Y);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* H5MicVolumeReader::getPointerByName(const std::string &fieldName)
{
  if (fieldName.compare(Ebsd::Mic::Euler1) == 0) { return static_cast<void*>(m_Euler1);}
  if (fieldName.compare(Ebsd::Mic::Euler2) == 0) { return static_cast<void*>(m_Euler2);}
  if (fieldName.compare(Ebsd::Mic::Euler3) == 0) { return static_cast<void*>(m_Euler3);}
  if (fieldName.compare(Ebsd::Mic::Confidence) == 0) { return static_cast<void*>(m_Conf);}
  if (fieldName.compare(Ebsd::Mic::Phase) == 0) { return static_cast<void*>(m_Phase);}
  if (fieldName.compare(Ebsd::Mic::X) == 0) { return static_cast<void*>(m_X);}
  if (fieldName.compare(Ebsd::Mic::Y) == 0) { return static_cast<void*>(m_Y);}
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType H5MicVolumeReader::getPointerType(const std::string &fieldName)
{
  if (fieldName.compare(Ebsd::Mic::Euler1) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Euler2) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Euler3) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Confidence) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Phase) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Mic::X) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Mic::Y) == 0) { return Ebsd::Float;}
  return Ebsd::UnknownNumType;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<MicPhase::Pointer> H5MicVolumeReader::getPhases()
{
  m_Phases.clear();

  // Get the first valid index of a z slice
  std::string index = StringUtils::numToString(getZStart());

  // Open the hdf5 file and read the data
  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if(fileId < 0)
  {
    std::cout << "Error: Could not open .h5ebsd file for reading." << std::endl;
    return m_Phases;
  }
  herr_t err = 0;

  hid_t gid = H5Gopen(fileId, index.c_str(), H5P_DEFAULT);
  H5MicReader::Pointer reader = H5MicReader::New();
  reader->setHDF5Path(index);
  err = reader->readHeader(gid);
  if(err < 0)
  {
    std::cout << "Error reading the header information from the .h5ebsd file" << std::endl;
    err = H5Gclose(gid);
    err = H5Utilities::closeFile(fileId);
    return m_Phases;
  }
  m_Phases = reader->getPhases();
  err = H5Gclose(gid);
  err = H5Utilities::closeFile(fileId);
  return m_Phases;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicVolumeReader::loadData(int64_t xpoints,
                                int64_t ypoints,
                                int64_t zpoints,
                                Ebsd::RefFrameZDir ZDir )
{
  int index = 0;
  int err = -1;
  // Initialize all the pointers
  initPointers(xpoints * ypoints * zpoints);

  int readerIndex;
  int xpointsslice;
  int ypointsslice;
  int xpointstemp;
  int ypointstemp;
  int xstop;
  int ystop;
  int zval = 0;

  int xstartspot;
  int ystartspot;

  err = readVolumeInfo();
  for (int slice = 0; slice < zpoints; ++slice)
  {
    H5MicReader::Pointer reader = H5MicReader::New();
    reader->setFileName(getFileName());
    reader->setHDF5Path(StringUtils::numToString(slice + getSliceStart()));
    reader->setUserZDir(getStackingOrder());
    reader->setSampleTransformationAngle(getSampleTransformationAngle());
    reader->setSampleTransformationAxis(getSampleTransformationAxis());
    reader->setEulerTransformationAngle(getEulerTransformationAngle());
    reader->setEulerTransformationAxis(getEulerTransformationAxis());
    reader->readAllArrays(getReadAllArrays());
    reader->setArraysToRead(getArraysToRead());
    err = reader->readFile();
    if(err < 0)
    {
      std::cout << "H5MicDataLoader Error: There was an issue loading the data from the hdf5 file." << std::endl;
      return -1;
    }
    readerIndex = 0;
    xpointsslice = reader->getXDimension();
    ypointsslice = reader->getYDimension();
    float* euler1Ptr = reader->getEuler1Pointer();
    float* euler2Ptr = reader->getEuler2Pointer();
    float* euler3Ptr = reader->getEuler3Pointer();
    float* xPtr = reader->getXPointer();
    float* yPtr = reader->getYPointer();
    float* confPtr = reader->getConfidencePointer();
    int* phasePtr = reader->getPhasePointer();


    xpointstemp = static_cast<int>(xpoints);
    ypointstemp = static_cast<int>(ypoints);
    xstartspot = (xpointstemp - xpointsslice) / 2;
    ystartspot = (ypointstemp - ypointsslice) / 2;
    xstop = xpointsslice;
    ystop = ypointsslice;

    // If no stacking order preference was passed, read it from the file and use that value
    if(ZDir == Ebsd::UnknownRefFrameZDirection)
    {
      ZDir = getStackingOrder();
    }
	if (ZDir == Ebsd::LowtoHigh) { zval = slice; }
	else if (ZDir == Ebsd::HightoLow) { zval = static_cast<int>((zpoints - 1) - slice); }
	

    // Copy the data from the current storage into the ReconstructionFunc Storage Location
    for (int j = 0; j < ystop; j++)
    {
      for (int i = 0; i < xstop; i++)
      {
        index = (zval * xpointstemp * ypointstemp) + ((j + ystartspot) * xpointstemp) + (i + xstartspot);
        if (NULL != euler1Ptr) {m_Euler1[index] = euler1Ptr[readerIndex];}
        if (NULL != euler2Ptr) {m_Euler2[index] = euler2Ptr[readerIndex];}
        if (NULL != euler3Ptr) {m_Euler3[index] = euler3Ptr[readerIndex];}
        if (NULL != xPtr) {m_X[index] = xPtr[readerIndex];}
        if (NULL != yPtr) {m_Y[index] = yPtr[readerIndex];}
        if (NULL != confPtr) {m_Conf[index] = confPtr[readerIndex];}
        if (NULL != phasePtr) {m_Phase[index] = phasePtr[readerIndex];} // Phase

        /* For HEDM OIM Files if there is a single phase then the value of the phase
         * data is zero (0). If there are 2 or more phases then the lowest value
         * of phase is one (1). In the rest of the reconstruction code we follow the
         * convention that the lowest value is One (1) even if there is only a single
         * phase. The next if statement converts all zeros to ones if there is a single
         * phase in the OIM data.
         */
        if(NULL != phasePtr && m_Phase[index] < 1)
        {
          m_Phase[index] = 1;
        }

        ++readerIndex;
      }
    }
  }
  return err;
}

