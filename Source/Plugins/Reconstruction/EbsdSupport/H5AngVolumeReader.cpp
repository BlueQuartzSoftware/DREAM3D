/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "H5AngVolumeReader.h"

#include <cmath>


#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "MXA/Utilities/StringUtils.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/H5AngReader.h"

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/OrientationMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngVolumeReader::H5AngVolumeReader() :
H5EbsdVolumeReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngVolumeReader::~H5AngVolumeReader()
{
}

#if 0
This code is just here for some reference sake. It should NOT be used unless you
truly know what you are doing.
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngVolumeReader::readZHeader(int &zStart, int &zEnd, float &zRes)
{
  int err = -1;
  int retErr = 0;
  hid_t fileId = H5Utilities::openFile(m_Filename, true);
  if (fileId < 0)
  {
    std::cout << "Error Opening file '" << m_Filename << "'" << std::endl;
    return -1;
  }
  err = H5Lite::readScalarDataset(fileId, Ebsd::H5::ZResolution, zRes);
  if (err < 0)
  {
    std::cout << "H5AngDataLoader::readZHeader Error: Could not load ZResolution data" << std::endl;
    retErr = err;
  }
  err = H5Lite::readScalarDataset(fileId, Ebsd::H5::ZStartIndex, zStart);
  if (err < 0)
  {
    std::cout << "H5AngDataLoader::readZHeader Error: Could not load ZStartIndex data" << std::endl;
    retErr = err;
  }
  err = H5Lite::readScalarDataset(fileId, Ebsd::H5::ZEndIndex, zEnd);
  if (err < 0)
  {
    std::cout << "H5AngDataLoader::readZHeader Error: Could not load ZEndIndex data" << std::endl;
    retErr = err;
  }

  err = H5Fclose(fileId);
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngVolumeReader::getSizeAndResolution(int &xpoints, int &ypoints, int &zpoints,
                                    float &xres, float &yres, float &zres)
{
  int err = -1;
  zpoints = (getZEndIndex() - getZStartIndex() );
  int xpointstemp = 0;
  int ypointstemp = 0;
  xpoints = 0;
  ypoints = 0;

  hid_t fileId = H5Utilities::openFile(m_Filename, true);
  if (fileId < 0)
  {
    std::cout << "Error - Could not open HDF5 based OIM file" << std::endl;
    return -1;
  }


  zres = 0.0f;
  err = H5Lite::readScalarDataset(fileId, Ebsd::H5::ZResolution, zres);

  for(int i=0;i<zpoints;i++)
  {
    std::string index = StringUtils::numToString(i + getZStartIndex());
    hid_t gid = H5Gopen(fileId, index.c_str());
    H5AngReader::Pointer reader = H5AngReader::New();
    reader->setHDF5Path(index);
    err = reader->readHeader(gid);
    if (err < 0)
    {
      std::cout << "Error reading the .HDF5 Ang Header data" << std::endl;
      err = H5Gclose(gid);
      err = H5Fclose(fileId);
      return -1;
    }
    xres = reader->getXStep();
    yres = reader->getYStep();
    xpointstemp = reader->getNumEvenCols();
    ypointstemp = reader->getNumRows();
    if(xpointstemp > xpoints) xpoints = xpointstemp;
    if(ypointstemp > ypoints) ypoints = ypointstemp;
    err = H5Gclose(gid);
  }
  err = H5Fclose(fileId);

  return err;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<AngPhase::Pointer> H5AngVolumeReader::getPhases()
{
  m_Phases.clear();

  // Get the first valid index of a z slice
  std::string index = StringUtils::numToString(getZStart());

  // Open the hdf5 file and read the data
  hid_t fileId = H5Utilities::openFile(getFilename(), true);
  if (fileId < 0)
  {
    std::cout << "Error" << std::endl;
    return m_Phases;
  }
  herr_t err = 0;


  hid_t gid = H5Gopen(fileId, index.c_str());
  H5AngReader::Pointer reader = H5AngReader::New();
  reader->setHDF5Path(index);
  err = reader->readHeader(gid);
  m_Phases = reader->getPhases();
  if (err < 0)
  {
    std::cout << "Error reading the .HDF5 Ang Header data" << std::endl;
    err = H5Gclose(gid);
    err = H5Fclose(fileId);
    return m_Phases;
  }
  return m_Phases;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngVolumeReader::loadData(ReconstructionFunc* m)
{
  int index = 0;
  int err = -1;

  int readerIndex;
  int xpointstemp;
  int ypointstemp;

  float* euler1Ptr = NULL;
  float* euler2Ptr = NULL;
  float* euler3Ptr = NULL;
  float* confPtr = NULL;
  int* phasePtr = NULL;
  float* imqualPtr = NULL;
  float* semSignalPtr = NULL;
  int xstartspot;
  int ystartspot;

  err = readVolumeInfo();
  int sliceStart = getSliceStart();
  int sliceEnd = getSliceEnd();
  for (int slice = 0; slice < m->zpoints; ++slice)
  {
    H5AngReader::Pointer reader = H5AngReader::New();
    reader->setFileName(getFilename());
    reader->setHDF5Path(StringUtils::numToString(slice + getZStart()) );
    reader->setUserOrigin(getOrientation());

    err = reader->readFile();
    if (err < 0)
    {
      std::cout << "H5AngDataLoader Error: There was an issue loading the data from the hdf5 file." << std::endl;
      return -1;
    }
    readerIndex = 0;
    xpointstemp = reader->getNumEvenCols();
    ypointstemp = reader->getNumRows();
    euler1Ptr = reader->getPhi1Pointer();
    euler2Ptr = reader->getPhiPointer();
    euler3Ptr = reader->getPhi2Pointer();
    phasePtr = reader->getPhasePointer();
    confPtr = reader->getConfidenceIndexPointer();
    imqualPtr = reader->getImageQualityPointer();
    xstartspot = (m->xpoints - xpointstemp) / 2;
    ystartspot = (m->ypoints - ypointstemp) / 2;
    semSignalPtr = reader->getSEMSignalPointer();
    for (int j = 0; j < ypointstemp; j++)
    {
      for (int i = 0; i < xpointstemp; i++)
      {
        index = ((m->zpoints - 1 - slice) * m->xpoints * m->ypoints) + ((j + ystartspot) * m->xpoints) + (i + xstartspot);
        m->euler1s[index] = euler1Ptr[readerIndex]; // Phi1
        m->euler2s[index] = euler2Ptr[readerIndex]; // Phi
        m->euler3s[index] = euler3Ptr[readerIndex]; // Phi2
        m->imagequalities[index] = imqualPtr[readerIndex]; // Image Quality
        m->confidences[index] = confPtr[readerIndex]; // Confidence
        m->phases[index] = phasePtr[readerIndex]; // Phase
        /* For TSL OIM Files if there is a single phase then the value of the phase
         * data is zero (0). If there are 2 or more phases then the lowest value
         * of phase is one (1). In the rest of the reconstruction code we follow the
         * convention that the lowest value is One (1) even if there is only a single
         * phase. The next if statement converts all zeros to ones if there is a single
         * phase in the OIM data.
         */
        if (m->phases[index] < 1)
        {
          m->phases[index] = 1;
        }
        ++readerIndex;
      }
    }
  }
  return err;
}


