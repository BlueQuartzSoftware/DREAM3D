/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "DataArrayPath.h"



#include "DREAM3DLib/Common/Constants.h"

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
class DataArrayPathPrivate {
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

  if(tokens.size() > 0) { d_ptr->m_DataContainerName = tokens.at(0); }
  if(tokens.size() > 1) { d_ptr->m_AttributeMatrixName = tokens.at(1); }
  if(tokens.size() > 2) { d_ptr->m_DataArrayName = tokens.at(2); }
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_PIMPL_PROPERTY_DEF(DataArrayPath, QString, DataContainerName)
DREAM3D_PIMPL_PROPERTY_DEF(DataArrayPath, QString, AttributeMatrixName)
DREAM3D_PIMPL_PROPERTY_DEF(DataArrayPath, QString, DataArrayName)

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
QString DataArrayPath::serialize(QString delimiter) const
{
    QString s = d_ptr->m_DataContainerName + delimiter + d_ptr->m_AttributeMatrixName + delimiter + d_ptr->m_DataArrayName;
  return s;
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
  { return true; }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::isValid() const
{
  if (d_ptr->m_DataContainerName.isEmpty() == false && d_ptr->m_AttributeMatrixName.isEmpty() == false && d_ptr->m_DataArrayName.isEmpty() == false )
  { return true; }
  return false;
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
