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
m_NumVertexTuples(0),
m_NumEdgeTuples(0),
m_NumFaceTuples(0),
m_NumCellTuples(0),
m_NumFieldTuples(0),
m_NumEnsembleTuples(0)
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
DOES_DATASET_EXIST_DEFN(VolumeDataContainer, VertexData)
DOES_DATASET_EXIST_DEFN(VolumeDataContainer, EdgeData)
DOES_DATASET_EXIST_DEFN(VolumeDataContainer, FaceData)
DOES_DATASET_EXIST_DEFN(VolumeDataContainer, CellData)
DOES_DATASET_EXIST_DEFN(VolumeDataContainer, FieldData)
DOES_DATASET_EXIST_DEFN(VolumeDataContainer, EnsembleData)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::addVertexData(const std::string &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    std::cout << "Adding Vertex array with different array name than key name" << std::endl;
    std::cout << "Key name: " << name << std::endl;
    std::cout << "Array Name:" << data->GetName() << std::endl;
    data->SetName(name);
  }
  m_VertexData[name] = data;
  m_NumVertexTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::getVertexData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_VertexData.find(name);
  if ( it == m_VertexData.end() )
  {
    return IDataArray::NullPointer();
  }
  return (*it).second;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::removeVertexData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_VertexData.find(name);
  if ( it == m_VertexData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = (*it).second;
  m_VertexData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VolumeDataContainer::renameVertexData(const std::string &oldname, const std::string &newname)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_VertexData.find(oldname);
  if ( it == m_VertexData.end() )
  {
	  return false;
  }
  IDataArray::Pointer p = (*it).second;
  p->SetName(newname);
  removeVertexData(oldname);
  addVertexData(newname, p);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::clearVertexData()
{
  m_VertexData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> VolumeDataContainer::getVertexArrayNameList()
{
  std::list<std::string> keys;
  for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_VertexData.begin(); iter != m_VertexData.end(); ++iter)
  {
    keys.push_back( (*iter).first);
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::getNumVertexArrays()
{
  return static_cast<int>(m_VertexData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::addEdgeData(const std::string &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    std::cout << "Adding Edge array with different array name than key name" << std::endl;
    std::cout << "Key name: " << name << std::endl;
    std::cout << "Array Name:" << data->GetName() << std::endl;
    data->SetName(name);
  }
  m_EdgeData[name] = data;
  m_NumEdgeTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::getEdgeData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_EdgeData.find(name);
  if ( it == m_EdgeData.end() )
  {
    return IDataArray::NullPointer();
  }
  return (*it).second;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::removeEdgeData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_EdgeData.find(name);
  if ( it == m_EdgeData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = (*it).second;
  m_EdgeData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VolumeDataContainer::renameEdgeData(const std::string &oldname, const std::string &newname)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_EdgeData.find(oldname);
  if ( it == m_EdgeData.end() )
  {
	  return false;
  }
  IDataArray::Pointer p = (*it).second;
  p->SetName(newname);
  removeEdgeData(oldname);
  addEdgeData(newname, p);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::clearEdgeData()
{
  m_EdgeData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> VolumeDataContainer::getEdgeArrayNameList()
{
  std::list<std::string> keys;
  for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_EdgeData.begin(); iter != m_EdgeData.end(); ++iter)
  {
    keys.push_back( (*iter).first);
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::getNumEdgeArrays()
{
  return static_cast<int>(m_EdgeData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::addFaceData(const std::string &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    std::cout << "Adding Face array with different array name than key name" << std::endl;
    std::cout << "Key name: " << name << std::endl;
    std::cout << "Array Name:" << data->GetName() << std::endl;
    data->SetName(name);
  }
  m_FaceData[name] = data;
  m_NumFaceTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::getFaceData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_FaceData.find(name);
  if ( it == m_FaceData.end() )
  {
    return IDataArray::NullPointer();
  }
  return (*it).second;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::removeFaceData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_FaceData.find(name);
  if ( it == m_FaceData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = (*it).second;
  m_FaceData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VolumeDataContainer::renameFaceData(const std::string &oldname, const std::string &newname)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_FaceData.find(oldname);
  if ( it == m_FaceData.end() )
  {
	  return false;
  }
  IDataArray::Pointer p = (*it).second;
  p->SetName(newname);
  removeFaceData(oldname);
  addFaceData(newname, p);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::clearFaceData()
{
  m_FaceData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> VolumeDataContainer::getFaceArrayNameList()
{
  std::list<std::string> keys;
  for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_FaceData.begin(); iter != m_FaceData.end(); ++iter)
  {
    keys.push_back( (*iter).first);
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::getNumFaceArrays()
{
  return static_cast<int>(m_FaceData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::addCellData(const std::string &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    std::cout << "Adding Cell array with different array name than key name" << std::endl;
    std::cout << "Key name: " << name << std::endl;
    std::cout << "Array Name:" << data->GetName() << std::endl;
    data->SetName(name);
  }
  m_CellData[name] = data;
  m_NumCellTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::getCellData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_CellData.find(name);
  if ( it == m_CellData.end() )
  {
    return IDataArray::NullPointer();
  }
  return (*it).second;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::removeCellData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_CellData.find(name);
  if ( it == m_CellData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = (*it).second;
  m_CellData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VolumeDataContainer::renameCellData(const std::string &oldname, const std::string &newname)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_CellData.find(oldname);
  if ( it == m_CellData.end() )
  {
	  return false;
  }
  IDataArray::Pointer p = (*it).second;
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
std::list<std::string> VolumeDataContainer::getCellArrayNameList()
{
  std::list<std::string> keys;
  for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_CellData.begin(); iter != m_CellData.end(); ++iter)
  {
    keys.push_back( (*iter).first);
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
IDataArray::Pointer VolumeDataContainer::getFieldData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_FieldData.find(name);
  if ( it == m_FieldData.end() )
  {
    return IDataArray::NullPointer();
  }
  return (*it).second;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::addFieldData(const std::string &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    std::cout << "Adding Field array with different array name than key name" << std::endl;
    std::cout << "Key name: " << name << std::endl;
    std::cout << "Array Name:" << data->GetName() << std::endl;
    data->SetName(name);
  }
  m_FieldData[name] = data;
  m_NumFieldTuples = data->GetNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::removeFieldData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_FieldData.find(name);
  if ( it == m_FieldData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = (*it).second;
  m_FieldData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VolumeDataContainer::renameFieldData(const std::string &oldname, const std::string &newname)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_FieldData.find(oldname);
  if ( it == m_FieldData.end() )
  {
	  return false;
  }
  IDataArray::Pointer p = (*it).second;
  p->SetName(newname);
  removeFieldData(oldname);
  addFieldData(newname, p);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::clearFieldData()
{
  m_FieldData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> VolumeDataContainer::getFieldArrayNameList()
{
  std::list<std::string> keys;
  for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_FieldData.begin(); iter != m_FieldData.end(); ++iter)
  {
    keys.push_back( (*iter).first);
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::getNumFieldArrays()
{
  return static_cast<int>(m_FieldData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::resizeFieldDataArrays(size_t size)
{
 // int success = 0;
  for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_FieldData.begin(); iter != m_FieldData.end(); ++iter)
  {
    //std::cout << "Resizing Array '" << (*iter).first << "' : " << success << std::endl;
    IDataArray::Pointer d = (*iter).second;
    d->Resize(size);
  }
  m_NumFieldTuples = size;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::getEnsembleData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_EnsembleData.find(name);
  if ( it == m_EnsembleData.end() )
  {
    return IDataArray::NullPointer();
  }
  return (*it).second;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::addEnsembleData(const std::string &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    std::cout << "Adding Ensemble array with different array name than key name" << std::endl;
    std::cout << "Key name: " << name << std::endl;
    std::cout << "Array Name:" << data->GetName() << std::endl;
    data->SetName(name);
  }
  m_EnsembleData[name] = data;
  m_NumEnsembleTuples = data->GetNumberOfTuples();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer VolumeDataContainer::removeEnsembleData(const std::string &name)
{
  std::map<std::string, IDataArray::Pointer>::iterator it;
  it =  m_EnsembleData.find(name);
  if ( it == m_EnsembleData.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = (*it).second;
  m_EnsembleData.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeDataContainer::clearEnsembleData()
{
  m_EnsembleData.clear();
  m_NumEnsembleTuples = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> VolumeDataContainer::getEnsembleArrayNameList()
{
  std::list<std::string> keys;
  for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_EnsembleData.begin(); iter != m_EnsembleData.end(); ++iter)
  {
    keys.push_back( (*iter).first);
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VolumeDataContainer::getNumEnsembleArrays()
{
  return static_cast<int>(m_EnsembleData.size());
}


