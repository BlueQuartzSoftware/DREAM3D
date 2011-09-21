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
  hid_t fileId = H5Utilities::openFile(m_FileName, true);
  if (fileId < 0)
  {
    std::cout << "Error Opening file '" << m_FileName << "'" << std::endl;
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

  hid_t fileId = H5Utilities::openFile(m_FileName, true);
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
  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    std::cout << "Error: Could not open .h5ebsd file for reading." << std::endl;
    return m_Phases;
  }
  herr_t err = 0;


  hid_t gid = H5Gopen(fileId, index.c_str(), H5P_DEFAULT);
  H5AngReader::Pointer reader = H5AngReader::New();
  reader->setHDF5Path(index);
  err = reader->readHeader(gid);
  if (err < 0)
  {
    std::cout  << "Error reading the header information from the .h5ebsd file" << std::endl;
    err = H5Gclose(gid);
    err = H5Fclose(fileId);
    return m_Phases;
  }
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
int H5AngVolumeReader::loadData(float* euler1s,
                                float* euler2s,
                                float* euler3s,
                                int* phases,
                                bool* goodVoxels,
                                int xpoints,
                                int ypoints,
                                int zpoints,
                                Ebsd::RefFrameZDir ZDir,
                                std::vector<QualityMetricFilter::Pointer> filters)
{
  int index = 0;
  int err = -1;

  int readerIndex;
  int xpointstemp;
  int ypointstemp;
  int xstop;
  int ystop;
  int zval;

  float* euler1Ptr = NULL;
  float* euler2Ptr = NULL;
  float* euler3Ptr = NULL;
  int* phasePtr = NULL;

  int xstartspot;
  int ystartspot;

  // Create an Array of Void Pointers that will point to the data that is going to
  // serve as the filter data, such as Confidence Index or Image Quality
  std::vector<void*> dataPointers(filters.size(), NULL);
  std::vector<Ebsd::NumType> dataTypes(filters.size(), Ebsd::UnknownNumType);

  err = readVolumeInfo();
//  int sliceStart = getSliceStart();
//  int sliceEnd = getSliceEnd();
  for (int slice = 0; slice < zpoints; ++slice)
  {
    H5AngReader::Pointer reader = H5AngReader::New();
    reader->setFileName(getFileName());
    reader->setHDF5Path(StringUtils::numToString(slice + getZStart()) );
    reader->setUserOrigin(getRefFrameOrigin());
    reader->setUserZDir(getRefFrameZDir());

    err = reader->readFile();
    if (err < 0)
    {
      std::cout << "H5AngDataLoader Error: There was an issue loading the data from the hdf5 file." << std::endl;
      return -1;
    }
    setAxesFlipped(reader->getAxesFlipped());
    readerIndex = 0;
    xpointstemp = reader->getNumEvenCols();
    ypointstemp = reader->getNumRows();
    euler1Ptr = reader->getPhi1Pointer();
    euler2Ptr = reader->getPhiPointer();
    euler3Ptr = reader->getPhi2Pointer();
    phasePtr = reader->getPhasePointer();

    // Gather some information about the filters and types in order to run the QualityMetric Filter
    for(size_t i = 0; i < filters.size(); ++i)
    {
      dataPointers[i] = reader->getPointerByName(filters[i]->getFieldName());
      dataTypes[i] = reader->getPointerType(filters[i]->getFieldName());
    }

    // Figure out which are good voxels
    AIMArray<bool>::Pointer good_voxels = determineGoodVoxels(filters, dataPointers, xpointstemp * ypointstemp, dataTypes);

    if(getAxesFlipped() == true)
    {
      xstartspot = (ypoints - xpointstemp) / 2;
      ystartspot = (xpoints - ypointstemp) / 2;
      xstop = xpointstemp;
      ystop = ypointstemp;
    }
    else if(getAxesFlipped() == false)
    {
      xstartspot = (xpoints - xpointstemp) / 2;
      ystartspot = (ypoints - ypointstemp) / 2;
      xstop = xpointstemp;
      ystop = ypointstemp;
    }

    if(ZDir == Ebsd::IntoSlice) zval = slice;
    if(ZDir == Ebsd::OutofSlice) zval = (zpoints-1) - slice;

    // Copy the data from the current storage into the ReconstructionFunc Storage Location
    for (int j = 0; j < ystop; j++)
    {
      for (int i = 0; i < xstop; i++)
      {
        if(getAxesFlipped() == true) index = (zval * xpoints * ypoints) + ((j + ystartspot) * ypoints) + (i + xstartspot);
        else if(getAxesFlipped() == false) index = (zval * xpoints * ypoints) + ((j + ystartspot) * xpoints) + (i + xstartspot);
        euler1s[index] = euler1Ptr[readerIndex]; // Phi1
        euler2s[index] = euler2Ptr[readerIndex]; // Phi
        euler3s[index] = euler3Ptr[readerIndex]; // Phi2
        phases[index] = phasePtr[readerIndex]; // Phase
        if (NULL != good_voxels.get()) {
          goodVoxels[index] = good_voxels->GetValue(readerIndex);
        }
        /* For TSL OIM Files if there is a single phase then the value of the phase
         * data is zero (0). If there are 2 or more phases then the lowest value
         * of phase is one (1). In the rest of the reconstruction code we follow the
         * convention that the lowest value is One (1) even if there is only a single
         * phase. The next if statement converts all zeros to ones if there is a single
         * phase in the OIM data.
         */
        if (phases[index] < 1)
        {
          phases[index] = 1;
        }

        ++readerIndex;
      }
    }
  }
  return err;
}


