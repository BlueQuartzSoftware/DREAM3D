/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
#ifndef H5RECONVOLUMEREADER_H_
#define H5RECONVOLUMEREADER_H_

#include "hdf5.h"

#include <vector>
#include <string>

#include <boost/shared_array.hpp>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"


#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Reconstruction/ReconstructionVoxel.h"
#include "HDF5/H5Lite.h"
#include "HDF5/H5Utilities.h"
#include "DREAM3D/HDF5/VTKH5Constants.h"


#define OPEN_HDF5_FILE(fileId, filename)\
  hid_t fileId = H5Utilities::openFile(filename, false);\
  if (fileId < 0) { return fileId; }


#define OPEN_RECONSTRUCTION_GROUP(gid, name, fileId)\
  hid_t gid = H5Gopen(fileId, name);\
  if (gid < 0) { \
    err = H5Utilities::closeFile(fileId);\
    return -1; }

/*
 *
 */
class H5ReconVolumeReader
{
  public:
    MXA_SHARED_POINTERS(H5ReconVolumeReader)
    MXA_TYPE_MACRO(H5ReconVolumeReader)
    MXA_STATIC_NEW_MACRO(H5ReconVolumeReader)

    virtual ~H5ReconVolumeReader();

    MXA_INSTANCE_STRING_PROPERTY(Filename);

    int getSizeAndResolution(int volDims[3], float spacing[3]);

	template<typename T>
	int readVoxelData(boost::shared_array<T> voxels,
                                       std::vector<AIM::Reconstruction::CrystalStructure> &crystruct,
                                       int totalpoints)
{
  int err = 0;
  if (m_Filename.empty() == true)
  {
    std::cout << "H5ReconVolumeReader Error; Filename was empty" << std::endl;
    return -1;
  }

  OPEN_HDF5_FILE(fileId, m_Filename)
  OPEN_RECONSTRUCTION_GROUP(reconGid, AIM::Reconstruction::VoxelDataName.c_str(), fileId)
  OPEN_RECONSTRUCTION_GROUP(scalarGid, H5_SCALAR_DATA_GROUP_NAME, reconGid)

  int* iData = (int*)(malloc(totalpoints * sizeof(int)));

  // Read in the Grain ID data
  err = H5Lite::readPointerDataset(scalarGid, AIM::Reconstruction::GrainIdScalarName, iData);
  if (err < 0)
  {
    std::cout << "H5ReconVolumeReader Error Reading the Grain IDs" << std::endl;
    free(iData);
    err = H5Gclose(scalarGid);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  for(int i = 0; i < totalpoints; ++i)
  {
    voxels[i].grain_index = iData[i];
  }

  // Read the Phase ID data
  err = H5Lite::readPointerDataset(scalarGid, AIM::Reconstruction::PhaseIdScalarName, iData);
  if (err < 0)
  {
    std::cout << "H5ReconVolumeReader Error Reading the Phase IDs" << std::endl;
    free(iData);
    err = H5Gclose(scalarGid);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  for(int i = 0; i < totalpoints; ++i)
  {
    voxels[i].phase = iData[i];
  }
  free(iData);


  // Read in the Euler Angles Data
  float* fData = (float*)(malloc(totalpoints * 3 * sizeof(float)));
  err = H5Lite::readPointerDataset(scalarGid, AIM::Reconstruction::EulerAnglesName, fData);
  if (err < 0)
  {
    std::cout << "H5ReconVolumeReader Error Reading the Euler Angles" << std::endl;
    free(fData);
    err = H5Gclose(scalarGid);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  for(int i = 0; i < totalpoints; ++i)
  {
    voxels[i].euler1 = fData[i*3];
    voxels[i].euler2 = fData[i*3+1];
    voxels[i].euler3 = fData[i*3+2];
  }
  free(fData);
  // Close the group as we are done with it.
  err = H5Gclose(scalarGid);

  // Open the Field Data Group
  OPEN_RECONSTRUCTION_GROUP(fieldGid, H5_FIELD_DATA_GROUP_NAME, reconGid)

  // Read the CrystalStructure Field Data
  std::vector<unsigned int> xtals;
  err = H5Lite::readVectorDataset(fieldGid, AIM::Reconstruction::CrystalStructureName, xtals);
  if (err < 0)
  {
    std::cout << "H5ReconVolumeReader Error Reading the Crystal Structure Field Data" << std::endl;
    err = H5Gclose(fieldGid);
    err = H5Gclose(reconGid);
    err = H5Fclose(fileId);
    return err;
  }
  crystruct.resize(xtals.size());
  for (size_t i =0; i < xtals.size(); ++i)
  {
    crystruct[i] = static_cast<AIM::Reconstruction::CrystalStructure>(xtals[i]);
  }


  // Close all the HDF5 Groups and close the file
  err = H5Gclose(fieldGid);
  err = H5Gclose(reconGid);
  err = H5Fclose(fileId);

  return err;
}



  protected:
    H5ReconVolumeReader();

  private:
    H5ReconVolumeReader(const H5ReconVolumeReader&); // Copy Constructor Not Implemented
    void operator=(const H5ReconVolumeReader&); // Operator '=' Not Implemented
};

#endif /* H5RECONVOLUMEREADER_H_ */
