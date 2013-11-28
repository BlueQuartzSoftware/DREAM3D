/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "H5FilterParametersReader.h"

#include <QtCore/QStringList>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"


#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "DREAM3DLib/FilterParameters/H5FilterParametersConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5FilterParametersReader::H5FilterParametersReader() :
  AbstractFilterParametersReader(),
  m_PipelineGroupId(-1)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5FilterParametersReader::~H5FilterParametersReader()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5FilterParametersReader::Pointer H5FilterParametersReader::OpenDREAM3DFileForReadingPipeline(QString filePath, hid_t& fid)
{
  fid = -1;
  fid = QH5Utilities::openFile(filePath);
  if(fid < 0) { return H5FilterParametersReader::NullPointer(); }

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();
  hid_t pipelineGroupId = H5Gopen(fid, DREAM3D::HDF5::PipelineGroupName.toLatin1().data(), H5P_DEFAULT);
  if (pipelineGroupId < 0)
  {
    H5Fclose(fid);
    fid = -1;
    return H5FilterParametersReader::NullPointer();
  }
  reader->setPipelineGroupId(pipelineGroupId);
  return reader;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer H5FilterParametersReader::ReadPipelineFromFile(QString filePath)
{
  hid_t fid = -1;
  fid = QH5Utilities::openFile(filePath);
  if(fid < 0) { return FilterPipeline::NullPointer(); }

  HDF5ScopedFileSentinel sentinel(&fid, true);


  // Open the Pipeline Group
  hid_t pipelineGroupId = H5Gopen(fid, DREAM3D::HDF5::PipelineGroupName.toLatin1().data(), H5P_DEFAULT);
  if (pipelineGroupId < 0)
  {
    H5Fclose(fid);
    fid = -1;
    return FilterPipeline::NullPointer();
  }

  sentinel.addGroupId(&pipelineGroupId);
  // Use QH5Lite to ask how many "groups" are in the "Pipeline Group"
  QList<QString> groupList;
  herr_t err = QH5Utilities::getGroupObjects(pipelineGroupId, H5Utilities::H5Support_GROUP, groupList);


  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();
  reader->setPipelineGroupId(pipelineGroupId);

  // Get a FilterManager Instance
  FilterManager::Pointer filterManager = FilterManager::Instance();

  // Create a FilterPipeline Object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  // Loop over the items getting the "ClassName" attribute from each group
  QString classNameStr = "";
  for (int i = 0; i < groupList.size(); i++)
  {

    QString iStr = QString::number(i);
    err = QH5Lite::readStringAttribute(pipelineGroupId, iStr, "ClassName", classNameStr);

    // Instantiate a new filter using the FilterFactory based on the value of the className attribute
    IFilterFactory::Pointer ff = filterManager->getFactoryForFilter(classNameStr);
    if (NULL != ff)
    {
      AbstractFilter::Pointer filter = ff->create();
      if(NULL != ff)
      {
        // Read the parameters
        filter->readFilterParameters( reader.get(), i );

        // Add filter to pipeline
        pipeline->pushBack(filter);
      }
    }
  }

  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersReader::openFilterGroup(AbstractFilter* filter, int index)
{
  int err = 0;
  if (m_PipelineGroupId <= 0)
  {
    return -1;
  }

  QString name = QString::number(index);
  m_CurrentGroupId = H5Gopen(m_PipelineGroupId, name.toLatin1().data(), H5P_DEFAULT);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersReader::closeFilterGroup()
{
  H5Gclose(m_CurrentGroupId);
  m_CurrentGroupId = -1;
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString H5FilterParametersReader::readString(const QString name, QString value)
{
  QString defaultStr = value;
  value.clear();
  int err = 0;
  err = QH5Lite::readStringDataset(m_CurrentGroupId, name, value);
  if (err == 0)
  { return value; }
  else
  { return defaultStr; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> H5FilterParametersReader::readStrings(const QString name, QVector<QString> value)
{
  int vectorSize = 0;
  QString str = "";
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readStringAttribute(m_CurrentGroupId, name, ss, str);
    value.push_back(str);
  }

  return value;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int8_t H5FilterParametersReader::readValue(const QString name, int8_t value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int16_t H5FilterParametersReader::readValue(const QString name, int16_t value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t H5FilterParametersReader::readValue(const QString name, int32_t value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t H5FilterParametersReader::readValue(const QString name, int64_t value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t H5FilterParametersReader::readValue(const QString name, uint8_t value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint16_t H5FilterParametersReader::readValue(const QString name, uint16_t value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t H5FilterParametersReader::readValue(const QString name, uint32_t value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t H5FilterParametersReader::readValue(const QString name, uint64_t value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float H5FilterParametersReader::readValue(const QString name, float value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double H5FilterParametersReader::readValue(const QString name, double value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool H5FilterParametersReader::readValue(const QString name, bool value)
{
  QH5Lite::readScalarDataset(m_CurrentGroupId, name, value);
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int8_t> H5FilterParametersReader::readArray(const QString name, QVector<int8_t> value)
{
  int vectorSize = 0;
  int8_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int16_t> H5FilterParametersReader::readArray(const QString name, QVector<int16_t> value)
{
  int vectorSize = 0;
  int16_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int32_t> H5FilterParametersReader::readArray(const QString name, QVector<int32_t> value)
{
  int vectorSize = 0;
  int32_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int64_t> H5FilterParametersReader::readArray(const QString name, QVector<int64_t> value)
{
  int vectorSize = 0;
  int64_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint8_t> H5FilterParametersReader::readArray(const QString name, QVector<uint8_t> value)
{
  int vectorSize = 0;
  uint8_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint16_t> H5FilterParametersReader::readArray(const QString name, QVector<uint16_t> value)
{
  int vectorSize = 0;
  uint16_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint32_t> H5FilterParametersReader::readArray(const QString name, QVector<uint32_t> value)
{
  int vectorSize = 0;
  uint32_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint64_t> H5FilterParametersReader::readArray(const QString name, QVector<uint64_t> value)
{
  int vectorSize = 0;
  uint64_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> H5FilterParametersReader::readArray(const QString name, QVector<float> value)
{
  int vectorSize = 0;
  float scalar = 0.0f;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> H5FilterParametersReader::readArray(const QString name, QVector<double> value)
{
  int vectorSize = 0;
  double scalar = 0.0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  if (err < 0) { return value; }
  else { value.clear();}
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss, scalar);
    value.push_back(scalar);
  }

  return value;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Widget_t H5FilterParametersReader::readIntVec3(const QString name, IntVec3Widget_t defaultValue)
{
  int err = 0;
  IntVec3Widget_t v;
  err = QH5Lite::readPointerDataset<int32_t>(m_CurrentGroupId, name, reinterpret_cast<int32_t*>(&v) );
  if (err < 0) { return defaultValue; }

  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Widget_t H5FilterParametersReader::readFloatVec3(const QString name, FloatVec3Widget_t defaultValue)
{
  int err = 0;
  FloatVec3Widget_t v;
  err = QH5Lite::readPointerDataset<float>(m_CurrentGroupId, name, reinterpret_cast<float*>(&v) );
  if (err < 0) { return defaultValue; }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInput_t H5FilterParametersReader::readComparisonInput(const QString name, ComparisonInput_t defaultValue, int vectorPos)
{
  QVector<ComparisonInput_t> comps(1, defaultValue);
  QVector<ComparisonInput_t> values = readComparisonInputs(name, comps);
  if(values.size() >= 1)
  {
    return values[0];
  }
  return defaultValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<ComparisonInput_t> H5FilterParametersReader::readComparisonInputs(const QString name, QVector<ComparisonInput_t> defValue)
{
  int size = 0;
  QString strData = "";
  bool ok = false;
  // See if the data set actually exists, if it does NOT just return what the user passed in as a default value
  if(false == QH5Lite::datasetExists(m_CurrentGroupId, name) ) { return defValue; }

  herr_t err = QH5Lite::readStringDataset(m_CurrentGroupId, name, strData);
  if(err < 0) { return defValue; } // If the data set does not exist no point in going any further

  // Now read the the attribute that says how many arrays are in the data set.
  err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, "NumInputs", size);

  QStringList strVector = strData.split('\n', QString::SkipEmptyParts);
  qint32 strVecSize = strVector.size();
  if (strVecSize != size)
  {
    // Something has gone wrong in the tokenization and the number of tokens does not match what
    // was written to the HDF5 file.
    return defValue;
  }

  QVector<ComparisonInput_t> values(size);
  for(qint32 i = 0; i < size; ++i)
  {
    ComparisonInput_t& v = values[i];
    QStringList tokens = strVector[i].split(',', QString::SkipEmptyParts);
    if(tokens.size() >= 1)
    {
      v.arrayName = tokens[0];
    }
    if(tokens.size() >= 2)
    {
      v.compOperator = QString(tokens[1]).toInt(&ok, 10);
    }
    if(tokens.size() >= 3)
    {
      v.compValue = QString(tokens[2]).toDouble(&ok);
    }
  }
  return values;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleInput_t H5FilterParametersReader::readAxisAngle(const QString name, AxisAngleInput_t v, int vectorPos)
{
  int err = 0;

  QString ss = QString::number(vectorPos) + H5FilterParameter::AxisAngleInput;
  err = QH5Lite::readPointerAttribute<float>(m_CurrentGroupId, name, ss, reinterpret_cast<float*>(&v) );

  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AxisAngleInput_t> H5FilterParametersReader::readAxisAngles(const QString name, QVector<AxisAngleInput_t> v)
{
  QVector<AxisAngleInput_t> axisAngleInputsVector;
  AxisAngleInput_t axisAngleDummyInput;
  axisAngleDummyInput.angle = 0.0f;
  axisAngleDummyInput.h = 0.0f;
  axisAngleDummyInput.k = 0.0f;
  axisAngleDummyInput.l = 0.0f;
  int vectorSize = static_cast<int>( readValue(name, 0) );
  for(int i = 0; i < vectorSize; i++)
  {
    axisAngleInputsVector.push_back( readAxisAngle(name, axisAngleDummyInput, i) );
  }
  return axisAngleInputsVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> H5FilterParametersReader::readArraySelections(const QString name, QSet<QString> v)
{
  int size = 0;
  QString strData = "";

  // See if the data set actually exists, if it does NOT just return what the user passed in as a default value
  if(false == QH5Lite::datasetExists(m_CurrentGroupId, name) ) { return v; }

  herr_t err = QH5Lite::readStringDataset(m_CurrentGroupId, name, strData);
  if(err < 0) { return v; } // If the data set does not exist no point in going any further

  // Now read the the attribute that says how many arrays are in the data set.
  err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, "NumArrays", size);

  QStringList strVector = strData.split('\n', QString::SkipEmptyParts);
  qint32 strVecSize = strVector.size();
  if (strVecSize != size)
  {
    // Something has gone wrong in the tokenization and the number of tokens does not match what
    // was written to the HDF5 file.
    return v;
  }
  for (int i = 0; i < strVector.size(); i++)
  {
    v.insert(strVector[i]);
  }
  return v;
}

