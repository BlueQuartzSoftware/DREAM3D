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

#include "H5ReconStatsWriter.h"

#include "AIM/Common/Constants.h"
#include "AIM/Common/Bin.h"
#include "MXA/HDF5/H5Utilities.h"
#include "MXA/HDF5/H5Lite.h"


#define OPEN_HDF5_FILE(filename)\
  hid_t fileId = H5Utilities::openFile(filename, false);\
  if (fileId < 0) { return fileId; }

#define CREATE_RECONSTRUCTION_GROUP(name)\
  hid_t gid = H5Utilities::createGroup(fileId, name);\
  if (gid < 0) { \
    err = H5Utilities::closeFile(fileId);\
    return -1; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5ReconStatsWriter::H5ReconStatsWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5ReconStatsWriter::~H5ReconStatsWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5ReconStatsWriter::Pointer H5ReconStatsWriter::New(const std::string &filename)
{
  Pointer sharedPtr(new H5ReconStatsWriter);
  sharedPtr->setFileName(filename);
  hid_t fileId = H5Utilities::createFile(filename);
  if (fileId < 0)
  {
    sharedPtr = NullPointer();
  }
  else
  {
    H5Utilities::closeFile(fileId);
  }
  return sharedPtr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeStatsData(int maxdiameter, int mindiameter,
                                       double avglogdiam, double sdlogdiam, double actualgrains,
                                       const std::vector<std::vector<double> > &neighborhoodfit,
                                       const std::vector<std::vector<double> > &svbovera,
                                       const std::vector<std::vector<double> > &svcovera,
                                       const std::vector<std::vector<double> > &svcoverb,
                                       const std::vector<std::vector<double> > &svschmid,
                                       const std::vector<std::vector<double> > &svomega3)
{
  herr_t err = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  std::vector<hsize_t> dims(1);
  dims[0] = 3;

  std::vector<int> int_data(3);
  int_data[0] = (maxdiameter - mindiameter) + 1;
  int_data[1] = maxdiameter;
  int_data[2] = mindiameter;
  err = H5Lite::writeVectorDataset(gid, AIM::HDF5::Grain_Diameter_Info, dims, int_data);
  //TODO: Check for error

  std::vector<double> double_data(2);
  double_data[0] = avglogdiam;
  double_data[1] = sdlogdiam;
  err = H5Lite::writeVectorDataset(gid, AIM::HDF5::Grain_Size_Distribution, dims, double_data);
  //TODO: Check for error

/* ************   Grain_SizeVBoverA_Distributions *************************** */
  hid_t disId = H5Utilities::createGroup(gid, AIM::HDF5::Grain_SizeVBoverA_Distributions);
  std::vector<int> binNum((maxdiameter) + 1);
  std::vector<double> average((maxdiameter) + 1, 0.0);
  std::vector<double> stdDev((maxdiameter) + 1, 0.0);
  // Convert from Row Major to Column Major
  for (int temp7 = mindiameter; temp7 < (maxdiameter) + 1; temp7++)
  {
    binNum[temp7] = temp7;
    average[temp7] = svbovera[temp7][3];
    stdDev[temp7] = svbovera[temp7][4];
  }
  dims.resize(1); // Single Dimension
  dims[0] = binNum.size();
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::BinNumber, dims, binNum);
  //TODO: Check Error
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::Average, dims, average);
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::StandardDeviation, dims, stdDev);
  err = H5Gclose(disId);



  /* ************   Grain_SizeVCoverA_Distributions *************************** */
  disId = H5Utilities::createGroup(gid, AIM::HDF5::Grain_SizeVCoverA_Distributions);
  for (int temp7 = mindiameter; temp7 < (maxdiameter) + 1; temp7++)
  {
    binNum[temp7] = temp7;
    average[temp7] = svcovera[temp7][3];
    stdDev[temp7] = svcovera[temp7][4];
  }
  dims.resize(1); // Single Dimension
  dims[0] = binNum.size();
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::BinNumber, dims, binNum);
  //TODO: Check Error
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::Average, dims, average);
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::StandardDeviation, dims, stdDev);
  err = H5Gclose(disId);


  /* ************   Grain_SizeVCoverB_Distributions *************************** */
  disId = H5Utilities::createGroup(gid, AIM::HDF5::Grain_SizeVCoverB_Distributions);
  for (int temp7 = mindiameter; temp7 < (maxdiameter) + 1; temp7++)
  {
    binNum[temp7] = temp7;
    average[temp7] = svcoverb[temp7][3];
    stdDev[temp7] = svcoverb[temp7][4];
  }
  dims.resize(1); // Single Dimension
  dims[0] = binNum.size();
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::BinNumber, dims, binNum);
  //TODO: Check Error
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::Average, dims, average);
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::StandardDeviation, dims, stdDev);
  err = H5Gclose(disId);

  /* ************   Grain_SizeVNeighbors_Distributions *************************** */
  disId = H5Utilities::createGroup(gid, AIM::HDF5::Grain_SizeVNeighbors_Distributions);
  std::vector<double> a((maxdiameter) + 1, 0.0);
  std::vector<double> b((maxdiameter) + 1, 0.0);
  std::vector<double> k((maxdiameter) + 1, 0.0);
  for (int temp7 = mindiameter; temp7 < (maxdiameter) + 1; temp7++)
  {
    binNum[temp7] = temp7;
    a[temp7] = neighborhoodfit[temp7][1];
    b[temp7] =  neighborhoodfit[temp7][2];
    k[temp7] =  neighborhoodfit[temp7][3];
  }
  dims.resize(1); // Single Dimension
  dims[0] = binNum.size();
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::BinNumber, dims, binNum);
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::alpha, dims, a);
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::beta, dims, b);
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::Exp_k, dims, k);
  err = H5Gclose(disId);

  /* ************   Grain_SizeVOmega3_Distributions *************************** */
  disId = H5Utilities::createGroup(gid, AIM::HDF5::Grain_SizeVOmega3_Distributions);
  for (int temp7 = mindiameter; temp7 < (maxdiameter) + 1; temp7++)
  {
    binNum[temp7] = temp7;
    average[temp7] = svomega3[temp7][3];
    stdDev[temp7] = svomega3[temp7][4];
  }
  dims.resize(1); // Single Dimension
  dims[0] = binNum.size();
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::BinNumber, dims, binNum);
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::Average, dims, average);
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::StandardDeviation, dims, stdDev);
  err = H5Gclose(disId);

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(gid);
  err = H5Utilities::closeFile(fileId);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeMisorientationBinsData(double* misobins, size_t nElements)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)
  int32_t rank = 1; // Single Dimension
  hsize_t dims = static_cast<hsize_t>(nElements);
  err = H5Lite::writePointerDataset<double>(gid, AIM::HDF5::MisorientationBins, rank, &dims, misobins);
  if (err < 0)
  {
    retErr = err;
  }
  err = H5Gclose(gid);
  err = H5Utilities::closeFile(fileId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeMicroTextureData(double* microbin, size_t nElements, double actualgrains)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  std::vector<hsize_t> dims(1);
  dims[0] = static_cast<hsize_t>(nElements);
  std::vector<double> data(nElements, 0.0);
  for (size_t i = 0; i < nElements; ++i)
  {
    data[i] = microbin[i]/actualgrains;
  }
  err = H5Lite::writeVectorDataset(gid, AIM::HDF5::MicroTextureBins, dims, data);
  if (err < 0)
  {
    retErr = err;
  }
  err = H5Gclose(gid);
  err = H5Utilities::closeFile(fileId);
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeAxisOrientationData(double* axisodf, double totalaxes)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  int size = 18 * 18 * 18;
  std::vector<double> data(size, 0.0);
  for (int i = 0; i < size; ++i)
  {
    data[i] = axisodf[i] / totalaxes;
  }

  std::vector<hsize_t> dims(1);
  dims[0] = size;
  err = H5Lite::writeVectorDataset(gid, AIM::HDF5::AxisOrientation, dims, data);
  if (err < 0)
  {
    retErr = err;
  }

  err = H5Gclose(gid);
  err = H5Utilities::closeFile(fileId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeODFData(AIM::Reconstruction::CrystalStructure crystruct,
                                     double* eulerodf, double totalvol)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)


  size_t numbins = 0;
  if (crystruct == AIM::Reconstruction::Hexagonal) numbins = 36 * 36 * 12;
  if (crystruct == AIM::Reconstruction::Cubic) numbins = 18 * 18 * 18;
  std::vector<double> data(numbins, 0.0);
  for (size_t i = 0; i < numbins; i++)
  {
    data[i] = eulerodf[i] / totalvol;
  }
  std::vector<hsize_t> dims(1);
  dims[0] = numbins;
  err = H5Lite::writeVectorDataset(gid, AIM::HDF5::ODF, dims, data);
  if (err < 0)
  {
   retErr = err;
  }

  err = H5Gclose(gid);
  err = H5Utilities::closeFile(fileId);

  return retErr;
}
