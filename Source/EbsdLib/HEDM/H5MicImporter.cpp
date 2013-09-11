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




#include "H5MicImporter.h"

#include <sstream>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/Utilities/MXADir.h"
#include "EbsdLib/Utilities/StringUtils.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

#define AIM_STRING std::string

#define CHECK_FOR_CANCELED(AClass)\
  if (m_Cancel == true){\
  break; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5MicImporter::H5MicImporter() :
  EbsdImporter(),
  xDim(0),
  yDim(0),
  xRes(0),
  yRes(0),
m_FileVersion(Ebsd::H5::FileVersion)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5MicImporter::~H5MicImporter()
{
}


#define WRITE_Mic_HEADER_DATA(reader, m_msgType, prpty, key)\
{\
  m_msgType t = reader.get##prpty();\
  err = H5Lite::writeScalarDataset(gid, key, t);\
  if (err < 0) {\
  std::ostringstream ss;\
  ss << "H5MicImporter Error: Could not write Mic Header value '" << t\
  <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
  progressMessage(ss.str(), 100);\
  err = H5Gclose(gid); err = H5Gclose(MicGroup);\
  return -1; }\
  }

#define WRITE_Mic_HEADER_STRING_DATA(reader, m_msgType, prpty, key)\
{\
  m_msgType t = reader.get##prpty();\
  err = H5Lite::writeStringDataset(gid, key, t);\
  if (err < 0) {\
  std::ostringstream ss;\
  ss << "H5MicImporter Error: Could not write Mic Header value '" << t\
  <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
  progressMessage(ss.str(), 100);\
  err = H5Gclose(gid); err = H5Gclose(MicGroup);\
  return -1; }\
  }

#define WRITE_Mic_DATA_ARRAY(reader, m_msgType, gid, prpty, key)\
{\
  m_msgType* dataPtr = reader.get##prpty##Pointer();\
  if (NULL != dataPtr) {\
  err = H5Lite::writePointerDataset(gid, key, rank, dims, dataPtr);\
  if (err < 0) {\
  std::ostringstream ss;\
  ss << "H5MicImporter Error: Could not write Mic Data array for '" << key\
  <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
  progressMessage(ss.str(), 100);\
  err = H5Gclose(gid); err = H5Gclose(MicGroup);\
  return -1; }\
  }\
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5MicImporter::getDims(int64_t &x, int64_t &y)
{
  x = xDim;
  y = yDim;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5MicImporter::getResolution(float &x, float &y)
{
  x = xRes;
  y = yRes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicImporter::numberOfSlicesImported()
{
  return 1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicImporter::importFile(hid_t fileId, int64_t z, const QString &MicFile)
{
  herr_t err = -1;
  setCancel(false);
  setErrorCondition(false);
  setPipelineMessage("");

  //  std::cout << "H5MicImporter: Importing " << MicFile << std::endl;
  MicReader reader;
  reader.setFileName(MicFile);

  // Now actually read the file
  err = reader.readFile();

  // Check for errors
  if (err < 0)
  {
    setPipelineMessage(reader.getErrorMessage());
    setErrorCondition(reader.getErrorCode());
    progressMessage(reader.getErrorMessage(), reader.getErrorCode());
    return -1;
  }

  // Write the fileversion attribute if it does not exist
  {
    QVector<hsize_t> dims;
    H5T_class_t type_class;
    size_t type_size = 0;
    hid_t attr_type = -1;
    err = H5Lite::getAttributeInfo(fileId, "/", Ebsd::H5::FileVersionStr, dims, type_class, type_size, attr_type);
    if (attr_type < 0) // The attr_type variable was never set which means the attribute was NOT there
    {
      // The file version does not exist so write it to the file
      err = H5Lite::writeScalarAttribute(fileId, "/", Ebsd::H5::FileVersionStr, m_FileVersion);
    }
    else
    {
      H5Aclose(attr_type);
    }
  }

  // Start creating the HDF5 group structures for this file
  hid_t MicGroup = H5Utilities::createGroup(fileId, StringUtils::numToString(z));
  if (MicGroup < 0)
  {
    std::ostringstream ss;
    ss << "H5MicImporter Error: A Group for Z index " << z << " could not be created."
       << " Please check other error messages from the HDF5 library for possible reasons.";
    setPipelineMessage(ss.str());
    setErrorCondition(-500);
    return -1;
  }

  hid_t gid = H5Utilities::createGroup(MicGroup, Ebsd::H5::Header);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5MicImporter Error: The 'Header' Group for Z index " << z << " could not be created."
       << " Please check other error messages from the HDF5 library for possible reasons.";
    progressMessage(ss.str(), 100);
    err = H5Gclose(MicGroup);
    setPipelineMessage(ss.str());
    setErrorCondition(-600);
    return -1;
  }

  hid_t phasesGid = H5Utilities::createGroup(gid, Ebsd::H5::Phases);
  err = writePhaseData(reader, phasesGid);
  // Close this group
  err = H5Gclose(phasesGid);

  xDim = reader.getXDimension();
  yDim = reader.getYDimension();
  xRes = reader.getXStep();
  yRes = reader.getYStep();

  WRITE_Mic_HEADER_STRING_DATA(reader, std::string, InfileBasename, Ebsd::Mic::InfileBasename)
      WRITE_Mic_HEADER_DATA(reader, int, InfileSerialLength, Ebsd::Mic::InfileSerialLength)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, OutfileBasename, Ebsd::Mic::OutfileBasename)
      WRITE_Mic_HEADER_DATA(reader, int, OutfileSerialLength, Ebsd::Mic::OutfileSerialLength)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, OutStructureBasename, Ebsd::Mic::OutStructureBasename)
      WRITE_Mic_HEADER_DATA(reader, int, BCPeakDetectorOffset, Ebsd::Mic::BCPeakDetectorOffset)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, InFileType, Ebsd::Mic::InFileType)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, OutfileExtension, Ebsd::Mic::OutfileExtension)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, InfileExtesnion, Ebsd::Mic::InfileExtesnion)
      WRITE_Mic_HEADER_DATA(reader, float, BeamEnergyWidth, Ebsd::Mic::BeamEnergyWidth)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, BeamDirection, Ebsd::Mic::BeamDirection)
      WRITE_Mic_HEADER_DATA(reader, int, BeamDeflectionChiLaue, Ebsd::Mic::BeamDeflectionChiLaue)
      WRITE_Mic_HEADER_DATA(reader, float, BeamHeight, Ebsd::Mic::BeamHeight)
      WRITE_Mic_HEADER_DATA(reader, float, BeamEnergy, Ebsd::Mic::BeamEnergy)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, DetectorFilename, Ebsd::Mic::DetectorFilename)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, OptimizationConstrainFilename, Ebsd::Mic::OptimizationConstrainFilename)
      WRITE_Mic_HEADER_DATA(reader, int, EtaLimit, Ebsd::Mic::EtaLimit)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, SampleFilename, Ebsd::Mic::SampleFilename)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, StructureFilename, Ebsd::Mic::StructureFilename)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, RotationRangeFilename, Ebsd::Mic::RotationRangeFilename)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, FundamentalZoneFilename, Ebsd::Mic::FundamentalZoneFilename)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, SampleSymmetry, Ebsd::Mic::SampleSymmetry)
      WRITE_Mic_HEADER_DATA(reader, float, MinAmplitudeFraction, Ebsd::Mic::MinAmplitudeFraction)
      WRITE_Mic_HEADER_DATA(reader, int, MaxQ, Ebsd::Mic::MaxQ)
      WRITE_Mic_HEADER_DATA(reader, float, MaxInitSideLength, Ebsd::Mic::MaxInitSideLength)
      WRITE_Mic_HEADER_DATA(reader, float, MinSideLength, Ebsd::Mic::MinSideLength)
      WRITE_Mic_HEADER_DATA(reader, float, LocalOrientationGridRadius, Ebsd::Mic::LocalOrientationGridRadius)
      WRITE_Mic_HEADER_DATA(reader, float, MinLocalResolution, Ebsd::Mic::MinLocalResolution)
      WRITE_Mic_HEADER_DATA(reader, float, MaxLocalResolution, Ebsd::Mic::MaxLocalResolution)
      WRITE_Mic_HEADER_DATA(reader, float, MaxAcceptedCost, Ebsd::Mic::MaxAcceptedCost)
      WRITE_Mic_HEADER_DATA(reader, float, MaxConvergenceCost, Ebsd::Mic::MaxConvergenceCost)
      WRITE_Mic_HEADER_DATA(reader, int, MaxMCSteps, Ebsd::Mic::MaxMCSteps)
      WRITE_Mic_HEADER_DATA(reader, float, MCRadiusScaleFactor, Ebsd::Mic::MCRadiusScaleFactor)
      WRITE_Mic_HEADER_DATA(reader, int, SuccessiveRestarts, Ebsd::Mic::SuccessiveRestarts)
      WRITE_Mic_HEADER_DATA(reader, int, SecondsBetweenSave, Ebsd::Mic::SecondsBetweenSave)
      WRITE_Mic_HEADER_DATA(reader, int, NumParameterOptimizationSteps, Ebsd::Mic::NumParameterOptimizationSteps)
      WRITE_Mic_HEADER_DATA(reader, int, NumElementToOptimizePerPE, Ebsd::Mic::NumElementToOptimizePerPE)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, OptimizationFilename, Ebsd::Mic::OptimizationFilename)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, DetectionLimitFilename, Ebsd::Mic::DetectionLimitFilename)
      WRITE_Mic_HEADER_DATA(reader, float, ParameterMCInitTemperature, Ebsd::Mic::ParameterMCInitTemperature)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, OrientationSearchMethod, Ebsd::Mic::OrientationSearchMethod)
      WRITE_Mic_HEADER_DATA(reader, float, CoolingFraction, Ebsd::Mic::CoolingFraction)
      WRITE_Mic_HEADER_DATA(reader, float, ThermalizeFraction, Ebsd::Mic::ThermalizeFraction)
      WRITE_Mic_HEADER_DATA(reader, int, ParameterRefinements, Ebsd::Mic::ParameterRefinements)
      WRITE_Mic_HEADER_DATA(reader, int, NumDetectors, Ebsd::Mic::NumDetectors)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, DetectorSpacing, Ebsd::Mic::DetectorSpacing)
      WRITE_Mic_HEADER_DATA(reader, float, DetectorSpacingDeviation, Ebsd::Mic::DetectorSpacingDeviation)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, DetectorOrientationDeviationInEuler, Ebsd::Mic::DetectorOrientationDeviationInEuler)
      WRITE_Mic_HEADER_DATA(reader, float, DetectorOrientationDeviationInSO3, Ebsd::Mic::DetectorOrientationDeviationInSO3)
      WRITE_Mic_HEADER_DATA(reader, int, ParamMCMaxLocalRestarts, Ebsd::Mic::ParamMCMaxLocalRestarts)
      WRITE_Mic_HEADER_DATA(reader, int, ParamMCMaxGlobalRestarts, Ebsd::Mic::ParamMCMaxGlobalRestarts)
      WRITE_Mic_HEADER_DATA(reader, int, ParamMCNumGlobalSearchElements, Ebsd::Mic::ParamMCNumGlobalSearchElements)
      WRITE_Mic_HEADER_DATA(reader, int, ConstrainedOptimization, Ebsd::Mic::ConstrainedOptimization)
      WRITE_Mic_HEADER_DATA(reader, int, SearchVolumeReductionFactor, Ebsd::Mic::SearchVolumeReductionFactor)
      WRITE_Mic_HEADER_DATA(reader, int, FileNumStart, Ebsd::Mic::FileNumStart)
      WRITE_Mic_HEADER_DATA(reader, int, FileNumEnd, Ebsd::Mic::FileNumEnd)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, SampleLocation, Ebsd::Mic::SampleLocation)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, SampleOrientation, Ebsd::Mic::SampleOrientation)
      WRITE_Mic_HEADER_DATA(reader, int, EnableStrain, Ebsd::Mic::EnableStrain)
      WRITE_Mic_HEADER_STRING_DATA(reader, std::string, SampleCenter, Ebsd::Mic::SampleCenter)
      WRITE_Mic_HEADER_DATA(reader, float, SampleRadius, Ebsd::Mic::SampleRadius)
      WRITE_Mic_HEADER_DATA(reader, float, MaxDeepeningHitRatio, Ebsd::Mic::MaxDeepeningHitRatio)
      WRITE_Mic_HEADER_DATA(reader, float, ConsistencyError, Ebsd::Mic::ConsistencyError)
      WRITE_Mic_HEADER_DATA(reader, float, BraggFilterTolerance, Ebsd::Mic::BraggFilterTolerance)
      WRITE_Mic_HEADER_DATA(reader, float, MinAccelerationThreshold, Ebsd::Mic::MinAccelerationThreshold)
      WRITE_Mic_HEADER_DATA(reader, int, MaxDiscreteCandidates, Ebsd::Mic::MaxDiscreteCandidates)
      WRITE_Mic_HEADER_DATA(reader, int, XDim, Ebsd::Mic::XDim)
      WRITE_Mic_HEADER_DATA(reader, int, YDim, Ebsd::Mic::YDim)
      WRITE_Mic_HEADER_DATA(reader, float, XRes, Ebsd::Mic::XRes)
      WRITE_Mic_HEADER_DATA(reader, float, YRes, Ebsd::Mic::YRes)

      QString micCompleteHeader = reader.getOriginalHeader();
  err = H5Lite::writeStringDataset(gid, Ebsd::H5::OriginalHeader, micCompleteHeader);

  // Close the "Header" group
  err = H5Gclose(gid);

  // Create the "Data" group
  gid = H5Utilities::createGroup(MicGroup, Ebsd::H5::Data);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5MicImporter Error: The 'Data' Group for Z index " << z << " could not be created."
       << " Please check other error messages from the HDF5 library for possible reasons."<< std::endl;
    progressMessage(ss.str(), 100);
    err = H5Gclose(MicGroup);
    setPipelineMessage(ss.str());
    setErrorCondition(-700);
    return -1;
  }

  int32_t rank = 1;
  hsize_t dims[1] = { reader.getXDimension() * reader.getYDimension() };

  WRITE_Mic_DATA_ARRAY(reader, float, gid, Euler1, Ebsd::Mic::Euler1);
  WRITE_Mic_DATA_ARRAY(reader, float, gid, Euler2, Ebsd::Mic::Euler2);
  WRITE_Mic_DATA_ARRAY(reader, float, gid, Euler3, Ebsd::Mic::Euler3);
  WRITE_Mic_DATA_ARRAY(reader, float, gid, X, Ebsd::Mic::X);
  WRITE_Mic_DATA_ARRAY(reader, float, gid, Y, Ebsd::Mic::Y);
  WRITE_Mic_DATA_ARRAY(reader, float, gid, Confidence, Ebsd::Mic::Confidence);
  WRITE_Mic_DATA_ARRAY(reader, int, gid, Phase, Ebsd::Mic::Phase);
  // Close the "Data" group
  err = H5Gclose(gid);

  // Close the group for this file
  err = H5Gclose(MicGroup);

  return err;
}


#define WRITE_PHASE_HEADER_DATA(reader, m_msgType, prpty, key)\
{\
  m_msgType t = reader->get##prpty();\
  err = H5Lite::writeScalarDataset(pid, key, t);\
  if (err < 0) {\
  std::ostringstream ss;\
  ss << "H5MicImporter Error: Could not write Mic Header value '" << t\
  <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
  progressMessage(ss.str(), 100);\
  err = H5Gclose(pid);\
  return -1; }\
  }

#define WRITE_PHASE_HEADER_STRING_DATA(reader, m_msgType, prpty, key)\
{\
  m_msgType t = reader->get##prpty();\
  err = H5Lite::writeStringDataset(pid, key, t);\
  if (err < 0) {\
  std::ostringstream ss;\
  ss << "H5MicImporter Error: Could not write Mic Header value '" << t\
  <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
  progressMessage(ss.str(), 100);\
  err = H5Gclose(pid);\
  return -1; }\
  }

#define WRITE_PHASE_DATA_ARRAY(reader, m_msgType, gid, prpty, key)\
{\
  QVector<m_msgType> tempVar = reader->get##prpty();\
  dims[0] = tempVar.size();\
  m_msgType* dataPtr = &(tempVar.front());\
  if (NULL != dataPtr) {\
  err = H5Lite::writePointerDataset(pid, key, rank, dims, dataPtr);\
  if (err < 0) {\
  std::ostringstream ss;\
  ss << "H5MicImporter Error: Could not write Mic Data array for '" << key\
  <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
  progressMessage(ss.str(), 100);\
  err = H5Gclose(pid); \
  return -1; }\
  }\
  }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicImporter::writePhaseData(MicReader &reader, hid_t phasesGid)
{
  int err = 0;
  // int retErr = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 0 };
  QVector<MicPhase::Pointer> phases = reader.getPhaseVector();
  for (QVector<MicPhase::Pointer>::iterator phase = phases.begin(); phase != phases.end(); ++phase )
  {
    MicPhase* p = (*phase).get();
    hid_t pid = H5Utilities::createGroup(phasesGid, StringUtils::numToString(p->getPhaseIndex()));
    WRITE_PHASE_HEADER_DATA((*phase), int, PhaseIndex, Ebsd::Mic::Phase)
    WRITE_PHASE_DATA_ARRAY((*phase), float, pid, LatticeConstants, Ebsd::Mic::LatticeConstants)
  //  WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, LatticeAngles, Ebsd::Mic::LatticeAngles)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, BasisAtoms, Ebsd::Mic::BasisAtoms)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, Symmetry, Ebsd::Mic::Symmetry)

    if (p->getZandCoordinates().size() > 0) {
      hid_t ZandCGid = H5Utilities::createGroup(pid, Ebsd::Mic::ZandCoordinates);
      err = writeZandCoordinates(p, ZandCGid);
      if (err < 0) {
        std::ostringstream ss;
        ss << "H5MicImporter Error: Could not write Mic Z and Coordinates to the HDF5 file with data set name '"
           << Ebsd::Mic::ZandCoordinates << "'" << std::endl;
        progressMessage(ss.str(), 100);
        err = H5Gclose(ZandCGid);
        return -1;
      }
      err = H5Gclose(ZandCGid);
    }
    err = H5Gclose(pid);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicImporter::writeZandCoordinates(MicPhase* p, hid_t ZandCGid)
{
  int err = 0;
  QVector<std::string> ZandCs = p->getZandCoordinates();
  QString ZandC;
  int count = 0;
  for (QVector<std::string>::iterator f = ZandCs.begin(); f != ZandCs.end(); ++f )
  {
    err = H5Lite::writeStringDataset(ZandCGid, StringUtils::numToString(count), *f);
    count++;
  }
  err = H5Gclose(ZandCGid);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5MicImporter::setFileVersion(uint32_t version)
{
  m_FileVersion = version;
}
