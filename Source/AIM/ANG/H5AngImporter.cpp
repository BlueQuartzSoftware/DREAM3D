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

#include "H5AngImporter.h"

#include <sstream>

#include "MXA/MXATypes.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/HDF5/H5Lite.h"
#include "MXA/HDF5/H5Utilities.h"
#include "MXA/Utilities/StringUtils.h"


#include "AIM/ANG/AngReader.h"
#include "AIM/Common/Constants.h"
#include "AIM/ANG/AngDirectoryPatterns.h"


#ifdef AIM_USE_QT
#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif

#ifdef AIM_USE_QT
#define CHECK_FOR_CANCELED(AClass)\
    if (this->m_Cancel) { \
      QString msg = #AClass; \
              msg += " was Canceled"; \
              emit updateMessage(msg);\
              emit updateProgress(100);\
      break;}

#else
#define CHECK_FOR_CANCELED(AClass)\
    if (m_Cancel == true){\
      break; }
#endif


#if AIM_USE_QT

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngImporter::Pointer H5AngImporter::New( QObject* parent)
{
  Pointer sharedPtr(new H5AngImporter(parent));
  return sharedPtr;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngImporter::H5AngImporter(
#if AIM_USE_QT
QObject* parent
#endif
) :
m_ZStartIndex(0),
m_ZEndIndex(0),
m_ZResolution(1.0),
m_Cancel(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngImporter::~H5AngImporter()
{
}

#if AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngImporter::run()
{
  compute();
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngImporter::compute()
{
  herr_t err = 0;
  int32_t sliceCount = 1;
  int32_t width = 0;
  int32_t totalSlices = m_AngSeriesMaxSlice;
  while (sliceCount < totalSlices)
  {
    ++width;
    sliceCount *= 10;
  }

  m_InputDirectory = MXADir::toNativeSeparators(m_InputDirectory);
  AngDirectoryPatterns::Pointer p
          = AngDirectoryPatterns::New(m_InputDirectory, m_AngFilePrefix, width);


  if (p.get() == NULL)
  {
    std::string s("H5AngImport Error: The pointer for the helper class AngDirectoryPatterns is NULL which means");
    s.append(" this class was NOT set. This algorithm depends on that helper class to generate");
    s.append(" the proper file names to import. No data was imported.");
    progressMessage(s, 100);
    return;
  }
  if (m_OutputFile.empty() == true)
  {
    std::string s("H5AngImport Error: The output file was not set correctly or is empty. The current value is '");
    s.append("'. Please set the output file before running the importer. ");
    progressMessage(s, 100);
    return;
  }
  // Create File
  hid_t fileId = H5Utilities::createFile(m_OutputFile);
  if (fileId < 0) {
    std::string s("Error Creating HDF5 file. No data imported.");
    progressMessage(s, 100);
    return;
  }

  // Write Z index start, Z index end and Z Resolution to the HDF5 file
  err = H5Lite::writeScalarDataset(fileId, AIM::ANG::ZStartIndex, m_ZStartIndex);
  err = H5Lite::writeScalarDataset(fileId, AIM::ANG::ZEndIndex, m_ZEndIndex);
  err = H5Lite::writeScalarDataset(fileId, AIM::ANG::ZResolution, m_ZResolution);
  // Write the Manufacturer of the OIM file here
  err = H5Lite::writeStringDataset(fileId, AIM::ANG::Manufacturer, AIM::ANG::TSL );

  std::vector<int> indices;
  // Loop on Each Ang File
  float total = m_ZEndIndex - m_ZStartIndex;
  int progress = 0;
  for (int z = m_ZStartIndex; z <= m_ZEndIndex; z++)
  {
    std::string angFName = p->generateFullPathAngFileName(z);

    CHECK_FOR_CANCELED(H5AngImporter)
    progress = z - m_ZStartIndex;
    progress = (int)(100.0f * (float)(progress)/total);
    std::string msg = "Importing: " + angFName;
    progressMessage(msg, progress );

    err = importAngFile(fileId, z, angFName);
    if (err < 0)
    {
      setCancel(true);
    }
    indices.push_back(z);
  }

  if (false == m_Cancel) {
  // Write an Index data set which contains all the z index values which
  // should help speed up the reading side of this file
    std::vector<hsize_t> dims(1, indices.size());
    err = H5Lite::writeVectorDataset(fileId, AIM::ANG::Index, dims, indices);
  }
  err = H5Fclose(fileId);
  return;
}

#define WRITE_ANG_HEADER_DATA(HeaderType, type, prpty, key)\
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

#define WRITE_ANG_HEADER_STRING_DATA(HeaderType, type, prpty, key)\
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

  hid_t angGroup = H5Utilities::createGroup(fileId, StringUtils::numToString(z));
  if (angGroup < 0)
  {
    std::ostringstream ss;
    ss << "H5AngImporter Error: A Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons.";
    progressMessage(ss.str(), 100);
    return -1;
  }

//  std::cout << "H5AngImporter: Importing " << angFile << std::endl;
  AngReader reader;
  reader.setFileName(angFile);
  reader.setUserOrigin(AngReader::NoOrientation);
  err = reader.readFile();

  hid_t gid = H5Utilities::createGroup(angGroup, AIM::ANG::Header);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5AngImporter Error: The 'Header' Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons.";
    progressMessage(ss.str(), 100);
    err = H5Gclose(angGroup);
    return -1;
  }
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<float>, float, TEMpixPerum, TSL::OIM::TEMPIXPerUM)
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<float>, float, XStar, TSL::OIM::XStar)
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<float>, float, YStar, TSL::OIM::YStar)
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<float>, float, ZStar, TSL::OIM::ZStar)
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<float>, float, WorkingDistance, TSL::OIM::WorkingDistance)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Phase, TSL::OIM::Phase)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, MaterialName, TSL::OIM::MaterialName)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Formula, TSL::OIM::Formula)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Info, TSL::OIM::Info)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Symmetry, TSL::OIM::Symmetry)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, LatticeConstants, TSL::OIM::LatticeConstants)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, NumberFamilies, TSL::OIM::NumberFamilies)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, HKLFamilies, TSL::OIM::HKLFamilies)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Categories, TSL::OIM::Categories)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Grid, TSL::OIM::Grid)
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<float>, float, XStep, TSL::OIM::XStep)
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<float>, float, YStep, TSL::OIM::YStep)
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<int>, int, NumOddCols, TSL::OIM::NColsOdd)
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<int>, int, NumEvenCols, TSL::OIM::NColsEven)
  WRITE_ANG_HEADER_DATA(AngHeaderEntry<int>, int, NumRows, TSL::OIM::NRows)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, OIMOperator, TSL::OIM::Operator)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, SampleID, TSL::OIM::SampleId)
  WRITE_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, ScanID, TSL::OIM::ScanId)

  std::string angCompleteHeader = reader.getCompleteHeader();
  err = H5Lite::writeStringDataset(gid, AIM::ANG::AngHeader, angCompleteHeader);
  err = H5Lite::writeStringDataset(gid, AIM::ANG::OriginalFile, angFile);

  // Close the "Header" group
  err = H5Gclose(gid);

  // Create the "Data" group
  gid = H5Utilities::createGroup(angGroup, AIM::ANG::Data);
  if (gid < 0)
  {
    std::ostringstream ss;
    ss << "H5AngImporter Error: The 'Data' Group for Z index " << z << " could not be created."
         << " Please check other error messages from the HDF5 library for possible reasons."<< std::endl;
    progressMessage(ss.str(), 100);
    err = H5Gclose(angGroup);
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
  WRITE_ANG_DATA_ARRAY(reader, float, gid, Phase, TSL::OIM::PhaseData);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, D1, TSL::OIM::D1);
  WRITE_ANG_DATA_ARRAY(reader, float, gid, D2, TSL::OIM::D2);
  // Close the "Data" group
  err = H5Gclose(gid);

  // Close the group for this file
  err = H5Gclose(angGroup);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngImporter::progressMessage(const std::string &message, int progress)
{
#ifdef AIM_USE_QT
      emit updateMessage(QString(message.c_str()));
      emit updateProgress(progress);
    //  std::cout << message.toStdString() << std::endl;
#else
  std::cout << progress << "% " << message << std::endl;
#endif
}

#ifdef AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5AngImporter::on_CancelWorker()
{
  this->m_Cancel = true;
}
#endif



