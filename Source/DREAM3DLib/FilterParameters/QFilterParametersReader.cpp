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
#include <QtCore/QStringList>

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterParametersReader::QFilterParametersReader() :
  AbstractFilterParametersReader(),
  m_Prefs(NULL)
{

  //  qRegisterMetaType<IntVec3Widget_t>("IntVec3Widget_t");
  //  qRegisterMetaTypeStreamOperators<IntVec3Widget_t>("IntVec3Widget_t");

  //  qRegisterMetaType<FloatVec3Widget_t>("FloatVec3Widget_t");
  //  qRegisterMetaTypeStreamOperators<FloatVec3Widget_t>("FloatVec3Widget_t");
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
FilterPipeline::Pointer QFilterParametersReader::ReadPipelineFromFile(QString filePath, QSettings::Format format, Observer* obs)
{
  FilterManager::Pointer filtManager = FilterManager::Instance();
  QFilterParametersReader::Pointer paramsReader = QFilterParametersReader::New();
  paramsReader->openFile(filePath, format);
  QSettings* prefs = paramsReader->getPrefs();
  prefs->beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
  bool ok = false;
  int filterCount = prefs->value(DREAM3D::Settings::NumFilters).toInt(&ok);
  prefs->endGroup();

  FilterPipeline::Pointer pipeline = FilterPipeline::New();


  if (false == ok) {filterCount = 0;}

  for (int i = 0; i < filterCount; ++i)
  {
    QString gName = QString::number(i);

    // Open the group to get the name of the filter then close again.
    prefs->beginGroup(gName);
    QString filterName = prefs->value(DREAM3D::Settings::FilterName, "").toString();
    prefs->endGroup();
    //qDebug() << "Group: " << gName << " FilterName: " << filterName;
    if(filterName.isEmpty() == false)
    {
      IFilterFactory::Pointer factory = filtManager->getFactoryForFilter(filterName);
      if(factory.get() != NULL)
      {
        AbstractFilter::Pointer filter = factory->create();

        if(NULL != filter.get())
        {
          filter->readFilterParameters(paramsReader.get(), i);
          pipeline->pushBack(filter);
        }
      }
    }
  }
  return pipeline;
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
void QFilterParametersReader::openFile(QString filename, QSettings::Format format)
{
  if(NULL != m_Prefs)
  {
    closeFile();
  }
  m_Prefs = new QSettings(filename, format);
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

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<QString> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split('|');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).trimmed());
  }
  return values;
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
int64_t QFilterParametersReader::readValue(const QString name, int64_t v)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  qint64 def = static_cast<qint64>(v);
  QVariant var = m_Prefs->value(name, def);
  qint64 value = var.toLongLong(&ok);
  if(ok) { return static_cast<int64_t>(value); }
  return v;
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
uint64_t QFilterParametersReader::readValue(const QString name, uint64_t v)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  quint64 def = static_cast<quint64>(v);
  QVariant var = m_Prefs->value(name, def);
  quint64 value = var.toULongLong(&ok);
  if(ok) { return static_cast<uint64_t>(value); }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float QFilterParametersReader::readValue(const QString name, float def)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  QVariant var = m_Prefs->value(name, def);
  double value = var.toDouble(&ok);
  if(ok) { return static_cast<float>(value); }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double QFilterParametersReader::readValue(const QString name, double def)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  QVariant var = m_Prefs->value(name, def);
  double value = var.toDouble(&ok);
  if(ok) { return value; }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QFilterParametersReader::readValue(const QString name, bool def)
{
  BOOST_ASSERT(m_Prefs != NULL);
  QVariant var = m_Prefs->value(name, def);
  return var.toBool();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int8_t> QFilterParametersReader::readArray(const QString name, QVector<int8_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<int8_t> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toInt(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int16_t> QFilterParametersReader::readArray(const QString name, QVector<int16_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<int16_t> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toShort(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int32_t> QFilterParametersReader::readArray(const QString name, QVector<int32_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<int32_t> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toInt(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int64_t> QFilterParametersReader::readArray(const QString name, QVector<int64_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<int64_t> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toLongLong(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint8_t> QFilterParametersReader::readArray(const QString name, QVector<uint8_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<uint8_t> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toUInt(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint16_t> QFilterParametersReader::readArray(const QString name, QVector<uint16_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<uint16_t> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toUShort(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint32_t> QFilterParametersReader::readArray(const QString name, QVector<uint32_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<uint32_t> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toUInt(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint64_t> QFilterParametersReader::readArray(const QString name, QVector<uint64_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<uint64_t> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toULongLong(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> QFilterParametersReader::readArray(const QString name, QVector<float> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<float> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toFloat(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> QFilterParametersReader::readArray(const QString name, QVector<double> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;

  QString data = m_Prefs->value(name).toByteArray();
  if(data.size() == 0)
  {
    return value;
  }

  QVector<double> values;
  // Parse the space delimited values
  QList<QString> tokens = data.split(' ');
  for(qint32 i = 0; i < tokens.size(); ++i)
  {
    values.push_back(tokens.at(i).toDouble(&ok));
    if (false == ok) { return value; }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Widget_t QFilterParametersReader::readIntVec3(const QString name, IntVec3Widget_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  //QVariant var = m_Prefs->value(name);
  bool ok = false;
  IntVec3Widget_t v3;
  v3.x = 0;
  v3.y = 0;
  v3.z = 0;

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
  v.compValue = m_Prefs->value("CompValue").toDouble(&ok);
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
    v.compValue = m_Prefs->value("CompValue").toDouble(&ok);
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

  bool ok = false;

  m_Prefs->setArrayIndex(vectorPos);
  v.angle = m_Prefs->value("Angle").toFloat(&ok);
  v.h = m_Prefs->value("H").toFloat(&ok);
  v.k = m_Prefs->value("K").toFloat(&ok);
  v.l = m_Prefs->value("L").toFloat(&ok);

  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AxisAngleInput_t> QFilterParametersReader::readAxisAngles(const QString name, QVector<AxisAngleInput_t> v)
{
  BOOST_ASSERT(m_Prefs != NULL);

  AxisAngleInput_t defValue;
  defValue.angle = 0.0f;
  defValue.h = 0.0f;
  defValue.k = 0.0f;
  defValue.l = 0.0f;

  int count = m_Prefs->beginReadArray(name);
  QVector<AxisAngleInput_t> values(count);
  for(int i = 0; i < count; i++)
  {
    if (i < v.size()) { defValue = v[i]; }
    else
    {
      defValue.angle = 0.0f;
      defValue.h = 0.0f;
      defValue.k = 0.0f;
      defValue.l = 0.0f;
    }
    values[i] = readAxisAngle(name, defValue, i);
  }
  m_Prefs->endArray();

  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> QFilterParametersReader::readArraySelections(const QString name, QSet<QString> v)
{
  BOOST_ASSERT(m_Prefs != NULL);
  QString defValue;
  int count = m_Prefs->beginReadArray("ArraySelections_" + name);
  QSet<QString> values;
  for(int i = 0; i < count; i++)
  {
    m_Prefs->setArrayIndex(i);
    QString data = m_Prefs->value(name, defValue).toString();
    values.insert(data);
  }
  m_Prefs->endArray();

  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy QFilterParametersReader::readDataContainerArrayProxy(const QString& name, DataContainerArrayProxy v)
{
  BOOST_ASSERT(m_Prefs != NULL);

  DataContainerArrayProxy dcaProxy;
  QString defValue;
  int count = m_Prefs->beginReadArray(name + "_DataContainerNames");
  if (count == 0) { return v; } // If there is nothing to read return the default value
  // First, gather the list of DataContainers

  for(int i = 0; i < count; i++)
  {
    m_Prefs->setArrayIndex(i);
    QString data = m_Prefs->value("Name", defValue).toString();
    DataContainerProxy dcProxy(data, true);
    dcaProxy.list.push_back(dcProxy);
   // qDebug() << "Found DataContainer: " << data;
  }
  m_Prefs->endArray();

  //Next, we are going to loop over all the DataContainers and read up their attribute matricies.
  for(int i = 0; i < dcaProxy.list.size(); i++)
  {
    DataContainerProxy& dcProxy = dcaProxy.list[i];
    count = m_Prefs->beginReadArray(name + "_" + dcProxy.name);
    for(int a = 0; a < count; a++)
    {
      m_Prefs->setArrayIndex(a);

      QString data = m_Prefs->value("ArrayName", defValue).toString(); // This gives the combined AttributeMatrixName + DataArrayName
      // Split the string into 2 parts using the '/' charater. First is the name of the AttributeMatrix, second is the data array
      QStringList tokens = data.split("/");
      if(dcProxy.attributeMatricies.find(tokens[0]) == dcProxy.attributeMatricies.end() ) // AttributeMatrix does not exist.
      {
        AttributeMatrixProxy amProxy(tokens[0], true);
        dcProxy.attributeMatricies[tokens[0]] = amProxy; // Add it to the DataContainerProxy
      }

      AttributeMatrixProxy& amProxy = dcProxy.attributeMatricies[tokens[0]];
      QString path = DREAM3D::StringConstants::DataContainerGroupName + "/" + dcProxy.name + "/" + amProxy.name;
      DataArrayProxy daProxy(path, tokens[1], true);
      amProxy.dataArrays.insert(daProxy.name, daProxy);
     // qDebug() << "Found AttributeMatrix/DataArray: " << data;
    }
    m_Prefs->endArray();
  }
  dcaProxy.isValid = true;
  return dcaProxy;
}
