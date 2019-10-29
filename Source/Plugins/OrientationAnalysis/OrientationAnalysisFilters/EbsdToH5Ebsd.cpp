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

#include <memory>

#include "EbsdToH5Ebsd.h"

#include <QtCore/QDir>

#include "H5Support/QH5Utilities.h"
#include "H5Support/H5ScopedSentinel.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"

#include "EbsdLib/HKL/H5CtfImporter.h"
#include "EbsdLib/TSL/H5AngImporter.h"

#include "OrientationAnalysis/FilterParameters/EbsdToH5EbsdFilterParameter.h"
#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdToH5Ebsd::EbsdToH5Ebsd()
: m_OutputFile("")
, m_ZStartIndex(0)
, m_ZEndIndex(0)
, m_ZResolution(1.0f)
, m_RefFrameZDir(SIMPL::RefFrameZDir::LowtoHigh)
, m_InputPath("")
, m_FilePrefix("")
, m_FileSuffix("")
, m_FileExtension("ang")
, m_PaddingDigits(4)
{
  m_SampleTransformation.angle = 0.0f;
  m_SampleTransformation.h = 0.0f;
  m_SampleTransformation.k = 0.0f;
  m_SampleTransformation.l = 1.0f;

  m_EulerTransformation.angle = 0.0f;
  m_EulerTransformation.h = 0.0f;
  m_EulerTransformation.k = 0.0f;
  m_EulerTransformation.l = 1.0f;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdToH5Ebsd::~EbsdToH5Ebsd() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(EbsdToH5EbsdFilterParameter::New("Import Orientation Data", "OrientationData", getOutputFile(), FilterParameter::Parameter, this));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputFile(reader->readString("OutputFile", getOutputFile()));
  setZStartIndex(reader->readValue("ZStartIndex", getZStartIndex()));
  setZEndIndex(reader->readValue("ZEndIndex", getZEndIndex()));
  setZResolution(reader->readValue("ZResolution", getZResolution()));
  setRefFrameZDir(reader->readValue("RefFrameZDir", getRefFrameZDir()));
  setInputPath(reader->readString("InputPath", getInputPath()));
  setFilePrefix(reader->readString("FilePrefix", getFilePrefix()));
  setFileSuffix(reader->readString("FileSuffix", getFileSuffix()));
  setFileExtension(reader->readString("FileExtension", getFileExtension()));
  setPaddingDigits(reader->readValue("PaddingDigits", getPaddingDigits()));
  setSampleTransformation(reader->readAxisAngle("SampleTransformation", getSampleTransformation(), -1));
  setEulerTransformation(reader->readAxisAngle("EulerTransformation", getEulerTransformation(), -1));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  QString ss;

  if(m_OutputFile.isEmpty())
  {
    ss = QObject::tr("The output file must be set");
    setErrorCondition(-12, ss);
  }

  QFileInfo fi(m_InputPath);
  if(m_InputPath.isEmpty())
  {
    ss = QObject::tr("The Input Directory must be set");
    setErrorCondition(-13, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input directory does not exist: %1").arg(getInputPath());
    setErrorCondition(-388, ss);
  }

  bool hasMissingFiles = false;
  const bool stackLowToHigh = true;
  int increment = 1;

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList =
      FilePathGenerator::GenerateFileList(m_ZStartIndex, m_ZEndIndex, increment, hasMissingFiles, stackLowToHigh, m_InputPath, m_FilePrefix, m_FileSuffix, m_FileExtension, m_PaddingDigits);

  if(fileList.empty())
  {
    ss.clear();
    QTextStream out(&ss);
    out << " No files have been selected for import. Have you set the input directory and other values so that input files will be generated?\n";
    out << "InputPath: " << m_InputPath << "\n";
    out << "FilePrefix: " << m_FilePrefix << "\n";
    out << "FileSuffix: " << m_FileSuffix << "\n";
    out << "FileExtension: " << m_FileExtension << "\n";
    out << "PaddingDigits: " << m_PaddingDigits << "\n";
    out << "StartIndex: " << m_ZStartIndex << "\n";
    out << "EndIndex: " << m_ZEndIndex << "\n";

    setErrorCondition(-11, ss);
  }
  else
  {
    // Based on the type of file (.ang or .ctf) get the list of arrays that would be created
    QFileInfo fi(fileList.front());
    QString ext = fi.suffix();
    if(ext.compare(Ebsd::Ang::FileExt) == 0)
    {
      // ebsdFeatures = new AngFields;
    }
    else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
    {
      // ebsdFeatures = new CtfFields;
    }
    else
    {
      ss = QObject::tr("The file extension '%1' was not recognized. Currently .ang or .ctf are the only recognized file extensions").arg(ext);
      setErrorCondition(-997, ss);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  herr_t err = 0;
  hid_t fileId = -1;

  if(m_OutputFile.isEmpty())
  {
    QString ss = QObject::tr("EbsdToH5Ebsd Error: The output file was not set correctly or is empty. The current value is '%1'"
                             ". Please set the output file before running the importer. ")
                     .arg(m_OutputFile);
    setErrorCondition(-1, ss);
    return;
  }
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss;
    setErrorCondition(-1, ss);
    return;
  }

  // Create File
  fileId = QH5Utilities::createFile(m_OutputFile);
  if(fileId < 0)
  {
    err = -1;
    QString ss = QObject::tr("The output HDF5 file could not be created. Check permissions or if the file is in use by another program");
    setErrorCondition(-1, ss);
    return;
  }

  H5ScopedFileSentinel sentinel(&fileId, true);

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::ZResolution, m_ZResolution);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the Z Spacing Scalar to the HDF5 File");
    setErrorCondition(-1, ss);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::StackingOrder, m_RefFrameZDir);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the Stacking Order Scalar to the HDF5 File");
    setErrorCondition(-1, ss);
  }

  QString s = Ebsd::StackingOrder::Utils::getStringForEnum(m_RefFrameZDir);
  err = QH5Lite::writeStringAttribute(fileId, Ebsd::H5Ebsd::StackingOrder, "Name", s);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the Stacking Order Name Attribute to the HDF5 File");
    setErrorCondition(-1, ss);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::SampleTransformationAngle, m_SampleTransformation.angle);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the Sample Transformation Angle to the HDF5 File");
    setErrorCondition(-1, ss);
  }

  int32_t rank = 1;
  hsize_t dims[3] = {3, 0, 0};
  err = QH5Lite::writePointerDataset<float>(fileId, Ebsd::H5Ebsd::SampleTransformationAxis, rank, dims, &(m_SampleTransformation.h));
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the Sample Transformation Axis to the HDF5 File");
    setErrorCondition(-1, ss);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::EulerTransformationAngle, m_EulerTransformation.angle);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the Euler Transformation Angle to the HDF5 File");
    setErrorCondition(-1, ss);
  }

  err = QH5Lite::writePointerDataset<float>(fileId, Ebsd::H5Ebsd::EulerTransformationAxis, rank, dims, &(m_EulerTransformation.h));
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the Euler Transformation Axis to the HDF5 File");
    setErrorCondition(-1, ss);
  }

  // QString filename = QString("%1%2%3.%4").arg(m_FilePrefix)
  //      .arg(QString::number(m_ZStartIndex), m_PaddingDigits, '0')
  //      .arg(m_FileSuffix).arg(m_FileExtension);

  bool hasMissingFiles = false;
  const bool stackLowToHigh = true;
  int increment = 1;

  // if( SIMPL::RefFrameZDir::LowtoHigh == m_RefFrameZDir) { stackLowToHigh = true; }
  // else if (SIMPL::RefFrameZDir::HightoLow == m_RefFrameZDir) { stackLowToHigh = false; }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList =
      FilePathGenerator::GenerateFileList(m_ZStartIndex, m_ZEndIndex, increment, hasMissingFiles, stackLowToHigh, m_InputPath, m_FilePrefix, m_FileSuffix, m_FileExtension, m_PaddingDigits);

  EbsdImporter::Pointer fileImporter;

  // Write the Manufacturer of the OIM file here
  // This list will grow to be the number of EBSD file formats we support
  QFileInfo fiExt(fileList.front());
  QString ext = fiExt.suffix();
  if(ext.compare(Ebsd::Ang::FileExt) == 0)
  {
    err = QH5Lite::writeStringDataset(fileId, Ebsd::H5Ebsd::Manufacturer, Ebsd::Ang::Manufacturer);
    if(err < 0)
    {

      QString ss = QObject::tr("Could not write the Manufacturer Data to the HDF5 File");
      setErrorCondition(-1, ss);
    }
    fileImporter = H5AngImporter::New();
  }
  else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
  {
    err = QH5Lite::writeStringDataset(fileId, Ebsd::H5Ebsd::Manufacturer, Ebsd::Ctf::Manufacturer);
    if(err < 0)
    {

      QString ss = QObject::tr("Could not write the Manufacturer Data to the HDF5 File");
      setErrorCondition(-1, ss);
    }
    fileImporter = H5CtfImporter::New();
    CtfReader ctfReader;
    ctfReader.setFileName(fileList.front());
    err = ctfReader.readHeaderOnly();
    if(ctfReader.getZCells() > 1 && fileList.size() == 1)
    {
      m_ZStartIndex = 0;
      m_ZEndIndex = 0;
    }
  }
  else
  {
    err = -1;

    QString ss = QObject::tr("The file extension was not detected correctly");
    setErrorCondition(-1, ss);

    return;
  }

  QVector<int32_t> indices;
  // Loop on Each EBSD File
  float total = static_cast<float>(m_ZEndIndex - m_ZStartIndex);
  int32_t progress = 0;
  int64_t z = m_ZStartIndex;
  int64_t xDim = 0, yDim = 0;
  float xRes = 0.0f, yRes = 0.0f;
  /* There is a frailness about the z index and the file list. The programmer
   * using this code MUST ensure that the list of files that is sent into this
   * class is in the appropriate order to match up with the z index (slice index)
   * otherwise the import will have subtle errors. The programmer is urged NOT to
   * simply gather a list from the file system as those lists are sorted in such
   * a way that if the number of digits appearing in the filename are NOT the same
   * then the list will be wrong, ie, this example:
   *
   * slice_1.ang
   * slice_2.ang
   * ....
   * slice_10.ang
   *
   * Most, if not ALL C++ libraries when asked for that list will return the list
   * sorted like the following:
   *
   * slice_1.ang
   * slice_10.ang
   * slice_2.ang
   *
   * which is going to cause problems because the data is going to be placed
   * into the HDF5 file at the wrong index. YOU HAVE BEEN WARNED.
   */
  int64_t biggestxDim = 0;
  int64_t biggestyDim = 0;
  int32_t totalSlicesImported = 0;
  for(QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
  {
    QString ebsdFName = *filepath;
    progress = static_cast<int32_t>(z - m_ZStartIndex);
    progress = (int32_t)(100.0f * (float)(progress) / total);
    QString msg = "Converting File: " + ebsdFName;

    notifyStatusMessage(msg.toLatin1().data());
    err = fileImporter->importFile(fileId, z, ebsdFName);
    if(err < 0)
    {
      setErrorCondition(err, fileImporter->getPipelineMessage());
      return;
    }
    totalSlicesImported = totalSlicesImported + fileImporter->numberOfSlicesImported();

    fileImporter->getDims(xDim, yDim);
    fileImporter->getSpacing(xRes, yRes);
    if(xDim > biggestxDim)
    {
      biggestxDim = xDim;
    }
    if(yDim > biggestyDim)
    {
      biggestyDim = yDim;
    }

    if(err < 0)
    {
      QString ss = QObject::tr("Could not write dataset for slice to HDF5 file");
      setErrorCondition(-1, ss);
    }

    indices.push_back(static_cast<int32_t>(z));
    ++z;
    if(getCancel())
    {
      return;
    }
  }

  // Write Z index start, Z index end and Z Spacing to the HDF5 file
  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::ZStartIndex, m_ZStartIndex);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the Z Start Index Scalar to the HDF5 File");
    setErrorCondition(-1, ss);
  }

  m_ZEndIndex = m_ZStartIndex + totalSlicesImported - 1;
  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::ZEndIndex, m_ZEndIndex);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the Z End Index Scalar to the HDF5 File");
    setErrorCondition(-1, ss);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::XPoints, biggestxDim);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the XPoints Scalar to HDF5 file");
    setErrorCondition(-1, ss);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::YPoints, biggestyDim);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the YPoints Scalar to HDF5 file");
    setErrorCondition(-1, ss);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::XResolution, xRes);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the XResolution Scalar to HDF5 file");
    setErrorCondition(-1, ss);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5Ebsd::YResolution, yRes);
  if(err < 0)
  {
    QString ss = QObject::tr("Could not write the YResolution Scalar to HDF5 file");
    setErrorCondition(-1, ss);
  }

  if(!getCancel())
  {
    // Write an Index data set which contains all the z index values which
    // should help speed up the reading side of this file
    QVector<hsize_t> dimsL(1, indices.size());
    err = QH5Lite::writeVectorDataset(fileId, Ebsd::H5Ebsd::Index, dimsL, indices);
  }
  err = QH5Utilities::closeFile(fileId);
  fileId = -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EbsdToH5Ebsd::newFilterInstance(bool copyFilterParameters) const
{
  EbsdToH5Ebsd::Pointer filter = EbsdToH5Ebsd::New();
  if(copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    // We are going to hand copy all of the parameters because the other way of copying the parameters are going to
    // miss some of them because we are not enumerating all of them.
    filter->setOutputFile(getOutputFile());
    filter->setZStartIndex(getZStartIndex());
    filter->setZEndIndex(getZEndIndex());
    filter->setZResolution(getZResolution());
    filter->setRefFrameZDir(getRefFrameZDir());
    filter->setInputPath(getInputPath());
    filter->setFilePrefix(getFilePrefix());
    filter->setFileSuffix(getFileSuffix());
    filter->setFileExtension(getFileExtension());
    filter->setPaddingDigits(getPaddingDigits());
    filter->setSampleTransformation(getSampleTransformation());
    filter->setEulerTransformation(getEulerTransformation());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid EbsdToH5Ebsd::getUuid() const
{
  return QUuid("{6e332fca-0475-5fec-821e-e01f668ec1d3}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getHumanLabel() const
{
  return "Import Orientation File(s) to H5EBSD";
}

// -----------------------------------------------------------------------------
EbsdToH5Ebsd::Pointer EbsdToH5Ebsd::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<EbsdToH5Ebsd> EbsdToH5Ebsd::New()
{
  struct make_shared_enabler : public EbsdToH5Ebsd
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getNameOfClass() const
{
  return QString("EbsdToH5Ebsd");
}

// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::ClassName()
{
  return QString("EbsdToH5Ebsd");
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setOutputFile(const QString& value)
{
  m_OutputFile = value;
}

// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getOutputFile() const
{
  return m_OutputFile;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setZStartIndex(int64_t value)
{
  m_ZStartIndex = value;
}

// -----------------------------------------------------------------------------
int64_t EbsdToH5Ebsd::getZStartIndex() const
{
  return m_ZStartIndex;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setZEndIndex(int64_t value)
{
  m_ZEndIndex = value;
}

// -----------------------------------------------------------------------------
int64_t EbsdToH5Ebsd::getZEndIndex() const
{
  return m_ZEndIndex;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setZResolution(float value)
{
  m_ZResolution = value;
}

// -----------------------------------------------------------------------------
float EbsdToH5Ebsd::getZResolution() const
{
  return m_ZResolution;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setRefFrameZDir(uint32_t value)
{
  m_RefFrameZDir = value;
}

// -----------------------------------------------------------------------------
uint32_t EbsdToH5Ebsd::getRefFrameZDir() const
{
  return m_RefFrameZDir;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setInputPath(const QString& value)
{
  m_InputPath = value;
}

// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getInputPath() const
{
  return m_InputPath;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setFilePrefix(const QString& value)
{
  m_FilePrefix = value;
}

// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getFilePrefix() const
{
  return m_FilePrefix;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setFileSuffix(const QString& value)
{
  m_FileSuffix = value;
}

// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getFileSuffix() const
{
  return m_FileSuffix;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setFileExtension(const QString& value)
{
  m_FileExtension = value;
}

// -----------------------------------------------------------------------------
QString EbsdToH5Ebsd::getFileExtension() const
{
  return m_FileExtension;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setPaddingDigits(int value)
{
  m_PaddingDigits = value;
}

// -----------------------------------------------------------------------------
int EbsdToH5Ebsd::getPaddingDigits() const
{
  return m_PaddingDigits;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setSampleTransformation(const AxisAngleInput_t& value)
{
  m_SampleTransformation = value;
}

// -----------------------------------------------------------------------------
AxisAngleInput_t EbsdToH5Ebsd::getSampleTransformation() const
{
  return m_SampleTransformation;
}

// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setEulerTransformation(const AxisAngleInput_t& value)
{
  m_EulerTransformation = value;
}

// -----------------------------------------------------------------------------
AxisAngleInput_t EbsdToH5Ebsd::getEulerTransformation() const
{
  return m_EulerTransformation;
}
