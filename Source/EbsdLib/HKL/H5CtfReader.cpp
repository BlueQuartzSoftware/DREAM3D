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
#include "H5Support/H5ScopedSentinel.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/HKL/CtfConstants.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfReader::H5CtfReader()
: m_ReadAllArrays(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfReader::~H5CtfReader()
{
  if(m_PhaseCleanup)
  {
    deallocateArrayData<int32_t>(m_Phase);
    m_Phase = nullptr;
  }
  if(m_XCleanup)
  {
    deallocateArrayData<float>(m_X);
    m_X = nullptr;
  }
  if(m_YCleanup)
  {
    deallocateArrayData<float>(m_Y);
    m_Y = nullptr;
  }
  if(m_ZCleanup)
  {
    deallocateArrayData<float>(m_Z);
    m_Z = nullptr;
  }
  if(m_BandsCleanup)
  {
    deallocateArrayData<int32_t>(m_Bands);
    m_Bands = nullptr;
  }
  if(m_ErrorCleanup)
  {
    deallocateArrayData<int32_t>(m_Error);
    m_Error = nullptr;
  }
  if(m_Euler1Cleanup)
  {
    deallocateArrayData<float>(m_Euler1);
    m_Euler1 = nullptr;
  }
  if(m_Euler2Cleanup)
  {
    deallocateArrayData<float>(m_Euler2);
    m_Euler2 = nullptr;
  }
  if(m_Euler3Cleanup)
  {
    deallocateArrayData<float>(m_Euler3);
    m_Euler3 = nullptr;
  }
  if(m_MADCleanup)
  {
    deallocateArrayData<float>(m_MAD);
    m_MAD = nullptr;
  }
  if(m_BCCleanup)
  {
    deallocateArrayData<int32_t>(m_BC);
    m_BC = nullptr;
  }
  if(m_BSCleanup)
  {
    deallocateArrayData<int32_t>(m_BS);
    m_BS = nullptr;
  }
  if(m_GrainIndexCleanup)
  {
    deallocateArrayData<int32_t>(m_GrainIndex);
    m_GrainIndex = nullptr;
  }
  if(m_GrainRandomColourRCleanup)
  {
    deallocateArrayData<int32_t>(m_GrainRandomColourR);
    m_GrainRandomColourR = nullptr;
  }
  if(m_GrainRandomColourGCleanup)
  {
    deallocateArrayData<int32_t>(m_GrainRandomColourG);
    m_GrainRandomColourG = nullptr;
  }
  if(m_GrainRandomColourBCleanup)
  {
    deallocateArrayData<int32_t>(m_GrainRandomColourB);
    m_GrainRandomColourB = nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfReader::readHeaderOnly()
{
  int err = -1;
  if(m_HDF5Path.isEmpty())
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

  H5ScopedFileSentinel sentinel(&fileId, true);

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
  if(m_HDF5Path.isEmpty())
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
  err = readHeader(gid);

  // Read and transform data
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
  hid_t gid = H5Gopen(parId, Ebsd::H5Aztec::Header.toLatin1().data(), H5P_DEFAULT);
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

  hid_t phasesGid = H5Gopen(gid, Ebsd::H5Aztec::Phases.toLatin1().data(), H5P_DEFAULT);
  if (phasesGid < 0)
  {
    setErrorCode(-90007);
    setErrorMessage("H5CtfReader Error: Could not open Header/Phases HDF Group.");
    H5Gclose(gid);
    return -1;
  }

  QList<QString> names;
  err = QH5Utilities::getGroupObjects(phasesGid, H5Utilities::H5Support_GROUP, names);
  if(err < 0 || names.empty())
  {
    setErrorCode(-90009);
    setErrorMessage("H5CtfReader Error: There were no Phase groups present in the HDF5 file");
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return -1;
  }
  bool ok = false;
  m_Phases.clear();

  for(const auto& phaseGroupName : names)
  {
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

  QString completeHeader;
  err = QH5Lite::readStringDataset(gid, Ebsd::H5Aztec::OriginalHeader, completeHeader);
  if(err < 0)
  {
    setErrorCode(-90010);
    setErrorMessage("The dataset 'Original Header' was missing from the HDF5 file.");
  }
  setOriginalHeader(completeHeader);
  err = H5Gclose(phasesGid);
  err = H5Gclose(gid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfReader::readData(hid_t parId)
{
  int err = -1;

  size_t yCells = getYCells();
  size_t xCells = getXCells();
  size_t totalDataRows = yCells * xCells;
  if( totalDataRows == 0)
  {
    setErrorCode(-1);
    setErrorMessage(QString("TotalDataRows = 0;"));
    return -1;
  }

  hid_t gid = H5Gopen(parId, Ebsd::H5Aztec::Data.toLatin1(), H5P_DEFAULT);
  if (gid < 0)
  {
    setErrorMessage("H5CtfReader Error: Could not open 'Data' Group");
    setErrorCode(-90012);
    return getErrorCode();
  }

  setNumberOfElements(totalDataRows);
  size_t numBytes = totalDataRows * sizeof(float);
  QString sBuf;
  QTextStream ss(&sBuf);

  if(m_ArrayNames.empty() && !m_ReadAllArrays)
  {
    err = H5Gclose(gid);
    err = -90013;
    setErrorMessage("H5CtfReader Error: ReadAllArrays was FALSE and no other arrays were requested to be read.");
    setErrorCode(err);
    return err;
  }

  ANG_READER_ALLOCATE_AND_READ(Phase, Ebsd::Ctf::Phase, int);
  ANG_READER_ALLOCATE_AND_READ(BandCount, Ebsd::Ctf::Bands, int);
  ANG_READER_ALLOCATE_AND_READ(Error, Ebsd::Ctf::Error, int);
  ANG_READER_ALLOCATE_AND_READ(Euler1, Ebsd::Ctf::Euler1, float);
  ANG_READER_ALLOCATE_AND_READ(Euler2, Ebsd::Ctf::Euler2, float);
  ANG_READER_ALLOCATE_AND_READ(Euler3, Ebsd::Ctf::Euler3, float);
  ANG_READER_ALLOCATE_AND_READ(MeanAngularDeviation, Ebsd::Ctf::MAD, float);
  ANG_READER_ALLOCATE_AND_READ(BandContrast, Ebsd::Ctf::BC, int);
  ANG_READER_ALLOCATE_AND_READ(BandSlope, Ebsd::Ctf::BS, int);
  ANG_READER_ALLOCATE_AND_READ(GrainIndex, Ebsd::Ctf::GrainIndex, int);
  ANG_READER_ALLOCATE_AND_READ(GrainRandomColourR, Ebsd::Ctf::GrainRandomColourR, int);
  ANG_READER_ALLOCATE_AND_READ(GrainRandomColourG, Ebsd::Ctf::GrainRandomColourG, int);
  ANG_READER_ALLOCATE_AND_READ(GrainRandomColourB, Ebsd::Ctf::GrainRandomColourB, int);

  err = H5Gclose(gid);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfReader::setArraysToRead(const QSet<QString>& names)
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
