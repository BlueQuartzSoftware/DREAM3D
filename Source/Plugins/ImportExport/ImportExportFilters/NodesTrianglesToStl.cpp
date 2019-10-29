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

#include <memory>

#include "NodesTrianglesToStl.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/ScopedFileMonitor.hpp"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesToStl::NodesTrianglesToStl()
: m_NodesFile("")
, m_TrianglesFile("")
, m_OutputStlDirectory("")
, m_OutputStlPrefix("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesToStl::~NodesTrianglesToStl() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Nodes File", NodesFile, FilterParameter::Parameter, NodesTrianglesToStl));
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Triangles File", TrianglesFile, FilterParameter::Parameter, NodesTrianglesToStl));
  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output STL Directory", OutputStlDirectory, FilterParameter::Parameter, NodesTrianglesToStl));
  parameters.push_back(SIMPL_NEW_STRING_FP("Stl File Prefix", OutputStlPrefix, FilterParameter::Parameter, NodesTrianglesToStl));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToStl::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNodesFile(reader->readString("NodesFile", getNodesFile()));
  setTrianglesFile(reader->readString("TrianglesFile", getTrianglesFile()));
  setOutputStlDirectory(reader->readString("OutputStlDirectory", getOutputStlDirectory()));
  setOutputStlPrefix(reader->readString("OutputStlPrefix", getOutputStlPrefix()));
  /* readString END*/
  reader->closeFilterGroup();
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
  clearErrorCode();
  clearWarningCode();

  QFileInfo fi(m_TrianglesFile);

  if(m_TrianglesFile.isEmpty())
  {
    setErrorCondition(-1001, "Triangles file path or name is emtpy");
  }
  else if(!fi.exists())
  {

    if(getInPreflight())
    {
      QString ss = "Triangles file does not exist currently.\nYou must have another filter that creates these files before this filter in your pipeline";
      setWarningCondition(-1002, ss);
    }
    else
    {
      setErrorCondition(-1003, "Triangles file does not exist currently.\nYou must have another filter that creates these files before this filter in your pipeline");
    }
  }

  QFileInfo fii(m_NodesFile);
  if(m_NodesFile.isEmpty())
  {
    setErrorCondition(-1004, "Nodes file path or name is emtpy");
  }
  else if(!fii.exists())
  {

    if(getInPreflight())
    {
      QString ss = "Nodes file does not exist currently. You must have another filter that creates these files before this filter in your pipeline";
      setWarningCondition(-1005, ss);
    }
    else
    {
      setErrorCondition(-1006, "Nodes file does not exist currently. You must have another filter that creates these files before this filter in your pipeline");
    }
  }

  if(m_OutputStlDirectory.isEmpty())
  {
    setErrorCondition(-1007, "Stl Output Directory is Not set correctly");
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
  // int err = 0;

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir stlDir(getOutputStlDirectory());
  if(!stlDir.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(getOutputStlDirectory());
    setErrorCondition(-1, ss);
    return;
  }

  // Open the Nodes file for reading
  FILE* nodesFile = fopen(m_NodesFile.toLatin1().data(), "rb+");
  if(nodesFile == nullptr)
  {

    QString ss = QObject::tr("Error opening nodes file '%1'").arg(m_NodesFile);
    setErrorCondition(-666, ss);
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
  if(triFile == nullptr)
  {

    QString ss = QObject::tr(": Error opening Triangles file '%1'").arg(m_TrianglesFile);
    setErrorCondition(-667, ss);
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
  float pos[3] = {0.0f, 0.0f, 0.0f};
  size_t nread = 0;
  // Read the POINTS data (Vertex)
  QMap<int, int> nodeIdToIndex;
  std::vector<size_t> cDims(1, 3);
  SharedVertexList::Pointer nodesPtr = SharedVertexList::CreateArray(nNodes, cDims, SIMPL::VertexData::SurfaceMeshNodes, true);
  float* nodes = nodesPtr->getPointer(0);

  for(int i = 0; i < nNodes; i++)
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

  SharedTriList::Pointer trianglePtr = SharedTriList::CreateArray(nTriangles, cDims, SIMPL::Geometry::TriangleGeometry, true);
  MeshIndexType* triangles = trianglePtr->getPointer(0);

  DataArray<int32_t>::Pointer faceLabelPtr = DataArray<int32_t>::CreateArray(nTriangles, SIMPL::FaceData::SurfaceMeshFaceLabels, true);
  int32_t* faceLabels = faceLabelPtr->getPointer(0);

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for(int i = 0; i < nTriangles; i++)
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

  // Loop over the unique Spins
  for(QSet<int>::iterator spinIter = uniqueSpins.begin(); spinIter != uniqueSpins.end(); ++spinIter)
  {
    spin = *spinIter;

    // Generate the output file name

    QString filename = getOutputStlDirectory() + "/" + getOutputStlPrefix() + QString::number(spin) + ".stl";
    FILE* f = fopen(filename.toLatin1().data(), "wb");
    ScopedFileMonitor fPtr(f);

    {
      QString ss = QObject::tr("Writing STL for Feature Id %1").arg(spin);
      notifyStatusMessage(ss);
    }

    {
      QString ss = "DREAM3D Generated For Feature ID " + QString::number(spin);
      int err = writeHeader(f, ss, 0);
      if(err < 0)
      {
        QString ss = QObject::tr("Error Writing STL header").arg(spin);
        notifyStatusMessage(ss);
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

      if(faceLabels[t * 2] == spin)
      {
        // winding = 0; // 0 = Write it using forward spin
      }
      else if(faceLabels[t * 2 + 1] == spin)
      {
        // winding = 1; // Write it using backward spin
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
      if(totalWritten != 50)
      {

        QString ss = QObject::tr("Error Writing STL File. Not enough elements written for feature id %1 Wrote %2 of 50.").arg(spin).arg(totalWritten);
        setErrorCondition(-1201, ss);
      }
      triCount++;
    }
    int err = writeNumTrianglesToFile(filename, triCount);
    if(err < 0)
    {
      QString ss = QObject::tr("Error writing number of Triangles to STL file");
      setErrorCondition(-1201, ss);
    }
  }

  clearErrorCode();
  clearWarningCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToStl::writeHeader(FILE* f, const QString& header, int triCount)
{
  if(nullptr == f)
  {
    return -1;
  }
  char h[80];
  size_t headlength = 80;
  if(header.length() < 80)
  {
    headlength = header.length();
  }
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
  fwrite((char*)(&triCount), 1, 4, out);
  fclose(out);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer NodesTrianglesToStl::newFilterInstance(bool copyFilterParameters) const
{
  /*
   * NodesFile
   * TrianglesFile
   * OutputStlDirectory
   * OutputStlPrefix
   */
  NodesTrianglesToStl::Pointer filter = NodesTrianglesToStl::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid NodesTrianglesToStl::getUuid() const
{
  return QUuid("{5c4e21ac-902d-51d6-992b-2ddb89f26b84}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getHumanLabel() const
{
  return "Convert Nodes & Triangles To STL Files";
}

// -----------------------------------------------------------------------------
NodesTrianglesToStl::Pointer NodesTrianglesToStl::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<NodesTrianglesToStl> NodesTrianglesToStl::New()
{
  struct make_shared_enabler : public NodesTrianglesToStl
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getNameOfClass() const
{
  return QString("NodesTrianglesToStl");
}

// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::ClassName()
{
  return QString("NodesTrianglesToStl");
}

// -----------------------------------------------------------------------------
void NodesTrianglesToStl::setNodesFile(const QString& value)
{
  m_NodesFile = value;
}

// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getNodesFile() const
{
  return m_NodesFile;
}

// -----------------------------------------------------------------------------
void NodesTrianglesToStl::setTrianglesFile(const QString& value)
{
  m_TrianglesFile = value;
}

// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getTrianglesFile() const
{
  return m_TrianglesFile;
}

// -----------------------------------------------------------------------------
void NodesTrianglesToStl::setOutputStlDirectory(const QString& value)
{
  m_OutputStlDirectory = value;
}

// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getOutputStlDirectory() const
{
  return m_OutputStlDirectory;
}

// -----------------------------------------------------------------------------
void NodesTrianglesToStl::setOutputStlPrefix(const QString& value)
{
  m_OutputStlPrefix = value;
}

// -----------------------------------------------------------------------------
QString NodesTrianglesToStl::getOutputStlPrefix() const
{
  return m_OutputStlPrefix;
}
