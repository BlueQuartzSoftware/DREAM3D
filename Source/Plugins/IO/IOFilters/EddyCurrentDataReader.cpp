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


#include "EddyCurrentDataReader.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include <QtCore/QFileInfo>

#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/DataArrays/StringDataArray.hpp"

#include "IO/IOConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EddyCurrentDataReader::EddyCurrentDataReader() :
  AbstractFilter(),
  m_VolumeDataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_MetaDataAttributeMatrixName(DREAM3D::StringConstants::MetaData),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_HasMultipleValues("False"),
  m_AverageMultipleValues(false),
  m_InputFile("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EddyCurrentDataReader::~EddyCurrentDataReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EddyCurrentDataReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Input File", "InputFile", FilterParameterWidgetType::InputFileWidget, getInputFile(), false, "", "*.*"));
  parameters.push_back(FilterParameter::New("Has Multiple Values", "HasMultipleValues", FilterParameterWidgetType::PreflightUpdatedValueWidget, getHasMultipleValues(), false, ""));
  parameters.back()->setReadOnly(true);
  parameters.push_back(FilterParameter::New("Average Multiple Values", "AverageMultipleValues", FilterParameterWidgetType::BooleanWidget, getAverageMultipleValues(), false, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Volume Data Container", "VolumeDataContainerName", FilterParameterWidgetType::StringWidget, getVolumeDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Meta Data Attribute Matrix", "MetaDataAttributeMatrixName", FilterParameterWidgetType::StringWidget, getMetaDataAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EddyCurrentDataReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVolumeDataContainerName(reader->readString("VolumeDataContainerName", getVolumeDataContainerName() ) );
  setMetaDataAttributeMatrixName(reader->readString("MetaDataAttributeMatrixName", getMetaDataAttributeMatrixName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EddyCurrentDataReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(VolumeDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(MetaDataAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(InputFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EddyCurrentDataReader::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getVolumeDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }
  tDims.resize(1);
  tDims[0] = 1;
  AttributeMatrix::Pointer metaAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getMetaDataAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::MetaData);
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EddyCurrentDataReader::preflight()
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
QString EddyCurrentDataReader::getHasMultipleValues()
{
  return m_HasMultipleValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void shuffleArray(IDataArray::Pointer inputData, IDataArray::Pointer inputDataCopy, size_t m_DataPointCount, size_t xDim, size_t yDim)
{
  DataArray<T>* inArray = DataArray<T>::SafePointerDownCast(inputData.get());
  DataArray<T>* copyArray = DataArray<T>::SafePointerDownCast(inputDataCopy.get());
  if (NULL == inArray || copyArray == NULL)
  {
    return;
  }

  T* inPtr = inArray->getPointer(0);
  T* copyPtr = copyArray->getPointer(0);

  for(size_t i = 0; i < m_DataPointCount; i++)
  {
    size_t row = i%yDim;
    size_t col = i/yDim;
    size_t newPos = (row*xDim) + col;
    copyPtr[newPos] = inPtr[i];
  }
 }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EddyCurrentDataReader::execute()
{
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  m_InStream.open(QIODevice::ReadOnly | QIODevice::Text);
  readFile(m_InStream);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EddyCurrentDataReader::readHeader(QFile &reader)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getVolumeDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  AttributeMatrix::Pointer metaAttrMat = m->getAttributeMatrix(getMetaDataAttributeMatrixName());

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

  bool ok = true;
  int32_t startFreq = 0;
  int32_t stopFreq = 0;
  int32_t numPtsSweep = 0;
  bool keepGoing = true;
  QList<QByteArray> words;
  QVector<size_t> tDims(1, 1);
  QVector<size_t> cDims(1, 1);
  while(keepGoing == true)
  {
    buf = m_InStream.readLine();
    buf = buf.trimmed();
    buf = buf.simplified();
    words = buf.split(',');
    if(words.size() > 1) keepGoing = false;
    if(words.size() == 1)
    {
      words = buf.split(':');
      if(words.size() == 2)
      {
        QString word(words.at(0));
        if(word.compare("Comments") != 0)
        {
          //do some comparisons to get the values of the start and stop frequencies and the number of points in the swwep
          if(word.compare("Start Freq (Hz)") == 0) startFreq = words[1].toInt(&ok);
          else if(word.compare("Stop Freq (Hz)") == 0) stopFreq = words[1].toInt(&ok);
          else if(word.compare("No. of Pts Per Sweep") == 0) numPtsSweep = words[1].toInt(&ok);
          Int32ArrayType::Pointer data = Int32ArrayType::CreateArray(tDims, cDims, word);
          metaAttrMat->addAttributeArray(word, data);
        }
        else
        {
          StringDataArray::Pointer dataS = StringDataArray::CreateArray(1, word);
          metaAttrMat->addAttributeArray(word, dataS);
        }
      }
    }
  }

  //do logic check to see if there are multiple values at the frequency
  if(startFreq == stopFreq && numPtsSweep > 1) m_HasMultipleValues = "True";
  else m_HasMultipleValues = "False";

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

  //resize the attribute matrix with the number of points (which we now know from reading the header
  //Note that the tDims should actually be a vector of 3 for the x, y and z dims....but this will atleast 
  //size the arrays to the right size and we will figure out the x, y and z dims later
  tDims[0] = m_DataPointCount;
  cellAttrMat->resizeAttributeArrays(tDims);

  // Make sure we did not have any errors
  if(getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Error reading header information from file: '%1'").arg(getInputFile());
    setErrorCondition(-389);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  QMapIterator<QString, GenericDataParser::Pointer> parserIter(m_NamePointerMap);
  while(parserIter.hasNext())
  {
    parserIter.next();
    QString name = parserIter.key();
    GenericDataParser::Pointer parser = parserIter.value();
    IDataArray::Pointer dataPtr = parser->initializeNewDataArray(m_DataPointCount, name, !getInPreflight()) ; // Get a copy of the DataArray
    cellAttrMat->addAttributeArray(dataPtr->getName(), dataPtr);
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EddyCurrentDataReader::readFile(QFile &reader)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getVolumeDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  AttributeMatrix::Pointer metaAttrMat = m->getAttributeMatrix(getMetaDataAttributeMatrixName());

  QByteArray buf;
  QList<QByteArray> tokens;
  QVector<QByteArray> values;
  QMapIterator<QString, GenericDataParser::Pointer> parserIter(m_NamePointerMap);

  QString name;
  GenericDataParser::Pointer parser;

  float x, y;
  float xMin = 100000000000.0;
  float yMin = 100000000000.0;
  float xMax = -100000000000.0;
  float yMax = -100000000000.0;
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
  bool keepGoing = true;
  QList<QByteArray> words;
  bool ok = true;
  while(keepGoing == true)
  {
    buf = m_InStream.readLine();
    buf = buf.trimmed();
    buf = buf.simplified();
    words = buf.split(',');
    if(words.size() > 1) keepGoing = false;
    if(words.size() == 1)
    {
      words = buf.split(':');
      if(words.size() == 2)
      {
        QString word(words[0]);
        IDataArray::Pointer dataPtr = metaAttrMat->getAttributeArray(word);
        if(word.compare("Comments") != 0)
        {
          Int32ArrayType::Pointer castDataPtr = boost::dynamic_pointer_cast<Int32ArrayType>(dataPtr);
          castDataPtr->setValue(0, words[1].toInt(&ok));
        }
        else
        {
          StringDataArray::Pointer castDataPtr = boost::dynamic_pointer_cast<StringDataArray>(dataPtr);
          castDataPtr->setValue(0, words[1]);
        }
      }
    }
  }

  //now begin reading the data....assuming the file is orderd with x incrementing fastest....we will correct this later if it is wrong
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

  bool xFastest = true;
  for(size_t i = 0; i < m_DataPointCount; i++)
  {
    x = xVal[i];
    y = yVal[i];
    if(x < xMin) xMin = x;
    if(y < yMin) yMin = y;
    if(x > xMax) xMax = x;
    if(y > yMax) yMax = y;
    //logic to determine if x or y increments fastest in file....x assumed to be fastest, so only check y change
    if(i == 1 && y != lastYcoord) xFastest = false;
    if(i == 0) xDim++, yDim++;
    if(i > 0 && xFastest == true)
    {
      if(x != lastXcoord)
      {
        xDim++;
      }
      if(y != lastYcoord)
      {
        yDim++;
        xDim = 1;
      }
    }
    else if(i > 0 && xFastest == false)
    {
      if(y != lastYcoord)
      {
        yDim++;
      }
      if(x != lastXcoord)
      {
        xDim++;
        yDim = 1;
      }
    }
    lastXcoord = x;
    lastYcoord = y;
  }

  //logic here assumes the max x and y coordinate is the last data point
  xStep = (xMax-xMin)/float(xDim-1);
  yStep = (yMax-yMin)/float(yDim-1);

  m->setDimensions(xDim, yDim, 1);
  m->setResolution(xStep, yStep, ((xStep+yStep)/2.0));
  m->setOrigin(xMin, yMin, 0.0);

  QVector<size_t> tDims(3,0);
  tDims[0] = xDim;
  tDims[1] = yDim;
  tDims[2] = 1;
  cellAttrMat->resizeAttributeArrays(tDims);

  if(xFastest == false)
  {
    //get the attribute matrix the data was added to
    AttributeMatrix::Pointer attrMat = getDataContainerArray()->getDataContainer(getVolumeDataContainerName())->getAttributeMatrix(getCellAttributeMatrixName());

    // Get all the names of the arrays added
    QList<QString> headers = attrMat->getAttributeArrayNames();


    //loop through the list of arrays...walking all the points for each one and reordering them
    for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
    {
      IDataArray::Pointer p = attrMat->getAttributeArray(*iter);
      IDataArray::Pointer pCopy = p->deepCopy();
      QString dType = p->getTypeAsString();
      if (dType.compare("int8_t") == 0)
      {
        shuffleArray<int8_t>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("uint8_t") == 0)
      {
        shuffleArray<uint8_t>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("int16_t") == 0)
      {
        shuffleArray<int16_t>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("uint16_t") == 0)
      {
        shuffleArray<uint16_t>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("int32_t") == 0)
      {
        shuffleArray<int32_t>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("uint32_t") == 0)
      {
        shuffleArray<uint32_t>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("int64_t") == 0)
      {
        shuffleArray<int64_t>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("uint64_t") == 0)
      {
        shuffleArray<uint64_t>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("float") == 0)
      {
        shuffleArray<float>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("double") == 0)
      {
        shuffleArray<double>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      else if (dType.compare("bool") == 0)
      {
        shuffleArray<bool>(p, pCopy, m_DataPointCount, xDim, yDim);
      }
      attrMat->addAttributeArray(p->getName(), pCopy);
    }
  }

  m_InStream.close();

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EddyCurrentDataReader::newFilterInstance(bool copyFilterParameters)
{
  EddyCurrentDataReader::Pointer filter = EddyCurrentDataReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EddyCurrentDataReader::getCompiledLibraryName()
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EddyCurrentDataReader::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EddyCurrentDataReader::getSubGroupName()
{ return DREAM3D::FilterSubGroups::InputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EddyCurrentDataReader::getHumanLabel()
{ return "Read Eddy Current Data"; }

