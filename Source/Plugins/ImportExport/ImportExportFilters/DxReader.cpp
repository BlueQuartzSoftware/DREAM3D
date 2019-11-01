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

#include "DxReader.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

#include <QtCore/QTextStream>

#include <QtCore/QDebug>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID31 = 31,

  DataContainerID = 1
};

/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DxReaderPrivate
{
  Q_DISABLE_COPY(DxReaderPrivate)
  Q_DECLARE_PUBLIC(DxReader)
  DxReader* const q_ptr;
  DxReaderPrivate(DxReader* ptr);
  std::vector<size_t> m_Dims;
  QString m_InputFile_Cache;
  QDateTime m_LastRead;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReaderPrivate::DxReaderPrivate(DxReader* ptr)
: q_ptr(ptr)
, m_InputFile_Cache("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReader::DxReader()
: m_VolumeDataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_InputFile("")
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, d_ptr(new DxReaderPrivate(this))
{
  m_Origin[0] = 0.0f;
  m_Origin[1] = 0.0f;
  m_Origin[2] = 0.0f;

  m_Spacing[0] = 1.0f;
  m_Spacing[1] = 1.0f;
  m_Spacing[2] = 1.0f;

  m_Dims[0] = 0;
  m_Dims[1] = 0;
  m_Dims[2] = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReader::~DxReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void DxReader::setDims(const std::vector<size_t>& value)
{
  Q_D(DxReader);
  d->m_Dims = value;
}

// -----------------------------------------------------------------------------
std::vector<size_t> DxReader::getDims() const
{
  Q_D(const DxReader);
  return d->m_Dims;
}

// -----------------------------------------------------------------------------
void DxReader::setInputFile_Cache(const QString& value)
{
  Q_D(DxReader);
  d->m_InputFile_Cache = value;
}

// -----------------------------------------------------------------------------
QString DxReader::getInputFile_Cache() const
{
  Q_D(const DxReader);
  return d->m_InputFile_Cache;
}

// -----------------------------------------------------------------------------
void DxReader::setLastRead(const QDateTime& value)
{
  Q_D(DxReader);
  d->m_LastRead = value;
}

// -----------------------------------------------------------------------------
QDateTime DxReader::getLastRead() const
{
  Q_D(const DxReader);
  return d->m_LastRead;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::setupFilterParameters()
{
  FileReader::setupFilterParameters();

  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, DxReader, "*.dx"));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, DxReader));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, DxReader));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", VolumeDataContainerName, FilterParameter::CreatedArray, DxReader));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, VolumeDataContainerName, FilterParameter::CreatedArray, DxReader));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Cell Feature Ids", FeatureIdsArrayName, VolumeDataContainerName, CellAttributeMatrixName, FilterParameter::CreatedArray, DxReader));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void DxReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVolumeDataContainerName(reader->readDataArrayPath("VolumeDataContainerName", getVolumeDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setSpacing(reader->readFloatVec3("Spacing", getSpacing()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::updateCellInstancePointers()
{
  clearErrorCode();
  clearWarningCode();
  if(nullptr != m_FeatureIdsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::flushCache()
{
  setInputFile_Cache("");
  std::vector<size_t> v;
  v.push_back(0);
  v.push_back(0);
  v.push_back(0);
  setDims(v);
  setLastRead(QDateTime());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::initialize()
{
  if(m_InStream.isOpen())
  {
    m_InStream.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVolumeDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  QFileInfo fi(getInputFile());

  if(getInputFile().isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: %1").arg(getInputFile());
    setErrorCondition(-388, ss);
  }

  if(m_InStream.isOpen())
  {
    m_InStream.close();
  }

  if(!getInputFile().isEmpty() && fi.exists())
  {
    QDateTime lastModified(fi.lastModified());

    if(getInputFile() == getInputFile_Cache() && getLastRead().isValid() && lastModified.msecsTo(getLastRead()) >= 0)
    {
      // We are reading from the cache, so set the FileWasRead flag to false
      m_FileWasRead = false;
      std::vector<size_t> v = getDims();
      ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
      if(nullptr != imageGeom.get())
      {
        imageGeom->setDimensions(v.data());
      }
    }
    else
    {
      // We are reading from the file, so set the FileWasRead flag to true
      m_FileWasRead = true;

      // We need to read the header of the input file to get the dimensions
      m_InStream.setFileName(getInputFile());
      if(!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
      {
        QString ss = QObject::tr("Error opening input file: %1").arg(getInputFile());
        setErrorCondition(-100, ss);
        return;
      }

      int32_t error = readHeader();
      m_InStream.close();
      if(error < 0)
      {
        QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file. Is the input file a Dx file?");
        setErrorCondition(error, ss);
      }

      // Set the file path and time stamp into the cache
      setLastRead(QDateTime::currentDateTime());
      setInputFile_Cache(getInputFile());
    }
  }

  std::vector<size_t> tDims = getDims();
  m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> cDims(1, 1);
  tempPath.update(getVolumeDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims, "", DataArrayID31);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  m->getGeometryAs<ImageGeom>()->setSpacing(std::make_tuple(m_Spacing[0], m_Spacing[1], m_Spacing[2]));
  m->getGeometryAs<ImageGeom>()->setOrigin(std::make_tuple(m_Origin[0], m_Origin[1], m_Origin[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::preflight()
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
void DxReader::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  m_InStream.setFileName(getInputFile());
  if(!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
    setErrorCondition(-100, ss);
    return;
  }

  int32_t err = readHeader();
  if(err < 0)
  {
    m_InStream.close();
    return;
  }
  err = readFile();
  m_InStream.close();
  if(err < 0)
  {
    return;
  }
}

//-----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t DxReader::readHeader()
{
  QString ss;
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());

  int32_t error = 0;

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */

  bool ok = false;
  // Process the header information and look for the QString "counts"
  // Then read the data size after that
  size_t nx = 0;
  size_t ny = 0;
  size_t nz = 0;
  bool done = false;
  while(!m_InStream.atEnd() && !done)
  {
    buf = m_InStream.readLine();
    buf = buf.trimmed();
    tokens = buf.split(' ');
    // continue until we find the keyword
    for(qint32 i = 0; i < tokens.size(); i++)
    {
      if(tokens[i] == "counts")
      {
        nz = tokens[i + 1].toInt(&ok, 10);
        ny = tokens[i + 2].toInt(&ok, 10);
        nx = tokens[i + 3].toInt(&ok, 10);
        done = true;
        break;
      }
    }
  }

  //  qDebug() << "INFO: DX data dimensions: " ;
  //  qDebug() << "nz= " << nz ;
  //  qDebug() << "ny= " << ny ;
  //  qDebug() << "nx= " << nx ;

  // The DX file has a unique format of 20 entries on each line. I have
  // no idea who initiated this insanity but I am about to perpetuate
  // it.
  //
  // The most simple thing to do is to read the entire dataset into one
  // long vector and then read that vector to assign values to the grid

  //  ADR:  6 Sep 08; time to make the input much more general!
  //  equivalent to list-direcvted input in Fortran, actually !!

  qint32 pos1 = 0;
  while(pos1 == 0 && !m_InStream.atEnd())
  {
    // continue until we find the keyword
    buf = m_InStream.readLine();
    buf = buf.simplified();
    tokens = buf.split(' ');
    for(qint32 i = 0; i < tokens.size(); i++)
    {
      if(tokens[i] == "items")
      {
        pos1 = i;
      }
    }
    // Read the next line of the header if we did not find the keyword
    // in the line
    if(pos1 == 0)
    {
      if(tokens.size() == 20)
      {
        ss = QObject::tr("Unable to locate the last header line");
        setErrorCondition(-8, ss);
        m_InStream.close();
        return getErrorCode();
      }
    }
  } // when we get here, we are looking at data
  if(pos1 != 0)
  {
    error = 0;
  }

  // Set the values into the cache, so that they can be used later
  std::vector<size_t> v;
  v.push_back(nx);
  v.push_back(ny);
  v.push_back(nz);

  setDims(v);

  if(nullptr != m.get())
  {
    ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
    if(nullptr != imageGeom.get())
    {
      imageGeom->setDimensions(v.data());
    }
  }

  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t DxReader::readFile()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());

  bool finished_header = true;
  bool finished_data = false;
  size_t index = 0;

  // Resize the Cell Attribute Matrix based on the number of points about to be read.
  std::vector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateCellInstancePointers();

  if(getErrorCode() < 0)
  {
    m_InStream.close();
    return -1;
  }

  bool ok = false;
  QByteArray buf = m_InStream.readLine();

  size_t xIdx = 0, yIdx = 0, zIdx = 0;
  size_t count = 0;

  while(buf.size() > 0 && !m_InStream.atEnd())
  {
    // Get the remaining lines of the header and ignore
    buf = buf.simplified();
    QList<QByteArray> tokens = buf.split(' ');

    size_t total = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
    if(count == total || (finished_header && !tokens.empty() && tokens[0] == "attribute"))
    {
      finished_data = true;
    }

    if(finished_header && !finished_data)
    {
      qint32 size = tokens.size();
      for(qint32 in_spins = 0; in_spins < size; in_spins++)
      {
        int32_t fId = tokens[in_spins].toInt(&ok, 10);
        count++;
        index = (zIdx * tDims[0] * tDims[1]) + (tDims[0] * yIdx) + xIdx;

        m_FeatureIds[index] = fId;

        zIdx++; // Increment zIdx and then check all the indices and wrap back to zero if needed.

        if(zIdx == tDims[2])
        {
          zIdx = 0;
          yIdx++;
        }

        if(yIdx == tDims[1])
        {
          yIdx = 0;
          xIdx++;
        }

        if(xIdx == tDims[0])
        {
          xIdx = 0;
          Q_ASSERT(count == total);
        }
      }
    }
    buf = m_InStream.readLine();
  }

  if(count != static_cast<size_t>(m->getGeometryAs<ImageGeom>()->getNumberOfElements()))
  {
    QString ss = QObject::tr("Data size does not match header dimensions\t%1\t%2").arg(index).arg(m->getGeometryAs<ImageGeom>()->getNumberOfElements());
    setErrorCondition(-495, ss);
    m_InStream.close();
    return getErrorCode();
  }

  m_InStream.close();

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DxReader::newFilterInstance(bool copyFilterParameters) const
{
  DxReader::Pointer filter = DxReader::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DxReader::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DxReader::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DxReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DxReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid DxReader::getUuid() const
{
  return QUuid("{ba2238f8-a20f-5f2f-ac9f-43ed458460f7}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DxReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DxReader::getHumanLabel() const
{
  return "Import Dx File (Feature Ids)";
}

// -----------------------------------------------------------------------------
DxReader::Pointer DxReader::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<DxReader> DxReader::New()
{
  struct make_shared_enabler : public DxReader
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString DxReader::getNameOfClass() const
{
  return QString("DxReader");
}

// -----------------------------------------------------------------------------
QString DxReader::ClassName()
{
  return QString("DxReader");
}

// -----------------------------------------------------------------------------
void DxReader::setVolumeDataContainerName(const DataArrayPath& value)
{
  m_VolumeDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath DxReader::getVolumeDataContainerName() const
{
  return m_VolumeDataContainerName;
}

// -----------------------------------------------------------------------------
void DxReader::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString DxReader::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void DxReader::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString DxReader::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void DxReader::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type DxReader::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void DxReader::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type DxReader::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
void DxReader::setFileWasRead(bool value)
{
  m_FileWasRead = value;
}

// -----------------------------------------------------------------------------
bool DxReader::getFileWasRead() const
{
  return m_FileWasRead;
}

// -----------------------------------------------------------------------------
void DxReader::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString DxReader::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}
