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
#include "BinaryNodesTrianglesReader.h"

#include <iostream>
#include <string>
#include <sstream>


#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "BinaryNodesTrianglesReader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BinaryNodesTrianglesReader::BinaryNodesTrianglesReader() :
  SurfaceMeshFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BinaryNodesTrianglesReader::~BinaryNodesTrianglesReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
    {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Binary Nodes Input File");
    option->setPropertyName("BinaryNodesFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
    {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Binary Triangles Input File");
    option->setPropertyName("BinaryTrianglesFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
 writer->writeValue("BinaryNodesFile", getBinaryNodesFile() );
 writer->writeValue("BinaryTrianglesFile", getBinaryTrianglesFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }

  if (getBinaryNodesFile().empty() == true)
  {
    std::stringstream ss;
    ss << ClassName() << " needs the Binary Nodes File path set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -4);
    setErrorCondition(-387);
  }

  if (getBinaryNodesFile().empty() == true)
  {
    std::stringstream ss;
    ss << ClassName() << " needs the Binary Nodes File path set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -4);
    setErrorCondition(-387);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryNodesTrianglesReader::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);

  /* Make sure everything is in place */
  dataCheck(false, 1, 1, 1);


  /* Place all your code to execute your filter here. */
  err = read();
  setErrorCondition(err);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BinaryNodesTrianglesReader::read()
{

  SurfaceMeshDataContainer *sm = getSurfaceMeshDataContainer();
  int err = 0;
  setErrorCondition(err);

  std::stringstream s;
  // Open the Nodes file for reading
  FILE* nodesFile = fopen(m_BinaryNodesFile.c_str(), "rb+");
  if(nodesFile == NULL)
  {
    s.str("");
    s << "Error opening nodes file '" << m_BinaryNodesFile << "'";
    setErrorCondition(786);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }
  ScopedFileMonitor nodesMonitor(nodesFile);

  // Calculate how many nodes are in the file based on the file size
  fseek(nodesFile, 0, SEEK_END);
  size_t fLength = ftell(nodesFile);
  size_t nNodes = fLength / SurfaceMesh::NodesFile::ByteCount;
  fseek(nodesFile, 0, SEEK_SET);
  fLength = ftell(nodesFile);
  if(0 != fLength)
  {
    s.str("");
    s << getNameOfClass() << ": Error Could not rewind to beginning of file after nodes count.'" << m_BinaryNodesFile << "'";
    setErrorCondition(787);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }
  s.str("");
  s << "Calc Node Count from Nodes.bin File: " << nNodes;
  notifyStatusMessage(s.str());

  // Open the triangles file for reading
  FILE* triFile = fopen(m_BinaryTrianglesFile.c_str(), "rb+");
  if(triFile == NULL)
  {
    s.str("");
    s << getNameOfClass() << ": Error opening Triangles file '" << m_BinaryTrianglesFile << "'";
    setErrorCondition(788);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }

  ScopedFileMonitor trianglesMonitor(triFile);
  // Calculate how many Triangles are in the file based in the file size
  fseek(triFile, 0, SEEK_END);
  fLength = ftell(triFile);
  size_t nTriangles = fLength / SurfaceMesh::TrianglesFile::ByteCount;
  fseek(triFile, 0, SEEK_SET);
  fLength = ftell(triFile);
  if(0 != fLength)
  {
    s.str("");
    s << getNameOfClass() << ": Error Could not rewind to beginning of file after triangles count.'" << m_BinaryTrianglesFile << "'";
    setErrorCondition(789);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }
  s.str("");
  s << "Calc Triangle Count from Triangles.bin File: " << nTriangles;
  notifyStatusMessage(s.str());

  // Allocate all the nodes
  typedef SurfaceMesh::DataStructures::Vert_t Vert_t;
  StructArray<Vert_t>::Pointer m_NodeListPtr = StructArray<Vert_t>::CreateArray(nNodes, DREAM3D::PointData::SurfaceMeshNodes);
  Vert_t* m_NodeList = m_NodeListPtr->GetPointer(0);

  Int8ArrayType::Pointer nodeTypePtr = Int8ArrayType::CreateArray(nNodes, 1, DREAM3D::PointData::SurfaceMeshNodeType);
  nodeTypePtr->initializeWithZeros();
  int8_t* nodeType = nodeTypePtr->GetPointer(0);

  s.str("");
  s << "Reading Nodes file into Memory";
  notifyStatusMessage(s.str());
  size_t nread = 0;
  SurfaceMesh::NodesFile::NodesFileRecord_t nRecord;

  for (size_t i = 0; i < nNodes; i++)
  {

    nread = fread(&nRecord, SurfaceMesh::NodesFile::ByteCount, 1, nodesFile); // Read one set of positions from the nodes file
    if(nread != 1)
    {
      break;
    }
    SurfaceMesh::DataStructures::Vert_t& node = m_NodeList[nRecord.nodeId];
    node.pos[0] = nRecord.x;
    node.pos[1] = nRecord.y;
    node.pos[2] = nRecord.z;
    nodeType[nRecord.nodeId] = nRecord.nodeKind;
  }

  s.str("");
  s << "Reading Triangles file into Memory";
  notifyStatusMessage(s.str());

  // Allocate all the Triangle Objects
  typedef SurfaceMesh::DataStructures::Face_t Face_t;
  StructArray<Face_t>::Pointer m_TriangleListPtr = StructArray<Face_t>::CreateArray(nTriangles, DREAM3D::FaceData::SurfaceMeshTriangles);
  Face_t* m_TriangleList = m_TriangleListPtr->GetPointer(0);
  ::memset(m_TriangleList, 0xAB, sizeof(Face_t) * nTriangles);

  DataArray<int32_t>::Pointer faceLabelPtr = DataArray<int32_t>::CreateArray(nTriangles, 2, DREAM3D::FaceData::SurfaceMeshTriangleLabels);
  int32_t* faceLabels = faceLabelPtr->GetPointer(0);
  faceLabelPtr->initializeWithZeros();


  SurfaceMesh::TrianglesFile::TrianglesFileRecord_t tRecord;
  for (size_t i = 0; i < nTriangles; i++)
  {
    // Read from the Input Triangles Temp File
    nread = fread(&tRecord, SurfaceMesh::TrianglesFile::ByteCount, 1, triFile);
    if(nread != 1)
    {
      break;
    }

    SurfaceMesh::DataStructures::Face_t& triangle = m_TriangleList[tRecord.triId];

    triangle.verts[0] = tRecord.nodeId_0;
    triangle.verts[1] = tRecord.nodeId_1;
    triangle.verts[2] = tRecord.nodeId_2;
    faceLabels[tRecord.triId * 2] = tRecord.label_0;
    faceLabels[tRecord.triId * 2 + 1] = tRecord.label_1;
  }

  sm->setVertices(m_NodeListPtr);
  sm->setFaces(m_TriangleListPtr);
  sm->addFaceData(faceLabelPtr->GetName(), faceLabelPtr);
  sm->addPointData(nodeTypePtr->GetName(), nodeTypePtr);





  // The ScopedFileMonitor classes will take care of closing the files

  return getErrorCondition();
}


