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

#include "H5CtfImporter.h"

#include <cassert>

#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdLibVersion.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

#define AIM_STRING QString

#define CHECK_FOR_CANCELED(AClass)\
  if (m_Cancel == true){\
    break; }


#define WRITE_EBSD_HEADER_DATA(reader, m_msgType, prpty, key)\
  {\
    m_msgType t = reader.get##prpty();\
    err = QH5Lite::writeScalarDataset(gid, key, t);\
    if (err < 0) {\
      QString ss = \
                   QObject::tr("H5CtfImporter Error: Could not write Ctf Header value '%1' to the HDF5 file with data set name '%2'\n")\
                   .arg(t).arg(key);\
      progressMessage(ss, 100);\
      err = H5Gclose(gid); err = H5Gclose(ctfGroup);\
      return -1; }\
  }

#define WRITE_EBSD_HEADER_STRING_DATA(reader, m_msgType, prpty, key)\
  {\
    m_msgType t = reader.get##prpty();\
    err = QH5Lite::writeStringDataset(gid, key, t);\
    if (err < 0) {\
      QString ss = \
                   QObject::tr("H5CtfImporter Error: Could not write Ctf Header value '%1' to the HDF5 file with data set name '%2'\n")\
                   .arg(t).arg(key);\
      progressMessage(ss, 100);\
      err = H5Gclose(gid); err = H5Gclose(ctfGroup);\
      return -1; }\
  }

#define WRITE_EBSD_DATA_ARRAY(reader, m_msgType, gid, key)\
  {\
    if (nullptr != dataPtr) {\
      err = QH5Lite::writePointerDataset(gid, key, rank, dims, dataPtr);\
      if (err < 0) {\
        QString ss = \
                     QObject::tr("H5CtfImporter Error: Could not write Ctf Data array for '%1' to the HDF5 file with data set name '%2'\n")\
                     .arg(key).arg(key);\
        progressMessage(ss, 100);\
        err = H5Gclose(gid); err = H5Gclose(ctfGroup);\
        return -1; }\
    }\
  }



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfImporter::H5CtfImporter():
  EbsdImporter(),
  xDim(0),
  yDim(0),
  xRes(0),
  yRes(0),
  zRes(0),
  m_NumSlicesImported(1),
  m_FileVersion(Ebsd::H5::FileVersion)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfImporter::~H5CtfImporter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfImporter::getDims(int64_t& x, int64_t& y)
{
  x = xDim;
  y = yDim;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfImporter::getResolution(float& x, float& y)
{
  x = xRes;
  y = yRes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfImporter::numberOfSlicesImported()
{
  return m_NumSlicesImported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfImporter::importFile(hid_t fileId, int64_t z, const QString& ctfFile)
{
  herr_t err = -1;
  setCancel(false);
  setErrorCondition(0);
  setPipelineMessage("");

  //  std::cout << "H5CtfImporter: Importing " << ctfFile << std::endl;
  CtfReader reader;
  reader.setFileName(ctfFile);

  // Now actually read the file
  err = reader.readFile();

  // Check for errors
  if (err < 0)
  {

    QString ss;
    if (err == -200)
    {
      ss = "H5CtfImporter Error: There was no data in the file.";
    }
    else if (err == -100)
    {
      ss = "H5CtfImporter Error: The Ctf file could not be opened.";
    }
    else if (reader.getXStep() == 0.0f)
    {
      ss = "H5CtfImporter Error: X Step value equals 0.0. This is bad. Please check the validity of the CTF file.";
    }
    else if(reader.getYStep() == 0.0f)
    {
      ss = "H5CtfImporter Error: Y Step value equals 0.0. This is bad. Please check the validity of the CTF file.";
    }
    else
    {
      ss = reader.getErrorMessage();
    }
    setPipelineMessage(ss);
    setErrorCondition(err);
    progressMessage(ss, 100);


    return -1;
  }

  // Write the fileversion attribute if it does not exist
  {
    QVector<hsize_t> dims;
    H5T_class_t type_class;
    size_t type_size = 0;
    hid_t attr_type = -1;
    err = QH5Lite::getAttributeInfo(fileId, "/", Ebsd::H5::FileVersionStr, dims, type_class, type_size, attr_type);
    if (attr_type < 0) // The attr_type variable was never set which means the attribute was NOT there
    {
      // The file version does not exist so write it to the file
      err = QH5Lite::writeScalarAttribute(fileId, QString("/"), Ebsd::H5::FileVersionStr, m_FileVersion);
    }
    else
    {
      H5Aclose(attr_type);
    }

    err = QH5Lite::getAttributeInfo(fileId, "/", Ebsd::H5::EbsdLibVersionStr, dims, type_class, type_size, attr_type);
    if (attr_type < 0) // The attr_type variable was never set which means the attribute was NOT there
    {
      // The file version does not exist so write it to the file
      err = QH5Lite::writeStringAttribute(fileId, QString("/"), Ebsd::H5::EbsdLibVersionStr, EbsdLib::Version::Complete());
    }
    else
    {
      H5Aclose(attr_type);
    }
  }


  int zSlices = reader.getZCells();

  // This scheme is going to fail if the user has multiple HKL 3D files where each
  // file as multiple slices. We really need to keep track of what slice we are
  // on at the next level up that this level.
  m_NumSlicesImported = 0;
  for(int slice = 0; slice < zSlices; ++slice)
  {
    writeSliceData(fileId, reader, static_cast<int>(z) + slice, slice);
    ++m_NumSlicesImported;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfImporter::writeSliceData(hid_t fileId, CtfReader& reader, int z, int actualSlice)
{
  //  std::cout << "Writing Slice " << actualSlice << " as " << z << std::endl;
  int err = 0;
  // Start creating the HDF5 group structures for this file
  hid_t ctfGroup = QH5Utilities::createGroup(fileId, QString::number(z));
  if(ctfGroup < 0)
  {
    QString ss = QObject::tr("H5CtfImporter Error: A Group for Z index %1 could not be created. Please check other error messages from the HDF5 library for possible reasons.").arg(z);
    setPipelineMessage(ss);
    setErrorCondition(-500);
    return -1;
  }

  hid_t gid = QH5Utilities::createGroup(ctfGroup, Ebsd::H5::Header);
  if(gid < 0)
  {
    QString ss = QObject::tr("H5CtfImporter Error: The 'Header' Group for Z index %1 could not be created. Please check other error messages from the HDF5 library for possible reasons.").arg(z);
    progressMessage(ss, 100);
    err = H5Gclose(ctfGroup);
    setPipelineMessage(ss);
    setErrorCondition(-600);
    return -1;
  }

  WRITE_EBSD_HEADER_STRING_DATA(reader, QString, Prj, Ebsd::Ctf::Prj);
  WRITE_EBSD_HEADER_STRING_DATA(reader, QString, Author, Ebsd::Ctf::Author);
  WRITE_EBSD_HEADER_STRING_DATA(reader, QString, JobMode, Ebsd::Ctf::JobMode);
  WRITE_EBSD_HEADER_DATA(reader, int, XCells, Ebsd::Ctf::XCells)
  xDim = reader.getXCells();
  WRITE_EBSD_HEADER_DATA(reader, int, YCells, Ebsd::Ctf::YCells)
  yDim = reader.getYCells();
  WRITE_EBSD_HEADER_DATA(reader, float, XStep, Ebsd::Ctf::XStep)
  xRes = reader.getXStep();
  WRITE_EBSD_HEADER_DATA(reader, float, YStep, Ebsd::Ctf::YStep)
  yRes = reader.getYStep();

  float* zPtr = reader.getZPointer();
  if(nullptr != zPtr)
  {
    WRITE_EBSD_HEADER_DATA(reader, int, ZCells, Ebsd::Ctf::ZCells)
    zDim = reader.getZCells();
    WRITE_EBSD_HEADER_DATA(reader, float, ZStep, Ebsd::Ctf::ZStep)
    zRes = reader.getZStep();
  }

  WRITE_EBSD_HEADER_DATA(reader, float, AcqE1, Ebsd::Ctf::AcqE1);
  WRITE_EBSD_HEADER_DATA(reader, float, AcqE2, Ebsd::Ctf::AcqE2);
  WRITE_EBSD_HEADER_DATA(reader, float, AcqE3, Ebsd::Ctf::AcqE3);
  WRITE_EBSD_HEADER_STRING_DATA(reader, QString, Euler, Ebsd::Ctf::Euler);
  WRITE_EBSD_HEADER_DATA(reader, int, Mag, Ebsd::Ctf::Mag);
  WRITE_EBSD_HEADER_DATA(reader, int, Coverage, Ebsd::Ctf::Coverage);
  WRITE_EBSD_HEADER_DATA(reader, int, Device, Ebsd::Ctf::Device);
  WRITE_EBSD_HEADER_DATA(reader, int, KV, Ebsd::Ctf::KV);
  WRITE_EBSD_HEADER_DATA(reader, float, TiltAngle, Ebsd::Ctf::TiltAngle);
  WRITE_EBSD_HEADER_DATA(reader, float, TiltAxis, Ebsd::Ctf::TiltAxis)

  hid_t phasesGid = QH5Utilities::createGroup(gid, Ebsd::H5::Phases);
  if(phasesGid < 0)
  {
    QString ss = QObject::tr("H5CtfImporter Error: The 'Header' Group for the Phases could not be created."
                             " Please check other error messages from the HDF5 library for possible reasons.");

    progressMessage(ss, 100);
    err = H5Gclose(gid);
    err = H5Gclose(ctfGroup);
    setPipelineMessage(ss);
    setErrorCondition(-600);
    return -1;
  }
  err = writePhaseData(reader, phasesGid);
  // Close this group
  err = H5Gclose(phasesGid);

  QString ctfCompleteHeader = reader.getOriginalHeader();
  err = QH5Lite::writeStringDataset(gid, Ebsd::H5::OriginalHeader, ctfCompleteHeader);
  err = QH5Lite::writeStringDataset(gid, Ebsd::H5::OriginalFile, reader.getFileName());

  // Close the "Header" group
  err = H5Gclose(gid);

  // Create the "Data" group
  gid = QH5Utilities::createGroup(ctfGroup, Ebsd::H5::Data);
  if(gid < 0)
  {
    QString ss = QObject::tr("H5CtfImporter Error: The 'Data' Group for Z index %1 could not be created."
                             " Please check other error messages from the HDF5 library for possible reasons.\n").arg(z);
    progressMessage(ss, 100);
    err = H5Gclose(ctfGroup);
    setPipelineMessage(ss);
    setErrorCondition(-700);
    return -1;
  }

  int32_t rank = 1;
  hsize_t dims[1] =
  { static_cast<hsize_t> (reader.getXCells() * reader.getYCells()) };

  Ebsd::NumType numType = Ebsd::UnknownNumType;
  QList<QString> columnNames = reader.getColumnNames();
  for (qint32 i = 0; i < columnNames.size(); ++i)
  {
    numType = reader.getPointerType(columnNames[i]);
    if(numType == Ebsd::Int32)
    {
      int32_t* dataPtr = static_cast<int32_t*>(reader.getPointerByName(columnNames[i]));
      if(nullptr == dataPtr)
      {
        assert(false);
      } // We are going to crash here. I would rather crash than have bad data
      dataPtr = dataPtr + (actualSlice * dims[0]); // Put the pointer at the proper offset into the larger array
      WRITE_EBSD_DATA_ARRAY(reader, int, gid, columnNames[i]);
    }
    else if(numType == Ebsd::Float)
    {
      float* dataPtr = static_cast<float*>(reader.getPointerByName(columnNames[i]));
      if(nullptr == dataPtr)
      {
        assert(false);
      } // We are going to crash here. I would rather crash than have bad data
      dataPtr = dataPtr + (actualSlice * dims[0]); // Put the pointer at the proper offset into the larger array
      WRITE_EBSD_DATA_ARRAY(reader, float, gid, columnNames[i]);
    }
    else
    {
      assert(false);
      // We are going to crash here because I would rather crash than have bad data
    }
  }

  // Close the "Data" group
  err = H5Gclose(gid);

  // Close the group for this file
  err = H5Gclose(ctfGroup);
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define WRITE_PHASE_HEADER_DATA(reader, m_msgType, prpty, key)\
  {\
    m_msgType t = reader->get##prpty();\
    err = QH5Lite::writeScalarDataset(pid, key, t);\
    if (err < 0) {\
      QString ss = \
                   QObject::tr("H5CtfImporter Error: Could not write Ctf Header value '%1' to the HDF5 file with data set name '%2'\n")\
                   .arg(t).arg(key);\
      progressMessage(ss, 100);\
      err = H5Gclose(pid);\
      return -1; }\
  }

#define WRITE_PHASE_HEADER_STRING_DATA(reader, m_msgType, prpty, key)\
  {\
    m_msgType t = reader->get##prpty();\
    err = QH5Lite::writeStringDataset(pid, key, t);\
    if (err < 0) {\
      QString ss = \
                   QObject::tr("H5CtfImporter Error: Could not write Ctf Header value '%1' to the HDF5 file with data set name '%2'\n")\
                   .arg(t).arg(key);\
      progressMessage(ss, 100);\
      progressMessage(ss, 100);\
      err = H5Gclose(pid);\
      return -1; }\
  }

#define WRITE_PHASE_DATA_ARRAY(reader, m_msgType, gid, prpty, key)\
  {\
    QVector<m_msgType> tempVar = reader->get##prpty();\
    dims[0] = tempVar.size();\
    m_msgType* dataPtr = &(tempVar.front());\
    if (nullptr != dataPtr) {\
      err = QH5Lite::writePointerDataset(pid, key, rank, dims, dataPtr);\
      if (err < 0) {\
        QString ss = \
                     QObject::tr("H5CtfImporter Error: Could not write Ctf Data array for '%1' to the HDF5 file with data set name '%2'\n")\
                     .arg(key).arg(key);\
        progressMessage(ss, 100);\
        err = H5Gclose(pid); \
        return -1; }\
    }\
  }



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfImporter::writePhaseData(CtfReader& reader, hid_t phasesGid)
{
  int err = 0;
  // int retErr = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 0 };
  QVector<CtfPhase::Pointer> phases = reader.getPhaseVector();
  Ebsd::Ctf::LaueGroupStrings laueGroupStrings;
  for (QVector<CtfPhase::Pointer>::iterator phase = phases.begin(); phase != phases.end(); ++phase )
  {
    CtfPhase* p = (*phase).get();
    hid_t pid = QH5Utilities::createGroup(phasesGid, QString::number(p->getPhaseIndex()));

    WRITE_PHASE_DATA_ARRAY( (*phase), float, pid, LatticeConstants, Ebsd::Ctf::LatticeConstants);
    WRITE_PHASE_HEADER_STRING_DATA((*phase), QString, PhaseName, Ebsd::Ctf::PhaseName)
    WRITE_PHASE_HEADER_DATA((*phase), int, LaueGroup, Ebsd::Ctf::LaueGroup)

    err = QH5Lite::writeStringAttribute(pid, Ebsd::Ctf::LaueGroup, "Name",
                                        laueGroupStrings.getString(p->getLaueGroup()));
    if (err < 0)
    {
      QString ss =
        QObject::tr("H5CtfImporter Error: Could not write Ctf Attribute 'Name' to Dataset '%1'").arg(Ebsd::Ctf::LaueGroup);
      progressMessage(ss, 100);
      err = H5Gclose(pid);
      return -1;
    }

    WRITE_PHASE_HEADER_DATA((*phase), int, SpaceGroup, Ebsd::Ctf::SpaceGroup)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), QString, Internal1, Ebsd::Ctf::Internal1)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), QString, Internal2, Ebsd::Ctf::Internal2)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), QString, Comment, Ebsd::Ctf::Comment)
    err = H5Gclose(pid);
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5CtfImporter::setFileVersion(uint32_t version)
{
  m_FileVersion = version;
}

