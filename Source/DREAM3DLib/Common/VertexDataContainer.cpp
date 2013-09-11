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

#include "DREAM3DLib/Common/VertexDataContainer.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>


// DREAM3D Includes
#include "DREAM3DLib/Common/DREAM3DMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexDataContainer::VertexDataContainer() :
Observable(),
m_NumVertexTuples(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexDataContainer::~VertexDataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DOES_DATASET_EXIST_DEFN(VertexDataContainer, VertexData)
DOES_DATASET_EXIST_DEFN(VertexDataContainer, VertexFieldData)
DOES_DATASET_EXIST_DEFN(VertexDataContainer, VertexEnsembleData)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainer::addVertexData(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "VertexDataContainer::Adding Cell array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_VertexData[name] = data;
  m_NumVertexTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VertexDataContainer::getVertexData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_VertexData.find(name);
  if ( it == m_VertexData.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VertexDataContainer::removeVertexData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_VertexData.find(name);
  if ( it == m_VertexData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_VertexData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainer::clearVertexData()
{
  m_VertexData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> VertexDataContainer::getVertexArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_VertexData.begin(); iter != m_VertexData.end(); ++iter)
  {
    keys.push_back( (*iter).first);
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainer::getNumVertexArrays()
{
  return static_cast<int>(m_VertexData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VertexDataContainer::getVertexFieldData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_VertexFieldData.find(name);
  if ( it == m_VertexFieldData.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainer::addVertexFieldData(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Field array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_VertexFieldData[name] = data;
  m_NumVertexFieldTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VertexDataContainer::removeVertexFieldData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_VertexFieldData.find(name);
  if ( it == m_VertexFieldData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_VertexFieldData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainer::clearVertexFieldData()
{
  m_VertexFieldData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> VertexDataContainer::getVertexFieldArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_VertexFieldData.begin(); iter != m_VertexFieldData.end(); ++iter)
  {
    keys.push_back( (*iter).first);
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainer::getNumVertexFieldArrays()
{
  return static_cast<int>(m_VertexFieldData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainer::resizeVertexFieldDataArrays(size_t size)
{
 // int success = 0;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_VertexFieldData.begin(); iter != m_VertexFieldData.end(); ++iter)
  {
    //qDebug() << "Resizing Array '" << (*iter).first << "' : " << success << "\n";
    IDataArray::Pointer d = (*iter).second;
    d->Resize(size);
  }
  m_NumVertexFieldTuples = size;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VertexDataContainer::getVertexEnsembleData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_VertexEnsembleData.find(name);
  if ( it == m_VertexEnsembleData.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainer::addVertexEnsembleData(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Ensemble array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_VertexEnsembleData[name] = data;
  m_NumVertexEnsembleTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VertexDataContainer::removeVertexEnsembleData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_VertexEnsembleData.find(name);
  if ( it == m_VertexEnsembleData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_VertexEnsembleData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainer::clearVertexEnsembleData()
{
  m_VertexEnsembleData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> VertexDataContainer::getVertexEnsembleArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_VertexEnsembleData.begin(); iter != m_VertexEnsembleData.end(); ++iter)
  {
    keys.push_back( (*iter).first);
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VertexDataContainer::getNumVertexEnsembleArrays()
{
  return static_cast<int>(m_VertexEnsembleData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexDataContainer::resizeVertexEnsembleDataArrays(size_t size)
{
 // int success = 0;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_VertexEnsembleData.begin(); iter != m_VertexEnsembleData.end(); ++iter)
  {
    //qDebug() << "Resizing Array '" << (*iter).first << "' : " << success << "\n";
    IDataArray::Pointer d = (*iter).second;
    d->Resize(size);
  }
  m_NumVertexEnsembleTuples = size;
}

