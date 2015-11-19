/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "H5OIMReader.h"

#include <vector>

#include <boost/shared_array.hpp>

#include <QtCore/QtDebug>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include "AngConstants.h"

#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdMacros.h"

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5OIMReader::H5OIMReader() :
  AngReader(),
  m_HDF5Path(),
  m_ReadPatternData(false),
  m_PatternData(NULL),
  m_ReadAllArrays(true)
{

  m_HeaderMap.clear();

  // Initialize the map of header key to header value
  m_HeaderMap[Ebsd::Ang::TEMPIXPerUM] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::TEMPIXPerUM);
  m_HeaderMap[Ebsd::Ang::XStar] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::XStar);
  m_HeaderMap[Ebsd::Ang::YStar] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::YStar);
  m_HeaderMap[Ebsd::Ang::ZStar] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::ZStar);
  m_HeaderMap[Ebsd::Ang::Working_Distance] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::Working_Distance);
  m_HeaderMap[Ebsd::Ang::GridType] = AngStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ang::GridType);
  m_HeaderMap[Ebsd::Ang::StepX] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::StepX);
  m_HeaderMap[Ebsd::Ang::StepY] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::StepY);
  //  m_HeaderMap[Ebsd::Ang::ZStep] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::ZStep); // NOT actually in the file>::NewEbsdHeaderEntry(); , but may be needed
  //  m_HeaderMap[Ebsd::Ang::ZPos] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::ZPos); // NOT actually in the file>::NewEbsdHeaderEntry(); , but may be needed
  //  m_HeaderMap[Ebsd::Ang::ZMax] = AngHeaderEntry<float>::NewEbsdHeaderEntry(Ebsd::Ang::ZMax); // NOT actually in the file>::NewEbsdHeaderEntry(); , but may be needed
  m_HeaderMap[Ebsd::Ang::nColumns] = AngHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ang::nColumns);
  m_HeaderMap[Ebsd::Ang::nRows] = AngHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ang::nRows);
  m_HeaderMap[Ebsd::Ang::Operator] = AngStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ang::Operator);
  m_HeaderMap[Ebsd::Ang::SampleID] = AngStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ang::SampleID);
  m_HeaderMap[Ebsd::Ang::ScanID] = AngStringHeaderEntry::NewEbsdHeaderEntry(Ebsd::Ang::ScanID);

  m_HeaderMap[Ebsd::Ang::PatternWidth] = AngHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ang::PatternWidth);
  m_HeaderMap[Ebsd::Ang::PatternHeight] = AngHeaderEntry<int>::NewEbsdHeaderEntry(Ebsd::Ang::PatternHeight);

  m_PatternDims[0] = 0;
  m_PatternDims[1] = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5OIMReader::~H5OIMReader()
{
  deletePointers();
  this->deallocateArrayData<uint8_t> (m_PatternData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OIMReader::readFile()
{
  int err = -1;
  if (m_HDF5Path.isEmpty() == true)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: HDF5 Path is empty.";
    setErrorCode(-1);
    setErrorMessage(str);
    return err;
  }

  hid_t fileId = QH5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: Could not open HDF5 file '" << getFileName() << "'";
    return err;
  }


  HDF5ScopedFileSentinel sentinel(&fileId, false);
  hid_t gid = H5Gopen(fileId, m_HDF5Path.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: Could not open path '" << m_HDF5Path << "'";
    err = QH5Utilities::closeFile(fileId);
    setErrorCode(-90020);
    setErrorMessage(str);
    return getErrorCode();
  }

  hid_t ebsdGid = H5Gopen(gid, Ebsd::H5::EBSD.toLatin1().data(), H5P_DEFAULT);
  if(ebsdGid < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: Could not open 'EBSD' Group";
    setErrorCode(-90007);
    setErrorMessage(str);
    return getErrorCode();
  }
  sentinel.addGroupId(&ebsdGid);

  // Read all the header information
  err = readHeader(ebsdGid);
  if(err < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: could not read header";
    err = QH5Utilities::closeFile(fileId);
    setErrorCode(-900021);
    setErrorMessage(str);
    return getErrorCode();
  }

  // Read data
  err = readData(ebsdGid);
  if(err < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: could not read data";
    err = QH5Utilities::closeFile(fileId);
    setErrorCode(-900022);
    setErrorMessage(str);
    return getErrorCode();
  }


  err = H5Gclose(ebsdGid);
  ebsdGid = -1;
  if(err < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: could not close group id ";
    err = QH5Utilities::closeFile(fileId);
    setErrorCode(-900023);
    setErrorMessage(str);
    return getErrorCode();
  }

  err = H5Gclose(gid);
  gid = -1;
  if(err < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: could not close group id ";
    err = QH5Utilities::closeFile(fileId);
    setErrorCode(-900023);
    setErrorMessage(str);
    return getErrorCode();
  }

  err = QH5Utilities::closeFile(fileId);
  fileId = -1;
  if(err < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: could not close file";
    err = QH5Utilities::closeFile(fileId);
    setErrorCode(-900024);
    setErrorMessage(str);
    return getErrorCode();
  }

  return getErrorCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OIMReader::readHeaderOnly()
{
  int err = -1;

  hid_t fileId = QH5Utilities::openFile(getFileName().toLatin1().data(), true);
  if (fileId < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: Could not open HDF5 file '" << getFileName() << "'";
    setErrorCode(-10);
    setErrorMessage(str);
    return getErrorCode();
  }
  HDF5ScopedFileSentinel sentinel(&fileId, false);

  if (m_HDF5Path.isEmpty() == true)
  {
    QStringList names;
    err = QH5Utilities::getGroupObjects(fileId, H5Utilities::H5Support_GROUP, names);

    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error (Internal HDF5 Path is empty): The name of the scan was not specified. There are " << names.count() << " scans available. ";
    int nameCount = names.count();
    if(nameCount < 10 )
    {
      ss << " The scan names are: ";
    }
    else
    {
      nameCount = 10;
      ss << " The first 10 scan names are: ";
    }
    for (int i = 0; i < nameCount; ++i)
    {
      ss << names.at(i) << "\n";
    }
    setErrorCode(-11);
    setErrorMessage(str);
    return getErrorCode();
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: Could not open path '" << m_HDF5Path << "'";
    err = QH5Utilities::closeFile(fileId);
    setErrorCode(-12);
    setErrorMessage(str);
    return getErrorCode();
  }
  sentinel.addGroupId(&gid);

  hid_t ebsdGid = H5Gopen(gid, Ebsd::H5::EBSD.toLatin1().data(), H5P_DEFAULT);
  if(ebsdGid < 0)
  {
    setErrorMessage("H5OIMReader Error: Could not open 'EBSD' Group");
    setErrorCode(-90007);
    return getErrorCode();
  }
  sentinel.addGroupId(&ebsdGid);

  err = readHeader(ebsdGid);

  err = H5Gclose(ebsdGid);
  ebsdGid = -1;
  if(err < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: could not close group id ";
    err = QH5Utilities::closeFile(fileId);
    setErrorMessage(str);
    setErrorCode(-900023);
    return getErrorCode();
  }

  err = H5Gclose(gid);
  gid = -1;
  if(err < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: could not close group id ";
    err = QH5Utilities::closeFile(fileId);
    setErrorCode(-900023);
    setErrorMessage(str);
    return getErrorCode();
  }

  err = QH5Utilities::closeFile(fileId);
  fileId = -1;
  if(err < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: could not close file";
    err = QH5Utilities::closeFile(fileId);
    setErrorCode(-900024);
    setErrorMessage(str);
    return getErrorCode();
  }

  return getErrorCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OIMReader::readScanNames(QStringList& names)
{
  int err = -1;
  hid_t fileId = QH5Utilities::openFile(getFileName().toLatin1().data(), true);
  if (fileId < 0)
  {
    QString str;
    QTextStream ss(&str);
    ss << "H5OIMReader Error: Could not open HDF5 file '" << getFileName() << "'";
    setErrorCode(-20);
    setErrorMessage(str);
    names.clear();
    return getErrorCode();
  }
  HDF5ScopedFileSentinel sentinel(&fileId, false);

  err = QH5Utilities::getGroupObjects(fileId, H5Utilities::H5Support_GROUP, names);
  setErrorCode(err);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename ClassType, typename T, typename HeaderEntryClass>
int ReadEbsdHeaderData(ClassType* c, const QString& key, hid_t gid, const QMap<QString, EbsdHeaderEntry::Pointer>& headerMap)
{

  T t;
  herr_t err = QH5Lite::readScalarDataset(gid, key, t);
  if (err < 0)
  {
    QString ss = QObject::tr("%1: The header value for '%2' was not found in the EDAX Hdf5 file. Was this header originally found in the files that were imported into this H5EBSD File?")
                 .arg(c->getNameOfClass()).arg(key);
    c->setErrorCode(-90001);
    c->setErrorMessage(ss);
    return -90001;
  }
  else
  {
    EbsdHeaderEntry::Pointer p = headerMap[key];
    typename HeaderEntryClass::Pointer c = std::dynamic_pointer_cast<HeaderEntryClass>(p);
    c->setValue(t);
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename ClassType, typename T, typename HeaderEntryClass>
int ReadEbsdHeaderStringData(ClassType* c, const QString& key, hid_t gid, const QMap<QString, EbsdHeaderEntry::Pointer>& headerMap)
{

  T t;
  herr_t err = QH5Lite::readStringDataset(gid, key, t);
  if (err < 0)
  {
    QString ss = QObject::tr("%1: The header value for '%2' was not found in the EDAX Hdf5 file. Was this header originally found in the files that were imported into this H5EBSD File?")
                 .arg(c->getNameOfClass()).arg(key);
    c->setErrorCode(-90001);
    c->setErrorMessage(ss);
    return -90001;
  }
  else
  {
    EbsdHeaderEntry::Pointer p = headerMap[key];
    typename HeaderEntryClass::Pointer c = std::dynamic_pointer_cast<HeaderEntryClass>(p);
    c->setValue(t);
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OIMReader::readHeader(hid_t parId)
{
  typedef AngHeaderEntry<float> AngHeaderFloatType;
  typedef AngHeaderEntry<int> AngHeaderIntType;
  int err = -1;

  hid_t gid = H5Gopen(parId, Ebsd::H5::Header.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    setErrorCode(-90008);
    setErrorMessage("H5OIMReader Error: Could not open 'Header' Group");
    return -1;
  }
  HDF5ScopedGroupSentinel sentinel(&gid, false);

  //READ_EBSD_HEADER_DATA("H5OIMReader", AngHeaderEntry<float>, flboost::dynamicEMPIXPerUM, Ebsd::Ang::TEMPIXPerUM)
  QString path = Ebsd::H5::PatternCenterCalibration + "/" + Ebsd::Ang::XStar;
  hid_t patternCenterCalibrationGid = H5Gopen(gid, Ebsd::H5::PatternCenterCalibration.toLatin1().data(), H5P_DEFAULT);
  if(patternCenterCalibrationGid < 0)
  {
    setErrorCode(-90008);
    setErrorMessage("H5OIMReader Error: Could not open 'Pattern Center Calibration' Group");
    return -1;
  }
  sentinel.addGroupId(&patternCenterCalibrationGid);
  ReadEbsdHeaderData<H5OIMReader, float, AngHeaderFloatType>(this, Ebsd::Ang::XStar, patternCenterCalibrationGid, m_HeaderMap);
  ReadEbsdHeaderData<H5OIMReader, float, AngHeaderFloatType>(this, Ebsd::Ang::YStar, patternCenterCalibrationGid, m_HeaderMap);
  ReadEbsdHeaderData<H5OIMReader, float, AngHeaderFloatType>(this, Ebsd::Ang::ZStar, patternCenterCalibrationGid, m_HeaderMap);

  ReadEbsdHeaderData<H5OIMReader, float, AngHeaderFloatType>(this, Ebsd::Ang::Working_Distance, gid, m_HeaderMap);
  ReadEbsdHeaderData<H5OIMReader, float, AngHeaderFloatType>(this, Ebsd::Ang::StepX, gid, m_HeaderMap);
  ReadEbsdHeaderData<H5OIMReader, float, AngHeaderFloatType>(this, Ebsd::Ang::StepY, gid, m_HeaderMap);
  ReadEbsdHeaderData<H5OIMReader, int, AngHeaderIntType>(this, Ebsd::Ang::nColumns, gid, m_HeaderMap);
  ReadEbsdHeaderData<H5OIMReader, int, AngHeaderIntType>(this, Ebsd::Ang::nRows, gid, m_HeaderMap);

  HDF_ERROR_HANDLER_OFF
  int value = 0;
  if (QH5Lite::datasetExists(gid, Ebsd::Ang::PatternWidth))
  {
    // Read the Pattern Width - This may not exist
    err = QH5Lite::readScalarDataset(gid, Ebsd::Ang::PatternWidth, value);
    EbsdHeaderEntry::Pointer p = m_HeaderMap[Ebsd::Ang::PatternWidth];
    AngHeaderIntType::Pointer c = std::dynamic_pointer_cast<AngHeaderIntType>(p);
    c->setValue(value);
    m_PatternDims[1] = value;
  }

  // Read the Pattern Height - This may not exist
  value = 0;
  if (QH5Lite::datasetExists(gid, Ebsd::Ang::PatternHeight))
  {
    err = QH5Lite::readScalarDataset(gid, Ebsd::Ang::PatternHeight, value);
    EbsdHeaderEntry::Pointer p = m_HeaderMap[Ebsd::Ang::PatternHeight];
    AngHeaderIntType::Pointer c = std::dynamic_pointer_cast<AngHeaderIntType>(p);
    c->setValue(value);
    m_PatternDims[0] = value;
  }
  HDF_ERROR_HANDLER_ON

  ReadEbsdHeaderStringData<H5OIMReader, QString, AngStringHeaderEntry>(this, Ebsd::Ang::Operator, gid, m_HeaderMap);
  ReadEbsdHeaderStringData<H5OIMReader, QString, AngStringHeaderEntry>(this, Ebsd::Ang::SampleID, gid, m_HeaderMap);
  ReadEbsdHeaderStringData<H5OIMReader, QString, AngStringHeaderEntry>(this, Ebsd::Ang::ScanID, gid, m_HeaderMap);
  ReadEbsdHeaderStringData<H5OIMReader, QString, AngStringHeaderEntry>(this, Ebsd::Ang::GridType, gid, m_HeaderMap);

  hid_t phasesGid = H5Gopen(gid, Ebsd::H5::Phase.toLatin1().data(), H5P_DEFAULT);
  if (phasesGid < 0)
  {
    setErrorCode(-90007);
    setErrorMessage("H5OIMReader Error: Could not open Header/Phase HDF Group.");
    H5Gclose(gid);
    return getErrorCode();
  }
  sentinel.addGroupId(&phasesGid);

  QStringList names;
  err = QH5Utilities::getGroupObjects(phasesGid, H5Utilities::H5Support_GROUP, names);
  if (err < 0 || names.size() == 0)
  {
    setErrorCode(-90009);
    setErrorMessage("H5OIMReader Error: There were no Phase groups present in the HDF5 file");
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return getErrorCode();
  }
  //m_Phases.clear();
  QVector<AngPhase::Pointer> phaseVector;

  foreach(QString phaseGroupName, names)
  {
    hid_t pid = H5Gopen(phasesGid, phaseGroupName.toLatin1().data(), H5P_DEFAULT);
    AngPhase::Pointer m_CurrentPhase = AngPhase::New();
    m_CurrentPhase->setPhaseIndex(phaseGroupName.toInt());
    READ_PHASE_STRING_DATA("H5OIMReader", pid, Ebsd::Ang::MaterialName, MaterialName, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5OIMReader", pid, Ebsd::Ang::Formula, Formula, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5OIMReader", pid, Ebsd::Ang::Info, Info, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5OIMReader", pid, int32_t, Ebsd::Ang::Symmetry, Symmetry, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5OIMReader", pid, int32_t, Ebsd::Ang::NumberFamilies, NumberFamilies, m_CurrentPhase)

    QVector<float> fillerValues(6, 0.0);
    m_CurrentPhase->setLatticeConstants(fillerValues);
    READ_PHASE_HEADER_DATA("H5OIMReader", pid, float, Ebsd::Ang::LatticeConstantA, LatticeConstantA, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5OIMReader", pid, float, Ebsd::Ang::LatticeConstantB, LatticeConstantB, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5OIMReader", pid, float, Ebsd::Ang::LatticeConstantC, LatticeConstantC, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5OIMReader", pid, float, Ebsd::Ang::LatticeConstantAlpha, LatticeConstantAlpha, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5OIMReader", pid, float, Ebsd::Ang::LatticeConstantBeta, LatticeConstantBeta, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5OIMReader", pid, float, Ebsd::Ang::LatticeConstantGamma, LatticeConstantGamma, m_CurrentPhase)


    if (m_CurrentPhase->getNumberFamilies() > 0)
    {
      // hid_t hklGid = H5Gopen(pid, Ebsd::Ang::HKLFamilies.toLatin1().data(), H5P_DEFAULT);
      // Only read the HKL Families if they are there. Trying to open the group will tell us if there
      // are any families to read

      err = readHKLFamilies(pid, m_CurrentPhase);
      if (getErrorCode() < 0) { err = H5Gclose(pid); return -1; }
    }
    /* The 'Categories' header may actually be missing from certain types of .ang files */
    if (QH5Lite::datasetExists(pid, Ebsd::Ang::Categories) == true)
    {
      READ_PHASE_HEADER_ARRAY("H5OIMReader", pid, int, Ebsd::Ang::Categories, Categories, m_CurrentPhase)
    }
    phaseVector.push_back(m_CurrentPhase);
    err = H5Gclose(pid);
  }

  setPhaseVector(phaseVector);

  QString completeHeader;
  setOriginalHeader(completeHeader);

  err = H5Gclose(phasesGid);
  phasesGid = -1;
  err = H5Gclose(gid);
  gid = -1;

  return getErrorCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OIMReader::readHKLFamilies(hid_t hklGid, AngPhase::Pointer phase)
{

  herr_t status = 1;
  //HKLFamily_t data;
  boost::shared_array<HKLFamily_t> data;
  QVector<HKLFamily::Pointer> families;

  // setup compound memory type
  hid_t memtype = H5Tcreate(H5T_COMPOUND, sizeof(HKLFamily_t));
  H5Tinsert(memtype, "H", HOFFSET (HKLFamily_t, h), H5T_NATIVE_INT);
  H5Tinsert(memtype, "K", HOFFSET (HKLFamily_t, k), H5T_NATIVE_INT);
  H5Tinsert(memtype, "L", HOFFSET (HKLFamily_t, l), H5T_NATIVE_INT);
  H5Tinsert(memtype, "Diffraction Intensity", HOFFSET (HKLFamily_t, diffractionIntensity), H5T_NATIVE_FLOAT);
  H5Tinsert(memtype, "Use in Indexing", HOFFSET (HKLFamily_t, s1), H5T_NATIVE_CHAR);
  H5Tinsert(memtype, "Show bands", HOFFSET (HKLFamily_t, s2), H5T_NATIVE_CHAR);

  // Create dataspace & dataset
  hid_t dataset = H5Dopen(hklGid, Ebsd::Ang::HKL_Families.toLatin1().data(), H5P_DEFAULT);
  hid_t dataspace = H5Dget_space(dataset);
  int rank = H5Sget_simple_extent_ndims(dataspace);
  if(rank == 1)
  {
    hsize_t dimsFam[1];
    int nDims = H5Sget_simple_extent_dims(dataspace, dimsFam, NULL);
    if(nDims > 0)
    {
      data = boost::shared_array<HKLFamily_t>(new HKLFamily_t[dimsFam[0]]); // (HKLFamily_t *) calloc(dimsFam[0], sizeof(HKLFamily_t));
      herr_t status = H5Dread (dataset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void*)(data.get()) );
      if (status < 0)
      {
        setErrorCode(-90011);
        QString ss = QObject::tr("H5OIMReader Error: Could not read the HKLFamily data");
        setErrorMessage(ss);
        return getErrorCode();
      }
    }
  }


  for (int i = 0; i < phase->getNumberFamilies(); ++i)
  {
    HKLFamily::Pointer f = HKLFamily::New();
    HKLFamily_t* ptr = data.get() + i;
    f->copyFromStruct(ptr);
    families.push_back(f);
  }

  //// close resources
  H5Tclose (memtype);
  H5Sclose (dataspace);
  H5Dclose (dataset);

  phase->setHKLFamilies(families);
  return status;
}


#define ANG_READER_ALLOCATE_AND_READ(name, h5name, type)\
  if (m_ReadAllArrays == true || m_ArrayNames.find(Ebsd::Ang::name) != m_ArrayNames.end()) {\
    type* _##name = allocateArray<type>(totalDataRows);\
    if (NULL != _##name) {\
      ::memset(_##name, 0, numBytes);\
      err = QH5Lite::readPointerDataset(gid, h5name, _##name);\
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
int H5OIMReader::readData(hid_t parId)
{
  int err = -1;

  // Delete any currently existing pointers
  deletePointers();
  // Initialize new pointers
  size_t totalDataRows = 0;

  QString grid = getGrid();

  size_t nColumns = getNumColumns();
  size_t nRows = getNumRows();

  if (nRows < 1)
  {
    err = -200;
    setErrorMessage("H5OIMReader Error: The number of Rows was < 1.");
    setErrorCode(err);
    return err;
  }
  else if (grid.startsWith(Ebsd::Ang::SquareGrid) == true)
  {
    // if (nCols > 0) { numElements = nRows * nCols; }
    if (nColumns > 0)
    {
      totalDataRows = nRows * nColumns;/* nCols = nOddCols;*/
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
  
  if(totalDataRows == 0)
  {
    setErrorCode(-90301);
    setErrorMessage("There is no data to read. NumRows or NumColumns is Zero (0)");
    return -301;
  }

  hid_t gid = H5Gopen(parId, Ebsd::H5::Data.toLatin1().data(), H5P_DEFAULT);
  if (gid < 0)
  {
    setErrorMessage("H5OIMReader Error: Could not open 'Data' Group");
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
    setErrorMessage("H5OIMReader Error: ReadAllArrays was FALSE and no other arrays were requested to be read.");
    setErrorCode(err);
    return err;
  }

  ANG_READER_ALLOCATE_AND_READ(Phi1, Ebsd::Ang::Phi1, float);
  ANG_READER_ALLOCATE_AND_READ(Phi, Ebsd::Ang::Phi, float);
  ANG_READER_ALLOCATE_AND_READ(Phi2, Ebsd::Ang::Phi2, float);
  ANG_READER_ALLOCATE_AND_READ(ImageQuality, Ebsd::Ang::IQ, float);
  ANG_READER_ALLOCATE_AND_READ(ConfidenceIndex, Ebsd::Ang::CI, float);

  ANG_READER_ALLOCATE_AND_READ(PhaseData, Ebsd::Ang::Phase, int);

  ANG_READER_ALLOCATE_AND_READ(XPosition, Ebsd::Ang::XPosition, float);
  ANG_READER_ALLOCATE_AND_READ(YPosition, Ebsd::Ang::YPosition, float);

  ANG_READER_ALLOCATE_AND_READ(Fit, Ebsd::Ang::Fit, float);

  if (err < 0)
  {
    setNumFeatures(9);
  }

  ANG_READER_ALLOCATE_AND_READ(SEMSignal, Ebsd::Ang::SEMSignal, float);
  if (err < 0)
  {
    setNumFeatures(8);
  }

  if(m_ReadPatternData)
  {
    H5T_class_t type_class;
    QVector<hsize_t> dims;
    size_t type_size = 0;
    err = QH5Lite::getDatasetInfo(gid, Ebsd::Ang::PatternData, dims, type_class, type_size);
    if (err >= 0) // Only read the pattern data if the pattern data is available.
    {
      totalDataRows = 1; // Calculate the total number of elements to allocate for the pattern data
      for(qint32 i = 0; i < dims.size(); i++)
      {
        totalDataRows = totalDataRows * dims[i];
      }
      // Set the pattern dimensions
      m_PatternDims[0] = dims[1];
      m_PatternDims[1] = dims[2];

      m_PatternData = this->allocateArray<uint8_t>(totalDataRows);
      err = QH5Lite::readPointerDataset(gid, Ebsd::Ang::PatternData, m_PatternData);
    }
  }
  err = H5Gclose(gid);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5OIMReader::setArraysToRead(QSet<QString> names)
{
  m_ArrayNames = names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5OIMReader::readAllArrays(bool b)
{
  m_ReadAllArrays = b;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OIMReader::getXDimension()
{
  return getNumColumns();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5OIMReader::setXDimension(int xdim)
{
  setNumColumns(xdim);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5OIMReader::getYDimension()
{
  return getNumRows();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5OIMReader::setYDimension(int ydim)
{
  setNumRows(ydim);
}
