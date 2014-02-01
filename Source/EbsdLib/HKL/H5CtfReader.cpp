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


#include "H5CtfReader.h"

#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

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

  hid_t gid = H5Gopen(fileId, m_HDF5Path.toAscii().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    qDebug() << "H5CtfReader Error: Could not open path '" << m_HDF5Path << "'";
    err = QH5Utilities::closeFile(fileId);
    return -1;
  }

  // Read all the header information
  err = readHeader(gid);
  err = QH5Utilities::closeFile(fileId);
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

  hid_t gid = H5Gopen(fileId, m_HDF5Path.toAscii().data(), H5P_DEFAULT);
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
  hid_t gid = H5Gopen(parId, Ebsd::H5::Header.toAscii().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    qDebug() << "H5CtfReader Error: Could not open 'Header' Group";
    return -1;
  }

  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, QString, Prj, Ebsd::Ctf::Prj)
  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, QString, Author, Ebsd::Ctf::Author)
  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, QString, JobMode, Ebsd::Ctf::JobMode)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, XCells, Ebsd::Ctf::XCells)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, YCells, Ebsd::Ctf::YCells)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, XStep, Ebsd::Ctf::XStep)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, YStep, Ebsd::Ctf::YStep)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, AcqE1, Ebsd::Ctf::AcqE1)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, AcqE2, Ebsd::Ctf::AcqE2)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, AcqE3, Ebsd::Ctf::AcqE3)
  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, QString, Euler, Ebsd::Ctf::Euler)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, Mag, Ebsd::Ctf::Mag)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, Coverage, Ebsd::Ctf::Coverage)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, Device, Ebsd::Ctf::Device)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, KV, Ebsd::Ctf::KV)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, TiltAngle, Ebsd::Ctf::TiltAngle)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, TiltAxis, Ebsd::Ctf::TiltAxis)

  hid_t phasesGid = H5Gopen(gid, Ebsd::H5::Phases.toAscii().data(), H5P_DEFAULT);
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
    hid_t pid = H5Gopen(phasesGid, phaseGroupName.toAscii().data(), H5P_DEFAULT);
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

  if(m_ArrayNames.size() == 0 && m_ReadAllArrays == false)
  {
    setErrorMessage("H5CtfReader Error: ReadAllArrays was FALSE and no other arrays were requested to be read.");
    setErrorCode(-90013);
    return -90013;
  }

  hid_t gid = H5Gopen(parId, Ebsd::H5::Data.toAscii().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    qDebug() << "H5CtfReader Error: Could not open 'Data' Group";
    return -1;
  }

  if(m_ArrayNames.size() == 0 && m_ReadAllArrays == false)
  {
    err = H5Gclose(gid);
    err = -90013;
    setErrorMessage("H5CtfReader Error: ReadAllArrays was FALSE and no other arrays were requested to be read.");
    setErrorCode(err);
    return err;
  }

  Ebsd::NumType numType = Ebsd::UnknownNumType;
  QList<QString> columnNames;
  err = QH5Utilities::getGroupObjects(gid, H5Utilities::H5Support_DATASET, columnNames);
  for (QList<QString>::iterator iter = columnNames.begin(); iter != columnNames.end(); ++iter )
  {
    if (m_ReadAllArrays == true || m_ArrayNames.find(*iter) != m_ArrayNames.end())
    {
      numType = getPointerType(*iter);
      if(numType == Ebsd::Int32)
      {
        int32_t* dataPtr = allocateArray<int32_t>(totalDataRows);
        if(NULL == dataPtr)
        {
          BOOST_ASSERT(false);
        } // We are going to crash here. I would rather crash than have bad data
        err = QH5Lite::readPointerDataset(gid, *iter, dataPtr);
        setPointerByName(*iter, dataPtr);
      }
      else if(numType == Ebsd::Float)
      {
        float* dataPtr = allocateArray<float>(totalDataRows);
        if(NULL == dataPtr)
        {
          BOOST_ASSERT(false);
        } // We are going to crash here. I would rather crash than have bad data
        err = QH5Lite::readPointerDataset(gid, *iter, dataPtr);
        setPointerByName(*iter, dataPtr);
      }
      else
      {
        BOOST_ASSERT(false);
        // We are going to crash here because I would rather crash than have bad data
      }
    }
  }

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
