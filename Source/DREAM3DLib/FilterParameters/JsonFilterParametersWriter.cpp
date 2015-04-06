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

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

#include "JsonFilterParametersWriter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::JsonFilterParametersWriter() :
currentIndex(0)
{
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::JsonFilterParametersWriter(QString &fileName, QString &pipelineName, int &numFilters) :
currentIndex(0)
{
  m_FileName = fileName;
  m_PipelineName = pipelineName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::~JsonFilterParametersWriter()
{
  QFile outputFile(getFileName());
  QFileInfo info(outputFile);
  QString parentPath = info.absolutePath();
  QDir parentDir(parentPath);
  
  if (parentDir.exists() == false)
  {
    parentDir.mkpath(parentPath);
  }

  QJsonObject meta;
  meta["Name"] = m_PipelineName;
  meta["Version"] = DREAM3DLib::Version::Complete();
  meta[DREAM3D::Settings::NumFilters] = currentIndex + 1;

  m_Root[DREAM3D::Settings::PipelineBuilderGroup] = meta;
  QJsonDocument doc(m_Root);

  if (outputFile.open(QIODevice::WriteOnly))
  {
    QByteArray byteArray = doc.toJson();
    int err = outputFile.write(doc.toJson());
    outputFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::openFilterGroup(AbstractFilter* filter, int index)
{
  m_CurrentFilterIndex = QJsonObject();
  currentIndex = index;

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::closeFilterGroup()
{
  {
    QJsonDocument doc(m_CurrentFilterIndex);
    QByteArray bytes = doc.toJson();
  }

  {
    QString numStr = QString::number(currentIndex);
    m_Root[numStr] = m_CurrentFilterIndex;

    QJsonDocument doc(m_Root[numStr].toObject());
    QByteArray bytes = doc.toJson();
  }

  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, const QString value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = value;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, const QVector<QString> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(QString str, value)
  {
    jsonArray << QJsonValue(str);
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, int8_t value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = static_cast<double>(value);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, int16_t value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = static_cast<double>(value);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, int32_t value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = static_cast<double>(value);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, int64_t value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = static_cast<double>(value);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, uint8_t value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = static_cast<double>(value);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, uint16_t value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = static_cast<double>(value);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, uint32_t value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = static_cast<double>(value);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, uint64_t value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = static_cast<double>(value);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, float value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = static_cast<double>(value);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, double value)
{
  int err = 0;

  m_CurrentFilterIndex[name] = value;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<int8_t> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(int8_t val, value)
  {
    jsonArray << QJsonValue(static_cast<double>(val));
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<int16_t> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(int16_t val, value)
  {
    jsonArray << QJsonValue(static_cast<double>(val));
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<int32_t> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(int32_t val, value)
  {
    jsonArray << QJsonValue(static_cast<double>(val));
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<int64_t> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(int64_t val, value)
  {
    jsonArray << QJsonValue(static_cast<double>(val));
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<uint8_t> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(uint8_t val, value)
  {
    jsonArray << QJsonValue(static_cast<double>(val));
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<uint16_t> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(uint16_t val, value)
  {
    jsonArray << QJsonValue(static_cast<double>(val));
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<uint32_t> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(uint32_t val, value)
  {
    jsonArray << QJsonValue(static_cast<double>(val));
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<uint64_t> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(uint64_t val, value)
  {
    jsonArray << QJsonValue(static_cast<double>(val));
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<float> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(float val, value)
  {
    jsonArray << static_cast<double>(val);
  }

  m_CurrentFilterIndex[name] = jsonArray; 

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<double> value)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(double val, value)
  {
    jsonArray << QJsonValue(val);
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, IntVec3_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, FloatVec3_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, FloatVec4_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, FloatVec21_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, Float2ndOrderPoly_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, Float3rdOrderPoly_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, Float4thOrderPoly_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, FileListInfo_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, ComparisonInput_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, ComparisonInputs v)
{
  int err = 0;

  QJsonArray jsonArray;
  for (int i = 0; i < v.size(); i++)
  {
    QJsonObject obj;
    v[i].writeJson(obj);
    jsonArray.push_back(obj);
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<AxisAngleInput_t> v)
{
  int err = 0;

  QJsonArray jsonArray;
  for (int i = 0; i < v.size(); i++)
  {
    QJsonObject obj;
    v[i].writeJson(obj);
    jsonArray.push_back(obj);
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, AxisAngleInput_t v)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, AxisAngleInput_t v, int vectorPos)
{
  int err = 0;

  QJsonObject obj;
  v.writeJson(obj);
  m_CurrentFilterIndex[name] = obj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeArraySelections(const QString name, QSet<QString> v)
{
  int err = 0;

  QJsonArray jsonArray;

  foreach(QString str, v)
  {
    jsonArray.push_back(str);
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, DataContainerArrayProxy& v)
{
  int err = 0;

  QJsonObject proxyObj;
  v.writeJson(proxyObj);
  m_CurrentFilterIndex[name] = proxyObj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, DataArrayPath& v)
{
  int err = 0;

  QJsonObject pathObj;
  v.writeJson(pathObj);
  m_CurrentFilterIndex[name] = pathObj;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, QVector<DataArrayPath>& v)
{
  int err = 0;

  QJsonArray jsonArray;
  foreach(DataArrayPath path, v)
  {
    QJsonObject pathObj;
    path.writeJson(pathObj);
    jsonArray.push_back(pathObj);
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, DynamicTableData& v)
{
  int err = 0;

  QJsonObject dataObj;
  v.writeJson(dataObj);
  m_CurrentFilterIndex[name] = dataObj;

  return err;
}


