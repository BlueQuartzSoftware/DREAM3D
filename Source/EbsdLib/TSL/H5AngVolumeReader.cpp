/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "H5AngVolumeReader.h"

#include <cmath>

#include <QtCore/QString>

#include "H5Support/H5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/H5AngReader.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngVolumeReader::H5AngVolumeReader() :
  H5EbsdVolumeReader()
{
  // Init all the arrays to NULL
  m_Phi1 = NULL;
  m_Phi = NULL;
  m_Phi2 = NULL;
  m_Iq = NULL;
  m_Ci = NULL;
  m_PhaseData = NULL;
  m_X = NULL;
  m_Y = NULL;
  m_SEMSignal = NULL;
  m_Fit = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngVolumeReader::~H5AngVolumeReader()
{
  deletePointers();
}


#define H5ANGREADER_ALLOCATE_ARRAY(name, type)\
  if (readAllArrays == true || arrayNames.find(Ebsd::Ang::name) != arrayNames.end()) {\
    type* _##name = allocateArray<type>(numElements);\
    if (NULL != _##name) {\
      ::memset(_##name, 0, numBytes);\
    }\
    set##name##Pointer(_##name);\
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngVolumeReader::initPointers(size_t numElements)
{
  setNumberOfElements(numElements);
  size_t numBytes = numElements * sizeof(float);
  bool readAllArrays = getReadAllArrays();
  QSet<QString> arrayNames = getArraysToRead();

  H5ANGREADER_ALLOCATE_ARRAY(Phi1, float)
  H5ANGREADER_ALLOCATE_ARRAY(Phi, float)
  H5ANGREADER_ALLOCATE_ARRAY(Phi2, float)
  H5ANGREADER_ALLOCATE_ARRAY(ImageQuality, float)
  H5ANGREADER_ALLOCATE_ARRAY(ConfidenceIndex, float)
  H5ANGREADER_ALLOCATE_ARRAY(PhaseData, int)
  H5ANGREADER_ALLOCATE_ARRAY(XPosition, float)
  H5ANGREADER_ALLOCATE_ARRAY(YPosition, float)
  H5ANGREADER_ALLOCATE_ARRAY(SEMSignal, float)
  H5ANGREADER_ALLOCATE_ARRAY(Fit, float)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngVolumeReader::deletePointers()
{
  this->deallocateArrayData<float > (m_Phi1);
  this->deallocateArrayData<float > (m_Phi);
  this->deallocateArrayData<float > (m_Phi2);
  this->deallocateArrayData<float > (m_Iq);
  this->deallocateArrayData<float > (m_Ci);
  this->deallocateArrayData<int > (m_PhaseData);
  this->deallocateArrayData<float > (m_X);
  this->deallocateArrayData<float > (m_Y);
  this->deallocateArrayData<float > (m_SEMSignal);
  this->deallocateArrayData<float > (m_Fit);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* H5AngVolumeReader::getPointerByName(const QString& featureName)
{
  if (featureName.compare(Ebsd::Ang::Phi1) == 0) { return static_cast<void*>(m_Phi1);}
  if (featureName.compare(Ebsd::Ang::Phi) == 0) { return static_cast<void*>(m_Phi);}
  if (featureName.compare(Ebsd::Ang::Phi2) == 0) { return static_cast<void*>(m_Phi2);}
  if (featureName.compare(Ebsd::Ang::ImageQuality) == 0) { return static_cast<void*>(m_Iq);}
  if (featureName.compare(Ebsd::Ang::ConfidenceIndex) == 0) { return static_cast<void*>(m_Ci);}
  if (featureName.compare(Ebsd::Ang::PhaseData) == 0) { return static_cast<void*>(m_PhaseData);}
  if (featureName.compare(Ebsd::Ang::XPosition) == 0) { return static_cast<void*>(m_X);}
  if (featureName.compare(Ebsd::Ang::YPosition) == 0) { return static_cast<void*>(m_Y);}
  if (featureName.compare(Ebsd::Ang::SEMSignal) == 0) { return static_cast<void*>(m_SEMSignal);}
  if (featureName.compare(Ebsd::Ang::Fit) == 0) { return static_cast<void*>(m_Fit);}
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType H5AngVolumeReader::getPointerType(const QString& featureName)
{
  if (featureName.compare(Ebsd::Ang::Phi1) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ang::Phi) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ang::Phi2) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ang::ImageQuality) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ang::ConfidenceIndex) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ang::PhaseData) == 0) { return Ebsd::Int32;}
  if (featureName.compare(Ebsd::Ang::XPosition) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ang::YPosition) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ang::SEMSignal) == 0) { return Ebsd::Float;}
  if (featureName.compare(Ebsd::Ang::Fit) == 0) { return Ebsd::Float;}
  return Ebsd::UnknownNumType;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AngPhase::Pointer> H5AngVolumeReader::getPhases()
{
  m_Phases.clear();

  // Get the first valid index of a z slice
  QString index = QString::number(getZStart());

  // Open the hdf5 file and read the data
  hid_t fileId = QH5Utilities::openFile(getFileName(), true);
  if(fileId < 0)
  {
    setErrorMessage("Error: Could not open .h5ebsd file for reading.");
    setErrorCode(-90000);
    return m_Phases;
  }
  herr_t err = 0;

  hid_t gid = H5Gopen(fileId, index.toLatin1().data(), H5P_DEFAULT);
  H5AngReader::Pointer reader = H5AngReader::New();
  reader->setHDF5Path(index);
  err = reader->readHeader(gid);
  if(err < 0)
  {
    setErrorMessage(reader->getErrorMessage());
    setErrorCode(reader->getErrorCode());
    err = H5Gclose(gid);
    err = QH5Utilities::closeFile(fileId);
    return m_Phases;
  }
  m_Phases = reader->getPhases();
  err = H5Gclose(gid);
  err = QH5Utilities::closeFile(fileId);
  return m_Phases;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngVolumeReader::loadData(int64_t xpoints,
                                int64_t ypoints,
                                int64_t zpoints,
                                uint32_t ZDir )
{
  int index = 0;
  int err = -1;
  // Initialize all the pointers
  initPointers(xpoints * ypoints * zpoints);


  int readerIndex = 0;
  int xpointsslice = 0;
  int ypointsslice = 0;
  int xpointstemp = 0;
  int ypointstemp = 0;
  int xstop = 0;
  int ystop = 0;
  int zval = 0;

  int xstartspot = 0;
  int ystartspot = 0;
  int numPhases = getNumPhases();
  err = readVolumeInfo();
  for (int slice = 0; slice < zpoints; ++slice)
  {
    H5AngReader::Pointer reader = H5AngReader::New();
    reader->setFileName(getFileName());
    reader->setHDF5Path(QString::number(slice + getSliceStart()));
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
      setErrorCode(reader->getErrorCode());
      setErrorMessage(reader->getErrorMessage());
      return getErrorCode();
    }
    readerIndex = 0;
    xpointsslice = reader->getNumEvenCols();
    ypointsslice = reader->getNumRows();
    float* euler1Ptr = reader->getPhi1Pointer();
    if (NULL == euler1Ptr) { setErrorCode(-99090); setErrorMessage("Euler1 Pointer was NULL from Reader"); return getErrorCode(); }
    float* euler2Ptr = reader->getPhiPointer();
    float* euler3Ptr = reader->getPhi2Pointer();
    float* xPtr = reader->getXPositionPointer();
    float* yPtr = reader->getYPositionPointer();
    float* iqPtr = reader->getImageQualityPointer();
    float* ciPtr = reader->getConfidenceIndexPointer();
    int* phasePtr = reader->getPhaseDataPointer();
    float* sigPtr = reader->getSEMSignalPointer();
    float* fitPtr = reader->getFitPointer();

    xpointstemp = static_cast<int>(xpoints);
    ypointstemp = static_cast<int>(ypoints);
    xstartspot = (xpointstemp - xpointsslice) / 2;
    ystartspot = (ypointstemp - ypointsslice) / 2;
    xstop = xpointsslice;
    ystop = ypointsslice;

    // If no stacking order preference was passed, read it from the file and use that value
    if(ZDir == SIMPL::RefFrameZDir::UnknownRefFrameZDirection)
    {
      ZDir = getStackingOrder();
    }
    if(ZDir == SIMPL::RefFrameZDir::LowtoHigh) { zval = slice; }
    if(ZDir == SIMPL::RefFrameZDir::HightoLow) { zval = static_cast<int>( (zpoints - 1) - slice ); }

    // Copy the data from the current storage into the new memory Location
    for (int j = 0; j < ystop; j++)
    {
      for (int i = 0; i < xstop; i++)
      {
        index = (zval * xpointstemp * ypointstemp) + ((j + ystartspot) * xpointstemp) + (i + xstartspot);
        if (NULL != euler1Ptr) {m_Phi1[index] = euler1Ptr[readerIndex];}
        if (NULL != euler2Ptr) {m_Phi[index] = euler2Ptr[readerIndex];}
        if (NULL != euler3Ptr) {m_Phi2[index] = euler3Ptr[readerIndex];}
        if (NULL != xPtr) {m_X[index] = xPtr[readerIndex];}
        if (NULL != yPtr) {m_Y[index] = yPtr[readerIndex];}
        if (NULL != iqPtr) {m_Iq[index] = iqPtr[readerIndex];}
        if (NULL != ciPtr) {m_Ci[index] = ciPtr[readerIndex];}
        if (NULL != phasePtr) {m_PhaseData[index] = phasePtr[readerIndex];} // Phase
        if (NULL != sigPtr) {m_SEMSignal[index] = sigPtr[readerIndex];}
        if (NULL != fitPtr) {m_Fit[index] = fitPtr[readerIndex];}

        /* For TSL OIM Files if there is a single phase then the value of the phase
         * data is zero (0). If there are 2 or more phases then the lowest value
         * of phase is one (1). In the rest of the reconstruction code we follow the
         * convention that the lowest value is One (1) even if there is only a single
         * phase. The next if statement converts all zeros to ones if there is a single
         * phase in the OIM data.
         */
        if (numPhases == 1 && NULL != phasePtr && m_PhaseData[index] < 1)
        {
          m_PhaseData[index] = 1;
        }

        ++readerIndex;
      }
    }
  }
  return err;
}


