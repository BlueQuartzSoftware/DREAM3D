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

#include "DxReader.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"


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
    QVector<size_t> m_Dims;
    QString m_InputFile_Cache;
    QDateTime m_LastRead;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReaderPrivate::DxReaderPrivate(DxReader* ptr) :
  q_ptr(ptr),
  m_Dims(),
  m_InputFile_Cache(""),
  m_LastRead()
{
}

// Include the MOC generated file for this class
#include "moc_DxReader.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReader::DxReader() :
  FileReader(),
  m_VolumeDataContainerName(SIMPL::Defaults::ImageDataContainerName),
  m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName),
  m_InputFile(""),
  m_FileWasRead(false),
  m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds),
  d_ptr(new DxReaderPrivate(this)),
  m_FeatureIds(NULL)
{
  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

  m_Dims[0] = 0;
  m_Dims[1] = 0;
  m_Dims[2] = 0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReader::~DxReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL_PIMPL_PROPERTY_DEF(DxReader, QVector<size_t>, Dims)
SIMPL_PIMPL_PROPERTY_DEF(DxReader, QString, InputFile_Cache)
SIMPL_PIMPL_PROPERTY_DEF(DxReader, QDateTime, LastRead)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, DxReader, "*.dx"));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, DxReader));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Resolution", Resolution, FilterParameter::Parameter, DxReader));

  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", VolumeDataContainerName, FilterParameter::CreatedArray, DxReader));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, DxReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Feature Ids", FeatureIdsArrayName, FilterParameter::CreatedArray, DxReader));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void DxReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVolumeDataContainerName(reader->readString("VolumeDataContainerName", getVolumeDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::updateCellInstancePointers()
{
  setErrorCondition(0);
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::flushCache()
{
  setInputFile_Cache("");
  QVector<size_t> v;
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
  setErrorCondition(0);
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVolumeDataContainerName());
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> tDims(3, 0);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  QFileInfo fi(getInputFile());

  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  QVector<size_t> cDims(1, 1);
  tempPath.update(getVolumeDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  if (m_InStream.isOpen() == true)
  {
    m_InStream.close();
  }

  if (getInputFile().isEmpty() == false && fi.exists() == true)
  {
    QDateTime lastModified(fi.lastModified());

    if (getInputFile() == getInputFile_Cache() && getLastRead().isValid() && lastModified.msecsTo(getLastRead()) >= 0)
    {
      // We are reading from the cache, so set the FileWasRead flag to false
      m_FileWasRead = false;
      QVector<size_t> v = getDims();
      m->getGeometryAs<ImageGeom>()->setDimensions(v[0], v[1], v[2]);
    }
    else
    {
      // We are reading from the file, so set the FileWasRead flag to true
      m_FileWasRead = true;

      // We need to read the header of the input file to get the dimensions
      m_InStream.setFileName(getInputFile());
      if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
      {
        QString ss = QObject::tr("Error opening input file: %1").arg(getInputFile());
        setErrorCondition(-100);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      int32_t error = readHeader();
      m_InStream.close();
      if (error < 0)
      {
        setErrorCondition(error);
        QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file. Is the input file a Dx file?");
        notifyErrorMessage(getHumanLabel(), ss, -11000);
      }

      // Set the file path and time stamp into the cache
      setLastRead(QDateTime::currentDateTime());
      setInputFile_Cache(getInputFile());
    }
  }
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
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  m_InStream.setFileName(getInputFile());
  if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  err = readHeader();
  if (err < 0)
  {
    m_InStream.close();
    return;
  }
  err = readFile();
  m_InStream.close();
  if (err < 0)
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
  while (m_InStream.atEnd() == false && done == false)
  {
    buf = m_InStream.readLine();
    buf = buf.trimmed();
    tokens = buf.split(' ');
    // continue until we find the keyword
    for (qint32 i = 0; i < tokens.size(); i++)
    {
      if (tokens[i] == "counts")
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

  //The DX file has a unique format of 20 entries on each line. I have
  //no idea who initiated this insanity but I am about to perpetuate
  //it.
  //
  //The most simple thing to do is to read the entire dataset into one
  //long vector and then read that vector to assign values to the grid

  //  ADR:  6 Sep 08; time to make the input much more general!
  //  equivalent to list-direcvted input in Fortran, actually !!

  qint32 pos1 = 0;
  while (pos1 == 0 && m_InStream.atEnd() == false)
  {
    // continue until we find the keyword
    buf = m_InStream.readLine();
    buf = buf.simplified();
    tokens = buf.split(' ');
    for (qint32 i = 0; i < tokens.size(); i++)
    {
      if (tokens[i] == "items")
      {
        pos1 = i;
      }
    }
    // Read the next line of the header if we did not find the keyword
    // in the line
    if (pos1 == 0)
    {
      if (tokens.size() == 20)
      {
        ss = QObject::tr("Unable to locate the last header line");
        setErrorCondition(-8);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        m_InStream.close();
        return getErrorCondition();
      }
    }
  } // when we get here, we are looking at data
  if (pos1 != 0)
  {
    error = 0;
  }

  // Set the values into the cache, so that they can be used later
  QVector<size_t> v;
  v.push_back(nx);
  v.push_back(ny);
  v.push_back(nz);

  setDims(v);

  m->getGeometryAs<ImageGeom>()->setDimensions(nx, ny, nz);
  //  qDebug() << "Compare no. points " << points << " with x*y*z: " << nx * ny * nz ;

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
  QVector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateCellInstancePointers();

  if (getErrorCondition() < 0)
  {
    m_InStream.close();
    return -1;
  }

  bool ok = false;
  QByteArray buf = m_InStream.readLine();

  size_t xIdx = 0, yIdx = 0, zIdx = 0;
  size_t count = 0;

  while (buf.size() > 0 && m_InStream.atEnd() == false)
  {
    // Get the remaining lines of the header and ignore
    buf = buf.simplified();
    QList<QByteArray> tokens = buf.split(' ');

    size_t total = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
    if (count == total || (finished_header && tokens.size() != 0 && tokens[0] == "attribute"))
    {
      finished_data = true;
    }

    if (finished_header && !finished_data)
    {
      qint32 size = tokens.size();
      for (qint32 in_spins = 0; in_spins < size; in_spins++)
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

  if (count != static_cast<size_t>(m->getGeometryAs<ImageGeom>()->getNumberOfElements()))
  {
    QString ss = QObject::tr("Data size does not match header dimensions\t%1\t%2").arg(index).arg(m->getGeometryAs<ImageGeom>()->getNumberOfElements());
    setErrorCondition(-495);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    m_InStream.close();
    return getErrorCondition();
  }

  m_InStream.close();

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DxReader::newFilterInstance(bool copyFilterParameters)
{
  DxReader::Pointer filter = DxReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxReader::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxReader::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxReader::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxReader::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxReader::getSubGroupName()
{ return SIMPL::FilterSubGroups::InputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxReader::getHumanLabel()
{ return "Read Dx File (Feature Ids)"; }
