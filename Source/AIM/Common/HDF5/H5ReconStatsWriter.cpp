/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#include <string>

#include "MXA/HDF5/H5Utilities.h"
#include "MXA/HDF5/H5Lite.h"
#include "MXA/MXATypes.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/StringUtils.h"

#include "AIM/Common/Constants.h"
#include "AIM/Common/CrystalStructure.h"
#include "AIM/Common/PhaseType.h"

#define OPEN_HDF5_FILE(filename)\
  hid_t fileId = H5Utilities::openFile(filename, false);\
  if (fileId < 0) { return fileId; }

#define CREATE_RECONSTRUCTION_GROUP(name)\
  hid_t gid = H5Utilities::createGroup(fileId, name);\
  if (gid < 0) { \
    err = H5Utilities::closeFile(fileId);\
    return -1; }


#define H5RSW_ERROR_CHECK(name)\
    std::cout << "Error writing HDF5 data to " << name << std::endl;\
    std::cout << "  File: " << __FILE__ << std::endl;\
    std::cout << "  Line: " << __LINE__ << std::endl;

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
int H5ReconStatsWriter::writePhaseInformation(int phase,
                                              AIM::Reconstruction::PhaseType  phaseType,
                                              AIM::Reconstruction::CrystalStructure xtal,
                                              float phasefraction)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  hid_t pid = H5Utilities::createGroup(gid, StringUtils::numToString(phase));
  std::vector<hsize_t> dims(1);
  dims[0] = 3;

  /* **** Write the CrystalStructure Information ****** */
  unsigned int crystruct = static_cast<unsigned int>(xtal);
  err = H5Lite::writeScalarDataset(pid, AIM::HDF5::CrystalStructure, crystruct);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::CrystalStructure)
    retErr = err;
  }
  std::string xtalName = CrystalStructure::getCrystalStructureString(xtal);
  std::string name("Name");
  err = H5Lite::writeStringAttribute(pid, AIM::HDF5::CrystalStructure, name, xtalName);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::CrystalStructure)
    retErr = err;
  }


/* **** Write the Phase Type Information ****** */
  unsigned int pt = static_cast<unsigned int>(phaseType);
  err = H5Lite::writeScalarDataset(pid, AIM::HDF5::PhaseType, pt);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::PhaseType)
    retErr = err;
  }
  std::string ptName = PhaseType::getPhaseTypeString(phaseType);
  name = std::string("Name");
  err = H5Lite::writeStringAttribute(pid, AIM::HDF5::PhaseType, name, ptName);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::PhaseType)
    retErr = err;
  }

  /* **** Write the Phase Fraction Information ****** */
  err = H5Lite::writeScalarDataset(pid, AIM::HDF5::PhaseFraction, phasefraction);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::PhaseFraction)
    retErr = err;
  }

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(pid);
  if (err < 0) { retErr = err; }
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeSizeDistribution(int phase, float maxdiameter, float mindiameter,
                                              float binStepSize,
                                              float avglogdiam, float sdlogdiam,
                                              size_t &numberOfBins)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  hid_t pid = H5Utilities::createGroup(gid, StringUtils::numToString(phase));
  std::vector<hsize_t> dims(1);
  dims[0] = 3;

  std::vector<float> grainDiameterInfo(3);
  grainDiameterInfo[0] = binStepSize;
  grainDiameterInfo[1] = maxdiameter;
  grainDiameterInfo[2] = mindiameter;
  err = H5Lite::writeVectorDataset(pid, AIM::HDF5::Grain_Diameter_Info, dims, grainDiameterInfo);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Grain_Diameter_Info)
    retErr = err;
  }


  std::vector<float> grainSizeInfo(2);
  grainSizeInfo[0] = avglogdiam;
  grainSizeInfo[1] = sdlogdiam;
  dims[0] = 2;
  err = H5Lite::writeVectorDataset(pid, AIM::HDF5::Grain_Size_Distribution, dims, grainSizeInfo);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Grain_Size_Distribution)
    retErr = err;
  }

  std::vector<float> bins;
  float d = mindiameter;
  while (d <= maxdiameter)
  {
    bins.push_back(d);
    d = d + binStepSize;
  }
  numberOfBins = bins.size();
  dims[0] = numberOfBins;
  err = H5Lite::writeVectorDataset(pid, AIM::HDF5::BinNumber, dims, bins);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::BinNumber)
    retErr = err;
  }

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(pid);
  if (err < 0) { retErr = err; }
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> H5ReconStatsWriter::generateBins(int phase, float maxDiameter, float minDiameter, float diameterStep)
{
  std::vector<float> bins;
  float d = minDiameter;
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
int H5ReconStatsWriter::writeBetaDistribution(int phase, const std::string &hdf5GroupName,
                           std::vector<float> &alpha,
                           std::vector<float> &beta)
{
  herr_t err = 0;

  std::vector<std::string> headers;
  headers.push_back(AIM::HDF5::Alpha);
  headers.push_back(AIM::HDF5::Beta);

  std::vector<std::vector<float> > data(AIM::HDF5::BetaColumnCount);
  size_t i = 0;
  data[i++] = alpha;
  data[i++] = beta;
  err = writeDistributionData(phase, AIM::HDF5::BetaDistribution, hdf5GroupName, headers, data);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeLogNormalDistribution(int phase, const std::string &hdf5GroupName,
                                                    std::vector<float> &average,
                                                    std::vector<float> &stdDev )
{
  herr_t err = 0;

  std::vector<std::string> headers;
  headers.push_back(AIM::HDF5::Average);
  headers.push_back(AIM::HDF5::StandardDeviation);

  std::vector<std::vector<float> > data(AIM::HDF5::LogNormalColumnCount);
  size_t i = 0;
  data[i++] = average;
  data[i++] = stdDev;
  err = writeDistributionData(phase, AIM::HDF5::LogNormalDistribution, hdf5GroupName, headers, data);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writePowerDistribution(int phase, const std::string &hdf5GroupName,
                                               std::vector<float> &alpha,
                                               std::vector<float> &k,
                                               std::vector<float> &beta)
{
  herr_t err = 0;

  std::vector<std::string> headers;
  headers.push_back(AIM::HDF5::Alpha);
  headers.push_back(AIM::HDF5::Exp_k);
  headers.push_back(AIM::HDF5::Beta);

  std::vector<std::vector<float> > data(AIM::HDF5::PowerLawColumnCount);
  size_t i = 0;
  data[i++] = alpha;
  data[i++] = k;
  data[i++] = beta;
  err = writeDistributionData(phase, AIM::HDF5::PowerLawDistribution, hdf5GroupName, headers, data);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeDistributionData(int phase, const std::string &disType,
                          const std::string &hdf5GroupName,
                          std::vector<std::string> &columnHeaders,
                          std::vector<std::vector<float> > &colData)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)
  std::vector<hsize_t> dims(1);

  hid_t pid = H5Utilities::createGroup(gid, StringUtils::numToString(phase));
  hid_t disId = H5Utilities::createGroup(pid, hdf5GroupName);
  if (disId > 0)
  {
    err = H5Lite::writeStringAttribute(pid, hdf5GroupName, AIM::HDF5::DistributionType, disType);
    if (err >= 0)
    {
      dims.resize(1); // Single Dimension
      dims[0] = colData[0].size(); // How many elements in each column

      // Loop through all the column data and write each one to the HDF5 file
      size_t numColumns = colData.size();
      for (size_t c = 0; c < numColumns; ++c)
      {
        //std::cout << "Writing Dataset:" << hdf5GroupName << "/" << columnHeaders[c] << std::endl;
        err = -1;
        if (colData[c].size() > 0) {
          err = H5Lite::writeVectorDataset(disId, columnHeaders[c], dims, colData[c]);
          if (err < 0)
          {
            H5RSW_ERROR_CHECK(columnHeaders[c])
            retErr = err;
            break;
          }
        }
        else
        {
          std::cout << hdf5GroupName << ":" << columnHeaders[c] << " Data Column had no data. Did you create the data?"  << std::endl;
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
  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(pid);
  if (err < 0) { retErr = err; }
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeVolumeStats(int phase, AIM::Reconstruction::CrystalStructure xtal,
                                         AIM::Reconstruction::PhaseType phaseType,
                                         float phasefraction, float maxdiameter, float mindiameter, float diamStepSize,
                                         float avglogdiam, float sdlogdiam, std::vector<std::vector<float> > &svbovera,
                                         std::vector<std::vector<float> > &svcovera, std::vector<std::vector<float> > &svcoverb,
                                         std::vector<std::vector<float> > &neighborhoodfit, std::vector<std::vector<float> > &svomega3)
{
  int err = 0;
  int retErr = 0;

 // std::vector<float> binNum; // = generateBins((float)maxdiameter, (float)mindiameter, diamStepSize);
  size_t nBins = 0; // Used as a variable that will get written to in the writeSizeDistribution() method

  err = writePhaseInformation(phase, phaseType, xtal, phasefraction);
  if (err < 0) { retErr = err; }

  err = writeSizeDistribution(phase, (float)maxdiameter, (float)mindiameter, diamStepSize, avglogdiam, sdlogdiam, nBins);
  if (err < 0) { retErr = err; }

/* Write the Grain_SizeVBoverA_Distributions Shape Statistics which are a Beta Distribution */
  {
    std::vector<float> alpha(nBins, 0.0);
    std::vector<float> beta(nBins, 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < nBins; ++temp7)
    {
      alpha[temp7] = svbovera[temp7][3];
      beta[temp7] = svbovera[temp7][4];
    }
    err = writeBetaDistribution(phase, AIM::HDF5::Grain_SizeVBoverA_Distributions, alpha, beta);
    if (err < 0)
    {
      H5RSW_ERROR_CHECK(AIM::HDF5::Grain_SizeVBoverA_Distributions)
      retErr = err;
    }
  }

  /* Write the Grain_SizeVCoverA_Distributions Shape Statistics which are a Beta Distribution */
  {
    std::vector<float> alpha(nBins, 0.0);
    std::vector<float> beta(nBins, 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < nBins; ++temp7)
    {
      alpha[temp7] = svcovera[temp7][3];
      beta[temp7] = svcovera[temp7][4];
    }
    err = writeBetaDistribution(phase, AIM::HDF5::Grain_SizeVCoverA_Distributions, alpha, beta);
    if (err < 0)
    {
      H5RSW_ERROR_CHECK(AIM::HDF5::Grain_SizeVCoverA_Distributions)
      retErr = err;
    }
  }

  /* Write the Grain_SizeVCoverB_Distributions Shape Statistics which are a Beta Distribution */
  {
    std::vector<float> alpha(nBins, 0.0);
    std::vector<float> beta(nBins, 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < nBins; ++temp7)
    {
      alpha[temp7] = svcoverb[temp7][3];
      beta[temp7] = svcoverb[temp7][4];
    }
    err = writeBetaDistribution(phase, AIM::HDF5::Grain_SizeVCoverB_Distributions, alpha, beta);
    if (err < 0)
    {
      H5RSW_ERROR_CHECK(AIM::HDF5::Grain_SizeVCoverB_Distributions)
      retErr = err;
    }
  }

  /* Write the Grain_SizeVNeighbors_Distributions Neighbor Statistics which is a Power Law Distribution */
  {
    std::vector<float> alpha(nBins, 0.0);
    std::vector<float> beta(nBins, 0.0);
    std::vector<float> k(nBins, 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < nBins; ++temp7)
    {
      alpha[temp7] = neighborhoodfit[temp7][1];
      beta[temp7] = neighborhoodfit[temp7][2];
      k[temp7] = neighborhoodfit[temp7][3];
    }
    err = writePowerDistribution(phase, AIM::HDF5::Grain_SizeVNeighbors_Distributions, alpha, k, beta);
    if (err < 0)
    {
      H5RSW_ERROR_CHECK(AIM::HDF5::Grain_SizeVNeighbors_Distributions)
      retErr = err;
    }
  }

  /* Write the Grain_SizeVOmega3_Distributions Shape Statistics which is a Beta Distribution */
  {
    std::vector<float> alpha(nBins, 0.0);
    std::vector<float> beta(nBins, 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < nBins; ++temp7)
    {
      alpha[temp7] = svomega3[temp7][3];
      beta[temp7] = svomega3[temp7][4];
    }
    err = writeBetaDistribution(phase, AIM::HDF5::Grain_SizeVOmega3_Distributions, alpha, beta);
    if (err < 0)
    {
      H5RSW_ERROR_CHECK(AIM::HDF5::Grain_SizeVOmega3_Distributions)
      retErr = err;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeVolumeStats2D(int phase,
                                           AIM::Reconstruction::CrystalStructure xtal,
                                           AIM::Reconstruction::PhaseType phaseType,
                                           float phasefraction, float maxdiameter, float mindiameter, float diamStepSize,
                                         float avglogdiam, float sdlogdiam, std::vector<std::vector<float> > &svbovera,
                                         std::vector<std::vector<float> > &neighborhoodfit)
{
  int err = 0;
  int retErr = 0;

 // std::vector<float> binNum; // = generateBins((float)maxdiameter, (float)mindiameter, diamStepSize);
  size_t nBins = 0; // Used as a variable that will get written to in the writeSizeDistribution() method
  err = writePhaseInformation(phase, phaseType, xtal, phasefraction);
  if (err < 0) { retErr = err; }

  err = writeSizeDistribution(phase, (float)maxdiameter, (float)mindiameter, diamStepSize, avglogdiam, sdlogdiam, nBins);
  if (err < 0) { retErr = err; }

/* Write the Grain_SizeVBoverA_Distributions Shape Statistics which are a Beta Distribution */
  {
    std::vector<float> alpha(nBins, 0.0);
    std::vector<float> beta(nBins, 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < nBins; ++temp7)
    {
      alpha[temp7] = svbovera[temp7][3];
      beta[temp7] = svbovera[temp7][4];
    }
    err = writeBetaDistribution(phase, AIM::HDF5::Grain_SizeVBoverA_Distributions, alpha, beta);
    if (err < 0)
    {
      H5RSW_ERROR_CHECK(AIM::HDF5::Grain_SizeVBoverA_Distributions)
      retErr = err;
    }
  }

  /* Write the Grain_SizeVNeighbors_Distributions Neighbor Statistics which is a Power Law Distribution */
  {
    std::vector<float> alpha(nBins, 0.0);
    std::vector<float> beta(nBins, 0.0);
    std::vector<float> k(nBins, 0.0);
    // Convert from Row Major to Column Major
    for (size_t temp7 = 0; temp7 < nBins; ++temp7)
    {
      alpha[temp7] = neighborhoodfit[temp7][1];
      beta[temp7] = neighborhoodfit[temp7][2];
      k[temp7] = neighborhoodfit[temp7][3];
    }
    err = writePowerDistribution(phase, AIM::HDF5::Grain_SizeVNeighbors_Distributions, alpha, k, beta);
    if (err < 0)
    {
      H5RSW_ERROR_CHECK(AIM::HDF5::Grain_SizeVNeighbors_Distributions)
      retErr = err;
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeMisorientationBinsData(int phase, unsigned long long int* nElements, float* misobins)
{
  herr_t err = 0;
  herr_t retErr = 0;
  if ( *nElements == 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::MisorientationBins)
    return -1;
  }
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)
  hid_t pid = H5Utilities::createGroup(gid, StringUtils::numToString(phase));
  int32_t rank = 1; // Single Dimension

  err = H5Lite::writePointerDataset<float>(pid, AIM::HDF5::MisorientationBins, rank, nElements, misobins);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::MisorientationBins)
    retErr = err;
  }
  err = H5Gclose(pid);
  err = H5Gclose(gid);
  err = H5Utilities::closeFile(fileId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeMDFWeights(int phase, uint64_t* dims, float* angles, float* axes, float* weights)
{
  herr_t err = 0;
  herr_t retErr = 0;
  if ( *dims == 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::MisorientationBins)
    return -1;
  }
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)
  hid_t pid = H5Utilities::createGroup(gid, StringUtils::numToString(phase));
  int32_t rank = 1; // Single Dimension

  hid_t wid = H5Utilities::createGroup(pid, AIM::HDF5::MDFWeights);

  err = H5Lite::writePointerDataset<float>(wid, AIM::HDF5::Angle, rank, dims, angles);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Angle);
    retErr = err;
  }
  err = H5Lite::writePointerDataset<float>(wid, AIM::HDF5::Axis, rank, dims, axes);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Axis); retErr = err;
  }
  err = H5Lite::writePointerDataset<float>(wid, AIM::HDF5::Weight, rank, dims, weights);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Weight); retErr = err;
  }

  err = H5Gclose(wid);
  if (err < 0) { retErr = err;}
  err = H5Gclose(pid);
  if (err < 0) { retErr = err;}
  err = H5Gclose(gid);
  if (err < 0) { retErr = err;}
  err = H5Utilities::closeFile(fileId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeMicroTextureData(int phase, float* microbin, size_t nElements, float actualgrains)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  hid_t pid = H5Utilities::createGroup(gid, StringUtils::numToString(phase));
  std::vector<hsize_t> dims(1);
  dims[0] = static_cast<hsize_t>(nElements);
  std::vector<float> data(nElements, 0.0);
  for (size_t i = 0; i < nElements; ++i)
  {
    data[i] = microbin[i]/actualgrains;
  }
  err = H5Lite::writeVectorDataset(pid, AIM::HDF5::MicroTextureBins, dims, data);
  if (err < 0)
  {
    retErr = err;
  }
  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(pid);
  if (err < 0) { retErr = err; }
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeAxisOrientationData(int phase, float* axisodf, float totalaxes)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  hid_t pid = H5Utilities::createGroup(gid, StringUtils::numToString(phase));
  int size = 36 * 36 * 36;
  std::vector<float> data(size, 0.0);
  for (int i = 0; i < size; ++i)
  {
    data[i] = axisodf[i] / totalaxes;
  }

  std::vector<hsize_t> dims(1);
  dims[0] = size;
  err = H5Lite::writeVectorDataset(pid, AIM::HDF5::AxisOrientation, dims, data);
  if (err < 0)
  {
    retErr = err;
  }

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(pid);
  if (err < 0)
  {
    retErr = err;
  }
  err = H5Gclose(gid);
  if (err < 0)
  {
    retErr = err;
  }
  err = H5Utilities::closeFile(fileId);
  if (err < 0)
  {
    retErr = err;
  }
  return retErr;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeODFData(int phase,
                                     unsigned long long* dims,
                                     float* eulerodf)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  hid_t pid = H5Utilities::createGroup(gid, StringUtils::numToString(phase));
  int32_t rank = 1;
  err = H5Lite::writePointerDataset<float>(pid, AIM::HDF5::ODF, rank, dims, eulerodf);
  if (err < 0)
  {
   retErr = err;
  }

  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(pid);
  if (err < 0) { retErr = err; }
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5ReconStatsWriter::writeODFWeights(int phase, unsigned long long int *dims,
                                                  float* e1, float* e2, float* e3,
                                                  float* weights, float* sigmas)
{
  herr_t err = 0;
  herr_t retErr = 0;
  OPEN_HDF5_FILE(m_FileName)
  CREATE_RECONSTRUCTION_GROUP(AIM::HDF5::Reconstruction)

  hid_t pid = H5Utilities::createGroup(gid, StringUtils::numToString(phase));
  int32_t rank = 1;

  hid_t wid = H5Utilities::createGroup(pid, AIM::HDF5::ODFWeights);


  err = H5Lite::writePointerDataset<float>(wid, AIM::HDF5::Euler1, rank, dims, e1);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Euler1); retErr = err;
  }
  err = H5Lite::writePointerDataset<float>(wid, AIM::HDF5::Euler2, rank, dims, e2);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Euler2); retErr = err;
  }
  err = H5Lite::writePointerDataset<float>(wid, AIM::HDF5::Euler3, rank, dims, e3);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Euler3); retErr = err;
  }
  err = H5Lite::writePointerDataset<float>(wid, AIM::HDF5::Weight, rank, dims, weights);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Weight); retErr = err;
  }
  err = H5Lite::writePointerDataset<float>(wid, AIM::HDF5::Sigma, rank, dims, sigmas);
  if (err < 0)
  {
    H5RSW_ERROR_CHECK(AIM::HDF5::Sigma); retErr = err;
  }
  /* Clean up the rest of the HDF5 structures and close the file */
  err = H5Gclose(wid);
  if (err < 0) { retErr = err; }
  err = H5Gclose(pid);
  if (err < 0) { retErr = err; }
  err = H5Gclose(gid);
  if (err < 0) { retErr = err; }
  err = H5Utilities::closeFile(fileId);
  if (err < 0) { retErr = err; }
  return retErr;
}







