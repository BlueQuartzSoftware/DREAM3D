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


#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

#include "SIMPLib/SIMPLibVersion.h"


#include "JsonFilterParametersWriter.h"
#include "SIMPLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::JsonFilterParametersWriter() :
  m_CurrentIndex(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JsonFilterParametersWriter::JsonFilterParametersWriter(QString& fileName, QString& pipelineName, int& numFilters) :
  m_CurrentIndex(0)
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

  // Write our File Version and DREAM3D Version strings
  QJsonObject meta;
  meta[DREAM3D::Settings::PipelineName] = m_PipelineName;
  meta[DREAM3D::Settings::Version] = SIMPLib::Version::Package();

  if (m_Root.size() > 0)
  {
    meta[DREAM3D::Settings::NumFilters] = m_CurrentIndex + 1;
  }
  else
  {
    meta[DREAM3D::Settings::NumFilters] = 0;
  }

  m_Root[DREAM3D::Settings::PipelineBuilderGroup] = meta;
  QJsonDocument doc(m_Root);

  if (outputFile.exists() == true)
  {
    outputFile.remove();
  }
  if (outputFile.open(QIODevice::WriteOnly))
  {
    outputFile.write(doc.toJson());
    outputFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::WritePipelineToFile(FilterPipeline::Pointer pipeline, QString filePath, QString name, IObserver* obs)
{
  if (NULL == pipeline.get())
  {
    if (NULL != obs)
    {
      PipelineMessage pm(JsonFilterParametersWriter::ClassName(), "FilterPipeline Object was NULL for writing", -1, PipelineMessage::Error);
      obs->processPipelineMessage(pm);
    }
    return -1;
  }

  QFileInfo fileInfo(filePath);

  // WRITE THE PIPELINE TO THE JSON FILE
  JsonFilterParametersWriter::Pointer writer = JsonFilterParametersWriter::New();
  writer->setFileName(filePath);
  writer->setPipelineName(fileInfo.completeBaseName());

  FilterPipeline::FilterContainerType& filters = pipeline->getFilterContainer();

  // Loop over each filter and write it's input parameters to the file
  int count = filters.size();
  int index = 0;
  for (qint32 i = 0; i < count; ++i)
  {
    AbstractFilter::Pointer filter = filters.at(i);
    if (NULL != filter.get())
    {
      index = filter->writeFilterParameters(writer.get(), index);
    }
    else
    {
      AbstractFilter::Pointer badFilter = AbstractFilter::New();
      writer->openFilterGroup(badFilter.get(), i);
      writer->writeValue("Unknown Filter", "ERROR: Filter instance was NULL within the PipelineFilterWidget instance. Report this error to the DREAM3D Developers");
      writer->closeFilterGroup();
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::openFilterGroup(AbstractFilter* filter, int index)
{
  m_CurrentIndex = index;
  QString numStr = QString::number(m_CurrentIndex);

  if (m_Root.contains(numStr))
  {
    m_CurrentFilterIndex = m_Root.value(numStr).toObject();
  }
  else
  {
    m_CurrentFilterIndex = QJsonObject();
    if(filter)
    {
      writeValue(DREAM3D::Settings::FilterName, filter->getNameOfClass());
      writeValue(DREAM3D::Settings::HumanLabel, filter->getHumanLabel());
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::closeFilterGroup()
{
  QString numStr = QString::number(m_CurrentIndex);
  m_Root[numStr] = m_CurrentFilterIndex;

  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, const QString value)
{
  int err = 0;
  QString repl = value;
  repl.replace("\\", QString("/"));
  m_CurrentFilterIndex[name] = repl;

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
    QString repl = str;
    repl.replace("\\", QString("/"));
    jsonArray << QJsonValue(str);
  }

  m_CurrentFilterIndex[name] = jsonArray;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int JsonFilterParametersWriter::writeValue(const QString name, const QStringList value)
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
  // We write this number as a string because if the value is greater than
  // 2^53 bits then the conversion to a double would truncate the value
  m_CurrentFilterIndex[name] = QString::number(value); // static_cast<double>(value);

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

  // We write this number as a string because if the value is greater than
  // 2^53 bits then the conversion to a double would truncate the value
  m_CurrentFilterIndex[name] = QString::number(value); // static_cast<double>(value);

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
    // We write this number as a string because if the value is greater than
    // 2^53 bits then the conversion to a double would truncate the value
    //jsonArray << QJsonValue(static_cast<double>(val));
    jsonArray << QJsonValue(QString::number(val));
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
    // We write this number as a string because if the value is greater than
    // 2^53 bits then the conversion to a double would truncate the value
    //jsonArray << QJsonValue(static_cast<double>(val));
    jsonArray << QJsonValue(QString::number(val));
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
int JsonFilterParametersWriter::writeValue(const QString name, const DataContainerArrayProxy& v)
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
int JsonFilterParametersWriter::writeValue(const QString name, const DataArrayPath& v)
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
int JsonFilterParametersWriter::writeValue(const QString name, const QVector<DataArrayPath>& v)
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
int JsonFilterParametersWriter::writeValue(const QString name, const DynamicTableData& v)
{
  int err = 0;

  QJsonObject dataObj;
  v.writeJson(dataObj);
  m_CurrentFilterIndex[name] = dataObj;

  return err;
}


