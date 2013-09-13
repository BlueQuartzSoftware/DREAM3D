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

#include "SurfaceDataContainer.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>


// DREAM3D Includes
#include "DREAM3DLib/Math/DREAM3DMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceDataContainer::SurfaceDataContainer() :
EdgeDataContainer(),
m_NumFaceTuples(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceDataContainer::~SurfaceDataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DOES_DATASET_EXIST_DEFN(SurfaceDataContainer, FaceData)
DOES_DATASET_EXIST_DEFN(SurfaceDataContainer, FaceFieldData)
DOES_DATASET_EXIST_DEFN(SurfaceDataContainer, FaceEnsembleData)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer SurfaceDataContainer::getFaceData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_FaceData.find(name);
  if ( it == m_FaceData.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::addFaceData(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Field array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_FaceData[name] = data;
  m_NumFaceTuples = data->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer SurfaceDataContainer::removeFaceData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_FaceData.find(name);
  if ( it == m_FaceData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_FaceData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::clearFaceData()
{
  m_FaceData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> SurfaceDataContainer::getFaceArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_FaceData.begin(); iter != m_FaceData.end(); ++iter)
  {
    keys.push_back( iter.key() );
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainer::getNumFaceArrays()
{
  return static_cast<int>(m_FaceData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::resizeFaceDataArrays(size_t size)
{
 // int success = 0;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_FaceData.begin(); iter != m_FaceData.end(); ++iter)
  {
    //qDebug() << "Resizing Array '" << iter.key() << "' : " << success << "\n";
    IDataArray::Pointer d = iter.value();
    d->Resize(size);
  }
  m_NumFaceTuples = size;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer SurfaceDataContainer::getFaceFieldData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_FaceFieldData.find(name);
  if ( it == m_FaceFieldData.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::addFaceFieldData(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Field array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_FaceFieldData[name] = data;
  m_NumFaceFieldTuples = data->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer SurfaceDataContainer::removeFaceFieldData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_FaceFieldData.find(name);
  if ( it == m_FaceFieldData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_FaceFieldData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::clearFaceFieldData()
{
  m_FaceFieldData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> SurfaceDataContainer::getFaceFieldArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_FaceFieldData.begin(); iter != m_FaceFieldData.end(); ++iter)
  {
    keys.push_back( iter.key());
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainer::getNumFaceFieldArrays()
{
  return static_cast<int>(m_FaceFieldData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::resizeFaceFieldDataArrays(size_t size)
{
 // int success = 0;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_FaceFieldData.begin(); iter != m_FaceFieldData.end(); ++iter)
  {
    //qDebug() << "Resizing Array '" << iter.key() << "' : " << success << "\n";
    IDataArray::Pointer d = iter.value();
    d->Resize(size);
  }
  m_NumFaceFieldTuples = size;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer SurfaceDataContainer::getFaceEnsembleData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_FaceEnsembleData.find(name);
  if ( it == m_FaceEnsembleData.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::addFaceEnsembleData(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Ensemble array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_FaceEnsembleData[name] = data;
  m_NumFaceEnsembleTuples = data->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer SurfaceDataContainer::removeFaceEnsembleData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_FaceEnsembleData.find(name);
  if ( it == m_FaceEnsembleData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_FaceEnsembleData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::clearFaceEnsembleData()
{
  m_FaceEnsembleData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> SurfaceDataContainer::getFaceEnsembleArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_FaceEnsembleData.begin(); iter != m_FaceEnsembleData.end(); ++iter)
  {
    keys.push_back( iter.key());
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainer::getNumFaceEnsembleArrays()
{
  return static_cast<int>(m_FaceEnsembleData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainer::resizeFaceEnsembleDataArrays(size_t size)
{
 // int success = 0;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_FaceEnsembleData.begin(); iter != m_FaceEnsembleData.end(); ++iter)
  {
    //qDebug() << "Resizing Array '" << iter.key() << "' : " << success << "\n";
    IDataArray::Pointer d = iter.value();
    d->Resize(size);
  }
  m_NumFaceEnsembleTuples = size;
}

