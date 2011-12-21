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

#include "H5VoxelGrainIdReader.h"

H5VoxelGrainIdReader::H5VoxelGrainIdReader() :
DREAM3D::FileReader()
{
  // TODO Auto-generated constructor stub

}

H5VoxelGrainIdReader::~H5VoxelGrainIdReader()
{
  // TODO Auto-generated destructor stub
}

int H5VoxelGrainIdReader::readHeader()
{
  return 0;
}

int H5VoxelGrainIdReader::readFile()
{
  if(NULL == getDataContainer())
  {
    std::stringstream ss;
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "(" << __LINE__<<")";
    setErrorMessage(ss.str());
    setErrorCondition(-1);
    return -1;
  }

  int err = 0;
  H5VoxelReader::Pointer reader = H5VoxelReader::New();
  reader->setFileName(getFileName());
  int volDims[3];
  float spacing[3];
  float origin[3];
  err = reader->getSizeResolutionOrigin(volDims, spacing, origin);
  if(err < 0)
  {
    setErrorMessage("Error Reading the Dimensions, Origin and Scaling values from the HDF5 Voxel File");
    return err;
  }
  getDataContainer()->setDimensions(volDims);
  getDataContainer()->setResolution(spacing);
  getDataContainer()->setOrigin(origin);

  size_t totalpoints = volDims[0] * volDims[1] * volDims[2];
  // Create an DataArray to hold the data
  DataArray<int>::Pointer grainIds = DataArray<int>::CreateArray(totalpoints);

  err = reader->readScalarData<int>(DREAM3D::VTK::GrainIdScalarName, grainIds->GetPointer(0));
  if(err < 0)
  {
    setErrorCondition(err);
    setErrorMessage("Error Reading the GrainIDs from the .h5voxel file.");
    grainIds = DataArray<int>::NullPointer();
  }

  getDataContainer()->addVoxelData(DREAM3D::VoxelData::GrainIds, grainIds);
  return err;
}
