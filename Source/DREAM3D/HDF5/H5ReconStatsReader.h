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

#ifndef H5RECONSTATSREADER_H_
#define H5RECONSTATSREADER_H_

#include "hdf5.h"

#include <vector>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/MXATypes.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/StringUtils.h"

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/HDF5/H5Macros.h"

/**
 * @class H5ReconStatsReader H5ReconStatsReader.h AIM/Common/HDF5/H5ReconStatsReader.h
 * @brief This class reads the statistics that are produced from a Reconstruction
 * run or through the StatsGenerator program.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Apr 21, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT H5ReconStatsReader
{
  public:
    MXA_SHARED_POINTERS(H5ReconStatsReader);
    MXA_STATIC_NEW_MACRO(H5ReconStatsReader);
    static Pointer New(const std::string &filename);

    virtual ~H5ReconStatsReader();


    MXA_INSTANCE_STRING_PROPERTY(FileName);

    /**
     * @brief
     * @param phases std::vector<int> object to place the phase values into
     * @param xtals std::vector to store the crystal structure values into
     */
    int getPhaseAndCrystalStructures(std::vector<int> &phases,
                                     std::vector<Ebsd::CrystalStructure> &xtals);


    /**
     * @brief
     * @param path
     * @param name
     * @param data
     */
    template<typename T>
    int readVectorDataset(const std::string &path, const std::string &name, std::vector<T> &data)
    {
      herr_t err = 0;
      herr_t retErr = 0;
      OPEN_HDF5_FILE(fileId, m_FileName)

      // The group may NOT be written to the file
      hid_t pid = H5Gopen(fileId, path.c_str(), H5P_DEFAULT);
      if (pid < 0)
      {
        err = H5Utilities::closeFile(fileId);
        return -1;
      }

      err = H5Lite::readVectorDataset(pid, name, data);
      if (err < 0)
      {
        data.clear(); // Clear all the data from the vector.
        retErr = err;
      }

      err = H5Gclose(pid);
      if (err < 0) { retErr = err; }
      err = H5Utilities::closeFile(fileId);
      if (err < 0) { retErr = err; }
      return retErr;
    }


    template<typename T>
    int readScalarAttribute(int phase, const std::string &name, const std::string &attrName, T &data)
    {
      herr_t err = 0;
      herr_t retErr = 0;
      OPEN_HDF5_FILE(fileId, m_FileName)

      OPEN_RECONSTRUCTION_GROUP(reconGid, AIM::HDF5::Reconstruction.c_str(), fileId)

      std::string index = StringUtils::numToString(phase);
      hid_t pid = H5Gopen(reconGid, index.c_str(), H5P_DEFAULT);

      err = H5Lite::readScalarAttribute(pid, name, attrName, data);

      err = H5Gclose(pid);
      if (err < 0) { retErr = err; }
      err = H5Gclose(reconGid);
      if (err < 0) { retErr = err; }
      err = H5Utilities::closeFile(fileId);
      if (err < 0) { retErr = err; }
      return retErr;
    }



    /**
     * @brief
     * @param phase
     * @param name
     * @param data
     * @return Error Code
     */
    template<typename T>
    int readStatsDataset(int phase, const std::string &name, std::vector<T> &data)
    {
      herr_t err = 0;
      herr_t retErr = 0;
      OPEN_HDF5_FILE(fileId, m_FileName)

      OPEN_RECONSTRUCTION_GROUP(reconGid, AIM::HDF5::Reconstruction.c_str(), fileId)

      std::string index = StringUtils::numToString(phase);
      hid_t pid = H5Gopen(reconGid, index.c_str(), H5P_DEFAULT);

      err = H5Lite::readVectorDataset(pid, name, data);
      if (err < 0)
      {
        data.clear(); // Clear all the data from the vector.
        retErr = err;
      }

      err = H5Gclose(pid);
      if (err < 0) { retErr = err; }
      err = H5Gclose(reconGid);
      if (err < 0) { retErr = err; }
      err = H5Utilities::closeFile(fileId);
      if (err < 0) { retErr = err; }
      return retErr;
    }

#if 0
    template<typename T>
    int readPhaseFractionDataset(int phase, const std::string &name, std::vector<T> &data)
    {
      herr_t err = 0;
      herr_t retErr = 0;
      double value;
      OPEN_HDF5_FILE(fileId, m_FileName)
      OPEN_RECONSTRUCTION_GROUP(reconGid, AIM::HDF5::Reconstruction.c_str(), fileId)
      // Ensure the size of the data vector is lage enough:
      if (phase >= data.size() )
      {
        data.resize(phase, 0); //Resize adding Zeros as default data
      }

      std::string index = StringUtils::numToString(phase);
      hid_t pid = H5Gopen(reconGid, index.c_str());

      err = H5Lite::readScalarDataset(pid, name, value);
      if (err < 0)
      {
        data[phase] = 0; // Clear all the data from the vector.
        retErr = err;
      }
      data[phase] = value;

      err = H5Gclose(pid);
      if (err < 0) { retErr = err; }

      err = H5Gclose(reconGid);
      if (err < 0) { retErr = err; }
      err = H5Utilities::closeFile(fileId);
      if (err < 0) { retErr = err; }
      return retErr;
    }
#endif

	/**
     * @brief
     * @param group
     * @param dt (out) Enumerated value for the distribution type
     */
    std::string getDistributionType(int phase, const std::string &group, AIM::Reconstruction::DistributionType &dt);



  protected:
    H5ReconStatsReader();
  private:
    hid_t m_FileId;

    H5ReconStatsReader(const H5ReconStatsReader&); // Copy Constructor Not Implemented
    void operator=(const H5ReconStatsReader&); // Operator '=' Not Implemented

};

#endif /* H5RECONSTATSREADER_H_ */
