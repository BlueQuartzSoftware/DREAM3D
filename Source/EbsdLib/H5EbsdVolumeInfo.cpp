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


#include "H5EbsdVolumeInfo.h"

#include <QtCore/QString>
#include <QtCore/QtDebug>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"

#define EBSD_VOLREADER_READ_HEADER(fileId, path, var)\
  err = H5Lite::readScalarDataset(fileId, path.toStdString(), var);\
  if (err < 0) {\
    qDebug() << "H5EbsdVolumeInfo Error: Could not load header value for " << path ;\
    err = H5Utilities::closeFile(fileId);\
    return err;\
  }

#define EBSD_VOLREADER_READ_VECTOR_HEADER(fileId, path, var, type)\
  {\
    std::vector<type> data;\
    err = H5Lite::readVectorDataset(fileId, path.toStdString(), data);\
    if (err < 0) {\
      qDebug() << "H5EbsdVolumeInfo Error: Could not load header (as vector) for " << path ;\
      err = H5Utilities::closeFile(fileId);\
      return err;\
    } else {\
      var.resize(static_cast<qint32>(data.size()));\
      ::memcpy(var.data(), &(data.front()), sizeof(type) * var.size());\
    }\
  }


#define EBSD_VOLREADER_READ_HEADER_CAST(fileId, path, var, m_msgType, cast)\
  { cast t;\
    err = H5Lite::readScalarDataset(fileId, path.toStdString(), t);\
    if (err < 0) {\
      qDebug() << "H5EbsdVolumeInfo Error: Could not load header value (with cast) for " << path ;\
      err = H5Utilities::closeFile(fileId);\
      return err;\
    }\
    var = static_cast<m_msgType>(t); }

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeInfo::H5EbsdVolumeInfo() :
  m_ErrorCode(0),
  m_ErrorMessage(""),
  m_ValuesAreCached(false),
  m_FileVersion(0),
  m_XDim(0),
  m_YDim(0),
  m_ZDim(0),
  m_XRes(0.0f),
  m_YRes(0.0f),
  m_ZRes(0.0f),
  m_ZStart(0),
  m_ZEnd(0),
  m_StackingOrder(SIMPL::RefFrameZDir::LowtoHigh),
  m_NumPhases(0),
  m_SampleTransformationAngle(0.0),
  m_EulerTransformationAngle(0.0)
{
  m_Manufacturer = "Unknown";

  m_SampleTransformationAxis.resize(3);
  m_SampleTransformationAxis[0] = 0.0;
  m_SampleTransformationAxis[1] = 0.0;
  m_SampleTransformationAxis[2] = 1.0;

  m_EulerTransformationAxis.resize(3);
  m_EulerTransformationAxis[0] = 0.0;
  m_EulerTransformationAxis[1] = 0.0;
  m_EulerTransformationAxis[2] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeInfo::~H5EbsdVolumeInfo()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5EbsdVolumeInfo::invalidateCache()
{
  m_ValuesAreCached = (false);
  m_XDim = (0);
  m_YDim = (0);
  m_ZDim = (0);
  m_XRes = (0.0f);
  m_YRes = (0.0f);
  m_ZRes = (0.0f);
  m_ZStart = (0);
  m_ZEnd = (0);
  m_NumPhases = (0);
  m_Manufacturer = "";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeInfo::updateToLatestVersion()
{
  invalidateCache();
  // Open the file with Read/Write access
  hid_t fileId = H5Utilities::openFile(m_FileName.toStdString(), false);
  if (fileId < 0)
  {
    //std::cout << "Error Opening file '" << m_FileName << "'" << std::endl;
    return -1;
  }
  // This sentinel will make sure the file is closed and the errors turned back on when we
  // exit the function
  HDF5ScopedFileSentinel sentinel(&fileId, true);

  // Update any existing datasets/attributes with new datasets/attributes/

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeInfo::readVolumeInfo()
{
  int err = -1;
  m_ValuesAreCached = false;
  int retErr = 0;
  hid_t fileId = H5Utilities::openFile(m_FileName.toStdString(), true);
  if (fileId < 0)
  {
    //std::cout << "Error Opening file '" << m_FileName << "'" << std::endl;
    return -1;
  }
  HDF5ScopedFileSentinel sentinel(&fileId, true);

  m_FileVersion = 0;
  // Attempt to read the file version number. If it is not there that is OK as early h5ebsd
  // files did not have this information written.
  err = H5Lite::readScalarAttribute(fileId, "/", Ebsd::H5::FileVersionStr.toStdString(), m_FileVersion);

  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::ZStartIndex, m_ZStart);
  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::ZEndIndex, m_ZEnd);
  m_ZDim = m_ZEnd - m_ZStart + 1; // The range is inclusive (zStart, zEnd)
  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::XPoints, m_XDim);
  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::YPoints, m_YDim);
  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::XResolution, m_XRes);
  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::YResolution, m_YRes);
  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::ZResolution, m_ZRes);

  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::StackingOrder, m_StackingOrder);
  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::SampleTransformationAngle, m_SampleTransformationAngle);
  EBSD_VOLREADER_READ_VECTOR_HEADER(fileId, Ebsd::H5::SampleTransformationAxis, m_SampleTransformationAxis, float);
  EBSD_VOLREADER_READ_HEADER(fileId, Ebsd::H5::EulerTransformationAngle, m_EulerTransformationAngle);
  EBSD_VOLREADER_READ_VECTOR_HEADER(fileId, Ebsd::H5::EulerTransformationAxis, m_EulerTransformationAxis, float);

  // Read the manufacturer from the file
  m_Manufacturer = "";
  std::string data;
  err = H5Lite::readStringDataset(fileId, Ebsd::H5::Manufacturer.toStdString(), data);
  if (err < 0)
  {
    std::cout << "H5EbsdVolumeInfo Error: Could not load header value for " << Ebsd::H5::Manufacturer.toStdString() << std::endl;
    err = H5Utilities::closeFile(fileId);
    return err;
  }
  m_Manufacturer = QString::fromStdString(data);

  // Get the Number of Phases in the Material
  // DO NOT Use the accessor methods below to get variables. Directly access them otherwise you
  // will cause an infinite recursion to occur.
  QString index = QString::number(m_ZStart);
  hid_t gid = H5Gopen(fileId, index.toStdString().c_str(), H5P_DEFAULT);
  if (gid > 0)
  {
    hid_t headerId = H5Gopen(gid, Ebsd::H5::Header.toStdString().c_str(), H5P_DEFAULT);
    if (headerId > 0)
    {
      hid_t phasesGid = H5Gopen(headerId, Ebsd::H5::Phases.toStdString().c_str(), H5P_DEFAULT);
      if (phasesGid > 0)
      {
        std::list<std::string> names;
        err = H5Utilities::getGroupObjects(phasesGid, H5Utilities::H5Support_GROUP, names);
        if (err >= 0)
        {
          m_NumPhases = static_cast<int>( names.size() );
        }
        H5Gclose(phasesGid);
      }
      H5Gclose(headerId);
    }

    // Now read out the names of the data arrays in the file

    hid_t dataGid = H5Gopen(gid, Ebsd::H5::Data.toStdString().c_str(), H5P_DEFAULT);
    if(dataGid > 0)
    {
      std::list<std::string> names;
      err = H5Utilities::getGroupObjects(dataGid, H5Utilities::H5Support_DATASET, names);
      if (err >= 0)
      {
        for(std::list<std::string>::iterator iter = names.begin(); iter != names.end(); iter++)
        {
          m_DataArrayNames.insert(QString::fromStdString(*iter));
        }
      }
      H5Gclose(dataGid);
    }
    H5Gclose(gid);
  }

// we are going to selectively replace some of the data array names with some common names instead
  if(m_Manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
  {
    if(m_DataArrayNames.contains(Ebsd::Ang::Phi1)
        && m_DataArrayNames.contains(Ebsd::Ang::Phi)
        && m_DataArrayNames.contains(Ebsd::Ang::Phi2) )
    {
      m_DataArrayNames.remove(Ebsd::Ang::Phi1);
      m_DataArrayNames.remove(Ebsd::Ang::Phi);
      m_DataArrayNames.remove(Ebsd::Ang::Phi2);
      m_DataArrayNames.insert(Ebsd::CellData::EulerAngles);
    }
    if(m_DataArrayNames.contains(Ebsd::Ang::PhaseData) )
    {
      m_DataArrayNames.remove(Ebsd::Ang::PhaseData);
      m_DataArrayNames.insert(Ebsd::CellData::Phases);
    }
  }
  else if(m_Manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
    if(m_DataArrayNames.contains(Ebsd::Ctf::Euler1)
        && m_DataArrayNames.contains(Ebsd::Ctf::Euler2)
        && m_DataArrayNames.contains(Ebsd::Ctf::Euler3) )
    {
      m_DataArrayNames.remove(Ebsd::Ctf::Euler1);
      m_DataArrayNames.remove(Ebsd::Ctf::Euler2);
      m_DataArrayNames.remove(Ebsd::Ctf::Euler3);
      m_DataArrayNames.insert(Ebsd::CellData::EulerAngles);
    }
    if(m_DataArrayNames.contains(Ebsd::Ctf::Phase) )
    {
      m_DataArrayNames.remove(Ebsd::Ctf::Phase);
      m_DataArrayNames.insert(Ebsd::CellData::Phases);
    }
  }



  m_ValuesAreCached = true;
  err = H5Utilities::closeFile(fileId);
  fileId = -1;
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t H5EbsdVolumeInfo::getFileVersion()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return 0; }
  }
  return m_FileVersion;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeInfo::getDimsAndResolution(int64_t& xDim, int64_t& yDim, int64_t& zDim,
                                           float& xRes, float& yRes, float& zRes)
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return -1; }
  }
  if (m_ValuesAreCached == true)
  {
    xDim = m_XDim;
    yDim = m_YDim;
    zDim = m_ZDim;
    xRes = m_XRes;
    yRes = m_YRes;
    zRes = m_ZRes;
    err = 0;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeInfo::getDims(int64_t& xDim, int64_t& yDim, int64_t& zDim)
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return -1; }
  }
  if (m_ValuesAreCached == true)
  {
    xDim = m_XDim;
    yDim = m_YDim;
    zDim = m_ZDim;
    err = 0;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeInfo::getResolution(float& xRes, float& yRes, float& zRes)
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return -1; }
  }
  if (m_ValuesAreCached == true)
  {
    xRes = m_XRes;
    yRes = m_YRes;
    zRes = m_ZRes;
    err = 0;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString H5EbsdVolumeInfo::getManufacturer()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return QString(""); }
  }
  return m_Manufacturer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeInfo::getNumSlices()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return 0; }
  }
  return m_ZDim;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeInfo::getZStart()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return 0; }
  }
  return m_ZStart;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeInfo::getZEnd()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return 0; }
  }
  return m_ZEnd;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeInfo::getNumPhases()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return 0; }
  }
  return m_NumPhases;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t H5EbsdVolumeInfo::getStackingOrder()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return SIMPL::RefFrameZDir::UnknownRefFrameZDirection; }
  }
  return m_StackingOrder;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float H5EbsdVolumeInfo::getSampleTransformationAngle()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return 0.0; }
  }
  return m_SampleTransformationAngle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> H5EbsdVolumeInfo::getSampleTransformationAxis()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0)
    {
      QVector<float> axis(3);
      axis[0] = 0.0;
      axis[1] = 0.0;
      axis[2] = 1.0;
      return axis;
    }
  }
  return m_SampleTransformationAxis;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float H5EbsdVolumeInfo::getEulerTransformationAngle()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return 0.0; }
  }
  return m_EulerTransformationAngle;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> H5EbsdVolumeInfo::getEulerTransformationAxis()
{
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0)
    {
      QVector<float> axis(3);
      axis[0] = 0.0;
      axis[1] = 0.0;
      axis[2] = 1.0;
      return axis;
    }
  }
  return m_EulerTransformationAxis;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> H5EbsdVolumeInfo::getDataArrayNames()
{
  QSet<QString> empty;
  int err = -1;
  if (m_ValuesAreCached == false)
  {
    err = readVolumeInfo();
    if (err < 0) { return empty; }
  }
  return m_DataArrayNames;
}
