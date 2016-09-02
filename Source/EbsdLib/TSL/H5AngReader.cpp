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


#include "H5AngReader.h"

#include <vector>

#include <QtCore/QtDebug>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include "AngConstants.h"

#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdMacros.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngReader::H5AngReader() :
  AngReader(),
  m_ReadAllArrays(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngReader::~H5AngReader()
{
  deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readFile()
{
  int err = -1;
  if (m_HDF5Path.isEmpty() == true)
  {
    qDebug() << "H5AngReader Error: HDF5 Path is empty.";
    return err;
  }

  hid_t fileId = QH5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    qDebug() << "H5AngReader Error: Could not open HDF5 file '" << getFileName() << "'";
    return err;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    qDebug() << "H5AngReader Error: Could not open path '" << m_HDF5Path << "'";
    err = QH5Utilities::closeFile(fileId);
    return -1;
  }

  // Read all the header information
  err = readHeader(gid);
  if(err < 0)
  {
    qDebug() << "H5AngReader Error: could not read header";
    err = QH5Utilities::closeFile(fileId);
    return err;
  }

  // Read and transform data
  err = readData(gid);
  if(err < 0)
  {
    qDebug() << "H5AngReader Error: could not read data";
    err = QH5Utilities::closeFile(fileId);
    return err;
  }

  err = H5Gclose(gid);
  if(err < 0)
  {
    qDebug() << "H5AngReader Error: could not close group id ";
    err = QH5Utilities::closeFile(fileId);
    return err;
  }

  err = QH5Utilities::closeFile(fileId);
  if(err < 0)
  {
    qDebug() << "H5AngReader Error: could not close file";
    err = QH5Utilities::closeFile(fileId);
    return err;
  }

  return getErrorCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readHeaderOnly()
{
  int err = -1;
  if (m_HDF5Path.isEmpty() == true)
  {
    qDebug() << "H5AngReader Error: HDF5 Path is empty.";
    return -1;
  }

  hid_t fileId = QH5Utilities::openFile(getFileName().toLatin1().data(), true);
  if (fileId < 0)
  {
    qDebug() << "H5AngReader Error: Could not open HDF5 file '" << getFileName() << "'";
    return -1;
  }

  HDF5ScopedFileSentinel sentinel(&fileId, true);

  hid_t gid = H5Gopen(fileId, m_HDF5Path.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    qDebug() << "H5AngReader Error: Could not open path '" << m_HDF5Path << "'";
    err = QH5Utilities::closeFile(fileId);
    return -1;
  }
  sentinel.addGroupId(&gid);

  // Read all the header information
  // qDebug() << "H5AngReader:: reading Header .. ";
  err = readHeader(gid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readHeader(hid_t parId)
{
  int err = -1;
  hid_t gid = H5Gopen(parId, Ebsd::H5::Header.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    setErrorCode(-90008);
    setErrorMessage("H5AngReader Error: Could not open 'Header' Group");
    return -1;
  }

  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, TEMPIXPerUM, Ebsd::Ang::TEMPIXPerUM, gid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, XStar, Ebsd::Ang::XStar, gid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, YStar, Ebsd::Ang::YStar, gid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, ZStar, Ebsd::Ang::ZStar, gid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, WorkingDistance, Ebsd::Ang::WorkingDistance, gid)
  READ_EBSD_HEADER_STRING_DATA("H5AngReader", AngStringHeaderEntry, QString, Grid, Ebsd::Ang::Grid, gid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, XStep, Ebsd::Ang::XStep, gid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, YStep, Ebsd::Ang::YStep, gid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<int>, int, NumOddCols, Ebsd::Ang::NColsOdd, gid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<int>, int, NumEvenCols, Ebsd::Ang::NColsEven, gid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<int>, int, NumRows, Ebsd::Ang::NRows, gid)
  READ_EBSD_HEADER_STRING_DATA("H5AngReader", AngStringHeaderEntry, QString, OIMOperator, Ebsd::Ang::OPERATOR, gid)
  READ_EBSD_HEADER_STRING_DATA("H5AngReader", AngStringHeaderEntry, QString, SampleID, Ebsd::Ang::SAMPLEID, gid)
  READ_EBSD_HEADER_STRING_DATA("H5AngReader", AngStringHeaderEntry, QString, SCANID, Ebsd::Ang::SCANID, gid)

  hid_t phasesGid = H5Gopen(gid, Ebsd::H5::Phases.toLatin1().data(), H5P_DEFAULT);
  if (phasesGid < 0)
  {
    setErrorCode(-90007);
    setErrorMessage("H5AngReader Error: Could not open Header/Phases HDF Group.");
    H5Gclose(gid);
    return -1;
  }

  QStringList names;
  err = QH5Utilities::getGroupObjects(phasesGid, H5Utilities::H5Support_GROUP, names);
  if (err < 0 || names.size() == 0)
  {
    setErrorCode(-90009);
    setErrorMessage("H5AngReader Error: There were no Phase groups present in the HDF5 file");
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return -1;
  }
  m_Phases.clear();


  foreach(QString phaseGroupName, names)
    //for (QStringList<QString>::iterator phaseGroupName = names.begin(); phaseGroupName != names.end(); ++phaseGroupName )
  {
    hid_t pid = H5Gopen(phasesGid, phaseGroupName.toLatin1().data(), H5P_DEFAULT);
    AngPhase::Pointer m_CurrentPhase = AngPhase::New();
    READ_PHASE_HEADER_DATA("H5AngReader", pid, int, Ebsd::Ang::Phase, PhaseIndex, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5AngReader", pid, Ebsd::Ang::MaterialName, MaterialName, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5AngReader", pid, Ebsd::Ang::Formula, Formula, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5AngReader", pid, Ebsd::Ang::Info, Info, m_CurrentPhase)
    READ_PHASE_HEADER_DATA_CAST("H5AngReader", pid, uint32_t, int, Ebsd::Ang::Symmetry, Symmetry, m_CurrentPhase)
    READ_PHASE_HEADER_ARRAY("H5AngReader", pid, float, Ebsd::Ang::LatticeConstants, LatticeConstants, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5AngReader", pid, int, Ebsd::Ang::NumberFamilies, NumberFamilies, m_CurrentPhase)

    if (m_CurrentPhase->getNumberFamilies() > 0)
    {
      hid_t hklGid = H5Gopen(pid, Ebsd::Ang::HKLFamilies.toLatin1().data(), H5P_DEFAULT);
      // Only read the HKL Families if they are there. Trying to open the group will tell us if there
      // are any families to read

      err = readHKLFamilies(hklGid, m_CurrentPhase);
      err = H5Gclose(hklGid);
      if (getErrorCode() < 0) { err = H5Gclose(pid); H5Gclose(phasesGid); H5Gclose(gid); return -1; }
    }
    /* The 'Categories' header may actually be missing from certain types of .ang files */
    if (QH5Lite::datasetExists(pid, Ebsd::Ang::Categories) == true)
    {
      READ_PHASE_HEADER_ARRAY("H5AngReader", pid, int, Ebsd::Ang::Categories, Categories, m_CurrentPhase)
    }
    m_Phases.push_back(m_CurrentPhase);
    err = H5Gclose(pid);
  }

  QString completeHeader;
  err = QH5Lite::readStringDataset(gid, Ebsd::H5::OriginalHeader, completeHeader);
  if (err < 0)
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
int H5AngReader::readHKLFamilies(hid_t hklGid, AngPhase::Pointer phase)
{

  hid_t dataset, memtype;
  herr_t status = 1;
  HKLFamily_t data;
  QVector<HKLFamily::Pointer> families;
  for (int i = 0; i < phase->getNumberFamilies(); ++i)
  {
    QString dsetName = QString::number(i);

    dataset = H5Dopen(hklGid, dsetName.toLatin1().data(), H5P_DEFAULT);

    memtype = H5Tcreate (H5T_COMPOUND, sizeof (HKLFamily_t));
    status = H5Tinsert(memtype, "H", HOFFSET (HKLFamily_t, h), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "K", HOFFSET (HKLFamily_t, k), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "L", HOFFSET (HKLFamily_t, l), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "Solution 1", HOFFSET (HKLFamily_t, s1), H5T_NATIVE_CHAR);
    status = H5Tinsert(memtype, "Diffraction Intensity", HOFFSET (HKLFamily_t, diffractionIntensity), H5T_NATIVE_FLOAT);
    status = H5Tinsert(memtype, "Solution 2", HOFFSET (HKLFamily_t, s2), H5T_NATIVE_CHAR);

    status = H5Dread(dataset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void*)(&data));
    if (status < 0)
    {
      setErrorCode(-90011);
      QString ss = QObject::tr("H5AngReader Error: Could not read the HKLFamily data for family number ").arg(i);
      setErrorMessage(ss);
      break;
    }
    status = H5Dclose(dataset); // Close the data set
    status = H5Tclose(memtype);
    HKLFamily::Pointer f = HKLFamily::New();
    f->copyFromStruct(&data);
    families.push_back(f);
  }
  phase->setHKLFamilies(families);
  return status;
}


#define ANG_READER_ALLOCATE_AND_READ(name, type)\
  if (m_ReadAllArrays == true || m_ArrayNames.find(Ebsd::Ang::name) != m_ArrayNames.end()) {\
    type* _##name = allocateArray<type>(totalDataRows);\
    if (NULL != _##name) {\
      ::memset(_##name, 0, numBytes);\
      err = QH5Lite::readPointerDataset(gid, Ebsd::Ang::name, _##name);\
      if (err < 0) {\
        deallocateArrayData(_##name); /*deallocate the array*/\
        setErrorCode(-90020);\
        ss << "Error reading dataset '" << #name << "' from the HDF5 file. This data set is required to be in the file because either "\
           "the program is set to read ALL the Data arrays or the program was instructed to read this array.";\
        setErrorMessage(ss.string());\
        err = H5Gclose(gid);\
        return -90020;\
      }\
    }\
    set##name##Pointer(_##name);\
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readData(hid_t parId)
{
  int err = -1;

  // Delete any currently existing pointers
  deletePointers();
  // Initialize new pointers
  size_t totalDataRows = 0;

  QString grid = getGrid();

  size_t nOddCols = getNumOddCols();
  size_t nEvenCols = getNumEvenCols();
  size_t nRows = getNumRows();

  if (nRows < 1)
  {
    err = -200;
    setErrorMessage("H5AngReader Error: The number of Rows was < 1.");
    setErrorCode(err);
    return err;
  }
  else if (grid.startsWith(Ebsd::Ang::SquareGrid) == true)
  {
    // if (nCols > 0) { numElements = nRows * nCols; }
    if (nOddCols > 0)
    {
      totalDataRows = nRows * nOddCols;/* nCols = nOddCols;*/
    }
    else if (nEvenCols > 0)
    {
      totalDataRows = nRows * nEvenCols; /* nCols = nEvenCols; */
    }
    else
    {
      totalDataRows = 0;
    }
  }
  else if (grid.startsWith(Ebsd::Ang::HexGrid) == true)
  {
    setErrorCode(-90400);
    setErrorMessage("Ang Files with Hex Grids Are NOT currently supported. Please convert them to Square Grid files first");
    return -400;
  }
  else // Grid was not set
  {
    setErrorCode(-90300);
    setErrorMessage("The Grid Type was not set in the file.");
    return -300;
  }


  hid_t gid = H5Gopen(parId, Ebsd::H5::Data.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    setErrorMessage("H5AngReader Error: Could not open 'Data' Group");
    setErrorCode(-90012);
    return -90012;
  }
  setNumberOfElements(totalDataRows);
  size_t numBytes = totalDataRows * sizeof(float);
  QString sBuf;
  QTextStream ss(&sBuf);

  if(m_ArrayNames.size() == 0 && m_ReadAllArrays == false)
  {
    err = H5Gclose(gid);
    err = -90013;
    setErrorMessage("H5AngReader Error: ReadAllArrays was FALSE and no other arrays were requested to be read.");
    setErrorCode(err);
    return err;
  }

  ANG_READER_ALLOCATE_AND_READ(Phi1, float);
  ANG_READER_ALLOCATE_AND_READ(Phi, float);
  ANG_READER_ALLOCATE_AND_READ(Phi2, float);
  ANG_READER_ALLOCATE_AND_READ(ImageQuality, float);
  ANG_READER_ALLOCATE_AND_READ(ConfidenceIndex, float);

  ANG_READER_ALLOCATE_AND_READ(PhaseData, int);

  ANG_READER_ALLOCATE_AND_READ(XPosition, float);
  ANG_READER_ALLOCATE_AND_READ(YPosition, float);

  ANG_READER_ALLOCATE_AND_READ(Fit, float);
  if (err < 0)
  {
    setNumFeatures(9);
  }

  ANG_READER_ALLOCATE_AND_READ(SEMSignal, float);
  if (err < 0)
  {
    setNumFeatures(8);
  }

  err = H5Gclose(gid);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngReader::setArraysToRead(QSet<QString> names)
{
  m_ArrayNames = names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngReader::readAllArrays(bool b)
{
  m_ReadAllArrays = b;
}
