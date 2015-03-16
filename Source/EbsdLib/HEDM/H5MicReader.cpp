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



#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdMacros.h"
#include "MicConstants.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5MicReader::H5MicReader() :
  MicReader(),
  m_ReadAllArrays(true)
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
  if (m_HDF5Path.isEmpty() == true)
  {
    std::cout << "H5MicReader Error: HDF5 Path is empty." << std::endl;
    return -1;
  }

  hid_t fileId = QH5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    QString ss = QObject::tr("H5MicReader Error: Could not open HDF5 file '%1'").arg(getFileName());
    setErrorMessage(ss);
    setErrorCode(-100);
    return -100;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    err = QH5Utilities::closeFile(fileId);
    QString ss = QObject::tr("H5MicReader Error: Could not open path '%1'").arg(m_HDF5Path);
    setErrorMessage(ss);
    setErrorCode(-101);
    return -101;
  }

  // Read all the header information
  // std::cout << "H5MicReader:: reading Header .. " << std::endl;
  err = readHeader(gid);

  // Read and transform data
  // std::cout << "H5MicReader:: Reading Data .. " << std::endl;
  err = readData(gid);

  err = H5Gclose(gid);
  err = QH5Utilities::closeFile(fileId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicReader::readHeaderOnly()
{
  int err = -1;
  if (m_HDF5Path.isEmpty() == true)
  {
    std::cout << "H5MicReader Error: HDF5 Path is empty." << std::endl;
    QString ss = QObject::tr("H5MicReader Error: HDF5 Path is empty.");
    setErrorMessage(ss);
    setErrorCode(-102);
    return -102;
  }

  hid_t fileId = QH5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    QString ss = QObject::tr("H5MicReader Error: Could not open HDF5 file '%1'").arg(getFileName());
    setErrorMessage(ss);
    setErrorCode(-100);
    return -100;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    err = QH5Utilities::closeFile(fileId);
    QString ss = QObject::tr("H5MicReader Error: Could not open path '%1'").arg(m_HDF5Path);
    setErrorMessage(ss);
    setErrorCode(-101);
    return -101;
  }

  // Read all the header information
  // std::cout << "H5MicReader:: reading Header .. " << std::endl;
  err = readHeader(gid);
  err = QH5Utilities::closeFile(fileId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5MicReader::readHeader(hid_t parId)
{
  int err = -1;
  hid_t gid = H5Gopen(parId, Ebsd::H5::Header.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    QString ss = QObject::tr("H5MicReader Error: Could not open 'Header' Group").arg(m_HDF5Path);
    setErrorMessage(ss);
    setErrorCode(-105);
    return -105;
  }


  READ_EBSD_HEADER_DATA("H5MicReader", MicHeaderEntry<float>, float, XRes, Ebsd::Mic::XRes, gid)
  READ_EBSD_HEADER_DATA("H5MicReader", MicHeaderEntry<float>, float, YRes, Ebsd::Mic::YRes, gid)
  READ_EBSD_HEADER_DATA("H5MicReader", MicHeaderEntry<int>, int, XDim, Ebsd::Mic::XDim, gid)
  READ_EBSD_HEADER_DATA("H5MicReader", MicHeaderEntry<int>, int, YDim, Ebsd::Mic::YDim, gid)


  hid_t phasesGid = H5Gopen(gid, Ebsd::H5::Phases.toLatin1().data(), H5P_DEFAULT);
  if (phasesGid < 0)
  {
    std::cout << "H5MicReader Error: Could not open Header/Phases HDF Group. Is this an older file?" << std::endl;
    H5Gclose(gid);
    return -1;
  }

  QList<QString> names;
  err = QH5Utilities::getGroupObjects(phasesGid, H5Utilities::H5Support_GROUP, names);
  if (err < 0 || names.size() == 0)
  {
    std::cout << "H5MicReader Error: There were no Phase groups present in the HDF5 file" << std::endl;
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return -1;
  }
  m_Phases.clear();
  for (QList<QString>::iterator phaseGroupName = names.begin(); phaseGroupName != names.end(); ++phaseGroupName )
  {
    hid_t pid = H5Gopen(phasesGid, (*phaseGroupName).toLatin1().data(), H5P_DEFAULT);
    MicPhase::Pointer m_CurrentPhase = MicPhase::New();

    READ_PHASE_HEADER_DATA("H5MicReader", pid, int, Ebsd::Mic::Phase, PhaseIndex, m_CurrentPhase)
    READ_PHASE_HEADER_ARRAY("H5MicReader", pid, float, Ebsd::Mic::LatticeConstants, LatticeConstants, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5MicReader", pid, Ebsd::Mic::BasisAtoms, BasisAtoms, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5MicReader", pid, Ebsd::Mic::Symmetry, Symmetry, m_CurrentPhase)

    m_Phases.push_back(m_CurrentPhase);
    err = H5Gclose(pid);
  }

  QString completeHeader;
  err = QH5Lite::readStringDataset(gid, Ebsd::H5::OriginalHeader, completeHeader);
  setOriginalHeader(completeHeader);
  err = H5Gclose(phasesGid);
  err = H5Gclose(gid);
  return err;
}


#define MIC_READER_ALLOCATE_AND_READ(name, type)\
  if (m_ReadAllArrays == true || m_ArrayNames.find(Ebsd::Mic::name) != m_ArrayNames.end()) {\
    type* _##name = allocateArray<type>(totalDataRows);\
    if (NULL != _##name) {\
      ::memset(_##name, 0, numBytes);\
      err = QH5Lite::readPointerDataset(gid, Ebsd::Mic::name, _##name);\
    }\
    set##name##Pointer(_##name);\
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

  totalDataRows = xDim * yDim;

  hid_t gid = H5Gopen(parId, Ebsd::H5::Data.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5MicReader Error: Could not open 'Data' Group" << std::endl;
    return -1;
  }

  setNumberOfElements(totalDataRows);
  size_t numBytes = totalDataRows * sizeof(float);

  MIC_READER_ALLOCATE_AND_READ(Euler1, float);
  MIC_READER_ALLOCATE_AND_READ(Euler2, float);
  MIC_READER_ALLOCATE_AND_READ(Euler3, float);
  MIC_READER_ALLOCATE_AND_READ(Confidence, float);
  MIC_READER_ALLOCATE_AND_READ(Phase, int);
  MIC_READER_ALLOCATE_AND_READ(X, float);
  MIC_READER_ALLOCATE_AND_READ(Y, float);

  err = H5Gclose(gid);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5MicReader::setArraysToRead(QSet<QString> names)
{
  m_ArrayNames = names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5MicReader::readAllArrays(bool b)
{
  m_ReadAllArrays = b;
}
