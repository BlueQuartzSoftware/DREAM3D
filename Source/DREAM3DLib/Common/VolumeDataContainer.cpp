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

#include "DREAM3DLib/Common/VolumeDataContainer.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>

// EbsdLib Includes
#include "EbsdLib/EbsdConstants.h"

// DREAM3D Includes
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeDataContainer::VolumeDataContainer() :
m_NumCellTuples(0),
m_NumCellFieldTuples(0),
m_NumCellEnsembleTuples(0)
{
  m_Dimensions[0] = 0; m_Dimensions[1] = 0; m_Dimensions[2] = 0;
  m_Resolution[0] = 1.0f; m_Resolution[1] = 1.0f; m_Resolution[2] = 1.0f;
  m_Origin[0] = 0.0f; m_Origin[1] = 0.0f; m_Origin[2] = 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeDataContainer::~VolumeDataContainer()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DOES_DATASET_EXIST_DEFN(VolumeDataContainer, CellData)
DOES_DATASET_EXIST_DEFN(VolumeDataContainer, CellFieldData)
DOES_DATASET_EXIST_DEFN(VolumeDataContainer, CellEnsembleData)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::addCellData(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Cell array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_CellData[name] = data;
  m_NumCellTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::getCellData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_CellData.find(name);
  if ( it == m_CellData.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::removeCellData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_CellData.find(name);
  if ( it == m_CellData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_CellData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VolumeDataContainer::renameCellData(const QString &oldname, const QString &newname)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_CellData.find(oldname);
  if ( it == m_CellData.end() )
  {
    return false;
  }
  IDataArray::Pointer p = it.value();
  p->SetName(newname);
  removeCellData(oldname);
  addCellData(newname, p);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::clearCellData()
{
  m_CellData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> VolumeDataContainer::getCellArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_CellData.begin(); iter != m_CellData.end(); ++iter)
  {
    keys.push_back( iter.key() );
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::getNumCellArrays()
{
  return static_cast<int>(m_CellData.size());
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::getCellFieldData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_CellFieldData.find(name);
  if ( it == m_CellFieldData.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::addCellFieldData(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Field array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_CellFieldData[name] = data;
  m_NumCellFieldTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::removeCellFieldData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_CellFieldData.find(name);
  if ( it == m_CellFieldData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_CellFieldData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VolumeDataContainer::renameCellFieldData(const QString &oldname, const QString &newname)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_CellFieldData.find(oldname);
  if ( it == m_CellFieldData.end() )
  {
    return false;
  }
  IDataArray::Pointer p = it.value();
  p->SetName(newname);
  removeCellFieldData(oldname);
  addCellFieldData(newname, p);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::clearCellFieldData()
{
  m_CellFieldData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> VolumeDataContainer::getCellFieldArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_CellFieldData.begin(); iter != m_CellFieldData.end(); ++iter)
  {
    keys.push_back(iter.key());
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::getNumCellFieldArrays()
{
  return static_cast<int>(m_CellFieldData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::resizeCellFieldDataArrays(size_t size)
{
 // int success = 0;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_CellFieldData.begin(); iter != m_CellFieldData.end(); ++iter)
  {
    //qDebug() << "Resizing Array '" << iter.key() << "' : " << success << "\n";
    IDataArray::Pointer d = iter.value();
    d->Resize(size);
  }
  m_NumCellFieldTuples = size;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::getCellEnsembleData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_CellEnsembleData.find(name);
  if ( it == m_CellEnsembleData.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::addCellEnsembleData(const QString &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Ensemble array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_CellEnsembleData[name] = data;
  m_NumCellEnsembleTuples = data->GetNumberOfTuples();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::removeCellEnsembleData(const QString &name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_CellEnsembleData.find(name);
  if ( it == m_CellEnsembleData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_CellEnsembleData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::clearCellEnsembleData()
{
  m_CellEnsembleData.clear();
  m_NumCellEnsembleTuples = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> VolumeDataContainer::getCellEnsembleArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_CellEnsembleData.begin(); iter != m_CellEnsembleData.end(); ++iter)
  {
    keys.push_back(iter.key());
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::getNumCellEnsembleArrays()
{
  return static_cast<int>(m_CellEnsembleData.size());
}


