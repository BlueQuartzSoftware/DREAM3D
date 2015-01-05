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


#include "UniWestEddyCurrentReader.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include <QtCore/QFileInfo>

#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "IO/IOConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UniWestEddyCurrentReader::UniWestEddyCurrentReader() :
  AbstractFilter(),
  m_VolumeDataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_InputFile("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UniWestEddyCurrentReader::~UniWestEddyCurrentReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UniWestEddyCurrentReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Input File", "InputFile", FilterParameterWidgetType::InputFileWidget, getInputFile(), false, "", "*.*"));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Volume Data Container", "VolumeDataContainerName", FilterParameterWidgetType::StringWidget, getVolumeDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UniWestEddyCurrentReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVolumeDataContainerName(reader->readString("VolumeDataContainerName", getVolumeDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UniWestEddyCurrentReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(VolumeDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(InputFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UniWestEddyCurrentReader::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getVolumeDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  m_InStream.setFileName(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (m_InStream.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Input file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //read the header to create the necessary arrays
  readHeader(m_InStream);
  m_InStream.close();

  //resize the attribute matrix with the number of points (which we now know from reading the header
  //Note that the tDims should actually be a vector of 3 for the x, y and z dims....but this will atleast 
  //size the arrays to the right size and we will figure out the x, y and z dims later
  tDims.resize(1);
  tDims[0] = m_DataPointCount;
  attrMat->resizeAttributeArrays(tDims);

  // Make sure we did not have any errors
  if(getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Error reading header information from file: '%1'").arg(getInputFile());
    setErrorCondition(-389);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QMapIterator<QString, GenericDataParser::Pointer> parserIter(m_NamePointerMap);
  while(parserIter.hasNext())
  {
    parserIter.next();
    QString name = parserIter.key();
    GenericDataParser::Pointer parser = parserIter.value();
    IDataArray::Pointer dataPtr = parser->initializeNewDataArray(m_DataPointCount, name, !getInPreflight()) ; // Get a copy of the DataArray
    attrMat->addAttributeArray(dataPtr->getName(), dataPtr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UniWestEddyCurrentReader::preflight()
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
void UniWestEddyCurrentReader::execute()
{
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  m_InStream.open(QIODevice::ReadOnly | QIODevice::Text);
  readFile(m_InStream);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UniWestEddyCurrentReader::readHeader(QFile &reader)
{
  m_InStream.setFileName(getInputFile());
  if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Input file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */

  for(int i = 0; i < 13; i++)
  {
    buf = m_InStream.readLine();
  }
  buf = buf.trimmed();
  buf = buf.simplified();
  QList<QByteArray> words = buf.split(',');

  for(int i=0;i<words.size();i++)
  {
    FloatArrayType::Pointer data = FloatArrayType::CreateArray(0, words[i], false);
    GenericFloatParser::Pointer dparser = GenericFloatParser::New(data, words[i], i);
    m_NamePointerMap.insert(words[i], dparser);
  }

  m_DataPointCount = 0;
  while(!m_InStream.atEnd())
  {
    buf = m_InStream.readLine();
    m_DataPointCount++;
  }

  //rerun the parser with the proper number of dataPoints
  for(int i=0;i<words.size();i++)
  {
    FloatArrayType::Pointer data = FloatArrayType::CreateArray(m_DataPointCount, words[i], false);
    GenericFloatParser::Pointer dparser = GenericFloatParser::New(data, words[i], i);
    m_NamePointerMap.insert(words[i], dparser);
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UniWestEddyCurrentReader::readFile(QFile &reader)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getVolumeDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  QByteArray buf;
  QList<QByteArray> tokens;
  QVector<QByteArray> values;
  QMapIterator<QString, GenericDataParser::Pointer> parserIter(m_NamePointerMap);

  QString name;
  GenericDataParser::Pointer parser;

  float x, y;
  float xMin = 100000000000.0;
  float yMin = 100000000000.0;
  float xStep = 0.0;
  float yStep = 0.0;
  size_t xDim = 0;
  size_t yDim = 0;
  float lastXcoord = -1000000000000.0;
  float lastYcoord = -1000000000000.0;

  while(parserIter.hasNext())
  {
    parserIter.next();
    QString name = parserIter.key();
    GenericDataParser::Pointer parser = parserIter.value();
    IDataArray::Pointer data = cellAttrMat->getAttributeArray(name);
    if(data->isAllocated() == false)
    {
      qDebug() << name << " is NOT allocated";
    }
    parser->setDataArray(data);
  }
  parserIter.toFront();

  //reader header lines first
  for(int i = 0; i < 13; i++)
  {
    buf = reader.readLine();
    tokens = buf.trimmed().simplified().split(',');
  }

  //now begin reading the data
  for(size_t i = 0; i < m_DataPointCount; i++)
  {
    tokens = reader.readLine().trimmed().simplified().split(',');
    QListIterator<QByteArray> iter(tokens);
    while(iter.hasNext())
    {
      values.push_back( iter.next() );
    }
    while(parserIter.hasNext())
    {
      parserIter.next();
      name = parserIter.key();
      parser = parserIter.value();
      parser->parse(values.at(parser->getColumnIndex()), i);
    }
    parserIter.toFront();
    values.clear();
  }

  FloatArrayType::Pointer xPtr = getDataContainerArray()->getDataContainer(getVolumeDataContainerName())->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayAs<FloatArrayType>("X (mm)");
  float* xVal = xPtr->getPointer(0);
  FloatArrayType::Pointer yPtr = getDataContainerArray()->getDataContainer(getVolumeDataContainerName())->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayAs<FloatArrayType>(" Y (mm)");
  float* yVal = yPtr->getPointer(0);
  for(size_t i = 0; i < m_DataPointCount; i++)
  {
    x = xVal[i];
    y = yVal[i];
    if(x < xMin) xMin = x;
    if(y < yMin) yMin = y;
    //logic here assumes x increments fastest in file
    if(x != lastXcoord)
    {
      lastXcoord = x;
      xDim++;
    }
    if(y != lastYcoord)
    {
      lastYcoord = y;
      yDim++;
      xDim = 1;
    }
  }

  //logic here assumes the max x and y coordinate is the last data point
  xStep = (x-xMin)/float(xDim-1);
  yStep = (y-yMin)/float(yDim-1);

  m->setDimensions(xDim, yDim, 1);
  m->setResolution(xStep, yStep, ((xStep+yStep)/2.0));
  m->setOrigin(xMin, yMin, 0.0);

  QVector<size_t> tDims(3,0);
  tDims[0] = xDim;
  tDims[1] = yDim;
  tDims[2] = 1;
  cellAttrMat->resizeAttributeArrays(tDims);

  m_InStream.close();

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer UniWestEddyCurrentReader::newFilterInstance(bool copyFilterParameters)
{
  UniWestEddyCurrentReader::Pointer filter = UniWestEddyCurrentReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString UniWestEddyCurrentReader::getCompiledLibraryName()
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString UniWestEddyCurrentReader::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString UniWestEddyCurrentReader::getSubGroupName()
{ return DREAM3D::FilterSubGroups::InputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString UniWestEddyCurrentReader::getHumanLabel()
{ return "Read UniWest Eddy Current File"; }

