/* ============================================================================
 * Copyright (c) 2009, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2009, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "DREAM3DLib/DataContainers/AttributeMatrix.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>

// EbsdLib Includes
#include "EbsdLib/EbsdConstants.h"

// DREAM3D Includes
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::AttributeMatrix() :
m_NumTuples(0)
{
  setName(DREAM3D::HDF5::AttributeMatrixName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::~AttributeMatrix()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::doesAttributeArrayExist(const QString &name)
{
  return  m_AttributeMatrix.contains(name);\
}
  
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::addAttributeArray(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Attribute Array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_AttributeMatrix[name] = data;
  m_NumTuples = data->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer AttributeMatrix::getAttributeArray(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_AttributeMatrix.find(name);
  if ( it == m_AttributeMatrix.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer AttributeMatrix::removeAttributeArray(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_AttributeMatrix.find(name);
  if ( it == m_AttributeMatrix.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_AttributeMatrix.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::renameAttributeArray(const QString &oldname, const QString &newname)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_AttributeMatrix.find(oldname);
  if ( it == m_AttributeMatrix.end() )
  {
    return false;
  }
  IDataArray::Pointer p = it.value();
  p->SetName(newname);
  removeAttributeArray(oldname);
  addAttributeArray(newname, p);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::resizeAttributeArrays(size_t size)
{
// int success = 0;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeMatrix.begin(); iter != m_AttributeMatrix.end(); ++iter)
  {
    //std::cout << "Resizing Array '" << (*iter).first << "' : " << success << std::endl;
    IDataArray::Pointer d = iter.value();
    d->Resize(size);
  }
  m_NumTuples = size;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::clearAttributeArrays()
{
  m_AttributeMatrix.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> AttributeMatrix::getAttributeArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeMatrix.begin(); iter != m_AttributeMatrix.end(); ++iter)
  {
    keys.push_back( iter.key() );
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::getNumAttributeArrays()
{
  return static_cast<int>(m_AttributeMatrix.size());
}