/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "StatsData.h"

#include <string>
#include <vector>

#include "H5Support/H5Utilities.h"

#include "DREAM3DLib/IOFilters/H5StatsDataWriter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::StatsData()
{
//FIXME: Initialize all the data to Zeros or something like that
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::~StatsData()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsData::initialize()
{
  m_BOverA_DistType = DREAM3D::DistributionType::Beta;
  m_COverA_DistType = DREAM3D::DistributionType::Beta;
  m_COverB_DistType = DREAM3D::DistributionType::Beta;
  m_Neighbors_DistType = DREAM3D::DistributionType::Power;
  m_Omegas_DistType = DREAM3D::DistributionType::Beta;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writeHDF5Data(hid_t groupId)
{
  int err = 0;

  // Write the PhaseFraction
  err |= writePhaseFraction( groupId);
  if (err < 0)
  {
    return err;
  }

  // Write the Grain Diameter Info

  err |= writeGrainDiameterInfo(groupId);
  // Write the Grain Size Distribution
  err |= writeGrainSizeDistribution(groupId);
  // Write the Bin Numbers
  err |= writeBinNumbers(groupId);

  // Write the B Over A
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::BetaDistribution,
                               DREAM3D::HDF5::Grain_SizeVBoverA_Distributions,
                               getGrainSize_BOverA());

  // Write the C Over A
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::BetaDistribution,
                               DREAM3D::HDF5::Grain_SizeVCoverA_Distributions,
                               getGrainSize_COverA());
  // Write the C Over B
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::BetaDistribution,
                               DREAM3D::HDF5::Grain_SizeVCoverB_Distributions,
                               getGrainSize_COverB());
  // Write the Neighbors
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::PowerLawDistribution,
                               DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions,
                               getGrainSize_Neighbors());
  // Write the Omegas
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::BetaDistribution,
                               DREAM3D::HDF5::Grain_SizeVOmega3_Distributions,
                               getGrainSize_Omegas());
  // Write the Misorientation Bins
  if (NULL != getMisorientationBins().get())
  {
    err |= getMisorientationBins()->writeH5Data(groupId);
  }
  err |= writeVectorOfArrays(groupId, DREAM3D::HDF5::MDFWeights, getMDF_Weights());


  // Write the ODF
  if (NULL != getODF().get())
  {
    err |= getODF()->writeH5Data(groupId);
  }
  err |= writeVectorOfArrays(groupId, DREAM3D::HDF5::ODFWeights, getODF_Weights());


  // Write the Axis ODF
  if (NULL != getAxisOrientation().get())
  {
    err |= getAxisOrientation()->writeH5Data(groupId);
  }
  err |= writeVectorOfArrays(groupId, DREAM3D::HDF5::AxisODFWeights, getAxisODF_Weights());

  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writeVectorOfArrays(hid_t pid, const std::string &hdf5GroupName,
                                           VectorOfFloatArray colData)
{

  herr_t err = 0;
  herr_t retErr = 0;

  // Create the Group Folder
  hid_t disId = H5Utilities::createGroup(pid, hdf5GroupName);
  if (disId > 0)
  {
    // Loop through all the column data and write each one to the HDF5 file
    size_t numColumns = colData.size();
    for (size_t c = 0; c < numColumns; ++c)
    {
      //std::cout << "Writing Dataset:" << hdf5GroupName << "/" << columnHeaders[c] << std::endl;
      err = -1;
      if(NULL != colData[c].get() && colData[c]->GetSize() > 0)
      {
        err = colData[c]->writeH5Data(disId);
        if(err < 0)
        {
          retErr = err;
          break;
        }
      }
      else
      {
        std::cout << hdf5GroupName << ":Null Data Column had no data. Did you create the data?" << std::endl;
        std::cout << "  File: " << __FILE__ << std::endl;
        std::cout << "  Line: " << __LINE__ << std::endl;
        break;
      }

    }

    // Close the HDF5 Group
    err = H5Gclose(disId);
  }
  else
  {
    retErr = disId;
  }

  return retErr;

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writeDistributionData(hid_t pid, const std::string &disType,
                                              const std::string &hdf5GroupName,
                                              VectorOfFloatArray colData)
{
  herr_t err = 0;
  herr_t retErr = 0;

  // Create the Group Folder
  hid_t disId = H5Utilities::createGroup(pid, hdf5GroupName);
  if (disId > 0)
  {
    err = H5Lite::writeStringAttribute(pid, hdf5GroupName, DREAM3D::HDF5::DistributionType, disType);
    if(err >= 0)
    {
      // Loop through all the column data and write each one to the HDF5 file
      size_t numColumns = colData.size();
      for (size_t c = 0; c < numColumns; ++c)
      {
        //std::cout << "Writing Dataset:" << hdf5GroupName << "/" << columnHeaders[c] << std::endl;
        err = -1;
        if(NULL != colData[c].get() && colData[c]->GetSize() > 0)
        {
          err = colData[c]->writeH5Data(disId);
          if(err < 0)
          {
            retErr = err;
            break;
          }
        }
        else
        {
          std::cout << hdf5GroupName << ":Null Data Column had no data. Did you create the data?" << std::endl;
          std::cout << "  File: " << __FILE__ << std::endl;
          std::cout << "  Line: " << __LINE__ << std::endl;
          break;
        }

      }
    }
    else
    {
      retErr = err;
    }
    // Close the HDF5 Group
    err = H5Gclose(disId);
  }
  else
  {
    retErr = disId;
  }

  return retErr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writePhaseFraction(hid_t pid)
{
  float phaseFraction = getPhaseFraction();
  return H5Lite::writeScalarDataset(pid, DREAM3D::HDF5::PhaseFraction, phaseFraction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writeGrainDiameterInfo(hid_t pid)
{
  hsize_t dims[1];
  dims[0] = 3;
  int32_t rank = 1;

  /*
   * Grain Diameter Info is encode as 3 floats: BinStepSize, MaxDiameter, MinDiameter
   */
  float grainDiameterInfo[3];
  getGrainDiameterInfo(grainDiameterInfo);

  return H5Lite::writePointerDataset(pid, DREAM3D::HDF5::Grain_Diameter_Info, rank, dims, grainDiameterInfo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writeGrainSizeDistribution(hid_t pid)
{
  hsize_t dims[1];
  dims[0] = 2;
  int32_t rank = 1;

  /*
   * Grain Size Distribution Info is encode as 2 floats: Average and Standard Deviation
   */
  float grainSizeDistribution[2];
  getGrainSizeDistribution(grainSizeDistribution);

  return H5Lite::writePointerDataset(pid, DREAM3D::HDF5::Grain_Size_Distribution, rank, dims, grainSizeDistribution);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsData::writeBinNumbers(hid_t groupId)
{
  float grainDiameterInfo[3];
  getGrainDiameterInfo(grainDiameterInfo);

  std::vector<hsize_t> dims(1, 0);

  std::vector<float> bins;
  float d = grainDiameterInfo[2];
  while (d <= grainDiameterInfo[1])
  {
    bins.push_back(d);
    d = d + grainDiameterInfo[0];
  }
  dims[0] = bins.size();

  return H5Lite::writeVectorDataset(groupId, DREAM3D::HDF5::BinNumber, dims, bins);
}


