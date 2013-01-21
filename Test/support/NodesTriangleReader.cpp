#include "NodesTriangleReader.h"

#include <iostream>
#include <string>
#include <sstream>


#define DREAM3D_BENCHMARKS 1

#if DREAM3D_BENCHMARKS
#include "MXA/Common/LogTime.h"

#define DEFINE_CLOCK unsigned long long int millis;

#define START_CLOCK millis = MXA::getMilliSeconds();

#define END_CLOCK(message)\
  std::cout << message << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;


#else
#define DEFINE_CLOCK
#define START_CLOCK
#define END_CLOCK
#endif





#include "ScopedFilePointer.hpp"
#include "SurfaceMeshDataStructures.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesReader::NodesTrianglesReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesReader::~NodesTrianglesReader()
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesReader::setErrorCondition(int err)
{
  m_ErrorCondition = err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesReader::getErrorCondition()
{
  return m_ErrorCondition;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string NodesTrianglesReader::getNameOfClass()
{
  return std::string("NodesTrianglesReader");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesReader::notifyStatusMessage(const std::string &message)
{
  std::cout << getNameOfClass() << ": " << message << std::endl;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodeList_t& NodesTrianglesReader::getNodes()
{
  return m_NodeList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleList_t& NodesTrianglesReader::getTriangles()
{
  return m_TriangleList;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesReader::read(const std::string &nodesFileName, const std::string &trianglesFileName)
{

  DEFINE_CLOCK;
  //int err = 0;
  std::stringstream s;
  // Open the Nodes file for reading
  FILE* nodesFile = fopen(nodesFileName.c_str(), "rb+");
  if(nodesFile == NULL)
  {
    s.str("");
    s << "Error opening nodes file '" << nodesFileName << "'";
    setErrorCondition(-1);
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
    s << getNameOfClass() << ": Error Could not rewind to beginning of file after nodes count.'" << nodesFileName << "'";
    setErrorCondition(-1);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }
  s.str("");
  s << "Calc Node Count from Nodes.bin File: " << nNodes;
  notifyStatusMessage(s.str());

  // Open the triangles file for reading
  FILE* triFile = fopen(trianglesFileName.c_str(), "rb+");
  if(triFile == NULL)
  {
    s.str("");
    s << getNameOfClass() << ": Error opening Triangles file '" << trianglesFileName << "'";
    setErrorCondition(-1);
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
    s << getNameOfClass() << ": Error Could not rewind to beginning of file after triangles count.'" << trianglesFileName << "'";
    setErrorCondition(-1);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }
  s.str("");
  s << "Calc Triangle Count from Triangles.bin File: " << nTriangles;
  notifyStatusMessage(s.str());

  // Allocate all the nodes
  m_NodeList.resize(nNodes);

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
    SurfaceMesh::Node_t& node = m_NodeList[nRecord.nodeId];
    node.x = nRecord.x;
    node.y = nRecord.y;
    node.z = nRecord.z;
  }

  s.str("");
  s << "Reading Triangles file into Memory";
  notifyStatusMessage(s.str());

  // Allocate all the Triangle Objects

  m_TriangleList.resize(nTriangles);

  SurfaceMesh::TrianglesFile::TrianglesFileRecord_t tRecord;
  for (size_t i = 0; i < nTriangles; i++)
  {
    // Read from the Input Triangles Temp File
    nread = fread(&tRecord, SurfaceMesh::TrianglesFile::ByteCount, 1, triFile);
    if(nread != 1)
    {
      break;
    }

    SurfaceMesh::Triangle_t& triangle = m_TriangleList[tRecord.triId];

    triangle.verts[0] = tRecord.nodeId_0;
    triangle.verts[1] = tRecord.nodeId_1;
    triangle.verts[2] = tRecord.nodeId_2;
    triangle.labels[0] = tRecord.label_0;
    triangle.labels[1] = tRecord.label_1;
  }

  // The ScopedFileMonitor classes will take care of closing the files

  return 0;
}


