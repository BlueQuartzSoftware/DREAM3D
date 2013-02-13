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

#include "H5MicReader.h"

#include "MicConstants.h"
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/Utilities/StringUtils.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5MicReader::H5MicReader() :
MicReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5MicReader::~H5MicReader()
{
 deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicReader::readFile()
{
  int err = -1;
  if (m_HDF5Path.empty() == true)
  {
    std::cout << "H5MicReader Error: HDF5 Path is empty." << std::endl;
    return -1;
  }

  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    std::cout << "H5MicReader Error: Could not open HDF5 file '" << getFileName() << "'" << std::endl;
    return -1;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5MicReader Error: Could not open path '" << m_HDF5Path << "'" << std::endl;
    err = H5Utilities::closeFile(fileId);
    return -1;
  }

  // Read all the header information
 // std::cout << "H5MicReader:: reading Header .. " << std::endl;
  err = readHeader(gid);

  // Read and transform data
 // std::cout << "H5MicReader:: Reading Data .. " << std::endl;
  err = readData(gid);

  err = H5Gclose(gid);
  err = H5Utilities::closeFile(fileId);

  if(getRotateSlice() == true || getReorderArray() == true || getAlignEulers() == true)
  {
	  transformData();
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicReader::readHeaderOnly()
{
  int err = -1;
  if (m_HDF5Path.empty() == true)
  {
    std::cout << "H5MicReader Error: HDF5 Path is empty." << std::endl;
    return -1;
  }

  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    std::cout << "H5MicReader Error: Could not open HDF5 file '" << getFileName() << "'" << std::endl;
    return -1;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5MicReader Error: Could not open path '" << m_HDF5Path << "'" << std::endl;
    err = H5Utilities::closeFile(fileId);
    return -1;
  }

  // Read all the header information
 // std::cout << "H5MicReader:: reading Header .. " << std::endl;
  err = readHeader(gid);
  err = H5Utilities::closeFile(fileId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicReader::readHeader(hid_t parId)
{
  int err = -1;
  hid_t gid = H5Gopen(parId, Ebsd::H5::Header.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5MicReader Error: Could not open 'Header' Group" << std::endl;
    return -1;
  }



  READ_EBSD_HEADER_DATA("H5MicReader", MicHeaderEntry<float>, float, XRes, Ebsd::Mic::XRes)
  READ_EBSD_HEADER_DATA("H5MicReader", MicHeaderEntry<float>, float, YRes, Ebsd::Mic::YRes)
  READ_EBSD_HEADER_DATA("H5MicReader", MicHeaderEntry<int>, int, XDim, Ebsd::Mic::XDim)
  READ_EBSD_HEADER_DATA("H5MicReader", MicHeaderEntry<int>, int, YDim, Ebsd::Mic::YDim)


  hid_t phasesGid = H5Gopen(gid, Ebsd::H5::Phases.c_str(), H5P_DEFAULT);
  if (phasesGid < 0)
  {
    std::cout << "H5MicReader Error: Could not open Header/Phases HDF Group. Is this an older file?" << std::endl;
    H5Gclose(gid);
    return -1;
  }

  std::list<std::string> names;
  err = H5Utilities::getGroupObjects(phasesGid, H5Utilities::H5Support_GROUP, names);
  if (err < 0 || names.size() == 0)
  {
    std::cout << "H5MicReader Error: There were no Phase groups present in the HDF5 file" << std::endl;
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return -1;
  }
  m_Phases.clear();
  for (std::list<std::string>::iterator phaseGroupName = names.begin(); phaseGroupName != names.end(); ++phaseGroupName )
  {
    hid_t pid = H5Gopen(phasesGid, (*phaseGroupName).c_str(), H5P_DEFAULT);
    MicPhase::Pointer m_CurrentPhase = MicPhase::New();

    READ_PHASE_HEADER_DATA("H5MicReader", pid, int, Ebsd::Mic::Phase, PhaseIndex, m_CurrentPhase)
	READ_PHASE_STRING_DATA("H5MicReader", pid, Ebsd::Mic::LatticeConstants, LatticeConstants, m_CurrentPhase)
	READ_PHASE_STRING_DATA("H5MicReader", pid, Ebsd::Mic::LatticeAngles, LatticeAngles, m_CurrentPhase)
	READ_PHASE_STRING_DATA("H5MicReader", pid, Ebsd::Mic::BasisAtoms, BasisAtoms, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5MicReader", pid, Ebsd::Mic::Symmetry, Symmetry, m_CurrentPhase)

    m_Phases.push_back(m_CurrentPhase);
    err = H5Gclose(pid);
  }

  std::string completeHeader;
  err = H5Lite::readStringDataset(gid, Ebsd::H5::OriginalHeader, completeHeader);
  setOriginalHeader(completeHeader);
  err = H5Gclose(phasesGid);
  err = H5Gclose(gid);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicReader::readData(hid_t parId)
{
  int err = -1;

  // Delete any currently existing pointers
  deletePointers();
  // Initialize new pointers
  size_t totalDataRows = 0;

  size_t xDim = getXDimension();
  size_t yDim = getYDimension();

  if (yDim < 1)
  {
    return -200;
  }

  totalDataRows = xDim*yDim;

  initPointers(totalDataRows);
  if (NULL == getEuler1Pointer() || NULL == getEuler2Pointer() || NULL == getEuler3Pointer()
      || NULL == getConfidencePointer() || NULL == getPhasePointer() || getXPosPointer() == NULL || getYPosPointer() == NULL)
  {
    return -1;
  }


  hid_t gid = H5Gopen(parId, Ebsd::H5::Data.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5MicReader Error: Could not open 'Data' Group" << std::endl;
    return -1;
  }

  err = H5Lite::readPointerDataset(gid, Ebsd::Mic::Euler1, getEuler1Pointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Mic::Euler2, getEuler2Pointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Mic::Euler3, getEuler3Pointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Mic::Confidence, getConfidencePointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Mic::Phase, getPhasePointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Mic::X, getXPosPointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Mic::Y, getYPosPointer());

  err = H5Gclose(gid);

  return err;
}

