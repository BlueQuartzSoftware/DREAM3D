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




#include "H5CtfImporter.h"

#include <sstream>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/Utilities/MXADir.h"
#include "EbsdLib/Utilities/StringUtils.h"
#include "EbsdLib/HKL/CtfReader.h"
#include "EbsdLib/HKL/CtfPhase.h"


#define AIM_STRING std::string

#define CHECK_FOR_CANCELED(AClass)\
    if (m_Cancel == true){\
      break; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfImporter::H5CtfImporter():
EbsdImporter(),
xDim(0),
yDim(0),
xRes(0),
yRes(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfImporter::~H5CtfImporter()
{
}

#define WRITE_EBSD_HEADER_DATA(reader, type, prpty, key)\
{\
  type t = reader.get##prpty();\
  err = H5Lite::writeScalarDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5CtfImporter Error: Could not write Ctf Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(gid); err = H5Gclose(ctfGroup);\
    return -1; }\
}

#define WRITE_EBSD_HEADER_STRING_DATA(reader, type, prpty, key)\
{\
  type t = reader.get##prpty();\
  err = H5Lite::writeStringDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5CtfImporter Error: Could not write Ctf Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(gid); err = H5Gclose(ctfGroup);\
    return -1; }\
}

#define WRITE_EBSD_DATA_ARRAY(reader, type, gid, prpty, key)\
{\
  type* dataPtr = reader.get##prpty##Pointer();\
  if (NULL != dataPtr) {\
    err = H5Lite::writePointerDataset(gid, key, rank, dims, dataPtr);\
    if (err < 0) {\
      std::ostringstream ss;\
      ss << "H5CtfImporter Error: Could not write Ctf Data array for '" << key\
      <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
      progressMessage(ss.str(), 100);\
      err = H5Gclose(gid); err = H5Gclose(ctfGroup);\
      return -1; }\
}\
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfImporter::getDims(int &x, int &y)
{
  x = xDim;
  y = yDim;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfImporter::getResolution(float &x, float &y)
{
  x = xRes;
  y = yRes;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfImporter::importFile(hid_t fileId, int z, const std::string &ctfFile)
{
  herr_t err = -1;
  setCancel(false);
  setErrorCondition(false);
  setErrorMessage("");

//  std::cout << "H5CtfImporter: Importing " << ctfFile << std::endl;
  CtfReader reader;
  reader.setFileName(ctfFile);
  // This is set to NoOrientation because we want to leave the data intact as it
  // was received from the instrument. The user can choose to rotate the data as
  // it is read from the resulting HDF5 data file.
  reader.setUserOrigin(Ebsd::NoOrientation);

  // Now actually read the file
  err = reader.readFile();

  // Check for errors
  if (err < 0)
  {
    std::ostringstream ss;
    if (err == -200)
    {
      ss << "H5CtfImporter Error: There was no data in the file.";
    }
    else if (err == -100)
    {
      ss << "H5CtfImporter Error: The Ctf file could not be opened.";
    }
    else
    {
      ss << "H5CtfImporter Error: Unknown error.";
    }
    setErrorMessage(ss.str());
    setErrorCondition(err);
    progressMessage(ss.str(), 100);
    return -1;
  }

  // Start creating the HDF5 group structures for this file
  hid_t ctfGroup = H5Utilities::createGroup(fileId, StringUtils::numToString(z));
  if (ctfGroup < 0)
  {
    std::ostringstream ss;
    ss << "H5CtfImporter Error: A Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons.";
    setErrorMessage(ss.str());
    setErrorCondition(-500);
    return -1;
  }

  hid_t gid = H5Utilities::createGroup(ctfGroup, Ebsd::H5::Header);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5CtfImporter Error: The 'Header' Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons.";
    progressMessage(ss.str(), 100);
    err = H5Gclose(ctfGroup);
    setErrorMessage(ss.str());
    setErrorCondition(-600);
    return -1;
  }
  WRITE_EBSD_HEADER_STRING_DATA(reader, std::string, Prj, Ebsd::Ctf::Prj)
  WRITE_EBSD_HEADER_STRING_DATA(reader, std::string, Author, Ebsd::Ctf::Author)
  WRITE_EBSD_HEADER_STRING_DATA(reader, std::string, JobMode, Ebsd::Ctf::JobMode)
  WRITE_EBSD_HEADER_DATA(reader, int, XCells, Ebsd::Ctf::XCells)
  xDim = reader.getXCells();
  WRITE_EBSD_HEADER_DATA(reader, int, YCells, Ebsd::Ctf::YCells)
  yDim = reader.getYCells();
  WRITE_EBSD_HEADER_DATA(reader, float, XStep, Ebsd::Ctf::XStep)
  xRes = reader.getXStep();
  WRITE_EBSD_HEADER_DATA(reader, float, YStep, Ebsd::Ctf::YStep)
  yRes = reader.getYStep();

  WRITE_EBSD_HEADER_DATA(reader, float, AcqE1, Ebsd::Ctf::AcqE1)
  WRITE_EBSD_HEADER_DATA(reader, float, AcqE2, Ebsd::Ctf::AcqE2)
  WRITE_EBSD_HEADER_DATA(reader, float, AcqE3, Ebsd::Ctf::AcqE3)
  WRITE_EBSD_HEADER_STRING_DATA(reader, std::string, Euler, Ebsd::Ctf::Euler)
  WRITE_EBSD_HEADER_DATA(reader, int, Mag, Ebsd::Ctf::Mag)
  WRITE_EBSD_HEADER_DATA(reader, int, Coverage, Ebsd::Ctf::Coverage)
  WRITE_EBSD_HEADER_DATA(reader, int, Device, Ebsd::Ctf::Device)
  WRITE_EBSD_HEADER_DATA(reader, int, KV, Ebsd::Ctf::KV)
  WRITE_EBSD_HEADER_DATA(reader, float, TiltAngle, Ebsd::Ctf::TiltAngle)
  WRITE_EBSD_HEADER_DATA(reader, float, TiltAxis, Ebsd::Ctf::TiltAxis)

  hid_t phasesGid = H5Utilities::createGroup(gid, Ebsd::H5::Phases);
  if (phasesGid < 0) {
    std::ostringstream ss;
    ss << "H5CtfImporter Error: The 'Header' Group for the Phases could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons.";
    progressMessage(ss.str(), 100);
    err = H5Gclose(gid);
    err = H5Gclose(ctfGroup);
    setErrorMessage(ss.str());
    setErrorCondition(-600);
    return -1;
  }
  err = writePhaseData(reader, phasesGid);
  // Close this group
  err = H5Gclose(phasesGid);


  std::string ctfCompleteHeader = reader.getOriginalHeader();
  err = H5Lite::writeStringDataset(gid, Ebsd::H5::OriginalHeader, ctfCompleteHeader);
  err = H5Lite::writeStringDataset(gid, Ebsd::H5::OriginalFile, ctfFile);

  // Close the "Header" group
  err = H5Gclose(gid);

  // Create the "Data" group
  gid = H5Utilities::createGroup(ctfGroup, Ebsd::H5::Data);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5CtfImporter Error: The 'Data' Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons."<< std::endl;
    progressMessage(ss.str(), 100);
    err = H5Gclose(ctfGroup);
    setErrorMessage(ss.str());
    setErrorCondition(-700);
    return -1;
  }

  int32_t rank = 1;
  hsize_t dims[1] = { reader.getXCells() * reader.getYCells() };

  WRITE_EBSD_DATA_ARRAY(reader, int, gid, Phase, Ebsd::Ctf::Phase);
  WRITE_EBSD_DATA_ARRAY(reader, float, gid, X, Ebsd::Ctf::X);
  WRITE_EBSD_DATA_ARRAY(reader, float, gid, Y, Ebsd::Ctf::Y);
  WRITE_EBSD_DATA_ARRAY(reader, int, gid, BandCount, Ebsd::Ctf::BandCount);
  WRITE_EBSD_DATA_ARRAY(reader, int, gid, Error, Ebsd::Ctf::Error);
  WRITE_EBSD_DATA_ARRAY(reader, float, gid, Euler1, Ebsd::Ctf::Euler1);
  WRITE_EBSD_DATA_ARRAY(reader, float, gid, Euler2, Ebsd::Ctf::Euler2);
  WRITE_EBSD_DATA_ARRAY(reader, float, gid, Euler3, Ebsd::Ctf::Euler3);
  WRITE_EBSD_DATA_ARRAY(reader, float, gid, MeanAngularDeviation, Ebsd::Ctf::MeanAngularDeviation);
  WRITE_EBSD_DATA_ARRAY(reader, int, gid, BandContrast, Ebsd::Ctf::BandContrast);
  WRITE_EBSD_DATA_ARRAY(reader, int, gid, BandSlope, Ebsd::Ctf::BandSlope);

  // Close the "Data" group
  err = H5Gclose(gid);

  // Close the group for this file
  err = H5Gclose(ctfGroup);

  return err;
}



#define WRITE_PHASE_HEADER_DATA(reader, type, prpty, key)\
{\
  type t = reader->get##prpty();\
  err = H5Lite::writeScalarDataset(pid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5CtfImporter Error: Could not write Ctf Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(pid);\
    return -1; }\
}

#define WRITE_PHASE_HEADER_STRING_DATA(reader, type, prpty, key)\
{\
  type t = reader->get##prpty();\
  err = H5Lite::writeStringDataset(pid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5CtfImporter Error: Could not write Ctf Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(pid);\
    return -1; }\
}

#define WRITE_PHASE_DATA_ARRAY(reader, type, gid, prpty, key)\
{\
  std::vector<type> tempVar = reader->get##prpty();\
  dims[0] = tempVar.size();\
  type* dataPtr = &(tempVar.front());\
  if (NULL != dataPtr) {\
    err = H5Lite::writePointerDataset(pid, key, rank, dims, dataPtr);\
    if (err < 0) {\
      std::ostringstream ss;\
      ss << "H5CtfImporter Error: Could not write Ctf Data array for '" << key\
      <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
      progressMessage(ss.str(), 100);\
      err = H5Gclose(pid); \
      return -1; }\
}\
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfImporter::writePhaseData(CtfReader &reader, hid_t phasesGid)
{
  int err = 0;
 // int retErr = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 0 };
  std::vector<CtfPhase::Pointer> phases = reader.getPhaseVector();
  for (std::vector<CtfPhase::Pointer>::iterator phase = phases.begin(); phase != phases.end(); ++phase )
  {
    CtfPhase* p = (*phase).get();
    hid_t pid = H5Utilities::createGroup(phasesGid, StringUtils::numToString(p->getPhaseIndex()));

    WRITE_PHASE_DATA_ARRAY( (*phase), float, pid, LatticeDimensions, Ebsd::Ctf::LatticeDimensions);
    WRITE_PHASE_DATA_ARRAY( (*phase), float, pid, LatticeAngles, Ebsd::Ctf::LatticeAngles);
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, PhaseName, Ebsd::Ctf::PhaseName)
    WRITE_PHASE_HEADER_DATA((*phase), int, Symmetry, Ebsd::Ctf::LaueGroup)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, Section4, Ebsd::Ctf::Section4)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, Section5, Ebsd::Ctf::Section5)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, Section6, Ebsd::Ctf::Section6)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, Comment, Ebsd::Ctf::Comment)
    err = H5Gclose(pid);
  }
  return err;
}

