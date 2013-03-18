/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include "H5CtfVolumeReader.h"

#include <cmath>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/Utilities/StringUtils.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/HKL/H5CtfReader.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfVolumeReader::H5CtfVolumeReader() :
H5EbsdVolumeReader()
{
  m_Phase = NULL;
  m_X = NULL;
  m_Y = NULL;
  m_Bands = NULL;
  m_Error = NULL;
  m_Euler1 = NULL;
  m_Euler2 = NULL;
  m_Euler3 = NULL;
  m_MAD = NULL;
  m_BC = NULL;
  m_BS = NULL;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfVolumeReader::~H5CtfVolumeReader()
{
  deletePointers();
}


#define H5CTFREADER_ALLOCATE_ARRAY(name, type)\
  if (readAllArrays == true || arrayNames.find(Ebsd::Ctf::name) != arrayNames.end()) {\
  type* _##name = allocateArray<type>(numElements);\
  if (NULL != _##name) {\
  ::memset(_##name, 0, numBytes);\
  }\
  set##name##Pointer(_##name);\
 }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfVolumeReader::initPointers(size_t numElements)
{
  setNumberOfElements(numElements);
  size_t numBytes = numElements * sizeof(float);
  bool readAllArrays = getReadAllArrays();
  std::set<std::string> arrayNames = getArraysToRead();

  H5CTFREADER_ALLOCATE_ARRAY(Phase, int)
  H5CTFREADER_ALLOCATE_ARRAY(X, float)
  H5CTFREADER_ALLOCATE_ARRAY(Y, float)
  H5CTFREADER_ALLOCATE_ARRAY(Z, float)
  H5CTFREADER_ALLOCATE_ARRAY(Bands, int)
  H5CTFREADER_ALLOCATE_ARRAY(Error, int)
  H5CTFREADER_ALLOCATE_ARRAY(Euler1, float)
  H5CTFREADER_ALLOCATE_ARRAY(Euler2, float)
  H5CTFREADER_ALLOCATE_ARRAY(Euler3, float)
  H5CTFREADER_ALLOCATE_ARRAY(MAD, float)
  H5CTFREADER_ALLOCATE_ARRAY(BC, int)
  H5CTFREADER_ALLOCATE_ARRAY(BS, int)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfVolumeReader::deletePointers()
{
  this->deallocateArrayData<int > (m_Phase);
  this->deallocateArrayData<float > (m_X);
  this->deallocateArrayData<float > (m_Y);
  this->deallocateArrayData<int > (m_Bands);
  this->deallocateArrayData<int > (m_Error);
  this->deallocateArrayData<float > (m_Euler1);
  this->deallocateArrayData<float > (m_Euler2);
  this->deallocateArrayData<float > (m_Euler3);
  this->deallocateArrayData<float > (m_MAD);
  this->deallocateArrayData<int > (m_BC);
  this->deallocateArrayData<int > (m_BS);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* H5CtfVolumeReader::getPointerByName(const std::string &fieldName)
{
  if (fieldName.compare(Ebsd::Ctf::Phase) == 0) { return static_cast<void*>(m_Phase);}
  if (fieldName.compare(Ebsd::Ctf::X) == 0) { return static_cast<void*>(m_X);}
  if (fieldName.compare(Ebsd::Ctf::Y) == 0) { return static_cast<void*>(m_Y);}
  if (fieldName.compare(Ebsd::Ctf::Bands) == 0) { return static_cast<void*>(m_Bands);}
  if (fieldName.compare(Ebsd::Ctf::Error) == 0) { return static_cast<void*>(m_Error);}
  if (fieldName.compare(Ebsd::Ctf::Euler1) == 0) { return static_cast<void*>(m_Euler1);}
  if (fieldName.compare(Ebsd::Ctf::Euler2) == 0) { return static_cast<void*>(m_Euler2);}
  if (fieldName.compare(Ebsd::Ctf::Euler3) == 0) { return static_cast<void*>(m_Euler3);}
  if (fieldName.compare(Ebsd::Ctf::MAD) == 0) { return static_cast<void*>(m_MAD);}
  if (fieldName.compare(Ebsd::Ctf::BC) == 0) { return static_cast<void*>(m_BC);}
  if (fieldName.compare(Ebsd::Ctf::BS) == 0) { return static_cast<void*>(m_BS);}
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType H5CtfVolumeReader::getPointerType(const std::string &fieldName)
  {
  if (fieldName.compare(Ebsd::Ctf::Phase) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::X) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Y) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Bands) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::Error) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::Euler1) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Euler2) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::Euler3) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::MAD) == 0) { return Ebsd::Float;}
  if (fieldName.compare(Ebsd::Ctf::BC) == 0) { return Ebsd::Int32;}
  if (fieldName.compare(Ebsd::Ctf::BS) == 0) { return Ebsd::Int32;}
  return Ebsd::UnknownNumType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<CtfPhase::Pointer> H5CtfVolumeReader::getPhases()
{
  m_Phases.clear();

  // Get the first valid index of a z slice
  std::string index = StringUtils::numToString(getZStart());

  // Open the hdf5 file and read the data
  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    std::cout << "Error" << std::endl;
    return m_Phases;
  }
  herr_t err = 0;

  hid_t gid = H5Gopen(fileId, index.c_str(), H5P_DEFAULT);
  H5CtfReader::Pointer reader = H5CtfReader::New();
  reader->setHDF5Path(index);
  err = reader->readHeader(gid);
  if (err < 0)
  {
    std::cout  << "Error reading the header information from the .h5ebsd file" << std::endl;
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
int H5CtfVolumeReader::loadData(int64_t xpoints,
                                int64_t ypoints,
                                int64_t zpoints,
                                Ebsd::RefFrameZDir ZDir)
{
  int index = 0;
  int err = -1;
// Initialize all the pointers
  initPointers(xpoints * ypoints * zpoints);


  int readerIndex;
  int64_t xpointsslice;
  int64_t ypointsslice;
  int64_t xpointstemp;
  int64_t ypointstemp;
//  int xstop;
//  int ystop;
  int zval;


  int xstartspot;
  int ystartspot;

  err = readVolumeInfo();

  for (int slice = 0; slice < zpoints; ++slice)
  {
    H5CtfReader::Pointer reader = H5CtfReader::New();
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
    if (err < 0)
    {
      std::cout << "H5CtfVolumeReader Error: There was an issue loading the data from the hdf5 file." << std::endl;
      return -1;
    }
    readerIndex = 0;
    xpointsslice = reader->getXCells();
    ypointsslice = reader->getYCells();
    int* phasePtr = reader->getPhasePointer();
    float* xPtr = reader->getXPointer();
    float* yPtr = reader->getYPointer();
    int* bandPtr = reader->getBandCountPointer();
    int* errorPtr = reader->getErrorPointer();
    float* euler1Ptr = reader->getEuler1Pointer();
    float* euler2Ptr = reader->getEuler2Pointer();
    float* euler3Ptr = reader->getEuler3Pointer();
    float* madPtr = reader->getMeanAngularDeviationPointer();
    int* bcPtr = reader->getBandContrastPointer();
    int* bsPtr = reader->getBandSlopePointer();

    xpointstemp = xpoints;
    ypointstemp = ypoints;
    xstartspot = static_cast<int>( (xpointstemp - xpointsslice) / 2 );
    ystartspot = static_cast<int>( (ypointstemp - ypointsslice) / 2 );

    // If no stacking order preference was passed, read it from the file and use that value
    if(ZDir == Ebsd::UnknownRefFrameZDirection)
    {
      ZDir = getStackingOrder();
    }
    if (ZDir == 0) zval = slice;
    if (ZDir == 1) zval = static_cast<int>( (zpoints - 1) - slice );

    // Copy the data from the current storage into the Storage Location
    for (int j = 0; j < ypointsslice; j++)
    {
      for (int i = 0; i < xpointsslice; i++)
      {
        index = static_cast<int>( (zval * xpointstemp * ypointstemp) + ((j + ystartspot) * xpointstemp) + (i + xstartspot) );
        if (NULL != phasePtr) {m_Phase[index] = phasePtr[readerIndex];} // Phase Add 1 to the phase number because .ctf files are zero based for phases
        if (NULL != xPtr) {m_X[index] = xPtr[readerIndex];}
        if (NULL != yPtr) {m_Y[index] = yPtr[readerIndex];}
        if (NULL != bandPtr) {m_Bands[index] = bandPtr[readerIndex];}
        if (NULL != errorPtr) {m_Error[index] = errorPtr[readerIndex];}
        if (NULL != euler1Ptr) {m_Euler1[index] = euler1Ptr[readerIndex];}
        if (NULL != euler2Ptr) {m_Euler2[index] = euler2Ptr[readerIndex];}
        if (NULL != euler3Ptr) {m_Euler3[index] = euler3Ptr[readerIndex];}
        if (NULL != madPtr) {m_MAD[index] = madPtr[readerIndex];}
        if (NULL != bcPtr) {m_BC[index] = bcPtr[readerIndex];}
        if (NULL != bsPtr) {m_BS[index] = bsPtr[readerIndex];}

        /* For HKL OIM Files if there is a single phase then the value of the phase
         * data is one (1). If there are 2 or more phases then the lowest value
         * of phase is also one (1). However, if there are "zero solutions" in the data
         * then those points are assigned a phase of zero.  Since those points can be identified
         * by other methods, the phase of these points should be changed to one since in the rest
         * of the reconstruction code we follow theconvention that the lowest value is One (1)
         * even if there is only a single phase. The next if statement converts all zeros to ones
         * if there is a single phase in the OIM data.
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

