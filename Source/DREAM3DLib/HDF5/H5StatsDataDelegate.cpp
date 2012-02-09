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

#include "H5StatsDataDelegate.h"

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5StatsDataDelegate::H5StatsDataDelegate()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5StatsDataDelegate::~H5StatsDataDelegate()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5StatsDataDelegate::readStatsData(StatsData* data, hid_t groupId)
{
  int err = 0;
  //Read the PhaseFraction

  err |= readPhaseFraction(data, groupId);

  // Read the Grain Diameter Info
  err |= readGrainDiameterInfo(data, groupId);




  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5StatsDataDelegate::writeStatsData(StatsData* data, hid_t groupId)
{
  if (NULL == data)
  {
    return -1;
  }
  int err = 0;

  // Write the PhaseFraction
  err |= writePhaseFraction(data, groupId);
  if (err < 0)
  {
    return err;
  }

  // Write the Grain Diameter Info
  err |= writeGrainDiameterInfo(data, groupId);

  // Write the Grain Size Distribution
  err |= writeGrainSizeDistribution(data, groupId);

  // Write the Bin Numbers
  err |= writeBinNumbers(data, groupId);

  // Write the B Over A
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::BetaDistribution,
                               DREAM3D::HDF5::Grain_SizeVBoverA_Distributions,
                               data->getGrainSize_BOverA());

  // Write the C Over A
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::BetaDistribution,
                               DREAM3D::HDF5::Grain_SizeVCoverA_Distributions,
                               data->getGrainSize_COverA());
  // Write the C Over B
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::BetaDistribution,
                               DREAM3D::HDF5::Grain_SizeVCoverB_Distributions,
                               data->getGrainSize_COverB());
  // Write the Neighbors
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::PowerLawDistribution,
                               DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions,
                               data->getGrainSize_Neighbors());
  // Write the Omegas
  err |= writeDistributionData(groupId,
                               DREAM3D::HDF5::BetaDistribution,
                               DREAM3D::HDF5::Grain_SizeVOmega3_Distributions,
                               data->getGrainSize_Omegas());
  // Write the Misorientation Bins
  if (NULL != data->getMisorientationBins().get())
  {
    err |= data->getMisorientationBins()->writeH5Data(groupId);
  }
  err |= writeVectorOfArrays(groupId, DREAM3D::HDF5::MDFWeights, data->getMDF_Weights());


  // Write the ODF
  if (NULL != data->getODF().get())
  {
    err |= data->getODF()->writeH5Data(groupId);
  }
  err |= writeVectorOfArrays(groupId, DREAM3D::HDF5::ODFWeights, data->getODF_Weights());


  // Write the Axis ODF
  if (NULL != data->getAxisOrientation().get())
  {
    err |= data->getAxisOrientation()->writeH5Data(groupId);
  }
  err |= writeVectorOfArrays(groupId, DREAM3D::HDF5::AxisODFWeights, data->getAxisODF_Weights());

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5StatsDataDelegate::writeVectorOfArrays(hid_t pid, const std::string &hdf5GroupName,
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
int H5StatsDataDelegate::writeDistributionData(hid_t pid, const std::string &disType,
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
int H5StatsDataDelegate::writePhaseFraction(StatsData* data, hid_t pid)
{
  float phaseFraction = data->getPhaseFraction();
  return H5Lite::writeScalarDataset(pid, DREAM3D::HDF5::PhaseFraction, phaseFraction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5StatsDataDelegate::readPhaseFraction(StatsData* data, hid_t pid)
{
  float phaseFraction = 0.0f;
  int err = H5Lite::readScalarDataset(pid, DREAM3D::HDF5::PhaseFraction, phaseFraction);
  data->setPhaseFraction(phaseFraction);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5StatsDataDelegate::writeGrainDiameterInfo(StatsData* data, hid_t pid)
{
  hsize_t dims[1];
  dims[0] = 3;
  int32_t rank = 1;

  /*
   * Grain Diameter Info is encode as 3 floats: BinStepSize, MaxDiameter, MinDiameter
   */
  float grainDiameterInfo[3];
  data->getGrainDiameterInfo(grainDiameterInfo);

  return H5Lite::writePointerDataset(pid, DREAM3D::HDF5::Grain_Diameter_Info, rank, dims, grainDiameterInfo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5StatsDataDelegate::readGrainDiameterInfo(StatsData* data, hid_t groupId)
{
  int err = 0;
  /*
   * Grain Diameter Info is encode as 3 floats: BinStepSize, MaxDiameter, MinDiameter
   */
  float grainDiameterInfo[3] =
  { 0.0f, 0.0f, 0.0f };

  err = H5Lite::readPointerDataset(groupId, DREAM3D::HDF5::Grain_Diameter_Info, grainDiameterInfo);
  data->setGrainDiameterInfo(grainDiameterInfo);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5StatsDataDelegate::writeGrainSizeDistribution(StatsData* data, hid_t pid)
{
  hsize_t dims[1];
  dims[0] = 2;
  int32_t rank = 1;

  /*
   * Grain Size Distribution Info is encode as 2 floats: Average and Standard Deviation
   */
  float grainSizeDistribution[2];
  data->getGrainSizeDistribution(grainSizeDistribution);

  return H5Lite::writePointerDataset(pid, DREAM3D::HDF5::Grain_Size_Distribution, rank, dims, grainSizeDistribution);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5StatsDataDelegate::writeBinNumbers(StatsData* data, hid_t groupId)
{
  // Ensure we have valid bin numbers
  if(NULL == data->getBinNumbers().get())
  {
    data->generateBinNumbers();
  }
  return data->getBinNumbers()->writeH5Data(groupId);
}
