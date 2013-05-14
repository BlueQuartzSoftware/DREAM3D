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

#include "H5AngReader.h"

#include "AngConstants.h"
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
  if (m_HDF5Path.empty() == true)
  {
    std::cout << "H5AngReader Error: HDF5 Path is empty." << std::endl;
    return -1;
  }

  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    std::cout << "H5AngReader Error: Could not open HDF5 file '" << getFileName() << "'" << std::endl;
    return -1;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5AngReader Error: Could not open path '" << m_HDF5Path << "'" << std::endl;
    err = H5Utilities::closeFile(fileId);
    return -1;
  }

  // Read all the header information
  err = readHeader(gid);

  // Read and transform data
  err = readData(gid);

  err = H5Gclose(gid);
  err = H5Utilities::closeFile(fileId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readHeaderOnly()
{
  int err = -1;
  if (m_HDF5Path.empty() == true)
  {
    std::cout << "H5AngReader Error: HDF5 Path is empty." << std::endl;
    return -1;
  }

  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    std::cout << "H5AngReader Error: Could not open HDF5 file '" << getFileName() << "'" << std::endl;
    return -1;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5AngReader Error: Could not open path '" << m_HDF5Path << "'" << std::endl;
    err = H5Utilities::closeFile(fileId);
    return -1;
  }

  // Read all the header information
  // std::cout << "H5AngReader:: reading Header .. " << std::endl;
  err = readHeader(gid);
  err = H5Utilities::closeFile(fileId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readHeader(hid_t parId)
{
  int err = -1;
  hid_t gid = H5Gopen(parId, Ebsd::H5::Header.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    setErrorCode(-90008);
    setErrorMessage("H5AngReader Error: Could not open 'Header' Group");
    return -1;
  }


  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, TEMPIXPerUM, Ebsd::Ang::TEMPIXPerUM)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, XStar, Ebsd::Ang::XStar)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, YStar, Ebsd::Ang::YStar)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, ZStar, Ebsd::Ang::ZStar)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, WorkingDistance, Ebsd::Ang::WorkingDistance)
  READ_EBSD_HEADER_STRING_DATA("H5AngReader", AngStringHeaderEntry, std::string, Grid, Ebsd::Ang::Grid)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, XStep, Ebsd::Ang::XStep)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<float>, float, YStep, Ebsd::Ang::YStep)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<int>, int, NumOddCols, Ebsd::Ang::NColsOdd)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<int>, int, NumEvenCols, Ebsd::Ang::NColsEven)
  READ_EBSD_HEADER_DATA("H5AngReader", AngHeaderEntry<int>, int, NumRows, Ebsd::Ang::NRows)
  READ_EBSD_HEADER_STRING_DATA("H5AngReader", AngStringHeaderEntry, std::string, OIMOperator, Ebsd::Ang::Operator)
  READ_EBSD_HEADER_STRING_DATA("H5AngReader", AngStringHeaderEntry, std::string, SampleID, Ebsd::Ang::SampleId)
  READ_EBSD_HEADER_STRING_DATA("H5AngReader", AngStringHeaderEntry, std::string, ScanID, Ebsd::Ang::ScanId)

  hid_t phasesGid = H5Gopen(gid, Ebsd::H5::Phases.c_str(), H5P_DEFAULT);
  if (phasesGid < 0)
  {
    setErrorCode(-90007);
    setErrorMessage("H5AngReader Error: Could not open Header/Phases HDF Group.");
    H5Gclose(gid);
    return -1;
  }

  std::list<std::string> names;
  err = H5Utilities::getGroupObjects(phasesGid, H5Utilities::H5Support_GROUP, names);
  if (err < 0 || names.size() == 0)
  {
    setErrorCode(-90009);
    setErrorMessage("H5AngReader Error: There were no Phase groups present in the HDF5 file");
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return -1;
  }
  m_Phases.clear();
  for (std::list<std::string>::iterator phaseGroupName = names.begin(); phaseGroupName != names.end(); ++phaseGroupName )
  {
    hid_t pid = H5Gopen(phasesGid, (*phaseGroupName).c_str(), H5P_DEFAULT);
    AngPhase::Pointer m_CurrentPhase = AngPhase::New();
    READ_PHASE_HEADER_DATA("H5AngReader", pid, int, Ebsd::Ang::Phase, PhaseIndex, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5AngReader", pid, Ebsd::Ang::MaterialName, MaterialName, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5AngReader", pid, Ebsd::Ang::Formula, Formula, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5AngReader", pid, Ebsd::Ang::Info, Info, m_CurrentPhase)
    READ_PHASE_HEADER_DATA_CAST("H5AngReader", pid, uint32_t, int, Ebsd::Ang::Symmetry, Symmetry, m_CurrentPhase)
    READ_PHASE_HEADER_ARRAY("H5AngReader", pid, std::vector<float>, Ebsd::Ang::LatticeConstants, LatticeConstants, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5AngReader", pid, int, Ebsd::Ang::NumberFamilies, NumberFamilies, m_CurrentPhase)

    if (m_CurrentPhase->getNumberFamilies() > 0)
    {
      hid_t hklGid = H5Gopen(pid, Ebsd::Ang::HKLFamilies.c_str(), H5P_DEFAULT);
      // Only read the HKL Families if they are there. Trying to open the group will tell us if there
      // are any families to read

      err = readHKLFamilies(hklGid, m_CurrentPhase);
      err = H5Gclose(hklGid);
      if (getErrorCode() < 0) { err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid); return -1; }
    }
    /* The 'Categories' header may actually be missing from certain types of .ang files */
    if (H5Lite::datasetExists(pid, Ebsd::Ang::Categories) == true)
    {
      READ_PHASE_HEADER_ARRAY("H5AngReader", pid, std::vector<int>, Ebsd::Ang::Categories, Categories, m_CurrentPhase)
    }
    m_Phases.push_back(m_CurrentPhase);
    err = H5Gclose(pid);
  }

  std::string completeHeader;
  err = H5Lite::readStringDataset(gid, Ebsd::H5::OriginalHeader, completeHeader);
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
  std::stringstream ss;
  hid_t dataset, memtype;
  herr_t status = 1;
  HKLFamily_t data;
  std::vector<HKLFamily::Pointer> families;
  for (int i = 0; i < phase->getNumberFamilies(); ++i)
  {
    std::string dsetName = StringUtils::numToString(i);

    dataset = H5Dopen(hklGid, dsetName.c_str(), H5P_DEFAULT);

    memtype = H5Tcreate (H5T_COMPOUND, sizeof (HKLFamily_t));
    status = H5Tinsert(memtype, "H", HOFFSET (HKLFamily_t, h), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "K", HOFFSET (HKLFamily_t, k), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "L", HOFFSET (HKLFamily_t, l), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "Solution 1", HOFFSET (HKLFamily_t, s1), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "Diffraction Intensity", HOFFSET (HKLFamily_t, diffractionIntensity), H5T_NATIVE_FLOAT);
    status = H5Tinsert(memtype, "Solution 2", HOFFSET (HKLFamily_t, s2), H5T_NATIVE_INT);

    status = H5Dread(dataset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void*)(&data));
    if (status < 0)
    {
      setErrorCode(-90011);
      ss.str("");
      ss << "H5AngReader Error: Could not read the HKLFamily data for family number " << i;
      setErrorMessage(ss.str());
      break;
    }
    status = H5Dclose(dataset); // Close the data set

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
    err = H5Lite::readPointerDataset(gid, Ebsd::Ang::name, _##name);\
    if (err < 0) {\
      setErrorCode(-90020);\
      ss << "Error reading dataset '" << #name << "' from the HDF5 file. This data set is required to be in the file because either "\
      "the program is set to read ALL the Data arrays or the program was instructed to read this array.";\
      setErrorMessage(ss.str());\
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

  std::string grid = getGrid();

  size_t nOddCols = getNumOddCols();
  size_t nEvenCols = getNumEvenCols();
  size_t nRows = getNumRows();

  if (nRows < 1)
  {
    return -200;
  }
  else if (grid.find(Ebsd::Ang::SquareGrid) == 0)
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
  else if (grid.find(Ebsd::Ang::HexGrid) == 0)
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


  hid_t gid = H5Gopen(parId, Ebsd::H5::Data.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    setErrorMessage("H5AngReader Error: Could not open 'Data' Group");
    setErrorCode(-90012);
    return -90012;
  }

  setNumberOfElements(totalDataRows);
  size_t numBytes = totalDataRows * sizeof(float);
  std::stringstream ss;

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
    setNumFields(9);
  }

  ANG_READER_ALLOCATE_AND_READ(SEMSignal, float);
  if (err < 0)
  {
    setNumFields(8);
  }

  err = H5Gclose(gid);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngReader::setArraysToRead(std::set<std::string> names)
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
