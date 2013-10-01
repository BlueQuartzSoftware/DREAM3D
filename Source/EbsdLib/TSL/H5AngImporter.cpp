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




#include "H5AngImporter.h"

#include <sstream>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdLibVersion.h"
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
H5AngImporter::H5AngImporter() :
EbsdImporter(),
xDim(0),
yDim(0),
xRes(0),
yRes(0),
m_FileVersion(Ebsd::H5::FileVersion)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngImporter::~H5AngImporter()
{
}


#define WRITE_ANG_HEADER_DATA(reader, m_msgType, prpty, key)\
{\
  m_msgType t = reader.get##prpty();\
  err = H5Lite::writeScalarDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(gid); err = H5Gclose(angGroup);\
    return -1; }\
}

#define WRITE_ANG_HEADER_STRING_DATA(reader, m_msgType, prpty, key)\
{\
  m_msgType t = reader.get##prpty();\
  err = H5Lite::writeStringDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(gid); err = H5Gclose(angGroup);\
    return -1; }\
}

#define WRITE_ANG_DATA_ARRAY(reader, m_msgType, gid, prpty, key)\
{\
  m_msgType* dataPtr = reader.get##prpty##Pointer();\
  if (NULL != dataPtr) {\
    err = H5Lite::writePointerDataset(gid, key, rank, dims, dataPtr);\
    if (err < 0) {\
      std::ostringstream ss;\
      ss << "H5AngImporter Error: Could not write Ang Data array for '" << key\
      <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
      progressMessage(ss.str(), 100);\
      err = H5Gclose(gid); err = H5Gclose(angGroup);\
      return -1; }\
}\
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngImporter::getDims(int64_t &x, int64_t &y)
{
  x = xDim;
  y = yDim;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngImporter::getResolution(float &x, float &y)
{
  x = xRes;
  y = yRes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngImporter::numberOfSlicesImported()
{
  return 1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngImporter::importFile(hid_t fileId, int64_t z, const std::string &angFile)
{
  herr_t err = -1;
  setCancel(false);
  setErrorCondition(false);
  setPipelineMessage("");

//  std::cout << "H5AngImporter: Importing " << angFile << std::endl;
  AngReader reader;
  reader.setFileName(angFile);

  // Now actually read the file
  err = reader.readFile();

  // Check for errors
  if (err < 0)
  {
    std::ostringstream ss;
    if (err == -400) {
      ss << "H5AngImporter Error: HexGrid Files are not currently supported.";
    }
    else if (err == -300)
    {
      ss << "H5AngImporter Error: Grid was NOT set in the header.";
    }
    else if (err == -200)
    {
      ss << "H5AngImporter Error: There was no data in the file.";
    }
    else if (err == -100)
    {
      ss << "H5AngImporter Error: The Ang file could not be opened.";
    }
    else if (reader.getXStep() == 0.0f)
    {
      ss << "H5AngImporter Error: X Step value equals 0.0. This is bad. Please check the validity of the ANG file.";
    }
    else if(reader.getYStep() == 0.0f)
    {
      ss << "H5AngImporter Error: Y Step value equals 0.0. This is bad. Please check the validity of the ANG file.";
    }
    else
    {
      ss << "H5AngImporter Error: Unknown error.";
    }
    setPipelineMessage(ss.str());

    setErrorCondition(err);
    progressMessage(ss.str(), 100);
    return -1;
  }

  // Write the file Version number to the file
  {
    std::vector<hsize_t> dims;
    H5T_class_t type_class;
    size_t type_size = 0;
    hid_t attr_type = -1;
    err = H5Lite::getAttributeInfo(fileId, "/", Ebsd::H5::FileVersionStr, dims, type_class, type_size, attr_type);
    if (attr_type < 0) // The attr_type variable was never set which means the attribute was NOT there
    {
      // The file version does not exist so write it to the file
      err = H5Lite::writeScalarAttribute(fileId, "/", Ebsd::H5::FileVersionStr, m_FileVersion);
    }
    else
    {
      H5Aclose(attr_type);
    }

    err = H5Lite::getAttributeInfo(fileId, "/", Ebsd::H5::EbsdLibVersionStr, dims, type_class, type_size, attr_type);
    if (attr_type < 0) // The attr_type variable was never set which means the attribute was NOT there
    {
      // The file version does not exist so write it to the file
      err = H5Lite::writeStringAttribute(fileId, "/", Ebsd::H5::EbsdLibVersionStr, EbsdLib::Version::Complete());
    }
    else
    {
      H5Aclose(attr_type);
    }


  }

  // Start creating the HDF5 group structures for this file
  hid_t angGroup = H5Utilities::createGroup(fileId, StringUtils::numToString(z));
  if (angGroup < 0)
  {
    std::ostringstream ss;
    ss << "H5AngImporter Error: A Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons.";
    setPipelineMessage(ss.str());
    setErrorCondition(-500);
    return -1;
  }

  hid_t gid = H5Utilities::createGroup(angGroup, Ebsd::H5::Header);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5AngImporter Error: The 'Header' Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons.";
    progressMessage(ss.str(), 100);
    err = H5Gclose(angGroup);
    setPipelineMessage(ss.str());
    setErrorCondition(-600);
    return -1;
  }
  WRITE_ANG_HEADER_DATA(reader, float, TEMpixPerum, Ebsd::Ang::TEMPIXPerUM)
  WRITE_ANG_HEADER_DATA(reader, float, XStar, Ebsd::Ang::XStar)
  WRITE_ANG_HEADER_DATA(reader, float, YStar, Ebsd::Ang::YStar)
  WRITE_ANG_HEADER_DATA(reader, float, ZStar, Ebsd::Ang::ZStar)
  WRITE_ANG_HEADER_DATA(reader, float, WorkingDistance, Ebsd::Ang::WorkingDistance)

  hid_t phasesGid = H5Utilities::createGroup(gid, Ebsd::H5::Phases);
  err = writePhaseData(reader, phasesGid);
  // Close this group
  err = H5Gclose(phasesGid);

  WRITE_ANG_HEADER_STRING_DATA(reader, std::string, Grid, Ebsd::Ang::Grid)
  WRITE_ANG_HEADER_DATA(reader, float, XStep, Ebsd::Ang::XStep)
  xRes = reader.getXStep();
  WRITE_ANG_HEADER_DATA(reader, float, YStep, Ebsd::Ang::YStep)
  yRes = reader.getYStep();
  WRITE_ANG_HEADER_DATA(reader, int, NumOddCols, Ebsd::Ang::NColsOdd)
  WRITE_ANG_HEADER_DATA(reader, int, NumEvenCols, Ebsd::Ang::NColsEven)
  xDim = reader.getNumEvenCols();
  WRITE_ANG_HEADER_DATA(reader, int, NumRows, Ebsd::Ang::NRows)
  yDim = reader.getNumRows();
  WRITE_ANG_HEADER_STRING_DATA(reader, std::string, OIMOperator, Ebsd::Ang::Operator)
  WRITE_ANG_HEADER_STRING_DATA(reader, std::string, SampleID, Ebsd::Ang::SampleId)
  WRITE_ANG_HEADER_STRING_DATA(reader, std::string, ScanID, Ebsd::Ang::ScanId)

  std::string angCompleteHeader = reader.getOriginalHeader();
  err = H5Lite::writeStringDataset(gid, Ebsd::H5::OriginalHeader, angCompleteHeader);
  err = H5Lite::writeStringDataset(gid, Ebsd::H5::OriginalFile, angFile);

  // Close the "Header" group
  err = H5Gclose(gid);

  // Create the "Data" group
  gid = H5Utilities::createGroup(angGroup, Ebsd::H5::Data);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5AngImporter Error: The 'Data' Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons."<< std::endl;
    progressMessage(ss.str(), 100);
    err = H5Gclose(angGroup);
    setPipelineMessage(ss.str());
    setErrorCondition(-700);
    return -1;
  }

  int32_t rank = 1;
  hsize_t dims[1] = { reader.getNumEvenCols() * reader.getNumRows() };

  WRITE_ANG_DATA_ARRAY(reader, float, gid, Phi1, Ebsd::Ang::Phi1);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, Phi, Ebsd::Ang::Phi);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, Phi2, Ebsd::Ang::Phi2);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, XPosition, Ebsd::Ang::XPosition);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, YPosition, Ebsd::Ang::YPosition);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, ImageQuality, Ebsd::Ang::ImageQuality);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, ConfidenceIndex, Ebsd::Ang::ConfidenceIndex);
  WRITE_ANG_DATA_ARRAY(reader, int, gid, PhaseData, Ebsd::Ang::PhaseData);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, SEMSignal, Ebsd::Ang::SEMSignal);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, Fit, Ebsd::Ang::Fit);
  // Close the "Data" group
  err = H5Gclose(gid);

  // Close the group for this file
  err = H5Gclose(angGroup);

  return err;
}


#define WRITE_PHASE_HEADER_DATA(reader, m_msgType, prpty, key)\
{\
  m_msgType t = reader->get##prpty();\
  err = H5Lite::writeScalarDataset(pid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
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
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
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
      ss << "H5AngImporter Error: Could not write Ang Data array for '" << key\
      <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
      progressMessage(ss.str(), 100);\
      err = H5Gclose(pid); \
      return -1; }\
}\
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngImporter::writePhaseData(AngReader &reader, hid_t phasesGid)
{
  int err = 0;
 // int retErr = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 0 };
  std::vector<AngPhase::Pointer> phases = reader.getPhaseVector();
  for (std::vector<AngPhase::Pointer>::iterator phase = phases.begin(); phase != phases.end(); ++phase )
  {
    AngPhase* p = (*phase).get();
    hid_t pid = H5Utilities::createGroup(phasesGid, StringUtils::numToString(p->getPhaseIndex()));
    WRITE_PHASE_HEADER_DATA((*phase), int, PhaseIndex, Ebsd::Ang::Phase)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, MaterialName, Ebsd::Ang::MaterialName)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, Formula, Ebsd::Ang::Formula)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, Info, Ebsd::Ang::Info)
    WRITE_PHASE_HEADER_DATA((*phase), int, Symmetry, Ebsd::Ang::Symmetry)
    WRITE_PHASE_DATA_ARRAY( (*phase), float, pid, LatticeConstants, Ebsd::Ang::LatticeConstants)
    WRITE_PHASE_HEADER_DATA((*phase), int, NumberFamilies, Ebsd::Ang::NumberFamilies)

    // Create a Group for the HKLFamilies
    if (p->getNumberFamilies() > 0) {
      hid_t hklGid = H5Utilities::createGroup(pid, Ebsd::Ang::HKLFamilies);
      err = writeHKLFamilies(p, hklGid);
      if (err < 0) {
        std::ostringstream ss;
        ss << "H5AngImporter Error: Could not write Ang HKL Families to the HDF5 file with data set name '"
          << Ebsd::Ang::HKLFamilies << "'" << std::endl;
        progressMessage(ss.str(), 100);
        err = H5Gclose(hklGid);
        return -1;
      }
      err = H5Gclose(hklGid);
    }
    WRITE_PHASE_DATA_ARRAY( (*phase), int, pid, Categories, Ebsd::Ang::Categories)
    err = H5Gclose(pid);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngImporter::writeHKLFamilies(AngPhase* p, hid_t hklGid)
{
 // int err = 0;
  hid_t       memtype, space, dset;
  hsize_t     dims[1] = {1};
  herr_t      status = -1;
  int index = 0;
  std::vector<HKLFamily::Pointer> families = p->getHKLFamilies();
  HKLFamily_t hkl;
  for (std::vector<HKLFamily::Pointer>::iterator f = families.begin(); f != families.end(); ++f )
  {
    (*f)->copyToStruct(&hkl);

    memtype = H5Tcreate (H5T_COMPOUND, sizeof (HKLFamily_t));
    status = H5Tinsert(memtype, "H", HOFFSET (HKLFamily_t, h), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "K", HOFFSET (HKLFamily_t, k), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "L", HOFFSET (HKLFamily_t, l), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "Solution 1", HOFFSET (HKLFamily_t, s1), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "Diffraction Intensity", HOFFSET (HKLFamily_t, diffractionIntensity), H5T_NATIVE_FLOAT);
    status = H5Tinsert(memtype, "Solution 2", HOFFSET (HKLFamily_t, s2), H5T_NATIVE_INT);

    /*
     * Create dataspace.  Setting maximum size to NULL sets the maximum
     * size to be the current size.
     */
    space = H5Screate_simple(1, dims, NULL);

    /*
     * Create the dataset and write the compound data to it.
     */

    dset = H5Dcreate (hklGid, StringUtils::numToString(index++).c_str(), memtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Dwrite (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void*)(&hkl)  );

    /*
     * Close and release resources.
     */
    status = H5Tclose (memtype);
    status = H5Sclose (space);
    status = H5Dclose (dset);
  }
  return status;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngImporter::setFileVersion(uint32_t version)
{
  m_FileVersion = version;
}

