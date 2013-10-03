/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "EbsdToH5Ebsd.h"

#include "H5Support/QH5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>


#include "EbsdLib/EbsdImporter.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/TSL/H5AngImporter.h"
#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/HKL/CtfFields.h"
#include "EbsdLib/HKL/H5CtfImporter.h"
#include "EbsdLib/HEDM/MicConstants.h"
#include "EbsdLib/HEDM/MicFields.h"
#include "EbsdLib/HEDM/H5MicImporter.h"

#include "DREAM3DLib/Common/Observable.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdToH5Ebsd::EbsdToH5Ebsd() :
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_ZResolution(1.0),
  m_SampleTransformationAngle(0.0),
  m_EulerTransformationAngle(0.0)
{
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
EbsdToH5Ebsd::~EbsdToH5Ebsd()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::setupFilterParameters()
{
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputFile( reader->readValue("OutputFile", getOutputFile()) );
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex()) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex()) );
  setZResolution( reader->readValue("ZResolution", getZResolution()) );
  setSampleTransformationAngle( reader->readValue("SampleTransformationAngle", getSampleTransformationAngle()) );
  setSampleTransformationAxis( reader->readValue("SampleTransformationAxis", getSampleTransformationAxis()) );
  setEulerTransformationAngle( reader->readValue("EulerTransformationAngle", getEulerTransformationAngle()) );
  setEulerTransformationAxis( reader->readValue("EulerTransformationAxis", getEulerTransformationAxis()) );
  setRefFrameZDir( static_cast<Ebsd::RefFrameZDir>( reader->readValue("RefFrameZDir", getRefFrameZDir() ) ) );
  setEbsdFileList( reader->readValue("EbsdFileList", getEbsdFileList()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EbsdToH5Ebsd::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile());
  writer->writeValue("ZStartIndex", getZStartIndex());
  writer->writeValue("ZEndIndex", getZEndIndex());
  writer->writeValue("ZResolution", getZResolution());
  writer->writeValue("SampleTransformationAngle", getSampleTransformationAngle());
  writer->writeValue("SampleTransformationAxis", getSampleTransformationAxis());
  writer->writeValue("EulerTransformationAngle", getEulerTransformationAngle());
  writer->writeValue("EulerTransformationAxis", getEulerTransformationAxis());
  writer->writeValue("RefFrameZDir", getRefFrameZDir());
  writer->writeValue("EbsdFileList", getEbsdFileList());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  QString ss;

  if (m_EbsdFileList.size() == 0)
  {
    ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    addErrorMessage(getHumanLabel(), ss, -11);
    setErrorCondition(-1);
  }
  else
  {
    // Based on the type of file (.ang or .ctf) get the list of arrays that would be created
    QFileInfo fi(m_EbsdFileList.front());
    QString ext = fi.suffix();
    QVector<QString> columnNames;
    //    AbstractEbsdFields* ebsdFields = NULL;
    if(ext.compare(Ebsd::Ang::FileExt) == 0)
    {
      //      ebsdFields = new AngFields;
    }
    else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
    {
      //       ebsdFields = new CtfFields;
    }
    else if(ext.compare(Ebsd::Mic::FileExt) == 0)
    {
      //       ebsdFields = new MicFields;
    }
    else
    {

      ss = QObject::tr("The File extension '%1' was not recognized. Currently .ang or .ctf are the only recognized file extensions").arg(ext);
      addErrorMessage(getHumanLabel(), ss, -997);
      setErrorCondition(-1);
      return;
    }
    //    columnNames = ebsdFields->getFieldNames();
    //    for(QVector<QString>::size_type i = 0; i < columnNames.size(); ++i)
    //    {
    //      addCreatedCellData(columnNames[i]);
    //    }
  }

  if(m_OutputFile.isEmpty() == true)
  {

    ss = QObject::tr("The output file must be set before executing this filter.");
    addErrorMessage(getHumanLabel(), ss, -12);
    setErrorCondition(-1);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5Ebsd::execute()
{
  QString ss;
  herr_t err = 0;
  hid_t fileId = -1;

  if(m_OutputFile.isEmpty() == true)
  {
    ss = QObject::tr("EbsdToH5Ebsd Error: The output file was not set correctly or is empty. The current value is '%1'"
                     ". Please set the output file before running the importer. ").arg(m_OutputFile);
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
    PipelineMessage em (getHumanLabel(), ss, -1);
    addErrorMessage(em);
    setErrorCondition(-1);
    return;
  }

  // Create File
  fileId = QH5Utilities::createFile(m_OutputFile);
  if(fileId < 0)
  {
    err = -1;

    ss = QObject::tr("The Output HDF5 file could not be created. Check Permissions, if the File is in use by another program.");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
    return;
  }

  HDF5ScopedFileSentinel sentinel(&fileId, true);

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::ZResolution, m_ZResolution);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the Z Resolution Scalar to the HDF5 File");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }
  unsigned int ui = static_cast<unsigned int>(m_RefFrameZDir);
  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::StackingOrder, ui);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the Stacking Order Scalar to the HDF5 File");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  QString s = Ebsd::StackingOrder::Utils::getStringForEnum(m_RefFrameZDir);
  err = QH5Lite::writeStringAttribute(fileId, Ebsd::H5::StackingOrder, "Name", s);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the Stacking Order Name Attribute to the HDF5 File");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::SampleTransformationAngle, m_SampleTransformationAngle);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the Sample Transformation Angle to the HDF5 File");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  QVector<hsize_t> dims(1, 3);
  err = QH5Lite::writeVectorDataset(fileId, Ebsd::H5::SampleTransformationAxis, dims, m_SampleTransformationAxis);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the Sample Transformation Axis to the HDF5 File");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::EulerTransformationAngle, m_EulerTransformationAngle);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the Euler Transformation Angle to the HDF5 File");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  err = QH5Lite::writeVectorDataset(fileId, Ebsd::H5::EulerTransformationAxis, dims, m_EulerTransformationAxis);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the Euler Transformation Axis to the HDF5 File");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  EbsdImporter::Pointer fileImporter;

  // Write the Manufacturer of the OIM file here
  // This list will grow to be the number of EBSD file formats we support
  QFileInfo fiExt(m_EbsdFileList.front());
  QString ext = fiExt.suffix();
  if(ext.compare(Ebsd::Ang::FileExt) == 0)
  {
    err = QH5Lite::writeStringDataset(fileId, Ebsd::H5::Manufacturer, Ebsd::Ang::Manufacturer);
    if(err < 0)
    {

      ss = QObject::tr("Could not write the Manufacturer Data to the HDF5 File");
      addErrorMessage(getHumanLabel(), ss, err);
      setErrorCondition(-1);
    }
    fileImporter = H5AngImporter::New();
  }
  else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
  {
    err = QH5Lite::writeStringDataset(fileId, Ebsd::H5::Manufacturer, Ebsd::Ctf::Manufacturer);
    if(err < 0)
    {

      ss = QObject::tr("Could not write the Manufacturer Data to the HDF5 File");
      addErrorMessage(getHumanLabel(), ss, err);
      setErrorCondition(-1);
    }
    fileImporter = H5CtfImporter::New();
  }
  else if(ext.compare(Ebsd::Mic::FileExt) == 0)
  {
    err = QH5Lite::writeStringDataset(fileId, Ebsd::H5::Manufacturer, Ebsd::Mic::Manufacturer);
    if(err < 0)
    {

      ss = QObject::tr("Could not write the Manufacturer Data to the HDF5 File");
      addErrorMessage(getHumanLabel(), ss, err);
      setErrorCondition(-1);
    }
    fileImporter = H5MicImporter::New();
  }
  else
  {
    err = -1;

    ss = QObject::tr("The File extension was not detected correctly");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
    return;
  }

  QVector<int> indices;
  // Loop on Each EBSD File
  float total = static_cast<float>( m_ZEndIndex - m_ZStartIndex );
  int progress = 0;
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
  int totalSlicesImported = 0;
  for (QVector<QString>::iterator filepath = m_EbsdFileList.begin(); filepath != m_EbsdFileList.end(); ++filepath)
  {
    QString ebsdFName = *filepath;
    progress = static_cast<int>( z - m_ZStartIndex );
    progress = (int)(100.0f * (float)(progress) / total);
    QString msg = "Converting File: " + ebsdFName;


    notifyStatusMessage(msg.toLatin1().data());
    err = fileImporter->importFile(fileId, z, ebsdFName);
    if (err < 0)
    {
      addErrorMessage(getHumanLabel(), fileImporter->getPipelineMessage(), fileImporter->getErrorCondition());
      setErrorCondition(err);
      return;
    }
    totalSlicesImported = totalSlicesImported + fileImporter->numberOfSlicesImported();

    fileImporter->getDims(xDim, yDim);
    fileImporter->getResolution(xRes, yRes);
    if(xDim > biggestxDim) { biggestxDim = xDim; }
    if(yDim > biggestyDim) { biggestyDim = yDim; }

    if(err < 0)
    {

      ss = QObject::tr("Could not write dataset for slice to HDF5 file");
      addErrorMessage(getHumanLabel(), ss, err);
      setErrorCondition(-1);
    }
    indices.push_back( static_cast<int>(z) );
    ++z;
    if(getCancel() == true)
    {
      notifyStatusMessage("Conversion was Canceled");
      return;
    }
  }

  // Write Z index start, Z index end and Z Resolution to the HDF5 file
  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::ZStartIndex, m_ZStartIndex);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the Z Start Index Scalar to the HDF5 File");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  m_ZEndIndex = m_ZStartIndex + totalSlicesImported - 1;
  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::ZEndIndex, m_ZEndIndex);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the Z End Index Scalar to the HDF5 File");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::XPoints, biggestxDim);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the XPoints Scalar to HDF5 file");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::YPoints, biggestyDim);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the YPoints Scalar to HDF5 file");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::XResolution, xRes);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the XResolution Scalar to HDF5 file");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  err = QH5Lite::writeScalarDataset(fileId, Ebsd::H5::YResolution, yRes);
  if(err < 0)
  {

    ss = QObject::tr("Could not write the YResolution Scalar to HDF5 file");
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  if(false == getCancel())
  {
    // Write an Index data set which contains all the z index values which
    // should help speed up the reading side of this file
    QVector<hsize_t> dims(1, indices.size());
    err = QH5Lite::writeVectorDataset(fileId, Ebsd::H5::Index, dims, indices);
  }
  err = QH5Utilities::closeFile(fileId);
  fileId = -1;
  notifyStatusMessage("Import Complete");
}

