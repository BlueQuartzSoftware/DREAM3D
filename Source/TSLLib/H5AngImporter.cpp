/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
///////////////////////////////////////////////////////////////////////////////
// This code was partly written under US Air Force Contract FA8650-07-D-5800
///////////////////////////////////////////////////////////////////////////////

#include "H5AngImporter.h"

#include <sstream>

#include "TSLLib/HDF5/H5Lite.h"
#include "TSLLib/HDF5/H5Utilities.h"
#include "TSLLib/Utilities/MXADir.h"
#include "TSLLib/Utilities/StringUtils.h"


//#include "DREAM3D/Common/Constants.h"

#define AIM_STRING std::string

#define CHECK_FOR_CANCELED(AClass)\
    if (m_Cancel == true){\
      break; }

using namespace Ang;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngImporter::H5AngImporter() :
m_ErrorCondition(0),
m_Cancel(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngImporter::~H5AngImporter()
{
}


#define WRITE_ANG_HEADER_DATA(reader, type, prpty, key)\
{\
  type t = reader.get##prpty();\
  err = H5Lite::writeScalarDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(gid); err = H5Gclose(angGroup);\
    return -1; }\
}

#define WRITE_ANG_HEADER_STRING_DATA(reader, type, prpty, key)\
{\
  type t = reader.get##prpty();\
  err = H5Lite::writeStringDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(gid); err = H5Gclose(angGroup);\
    return -1; }\
}

#define WRITE_ANG_DATA_ARRAY(reader, type, gid, prpty, key)\
{\
  type* dataPtr = reader.get##prpty##Pointer();\
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
int H5AngImporter::importAngFile(hid_t fileId, int z, const std::string &angFile)
{
  herr_t err = -1;

//  std::cout << "H5AngImporter: Importing " << angFile << std::endl;
  AngReader reader;
  reader.setFileName(angFile);
  // This is set to NoOrientation because we want to leave the data intact as it
  // was received from the instrument. The user can choose to rotate the data as
  // it is read from the resulting HDF5 data file.
  reader.setUserOrigin(Ang::NoOrientation);

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
    else
    {
      ss << "H5AngImporter Error: Unknown error.";
    }
    setErrorMessage(ss.str());
    setErrorCondition(err);
    progressMessage(ss.str(), 100);
    return -1;
  }

  // Start creating the HDF5 group structures for this file
  hid_t angGroup = H5Utilities::createGroup(fileId, StringUtils::numToString(z));
  if (angGroup < 0)
  {
    std::ostringstream ss;
    ss << "H5AngImporter Error: A Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons.";
    setErrorMessage(ss.str());
    setErrorCondition(-500);
    return -1;
  }

  hid_t gid = H5Utilities::createGroup(angGroup, Ang::Header);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5AngImporter Error: The 'Header' Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons.";
    progressMessage(ss.str(), 100);
    err = H5Gclose(angGroup);
    setErrorMessage(ss.str());
    setErrorCondition(-600);
    return -1;
  }
  WRITE_ANG_HEADER_DATA(reader, float, TEMpixPerum, TSL::OIM::TEMPIXPerUM)
  WRITE_ANG_HEADER_DATA(reader, float, XStar, TSL::OIM::XStar)
  WRITE_ANG_HEADER_DATA(reader, float, YStar, TSL::OIM::YStar)
  WRITE_ANG_HEADER_DATA(reader, float, ZStar, TSL::OIM::ZStar)
  WRITE_ANG_HEADER_DATA(reader, float, WorkingDistance, TSL::OIM::WorkingDistance)

  hid_t phasesGid = H5Utilities::createGroup(gid, Ang::Phases);
  err = writePhaseData(reader, phasesGid);
  // Close this group
  err = H5Gclose(phasesGid);

  WRITE_ANG_HEADER_STRING_DATA(reader, std::string, Grid, TSL::OIM::Grid)
  WRITE_ANG_HEADER_DATA(reader, float, XStep, TSL::OIM::XStep)
  WRITE_ANG_HEADER_DATA(reader, float, YStep, TSL::OIM::YStep)
  WRITE_ANG_HEADER_DATA(reader, int, NumOddCols, TSL::OIM::NColsOdd)
  WRITE_ANG_HEADER_DATA(reader, int, NumEvenCols, TSL::OIM::NColsEven)
  WRITE_ANG_HEADER_DATA(reader, int, NumRows, TSL::OIM::NRows)
  WRITE_ANG_HEADER_STRING_DATA(reader, std::string, OIMOperator, TSL::OIM::Operator)
  WRITE_ANG_HEADER_STRING_DATA(reader, std::string, SampleID, TSL::OIM::SampleId)
  WRITE_ANG_HEADER_STRING_DATA(reader, std::string, ScanID, TSL::OIM::ScanId)

  std::string angCompleteHeader = reader.getCompleteHeader();
  err = H5Lite::writeStringDataset(gid, Ang::AngHeader, angCompleteHeader);
  err = H5Lite::writeStringDataset(gid, Ang::OriginalFile, angFile);

  // Close the "Header" group
  err = H5Gclose(gid);

  // Create the "Data" group
  gid = H5Utilities::createGroup(angGroup, Ang::Data);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5AngImporter Error: The 'Data' Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons."<< std::endl;
    progressMessage(ss.str(), 100);
    err = H5Gclose(angGroup);
    setErrorMessage(ss.str());
    setErrorCondition(-700);
    return -1;
  }

  int32_t rank = 1;
  hsize_t dims[1] = { reader.getNumEvenCols() * reader.getNumRows() };

  WRITE_ANG_DATA_ARRAY(reader, float, gid, Phi1, TSL::OIM::Phi1);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, Phi, TSL::OIM::Phi);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, Phi2, TSL::OIM::Phi2);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, XPos, TSL::OIM::XPosition);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, YPos, TSL::OIM::YPosition);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, ImageQuality, TSL::OIM::ImageQuality);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, ConfidenceIndex, TSL::OIM::ConfidenceIndex);
  WRITE_ANG_DATA_ARRAY(reader, int, gid, Phase, TSL::OIM::PhaseData);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, SEMSignal, TSL::OIM::SEMSignal);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, Fit, TSL::OIM::Fit);
  // Close the "Data" group
  err = H5Gclose(gid);

  // Close the group for this file
  err = H5Gclose(angGroup);

  return err;
}


#define WRITE_PHASE_HEADER_DATA(reader, type, prpty, key)\
{\
  type t = reader->get##prpty();\
  err = H5Lite::writeScalarDataset(pid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(pid);\
    return -1; }\
}

#define WRITE_PHASE_HEADER_STRING_DATA(reader, type, prpty, key)\
{\
  type t = reader->get##prpty();\
  err = H5Lite::writeStringDataset(pid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    progressMessage(ss.str(), 100);\
    err = H5Gclose(pid);\
    return -1; }\
}

#define WRITE_PHASE_DATA_ARRAY(reader, type, gid, prpty, key)\
{\
  std::vector<type> tempVar = reader->get##prpty();\
  dims[0] = tempVar.size();\
  type* dataPtr = &(tempVar.front());\
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
  int retErr = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 0 };
  std::vector<AngPhase::Pointer> phases = reader.getPhases();
  for (std::vector<AngPhase::Pointer>::iterator phase = phases.begin(); phase != phases.end(); ++phase )
  {
    AngPhase* p = (*phase).get();
    hid_t pid = H5Utilities::createGroup(phasesGid, StringUtils::numToString(p->getPhase()));
    WRITE_PHASE_HEADER_DATA((*phase), int, Phase, TSL::OIM::Phase)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, MaterialName, TSL::OIM::MaterialName)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, Formula, TSL::OIM::Formula)
    WRITE_PHASE_HEADER_STRING_DATA((*phase), std::string, Info, TSL::OIM::Info)
    WRITE_PHASE_HEADER_DATA((*phase), int, Symmetry, TSL::OIM::Symmetry)
    WRITE_PHASE_DATA_ARRAY( (*phase), float, pid, LatticeConstants, TSL::OIM::LatticeConstants)
    WRITE_PHASE_HEADER_DATA((*phase), int, NumberFamilies, TSL::OIM::NumberFamilies)

    // Create a Group for the HKLFamilies
    if (p->getNumberFamilies() > 0) {
      hid_t hklGid = H5Utilities::createGroup(pid, TSL::OIM::HKLFamilies);
      err = writeHKLFamilies(p, hklGid);
      if (err < 0) {
        std::ostringstream ss;
        ss << "H5AngImporter Error: Could not write Ang HKL Families to the HDF5 file with data set name '" 
          << TSL::OIM::HKLFamilies << "'" << std::endl;
        progressMessage(ss.str(), 100);
        err = H5Gclose(hklGid);
        return -1; 
      }
      err = H5Gclose(hklGid);
    }
    WRITE_PHASE_DATA_ARRAY( (*phase), int, pid, Categories, TSL::OIM::Categories)
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

    dset = H5Dcreate (hklGid, StringUtils::numToString(index++).c_str(), memtype, space, H5P_DEFAULT);

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
void H5AngImporter::progressMessage(const std::string &message, int progress)
{
  std::cout << progress << "% " << message << std::endl;
}

