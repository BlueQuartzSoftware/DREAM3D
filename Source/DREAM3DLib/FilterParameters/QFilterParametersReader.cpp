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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "QFilterParametersReader.h"

#include <QtCore/QMetaType>
#include <QtCore/QDataStream>


// These Streams need to be implemented so that our 3 Element Vectors can be read/write to disk/prefs files
QDataStream& operator<<( QDataStream& out, const IntVec3Widget_t& v)
{
  out << v.x << v.y << v.z;
  return out;
}
QDataStream& operator>>( QDataStream& in, IntVec3Widget_t& v) { in >> v.x >> v.y >> v.z; return in; }

QDataStream& operator<<( QDataStream& out, const FloatVec3Widget_t& v) { out << v.x << v.y << v.z; return out; }
QDataStream& operator>>( QDataStream& in, FloatVec3Widget_t& v) { in >> v.x >> v.y >> v.z; return in; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterParametersReader::QFilterParametersReader() :
  m_Prefs(NULL)
{

  qRegisterMetaType<IntVec3Widget_t>("IntVec3Widget_t");
  qRegisterMetaTypeStreamOperators<IntVec3Widget_t>("IntVec3Widget_t");

  qRegisterMetaType<FloatVec3Widget_t>("FloatVec3Widget_t");
  qRegisterMetaTypeStreamOperators<FloatVec3Widget_t>("FloatVec3Widget_t");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterParametersReader::~QFilterParametersReader()
{
  if(NULL != m_Prefs)
  {
    closeFile();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSettings* QFilterParametersReader::getPrefs()
{
  return m_Prefs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterParametersReader::openFile(QString filename)
{
  if(NULL != m_Prefs)
  {
    closeFile();
  }
  m_Prefs = new QSettings(filename, QSettings::IniFormat);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterParametersReader::closeFile()
{
  delete m_Prefs;
  m_Prefs = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QFilterParametersReader::openFilterGroup(AbstractFilter* unused, int index)
{
  BOOST_ASSERT(m_Prefs != NULL);
  int err = 0;
  QString gName = QString::number(index);
  m_Prefs->beginGroup(gName);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QFilterParametersReader::closeFilterGroup()
{
  BOOST_ASSERT(m_Prefs != NULL);
  m_Prefs->endGroup();
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterParametersReader::readString(const QString name, QString value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  return m_Prefs->value(name, value).toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> QFilterParametersReader::readStrings(const QString name, QVector<QString> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  int count = m_Prefs->beginReadArray(name);
  QVector<QString> selections;
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    QString str = m_Prefs->value(name, QString::fromUtf8("NOT_FOUND")).toString();
    selections.push_back(str);
  }
  m_Prefs->endArray();
  return value;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int8_t QFilterParametersReader::readValue(const QString name, int8_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int8_t def = value;
  value = m_Prefs->value(name, def).toInt(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int16_t QFilterParametersReader::readValue(const QString name, int16_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int16_t def = value;
  value = m_Prefs->value(name, def).toInt(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t QFilterParametersReader::readValue(const QString name, int32_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int32_t def = value;
  value = m_Prefs->value(name, def).toInt(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t QFilterParametersReader::readValue(const QString name, int64_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int64_t def = value;
  value = m_Prefs->value(name, def).toLongLong(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t QFilterParametersReader::readValue(const QString name, uint8_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  uint8_t def = value;
  value = m_Prefs->value(name, def).toUInt(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint16_t QFilterParametersReader::readValue(const QString name, uint16_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  uint16_t def = value;
  value = m_Prefs->value(name, def).toUInt(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t QFilterParametersReader::readValue(const QString name, uint32_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  uint32_t def = value;
  value = m_Prefs->value(name, def).toUInt(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t QFilterParametersReader::readValue(const QString name, uint64_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  uint64_t def = value;
  value = m_Prefs->value(name, def).toLongLong(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float QFilterParametersReader::readValue(const QString name, float value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  float def = value;
  value = m_Prefs->value(name, def).toFloat(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double QFilterParametersReader::readValue(const QString name, double value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  double def = value;
  value = m_Prefs->value(name, def).toDouble(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QFilterParametersReader::readValue(const QString name, bool def)
{
  BOOST_ASSERT(m_Prefs != NULL);
  //QByteArray ba = name.toAscii();
  QVariant v = m_Prefs->value(name, def);
  bool bvalue = v.toBool();
  return bvalue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int8_t> QFilterParametersReader::readArray(const QString name, QVector<int8_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<int8_t> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0).toInt(&ok);
    if (!ok) { selections[i] = 0; }
  }
  m_Prefs->endArray();
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int16_t> QFilterParametersReader::readArray(const QString name, QVector<int16_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<int16_t> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0).toInt(&ok);
    if (!ok) { selections[i] = 0; }
  }
  m_Prefs->endArray();
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int32_t> QFilterParametersReader::readArray(const QString name, QVector<int32_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<int32_t> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0).toInt(&ok);
    if (!ok) { selections[i] = 0; }
  }
  m_Prefs->endArray();
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int64_t> QFilterParametersReader::readArray(const QString name, QVector<int64_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<int64_t> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0).toLongLong(&ok);
    if (!ok) { selections[i] = 0; }
  }
  m_Prefs->endArray();
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint8_t> QFilterParametersReader::readArray(const QString name, QVector<uint8_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<uint8_t> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0).toUInt(&ok);
    if (!ok) { selections[i] = 0; }
  }
  m_Prefs->endArray();
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint16_t> QFilterParametersReader::readArray(const QString name, QVector<uint16_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<uint16_t> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0).toUInt(&ok);
    if (!ok) { selections[i] = 0; }
  }
  m_Prefs->endArray();
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint32_t> QFilterParametersReader::readArray(const QString name, QVector<uint32_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<uint32_t> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0).toUInt(&ok);
    if (!ok) { selections[i] = 0; }
  }
  m_Prefs->endArray();
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint64_t> QFilterParametersReader::readArray(const QString name, QVector<uint64_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<uint64_t> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0).toLongLong(&ok);
    if (!ok) { selections[i] = 0; }
  }
  m_Prefs->endArray();
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> QFilterParametersReader::readArray(const QString name, QVector<float> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<float> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0.0f).toFloat(&ok);
    if (!ok) { selections[i] = 0.0f; }
  }
  m_Prefs->endArray();
  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> QFilterParametersReader::readArray(const QString name, QVector<double> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  int count = m_Prefs->beginReadArray(name);
  QVector<double> selections(count, 0);
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    selections[i] = m_Prefs->value(name, 0.0).toDouble(&ok);
    if (!ok) { selections[i] = 0.0; }
  }
  m_Prefs->endArray();
  return value;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Widget_t QFilterParametersReader::readIntVec3(const QString name, IntVec3Widget_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  QVariant var = m_Prefs->value(name);
  bool ok = false;
  IntVec3Widget_t v3 = var.value<IntVec3Widget_t>();
  m_Prefs->beginReadArray(name);
  m_Prefs->setArrayIndex(0);
  v3.x = m_Prefs->value("x", v3.x).toInt(&ok);
  if(!ok) { v3.x = defaultValue.x; }

  m_Prefs->setArrayIndex(1);
  v3.y = m_Prefs->value("y", v3.y).toInt(&ok);
  if(!ok) { v3.y = defaultValue.y; }

  m_Prefs->setArrayIndex(2);
  v3.z = m_Prefs->value("z", v3.z).toInt(&ok);
  if(!ok) { v3.z = defaultValue.z; }

  m_Prefs->endArray();

  return v3;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Widget_t QFilterParametersReader::readFloatVec3(const QString name, FloatVec3Widget_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  QVariant var = m_Prefs->value(name);
  bool ok = false;
  FloatVec3Widget_t v3 = var.value<FloatVec3Widget_t>();
  m_Prefs->beginReadArray(name);
  m_Prefs->setArrayIndex(0);
  v3.x = m_Prefs->value("x", v3.x).toFloat(&ok);
  if(!ok) { v3.x = defaultValue.x; }

  m_Prefs->setArrayIndex(1);
  v3.y = m_Prefs->value("y", v3.y).toFloat(&ok);
  if(!ok) { v3.y = defaultValue.y; }

  m_Prefs->setArrayIndex(2);
  v3.z = m_Prefs->value("z", v3.z).toFloat(&ok);
  if(!ok) { v3.z = defaultValue.z; }


  m_Prefs->endArray();

  return v3;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInput_t QFilterParametersReader::readComparisonInput(const QString name, ComparisonInput_t defaultValue, int arrayIndex)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  ComparisonInput_t v;
  m_Prefs->beginReadArray(name);
  m_Prefs->setArrayIndex(arrayIndex);
  v.arrayName = m_Prefs->value("ArrayName").toString();
  v.compOperator = m_Prefs->value("CompOperator").toInt(&ok);
  if(!ok) { m_Prefs->endArray(); return defaultValue; }
  v.compValue = m_Prefs->value("CompValue").toFloat(&ok);
  if(!ok) { m_Prefs->endArray(); return defaultValue; }

  m_Prefs->endArray();
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<ComparisonInput_t> QFilterParametersReader::readComparisonInputs(const QString name, QVector<ComparisonInput_t> defValues)
{
  BOOST_ASSERT(m_Prefs != NULL);
  QByteArray ba = name.toAscii();
  QVector<ComparisonInput_t> comparisons;
  int count = m_Prefs->beginReadArray(name);
  bool ok = false;
  ComparisonInput_t v;
  v.compOperator = 0;
  v.compValue = 0.0;
  for(int i = 0; i < count; i++)
  {

    m_Prefs->setArrayIndex(i);
    v.arrayName = m_Prefs->value("ArrayName").toString();
    v.compOperator = m_Prefs->value("CompOperator").toInt(&ok);
    if(!ok) { continue; }
    v.compValue = m_Prefs->value("CompValue").toFloat(&ok);
    if(!ok) { continue; }
    comparisons.push_back(v);
  }
  m_Prefs->endArray();
  return comparisons;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleInput_t QFilterParametersReader::readAxisAngle(const QString name, AxisAngleInput_t v, int vectorPos)
{
  BOOST_ASSERT(m_Prefs != NULL);
  int count = m_Prefs->beginReadArray(name);

  QVector<float> angles(count);
  QVector<float> axis(count * 3);
  bool ok = false;
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    angles[i] = m_Prefs->value("Angle").toFloat(&ok);
    axis[i * 3 + 0] = m_Prefs->value("H").toFloat(&ok);
    axis[i * 3 + 1] = m_Prefs->value("K").toFloat(&ok);
    axis[i * 3 + 2] = m_Prefs->value("L").toFloat(&ok);
  }
  m_Prefs->endArray();
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AxisAngleInput_t> QFilterParametersReader::readAxisAngles(const QString name, QVector<AxisAngleInput_t> v)
{
  BOOST_ASSERT(m_Prefs != NULL);
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
QSet<QString> QFilterParametersReader::readArraySelections(const QString name, QSet<QString> v)
{
  BOOST_ASSERT(m_Prefs != NULL);
  QString arrayName = QString("ArraySelections_") + name;
  int count = m_Prefs->beginReadArray(arrayName);
  QSet<QString> selections;
  for(int i = 0; i < count; ++i)
  {
    m_Prefs->setArrayIndex(i);
    QString str = m_Prefs->value(name, QString::fromUtf8("NOT_FOUND")).toString();
    selections.insert(str);
  }
  m_Prefs->endArray();
  return selections;
}

