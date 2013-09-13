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
#include "NodesTrianglesToStl.h"

#include <boost/shared_array.hpp>

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/DataContainers/MeshStructs.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesToStl::NodesTrianglesToStl() :
AbstractFilter()
{
  setupFilterParameters();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesToStl::~NodesTrianglesToStl()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Nodes File");
     option->setPropertyName("NodesFile");
     option->setWidgetType(FilterParameter::InputFileWidget);
     option->setValueType("string");
     parameters.push_back(option);
   }
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Triangles File");
     option->setPropertyName("TrianglesFile");
     option->setWidgetType(FilterParameter::InputFileWidget);
     option->setValueType("string");
     parameters.push_back(option);
   }
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Output STL Directory");
     option->setPropertyName("OutputStlDirectory");
     option->setWidgetType(FilterParameter::OutputPathWidget);
     option->setValueType("string");
     parameters.push_back(option);
   }
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Stl File Prefix");
     option->setPropertyName("OutputStlPrefix");
     option->setWidgetType(FilterParameter::StringWidget);
     option->setValueType("string");
     parameters.push_back(option);
   }


   setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setNodesFile( reader->readValue( "NodesFile", getNodesFile() ) );
  setTrianglesFile( reader->readValue( "TrianglesFile", getTrianglesFile() ) );
  setOutputStlDirectory( reader->readValue( "OutputStlDirectory", getOutputStlDirectory() ) );
  setOutputStlPrefix( reader->readValue( "OutputStlPrefix", getOutputStlPrefix() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToStl::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("NodesFile", getNodesFile() );
  writer->writeValue("TrianglesFile", getTrianglesFile() );
  writer->writeValue("OutputStlDirectory", getOutputStlDirectory() );
  writer->writeValue("OutputStlPrefix", getOutputStlPrefix() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  QFileInfo fi(m_TrianglesFile);

  if (m_TrianglesFile.isEmpty() == true)
  {
    setErrorCondition(-1001);
    addErrorMessage(getHumanLabel(), "Triangles file path or name is emtpy", -1001);
  }
  else if (fi.exists() == false)
  {

    if (preflight == true)
    addWarningMessage(getHumanLabel(), "Triangles file does not exist currently.\nYou must have another filter that creates these files before this filter in your pipeline", -1004);
    else {
    setErrorCondition(-1001);
    addErrorMessage(getHumanLabel(), "Triangles file does not exist currently.\nYou must have another filter that creates these files before this filter in your pipeline", -1004);
    }
  }

  QFileInfo fii(m_NodesFile);
  if (m_NodesFile.isEmpty() == true)
  {
    setErrorCondition(-1002);
    addErrorMessage(getHumanLabel(), "Nodes file path or name is emtpy", -1002);
  }
  else if (fii.exists() == false)
  {

    if (preflight == true)
    addWarningMessage(getHumanLabel(), "Nodes file does not exist currently. You must have another filter that creates these files before this filter in your pipeline", -1005);
    else {
      setErrorCondition(-1002);
      addErrorMessage(getHumanLabel(), "Nodes file does not exist currently. You must have another filter that creates these files before this filter in your pipeline", -1005);
    }
  }

  if (m_OutputStlDirectory.isEmpty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Stl Output Directory is Not set correctly", -1003);
  }



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::preflight()
{
  dataCheck(true, 1, 1, 1);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::execute()
{
  int err = 0;


  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }


  dataCheck(false, 1, 1, 1);
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir stlDir(getOutputStlDirectory());
  if(!stlDir.mkpath("."))
  {

      QString ss = QObject::tr("Error creating parent path '%1'").arg(getOutputStlDirectory());
      notifyErrorMessage(ss, -1);
      setErrorCondition(-1);
      return;
  }


  // Open the Nodes file for reading
  FILE* nodesFile = fopen(m_NodesFile.toLatin1().data(), "rb+");
  if(nodesFile == NULL)
  {

    QString ss = QObject::tr("Error opening nodes file '%1'").arg(m_NodesFile);
    setErrorCondition(-1);
    PipelineMessage em(getHumanLabel(), ss, -666);
    addErrorMessage(em);
    return;
  }
  ScopedFileMonitor nodesFilePtr(nodesFile);
  //  how many nodes are in the file
  int nNodes = 0;
  fscanf(nodesFile, "%d", &nNodes);
  {
  QString ss = QObject::tr("Node Count from %1 File: %2").arg(getNodesFile()).arg(nNodes);
  notifyStatusMessage(ss);
  }
  // Open the triangles file for reading
  FILE* triFile = fopen(m_TrianglesFile.toLatin1().data(), "rb+");
  if(triFile == NULL)
  {

    QString ss = QObject::tr(": Error opening Triangles file '%1'").arg(m_TrianglesFile);
    setErrorCondition(-1);
    PipelineMessage em(getHumanLabel(), ss, -666);
    addErrorMessage(em);
    return;
  }
  ScopedFileMonitor triFilePtr(triFile);
  // how many triangles are in the file
  int nTriangles = 0;
  fscanf(triFile, "%d", &nTriangles);

  {
  QString ss = QObject::tr("Triangle Count from %1 File: %2").arg(getTrianglesFile()).arg(nTriangles);
  notifyStatusMessage(ss);
  }

  int nodeId = 0;
  int nodeKind = 0;
  float pos[3] =
  { 0.0f, 0.0f, 0.0f };
  size_t nread = 0;
  // Read the POINTS data (Vertex)
  QMap<int, int> nodeIdToIndex;
  VertexArray::Pointer nodesPtr = VertexArray::CreateArray(nNodes, DREAM3D::VertexData::SurfaceMeshNodes);
  VertexArray::Vert_t* nodes = nodesPtr->getPointer(0);

//  DataArray<int8_t>::Pointer nodeKindPtr = DataArray<int8_t>::CreateArray(nNodes, 1, DREAM3D::VertexData::SurfaceMeshNodeType);
//  int8_t* nodeKindArray = nodeKindPtr->getPointer(0);

  for (int i = 0; i < nNodes; i++)
  {
    nread = fscanf(nodesFile, "%d %d %f %f %f", &nodeId, &nodeKind, pos, pos + 1, pos + 2); // Read one set of positions from the nodes file
    if(nread != 5)
    {
      break;
    }
    nodeIdToIndex[nodeId] = i;
   // nodeKindArray[nodeId] = nodeKind;
    nodes[nodeId].pos[0] = pos[0];
    nodes[nodeId].pos[1] = pos[1];
    nodes[nodeId].pos[2] = pos[2];
  }

  // column 1 = triangle id, starts from zero
  // column 2 to 4 = node1, node2 and node3 of individual triangles
  // column 5 to 7 = edge1 (from node1 and node2), edge2 (from node2 and node3) and edge3 (from node3 and node1) of individual triangle
  // column 8 and 9 = neighboring spins of individual triangles, column 8 = spins on the left side when following winding order using right hand.
  int tData[9];

  FaceArray::Pointer trianglePtr = FaceArray::CreateArray(nTriangles, DREAM3D::FaceData::SurfaceMeshFaces, nodesPtr.get());
  FaceArray& triangles = *(trianglePtr.get());

  DataArray<int32_t>::Pointer faceLabelPtr = DataArray<int32_t>::CreateArray(nTriangles, DREAM3D::FaceData::SurfaceMeshFaceLabels);
  int32_t* faceLabels = faceLabelPtr->getPointer(0);

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for (int i = 0; i < nTriangles; i++)
  {
    // Read from the Input Triangles Temp File
    nread = fscanf(triFile, "%d %d %d %d %d %d %d %d %d", tData, tData + 1, tData + 2, tData + 3, tData + 4, tData + 5, tData + 6, tData + 7, tData + 8);
    triangles[i].verts[0] = tData[1];
    triangles[i].verts[1] = tData[2];
    triangles[i].verts[2] = tData[3];
    faceLabels[i*2] = tData[7];
    faceLabels[i*2+1] = tData[8];
    uniqueSpins.insert(tData[7]);
    uniqueSpins.insert(tData[8]);
  }


  unsigned char data[50];
  float* normal = (float*)data;
  float* vert1 = (float*)(data + 12);
  float* vert2 = (float*)(data + 24);
  float* vert3 = (float*)(data + 36);
  uint16_t* attrByteCount = (uint16_t*)(data + 48);
  *attrByteCount = 0;

  size_t totalWritten = 0;
  float u[3], w[3];
  float length;

  int spin = 0;
  int triCount = 0;

  //Loop over the unique Spins
  for (QSet<int>::iterator spinIter = uniqueSpins.begin(); spinIter != uniqueSpins.end(); ++spinIter )
  {
    spin = *spinIter;

    // Generate the output file name

    QString filename = getOutputStlDirectory() + "/" + getOutputStlPrefix() + QString::number(spin) + ".stl";
    FILE* f = fopen(filename.toLatin1().data(), "wb");
    ScopedFileMonitor fPtr(f);

    {
      QString ss = QObject::tr("Writing STL for Grain Id %1").arg(spin);
      notifyStatusMessage(ss);
    }

    {
    QString ss = "DREAM3D Generated For Grain ID " + QString::number(spin);
    err = writeHeader(f, ss, 0);
    }
    triCount = 0; // Reset this to Zero. Increment for every triangle written

    // Loop over all the triangles for this spin
    for(int t = 0; t < nTriangles; ++t)
    {
      char winding = 2; // 2 = Do NOT write this triangle
      // Get the true indices of the 3 nodes
      int nId0 = nodeIdToIndex[triangles[t].verts[0]];
      int nId1 = nodeIdToIndex[triangles[t].verts[1]];
      int nId2 = nodeIdToIndex[triangles[t].verts[2]];

      vert1[0] = static_cast<float>(nodes[nId0].pos[0]);
      vert1[1] = static_cast<float>(nodes[nId0].pos[1]);
      vert1[2] = static_cast<float>(nodes[nId0].pos[2]);

      if (faceLabels[t*2] == spin)
      {
        winding = 0; // 0 = Write it using forward spin
      }
      else if (faceLabels[t*2+1] == spin)
      {
        winding = 1; // Write it using backward spin
        // Switch the 2 node indices
        int t = nId1;
        nId1 = nId2;
        nId2 = t;
      }
      else
      {
        continue; // We do not match either spin so move to the next triangle
      }

      vert2[0] = static_cast<float>(nodes[nId1].pos[0]);
      vert2[1] = static_cast<float>(nodes[nId1].pos[1]);
      vert2[2] = static_cast<float>(nodes[nId1].pos[2]);

      vert3[0] = static_cast<float>(nodes[nId2].pos[0]);
      vert3[1] = static_cast<float>(nodes[nId2].pos[1]);
      vert3[2] = static_cast<float>(nodes[nId2].pos[2]);

      //
      // Compute the normal
      u[0] = vert2[0] - vert1[0];
      u[1] = vert2[1] - vert1[1];
      u[2] = vert2[2] - vert1[2];

      w[0] = vert3[0] - vert1[0];
      w[1] = vert3[1] - vert1[1];
      w[2] = vert3[2] - vert1[2];

      normal[0] = u[1] * w[2] - u[2] * w[1];
      normal[1] = u[2] * w[0] - u[0] * w[2];
      normal[2] = u[0] * w[1] - u[1] * w[0];

      length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
      normal[0] = normal[0] / length;
      normal[1] = normal[1] / length;
      normal[2] = normal[2] / length;

      totalWritten = fwrite(data, 1, 50, f);
      if (totalWritten != 50)
      {

        QString ss = QObject::tr("Error Writing STL File. Not enough elements written for grain id %1 Wrote %2 of 50.").arg(spin).arg(totalWritten);
        notifyErrorMessage(ss, -1201);
      }
      triCount++;
    }
    err = writeNumTrianglesToFile(filename, triCount);
  }

  setErrorCondition(0);
  notifyStatusMessage("Complete");

  return;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToStl::writeHeader(FILE* f, const QString &header, int triCount)
{
  if (NULL == f)
  {
    return -1;
  }
  char h[80];
  size_t headlength = 80;
  if(header.length() < 80) headlength = header.length();
  ::memset(h, 0, 80);
  ::memcpy(h, header.data(), headlength);
  // Return the number of bytes written - which should be 80
  fwrite(h, 1, 80, f);
  fwrite(&triCount, 1, 4, f);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToStl::writeNumTrianglesToFile(const QString &filename, int triCount)
{
  // We need to update the number of triangles in the file
  int err =0;

  FILE* out = fopen(filename.toLatin1().data(), "r+b");
  fseek(out, 80L, SEEK_SET);
  fwrite( (char*)(&triCount), 1, 4, out);
  fclose(out);

  return err;
}




