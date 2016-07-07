/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "NodesTrianglesToStl.h"

#include <boost/shared_array.hpp>

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "SIMPLib/Common/ScopedFileMonitor.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

// Include the MOC generated file for this class
#include "moc_NodesTrianglesToStl.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesToStl::NodesTrianglesToStl() :
  AbstractFilter(),
  m_NodesFile(""),
  m_TrianglesFile(""),
  m_OutputStlDirectory(""),
  m_OutputStlPrefix("")
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
  FilterParameterVector parameters;

  parameters.push_back(InputFileFilterParameter::New("Nodes File", "NodesFile", getNodesFile(), FilterParameter::Parameter));
  parameters.push_back(InputFileFilterParameter::New("Triangles File", "TrianglesFile", getTrianglesFile(), FilterParameter::Parameter));
  parameters.push_back(OutputPathFilterParameter::New("Output STL Directory", "OutputStlDirectory", getOutputStlDirectory(), FilterParameter::Parameter));
  parameters.push_back(StringFilterParameter::New("Stl File Prefix", "OutputStlPrefix", getOutputStlPrefix(), FilterParameter::Parameter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNodesFile( reader->readString( "NodesFile", getNodesFile() ) );
  setTrianglesFile( reader->readString( "TrianglesFile", getTrianglesFile() ) );
  setOutputStlDirectory( reader->readString( "OutputStlDirectory", getOutputStlDirectory() ) );
  setOutputStlPrefix( reader->readString( "OutputStlPrefix", getOutputStlPrefix() ) );
  /* readString END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToStl::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(NodesFile)
  SIMPL_FILTER_WRITE_PARAMETER(TrianglesFile)
  SIMPL_FILTER_WRITE_PARAMETER(OutputStlDirectory)
  SIMPL_FILTER_WRITE_PARAMETER(OutputStlPrefix)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::dataCheck()
{
  setErrorCondition(0);

  QFileInfo fi(m_TrianglesFile);

  if (m_TrianglesFile.isEmpty() == true)
  {
    setErrorCondition(-1001);
    notifyErrorMessage(getHumanLabel(), "Triangles file path or name is emtpy", -1001);
  }
  else if (fi.exists() == false)
  {

    if (getInPreflight())
    { notifyWarningMessage(getHumanLabel(), "Triangles file does not exist currently.\nYou must have another filter that creates these files before this filter in your pipeline", -1004); }
    else
    {
      setErrorCondition(-1001);
      notifyErrorMessage(getHumanLabel(), "Triangles file does not exist currently.\nYou must have another filter that creates these files before this filter in your pipeline", -1004);
    }
  }

  QFileInfo fii(m_NodesFile);
  if (m_NodesFile.isEmpty() == true)
  {
    setErrorCondition(-1002);
    notifyErrorMessage(getHumanLabel(), "Nodes file path or name is emtpy", -1002);
  }
  else if (fii.exists() == false)
  {

    if (getInPreflight())
    { notifyWarningMessage(getHumanLabel(), "Nodes file does not exist currently. You must have another filter that creates these files before this filter in your pipeline", -1005); }
    else
    {
      setErrorCondition(-1002);
      notifyErrorMessage(getHumanLabel(), "Nodes file does not exist currently. You must have another filter that creates these files before this filter in your pipeline", -1005);
    }
  }

  if (m_OutputStlDirectory.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Stl Output Directory is Not set correctly", -1003);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::execute()
{
  //int err = 0;

  dataCheck();
  if(getErrorCondition() < 0) { return; }
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir stlDir(getOutputStlDirectory());
  if(!stlDir.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(getOutputStlDirectory());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }


  // Open the Nodes file for reading
  FILE* nodesFile = fopen(m_NodesFile.toLatin1().data(), "rb+");
  if(nodesFile == NULL)
  {

    QString ss = QObject::tr("Error opening nodes file '%1'").arg(m_NodesFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -666);
    return;
  }
  ScopedFileMonitor nodesFilePtr(nodesFile);
  //  how many nodes are in the file
  int nNodes = 0;
  fscanf(nodesFile, "%d", &nNodes);
  {
    QString ss = QObject::tr("Node Count from %1 File: %2").arg(getNodesFile()).arg(nNodes);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  }
  // Open the triangles file for reading
  FILE* triFile = fopen(m_TrianglesFile.toLatin1().data(), "rb+");
  if(triFile == NULL)
  {

    QString ss = QObject::tr(": Error opening Triangles file '%1'").arg(m_TrianglesFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -666);
    return;
  }
  ScopedFileMonitor triFilePtr(triFile);
  // how many triangles are in the file
  int nTriangles = 0;
  fscanf(triFile, "%d", &nTriangles);

  {
    QString ss = QObject::tr("Triangle Count from %1 File: %2").arg(getTrianglesFile()).arg(nTriangles);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  }

  int nodeId = 0;
  int nodeKind = 0;
  float pos[3] =
  { 0.0f, 0.0f, 0.0f };
  size_t nread = 0;
  // Read the POINTS data (Vertex)
  QMap<int, int> nodeIdToIndex;
  QVector<size_t> cDims(1, 3);
  SharedVertexList::Pointer nodesPtr = SharedVertexList::CreateArray(nNodes, cDims, SIMPL::VertexData::SurfaceMeshNodes);
  float* nodes = nodesPtr->getPointer(0);

  for (int i = 0; i < nNodes; i++)
  {
    nread = fscanf(nodesFile, "%d %d %f %f %f", &nodeId, &nodeKind, pos, pos + 1, pos + 2); // Read one set of positions from the nodes file
    if(nread != 5)
    {
      break;
    }
    nodeIdToIndex[nodeId] = i;
    // nodeKindArray[nodeId] = nodeKind;
    nodes[nodeId * 3] = pos[0];
    nodes[nodeId * 3 + 1] = pos[1];
    nodes[nodeId * 3 + 2] = pos[2];
  }

  // column 1 = triangle id, starts from zero
  // column 2 to 4 = node1, node2 and node3 of individual triangles
  // column 5 to 7 = edge1 (from node1 and node2), edge2 (from node2 and node3) and edge3 (from node3 and node1) of individual triangle
  // column 8 and 9 = neighboring spins of individual triangles, column 8 = spins on the left side when following winding order using right hand.
  int tData[9];

  SharedTriList::Pointer trianglePtr = SharedTriList::CreateArray(nTriangles, cDims, SIMPL::Geometry::TriangleGeometry);
  int64_t* triangles = trianglePtr->getPointer(0);

  DataArray<int32_t>::Pointer faceLabelPtr = DataArray<int32_t>::CreateArray(nTriangles, SIMPL::FaceData::SurfaceMeshFaceLabels);
  int32_t* faceLabels = faceLabelPtr->getPointer(0);

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for (int i = 0; i < nTriangles; i++)
  {
    // Read from the Input Triangles Temp File
    nread = fscanf(triFile, "%d %d %d %d %d %d %d %d %d", tData, tData + 1, tData + 2, tData + 3, tData + 4, tData + 5, tData + 6, tData + 7, tData + 8);
    triangles[i * 3] = tData[1];
    triangles[i * 3 + 1] = tData[2];
    triangles[i * 3 + 2] = tData[3];
    faceLabels[i * 2] = tData[7];
    faceLabels[i * 2 + 1] = tData[8];
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
      QString ss = QObject::tr("Writing STL for Feature Id %1").arg(spin);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }

    {
      QString ss = "DREAM3D Generated For Feature ID " + QString::number(spin);
      int err = writeHeader(f, ss, 0);
      if(err < 0)
      {
        QString ss = QObject::tr("Error Writing STL header").arg(spin);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      }
    }
    triCount = 0; // Reset this to Zero. Increment for every triangle written

    // Loop over all the triangles for this spin
    for(int t = 0; t < nTriangles; ++t)
    {
      // Get the true indices of the 3 nodes
      int nId0 = nodeIdToIndex[triangles[t * 3]];
      int nId1 = nodeIdToIndex[triangles[t * 3 + 1]];
      int nId2 = nodeIdToIndex[triangles[t * 3 + 2]];

      vert1[0] = static_cast<float>(nodes[nId0 * 3]);
      vert1[1] = static_cast<float>(nodes[nId0 * 3 + 1]);
      vert1[2] = static_cast<float>(nodes[nId0 * 3 + 2]);

      if (faceLabels[t * 2] == spin)
      {
        // winding = 0; // 0 = Write it using forward spin
      }
      else if (faceLabels[t * 2 + 1] == spin)
      {
        //winding = 1; // Write it using backward spin
        // Switch the 2 node indices
        int temp = nId1;
        nId1 = nId2;
        nId2 = temp;
      }
      else
      {
        continue; // We do not match either spin so move to the next triangle
      }

      vert2[0] = static_cast<float>(nodes[nId1 * 3]);
      vert2[1] = static_cast<float>(nodes[nId1 * 3 + 1]);
      vert2[2] = static_cast<float>(nodes[nId1 * 3 + 2]);

      vert3[0] = static_cast<float>(nodes[nId2 * 3]);
      vert3[1] = static_cast<float>(nodes[nId2 * 3 + 1]);
      vert3[2] = static_cast<float>(nodes[nId2 * 3 + 2]);

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

        QString ss = QObject::tr("Error Writing STL File. Not enough elements written for feature id %1 Wrote %2 of 50.").arg(spin).arg(totalWritten);
        notifyErrorMessage(getHumanLabel(), ss, -1201);
      }
      triCount++;
    }
    int err = writeNumTrianglesToFile(filename, triCount);
    if(err < 0)
    {
      QString ss = QObject::tr("Error writing number of Triangles to STL file");
      notifyErrorMessage(getHumanLabel(), ss, -1201);
    }
  }

  setErrorCondition(0);
  notifyStatusMessage(getHumanLabel(), "Complete");

  return;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToStl::writeHeader(FILE* f, const QString& header, int triCount)
{
  if (NULL == f)
  {
    return -1;
  }
  char h[80];
  size_t headlength = 80;
  if(header.length() < 80) { headlength = header.length(); }
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
int NodesTrianglesToStl::writeNumTrianglesToFile(const QString& filename, int triCount)
{
  // We need to update the number of triangles in the file
  int err = 0;

  FILE* out = fopen(filename.toLatin1().data(), "r+b");
  fseek(out, 80L, SEEK_SET);
  fwrite( (char*)(&triCount), 1, 4, out);
  fclose(out);

  return err;
}





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer NodesTrianglesToStl::newFilterInstance(bool copyFilterParameters)
{
  /*
  * NodesFile
  * TrianglesFile
  * OutputStlDirectory
  * OutputStlPrefix
  */
  NodesTrianglesToStl::Pointer filter = NodesTrianglesToStl::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NodesTrianglesToStl::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NodesTrianglesToStl::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NodesTrianglesToStl::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NodesTrianglesToStl::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NodesTrianglesToStl::getSubGroupName()
{ return SIMPL::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NodesTrianglesToStl::getHumanLabel()
{ return "Convert Nodes & Triangles To STL Files"; }

