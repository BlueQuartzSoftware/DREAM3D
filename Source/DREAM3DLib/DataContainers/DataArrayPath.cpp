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


QDebug operator<<(QDebug out, const DataArrayPath& v)
{
  out << v.getDataContainerName() << "|" << v.getAttributeMatrixName() << "|" << v.getDataArrayName();
  return out;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const QString& dcName, const QString& amName, const QString& daName) :
  m_DataContainerName(dcName),
  m_AttributeMatrixName(amName),
  m_DataArrayName(daName)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const QString& path)
{
  QStringList tokens = path.split(DREAM3D::PathSep);

  if(tokens.size() > 0) { m_DataContainerName = tokens.at(0); }
  if(tokens.size() > 1) { m_AttributeMatrixName = tokens.at(1); }
  if(tokens.size() > 2) { m_DataArrayName = tokens.at(2); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath::DataArrayPath(const DataArrayPath& rhs)
{
  m_DataContainerName = rhs.m_DataContainerName;
  m_AttributeMatrixName = rhs.m_AttributeMatrixName;
  m_DataArrayName = rhs.m_DataArrayName;
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
void DataArrayPath::operator=(const DataArrayPath& rhs)
{
  m_DataContainerName = rhs.m_DataContainerName;
  m_AttributeMatrixName = rhs.m_AttributeMatrixName;
  m_DataArrayName = rhs.m_DataArrayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPath::serialize(QString delimiter) const
{
  QString s = m_DataContainerName + delimiter + m_AttributeMatrixName + delimiter + m_DataArrayName;
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataArrayPath::getAsStringList()
{
  QStringList l;
  l << m_DataContainerName << m_AttributeMatrixName << m_DataArrayName;
  return l;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> DataArrayPath::getAsVector()
{
  QVector<QString> v(3);
  v[0] = m_DataContainerName;
  v[1] = m_AttributeMatrixName;
  v[2] = m_DataArrayName;
  return v;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::isEmpty() const
{
  if (m_DataContainerName.isEmpty() && m_AttributeMatrixName.isEmpty() && m_DataArrayName.isEmpty() )
  { return true; }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPath::isValid() const
{
  if (m_DataContainerName.isEmpty() == false && m_AttributeMatrixName.isEmpty() == false && m_DataArrayName.isEmpty() == false )
  { return true; }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataArrayPath::split(QString NOT_USED) const
{
  QStringList l;
  l << m_DataContainerName << m_AttributeMatrixName << m_DataArrayName;
  return l;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPath::update(const QString& dcName, const QString& amName, const QString& daName)
{
  m_DataContainerName = dcName;
  m_AttributeMatrixName = amName;
  m_DataArrayName = daName;
}
