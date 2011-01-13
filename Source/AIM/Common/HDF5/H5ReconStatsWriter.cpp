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
int H5ReconStatsWriter::writeSizeDistribution(double maxdiameter, double mindiameter,
                                              double binStepSize,
                                              double avglogdiam, double sdlogdiam)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  std::vector<hsize_t> dims(1);
  dims[0] = 3;

  std::vector<double> grainDiameterInfo(3);
  grainDiameterInfo[0] = binStepSize;
  grainDiameterInfo[1] = maxdiameter;
  grainDiameterInfo[2] = mindiameter;
  err = H5Lite::writeVectorDataset(gid, AIM::HDF5::Grain_Diameter_Info, dims, grainDiameterInfo);
  if (err < 0) { retErr = err; }


  std::vector<double> grainSizeInfo(2);
  grainSizeInfo[0] = avglogdiam;
  grainSizeInfo[1] = sdlogdiam;
  dims[0] = 2;
  err = H5Lite::writeVectorDataset(gid, AIM::HDF5::Grain_Size_Distribution, dims, grainSizeInfo);
  if (err < 0) { retErr = err; }

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> H5ReconStatsWriter::generateBins(double maxDiameter, double minDiameter, double diameterStep)
{
  std::vector<double> bins;
  double d = minDiameter;
  while (d <= maxDiameter)
  {
    bins.push_back(d);
    d = d + diameterStep;
  }
  return bins;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeBetaDistribution(const std::string &hdf5GroupName,
                           std::vector<double> &bins,
                           std::vector<double> &alpha,
                           std::vector<double> &beta)
{
  herr_t err = 0;

  std::vector<std::string> headers;
  headers.push_back(AIM::HDF5::BinNumber);
  headers.push_back(AIM::HDF5::Alpha);
  headers.push_back(AIM::HDF5::Beta);

  std::vector<std::vector<double> > data(3);
  size_t i = 0;
  data[i++] = bins;
  data[i++] = alpha;
  data[i++] = beta;
  err = writeDistributionData(AIM::HDF5::BetaDistribution, hdf5GroupName, headers, data);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeLogNormalDistribution(const std::string &hdf5GroupName,
                                                    std::vector<double> &bins,
                                                    std::vector<double> &average,
                                                    std::vector<double> &stdDev )
{
  herr_t err = 0;

  std::vector<std::string> headers;
  headers.push_back(AIM::HDF5::BinNumber);
  headers.push_back(AIM::HDF5::Average);
  headers.push_back(AIM::HDF5::StandardDeviation);

  std::vector<std::vector<double> > data(3);
  size_t i = 0;
  data[i++] = bins;
  data[i++] = average;
  data[i++] = stdDev;
  err = writeDistributionData(AIM::HDF5::LogNormalDistribution, hdf5GroupName, headers, data);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writePowerDistribution(const std::string &hdf5GroupName,
                                               std::vector<double> &bins,
                                               std::vector<double> &alpha,
                                               std::vector<double> &k,
                                               std::vector<double> &beta)
{
  herr_t err = 0;

  std::vector<std::string> headers;
  headers.push_back(AIM::HDF5::BinNumber);
  headers.push_back(AIM::HDF5::Alpha);
  headers.push_back(AIM::HDF5::Exp_k);
  headers.push_back(AIM::HDF5::Beta);

  std::vector<std::vector<double> > data(3);
  size_t i = 0;
  data[i++] = bins;
  data[i++] = alpha;
  data[i++] = k;
  data[i++] = beta;
  err = writeDistributionData(AIM::HDF5::PowerLawDistribution, hdf5GroupName, headers, data);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeDistributionData(const std::string &disType,
                          const std::string &hdf5GroupName,
                          std::vector<std::string> &columnHeaders,
                          std::vector<std::vector<double> > &colData)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)
  std::vector<hsize_t> dims(1);


  hid_t disId = H5Utilities::createGroup(gid, hdf5GroupName);
  if (disId > 0)
  {
    err = H5Lite::writeStringAttribute(fileId, hdf5GroupName, AIM::HDF5::DistributionType, disType);
    if (err > 0)
    {
      dims.resize(1); // Single Dimension
      dims[0] = colData[0].size(); // How many elements in each column

      // Loop through all the column data and write each one to the HDF5 file
      size_t numColumns = colData.size();
      for (size_t c = 0; c < numColumns; ++c)
      {
        err = H5Lite::writeVectorDataset(disId, columnHeaders[c], dims, colData[c]);
        if (err < 0) { retErr = err;}
        break;
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
  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeVolumeStats(double maxdiameter, double mindiameter, double diamStepSize,
                                         double avglogdiam, double sdlogdiam,
                                         std::vector<std::vector<double> > &svbovera,
                                         std::vector<std::vector<double> > &svcovera,
                                         std::vector<std::vector<double> > &svcoverb,
                                         std::vector<std::vector<double> > &neighborhoodfit,
                                         std::vector<std::vector<double> > &svomega3)
{
  int err = 0;
  int retErr = 0;

  std::vector<double> binNum = generateBins((double)maxdiameter, (double)mindiameter, diamStepSize);
  err = writeSizeDistribution((double)maxdiameter, (double)mindiameter, 1.0, avglogdiam, sdlogdiam);
  if (err < 0) { retErr = err; }

/* Write the Grain_SizeVBoverA_Distributions Shape Statistics which are a Beta Distribution */
  {
    std::vector<double> alpha(binNum.size(), 0.0);
    std::vector<double> beta(binNum.size(), 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < binNum.size(); ++temp7)
    {
      alpha[temp7] = svbovera[temp7 + mindiameter][3];
      beta[temp7] = svbovera[temp7 + mindiameter][4];
    }
    err = writeBetaDistribution(AIM::HDF5::Grain_SizeVBoverA_Distributions, binNum, alpha, beta);
    if (err < 0) { retErr = err; }
  }

  /* Write the Grain_SizeVCoverA_Distributions Shape Statistics which are a Beta Distribution */
  {
    std::vector<double> alpha(binNum.size(), 0.0);
    std::vector<double> beta(binNum.size(), 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < binNum.size(); ++temp7)
    {
      alpha[temp7] = svcovera[temp7 + mindiameter][3];
      beta[temp7] = svcovera[temp7 + mindiameter][4];
    }
    err = writeBetaDistribution(AIM::HDF5::Grain_SizeVCoverA_Distributions, binNum, alpha, beta);
    if (err < 0) { retErr = err; }
  }

  /* Write the Grain_SizeVCoverB_Distributions Shape Statistics which are a Beta Distribution */
  {
    std::vector<double> alpha(binNum.size(), 0.0);
    std::vector<double> beta(binNum.size(), 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < binNum.size(); ++temp7)
    {
      alpha[temp7] = svcoverb[temp7 + mindiameter][3];
      beta[temp7] = svcoverb[temp7 + mindiameter][4];
    }
    err = writeBetaDistribution(AIM::HDF5::Grain_SizeVCoverB_Distributions, binNum, alpha, beta);
    if (err < 0) { retErr = err; }
  }

  /* Write the Grain_SizeVNeighbors_Distributions Neighbor Statistics which is a Power Law Distribution */
  {
    std::vector<double> alpha(binNum.size(), 0.0);
    std::vector<double> beta(binNum.size(), 0.0);
    std::vector<double> k(binNum.size(), 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < binNum.size(); ++temp7)
    {
      alpha[temp7] = neighborhoodfit[temp7 + mindiameter][1];
      beta[temp7] = neighborhoodfit[temp7 + mindiameter][2];
      k[temp7] = neighborhoodfit[temp7 + mindiameter][3];
    }
    err = writePowerDistribution(AIM::HDF5::Grain_SizeVNeighbors_Distributions, binNum, alpha, k, beta);
    if (err < 0) { retErr = err; }
  }

  /* Write the Grain_SizeVOmega3_Distributions Shape Statistics which is a LogNormal Distribution */
  {
    std::vector<double> average(binNum.size(), 0.0);
    std::vector<double> stdDev(binNum.size(), 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < binNum.size(); ++temp7)
    {
      average[temp7] = svomega3[temp7 + mindiameter][3];
      stdDev[temp7] = svomega3[temp7 + mindiameter][4];
    }
    err = writeLogNormalDistribution(AIM::HDF5::Grain_SizeVOmega3_Distributions, binNum, average, stdDev);
    if (err < 0) { retErr = err; }
  }
}



#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeNeighborDistribution(int maxdiameter, int mindiameter,
                                              const std::vector<std::vector<double> > &neighborhoodfit)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)
  std::vector<hsize_t> dims(1);
  dims[0] = 3;
  std::vector<int> binNum((maxdiameter) + 1);

  /* ************   Grain_SizeVNeighbors_Distributions *************************** */
  hid_t disId = H5Utilities::createGroup(gid, AIM::HDF5::Grain_SizeVNeighbors_Distributions);
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

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeShapeDistribution(const std::string &name,
                                               int maxdiameter, int mindiameter,
                                               const std::vector<std::vector<double> > &data )
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)
  std::vector<hsize_t > dims(1);
  dims[0] = 3;

  /* ************   Shape Distribution *************************** */
  hid_t disId = H5Utilities::createGroup(gid, name);
  std::vector<int> binNum((maxdiameter) + 1);
  std::vector<double> average((maxdiameter) + 1, 0.0);
  std::vector<double> stdDev((maxdiameter) + 1, 0.0);
  // Convert from Row Major to Column Major
  for (int temp7 = mindiameter; temp7 < (maxdiameter) + 1; temp7++)
  {
    binNum[temp7] = temp7;
    average[temp7] = data[temp7][3];
    stdDev[temp7] = data[temp7][4];
  }
  dims.resize(1); // Single Dimension
  dims[0] = binNum.size();
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::BinNumber, dims, binNum);
  //TODO: Check Error
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::Average, dims, average);
  err = H5Lite::writeVectorDataset(disId, AIM::HDF5::StandardDeviation, dims, stdDev);
  err = H5Gclose(disId);

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeOmega3Distribution(int maxdiameter, int mindiameter,
                            const std::vector<std::vector<double> > &svomega3)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)
  std::vector<hsize_t> dims(1);
  dims[0] = 3;
  std::vector<int> binNum((maxdiameter) + 1);
  std::vector<double> average((maxdiameter) + 1, 0.0);
  std::vector<double> stdDev((maxdiameter) + 1, 0.0);

  /* ************   Grain_SizeVOmega3_Distributions *************************** */
  hid_t disId = H5Utilities::createGroup(gid, AIM::HDF5::Grain_SizeVOmega3_Distributions);
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
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeStatsData(int maxdiameter, int mindiameter,
                                       double avglogdiam, double sdlogdiam,
                                       const std::vector<std::vector<double> > &neighborhoodfit,
                                       const std::vector<std::vector<double> > &svbovera,
                                       const std::vector<std::vector<double> > &svcovera,
                                       const std::vector<std::vector<double> > &svcoverb,
                                       const std::vector<std::vector<double> > &svomega3)
{
  herr_t err = 0;
  herr_t retErr = 0;
  err = writeSizeDistribution(maxdiameter, mindiameter, avglogdiam, sdlogdiam);
  if (err < 0) { retErr = err; }
  err = writeShapeDistribution(AIM::HDF5::Grain_SizeVBoverA_Distributions, maxdiameter, mindiameter, svbovera);
  if (err < 0) { retErr = err; }
  err = writeShapeDistribution(AIM::HDF5::Grain_SizeVCoverA_Distributions, maxdiameter, mindiameter, svcovera);
  if (err < 0) { retErr = err; }
  err = writeShapeDistribution(AIM::HDF5::Grain_SizeVCoverB_Distributions, maxdiameter, mindiameter, svcoverb);
  if (err < 0) { retErr = err; }
  err = writeNeighborDistribution(maxdiameter, mindiameter,neighborhoodfit);
  if (err < 0) { retErr = err; }
  err = writeOmega3Distribution(maxdiameter, mindiameter,svomega3);
  if (err < 0) { retErr = err; }

  return retErr;
}
#endif

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
  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
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

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
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

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}
