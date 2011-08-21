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

#include "MXA/Utilities/StringUtils.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/HKL/H5CtfReader.h"

//#include "DREAM3D/Common/Constants.h"
//#include "DREAM3D/Common/OrientationMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfVolumeReader::H5CtfVolumeReader() :
H5EbsdVolumeReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfVolumeReader::~H5CtfVolumeReader()
{

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
  hid_t fileId = H5Utilities::openFile(getFilename(), true);
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
int H5CtfVolumeReader::loadData(float* euler1s, float* euler2s, float* euler3s,
                                int* phases, bool* goodVoxels,
                                int xpoints, int ypoints, int zpoints,
                                std::vector<QualityMetricFilter::Pointer> filters)
{

  int index = 0;
  int err = -1;
#if !defined (WIN32)
   #warning This whole method needs to be validated
#else
  #pragma NOTE(This whole method needs to be validated)
#endif

  int readerIndex;
  int xpointstemp;
  int ypointstemp;

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

  for (int slice = 0; slice < zpoints; ++slice)
  {
    H5CtfReader::Pointer reader = H5CtfReader::New();
    reader->setFileName(getFilename());
    reader->setHDF5Path(StringUtils::numToString(slice + getZStart() ) );
    reader->setUserOrigin(getOrientation());

    err = reader->readFile();
    if (err < 0)
    {
      std::cout << "H5CtfVolumeReader Error: There was an issue loading the data from the hdf5 file." << std::endl;
      return -1;
    }
    readerIndex = 0;
    xpointstemp = reader->getXCells();
    ypointstemp = reader->getYCells();
    euler1Ptr = reader->getEuler1Pointer();
    euler2Ptr = reader->getEuler2Pointer();
    euler3Ptr = reader->getEuler3Pointer();
    phasePtr = reader->getPhasePointer();
    // Gather some information about the filters and types in order to run the QualityMetric Filter
    for(size_t i = 0; i < filters.size(); ++i)
    {
      dataPointers[i] = reader->getPointerByName(filters[i]->getFieldName());
      dataTypes[i] = reader->getPointerType(filters[i]->getFieldName());
    }

    // Figure out which are good voxels
    AIMArray<bool>::Pointer good_voxels = determineGoodVoxels(filters, dataPointers, xpointstemp * ypointstemp, dataTypes);


    xstartspot = (xpoints - xpointstemp) / 2;
    ystartspot = (ypoints - ypointstemp) / 2;

    // Copy the data from the current storage into the ReconstructionFunc Storage Location
    for (int j = 0; j < ypointstemp; j++)
    {
      for (int i = 0; i < xpointstemp; i++)
      {
        index = ((zpoints - 1 - slice) * xpoints * ypoints) + ((j + ystartspot) * xpoints) + (i + xstartspot);
        euler1s[index] = euler1Ptr[readerIndex]; // Phi1
        euler2s[index] = euler2Ptr[readerIndex]; // Phi
        euler3s[index] = euler3Ptr[readerIndex]; // Phi2
        phases[index] = phasePtr[readerIndex] + 1; // Phase Add 1 to the phase number because .ctf files are zero based for phases
        if (NULL != good_voxels.get()) {
          goodVoxels[index] = good_voxels->GetValue(readerIndex);
        }

        ++readerIndex;
      }
    }
  }
  return err;

}

