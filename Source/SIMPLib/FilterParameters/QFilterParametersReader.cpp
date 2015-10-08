/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "QFilterParametersReader.h"

#include <QtCore/QMetaType>
#include <QtCore/QDataStream>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/CoreFilters/EmptyFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterParametersReader::QFilterParametersReader() :
  AbstractFilterParametersReader(),
  m_Prefs(NULL)
{
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
FilterPipeline::Pointer QFilterParametersReader::ReadPipelineFromFile(QString filePath, QSettings::Format format, IObserver* obs)
{


  if (filePath.isEmpty() == true)
  {
    return FilterPipeline::NullPointer();
  }
  QFileInfo fi(filePath);
  if(fi.exists() == false)
  {
    return FilterPipeline::NullPointer();
  }

  FilterManager* filtManager = FilterManager::Instance();
  FilterFactory<EmptyFilter>::Pointer emptyFilterFactory = FilterFactory<EmptyFilter>::New();
  filtManager->addFilterFactory("EmptyFilter", emptyFilterFactory);

  QFilterParametersReader::Pointer paramsReader = QFilterParametersReader::New();
  paramsReader->openFile(filePath, format);
  QSettings* prefs = paramsReader->getPrefs();
  prefs->beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
  bool ok = false;
  int filterCount = prefs->value(DREAM3D::Settings::NumFilters).toInt(&ok);
  prefs->endGroup();

  FilterPipeline::Pointer pipeline = FilterPipeline::New();


  if (false == ok)
  {
    filterCount = 0;
  }

  for (int i = 0; i < filterCount; ++i)
  {
    QString gName = QString::number(i);

    // Open the group to get the name of the filter then close again.
    //qDebug() << " Current QSettings Group: " << prefs->group();
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
      else // Could not find the filter because the specific name has not been registered. This could
        // be due to a name change for the filter.
      {
        EmptyFilter::Pointer filter = EmptyFilter::New();
        QString humanLabel = QString("UNKNOWN FILTER: ") + filterName;
        filter->setHumanLabel(humanLabel);
        filter->setOriginalFilterName(filterName);
        pipeline->pushBack(filter);

        if (NULL != obs)
        {
          QString ss = QObject::tr("An implementation for filter '%1' could not be located. Possible reasons include a name change of the filter, plugin not loading or a simple spelling mistake? A blank filter has been inserted in its place.").arg(filterName);
          PipelineMessage pm(filterName, ss, -66066, PipelineMessage::Error);
          pm.setPrefix("QFilterParametersReader::ReadPipelineFromFile()");
          obs->processPipelineMessage(pm);
        }
      }
    }
    else
    {
      EmptyFilter::Pointer filter = EmptyFilter::New();
      QString humanLabel = QString("MISSING FILTER: ") + filterName;
      filter->setHumanLabel(humanLabel);
      filter->setOriginalFilterName(filterName);
      pipeline->pushBack(filter);

      if (NULL != obs)
      {
        QString ss = QObject::tr("A filter for index '%1' is missing in the file. Is the numbering of the filters correct in the pipeline file?").arg(gName);
        PipelineMessage pm(filterName, ss, -66067, PipelineMessage::Error);
        pm.setPrefix("QFilterParametersReader::ReadPipelineFromFile()");
        obs->processPipelineMessage(pm);
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
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
  return m_Prefs->value(name, value).toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> QFilterParametersReader::readStrings(const QString name, QVector<QString> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }

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
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
  bool ok = false;
  int8_t def = value;
  value = m_Prefs->value(name, def).toInt(&ok);
  if(ok)
  {
    return value;
  }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int16_t QFilterParametersReader::readValue(const QString name, int16_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
  bool ok = false;
  int16_t def = value;
  value = m_Prefs->value(name, def).toInt(&ok);
  if(ok)
  {
    return value;
  }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t QFilterParametersReader::readValue(const QString name, int32_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
  bool ok = false;
  int32_t def = value;
  value = m_Prefs->value(name, def).toInt(&ok);
  if(ok)
  {
    return value;
  }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t QFilterParametersReader::readValue(const QString name, int64_t v)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return v;
  }
  bool ok = false;
  qint64 def = static_cast<qint64>(v);
  QVariant var = m_Prefs->value(name, def);
  qint64 value = var.toLongLong(&ok);
  if(ok)
  {
    return static_cast<int64_t>(value);
  }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t QFilterParametersReader::readValue(const QString name, uint8_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
  bool ok = false;
  uint8_t def = value;
  value = m_Prefs->value(name, def).toUInt(&ok);
  if(ok)
  {
    return value;
  }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint16_t QFilterParametersReader::readValue(const QString name, uint16_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
  bool ok = false;
  uint16_t def = value;
  value = m_Prefs->value(name, def).toUInt(&ok);
  if(ok)
  {
    return value;
  }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t QFilterParametersReader::readValue(const QString name, uint32_t value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
  bool ok = false;
  uint32_t def = value;
  value = m_Prefs->value(name, def).toUInt(&ok);
  if(ok)
  {
    return value;
  }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t QFilterParametersReader::readValue(const QString name, uint64_t v)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return v;
  }
  bool ok = false;
  quint64 def = static_cast<quint64>(v);
  QVariant var = m_Prefs->value(name, def);
  quint64 value = var.toULongLong(&ok);
  if(ok)
  {
    return static_cast<uint64_t>(value);
  }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float QFilterParametersReader::readValue(const QString name, float def)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return def;
  }
  bool ok = false;
  QVariant var = m_Prefs->value(name, def);
  double value = var.toDouble(&ok);
  if(ok)
  {
    return static_cast<float>(value);
  }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double QFilterParametersReader::readValue(const QString name, double def)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return def;
  }

  bool ok = false;
  QVariant var = m_Prefs->value(name, def);
  double value = var.toDouble(&ok);
  if(ok)
  {
    return value;
  }
  return def;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QFilterParametersReader::readValue(const QString name, bool def)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return def;
  }

  QVariant var = m_Prefs->value(name, def);
  return var.toBool();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int8_t> QFilterParametersReader::readArray(const QString name, QVector<int8_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int16_t> QFilterParametersReader::readArray(const QString name, QVector<int16_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int32_t> QFilterParametersReader::readArray(const QString name, QVector<int32_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int64_t> QFilterParametersReader::readArray(const QString name, QVector<int64_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint8_t> QFilterParametersReader::readArray(const QString name, QVector<uint8_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint16_t> QFilterParametersReader::readArray(const QString name, QVector<uint16_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint32_t> QFilterParametersReader::readArray(const QString name, QVector<uint32_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<uint64_t> QFilterParametersReader::readArray(const QString name, QVector<uint64_t> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> QFilterParametersReader::readArray(const QString name, QVector<float> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }
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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> QFilterParametersReader::readArray(const QString name, QVector<double> value)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return value;
  }

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
    if (false == ok)
    {
      return value;
    }
  }
  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3_t QFilterParametersReader::readIntVec3(const QString name, IntVec3_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  IntVec3_t v3;
  v3.x = 0;
  v3.y = 0;
  v3.z = 0;

  int count = m_Prefs->beginReadArray(name);
  if (count == 0)
  {
    m_Prefs->endArray();
    return defaultValue;
  }
  m_Prefs->setArrayIndex(0);
  v3.x = m_Prefs->value("x", v3.x).toInt(&ok);
  if(!ok)
  {
    v3.x = defaultValue.x;
  }

  m_Prefs->setArrayIndex(1);
  v3.y = m_Prefs->value("y", v3.y).toInt(&ok);
  if(!ok)
  {
    v3.y = defaultValue.y;
  }

  m_Prefs->setArrayIndex(2);
  v3.z = m_Prefs->value("z", v3.z).toInt(&ok);
  if(!ok)
  {
    v3.z = defaultValue.z;
  }

  m_Prefs->endArray();

  return v3;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3_t QFilterParametersReader::readFloatVec3(const QString name, FloatVec3_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  FloatVec3_t v3;
  v3.x = 0.0f;
  v3.y = 0.0f;
  v3.z = 0.0f;
  int count = m_Prefs->beginReadArray(name);
  if (count == 0)
  {
    m_Prefs->endArray();
    return defaultValue;
  }
  m_Prefs->setArrayIndex(0);
  v3.x = m_Prefs->value("x", v3.x).toFloat(&ok);
  if(!ok)
  {
    v3.x = defaultValue.x;
  }

  m_Prefs->setArrayIndex(1);
  v3.y = m_Prefs->value("y", v3.y).toFloat(&ok);
  if(!ok)
  {
    v3.y = defaultValue.y;
  }

  m_Prefs->setArrayIndex(2);
  v3.z = m_Prefs->value("z", v3.z).toFloat(&ok);
  if(!ok)
  {
    v3.z = defaultValue.z;
  }


  m_Prefs->endArray();

  return v3;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec4_t QFilterParametersReader::readFloatVec4(const QString name, FloatVec4_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  FloatVec4_t v4;
  v4.a = 0.0f;
  v4.b = 0.0f;
  v4.c = 0.0f;
  v4.d = 0.0f;
  int count = m_Prefs->beginReadArray(name);
  if (count == 0)
  {
    m_Prefs->endArray();
    return defaultValue;
  }
  m_Prefs->setArrayIndex(0);
  v4.a = m_Prefs->value("a", v4.a).toFloat(&ok);
  if(!ok)
  {
    v4.a = defaultValue.a;
  }

  m_Prefs->setArrayIndex(1);
  v4.b = m_Prefs->value("b", v4.b).toFloat(&ok);
  if(!ok)
  {
    v4.b = defaultValue.b;
  }

  m_Prefs->setArrayIndex(2);
  v4.c = m_Prefs->value("c", v4.c).toFloat(&ok);
  if(!ok)
  {
    v4.c = defaultValue.c;
  }

  m_Prefs->setArrayIndex(3);
  v4.d = m_Prefs->value("d", v4.d).toFloat(&ok);
  if(!ok)
  {
    v4.d = defaultValue.d;
  }


  m_Prefs->endArray();

  return v4;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec21_t QFilterParametersReader::readFloatVec21(const QString name, FloatVec21_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  FloatVec21_t vec;
  vec.v11 = 0.0f;
  vec.v12 = 0.0f;
  vec.v13 = 0.0f;
  vec.v14 = 0.0f;
  vec.v15 = 0.0f;
  vec.v16 = 0.0f;

  vec.v22 = 0.0f;
  vec.v23 = 0.0f;
  vec.v24 = 0.0f;
  vec.v25 = 0.0f;
  vec.v26 = 0.0f;

  vec.v33 = 0.0f;
  vec.v34 = 0.0f;
  vec.v35 = 0.0f;
  vec.v36 = 0.0f;

  vec.v44 = 0.0f;
  vec.v45 = 0.0f;
  vec.v46 = 0.0f;

  vec.v55 = 0.0f;
  vec.v56 = 0.0f;

  vec.v66 = 0.0f;

  int count = m_Prefs->beginReadArray(name);
  if (count == 0)
  {
    m_Prefs->endArray();
    return defaultValue;
  }

  m_Prefs->setArrayIndex(0);
  vec.v11 = m_Prefs->value("v11", vec.v11).toFloat(&ok);
  if(!ok)
  {
    vec.v11 = defaultValue.v11;
  }
  m_Prefs->setArrayIndex(1);
  vec.v12 = m_Prefs->value("v12", vec.v12).toFloat(&ok);
  if(!ok)
  {
    vec.v12 = defaultValue.v12;
  }
  m_Prefs->setArrayIndex(2);
  vec.v13 = m_Prefs->value("v13", vec.v13).toFloat(&ok);
  if(!ok)
  {
    vec.v13 = defaultValue.v13;
  }
  m_Prefs->setArrayIndex(3);
  vec.v14 = m_Prefs->value("v14", vec.v14).toFloat(&ok);
  if(!ok)
  {
    vec.v14 = defaultValue.v14;
  }
  m_Prefs->setArrayIndex(4);
  vec.v15 = m_Prefs->value("v15", vec.v15).toFloat(&ok);
  if(!ok)
  {
    vec.v15 = defaultValue.v15;
  }
  m_Prefs->setArrayIndex(5);
  vec.v16 = m_Prefs->value("v16", vec.v16).toFloat(&ok);
  if(!ok)
  {
    vec.v16 = defaultValue.v16;
  }

  m_Prefs->setArrayIndex(6);
  vec.v22 = m_Prefs->value("v22", vec.v22).toFloat(&ok);
  if(!ok)
  {
    vec.v22 = defaultValue.v22;
  }
  m_Prefs->setArrayIndex(7);
  vec.v23 = m_Prefs->value("v23", vec.v23).toFloat(&ok);
  if(!ok)
  {
    vec.v23 = defaultValue.v23;
  }
  m_Prefs->setArrayIndex(8);
  vec.v24 = m_Prefs->value("v24", vec.v24).toFloat(&ok);
  if(!ok)
  {
    vec.v24 = defaultValue.v24;
  }
  m_Prefs->setArrayIndex(9);
  vec.v25 = m_Prefs->value("v25", vec.v25).toFloat(&ok);
  if(!ok)
  {
    vec.v25 = defaultValue.v25;
  }
  m_Prefs->setArrayIndex(10);
  vec.v26 = m_Prefs->value("v26", vec.v26).toFloat(&ok);
  if(!ok)
  {
    vec.v26 = defaultValue.v26;
  }

  m_Prefs->setArrayIndex(11);
  vec.v33 = m_Prefs->value("v33", vec.v33).toFloat(&ok);
  if(!ok)
  {
    vec.v33 = defaultValue.v33;
  }
  m_Prefs->setArrayIndex(12);
  vec.v34 = m_Prefs->value("v34", vec.v34).toFloat(&ok);
  if(!ok)
  {
    vec.v34 = defaultValue.v34;
  }
  m_Prefs->setArrayIndex(13);
  vec.v35 = m_Prefs->value("v35", vec.v35).toFloat(&ok);
  if(!ok)
  {
    vec.v35 = defaultValue.v35;
  }
  m_Prefs->setArrayIndex(14);
  vec.v36 = m_Prefs->value("v36", vec.v36).toFloat(&ok);
  if(!ok)
  {
    vec.v36 = defaultValue.v36;
  }

  m_Prefs->setArrayIndex(15);
  vec.v44 = m_Prefs->value("v44", vec.v44).toFloat(&ok);
  if(!ok)
  {
    vec.v44 = defaultValue.v44;
  }
  m_Prefs->setArrayIndex(16);
  vec.v45 = m_Prefs->value("v45", vec.v45).toFloat(&ok);
  if(!ok)
  {
    vec.v45 = defaultValue.v45;
  }
  m_Prefs->setArrayIndex(17);
  vec.v46 = m_Prefs->value("v46", vec.v46).toFloat(&ok);
  if(!ok)
  {
    vec.v46 = defaultValue.v46;
  }

  m_Prefs->setArrayIndex(18);
  vec.v55 = m_Prefs->value("v55", vec.v55).toFloat(&ok);
  if(!ok)
  {
    vec.v55 = defaultValue.v55;
  }
  m_Prefs->setArrayIndex(19);
  vec.v56 = m_Prefs->value("v56", vec.v56).toFloat(&ok);
  if(!ok)
  {
    vec.v56 = defaultValue.v56;
  }

  m_Prefs->setArrayIndex(20);
  vec.v66 = m_Prefs->value("v66", vec.v66).toFloat(&ok);
  if(!ok)
  {
    vec.v66 = defaultValue.v66;
  }

  m_Prefs->endArray();

  return vec;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Float2ndOrderPoly_t QFilterParametersReader::readFloat2ndOrderPoly(const QString name, Float2ndOrderPoly_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  Float2ndOrderPoly_t vec;
  vec.c20 = 0.0f;
  vec.c02 = 0.0f;
  vec.c11 = 0.0f;

  vec.c10 = 0.0f;
  vec.c01 = 0.0f;
  vec.c00 = 0.0f;


  int count = m_Prefs->beginReadArray(name);
  if (count == 0)
  {
    m_Prefs->endArray();
    return defaultValue;
  }

  m_Prefs->setArrayIndex(0);
  vec.c20 = m_Prefs->value("c20", vec.c20).toFloat(&ok);
  if(!ok)
  {
    vec.c20 = defaultValue.c20;
  }
  m_Prefs->setArrayIndex(1);
  vec.c02 = m_Prefs->value("c02", vec.c02).toFloat(&ok);
  if(!ok)
  {
    vec.c02 = defaultValue.c02;
  }
  m_Prefs->setArrayIndex(2);
  vec.c11 = m_Prefs->value("c11", vec.c11).toFloat(&ok);
  if(!ok)
  {
    vec.c11 = defaultValue.c11;
  }

  m_Prefs->setArrayIndex(3);
  vec.c10 = m_Prefs->value("c10", vec.c10).toFloat(&ok);
  if(!ok)
  {
    vec.c10 = defaultValue.c10;
  }
  m_Prefs->setArrayIndex(4);
  vec.c01 = m_Prefs->value("c01", vec.c01).toFloat(&ok);
  if(!ok)
  {
    vec.c01 = defaultValue.c01;
  }
  m_Prefs->setArrayIndex(5);
  vec.c00 = m_Prefs->value("c00", vec.c00).toFloat(&ok);
  if(!ok)
  {
    vec.c00 = defaultValue.c00;
  }

  m_Prefs->endArray();

  return vec;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Float3rdOrderPoly_t QFilterParametersReader::readFloat3rdOrderPoly(const QString name, Float3rdOrderPoly_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  Float3rdOrderPoly_t vec;
  vec.c30 = 0.0f;
  vec.c03 = 0.0f;
  vec.c21 = 0.0f;
  vec.c12 = 0.0f;
  vec.c20 = 0.0f;
  vec.c02 = 0.0f;
  vec.c11 = 0.0f;
  vec.c10 = 0.0f;
  vec.c01 = 0.0f;
  vec.c00 = 0.0f;

  int count = m_Prefs->beginReadArray(name);
  if (count == 0)
  {
    m_Prefs->endArray();
    return defaultValue;
  }

  m_Prefs->setArrayIndex(0);
  vec.c30 = m_Prefs->value("c30", vec.c30).toFloat(&ok);
  if(!ok)
  {
    vec.c30 = defaultValue.c30;
  }
  m_Prefs->setArrayIndex(1);
  vec.c03 = m_Prefs->value("c03", vec.c03).toFloat(&ok);
  if(!ok)
  {
    vec.c03 = defaultValue.c03;
  }
  m_Prefs->setArrayIndex(2);
  vec.c21 = m_Prefs->value("c21", vec.c21).toFloat(&ok);
  if(!ok)
  {
    vec.c21 = defaultValue.c21;
  }
  m_Prefs->setArrayIndex(3);
  vec.c12 = m_Prefs->value("c12", vec.c12).toFloat(&ok);
  if(!ok)
  {
    vec.c12 = defaultValue.c12;
  }

  m_Prefs->setArrayIndex(4);
  vec.c20 = m_Prefs->value("c20", vec.c20).toFloat(&ok);
  if(!ok)
  {
    vec.c20 = defaultValue.c20;
  }
  m_Prefs->setArrayIndex(5);
  vec.c02 = m_Prefs->value("c02", vec.c02).toFloat(&ok);
  if(!ok)
  {
    vec.c02 = defaultValue.c02;
  }
  m_Prefs->setArrayIndex(6);
  vec.c11 = m_Prefs->value("c11", vec.c11).toFloat(&ok);
  if(!ok)
  {
    vec.c11 = defaultValue.c11;
  }
  m_Prefs->setArrayIndex(7);
  vec.c10 = m_Prefs->value("c10", vec.c10).toFloat(&ok);
  if(!ok)
  {
    vec.c10 = defaultValue.c10;
  }

  m_Prefs->setArrayIndex(8);
  vec.c01 = m_Prefs->value("c01", vec.c01).toFloat(&ok);
  if(!ok)
  {
    vec.c01 = defaultValue.c01;
  }
  m_Prefs->setArrayIndex(9);
  vec.c00 = m_Prefs->value("c00", vec.c00).toFloat(&ok);
  if(!ok)
  {
    vec.c00 = defaultValue.c00;
  }

  m_Prefs->endArray();

  return vec;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Float4thOrderPoly_t QFilterParametersReader::readFloat4thOrderPoly(const QString name, Float4thOrderPoly_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  Float4thOrderPoly_t vec;
  vec.c40 = 0.0f;
  vec.c04 = 0.0f;
  vec.c31 = 0.0f;
  vec.c13 = 0.0f;
  vec.c22 = 0.0f;
  vec.c30 = 0.0f;
  vec.c03 = 0.0f;
  vec.c21 = 0.0f;
  vec.c12 = 0.0f;
  vec.c20 = 0.0f;
  vec.c02 = 0.0f;
  vec.c11 = 0.0f;
  vec.c10 = 0.0f;
  vec.c01 = 0.0f;
  vec.c00 = 0.0f;


  int count = m_Prefs->beginReadArray(name);
  if (count == 0)
  {
    m_Prefs->endArray();
    return defaultValue;
  }

  m_Prefs->setArrayIndex(0);
  vec.c40 = m_Prefs->value("c40", vec.c40).toFloat(&ok);
  if(!ok)
  {
    vec.c40 = defaultValue.c40;
  }
  m_Prefs->setArrayIndex(1);
  vec.c04 = m_Prefs->value("c04", vec.c04).toFloat(&ok);
  if(!ok)
  {
    vec.c04 = defaultValue.c04;
  }
  m_Prefs->setArrayIndex(2);
  vec.c31 = m_Prefs->value("c31", vec.c31).toFloat(&ok);
  if(!ok)
  {
    vec.c31 = defaultValue.c31;
  }
  m_Prefs->setArrayIndex(3);
  vec.c13 = m_Prefs->value("c13", vec.c13).toFloat(&ok);
  if(!ok)
  {
    vec.c13 = defaultValue.c13;
  }
  m_Prefs->setArrayIndex(4);
  vec.c22 = m_Prefs->value("c22", vec.c22).toFloat(&ok);
  if(!ok)
  {
    vec.c22 = defaultValue.c22;
  }

  m_Prefs->setArrayIndex(5);
  vec.c30 = m_Prefs->value("c30", vec.c30).toFloat(&ok);
  if(!ok)
  {
    vec.c30 = defaultValue.c30;
  }
  m_Prefs->setArrayIndex(6);
  vec.c03 = m_Prefs->value("c03", vec.c03).toFloat(&ok);
  if(!ok)
  {
    vec.c03 = defaultValue.c03;
  }
  m_Prefs->setArrayIndex(7);
  vec.c21 = m_Prefs->value("c21", vec.c21).toFloat(&ok);
  if(!ok)
  {
    vec.c21 = defaultValue.c21;
  }
  m_Prefs->setArrayIndex(8);
  vec.c12 = m_Prefs->value("c12", vec.c12).toFloat(&ok);
  if(!ok)
  {
    vec.c12 = defaultValue.c12;
  }
  m_Prefs->setArrayIndex(9);
  vec.c20 = m_Prefs->value("c20", vec.c20).toFloat(&ok);
  if(!ok)
  {
    vec.c20 = defaultValue.c20;
  }

  m_Prefs->setArrayIndex(10);
  vec.c02 = m_Prefs->value("c02", vec.c02).toFloat(&ok);
  if(!ok)
  {
    vec.c02 = defaultValue.c02;
  }
  m_Prefs->setArrayIndex(11);
  vec.c11 = m_Prefs->value("c11", vec.c11).toFloat(&ok);
  if(!ok)
  {
    vec.c11 = defaultValue.c11;
  }
  m_Prefs->setArrayIndex(12);
  vec.c10 = m_Prefs->value("c10", vec.c10).toFloat(&ok);
  if(!ok)
  {
    vec.c10 = defaultValue.c10;
  }
  m_Prefs->setArrayIndex(13);
  vec.c01 = m_Prefs->value("c01", vec.c01).toFloat(&ok);
  if(!ok)
  {
    vec.c01 = defaultValue.c01;
  }
  m_Prefs->setArrayIndex(14);
  vec.c00 = m_Prefs->value("c00", vec.c00).toFloat(&ok);
  if(!ok)
  {
    vec.c00 = defaultValue.c00;
  }

  m_Prefs->endArray();

  return vec;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfo_t QFilterParametersReader::readFileListInfo(const QString name, FileListInfo_t defaultValue)
{
  BOOST_ASSERT(m_Prefs != NULL);
  bool ok = false;
  FileListInfo_t v;
  int count = m_Prefs->beginReadArray(name);
  if (count == 0)
  {
    m_Prefs->endArray();
    return defaultValue;
  }
  m_Prefs->setArrayIndex(0);
  v.EndIndex = m_Prefs->value("EndIndex", v.EndIndex).toLongLong(&ok);
  if(!ok)
  {
    v.EndIndex = defaultValue.EndIndex;
  }

  m_Prefs->setArrayIndex(1);
  v.FileExtension = m_Prefs->value("FileExtension", v.FileExtension).toString();
  if(!ok)
  {
    v.FileExtension = defaultValue.FileExtension;
  }

  m_Prefs->setArrayIndex(2);
  v.FilePrefix = m_Prefs->value("FilePrefix", v.FilePrefix).toString();
  if(!ok)
  {
    v.FilePrefix = defaultValue.FilePrefix;
  }

  m_Prefs->setArrayIndex(3);
  v.FileSuffix = m_Prefs->value("FileSuffix", v.FileSuffix).toString();
  if(!ok)
  {
    v.FileSuffix = defaultValue.FileSuffix;
  }

  m_Prefs->setArrayIndex(4);
  v.InputPath = m_Prefs->value("InputPath", v.InputPath).toString();
  if(!ok)
  {
    v.InputPath = defaultValue.InputPath;
  }

  m_Prefs->setArrayIndex(5);
  v.Ordering = m_Prefs->value("Ordering", v.Ordering).toUInt(&ok);
  if(!ok)
  {
    v.Ordering = defaultValue.Ordering;
  }

  m_Prefs->setArrayIndex(6);
  v.PaddingDigits = m_Prefs->value("PaddingDigits", v.PaddingDigits).toInt(&ok);
  if(!ok)
  {
    v.PaddingDigits = defaultValue.PaddingDigits;
  }

  m_Prefs->setArrayIndex(7);
  v.StartIndex = m_Prefs->value("StartIndex", v.StartIndex).toLongLong(&ok);
  if(!ok)
  {
    v.StartIndex = defaultValue.StartIndex;
  }

  m_Prefs->endArray();

  return v;
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
  v.dataContainerName = m_Prefs->value("DataContainerName").toString();
  v.attributeMatrixName = m_Prefs->value("AttributeMatrixName").toString();
  v.attributeArrayName = m_Prefs->value("AttributeArrayName").toString();
  v.compOperator = m_Prefs->value("CompOperator").toInt(&ok);
  if(!ok)
  {
    m_Prefs->endArray();
    m_Prefs->endArray();
    return defaultValue;
  }
  v.compValue = m_Prefs->value("CompValue").toDouble(&ok);
  if(!ok)
  {
    m_Prefs->endArray();
    m_Prefs->endArray();
    return defaultValue;
  }

  m_Prefs->endArray();
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputs QFilterParametersReader::readComparisonInputs(const QString name, ComparisonInputs defValues)
{
  BOOST_ASSERT(m_Prefs != NULL);
  ComparisonInputs comparisons;
  int count = m_Prefs->beginReadArray(name);
  bool ok = false;
  ComparisonInput_t v;
  v.compOperator = 0;
  v.compValue = 0.0;
  for(int i = 0; i < count; i++)
  {
    m_Prefs->setArrayIndex(i);
    v.dataContainerName = m_Prefs->value("DataContainerName").toString();
    v.attributeMatrixName = m_Prefs->value("AttributeMatrixName").toString();
    v.attributeArrayName = m_Prefs->value("AttributeArrayName").toString();
    v.compOperator = m_Prefs->value("CompOperator").toInt(&ok);
    if(!ok)
    {
      continue;
    }
    v.compValue = m_Prefs->value("CompValue").toDouble(&ok);
    if(!ok)
    {
      continue;
    }
    comparisons.addInput(v);
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
  bool closeArray = false;
  if (vectorPos == -1)
  {
    m_Prefs->beginReadArray(name);
    vectorPos = 0;
    closeArray = true;
  }
  bool ok = false;

  m_Prefs->setArrayIndex(vectorPos);
  v.angle = m_Prefs->value("Angle").toFloat(&ok);
  v.h = m_Prefs->value("H").toFloat(&ok);
  v.k = m_Prefs->value("K").toFloat(&ok);
  v.l = m_Prefs->value("L").toFloat(&ok);

  if (closeArray == true)
  {
    m_Prefs->endArray();
  }
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AxisAngleInput_t> QFilterParametersReader::readAxisAngles(const QString name, QVector<AxisAngleInput_t> v)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return v;
  }
  AxisAngleInput_t defValue;
  defValue.angle = 0.0f;
  defValue.h = 0.0f;
  defValue.k = 0.0f;
  defValue.l = 0.0f;

  int count = m_Prefs->beginReadArray(name);
  QVector<AxisAngleInput_t> values(count);
  for(int i = 0; i < count; i++)
  {
    if (i < v.size())
    {
      defValue = v[i];
    }
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
  // if(m_Prefs->contains(name) == false){ return v; }
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
  int count = m_Prefs->beginReadArray(name);
  //  QStringList paths;
  //  QStringList flags;

  for(int i = 0; i < count; i++)
  {
    m_Prefs->setArrayIndex(i);
    DataArrayPath dap(m_Prefs->value("Path", defValue).toString());
    QString dcFlag = m_Prefs->value("DCFlag", defValue).toString();
    QString attrFlag = m_Prefs->value("ATTRFlag", defValue).toString();
    QString daFlag = m_Prefs->value("DAFlag", defValue).toString();
    // Add the data container proxy if it does not exist
    if (dcaProxy.contains(dap.getDataContainerName()) == false)
    {
      DataContainerProxy dcp(dap.getDataContainerName());
      if (dcFlag.compare("0") == 0)
      {
        dcp.flag = Qt::Unchecked;
      }
      else if (dcFlag.compare("1") == 0)
      {
        dcp.flag = Qt::PartiallyChecked;
      }
      else
      {
        dcp.flag = Qt::Checked;
      }
      dcaProxy.dataContainers.insert(dcp.name, dcp);
    }

    // Now we check for the AttributeMatrix
    if(dcaProxy.contains(dap.getDataContainerName()) )
    {
      DataContainerProxy& dcProxy = dcaProxy.getDataContainerProxy(dap.getDataContainerName());
      if(dcProxy.attributeMatricies.find(dap.getAttributeMatrixName()) == dcProxy.attributeMatricies.end() && dap.getAttributeMatrixName().isEmpty() == false)
      {
        AttributeMatrixProxy attrProxy(dap.getAttributeMatrixName());
        if (attrFlag.compare("0") == 0)
        {
          attrProxy.flag = Qt::Unchecked;
        }
        else if (attrFlag.compare("1") == 0)
        {
          attrProxy.flag = Qt::PartiallyChecked;
        }
        else
        {
          attrProxy.flag = Qt::Checked;
        }
        dcProxy.attributeMatricies.insert(dap.getAttributeMatrixName(), attrProxy);
      }

      // Now we have the attribute matrix
      if(dcProxy.attributeMatricies.find(dap.getAttributeMatrixName()) != dcProxy.attributeMatricies.end() )
      {
        AttributeMatrixProxy& attrProxy = dcProxy.attributeMatricies[dap.getAttributeMatrixName()];
        // Now check for the data array
        if (attrProxy.dataArrays.find(dap.getDataArrayName()) == attrProxy.dataArrays.end() && dap.getDataArrayName().isEmpty() == false)
        {
          DataArrayProxy proxy(QString("%1|%2").arg(dap.getDataContainerName()).arg(dap.getAttributeMatrixName()), dap.getDataArrayName());
          if (daFlag.compare("0") == 0)
          {
            proxy.flag = Qt::Unchecked;
          }
          else if (daFlag.compare("1") == 0)
          {
            proxy.flag = Qt::PartiallyChecked;
          }
          else
          {
            proxy.flag = Qt::Checked;
          }
          attrProxy.dataArrays.insert(dap.getDataArrayName(), proxy);
        }
      }
    }
  }
  m_Prefs->endArray();
  return dcaProxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath QFilterParametersReader::readDataArrayPath(const QString& name, DataArrayPath def)
{
  BOOST_ASSERT(m_Prefs != NULL);
  if(m_Prefs->contains(name) == false)
  {
    return def;
  }
  QString defPath("");
  QString str = m_Prefs->value(name, defPath).toString();
  DataArrayPath path(str);
  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<DataArrayPath> QFilterParametersReader::readDataArrayPathVector(const QString& name, QVector<DataArrayPath> def)
{
  BOOST_ASSERT(m_Prefs != NULL);

  QVector<DataArrayPath> vector;

  int size = m_Prefs->beginReadArray(name);
  if (size <= 0)
  {
    return def;
  }

  for (int i = 0; i < size; ++i)
  {
    m_Prefs->setArrayIndex(i);
    QString pathStr = m_Prefs->value(DREAM3D::IO::DAPSettingsHeader).toString();
    DataArrayPath path = DataArrayPath::Deserialize(pathStr, "|");
    vector.append(path);
  }
  m_Prefs->endArray();

  return vector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData QFilterParametersReader::readDynamicTableData(const QString& name, DynamicTableData def)
{
  BOOST_ASSERT(m_Prefs != NULL);

  QString dataStr;
  int numRows, numCols;
  QStringList rHeaders;
  QStringList cHeaders;

  int size = m_Prefs->beginReadArray(name);
  if (size <= 0)
  {
    m_Prefs->endArray();
    return def;
  }

  m_Prefs->setArrayIndex(0);
  dataStr = m_Prefs->value(name, "").toString();
  m_Prefs->setArrayIndex(1);
  rHeaders = m_Prefs->value(name, "").toStringList();
  m_Prefs->setArrayIndex(2);
  numRows = m_Prefs->value(name, 0).toInt();
  m_Prefs->setArrayIndex(3);
  numCols = m_Prefs->value(name, 0).toInt();
  m_Prefs->setArrayIndex(4);
  cHeaders = m_Prefs->value(name, "").toStringList();

  m_Prefs->endArray();

  std::vector<std::vector<double> > data = DynamicTableData::DeserializeData(dataStr, numRows, numCols, ',');

  DynamicTableData tableData(data, rHeaders, cHeaders);

  return tableData;
}

