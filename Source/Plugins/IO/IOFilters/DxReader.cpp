/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "DxReader.h"

#include <QtCore/QtDebug>
#include <fstream>

#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>

#include "IO/IOConstants.h"


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

	size_t m_XDim_Cache;
	size_t m_YDim_Cache;
	size_t m_ZDim_Cache;
	QString m_InputFile_Cache;
	QDateTime m_LastRead;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReaderPrivate::DxReaderPrivate(DxReader* ptr) :
q_ptr(ptr),
m_InputFile_Cache(""),
m_LastRead(QDateTime()),
m_XDim_Cache(0),
m_YDim_Cache(0),
m_ZDim_Cache(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReader::DxReader() :
  FileReader(),
  m_VolumeDataContainerName(DREAM3D::Defaults::DataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_InputFile(""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_FileWasRead(false),
  d_ptr(new DxReaderPrivate(this))
{
  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Resolution.x = 1.0;
  m_Resolution.y = 1.0;
  m_Resolution.z = 1.0;

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
DREAM3D_PIMPL_PROPERTY_DEF(DxReader, size_t, XDim_Cache)
DREAM3D_PIMPL_PROPERTY_DEF(DxReader, size_t, YDim_Cache)
DREAM3D_PIMPL_PROPERTY_DEF(DxReader, size_t, ZDim_Cache)
DREAM3D_PIMPL_PROPERTY_DEF(DxReader, QString, InputFile_Cache)
DREAM3D_PIMPL_PROPERTY_DEF(DxReader, QDateTime, LastRead)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Input File", "InputFile", FilterParameterWidgetType::InputFileWidget, getInputFile(), false, "", "*.dx"));
  parameters.push_back(FilterParameter::New("Origin", "Origin", FilterParameterWidgetType::FloatVec3Widget, getOrigin(), false, "XYZ"));
  parameters.push_back(FilterParameter::New("Resolution", "Resolution", FilterParameterWidgetType::FloatVec3Widget, getResolution(), false, "XYZ"));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Volume Data Container", "VolumeDataContainerName", FilterParameterWidgetType::StringWidget, getVolumeDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureIdsArrayName(), true, ""));
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
int DxReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(VolumeDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(InputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(Origin)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
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
	setXDim_Cache(0);
	setYDim_Cache(0);
	setZDim_Cache(0);
	setLastRead(QDateTime());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVolumeDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  m->setGeometry(image);

  QFileInfo fi(getInputFile());

  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist.");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  QVector<size_t> dims(1, 1);
  tempPath.update(getVolumeDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
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

	  QString lastRead = getLastRead().toString();
	  bool lastReadValid = getLastRead().isValid();
	  qint64 secs = lastModified.msecsTo(getLastRead());

	  if (getInputFile() == getInputFile_Cache() && getLastRead().isValid() && lastModified.msecsTo(getLastRead()) >= 0)
	  {
		  // We are reading from the cache, so set the FileWasRead flag to false
		  m_FileWasRead = false;

		  m->getGeometryAs<ImageGeom>()->setDimensions(getXDim_Cache(), getYDim_Cache(), getZDim_Cache());
	  }
	  else
	  {
		  // We are reading from the file, so set the FileWasRead flag to true
		  m_FileWasRead = true;

		  // We need to read the header of the input file to get the dimensions
		  m_InStream.setFileName(getInputFile());
		  if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
		  {
			  QString ss = QObject::tr("DxReader Input file could not be opened: %1").arg(getInputFile());
			  setErrorCondition(-100);
			  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
			  return;
		  }

		  int error = readHeader();
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
  int err = 0;

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  m_InStream.setFileName(getInputFile());
  if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("DxReader Input file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  err = readHeader();
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
int DxReader::readHeader()
{
  QString ss;
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());

  int error = 0;

  QByteArray buf;
  QString delimeters(", ;\t"); /* delimeters to split the data */
  QList<QByteArray> tokens; /* vector to store the split data */

  bool ok = false;
  // Process the header information and look for the QString "counts"
  // Then read the data size after that
  int nx = 0;
  int ny = 0;
  int nz = 0;
  bool done = false;
  while (m_InStream.atEnd() == false && done == false)
  {
    buf = m_InStream.readLine();
    buf = buf.trimmed();
    tokens = buf.split(' ');
    // continue until we find the keyword
    for (qint32 i = 0; i < tokens.size(); i++)
    {
      if(tokens[i] == "counts")
      {
        nz = tokens[i + 1].toInt(&ok, 10) - 1;
        ny = tokens[i + 2].toInt(&ok, 10) - 1;
        nx = tokens[i + 3].toInt(&ok, 10) - 1;
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
        ss = QObject::tr("ERROR: Unable to locate the last header line");
        notifyErrorMessage(getHumanLabel(), ss, -8);
        setErrorCondition(-496);
        m_InStream.close();
        return -496;
      }
    }
  } // when we get here, we are looking at data
  //int points = 0;
  if(pos1 != 0)
  {
    error = 0;
    // points = tokens[pos1 + 1].toInt(&ok, 10);
  }

  // Set the values into the cache, so that they can be used later
  setXDim_Cache(nx);
  setYDim_Cache(ny);
  setZDim_Cache(nz);

  m->getGeometryAs<ImageGeom>()->setDimensions(nx, ny, nz);
  //  qDebug() << "Compare no. points " << points << " with x*y*z: " << nx * ny * nz ;

  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxReader::readFile()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());

  QString line;
  QString delimeters(", ;\t"); /* delimeters to split the data */

  bool finished_header, finished_data;
  finished_header = true;
  finished_data = false;
  size_t index = 0;

  // Reseize the Cell Attribute Matrix based on the number of points about to be read.
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

  while (buf.size() > 0 && m_InStream.atEnd() == false)
  {
    // Get the remaining lines of the header and ignore
    buf = buf.simplified();
    QList<QByteArray> tokens = buf.split(' ');

    int64_t total = m->getGeometryAs<ImageGeom>()->getNumberOfTuples();
    if( index == total || ( finished_header && tokens.size() != 0 && tokens[0] == "attribute") )
    {
      finished_data = true;
    }

    // Allocate the DataArray at this point:
    if(finished_header && !finished_data)
    {
      qint32 size = tokens.size();
      for (int32_t in_spins = 0; in_spins < size; in_spins++)
      {
        int fId = tokens[in_spins].toInt(&ok, 10);
        m_FeatureIds[index] = fId;
        ++index;
      }

    }
    buf = m_InStream.readLine();
  }

  if(index != static_cast<size_t>(m->getGeometryAs<ImageGeom>()->getNumberOfTuples()))
  {
    QString ss = QObject::tr("ERROR: data size does not match header dimensions\t%1\t%2").arg(index).arg(m->getGeometryAs<ImageGeom>()->getNumberOfTuples());
    setErrorCondition(-495);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    m_InStream.close();
    return getErrorCondition();
  }

  m_InStream.close();


  // Find the unique set of feature ids
  //  QSet<int32_t> featureIdSet;
  //  for (int64_t i = 0; i < totalPoints; ++i)
  //  {
  //    featureIdSet.insert(m_FeatureIds[i]);
  //  }
  //  for (QSet<int32_t>::iterator iter = featureIdSet.begin(); iter != featureIdSet.end(); ++iter )
  //  {
  //    qDebug() << "Feature ID: " << (*iter) ;
  //  }

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
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxReader::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxReader::getSubGroupName()
{ return DREAM3D::FilterSubGroups::InputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxReader::getHumanLabel()
{ return "Read Dx File (Feature Ids)"; }

