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

#include "DREAM3DLib/DataContainers/EdgeDataContainer.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>


// DREAM3D Includes
#include "DREAM3DLib/Math/DREAM3DMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeDataContainer::EdgeDataContainer() :
Observable(),
m_NumVertexTuples(0),
m_NumEdgeTuples(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeDataContainer::~EdgeDataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DOES_DATASET_EXIST_DEFN(EdgeDataContainer, VertexData)
DOES_DATASET_EXIST_DEFN(EdgeDataContainer, EdgeData)
DOES_DATASET_EXIST_DEFN(EdgeDataContainer, FieldData)
DOES_DATASET_EXIST_DEFN(EdgeDataContainer, EnsembleData)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainer::buildMeshLinks()
{
  m_MeshLinks = MeshLinks::New();
  m_MeshLinks->generateMeshLinksEdges(m_Vertices, m_Edges);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainer::removeMeshLinks()
{
  m_MeshLinks = MeshLinks::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainer::setMeshLinks(MeshLinks::Pointer vertLinks)
{
  m_MeshLinks = vertLinks;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MeshLinks::Pointer EdgeDataContainer::getMeshLinks()
{
  return m_MeshLinks;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainer::addVertexData(const std::string &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    std::cout << "EdgeDataContainer::Adding Cell array with different array name than key name" << std::endl;
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
IDataArray::Pointer EdgeDataContainer::getVertexData(const std::string &name)
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
IDataArray::Pointer EdgeDataContainer::removeVertexData(const std::string &name)
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
void EdgeDataContainer::clearVertexData()
{
  m_VertexData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> EdgeDataContainer::getVertexArrayNameList()
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
int EdgeDataContainer::getNumVertexArrays()
{
  return static_cast<int>(m_VertexData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer EdgeDataContainer::getEdgeData(const std::string &name)
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
void EdgeDataContainer::addEdgeData(const std::string &name, IDataArray::Pointer data)
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
IDataArray::Pointer EdgeDataContainer::removeEdgeData(const std::string &name)
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
void EdgeDataContainer::clearEdgeData()
{
  m_EdgeData.clear();
  m_NumEdgeTuples = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> EdgeDataContainer::getEdgeArrayNameList()
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
int EdgeDataContainer::getNumEdgeArrays()
{
  return static_cast<int>(m_EdgeData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer EdgeDataContainer::getFieldData(const std::string &name)
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
void EdgeDataContainer::addFieldData(const std::string &name, IDataArray::Pointer data)
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
IDataArray::Pointer EdgeDataContainer::removeFieldData(const std::string &name)
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
void EdgeDataContainer::clearFieldData()
{
  m_FieldData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> EdgeDataContainer::getFieldArrayNameList()
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
int EdgeDataContainer::getNumFieldArrays()
{
  return static_cast<int>(m_FieldData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainer::resizeFieldDataArrays(size_t size)
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
IDataArray::Pointer EdgeDataContainer::getEnsembleData(const std::string &name)
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
void EdgeDataContainer::addEnsembleData(const std::string &name, IDataArray::Pointer data)
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
IDataArray::Pointer EdgeDataContainer::removeEnsembleData(const std::string &name)
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
void EdgeDataContainer::clearEnsembleData()
{
  m_EnsembleData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> EdgeDataContainer::getEnsembleArrayNameList()
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
int EdgeDataContainer::getNumEnsembleArrays()
{
  return static_cast<int>(m_EnsembleData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeDataContainer::resizeEnsembleDataArrays(size_t size)
{
 // int success = 0;
  for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_EnsembleData.begin(); iter != m_EnsembleData.end(); ++iter)
  {
    //std::cout << "Resizing Array '" << (*iter).first << "' : " << success << std::endl;
    IDataArray::Pointer d = (*iter).second;
    d->Resize(size);
  }
  m_NumEnsembleTuples = size;
}

