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

#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>


// DREAM3D Includes
#include "DREAM3DLib/Common/DREAM3DMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshDataContainer::SurfaceMeshDataContainer() :
Observable(),
m_NumVertexTuples(0),
m_NumFaceTuples(0),
m_NumEdgeTuples(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshDataContainer::~SurfaceMeshDataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DOES_DATASET_EXIST_DEFN(SurfaceMeshDataContainer, VertexData)
DOES_DATASET_EXIST_DEFN(SurfaceMeshDataContainer, FaceData)
DOES_DATASET_EXIST_DEFN(SurfaceMeshDataContainer, EdgeData)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainer::buildMeshVertLinks()
{
  m_MeshVertLinks = MeshVertLinks::New();
  m_MeshVertLinks->generateMeshVertLinks(m_Vertices, m_Faces);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainer::removeMeshVertLinks()
{
  m_MeshVertLinks = MeshVertLinks::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainer::buildMeshFaceNeighborLists()
{
  bool deleteMeshVertLinks = false;
  if (m_MeshVertLinks.get() == NULL)
  {
    buildMeshVertLinks();
    deleteMeshVertLinks = true;
  }
  m_FaceNeighbors = MeshFaceNeighbors::New();
  m_FaceNeighbors->generateNeighborLists(m_Vertices, m_Faces, m_MeshVertLinks);
  if (deleteMeshVertLinks == true)
  {
    m_MeshVertLinks = MeshVertLinks::NullPointer();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainer::removeMeshFaceNeighborLists()
{
  m_FaceNeighbors = MeshFaceNeighbors::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainer::setMeshVertLinks(MeshVertLinks::Pointer vertLinks)
{
  m_MeshVertLinks = vertLinks;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MeshVertLinks::Pointer SurfaceMeshDataContainer::getMeshVertLinks()
{
  return m_MeshVertLinks;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MeshFaceNeighbors::Pointer SurfaceMeshDataContainer::getMeshFaceNeighborLists()
{
  return m_FaceNeighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainer::setMeshFaceNeighborLists(MeshFaceNeighbors::Pointer neighbors)
{
  m_FaceNeighbors = neighbors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainer::addVertexData(const std::string &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    std::cout << "SurfaceMeshDataContainer::Adding Cell array with different array name than key name" << std::endl;
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
IDataArray::Pointer SurfaceMeshDataContainer::getVertexData(const std::string &name)
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
IDataArray::Pointer SurfaceMeshDataContainer::removeVertexData(const std::string &name)
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
void SurfaceMeshDataContainer::clearVertexData()
{
  m_VertexData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> SurfaceMeshDataContainer::getPointArrayNameList()
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
int SurfaceMeshDataContainer::getNumPointArrays()
{
  return static_cast<int>(m_VertexData.size());
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer SurfaceMeshDataContainer::getFaceData(const std::string &name)
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
void SurfaceMeshDataContainer::addFaceData(const std::string &name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    std::cout << "Adding Field array with different array name than key name" << std::endl;
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
IDataArray::Pointer SurfaceMeshDataContainer::removeFaceData(const std::string &name)
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
void SurfaceMeshDataContainer::clearFaceData()
{
  m_FaceData.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> SurfaceMeshDataContainer::getFaceArrayNameList()
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
int SurfaceMeshDataContainer::getNumFaceArrays()
{
  return static_cast<int>(m_FaceData.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshDataContainer::resizeFaceDataArrays(size_t size)
{
 // int success = 0;
  for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_FaceData.begin(); iter != m_FaceData.end(); ++iter)
  {
    //std::cout << "Resizing Array '" << (*iter).first << "' : " << success << std::endl;
    IDataArray::Pointer d = (*iter).second;
    d->Resize(size);
  }
  m_NumFaceTuples = size;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer SurfaceMeshDataContainer::getEdgeData(const std::string &name)
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
void SurfaceMeshDataContainer::addEdgeData(const std::string &name, IDataArray::Pointer data)
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
IDataArray::Pointer SurfaceMeshDataContainer::removeEdgeData(const std::string &name)
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
void SurfaceMeshDataContainer::clearEdgeData()
{
  m_EdgeData.clear();
  m_NumEdgeTuples = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::list<std::string> SurfaceMeshDataContainer::getEdgeArrayNameList()
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
int SurfaceMeshDataContainer::getNumEdgeArrays()
{
  return static_cast<int>(m_EdgeData.size());
}


