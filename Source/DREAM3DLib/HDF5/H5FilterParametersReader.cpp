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



#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/HDF5/H5FilterParametersConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5FilterParametersReader::H5FilterParametersReader()
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
int H5FilterParametersReader::openFilterGroup(AbstractFilter* filter, int index)
{
  int err = 0;
  if (m_GroupId <= 0)
  {
    return -1;
  }

  QString name = QString::number(index);
  m_CurrentGroupId = H5Gopen(m_GroupId, name.c_str(), H5P_DEFAULT);
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
QString H5FilterParametersReader::readValue(const QString name, QString value)
{
  QString defaultStr = value;
  value.clear();
  int err = 0;
  err = QH5Lite::readStringDataset(m_CurrentGroupId, name, value);
  if (err == 0)
    return value;
  else
    return defaultStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<QString> H5FilterParametersReader::readValue(const QString name, std::vector<QString> value)
{
  int vectorSize = 0;
  QString str = "";
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readStringAttribute(m_CurrentGroupId, name, ss.str(), str);
    value.push_back(str);
    ss.clear();
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
std::vector<int8_t> H5FilterParametersReader::readValue(const QString name, std::vector<int8_t> value)
{
  int vectorSize = 0;
  int8_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<int16_t> H5FilterParametersReader::readValue(const QString name, std::vector<int16_t> value)
{
  int vectorSize = 0;
  int16_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<int32_t> H5FilterParametersReader::readValue(const QString name, std::vector<int32_t> value)
{
  int vectorSize = 0;
  int32_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<int64_t> H5FilterParametersReader::readValue(const QString name, std::vector<int64_t> value)
{
  int vectorSize = 0;
  int64_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<uint8_t> H5FilterParametersReader::readValue(const QString name, std::vector<uint8_t> value)
{
  int vectorSize = 0;
  uint8_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<uint16_t> H5FilterParametersReader::readValue(const QString name, std::vector<uint16_t> value)
{
  int vectorSize = 0;
  uint16_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<uint32_t> H5FilterParametersReader::readValue(const QString name, std::vector<uint32_t> value)
{
  int vectorSize = 0;
  uint32_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<uint64_t> H5FilterParametersReader::readValue(const QString name, std::vector<uint64_t> value)
{
  int vectorSize = 0;
  uint64_t scalar = 0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> H5FilterParametersReader::readValue(const QString name, std::vector<float> value)
{
  int vectorSize = 0;
  float scalar = 0.0f;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> H5FilterParametersReader::readValue(const QString name, std::vector<double> value)
{
  int vectorSize = 0;
  double scalar = 0.0;
  int err = QH5Lite::readScalarDataset(m_CurrentGroupId, name, vectorSize);
  QString ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, ss.str(), scalar);
    value.push_back(scalar);
    ss.clear();
  }

  return value;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Widget_t H5FilterParametersReader::readValue(const QString name, IntVec3Widget_t defaultValue)
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
FloatVec3Widget_t H5FilterParametersReader::readValue(const QString name, FloatVec3Widget_t defaultValue)
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
ComparisonInput_t H5FilterParametersReader::readValue(const QString name, ComparisonInput_t defaultValue, int vectorPos)
{
  int err = 0;
  ComparisonInput_t v;

  QString ss;
  ss << vectorPos <<  H5FilterParameter::ComparisonInput;

  QString data;

  err = QH5Lite::readStringAttribute(m_CurrentGroupId, name, ss.str(), data);
  if(err < 0)
  {
    return defaultValue;
  }

  std::vector<QString> tokens = tokenize(data.c_str(), '\n');
  if(tokens.size() >= 1)
  {
    v.arrayName = tokens[0];
  }
  if(tokens.size() >= 2)
  {
    StringUtils::stringToNum(v.compOperator, tokens[1]);
  }
  if(tokens.size() >= 3)
  {
    StringUtils::stringToNum(v.compValue, tokens[2]);
  }

  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<ComparisonInput_t> H5FilterParametersReader::readValue(const QString name, std::vector<ComparisonInput_t> v)
{
  std::vector<ComparisonInput_t> comparisons;
  ComparisonInput_t cellComparisonDefault;
  int numQFilters = static_cast<int>( readValue(name, 0) );
  for(int i = 0; i < numQFilters; i++)
  {
    comparisons.push_back( readValue(name, cellComparisonDefault, i) );
  }
  return comparisons;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleInput_t H5FilterParametersReader::readValue(const QString name, AxisAngleInput_t v, int vectorPos)
{
  int err = 0;
  //  int32_t rank = 1;
  //  hsize_t dims[1] = { 4 };

  QString ss;
  ss << vectorPos <<  H5FilterParameter::AxisAngleInput;

  err = QH5Lite::readPointerAttribute<float>(m_CurrentGroupId, name, ss.str(), reinterpret_cast<float*>(&v) );

  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<AxisAngleInput_t> H5FilterParametersReader::readValue(const QString name, std::vector<AxisAngleInput_t> v)
{
  std::vector<AxisAngleInput_t> axisAngleInputsVector;
  AxisAngleInput_t axisAngleDummyInput;
  axisAngleDummyInput.angle = 0.0f;
  axisAngleDummyInput.h = 0.0f;
  axisAngleDummyInput.k = 0.0f;
  axisAngleDummyInput.l = 0.0f;
  int vectorSize = static_cast<int>( readValue(name, 0) );
  for(int i = 0; i < vectorSize; i++)
  {
    axisAngleInputsVector.push_back( readValue(name, axisAngleDummyInput, i) );
  }
  return axisAngleInputsVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> H5FilterParametersReader::readValue(const QString name, QSet<QString> v)
{
  size_t size = 0;
  QString strData = "";

  herr_t err = QH5Lite::readStringDataset(m_CurrentGroupId, name, strData);
  if(err < 0) { return v; } // If the data set does not exist no point in going any further

  // Now read the the attribute that says how many arrays are in the data set.
  err = QH5Lite::readScalarAttribute(m_CurrentGroupId, name, "NumArrays", size);

  std::vector<QString> strVector = tokenize(strData.c_str(), '\n');

  if (strVector.size() != size)
  {
    // Something has gone wrong in the tokenization and the number of tokens does not match what
    // was written to the HDF5 file.
    return v;
  }
  for (int i=0; i<strVector.size(); i++)
  {
    v.insert(strVector[i]);
  }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<QString> H5FilterParametersReader::tokenize(const char* buf, char delimiter)
{
  std::vector<QString> output;
  QString values(buf);
  QString::size_type start = 0;
  QString::size_type pos = 0;
  //  qDebug() << "-----------------------------" ;
  while(pos != QString::npos && pos != values.size() - 1)
  {
    pos = values.find(delimiter, start);
    output.push_back(values.substr(start, pos-start));
    //   qDebug() << "Adding: " << output.back() ;
    if (pos != QString::npos)
    {
      start = pos + 1;
    }
  }
  return output;
}
