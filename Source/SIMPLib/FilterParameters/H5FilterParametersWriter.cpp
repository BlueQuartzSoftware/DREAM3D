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

#include "H5FilterParametersWriter.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/H5FilterParametersConstants.h"
#include "SIMPLib/Common/Constants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5FilterParametersWriter::H5FilterParametersWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5FilterParametersWriter::~H5FilterParametersWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::WritePipelineToFile(FilterPipeline::Pointer pipeline, QString filePath, QString name, IObserver* obs)
{
  if (NULL == pipeline.get())
  {
    if (NULL != obs)
    {
      PipelineMessage pm(H5FilterParametersWriter::ClassName(), "FilterPipeline Object was NULL for writing", -1, PipelineMessage::Error);
      obs->processPipelineMessage(pm);
    }
    return -1;
  }

  QFileInfo fileInfo(filePath);

  // WRITE THE PIPELINE TO THE HDF5 FILE
  H5FilterParametersWriter::Pointer writer = H5FilterParametersWriter::New();

  hid_t fileId = -1;

  fileId = QH5Utilities::createFile(filePath);
  if (fileId < 0)
  {
    if (NULL != obs)
    {
      PipelineMessage pm(H5FilterParametersWriter::ClassName(), "Output .dream3d file could not be created.", -1, PipelineMessage::Error);
      obs->processPipelineMessage(pm);
    }
    return -1;
  }

  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  // This will also take care of making sure all groups and file ids are closed
  // before this method returns.
  HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

  // Write our File Version string to the Root "/" group
  QH5Lite::writeStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, DREAM3D::HDF5::FileVersion);
  QH5Lite::writeStringAttribute(fileId, "/", DREAM3D::HDF5::DREAM3DVersion, SIMPLib::Version::Complete() );

  hid_t pipelineGroupId = QH5Utilities::createGroup(fileId, DREAM3D::StringConstants::PipelineGroupName);
  scopedFileSentinel.addGroupId(&pipelineGroupId);
  writer->setGroupId(pipelineGroupId);

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
int H5FilterParametersWriter::openFilterGroup(AbstractFilter* filter, int index)
{
  int err = 0;
  if (m_GroupId <= 0)
  {
    return -1;
  }
  QString name = QString::number(index);
  m_CurrentGroupId = QH5Utilities::createGroup(m_GroupId, name);
  err = QH5Lite::writeStringAttribute(m_GroupId, name, "ClassName", filter->getNameOfClass());
  err = QH5Lite::writeStringAttribute(m_GroupId, name, "HumanLabel", filter->getHumanLabel());
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::closeFilterGroup()
{
  H5Gclose(m_CurrentGroupId);
  m_CurrentGroupId = -1;
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, const QString value)
{
  int err = 0;
  err = QH5Lite::writeStringDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, const QVector<QString> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeStringAttribute(m_CurrentGroupId, name, ss, value[i]);
    ss.clear();
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, int8_t value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, int16_t value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, int32_t value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, int64_t value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, uint8_t value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, uint16_t value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, uint32_t value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, uint64_t value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, float value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, double value)
{
  int err = 0;
  err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<int8_t> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<int16_t> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<int32_t> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<int64_t> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<uint8_t> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<uint16_t> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<uint32_t> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<uint64_t> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<float> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<double> value)
{
  int vectorSize = value.size();
  int err = QH5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  for (int i = 0; i < vectorSize; i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss, value[i]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, IntVec3_t v)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 3 };
  err = QH5Lite::writePointerDataset<int32_t>(m_CurrentGroupId, name, rank, dims, reinterpret_cast<int32_t*>(&v) );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, FloatVec3_t v)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 3 };
  err = QH5Lite::writePointerDataset<float>(m_CurrentGroupId, name, rank, dims, reinterpret_cast<float*>(&v) );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, Float2ndOrderPoly_t v)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 6 };
  err = QH5Lite::writePointerDataset<float>(m_CurrentGroupId, name, rank, dims, reinterpret_cast<float*>(&v) );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, Float3rdOrderPoly_t v)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 21 };
  err = QH5Lite::writePointerDataset<float>(m_CurrentGroupId, name, rank, dims, reinterpret_cast<float*>(&v) );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, Float4thOrderPoly_t v)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 21 };
  err = QH5Lite::writePointerDataset<float>(m_CurrentGroupId, name, rank, dims, reinterpret_cast<float*>(&v) );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, FileListInfo_t v)
{
  int err = 0;
  hid_t dcaGid = QH5Utilities::createGroup(m_CurrentGroupId, name);

  err = QH5Lite::writeScalarDataset<qint32>(dcaGid, "EndIndex", v.EndIndex );
  err = QH5Lite::writeScalarDataset<qint32>(dcaGid, "StartIndex", v.StartIndex );
  err = QH5Lite::writeScalarDataset<qint32>(dcaGid, "PaddingDigits", v.PaddingDigits );
  err = QH5Lite::writeScalarDataset<quint32>(dcaGid, "Ordering", v.Ordering );
  err = QH5Lite::writeStringDataset(dcaGid, "FileExtension", v.FileExtension );
  err = QH5Lite::writeStringDataset(dcaGid, "FilePrefix", v.FilePrefix );
  err = QH5Lite::writeStringDataset(dcaGid, "FileSuffix", v.FileSuffix );
  err = QH5Lite::writeStringDataset(dcaGid, "InputPath", v.InputPath );
  dcaGid = H5Gclose(dcaGid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, ComparisonInput_t comp)
{
  ComparisonInputs v;
  v.addInput(comp);
  return writeValue(name, v);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, ComparisonInputs v)
{
  int numQFilters = static_cast<int>( v.size() );
  int err = 0;
  QString buf;
  QTextStream ss(&buf);

  // Create a string that is comma delimited for each comparison operator and new line delimited
  // to mark the end of each ComparisonOpertor struct
  for(int i = 0; i < numQFilters; i++)
  {
    ss << v[i].dataContainerName << "," << v[i].attributeMatrixName << "," << v[i].attributeArrayName << "," <<  v[i].compOperator << "," << v[i].compValue << "\n";
  }

  // Write the data set to the file and attach an attribute that says how many there were.
  if(numQFilters > 0)
  {
    err = QH5Lite::writeStringDataset(m_CurrentGroupId, name, buf);
    if (err < 0)
    {
      return err;
    }
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, "NumInputs", numQFilters);
    if (err < 0)
    {
      return err;
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, AxisAngleInput_t v)
{
  int numQFilters = static_cast<int>( 1 );
  int err = writeValue(name,  numQFilters);
  if (err < 0)
  {
    return err;
  }
  QString parseOrder = "Angle, H, K, L";
  err = QH5Lite::writeStringAttribute(m_CurrentGroupId, name, "Data Order", parseOrder);
  if (err < 0)
  {
    return err;
  }
  err = writeValue(name, v, 0);
  if (err < 0)
  {
    return err;
  }
  return err;
}

// -----------------------------------------------------------------------------
// These are actually written as a binary 4x1 float array as an attribute
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, AxisAngleInput_t v, int vectorPos)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 4 };

  QString ss = QString::number(vectorPos) + H5FilterParameter::AxisAngleInput;

  err = QH5Lite::writePointerAttribute<float>(m_CurrentGroupId, name, ss, rank, dims, reinterpret_cast<float*>(&v) );
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, QVector<AxisAngleInput_t> v)
{
  int numQFilters = static_cast<int>( v.size() );
  int err = writeValue(name,  numQFilters);
  if (err < 0)
  {
    return err;
  }
  QString parseOrder = "Angle, H, K, L";
  err = QH5Lite::writeStringAttribute(m_CurrentGroupId, name, "Data Order", parseOrder);
  if (err < 0)
  {
    return err;
  }
  for(int i = 0; i < numQFilters; i++)
  {
    err = writeValue(name, v[i], i);
    if (err < 0)
    {
      return err;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeArraySelections(const QString name, QSet<QString> v)
{
  size_t size = v.size();
  herr_t err = 0;
  if (size > 0)
  {
    QString setStr = "";
    QSet<QString>::iterator iter = v.begin();
    for (; iter != v.end(); iter++)
    {
      setStr.append(*iter).append("\n");
    }

    err = QH5Lite::writeStringDataset(m_CurrentGroupId, name, setStr);
    if (err < 0)
    {
      return err;
    }
  }
  if(size > 0)
  {
    err = QH5Lite::writeScalarAttribute(m_CurrentGroupId, name, "NumArrays", size);
    if (err < 0)
    {
      return err;
    }
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, const DataContainerArrayProxy& v)
{
  int err = 0;
  QList<DataContainerProxy> dcProxies = v.dataContainers.values();
  QListIterator<DataContainerProxy> dcIter(dcProxies);
  hid_t dcaGid = QH5Utilities::createGroup(m_CurrentGroupId, name);

  while (dcIter.hasNext()) // DataContainerLevel
  {
    const DataContainerProxy& dcProxy =  dcIter.next();
    if(dcProxy.flag == Qt::Unchecked)
    {
      continue;    // Skip to the next DataContainer if we are not reading this one.
    }
    hid_t dcGid = QH5Utilities::createGroup(dcaGid, dcProxy.name);

    QStringList flat;
    QMapIterator<QString, AttributeMatrixProxy> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();

      const AttributeMatrixProxy& amProxy = amIter.value();
      if(amProxy.flag == Qt::Unchecked)
      {
        continue;    // Skip to the next AttributeMatrix if not reading this one
      }
      hid_t amGid = QH5Utilities::createGroup(dcGid, amProxy.name);

      QMapIterator<QString, DataArrayProxy> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();

        const DataArrayProxy& daProxy = dIter.value();
        if(daProxy.flag == DREAM3D::Unchecked)
        {
          continue;    // Skip to the next DataArray if not reading this one
        }

        flat << dIter.value().name;
      }
      QString data = flat.join(QString('\n'));
      err = QH5Lite::writeStringDataset(amGid, QString::fromLatin1("Arrays"), data);

      H5Gclose(amGid);
    }

    H5Gclose(dcGid);
  }

  H5Gclose(dcaGid);
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, const DataArrayPath& v)
{
  int err = 0;
  QString value = v.serialize();
  err = QH5Lite::writeStringDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, const QVector<DataArrayPath>& v)
{
  int err = 0;
  QString pathStr;
  QTextStream ss(&pathStr);
  char sep = '\n'; // Use a new line to separate each record.
  for (int i = 0; i < v.size(); i++)
  {
    DataArrayPath path = v.at(i);
    ss << path.serialize("|") << sep;
  }
  err = QH5Lite::writeStringDataset(m_CurrentGroupId, name, pathStr);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const QString name, const DynamicTableData& v)
{
  int err = 0;

  QVector<double> flat = v.flattenData();
  QString rHeaders = v.serializeRowHeaders('|');
  QString cHeaders = v.serializeColumnHeaders('|');

  QVector<hsize_t> dims(2);
  dims[0] = v.getNumRows();
  dims[1] = v.getNumCols();

  err = QH5Lite::writeVectorDataset(m_CurrentGroupId, name, dims, flat);
  if (err < 0) { return err; }
  err = QH5Lite::writeStringAttribute(m_CurrentGroupId, name, "RowHeaders", rHeaders);
  if (err < 0) { return err; }
  err = QH5Lite::writeStringAttribute(m_CurrentGroupId, name, "ColHeaders", cHeaders);

  return err;
}

