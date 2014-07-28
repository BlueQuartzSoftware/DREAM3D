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
#include "LoadAdditiveMonitoringData.h"

#include <QtCore/QtDebug>
#include <fstream>

#include "H5Support/QH5Utilities.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DREAM3DLib/Utilities/FilePathGenerator.h"

#include "IO/IOConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadAdditiveMonitoringData::LoadAdditiveMonitoringData() :
  m_VolumeDataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CurrentArrayName(DREAM3D::CellData::Current),
  m_SpeedArrayName(DREAM3D::CellData::Speed),
  m_Current(NULL),
  m_Speed(NULL),
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_InputPath(),
  m_FilePrefix(),
  m_FileSuffix(),
  m_FileExtension("txt"),
  m_PaddingDigits(1),
  m_RefFrameZDir(Ebsd::RefFrameZDir::LowtoHigh),
  m_Stack(0), // this is just a dummy variable,
  m_NumCols(0),
  m_NumRows(0),
  m_HeaderIsComplete(false)
{
  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Resolution.x = 1.0;
  m_Resolution.y = 1.0;
  m_Resolution.z = 1.0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadAdditiveMonitoringData::~LoadAdditiveMonitoringData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadAdditiveMonitoringData::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Load ARCAM Process Monitoring Data", "Stack", FilterParameterWidgetType::ProcessMonitoringWidget, getStack(), false));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Volume Data Container", "VolumeDataContainerName", FilterParameterWidgetType::StringWidget, getVolumeDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Current", "CurrentArrayName", FilterParameterWidgetType::StringWidget, getCurrentArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Speed", "SpeedArrayName", FilterParameterWidgetType::StringWidget, getSpeedArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadAdditiveMonitoringData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVolumeDataContainerName(reader->readString("VolumeDataContainerName", getVolumeDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setCurrentArrayName(reader->readString("CurrentArrayName", getCurrentArrayName() ) );
  setSpeedArrayName(reader->readString("SpeedArrayName", getSpeedArrayName() ) );
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex() ) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin()) );
  setResolution( reader->readFloatVec3("Resolution", getResolution()) );
  setPaddingDigits( reader->readValue("PaddingDigits", getPaddingDigits()) );
  setRefFrameZDir( reader->readValue("RefFrameZDir", getRefFrameZDir()) );
  setInputPath( reader->readString("InputPath", getInputPath()) );
  setFilePrefix( reader->readString("FilePrefix", getFilePrefix()) );
  setFileSuffix( reader->readString("FileSuffix", getFileSuffix()) );
  setFileExtension( reader->readString("FileExtension", getFileExtension()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LoadAdditiveMonitoringData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(VolumeDataContainerName)
  writer->writeValue("CellAttributeMatrixName", getCellAttributeMatrixName() ) ;
  DREAM3D_FILTER_WRITE_PARAMETER(CurrentArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SpeedArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(ZStartIndex)
  DREAM3D_FILTER_WRITE_PARAMETER(ZEndIndex)
  DREAM3D_FILTER_WRITE_PARAMETER(PaddingDigits)
  DREAM3D_FILTER_WRITE_PARAMETER(RefFrameZDir)
  DREAM3D_FILTER_WRITE_PARAMETER(InputPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FilePrefix)
  DREAM3D_FILTER_WRITE_PARAMETER(FileSuffix)
  DREAM3D_FILTER_WRITE_PARAMETER(FileExtension)
  DREAM3D_FILTER_WRITE_PARAMETER(Origin)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadAdditiveMonitoringData::updateCellInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_CurrentPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Current = m_CurrentPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_SpeedPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Speed = m_SpeedPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadAdditiveMonitoringData::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  QString ss;

  if(m_InputPath.isEmpty() == true)
  {
    ss = QObject::tr("The Input Directory must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -13);
    setErrorCondition(-13);
  }

  bool hasMissingFiles = false;
  bool stackLowToHigh = false;

  if( Ebsd::RefFrameZDir::LowtoHigh == m_RefFrameZDir) { stackLowToHigh = true; }
  else if (Ebsd::RefFrameZDir::HightoLow == m_RefFrameZDir) { stackLowToHigh = false; }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_ZStartIndex, m_ZEndIndex,
                              hasMissingFiles, stackLowToHigh, m_InputPath,
                              m_FilePrefix, m_FileSuffix, m_FileExtension,
                              m_PaddingDigits);
  if (fileList.size() == 0)
  {
    QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getVolumeDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims (1, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  tempPath.update(getVolumeDataContainerName(), getCellAttributeMatrixName(), getCurrentArrayName() );
  m_CurrentPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CurrentPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Current = m_CurrentPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  tempPath.update(getVolumeDataContainerName(), getCellAttributeMatrixName(), getSpeedArrayName() );
  m_SpeedPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SpeedPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Speed = m_SpeedPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadAdditiveMonitoringData::preflight()
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
void LoadAdditiveMonitoringData::execute()
{

  herr_t err = 0;

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  bool hasMissingFiles = false;
  bool stackLowToHigh = false;

  if( Ebsd::RefFrameZDir::LowtoHigh == m_RefFrameZDir) { stackLowToHigh = true; }
  else if (Ebsd::RefFrameZDir::HightoLow == m_RefFrameZDir) { stackLowToHigh = false; }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_ZStartIndex, m_ZEndIndex,
                              hasMissingFiles, stackLowToHigh, m_InputPath,
                              m_FilePrefix, m_FileSuffix, m_FileExtension,
                              m_PaddingDigits);


  QVector<int> indices;
  // Loop on Each File
  float total = static_cast<float>( m_ZEndIndex - m_ZStartIndex );
  int progress = 0;
  size_t zSlot = 0;
  int64_t z = m_ZStartIndex;
  /* There is a frailness about the z index and the file list. The programmer
   * using this code MUST ensure that the list of files that is sent into this
   * class is in the appropriate order to match up with the z index (slice index)
   * otherwise the import will have subtle errors. The programmer is urged NOT to
   * simply gather a list from the file system as those lists are sorted in such
   * a way that if the number of digits appearing in the filename are NOT the same
   * then the list will be wrong, ie, this example:
   *
   * slice_1.txt
   * slice_2.txt
   * ....
   * slice_10.txt
   *
   * Most, if not ALL C++ libraries when asked for that list will return the list
   * sorted like the following:
   *
   * slice_1.txt
   * slice_10.txt
   * slice_2.txt
   *
   * which could cause problems. YOU HAVE BEEN WARNED.
   */
  // int totalSlicesImported = 0;
  for (QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
  {
    zSlot = (z - m_ZStartIndex);

    QString FName = *filepath;
    {
      progress = static_cast<int>( z - m_ZStartIndex );
      progress = (int)(100.0f * (float)(progress) / total);
      QString msg = "Converting File: " + FName;
      notifyStatusMessage(getHumanLabel(), msg.toLatin1().data());
    }
    if (getCancel() == true)
    {
      break;
    }

    QFile in(FName);
    if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QString msg = QObject::tr("File could not be opened: %1").arg(FName);
      notifyStatusMessage(getHumanLabel(), msg.toLatin1().data());
    }

    float x = -1.0f, y = -1.0f, current = 0.0f, speed = 0.0f;
    float minX = 100000000.0;
    float minY = 100000000.0;
    float maxX = 0.0;
    float maxY = 0.0;
    size_t xSlot=0, ySlot = 0;

    size_t count = 2;
    QByteArray buf;
    QList<QByteArray> tokens; /* vector to store the split data */
    QList<QByteArray> tokens2; /* vector to store the split data */
    bool ok = false;
    buf = in.readLine();
    buf = in.readLine();
    while (!in.atEnd())
    {
      buf = in.readLine();
      tokens = buf.split(',');
      x = tokens[0].trimmed().toFloat(&ok);
      y = tokens[1].trimmed().toFloat(&ok);
      current = tokens[2].trimmed().toFloat(&ok);
      speed = tokens[3].trimmed().toFloat(&ok);
      if(x < minX) minX = x;
      if(x > maxX) maxX = x;
      if(y < minY) minY = y;
      if(y > maxY) maxY = y;
      count++;
    }
    in.close();
    
    size_t numXpoints = ((maxX-m_Origin.x)/m_Resolution.x) + 1;
    size_t numYpoints = ((maxY-m_Origin.y)/m_Resolution.y) + 1;
    size_t totalPoints = numXpoints*numYpoints*size_t(fileList.size());


    VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getVolumeDataContainerName());
    m->setDimensions(static_cast<size_t>(numXpoints), static_cast<size_t>(numYpoints), static_cast<size_t>(fileList.size()));
    m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
    m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
    AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
    QVector<size_t> tDims(3, 0);
    tDims[0] = numXpoints;
    tDims[1] = numYpoints;
    tDims[2] = size_t(fileList.size());
    cellAttrMat->resizeAttributeArrays(tDims);
    updateCellInstancePointers();

    in.open(QIODevice::ReadOnly | QIODevice::Text);
    buf = in.readLine();
    buf = in.readLine();
    for (size_t i = 2; i < count; i++)
    {
      buf = in.readLine();
      size_t offset = 0;
      tokens = buf.split(',');
      x = tokens[0].trimmed().toFloat(&ok);
      y = tokens[1].trimmed().toFloat(&ok);
      current = tokens[2].trimmed().toFloat(&ok);
      speed = tokens[3].trimmed().toFloat(&ok);

      xSlot = x/m_Resolution.x;
      ySlot = y/m_Resolution.y;

      offset = (zSlot*numXpoints*numYpoints)+(ySlot*numXpoints)+xSlot;

      m_Current[offset] = current;
      m_Speed[offset] = speed;
    }
    z++;
  }
  notifyStatusMessage(getHumanLabel(), "Import Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer LoadAdditiveMonitoringData::newFilterInstance(bool copyFilterParameters)
{
  LoadAdditiveMonitoringData::Pointer filter = LoadAdditiveMonitoringData::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    // We are going to hand copy all of the parameters because the other way of copying the parameters are going to
    // miss some of them because we are not enumerating all of them.
    DREAM3D_COPY_INSTANCEVAR(ZStartIndex)
    DREAM3D_COPY_INSTANCEVAR(ZEndIndex)
    DREAM3D_COPY_INSTANCEVAR(Resolution)
    DREAM3D_COPY_INSTANCEVAR(Origin)
    DREAM3D_COPY_INSTANCEVAR(InputPath)
    DREAM3D_COPY_INSTANCEVAR(FilePrefix)
    DREAM3D_COPY_INSTANCEVAR(FileSuffix)
    DREAM3D_COPY_INSTANCEVAR(FileExtension)
    DREAM3D_COPY_INSTANCEVAR(PaddingDigits)
    DREAM3D_COPY_INSTANCEVAR(Stack)
    DREAM3D_COPY_INSTANCEVAR(VolumeDataContainerName)
    DREAM3D_COPY_INSTANCEVAR(CellAttributeMatrixName)
    DREAM3D_COPY_INSTANCEVAR(CurrentArrayName)
    DREAM3D_COPY_INSTANCEVAR(SpeedArrayName)
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LoadAdditiveMonitoringData::getCompiledLibraryName()
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LoadAdditiveMonitoringData::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LoadAdditiveMonitoringData::getHumanLabel()
{ return "Load ARCAM Process Monitoring Data"; }

