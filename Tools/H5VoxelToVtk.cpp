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


/**
 * @file H5VoxelToVtk.cpp
 * @brief This program is mainly a debugging program to ensure the .h5voxel file
 * has the correct data since at the current time there is no way to visualize
 * the .h5voxel in ParaView or anything else. This will convert the .h5voxel to a
 * VTK structured points data file with Feature IDs and IPS Colors.
 */

#include <iostream>
#include <limits>
#include <QtCore/QString>
#include <vector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/VTKUtils/VTKWriterMacros.h"

#include "SIMPLib/IOFilters/DataContainerReader.h"


typedef struct
{
  int xpoints;
  int ypoints;
  int zpoints;
  float resx;
  float resy;
  float resz;
  int* feature_indicies;
  int* phases;
  float* euler1s;
  float* euler2s;
  float* euler3s;
  float* quats;
  unsigned int* crystruct;
} Test;



void updateProgressAndMessage(const std::string& msg, int prog)
{
  std::cout << prog << "% - " << msg << std::endl;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void pipelineProgress(int value)
{
  std::cout << value << "%" << std::endl;
}

void pipelineProgressMessage(const std::string& msg)
{
  std::cout << msg << std::endl;
}

void pipelineErrorMessage(const char* message)
{
  std::cout << "Error Message: " << message << std::endl;
}

void setErrorCondition(int err)
{
  std::cout << "Error Condition: " << err << std::endl;
}

void pipelineFinished()
{
  std::cout << "Pipeline Complete." << std::endl;
}

void setErrorMessage(const std::string& msg)
{
  std::cout << msg << std::endl;
}

std::string getNameOfClass()
{
  return "H5VoxelToVtk";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  std::cout << "Starting Conversion of H5Voxel to VTK with Feature ID and IPF Colors" << std::endl;
  if (argc < 3)
  {
    std::cout << "This program takes 2 arguments: Input .h5voxel file and output vtk file." << std::endl;
    return EXIT_FAILURE;
  }


  QString iFile = argv[1];

  int err = 0;

  DataContainerArray::Pointer dca = DataContainerArray::New();
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  dca->pushBack(m);

  DataContainerReader::Pointer h5Reader = DataContainerReader::New();
  h5Reader->setInputFile(iFile);
  h5Reader->setDataContainerArray(dca);
  size_t dcDims[3];
  float spacing[3];
  float origin[3];
  h5Reader->execute();
  err = h5Reader->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(err);
//   addErrorMessages(h5Reader->getErrorMessages());
    return EXIT_FAILURE;
  }
  m->getDimensions(dcDims);
  m->getResolution(spacing);
  m->getOrigin(origin);

  int64_t dims[3] = {dcDims[0], dcDims[1], dcDims[2]};

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
    return EXIT_FAILURE;
  }

  if (dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    std::stringstream s;
    s << "One of the dimensions is greater than the max index for this sysem. Try the 64 bit version.";
    s << " dim[0]=" << dims[0] << "  dim[1]=" << dims[1] << "  dim[2]=" << dims[2];
    setErrorCondition(err);
    setErrorMessage(s.str());
    return EXIT_FAILURE;
  }
  /* ************ End Sanity Check *************************** */


  std::stringstream ss;

  std::cout << "Writing VTK file" << std::endl;

  FILE* f = fopen(argv[2], "wb");

  WRITE_STRUCTURED_POINTS_HEADER("ASCII", m)


//  VoxelIPFColorScalarWriter<VolumeDataContainer> ipfWriter(m.get());
//  ipfWriter.m_WriteBinaryFiles = false;
//  ipfWriter.writeScalars(f);


  int64_t totalPoints = m->getTotalPoints();
  int32_t* m_FeatureIds = NULL;
  m_FeatureIds = m->getCellDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(SIMPL::CellData::FeatureIds, totalPoints, 1, NULL);
  if (0 == m_FeatureIds )
  {
    ss << "Filter " << getNameOfClass() << " requires the data array '" <<
       "DREAM3D" << "::" << "CellData" << "::" <<  "FeatureIds" << "' to already be created prior to execution." << std::endl;
    setErrorCondition(-300);
  }

  WRITE_VTK_GRAIN_IDS_ASCII(m, SIMPL::CellData::FeatureIds, m_FeatureIds)

  fclose(f);


  std::cout << "Done Converting" << std::endl;
  return EXIT_SUCCESS;
}
