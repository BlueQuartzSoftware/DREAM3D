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

#include "H5PrecipitateStatsDataDelegate.h"

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5PrecipitateStatsDataDelegate::H5PrecipitateStatsDataDelegate()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5PrecipitateStatsDataDelegate::~H5PrecipitateStatsDataDelegate()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorOfFloatArray H5PrecipitateStatsDataDelegate::createBetaDistributionArrays()
{
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Alpha);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Beta);
  std::vector<FloatArrayType::Pointer> vect;
  vect.push_back(alphas);
  vect.push_back(betas);
  return vect;
}

VectorOfFloatArray H5PrecipitateStatsDataDelegate::createPowerDistributionArrays()
{
  FloatArrayType::Pointer alphas = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Alpha);
  FloatArrayType::Pointer ks = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Exp_k);
  FloatArrayType::Pointer betas = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Beta);
  std::vector<FloatArrayType::Pointer> vect;
  vect.push_back(alphas);
  vect.push_back(ks);
  vect.push_back(betas);
  return vect;
}

VectorOfFloatArray H5PrecipitateStatsDataDelegate::createLogNormalDistributionArrays()
{
  FloatArrayType::Pointer avgs = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Average);
  FloatArrayType::Pointer sd = FloatArrayType::CreateArray(0, DREAM3D::HDF5::StandardDeviation);
  std::vector<FloatArrayType::Pointer> vect;
  vect.push_back(avgs);
  vect.push_back(sd);
  return vect;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorOfFloatArray H5PrecipitateStatsDataDelegate::createDistributionVector(unsigned int distType)
{
  if (distType == DREAM3D::DistributionType::Beta)
  {
    return createBetaDistributionArrays();
  }
  else if (distType == DREAM3D::DistributionType::Power)
  {
    return createPowerDistributionArrays();
  }
  else if (distType == DREAM3D::DistributionType::LogNormal)
  {
    return createLogNormalDistributionArrays();
  }
   std::vector<FloatArrayType::Pointer> empty;
   return empty;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readPrecipitateStatsData(PrecipitateStatsData* data, hid_t groupId)
{
  int err = 0;
  //Read the NumFields
  err = readBoundaryArea(data, groupId);

  //Read the PhaseFraction
  err = readPhaseFraction(data, groupId);

  // Read the Precip Boundary Fraction
  err = readPrecipBoundaryFraction(data, groupId);

  // Read the Grain Diameter Info
  err = readGrainDiameterInfo(data, groupId);

  // Read the Grain Size Distribution
  uint32_t dType = readDistributionType(groupId, DREAM3D::HDF5::Grain_Size_Distribution);
  data->setGrainSize_DistType(dType);
  data->setGrainSizeDistribution(createDistributionVector(data->getGrainSize_DistType()));
  err = readDistributionData(groupId,
                DREAM3D::HDF5::Grain_Size_Distribution,
                data->getGrainSizeDistribution());

   // Read the Bin Numbers
   err = readBinNumbers(data, groupId);

   // Read the B Over A
   dType = readDistributionType(groupId, DREAM3D::HDF5::Grain_SizeVBoverA_Distributions);
   data->setBOverA_DistType(dType);
   data->setGrainSize_BOverA( createDistributionVector(data->getBOverA_DistType()));
   err = readDistributionData(groupId,
                                DREAM3D::HDF5::Grain_SizeVBoverA_Distributions,
                                data->getGrainSize_BOverA());

   // Read the C Over A
   dType = readDistributionType(groupId, DREAM3D::HDF5::Grain_SizeVCoverA_Distributions);
   data->setCOverA_DistType(dType);
   data->setGrainSize_COverA( createDistributionVector(data->getCOverA_DistType()));
   err = readDistributionData(groupId,
                                DREAM3D::HDF5::Grain_SizeVCoverA_Distributions,
                                data->getGrainSize_COverA());

   // Read the Neighbors
   dType = readDistributionType(groupId, DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions);
   data->setNeighbors_DistType(dType);
   data->setGrainSize_Neighbors( createDistributionVector(data->getNeighbors_DistType()));
   err = readDistributionData(groupId,
                                DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions,
                                data->getGrainSize_Neighbors());

   // Read the Omegas
   dType = readDistributionType(groupId, DREAM3D::HDF5::Grain_SizeVOmega3_Distributions);
   data->setOmegas_DistType(dType);
   data->setGrainSize_Omegas( createDistributionVector(data->getOmegas_DistType()));
   err = readDistributionData(groupId,
                                DREAM3D::HDF5::Grain_SizeVOmega3_Distributions,
                                data->getGrainSize_Omegas());

   // Read the Misorientation Bins
   FloatArrayType::Pointer misoBins = FloatArrayType::CreateArray(0, DREAM3D::HDF5::MisorientationBins);
   err = misoBins->readH5Data(groupId);
   if (err < 0)
   {
     misoBins = FloatArrayType::NullPointer();
   }
   data->setMisorientationBins(misoBins);
   err = readMDFWeights(groupId, data);

   // Read the ODF Data
   FloatArrayType::Pointer odfBins = FloatArrayType::CreateArray(0, DREAM3D::HDF5::ODF);
   err = odfBins->readH5Data(groupId);
   if (err < 0)
   {
     odfBins = FloatArrayType::NullPointer();
   }
   data->setODF(odfBins);
   err = readODFWeights(groupId, data);

   // Read the Axis ODF Data
   FloatArrayType::Pointer axisOdfBins = FloatArrayType::CreateArray(0, DREAM3D::HDF5::AxisOrientation);
   err = axisOdfBins->readH5Data(groupId);
   if (err < 0)
   {
     axisOdfBins = FloatArrayType::NullPointer();
   }
   data->setAxisOrientation(axisOdfBins);
   err = readAxisODFWeights(groupId, data);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writePrecipitateStatsData(PrecipitateStatsData* data, hid_t groupId)
{
  if (NULL == data)
  {
    return -1;
  }
  int err = 0;

  // Write the NumFields
  err = writeBoundaryArea(data, groupId);
  if (err < 0)
  {
    return err;
  }

  // Write the PhaseFraction
  err = writePhaseFraction(data, groupId);
  if (err < 0)
  {
    return err;
  }

  // Write the Precip Boundary Fraction
  err = writePrecipBoundaryFraction(data, groupId);

  // Write the Grain Diameter Info
  err = writeGrainDiameterInfo(data, groupId);

  // Write the Grain Size Distribution
  err = writeDistributionData(groupId,
                 data->getGrainSize_DistType(),
                 DREAM3D::HDF5::Grain_Size_Distribution,
                 data->getGrainSizeDistribution());

  // Write the Bin Numbers
  err = writeBinNumbers(data, groupId);

  // Write the B Over A
  err = writeDistributionData(groupId,
                               data->getBOverA_DistType(),
                               DREAM3D::HDF5::Grain_SizeVBoverA_Distributions,
                               data->getGrainSize_BOverA());

  // Write the C Over A
  err = writeDistributionData(groupId,
                               data->getCOverA_DistType(),
                               DREAM3D::HDF5::Grain_SizeVCoverA_Distributions,
                               data->getGrainSize_COverA());

  // Write the Neighbors
  err = writeDistributionData(groupId,
                               data->getNeighbors_DistType(),
                               DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions,
                               data->getGrainSize_Neighbors());

  // Write the Omegas
  err = writeDistributionData(groupId,
                               data->getOmegas_DistType(),
                               DREAM3D::HDF5::Grain_SizeVOmega3_Distributions,
                               data->getGrainSize_Omegas());

  // Write the Misorientation Bins
  if (NULL != data->getMisorientationBins().get())
  {
    err = data->getMisorientationBins()->writeH5Data(groupId);
  }

  err = writeWeightsData(groupId, DREAM3D::HDF5::MDFWeights, data->getMDF_Weights());


  // Write the ODF
  if (NULL != data->getODF().get())
  {
    err = data->getODF()->writeH5Data(groupId);
  }
  err = writeWeightsData(groupId, DREAM3D::HDF5::ODFWeights, data->getODF_Weights());


  // Write the Axis ODF
  if (NULL != data->getAxisOrientation().get())
  {
    err = data->getAxisOrientation()->writeH5Data(groupId);
  }
  err = writeWeightsData(groupId, DREAM3D::HDF5::AxisODFWeights, data->getAxisODF_Weights());

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeVectorOfArrays(hid_t pid,
                                           VectorOfFloatArray colData)
{
  herr_t err = 0;
  herr_t retErr = 0;

  // Loop through all the column data and write each one to the HDF5 file
  size_t numColumns = colData.size();
  for (size_t c = 0; c < numColumns; ++c)
  {
    //std::cout << "Writing Dataset:" << hdf5GroupName << "/" << columnHeaders[c] << std::endl;
    err = -1;
    if(NULL != colData[c].get() && colData[c]->GetSize() > 0)
    {
      err = colData[c]->writeH5Data(pid);
      if(err < 0)
      {
        retErr = err;
        break;
      }
    }
    else
    {
      std::cout << ":Null Data Column had no data. Did you create the data?" << std::endl;
      std::cout << "  File: " << __FILE__ << std::endl;
      std::cout << "  Line: " << __LINE__ << std::endl;
      break;
    }

  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readVectorOfArrays(hid_t pid, VectorOfFloatArray colData)
{
  int err = 0;

  for (VectorOfFloatArray::iterator iter = colData.begin(); iter != colData.end(); ++iter )
  {
    FloatArrayType::Pointer d = *iter;
    err = d->readH5Data(pid);
    if (err < 0) { return err; }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readMDFWeights(hid_t pid, PrecipitateStatsData* data)
{
  int err = 0;
  FloatArrayType::Pointer angles = FloatArrayType::CreateArray(1, DREAM3D::HDF5::Angle);

  FloatArrayType::Pointer axis = FloatArrayType::CreateArray(3, DREAM3D::HDF5::Axis);
  axis->SetNumberOfComponents(3);

  FloatArrayType::Pointer weight = FloatArrayType::CreateArray(1, DREAM3D::HDF5::Weight);

  VectorOfFloatArray mdfWeights;
  mdfWeights.push_back(angles);
  mdfWeights.push_back(axis);
  mdfWeights.push_back(weight);

  hid_t groupId = H5Utilities::openHDF5Object(pid, DREAM3D::HDF5::MDFWeights);

  err = readVectorOfArrays(groupId, mdfWeights);
  if (err >= 0) {
    data->setMDF_Weights(mdfWeights);
  }

  err = H5Utilities::closeHDF5Object(groupId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readODFWeights(hid_t pid, PrecipitateStatsData* data)
{

  int err = 0;

  FloatArrayType::Pointer euler1 = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Euler1);
  FloatArrayType::Pointer euler2 = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Euler2);
  FloatArrayType::Pointer euler3 = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Euler3);
  FloatArrayType::Pointer weight = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Weight);
  FloatArrayType::Pointer sigma = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Sigma);

  VectorOfFloatArray odfWeights;
  odfWeights.push_back(euler1);
  odfWeights.push_back(euler2);
  odfWeights.push_back(euler3);
  odfWeights.push_back(weight);
  odfWeights.push_back(sigma);

  hid_t groupId = H5Utilities::openHDF5Object(pid, DREAM3D::HDF5::ODFWeights);

  err = readVectorOfArrays(groupId, odfWeights);
  if(err >= 0)
  {
    data->setODF_Weights(odfWeights);
  }

  err |= H5Utilities::closeHDF5Object(groupId);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readAxisODFWeights(hid_t pid, PrecipitateStatsData* data)
{

  int err = 0;

  FloatArrayType::Pointer euler1 = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Euler1);
  FloatArrayType::Pointer euler2 = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Euler2);
  FloatArrayType::Pointer euler3 = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Euler3);
  FloatArrayType::Pointer sigma = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Sigma);
  FloatArrayType::Pointer weight = FloatArrayType::CreateArray(0, DREAM3D::HDF5::Weight);

  VectorOfFloatArray odfWeights;
  odfWeights.push_back(euler1);
  odfWeights.push_back(euler2);
  odfWeights.push_back(euler3);
  odfWeights.push_back(sigma);
  odfWeights.push_back(weight);

  hid_t groupId = H5Utilities::openHDF5Object(pid, DREAM3D::HDF5::AxisODFWeights);

  err = readVectorOfArrays(groupId, odfWeights);
  if(err >= 0)
  {
    data->setAxisODF_Weights(odfWeights);
  }

  err |= H5Utilities::closeHDF5Object(groupId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeWeightsData(hid_t pid, const std::string &hdf5GroupName,
                                          VectorOfFloatArray colData)
{
  herr_t err = 0;
  if (colData.size() == 0) { return err; }
  // Create the Group Folder
  hid_t disId = H5Utilities::createGroup(pid, hdf5GroupName);
  if (disId > 0)
  {
    err = writeVectorOfArrays(disId, colData);
  }
  // Close the HDF5 Group
  err |= H5Gclose(disId);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeDistributionData(hid_t pid,
                                               uint32_t disType,
                                               const std::string &hdf5GroupName,
                                               VectorOfFloatArray colData)
{
  herr_t err = 0;
  herr_t retErr = 0;

  std::string disTypeStr;
  switch(disType)
  {
    case DREAM3D::DistributionType::Beta:
      disTypeStr = DREAM3D::HDF5::BetaDistribution;
      break;
    case DREAM3D::DistributionType::LogNormal:
      disTypeStr = DREAM3D::HDF5::LogNormalDistribution;
      break;
    case DREAM3D::DistributionType::Power:
      disTypeStr = DREAM3D::HDF5::PowerLawDistribution;
      break;
    case DREAM3D::DistributionType::UnknownDistributionType:
      disTypeStr = DREAM3D::HDF5::UnknownDistribution;
      break;
    default:
      disTypeStr = DREAM3D::HDF5::UnknownDistribution;
  }

  // Create the Group Folder
  hid_t disId = H5Utilities::createGroup(pid, hdf5GroupName);
  if (disId > 0)
  {
    err = H5Lite::writeStringAttribute(pid, hdf5GroupName, DREAM3D::HDF5::DistributionType, disTypeStr);
    if(err >= 0)
    {
      err |= writeVectorOfArrays(disId, colData);
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
uint32_t H5PrecipitateStatsDataDelegate::readDistributionType(hid_t pid, const std::string &hdf5GroupName)
{
  int err = 0;
  uint32_t dType = DREAM3D::DistributionType::UnknownDistributionType;

  std::string disTypeStr;
  err = H5Lite::readStringAttribute(pid, hdf5GroupName, DREAM3D::HDF5::DistributionType, disTypeStr);

  if (disTypeStr.compare(DREAM3D::HDF5::BetaDistribution) == 0)
  {
    dType = DREAM3D::DistributionType::Beta;
  }
  else   if (disTypeStr.compare(DREAM3D::HDF5::LogNormalDistribution) == 0)
  {
    dType = DREAM3D::DistributionType::LogNormal;
  }
  else   if (disTypeStr.compare(DREAM3D::HDF5::PowerLawDistribution) == 0)
  {
    dType = DREAM3D::DistributionType::Power;
  }
  return dType;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readDistributionData(hid_t pid,
                                              const std::string &hdf5GroupName,
                                              VectorOfFloatArray colData)
{
  int err = 0;
  hid_t disId = H5Utilities::openHDF5Object(pid, hdf5GroupName);
  if(disId < 0)
  {
    return -1;
  }

  for (VectorOfFloatArray::iterator iter = colData.begin(); iter != colData.end(); ++iter)
  {
    FloatArrayType::Pointer d = *iter;
    err |= d->readH5Data(disId);
  }

  err |= H5Utilities::closeHDF5Object(disId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeBoundaryArea(PrecipitateStatsData* data, hid_t pid)
{
  float BoundaryArea = data->getBoundaryArea();
  return H5Lite::writeScalarDataset(pid, DREAM3D::HDF5::BoundaryArea, BoundaryArea);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readBoundaryArea(PrecipitateStatsData* data, hid_t pid)
{
  float BoundaryArea = 0.0f;
  int err = H5Lite::readScalarDataset(pid, DREAM3D::HDF5::BoundaryArea, BoundaryArea);
  data->setBoundaryArea(BoundaryArea);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writePhaseFraction(PrecipitateStatsData* data, hid_t pid)
{
  float phaseFraction = data->getPhaseFraction();
  return H5Lite::writeScalarDataset(pid, DREAM3D::HDF5::PhaseFraction, phaseFraction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readPhaseFraction(PrecipitateStatsData* data, hid_t pid)
{
  float phaseFraction = 0.0f;
  int err = H5Lite::readScalarDataset(pid, DREAM3D::HDF5::PhaseFraction, phaseFraction);
  data->setPhaseFraction(phaseFraction);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writePrecipBoundaryFraction(PrecipitateStatsData* data, hid_t pid)
{
  float var = data->getPrecipBoundaryFraction();
  return H5Lite::writeScalarDataset(pid, DREAM3D::HDF5::PrecipitateBoundaryFraction, var);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readPrecipBoundaryFraction(PrecipitateStatsData* data, hid_t pid)
{
  float precipBoundaryFraction = 0.0f;
  int err = H5Lite::readScalarDataset(pid, DREAM3D::HDF5::PrecipitateBoundaryFraction, precipBoundaryFraction);
  data->setPrecipBoundaryFraction(precipBoundaryFraction);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::writeGrainDiameterInfo(PrecipitateStatsData* data, hid_t pid)
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
int H5PrecipitateStatsDataDelegate::readGrainDiameterInfo(PrecipitateStatsData* data, hid_t groupId)
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
int H5PrecipitateStatsDataDelegate::writeBinNumbers(PrecipitateStatsData* data, hid_t groupId)
{
  // Ensure we have valid bin numbers
  if(NULL == data->getBinNumbers().get())
  {
    data->generateBinNumbers();
  }
  return data->getBinNumbers()->writeH5Data(groupId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PrecipitateStatsDataDelegate::readBinNumbers(PrecipitateStatsData* data, hid_t groupId)
{
  int err = 0;
  FloatArrayType::Pointer p = FloatArrayType::CreateArray(1, DREAM3D::HDF5::BinNumber);
  err = p->readH5Data(groupId);
  data->setBinNumbers(p);
  return err;
}




