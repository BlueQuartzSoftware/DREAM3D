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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxReader::DxReader() :
  FileReader(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_InputFile(""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
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
void DxReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setFileExtension("*.dx");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Origin");
    option->setPropertyName("Origin");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Resolution");
    option->setPropertyName("Resolution");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("XYZ");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void DxReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("Resolution", getResolution() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(getDataContainerName(), false, NULL);
  if(NULL == m)
  {
    m = getDataContainerArray()->createDataContainerWithAttributeMatrix<VolumeDataContainer>(getDataContainerName(), getCellAttributeMatrixName() );
  }

  QFileInfo fi(getInputFile());

  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist.");
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = attrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, 0, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  if (m_InStream.isOpen() == true)
  {
    m_InStream.close();
  }

  if (getInputFile().isEmpty() == false && fi.exists() == true)
  {
    // We need to read the header of the input file to get the dimensions
    m_InStream.setFileName(getInputFile());
    if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QString ss = QObject::tr("DxReader Input file could not be opened: %1").arg(getInputFile());
      setErrorCondition(-100);
      notifyErrorMessage(ss, getErrorCondition());
      return;
    }

    int error = readHeader();
    m_InStream.close();
    if (error < 0)
    {
      setErrorCondition(error);
      QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file. Is the input file a Dx file?");
      addErrorMessage(getHumanLabel(), ss, -11000);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxReader::execute()
{
  int err = 0;

  dataCheck(false, 0, 0, 0);


  m_InStream.setFileName(getInputFile());
  if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("DxReader Input file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(ss, getErrorCondition());
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

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
        addErrorMessage(getHumanLabel(), ss, -8);
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
  m->setDimensions(nx, ny, nz);
  //  qDebug() << "Compare no. points " << points << " with x*y*z: " << nx * ny * nz ;
  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxReader::readFile()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QString line;
  QString delimeters(", ;\t"); /* delimeters to split the data */

  int error; /* dummy variables */

  bool finished_header, finished_data;
  finished_header = true;
  finished_data = false;
  size_t index = 0;

  int64_t totalPoints = m->getTotalPoints();

  // Remove the array that we are about to create first as a 'datacheck()' was called from the super class's 'execute'
  // method which is performed before this function. This will cause an error -501 because the array with the name
  // m_FeatureIdsArrayName already exists but of size 1, not the size we are going to read. So we get rid of the array
  m->getAttributeMatrix(getCellAttributeMatrixName())->removeAttributeArray(m_FeatureIdsArrayName);
  // Rerun the data check in order to allocate the array to store the data from the .dx file.
  //  int64_t totalPoints = m->getTotalPoints();
  //size_t totalFeatures = 0;
  //size_t totalEnsembles = 0;
  //dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = attrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, 0, totalPoints, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


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
    error = 0;
    buf = buf.simplified();
    QList<QByteArray> tokens = buf.split(' ');

    int64_t total = m->getTotalPoints();
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
        m_FeatureIds[index] = tokens[in_spins].toInt(&ok, 10);
        ++index;
      }

    }
    buf = m_InStream.readLine();
  }

  if(index != static_cast<size_t>(m->getTotalPoints()))
  {
    QString ss = QObject::tr("ERROR: data size does not match header dimensions\t%1\t%2").arg(index).arg(m->getTotalPoints());
    setErrorCondition(-495);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
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

  notifyStatusMessage("Complete");
  return 0;
}

