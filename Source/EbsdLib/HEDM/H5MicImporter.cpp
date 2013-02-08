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
yRes(0)
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
int H5MicImporter::importFile(hid_t fileId, int64_t z, const std::string &MicFile)
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
    std::ostringstream ss;
    if (err == -400) {
      ss << "H5MicImporter Error: HexGrid Files are not currently supported.";
    }
    else if (err == -300)
    {
      ss << "H5MicImporter Error: Grid was NOT set in the header.";
    }
    else if (err == -200)
    {
      ss << "H5MicImporter Error: There was no data in the file.";
    }
    else if (err == -100)
    {
      ss << "H5MicImporter Error: The Mic file could not be opened.";
    }
    else
    {
      ss << "H5MicImporter Error: Unknown error.";
    }
    setPipelineMessage(ss.str());

    setErrorCondition(err);
    progressMessage(ss.str(), 100);
    return -1;
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
  WRITE_Mic_DATA_ARRAY(reader, float, gid, XPos, Ebsd::Mic::X);
  WRITE_Mic_DATA_ARRAY(reader, float, gid, YPos, Ebsd::Mic::Y);
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
  std::vector<m_msgType> tempVar = reader->get##prpty();\
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
//  int32_t rank = 1;
//  hsize_t dims[1] = { 0 };
  std::vector<MicPhase::Pointer> phases = reader.getPhaseVector();
  for (std::vector<MicPhase::Pointer>::iterator phase = phases.begin(); phase != phases.end(); ++phase )
  {
    MicPhase* p = (*phase).get();
    hid_t pid = H5Utilities::createGroup(phasesGid, StringUtils::numToString(p->getPhaseIndex()));
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, PhaseName, Ebsd::Mic::PhaseName)

    err = H5Gclose(pid);
  }
  return err;
}
