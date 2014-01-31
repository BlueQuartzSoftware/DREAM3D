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
#ifndef H5RECONVOLUMEREADER_H_
#define H5RECONVOLUMEREADER_H_

#include "hdf5.h"

#include <vector>
#include <QtCore/QString>
#include <sstream>

#include <boost/shared_array.hpp>




#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/H5Macros.h"


/**
 * @class H5VoxelReader H5VoxelReader.h DREAM3D/HDF5/H5VoxelReader.h
 * @brief Reads a Voxel volume from an HDF5 file
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jun 30, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT H5VoxelReader : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(H5VoxelReader)
    DREAM3D_TYPE_MACRO(H5VoxelReader)
    DREAM3D_STATIC_NEW_MACRO(H5VoxelReader)

    virtual ~H5VoxelReader();

    DREAM3D_INSTANCE_STRING_PROPERTY(FileName)
//   DREAM3D_INSTANCE_STRING_PROPERTY(ErrorMessage)

    int getSizeResolutionOrigin(int64_t volDims[3], float spacing[3], float origin[3]);

    int readHyperSlab(int64_t xdim, int64_t ydim, int64_t zIndex, int* fileVoxelLayer);



    /**
     *
     */
    int readVoxelData(int* feature_indicies,
                      int* phases,
                      float* eulerangles,
                      std::vector<unsigned int>& crystruct,
                      std::vector<unsigned int>& phaseType,
                      int64_t totalpoints);


    /**
     *
     */
    template<typename T>
    int readScalarData(const QString& dsetName, T* data )
    {
      int err = 0;
      if (m_FileId < 0)
      {
        if(m_FileName.isEmpty() == true)
        {
          notifyErrorMessage(getHumanLabel(), "H5VoxelReader Error; Filename was empty", -1);
          return -1;
        }
        OPEN_HDF5_FILE(fileId, m_FileName);
        m_FileId = fileId;
      }
      OPEN_RECONSTRUCTION_GROUP(reconGid, DREAM3D::StringConstants::VoxelDataName.toLatin1().data(), m_FileId);
      OPEN_RECONSTRUCTION_GROUP(scalarGid, H5_SCALAR_DATA_GROUP_NAME, reconGid);

      // Read in the Feature ID data
      err = QH5Lite::readPointerDataset(scalarGid, dsetName, data);
      if(err < 0)
      {
        QString ss = QObject::tr(" Error Reading the ").arg(dsetName);
        notifyErrorMessage(getHumanLabel(), ss, err);
        err = H5Gclose(scalarGid);
        err = H5Gclose(reconGid);
        return err;
      }

      err |= H5Gclose(scalarGid);
      err |= H5Gclose(reconGid);
      return err;
    }

    /**
     *
     */
    template<typename CastTo, typename NativeType>
    int readFeatureData(const QString& dsetName, std::vector<CastTo>& data)
    {
      int err = 0;
      if (m_FileId < 0)
      {
        if(m_FileName.isEmpty() == true)
        {
          notifyErrorMessage(getHumanLabel(), "H5ReconVolumeReader Error; Filename was empty", -1);
          return -1;
        }
        OPEN_HDF5_FILE(fileId, m_FileName);
        m_FileId = fileId;
      }
      OPEN_RECONSTRUCTION_GROUP(reconGid, DREAM3D::StringConstants::VoxelDataName.toLatin1().data(), m_FileId);
      OPEN_RECONSTRUCTION_GROUP(featureGid, H5_FIELD_DATA_GROUP_NAME, reconGid);

      std::vector<NativeType> nativeData;
      err = QH5Lite::readVectorDataset(featureGid, dsetName, nativeData);
      if(err < 0)
      {
        QString ss = QObject::tr(" Error Reading the ").arg(dsetName);
        notifyErrorMessage(getHumanLabel(), ss, err);
        err |= H5Gclose(featureGid);
        err |= H5Gclose(reconGid);
        return err;
      }
      data.resize(nativeData.size());
      for (size_t i = 0; i < nativeData.size(); ++i)
      {
        data[i] = static_cast<CastTo>(nativeData[i]);
      }
      err |= H5Gclose(featureGid);
      err |= H5Gclose(reconGid);

      return err;
    }


  protected:
    H5VoxelReader();

  private:
    hid_t m_FileId;


    H5VoxelReader(const H5VoxelReader&); // Copy Constructor Not Implemented
    void operator=(const H5VoxelReader&); // Operator '=' Not Implemented
};

#endif /* H5RECONVOLUMEREADER_H_ */

