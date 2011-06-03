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

#ifndef _H5RECONVOLUMEWRITER_H_
#define _H5RECONVOLUMEWRITER_H_

#include "hdf5.h"

#include <vector>
#include <string>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"


#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/HDF5/AIM_H5VtkDataWriter.h"

#define H5_WRITE_SCALAR(type, group, scalar_name, voxel_attr)\
{\
std::vector<type> data(totalPoints);\
int32_t rank = 1;\
hsize_t dims[2] = {totalPoints};\
 for (int i = 0; i < totalPoints; ++i)\
 {\
   data[i] = voxels[i].voxel_attr;\
 }\
 err = h5writer->writeScalarData(group, data, scalar_name, numComp, rank, dims);\
 if (err < 0)\
 {\
   std::cout << "Error Writing Scalars '" << scalar_name << "' to " << group << std::endl;\
   return err;\
 }\
 }


/**
 * @class H5ReconVolumeWriter H5ReconVolumeWriter.h DREAM3D/Reconstruction/H5ReconVolumeWriter.h
 * @brief This class will write Voxel data to an HDF5 file for the Reconstruction
 * module.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jun 2, 2011
 * @version 1.0
 */

class DREAM3DLib_EXPORT H5ReconVolumeWriter
{
  public:
    MXA_SHARED_POINTERS(H5ReconVolumeWriter)
    MXA_TYPE_MACRO(H5ReconVolumeWriter)
    MXA_STATIC_NEW_MACRO(H5ReconVolumeWriter)

    virtual ~H5ReconVolumeWriter();

    MXA_INSTANCE_STRING_PROPERTY(Filename);

template<typename T, typename K>
int writeVoxelData(T* m)
{
  int err = -1;
  AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();
  h5writer->setFileName(m_Filename);
  err = h5writer->openFile(false);

  int volDims[3] =
  { m->xpoints, m->ypoints, m->zpoints };
  float spacing[3] =
  { m->resx, m->resy, m->resz };
  float origin[3] =
  { 0.0f, 0.0f, 0.0f };
  // This just creates the group and writes the header information
  h5writer->writeStructuredPoints(AIM::Reconstruction::VoxelDataName, volDims, spacing, origin);

  // We now need to write the actual voxel data
  int numComp = 1; //
  int totalPoints = m->totalpoints;
  boost::shared_array<K> voxels = m->voxels;
  H5_WRITE_SCALAR(int, AIM::Reconstruction::VoxelDataName, AIM::Reconstruction::GrainIdScalarName.c_str(), grain_index);

  H5_WRITE_SCALAR(int, AIM::Reconstruction::VoxelDataName, AIM::Reconstruction::PhaseIdScalarName.c_str(), phase);

  std::vector<float> dataf(totalPoints * 3);
  for (int i = 0; i < totalPoints; ++i)
  {
    dataf[i * 3] = voxels[i].euler1;
    dataf[i * 3 + 1] = voxels[i].euler2;
    dataf[i * 3 + 2] = voxels[i].euler3;
  }
  numComp = 3;
  int32_t rank = 2;
  hsize_t dims[2] = {totalPoints, numComp};
  err = h5writer->writeScalarData(AIM::Reconstruction::VoxelDataName,
                                  dataf, AIM::Reconstruction::EulerAnglesName.c_str(),
                                  numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << AIM::Reconstruction::EulerAnglesName << "' to " << AIM::Reconstruction::VoxelDataName << std::endl;
  }

  std::vector<int> fieldData(m->crystruct.size());
  for(size_t i = 0; i < m->crystruct.size(); ++i)
  {
    fieldData[i] = m->crystruct[i];
  }

  err = h5writer->writeFieldData<int>(AIM::Reconstruction::VoxelDataName, fieldData, AIM::Reconstruction::CrystalStructureName.c_str(), 1);
  if (err < 0)
  {
    std::cout << "Error Writing Field Data '" << AIM::Reconstruction::CrystalStructureName << "' to " << AIM::Reconstruction::VoxelDataName << std::endl;
  }

  err = h5writer->closeFile();
  return err;
}



  protected:
    H5ReconVolumeWriter();

  private:
    H5ReconVolumeWriter(const H5ReconVolumeWriter&); // Copy Constructor Not Implemented
    void operator=(const H5ReconVolumeWriter&); // Operator '=' Not Implemented
};

#endif /* _H5RECONVOLUMEWRITER_H_ */
