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

#include "DataArrayPath.h"

#include <QtCore/QJsonObject>

#include "SIMPLib/Common/Constants.h"

#include "moc_DataArrayPath.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDebug operator<<(QDebug out, const DataArrayPath& v)
{
  out << v.getDataContainerName() << "|" << v.getAttributeMatrixName() << "|" << v.getDataArrayName();
  return out;
}

/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DataArrayPathPrivate
{
    Q_DISABLE_COPY(DataArrayPathPrivate)
    Q_DECLARE_PUBLIC(DataArrayPath)
    DataArrayPath* const q_ptr;
    QString m_DataContainerName;
    QString m_AttributeMatrixName;
    QString m_DataArrayName;

    DataArrayPathPrivate(DataArrayPath* ptr);
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathPrivate::DataArrayPathPrivate(DataArrayPath* ptr) :
  q_ptr(ptr)
{

}

/* ############## Start API/Public Implementation ########################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath() :
  d_ptr(new DataArrayPathPrivate(this))
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const QString& dcName, const QString& amName, const QString& daName) :
  d_ptr(new DataArrayPathPrivate(this))
{
  d_ptr->m_DataContainerName = dcName;
  d_ptr->m_AttributeMatrixName = amName;
  d_ptr->m_DataArrayName = daName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const QString& path) :
  d_ptr(new DataArrayPathPrivate(this))
{
  QStringList tokens = path.split(DREAM3D::PathSep);

  if(tokens.size() > 0)
  {
    d_ptr->m_DataContainerName = tokens.at(0);
  }
  if(tokens.size() > 1)
  {
    d_ptr->m_AttributeMatrixName = tokens.at(1);
  }
  if(tokens.size() > 2)
  {
    d_ptr->m_DataArrayName = tokens.at(2);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const DataArrayPath& rhs) :
  d_ptr(new DataArrayPathPrivate(this))
{
  d_ptr->m_DataContainerName = rhs.getDataContainerName();
  d_ptr->m_AttributeMatrixName = rhs.getAttributeMatrixName();
  d_ptr->m_DataArrayName = rhs.getDataArrayName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::~DataArrayPath()
{
  delete d_ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<DataArrayPath> DataArrayPath::ConvertToQVector(QStringList& paths)
{
  QVector<DataArrayPath> vPaths;
  foreach(const QString sPath, paths)
  {
    DataArrayPath p(sPath);
    vPaths.push_back(p);
  }
  return vPaths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL_PIMPL_PROPERTY_DEF(DataArrayPath, QString, DataContainerName)
SIMPL_PIMPL_PROPERTY_DEF(DataArrayPath, QString, AttributeMatrixName)
SIMPL_PIMPL_PROPERTY_DEF(DataArrayPath, QString, DataArrayName)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::operator=(const DataArrayPath& rhs)
{
  d_ptr->m_DataContainerName = rhs.getDataContainerName();
  d_ptr->m_AttributeMatrixName = rhs.getAttributeMatrixName();
  d_ptr->m_DataArrayName = rhs.getDataArrayName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::operator==(const DataArrayPath& rhs)
{
  if( d_ptr->m_DataContainerName == rhs.getDataContainerName()
      && d_ptr->m_AttributeMatrixName == rhs.getAttributeMatrixName()
      && d_ptr->m_DataArrayName == rhs.getDataArrayName() )
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPath::serialize(QString delimiter) const
{
  QString s = d_ptr->m_DataContainerName + delimiter + d_ptr->m_AttributeMatrixName + delimiter + d_ptr->m_DataArrayName;
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataArrayPath::Deserialize(QString str, QString delimiter)
{
  if (str.isEmpty())
  {
    return DataArrayPath();
  }

  int start = 0;

  int tokenIndex = str.indexOf(delimiter, start);
  QString dcName = str.mid(start, tokenIndex);
  start = tokenIndex + 1;
  tokenIndex = str.indexOf(delimiter, start);
  QString amName = str.mid(start, tokenIndex - start);
  start = tokenIndex + 1;
  QString daName = str.mid(start);

  DataArrayPath path(dcName, amName, daName);
  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataArrayPath::getAsStringList()
{
  QStringList l;
  l << d_ptr->m_DataContainerName << d_ptr->m_AttributeMatrixName << d_ptr->m_DataArrayName;
  return l;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> DataArrayPath::getAsVector()
{
  QVector<QString> v(3);
  v[0] = d_ptr->m_DataContainerName;
  v[1] = d_ptr->m_AttributeMatrixName;
  v[2] = d_ptr->m_DataArrayName;
  return v;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::isEmpty() const
{
  if (d_ptr->m_DataContainerName.isEmpty() && d_ptr->m_AttributeMatrixName.isEmpty() && d_ptr->m_DataArrayName.isEmpty() )
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::isValid() const
{
  if (d_ptr->m_DataContainerName.isEmpty() == false && d_ptr->m_AttributeMatrixName.isEmpty() == false && d_ptr->m_DataArrayName.isEmpty() == false )
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DataArrayPath::GetDataArrayNames(const QVector<DataArrayPath>& paths)
{
  QList<QString> arrayNames;
  for (int i = 0; i < paths.count(); i++)
  {
    arrayNames.push_back(paths.at(i).getDataArrayName());
  }

  return arrayNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataArrayPath::GetAttributeMatrixPath(const QVector<DataArrayPath>& paths)
{
  if (paths.isEmpty())
  {
    return DataArrayPath();
  }
  else
  {
    return DataArrayPath(paths.first().getDataContainerName(), paths.first().getAttributeMatrixName(), "");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataArrayPath::split(QString NOT_USED) const
{
  QStringList l;
  l << d_ptr->m_DataContainerName << d_ptr->m_AttributeMatrixName << d_ptr->m_DataArrayName;
  return l;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::update(const QString& dcName, const QString& amName, const QString& daName)
{
  d_ptr->m_DataContainerName = dcName;
  d_ptr->m_AttributeMatrixName = amName;
  d_ptr->m_DataArrayName = daName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameDataContainer(const DataArrayPath& other) const
{
  return (d_ptr->m_DataContainerName == other.d_ptr->m_DataContainerName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameAttributeMatrix(const DataArrayPath& other) const
{
  return (d_ptr->m_AttributeMatrixName == other.d_ptr->m_AttributeMatrixName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameDataArray(const DataArrayPath& other) const
{
  return (d_ptr->m_DataArrayName == other.d_ptr->m_DataArrayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::hasSameAttributeMatrixPath(const DataArrayPath& other) const
{
  return(hasSameDataContainer(other) && hasSameAttributeMatrix(other));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::ValidateVector(const QVector<DataArrayPath>& other)
{
  QString dcName = "";
  QString amName = "";

  if (other.isEmpty())
  {
    return true;
  }
  else
  {
    dcName = other.first().getDataContainerName();
    amName = other.first().getAttributeMatrixName();
  }

  for (int i = 0; i < other.size(); ++i)
  {
    if (other.at(i).getDataContainerName() != dcName || other.at(i).getAttributeMatrixName() != amName)
    {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::writeJson(QJsonObject& json) const
{
  json["Data Container Name"] = getDataContainerName();
  json["Attribute Matrix Name"] = getAttributeMatrixName();
  json["Data Array Name"] = getDataArrayName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::readJson(QJsonObject& json) const
{
  if (json["Data Container Name"].isString() && json["Attribute Matrix Name"].isString() && json["Data Array Name"].isString())
  {
    d_ptr->m_DataContainerName = json["Data Container Name"].toString();
    d_ptr->m_AttributeMatrixName = json["Attribute Matrix Name"].toString();
    d_ptr->m_DataArrayName = json["Data Array Name"].toString();
    return true;
  }
  return false;
}



