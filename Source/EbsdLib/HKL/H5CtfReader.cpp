/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */



#include "H5CtfReader.h"

#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/HKL/CtfConstants.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfReader::H5CtfReader() :
  CtfReader(),
  m_ReadAllArrays(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfReader::~H5CtfReader()
{
  deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfReader::readHeaderOnly()
{
  int err = -1;
  if (m_HDF5Path.isEmpty() == true)
  {
    qDebug() << "H5CtfReader Error: HDF5 Path is empty.";
    return -1;
  }

  hid_t fileId = QH5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    qDebug() << "H5CtfReader Error: Could not open HDF5 file '" << getFileName() << "'";
    return -1;
  }

  HDF5ScopedFileSentinel sentinel(&fileId, true);

  hid_t gid = H5Gopen(fileId, m_HDF5Path.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    qDebug() << "H5CtfReader Error: Could not open path '" << m_HDF5Path << "'";
    err = QH5Utilities::closeFile(fileId);
    return -1;
  }
  sentinel.addGroupId(&gid);

  // Read all the header information
  err = readHeader(gid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfReader::readFile()
{
  int err = -1;
  if (m_HDF5Path.isEmpty() == true)
  {
    qDebug() << "H5CtfReader Error: HDF5 Path is empty.";
    return -1;
  }

  hid_t fileId = QH5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    qDebug() << "H5CtfReader Error: Could not open HDF5 file '" << getFileName() << "'";
    return -1;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    qDebug() << "H5CtfReader Error: Could not open path '" << m_HDF5Path << "'";
    err = QH5Utilities::closeFile(fileId);
    return -1;
  }

  // Read all the header information
// qDebug() << "H5CtfReader:: reading Header .. ";
  err = readHeader(gid);

  // Read and transform data
// qDebug() << "H5CtfReader:: Reading Data .. ";
  err = readData(gid);

  err = H5Gclose(gid);
  err = QH5Utilities::closeFile(fileId);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfReader::readHeader(hid_t parId)
{
  QString sBuf;
  QTextStream ss(&sBuf);
  int err = -1;
  hid_t gid = H5Gopen(parId, Ebsd::H5::Header.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    qDebug() << "H5CtfReader Error: Could not open 'Header' Group";
    return -1;
  }

  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, QString, Prj, Ebsd::Ctf::Prj, gid)
  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, QString, Author, Ebsd::Ctf::Author, gid)
  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, QString, JobMode, Ebsd::Ctf::JobMode, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, XCells, Ebsd::Ctf::XCells, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, YCells, Ebsd::Ctf::YCells, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, XStep, Ebsd::Ctf::XStep, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, YStep, Ebsd::Ctf::YStep, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, AcqE1, Ebsd::Ctf::AcqE1, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, AcqE2, Ebsd::Ctf::AcqE2, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, AcqE3, Ebsd::Ctf::AcqE3, gid)
  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, QString, Euler, Ebsd::Ctf::Euler, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, Mag, Ebsd::Ctf::Mag, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, Coverage, Ebsd::Ctf::Coverage, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, Device, Ebsd::Ctf::Device, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, KV, Ebsd::Ctf::KV, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, TiltAngle, Ebsd::Ctf::TiltAngle, gid)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, TiltAxis, Ebsd::Ctf::TiltAxis, gid)

  hid_t phasesGid = H5Gopen(gid, Ebsd::H5::Phases.toLatin1().data(), H5P_DEFAULT);
  if (phasesGid < 0)
  {
    qDebug() << "H5CtfReader Error: Could not open Header/Phases HDF Group. Is this an older file?";
    H5Gclose(gid);
    return -1;
  }

  QList<QString> names;
  err = QH5Utilities::getGroupObjects(phasesGid, H5Utilities::H5Support_GROUP, names);
  if (err < 0 || names.size() == 0)
  {
    qDebug() << "H5CtfReader Error: There were no Phase groups present in the HDF5 file";
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return -1;
  }
  bool ok = false;
  m_Phases.clear();
  for (int p = 0; p < names.size(); ++p)
  {
    QString phaseGroupName = names[p];
    hid_t pid = H5Gopen(phasesGid, phaseGroupName.toLatin1().data(), H5P_DEFAULT);
    CtfPhase::Pointer m_CurrentPhase = CtfPhase::New();

    READ_PHASE_HEADER_ARRAY("H5CtfReader", pid, float, Ebsd::Ctf::LatticeConstants, LatticeConstants, m_CurrentPhase);
    READ_PHASE_STRING_DATA("H5CtfReader", pid, Ebsd::Ctf::PhaseName, PhaseName, m_CurrentPhase)
    READ_PHASE_HEADER_DATA_CAST("H5CtfReader", pid, Ebsd::Ctf::LaueGroupTable, int, Ebsd::Ctf::LaueGroup, LaueGroup, m_CurrentPhase)
    READ_PHASE_HEADER_DATA_CAST("H5CtfReader", pid, int, int, Ebsd::Ctf::SpaceGroup, SpaceGroup, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5CtfReader", pid, Ebsd::Ctf::Internal1, Internal1, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5CtfReader", pid, Ebsd::Ctf::Internal2, Internal2, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5CtfReader", pid, Ebsd::Ctf::Comment, Comment, m_CurrentPhase)

    // For HKL Imports, the phase index is the HDF5 Group Name for this phase so
    // convert the phaseGroupName string variable into an integer
    int pIndex = phaseGroupName.toInt(&ok, 10);
    m_CurrentPhase->setPhaseIndex(pIndex);
    m_Phases.push_back(m_CurrentPhase);
    err = H5Gclose(pid);
  }
  err = H5Gclose(phasesGid);

  QString completeHeader;
  err = QH5Lite::readStringDataset(gid, Ebsd::H5::OriginalHeader, completeHeader);
  setOriginalHeader(completeHeader);

  err = H5Gclose(gid);
  return err;
}


#define CTF_READER_ALLOCATE_AND_READ(name, type)\
  if (m_ReadAllArrays == true || m_ArrayNames.find(Ebsd::Ctf::name) != m_ArrayNames.end()) {\
    type* _##name = allocateArray<type>(totalDataRows);\
    if (NULL != _##name) {\
      ::memset(_##name, 0, numBytes);\
      err = QH5Lite::readPointerDataset(gid, Ebsd::Ctf::name, _##name);\
      if (err < 0) {\
        deallocateArrayData(_##name); /*deallocate the array*/\
        _##name = NULL;\
        setErrorCode(-90020);\
        ss << "Error reading dataset '" << #name << "' from the HDF5 file. This data set is required to be in the file because either "\
           "the program is set to read ALL the Data arrays or the program was instructed to read this array.";\
        setErrorMessage(ss.string());\
        err = H5Gclose(gid);if (err < 0) {}\
        return getErrorCode();\
      }\
    }\
    setPointerByName(#name, _##name);\
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfReader::readData(hid_t parId)
{
  int err = -1;

  // Delete any currently existing pointers
  deletePointers();

  size_t yCells = getYCells();
  size_t xCells = getXCells();
  size_t totalDataRows = yCells * xCells;
  if( totalDataRows == 0)
  {
    setErrorCode(-1);
    setErrorMessage(QString("TotalDataRows = 0;"));
    return -1;
  }


  hid_t gid = H5Gopen(parId, Ebsd::H5::Data.toLatin1(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5CtfReader Error: Could not open 'Data' Group" << std::endl;
    return -1;
  }

  setNumberOfElements(totalDataRows);
  size_t numBytes = totalDataRows * sizeof(float);
  QString sBuf;
  QTextStream ss(&sBuf);

  CTF_READER_ALLOCATE_AND_READ(Phase, int);
  CTF_READER_ALLOCATE_AND_READ(Bands, int);
  CTF_READER_ALLOCATE_AND_READ(Error, int);
  CTF_READER_ALLOCATE_AND_READ(Euler1, float);
  CTF_READER_ALLOCATE_AND_READ(Euler2, float);
  CTF_READER_ALLOCATE_AND_READ(Euler3, float);
  CTF_READER_ALLOCATE_AND_READ(MAD, float);
  CTF_READER_ALLOCATE_AND_READ(BC, int);
  CTF_READER_ALLOCATE_AND_READ(BS, int);
  CTF_READER_ALLOCATE_AND_READ(GrainIndex, int);
  CTF_READER_ALLOCATE_AND_READ(GrainRandomColourR, int);
  CTF_READER_ALLOCATE_AND_READ(GrainRandomColourG, int);
  CTF_READER_ALLOCATE_AND_READ(GrainRandomColourB, int);


  err = H5Gclose(gid);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfReader::setArraysToRead(QSet<QString> names)
{
  m_ArrayNames = names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfReader::readAllArrays(bool b)
{
  m_ReadAllArrays = b;
}
