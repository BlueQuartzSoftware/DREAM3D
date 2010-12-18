/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

#ifndef H5RECONSTATSREADER_H_
#define H5RECONSTATSREADER_H_

#include "hdf5.h"

#include <vector>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/HDF5/H5Utilities.h"
#include "MXA/HDF5/H5Lite.h"

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"
#include "AIM/Common/Bin.h"


#define OPEN_HDF5_FILE(fileId, filename)\
  hid_t fileId = H5Utilities::openFile(filename, false);\
  if (fileId < 0) { return fileId; }


#define OPEN_RECONSTRUCTION_GROUP(gid, name, fileId)\
  hid_t gid = H5Gopen(fileId, name);\
  if (gid < 0) { \
    err = H5Utilities::closeFile(fileId);\
    return -1; }



class AIMCOMMON_EXPORT H5ReconStatsReader
{
  public:
    MXA_SHARED_POINTERS(H5ReconStatsReader);
    MXA_STATIC_NEW_MACRO(H5ReconStatsReader);
    static Pointer New(const std::string &filename);

    virtual ~H5ReconStatsReader();


    MXA_INSTANCE_STRING_PROPERTY(FileName);


    template<typename T>
    int readStatsDataset(const std::string &name, std::vector<T> &data)
    {
      herr_t err = 0;
      herr_t retErr = 0;
      OPEN_HDF5_FILE(fileId, m_FileName)

      OPEN_RECONSTRUCTION_GROUP(reconGid, AIM::HDF5::Reconstruction.c_str(), fileId)

      err = H5Lite::readVectorDataset(reconGid, name, data);
      if (err < 0)
      {
        data.clear(); // Clear all the data from the vector.
        retErr = err;
      }

      err = H5Gclose(reconGid);
      if (err < 0)
      {
        retErr = err;
      }
      err = H5Utilities::closeFile(fileId);
      if (err < 0)
      {
        retErr = err;
      }
      return retErr;
    }

  protected:
    H5ReconStatsReader();
  private:
    hid_t m_FileId;

    H5ReconStatsReader(const H5ReconStatsReader&); // Copy Constructor Not Implemented
    void operator=(const H5ReconStatsReader&); // Operator '=' Not Implemented

};

#endif /* H5RECONSTATSREADER_H_ */
